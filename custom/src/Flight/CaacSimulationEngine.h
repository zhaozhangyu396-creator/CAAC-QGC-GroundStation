#pragma once

#include <QtCore/QMap>
#include <QtCore/QString>
#include <QtPositioning/QGeoCoordinate>

#include "MAVLinkLib.h"

/// Deterministic kinematic training model, independent of vehicle command transport.
class CaacSimulationEngine
{
public:
    struct Options
    {
        double horizontalSpeed = 5;
        double verticalSpeed = 2;
        double yawRate = 60;
        double windSpeed = 0;
        double windBearing = 0;
        int turnMode = 1;
        int climbMode = 1;
    };

    struct State
    {
        QGeoCoordinate position;
        double heading = 0;
        double roll = 0;
        double pitch = 0;
        double northSpeed = 0;
        double eastSpeed = 0;
        double climbSpeed = 0;
        int sequence = 0;
        bool airborne = false;
        bool finished = false;
    };

    struct WaypointOptions
    {
        double verticalSpeed = 2;
        int turnMode = 1;
        int climbMode = 1;
    };

    void initialize(const QGeoCoordinate& home, const Options& options);
    bool load(const QMap<uint16_t, mavlink_mission_item_int_t>& mission, const QGeoCoordinate& home,
              const Options& options, QString& error, const QMap<int, WaypointOptions>& waypointOptions = {});
    bool jump(int sequence);
    void advance(double seconds);
    void pause();
    bool setHorizontalSpeed(double speed);
    void setAltitude(double amslAltitude);
    void advanceHoldAltitude(double amslAltitude, double seconds);
    void advanceGuidedTarget(const QGeoCoordinate& target, double seconds);
    void advanceManual(double forward, double right, double yaw, double throttle, double seconds,
                       bool attitudeMode = false);
    bool advanceLanding(double seconds);
    bool advanceReturn(double seconds);
    void restoreState(const State& state);

    const State& state() const { return _state; }

    const QGeoCoordinate& home() const { return _home; }

    bool loaded() const { return !_items.isEmpty(); }

    int remainingLoops() const;

private:
    QGeoCoordinate _target(const mavlink_mission_item_int_t& item) const;
    void _next();
    bool _move(const QGeoCoordinate& target, double seconds, bool landing);
    double _rotate(double target, double seconds, double rate);
    QMap<uint16_t, mavlink_mission_item_int_t> _items;
    QMap<int, int> _jumpCounts;
    QMap<int, WaypointOptions> _waypointOptions;
    QGeoCoordinate _home;
    Options _options;
    State _state;
    double _wait = -1;
    bool _returnLanding = false;
};
