#pragma once

#include "MapProvider.h"

class GaodeMapProvider : public MapProvider
{
public:
    bool requiresGcj02Reprojection() const final { return true; }

    int maximumNativeZoom() const final { return 20; }

protected:
    GaodeMapProvider(const QString& mapName, int style, MapStyle mapStyle);

private:
    QString _getURL(int x, int y, int zoom) const final;

    const int _style;
};

class GaodeRoadMapProvider : public GaodeMapProvider
{
public:
    GaodeRoadMapProvider()
        : GaodeMapProvider(QStringLiteral("Gaode Road"), 7, MapProvider::StreetMap)
    {}
};

class GaodeSatelliteMapProvider : public GaodeMapProvider
{
public:
    GaodeSatelliteMapProvider()
        : GaodeMapProvider(QStringLiteral("Gaode Satellite"), 6, MapProvider::SatelliteMapDay)
    {}
};
