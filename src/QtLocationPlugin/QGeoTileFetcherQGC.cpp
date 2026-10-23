#include "QGeoTileFetcherQGC.h"

#include <algorithm>
#include <chrono>

#include <QtLocation/private/qgeotiledmappingmanagerengine_p.h>
#include <QtLocation/private/qgeotilespec_p.h>
#include <QtNetwork/QNetworkRequest>

#include "Gcj02TileReply.h"
#include "Gcj02TileReprojector.h"
#include "MapProvider.h"
#include "QGCLoggingCategory.h"
#include "QGCMapUrlEngine.h"
#include "QGeoMapReplyQGC.h"
#include "QGeoTiledMappingManagerEngineQGC.h"

QGC_LOGGING_CATEGORY(QGeoTileFetcherQGCLog, "QtLocationPlugin.QGeoTileFetcherQGC")

namespace {
// Keep pooled sockets warm across sparse tile/terrain fetches; Qt 6.11 otherwise reaps idle ones after 2 min.
constexpr int kConnectionCacheExpirySecs = 300;
constexpr std::chrono::seconds kTcpKeepAliveIdle{60};
constexpr std::chrono::seconds kTcpKeepAliveInterval{30};
constexpr int kTcpKeepAliveProbeCount = 3;
}  // namespace

QGeoTileFetcherQGC::QGeoTileFetcherQGC(QNetworkAccessManager* networkManager, const QVariantMap& parameters,
                                       QGeoTiledMappingManagerEngineQGC* parent)
    : QGeoTileFetcher(parent), m_networkManager(networkManager)
{
    if (!networkManager) {
        qCWarning(QGeoTileFetcherQGCLog) << "Network manager not available";
        return;
    }
    Q_UNUSED(parameters);

    qCDebug(QGeoTileFetcherQGCLog) << this;

    // TODO: Allow useragent override again
    /*if (parameters.contains(QStringLiteral("useragent"))) {
        setUserAgent(parameters.value(QStringLiteral("useragent")).toString().toLatin1());
    }*/
}

QGeoTileFetcherQGC::~QGeoTileFetcherQGC()
{
    qCDebug(QGeoTileFetcherQGCLog) << this;
}

QGeoTiledMapReply* QGeoTileFetcherQGC::getTileImage(const QGeoTileSpec& spec)
{
    const SharedMapProvider provider = UrlFactory::getMapProviderFromQtMapId(spec.mapId());
    if (!provider) {
        return nullptr;
    }

    /*if (spec.zoom() > provider->maximumZoomLevel() || spec.zoom() < provider->minimumZoomLevel()) {
        return nullptr;
    }*/

    const QNetworkRequest request = getNetworkRequest(spec.mapId(), spec.x(), spec.y(), spec.zoom());
    if (request.url().isEmpty()) {
        return nullptr;
    }

    QGeoTiledMapReplyQGC* tileImage = new QGeoTiledMapReplyQGC(m_networkManager, request, spec);
    if (!tileImage->init()) {
        tileImage->deleteLater();
        return nullptr;
    }

    return tileImage;
}

bool QGeoTileFetcherQGC::initialized() const
{
    return (m_networkManager != nullptr);
}

bool QGeoTileFetcherQGC::fetchingEnabled() const
{
    return initialized();
}

void QGeoTileFetcherQGC::timerEvent(QTimerEvent* event)
{
    QGeoTileFetcher::timerEvent(event);
}

void QGeoTileFetcherQGC::handleReply(QGeoTiledMapReply* reply, const QGeoTileSpec& spec)
{
    if (!reply) {
        return;
    }

    reply->deleteLater();

    if (!initialized()) {
        return;
    }

    if (reply->error() == QGeoTiledMapReply::NoError) {
        emit tileFinished(spec, reply->mapImageData(), reply->mapImageFormat());
    } else {
        emit tileError(spec, reply->errorString());
    }
}

QNetworkRequest QGeoTileFetcherQGC::getNetworkRequest(int mapId, int x, int y, int zoom)
{
    const SharedMapProvider mapProvider = UrlFactory::getMapProviderFromQtMapId(mapId);
    if (!mapProvider) {
        return QNetworkRequest();
    }

    QNetworkRequest request;
    if (zoom > mapProvider->maximumNativeZoom() && zoom <= QGC_MAX_MAP_ZOOM) {
        const int difference = zoom - mapProvider->maximumNativeZoom();
        request.setUrl(mapProvider->getTileURL(x >> difference, y >> difference, mapProvider->maximumNativeZoom()));
    } else {
        request.setUrl(mapProvider->getTileURL(x, y, zoom));
    }
    request.setPriority(QNetworkRequest::NormalPriority);
    request.setTransferTimeout(10000);
    // request.setOriginatingObject(this);

    // Headers
    request.setRawHeader(QByteArrayLiteral("Accept"), QByteArrayLiteral("*/*"));
    request.setHeader(QNetworkRequest::UserAgentHeader, s_userAgent);
    const QByteArray referrer = mapProvider->getReferrer().toUtf8();
    if (!referrer.isEmpty()) {
        request.setRawHeader(QByteArrayLiteral("Referer"), referrer);
    }
    const QByteArray token = mapProvider->getToken();
    if (!token.isEmpty()) {
        request.setRawHeader(QByteArrayLiteral("User-Token"), token);
    }
    request.setRawHeader(QByteArrayLiteral("Connection"), QByteArrayLiteral("keep-alive"));
    request.setAttribute(QNetworkRequest::ConnectionCacheExpiryTimeoutSecondsAttribute, kConnectionCacheExpirySecs);
    request.setTcpKeepAliveIdleTimeBeforeProbes(kTcpKeepAliveIdle);
    request.setTcpKeepAliveIntervalBetweenProbes(kTcpKeepAliveInterval);
    request.setTcpKeepAliveProbeCount(kTcpKeepAliveProbeCount);
    // request.setRawHeader(QByteArrayLiteral("Accept-Encoding"), QByteArrayLiteral("gzip, deflate, br"));

    // Attributes
    request.setAttribute(QNetworkRequest::CacheLoadControlAttribute, QNetworkRequest::PreferCache);
    request.setAttribute(QNetworkRequest::BackgroundRequestAttribute, true);
    request.setAttribute(QNetworkRequest::CacheSaveControlAttribute, true);
    request.setAttribute(QNetworkRequest::RedirectPolicyAttribute, QNetworkRequest::NoLessSafeRedirectPolicy);
    request.setAttribute(QNetworkRequest::Http2AllowedAttribute, true);
    request.setAttribute(QNetworkRequest::DoNotBufferUploadDataAttribute, false);
    // request.setAttribute(QNetworkRequest::AutoDeleteReplyOnFinishAttribute, true);

    return request;
}

QNetworkReply* QGeoTileFetcherQGC::getTileReply(QNetworkAccessManager* manager, const QNetworkRequest& request,
                                                int mapId, int x, int y, int zoom)
{
    if (!manager) {
        return nullptr;
    }
    const auto provider = UrlFactory::getMapProviderFromQtMapId(mapId);
    if (provider && provider->requiresGcj02Reprojection() &&
        (zoom > provider->maximumNativeZoom() || Gcj02TileReprojector::needsReprojection(x, y, zoom))) {
        return new Gcj02TileReply(manager, request, mapId, x, y, zoom, std::min(zoom, provider->maximumNativeZoom()));
    }
    return manager->get(request);
}
