#include "CaacMissionUpdateController.h"

#include <cmath>
#include <limits>

#include <QtCore/QCoreApplication>

#include "CaacMissionUpdateUtils.h"
#include "CaacSimulationController.h"
#include "FirmwarePlugin.h"
#include "MissionController.h"
#include "MissionItem.h"
#include "MissionManager.h"
#include "MultiVehicleManager.h"
#include "PlanMasterController.h"
#include "Vehicle.h"
#include "VehicleLinkManager.h"
#include "VehicleSupports.h"
#include "VisualMissionItem.h"

CaacMissionUpdateController::CaacMissionUpdateController(QObject* parent)
    : QObject(parent)
{
    _timer.setInterval(100);
    connect(&_timer, &QTimer::timeout, this, &CaacMissionUpdateController::_tick);
}

CaacMissionUpdateController::~CaacMissionUpdateController()
{
    for (const auto& connection : _connections) {
        disconnect(connection);
    }
    qDeleteAll(_pendingItems);
    if (_plan) {
        _plan->setProperty("caacMissionUpdateActive", false);
    }
}

void CaacMissionUpdateController::_setStage(Stage stage)
{
    const bool wasBusy = busy();
    _stage = stage;
    _elapsed.restart();
    if (wasBusy != busy()) {
        emit busyChanged();
    }
}

bool CaacMissionUpdateController::update(PlanMasterController* plan, CaacSimulationController* simulation)
{
    return _update(plan, simulation, false, qQNaN(), -1);
}

bool CaacMissionUpdateController::updateSpeeds(PlanMasterController* plan, CaacSimulationController* simulation,
                                               double currentFlightSpeed, int expectedCurrentSequence)
{
    return _update(plan, simulation, true, currentFlightSpeed, expectedCurrentSequence);
}

bool CaacMissionUpdateController::_update(PlanMasterController* plan, CaacSimulationController* simulation,
                                          bool allowSpeedChanges, double currentFlightSpeed,
                                          int expectedCurrentSequence)
{
    if (busy()) {
        return false;
    }
    if (!std::isnan(currentFlightSpeed) &&
        (!std::isfinite(currentFlightSpeed) || currentFlightSpeed <= 0 || currentFlightSpeed > 100)) {
        emit finished(false, QCoreApplication::translate("CaacFlightSpeedDialog",
                                                         "Enter a valid flight speed within the aircraft limits."));
        return false;
    }
    auto* vehicle = plan ? plan->managerVehicle() : nullptr;
    if (!vehicle || vehicle != MultiVehicleManager::instance()->activeVehicle() || plan->offline() ||
        !vehicle->isInitialConnectComplete() || !vehicle->armed() ||
        (vehicle->flightMode() != vehicle->missionFlightMode() &&
         vehicle->flightMode() != vehicle->firmwarePlugin()->pauseFlightMode()) ||
        !vehicle->supports()->pauseVehicle() || vehicle->firmwarePlugin()->pauseFlightMode().isEmpty() ||
        plan->syncInProgress() || plan->property("caacMissionUpdateActive").toBool() ||
        plan->property("caacRotationPreviewActive").toBool() ||
        plan->missionController()->readyForSaveState() != VisualMissionItem::ReadyForSave) {
        emit finished(false, tr("The mission or connection changed. No waypoint update was sent."));
        return false;
    }
    _vehicle = vehicle;
    _plan = plan;
    _simulation = simulation && simulation->ownsVehicle(vehicle) ? simulation : nullptr;
    _allowSpeedChanges = allowSpeedChanges;
    _forcedFlightSpeed = currentFlightSpeed;
    _expectedCurrentSequence = expectedCurrentSequence;
    _flightSpeedType = vehicle->inFwdFlight() ? 0 : 1;
    _runtimeFlightSpeed =
        vehicle->commandedFlightSpeedType() == _flightSpeedType ? vehicle->commandedFlightSpeed() : qQNaN();
    _desiredFlightSpeed = qQNaN();
    _snapshot = plan->saveToJson();
    _lastTransferError.clear();
    plan->setProperty("caacMissionUpdateActive", true);
    _setStage(Stage::Pausing);
    _timer.start();
    vehicle->pauseVehicle();
    return true;
}

bool CaacMissionUpdateController::_prepareNative()
{
    if (!_vehicle || !_plan) {
        return false;
    }
    auto* manager = _vehicle->missionManager();
    _pendingItems = MissionController::createMissionItems(_plan->missionController()->visualItems(), this);
    const int offset = _vehicle->firmwarePlugin()->sendHomePositionToVehicle() ? 0 : 1;
    const auto& previous = manager->missionItems();
    if (_allowSpeedChanges) {
        return _prepareNativeSpeeds();
    }
    if (_pendingItems.size() != previous.size() + offset) {
        return false;
    }
    const auto equal = [](double left, double right) {
        return std::abs(left - right) < 1e-5 || (std::isnan(left) && std::isnan(right));
    };
    int jumps = 0;
    for (int i = 0; i < previous.size(); ++i) {
        auto* item = _pendingItems[i + offset];
        const auto* old = previous[i];
        if (item->command() != old->command() || item->frame() != old->frame() ||
            item->autoContinue() != old->autoContinue() || !equal(item->param3(), old->param3()) ||
            !equal(item->param4(), old->param4()) || !equal(item->param7(), old->param7())) {
            return false;
        }
        if (item->command() == MAV_CMD_DO_JUMP) {
            ++jumps;
            if (jumps > 1 || !equal(item->param1() - offset, old->param1()) || old->param1() >= i ||
                manager->remainingLoops() == -2) {
                return false;
            }
            item->setParam2(manager->remainingLoops() < 0 ? -1 : qMax(0, manager->remainingLoops() - 1));
        } else if (!equal(item->param1(), old->param1()) || !equal(item->param2(), old->param2())) {
            return false;
        }
        if (item->command() != MAV_CMD_NAV_WAYPOINT && item->command() != MAV_CMD_NAV_LAND &&
            (!equal(item->param5(), old->param5()) || !equal(item->param6(), old->param6()))) {
            return false;
        }
    }
    return _target >= 0 && _target < previous.size();
}

bool CaacMissionUpdateController::_prepareNativeSpeeds()
{
    if (!_vehicle || !_plan) {
        return false;
    }
    auto* manager = _vehicle->missionManager();
    const int offset = _vehicle->firmwarePlugin()->sendHomePositionToVehicle() ? 0 : 1;
    const auto& previous = manager->missionItems();
    const auto wireMission = [](const QList<MissionItem*>& items, int homeOffset) {
        CaacMissionUpdateUtils::Mission result;
        const auto coordinate = [](double value, double scale, int32_t& output) {
            const double scaled = value * scale;
            if (std::isnan(scaled)) {
                output = 0;
                return true;
            }
            if (!std::isfinite(scaled) || scaled < std::numeric_limits<int32_t>::min() ||
                scaled > std::numeric_limits<int32_t>::max()) {
                return false;
            }
            output = static_cast<int32_t>(scaled);
            return true;
        };
        for (int i = homeOffset; i < items.size(); ++i) {
            const auto* item = items[i];
            if (!item) {
                return CaacMissionUpdateUtils::Mission();
            }
            mavlink_mission_item_int_t wire{};
            wire.seq = i - homeOffset;
            wire.command = item->command();
            wire.frame = item->frame();
            wire.autocontinue = item->autoContinue();
            wire.param1 = item->param1() - (item->command() == MAV_CMD_DO_JUMP ? homeOffset : 0);
            wire.param2 = item->param2();
            wire.param3 = item->param3();
            wire.param4 = item->param4();
            const double scale = item->frame() == MAV_FRAME_MISSION ? 1 : 1e7;
            if (!coordinate(item->param5(), scale, wire.x) || !coordinate(item->param6(), scale, wire.y)) {
                return CaacMissionUpdateUtils::Mission();
            }
            wire.z = item->param7();
            result.insert(wire.seq, wire);
        }
        return result;
    };
    auto oldWire = wireMission(previous, 0);
    const auto newWire = wireMission(_pendingItems, offset);
    auto mapping = CaacMissionUpdateUtils::speedUpdateMapping(oldWire, newWire);
    if (offset > 0) {
        auto downloadedWire = oldWire;
        for (auto it = downloadedWire.begin(); it != downloadedWire.end(); ++it) {
            if (it->command == MAV_CMD_DO_JUMP) {
                // Downloaded PX4 items retain wire sequence numbers but expose home-adjusted jump targets.
                it->param1 -= offset;
            }
        }
        const auto downloadedMapping = CaacMissionUpdateUtils::speedUpdateMapping(downloadedWire, newWire);
        if (mapping.valid && downloadedMapping.valid) {
            for (auto it = oldWire.cbegin(); it != oldWire.cend(); ++it) {
                if (it->command == MAV_CMD_DO_JUMP &&
                    mapping.jumpTargetToNew.value(static_cast<int>(it->param1), -1) !=
                        downloadedMapping.jumpTargetToNew.value(static_cast<int>(downloadedWire.value(it.key()).param1),
                                                                -1)) {
                    return false;
                }
            }
        } else if (!mapping.valid && downloadedMapping.valid) {
            oldWire = downloadedWire;
            mapping = downloadedMapping;
        }
    }
    if (!mapping.valid || !mapping.oldToNew.contains(_target)) {
        return false;
    }
    const auto equal = [](double left, double right) {
        return std::abs(left - right) < 1e-5 || (std::isnan(left) && std::isnan(right));
    };
    int jumps = 0;
    for (int i = 0; i < previous.size(); ++i) {
        if (CaacMissionUpdateUtils::isHorizontalAbsoluteSpeed(oldWire.value(i))) {
            continue;
        }
        const int newIndex = mapping.oldToNew.value(i, -1);
        if (newIndex < 0 || newIndex + offset >= _pendingItems.size()) {
            return false;
        }
        const auto* old = previous[i];
        auto* item = _pendingItems[newIndex + offset];
        if (!equal(item->param3(), old->param3()) || !equal(item->param4(), old->param4()) ||
            !equal(item->param7(), old->param7()) || !equal(item->param2(), old->param2()) ||
            (item->command() != MAV_CMD_DO_JUMP && !equal(item->param1(), old->param1()))) {
            return false;
        }
        if (item->command() != MAV_CMD_NAV_WAYPOINT && item->command() != MAV_CMD_NAV_LAND &&
            (!equal(item->param5(), old->param5()) || !equal(item->param6(), old->param6()))) {
            return false;
        }
        if (item->command() == MAV_CMD_DO_JUMP) {
            ++jumps;
            if (jumps > 1 || oldWire.value(i).param1 >= i || manager->remainingLoops() == -2) {
                return false;
            }
            item->setParam2(manager->remainingLoops() < 0 ? -1 : qMax(0, manager->remainingLoops() - 1));
        }
    }
    const double fallback = _flightSpeedType == 0 ? _vehicle->defaultCruiseSpeed() : _vehicle->defaultHoverSpeed();
    const auto prefixSpeed = [this, fallback](const CaacMissionUpdateUtils::Mission& mission, int target) {
        double speed = fallback;
        for (auto it = mission.cbegin(); it != mission.cend() && it.key() < target; ++it) {
            if (CaacMissionUpdateUtils::isHorizontalAbsoluteSpeed(it.value()) && it->param1 == _flightSpeedType) {
                speed = it->param2;
            }
        }
        return speed;
    };
    const double oldSpeed = prefixSpeed(oldWire, _target);
    _target = mapping.oldToNew.value(_target);
    const double newSpeed = prefixSpeed(newWire, _target);
    _desiredFlightSpeed = std::isfinite(_forcedFlightSpeed) ? _forcedFlightSpeed
                          : equal(oldSpeed, newSpeed) && std::isfinite(_runtimeFlightSpeed) && _runtimeFlightSpeed > 0
                              ? _runtimeFlightSpeed
                              : newSpeed;
    return std::isfinite(_desiredFlightSpeed) && _desiredFlightSpeed > 0;
}

void CaacMissionUpdateController::_applySpeed()
{
    if (!_vehicle) {
        return;
    }
    _setStage(Stage::ApplyingSpeed);
    _connections.append(connect(
        _vehicle, &Vehicle::mavCommandResult, this, [this](int, int component, int command, int result, int failure) {
            if (_stage != Stage::ApplyingSpeed || !_vehicle || component != _vehicle->defaultComponentId() ||
                command != MAV_CMD_DO_CHANGE_SPEED) {
                return;
            }
            if (!_plan || _vehicle != MultiVehicleManager::instance()->activeVehicle() ||
                _plan->managerVehicle() != _vehicle || !_vehicle->armed() ||
                _vehicle->vehicleLinkManager()->communicationLost()) {
                _finish(false, tr("Connection or aircraft state changed. Automatic resume was cancelled."));
                return;
            }
            if (_vehicle->flightMode() != _vehicle->firmwarePlugin()->pauseFlightMode()) {
                _finish(false, tr("Flight mode changed. Automatic resume was cancelled."));
                return;
            }
            if (result == MAV_RESULT_ACCEPTED && failure == Vehicle::MavCmdResultCommandResultOnly) {
                _setStage(Stage::Resuming);
                _vehicle->setFlightMode(_vehicle->missionFlightMode());
            } else {
                _finish(false, tr("The aircraft did not accept the flight speed. The mission remains paused."));
            }
        }));
    if (_flightSpeedType == 0) {
        _vehicle->guidedModeChangeEquivalentAirspeedMetersSecond(_desiredFlightSpeed);
    } else {
        _vehicle->guidedModeChangeGroundSpeedMetersSecond(_desiredFlightSpeed);
    }
}

void CaacMissionUpdateController::_tick()
{
    if (!_vehicle || !_plan || _vehicle != MultiVehicleManager::instance()->activeVehicle() ||
        _plan->managerVehicle() != _vehicle || !_vehicle->armed() ||
        _vehicle->vehicleLinkManager()->communicationLost()) {
        _finish(false, tr("Connection or aircraft state changed. Automatic resume was cancelled."));
        return;
    }
    const auto mode = _vehicle->flightMode();
    const auto pauseMode = _vehicle->firmwarePlugin()->pauseFlightMode();
    if (_elapsed.elapsed() > (_stage == Stage::Uploading ? 65000 : 15000)) {
        _vehicle->pauseVehicle();
        _finish(false, tr("Waypoint update timed out. The mission has not been automatically resumed."));
        return;
    }
    if (_stage == Stage::Pausing) {
        if (mode != pauseMode) {
            if (mode != _vehicle->missionFlightMode()) {
                _finish(false, tr("Flight mode changed. The waypoint update was cancelled."));
            }
            return;
        }
        if (_plan->syncInProgress()) {
            return;
        }
        if (_plan->saveToJson() != _snapshot) {
            _finish(false, tr("The plan changed while pausing. The mission remains paused."));
            return;
        }
        _target = _vehicle->missionManager()->currentIndex();
        const int homeOffset = _vehicle->firmwarePlugin()->sendHomePositionToVehicle() ? 0 : 1;
        if (_allowSpeedChanges && _expectedCurrentSequence >= 0 && _target + homeOffset != _expectedCurrentSequence) {
            _forcedFlightSpeed = qQNaN();
        }
        if (_target < 0 || (!_simulation && !_prepareNative())) {
            _finish(false,
                    _allowSpeedChanges
                        ? tr("The mission structure changed. The speed update was cancelled.")
                        : tr("Only waypoint positions can be updated during flight. Pause and upload other mission "
                             "changes manually."));
            return;
        }
        auto* manager = _vehicle->missionManager();
        _connections.append(connect(manager, &MissionManager::error, this, [this](int code, const QString& message) {
            if (_stage == Stage::Uploading) {
                _lastTransferError = tr("Upload error %1: %2").arg(code).arg(message);
            }
        }));
        _connections.append(connect(manager, &MissionManager::sendComplete, this, [this](bool error) {
            if (_stage != Stage::Uploading) {
                return;
            }
            if (error) {
                _uploadFailed();
            } else if (!_simulation) {
                _uploaded();
            }
        }));
        _setStage(Stage::Uploading);
        if (_simulation) {
            _connections.append(connect(_simulation, &CaacSimulationController::missionUploaded, this,
                                        &CaacMissionUpdateController::_uploaded));
            _connections.append(connect(_simulation, &CaacSimulationController::missionUploadFailed, this,
                                        [this]() { _uploadFailed(_simulation ? _simulation->status() : QString()); }));
            if (!_simulation->upload(_plan, true, _allowSpeedChanges, _forcedFlightSpeed)) {
                _uploadFailed(_simulation ? _simulation->status() : QString());
            }
        } else {
            const auto items = _pendingItems;
            _pendingItems.clear();
            for (auto* item : items) {
                item->setParent(manager);
            }
            _plan->missionController()->trackMissionUpload();
            manager->writeMissionItems(items);
        }
    } else if (_stage == Stage::Uploading || _stage == Stage::Restoring || _stage == Stage::ApplyingSpeed) {
        if (mode != pauseMode) {
            _vehicle->pauseVehicle();
            _finish(false, tr("Flight mode changed during upload. Automatic resume was cancelled."));
        } else if (_stage == Stage::Restoring && _vehicle->missionManager()->currentIndex() == _target) {
            if (_allowSpeedChanges && !_simulation) {
                _applySpeed();
            } else {
                _setStage(Stage::Resuming);
                _vehicle->setFlightMode(_vehicle->missionFlightMode());
            }
        }
    } else if (_stage == Stage::Resuming) {
        if (mode == _vehicle->missionFlightMode()) {
            _finish(true, tr("Waypoints updated. Continuing from the previous target waypoint."));
        } else if (mode != pauseMode) {
            _finish(false, tr("Flight mode changed. Automatic resume was cancelled."));
        }
    }
}

void CaacMissionUpdateController::_uploaded()
{
    if (_stage != Stage::Uploading || !_vehicle) {
        return;
    }
    if (_vehicle->flightMode() != _vehicle->firmwarePlugin()->pauseFlightMode()) {
        _vehicle->pauseVehicle();
        _finish(false, tr("Flight mode changed during upload. Automatic resume was cancelled."));
        return;
    }
    _setStage(Stage::Restoring);
    const int offset = _vehicle->firmwarePlugin()->sendHomePositionToVehicle() ? 0 : 1;
    if (_simulation && _allowSpeedChanges) {
        _target = _simulation->currentMissionDetails().value(QStringLiteral("sequence"), -1).toInt() - offset;
        if (_target < 0) {
            _finish(false, tr("The mission structure changed. The speed update was cancelled."));
            return;
        }
    }
    if (!_simulation) {
        _vehicle->setCurrentMissionSequence(_target + offset);
    }
}

void CaacMissionUpdateController::_uploadFailed(const QString& detail)
{
    if (!busy()) {
        return;
    }
    const auto reason = _lastTransferError.isEmpty() ? detail : _lastTransferError;
    _finish(false, reason.isEmpty() ? tr("Waypoint upload failed. The mission remains paused.")
                                    : tr("Waypoint upload failed. The mission remains paused.\n%1").arg(reason));
}

void CaacMissionUpdateController::_finish(bool success, const QString& message)
{
    _timer.stop();
    for (const auto& connection : _connections) {
        disconnect(connection);
    }
    _connections.clear();
    qDeleteAll(_pendingItems);
    _pendingItems.clear();
    if (_plan) {
        _plan->setProperty("caacMissionUpdateActive", false);
    }
    _setStage(Stage::Idle);
    _vehicle.clear();
    _plan.clear();
    _simulation.clear();
    emit finished(success, message);
}
