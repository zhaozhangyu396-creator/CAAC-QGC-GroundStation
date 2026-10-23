#include "Gcj02TileReply.h"

#include <algorithm>
#include <cstring>
#include <utility>

#include <QtConcurrent/QtConcurrentRun>
#include <QtCore/QBuffer>
#include <QtCore/QCoreApplication>
#include <QtCore/QFutureWatcher>
#include <QtCore/QTimer>
#include <QtNetwork/QNetworkAccessManager>

#include "QGCNetworkHelper.h"
#include "QGeoTileFetcherQGC.h"

Gcj02TileReply::Gcj02TileReply(QNetworkAccessManager* manager, const QNetworkRequest& request, int mapId, int x, int y,
                               int zoom, int sourceZoom)
    : QNetworkReply(manager)
    , _manager(manager)
    , _mapId(mapId)
    , _x(x)
    , _y(y)
    , _zoom(zoom)
    , _sourceZoom(sourceZoom)
{
    setRequest(request);
    setUrl(request.url());
    setOperation(QNetworkAccessManager::GetOperation);
    open(QIODevice::ReadOnly);
    QTimer::singleShot(0, this, &Gcj02TileReply::_start);
}

Gcj02TileReply::~Gcj02TileReply()
{
    _cancelSources();
}

void Gcj02TileReply::_start()
{
    if (_terminal) {
        return;
    }
    auto* watcher = new QFutureWatcher<Gcj02TileReprojector::Plan>(this);
    connect(watcher, &QFutureWatcher<Gcj02TileReprojector::Plan>::finished, this, [this, watcher]() {
        watcher->deleteLater();
        if (_terminal) {
            return;
        }
        _plan = watcher->result();
        _fetchSources();
    });
    watcher->setFuture(QtConcurrent::run([x = _x, y = _y, zoom = _zoom, sourceZoom = _sourceZoom]() {
        return Gcj02TileReprojector::createPlan(x, y, zoom, sourceZoom);
    }));
}

void Gcj02TileReply::_fetchSources()
{
    if (!_manager || !_plan.isValid() || _plan.sourceTiles.size() > 64) {
        _fail(ProtocolInvalidOperationError,
              QCoreApplication::translate("Gcj02TileReply", "Invalid map tile coordinates."));
        return;
    }
    for (const auto& key : _plan.sourceTiles) {
        QNetworkRequest source = request();
        source.setUrl(QGeoTileFetcherQGC::getNetworkRequest(_mapId, key.x(), key.y(), _sourceZoom).url());
        if (source.url().isEmpty()) {
            _fail(ProtocolInvalidOperationError,
                  QCoreApplication::translate("Gcj02TileReply", "Invalid map tile coordinates."));
            return;
        }
        QNetworkReply* const reply = _manager->get(source);
        reply->setParent(this);
        _sourceReplies.insert(reply, key);
        QGCNetworkHelper::ignoreSslErrorsIfNeeded(reply);
        connect(reply, &QNetworkReply::finished, this, [this, reply]() { _sourceFinished(reply); });
        connect(reply, &QNetworkReply::sslErrors, this, &QNetworkReply::sslErrors);
        connect(reply, &QObject::destroyed, this, [this, reply]() {
            if (!_terminal && _sourceReplies.remove(reply)) {
                _fail(OperationCanceledError,
                      QCoreApplication::translate("Gcj02TileReply", "Map tile request cancelled."));
            }
        });
    }
}

void Gcj02TileReply::_sourceFinished(QNetworkReply* reply)
{
    if (_terminal || !_sourceReplies.contains(reply)) {
        return;
    }
    const QPoint key = _sourceReplies.take(reply);
    disconnect(reply, nullptr, this, nullptr);
    reply->deleteLater();
    if (reply->error() != NoError) {
        setAttribute(QNetworkRequest::HttpStatusCodeAttribute,
                     reply->attribute(QNetworkRequest::HttpStatusCodeAttribute));
        _fail(reply->error(), reply->errorString());
        return;
    }
    const int status = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    if (status < 200 || status >= 300) {
        setAttribute(QNetworkRequest::HttpStatusCodeAttribute, status);
        _fail(ContentAccessDenied, reply->attribute(QNetworkRequest::HttpReasonPhraseAttribute).toString());
        return;
    }
    const QImage image = QImage::fromData(reply->readAll());
    if (image.size() != QSize(256, 256)) {
        _fail(UnknownContentError,
              QCoreApplication::translate("Gcj02TileReply", "Map tile image could not be decoded."));
        return;
    }
    _sourceImages.insert(key, image.convertToFormat(QImage::Format_ARGB32_Premultiplied));
    if (_sourceReplies.isEmpty()) {
        _complete();
    }
}

void Gcj02TileReply::_cancelSources()
{
    const auto replies = _sourceReplies.keys();
    _sourceReplies.clear();
    for (auto* reply : replies) {
        disconnect(reply, nullptr, this, nullptr);
        reply->abort();
        reply->deleteLater();
    }
}

void Gcj02TileReply::_fail(NetworkError error, const QString& message)
{
    if (_terminal) {
        return;
    }
    _terminal = true;
    _cancelSources();
    setError(error, message);
    setFinished(true);
    const QPointer<Gcj02TileReply> guard(this);
    emit errorOccurred(error);
    if (guard) {
        emit finished();
    }
}

void Gcj02TileReply::_complete()
{
    auto* watcher = new QFutureWatcher<QByteArray>(this);
    connect(watcher, &QFutureWatcher<QByteArray>::finished, this, [this, watcher]() {
        watcher->deleteLater();
        if (_terminal) {
            return;
        }
        _bytes = watcher->result();
        if (_bytes.isEmpty()) {
            _fail(UnknownContentError, QCoreApplication::translate("Gcj02TileReply", "Map tile reprojection failed."));
            return;
        }
        _terminal = true;
        setHeader(QNetworkRequest::ContentTypeHeader, QStringLiteral("image/png"));
        setHeader(QNetworkRequest::ContentLengthHeader, _bytes.size());
        setAttribute(QNetworkRequest::HttpStatusCodeAttribute, 200);
        setFinished(true);
        const QPointer<Gcj02TileReply> guard(this);
        emit readyRead();
        if (guard) {
            emit finished();
        }
    });
    // Workers capture only image/value data; destroying or aborting the reply never waits for CPU work.
    watcher->setFuture(QtConcurrent::run([plan = std::move(_plan), images = std::move(_sourceImages)]() {
        const QImage corrected = Gcj02TileReprojector::reproject(plan, images);
        QByteArray bytes;
        QBuffer buffer(&bytes);
        if (corrected.isNull() || !buffer.open(QIODevice::WriteOnly) || !corrected.save(&buffer, "PNG")) {
            return QByteArray();
        }
        return bytes;
    }));
}

void Gcj02TileReply::abort()
{
    _fail(OperationCanceledError, QCoreApplication::translate("Gcj02TileReply", "Map tile request cancelled."));
}

qint64 Gcj02TileReply::bytesAvailable() const
{
    return _bytes.size() - _offset + QNetworkReply::bytesAvailable();
}

qint64 Gcj02TileReply::readData(char* data, qint64 maximum)
{
    const qint64 size = std::min<qint64>(maximum, _bytes.size() - _offset);
    if (size <= 0) {
        return _terminal ? -1 : 0;
    }
    std::memcpy(data, _bytes.constData() + _offset, size_t(size));
    _offset += size;
    return size;
}
