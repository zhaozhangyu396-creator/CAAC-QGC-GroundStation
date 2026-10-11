#pragma once

#include <optional>

#include <QtCore/QList>
#include <QtPositioning/QGeoCoordinate>

namespace CaacRouteTemplates {
enum class Shape
{
    Circle,
    Hexagon,
    Bow,
    Star
};

struct Options
{
    Shape shape = Shape::Circle;
    double size = 100;
    double width = 50;
    int count = 10;
    bool clockwise = true;
    bool vertexTop = false;
    int start = 0;
    double bearing = 0;
    bool close = false;
};

/// Circle uses a centre anchor; other shapes use the selected existing first vertex, without duplicating it.
std::optional<QList<QGeoCoordinate>> generate(const QGeoCoordinate& anchor, const Options& options);
}  // namespace CaacRouteTemplates
