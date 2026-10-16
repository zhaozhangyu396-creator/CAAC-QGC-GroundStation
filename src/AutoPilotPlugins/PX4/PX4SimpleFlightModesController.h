#pragma once

#include <QtCore/QPointer>
#include <QtCore/QVariantList>
#include <QtQmlIntegration/QtQmlIntegration>

#include "FactPanelController.h"

class Joystick;

/// \brief MVC Controller for PX4SimpleFlightModes.qml
///
class PX4SimpleFlightModesController : public FactPanelController
{
    Q_OBJECT
    QML_ELEMENT
public:
    PX4SimpleFlightModesController();

    Q_PROPERTY(int activeFlightMode READ activeFlightMode NOTIFY activeFlightModeChanged)
    Q_PROPERTY(int channelCount MEMBER _channelCount NOTIFY channelCountChanged)
    Q_PROPERTY(QVariantList rcChannelValues MEMBER _rcChannelValues NOTIFY rcChannelValuesChanged)

    Q_PROPERTY(bool joystickInput READ joystickInput NOTIFY joystickInputChanged)

    bool joystickInput() const { return _joystickInput; }

    int activeFlightMode(void) const { return _activeFlightMode; }

signals:
    void joystickInputChanged();
    void activeFlightModeChanged(int activeFlightMode);
    void channelOptionEnabledChanged(void);
    void rcChannelValuesChanged(void);
    void channelCountChanged();

private slots:
    void channelValuesChanged(QVector<int> pwmValues);

private:
    void _setJoystick(Joystick* joystick);
    void _refreshInputSource();
    void _updateChannels(const QVector<int>& values);
    void _updateActiveFlightMode();

    QPointer<Joystick> _joystick;
    QMetaObject::Connection _joystickConnection;
    QVector<int> _joystickValues;
    QVector<int> _receiverValues;
    bool _joystickInput = false;
    int _activeFlightMode = 0;
    int _channelCount = 0;
    QVariantList _rcChannelValues;
};
