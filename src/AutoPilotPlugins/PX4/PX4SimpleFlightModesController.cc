#include "PX4SimpleFlightModesController.h"

#include <algorithm>
#include <cmath>

#include "Fact.h"
#include "Joystick.h"
#include "JoystickManager.h"
#include "ParameterManager.h"
#include "Vehicle.h"

PX4SimpleFlightModesController::PX4SimpleFlightModesController()
{
    const QStringList usedParams{QStringLiteral("COM_FLTMODE1"),  QStringLiteral("COM_FLTMODE2"),
                                 QStringLiteral("COM_FLTMODE3"),  QStringLiteral("COM_FLTMODE4"),
                                 QStringLiteral("COM_FLTMODE5"),  QStringLiteral("COM_FLTMODE6"),
                                 QStringLiteral("RC_MAP_FLTMODE")};
    if (!_vehicle || !_allParametersExists(ParameterManager::defaultComponentId, usedParams)) {
        return;
    }
    connect(_vehicle, &Vehicle::rcChannelsClampedChanged, this, &PX4SimpleFlightModesController::channelValuesChanged);
    connect(getParameterFact(-1, QStringLiteral("RC_MAP_FLTMODE")), &Fact::rawValueChanged, this,
            &PX4SimpleFlightModesController::_updateActiveFlightMode);
    auto* manager = JoystickManager::instance();
    connect(manager, &JoystickManager::activeJoystickChanged, this, &PX4SimpleFlightModesController::_setJoystick);
    connect(manager, &JoystickManager::activeJoystickEnabledForActiveVehicleChanged, this,
            &PX4SimpleFlightModesController::_refreshInputSource);
    _setJoystick(manager->activeJoystick());
}

void PX4SimpleFlightModesController::_setJoystick(Joystick* joystick)
{
    disconnect(_joystickConnection);
    _joystick = joystick;
    _joystickValues.clear();
    if (joystick) {
        _joystickConnection = connect(
            joystick, &Joystick::rawChannelValuesChanged, this,
            [this, source = QPointer<Joystick>(joystick)](const QVector<int>& values) {
                if (source && source == _joystick) {
                    _joystickValues = values;
                    if (_joystickInput) {
                        _updateChannels(values);
                    }
                }
            },
            Qt::QueuedConnection);
    }
    _refreshInputSource();
}

void PX4SimpleFlightModesController::_refreshInputSource()
{
    const bool joystickInput = _joystick && JoystickManager::instance()->activeJoystickEnabledForActiveVehicle();
    if (_joystickInput != joystickInput) {
        _joystickInput = joystickInput;
        emit joystickInputChanged();
    }
    _updateChannels(_joystickInput ? _joystickValues : _receiverValues);
}

void PX4SimpleFlightModesController::channelValuesChanged(QVector<int> pwmValues)
{
    _receiverValues = pwmValues;
    if (!_joystickInput) {
        _updateChannels(pwmValues);
    }
}

void PX4SimpleFlightModesController::_updateChannels(const QVector<int>& values)
{
    _rcChannelValues.clear();
    for (int channel = 0; channel < values.size(); ++channel) {
        int value = values[channel];
        if (_joystickInput && _joystick && channel < _joystick->axisCount()) {
            const auto calibration = _joystick->getAxisCalibration(channel);
            const int range = value > calibration.center ? calibration.max - calibration.center
                                                         : calibration.center - calibration.min;
            if (range > 0) {
                const double normalized =
                    std::clamp((value - calibration.center) / static_cast<double>(range), -1.0, 1.0);
                value = 1500 + std::lround(normalized * (calibration.reversed ? -500 : 500));
            } else {
                value = -1;
            }
        }
        _rcChannelValues.append(value);
    }
    if (_channelCount != values.size()) {
        _channelCount = values.size();
        emit channelCountChanged();
    }
    emit rcChannelValuesChanged();
    _updateActiveFlightMode();
}

void PX4SimpleFlightModesController::_updateActiveFlightMode()
{
    int mode = 0;
    auto* modeChannel = getParameterFact(-1, QStringLiteral("RC_MAP_FLTMODE"), false);
    const int channel = modeChannel ? modeChannel->rawValue().toInt() - 1 : -1;
    if (channel >= 0 && channel < _rcChannelValues.size()) {
        const int value = _rcChannelValues[channel].toInt();
        int minimum = 0;
        int maximum = 0;
        int center = 0;
        int reverse = 1;
        if (_joystickInput && _joystick && channel < _joystick->axisCount()) {
            minimum = 1000;
            maximum = 2000;
            center = 1500;
        } else if (!_joystickInput && value > 0) {
            const auto parameter = [this, channel](const char* suffix) {
                return getParameterFact(-1, QStringLiteral("RC%1_%2").arg(channel + 1).arg(QLatin1String(suffix)),
                                        false);
            };
            auto* minFact = parameter("MIN");
            auto* maxFact = parameter("MAX");
            auto* trimFact = parameter("TRIM");
            auto* reverseFact = parameter("REV");
            if (minFact && maxFact && trimFact && reverseFact) {
                minimum = minFact->rawValue().toInt();
                maximum = maxFact->rawValue().toInt();
                center = trimFact->rawValue().toInt();
                reverse = reverseFact->rawValue().toDouble() < 0 ? -1 : 1;
            }
        }
        if (minimum < center && center < maximum) {
            const double range = value > center ? maximum - center : center - minimum;
            const double normalized = std::clamp((value - center) / range * reverse, -1.0, 1.0);
            // Preserve the PX4 six-slot thresholds used for receiver inputs.
            mode = std::clamp(static_cast<int>(((normalized + 1.05) * 6 + 1.0 / 6) / 2.1 + 1.0 / 6), 0, 5) + 1;
        }
    }
    if (_activeFlightMode != mode) {
        _activeFlightMode = mode;
        emit activeFlightModeChanged(mode);
    }
}
