#include "Gcj02TileReprojector.h"

#include <algorithm>
#include <cmath>
#include <numbers>

#include <QtCore/QSet>

namespace {
constexpr int TILE_SIZE = 256;
constexpr double MIN_LONGITUDE = 72.004;
constexpr double MAX_LONGITUDE = 137.8347;
constexpr double MIN_LATITUDE = 0.8293;
constexpr double MAX_LATITUDE = 55.8271;
constexpr double PI = std::numbers::pi;

bool validTile(int x, int y, int zoom)
{
    return zoom >= 0 && zoom <= 23 && x >= 0 && y >= 0 && x < (1 << zoom) && y < (1 << zoom);
}

double latitudeAtPixel(double pixel, double worldSize)
{
    return std::atan(std::sinh(PI * (1 - 2 * pixel / worldSize))) * 180 / PI;
}

QPointF sourcePixel(const QGeoCoordinate& coordinate, double worldSize)
{
    const double latitude = coordinate.latitude() * PI / 180;
    return {(coordinate.longitude() + 180) / 360 * worldSize,
            (1 - std::asinh(std::tan(latitude)) / PI) / 2 * worldSize};
}

QPoint sourceTile(qint64 x, qint64 y, int zoom)
{
    const qint64 side = qint64(1) << zoom;
    const qint64 tileX = static_cast<qint64>(std::floor(double(x) / TILE_SIZE));
    const qint64 tileY = std::clamp<qint64>(y / TILE_SIZE, 0, side - 1);
    return {int((tileX % side + side) % side), int(tileY)};
}
}  // namespace

QGeoCoordinate Gcj02TileReprojector::wgs84ToGcj02(const QGeoCoordinate& coordinate)
{
    const double longitude = coordinate.longitude();
    const double latitude = coordinate.latitude();
    if (!coordinate.isValid() || longitude < MIN_LONGITUDE || longitude > MAX_LONGITUDE || latitude < MIN_LATITUDE ||
        latitude > MAX_LATITUDE) {
        return coordinate;
    }
    const double x = longitude - 105;
    const double y = latitude - 35;
    double deltaLatitude = -100 + 2 * x + 3 * y + 0.2 * y * y + 0.1 * x * y + 0.2 * std::sqrt(std::abs(x));
    double deltaLongitude = 300 + x + 2 * y + 0.1 * x * x + 0.1 * x * y + 0.1 * std::sqrt(std::abs(x));
    const double common = (20 * std::sin(6 * x * PI) + 20 * std::sin(2 * x * PI)) * 2 / 3;
    deltaLatitude += common + (20 * std::sin(y * PI) + 40 * std::sin(y * PI / 3)) * 2 / 3 +
                     (160 * std::sin(y * PI / 12) + 320 * std::sin(y * PI / 30)) * 2 / 3;
    deltaLongitude += common + (20 * std::sin(x * PI) + 40 * std::sin(x * PI / 3)) * 2 / 3 +
                      (150 * std::sin(x * PI / 12) + 300 * std::sin(x * PI / 30)) * 2 / 3;
    constexpr double SEMI_MAJOR_AXIS = 6378245.0;
    constexpr double ECCENTRICITY_SQUARED = 0.00669342162296594323;
    const double radians = latitude * PI / 180;
    const double sine = std::sin(radians);
    const double magic = 1 - ECCENTRICITY_SQUARED * sine * sine;
    const double root = std::sqrt(magic);
    deltaLatitude *= 180 / ((SEMI_MAJOR_AXIS * (1 - ECCENTRICITY_SQUARED) / (magic * root)) * PI);
    deltaLongitude *= 180 / ((SEMI_MAJOR_AXIS / root) * std::cos(radians) * PI);
    return {latitude + deltaLatitude, longitude + deltaLongitude, coordinate.altitude()};
}

bool Gcj02TileReprojector::needsReprojection(int x, int y, int zoom)
{
    if (!validTile(x, y, zoom)) {
        return false;
    }
    const double side = std::ldexp(1.0, zoom);
    const double west = x / side * 360 - 180;
    const double east = (x + 1) / side * 360 - 180;
    const double north = latitudeAtPixel(y, side);
    const double south = latitudeAtPixel(y + 1, side);
    return east >= MIN_LONGITUDE && west <= MAX_LONGITUDE && north >= MIN_LATITUDE && south <= MAX_LATITUDE;
}

Gcj02TileReprojector::Plan Gcj02TileReprojector::createPlan(int x, int y, int zoom, int sourceZoom)
{
    Plan plan;
    if (!validTile(x, y, zoom)) {
        return plan;
    }
    sourceZoom = sourceZoom < 0 ? zoom : sourceZoom;
    if (sourceZoom > zoom || sourceZoom < 0) {
        return plan;
    }
    plan.sourceZoom = sourceZoom;
    plan.sourcePixels.reserve(TILE_SIZE * TILE_SIZE);
    QSet<QPoint> tiles;
    const double worldSize = std::ldexp(double(TILE_SIZE), zoom);
    const double sourceWorldSize = std::ldexp(double(TILE_SIZE), sourceZoom);
    for (int row = 0; row < TILE_SIZE; ++row) {
        const double latitude = latitudeAtPixel(double(y) * TILE_SIZE + row + 0.5, worldSize);
        for (int column = 0; column < TILE_SIZE; ++column) {
            const double longitude = (double(x) * TILE_SIZE + column + 0.5) / worldSize * 360 - 180;
            const QPointF pixel = sourcePixel(wgs84ToGcj02({latitude, longitude}), sourceWorldSize) - QPointF(0.5, 0.5);
            plan.sourcePixels.append(pixel);
            const auto left = qint64(std::floor(pixel.x()));
            const auto top = qint64(std::floor(pixel.y()));
            // Exact per-pixel coverage also handles discontinuities at the GCJ region boundary.
            for (int dy = 0; dy < 2; ++dy) {
                for (int dx = 0; dx < 2; ++dx) {
                    tiles.insert(sourceTile(left + dx, top + dy, sourceZoom));
                }
            }
        }
    }
    plan.sourceTiles = tiles.values();
    std::sort(plan.sourceTiles.begin(), plan.sourceTiles.end(),
              [](const QPoint& a, const QPoint& b) { return a.y() == b.y() ? a.x() < b.x() : a.y() < b.y(); });
    return plan;
}

QImage Gcj02TileReprojector::reproject(const Plan& plan, const QHash<QPoint, QImage>& sourceTiles)
{
    if (!plan.isValid()) {
        return {};
    }
    for (const auto& key : plan.sourceTiles) {
        const auto source = sourceTiles.constFind(key);
        if (source == sourceTiles.cend() || source->size() != QSize(TILE_SIZE, TILE_SIZE) ||
            source->format() != QImage::Format_ARGB32_Premultiplied) {
            return {};
        }
    }
    const qint64 maximum = std::ldexp(double(TILE_SIZE), plan.sourceZoom) - 1;
    const auto sample = [&](qint64 x, qint64 y) {
        y = std::clamp<qint64>(y, 0, maximum);
        const QPoint key = sourceTile(x, y, plan.sourceZoom);
        const auto source = sourceTiles.constFind(key);
        const int column = int((x % TILE_SIZE + TILE_SIZE) % TILE_SIZE);
        return reinterpret_cast<const QRgb*>(source->constScanLine(int(y % TILE_SIZE)))[column];
    };
    QImage result(TILE_SIZE, TILE_SIZE, QImage::Format_ARGB32_Premultiplied);
    for (int row = 0; row < TILE_SIZE; ++row) {
        auto* output = reinterpret_cast<QRgb*>(result.scanLine(row));
        for (int column = 0; column < TILE_SIZE; ++column) {
            const QPointF pixel = plan.sourcePixels.at(row * TILE_SIZE + column);
            const auto left = qint64(std::floor(pixel.x()));
            const auto top = qint64(std::floor(pixel.y()));
            const double dx = pixel.x() - left;
            const double dy = pixel.y() - top;
            const QRgb samples[]{sample(left, top), sample(left + 1, top), sample(left, top + 1),
                                 sample(left + 1, top + 1)};
            const auto channel = [&](int (*get)(QRgb)) {
                return int(std::lround((get(samples[0]) * (1 - dx) + get(samples[1]) * dx) * (1 - dy) +
                                       (get(samples[2]) * (1 - dx) + get(samples[3]) * dx) * dy));
            };
            output[column] = qRgba(channel(qRed), channel(qGreen), channel(qBlue), channel(qAlpha));
        }
    }
    return result;
}
