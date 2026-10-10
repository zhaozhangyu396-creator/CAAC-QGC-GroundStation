#pragma once

#include <QtCore/QObject>
#include <QtCore/QPointer>
#include <QtPositioning/QGeoCoordinate>
#include <QtQmlIntegration/QtQmlIntegration>

class MissionController;
class PlanMasterController;
class SimpleMissionItem;

/// Edits the existing QGC plan model without uploading or changing vehicle parameters.
class CaacRouteEditorController : public QObject
{
    Q_OBJECT
    QML_ELEMENT
    Q_MOC_INCLUDE("PlanMasterController.h")
    Q_MOC_INCLUDE("SimpleMissionItem.h")
    Q_PROPERTY(PlanMasterController* planMasterController READ planMasterController WRITE setPlanMasterController NOTIFY
                   planMasterControllerChanged)
    Q_PROPERTY(SimpleMissionItem* currentItem READ currentItem NOTIFY selectionChanged)
    Q_PROPERTY(QString lastError READ lastError NOTIFY lastErrorChanged)

public:
    explicit CaacRouteEditorController(QObject* parent = nullptr);

    PlanMasterController* planMasterController() const;
    void setPlanMasterController(PlanMasterController* controller);
    SimpleMissionItem* currentItem() const;

    QString lastError() const { return _lastError; }

    Q_INVOKABLE bool addWaypoint(const QGeoCoordinate& coordinate);
    Q_INVOKABLE bool selectWaypoint(QObject* item);
    Q_INVOKABLE bool updateWaypoint(QObject* item, double latitude, double longitude, double altitude,
                                    bool specifySpeed, double speed);
    Q_INVOKABLE bool removeCurrentWaypoint();
    Q_INVOKABLE bool moveCurrentWaypoint(int offset);

signals:
    void planMasterControllerChanged();
    void selectionChanged();
    void lastErrorChanged();

private:
    MissionController* _missionController() const;
    bool _canEdit();
    bool _canChangeOrder();
    bool _fail(const QString& error);
    void _clearError();

    QPointer<PlanMasterController> _masterController;
    QString _lastError;
};
