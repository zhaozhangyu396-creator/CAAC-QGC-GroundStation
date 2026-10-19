#include "CompassHeadingController.h"

#include <cmath>

#include "Vehicle.h"

namespace {
constexpr qint64 sampleLifetimeMs = 5000;
}

CompassHeadingController::CompassHeadingController(QObject* parent)
    : FactGroup(100, parent)
{
    auto* metadata = new FactMetaData(FactMetaData::valueTypeDouble, &_heading);
    metadata->setRawUnits(QStringLiteral("deg"));
    metadata->setDecimalPlaces(1);
    metadata->setShortDescription(tr("Compass magnetic heading"));
    _heading.setMetaData(metadata);
    _addFact(&_heading);
    _clock.start();
    _reset();
    _expiryTimer.setInterval(500);
    connect(&_expiryTimer, &QTimer::timeout, this, &CompassHeadingController::_updateHeading);
    _expiryTimer.start();
}

Vehicle* CompassHeadingController::vehicle() const
{
    return _vehicle.data();
}

void CompassHeadingController::setVehicle(Vehicle* vehicle)
{
    if (_vehicle == vehicle) {
        return;
    }
    if (_vehicle) {
        disconnect(_vehicle, nullptr, this, nullptr);
    }
    _vehicle = vehicle;
    _reset();
    if (_vehicle) {
        connect(_vehicle, &Vehicle::mavlinkMessageReceived, this, &CompassHeadingController::_handleMessage);
        connect(_vehicle, &QObject::destroyed, this, [this]() {
            _vehicle = nullptr;
            _reset();
            emit vehicleChanged();
        });
    }
    emit vehicleChanged();
}

void CompassHeadingController::_reset()
{
    _attitudeTime = _quaternionTime = _magneticTime = _highResolutionTime = -1;
    _heading.setRawValue(qQNaN());
    _setTelemetryAvailable(false);
}

void CompassHeadingController::_handleMessage(const mavlink_message_t& message)
{
    if (!_vehicle || message.sysid != _vehicle->id() || message.compid != _vehicle->defaultComponentId()) {
        return;
    }
    const qint64 now = _clock.elapsed();
    switch (message.msgid) {
        case MAVLINK_MSG_ID_ATTITUDE: {
            if (_quaternionTime >= 0 && now - _quaternionTime < sampleLifetimeMs) {
                return;
            }
            mavlink_attitude_t attitude{};
            mavlink_msg_attitude_decode(&message, &attitude);
            _roll = attitude.roll;
            _pitch = attitude.pitch;
            _attitudeTime = now;
            break;
        }
        case MAVLINK_MSG_ID_ATTITUDE_QUATERNION: {
            mavlink_attitude_quaternion_t attitude{};
            mavlink_msg_attitude_quaternion_decode(&message, &attitude);
            const double norm = std::hypot(std::hypot(attitude.q1, attitude.q2), std::hypot(attitude.q3, attitude.q4));
            if (!std::isfinite(norm) || norm < 1e-6) {
                return;
            }
            const float quaternion[] = {float(attitude.q1 / norm), float(attitude.q2 / norm), float(attitude.q3 / norm),
                                        float(attitude.q4 / norm)};
            float roll = 0;
            float pitch = 0;
            float yaw = 0;
            // Use the body-frame attitude, without display-only repr_offset_q rotations.
            mavlink_quaternion_to_euler(quaternion, &roll, &pitch, &yaw);
            _roll = roll;
            _pitch = pitch;
            _attitudeTime = _quaternionTime = now;
            break;
        }
        case MAVLINK_MSG_ID_HIGHRES_IMU: {
            mavlink_highres_imu_t imu{};
            mavlink_msg_highres_imu_decode(&message, &imu);
            if (imu.id != 0 || (imu.fields_updated & 0x01c0) != 0x01c0) {
                return;
            }
            _magnetic = {imu.xmag, imu.ymag, imu.zmag};
            _magneticTime = _highResolutionTime = now;
            break;
        }
        case MAVLINK_MSG_ID_RAW_IMU: {
            mavlink_raw_imu_t imu{};
            mavlink_msg_raw_imu_decode(&message, &imu);
            if (imu.id != 0 || (_highResolutionTime >= 0 && now - _highResolutionTime < sampleLifetimeMs)) {
                return;
            }
            _magnetic = {double(imu.xmag), double(imu.ymag), double(imu.zmag)};
            _magneticTime = now;
            break;
        }
        default:
            return;
    }
    _updateHeading();
}

void CompassHeadingController::_updateHeading()
{
    const qint64 now = _clock.elapsed();
    double heading = qQNaN();
    if (_vehicle && _attitudeTime >= 0 && _magneticTime >= 0 && now - _attitudeTime < sampleLifetimeMs &&
        now - _magneticTime < sampleLifetimeMs && std::isfinite(_roll) && std::isfinite(_pitch) &&
        std::abs(_pitch) < qDegreesToRadians(85.0)) {
        const auto [x, y, z] = _magnetic;
        const double norm = std::hypot(x, y, z);
        const double horizontalX =
            x * std::cos(_pitch) + (y * std::sin(_roll) + z * std::cos(_roll)) * std::sin(_pitch);
        const double horizontalY = y * std::cos(_roll) - z * std::sin(_roll);
        if (std::isfinite(norm) && norm > 0 && std::hypot(horizontalX, horizontalY) > norm * 1e-6) {
            heading = std::fmod(qRadiansToDegrees(std::atan2(-horizontalY, horizontalX)) + 360.0, 360.0);
        }
    }
    _heading.setRawValue(heading);
    _setTelemetryAvailable(std::isfinite(heading));
}
