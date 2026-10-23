#include "TileMathTest.h"

#include <cmath>
#include <numbers>

#include <QtCore/QtMath>

#include "Gcj02TileReprojector.h"
#include "TileMath.h"

using namespace TileMath;

void TileMathTest::_geoWorldOrigin()
{
    const QPointF world = geoToWorld(QGeoCoordinate(0, 0));
    QCOMPARE_LT(qAbs(world.x()), 1e-6);
    QCOMPARE_LT(qAbs(world.y()), 1e-6);

    // Antimeridian maps to the world edge
    const double half = worldSize() / 2.0;
    QCOMPARE_LT(qAbs(geoToWorld(QGeoCoordinate(0, 180)).x() - half), 1e-6);
    QCOMPARE_LT(qAbs(geoToWorld(QGeoCoordinate(0, -180)).x() + half), 1e-6);
}

void TileMathTest::_geoWorldRoundTrip()
{
    const QList<QGeoCoordinate> coords = {
        QGeoCoordinate(47.3977419, 8.5455938),  // Zurich
        QGeoCoordinate(-33.8688, 151.2093),     // Sydney
        QGeoCoordinate(64.15, -21.95),          // Reykjavik
        QGeoCoordinate(0.0, 0.0),
        QGeoCoordinate(-85.0, 179.9),
    };
    for (const QGeoCoordinate& coord : coords) {
        const QGeoCoordinate back = worldToGeo(geoToWorld(coord));
        QCOMPARE_LT(qAbs(back.latitude() - coord.latitude()), 1e-9);
        QCOMPARE_LT(qAbs(back.longitude() - coord.longitude()), 1e-9);
    }
}

void TileMathTest::_latitudeClamp()
{
    const QPointF clamped = geoToWorld(QGeoCoordinate(89.0, 10.0));
    const QPointF atLimit = geoToWorld(QGeoCoordinate(kMaxLatitude, 10.0));
    QCOMPARE(clamped, atLimit);

    // Mercator world is square: y at the latitude limit equals half the world size
    const double half = worldSize() / 2.0;
    QCOMPARE_LT(qAbs(atLimit.y() - half), 1e-3);
}

void TileMathTest::_metersPerPixel()
{
    // Canonical Web Mercator zoom 0 resolution at 256px tiles
    QCOMPARE_LT(qAbs(metersPerPixelAtZoom(0) - 156543.03392804097), 1e-6);
    QCOMPARE_LT(qAbs(metersPerPixelAtZoom(1) - (156543.03392804097 / 2.0)), 1e-6);
}

void TileMathTest::_tileZoom0()
{
    const TileKey key = tileForWorld(QPointF(0, 0), 0);
    QCOMPARE(key, (TileKey{0, 0, 0}));

    const double half = worldSize() / 2.0;
    const QPointF minCorner = tileMinCorner(key);
    QCOMPARE_LT(qAbs(minCorner.x() + half), 1e-6);
    QCOMPARE_LT(qAbs(minCorner.y() + half), 1e-6);
    QCOMPARE_LT(qAbs(tileSpanAtZoom(0) - worldSize()), 1e-6);
}

void TileMathTest::_tileZoom1Quadrants()
{
    const double quarter = worldSize() / 4.0;
    // Slippy scheme: (0,0) is north-west
    QCOMPARE(tileForWorld(QPointF(-quarter, quarter), 1), (TileKey{0, 0, 1}));   // NW
    QCOMPARE(tileForWorld(QPointF(quarter, quarter), 1), (TileKey{1, 0, 1}));    // NE
    QCOMPARE(tileForWorld(QPointF(-quarter, -quarter), 1), (TileKey{0, 1, 1}));  // SW
    QCOMPARE(tileForWorld(QPointF(quarter, -quarter), 1), (TileKey{1, 1, 1}));   // SE
}

void TileMathTest::_tileWorldRoundTrip()
{
    for (int zoom : {2, 8, 15}) {
        const TileKey key{(1 << zoom) / 3, (1 << zoom) / 5, zoom};
        const double span = tileSpanAtZoom(zoom);
        const QPointF tileCenter = tileMinCorner(key) + QPointF(span / 2.0, span / 2.0);
        QCOMPARE(tileForWorld(tileCenter, zoom), key);
    }
}

void TileMathTest::_tileEdgeClamp()
{
    const double half = worldSize() / 2.0;
    const int last = (1 << 5) - 1;
    QCOMPARE(tileForWorld(QPointF(half, -half), 5), (TileKey{last, last, 5}));
    QCOMPARE(tileForWorld(QPointF(-half - 1000.0, half + 1000.0), 5), (TileKey{0, 0, 5}));
}

void TileMathTest::_zoomForMetersPerPixel()
{
    QCOMPARE(zoomForMetersPerPixel(metersPerPixelAtZoom(5)), 5);
    // Slightly finer resolution requires the next zoom level
    QCOMPARE(zoomForMetersPerPixel(metersPerPixelAtZoom(5) * 0.99), 6);
    QCOMPARE(zoomForMetersPerPixel(1e9), kMinZoom);
    QCOMPARE(zoomForMetersPerPixel(0.0), kMaxZoom);
    QCOMPARE(zoomForMetersPerPixel(1e-9), kMaxZoom);
}

void TileMathTest::_mercatorScale()
{
    // 1 at the equator, 1/cos(lat) elsewhere, symmetric in hemisphere
    QCOMPARE_LT(qAbs(mercatorScale(0.0) - 1.0), 1e-12);
    QCOMPARE_LT(qAbs(mercatorScale(60.0) - 2.0), 1e-12);
    QCOMPARE_LT(qAbs(mercatorScale(-60.0) - 2.0), 1e-12);

    // Consistent with the projection: d(world.y)/d(ground meters) at 60 deg,
    // where ground meters are the meridian arc on the projection's own sphere
    const double dLatDeg = 0.0001;
    const double worldDy =
        geoToWorld(QGeoCoordinate(60.0 + dLatDeg, 0.0)).y() - geoToWorld(QGeoCoordinate(60.0, 0.0)).y();
    const double groundDy = qDegreesToRadians(dLatDeg) * kEarthRadius;
    QCOMPARE_LT(qAbs((worldDy / groundDy) - mercatorScale(60.0)), 1e-3);

    // Clamped at the mercator latitude limit: finite beyond it
    QCOMPARE(mercatorScale(89.0), mercatorScale(kMaxLatitude));
}

void TileMathTest::_isValidKey()
{
    QVERIFY(isValidKey(TileKey{0, 0, 0}));
    QVERIFY(isValidKey(TileKey{7, 0, 3}));
    QVERIFY(isValidKey(TileKey{(1 << kMaxZoom) - 1, (1 << kMaxZoom) - 1, kMaxZoom}));

    QVERIFY(!isValidKey(TileKey{0, 0, -1}));
    QVERIFY(!isValidKey(TileKey{0, 0, kMaxZoom + 1}));
    QVERIFY(!isValidKey(TileKey{-1, 0, 3}));
    QVERIFY(!isValidKey(TileKey{8, 0, 3}));
    QVERIFY(!isValidKey(TileKey{0, -1, 3}));
    QVERIFY(!isValidKey(TileKey{0, 8, 3}));
}

namespace {
QPointF slippyPixel(const QGeoCoordinate& coordinate, int zoom)
{
    const double side = std::ldexp(256.0, zoom);
    return {(coordinate.longitude() + 180.0) / 360.0 * side,
            (1.0 - std::asinh(std::tan(qDegreesToRadians(coordinate.latitude()))) / std::numbers::pi) * side / 2.0};
}

QImage markerTile(const QPoint& key, const QPointF& marker)
{
    QImage image(256, 256, QImage::Format_ARGB32_Premultiplied);
    image.fill(Qt::darkBlue);
    for (int row = 0; row < 256; ++row) {
        for (int column = 0; column < 256; ++column) {
            const QPointF delta = QPointF(key.x() * 256.0 + column + 0.5, key.y() * 256.0 + row + 0.5) - marker;
            if (QPointF::dotProduct(delta, delta) <= 16.0) {
                image.setPixel(column, row, qRgb(255, 0, 255));
            }
        }
    }
    return image;
}
}  // namespace

void TileMathTest::_gcjReferenceCoordinates()
{
    const QGeoCoordinate wgs(39.908823, 116.397470, 42.0);
    const QGeoCoordinate gcj = Gcj02TileReprojector::wgs84ToGcj02(wgs);
    QCOMPARE_LT(qAbs(gcj.latitude() - 39.91022649807321), 1e-9);
    QCOMPARE_LT(qAbs(gcj.longitude() - 116.4037135824225), 1e-9);
    QCOMPARE(gcj.altitude(), wgs.altitude());
    for (const auto& outside : {QGeoCoordinate(47.3977419, 8.5455938), QGeoCoordinate(-33.8688, 151.2093),
                                QGeoCoordinate(30.0, 72.0), QGeoCoordinate(56.0, 116.0)}) {
        QCOMPARE(Gcj02TileReprojector::wgs84ToGcj02(outside), outside);
    }
    QVERIFY(!Gcj02TileReprojector::wgs84ToGcj02(QGeoCoordinate()).isValid());
}

void TileMathTest::_gcjTileAlignment()
{
    const QGeoCoordinate wgs(39.908823, 116.397470);
    // Independent reference GCJ coordinate: a source marker must return to the WGS pixel.
    const QGeoCoordinate gcj(39.91022649807321, 116.4037135824225);
    for (const int zoom : {16, 23}) {
        const int sourceZoom = qMin(zoom, 20);
        const QPointF destination = slippyPixel(wgs, zoom);
        const int x = int(destination.x()) / 256;
        const int y = int(destination.y()) / 256;
        const auto plan = Gcj02TileReprojector::createPlan(x, y, zoom, sourceZoom);
        QVERIFY(plan.isValid());
        QVERIFY(Gcj02TileReprojector::needsReprojection(x, y, zoom));
        QCOMPARE(plan.sourceZoom, sourceZoom);
        QHash<QPoint, QImage> sources;
        const QPointF marker = slippyPixel(gcj, sourceZoom);
        for (const auto& key : plan.sourceTiles) {
            sources.insert(key, markerTile(key, marker));
        }
        const QImage corrected = Gcj02TileReprojector::reproject(plan, sources);
        QCOMPARE(corrected.size(), QSize(256, 256));
        const QPoint local(int(destination.x()) % 256, int(destination.y()) % 256);
        const QRgb pixel = corrected.pixel(local);
        QCOMPARE_GT(qRed(pixel), 240);
        QCOMPARE_GT(qBlue(pixel), 240);
        QCOMPARE_LT(qGreen(pixel), 10);
        QCOMPARE(qRed(corrected.pixel((local.x() + 64) % 256, local.y())), 0);
        sources.remove(plan.sourceTiles.first());
        QVERIFY(Gcj02TileReprojector::reproject(plan, sources).isNull());
    }

    const auto key = tileForWorld(geoToWorld(QGeoCoordinate(47.3977419, 8.5455938)), 13);
    QVERIFY(!Gcj02TileReprojector::needsReprojection(key.x, key.y, key.zoom));
    const auto plan = Gcj02TileReprojector::createPlan(key.x, key.y, key.zoom);
    QVERIFY(plan.isValid());
    QHash<QPoint, QImage> sources;
    for (const auto& source : plan.sourceTiles) {
        QImage image(256, 256, QImage::Format_ARGB32_Premultiplied);
        for (int row = 0; row < 256; ++row) {
            for (int column = 0; column < 256; ++column) {
                image.setPixel(column, row, qRgb(column, row, (source.x() + source.y()) % 256));
            }
        }
        sources.insert(source, image);
    }
    QCOMPARE(Gcj02TileReprojector::reproject(plan, sources), sources.value(QPoint(key.x, key.y)));
}

void TileMathTest::_gcjRegionBoundary()
{
    const auto key = tileForWorld(geoToWorld(QGeoCoordinate(30.0, 137.8347)), 23);
    const auto plan = Gcj02TileReprojector::createPlan(key.x, key.y, key.zoom);
    QVERIFY(plan.isValid());
    QVERIFY(Gcj02TileReprojector::needsReprojection(key.x, key.y, key.zoom));
    // The boundary has two distant source groups; coverage must not fetch their huge bounding rectangle.
    QCOMPARE_LT(plan.sourceTiles.size(), 16);
    bool shifted = false;
    bool unchanged = false;
    for (int column = 0; column < 256; ++column) {
        const double delta = plan.sourcePixels.at(128 * 256 + column).x() - (key.x * 256.0 + column);
        shifted |= qAbs(delta) > 100.0;
        unchanged |= qAbs(delta) < 1e-4;
    }
    QVERIFY(shifted);
    QVERIFY(unchanged);
    QHash<QPoint, QImage> sources;
    for (const auto& source : plan.sourceTiles) {
        QImage image(256, 256, QImage::Format_ARGB32_Premultiplied);
        image.fill(Qt::cyan);
        sources.insert(source, image);
    }
    const QImage corrected = Gcj02TileReprojector::reproject(plan, sources);
    QVERIFY(!corrected.isNull());
    for (int column = 0; column < 256; ++column) {
        QCOMPARE(corrected.pixel(column, 128), qRgb(0, 255, 255));
    }
    QVERIFY(!Gcj02TileReprojector::createPlan(-1, 0, 3).isValid());
    QVERIFY(!Gcj02TileReprojector::createPlan(0, 0, 24).isValid());
    QVERIFY(!Gcj02TileReprojector::createPlan(0, 0, 3, 4).isValid());
}

UT_REGISTER_TEST_LIGHTWEIGHT(TileMathTest, TestLabel::Unit)
