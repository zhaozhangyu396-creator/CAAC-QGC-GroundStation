#include "CaacExamAudio.h"

#include <algorithm>
#include <cmath>

#include <QtCore/QFile>
#include <QtCore/QScopedValueRollback>
#include <QtCore/QUrl>
#include <QtMultimedia/QSoundEffect>

#include "AppMessages.h"
#include "AppSettings.h"
#include "Fact.h"
#include "QGCLoggingCategory.h"
#include "SettingsManager.h"

QGC_LOGGING_CATEGORY(CaacExamAudioLog, "CAAC.ExamAudio")

namespace {
constexpr qint64 WARNING_EXPIRY_MS = 3500;
constexpr qint64 PHASE_CUE_EXPIRY_MS = 10000;
}  // namespace

CaacExamAudio::CaacExamAudio(QObject* parent)
    : QObject(parent)
{}

int CaacExamAudio::_priority(const QString& id)
{
    if (id == QStringLiteral("completed") || id == QStringLiteral("failed")) {
        return 4;
    }
    if (id == QStringLiteral("spin_completed") || id == QStringLiteral("stopped") ||
        id == QStringLiteral("telemetry") || id == QStringLiteral("time_up") || id == QStringLiteral("wind") ||
        id == QStringLiteral("spin_too_fast") || id == QStringLiteral("spin_timeout") || id == QStringLiteral("area") ||
        id == QStringLiteral("altitude") || id == QStringLiteral("heading") || id == QStringLiteral("speed_high") ||
        id == QStringLiteral("speed_low")) {
        return 3;
    }
    if (id == QStringLiteral("ready") || id == QStringLiteral("positioning") || id == QStringLiteral("spin") ||
        id == QStringLiteral("spin_reverse") || id == QStringLiteral("figure_eight") || id == QStringLiteral("retry")) {
        return 2;
    }
    return 1;
}

bool CaacExamAudio::_cueExpired(const QString& id, qint64 elapsed)
{
    const int priority = _priority(id);
    return priority <= 2 && elapsed > (priority == 1 ? WARNING_EXPIRY_MS : PHASE_CUE_EXPIRY_MS);
}

bool CaacExamAudio::_ensureSettings()
{
    if (_volumeFact && _mutedFact) {
        return true;
    }
    auto* settings = SettingsManager::instance()->appSettings();
    if (!settings) {
        return false;
    }
    _volumeFact = settings->audioVolume();
    _mutedFact = settings->audioMuted();
    if (!_volumeFact || !_mutedFact) {
        return false;
    }
    connect(_volumeFact, &Fact::valueChanged, this, &CaacExamAudio::_syncVolume);
    connect(_mutedFact, &Fact::valueChanged, this, &CaacExamAudio::_syncVolume);
    return true;
}

double CaacExamAudio::_volume() const
{
    if (!_volumeFact || !_mutedFact || _mutedFact->rawValue().toBool()) {
        return 0;
    }
    const double volume = _volumeFact->rawValue().toDouble();
    return std::isfinite(volume) ? std::clamp(volume / 100.0, 0.0, 1.0) : 0;
}

void CaacExamAudio::_syncVolume()
{
    if (!_effect) {
        return;
    }
    const double volume = _volume();
    _effect->setVolume(static_cast<float>(volume));
    if (volume <= 0) {
        stop();
    }
}

void CaacExamAudio::stop()
{
    const QScopedValueRollback changing(_changingCue, true);
    _pending = false;
    _clearQueuedCue();
    _currentCueSince.invalidate();
    _currentId.clear();
    _currentText.clear();
    if (_effect) {
        _effect->stop();
    }
}

void CaacExamAudio::_playLoaded()
{
    if (_changingCue || !_pending || !_effect || _effect->status() != QSoundEffect::Ready) {
        return;
    }
    _pending = false;
    // Results remain valid across device initialization; obsolete phase cues can expire.
    if (_volume() <= 0 || (_currentCueSince.isValid() && _cueExpired(_currentId, _currentCueSince.elapsed()))) {
        _playQueuedCue();
        return;
    }
    _syncVolume();
    _effect->play();
}

void CaacExamAudio::_clearQueuedCue()
{
    _queuedCues.clear();
}

void CaacExamAudio::clearPendingWarnings()
{
    _queuedCues.removeIf([](const QueuedCue& cue) { return _priority(cue.id) <= 2; });
    if (_pending && _priority(_currentId) <= 2) {
        const QScopedValueRollback changing(_changingCue, true);
        _pending = false;
        _currentCueSince.invalidate();
        _currentId.clear();
        if (_effect) {
            _effect->stop();
        }
    }
}

bool CaacExamAudio::_deferCue(const QString& id, const QString& text, const QElapsedTimer& since)
{
    const int priority = _priority(id);
    const bool busy = _pending || (_effect && _effect->isPlaying());
    const bool replacesRetry = _currentId == QStringLiteral("retry") && id != QStringLiteral("retry") && priority == 2;
    const bool waitForCurrent = (priority <= 2 && !replacesRetry) || _priority(_currentId) == 4 ||
                                (id == QStringLiteral("failed") && _priority(_currentId) == 3);
    const bool queuedResult = !_queuedCues.isEmpty() && _priority(_queuedCues.first().id) > priority;
    if (!queuedResult && (!busy || !waitForCurrent)) {
        return false;
    }
    _queuedCues.removeIf(
        [&](const QueuedCue& cue) { return cue.id == id || (priority <= 2 && _priority(cue.id) <= 2); });
    const auto next = std::find_if(_queuedCues.begin(), _queuedCues.end(),
                                   [priority](const QueuedCue& cue) { return _priority(cue.id) < priority; });
    _queuedCues.insert(next, {id, text, since});
    return true;
}

std::optional<CaacExamAudio::QueuedCue> CaacExamAudio::_nextQueuedCue()
{
    while (!_queuedCues.isEmpty()) {
        auto cue = _queuedCues.takeFirst();
        if (cue.since.isValid() && !_cueExpired(cue.id, cue.since.elapsed())) {
            return cue;
        }
    }
    return std::nullopt;
}

void CaacExamAudio::_playQueuedCue()
{
    if (_changingCue || _pending || !_effect || _effect->isPlaying() || _volume() <= 0) {
        return;
    }
    if (const auto cue = _nextQueuedCue()) {
        _startCue(cue->id, cue->text, cue->since);
    }
}

void CaacExamAudio::_reportError(const QString& id, const QString& text)
{
    if (_reportedErrors.contains(id)) {
        return;
    }
    _reportedErrors.insert(id);
    qCWarning(CaacExamAudioLog) << "Unable to play bundled examination announcement:" << id << text;
}

void CaacExamAudio::play(const QString& id, const QString& text)
{
    static const QSet<QString> CUES{
        QStringLiteral("ready"),          QStringLiteral("spin"),        QStringLiteral("spin_reverse"),
        QStringLiteral("figure_eight"),   QStringLiteral("area"),        QStringLiteral("speed_high"),
        QStringLiteral("speed_low"),      QStringLiteral("time_up"),     QStringLiteral("heading"),
        QStringLiteral("altitude"),       QStringLiteral("completed"),   QStringLiteral("stopped"),
        QStringLiteral("telemetry"),      QStringLiteral("wind"),        QStringLiteral("spin_too_fast"),
        QStringLiteral("positioning"),    QStringLiteral("retry"),       QStringLiteral("failed"),
        QStringLiteral("spin_completed"), QStringLiteral("spin_timeout")};
    if (!CUES.contains(id) || QGC::runningUnitTests() || !_ensureSettings() || _volume() <= 0) {
        return;
    }
    if (_priority(id) >= 3) {
        _queuedCues.removeIf([](const QueuedCue& cue) { return _priority(cue.id) <= 2; });
    }
    QElapsedTimer warningSince;
    warningSince.start();
    if (_deferCue(id, text, warningSince)) {
        _playQueuedCue();
        return;
    }
    _queuedCues.removeIf([&id](const QueuedCue& cue) { return cue.id == id; });
    _startCue(id, text, warningSince);
}

void CaacExamAudio::_startCue(const QString& id, const QString& text, const QElapsedTimer& warningSince)
{
    const auto path = QStringLiteral(":/CAAC/ExamAudio/%1.wav").arg(id);
    {
        const QScopedValueRollback changing(_changingCue, true);
        _pending = false;
        if (_effect) {
            _effect->stop();
        }
        if (!QFile::exists(path)) {
            _currentId.clear();
            _reportError(id, text);
            return;
        }
        if (!_effect) {
            _effect = new QSoundEffect(this);
            _effect->setLoopCount(1);
            connect(_effect, &QSoundEffect::statusChanged, this, [this]() {
                if (_effect->status() == QSoundEffect::Error) {
                    _pending = false;
                    if (!_currentId.isEmpty()) {
                        _reportError(_currentId, _currentText);
                    }
                    QMetaObject::invokeMethod(this, &CaacExamAudio::_playQueuedCue, Qt::QueuedConnection);
                } else {
                    _playLoaded();
                }
            });
            connect(_effect, &QSoundEffect::playingChanged, this, [this]() {
                if (!_changingCue && !_effect->isPlaying()) {
                    QMetaObject::invokeMethod(this, &CaacExamAudio::_playQueuedCue, Qt::QueuedConnection);
                }
            });
        }
        _currentId = id;
        _currentText = text;
        _currentCueSince = warningSince;
        _pending = true;
        _syncVolume();
        _effect->setSource(QUrl(QStringLiteral("qrc:/CAAC/ExamAudio/%1.wav").arg(id)));
    }
    _playLoaded();
}
