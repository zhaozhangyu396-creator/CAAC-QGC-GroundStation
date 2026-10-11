#include "CaacDirectionalDistance.h"

#include <algorithm>
#include <cmath>
#include <numbers>

#include "QGCGeo.h"

void CaacDirectionalDistance::setAirborne(bool airborne, const QGeoCoordinate& coordinate)
{
    if (_airborne == airborne) {
        return;
    }
    _airborne = airborne;
    if (airborne) {
        reset();
        _previous = coordinate;
    } else {
        breakTrack();
    }
}

bool CaacDirectionalDistance::updateCoordinate(const QGeoCoordinate& coordinate)
{
    if (!_airborne) {
        return false;
    }
    if (!coordinate.isValid()) {
        breakTrack();
        return false;
    }
    const auto previous = _previous;
    _previous = coordinate;
    if (!previous.isValid()) {
        return false;
    }
    const double distance = QGCGeo::geodesicDistance(previous, coordinate);
    if (!std::isfinite(distance) || distance <= 0) {
        return false;
    }
    const double radians = QGCGeo::geodesicAzimuth(previous, coordinate) * std::numbers::pi / 180.0;
    if (!std::isfinite(radians)) {
        breakTrack();
        return false;
    }
    const double north = distance * std::cos(radians);
    const double east = distance * std::sin(radians);
    _totals.north += std::max(north, 0.0);
    _totals.south += std::max(-north, 0.0);
    _totals.east += std::max(east, 0.0);
    _totals.west += std::max(-east, 0.0);
    return true;
}

void CaacDirectionalDistance::reset()
{
    _totals = {};
    breakTrack();
}
