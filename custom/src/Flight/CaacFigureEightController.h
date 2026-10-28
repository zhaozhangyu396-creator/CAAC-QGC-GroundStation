#pragma once

#include <QtCore/QElapsedTimer>
#include <QtCore/QMap>
#include <QtCore/QObject>
#include <QtCore/QPointer>
#include <QtCore/QTimer>
#include <QtCore/QVariantList>
#include <QtCore/QVariantMap>
#include <QtPositioning/QGeoCoordinate>
#include <QtQmlIntegration/QtQmlIntegration>

class Vehicle;
class CaacFigureEightFenceController;

/// Manual-flight training records with a separately managed aircraft fence.
class CaacFigureEightController : public QObject
{
    Q_OBJECT
    QML_ELEMENT
    Q_MOC_INCLUDE("CaacFigureEightFenceController.h")
    Q_PROPERTY(CaacFigureEightFenceController* fence READ fence CONSTANT)
    Q_PROPERTY(int standard READ standard WRITE setStandard NOTIFY settingsChanged)
    Q_PROPERTY(QVariantMap limits READ limits NOTIFY settingsChanged)
    Q_PROPERTY(QVariantList sites READ sites NOTIFY sitesChanged)
    Q_PROPERTY(int selectedSite READ selectedSite WRITE selectSite NOTIFY siteChanged)
    Q_PROPERTY(QGeoCoordinate pointA READ pointA NOTIFY siteChanged)
    Q_PROPERTY(QGeoCoordinate pointB READ pointB NOTIFY siteChanged)
    Q_PROPERTY(bool calibrationReady READ calibrationReady NOTIFY calibrationChanged)
    Q_PROPERTY(QString calibrationStatus READ calibrationStatus NOTIFY calibrationChanged)
    Q_PROPERTY(QVariantList referencePath READ referencePath NOTIFY siteChanged)
    Q_PROPERTY(QVariantList recordedPath READ recordedPath NOTIFY sampleChanged)
    Q_PROPERTY(bool viewActive READ viewActive WRITE setViewActive NOTIFY viewActiveChanged)
    Q_PROPERTY(bool showOnMap READ showOnMap WRITE setShowOnMap NOTIFY settingsChanged)
    Q_PROPERTY(double fenceWidth READ fenceWidth WRITE setFenceWidth NOTIFY settingsChanged)
    Q_PROPERTY(bool running READ running NOTIFY sessionChanged)
    Q_PROPERTY(QVariantMap metrics READ metrics NOTIFY sampleChanged)
    Q_PROPERTY(QString status READ status NOTIFY sessionChanged)
    Q_PROPERTY(QString lastError READ lastError NOTIFY lastErrorChanged)
    Q_PROPERTY(QVariantMap effectiveLimits READ effectiveLimits NOTIFY windChanged)
    Q_PROPERTY(int windSource READ windSource WRITE setWindSource NOTIFY windChanged)
    Q_PROPERTY(double manualWindSpeed READ manualWindSpeed WRITE setManualWindSpeed NOTIFY windChanged)
    Q_PROPERTY(double groundWindSpeed READ groundWindSpeed NOTIFY windChanged)
    Q_PROPERTY(double windSpeed READ windSpeed NOTIFY windChanged)
    Q_PROPERTY(bool windValid READ windValid NOTIFY windChanged)
    Q_PROPERTY(int windLevel READ windLevel NOTIFY windChanged)
    Q_PROPERTY(double windFactor READ windFactor NOTIFY windChanged)
    Q_PROPERTY(int exerciseMode READ exerciseMode WRITE setExerciseMode NOTIFY settingsChanged)
    Q_PROPERTY(QString phase READ phase NOTIFY sessionChanged)
    Q_PROPERTY(int attemptNumber READ attemptNumber NOTIFY sessionChanged)
    Q_PROPERTY(int attemptLimit READ attemptLimit NOTIFY settingsChanged)
    Q_PROPERTY(bool spinPassed READ spinPassed NOTIFY sessionChanged)
    Q_PROPERTY(QString lastAttemptFailure READ lastAttemptFailure NOTIFY sessionChanged)
    Q_PROPERTY(QString lastAnnouncement READ lastAnnouncement NOTIFY announcementChanged)
    Q_PROPERTY(bool voiceEnabled READ voiceEnabled WRITE setVoiceEnabled NOTIFY settingsChanged)

public:
    explicit CaacFigureEightController(QObject* parent = nullptr);

    CaacFigureEightFenceController* fence() const { return _fence; }

    int standard() const { return _standard; }

    void setStandard(int standard);

    QVariantMap limits() const { return _limits; }

    Q_INVOKABLE bool setLimits(const QVariantMap& limits);
    Q_INVOKABLE void resetLimits();

    QVariantList sites() const { return _sites; }

    int selectedSite() const { return _selectedSite; }

    Q_INVOKABLE void selectSite(int index);

    QGeoCoordinate pointA() const { return _pointA; }

    QGeoCoordinate pointB() const { return _pointB; }

    bool calibrationReady() const { return _calibrationReady; }

    QString calibrationStatus() const { return _calibrationStatus; }

    Q_INVOKABLE bool capturePointA();
    Q_INVOKABLE bool capturePointB();
    Q_INVOKABLE bool setPoints(const QGeoCoordinate& pointA, const QGeoCoordinate& pointB);
    Q_INVOKABLE bool saveSite(const QString& name, const QGeoCoordinate& pointA, const QGeoCoordinate& pointB);
    Q_INVOKABLE void removeSite(int index);
    Q_INVOKABLE bool importSites(const QString& filename);
    Q_INVOKABLE bool exportSites(const QString& filename);
    QVariantList referencePath() const;

    QVariantList recordedPath() const { return _recordedPath; }

    bool viewActive() const { return _viewActive; }

    void setViewActive(bool active);

    bool showOnMap() const { return _showOnMap; }

    void setShowOnMap(bool visible);

    double fenceWidth() const { return _fenceWidth; }

    void setFenceWidth(double width);

    bool running() const { return _running; }

    QVariantMap metrics() const { return _metrics; }

    QString status() const { return _status; }

    QString lastError() const { return _lastError; }

    Q_INVOKABLE bool start();
    Q_INVOKABLE void stop();
    Q_INVOKABLE void clearRecording();
    Q_INVOKABLE void clearTrajectory();
    QVariantMap effectiveLimits() const;

    int windSource() const { return _windSource; }

    void setWindSource(int source);

    double manualWindSpeed() const { return _manualWindSpeed; }

    void setManualWindSpeed(double speed);

    double groundWindSpeed() const { return _groundWindSpeed; }

    double windSpeed() const;
    bool windValid() const;
    int windLevel() const;
    double windFactor() const;
    Q_INVOKABLE bool updateGroundWind(double speed);

    int exerciseMode() const { return _exerciseMode; }

    void setExerciseMode(int mode);
    QString phase() const;

    int attemptNumber() const { return _attemptNumber; }

    int attemptLimit() const { return _exerciseMode == 2 ? 3 : 1; }

    bool spinPassed() const { return _spinPassed; }

    QString lastAttemptFailure() const { return _lastAttemptFailure; }

    QString lastAnnouncement() const { return _lastAnnouncement; }

    bool voiceEnabled() const { return _voiceEnabled; }

    void setVoiceEnabled(bool enabled);
    Q_INVOKABLE void testVoice();

    static QList<QGeoCoordinate> referenceCoordinates(const QGeoCoordinate& pointA, const QGeoCoordinate& pointB,
                                                      bool instructor = false);

signals:
    void settingsChanged();
    void sitesChanged();
    void siteChanged();
    void calibrationChanged();
    void viewActiveChanged();
    void sampleChanged();
    void sessionChanged();
    void lastErrorChanged();
    void windChanged();
    void announcementChanged();
    void announcementRequested(const QString& id, const QString& text);
    void alarmsCleared();

private slots:
    void _sample();
    void _checkWindFreshness();

private:
    void _setVehicle(Vehicle* vehicle);
    bool _capturePoint(bool pointA);
    QString _calibrationBlockReason() const;
    void _updateCalibration();
    void _finish(const QString& message, const QString& cue = QStringLiteral("stopped"));
    bool _fail(const QString& message);
    void _clearError();
    void _saveSettings() const;
    void _rebuildPath();
    void _windUpdated();
    void _updateReady();
    void _announce(const QString& id, const QString& text);
    void _beginPhase(bool reverseSpin = false);
    void _prepareAttempt();
    void _samplePositioning();
    void _failAttempt(const QString& reason);
    void _sampleSpin();
    bool _telemetryReady() const;
    double _nearestProgress(const QGeoCoordinate& coordinate, double& distance, double& heading) const;
    static QVariantMap _defaults(int standard);
    static bool _validSite(const QVariantMap& site);

    QPointer<Vehicle> _vehicle;
    CaacFigureEightFenceController* _fence = nullptr;
    QPointer<Vehicle> _calibrationVehicle;
    QList<QMetaObject::Connection> _vehicleConnections;
    QTimer _timer;
    QTimer _windTimer;
    QElapsedTimer _clock;
    QElapsedTimer _sessionClock;
    QVariantMap _limits;
    QMap<int, QVariantMap> _categoryLimits;
    QVariantMap _metrics;
    QVariantList _sites;
    QVariantList _recordedPath;
    QList<QGeoCoordinate> _reference;
    QList<double> _cumulativeLength;
    QGeoCoordinate _pointA;
    QGeoCoordinate _pointB;
    QGeoCoordinate _rtkCoordinate;
    QGeoCoordinate _previousPosition;
    QString _status;
    QString _lastError;
    QString _calibrationStatus;
    int _standard = 0;
    int _selectedSite = -1;
    bool _viewActive = false;
    bool _showOnMap = true;
    bool _running = false;
    bool _calibrationReady = false;
    double _fenceWidth = 1.2;
    double _baselineAltitude = 0;
    double _previousProgress = 0;
    double _signedProgress = 0;
    double _travelDistance = 0;
    int _violationSamples = 0;
    int _completedLaps = 0;
    qint64 _lastPosition = -1;
    qint64 _lastAttitude = -1;
    double _headingDegrees = qQNaN();
    bool _usingAttitudeQuaternion = false;
    qint64 _lastSpeed = -1;
    qint64 _lastGpsFix = -1;
    int _gpsFixType = 0;
    qint64 _lastSample = 0;
    qint64 _lastRecorded = 0;
    QStringList _violations;
    int _windSource = 0;
    double _manualWindSpeed = 0;
    double _groundWindSpeed = qQNaN();
    qint64 _groundWindReceived = -1;
    bool _lastWindValid = true;
    enum class Phase
    {
        Idle,
        Positioning,
        Spin,
        ReverseSpin,
        FigureEight,
        Finished
    };
    Phase _phase = Phase::Idle;
    int _exerciseMode = 0;
    bool _voiceEnabled = true;
    bool _readyAnnounced = false;
    QString _lastAnnouncement;
    QElapsedTimer _phaseClock;
    QElapsedTimer _hoverSince;
    QGeoCoordinate _phaseAnchor;
    double _spinLastHeading = 0;
    double _spinAngle = 0;
    int _attemptNumber = 0;
    bool _spinPassed = false;
    bool _subjectStarted = false;
    QString _lastAttemptFailure;
};
