#pragma once

#include <QtCore/QCache>
#include <QtCore/QDateTime>
#include <QtCore/QObject>
#include <QtCore/QQueue>
#include <QtCore/QSet>
#include <QtCore/QUrl>
#include <QtQmlIntegration/QtQmlIntegration>

class QNetworkAccessManager;
class QNetworkReply;
class QQmlEngine;
class QJSEngine;
class OnlineMessageTranslatorTest;

/// Optional asynchronous fallback. Protocol messages and their raw logs remain unchanged.
class OnlineMessageTranslator : public QObject
{
    Q_OBJECT
    QML_ELEMENT
    QML_SINGLETON
    Q_PROPERTY(int revision READ revision NOTIFY translationsChanged)

public:
    static OnlineMessageTranslator* instance();
    static OnlineMessageTranslator* create(QQmlEngine*, QJSEngine*);
    Q_INVOKABLE QString displayText(const QString& source, int revision);
    Q_INVOKABLE QString parameterText(const QString& source, int revision);
    Q_INVOKABLE QStringList parameterOptions(const QStringList& options, int revision);
    QString translateUnknown(const QString& source);

    int revision() const { return _revision; }

signals:
    void translationsChanged();

private:
    friend class OnlineMessageTranslatorTest;
    explicit OnlineMessageTranslator(QObject* parent = nullptr);
    bool _enabled() const;
    void _startNext();
    void _settingsChanged();
    void _finish(QNetworkReply* reply, const QString& key);

    QNetworkAccessManager* _network = nullptr;
    QUrl _endpoint{QStringLiteral("https://api.mymemory.translated.net/get")};
    QCache<QString, QString> _cache{512};
    QHash<QString, qint64> _failedUntil;
    QQueue<QString> _queue;
    QSet<QString> _pending;
    QNetworkReply* _active = nullptr;
    qint64 _backoffUntil = 0;
    qint64 _nextRequestAt = 0;
    bool _testEnabled = false;
    int _revision = 0;
    int _requestTimeoutMs = 5000;
};
