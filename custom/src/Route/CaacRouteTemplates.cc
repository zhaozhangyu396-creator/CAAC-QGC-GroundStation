#include "CaacRouteTemplates.h"

#include <cmath>
#include <numbers>

#include "QGCGeo.h"

namespace CaacRouteTemplates {
namespace {
struct Offset
{
    double east;
    double north;
};
}  // namespace

std::optional<QList<QGeoCoordinate>> generate(const QGeoCoordinate& anchor, const Options& options)
{
    if (!anchor.isValid() || !std::isfinite(options.size) || options.size <= 0 || options.size > 100000 ||
        !std::isfinite(options.bearing)) {
        return std::nullopt;
    }
    QList<Offset> vertices;
    const double direction = options.clockwise ? 1 : -1;
    constexpr double DEGREES = 180.0 / std::numbers::pi;
    switch (options.shape) {
        case Shape::Circle:
            if (options.count < 3 || options.count > 1000) {
                return std::nullopt;
            }
            for (int i = 0; i < options.count; ++i) {
                const double angle = direction * i * 360.0 / options.count / DEGREES;
                vertices.append({options.size / 2 * std::sin(angle), options.size / 2 * std::cos(angle)});
            }
            break;
        case Shape::Hexagon:
            if (options.start < 0 || options.start > 5) {
                return std::nullopt;
            }
            for (int i = 0; i < 6; ++i) {
                const double angle = ((options.vertexTop ? 0 : 30) + options.start * 60 + direction * i * 60) / DEGREES;
                vertices.append({options.size * std::sin(angle), options.size * std::cos(angle)});
            }
            break;
        case Shape::Bow: {
            if (options.count < 4 || options.count > 1000 || options.start < 0 || options.start > 3 ||
                !std::isfinite(options.width) || options.width <= 0 || options.width > 100000) {
                return std::nullopt;
            }
            const int rows = (options.count + 1) / 2;
            const double eastSign = (options.start % 2 == 0) ? 1 : -1;
            const double northSign = options.start < 2 ? -1 : 1;
            for (int i = 0; i < options.count; ++i) {
                const int row = i / 2;
                const bool farSide = (row % 2 == 0) ? (i % 2 == 1) : (i % 2 == 0);
                vertices.append({farSide ? eastSign * options.size : 0, northSign * row * options.width / (rows - 1)});
            }
            break;
        }
        case Shape::Star: {
            const double radius = options.size / (2 * std::sin(72 / DEGREES));
            for (int i = 0; i < 5; ++i) {
                const double angle = direction * i * 144 / DEGREES;
                vertices.append({radius * std::sin(angle), radius * std::cos(angle)});
            }
            break;
        }
        default:
            return std::nullopt;
    }
    const bool centreAnchor = options.shape == Shape::Circle;
    const Offset origin = centreAnchor ? Offset{0, 0} : vertices.first();
    QList<QGeoCoordinate> result;
    for (int i = centreAnchor ? 0 : 1; i < vertices.size(); ++i) {
        const double east = vertices[i].east - origin.east;
        const double north = vertices[i].north - origin.north;
        auto point = QGCGeo::geodesicDestination(anchor, options.bearing + std::atan2(east, north) * DEGREES,
                                                 std::hypot(east, north));
        point.setAltitude(anchor.altitude());
        if (!point.isValid()) {
            return std::nullopt;
        }
        result.append(point);
    }
    if (options.close) {
        result.append(centreAnchor ? result.first() : anchor);
    }
    return result;
}
}  // namespace CaacRouteTemplates
