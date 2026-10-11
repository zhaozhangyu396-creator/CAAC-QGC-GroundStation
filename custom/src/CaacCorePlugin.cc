#include "CaacCorePlugin.h"

#include <cmath>
#include <limits>

#include <QtCore/QApplicationStatic>
#include <QtCore/QJsonObject>
#include <QtCore/QSettings>
#include <QtPositioning/QGeoCoordinate>
#include <QtQml/QQmlAbstractUrlInterceptor>
#include <QtQml/QQmlApplicationEngine>

#include "CaacFlightDistance.h"
#include "MultiVehicleManager.h"
#include "PlanMasterController.h"
#include "QGCGeo.h"
#include "QmlObjectListModel.h"
#include "Vehicle.h"

Q_APPLICATION_STATIC(CaacCorePlugin, caacCorePluginInstance);

namespace {
class CaacFlyViewInterceptor : public QQmlAbstractUrlInterceptor
{
public:
    QUrl intercept(const QUrl& url, DataType type) override
    {
        if (type == QmlFile && url == QUrl(QStringLiteral("qrc:/qml/QGroundControl/FlyView/FlyViewCustomLayer.qml"))) {
            return QUrl(QStringLiteral("qrc:/Custom/qml/QGroundControl/FlyView/FlyViewCustomLayer.qml"));
        }
        return url;
    }
};
}  // namespace

CaacCorePlugin::CaacCorePlugin(QObject* parent)
    : QGCCorePlugin(parent)
{
    const QSettings settings;
    _doubleClickAddWaypoint = settings.value(QStringLiteral("CAAC/Map/DoubleClickAddWaypoint"), true).toBool();
    _dragWaypoints = settings.value(QStringLiteral("CAAC/Map/DragWaypoints"), true).toBool();
}

void CaacCorePlugin::setDoubleClickAddWaypoint(bool enabled)
{
    if (_doubleClickAddWaypoint != enabled) {
        _doubleClickAddWaypoint = enabled;
        QSettings().setValue(QStringLiteral("CAAC/Map/DoubleClickAddWaypoint"), enabled);
        emit mapPreferencesChanged();
    }
}

void CaacCorePlugin::setDragWaypoints(bool enabled)
{
    if (_dragWaypoints != enabled) {
        _dragWaypoints = enabled;
        QSettings().setValue(QStringLiteral("CAAC/Map/DragWaypoints"), enabled);
        emit mapPreferencesChanged();
    }
}

QVariantMap CaacCorePlugin::routeLegMetrics(const QGeoCoordinate& from, const QGeoCoordinate& to) const
{
    if (!from.isValid() || !to.isValid()) {
        return {};
    }
    const double distance = QGCGeo::geodesicDistance(from, to);
    if (distance < 0.001) {
        return {};
    }
    return {{QStringLiteral("distance"), distance},
            {QStringLiteral("bearing"), QGCGeo::geodesicAzimuth(from, to)},
            {QStringLiteral("midpoint"), QVariant::fromValue(QGCGeo::interpolateAtDistance(from, to, distance / 2))}};
}

QVariantMap CaacCorePlugin::coordinateToDms(double value, int maximumDegrees) const
{
    if ((maximumDegrees != 90 && maximumDegrees != 180) || !std::isfinite(value) || std::abs(value) > maximumDegrees) {
        return {};
    }
    const double absolute = std::abs(value);
    int degrees = static_cast<int>(std::floor(absolute));
    const double fraction = (absolute - degrees) * 60;
    int minutes = static_cast<int>(std::floor(fraction));
    double seconds = std::round((fraction - minutes) * 60 * 1e9) / 1e9;
    if (seconds >= 60) {
        seconds = 0;
        if (++minutes == 60) {
            minutes = 0;
            ++degrees;
        }
    }
    return {{QStringLiteral("degrees"), degrees},
            {QStringLiteral("minutes"), minutes},
            {QStringLiteral("seconds"), seconds},
            {QStringLiteral("negative"), std::signbit(value)}};
}

double CaacCorePlugin::coordinateFromDms(double degrees, double minutes, double seconds, bool negative,
                                         int maximumDegrees) const
{
    const double invalid = std::numeric_limits<double>::quiet_NaN();
    if ((maximumDegrees != 90 && maximumDegrees != 180) || !std::isfinite(degrees) || !std::isfinite(minutes) ||
        !std::isfinite(seconds) || degrees < 0 || degrees > maximumDegrees || std::floor(degrees) != degrees ||
        minutes < 0 || minutes >= 60 || std::floor(minutes) != minutes || seconds < 0 || seconds >= 60) {
        return invalid;
    }
    const double value = degrees + minutes / 60 + seconds / 3600;
    return value <= maximumDegrees ? (negative ? -value : value) : invalid;
}

QGCCorePlugin* CaacCorePlugin::instance()
{
    return caacCorePluginInstance();
}

void CaacCorePlugin::init()
{
    if (_initialized) {
        return;
    }
    _initialized = true;
    QGCCorePlugin::init();
    auto* manager = MultiVehicleManager::instance();
    connect(manager, &MultiVehicleManager::vehicleAdded, this, &CaacCorePlugin::_watchVehicle);
    connect(manager, &MultiVehicleManager::activeVehicleChanged, this, &CaacCorePlugin::flightDistanceChanged);
    for (int i = 0; i < manager->vehicles()->count(); ++i) {
        _watchVehicle(manager->vehicles()->value<Vehicle*>(i));
    }
}

void CaacCorePlugin::_watchVehicle(Vehicle* vehicle)
{
    if (!vehicle || _flightDistances.contains(vehicle)) {
        return;
    }
    auto* distance = new CaacFlightDistance(vehicle, this);
    _flightDistances.insert(vehicle, distance);
    connect(vehicle, &QObject::destroyed, this, [this, vehicle]() {
        delete _flightDistances.take(vehicle);
        emit flightDistanceChanged();
    });
    emit flightDistanceChanged();
}

CaacFlightDistance* CaacCorePlugin::flightDistance() const
{
    return _flightDistances.value(MultiVehicleManager::instance()->activeVehicle(), nullptr);
}

void CaacCorePlugin::resetFlightDistanceForSimulationRestart(Vehicle* vehicle)
{
    if (auto* distance = _flightDistances.value(vehicle, nullptr)) {
        distance->resetForSimulationRestart();
    }
}

QUrl CaacCorePlugin::planViewEditor() const
{
    return QUrl(QStringLiteral("qrc:/qml/CAAC/Plan/CaacRouteEditor.qml"));
}

QQmlApplicationEngine* CaacCorePlugin::createQmlApplicationEngine(QObject* parent)
{
    auto* engine = QGCCorePlugin::createQmlApplicationEngine(parent);
    auto* interceptor = new CaacFlyViewInterceptor;
    engine->addUrlInterceptor(interceptor);
    _flyInterceptors.insert(engine, interceptor);
    connect(engine, &QObject::destroyed, this, [this, engine]() { delete _flyInterceptors.take(engine); });
    return engine;
}

void CaacCorePlugin::destroyQmlApplicationEngine(QQmlApplicationEngine* engine)
{
    if (auto* interceptor = _flyInterceptors.take(engine)) {
        engine->removeUrlInterceptor(interceptor);
        delete interceptor;
    }
    QGCCorePlugin::destroyQmlApplicationEngine(engine);
}

void CaacCorePlugin::postSaveToJson(PlanMasterController* controller, QJsonObject& json)
{
    const auto center = controller->property("caacRotationCenter").value<QGeoCoordinate>();
    if (center.isValid()) {
        json[QStringLiteral("caac")] = QJsonObject{{QStringLiteral("version"), 1},
                                                   {QStringLiteral("centerLatitude"), center.latitude()},
                                                   {QStringLiteral("centerLongitude"), center.longitude()}};
    }
}

void CaacCorePlugin::postLoadFromJson(PlanMasterController* controller, QJsonObject& json)
{
    const auto metadata = json.value(QStringLiteral("caac")).toObject();
    QGeoCoordinate center;
    if (metadata.value(QStringLiteral("version")).toInt() == 1 &&
        metadata.value(QStringLiteral("centerLatitude")).isDouble() &&
        metadata.value(QStringLiteral("centerLongitude")).isDouble()) {
        center = QGeoCoordinate(metadata.value(QStringLiteral("centerLatitude")).toDouble(),
                                metadata.value(QStringLiteral("centerLongitude")).toDouble());
    }
    controller->setProperty("caacRotationCenter", QVariant::fromValue(center));
}
