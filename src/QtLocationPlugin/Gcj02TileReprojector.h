#pragma once

#include <QtCore/QHash>
#include <QtCore/QList>
#include <QtCore/QPointF>
#include <QtCore/QVector>
#include <QtGui/QImage>
#include <QtPositioning/QGeoCoordinate>

/// Imagery is resampled into WGS84 tile space; vehicle and mission coordinates are never transformed.
class Gcj02TileReprojector
{
public:
    struct Plan
    {
        int sourceZoom = -1;
        QVector<QPointF> sourcePixels;
        QList<QPoint> sourceTiles;

        bool isValid() const
        {
            return sourceZoom >= 0 && sourceZoom <= 23 && sourcePixels.size() == 256 * 256 && !sourceTiles.isEmpty();
        }
    };

    static QGeoCoordinate wgs84ToGcj02(const QGeoCoordinate& coordinate);
    static bool needsReprojection(int x, int y, int zoom);
    static Plan createPlan(int x, int y, int zoom, int sourceZoom = -1);
    static QImage reproject(const Plan& plan, const QHash<QPoint, QImage>& sourceTiles);
};
