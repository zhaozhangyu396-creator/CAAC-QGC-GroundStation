#pragma once

#include <cmath>

#include <QtCore/QList>
#include <QtCore/QMap>

#include "QGCMAVLink.h"

namespace CaacMissionUpdateUtils {

using Mission = QMap<uint16_t, mavlink_mission_item_int_t>;

struct SpeedUpdateMapping
{
    bool valid = false;
    QMap<int, int> oldToNew;
    QMap<int, int> newToOld;
    QMap<int, int> jumpTargetToNew;
};

inline bool isHorizontalAbsoluteSpeed(const mavlink_mission_item_int_t& item)
{
    return item.command == MAV_CMD_DO_CHANGE_SPEED && item.frame == MAV_FRAME_MISSION && item.autocontinue &&
           (item.param1 == 0 || item.param1 == 1) && std::isfinite(item.param2) && item.param2 > 0 &&
           item.param3 == -1 && item.param4 == 0 && item.x == 0 && item.y == 0 && item.z == 0;
}

inline SpeedUpdateMapping speedUpdateMapping(const Mission& previous, const Mission& updated,
                                             bool validateJumpTargets = true)
{
    SpeedUpdateMapping result;
    const auto anchors = [](const Mission& mission, QList<int>& indices) {
        if (mission.isEmpty() || mission.size() > 5000) {
            return false;
        }
        int sequence = 0;
        for (auto it = mission.cbegin(); it != mission.cend(); ++it, ++sequence) {
            if (it.key() != sequence || it->seq != sequence) {
                return false;
            }
            if (!isHorizontalAbsoluteSpeed(it.value())) {
                indices.append(sequence);
            }
        }
        return true;
    };
    QList<int> oldAnchors;
    QList<int> newAnchors;
    if (!anchors(previous, oldAnchors) || !anchors(updated, newAnchors) || oldAnchors.isEmpty() ||
        oldAnchors.size() != newAnchors.size()) {
        return result;
    }
    for (int i = 0; i < oldAnchors.size(); ++i) {
        result.oldToNew.insert(oldAnchors[i], newAnchors[i]);
        result.newToOld.insert(newAnchors[i], oldAnchors[i]);
        result.jumpTargetToNew.insert(oldAnchors[i], newAnchors[i]);
    }
    for (int gap = 0; gap <= oldAnchors.size(); ++gap) {
        const int oldBegin = gap == 0 ? 0 : oldAnchors[gap - 1] + 1;
        const int newBegin = gap == 0 ? 0 : newAnchors[gap - 1] + 1;
        const int oldEnd = gap < oldAnchors.size() ? oldAnchors[gap] : previous.size();
        const int newEnd = gap < newAnchors.size() ? newAnchors[gap] : updated.size();
        for (int index = oldBegin; index < oldEnd; ++index) {
            if (gap < newAnchors.size()) {
                result.oldToNew.insert(index, newEnd);
            }
            // A loop entering a speed section must execute that section again.
            if (newBegin < newEnd) {
                result.jumpTargetToNew.insert(index, newBegin);
            } else if (gap < newAnchors.size()) {
                result.jumpTargetToNew.insert(index, newEnd);
            }
        }
        if (gap < oldAnchors.size()) {
            for (int index = newBegin; index < newEnd; ++index) {
                result.newToOld.insert(index, oldEnd);
            }
        }
    }
    const auto equal = [](float left, float right) {
        return std::abs(left - right) < 1e-5f || (std::isnan(left) && std::isnan(right));
    };
    for (int i = 0; i < oldAnchors.size(); ++i) {
        const auto& old = previous[oldAnchors[i]];
        const auto& item = updated[newAnchors[i]];
        if (item.command != old.command || item.frame != old.frame || item.autocontinue != old.autocontinue ||
            !equal(item.param2, old.param2) || !equal(item.param3, old.param3) || !equal(item.param4, old.param4) ||
            !equal(item.z, old.z)) {
            return result;
        }
        if (item.command == MAV_CMD_DO_JUMP) {
            if (!std::isfinite(old.param1) || old.param1 != std::floor(old.param1) || old.param1 < 0 ||
                old.param1 >= previous.size() || !result.jumpTargetToNew.contains(static_cast<int>(old.param1)) ||
                (validateJumpTargets &&
                 !equal(item.param1, result.jumpTargetToNew.value(static_cast<int>(old.param1))))) {
                return result;
            }
        } else if (!equal(item.param1, old.param1)) {
            return result;
        }
        if (item.command != MAV_CMD_NAV_WAYPOINT && item.command != MAV_CMD_NAV_LAND &&
            (item.x != old.x || item.y != old.y)) {
            return result;
        }
    }
    result.valid = true;
    return result;
}

}  // namespace CaacMissionUpdateUtils
