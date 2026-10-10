#pragma once

#include <optional>

#include <QtCore/QList>
#include <QtPositioning/QGeoCoordinate>

/// WGS84 horizontal geometry; altitude values are carried through without conversion.
namespace CaacRouteGeometry {
double normalizedAngle(double angle);
std::optional<QGeoCoordinate> relativePoint(const QGeoCoordinate& reference, double bearing, double distance);
std::optional<QGeoCoordinate> interiorAnglePoint(const QGeoCoordinate& previous, const QGeoCoordinate& reference,
                                                 double angle, double distance);
std::optional<QList<QGeoCoordinate>> rotate(const QList<QGeoCoordinate>& coordinates, const QGeoCoordinate& center,
                                            double clockwiseDegrees);
std::optional<double> alignmentAngle(const QGeoCoordinate& a, const QGeoCoordinate& b, const QGeoCoordinate& center,
                                     double targetBearing);
}  // namespace CaacRouteGeometry
