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
constexpr qint64 LOAD_EXPIRY_MS = 2000;
constexpr qint64 WARNING_EXPIRY_MS = 3500;
}  // namespace

CaacExamAudio::CaacExamAudio(QObject* parent)
    : QObject(parent)
{}

int CaacExamAudio::_priority(const QString& id)
{
    if (id == QStringLiteral("completed") || id == QStringLiteral("stopped") || id == QStringLiteral("telemetry") ||
        id == QStringLiteral("time_up") || id == QStringLiteral("wind") || id == QStringLiteral("spin_too_fast")) {
        return 3;
    }
    if (id == QStringLiteral("ready") || id == QStringLiteral("spin") || id == QStringLiteral("spin_reverse") ||
        id == QStringLiteral("figure_eight")) {
        return 2;
    }
    return 1;
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
        const QScopedValueRollback changing(_changingCue, true);
        _pending = false;
        _clearQueuedWarning();
        _currentWarningSince.invalidate();
        _currentId.clear();
        _effect->stop();
    }
}

void CaacExamAudio::_playLoaded()
{
    if (_changingCue || !_pending || !_effect || _effect->status() != QSoundEffect::Ready) {
        return;
    }
    _pending = false;
    // Late device or sample initialization must not replay a cue after its flight phase has passed.
    if (_pendingSince.elapsed() > LOAD_EXPIRY_MS || _volume() <= 0 ||
        (_currentWarningSince.isValid() && _currentWarningSince.elapsed() > WARNING_EXPIRY_MS)) {
        _playQueuedWarning();
        return;
    }
    _syncVolume();
    _effect->play();
}

void CaacExamAudio::_clearQueuedWarning()
{
    _queuedWarningId.clear();
    _queuedWarningText.clear();
    _queuedWarningSince.invalidate();
}

void CaacExamAudio::clearPendingWarnings()
{
    _clearQueuedWarning();
    if (_pending && _priority(_currentId) == 1) {
        const QScopedValueRollback changing(_changingCue, true);
        _pending = false;
        _currentWarningSince.invalidate();
        _currentId.clear();
        if (_effect) {
            _effect->stop();
        }
    }
}

void CaacExamAudio::_playQueuedWarning()
{
    if (_changingCue || _pending || !_effect || _effect->isPlaying() || _queuedWarningId.isEmpty()) {
        return;
    }
    const auto id = _queuedWarningId;
    const auto text = _queuedWarningText;
    const auto since = _queuedWarningSince;
    _clearQueuedWarning();
    if (!since.isValid() || since.elapsed() > WARNING_EXPIRY_MS || _volume() <= 0) {
        return;
    }
    _startCue(id, text, since);
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
        QStringLiteral("ready"),        QStringLiteral("spin"),      QStringLiteral("spin_reverse"),
        QStringLiteral("figure_eight"), QStringLiteral("area"),      QStringLiteral("speed_high"),
        QStringLiteral("speed_low"),    QStringLiteral("time_up"),   QStringLiteral("heading"),
        QStringLiteral("altitude"),     QStringLiteral("completed"), QStringLiteral("stopped"),
        QStringLiteral("telemetry"),    QStringLiteral("wind"),      QStringLiteral("spin_too_fast")};
    if (!CUES.contains(id) || QGC::runningUnitTests() || !_ensureSettings() || _volume() <= 0) {
        return;
    }
    if (_priority(id) > 1) {
        _clearQueuedWarning();
    }
    if (_effect && _pending && _pendingSince.elapsed() > LOAD_EXPIRY_MS) {
        const QScopedValueRollback changing(_changingCue, true);
        _pending = false;
        _effect->stop();
    }
    QElapsedTimer warningSince;
    if (_priority(id) == 1) {
        warningSince.start();
    }
    if (_effect && (_pending || _effect->isPlaying()) && _priority(id) == 1) {
        _queuedWarningId = id;
        _queuedWarningText = text;
        _queuedWarningSince = warningSince;
        return;
    }
    _clearQueuedWarning();
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
                    QMetaObject::invokeMethod(this, &CaacExamAudio::_playQueuedWarning, Qt::QueuedConnection);
                } else {
                    _playLoaded();
                }
            });
            connect(_effect, &QSoundEffect::playingChanged, this, [this]() {
                if (!_changingCue && !_effect->isPlaying()) {
                    QMetaObject::invokeMethod(this, &CaacExamAudio::_playQueuedWarning, Qt::QueuedConnection);
                }
            });
        }
        _currentId = id;
        _currentText = text;
        _currentWarningSince = warningSince;
        _pending = true;
        _pendingSince.start();
        _syncVolume();
        _effect->setSource(QUrl(QStringLiteral("qrc:/CAAC/ExamAudio/%1.wav").arg(id)));
    }
    _playLoaded();
}
