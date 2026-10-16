#include "CaacSimulationEngine.h"

#include <algorithm>
#include <cmath>

#include <QtCore/QCoreApplication>

#include "QGCGeo.h"

namespace {
bool coordinateCommand(int command)
{
    return command == MAV_CMD_NAV_WAYPOINT || command == MAV_CMD_NAV_TAKEOFF || command == MAV_CMD_NAV_LAND ||
           command == MAV_CMD_NAV_LOITER_UNLIM;
}

double normalized(double value)
{
    value = std::fmod(value, 360.0);
    return value < 0 ? value + 360 : value;
}
}  // namespace

void CaacSimulationEngine::initialize(const QGeoCoordinate& home, const Options& options)
{
    _items.clear();
    _home = home;
    _options = options;
    _missionOptions = options;
    _state = {};
    _state.position = home;
    _jumpCounts.clear();
    _waypointOptions.clear();
    _wait = -1;
    _returnLanding = false;
    _takeoffAtCurrentPosition = false;
}

bool CaacSimulationEngine::setHorizontalSpeed(double speed)
{
    if (!std::isfinite(speed) || speed <= 0 || speed > 100) {
        return false;
    }
    _options.horizontalSpeed = speed;
    return true;
}

bool CaacSimulationEngine::load(const QMap<uint16_t, mavlink_mission_item_int_t>& mission, const QGeoCoordinate& home,
                                const Options& options, QString& error,
                                const QMap<int, WaypointOptions>& waypointOptions)
{
    if (!home.isValid() || !std::isfinite(home.altitude()) || mission.isEmpty() || mission.size() > 5000 ||
        !std::isfinite(options.horizontalSpeed) || options.horizontalSpeed <= 0 || options.horizontalSpeed > 100 ||
        !std::isfinite(options.verticalSpeed) || options.verticalSpeed <= 0 || options.verticalSpeed > 30 ||
        !std::isfinite(options.yawRate) || options.yawRate <= 0 || options.yawRate > 360 ||
        !std::isfinite(options.windSpeed) || options.windSpeed < 0 || options.windSpeed > 50 ||
        !std::isfinite(options.windBearing) || options.turnMode < 0 || options.turnMode > 3 || options.climbMode < 0 ||
        options.climbMode > 1) {
        error = QCoreApplication::translate("CaacSimulationEngine", "Invalid simulation settings or empty mission.");
        return false;
    }
    int sequence = 0;
    for (const auto& item : mission) {
        if (item.seq != sequence++) {
            error = QCoreApplication::translate("CaacSimulationEngine", "Mission sequence is not contiguous.");
            return false;
        }
        const auto command = item.command;
        if (!coordinateCommand(command) && command != MAV_CMD_NAV_RETURN_TO_LAUNCH && command != MAV_CMD_DO_JUMP &&
            command != MAV_CMD_DO_CHANGE_SPEED && command != MAV_CMD_CONDITION_DELAY &&
            command != MAV_CMD_CONDITION_YAW && command != MAV_CMD_NAV_DELAY) {
            error =
                QCoreApplication::translate("CaacSimulationEngine", "Unsupported simulation command: %1").arg(command);
            return false;
        }
        if (coordinateCommand(command)) {
            const QGeoCoordinate coordinate(item.x / 1e7, item.y / 1e7, item.z);
            if (!coordinate.isValid() || !std::isfinite(item.z) || std::abs(item.z) > 100000 ||
                (item.frame != MAV_FRAME_GLOBAL && item.frame != MAV_FRAME_GLOBAL_INT &&
                 item.frame != MAV_FRAME_GLOBAL_RELATIVE_ALT && item.frame != MAV_FRAME_GLOBAL_RELATIVE_ALT_INT) ||
                !std::isfinite(item.param1) || item.param1 < 0 || item.param1 > 3600 ||
                (!std::isnan(item.param4) && !std::isfinite(item.param4))) {
                error = QCoreApplication::translate("CaacSimulationEngine",
                                                    "Invalid coordinate, altitude frame or hold time at item %1.")
                            .arg(item.seq);
                return false;
            }
        } else if (command == MAV_CMD_DO_JUMP) {
            if (!std::isfinite(item.param1) || item.param1 != std::floor(item.param1) || item.param1 < 0 ||
                item.param1 > 65535 || !mission.contains(static_cast<uint16_t>(item.param1)) ||
                !std::isfinite(item.param2) || item.param2 != std::floor(item.param2) || item.param2 < -1 ||
                item.param2 > 32767) {
                error = QCoreApplication::translate("CaacSimulationEngine", "Invalid loop target or count.");
                return false;
            }
        } else if (command == MAV_CMD_DO_CHANGE_SPEED) {
            if (!std::isfinite(item.param1) || item.param1 < 0 || item.param1 > 3 ||
                item.param1 != std::floor(item.param1) || !std::isfinite(item.param2) || item.param2 <= 0 ||
                item.param2 > 100) {
                error = QCoreApplication::translate("CaacSimulationEngine", "Invalid speed command.");
                return false;
            }
        } else if (command == MAV_CMD_CONDITION_DELAY || command == MAV_CMD_NAV_DELAY) {
            if (!std::isfinite(item.param1) || item.param1 < 0 || item.param1 > 3600) {
                error = QCoreApplication::translate("CaacSimulationEngine", "Invalid delay command.");
                return false;
            }
        } else if (command == MAV_CMD_CONDITION_YAW && !std::isfinite(item.param1)) {
            error = QCoreApplication::translate("CaacSimulationEngine", "Invalid heading command.");
            return false;
        }
    }
    for (auto it = waypointOptions.cbegin(); it != waypointOptions.cend(); ++it) {
        if (!mission.contains(it.key()) || mission.value(it.key()).command != MAV_CMD_NAV_WAYPOINT ||
            !std::isfinite(it->verticalSpeed) || it->verticalSpeed <= 0 || it->verticalSpeed > 30 || it->turnMode < 0 ||
            it->turnMode > 3 || it->climbMode < 0 || it->climbMode > 1) {
            error =
                QCoreApplication::translate("CaacSimulationEngine", "Invalid simulation settings or empty mission.");
            return false;
        }
    }
    _waypointOptions = waypointOptions;
    _items = mission;
    _home = home;
    _options = options;
    _missionOptions = options;
    _state = {};
    _state.position = home;
    _jumpCounts.clear();
    _wait = -1;
    _returnLanding = false;
    _takeoffAtCurrentPosition = false;
    error.clear();
    return true;
}

bool CaacSimulationEngine::updateMission(const QMap<uint16_t, mavlink_mission_item_int_t>& mission,
                                         const QGeoCoordinate& home, const Options& options, QString& error,
                                         const QMap<int, WaypointOptions>& waypointOptions)
{
    const auto equal = [](float left, float right) { return left == right || (std::isnan(left) && std::isnan(right)); };
    bool same = mission.size() == _items.size() && home == _home && options == _missionOptions &&
                waypointOptions == _waypointOptions;
    for (auto it = mission.cbegin(); same && it != mission.cend(); ++it) {
        const auto old = _items.constFind(it.key());
        same = old != _items.cend() && old->command == it->command && old->frame == it->frame &&
               old->autocontinue == it->autocontinue && equal(old->param1, it->param1) &&
               equal(old->param2, it->param2) && equal(old->param3, it->param3) && equal(old->param4, it->param4) &&
               equal(old->z, it->z);
    }
    if (!same) {
        error = QCoreApplication::translate(
            "CaacSimulationEngine",
            "Only waypoint positions can be updated during flight. Pause and upload other mission changes manually.");
        return false;
    }
    auto updated = *this;
    if (!updated.load(mission, home, options, error, waypointOptions)) {
        return false;
    }
    updated._options = _options;
    updated._state = _state;
    updated._jumpCounts = _jumpCounts;
    updated._wait = _wait;
    updated._returnLanding = _returnLanding;
    updated._takeoffAtCurrentPosition = _takeoffAtCurrentPosition;
    updated.pause();
    *this = updated;
    return true;
}

bool CaacSimulationEngine::jump(int sequence)
{
    if (!_items.contains(sequence)) {
        return false;
    }
    _state.sequence = sequence;
    _state.finished = false;
    _wait = -1;
    _returnLanding = false;
    return true;
}

bool CaacSimulationEngine::restartMission()
{
    if (_items.isEmpty()) {
        return false;
    }
    _state.sequence = _items.firstKey();
    _state.finished = false;
    _options = _missionOptions;
    _jumpCounts.clear();
    _wait = -1;
    _returnLanding = false;
    _takeoffAtCurrentPosition = true;
    pause();
    return true;
}

int CaacSimulationEngine::remainingLoops() const
{
    int remaining = -2;
    for (const auto& item : _items) {
        if (item.command != MAV_CMD_DO_JUMP || item.param1 >= item.seq) {
            continue;
        }
        if (remaining != -2) {
            return -2;
        }
        if (_state.finished || _state.sequence > item.seq) {
            remaining = 0;
        } else if (item.param2 == -1) {
            remaining = -1;
        } else {
            remaining = std::max(1, static_cast<int>(item.param2) + 1 - _jumpCounts.value(item.seq));
        }
    }
    return remaining;
}

void CaacSimulationEngine::pause()
{
    _state.roll = 0;
    _state.pitch = 0;
    _state.northSpeed = 0;
    _state.eastSpeed = 0;
    _state.climbSpeed = 0;
}

void CaacSimulationEngine::setAltitude(double amslAltitude)
{
    if (std::isfinite(amslAltitude)) {
        _state.position.setAltitude(amslAltitude);
    }
}

void CaacSimulationEngine::advanceHoldAltitude(double amslAltitude, double seconds)
{
    pause();
    if (!_home.isValid() || !std::isfinite(amslAltitude) || !std::isfinite(seconds) || seconds <= 0 || seconds > 1) {
        return;
    }
    const double step = std::clamp(amslAltitude - _state.position.altitude(), -_options.verticalSpeed * seconds,
                                   _options.verticalSpeed * seconds);
    _state.position.setAltitude(_state.position.altitude() + step);
    _state.climbSpeed = step / seconds;
    _state.airborne = _state.position.altitude() > _home.altitude() + 0.01;
}

void CaacSimulationEngine::advanceGuidedTarget(const QGeoCoordinate& target, double seconds)
{
    if (!_home.isValid() || !target.isValid() || !std::isfinite(target.altitude()) || !std::isfinite(seconds) ||
        seconds <= 0 || seconds > 1) {
        return;
    }
    pause();
    _move(target, seconds, false);
}

void CaacSimulationEngine::advanceManual(double forward, double right, double yaw, double throttle, double seconds,
                                         bool attitudeMode)
{
    const double previousNorthSpeed = _state.northSpeed;
    const double previousEastSpeed = _state.eastSpeed;
    pause();
    if (!_home.isValid() || !std::isfinite(seconds) || seconds <= 0 || seconds > 1 || !std::isfinite(forward) ||
        !std::isfinite(right) || !std::isfinite(yaw) || !std::isfinite(throttle) || std::abs(forward) > 1 ||
        std::abs(right) > 1 || std::abs(yaw) > 1 || throttle < 0 || throttle > 1) {
        return;
    }
    const auto deadband = [](double value) {
        constexpr double threshold = 0.05;
        return std::abs(value) <= threshold ? 0.0
                                            : std::copysign((std::abs(value) - threshold) / (1 - threshold), value);
    };
    forward = deadband(forward);
    right = deadband(right);
    yaw = deadband(yaw);
    const double previousAltitude = _state.position.altitude();
    const double altitude =
        std::max(_home.altitude(), previousAltitude + deadband(throttle * 2 - 1) * _options.verticalSpeed * seconds);
    _state.position.setAltitude(altitude);
    _state.climbSpeed = (altitude - previousAltitude) / seconds;
    _state.airborne = altitude > _home.altitude() + 0.01;
    if (!_state.airborne) {
        return;
    }
    const double heading = (_state.heading + yaw * _options.yawRate * seconds / 2) * M_PI / 180.0;
    _state.heading = normalized(_state.heading + yaw * _options.yawRate * seconds);
    const double speedScale = _options.horizontalSpeed / std::max(1.0, std::hypot(forward, right));
    _state.northSpeed = (forward * std::cos(heading) - right * std::sin(heading)) * speedScale;
    _state.eastSpeed = (forward * std::sin(heading) + right * std::cos(heading)) * speedScale;
    if (attitudeMode) {
        const double forwardAcceleration = 9.80665 * std::tan(forward * 30 * M_PI / 180.0);
        const double rightAcceleration = 9.80665 * std::tan(right * 35 * M_PI / 180.0);
        const double windHeading = _options.windBearing * M_PI / 180.0;
        _state.northSpeed = previousNorthSpeed +
                            seconds * (forwardAcceleration * std::cos(heading) - rightAcceleration * std::sin(heading) +
                                       0.15 * (_options.windSpeed * std::cos(windHeading) - previousNorthSpeed));
        _state.eastSpeed = previousEastSpeed +
                           seconds * (forwardAcceleration * std::sin(heading) + rightAcceleration * std::cos(heading) +
                                      0.15 * (_options.windSpeed * std::sin(windHeading) - previousEastSpeed));
        const double scale = _options.horizontalSpeed /
                             std::max(_options.horizontalSpeed, std::hypot(_state.northSpeed, _state.eastSpeed));
        _state.northSpeed *= scale;
        _state.eastSpeed *= scale;
    }
    const double distance = std::hypot(_state.northSpeed, _state.eastSpeed) * seconds;
    if (distance > 0) {
        auto position = QGCGeo::geodesicDestination(
            _state.position, normalized(std::atan2(_state.eastSpeed, _state.northSpeed) * 180.0 / M_PI), distance);
        position.setAltitude(altitude);
        _state.position = position;
    }
    _state.roll = right * 35;
    _state.pitch = -forward * 30;
}

bool CaacSimulationEngine::advanceLanding(double seconds)
{
    pause();
    if (!_home.isValid() || !std::isfinite(seconds) || seconds <= 0 || seconds > 1) {
        return false;
    }
    auto target = _state.position;
    target.setAltitude(_home.altitude());
    return _move(target, seconds, true);
}

bool CaacSimulationEngine::advanceReturn(double seconds)
{
    pause();
    if (!_home.isValid() || !std::isfinite(seconds) || seconds <= 0 || seconds > 1) {
        return false;
    }
    auto target = _home;
    const bool landing = QGCGeo::geodesicDistance(_state.position, _home) < 0.05;
    if (!landing) {
        target.setAltitude(std::max(_state.position.altitude(), _home.altitude() + 10));
    }
    const bool arrived = _move(target, seconds, landing);
    return landing && arrived;
}

void CaacSimulationEngine::restoreState(const State& state)
{
    if (state.position.isValid() && std::isfinite(state.position.altitude())) {
        _state = state;
        if (!_items.contains(_state.sequence)) {
            _state.sequence = _items.firstKey();
        }
        _state.finished = false;
        pause();
    }
}

QGeoCoordinate CaacSimulationEngine::_target(const mavlink_mission_item_int_t& item) const
{
    auto altitude = static_cast<double>(item.z);
    if (item.frame == MAV_FRAME_GLOBAL_RELATIVE_ALT || item.frame == MAV_FRAME_GLOBAL_RELATIVE_ALT_INT) {
        altitude += _home.altitude();
    }
    return QGeoCoordinate(item.x / 1e7, item.y / 1e7, altitude);
}

void CaacSimulationEngine::_next()
{
    ++_state.sequence;
    _wait = -1;
    _returnLanding = false;
    if (!_items.contains(_state.sequence)) {
        _state.finished = true;
        pause();
    }
}

double CaacSimulationEngine::_rotate(double target, double seconds, double rate)
{
    const double difference = std::remainder(target - _state.heading, 360.0);
    _state.heading = normalized(_state.heading + std::clamp(difference, -rate * seconds, rate * seconds));
    return std::abs(difference);
}

bool CaacSimulationEngine::_move(const QGeoCoordinate& target, double seconds, bool landing)
{
    const auto legOptions = _waypointOptions.value(
        _state.sequence, WaypointOptions{_options.verticalSpeed, _options.turnMode, _options.climbMode});
    const auto before = _state.position;
    const double previousHeading = _state.heading;
    const double distance = QGCGeo::geodesicDistance(before, target);
    const double height = target.altitude() - before.altitude();
    const double bearing = distance > 0.01 ? QGCGeo::geodesicAzimuth(before, target) : _state.heading;
    double fraction = distance > 0.01 ? std::min(1.0, _options.horizontalSpeed * seconds / distance) : 1.0;
    if (legOptions.climbMode == 0 && std::abs(height) > 0.01 && !landing) {
        fraction = 0;
    }
    if (legOptions.turnMode == 0 && distance > 0.01 && _rotate(bearing, seconds, _options.yawRate) > 1) {
        fraction = 0;
    } else if (legOptions.turnMode == 1 || legOptions.turnMode == 2) {
        const double rate =
            legOptions.turnMode == 2
                ? std::clamp(_options.yawRate * _options.horizontalSpeed / std::max(distance, 1.0), 10.0, 180.0)
                : _options.yawRate;
        _rotate(bearing, seconds, rate);
    }
    double altitudeStep = std::clamp(height, -legOptions.verticalSpeed * seconds, legOptions.verticalSpeed * seconds);
    if (legOptions.climbMode == 1 && distance > 0.01 && std::abs(height) > 0.01) {
        fraction = std::min(fraction, std::abs(altitudeStep / height));
        altitudeStep = height * fraction;
    }
    if (fraction > 0 && distance > 0.01) {
        _state.position = QGCGeo::geodesicDestination(before, bearing, distance * fraction);
    }
    _state.position.setAltitude(before.altitude() + altitudeStep);
    // Wind is an uncompensated horizontal displacement in this training model.
    if (_options.windSpeed > 0 && _state.airborne) {
        const double altitude = _state.position.altitude();
        _state.position =
            QGCGeo::geodesicDestination(_state.position, _options.windBearing, _options.windSpeed * seconds);
        _state.position.setAltitude(altitude);
    }
    const double moved = QGCGeo::geodesicDistance(before, _state.position);
    const double movedBearing = moved > 0.001 ? QGCGeo::geodesicAzimuth(before, _state.position) : 0;
    _state.northSpeed = moved / seconds * std::cos(movedBearing * M_PI / 180.0);
    _state.eastSpeed = moved / seconds * std::sin(movedBearing * M_PI / 180.0);
    _state.climbSpeed = (_state.position.altitude() - before.altitude()) / seconds;
    const double horizontalSpeed = moved / seconds;
    const double yawRate = std::remainder(_state.heading - previousHeading, 360.0) * M_PI / (180.0 * seconds);
    // A multirotor tilts to oppose horizontal drag even during level flight.
    _state.roll = std::clamp(std::atan2(horizontalSpeed * yawRate, 9.80665) * 180.0 / M_PI, -35.0, 35.0);
    const double forwardSpeed = distance * fraction / seconds * std::cos((bearing - _state.heading) * M_PI / 180.0);
    _state.pitch =
        std::clamp(-std::atan2(0.06 * forwardSpeed * std::abs(forwardSpeed), 9.80665) * 180.0 / M_PI, -30.0, 30.0);
    _state.airborne = _state.position.altitude() > _home.altitude() + 0.01;
    const bool arrived =
        QGCGeo::geodesicDistance(_state.position, target) < std::max(0.05, _options.windSpeed * seconds + 0.01) &&
        std::abs(_state.position.altitude() - target.altitude()) < 0.05;
    if (arrived) {
        _state.position = target;
    }
    return arrived;
}

void CaacSimulationEngine::advance(double seconds)
{
    if (!std::isfinite(seconds) || seconds <= 0 || seconds > 1 || _state.finished || _items.isEmpty()) {
        return;
    }
    pause();
    // Limit zero-distance command chains, including forever-loops, to keep the UI responsive.
    for (int transitions = 0; transitions < 64 && !_state.finished; ++transitions) {
        const auto item = _items.value(_state.sequence);
        switch (item.command) {
            case MAV_CMD_DO_CHANGE_SPEED:
                if (item.param1 < 2) {
                    _options.horizontalSpeed = item.param2;
                } else {
                    _options.verticalSpeed = item.param2;
                }
                _next();
                continue;
            case MAV_CMD_DO_JUMP: {
                auto& count = _jumpCounts[_state.sequence];
                if (item.param2 == -1 || count < item.param2) {
                    if (item.param2 != -1) {
                        ++count;
                    }
                    _state.sequence = static_cast<int>(item.param1);
                    _wait = -1;
                } else {
                    _next();
                }
                continue;
            }
            case MAV_CMD_CONDITION_DELAY:
            case MAV_CMD_NAV_DELAY:
                if (_wait < 0) {
                    _wait = item.param1;
                }
                _wait -= seconds;
                if (_wait <= 0) {
                    _next();
                }
                return;
            case MAV_CMD_CONDITION_YAW:
                if (_rotate(normalized(item.param1), seconds, _options.yawRate) <= _options.yawRate * seconds) {
                    _next();
                }
                return;
            case MAV_CMD_NAV_RETURN_TO_LAUNCH: {
                auto target = _home;
                if (!_returnLanding) {
                    target.setAltitude(std::max(_state.position.altitude(), _home.altitude() + 10));
                    if (_move(target, seconds, false)) {
                        _returnLanding = true;
                    }
                } else if (_move(target, seconds, true)) {
                    _next();
                }
                return;
            }
            default:
                if (coordinateCommand(item.command)) {
                    auto target = _target(item);
                    if (item.command == MAV_CMD_NAV_TAKEOFF && _takeoffAtCurrentPosition) {
                        target.setLatitude(_state.position.latitude());
                        target.setLongitude(_state.position.longitude());
                    }
                    if (item.command == MAV_CMD_NAV_LAND) {
                        target.setAltitude(_home.altitude());
                    }
                    if (_wait < 0 && !_move(target, seconds, item.command == MAV_CMD_NAV_LAND)) {
                        return;
                    }
                    if (std::isfinite(item.param4)) {
                        pause();
                        if (_rotate(normalized(item.param4), seconds, _options.yawRate) > _options.yawRate * seconds) {
                            return;
                        }
                    }
                    if (item.command == MAV_CMD_NAV_LOITER_UNLIM) {
                        pause();
                        _state.finished = true;
                        return;
                    }
                    if (_wait < 0) {
                        _wait = item.param1;
                    }
                    _wait -= seconds;
                    if (_wait <= 0) {
                        if (item.command == MAV_CMD_NAV_TAKEOFF) {
                            _takeoffAtCurrentPosition = false;
                        }
                        _next();
                    }
                    return;
                }
                _state.finished = true;
                return;
        }
    }
}
