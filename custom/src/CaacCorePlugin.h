#pragma once

#include <QtCore/QHash>

#include "QGCCorePlugin.h"

class CaacFlightDistance;
class QQmlAbstractUrlInterceptor;
class Vehicle;

class CaacCorePlugin : public QGCCorePlugin
{
    Q_OBJECT
    Q_MOC_INCLUDE("CaacFlightDistance.h")
    Q_MOC_INCLUDE("Vehicle.h")
    Q_PROPERTY(CaacFlightDistance* flightDistance READ flightDistance NOTIFY flightDistanceChanged)

public:
    explicit CaacCorePlugin(QObject* parent = nullptr);
    static QGCCorePlugin* instance();
    void init() override;
    CaacFlightDistance* flightDistance() const;
    Q_INVOKABLE void resetFlightDistanceForSimulationRestart(Vehicle* vehicle);

    QUrl planViewEditor() const override;
    QQmlApplicationEngine* createQmlApplicationEngine(QObject* parent) override;
    void destroyQmlApplicationEngine(QQmlApplicationEngine* engine) override;

    bool directMissionItemEditing() const override { return true; }

    bool showPlanInformation() const override { return false; }

    QLocale::Language defaultLanguage() const override { return QLocale::Chinese; }

    void postSaveToJson(PlanMasterController* controller, QJsonObject& json) override;
    void postLoadFromJson(PlanMasterController* controller, QJsonObject& json) override;

signals:
    void flightDistanceChanged();

private:
    void _watchVehicle(Vehicle* vehicle);
    QHash<Vehicle*, CaacFlightDistance*> _flightDistances;
    QHash<QQmlApplicationEngine*, QQmlAbstractUrlInterceptor*> _flyInterceptors;
    bool _initialized = false;
};
