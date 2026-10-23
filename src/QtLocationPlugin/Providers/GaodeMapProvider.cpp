#include "GaodeMapProvider.h"

GaodeMapProvider::GaodeMapProvider(const QString& mapName, int style, MapStyle mapStyle)
    : MapProvider(mapName, QStringLiteral("https://www.amap.com/"), QStringLiteral("png"), QGC_AVERAGE_TILE_SIZE,
                  mapStyle)
    , _style(style)
{}

QString GaodeMapProvider::_getURL(int x, int y, int zoom) const
{
    if (zoom < 0 || zoom > 20 || x < 0 || y < 0 || x >= (1 << zoom) || y >= (1 << zoom)) {
        return {};
    }
    return QStringLiteral(
               "https://webst0%1.is.autonavi.com/appmaptile?style=%2&lang=zh_cn&size=1&scale=1&x=%3&y=%4&z=%5")
        .arg(_getServerNum(x, y, 4) + 1)
        .arg(_style)
        .arg(x)
        .arg(y)
        .arg(zoom);
}
