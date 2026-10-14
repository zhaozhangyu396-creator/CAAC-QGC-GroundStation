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

class CaacSimulationController : public QObject
{
    Q_OBJECT
    QML_ELEMENT
    QML_UNCREATABLE("Owned by the application plugin")
    Q_MOC_INCLUDE("Vehicle.h")
    Q_PROPERTY(bool active READ active NOTIFY statusChanged)
    Q_PROPERTY(bool emergencyMode READ emergencyMode WRITE setEmergencyMode NOTIFY emergencyModeChanged)
    Q_PROPERTY(bool missionReady READ missionReady NOTIFY statusChanged)
    Q_PROPERTY(double takeoffHeight READ takeoffHeight NOTIFY statusChanged)
    Q_PROPERTY(QString status READ status NOTIFY statusChanged)
    Q_PROPERTY(Vehicle* vehicle READ vehicle NOTIFY statusChanged)
    Q_PROPERTY(int waypointNumberRevision READ waypointNumberRevision NOTIFY waypointNumbersChanged)

public:
    explicit CaacSimulationController(QObject* parent = nullptr);
    ~CaacSimulationController() override;

    bool active() const { return !_link.isNull(); }

    bool emergencyMode() const { return _emergencyMode; }

    void setEmergencyMode(bool enabled);

    bool missionReady() const { return _missionReady; }

    double takeoffHeight() const { return _settings.value("takeoffHeight", 10.0).toDouble(); }

    QString status() const { return _status; }

    Vehicle* vehicle() const;
    bool ownsVehicle(const Vehicle* vehicle) const;

    int waypointNumberRevision() const { return _waypointNumberRevision; }

    Q_INVOKABLE int waypointNumber(int sequence, const QGeoCoordinate& coordinate) const;
    Q_INVOKABLE bool start(PlanMasterController* plan);
    Q_INVOKABLE bool upload(PlanMasterController* plan);
    Q_INVOKABLE bool restart();
    Q_INVOKABLE void stop();
    static QVariantMap defaults();
    static QString validateSettings(const QVariantMap& settings);

signals:
    void statusChanged();
    void missionUploaded();
    void emergencyModeChanged();
    void waypointNumbersChanged();

private:
    bool _prepareFlightPlan();
    void _tick();
    void _publish();
    void _setStatus(const QString& status);
    void _fail(const QString& error);
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
    QTimer _timer;
    QElapsedTimer _elapsed;
    QString _status;
    bool _emergencyMode = false;
    bool _uploadSent = false;
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
