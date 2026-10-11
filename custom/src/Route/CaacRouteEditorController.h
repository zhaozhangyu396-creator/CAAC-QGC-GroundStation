#pragma once

#include <QtCore/QJsonObject>
#include <QtCore/QObject>
#include <QtCore/QPointer>
#include <QtCore/QTimer>
#include <QtCore/QVariantList>
#include <QtCore/QVariantMap>
#include <QtPositioning/QGeoCoordinate>
#include <QtQmlIntegration/QtQmlIntegration>

class MissionController;
class PlanMasterController;
class SimpleMissionItem;
class VisualMissionItem;

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
    Q_PROPERTY(bool canUndo READ canUndo NOTIFY historyChanged)
    Q_PROPERTY(bool canRedo READ canRedo NOTIFY historyChanged)
    Q_PROPERTY(QGeoCoordinate rotationCenter READ rotationCenter NOTIFY rotationCenterChanged)
    Q_PROPERTY(QVariantList coordinateItems READ coordinateItems NOTIFY coordinateItemsChanged)

public:
    explicit CaacRouteEditorController(QObject* parent = nullptr);

    PlanMasterController* planMasterController() const;
    void setPlanMasterController(PlanMasterController* controller);
    SimpleMissionItem* currentItem() const;

    QString lastError() const { return _lastError; }

    bool canUndo() const { return _historyIndex > 0; }

    bool canRedo() const { return _historyIndex + 1 < _history.size(); }

    QGeoCoordinate rotationCenter() const { return _rotationCenter; }

    QVariantList coordinateItems() const;

    Q_INVOKABLE bool addWaypoint(const QGeoCoordinate& coordinate);
    Q_INVOKABLE bool selectWaypoint(QObject* item);
    Q_INVOKABLE bool updateWaypoint(QObject* item, double latitude, double longitude, double altitude,
                                    bool specifySpeed, double speed);
    Q_INVOKABLE QVariantMap waypointLeg(QObject* item) const;
    Q_INVOKABLE bool updateRelativeWaypoint(QObject* item, double angle, double distance, bool interiorAngle,
                                            double altitude, bool specifySpeed, double speed);
    Q_INVOKABLE bool removeCurrentWaypoint();
    Q_INVOKABLE bool moveCurrentWaypoint(int offset);
    Q_INVOKABLE bool setRotationCenter(const QGeoCoordinate& coordinate);
    Q_INVOKABLE bool rotateRoute(double clockwiseDegrees);
    Q_INVOKABLE bool rotateRouteAround(const QGeoCoordinate& center, double clockwiseDegrees);
    Q_INVOKABLE bool alignRoute(int fromIndex, int toIndex, double targetBearing);
    Q_INVOKABLE bool alignRouteAround(const QGeoCoordinate& center, int fromIndex, int toIndex, double targetBearing);
    Q_INVOKABLE bool insertRelativeWaypoint(int referenceIndex, int afterIndex, double angle, double distance,
                                            bool interiorAngle);
    Q_INVOKABLE void beginEdit();
    Q_INVOKABLE void endEdit();
    Q_INVOKABLE bool undo();
    Q_INVOKABLE bool redo();
    Q_INVOKABLE void clearHistory();
    Q_INVOKABLE QVariantList templateCoordinates(int shape, int referenceIndex, const QVariantMap& options) const;
    Q_INVOKABLE bool insertTemplate(int shape, int referenceIndex, const QVariantMap& options);
    Q_INVOKABLE bool translateRoute(const QGeoCoordinate& reference, const QGeoCoordinate& target);
    Q_INVOKABLE bool moveRouteBy(double bearing, double distance);
    Q_INVOKABLE bool updatePlannedHome(double latitude, double longitude, double amslAltitude);

signals:
    void planMasterControllerChanged();
    void selectionChanged();
    void lastErrorChanged();
    void historyChanged();
    void rotationCenterChanged();
    void coordinateItemsChanged();

private:
    bool eventFilter(QObject* watched, QEvent* event) override;
    MissionController* _missionController() const;
    VisualMissionItem* _previousCoordinateItem(int beforeIndex) const;
    bool _canEdit();
    bool _canChangeOrder();
    bool _fail(const QString& error);
    void _clearError();

    struct Snapshot
    {
        QJsonObject mission;
        QGeoCoordinate center;
        int selection = 0;
    };

    Snapshot _snapshot() const;
    void _capture();
    void _scheduleCapture();
    void _watchItems();
    bool _restore(int index);
    bool _rotationCoordinates(QList<QGeoCoordinate>& coordinates, QList<QObject*>& items);
    void _setCenter(const QGeoCoordinate& coordinate);

    QPointer<PlanMasterController> _masterController;
    QString _lastError;
    QGeoCoordinate _rotationCenter;
    QList<Snapshot> _history;
    QList<QMetaObject::Connection> _itemConnections;
    QTimer _captureTimer;
    int _historyIndex = -1;
    int _editDepth = 0;
    bool _restoring = false;
};
