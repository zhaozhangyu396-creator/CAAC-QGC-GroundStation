#include "TileImageSourceTest.h"

#include <cmath>
#include <cstring>
#include <functional>
#include <memory>
#include <numbers>

#include <QtConcurrent/QtConcurrentRun>
#include <QtCore/QBuffer>
#include <QtCore/QFile>
#include <QtCore/QScopeGuard>
#include <QtCore/QSemaphore>
#include <QtCore/QThreadPool>
#include <QtCore/QTimer>
#include <QtCore/QUrlQuery>
#include <QtGui/QImage>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>
#include <QtTest/QSignalSpy>

#include "Gcj02TileReprojector.h"
#include "QGCMapUrlEngine.h"
#include "QGeoFileTileCacheQGC.h"
#include "QGeoTileFetcherQGC.h"
#include "TileImageSource.h"
#include "TileMath.h"
#include "UnitTestTileGenerator.h"

namespace {

// Any registered map provider works: the unit-test tile generator serves a
// deterministic placeholder image on every cache miss, so no network is hit
QString mapType()
{
    const QStringList elevationTypes = UrlFactory::getElevationProviderTypes();
    for (const QString& type : UrlFactory::getProviderTypes()) {
        if (!elevationTypes.contains(type)) {
            return type;
        }
    }
    return QString();
}

const TileMath::TileKey kKey{4302, 2867, 13};  // Zurich area

/// Serves a canned body (or error) asynchronously; held replies stay open
/// until finished manually
class CannedReply : public QNetworkReply
{
public:
    CannedReply(const QNetworkRequest& request, const QByteArray& body, NetworkError cannedError, QObject* parent,
                bool hold, int status = 200)
        : QNetworkReply(parent)
        , _body(body)
        , _cannedError(cannedError)
    {
        setRequest(request);
        setAttribute(QNetworkRequest::HttpStatusCodeAttribute, status);
        setOperation(QNetworkAccessManager::GetOperation);
        (void) open(ReadOnly);
        if (!hold) {
            QTimer::singleShot(0, this, &CannedReply::finishNow);
        }
    }

    void finishNow()
    {
        if (_cannedError != NoError) {
            setError(_cannedError, QStringLiteral("canned error"));
            emit errorOccurred(_cannedError);
        }
        setFinished(true);
        emit finished();
    }

    void abort() final { aborted = true; }

    bool aborted = false;

    qint64 bytesAvailable() const final { return (_body.size() - _offset) + QNetworkReply::bytesAvailable(); }

protected:
    qint64 readData(char* data, qint64 maxSize) final
    {
        const qint64 n = qMin<qint64>(maxSize, _body.size() - _offset);
        if (n <= 0) {
            return (_offset >= _body.size()) ? -1 : 0;
        }
        (void) memcpy(data, _body.constData() + _offset, static_cast<size_t>(n));
        _offset += n;
        return n;
    }

private:
    const QByteArray _body;
    const NetworkError _cannedError;
    qint64 _offset = 0;
};

/// Counts requests and serves the configured body/error
class MockNam : public QNetworkAccessManager
{
public:
    using QNetworkAccessManager::QNetworkAccessManager;

    QByteArray body;
    std::function<QByteArray(const QNetworkRequest&)> bodyFactory;
    QList<QNetworkRequest> requests;
    QNetworkReply::NetworkError error = QNetworkReply::NoError;
    int requestCount = 0;
    int httpStatus = 200;
    bool holdReplies = false;     ///< replies stay open until finished manually
    QList<CannedReply*> replies;  ///< creation order, only tracked while holding

protected:
    QNetworkReply* createRequest(Operation, const QNetworkRequest& request, QIODevice*) final
    {
        requestCount++;
        requests.append(request);
        const QByteArray response = bodyFactory ? bodyFactory(request) : body;
        CannedReply* const reply = new CannedReply(request, response, error, this, holdReplies, httpStatus);
        if (holdReplies) {
            replies.append(reply);
        }
        return reply;
    }
};

/// Any decodable image works as a network tile body
QByteArray pngBody()
{
    QImage image(32, 32, QImage::Format_RGB32);
    image.fill(Qt::darkGreen);
    QByteArray data;
    QBuffer buffer(&data);
    if (!buffer.open(QIODevice::WriteOnly) || !image.save(&buffer, "PNG")) {
        return QByteArray();
    }
    return data;
}

QPointF slippyPixel(const QGeoCoordinate& coordinate, int zoom)
{
    const double side = std::ldexp(256.0, zoom);
    return {
        (coordinate.longitude() + 180.0) / 360.0 * side,
        (1.0 - std::asinh(std::tan(coordinate.latitude() * std::numbers::pi / 180.0)) / std::numbers::pi) * side / 2.0};
}

QByteArray gcjMarkerBody(const QNetworkRequest& request)
{
    const QUrlQuery query(request.url());
    const int x = query.queryItemValue(QStringLiteral("x")).toInt();
    const int y = query.queryItemValue(QStringLiteral("y")).toInt();
    const int zoom = query.queryItemValue(QStringLiteral("z")).toInt();
    const QPointF marker = slippyPixel({39.91022649807321, 116.4037135824225}, zoom);
    QImage image(256, 256, QImage::Format_RGB32);
    image.fill(Qt::darkBlue);
    for (int row = 0; row < 256; ++row) {
        for (int column = 0; column < 256; ++column) {
            const QPointF delta = QPointF(x * 256.0 + column + 0.5, y * 256.0 + row + 0.5) - marker;
            if (QPointF::dotProduct(delta, delta) <= 16.0) {
                image.setPixel(column, row, qRgb(255, 0, 255));
            }
        }
    }
    QByteArray bytes;
    QBuffer buffer(&bytes);
    if (!buffer.open(QIODevice::WriteOnly) || !image.save(&buffer, "PNG")) {
        return {};
    }
    return bytes;
}

TileMath::TileKey beijingKey(int zoom)
{
    return TileMath::tileForWorld(TileMath::geoToWorld({39.908823, 116.397470}), zoom);
}
}  // namespace

void TileImageSourceTest::_tileDelivered()
{
    const QString type = mapType();
    QVERIFY2(!type.isEmpty(), "no non-elevation map provider registered");

    TileImageSource source(type);
    QSignalSpy readySpy(&source, &TileImageSource::tileImageReady);
    QSignalSpy failedSpy(&source, &TileImageSource::tileImageFailed);

    const int requestId = source.requestTileImage(kKey);
    QCOMPARE_GT(requestId, 0);
    QCOMPARE(source.pendingCount(), 1);

    QTRY_COMPARE_WITH_TIMEOUT(readySpy.count(), 1, 5000);
    QCOMPARE(failedSpy.count(), 0);
    QCOMPARE(source.pendingCount(), 0);

    QCOMPARE(readySpy.first().at(0).toInt(), requestId);
    const QImage image = readySpy.first().at(1).value<QImage>();
    QVERIFY(!image.isNull());
    QCOMPARE_GT(image.width(), 0);
}

void TileImageSourceTest::_cancelledRequestSilent()
{
    const QString type = mapType();
    QVERIFY2(!type.isEmpty(), "no non-elevation map provider registered");

    TileImageSource source(type);
    QSignalSpy readySpy(&source, &TileImageSource::tileImageReady);
    QSignalSpy failedSpy(&source, &TileImageSource::tileImageFailed);

    const int cancelledId = source.requestTileImage(kKey);
    source.cancelRequest(cancelledId);
    QCOMPARE(source.pendingCount(), 0);

    // The cache worker runs tasks in order: once this later request delivers,
    // the cancelled one has already run its course without signalling
    const int sentinelId = source.requestTileImage(TileMath::TileKey{kKey.x + 1, kKey.y, kKey.zoom});
    QTRY_COMPARE_WITH_TIMEOUT(readySpy.count(), 1, 5000);
    QCOMPARE(readySpy.first().at(0).toInt(), sentinelId);
    QCOMPARE(failedSpy.count(), 0);
}

void TileImageSourceTest::_multipleRequestsIndependent()
{
    const QString type = mapType();
    QVERIFY2(!type.isEmpty(), "no non-elevation map provider registered");

    TileImageSource source(type);
    QSignalSpy readySpy(&source, &TileImageSource::tileImageReady);

    const int first = source.requestTileImage(kKey);
    const int second = source.requestTileImage(TileMath::TileKey{kKey.x + 1, kKey.y, kKey.zoom});
    QCOMPARE_NE(first, second);

    QTRY_COMPARE_WITH_TIMEOUT(readySpy.count(), 2, 5000);

    QSet<int> deliveredIds;
    for (const QList<QVariant>& args : readySpy) {
        deliveredIds.insert(args.at(0).toInt());
    }
    QCOMPARE(deliveredIds, (QSet<int>{first, second}));
}

void TileImageSourceTest::_unknownProviderFails()
{
    // Both the provider lookup and the source itself deliberately warn once at construction
    expectLogMessage("QtLocationPlugin.QGCMapUrlEngine", QtWarningMsg,
                     QRegularExpression(QStringLiteral("type not found: \"No Such Provider\"")));
    expectLogMessage("GeoMap.TileImageSource", QtWarningMsg, QRegularExpression(QStringLiteral("unknown map type")));
    TileImageSource source(QStringLiteral("No Such Provider"));
    verifyExpectedLogMessage();
    verifyExpectedLogMessage();

    QSignalSpy readySpy(&source, &TileImageSource::tileImageReady);
    QSignalSpy failedSpy(&source, &TileImageSource::tileImageFailed);

    const int requestId = source.requestTileImage(kKey);
    QCOMPARE_GT(requestId, 0);
    QCOMPARE(source.pendingCount(), 1);  // failure is still delivered asynchronously

    QTRY_COMPARE_WITH_TIMEOUT(failedSpy.count(), 1, 5000);
    QCOMPARE(failedSpy.first().at(0).toInt(), requestId);
    QCOMPARE(readySpy.count(), 0);
    QCOMPARE(source.pendingCount(), 0);
}

void TileImageSourceTest::_networkFallbackDeliversAndCaches()
{
    const QString type = mapType();
    QVERIFY2(!type.isEmpty(), "no non-elevation map provider registered");
    const TileMath::TileKey key{kKey.x + 10, kKey.y, kKey.zoom};  // unique: write-back caches this tile

    MockNam nam;
    nam.body = pngBody();
    QVERIFY(!nam.body.isEmpty());
    TileImageSource source(type, nullptr, &nam);
    QSignalSpy readySpy(&source, &TileImageSource::tileImageReady);
    QSignalSpy failedSpy(&source, &TileImageSource::tileImageFailed);

    // Two forced generator misses: the first request must fall back to the
    // network; the second must be served by the write-back cached tile (the
    // DB hit precedes the generator, so the second miss stays unconsumed)
    UnitTestTileGenerator::setForcedMissCount(2);
    const auto guard = qScopeGuard([] { UnitTestTileGenerator::setForcedMissCount(0); });

    source.requestTileImage(key);
    QTRY_COMPARE_WITH_TIMEOUT(readySpy.count(), 1, 5000);
    QCOMPARE(nam.requestCount, 1);
    QVERIFY(!readySpy.first().at(1).value<QImage>().isNull());

    source.requestTileImage(key);
    QTRY_COMPARE_WITH_TIMEOUT(readySpy.count(), 2, 5000);
    QCOMPARE(nam.requestCount, 1);  // cached by write-back: no second network fetch
    QVERIFY(failedSpy.isEmpty());
}

void TileImageSourceTest::_networkErrorFails()
{
    const QString type = mapType();
    QVERIFY2(!type.isEmpty(), "no non-elevation map provider registered");
    const TileMath::TileKey key{kKey.x + 11, kKey.y, kKey.zoom};

    MockNam nam;
    nam.error = QNetworkReply::ContentNotFoundError;
    TileImageSource source(type, nullptr, &nam);
    QSignalSpy readySpy(&source, &TileImageSource::tileImageReady);
    QSignalSpy failedSpy(&source, &TileImageSource::tileImageFailed);

    UnitTestTileGenerator::setForcedMissCount(1);
    const auto guard = qScopeGuard([] { UnitTestTileGenerator::setForcedMissCount(0); });

    // Fetch failures must be visible without logging configuration
    expectLogMessage("GeoMap.TileImageSource", QtWarningMsg, QRegularExpression(QStringLiteral("network error")));
    const int requestId = source.requestTileImage(key);
    QTRY_COMPARE_WITH_TIMEOUT(failedSpy.count(), 1, 5000);
    verifyExpectedLogMessage();
    QCOMPARE(failedSpy.first().at(0).toInt(), requestId);
    QCOMPARE(nam.requestCount, 1);
    QVERIFY(readySpy.isEmpty());
}

void TileImageSourceTest::_networkEmptyBodyFails()
{
    const QString type = mapType();
    QVERIFY2(!type.isEmpty(), "no non-elevation map provider registered");
    const TileMath::TileKey key{kKey.x + 12, kKey.y, kKey.zoom};

    MockNam nam;  // NoError with an empty body: HTTP success carrying nothing
    TileImageSource source(type, nullptr, &nam);
    QSignalSpy readySpy(&source, &TileImageSource::tileImageReady);
    QSignalSpy failedSpy(&source, &TileImageSource::tileImageFailed);

    UnitTestTileGenerator::setForcedMissCount(1);
    const auto guard = qScopeGuard([] { UnitTestTileGenerator::setForcedMissCount(0); });

    expectLogMessage("GeoMap.TileImageSource", QtWarningMsg, QRegularExpression(QStringLiteral("empty body")));
    const int requestId = source.requestTileImage(key);
    QTRY_COMPARE_WITH_TIMEOUT(failedSpy.count(), 1, 5000);
    verifyExpectedLogMessage();
    QCOMPARE(failedSpy.first().at(0).toInt(), requestId);
    QVERIFY(readySpy.isEmpty());
}

void TileImageSourceTest::_networkGarbageBodyFailsAndNotCached()
{
    // An HTTP-200 non-image body (e.g. an error page) must fail the request
    // and must NOT be written back to the cache — a cached invalid tile would
    // fail every retry from then on
    const QString type = mapType();
    QVERIFY2(!type.isEmpty(), "no non-elevation map provider registered");
    const TileMath::TileKey key{kKey.x + 13, kKey.y, kKey.zoom};

    MockNam nam;
    nam.body = QByteArrayLiteral("this is not an image");
    TileImageSource source(type, nullptr, &nam);
    QSignalSpy readySpy(&source, &TileImageSource::tileImageReady);
    QSignalSpy failedSpy(&source, &TileImageSource::tileImageFailed);

    UnitTestTileGenerator::setForcedMissCount(2);
    const auto guard = qScopeGuard([] { UnitTestTileGenerator::setForcedMissCount(0); });

    expectLogMessage("GeoMap.TileImageSource", QtWarningMsg, QRegularExpression(QStringLiteral("failed to decode")));
    source.requestTileImage(key);
    QTRY_COMPARE_WITH_TIMEOUT(failedSpy.count(), 1, 5000);
    verifyExpectedLogMessage();
    QCOMPARE(nam.requestCount, 1);

    // Garbage was not cached: the retry misses again and re-fetches, this
    // time getting a valid image
    nam.body = pngBody();
    source.requestTileImage(key);
    QTRY_COMPARE_WITH_TIMEOUT(readySpy.count(), 1, 5000);
    QCOMPARE(nam.requestCount, 2);
}

void TileImageSourceTest::_bingPlaceholderNotDelivered()
{
    // Bing serves a placeholder image instead of an HTTP error where it has
    // no imagery: it decodes fine, but delivering or caching it would drape
    // it onto patches
    QFile file(QStringLiteral(":/res/BingNoTileBytes.dat"));
    QVERIFY(file.open(QFile::ReadOnly));
    const QByteArray placeholder = file.readAll();
    QVERIFY(!placeholder.isEmpty());

    const TileMath::TileKey key{kKey.x + 14, kKey.y, kKey.zoom};
    MockNam nam;
    nam.body = placeholder;
    TileImageSource source(QStringLiteral("Bing Hybrid"), nullptr, &nam);
    QSignalSpy readySpy(&source, &TileImageSource::tileImageReady);
    QSignalSpy failedSpy(&source, &TileImageSource::tileImageFailed);

    UnitTestTileGenerator::setForcedMissCount(1);
    const auto guard = qScopeGuard([] { UnitTestTileGenerator::setForcedMissCount(0); });

    expectLogMessage("GeoMap.TileImageSource", QtWarningMsg, QRegularExpression(QStringLiteral("no-tile placeholder")));
    source.requestTileImage(key);
    QTRY_COMPARE_WITH_TIMEOUT(failedSpy.count(), 1, 5000);
    verifyExpectedLogMessage();
    QCOMPARE(nam.requestCount, 1);
    QVERIFY(readySpy.isEmpty());
}

void TileImageSourceTest::_cachedUnusableTileFallsBackToNetwork()
{
    // Pre-seed the shared cache with an undecodable body: the cache-hit path
    // must treat it as a miss and recover via the network — failing would
    // wedge the tile on the same bytes every paced retry
    const QString type = mapType();
    QVERIFY2(!type.isEmpty(), "no non-elevation map provider registered");
    const TileMath::TileKey key{kKey.x + 15, kKey.y, kKey.zoom};
    // The cache worker runs tasks in order: the store lands before the lookup
    QGeoFileTileCacheQGC::cacheTile(type, key.x, key.y, key.zoom, QByteArrayLiteral("garbage"), QStringLiteral("png"));

    MockNam nam;
    nam.body = pngBody();
    QVERIFY(!nam.body.isEmpty());
    TileImageSource source(type, nullptr, &nam);
    QSignalSpy readySpy(&source, &TileImageSource::tileImageReady);
    QSignalSpy failedSpy(&source, &TileImageSource::tileImageFailed);

    const int requestId = source.requestTileImage(key);
    QTRY_COMPARE_WITH_TIMEOUT(readySpy.count(), 1, 5000);
    QCOMPARE(readySpy.first().at(0).toInt(), requestId);
    QCOMPARE(nam.requestCount, 1);  // cache hit rejected, recovered from the network
    QVERIFY(failedSpy.isEmpty());

    // Write-back is INSERT OR IGNORE, so the bad entry survives: a repeat
    // request pays another network fetch but still delivers
    source.requestTileImage(key);
    QTRY_COMPARE_WITH_TIMEOUT(readySpy.count(), 2, 5000);
    QCOMPARE(nam.requestCount, 2);
    QVERIFY(failedSpy.isEmpty());
}

void TileImageSourceTest::_cancelAbortsNetworkFetch()
{
    const QString type = mapType();
    QVERIFY2(!type.isEmpty(), "no non-elevation map provider registered");
    const TileMath::TileKey key{kKey.x + 16, kKey.y, kKey.zoom};

    MockNam nam;
    nam.body = pngBody();
    nam.holdReplies = true;
    TileImageSource source(type, nullptr, &nam);
    QSignalSpy readySpy(&source, &TileImageSource::tileImageReady);
    QSignalSpy failedSpy(&source, &TileImageSource::tileImageFailed);

    UnitTestTileGenerator::setForcedMissCount(1);
    const auto guard = qScopeGuard([] { UnitTestTileGenerator::setForcedMissCount(0); });

    const int requestId = source.requestTileImage(key);
    QTRY_COMPARE_WITH_TIMEOUT(nam.requestCount, 1, 5000);

    source.cancelRequest(requestId);
    QCOMPARE(source.pendingCount(), 0);
    source.cancelRequest(requestId);  // double cancel is a no-op

    // The aborted reply's late finished emission must stay silent
    nam.replies.at(0)->finishNow();
    QVERIFY_NO_SIGNAL_WAIT(readySpy, TestTimeout::shortMs());
    QVERIFY(failedSpy.isEmpty());
}

void TileImageSourceTest::_gaodeCorrectionDeliveredAndCached_data()
{
    QTest::addColumn<QString>("type");
    QTest::addColumn<int>("zoom");
    QTest::newRow("road") << QStringLiteral("Gaode Road") << 16;
    QTest::newRow("satellite") << QStringLiteral("Gaode Satellite") << 16;
    QTest::newRow("overzoom") << QStringLiteral("Gaode Road") << 23;
}

void TileImageSourceTest::_gaodeCorrectionDeliveredAndCached()
{
    QFETCH(QString, type);
    QFETCH(int, zoom);
    const auto key = beijingKey(zoom);
    const QPointF pixel = slippyPixel({39.908823, 116.397470}, zoom);
    const QPoint local(int(pixel.x()) % 256, int(pixel.y()) % 256);
    MockNam nam;
    nam.bodyFactory = gcjMarkerBody;
    TileImageSource source(type, nullptr, &nam);
    QSignalSpy readySpy(&source, &TileImageSource::tileImageReady);
    QSignalSpy failedSpy(&source, &TileImageSource::tileImageFailed);
    UnitTestTileGenerator::setForcedMissCount(2);
    const auto guard = qScopeGuard([] { UnitTestTileGenerator::setForcedMissCount(0); });

    source.requestTileImage(key);
    QTRY_COMPARE_WITH_TIMEOUT(readySpy.count(), 1, TestTimeout::mediumMs());
    const QImage online = readySpy.first().at(1).value<QImage>();
    QCOMPARE(online.size(), QSize(256, 256));
    QCOMPARE_GT(qRed(online.pixel(local)), 240);
    QCOMPARE_GT(qBlue(online.pixel(local)), 240);
    QCOMPARE_LT(qGreen(online.pixel(local)), 10);
    QCOMPARE(qRed(online.pixel((local.x() + 64) % 256, local.y())), 0);
    const int networkRequests = nam.requestCount;
    QCOMPARE_GT(networkRequests, 0);
    for (const auto& request : nam.requests) {
        QCOMPARE(QUrlQuery(request.url()).queryItemValue(QStringLiteral("z")).toInt(), qMin(zoom, 20));
        QCOMPARE(request.url().scheme(), QStringLiteral("https"));
    }

    source.requestTileImage(key);
    QTRY_COMPARE_WITH_TIMEOUT(readySpy.count(), 2, TestTimeout::mediumMs());
    QCOMPARE(nam.requestCount, networkRequests);
    QCOMPARE(readySpy.last().at(1).value<QImage>(), online);
    QVERIFY(failedSpy.isEmpty());
}

void TileImageSourceTest::_gaodeNonChinaPreservesBytes()
{
    const int mapId = UrlFactory::getQtMapIdFromProviderType(QStringLiteral("Gaode Road"));
    const QNetworkRequest request = QGeoTileFetcherQGC::getNetworkRequest(mapId, kKey.x, kKey.y, kKey.zoom);
    QVERIFY(request.url().isValid());
    MockNam nam;
    nam.body = pngBody();
    QNetworkReply* const reply = QGeoTileFetcherQGC::getTileReply(&nam, request, mapId, kKey.x, kKey.y, kKey.zoom);
    QVERIFY(reply);
    QSignalSpy finishedSpy(reply, &QNetworkReply::finished);
    QTRY_COMPARE_WITH_TIMEOUT(finishedSpy.count(), 1, TestTimeout::mediumMs());
    QCOMPARE(reply->error(), QNetworkReply::NoError);
    QCOMPARE(reply->readAll(), nam.body);
    QCOMPARE(nam.requestCount, 1);
}

void TileImageSourceTest::_gaodeFailedSourceNotCached_data()
{
    QTest::addColumn<int>("failure");
    QTest::newRow("network-failure") << 0;
    QTest::newRow("invalid-source-image") << 1;
    QTest::newRow("http-failure") << 2;
}

void TileImageSourceTest::_gaodeFailedSourceNotCached()
{
    QFETCH(int, failure);
    const auto key = beijingKey(17 + failure);
    MockNam nam;
    nam.error = failure == 0 ? QNetworkReply::ContentNotFoundError : QNetworkReply::NoError;
    nam.httpStatus = failure == 2 ? 403 : 200;
    nam.body = QByteArrayLiteral("not a source tile");
    if (failure == 2) {
        nam.bodyFactory = gcjMarkerBody;
    }
    TileImageSource source(QStringLiteral("Gaode Road"), nullptr, &nam);
    QSignalSpy readySpy(&source, &TileImageSource::tileImageReady);
    QSignalSpy failedSpy(&source, &TileImageSource::tileImageFailed);
    UnitTestTileGenerator::setForcedMissCount(2);
    const auto guard = qScopeGuard([] { UnitTestTileGenerator::setForcedMissCount(0); });

    expectLogMessage("GeoMap.TileImageSource", QtWarningMsg, QRegularExpression(QStringLiteral("network error")));
    source.requestTileImage(key);
    QTRY_COMPARE_WITH_TIMEOUT(failedSpy.count(), 1, TestTimeout::mediumMs());
    verifyExpectedLogMessage();
    QVERIFY(readySpy.isEmpty());
    const int failedRequests = nam.requestCount;

    nam.error = QNetworkReply::NoError;
    nam.httpStatus = 200;
    nam.bodyFactory = gcjMarkerBody;
    source.requestTileImage(key);
    QTRY_COMPARE_WITH_TIMEOUT(readySpy.count(), 1, TestTimeout::mediumMs());
    QCOMPARE_GT(nam.requestCount, failedRequests);
    QCOMPARE(failedSpy.count(), 1);
}

void TileImageSourceTest::_gaodeCancelAbortsAllSources()
{
    const auto key = beijingKey(20);
    MockNam nam;
    nam.bodyFactory = gcjMarkerBody;
    nam.holdReplies = true;
    TileImageSource source(QStringLiteral("Gaode Road"), nullptr, &nam);
    QSignalSpy readySpy(&source, &TileImageSource::tileImageReady);
    QSignalSpy failedSpy(&source, &TileImageSource::tileImageFailed);
    UnitTestTileGenerator::setForcedMissCount(1);
    const auto guard = qScopeGuard([] { UnitTestTileGenerator::setForcedMissCount(0); });

    const int requestId = source.requestTileImage(key);
    QTRY_VERIFY_WITH_TIMEOUT(!nam.replies.isEmpty(), TestTimeout::mediumMs());
    source.cancelRequest(requestId);
    QCOMPARE(source.pendingCount(), 0);
    for (auto* reply : nam.replies) {
        QVERIFY(reply->aborted);
        reply->finishNow();
    }
    QVERIFY_NO_SIGNAL_WAIT(readySpy, TestTimeout::shortMs());
    QVERIFY(failedSpy.isEmpty());
}

void TileImageSourceTest::_gaodeCancelDuringPlanning()
{
    auto* const pool = QThreadPool::globalInstance();
    const int previousThreads = pool->maxThreadCount();
    pool->setMaxThreadCount(1);
    const auto entered = std::make_shared<QSemaphore>();
    const auto release = std::make_shared<QSemaphore>();
    const auto blocker = QtConcurrent::run([entered, release]() {
        entered->release();
        release->acquire();
    });
    const auto guard = qScopeGuard([&]() {
        release->release();
        pool->setMaxThreadCount(previousThreads);
    });
    QVERIFY(entered->tryAcquire(1, TestTimeout::mediumMs()));

    const auto key = beijingKey(16);
    const int mapId = UrlFactory::getQtMapIdFromProviderType(QStringLiteral("Gaode Road"));
    auto request = QGeoTileFetcherQGC::getNetworkRequest(mapId, key.x, key.y, key.zoom);
    request.setAttribute(QNetworkRequest::User, QStringLiteral("offline-target-hash"));
    MockNam nam;
    nam.bodyFactory = gcjMarkerBody;
    QNetworkReply* const reply = QGeoTileFetcherQGC::getTileReply(&nam, request, mapId, key.x, key.y, key.zoom);
    QSignalSpy finishedSpy(reply, &QNetworkReply::finished);
    QSignalSpy readySpy(reply, &QIODevice::readyRead);
    // _start is queued; its CPU future cannot run until the held worker is released.
    QCoreApplication::processEvents();
    QCOMPARE(reply->request().attribute(QNetworkRequest::User).toString(), QStringLiteral("offline-target-hash"));
    reply->abort();
    QCOMPARE(finishedSpy.count(), 1);
    QCOMPARE(reply->error(), QNetworkReply::OperationCanceledError);
    QCOMPARE(nam.requestCount, 0);
    release->release();
    QVERIFY(pool->waitForDone(TestTimeout::mediumMs()));
    QVERIFY(blocker.isFinished());
    QCoreApplication::processEvents();
    QCOMPARE(finishedSpy.count(), 1);
    QCOMPARE(nam.requestCount, 0);
    QVERIFY(readySpy.isEmpty());
}

UT_REGISTER_TEST(TileImageSourceTest, TestLabel::Integration)
