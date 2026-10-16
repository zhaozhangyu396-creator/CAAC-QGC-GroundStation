#include <algorithm>
#include <cmath>
#include <limits>
#include <numbers>

#include <QtCore/QPointF>
#include <QtGui/QPolygonF>

#include "CaacFigureEightFenceGeometry.h"
#include "QGCGeo.h"
#include "UnitTest.h"

namespace {
constexpr double DEGREES_PER_RADIAN = 180 / std::numbers::pi;

QGeoCoordinate fromLocal(const QGeoCoordinate& origin, double bearing, double x, double y)
{
    return QGCGeo::geodesicDestination(origin, bearing + std::atan2(y, x) * DEGREES_PER_RADIAN, std::hypot(x, y));
}

QPointF toLocal(const QGeoCoordinate& origin, double bearing, const QGeoCoordinate& coordinate)
{
    const double distance = QGCGeo::geodesicDistance(origin, coordinate);
    const double angle = (QGCGeo::geodesicAzimuth(origin, coordinate) - bearing) / DEGREES_PER_RADIAN;
    return QPointF(distance * std::cos(angle), distance * std::sin(angle));
}

double distanceToSegment(const QPointF& point, const QPointF& start, const QPointF& end)
{
    const auto edge = end - start;
    const double fraction =
        std::clamp(QPointF::dotProduct(point - start, edge) / QPointF::dotProduct(edge, edge), 0.0, 1.0);
    const auto residual = point - start - fraction * edge;
    return std::hypot(residual.x(), residual.y());
}
}  // namespace

class CaacFigureEightFenceGeometryTest : public UnitTest
{
    Q_OBJECT

private slots:
    void _clearance_data();
    void _clearance();
    void _filledInteriorAndConcaveNeck();
    void _widthAndMargin();
    void _invalid_data();
    void _invalid();
};

void CaacFigureEightFenceGeometryTest::_clearance_data()
{
    QTest::addColumn<QGeoCoordinate>("pointA");
    QTest::addColumn<double>("bearing");
    QTest::addColumn<double>("separation");
    QTest::addColumn<double>("width");
    QTest::addColumn<double>("margin");
    QTest::newRow("standard") << QGeoCoordinate(30, 120, 8) << 90.0 << 24.0 << 1.2 << 3.0;
    QTest::newRow("rotated") << QGeoCoordinate(-35, 18) << 237.0 << 24.0 << 1.2 << 3.0;
    QTest::newRow("date-line") << QGeoCoordinate(0, 179.99995) << 90.0 << 24.0 << 1.2 << 3.0;
    QTest::newRow("north-pole") << QGeoCoordinate(90, 110) << 38.0 << 24.0 << 1.2 << 3.0;
    QTest::newRow("south-high-latitude") << QGeoCoordinate(-89.99, -179.9999) << 315.0 << 24.0 << 1.2 << 3.0;
    QTest::newRow("narrow-neck") << QGeoCoordinate(30, 120) << 0.0 << 24.0 << 0.1 << 0.0;
    QTest::newRow("minimum-site") << QGeoCoordinate(30, 120) << 120.0 << 4.0 << 0.1 << 0.0;
    QTest::newRow("maximum-site") << QGeoCoordinate(85, 20) << 15.0 << 2000.0 << 20.0 << 100.0;
    QTest::newRow("maximum-site-narrow-neck") << QGeoCoordinate(85, 20) << 15.0 << 2000.0 << 0.1 << 0.0;
}

void CaacFigureEightFenceGeometryTest::_clearance()
{
    QFETCH(QGeoCoordinate, pointA);
    QFETCH(double, bearing);
    QFETCH(double, separation);
    QFETCH(double, width);
    QFETCH(double, margin);
    const auto pointB = QGCGeo::geodesicDestination(pointA, bearing, separation);
    const double siteBearing = QGCGeo::geodesicAzimuth(pointA, pointB);
    const auto origin = QGCGeo::geodesicDestination(pointA, siteBearing, separation / 2);
    const auto boundary = CaacFigureEightFenceGeometry::boundary(pointA, pointB, width, margin);
    QVERIFY(boundary.size() >= 3);
    QVERIFY(boundary.size() <= 62);
    QVERIFY(boundary.first() != boundary.last());

    QPolygonF localBoundary;
    QPolygonF roundedBoundary;
    const double radius = separation / 4;
    const double requestedRadius = radius + width + margin;
    for (const auto& vertex : boundary) {
        QVERIFY(vertex.isValid());
        QVERIFY(CaacFigureEightFenceGeometry::contains(pointA, pointB, width, margin, vertex));
        localBoundary.append(toLocal(origin, siteBearing, vertex));
        const QGeoCoordinate rounded(std::round(vertex.latitude() * 1e7) / 1e7,
                                     std::round(vertex.longitude() * 1e7) / 1e7);
        roundedBoundary.append(toLocal(origin, siteBearing, rounded));
    }
    for (qsizetype i = 0; i < localBoundary.size(); ++i) {
        const auto& start = localBoundary[i];
        const auto& end = localBoundary[(i + 1) % localBoundary.size()];
        const QPointF centre((start.x() + end.x()) / 2 < 0 ? -radius : radius, 0);
        // An inscribed polygon cuts into the requested margin between vertices.
        QVERIFY(distanceToSegment(centre, start, end) >= requestedRadius + 0.0299);
    }
    for (const double centre : {-radius, radius}) {
        for (int sample = 0; sample < 360; ++sample) {
            const double angle = sample / DEGREES_PER_RADIAN;
            const QPointF point(centre + requestedRadius * std::cos(angle), requestedRadius * std::sin(angle));
            QVERIFY(roundedBoundary.containsPoint(point, Qt::OddEvenFill));
            QVERIFY(CaacFigureEightFenceGeometry::contains(pointA, pointB, width, margin,
                                                           fromLocal(origin, siteBearing, point.x(), point.y())));
        }
    }
    QVERIFY(!CaacFigureEightFenceGeometry::contains(
        pointA, pointB, width, margin, fromLocal(origin, siteBearing, radius + requestedRadius * 1.02 + 0.1, 0)));
}

void CaacFigureEightFenceGeometryTest::_filledInteriorAndConcaveNeck()
{
    const QGeoCoordinate pointA(30, 120);
    const auto pointB = QGCGeo::geodesicDestination(pointA, 90, 24);
    const auto origin = QGCGeo::geodesicDestination(pointA, 90, 12);
    // Circle interiors are allowed, while the empty space above the neck remains outside.
    for (const double centre : {-6.0, 6.0}) {
        QVERIFY(CaacFigureEightFenceGeometry::contains(pointA, pointB, 0.1, 0, fromLocal(origin, 90, centre, 0)));
        QVERIFY(CaacFigureEightFenceGeometry::contains(pointA, pointB, 0.1, 0, fromLocal(origin, 90, centre, 3)));
    }
    QVERIFY(CaacFigureEightFenceGeometry::contains(pointA, pointB, 0.1, 0, origin));
    QVERIFY(!CaacFigureEightFenceGeometry::contains(pointA, pointB, 0.1, 0, fromLocal(origin, 90, 0, 3)));
}

void CaacFigureEightFenceGeometryTest::_widthAndMargin()
{
    const QGeoCoordinate pointA(30, 120);
    const auto pointB = QGCGeo::geodesicDestination(pointA, 90, 24);
    const auto origin = QGCGeo::geodesicDestination(pointA, 90, 12);
    const auto beyondCorridor = fromLocal(origin, 90, -6, 8.2);
    QVERIFY(!CaacFigureEightFenceGeometry::contains(pointA, pointB, 1.2, 0, beyondCorridor));
    QVERIFY(CaacFigureEightFenceGeometry::contains(pointA, pointB, 1.2, 3, beyondCorridor));
    QVERIFY(CaacFigureEightFenceGeometry::contains(pointA, pointB, 3, 0, beyondCorridor));
    QCOMPARE(CaacFigureEightFenceGeometry::boundary(pointA, pointB, 1.2, 3),
             CaacFigureEightFenceGeometry::boundary(pointA, pointB, 3.2, 1));
}

void CaacFigureEightFenceGeometryTest::_invalid_data()
{
    QTest::addColumn<QGeoCoordinate>("pointA");
    QTest::addColumn<QGeoCoordinate>("pointB");
    QTest::addColumn<double>("width");
    QTest::addColumn<double>("margin");
    const QGeoCoordinate pointA(30, 120);
    const auto pointB = QGCGeo::geodesicDestination(pointA, 90, 24);
    const double nan = std::numeric_limits<double>::quiet_NaN();
    QTest::newRow("missing-A") << QGeoCoordinate() << pointB << 1.2 << 3.0;
    QTest::newRow("missing-B") << pointA << QGeoCoordinate() << 1.2 << 3.0;
    QTest::newRow("coincident") << pointA << pointA << 1.2 << 3.0;
    QTest::newRow("too-small") << pointA << QGCGeo::geodesicDestination(pointA, 90, 3.9) << 1.2 << 3.0;
    QTest::newRow("too-large") << pointA << QGCGeo::geodesicDestination(pointA, 90, 2001) << 1.2 << 3.0;
    QTest::newRow("zero-width") << pointA << pointB << 0.0 << 3.0;
    QTest::newRow("wide-corridor") << pointA << pointB << 20.1 << 3.0;
    QTest::newRow("nan-width") << pointA << pointB << nan << 3.0;
    QTest::newRow("negative-margin") << pointA << pointB << 1.2 << -0.1;
    QTest::newRow("large-margin") << pointA << pointB << 1.2 << 100.1;
    QTest::newRow("nan-margin") << pointA << pointB << 1.2 << nan;
    QTest::newRow("infinite-margin") << pointA << pointB << 1.2 << std::numeric_limits<double>::infinity();
}

void CaacFigureEightFenceGeometryTest::_invalid()
{
    QFETCH(QGeoCoordinate, pointA);
    QFETCH(QGeoCoordinate, pointB);
    QFETCH(double, width);
    QFETCH(double, margin);
    QVERIFY(CaacFigureEightFenceGeometry::boundary(pointA, pointB, width, margin).isEmpty());
    QVERIFY(!CaacFigureEightFenceGeometry::contains(pointA, pointB, width, margin, QGeoCoordinate(30, 120)));
    const QGeoCoordinate validA(30, 120);
    const auto validB = QGCGeo::geodesicDestination(validA, 90, 24);
    QVERIFY(!CaacFigureEightFenceGeometry::contains(validA, validB, 1.2, 3, {}));
}

UT_REGISTER_TEST(CaacFigureEightFenceGeometryTest, TestLabel::Unit)

#include "CaacFigureEightFenceGeometryTest.moc"
