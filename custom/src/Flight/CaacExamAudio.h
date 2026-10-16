#pragma once

#include <QtCore/QElapsedTimer>
#include <QtCore/QObject>
#include <QtCore/QPointer>
#include <QtCore/QSet>
#include <QtCore/QString>

class Fact;
class QSoundEffect;

/// Plays the bundled Chinese examination announcements using the global voice settings.
class CaacExamAudio : public QObject
{
    Q_OBJECT

public:
    explicit CaacExamAudio(QObject* parent = nullptr);

public slots:
    void play(const QString& id, const QString& text);
    void clearPendingWarnings();

private:
    bool _ensureSettings();
    double _volume() const;
    void _syncVolume();
    void _startCue(const QString& id, const QString& text, const QElapsedTimer& warningSince);
    void _playLoaded();
    void _playQueuedWarning();
    void _clearQueuedWarning();
    void _reportError(const QString& id, const QString& text);
    static int _priority(const QString& id);

    QPointer<Fact> _volumeFact;
    QPointer<Fact> _mutedFact;
    QSoundEffect* _effect = nullptr;
    QElapsedTimer _pendingSince;
    QElapsedTimer _currentWarningSince;
    QElapsedTimer _queuedWarningSince;
    QSet<QString> _reportedErrors;
    QString _currentId;
    QString _currentText;
    QString _queuedWarningId;
    QString _queuedWarningText;
    bool _pending = false;
    bool _changingCue = false;
};
