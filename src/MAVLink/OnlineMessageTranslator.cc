#include "OnlineMessageTranslator.h"

#include <QtCore/QCoreApplication>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>
#include <QtCore/QRegularExpression>
#include <QtCore/QTimer>
#include <QtCore/QUrlQuery>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>
#include <QtQml/QQmlEngine>

#include "AppSettings.h"
#include "Fact.h"
#include "QGCApplication.h"
#include "SettingsManager.h"
#include "VehicleMessageTranslator.h"

namespace {
struct ProtectedText
{
    QString key;
    QStringList values;
};

ProtectedText protect(const QString& text)
{
    // Preserve complete links, markup, entities, URLs, IDs, numeric values, and their units.
    static const QRegularExpression tokens(QStringLiteral(
        R"(<a\b[^>]*>.*?</a>|<(?:[^<>"']|"[^"]*"|'[^']*')+>|&(?:#x?[0-9a-fA-F]+|[A-Za-z]+);|(?:https?://|param://)\S+|(?i:\b(?:ArduPilot|Pixhawk|Pixracer|Navio2|QGroundControl|MAVLink|GStreamer|ExpressLRS|ELRS|DShot|OneShot|MultiShot|UAVCAN|DroneCAN)\b)|\b[A-Z][A-Z0-9_]{1,}\b|[-+]?\d+(?:\.\d+)?(?:[eE][-+]?\d+)?(?![0-9.])(?:\s*(?:cm/s|m/s|km/h|deg/s|rad/s|mAh|mA|MHz|kHz|Hz|kPa|Pa|cm|mm|km|kg|ms|us|µs|deg|rad|V|v|A|g|m|s|%|°)(?![A-Za-z]))?)"));
    ProtectedText result;
    qsizetype offset = 0;
    auto matches = tokens.globalMatch(text);
    while (matches.hasNext()) {
        const auto match = matches.next();
        result.key += QStringView(text).mid(offset, match.capturedStart() - offset);
        result.key += QStringLiteral("QGCV%1X").arg(result.values.size());
        result.values.append(match.captured());
        offset = match.capturedEnd();
    }
    result.key += QStringView(text).mid(offset);
    return result;
}

QString restore(const QString& translated, const QStringList& values)
{
    static const QRegularExpression token(QStringLiteral(R"(Q\s*G\s*C\s*V\s*(\d+)\s*X)"),
                                          QRegularExpression::CaseInsensitiveOption);
    QString result;
    qsizetype offset = 0;
    QSet<int> used;
    const auto append = [&result](const QString& piece) {
        const auto asciiWord = [](QChar value) {
            const ushort code = value.unicode();
            return (code >= 'a' && code <= 'z') || (code >= 'A' && code <= 'Z') || (code >= '0' && code <= '9') ||
                   code == '_';
        };
        if (!result.isEmpty() && !piece.isEmpty() && asciiWord(result.back()) && asciiWord(piece.front())) {
            result += QLatin1Char(' ');
        }
        result += piece;
    };
    auto matches = token.globalMatch(translated);
    while (matches.hasNext()) {
        const auto match = matches.next();
        const int index = match.captured(1).toInt();
        if (index >= values.size() || used.contains(index)) {
            return {};
        }
        append(translated.mid(offset, match.capturedStart() - offset));
        append(values[index]);
        used.insert(index);
        offset = match.capturedEnd();
    }
    append(translated.mid(offset));
    return used.size() == values.size() ? result : QString();
}
}  // namespace

OnlineMessageTranslator::OnlineMessageTranslator(QObject* parent)
    : QObject(parent)
    , _network(new QNetworkAccessManager(this))
{
    connect(qgcApp(), &QGCApplication::languageChanged, this, &OnlineMessageTranslator::_settingsChanged);
    if (!qgcApp()->runningUnitTests()) {
        auto* settings = SettingsManager::instance()->appSettings();
        if (settings) {
            connect(settings->onlineMessageTranslation(), &Fact::rawValueChanged, this,
                    &OnlineMessageTranslator::_settingsChanged);
        }
    }
}

OnlineMessageTranslator* OnlineMessageTranslator::instance()
{
    static auto* translator = new OnlineMessageTranslator(QCoreApplication::instance());
    return translator;
}

OnlineMessageTranslator* OnlineMessageTranslator::create(QQmlEngine*, QJSEngine*)
{
    auto* translator = instance();
    QQmlEngine::setObjectOwnership(translator, QQmlEngine::CppOwnership);
    return translator;
}

bool OnlineMessageTranslator::_enabled() const
{
    if (qgcApp()->runningUnitTests()) {
        return _testEnabled;
    }
    auto* settings = SettingsManager::instance()->appSettings();
    return settings && settings->onlineMessageTranslation()->rawValue().toBool() &&
           qgcApp()->getCurrentLanguage().language() == QLocale::Chinese;
}

QString OnlineMessageTranslator::displayText(const QString& source, int revision)
{
    Q_UNUSED(revision);
    const QString translated = VehicleMessageTranslator::translate(source);
    if (translated != VehicleMessageTranslator::translateLocal(source)) {
        return translated + QStringLiteral("<br/><small>在线翻译 · 原文：") + source.toHtmlEscaped() +
               QStringLiteral("</small>");
    }
    return translated;
}

QString OnlineMessageTranslator::parameterText(const QString& source, int revision)
{
    Q_UNUSED(revision);
    return VehicleMessageTranslator::translate(source);
}

QStringList OnlineMessageTranslator::parameterOptions(const QStringList& options, int revision)
{
    QStringList translated;
    for (const auto& option : options) {
        translated.append(parameterText(option, revision));
    }
    return translated;
}

QString OnlineMessageTranslator::translateUnknown(const QString& source)
{
    if (!_enabled() || source.isEmpty()) {
        return source;
    }
    const auto text = protect(source);
    if (text.key.toUtf8().size() > 450) {
        // Public-service requests have a small byte limit. Split only at whitespace, keeping tokens whole.
        QStringList chunks;
        QString chunk;
        static const QRegularExpression words(QStringLiteral(R"(\s*\S+)"));
        auto matches = words.globalMatch(text.key);
        while (matches.hasNext()) {
            const QString word = matches.next().captured();
            if (!chunk.isEmpty() && (chunk + word).toUtf8().size() > 400) {
                chunks.append(chunk);
                chunk.clear();
            }
            chunk += word;
        }
        chunk += text.key.mid(text.key.trimmed().size() + text.key.indexOf(text.key.trimmed()));
        chunks.append(chunk);
        if (chunks.size() == 1) {
            return source;
        }
        QString translated;
        for (const auto& part : chunks) {
            translated += translateUnknown(part);
        }
        const QString restored = restore(translated, text.values);
        return restored.isEmpty() ? source : restored;
    }
    if (const auto* cached = _cache.object(text.key)) {
        const QString translated = restore(*cached, text.values);
        return translated.isEmpty() ? source : translated;
    }
    QString prose = text.key;
    prose.remove(QRegularExpression(QStringLiteral("QGCV[0-9]+X")));
    static const QRegularExpression englishWord(QStringLiteral("[A-Za-z][a-z]{2,}"));
    const qint64 now = QDateTime::currentMSecsSinceEpoch();
    if (!prose.contains(englishWord) || text.key.toUtf8().size() > 450 || _pending.size() >= 32 ||
        _pending.contains(text.key) || _failedUntil.value(text.key) > now || _backoffUntil > now) {
        return source;
    }
    _queue.enqueue(text.key);
    _pending.insert(text.key);
    QTimer::singleShot(0, this, &OnlineMessageTranslator::_startNext);
    return source;
}

void OnlineMessageTranslator::_settingsChanged()
{
    if (!_enabled()) {
        _queue.clear();
        _pending.clear();
        if (_active) {
            _active->abort();
        }
    }
    ++_revision;
    emit translationsChanged();
}

void OnlineMessageTranslator::_startNext()
{
    if (_active || _queue.isEmpty() || !_enabled()) {
        return;
    }
    const qint64 delay = _nextRequestAt - QDateTime::currentMSecsSinceEpoch();
    if (delay > 0) {
        QTimer::singleShot(static_cast<int>(delay), this, &OnlineMessageTranslator::_startNext);
        return;
    }
    const QString key = _queue.dequeue();
    _nextRequestAt = QDateTime::currentMSecsSinceEpoch() + 1000;
    QUrl url = _endpoint;
    QUrlQuery query;
    query.addQueryItem(QStringLiteral("q"), key);
    query.addQueryItem(QStringLiteral("langpair"), QStringLiteral("en|zh-CN"));
    url.setQuery(query);
    QNetworkRequest request(url);
    request.setTransferTimeout(_requestTimeoutMs);
    request.setAttribute(QNetworkRequest::RedirectPolicyAttribute, QNetworkRequest::SameOriginRedirectPolicy);
    request.setRawHeader("User-Agent", "QGroundControl-message-translation");
    _active = _network->get(request);
    auto* reply = _active;
    QTimer::singleShot(_requestTimeoutMs, reply, [reply] {
        if (!reply->isFinished()) {
            reply->abort();
        }
    });
    connect(reply, &QNetworkReply::readyRead, this, [reply] {
        if (reply->bytesAvailable() > 65536) {
            reply->abort();
        }
    });
    connect(reply, &QNetworkReply::finished, this, [this, reply, key] { _finish(reply, key); });
}

void OnlineMessageTranslator::_finish(QNetworkReply* reply, const QString& key)
{
    _active = nullptr;
    _pending.remove(key);
    const QByteArray body = reply->error() == QNetworkReply::NoError ? reply->readAll() : QByteArray();
    const auto json = QJsonDocument::fromJson(body).object();
    QString translated = json.value(QStringLiteral("responseData"))
                             .toObject()
                             .value(QStringLiteral("translatedText"))
                             .toString()
                             .trimmed();
    // Validate token identity and count before accepting public-service output. Never accept remote markup.
    static const QRegularExpression markup(QStringLiteral("[<>&]"));
    static const QRegularExpression chinese(QStringLiteral("[\\x{3400}-\\x{9fff}]"));
    QStringList placeholders;
    static const QRegularExpression placeholder(QStringLiteral("QGCV[0-9]+X"));
    auto matches = placeholder.globalMatch(key);
    while (matches.hasNext()) {
        placeholders.append(matches.next().captured());
    }
    QString prose = translated;
    prose.remove(
        QRegularExpression(QStringLiteral(R"(Q\s*G\s*C\s*V\s*\d+\s*X)"), QRegularExpression::CaseInsensitiveOption));
    static const QRegularExpression addedNumber(QStringLiteral("[0-9]"));
    const bool accepted = _enabled() && reply->error() == QNetworkReply::NoError &&
                          json.value(QStringLiteral("responseStatus")).toInt() == 200 && translated.size() <= 4096 &&
                          !translated.contains(markup) && translated.contains(chinese) &&
                          !prose.contains(addedNumber) && !restore(translated, placeholders).isEmpty();
    if (accepted) {
        _cache.insert(key, new QString(translated));
        ++_revision;
        emit translationsChanged();
    } else if (_enabled()) {
        const qint64 now = QDateTime::currentMSecsSinceEpoch();
        if (_failedUntil.size() >= 512) {
            _failedUntil.clear();
        }
        _failedUntil.insert(key, now + 300000);
        if (reply->error() != QNetworkReply::NoError || json.value(QStringLiteral("responseStatus")).toInt() != 200) {
            _backoffUntil = now + 120000;
            _queue.clear();
            _pending.clear();
        }
    }
    reply->deleteLater();
    QTimer::singleShot(1000, this, &OnlineMessageTranslator::_startNext);
}
