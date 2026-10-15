#pragma once

#include <QtCore/QElapsedTimer>
#include <QtCore/QJsonDocument>
#include <QtCore/QObject>
#include <QtCore/QPointer>
#include <QtCore/QTimer>
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
        Resuming
    };
    void _tick();
    void _uploaded();
    void _finish(bool success, const QString& message);
    bool _prepareNative();
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
};
