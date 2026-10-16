#include "CaacMissionUpdateController.h"

#include <cmath>

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
    if (busy()) {
        return false;
    }
    auto* vehicle = plan ? plan->managerVehicle() : nullptr;
    if (!vehicle || vehicle != MultiVehicleManager::instance()->activeVehicle() || plan->offline() ||
        !vehicle->isInitialConnectComplete() || !vehicle->armed() ||
        (vehicle->flightMode() != vehicle->missionFlightMode() &&
         vehicle->flightMode() != vehicle->firmwarePlugin()->pauseFlightMode()) ||
        !vehicle->supports()->pauseVehicle() || vehicle->firmwarePlugin()->pauseFlightMode().isEmpty() ||
        plan->syncInProgress() || plan->property("caacRotationPreviewActive").toBool() ||
        plan->missionController()->readyForSaveState() != VisualMissionItem::ReadyForSave) {
        emit finished(false, tr("The mission or connection changed. No waypoint update was sent."));
        return false;
    }
    _vehicle = vehicle;
    _plan = plan;
    _simulation = simulation && simulation->ownsVehicle(vehicle) ? simulation : nullptr;
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
        if (_target < 0 || (!_simulation && !_prepareNative())) {
            _finish(false, tr("Only waypoint positions can be updated during flight. Pause and upload other mission "
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
            if (!_simulation->upload(_plan, true)) {
                _uploadFailed(_simulation ? _simulation->status() : QString());
            }
        } else {
            const auto items = _pendingItems;
            _pendingItems.clear();
            for (auto* item : items) {
                item->setParent(manager);
            }
            manager->writeMissionItems(items);
        }
    } else if (_stage == Stage::Uploading || _stage == Stage::Restoring) {
        if (mode != pauseMode) {
            _vehicle->pauseVehicle();
            _finish(false, tr("Flight mode changed during upload. Automatic resume was cancelled."));
        } else if (_stage == Stage::Restoring && _vehicle->missionManager()->currentIndex() == _target) {
            _setStage(Stage::Resuming);
            _vehicle->setFlightMode(_vehicle->missionFlightMode());
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
