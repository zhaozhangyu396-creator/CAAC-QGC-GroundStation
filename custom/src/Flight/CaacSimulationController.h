#pragma once

#include <QtCore/QElapsedTimer>
#include <QtCore/QHash>
#include <QtCore/QJsonObject>
#include <QtCore/QObject>
#include <QtCore/QPointer>
#include <QtCore/QTimer>
#include <QtCore/QVariantMap>
#include <QtQmlIntegration/QtQmlIntegration>

#include "CaacSimulationEngine.h"

class MockLink;
class PlanMasterController;
class Vehicle;
class Joystick;

class CaacSimulationController : public QObject
{
    Q_OBJECT
    QML_ELEMENT
    QML_UNCREATABLE("Owned by the application plugin")
    Q_MOC_INCLUDE("Vehicle.h")
    Q_MOC_INCLUDE("PlanMasterController.h")
    Q_PROPERTY(bool active READ active NOTIFY statusChanged)
    Q_PROPERTY(bool emergencyMode READ emergencyMode WRITE setEmergencyMode NOTIFY emergencyModeChanged)
    Q_PROPERTY(bool missionReady READ missionReady NOTIFY statusChanged)
    Q_PROPERTY(bool missionFinished READ missionFinished NOTIFY statusChanged)
    Q_PROPERTY(bool hasFlown READ hasFlown NOTIFY statusChanged)
    Q_PROPERTY(double takeoffHeight READ takeoffHeight NOTIFY statusChanged)
    Q_PROPERTY(QString status READ status NOTIFY statusChanged)
    Q_PROPERTY(Vehicle* vehicle READ vehicle NOTIFY statusChanged)
    Q_PROPERTY(PlanMasterController* planningPlan READ planningPlan NOTIFY statusChanged)
    Q_PROPERTY(int waypointNumberRevision READ waypointNumberRevision NOTIFY waypointNumbersChanged)
    Q_PROPERTY(int remainingLoops READ remainingLoops NOTIFY statusChanged)
    Q_PROPERTY(QVariantMap currentMissionDetails READ currentMissionDetails NOTIFY missionDetailsChanged)
    Q_PROPERTY(bool manualControlActive READ manualControlActive NOTIFY statusChanged)
    Q_PROPERTY(QVariantList joystickChannels READ joystickChannels NOTIFY joystickChannelsChanged)
    Q_PROPERTY(int modeSwitchPosition READ modeSwitchPosition NOTIFY joystickChannelsChanged)
    Q_PROPERTY(
        QString modeSwitchUnavailableReason READ modeSwitchUnavailableReason NOTIFY modeSwitchConfigurationChanged)

public:
    explicit CaacSimulationController(QObject* parent = nullptr);
    ~CaacSimulationController() override;

    bool active() const { return !_link.isNull(); }

    bool hasFlown() const { return _hasFlown; }

    bool missionFinished() const { return _missionReady && _engine.state().finished; }

    bool emergencyMode() const { return _emergencyMode; }

    void setEmergencyMode(bool enabled);

    bool missionReady() const { return _missionReady; }

    double takeoffHeight() const { return _settings.value("takeoffHeight", 10.0).toDouble(); }

    QString status() const { return _status; }

    Vehicle* vehicle() const;
    PlanMasterController* planningPlan() const;
    bool ownsVehicle(const Vehicle* vehicle) const;

    int waypointNumberRevision() const { return _waypointNumberRevision; }

    int remainingLoops() const { return _missionReady ? _engine.remainingLoops() : -2; }

    bool manualControlActive() const;
    QVariantMap currentMissionDetails() const;

    QVariantList joystickChannels() const { return _joystickChannels; }

    int modeSwitchPosition() const { return _modeLastPosition; }

    Q_INVOKABLE int waypointNumber(int sequence, const QGeoCoordinate& coordinate) const;
    Q_INVOKABLE bool start(PlanMasterController* plan);
    Q_INVOKABLE bool upload(PlanMasterController* plan, bool preserveProgress = false, bool speedChanges = false,
                            double currentFlightSpeed = qQNaN());
    Q_INVOKABLE bool restart();
    Q_INVOKABLE bool beginManualControl();
    Q_INVOKABLE void endManualControl();
    Q_INVOKABLE bool modeSwitchAxisAllowed(int axis) const;
    QString modeSwitchUnavailableReason() const;
    Q_INVOKABLE void stop();
    static QVariantMap defaults();
    static QString validateSettings(const QVariantMap& settings);

signals:
    void statusChanged();
    void missionUploaded();
    void missionUploadFailed();
    void missionDetailsChanged();
    void emergencyModeChanged();
    void waypointNumbersChanged();
    void joystickChannelsChanged();
    void modeSwitchConfigurationChanged();

private:
    bool _prepareFlightPlan(bool speedChanges = false);
    void _tick();
    void _publish();
    void _setStatus(const QString& status);
    void _fail(const QString& error);
    void _setJoystick(Joystick* joystick);
    void _processJoystickChannels(const QVector<int>& channels);
    void _applyJoystickMode(int mode);
    void _enterManualControl();
    QPointer<Joystick> _joystick;
    bool _hasFlown = false;
    QMetaObject::Connection _joystickChannelsConnection;
    QList<QMetaObject::Connection> _joystickConfigurationConnections;
    QVariantList _joystickChannels;
    QString _modeMappingSignature;
    QElapsedTimer _modeCandidateTime;
    int _modeCandidatePosition = -1;
    int _modeLastPosition = -1;
    QPointer<MockLink> _link;
    QPointer<Vehicle> _vehicle;
    QPointer<PlanMasterController> _uploadPlan;
    QPointer<PlanMasterController> _planningPlan;
    CaacSimulationEngine _engine;
    CaacSimulationEngine::Options _options;
    QMap<int, CaacSimulationEngine::WaypointOptions> _waypointOptions;
    QGeoCoordinate _home;
    QGeoCoordinate _guidedTarget;
    QJsonObject _sourceMission;
    QVariantMap _settings;
    QVariantMap _publishedMissionDetails;
    QTimer _timer;
    QElapsedTimer _elapsed;
    QElapsedTimer _manualInputTime;
    double _manualForward = 0;
    double _manualRight = 0;
    double _manualYaw = 0;
    double _manualThrottle = 0.5;
    QString _status;
    bool _emergencyMode = false;
    bool _uploadSent = false;
    bool _preserveUploadProgress = false;
    bool _preserveUploadSpeedChanges = false;
    double _preserveUploadFlightSpeed = qQNaN();
    bool _missionReady = false;
    bool _settingsExplicit = false;

    struct WaypointNumber
    {
        int number;
        QGeoCoordinate coordinate;
    };

    QHash<int, WaypointNumber> _waypointNumbers;
    QHash<int, WaypointNumber> _pendingWaypointNumbers;
    int _waypointNumberRevision = 0;
    double _holdAltitude = qQNaN();
};
