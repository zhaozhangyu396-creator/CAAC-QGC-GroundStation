#include "CaacFlightDistance.h"

#include "Vehicle.h"
#include "VehicleLinkManager.h"

CaacFlightDistance::CaacFlightDistance(Vehicle* vehicle, QObject* parent)
    : QObject(parent)
{
    if (!vehicle) {
        return;
    }
    _communicationLost = vehicle->vehicleLinkManager()->communicationLost();
    _distance.setAirborne(vehicle->airborne(), _communicationLost ? QGeoCoordinate() : vehicle->coordinate());
    connect(vehicle, &Vehicle::airborneChanged, this, [this, vehicle](bool airborne) {
        if (!vehicle) {
            return;
        }
        _distance.setAirborne(airborne, _communicationLost ? QGeoCoordinate() : vehicle->coordinate());
        emit distanceChanged();
    });
    connect(vehicle, &Vehicle::coordinateChanged, this, [this](const QGeoCoordinate& coordinate) {
        if (!_communicationLost && _distance.updateCoordinate(coordinate)) {
            emit distanceChanged();
        }
    });
    connect(vehicle->vehicleLinkManager(), &VehicleLinkManager::communicationLostChanged, this, [this](bool lost) {
        _communicationLost = lost;
        // The path during a telemetry outage is unknown; do not bridge it with a guessed straight segment.
        _distance.breakTrack();
    });
}

void CaacFlightDistance::resetForSimulationRestart()
{
    _distance.reset();
    emit distanceChanged();
}
