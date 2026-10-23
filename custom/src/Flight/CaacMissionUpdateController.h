#pragma once

#include <QtCore/QElapsedTimer>
#include <QtCore/QJsonDocument>
#include <QtCore/QObject>
#include <QtCore/QPointer>
#include <QtCore/QTimer>
#include <QtCore/QtNumeric>
#include <QtQmlIntegration/QtQmlIntegration>

class CaacSimulationController;
class MissionItem;
class PlanMasterController;
class Vehicle;

/// Acknowledged pause, mission transfer, target restoration and resume transaction.
class CaacMissionUpdateController : public QObject
{
    Q_OBJECT
    QML_ELEMENT
    Q_MOC_INCLUDE("PlanMasterController.h")
    Q_MOC_INCLUDE("CaacSimulationController.h")
    Q_PROPERTY(bool busy READ busy NOTIFY busyChanged)

public:
    explicit CaacMissionUpdateController(QObject* parent = nullptr);
    ~CaacMissionUpdateController() override;

    bool busy() const { return _stage != Stage::Idle; }

    Q_INVOKABLE bool update(PlanMasterController* plan, CaacSimulationController* simulation);
    Q_INVOKABLE bool updateSpeeds(PlanMasterController* plan, CaacSimulationController* simulation,
                                  double currentFlightSpeed = qQNaN(), int expectedCurrentSequence = -1);

signals:
    void busyChanged();
    void finished(bool success, const QString& message);

private:
    enum class Stage
    {
        Idle,
        Pausing,
        Uploading,
        Restoring,
        ApplyingSpeed,
        Resuming
    };
    bool _update(PlanMasterController* plan, CaacSimulationController* simulation, bool allowSpeedChanges,
                 double currentFlightSpeed, int expectedCurrentSequence);
    void _tick();
    void _uploaded();
    void _finish(bool success, const QString& message);
    void _uploadFailed(const QString& detail = {});
    bool _prepareNative();
    bool _prepareNativeSpeeds();
    void _applySpeed();
    void _setStage(Stage stage);

    Stage _stage = Stage::Idle;
    QPointer<Vehicle> _vehicle;
    QPointer<PlanMasterController> _plan;
    QPointer<CaacSimulationController> _simulation;
    QJsonDocument _snapshot;
    QList<MissionItem*> _pendingItems;
    QList<QMetaObject::Connection> _connections;
    QTimer _timer;
    QElapsedTimer _elapsed;
    int _target = -1;
    bool _allowSpeedChanges = false;
    int _flightSpeedType = -1;
    double _runtimeFlightSpeed = qQNaN();
    double _forcedFlightSpeed = qQNaN();
    int _expectedCurrentSequence = -1;
    double _desiredFlightSpeed = qQNaN();
    QString _lastTransferError;
};
