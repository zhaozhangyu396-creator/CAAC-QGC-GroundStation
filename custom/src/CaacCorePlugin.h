#pragma once

#include <QtCore/QHash>
#include <QtCore/QPointer>
#include <QtCore/QVariantMap>
#include <QtPositioning/QGeoCoordinate>

#include "QGCCorePlugin.h"

class CaacFlightDistance;
class CaacSimulationController;
class QQmlAbstractUrlInterceptor;
class QTimer;
class Vehicle;

class CaacCorePlugin : public QGCCorePlugin
{
    Q_OBJECT
    Q_MOC_INCLUDE("CaacFlightDistance.h")
    Q_MOC_INCLUDE("Vehicle.h")
    Q_MOC_INCLUDE("PlanMasterController.h")
    Q_MOC_INCLUDE("CaacSimulationController.h")
    Q_PROPERTY(CaacSimulationController* simulation READ simulation CONSTANT)
    Q_PROPERTY(CaacFlightDistance* flightDistance READ flightDistance NOTIFY flightDistanceChanged)
    Q_PROPERTY(bool doubleClickAddWaypoint READ doubleClickAddWaypoint WRITE setDoubleClickAddWaypoint NOTIFY
                   mapPreferencesChanged)
    Q_PROPERTY(bool dragWaypoints READ dragWaypoints WRITE setDragWaypoints NOTIFY mapPreferencesChanged)
    Q_PROPERTY(QVariantMap planningPreferences READ planningPreferences NOTIFY mapPreferencesChanged)

public:
    explicit CaacCorePlugin(QObject* parent = nullptr);
    static QGCCorePlugin* instance();
    void init() override;
    void adjustSettingMetaData(const QString& settingsGroup, FactMetaData& metaData, bool& userVisible) override;

    bool showInitialSetupVehiclePreferences() const override { return false; }

    bool showInitialSetupMeasurementUnits() const override { return false; }
    CaacFlightDistance* flightDistance() const;

    CaacSimulationController* simulation() const { return _simulation; }

    Q_INVOKABLE QString refreshFlightMission(PlanMasterController* controller);
    Q_INVOKABLE void resetFlightDistanceForSimulationRestart(Vehicle* vehicle);
    Q_INVOKABLE QString updateFlightWaypointAltitude(PlanMasterController* controller, QObject* waypoint,
                                                     double altitude);
    Q_INVOKABLE QString updateFlightWaypointAltitudes(PlanMasterController* controller, const QVariantList& waypoints,
                                                      double altitude);

    bool doubleClickAddWaypoint() const { return _doubleClickAddWaypoint; }

    bool dragWaypoints() const { return _dragWaypoints; }

    void setDoubleClickAddWaypoint(bool enabled);
    void setDragWaypoints(bool enabled);

    QVariantMap planningPreferences() const { return _planningPreferences; }

    Q_INVOKABLE bool updatePlanningPreferences(const QVariantMap& values);
    Q_INVOKABLE void refreshFlightPreferenceParameter(const QString& name);
    Q_INVOKABLE QVariantMap routeLegMetrics(const QGeoCoordinate& from, const QGeoCoordinate& to) const;
    Q_INVOKABLE QVariantMap coordinateToDms(double value, int maximumDegrees) const;
    Q_INVOKABLE double coordinateFromDms(double degrees, double minutes, double seconds, bool negative,
                                         int maximumDegrees) const;

    QUrl planViewEditor() const override;
    QQmlApplicationEngine* createQmlApplicationEngine(QObject* parent) override;
    void destroyQmlApplicationEngine(QQmlApplicationEngine* engine) override;

    bool directMissionItemEditing() const override { return true; }

    bool showPlanInformation() const override { return false; }

    bool keepCurrentPlanOnVehicleChange(PlanMasterController* controller) const override;

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
    QVariantMap _planningPreferences;
    CaacSimulationController* _simulation = nullptr;
    QPointer<QTimer> _flightAltitudeUpdateTimer;
};
