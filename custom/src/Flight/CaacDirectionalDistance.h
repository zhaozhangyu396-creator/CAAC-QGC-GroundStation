#pragma once

#include <QtPositioning/QGeoCoordinate>

/// Horizontal distance components in metres, accumulated separately for each direction.
class CaacDirectionalDistance
{
public:
    struct Totals
    {
        double north = 0;
        double south = 0;
        double east = 0;
        double west = 0;
    };

    void setAirborne(bool airborne, const QGeoCoordinate& coordinate);
    bool updateCoordinate(const QGeoCoordinate& coordinate);
    void reset();

    void breakTrack() { _previous = QGeoCoordinate(); }

    const Totals& totals() const { return _totals; }

private:
    Totals _totals;
    QGeoCoordinate _previous;
    bool _airborne = false;
};
