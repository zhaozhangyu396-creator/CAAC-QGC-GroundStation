#include "CaacSimulationController.h"

#include <algorithm>
#include <cmath>

#include <QtCore/QCoreApplication>

#include "AppMessages.h"
#include "CaacCorePlugin.h"
#include "FirmwarePlugin.h"
#include "Joystick.h"
#include "JoystickManager.h"
#include "JoystickSettings.h"
#include "LinkManager.h"
#include "MissionController.h"
#include "MissionItem.h"
#include "MissionManager.h"
#include "MissionSettingsItem.h"
#include "MockConfiguration.h"
#include "MockLink.h"
#include "MockLinkMissionItemHandler.h"
#include "MultiVehicleManager.h"
#include "PlanMasterController.h"
#include "SimpleMissionItem.h"
#include "SpeedSection.h"
#include "Vehicle.h"

CaacSimulationController::CaacSimulationController(QObject* parent)
    : QObject(parent)
{
    _timer.setInterval(100);
    connect(&_timer, &QTimer::timeout, this, &CaacSimulationController::_tick);
    connect(QCoreApplication::instance(), &QCoreApplication::aboutToQuit, this, &CaacSimulationController::stop);
    QTimer::singleShot(0, this, [this]() {
        auto* manager = JoystickManager::instance();
        connect(manager, &JoystickManager::activeJoystickChanged, this, &CaacSimulationController::_setJoystick);
        _setJoystick(manager->activeJoystick());
    });
}

CaacSimulationController::~CaacSimulationController()
{
    stop();
}

QVariantMap CaacSimulationController::currentMissionDetails() const
{
    if (!_vehicle || !_missionReady || !_engine.loaded()) {
        return {};
    }
    const int offset = _vehicle->firmwarePlugin()->sendHomePositionToVehicle() ? 0 : 1;
    return {{QStringLiteral("sequence"), _engine.state().sequence + offset},
            {QStringLiteral("speed"), _engine.horizontalSpeed()},
            {QStringLiteral("climbMode"), _engine.currentClimbMode()}};
}

QVariantMap CaacSimulationController::defaults()
{
    return {{"horizontalSpeed", 5.0}, {"verticalSpeed", 2.0}, {"yawRate", 60.0},    {"windSpeed", 0.0},
            {"windBearing", 0.0},     {"turnMode", 1},        {"climbMode", 1},     {"holdSeconds", 1.0},
            {"specifyYaw", false},    {"yaw", 0.0},           {"closed", false},    {"loopMode", 0},
            {"loopCount", 1},         {"endBehavior", 1},     {"startSequence", 0}, {"returnSequence", 0},
            {"takeoffHeight", 10.0}};
}

QString CaacSimulationController::validateSettings(const QVariantMap& settings)
{
    const auto values = defaults();
    if (settings.size() != values.size()) {
        return tr("Simulation settings are incomplete.");
    }
    for (auto it = values.begin(); it != values.end(); ++it) {
        if (!settings.contains(it.key())) {
            return tr("Simulation settings are incomplete.");
        }
        if (it.key() == "closed" || it.key() == "specifyYaw") {
            if (settings.value(it.key()).metaType().id() != QMetaType::Bool) {
                return tr("Invalid simulation option.");
            }
            continue;
        }
        bool ok = false;
        const double value = settings.value(it.key()).toDouble(&ok);
        if (!ok || !std::isfinite(value)) {
            return tr("Enter finite simulation values.");
        }
        const auto key = it.key();
        double minimum = 0;
        double maximum = 360;
        bool integer = false;
        if (key == "horizontalSpeed") {
            minimum = 0.1;
            maximum = 100;
        } else if (key == "verticalSpeed") {
            minimum = 0.1;
            maximum = 30;
        } else if (key == "yawRate") {
            minimum = 0.1;
            maximum = 360;
        } else if (key == "windSpeed") {
            maximum = 50;
        } else if (key == "holdSeconds") {
            maximum = 3600;
        } else if (key == "takeoffHeight") {
            minimum = 0.1;
            maximum = 10000;
        } else if (key == "yaw") {
            minimum = -360;
            maximum = 360;
        } else if (key == "turnMode") {
            maximum = 3;
            integer = true;
        } else if (key == "climbMode" || key == "endBehavior") {
            maximum = 1;
            integer = true;
        } else if (key == "loopMode") {
            maximum = 2;
            integer = true;
        } else if (key == "loopCount") {
            minimum = 1;
            maximum = 32768;
            integer = true;
        } else if (key == "startSequence" || key == "returnSequence") {
            maximum = 65535;
            integer = true;
        }
        if (value < minimum || value > maximum || (integer && value != std::floor(value))) {
            return tr("Check the allowed ranges for simulation speeds, angles and counts.");
        }
    }
    return {};
}

Vehicle* CaacSimulationController::vehicle() const
{
    return _vehicle.data();
}

bool CaacSimulationController::ownsVehicle(const Vehicle* vehicle) const
{
    return vehicle && _link && vehicle->id() == _link->vehicleId();
}

bool CaacSimulationController::manualControlActive() const
{
    return _link && (_link->_mavBaseMode & MAV_MODE_FLAG_SAFETY_ARMED) &&
           (_link->_mavCustomMode == PX4CustomMode::POSCTL_POSCTL ||
            _link->_mavCustomMode == PX4CustomMode::STABILIZED);
}

void CaacSimulationController::_setJoystick(Joystick* joystick)
{
    disconnect(_joystickChannelsConnection);
    _joystick = joystick;
    _manualInputTime.invalidate();
    _modeMappingSignature.clear();
    _modeCandidatePosition = -1;
    _modeLastPosition = -1;
    _joystickChannels.clear();
    if (joystick) {
        _joystickChannelsConnection = connect(
            joystick, &Joystick::rawChannelValuesChanged, this,
            [this, source = QPointer<Joystick>(joystick)](const QVector<int>& channels) {
                if (source && source == _joystick) {
                    _processJoystickChannels(channels);
                }
            },
            Qt::QueuedConnection);
    }
    emit joystickChannelsChanged();
}

void CaacSimulationController::_processJoystickChannels(const QVector<int>& channels)
{
    _joystickChannels.clear();
    for (const int value : channels) {
        _joystickChannels.append(value);
    }
    if (!_joystick || !_link || !_vehicle || MultiVehicleManager::instance()->activeVehicle() != _vehicle ||
        !JoystickManager::instance()->activeJoystickEnabledForActiveVehicle()) {
        emit joystickChannelsChanged();
        return;
    }
    auto* settings = _joystick->settings();
    const bool enabled = settings->simulationModeSwitchEnabled()->rawValue().toBool();
    const int axis = settings->simulationModeSwitchAxis()->rawValue().toInt();
    const int positions = settings->simulationModeSwitchPositions()->rawValue().toInt();
    const bool reversed = settings->simulationModeSwitchReversed()->rawValue().toBool();
    const QList<int> modes{settings->simulationModeLow()->rawValue().toInt(),
                           settings->simulationModeMiddle()->rawValue().toInt(),
                           settings->simulationModeHigh()->rawValue().toInt()};
    const auto signature = QStringLiteral("%1/%2/%3/%4/%5/%6/%7")
                               .arg(enabled)
                               .arg(axis)
                               .arg(positions)
                               .arg(reversed)
                               .arg(modes[0])
                               .arg(modes[1])
                               .arg(modes[2]);
    if (_modeMappingSignature != signature) {
        _modeMappingSignature = signature;
        _modeCandidatePosition = -1;
        _modeLastPosition = -1;
    }
    if (!enabled || !modeSwitchAxisAllowed(axis) || axis >= channels.size() || channels[axis] < Joystick::AxisMin ||
        channels[axis] > Joystick::AxisMax || (positions != 2 && positions != 3) ||
        !_joystick->settings()->calibrated()->rawValue().toBool()) {
        emit joystickChannelsChanged();
        return;
    }
    const double value = std::clamp(channels[axis] / 32768.0, -1.0, 1.0) * (reversed ? -1 : 1);
    int position = positions == 2 ? (value < 0 ? 0 : 2) : (value < -0.4 ? 0 : value > 0.4 ? 2 : 1);
    // Keep the previous candidate in a small boundary band to reject switch noise.
    if (_modeCandidatePosition >= 0 &&
        (positions == 2 ? std::abs(value) < 0.1 : std::abs(std::abs(value) - 0.4) < 0.1)) {
        position = _modeCandidatePosition;
    }
    if (position != _modeCandidatePosition) {
        _modeCandidatePosition = position;
        _modeCandidateTime.restart();
    } else if (_modeCandidateTime.isValid() && _modeCandidateTime.elapsed() >= 150 && position != _modeLastPosition) {
        const bool firstSample = _modeLastPosition < 0;
        _modeLastPosition = position;
        // Reconnection and configuration establish a baseline; only a later switch movement commands a mode.
        if (!firstSample && _vehicle->armed() && _vehicle->airborne()) {
            _applyJoystickMode(modes[position]);
        }
    }
    emit joystickChannelsChanged();
}

bool CaacSimulationController::modeSwitchAxisAllowed(int axis) const
{
    if (!_joystick || axis < 0 || axis >= _joystick->axisCount()) {
        return false;
    }
    for (const auto function :
         {RemoteControlCalibrationController::stickFunctionRoll, RemoteControlCalibrationController::stickFunctionPitch,
          RemoteControlCalibrationController::stickFunctionYaw,
          RemoteControlCalibrationController::stickFunctionThrottle}) {
        if (_joystick->getChannelForFunction(function) == axis) {
            return false;
        }
    }
    return true;
}

void CaacSimulationController::_applyJoystickMode(int mode)
{
    if (!_link || !_vehicle || !ownsVehicle(_vehicle) || MultiVehicleManager::instance()->activeVehicle() != _vehicle) {
        return;
    }
    const QList<uint32_t> modes{PX4CustomMode::AUTO_LOITER, PX4CustomMode::STABILIZED, PX4CustomMode::AUTO_MISSION,
                                PX4CustomMode::AUTO_RTL, PX4CustomMode::AUTO_LAND};
    if (mode < 0 || mode >= modes.size() || (mode == 2 && (!_missionReady || _engine.state().finished))) {
        return;
    }
    _manualInputTime.invalidate();
    _holdAltitude = qQNaN();
    _guidedTarget = {};
    if (mode != 1) {
        _engine.pause();
    }
    _link->_mavCustomMode = modes[mode];
    _publish();
    emit statusChanged();
}

bool CaacSimulationController::beginManualControl()
{
    auto* manager = JoystickManager::instance();
    auto* joystick = manager->activeJoystick();
    if (!_emergencyMode || !_link || !_vehicle || !_vehicle->isInitialConnectComplete() || !_vehicle->airborne() ||
        MultiVehicleManager::instance()->activeVehicle() != _vehicle || !joystick || joystick->axisCount() < 4 ||
        (joystick->requiresCalibration() && !joystick->settings()->calibrated()->rawValue().toBool())) {
        _setStatus(tr("Connect and calibrate a USB joystick before taking manual control in flight."));
        return false;
    }
    manager->setActiveJoystickEnabledForActiveVehicle(true);
    if (!manager->activeJoystickEnabledForActiveVehicle()) {
        return false;
    }
    _manualInputTime.invalidate();
    _guidedTarget = {};
    _holdAltitude = qQNaN();
    _engine.pause();
    _link->_mavCustomMode = PX4CustomMode::STABILIZED;
    _setStatus(tr("Simulation manual control; waiting for joystick input."));
    _publish();
    emit statusChanged();
    return true;
}

void CaacSimulationController::endManualControl()
{
    if (!manualControlActive()) {
        return;
    }
    _manualInputTime.invalidate();
    _engine.pause();
    _link->_mavCustomMode = PX4CustomMode::AUTO_LOITER;
    _setStatus(tr("Simulation hovering"));
    _publish();
    emit statusChanged();
}

int CaacSimulationController::waypointNumber(int sequence, const QGeoCoordinate& coordinate) const
{
    const auto found = _waypointNumbers.constFind(sequence);
    if (!ownsVehicle(MultiVehicleManager::instance()->activeVehicle()) || found == _waypointNumbers.cend() ||
        !coordinate.isValid() || coordinate.distanceTo(found->coordinate) > 0.1) {
        return sequence;
    }
    return found->number;
}

void CaacSimulationController::_setStatus(const QString& status)
{
    if (_status != status) {
        _status = status;
        emit statusChanged();
    }
}

void CaacSimulationController::_fail(const QString& error)
{
    stop();
    _setStatus(error);
}

bool CaacSimulationController::start(PlanMasterController* plan)
{
    if (active() || !plan || plan->syncInProgress() || MultiVehicleManager::instance()->vehicles()->count() > 0) {
        _setStatus(tr("Stop the current simulation and disconnect other aircraft before starting."));
        return false;
    }
    _home = plan->missionController()->plannedHomePosition();
    _settings = defaults();
    _options = {};
    _engine = {};
    _holdAltitude = qQNaN();
    _guidedTarget = QGeoCoordinate();
    _uploadSent = false;
    _missionReady = false;
    _manualInputTime.invalidate();
    _modeMappingSignature.clear();
    _modeLastPosition = -1;
    auto* config = new MockConfiguration(tr("Built-in flight simulation"));
    config->setFirmwareType(MAV_AUTOPILOT_PX4);
    config->setVehicleType(MAV_TYPE_QUADROTOR);
    config->setSendStatusText(false);
    config->setSendRadioStatus(false);
    _link = MockLink::startMockLink(config);
    if (!_link) {
        _setStatus(tr("Unable to connect the simulation aircraft."));
        return false;
    }
    _planningPlan = plan;
    _planningPlan->setProperty("caacSimulationPlanning", true);
    _link->_caacSimulation = true;
    if (!_home.isValid()) {
        const double altitude = std::isfinite(_home.altitude()) ? _home.altitude() : 0;
        _home = QGeoCoordinate(25.137898058569604, 102.7212769885262, altitude);
        plan->missionController()->setHomePosition(_home);
    }
    if (!std::isfinite(_home.altitude())) {
        _home.setAltitude(0);
    }
    _engine.initialize(_home, _options);
    _waypointOptions.clear();
    _link->_vehicleAltitudeAMSL = _home.altitude();
    connect(_link, &MockLink::simulationAltitudeRequested, this, [this](double altitude) {
        _holdAltitude = altitude;
        _guidedTarget = _link ? _link->property("caacGuidedTarget").value<QGeoCoordinate>() : QGeoCoordinate();
    });
    connect(_link, &MockLink::simulationSpeedRequested, this, [this](double speed) {
        if (_link && _engine.setHorizontalSpeed(speed)) {
            _options.horizontalSpeed = speed;
        }
    });
    connect(_link, &MockLink::simulationManualControlRequested, this,
            [this](double forward, double right, double yaw, double throttle) {
                if (manualControlActive() && _vehicle && MultiVehicleManager::instance()->activeVehicle() == _vehicle) {
                    _manualForward = forward;
                    _manualRight = right;
                    _manualYaw = yaw;
                    _manualThrottle = throttle;
                    _manualInputTime.restart();
                }
            });
    connect(
        _link->_missionItemHandler, &MockLinkMissionItemHandler::missionUploaded, this,
        [this]() {
            if (!_link) {
                return;
            }
            const auto before = _engine.state();
            const bool wasLoaded = _engine.loaded();
            QString error;
            const bool preserve = _preserveUploadProgress;
            _preserveUploadProgress = false;
            const auto mission = _link->_missionItemHandler->uploadedMission();
            const bool loaded = preserve ? _engine.updateMission(mission, _home, _options, error, _waypointOptions)
                                         : _engine.load(mission, _home, _options, error, _waypointOptions);
            if (!loaded) {
                _uploadSent = false;
                _setStatus(error);
                emit missionUploadFailed();
                return;
            }
            if (!preserve && (wasLoaded || before.airborne)) {
                _engine.restoreState(before);
            }
            _missionReady = true;
            emit statusChanged();
        },
        Qt::QueuedConnection);
    connect(_link->_missionItemHandler, &MockLinkMissionItemHandler::missionSequenceRequested, this,
            [this](int sequence) {
                if (_engine.loaded()) {
                    _engine.jump(sequence);
                }
            });
    _elapsed.restart();
    _timer.start();
    _setStatus(tr("Connecting the simulation aircraft…"));
    emit statusChanged();
    return true;
}

bool CaacSimulationController::upload(PlanMasterController* plan, bool preserveProgress)
{
    if ((_vehicle && _vehicle->armed() && _vehicle->flightMode() == _vehicle->missionFlightMode()) ||
        (_link && (_link->_mavBaseMode & MAV_MODE_FLAG_SAFETY_ARMED) &&
         _link->_mavCustomMode == PX4CustomMode::AUTO_MISSION)) {
        const auto message = tr("当前航点任务正在执行，请先暂停当前任务，再上传新航线。");
        _setStatus(message);
        QGC::showAppMessage(message, tr("上传航线"));
        return false;
    }
    if (!_link || !_vehicle || !_vehicle->isInitialConnectComplete() || !plan || plan->syncInProgress() ||
        _vehicle->missionManager()->inProgress() || _uploadSent) {
        _setStatus(tr("Connect the simulation aircraft and pause the mission before uploading."));
        return false;
    }
    if (plan->property("caacRotationPreviewActive").toBool()) {
        _setStatus(QCoreApplication::translate(
            "PlanMasterController", "Confirm or cancel the orientation preview before saving or uploading."));
        return false;
    }
    auto* mission = plan->missionController();
    const auto home = mission->plannedHomePosition();
    auto settings = plan->property("caacSimulationSettings").toMap();
    const bool settingsExplicit = !settings.isEmpty();
    if (settings.isEmpty()) {
        settings = defaults();
    }
    const auto validation = validateSettings(settings);
    if (!validation.isEmpty() || !home.isValid() || !std::isfinite(home.altitude()) ||
        std::abs(home.altitude()) > 100000 || mission->readyForSaveState() != VisualMissionItem::ReadyForSave ||
        mission->visualItems()->count() <= 1) {
        _setStatus(validation.isEmpty() ? tr("Complete the planned home and mission before simulating.") : validation);
        return false;
    }
    _home = home;
    _settings = settings;
    _settingsExplicit = settingsExplicit;
    _sourceMission = {};
    mission->save(_sourceMission);
    _options = {.horizontalSpeed = _settings.value("horizontalSpeed").toDouble(),
                .verticalSpeed = _settings.value("verticalSpeed").toDouble(),
                .yawRate = _settings.value("yawRate").toDouble(),
                .windSpeed = _settings.value("windSpeed").toDouble(),
                .windBearing = _settings.value("windBearing").toDouble(),
                .turnMode = _settings.value("turnMode").toInt(),
                .climbMode = _settings.value("climbMode").toInt()};
    if (_uploadPlan) {
        _uploadPlan->deleteLater();
        _uploadPlan.clear();
    }
    if (!_prepareFlightPlan()) {
        return false;
    }
    if (preserveProgress) {
        const auto items = MissionController::createMissionItems(_uploadPlan->missionController()->visualItems(), this);
        QMap<uint16_t, mavlink_mission_item_int_t> wireMission;
        const int offset = _vehicle->firmwarePlugin()->sendHomePositionToVehicle() ? 0 : 1;
        for (int i = offset; i < items.size(); ++i) {
            const auto* item = items[i];
            mavlink_mission_item_int_t wire{};
            wire.seq = static_cast<uint16_t>(i - offset);
            wire.command = item->command();
            wire.frame = item->frame();
            wire.autocontinue = item->autoContinue();
            wire.param1 = item->param1() - (item->command() == MAV_CMD_DO_JUMP ? offset : 0);
            wire.param2 = item->param2();
            wire.param3 = item->param3();
            wire.param4 = item->param4();
            wire.x = static_cast<int32_t>(qRound64(item->param5() * 1e7));
            wire.y = static_cast<int32_t>(qRound64(item->param6() * 1e7));
            wire.z = item->param7();
            wireMission.insert(wire.seq, wire);
        }
        qDeleteAll(items);
        auto probe = _engine;
        QString error;
        if (!probe.updateMission(wireMission, _home, _options, error, _waypointOptions)) {
            _setStatus(error);
            return false;
        }
    }
    _preserveUploadProgress = preserveProgress;
    _uploadSent = true;
    _missionReady = false;
    _elapsed.restart();
    _setStatus(tr("Uploading the simulation mission…"));
    connect(
        _vehicle->missionManager(), &MissionManager::sendComplete, this,
        [this](bool error) {
            if (!_uploadSent) {
                return;
            }
            if (error) {
                _uploadSent = false;
                _setStatus(tr("Simulation mission upload failed."));
                emit missionUploadFailed();
            } else {
                _waypointNumbers = _pendingWaypointNumbers;
                ++_waypointNumberRevision;
                emit waypointNumbersChanged();
            }
        },
        Qt::SingleShotConnection);
    MissionController::sendItemsToVehicle(_vehicle, _uploadPlan->missionController()->visualItems());
    return true;
}

void CaacSimulationController::setEmergencyMode(bool enabled)
{
    if (enabled && (!_link || !_vehicle || !_vehicle->isInitialConnectComplete() ||
                    MultiVehicleManager::instance()->activeVehicle() != _vehicle)) {
        return;
    }
    if (_emergencyMode != enabled) {
        _emergencyMode = enabled;
        emit emergencyModeChanged();
    }
}

void CaacSimulationController::stop()
{
    _timer.stop();
    _manualInputTime.invalidate();
    setEmergencyMode(false);
    if (_planningPlan) {
        auto* manager = MultiVehicleManager::instance();
        if (ownsVehicle(manager->activeVehicle())) {
            const QPointer<PlanMasterController> plan = _planningPlan;
            // Keep the planning flag through queued vehicle removal so intentional simulation stops retain the plan.
            connect(
                manager, &MultiVehicleManager::activeVehicleChanged, plan,
                [plan](Vehicle*) {
                    if (plan) {
                        plan->setProperty("caacSimulationPlanning", false);
                    }
                },
                Qt::SingleShotConnection);
        } else {
            _planningPlan->setProperty("caacSimulationPlanning", false);
        }
    }
    if (_link) {
        const auto config = _link->linkConfiguration();
        LinkManager::instance()->disconnectLink(_link);
        if (config) {
            LinkManager::instance()->removeConfiguration(config.get());
        }
    }
    _planningPlan.clear();
    _uploadSent = false;
    _missionReady = false;
    _link.clear();
    _vehicle.clear();
    _waypointNumbers.clear();
    _pendingWaypointNumbers.clear();
    ++_waypointNumberRevision;
    emit waypointNumbersChanged();
    if (_uploadPlan) {
        _uploadPlan->deleteLater();
    }
    _uploadPlan.clear();
    _setStatus(tr("Simulation stopped"));
    emit statusChanged();
}

bool CaacSimulationController::restart()
{
    if (!_link || !_vehicle || !_missionReady || _vehicle->missionManager()->inProgress() ||
        MultiVehicleManager::instance()->vehicles()->count() != 1) {
        return false;
    }
    QString error;
    if (!_engine.load(_link->_missionItemHandler->uploadedMission(), _home, _options, error, _waypointOptions)) {
        _fail(error);
        return false;
    }
    _link->_mavBaseMode &= ~MAV_MODE_FLAG_SAFETY_ARMED;
    _link->_mavCustomMode = PX4CustomMode::AUTO_LOITER;
    _holdAltitude = qQNaN();
    _guidedTarget = QGeoCoordinate();
    _manualInputTime.invalidate();
    if (auto* plugin = qobject_cast<CaacCorePlugin*>(parent())) {
        plugin->resetFlightDistanceForSimulationRestart(_vehicle);
    }
    _publish();
    _setStatus(tr("Restarting simulation…"));
    return true;
}

void CaacSimulationController::_tick()
{
    if (!_link || !_link->isConnected()) {
        _fail(tr("Simulation connection closed."));
        return;
    }
    for (int i = 0; i < MultiVehicleManager::instance()->vehicles()->count(); ++i) {
        auto* vehicle = MultiVehicleManager::instance()->vehicles()->value<Vehicle*>(i);
        if (vehicle && vehicle->id() != _link->vehicleId()) {
            _fail(tr("Disconnect other aircraft before simulating."));
            return;
        }
    }
    if (!_vehicle) {
        for (int i = 0; i < MultiVehicleManager::instance()->vehicles()->count(); ++i) {
            auto* vehicle = MultiVehicleManager::instance()->vehicles()->value<Vehicle*>(i);
            if (vehicle && vehicle->id() == _link->vehicleId()) {
                _vehicle = vehicle;
                emit statusChanged();
                break;
            }
        }
    }
    _publish();
    if (!_vehicle || !_vehicle->isInitialConnectComplete()) {
        if (_elapsed.elapsed() > 30000) {
            _fail(tr("Simulation connection timed out."));
        }
        return;
    }
    if (_planningPlan && _planningPlan->missionController()->visualItems()->count() == 1 &&
        !_planningPlan->missionController()->plannedHomePosition().isValid()) {
        _planningPlan->missionController()->setHomePosition(_home);
    }
    if (_uploadSent) {
        if (_missionReady && !_vehicle->missionManager()->inProgress()) {
            _uploadSent = false;
            _setStatus(tr("Simulation mission uploaded; ready for flight controls."));
            emit missionUploaded();
        } else if (_elapsed.elapsed() > 60000) {
            _fail(tr("Simulation mission upload timed out."));
        }
        return;
    }
    const bool armed = _link->_mavBaseMode & MAV_MODE_FLAG_SAFETY_ARMED;
    if (!manualControlActive()) {
        _manualInputTime.invalidate();
    }
    if (manualControlActive()) {
        _guidedTarget = {};
        _holdAltitude = qQNaN();
        if (_manualInputTime.isValid() && _manualInputTime.elapsed() <= 500 &&
            MultiVehicleManager::instance()->activeVehicle() == _vehicle) {
            _engine.advanceManual(_manualForward, _manualRight, _manualYaw, _manualThrottle, 0.1,
                                  _link->_mavCustomMode == PX4CustomMode::STABILIZED);
            _setStatus(tr("Simulation controlled by USB joystick"));
            if (!_engine.state().airborne) {
                _link->_mavBaseMode &= ~MAV_MODE_FLAG_SAFETY_ARMED;
                _link->_mavCustomMode = PX4CustomMode::AUTO_LOITER;
                _manualInputTime.invalidate();
                _setStatus(tr("Simulation landed"));
            }
        } else {
            _engine.pause();
            _setStatus(tr("Joystick input lost; simulation hovering."));
        }
    } else if (armed && _link->_mavCustomMode == PX4CustomMode::AUTO_TAKEOFF) {
        _manualInputTime.invalidate();
        _engine.advanceHoldAltitude(_holdAltitude, 0.1);
        _setStatus(tr("Simulation taking off"));
        if (std::abs(_engine.state().position.altitude() - _holdAltitude) < 0.05) {
            _link->_mavCustomMode = PX4CustomMode::AUTO_LOITER;
            _setStatus(tr("Simulation hovering"));
        }
    } else if (armed && (_link->_mavCustomMode == PX4CustomMode::AUTO_LAND ||
                         _link->_mavCustomMode == PX4CustomMode::AUTO_RTL)) {
        const bool landed = _link->_mavCustomMode == PX4CustomMode::AUTO_LAND ? _engine.advanceLanding(0.1)
                                                                              : _engine.advanceReturn(0.1);
        _setStatus(tr("Simulation returning or landing"));
        if (landed) {
            _link->_mavBaseMode &= ~MAV_MODE_FLAG_SAFETY_ARMED;
            _link->_mavCustomMode = PX4CustomMode::AUTO_LOITER;
            _holdAltitude = qQNaN();
            _guidedTarget = QGeoCoordinate();
            _setStatus(tr("Simulation landed"));
        }
    } else if ((_link->_mavBaseMode & MAV_MODE_FLAG_SAFETY_ARMED) &&
               _link->_mavCustomMode == PX4CustomMode::AUTO_MISSION) {
        _guidedTarget = QGeoCoordinate();
        _holdAltitude = qQNaN();
        _engine.advance(0.1);
        _setStatus(tr("Simulation running · mission item %1").arg(_engine.state().sequence));
        if (_engine.state().finished) {
            _link->_mavCustomMode = PX4CustomMode::AUTO_LOITER;
            if (!_engine.state().airborne) {
                _link->_mavBaseMode &= ~MAV_MODE_FLAG_SAFETY_ARMED;
            }
            _setStatus(tr("Simulation mission finished"));
        }
    } else {
        _engine.pause();
        if (_link->_mavCustomMode == PX4CustomMode::AUTO_LOITER && _guidedTarget.isValid() &&
            (_link->_mavBaseMode & MAV_MODE_FLAG_SAFETY_ARMED)) {
            _engine.advanceGuidedTarget(_guidedTarget, 0.1);
            _setStatus(tr("Simulation guided flight"));
        } else if (armed && _link->_mavCustomMode == PX4CustomMode::AUTO_LOITER && std::isfinite(_holdAltitude)) {
            _engine.advanceHoldAltitude(_holdAltitude, 0.1);
        }
        if (!_engine.state().finished && !_guidedTarget.isValid()) {
            _setStatus(_missionReady ? tr("Simulation paused") : tr("Simulation connected; edit and upload a route."));
        }
    }
    _link->_vehicleAltitudeAMSL = _engine.state().position.altitude();
}

void CaacSimulationController::_publish()
{
    if (!_link || !_link->mavlinkChannelIsSet()) {
        return;
    }
    const auto details = currentMissionDetails();
    if (details != _publishedMissionDetails) {
        _publishedMissionDetails = details;
        emit missionDetailsChanged();
    }
    const auto state = _engine.state();
    const auto system = static_cast<uint8_t>(_link->vehicleId());
    const auto component = static_cast<uint8_t>(MAV_COMP_ID_AUTOPILOT1);
    const auto channel = _link->_outgoingMavlinkChannel;
    const auto time = static_cast<uint32_t>(_elapsed.elapsed());
    const float relativeAltitude = static_cast<float>(state.position.altitude() - _home.altitude());
    mavlink_message_t message{};
    mavlink_msg_heartbeat_pack_chan(system, component, channel, &message, MAV_TYPE_QUADROTOR, MAV_AUTOPILOT_PX4,
                                    _link->_mavBaseMode, _link->_mavCustomMode,
                                    state.airborne ? MAV_STATE_ACTIVE : MAV_STATE_STANDBY);
    _link->respondWithMavlinkMessage(message, true);
    const float identity[4] = {1, 0, 0, 0};
    mavlink_msg_home_position_pack_chan(
        system, component, channel, &message, static_cast<int32_t>(_home.latitude() * 1e7),
        static_cast<int32_t>(_home.longitude() * 1e7), static_cast<int32_t>(_home.altitude() * 1000), 0, 0, 0, identity,
        0, 0, 0, time * 1000ULL);
    _link->respondWithMavlinkMessage(message, true);
    mavlink_msg_global_position_int_pack_chan(
        system, component, channel, &message, time, static_cast<int32_t>(state.position.latitude() * 1e7),
        static_cast<int32_t>(state.position.longitude() * 1e7), static_cast<int32_t>(state.position.altitude() * 1000),
        static_cast<int32_t>(relativeAltitude * 1000), static_cast<int16_t>(state.northSpeed * 100),
        static_cast<int16_t>(state.eastSpeed * 100), static_cast<int16_t>(-state.climbSpeed * 100),
        static_cast<uint16_t>(state.heading * 100));
    _link->respondWithMavlinkMessage(message, true);
    const float speed = static_cast<float>(std::hypot(state.northSpeed, state.eastSpeed));
    mavlink_msg_vfr_hud_pack_chan(system, component, channel, &message, speed, speed,
                                  static_cast<int16_t>(state.heading), state.airborne ? 50 : 0,
                                  static_cast<float>(state.position.altitude()), static_cast<float>(state.climbSpeed));
    _link->respondWithMavlinkMessage(message, true);
    mavlink_msg_attitude_pack_chan(
        system, component, channel, &message, time, static_cast<float>(state.roll * M_PI / 180.0),
        static_cast<float>(state.pitch * M_PI / 180.0), static_cast<float>(state.heading * M_PI / 180.0), 0, 0, 0);
    _link->respondWithMavlinkMessage(message, true);
    float quaternion[4];
    mavlink_euler_to_quaternion(static_cast<float>(state.roll * M_PI / 180.0),
                                static_cast<float>(state.pitch * M_PI / 180.0),
                                static_cast<float>(state.heading * M_PI / 180.0), quaternion);
    const float representationOffset[4] = {};
    mavlink_msg_attitude_quaternion_pack_chan(system, component, channel, &message, time, quaternion[0], quaternion[1],
                                              quaternion[2], quaternion[3], 0, 0, 0, representationOffset);
    _link->respondWithMavlinkMessage(message, true);
    mavlink_msg_extended_sys_state_pack_chan(system, component, channel, &message, MAV_VTOL_STATE_UNDEFINED,
                                             state.airborne ? MAV_LANDED_STATE_IN_AIR : MAV_LANDED_STATE_ON_GROUND);
    _link->respondWithMavlinkMessage(message, true);
    mavlink_msg_gps_raw_int_pack_chan(
        system, component, channel, &message, time * 1000ULL, GPS_FIX_TYPE_3D_FIX,
        static_cast<int32_t>(state.position.latitude() * 1e7), static_cast<int32_t>(state.position.longitude() * 1e7),
        static_cast<int32_t>(state.position.altitude() * 1000), 100, 100, static_cast<uint16_t>(speed * 100),
        static_cast<uint16_t>(state.heading * 100), 10, 0, 0, 0, 0, 0, 0);
    _link->respondWithMavlinkMessage(message, true);
    if (_engine.loaded()) {
        const auto items = _link->_missionItemHandler->uploadedMission();
        const auto sequence = items.isEmpty() ? 0 : std::min(state.sequence, static_cast<int>(items.lastKey()));
        mavlink_msg_mission_current_pack_chan(
            system, component, channel, &message, static_cast<uint16_t>(sequence), static_cast<uint16_t>(items.size()),
            state.finished ? MISSION_STATE_COMPLETE : MISSION_STATE_ACTIVE, 1, 0, 0, 0);
        _link->respondWithMavlinkMessage(message, true);
    }
}

bool CaacSimulationController::_prepareFlightPlan()
{
    _uploadPlan = new PlanMasterController(this);
    _uploadPlan->setProperty("caacSimulationPlanning", true);
    _uploadPlan->setFlyView(false);
    _uploadPlan->start();
    auto* mission = _uploadPlan->missionController();
    QString error;
    if (!mission->load(_sourceMission, error)) {
        _fail(tr("Unable to prepare the simulation mission: %1").arg(error));
        return false;
    }
    auto* items = mission->visualItems();
    QList<SimpleMissionItem*> waypoints;
    QHash<const SimpleMissionItem*, int> waypointOrigins;
    bool takeoff = false;
    bool existingFlow = false;
    SimpleMissionItem* selectedStart = nullptr;
    SimpleMissionItem* selectedReturn = nullptr;
    for (int i = 1; i < items->count(); ++i) {
        auto* item = items->value<SimpleMissionItem*>(i);
        if (!item) {
            _fail(tr("Simulation flow requires a simple mission without existing jump commands."));
            return false;
        }
        existingFlow = existingFlow || item->command() == MAV_CMD_DO_JUMP;
        takeoff = takeoff || item->isTakeoffItem();
        if (item->command() == MAV_CMD_NAV_WAYPOINT) {
            waypoints.append(item);
            waypointOrigins.insert(item, item->waypointNumber());
            if (item->sequenceNumber() == _settings.value("startSequence").toInt()) {
                selectedStart = item;
            }
            if (item->sequenceNumber() == _settings.value("returnSequence").toInt()) {
                selectedReturn = item;
            }
        }
    }
    if (waypoints.isEmpty() ||
        (!existingFlow && ((_settings.value("startSequence").toInt() > 0 && !selectedStart) ||
                           (_settings.value("endBehavior").toInt() == 1 &&
                            _settings.value("returnSequence").toInt() > 0 && !selectedReturn)))) {
        _fail(tr("Select existing waypoints for the start and return points."));
        return false;
    }
    if (existingFlow) {
        for (auto it = waypointOrigins.begin(); it != waypointOrigins.end(); ++it) {
            it.value() = waypointNumber(it.key()->sequenceNumber(), it.key()->coordinate());
        }
        for (int i = 1; i < items->count(); ++i) {
            auto* item = items->value<SimpleMissionItem*>(i);
            if (item && item->command() == MAV_CMD_NAV_LOITER_UNLIM) {
                const int number = waypointNumber(item->sequenceNumber(), item->coordinate());
                if (number != item->sequenceNumber()) {
                    waypointOrigins.insert(item, number);
                }
            }
        }
    }
    const auto captureNumbers = [this, &waypointOrigins, existingFlow]() {
        _pendingWaypointNumbers.clear();
        const auto previousOptions = _waypointOptions;
        const int offset = _vehicle && !_vehicle->firmwarePlugin()->sendHomePositionToVehicle() ? 1 : 0;
        _waypointOptions.clear();
        for (auto it = waypointOrigins.cbegin(); it != waypointOrigins.cend(); ++it) {
            _pendingWaypointNumbers.insert(it.key()->sequenceNumber(), {it.value(), it.key()->coordinate()});
            const auto settings = it.key()->caacFlightSettings();
            if (!settings.isEmpty() && it.key()->command() == MAV_CMD_NAV_WAYPOINT) {
                _waypointOptions.insert(it.key()->sequenceNumber() - offset,
                                        {settings.value("verticalSpeed").toDouble(), settings.value("turnMode").toInt(),
                                         settings.value("climbMode").toInt()});
            } else if (existingFlow && it.key()->command() == MAV_CMD_NAV_WAYPOINT &&
                       previousOptions.contains(it.key()->sequenceNumber() - offset)) {
                _waypointOptions.insert(it.key()->sequenceNumber() - offset,
                                        previousOptions.value(it.key()->sequenceNumber() - offset));
            }
        }
    };
    if (existingFlow) {
        captureNumbers();
        return true;
    }
    if (!selectedStart) {
        selectedStart = waypoints.first();
    }
    bool nativeEnd = false;
    auto* last = items->value<SimpleMissionItem*>(items->count() - 1);
    const bool plannedLanding = last && last->command() == MAV_CMD_NAV_LAND;
    const auto landingCoordinate = plannedLanding ? last->coordinate() : QGeoCoordinate();
    const auto landingFrame = plannedLanding ? last->altitudeFrame() : QGroundControlQmlGlobal::AltitudeFrameRelative;
    const double landingAltitude = plannedLanding ? last->editableAlt() : 0;
    const double landingYaw = plannedLanding ? last->missionItem().param4() : qQNaN();
    if (last && (last->command() == MAV_CMD_NAV_LAND || last->command() == MAV_CMD_NAV_RETURN_TO_LAUNCH ||
                 last->command() == MAV_CMD_NAV_LOITER_UNLIM)) {
        nativeEnd = true;
        if (_settingsExplicit || plannedLanding) {
            mission->removeVisualItem(items->count() - 1);
            nativeEnd = false;
        }
    }
    for (auto* waypoint : waypoints) {
        if (_settingsExplicit && waypoint->caacFlightSettings().isEmpty() &&
            (_options.turnMode == 0 || _options.turnMode == 3) && waypoint->missionItem().param1() == 0) {
            waypoint->missionItem().setParam1(_settings.value("holdSeconds").toDouble());
        }
        if (_settingsExplicit && waypoint->caacFlightSettings().isEmpty() &&
            (_options.turnMode == 0 || _options.turnMode == 3) && std::isnan(waypoint->missionItem().param4()) &&
            _settings.value("specifyYaw").toBool()) {
            waypoint->missionItem().setParam4(_settings.value("yaw").toDouble());
        }
    }
    if (!takeoff) {
        auto* item = qobject_cast<SimpleMissionItem*>(mission->insertTakeoffItem(_home, 1, false));
        if (!item) {
            _fail(tr("Unable to create the simulation takeoff."));
            return false;
        }
        item->setCoordinate(_home);
        item->altitude()->setRawValue(_settings.value("takeoffHeight"));
    } else if (_settingsExplicit) {
        for (int i = 1; i < items->count(); ++i) {
            auto* item = items->value<SimpleMissionItem*>(i);
            if (item && item->isTakeoffItem()) {
                item->setAltitudeFrame(QGroundControlQmlGlobal::AltitudeFrameRelative);
                item->altitude()->setRawValue(_settings.value("takeoffHeight"));
            }
        }
    }
    const bool closed = _settings.value("closed").toBool();
    if (closed) {
        // Rotate the copied route so the preview and executed circle both start at the selected waypoint.
        const int startIndex = waypoints.indexOf(selectedStart);
        auto* tail = waypoints.last();
        for (int i = 0; i < startIndex; ++i) {
            if (!mission->moveVisualItem(items->indexOf(waypoints.at(i)), items->indexOf(tail))) {
                _fail(tr("Unable to prepare the simulation mission."));
                return false;
            }
            tail = waypoints.at(i);
        }
    }
    const auto append = [mission, items, &waypointOrigins](int command, const SimpleMissionItem* source) {
        const auto coordinate = source ? source->coordinate() : QGeoCoordinate();
        auto* item =
            qobject_cast<SimpleMissionItem*>(mission->insertSimpleMissionItem(coordinate, items->count(), false));
        if (!item) {
            return item;
        }
        item->setCommand(command);
        if (source && command == MAV_CMD_NAV_WAYPOINT) {
            item->missionItem().setParam1(source->missionItem().param1());
            item->missionItem().setParam2(source->missionItem().param2());
            item->missionItem().setParam3(source->missionItem().param3());
        }
        if (source && (command == MAV_CMD_NAV_WAYPOINT || command == MAV_CMD_NAV_LOITER_UNLIM)) {
            item->missionItem().setParam4(source->missionItem().param4());
        }
        if (source && item->specifiesAltitude()) {
            item->setAltitudeFrame(source->altitudeFrame());
            item->altitude()->setRawValue(source->editableAlt());
        }
        if (source && waypointOrigins.contains(source)) {
            waypointOrigins.insert(item, waypointOrigins.value(source));
        }
        return item;
    };
    if (!closed && selectedStart != waypoints.first()) {
        const int firstWaypointIndex = items->indexOf(waypoints.first());
        auto* jump = qobject_cast<SimpleMissionItem*>(
            mission->insertSimpleMissionItem(QGeoCoordinate(), firstWaypointIndex, false));
        if (!jump) {
            _fail(tr("Unable to prepare the simulation mission."));
            return false;
        }
        jump->setCommand(MAV_CMD_DO_JUMP);
        jump->missionItem().setParam1(selectedStart->sequenceNumber());
        jump->missionItem().setParam2(1);
    }
    auto* finishPoint = waypoints.last();
    if (closed) {
        finishPoint = append(MAV_CMD_NAV_WAYPOINT, selectedStart);
        if (!finishPoint) {
            _fail(tr("Unable to close the simulation route."));
            return false;
        }
        finishPoint->setCaacFlightSettings(selectedStart->caacFlightSettings());
    }
    const int loopMode = _settings.value("loopMode").toInt();
    if (loopMode > 0) {
        auto* jump = append(MAV_CMD_DO_JUMP, nullptr);
        if (!jump) {
            _fail(tr("Unable to prepare the simulation loop."));
            return false;
        }
        // The closing waypoint has already reached the start; skip its duplicate arrival and hold.
        jump->missionItem().setParam1(selectedStart->sequenceNumber() + (closed ? 1 : 0));
        jump->missionItem().setParam2(loopMode == 2 ? -1 : _settings.value("loopCount").toInt() - 1);
    }
    if (plannedLanding) {
        auto* landing = qobject_cast<SimpleMissionItem*>(
            mission->insertSimpleMissionItem(landingCoordinate, items->count(), false));
        if (!landing) {
            _fail(tr("Unable to create the landing point."));
            return false;
        }
        landing->setCommand(MAV_CMD_NAV_LAND);
        landing->setAltitudeFrame(landingFrame);
        landing->altitude()->setRawValue(landingAltitude);
        landing->missionItem().setParam4(landingYaw);
    } else if (!nativeEnd) {
        if (_settings.value("endBehavior").toInt() == 1 && !selectedReturn) {
            if (!append(MAV_CMD_NAV_RETURN_TO_LAUNCH, nullptr)) {
                _fail(tr("Unable to prepare the simulation return."));
                return false;
            }
        } else {
            auto* source = _settings.value("endBehavior").toInt() == 1 && selectedReturn ? selectedReturn : finishPoint;
            if (!append(MAV_CMD_NAV_LOITER_UNLIM, source)) {
                _fail(tr("Unable to prepare the simulation hover."));
                return false;
            }
        }
    }
    captureNumbers();
    return true;
}
