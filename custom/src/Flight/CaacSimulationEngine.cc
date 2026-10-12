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
    _state = {};
    _state.position = home;
    _jumpCounts.clear();
    _wait = -1;
    _returnLanding = false;
}

bool CaacSimulationEngine::load(const QMap<uint16_t, mavlink_mission_item_int_t>& mission, const QGeoCoordinate& home,
                                const Options& options, QString& error)
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
    _items = mission;
    _home = home;
    _options = options;
    _state = {};
    _state.position = home;
    _jumpCounts.clear();
    _wait = -1;
    _returnLanding = false;
    error.clear();
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

void CaacSimulationEngine::pause()
{
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
    const auto before = _state.position;
    const double distance = QGCGeo::geodesicDistance(before, target);
    const double height = target.altitude() - before.altitude();
    const double bearing = distance > 0.01 ? QGCGeo::geodesicAzimuth(before, target) : _state.heading;
    double fraction = distance > 0.01 ? std::min(1.0, _options.horizontalSpeed * seconds / distance) : 1.0;
    if (_options.climbMode == 0 && std::abs(height) > 0.01 && !landing) {
        fraction = 0;
    }
    if (_options.turnMode == 0 && distance > 0.01 && _rotate(bearing, seconds, _options.yawRate) > 1) {
        fraction = 0;
    } else if (_options.turnMode == 1 || _options.turnMode == 2) {
        const double rate =
            _options.turnMode == 2
                ? std::clamp(_options.yawRate * _options.horizontalSpeed / std::max(distance, 1.0), 10.0, 180.0)
                : _options.yawRate;
        _rotate(bearing, seconds, rate);
    }
    double altitudeStep = std::clamp(height, -_options.verticalSpeed * seconds, _options.verticalSpeed * seconds);
    if (_options.climbMode == 1 && distance > 0.01 && std::abs(height) > 0.01) {
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
                        _next();
                    }
                    return;
                }
                _state.finished = true;
                return;
        }
    }
}
