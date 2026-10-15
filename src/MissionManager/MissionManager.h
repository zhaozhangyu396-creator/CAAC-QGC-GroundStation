#pragma once

#include <QtQmlIntegration/QtQmlIntegration>

#include "PlanManager.h"

class Vehicle;

class MissionManager : public PlanManager
{
    Q_OBJECT
    QML_ELEMENT
    QML_UNCREATABLE("")
public:
    MissionManager(Vehicle* vehicle);
    ~MissionManager();

    /// Current mission item as reported by MISSION_CURRENT
    int currentIndex(void) const { return _currentMissionIndex; }

    /// Last current mission item reported while in Mission flight mode
    int lastCurrentIndex(void) const { return _lastCurrentIndex; }

    int remainingLoops() const { return _remainingLoops; }

    /// Writes the specified set mission items to the vehicle as an ArduPilot guided mode mission item.
    ///     @param gotoCoord Coordinate to move to
    ///     @param altChangeOnly true: only altitude change, false: lat/lon/alt change
    void writeArduPilotGuidedMissionItem(const QGeoCoordinate& gotoCoord, bool altChangeOnly);

    /// Generates a new mission which starts from the specified index. It will include all the CMD_DO items
    /// from mission start to resumeIndex in the generate mission.
    void generateResumeMission(int resumeIndex);

signals:
    void remainingLoopsChanged();

private slots:
    void _mavlinkMessageReceived(const mavlink_message_t& message);

private:
    void _handleHighLatency(const mavlink_message_t& message);
    void _handleHighLatency2(const mavlink_message_t& message);
    void _handleMissionCurrent(const mavlink_message_t& message);
    void _updateMissionIndex(int index);
    void _handleHeartbeat(const mavlink_message_t& message);
    void _configureLoopProgress(bool uploaded);
    void _trackLoopProgress(int index, int state);
    void _setRemainingLoops(int remaining);

    int _cachedLastCurrentIndex;
    int _loopStart = -1;
    int _loopEnd = -1;
    int _loopTotal = -2;
    int _loopPrevious = -1;
    int _remainingLoops = -2;  ///< -2: unknown/not a loop, -1: infinite; includes the current lap.
};
