#include "CaacCorePlugin.h"

#include <cmath>
#include <limits>

#include <QtCore/QApplicationStatic>
#include <QtCore/QJsonObject>
#include <QtCore/QSettings>
#include <QtCore/QTimer>
#include <QtGui/QGuiApplication>
#include <QtPositioning/QGeoCoordinate>
#include <QtQml/QQmlAbstractUrlInterceptor>
#include <QtQml/QQmlApplicationEngine>

#include "AppMessages.h"
#include "AppSettings.h"
#include "CaacFlightDistance.h"
#include "CaacSimulationController.h"
#include "FactMetaData.h"
#include "LinkConfiguration.h"
#include "LinkInterface.h"
#include "MissionController.h"
#include "MissionManager.h"
#include "MultiVehicleManager.h"
#include "PlanMasterController.h"
#include "QGCGeo.h"
#include "QGCMAVLink.h"
#include "QmlObjectListModel.h"
#include "SimpleMissionItem.h"
#include "UnitsSettings.h"
#include "Vehicle.h"
#include "VehicleLinkManager.h"

Q_APPLICATION_STATIC(CaacCorePlugin, caacCorePluginInstance);

namespace {
class CaacFlyViewInterceptor : public QQmlAbstractUrlInterceptor
{
public:
    QUrl intercept(const QUrl& url, DataType type) override
    {
        if (type == UrlString && url.scheme() == QStringLiteral("qrc") &&
            (url.path() == QStringLiteral("/res/QGCLogoFull.svg") ||
             url.path() == QStringLiteral("/res/QGCLogoWhite.svg"))) {
            return QUrl(QStringLiteral("qrc:/CAAC/Brand/company-logo.svg"));
        }
        if (type == UrlString && url.scheme() == QStringLiteral("qrc") &&
            url.path() == QStringLiteral("/res/qgroundcontrol.ico")) {
            return QUrl(QStringLiteral("qrc:/CAAC/Brand/company-logo.ico"));
        }
        if (type == QmlFile && url == QUrl(QStringLiteral("qrc:/qml/QGroundControl/FlyView/FlyViewCustomLayer.qml"))) {
            return QUrl(QStringLiteral("qrc:/Custom/qml/QGroundControl/FlyView/FlyViewCustomLayer.qml"));
        }
        if (type == QmlFile &&
            url == QUrl(QStringLiteral("qrc:/qml/QGroundControl/FlyView/FlyViewToolStripActionList.qml"))) {
            return QUrl(QStringLiteral("qrc:/Custom/qml/QGroundControl/FlyView/CaacFlightActionList.qml"));
        }
        return url;
    }
};
}  // namespace

CaacCorePlugin::CaacCorePlugin(QObject* parent)
    : QGCCorePlugin(parent)
{
    _simulation = new CaacSimulationController(this);
    const QSettings settings;
    _doubleClickAddWaypoint = settings.value(QStringLiteral("CAAC/Map/DoubleClickAddWaypoint"), true).toBool();
    _dragWaypoints = settings.value(QStringLiteral("CAAC/Map/DragWaypoints"), true).toBool();
}

void CaacCorePlugin::adjustSettingMetaData(const QString& settingsGroup, FactMetaData& metaData, bool& userVisible)
{
    QGCCorePlugin::adjustSettingMetaData(settingsGroup, metaData, userVisible);
    if (settingsGroup == AppSettings::settingsGroup) {
        if (metaData.name() == AppSettings::preferredFirmwareClassName) {
            metaData.setRawDefaultValue(QGCMAVLink::FirmwareClassPX4);
        } else if (metaData.name() == AppSettings::preferredVehicleClassName) {
            metaData.setRawDefaultValue(QGCMAVLink::VehicleClassMultiRotor);
        }
    } else if (settingsGroup == UnitsSettings::settingsGroup) {
        if (metaData.name() == UnitsSettings::horizontalDistanceUnitsName) {
            metaData.setRawDefaultValue(UnitsSettings::HorizontalDistanceUnitsMeters);
        } else if (metaData.name() == UnitsSettings::verticalDistanceUnitsName) {
            metaData.setRawDefaultValue(UnitsSettings::VerticalDistanceUnitsMeters);
        } else if (metaData.name() == UnitsSettings::areaUnitsName) {
            metaData.setRawDefaultValue(UnitsSettings::AreaUnitsSquareMeters);
        } else if (metaData.name() == UnitsSettings::speedUnitsName) {
            metaData.setRawDefaultValue(UnitsSettings::SpeedUnitsMetersPerSecond);
        } else if (metaData.name() == UnitsSettings::temperatureUnitsName) {
            metaData.setRawDefaultValue(UnitsSettings::TemperatureUnitsCelsius);
        }
    }
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
    QGuiApplication::setApplicationDisplayName(QStringLiteral("飞虎无人机地面站系统"));
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

QString CaacCorePlugin::refreshFlightMission(PlanMasterController* controller)
{
    auto* vehicle = MultiVehicleManager::instance()->activeVehicle();
    if (!vehicle) {
        return tr("Wait for the aircraft and mission transfer to be ready.");
    }
    if (!controller || controller->managerVehicle() != vehicle || controller->offline() ||
        controller->syncInProgress() || !vehicle->isInitialConnectComplete()) {
        return tr("Wait for the aircraft and mission transfer to be ready.");
    }
    const auto link = vehicle->vehicleLinkManager()->primaryLink().lock();
    if (!link) {
        return tr("Wait for the aircraft and mission transfer to be ready.");
    }
    if (link->linkConfiguration()->isHighLatency()) {
        return QCoreApplication::translate("PlanMasterController", "Download not supported on high latency links.");
    }
    controller->missionController()->loadFromVehicle();
    return {};
}

QString CaacCorePlugin::updateFlightWaypointAltitude(PlanMasterController* controller, QObject* waypoint,
                                                     double altitude)
{
    return updateFlightWaypointAltitudes(controller, {QVariant::fromValue(waypoint)}, altitude);
}

QString CaacCorePlugin::updateFlightWaypointAltitudes(PlanMasterController* controller, const QVariantList& waypoints,
                                                      double altitude)
{
    if (_flightAltitudeUpdateTimer) {
        return tr("Wait for the aircraft and mission transfer to be ready.");
    }
    auto* vehicle = MultiVehicleManager::instance()->activeVehicle();
    if (!vehicle) {
        return tr("Wait for the aircraft and mission transfer to be ready.");
    }
    auto* mission = controller ? controller->missionController() : nullptr;
    if (!controller || !mission || controller->managerVehicle() != vehicle || controller->offline() ||
        controller->syncInProgress() || !vehicle->isInitialConnectComplete()) {
        return tr("Wait for the aircraft and mission transfer to be ready.");
    }
    if (waypoints.isEmpty()) {
        return tr("Select a downloaded waypoint.");
    }
    QList<SimpleMissionItem*> items;
    for (const auto& waypoint : waypoints) {
        auto* item = qobject_cast<SimpleMissionItem*>(waypoint.value<QObject*>());
        if (!item || item->command() != MAV_CMD_NAV_WAYPOINT || mission->visualItems()->indexOf(item) <= 0 ||
            items.contains(item)) {
            return tr("Select a downloaded waypoint.");
        }
        auto* fact = item->altitude();
        if (!std::isfinite(altitude) || std::abs(altitude) > std::numeric_limits<float>::max() ||
            altitude < fact->rawMin().toDouble() || altitude > fact->rawMax().toDouble()) {
            return tr("Enter a valid waypoint altitude.");
        }
        items.append(item);
    }
    const auto preCheck = mission->sendToVehiclePreCheck();
    const bool pauseSimulation =
        preCheck == MissionController::SendToVehiclePreCheckStateActiveMission && _simulation->ownsVehicle(vehicle);
    if ((!pauseSimulation && preCheck != MissionController::SendToVehiclePreCheckStateOk) ||
        mission->readyForSaveState() != VisualMissionItem::ReadyForSave) {
        return tr("Pause the mission and complete all mission data before uploading.");
    }
    if (pauseSimulation) {
        auto* timer = new QTimer(this);
        timer->setSingleShot(true);
        timer->setProperty("uploading", false);
        _flightAltitudeUpdateTimer = timer;
        const QPointer<Vehicle> aircraft(vehicle);
        const QPointer<PlanMasterController> plan(controller);
        connect(MultiVehicleManager::instance(), &MultiVehicleManager::activeVehicleChanged, timer,
                [timer, aircraft](Vehicle* activeVehicle) {
                    if (!aircraft || activeVehicle != aircraft) {
                        timer->deleteLater();
                    }
                });
        QList<QPointer<SimpleMissionItem>> selected;
        for (auto* item : items) {
            selected.append(item);
        }
        connect(timer, &QTimer::timeout, timer, [this, timer]() {
            QGC::showAppMessage(tr("Wait for the aircraft and mission transfer to be ready."));
            timer->deleteLater();
        });
        connect(vehicle, &Vehicle::flightModeChanged, timer, [this, timer, aircraft, plan, selected, altitude]() {
            if (timer->property("uploading").toBool() || !aircraft || !plan ||
                aircraft != MultiVehicleManager::instance()->activeVehicle() ||
                aircraft->flightMode() != aircraft->pauseFlightMode()) {
                return;
            }
            timer->setProperty("uploading", true);
            QVariantList selectedWaypoints;
            for (const auto& item : selected) {
                selectedWaypoints.append(QVariant::fromValue(static_cast<QObject*>(item.data())));
            }
            connect(
                aircraft->missionManager(), &MissionManager::sendComplete, timer,
                [timer, aircraft](bool error) {
                    if (error) {
                        QGC::showAppMessage(QCoreApplication::translate("CaacSimulationController",
                                                                        "Simulation mission upload failed."));
                    } else if (aircraft && aircraft == MultiVehicleManager::instance()->activeVehicle() &&
                               aircraft->flightMode() == aircraft->pauseFlightMode()) {
                        aircraft->startMission();
                    }
                    timer->deleteLater();
                },
                Qt::SingleShotConnection);
            _flightAltitudeUpdateTimer.clear();
            const auto error = updateFlightWaypointAltitudes(plan, selectedWaypoints, altitude);
            _flightAltitudeUpdateTimer = timer;
            if (!error.isEmpty()) {
                QGC::showAppMessage(error);
                timer->deleteLater();
            }
        });
        timer->start(60000);
        vehicle->pauseVehicle();
        return {};
    }
    for (auto* item : items) {
        item->altitude()->setRawValue(altitude);
    }
    mission->sendToVehicle();
    return {};
}

QUrl CaacCorePlugin::planViewEditor() const
{
    return QUrl(QStringLiteral("qrc:/qml/CAAC/Plan/CaacRouteEditor.qml"));
}

bool CaacCorePlugin::keepCurrentPlanOnVehicleChange(PlanMasterController* controller) const
{
    return controller && _simulation->ownsVehicle(controller->managerVehicle());
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
    QJsonObject metadata{{QStringLiteral("version"), 1}};
    const auto center = controller->property("caacRotationCenter").value<QGeoCoordinate>();
    if (center.isValid()) {
        metadata[QStringLiteral("centerLatitude")] = center.latitude();
        metadata[QStringLiteral("centerLongitude")] = center.longitude();
    }
    const auto settings = controller->property("caacSimulationSettings").toMap();
    if (!settings.isEmpty()) {
        metadata[QStringLiteral("simulationSettings")] = QJsonObject::fromVariantMap(settings);
    }
    const auto orientation = controller->property("caacRouteOrientation");
    if (orientation.isValid() && std::isfinite(orientation.toDouble())) {
        metadata[QStringLiteral("routeOrientation")] = orientation.toDouble();
    }
    if (metadata.size() > 1) {
        json[QStringLiteral("caac")] = metadata;
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
    const auto orientation = metadata.value(QStringLiteral("routeOrientation"));
    controller->setProperty("caacRouteOrientation",
                            metadata.value(QStringLiteral("version")).toInt() == 1 && orientation.isDouble()
                                ? QVariant(orientation.toDouble())
                                : QVariant());
    controller->setProperty("caacRotationCenter", QVariant::fromValue(center));
    controller->setProperty("caacSimulationSettings",
                            metadata.value(QStringLiteral("version")).toInt() == 1
                                ? metadata.value(QStringLiteral("simulationSettings")).toObject().toVariantMap()
                                : QVariantMap());
}
