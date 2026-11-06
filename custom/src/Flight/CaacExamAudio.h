#pragma once

#include <optional>

#include <QtCore/QElapsedTimer>
#include <QtCore/QList>
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
    void stop();

private:
    friend class CaacFigureEightControllerTest;

    struct QueuedCue
    {
        QString id;
        QString text;
        QElapsedTimer since;
    };

    bool _ensureSettings();
    double _volume() const;
    void _syncVolume();
    void _startCue(const QString& id, const QString& text, const QElapsedTimer& warningSince);
    void _playLoaded();
    void _playQueuedCue();
    void _clearQueuedCue();
    bool _deferCue(const QString& id, const QString& text, const QElapsedTimer& since);
    std::optional<QueuedCue> _nextQueuedCue();
    void _reportError(const QString& id, const QString& text);
    static int _priority(const QString& id);
    static bool _cueExpired(const QString& id, qint64 elapsed);

    QPointer<Fact> _volumeFact;
    QPointer<Fact> _mutedFact;
    QSoundEffect* _effect = nullptr;
    QElapsedTimer _currentCueSince;
    QSet<QString> _reportedErrors;
    QString _currentId;
    QString _currentText;
    QList<QueuedCue> _queuedCues;
    bool _pending = false;
    bool _changingCue = false;
};
