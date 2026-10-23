#pragma once

#include <QtCore/QPointer>
#include <QtNetwork/QNetworkReply>

#include "Gcj02TileReprojector.h"

class QNetworkAccessManager;

/// A normal tile reply carrying one corrected WGS84 PNG, assembled from GCJ source tiles.
class Gcj02TileReply final : public QNetworkReply
{
public:
    Gcj02TileReply(QNetworkAccessManager* manager, const QNetworkRequest& request, int mapId, int x, int y, int zoom,
                   int sourceZoom);
    ~Gcj02TileReply() final;
    void abort() final;

    bool isSequential() const final { return true; }

    qint64 bytesAvailable() const final;

protected:
    qint64 readData(char* data, qint64 maximum) final;

private:
    void _start();
    void _fetchSources();
    void _sourceFinished(QNetworkReply* reply);
    void _cancelSources();
    void _fail(NetworkError error, const QString& message);
    void _complete();

    QPointer<QNetworkAccessManager> _manager;
    int _mapId;
    int _x;
    int _y;
    int _zoom;
    int _sourceZoom;
    Gcj02TileReprojector::Plan _plan;
    QHash<QNetworkReply*, QPoint> _sourceReplies;
    QHash<QPoint, QImage> _sourceImages;
    QByteArray _bytes;
    qint64 _offset = 0;
    bool _terminal = false;
};
