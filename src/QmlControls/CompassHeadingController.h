#pragma once

#include <array>

#include <QtCore/QElapsedTimer>
#include <QtCore/QPointer>

#include "FactGroup.h"

class Vehicle;

class CompassHeadingController : public FactGroup
{
    Q_OBJECT
    QML_ELEMENT
    Q_MOC_INCLUDE("Vehicle.h")
    Q_PROPERTY(Vehicle* vehicle READ vehicle WRITE setVehicle NOTIFY vehicleChanged)
    Q_PROPERTY(Fact* heading READ heading CONSTANT)

public:
    explicit CompassHeadingController(QObject* parent = nullptr);

    Vehicle* vehicle() const;
    void setVehicle(Vehicle* vehicle);

    Fact* heading() { return &_heading; }

signals:
    void vehicleChanged();

private:
    void _reset();
    void _handleMessage(const mavlink_message_t& message);
    void _updateHeading();

    QPointer<Vehicle> _vehicle;
    Fact _heading{0, QStringLiteral("heading"), FactMetaData::valueTypeDouble};
    QElapsedTimer _clock;
    QTimer _expiryTimer;
    std::array<double, 3> _magnetic{};
    double _roll = 0;
    double _pitch = 0;
    qint64 _attitudeTime = -1;
    qint64 _quaternionTime = -1;
    qint64 _magneticTime = -1;
    qint64 _highResolutionTime = -1;
};
