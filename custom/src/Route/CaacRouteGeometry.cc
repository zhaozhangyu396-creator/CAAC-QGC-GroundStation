#include "CaacRouteGeometry.h"

#include <algorithm>
#include <cmath>
#include <limits>

#include "QGCGeo.h"

namespace CaacRouteGeometry {

double normalizedAngle(double angle)
{
    return std::fmod(std::fmod(angle, 360.0) + 360.0, 360.0);
}

std::optional<QGeoCoordinate> relativePoint(const QGeoCoordinate& reference, double bearing, double distance)
{
    if (!reference.isValid() || !std::isfinite(bearing) || !std::isfinite(distance) || distance <= 0.0) {
        return std::nullopt;
    }
    const auto result = QGCGeo::geodesicDestination(reference, normalizedAngle(bearing), distance);
    return result.isValid() ? std::optional(result) : std::nullopt;
}

std::optional<QGeoCoordinate> interiorAnglePoint(const QGeoCoordinate& previous, const QGeoCoordinate& reference,
                                                 double angle, double distance)
{
    if (!previous.isValid() || !reference.isValid() || QGCGeo::geodesicDistance(previous, reference) < 0.001) {
        return std::nullopt;
    }
    // Use the reverse bearing at the vertex, rather than the initial bearing at the previous point.
    return relativePoint(reference, QGCGeo::geodesicAzimuth(reference, previous) + angle, distance);
}

std::optional<QList<QGeoCoordinate>> rotate(const QList<QGeoCoordinate>& coordinates, const QGeoCoordinate& center,
                                            double clockwiseDegrees)
{
    if (!center.isValid() || !std::isfinite(clockwiseDegrees)) {
        return std::nullopt;
    }
    const double angle = normalizedAngle(clockwiseDegrees);
    QList<QGeoCoordinate> result;
    result.reserve(coordinates.size());
    for (const auto& coordinate : coordinates) {
        if (!coordinate.isValid()) {
            return std::nullopt;
        }
        if (angle == 0.0 ||
            (coordinate.latitude() == center.latitude() && coordinate.longitude() == center.longitude())) {
            result.append(coordinate);
            continue;
        }
        const double distance = QGCGeo::geodesicDistance(center, coordinate);
        const double bearing = QGCGeo::geodesicAzimuth(center, coordinate);
        auto rotated = QGCGeo::geodesicDestination(center, bearing + angle, distance);
        rotated.setAltitude(coordinate.altitude());
        if (!rotated.isValid()) {
            return std::nullopt;
        }
        result.append(rotated);
    }
    return result;
}

std::optional<double> alignmentAngle(const QGeoCoordinate& a, const QGeoCoordinate& b, const QGeoCoordinate& center,
                                     double targetBearing)
{
    if (!a.isValid() || !b.isValid() || !center.isValid() || !std::isfinite(targetBearing) ||
        QGCGeo::geodesicDistance(a, b) < 0.001) {
        return std::nullopt;
    }
    const auto residual = [&](double angle) {
        const auto points = rotate({a, b}, center, angle);
        if (!points) {
            return std::numeric_limits<double>::quiet_NaN();
        }
        return std::remainder(QGCGeo::geodesicAzimuth(points->at(0), points->at(1)) - targetBearing, 360.0);
    };
    double angle = std::remainder(targetBearing - QGCGeo::geodesicAzimuth(a, b), 360.0);
    // Bearings on an ellipsoid do not add linearly when the pivot differs from A.
    for (int iteration = 0; iteration < 24; ++iteration) {
        const double error = residual(angle);
        if (!std::isfinite(error)) {
            return std::nullopt;
        }
        if (std::abs(error) < 1e-8) {
            return angle;
        }
        constexpr double STEP = 0.001;
        const double derivative = std::remainder(residual(angle + STEP) - residual(angle - STEP), 360.0) / (2 * STEP);
        if (!std::isfinite(derivative) || std::abs(derivative) < 1e-6) {
            return std::nullopt;
        }
        angle -= std::clamp(error / derivative, -90.0, 90.0);
    }
    return std::nullopt;
}
}  // namespace CaacRouteGeometry
