#pragma once

#include <QtCore/QElapsedTimer>
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
    Q_PROPERTY(QString status READ status NOTIFY statusChanged)
    Q_PROPERTY(Vehicle* vehicle READ vehicle NOTIFY statusChanged)

public:
    explicit CaacSimulationController(QObject* parent = nullptr);
    ~CaacSimulationController() override;

    bool active() const { return !_link.isNull(); }

    QString status() const { return _status; }

    Vehicle* vehicle() const;
    bool ownsVehicle(const Vehicle* vehicle) const;
    Q_INVOKABLE bool start(PlanMasterController* plan);
    Q_INVOKABLE bool restart();
    Q_INVOKABLE void stop();
    static QVariantMap defaults();
    static QString validateSettings(const QVariantMap& settings);

signals:
    void statusChanged();

private:
    bool _prepareFlightPlan();
    void _tick();
    void _publish();
    void _setStatus(const QString& status);
    void _fail(const QString& error);
    QPointer<MockLink> _link;
    QPointer<Vehicle> _vehicle;
    QPointer<PlanMasterController> _uploadPlan;
    CaacSimulationEngine _engine;
    CaacSimulationEngine::Options _options;
    QGeoCoordinate _home;
    QGeoCoordinate _guidedTarget;
    QJsonObject _sourceMission;
    QVariantMap _settings;
    QTimer _timer;
    QElapsedTimer _elapsed;
    QString _status;
    bool _uploadSent = false;
    bool _missionReady = false;
    bool _missionStarted = false;
    bool _settingsExplicit = false;
    double _holdAltitude = qQNaN();
};
