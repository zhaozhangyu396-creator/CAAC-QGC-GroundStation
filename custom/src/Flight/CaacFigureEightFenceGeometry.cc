#include "CaacFigureEightFenceGeometry.h"

#include <algorithm>
#include <cmath>
#include <numbers>
#include <optional>

#include <QtCore/QPointF>

#include "QGCGeo.h"

namespace {
constexpr double DEGREES_PER_RADIAN = 180 / std::numbers::pi;
constexpr double MAX_ARC_STEP = 12 / DEGREES_PER_RADIAN;
constexpr double COORDINATE_ROUNDING_ALLOWANCE = 0.03;
constexpr double EDGE_TOLERANCE = 1e-6;

struct Geometry
{
    QGeoCoordinate origin;
    double bearing = 0;
    QList<QPointF> polygon;
};

std::optional<Geometry> geometry(const QGeoCoordinate& pointA, const QGeoCoordinate& pointB, double corridorHalfWidth,
                                 double margin)
{
    if (!pointA.isValid() || !pointB.isValid() || !std::isfinite(corridorHalfWidth) || corridorHalfWidth < 0.1 ||
        corridorHalfWidth > 20 || !std::isfinite(margin) || margin < 0 || margin > 100) {
        return std::nullopt;
    }
    const double separation = QGCGeo::geodesicDistance(pointA, pointB);
    if (!std::isfinite(separation) || separation < 4 - EDGE_TOLERANCE || separation > 2000 + EDGE_TOLERANCE) {
        return std::nullopt;
    }

    Geometry result;
    result.bearing = QGCGeo::geodesicAzimuth(pointA, pointB);
    result.origin = QGCGeo::geodesicDestination(pointA, result.bearing, separation / 2);
    const double radius = separation / 4;
    const double outerRadius = radius + corridorHalfWidth + margin + COORDINATE_ROUNDING_ALLOWANCE;
    const double intersectionAngle = std::acos(std::clamp(radius / outerRadius, 0.0, 1.0));
    const double intersectionY = std::sqrt((outerRadius - radius) * (outerRadius + radius));
    const double span = 2 * (std::numbers::pi - intersectionAngle);
    const int segments = static_cast<int>(std::ceil(span / MAX_ARC_STEP));
    const double step = span / segments;
    const double tangentRadius = outerRadius / std::cos(step / 2);
    result.polygon.reserve(2 * segments + 2);

    // Tangent intersections keep every edge outside the requested clearance; sampled arc chords do not.
    for (int circle = 0; circle < 2; ++circle) {
        result.polygon.append(QPointF(0, circle == 0 ? intersectionY : -intersectionY));
        const double centre = circle == 0 ? -radius : radius;
        const double start = intersectionAngle + circle * std::numbers::pi;
        for (int i = 0; i < segments; ++i) {
            const double angle = start + (i + 0.5) * step;
            result.polygon.append(QPointF(centre + tangentRadius * std::cos(angle), tangentRadius * std::sin(angle)));
        }
    }
    return result;
}

QPointF toLocal(const Geometry& shape, const QGeoCoordinate& coordinate)
{
    const double distance = QGCGeo::geodesicDistance(shape.origin, coordinate);
    const double angle = (QGCGeo::geodesicAzimuth(shape.origin, coordinate) - shape.bearing) / DEGREES_PER_RADIAN;
    return QPointF(distance * std::cos(angle), distance * std::sin(angle));
}

bool containsPoint(const QList<QPointF>& polygon, const QPointF& point)
{
    bool inside = false;
    for (qsizetype i = 0; i < polygon.size(); ++i) {
        const auto& start = polygon[i];
        const auto& end = polygon[(i + 1) % polygon.size()];
        const auto segment = end - start;
        const auto offset = point - start;
        const double lengthSquared = QPointF::dotProduct(segment, segment);
        const double fraction = std::clamp(QPointF::dotProduct(offset, segment) / lengthSquared, 0.0, 1.0);
        const auto residual = offset - fraction * segment;
        if (std::hypot(residual.x(), residual.y()) <= EDGE_TOLERANCE) {
            return true;
        }
        if ((start.y() > point.y()) != (end.y() > point.y()) &&
            point.x() < start.x() + (point.y() - start.y()) * segment.x() / segment.y()) {
            inside = !inside;
        }
    }
    return inside;
}
}  // namespace

QList<QGeoCoordinate> CaacFigureEightFenceGeometry::boundary(const QGeoCoordinate& pointA, const QGeoCoordinate& pointB,
                                                             double corridorHalfWidth, double margin)
{
    const auto shape = geometry(pointA, pointB, corridorHalfWidth, margin);
    if (!shape) {
        return {};
    }
    QList<QGeoCoordinate> coordinates;
    coordinates.reserve(shape->polygon.size());
    for (const auto& point : shape->polygon) {
        coordinates.append(QGCGeo::geodesicDestination(
            shape->origin, shape->bearing + std::atan2(point.y(), point.x()) * DEGREES_PER_RADIAN,
            std::hypot(point.x(), point.y())));
    }
    return coordinates;
}

bool CaacFigureEightFenceGeometry::contains(const QGeoCoordinate& pointA, const QGeoCoordinate& pointB,
                                            double corridorHalfWidth, double margin, const QGeoCoordinate& position)
{
    if (!position.isValid()) {
        return false;
    }
    const auto shape = geometry(pointA, pointB, corridorHalfWidth, margin);
    return shape && containsPoint(shape->polygon, toLocal(*shape, position));
}
