#pragma once

#include <QtCore/QHash>
#include <QtCore/QVariantMap>
#include <QtPositioning/QGeoCoordinate>

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
    Q_PROPERTY(bool doubleClickAddWaypoint READ doubleClickAddWaypoint WRITE setDoubleClickAddWaypoint NOTIFY
                   mapPreferencesChanged)
    Q_PROPERTY(bool dragWaypoints READ dragWaypoints WRITE setDragWaypoints NOTIFY mapPreferencesChanged)

public:
    explicit CaacCorePlugin(QObject* parent = nullptr);
    static QGCCorePlugin* instance();
    void init() override;
    CaacFlightDistance* flightDistance() const;
    Q_INVOKABLE void resetFlightDistanceForSimulationRestart(Vehicle* vehicle);

    bool doubleClickAddWaypoint() const { return _doubleClickAddWaypoint; }

    bool dragWaypoints() const { return _dragWaypoints; }

    void setDoubleClickAddWaypoint(bool enabled);
    void setDragWaypoints(bool enabled);
    Q_INVOKABLE QVariantMap routeLegMetrics(const QGeoCoordinate& from, const QGeoCoordinate& to) const;
    Q_INVOKABLE QVariantMap coordinateToDms(double value, int maximumDegrees) const;
    Q_INVOKABLE double coordinateFromDms(double degrees, double minutes, double seconds, bool negative,
                                         int maximumDegrees) const;

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
    void mapPreferencesChanged();

private:
    void _watchVehicle(Vehicle* vehicle);
    QHash<Vehicle*, CaacFlightDistance*> _flightDistances;
    QHash<QQmlApplicationEngine*, QQmlAbstractUrlInterceptor*> _flyInterceptors;
    bool _initialized = false;
    bool _doubleClickAddWaypoint = true;
    bool _dragWaypoints = true;
};
