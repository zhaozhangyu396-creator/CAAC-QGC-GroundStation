#include "CaacCorePlugin.h"

#include <cmath>
#include <limits>
#include <utility>

#include <QtCore/QApplicationStatic>
#include <QtCore/QJsonObject>
#include <QtCore/QScopeGuard>
#include <QtCore/QSettings>
#include <QtCore/QTimer>
#include <QtGui/QGuiApplication>
#include <QtPositioning/QGeoCoordinate>
#include <QtQml/QQmlAbstractUrlInterceptor>
#include <QtQml/QQmlApplicationEngine>

#include "AppMessages.h"
#include "AppSettings.h"
#include "CaacActivationController.h"
#include "CaacExamAudio.h"
#include "CaacFigureEightController.h"
#include "CaacFlightDistance.h"
#include "CaacMissionUpdateUtils.h"
#include "CaacSimulationController.h"
#include "FactMetaData.h"
#include "FlightPathSegment.h"
#include "LinkConfiguration.h"
#include "LinkInterface.h"
#include "MissionController.h"
#include "MissionManager.h"
#include "MissionSettingsItem.h"
#include "MultiVehicleManager.h"
#include "ParameterManager.h"
#include "PlanMasterController.h"
#include "QGCGeo.h"
#include "QGCMAVLink.h"
#include "QmlObjectListModel.h"
#include "RemoteIDManager.h"
#include "RemoteIDSettings.h"
#include "SimpleMissionItem.h"
#include "SpeedSection.h"
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
            return QUrl(QStringLiteral("qrc:/CAAC/Brand/company-logo.png"));
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
    _activation = new CaacActivationController(this);
    _figureEight = new CaacFigureEightController(this);
    auto* examAudio = new CaacExamAudio(this);
    connect(_figureEight, &CaacFigureEightController::announcementRequested, examAudio, &CaacExamAudio::play);
    connect(_figureEight, &CaacFigureEightController::alarmsCleared, examAudio, &CaacExamAudio::clearPendingWarnings);
    const QSettings settings;
    _doubleClickAddWaypoint = settings.value(QStringLiteral("CAAC/Map/DoubleClickAddWaypoint"), true).toBool();
    _dragWaypoints = settings.value(QStringLiteral("CAAC/Map/DragWaypoints"), true).toBool();
    for (const auto* key :
         {"horizontalSnap", "verticalSnap", "angleSnap", "symmetrySnap", "midpointSnap", "intersectionSnap"}) {
        _planningPreferences[QLatin1String(key)] =
            settings.value(QStringLiteral("CAAC/Planning/") + QLatin1String(key), true).toBool();
    }
    const double angle = settings.value(QStringLiteral("CAAC/Planning/snapAngle"), 15.0).toDouble();
    _planningPreferences[QStringLiteral("snapAngle")] =
        std::isfinite(angle) && angle >= 1 && angle <= 90 ? angle : 15.0;
}

bool CaacCorePlugin::requestApplicationAuthorization(QObject* parent, std::function<void()> onAuthorized)
{
    return _activation->showWindow(this, parent, std::move(onAuthorized));
}

bool CaacCorePlugin::updatePlanningPreferences(const QVariantMap& values)
{
    auto updated = _planningPreferences;
    for (auto it = values.cbegin(); it != values.cend(); ++it) {
        if (!updated.contains(it.key())) {
            return false;
        }
        if (it.key() == QStringLiteral("snapAngle")) {
            bool valid = false;
            const double angle = it.value().toDouble(&valid);
            if (!valid || !std::isfinite(angle) || angle < 1 || angle > 90) {
                return false;
            }
            updated[it.key()] = angle;
        } else {
            if (it.value().metaType().id() != QMetaType::Bool) {
                return false;
            }
            updated[it.key()] = it.value().toBool();
        }
    }
    if (updated != _planningPreferences) {
        _planningPreferences = updated;
        QSettings settings;
        for (auto it = values.cbegin(); it != values.cend(); ++it) {
            settings.setValue(QStringLiteral("CAAC/Planning/") + it.key(), _planningPreferences.value(it.key()));
        }
        emit mapPreferencesChanged();
    }
    return true;
}

void CaacCorePlugin::refreshFlightPreferenceParameter(const QString& name)
{
    auto* vehicle = MultiVehicleManager::instance()->activeVehicle();
    if (!vehicle) {
        return;
    }
    auto* manager = vehicle->parameterManager();
    if (!manager || !manager->parameterExists(ParameterManager::defaultComponentId, name)) {
        return;
    }
    manager->refreshParameter(ParameterManager::defaultComponentId, name);
}

void CaacCorePlugin::refreshAircraftIdentity()
{
    auto* vehicle = MultiVehicleManager::instance()->activeVehicle();
    if (!vehicle) {
        return;
    }
    vehicle->requestMessage(nullptr, nullptr, vehicle->compId(), MAVLINK_MSG_ID_AUTOPILOT_VERSION);
    if (vehicle->remoteIDManager()->available()) {
        vehicle->requestMessage(nullptr, nullptr, vehicle->compId(), MAVLINK_MSG_ID_OPEN_DRONE_ID_BASIC_ID);
    }
}

void CaacCorePlugin::adjustSettingMetaData(const QString& settingsGroup, FactMetaData& metaData, bool& userVisible)
{
    QGCCorePlugin::adjustSettingMetaData(settingsGroup, metaData, userVisible);
    if (settingsGroup == AppSettings::settingsGroup) {
        if (metaData.name() == AppSettings::preferredFirmwareClassName) {
            metaData.setRawDefaultValue(QGCMAVLink::FirmwareClassPX4);
        } else if (metaData.name() == AppSettings::preferredVehicleClassName) {
            metaData.setRawDefaultValue(QGCMAVLink::VehicleClassMultiRotor);
        } else if (metaData.name() == AppSettings::audioMutedName) {
            metaData.setRawDefaultValue(true);
        }
    } else if (settingsGroup == RemoteIDSettings::settingsGroup) {
        if (metaData.name() == RemoteIDSettings::regionName) {
            metaData.setRawDefaultValue(static_cast<int>(RemoteIDSettings::RegionOperation::China));
        } else if (metaData.name() == RemoteIDSettings::locationTypeName) {
            metaData.setRawDefaultValue(static_cast<int>(RemoteIDSettings::LocationType::TAKEOFF));
        } else if (metaData.name() == RemoteIDSettings::sendBasicIDName) {
            metaData.setRawDefaultValue(true);
        } else if (metaData.name() == RemoteIDSettings::selfIDFreeName) {
            metaData.setRawDefaultValue(QString::fromUtf8("飞行训练"));
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
    if (_simulation->ownsVehicle(vehicle)) {
        QList<int> numbers;
        for (const auto* item : items) {
            numbers.append(_simulation->waypointNumber(item->sequenceNumber(), item->coordinate()));
        }
        // A compiled return-to-waypoint hover follows that waypoint's edited altitude.
        for (int i = 1; i < mission->visualItems()->count(); ++i) {
            auto* item = mission->visualItems()->value<SimpleMissionItem*>(i);
            if (item && item->command() == MAV_CMD_NAV_LOITER_UNLIM) {
                const int number = _simulation->waypointNumber(item->sequenceNumber(), item->coordinate());
                if (number != item->sequenceNumber() && numbers.contains(number)) {
                    if (altitude < item->altitude()->rawMin().toDouble() ||
                        altitude > item->altitude()->rawMax().toDouble()) {
                        return tr("Enter a valid waypoint altitude.");
                    }
                    items.append(item);
                }
            }
        }
    }
    for (auto* item : items) {
        item->altitude()->setRawValue(altitude);
    }
    mission->sendToVehicle();
    return {};
}

QString CaacCorePlugin::setFlightWaypointSpeeds(PlanMasterController* controller, const QVariantList& selectedWaypoints,
                                                double speed, bool allWaypoints)
{
    auto* mission = controller ? controller->missionController() : nullptr;
    auto* model = mission ? mission->visualItems() : nullptr;
    auto* settings = model && model->count() ? model->value<MissionSettingsItem*>(0) : nullptr;
    auto* planningVehicle = controller ? controller->controllerVehicle() : nullptr;
    if (!controller || !mission || !settings || !planningVehicle || _flightAltitudeUpdateTimer ||
        controller->syncInProgress() || controller->property("caacMissionUpdateActive").toBool()) {
        return tr("Wait for the aircraft and mission transfer to be ready.");
    }
    if (controller->property("caacRotationPreviewActive").toBool()) {
        return QCoreApplication::translate("PlanMasterController",
                                           "Confirm or cancel the orientation preview before saving or uploading.");
    }
    const int speedType = planningVehicle->multiRotor() ? 1 : 0;
    if (!controller->offline()) {
        auto* vehicle = MultiVehicleManager::instance()->activeVehicle();
        if (!vehicle) {
            return tr("Wait for the aircraft and mission transfer to be ready.");
        }
        if (controller->managerVehicle() != vehicle || !vehicle->isInitialConnectComplete()) {
            return tr("Wait for the aircraft and mission transfer to be ready.");
        }
        const double maximum = speedType == 1 ? vehicle->maximumHorizontalSpeedMultirotorMetersSecond()
                                              : vehicle->maximumEquivalentAirspeed();
        const double minimum = speedType == 1 ? 0 : vehicle->minimumEquivalentAirspeed();
        if ((std::isfinite(maximum) && speed > maximum) || (std::isfinite(minimum) && speed < minimum)) {
            return tr("Enter a valid waypoint flight speed.");
        }
    }
    if (!std::isfinite(speed) || speed <= 0 || speed > std::numeric_limits<float>::max()) {
        return tr("Enter a valid waypoint flight speed.");
    }

    QList<SimpleMissionItem*> waypoints;
    QList<SimpleMissionItem*> selected;
    QList<SimpleMissionItem*> jumps;
    for (int i = 1; i < model->count(); ++i) {
        auto* item = model->value<SimpleMissionItem*>(i);
        if (item && item->command() == MAV_CMD_DO_CHANGE_SPEED && item->missionItem().param1() == speedType) {
            const auto& command = item->missionItem();
            if (command.frame() != MAV_FRAME_MISSION || !command.autoContinue() || !std::isfinite(command.param2()) ||
                command.param2() <= 0 || command.param3() != -1 || command.param4() != 0 || command.param5() != 0 ||
                command.param6() != 0 || command.param7() != 0) {
                return tr("This mission contains speed or jump commands that cannot be safely updated.");
            }
        }
        if (item && item->command() == MAV_CMD_NAV_WAYPOINT) {
            waypoints.append(item);
        } else if (item && item->command() == MAV_CMD_DO_JUMP) {
            jumps.append(item);
        }
    }
    if (allWaypoints) {
        selected = waypoints;
    } else {
        for (const auto& value : selectedWaypoints) {
            auto* item = qobject_cast<SimpleMissionItem*>(value.value<QObject*>());
            if (!item || !waypoints.contains(item) || selected.contains(item)) {
                return tr("Select at least one ordinary waypoint.");
            }
            selected.append(item);
        }
    }
    if (selected.isEmpty()) {
        return tr("Select at least one ordinary waypoint.");
    }

    const auto absoluteSpeedCommand = [speedType](SimpleMissionItem* item) {
        return item && item->command() == MAV_CMD_DO_CHANGE_SPEED && item->missionItem().param1() == speedType &&
               item->missionItem().param4() == 0;
    };
    QHash<SimpleMissionItem*, double> outgoingSpeeds;
    double inheritedSpeed =
        speedType == 1 ? planningVehicle->defaultHoverSpeed() : planningVehicle->defaultCruiseSpeed();
    SimpleMissionItem* previousWaypoint = nullptr;
    for (int i = 0; i < model->count(); ++i) {
        auto* item = model->value<VisualMissionItem*>(i);
        auto* simple = qobject_cast<SimpleMissionItem*>(item);
        const bool speedCommand = simple && simple->command() == MAV_CMD_DO_CHANGE_SPEED;
        const double specified = !speedCommand || absoluteSpeedCommand(simple) ? item->specifiedFlightSpeed() : qQNaN();
        if (std::isfinite(specified) && specified > 0) {
            inheritedSpeed = specified;
            if (speedCommand && previousWaypoint) {
                outgoingSpeeds[previousWaypoint] = inheritedSpeed;
            }
        }
        if (item->specifiesCoordinate() && !item->isStandaloneCoordinate()) {
            previousWaypoint = simple && simple->command() == MAV_CMD_NAV_WAYPOINT ? simple : nullptr;
            if (previousWaypoint) {
                outgoingSpeeds.insert(previousWaypoint, inheritedSpeed);
            }
        }
    }

    struct SectionChange
    {
        SpeedSection* section;
        QVariant previousValue;
        bool previousSpecified;
        bool previousDirty;
        double value;
    };

    struct CommandChange
    {
        SimpleMissionItem* item;
        double previousValue;
    };

    QList<SectionChange> sections;
    QList<CommandChange> commands;
    QList<CommandChange> jumpTargets;
    const auto appendSection = [&sections](SpeedSection* section, double value) {
        if (!section || !section->available() || !std::isfinite(value) || value <= 0 ||
            value > std::numeric_limits<float>::max() || value < section->flightSpeed()->rawMin().toDouble() ||
            value > section->flightSpeed()->rawMax().toDouble()) {
            return false;
        }
        sections.append(
            {section, section->flightSpeed()->rawValue(), section->specifyFlightSpeed(), section->dirty(), value});
        return true;
    };
    if (allWaypoints && !appendSection(settings->speedSection(), speed)) {
        return tr("Enter a valid waypoint flight speed.");
    }
    if (allWaypoints) {
        for (int i = 1; i < model->count(); ++i) {
            auto* item = model->value<SimpleMissionItem*>(i);
            auto* section = item ? item->speedSection() : nullptr;
            if (item && item->command() != MAV_CMD_NAV_WAYPOINT && section && section->available() &&
                section->specifyFlightSpeed() && !appendSection(section, speed)) {
                return tr("Enter a valid waypoint flight speed.");
            }
        }
    }
    for (auto* item : waypoints) {
        double originalSpeed = outgoingSpeeds.value(item, qQNaN());
        if (auto* segment = item->simpleFlightPathSegment();
            segment && std::isfinite(segment->flightSpeed()) && segment->flightSpeed() > 0) {
            originalSpeed = segment->flightSpeed();
        }
        if (!appendSection(item->speedSection(), selected.contains(item) ? speed : originalSpeed)) {
            return selected.contains(item) ? tr("Enter a valid waypoint flight speed.")
                                           : tr("Unable to preserve the flight speeds of the other waypoints.");
        }
    }
    bool selectedLeg = false;
    for (int i = 1; i < model->count(); ++i) {
        auto* item = model->value<SimpleMissionItem*>(i);
        if (item && item->command() == MAV_CMD_NAV_WAYPOINT) {
            selectedLeg = selected.contains(item);
        } else if ((allWaypoints || selectedLeg) && absoluteSpeedCommand(item)) {
            const auto& command = item->missionItem();
            if (command.frame() != MAV_FRAME_MISSION || !command.autoContinue() || !std::isfinite(command.param2()) ||
                command.param2() <= 0 || command.param3() != -1 || command.param5() != 0 || command.param6() != 0 ||
                command.param7() != 0) {
                return tr("This mission contains speed or jump commands that cannot be safely updated.");
            }
            commands.append({item, item->missionItem().param2()});
        }
    }
    for (auto* jump : jumps) {
        jumpTargets.append({jump, jump->missionItem().param1()});
    }

    const auto simulationSettings = controller->property("caacSimulationSettings").toMap();
    SimpleMissionItem* startPoint = nullptr;
    SimpleMissionItem* returnPoint = nullptr;
    for (auto* item : waypoints) {
        if (item->sequenceNumber() == simulationSettings.value("startSequence").toInt()) {
            startPoint = item;
        }
        if (item->sequenceNumber() == simulationSettings.value("returnSequence").toInt()) {
            returnPoint = item;
        }
    }
    if ((!startPoint && simulationSettings.value("startSequence").toInt() > 0) ||
        (!returnPoint && simulationSettings.value("returnSequence").toInt() > 0)) {
        return QCoreApplication::translate("CaacSimulationController",
                                           "Select existing waypoints for the start and return points.");
    }
    const auto wireMission = [model]() {
        QObject owner;
        const auto items = MissionController::createMissionItems(model, &owner);
        CaacMissionUpdateUtils::Mission result;
        if (items.isEmpty() || items.size() > 5000) {
            return result;
        }
        for (int i = 0; i < items.size(); ++i) {
            const auto* item = items.at(i);
            mavlink_mission_item_int_t wire{};
            wire.seq = i;
            wire.command = item->command();
            wire.frame = item->frame();
            wire.autocontinue = item->autoContinue();
            wire.param1 = item->param1();
            wire.param2 = item->param2();
            wire.param3 = item->param3();
            wire.param4 = item->param4();
            const double scale = item->frame() == MAV_FRAME_MISSION ? 1 : 1e7;
            wire.x = std::isfinite(item->param5()) ? static_cast<int32_t>(item->param5() * scale) : 0;
            wire.y = std::isfinite(item->param6()) ? static_cast<int32_t>(item->param6() * scale) : 0;
            wire.z = item->param7();
            result.insert(wire.seq, wire);
        }
        return result;
    };
    const auto previousMission = wireMission();
    if (!CaacMissionUpdateUtils::speedUpdateMapping(previousMission, previousMission).valid) {
        return tr("This mission contains speed or jump commands that cannot be safely updated.");
    }
    QList<QPair<VisualMissionItem*, bool>> previousDirty;
    for (int i = 0; i < model->count(); ++i) {
        auto* item = model->value<VisualMissionItem*>(i);
        previousDirty.append({item, item->dirty()});
    }
    const bool missionDirty = mission->dirty();
    bool committed = false;
    const auto rollback = qScopeGuard([&]() {
        if (committed) {
            return;
        }
        for (const auto& change : sections) {
            change.section->flightSpeed()->setRawValue(change.previousValue);
            change.section->setSpecifyFlightSpeed(change.previousSpecified);
            change.section->setDirty(change.previousDirty);
        }
        for (const auto& change : commands) {
            change.item->missionItem().setParam2(change.previousValue);
        }
        for (const auto& change : jumpTargets) {
            change.item->missionItem().setParam1(change.previousValue);
        }
        controller->setProperty("caacSimulationSettings", simulationSettings);
        for (const auto& state : previousDirty) {
            state.first->setDirty(state.second);
        }
        mission->setDirty(missionDirty);
    });
    for (const auto& change : sections) {
        change.section->flightSpeed()->setRawValue(change.value);
        change.section->setSpecifyFlightSpeed(true);
    }
    for (const auto& change : commands) {
        change.item->missionItem().setParam2(speed);
    }
    const auto mapping = CaacMissionUpdateUtils::speedUpdateMapping(previousMission, wireMission(), false);
    if (!mapping.valid) {
        return tr("This mission contains speed or jump commands that cannot be safely updated.");
    }
    for (const auto& change : jumpTargets) {
        const int target = static_cast<int>(change.previousValue);
        if (!mapping.jumpTargetToNew.contains(target)) {
            return tr("This mission contains speed or jump commands that cannot be safely updated.");
        }
        change.item->missionItem().setParam1(mapping.jumpTargetToNew.value(target));
    }
    if (!CaacMissionUpdateUtils::speedUpdateMapping(previousMission, wireMission()).valid) {
        return tr("This mission contains speed or jump commands that cannot be safely updated.");
    }
    if (!simulationSettings.isEmpty()) {
        auto remapped = simulationSettings;
        if (startPoint) {
            remapped["startSequence"] = startPoint->sequenceNumber();
        }
        if (returnPoint) {
            remapped["returnSequence"] = returnPoint->sequenceNumber();
        }
        controller->setProperty("caacSimulationSettings", remapped);
    }
    committed = true;
    return {};
}

QUrl CaacCorePlugin::planViewEditor() const
{
    return QUrl(QStringLiteral("qrc:/qml/CAAC/Plan/CaacRouteEditor.qml"));
}

QGeoCoordinate CaacCorePlugin::defaultHomePosition()
{
    return QGeoCoordinate(25.137904838950554, 102.72128132348416, 2062.0);
}

void CaacCorePlugin::initializeDefaultPlan(PlanMasterController* controller) const
{
    if (!controller || controller->property("flyView").toBool() ||
        controller->property("caacHomeDragActive").toBool() ||
        (!controller->offline() && (!_simulation || !_simulation->ownsVehicle(controller->managerVehicle()))) ||
        controller->syncInProgress()) {
        return;
    }
    auto* mission = controller->missionController();
    auto* items = mission->visualItems();
    auto* home = items->count() ? items->value<MissionSettingsItem*>(0) : nullptr;
    if (!home || home->property("caacHomeAltitudeManual").toBool()) {
        return;
    }
    auto coordinate = mission->plannedHomePosition();
    const bool hadHome = coordinate.isValid();
    if (!hadHome) {
        if (items->count() != 1) {
            return;
        }
        coordinate = defaultHomePosition();
    } else {
        const bool hasAltitude = std::isfinite(coordinate.altitude()) && coordinate.altitude() != 0;
        if (hasAltitude && !home->property("caacHomeAltitudeAutoRefresh").toBool()) {
            return;
        }
        double altitude = home->terrainAltitude();
        if (!std::isfinite(altitude)) {
            if (hasAltitude) {
                return;
            }
            // The configured training-field height is only a local fallback, never a global terrain estimate.
            if (coordinate.distanceTo(defaultHomePosition()) > 50) {
                return;
            }
            altitude = defaultHomePosition().altitude();
        }
        coordinate.setAltitude(altitude);
    }
    if (coordinate == mission->plannedHomePosition()) {
        return;
    }
    const bool wasDirty = mission->dirty();
    mission->setHomePosition(coordinate);
    mission->setDirty(hadHome || wasDirty);
}

bool CaacCorePlugin::keepCurrentPlanOnVehicleChange(PlanMasterController* controller) const
{
    return controller && _simulation->ownsVehicle(controller->managerVehicle());
}

QQmlApplicationEngine* CaacCorePlugin::createQmlApplicationEngine(QObject* parent)
{
    // Each launch opens the training site, independent of the last panned map view.
    const auto home = defaultHomePosition();
    QSettings settings;
    settings.beginGroup(QStringLiteral("FlightMapPosition"));
    settings.setValue(QStringLiteral("Latitude"), home.latitude());
    settings.setValue(QStringLiteral("Longitude"), home.longitude());
    settings.setValue(QStringLiteral("FlightMapZoom"), 19.0);
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
    auto* items = controller->missionController()->visualItems();
    if (items && items->count() && items->get(0)->property("caacHomeAltitudeManual").toBool()) {
        metadata[QStringLiteral("manualHomeAltitude")] = true;
    }
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
    auto* items = controller->missionController()->visualItems();
    if (items && items->count()) {
        items->get(0)->setProperty("caacHomeAltitudeManual",
                                   metadata.value(QStringLiteral("version")).toInt() == 1 &&
                                       metadata.value(QStringLiteral("manualHomeAltitude")).toBool());
    }
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
