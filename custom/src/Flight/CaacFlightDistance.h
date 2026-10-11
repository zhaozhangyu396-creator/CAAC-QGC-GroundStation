#pragma once

#include <QtCore/QObject>
#include <QtQmlIntegration/QtQmlIntegration>

#include "CaacDirectionalDistance.h"

class Vehicle;

/// One flight's telemetry-derived distances; does not store or change vehicle parameters.
class CaacFlightDistance : public QObject
{
    Q_OBJECT
    QML_ELEMENT
    QML_UNCREATABLE("Created for each connected vehicle")
    Q_PROPERTY(double north READ north NOTIFY distanceChanged)
    Q_PROPERTY(double south READ south NOTIFY distanceChanged)
    Q_PROPERTY(double east READ east NOTIFY distanceChanged)
    Q_PROPERTY(double west READ west NOTIFY distanceChanged)

public:
    explicit CaacFlightDistance(Vehicle* vehicle, QObject* parent = nullptr);

    double north() const { return _distance.totals().north; }

    double south() const { return _distance.totals().south; }

    double east() const { return _distance.totals().east; }

    double west() const { return _distance.totals().west; }

    void resetForSimulationRestart();

signals:
    void distanceChanged();

private:
    CaacDirectionalDistance _distance;
    bool _communicationLost = false;
};
