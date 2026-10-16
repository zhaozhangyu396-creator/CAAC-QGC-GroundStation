#pragma once

#include <QtCore/QList>
#include <QtPositioning/QGeoCoordinate>

/// Horizontal inclusion fence around the filled outer boundary of an A/B figure-eight site.
class CaacFigureEightFenceGeometry final
{
public:
    /// Returns an unclosed, circumscribed polygon with a 3 cm allowance for MAVLink coordinate rounding.
    /// Valid ranges: A/B separation 4–2000 m, corridor half-width 0.1–20 m, margin 0–100 m.
    [[nodiscard]] static QList<QGeoCoordinate> boundary(const QGeoCoordinate& pointA, const QGeoCoordinate& pointB,
                                                        double corridorHalfWidth, double margin);

    /// Tests the same polygon, including its edges. Invalid geometry or positions return false.
    [[nodiscard]] static bool contains(const QGeoCoordinate& pointA, const QGeoCoordinate& pointB,
                                       double corridorHalfWidth, double margin, const QGeoCoordinate& position);

private:
    CaacFigureEightFenceGeometry() = delete;
};
