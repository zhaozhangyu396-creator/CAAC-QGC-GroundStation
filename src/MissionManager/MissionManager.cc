#include "MissionManager.h"

#include <cmath>

#include "AppMessages.h"
#include "FirmwarePlugin.h"
#include "MAVLinkLib.h"
#include "MAVLinkProtocol.h"
#include "MissionCommandTree.h"
#include "MissionCommandUIInfo.h"
#include "QGCLoggingCategory.h"
#include "Vehicle.h"
#include "VehicleLinkManager.h"

QGC_LOGGING_CATEGORY(MissionManagerLog, "PlanManager.MissionManager")

MissionManager::MissionManager(Vehicle* vehicle)
    : PlanManager               (vehicle, MAV_MISSION_TYPE_MISSION)
    , _cachedLastCurrentIndex   (-1)
{
    if (!_vehicle) {
        return;
    }
    connect(_vehicle, &Vehicle::mavlinkMessageReceived, this, &MissionManager::_mavlinkMessageReceived);
    connect(this, &PlanManager::newMissionItemsAvailable, this, [this]() { _configureLoopProgress(false); });
    connect(this, &PlanManager::sendComplete, this, [this](bool error) {
        if (!error) {
            _configureLoopProgress(true);
        }
    });
    connect(this, &PlanManager::removeAllComplete, this, [this](bool error) {
        if (!error) {
            _configureLoopProgress(true);
        }
    });
    connect(_vehicle, &Vehicle::armedChanged, this, [this](bool armed) {
        if (_vehicle && armed && _vehicle->isInitialConnectComplete() && _currentMissionIndex <= _loopStart) {
            _loopPrevious = -1;
            _setRemainingLoops(_loopTotal);
        }
    });
    // Vehicle creates its link manager after its mission manager.
    QTimer::singleShot(0, this, [this]() {
        if (!_vehicle || !_vehicle->vehicleLinkManager()) {
            return;
        }
        connect(_vehicle->vehicleLinkManager(), &VehicleLinkManager::communicationLostChanged, this, [this](bool lost) {
            if (lost) {
                _setRemainingLoops(-2);
                _loopPrevious = -1;
            }
        });
    });
}

void MissionManager::_setRemainingLoops(int remaining)
{
    if (_remainingLoops != remaining) {
        _remainingLoops = remaining;
        emit remainingLoopsChanged();
    }
}

void MissionManager::_configureLoopProgress(bool uploaded)
{
    int start = -1;
    int end = -1;
    int total = -2;
    for (const auto* item : missionItems()) {
        if (item->command() != MAV_CMD_DO_JUMP) {
            continue;
        }
        const double target =
            item->param1() -
            (!uploaded && _vehicle && !_vehicle->firmwarePlugin()->sendHomePositionToVehicle() ? 1 : 0);
        const double repeats = item->param2();
        if (end >= 0 || !std::isfinite(target) || target != std::floor(target) || target < 0 ||
            target >= item->sequenceNumber() - 1 || !std::isfinite(repeats) || repeats != std::floor(repeats) ||
            repeats < -1 || repeats > 32767) {
            end = -1;
            break;
        }
        start = static_cast<int>(target);
        end = item->sequenceNumber();
        total = repeats == -1 ? -1 : static_cast<int>(repeats) + 1;
    }
    if (!uploaded && start == _loopStart && end == _loopEnd && total == _loopTotal) {
        return;
    }
    _loopStart = start;
    _loopEnd = end;
    _loopTotal = end >= 0 ? total : -2;
    _loopPrevious = -1;
    _setRemainingLoops(_loopTotal == -1 || uploaded || (_vehicle && !_vehicle->armed()) ? _loopTotal : -2);
}

void MissionManager::_trackLoopProgress(int index, int state)
{
    if (!_vehicle || _loopEnd < 0) {
        return;
    }
    if (state == MISSION_STATE_COMPLETE) {
        _setRemainingLoops(0);
    } else if (_vehicle->armed() && _vehicle->flightMode() == _vehicle->missionFlightMode()) {
        if (_loopTotal == -1) {
            _setRemainingLoops(index > _loopEnd ? 0 : -1);
        } else if (_remainingLoops >= 0) {
            if (index > _loopEnd) {
                _setRemainingLoops(0);
            } else if (index < _loopPrevious) {
                if (index == _loopStart && _loopPrevious >= _loopEnd - 1 && _loopPrevious <= _loopEnd) {
                    _setRemainingLoops(qMax(1, _remainingLoops - 1));
                } else {
                    _setRemainingLoops(-2);
                }
            }
        }
    } else if (_loopPrevious >= 0 && index != _loopPrevious && _vehicle->armed()) {
        _setRemainingLoops(-2);
    }
    _loopPrevious = index;
}

MissionManager::~MissionManager()
{

}

void MissionManager::writeArduPilotGuidedMissionItem(const QGeoCoordinate& gotoCoord, bool altChangeOnly)
{
    if (inProgress()) {
        qCDebug(MissionManagerLog) << "writeArduPilotGuidedMissionItem called while transaction in progress";
        return;
    }

    _transactionInProgress = TransactionWrite;

    _connectToMavlink();

    SharedLinkInterfacePtr sharedLink = _vehicle->vehicleLinkManager()->primaryLink().lock();
    if (sharedLink) {
        mavlink_message_t       messageOut;
        mavlink_mission_item_t  missionItem;

        memset(&missionItem, 0, sizeof(missionItem));
        missionItem.target_system =     _vehicle->id();
        missionItem.target_component =  _vehicle->defaultComponentId();
        missionItem.seq =               0;
        missionItem.command =           MAV_CMD_NAV_WAYPOINT;
        missionItem.param1 =            0;
        missionItem.param2 =            0;
        missionItem.param3 =            0;
        missionItem.param4 =            0;
        missionItem.x =                 gotoCoord.latitude();
        missionItem.y =                 gotoCoord.longitude();
        missionItem.z =                 gotoCoord.altitude();
        missionItem.frame =             MAV_FRAME_GLOBAL_RELATIVE_ALT;
        missionItem.current =           altChangeOnly ? 3 : 2;
        missionItem.autocontinue =      true;

        mavlink_msg_mission_item_encode_chan(MAVLinkProtocol::instance()->getSystemId(),
                                             MAVLinkProtocol::getComponentId(),
                                             sharedLink->mavlinkChannel(),
                                             &messageOut,
                                             &missionItem);

        _vehicle->sendMessageOnLinkThreadSafe(sharedLink.get(), messageOut);
    }
    _startAckTimeout(AckGuidedItem);
    emit inProgressChanged(true);
}

void MissionManager::generateResumeMission(int resumeIndex)
{
    if (_vehicle->isOfflineEditingVehicle()) {
        return;
    }

    if (inProgress()) {
        qCDebug(MissionManagerLog) << "generateResumeMission called while transaction in progress";
        return;
    }

    for (int i=0; i<_missionItems.count(); i++) {
        MissionItem* item = _missionItems[i];
        if (item->command() == MAV_CMD_DO_JUMP) {
            QGC::showAppMessage(tr("Unable to generate resume mission due to MAV_CMD_DO_JUMP command."));
            return;
        }
    }

    // Be anal about crap input
    resumeIndex = qMax(0, qMin(resumeIndex, _missionItems.count() - 1));

    // Adjust resume index to be a location based command
    const MissionCommandUIInfo* uiInfo = MissionCommandTree::instance()->getUIInfo(_vehicle, _vehicle->vehicleClass(), _missionItems[resumeIndex]->command());
    if (!uiInfo || uiInfo->isStandaloneCoordinate() || !uiInfo->specifiesCoordinate()) {
        // We have to back up to the last command which the vehicle flies through
        while (--resumeIndex > 0) {
            uiInfo = MissionCommandTree::instance()->getUIInfo(_vehicle, _vehicle->vehicleClass(), _missionItems[resumeIndex]->command());
            if (uiInfo && (uiInfo->specifiesCoordinate() && !uiInfo->isStandaloneCoordinate())) {
                // Found it
                break;
            }

        }
    }
    resumeIndex = qMax(0, resumeIndex);

    QList<MissionItem*> resumeMission;

    QList<MAV_CMD> includedResumeCommands;

    // If any command in this list occurs before the resumeIndex it will be added to the front of the mission
    includedResumeCommands << MAV_CMD_DO_CONTROL_VIDEO
                           << MAV_CMD_DO_SET_ROI
                           << MAV_CMD_DO_DIGICAM_CONFIGURE
                           << MAV_CMD_DO_DIGICAM_CONTROL
                           << MAV_CMD_DO_MOUNT_CONFIGURE
                           << MAV_CMD_DO_MOUNT_CONTROL
                           << MAV_CMD_DO_SET_CAM_TRIGG_DIST
                           << MAV_CMD_DO_FENCE_ENABLE
                           << MAV_CMD_IMAGE_START_CAPTURE
                           << MAV_CMD_IMAGE_STOP_CAPTURE
                           << MAV_CMD_VIDEO_START_CAPTURE
                           << MAV_CMD_VIDEO_STOP_CAPTURE
                           << MAV_CMD_DO_CHANGE_SPEED
                           << MAV_CMD_SET_CAMERA_MODE;

    bool addHomePosition = _vehicle->firmwarePlugin()->sendHomePositionToVehicle();

    int prefixCommandCount = 0;
    for (int i=0; i<_missionItems.count(); i++) {
        MissionItem* oldItem = _missionItems[i];
        const MissionCommandUIInfo* loopUiInfo = MissionCommandTree::instance()->getUIInfo(_vehicle, _vehicle->vehicleClass(), oldItem->command());
        if ((i == 0 && addHomePosition) || i >= resumeIndex || includedResumeCommands.contains(oldItem->command()) || (loopUiInfo && loopUiInfo->isTakeoffCommand())) {
            if (i < resumeIndex) {
                prefixCommandCount++;
            }
            MissionItem* newItem = new MissionItem(*oldItem, this);
            newItem->setIsCurrentItem(false);
            resumeMission.append(newItem);
        }
    }
    prefixCommandCount = qMax(0, qMin(prefixCommandCount, resumeMission.count()));  // Anal prevention against crashes

    // De-dup and remove no-ops from the commands which were added to the front of the mission
    bool foundROI = false;
    bool foundCameraSetMode = false;
    bool foundCameraStartStop = false;
    prefixCommandCount--;   // Change from count to array index
    while (prefixCommandCount >= 0) {
        MissionItem* resumeItem = resumeMission[prefixCommandCount];
        switch (resumeItem->command()) {
        case MAV_CMD_SET_CAMERA_MODE:
            // Only keep the last one
            if (foundCameraSetMode) {
                resumeMission.removeAt(prefixCommandCount);
            }
            foundCameraSetMode = true;
            break;
        case MAV_CMD_DO_SET_ROI:
            // Only keep the last one
            if (foundROI) {
                resumeMission.removeAt(prefixCommandCount);
            }
            foundROI = true;
            break;
        case MAV_CMD_DO_SET_CAM_TRIGG_DIST:
        case MAV_CMD_IMAGE_STOP_CAPTURE:
        case MAV_CMD_VIDEO_START_CAPTURE:
        case MAV_CMD_VIDEO_STOP_CAPTURE:
            // Only keep the first of these commands that are found
            if (foundCameraStartStop) {
                resumeMission.removeAt(prefixCommandCount);
            }
            foundCameraStartStop = true;
            break;
        case MAV_CMD_IMAGE_START_CAPTURE:
            if (resumeItem->param3() != 0) {
                // Remove commands which do not trigger by time
                resumeMission.removeAt(prefixCommandCount);
                break;
            }
            if (foundCameraStartStop) {
                // Only keep the first of these commands that are found
                resumeMission.removeAt(prefixCommandCount);
            }
            foundCameraStartStop = true;
            break;
        default:
            break;
        }

        prefixCommandCount--;
    }

    // Adjust sequence numbers and current item
    int seqNum = 0;
    for (int i=0; i<resumeMission.count(); i++) {
        resumeMission[i]->setSequenceNumber(seqNum++);
    }
    int setCurrentIndex = addHomePosition ? 1 : 0;
    resumeMission[setCurrentIndex]->setIsCurrentItem(true);

    // Send to vehicle
    _clearAndDeleteWriteMissionItems();
    for (int i=0; i<resumeMission.count(); i++) {
        _writeMissionItems.append(new MissionItem(*resumeMission[i], this));
    }
    _resumeMission = true;
    _writeMissionItemsWorker();
}

/// Called when a new mavlink message for out vehicle is received
void MissionManager::_mavlinkMessageReceived(const mavlink_message_t& message)
{
    switch (message.msgid) {
    case MAVLINK_MSG_ID_HIGH_LATENCY:
        _handleHighLatency(message);
        break;

    case MAVLINK_MSG_ID_HIGH_LATENCY2:
        _handleHighLatency2(message);
        break;

    case MAVLINK_MSG_ID_MISSION_CURRENT:
        _handleMissionCurrent(message);
        break;

    case MAVLINK_MSG_ID_HEARTBEAT:
        _handleHeartbeat(message);
        break;
    }
}

void MissionManager::_updateMissionIndex(int index)
{
    if (index != _currentMissionIndex) {
        qCDebug(MissionManagerLog) << "_updateMissionIndex currentIndex:" << index;
        _currentMissionIndex = index;
        emit currentIndexChanged(_currentMissionIndex);
    }

    if (_currentMissionIndex != _lastCurrentIndex && _cachedLastCurrentIndex != _currentMissionIndex) {
        // We have to be careful of an RTL sequence causing a change of index to the DO_LAND_START sequence. This also triggers
        // a flight mode change away from mission flight mode. So we only update _lastCurrentIndex when the flight mode is mission.
        // But we can run into problems where we may get the MISSION_CURRENT message for the RTL/DO_LAND_START sequenc change prior
        // to the HEARTBEAT message which contains the flight mode change which will cause things to work incorrectly. To fix this
        // We force the sequencing of HEARTBEAT following by MISSION_CURRENT by caching the possible _lastCurrentIndex update until
        // the next HEARTBEAT comes through.
        qCDebug(MissionManagerLog) << "_updateMissionIndex caching _lastCurrentIndex for possible update:" << _currentMissionIndex;
        _cachedLastCurrentIndex = _currentMissionIndex;
    }
}

void MissionManager::_handleHighLatency(const mavlink_message_t& message)
{
    mavlink_high_latency_t highLatency;
    mavlink_msg_high_latency_decode(&message, &highLatency);
    _updateMissionIndex(highLatency.wp_num);
}

void MissionManager::_handleHighLatency2(const mavlink_message_t& message)
{
    mavlink_high_latency2_t highLatency2;
    mavlink_msg_high_latency2_decode(&message, &highLatency2);
    _updateMissionIndex(highLatency2.wp_num);
}

void MissionManager::_handleMissionCurrent(const mavlink_message_t& message)
{
    mavlink_mission_current_t missionCurrent;
    mavlink_msg_mission_current_decode(&message, &missionCurrent);
    if (_vehicle && message.compid == _vehicle->defaultComponentId()) {
        _trackLoopProgress(missionCurrent.seq, missionCurrent.mission_state);
    }
    _updateMissionIndex(missionCurrent.seq);
}

void MissionManager::_handleHeartbeat(const mavlink_message_t& message)
{
    Q_UNUSED(message);

    if (_cachedLastCurrentIndex != -1 &&  _vehicle->flightMode() == _vehicle->missionFlightMode()) {
        qCDebug(MissionManagerLog) << "_handleHeartbeat updating lastCurrentIndex from cached value:" << _cachedLastCurrentIndex;
        _lastCurrentIndex = _cachedLastCurrentIndex;
        _cachedLastCurrentIndex = -1;
        emit lastCurrentIndexChanged(_lastCurrentIndex);
    }
}
