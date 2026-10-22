#include "CaacRouteEditorController.h"

#include <algorithm>
#include <cmath>
#include <limits>

#include <QtCore/QEvent>
#include <QtCore/QJsonArray>
#include <QtCore/QLineF>
#include <QtCore/QScopeGuard>
#include <QtCore/QSet>
#include <QtCore/QtMath>
#include <QtPositioning/QGeoRectangle>

#include "CaacCorePlugin.h"
#include "CaacRouteGeometry.h"
#include "CaacRouteTemplates.h"
#include "CaacSimulationController.h"
#include "Fact.h"
#include "MissionController.h"
#include "MissionSettingsItem.h"
#include "PlanMasterController.h"
#include "QGCGeo.h"
#include "SimpleMissionItem.h"
#include "SpeedSection.h"
#include "Vehicle.h"
#include "VehicleGPSFactGroup.h"

CaacRouteEditorController::CaacRouteEditorController(QObject* parent)
    : QObject(parent)
{
    _captureTimer.setSingleShot(true);
    _captureTimer.setInterval(0);
    connect(&_captureTimer, &QTimer::timeout, this, &CaacRouteEditorController::_capture);
}

bool CaacRouteEditorController::setMissionFlightSpeed(bool specified, double speed)
{
    if (!_canEdit()) {
        return false;
    }
    auto* model = _missionController()->visualItems();
    auto* settings = model && model->count() ? model->value<MissionSettingsItem*>(0) : nullptr;
    auto* section = settings ? settings->speedSection() : nullptr;
    if (!section || !section->available()) {
        return _fail(tr("Mission flight speed is unavailable."));
    }
    if (specified &&
        (!std::isfinite(speed) || speed <= 0 || speed > std::numeric_limits<float>::max() ||
         speed < section->flightSpeed()->rawMin().toDouble() || speed > section->flightSpeed()->rawMax().toDouble())) {
        return _fail(tr("Enter a valid mission flight speed."));
    }
    if (specified != section->specifyFlightSpeed() && !_canChangeOrder()) {
        return false;
    }
    beginEdit();
    const auto finish = qScopeGuard([this]() { endEdit(); });
    if (specified) {
        section->flightSpeed()->setRawValue(speed);
    }
    section->setSpecifyFlightSpeed(specified);
    return true;
}

bool CaacRouteEditorController::syncWaypointDefaults(const QVariantMap& properties)
{
    if (!_canEdit() || _rotationPreviewActive) {
        return false;
    }
    const bool syncAltitude = properties.contains(QStringLiteral("altitude"));
    const bool syncSpeed = properties.contains(QStringLiteral("specifySpeed"));
    bool altitudeValid = false;
    bool speedValid = false;
    const double altitude = properties.value(QStringLiteral("altitude")).toDouble(&altitudeValid);
    const double speed = properties.value(QStringLiteral("speed")).toDouble(&speedValid);
    const bool specifySpeed = properties.value(QStringLiteral("specifySpeed")).toBool();
    QList<SimpleMissionItem*> waypoints;
    bool orderChanges = false;
    auto* model = _missionController()->visualItems();
    for (int i = 1; i < model->count(); ++i) {
        auto* item = model->value<SimpleMissionItem*>(i);
        if (!item || item->command() != MAV_CMD_NAV_WAYPOINT) {
            continue;
        }
        auto* section = qobject_cast<SpeedSection*>(item->speedSection());
        if ((syncAltitude &&
             (!altitudeValid || !std::isfinite(altitude) || altitude < item->altitude()->rawMin().toDouble() ||
              altitude > item->altitude()->rawMax().toDouble())) ||
            (syncSpeed && (!section || !section->available() ||
                           (specifySpeed && (!speedValid || !std::isfinite(speed) || speed <= 0 ||
                                             speed < section->flightSpeed()->rawMin().toDouble() ||
                                             speed > section->flightSpeed()->rawMax().toDouble()))))) {
            return _fail(tr("Check waypoint altitude and flight speed."));
        }
        orderChanges = orderChanges || (syncSpeed && section->specifyFlightSpeed() != specifySpeed);
        waypoints.append(item);
    }
    if (waypoints.isEmpty() || (!syncAltitude && !syncSpeed)) {
        return _fail(tr("No ordinary waypoints to synchronize."));
    }
    if (orderChanges && !_canChangeOrder()) {
        return false;
    }
    beginEdit();
    const auto finish = qScopeGuard([this]() { endEdit(); });
    for (auto* item : waypoints) {
        if (syncAltitude) {
            item->altitude()->setRawValue(altitude);
        }
        if (syncSpeed) {
            auto* section = qobject_cast<SpeedSection*>(item->speedSection());
            if (specifySpeed) {
                section->flightSpeed()->setRawValue(speed);
            }
            section->setSpecifyFlightSpeed(specifySpeed);
        }
    }
    return true;
}

PlanMasterController* CaacRouteEditorController::planMasterController() const
{
    return _masterController.data();
}

MissionController* CaacRouteEditorController::_missionController() const
{
    return _masterController ? _masterController->missionController() : nullptr;
}

QVariantMap CaacRouteEditorController::simulationSettings() const
{
    const auto settings =
        _masterController ? _masterController->property("caacSimulationSettings").toMap() : QVariantMap();
    return settings.isEmpty() || !CaacSimulationController::validateSettings(settings).isEmpty()
               ? CaacSimulationController::defaults()
               : settings;
}

double CaacRouteEditorController::routeOrientation() const
{
    const auto value = _masterController ? _masterController->property("caacRouteOrientation") : QVariant();
    bool valid = false;
    const double heading = value.toDouble(&valid);
    return valid && std::isfinite(heading) ? CaacRouteGeometry::normalizedAngle(heading) : 0;
}

bool CaacRouteEditorController::hasSimulationSettings() const
{
    return _masterController && !_masterController->property("caacSimulationSettings").toMap().isEmpty();
}

QVariantList CaacRouteEditorController::simulationPreviewSegments(const QVariantMap& settings) const
{
    auto* mission = _missionController();
    if (!mission || !mission->plannedHomePosition().isValid() ||
        !CaacSimulationController::validateSettings(settings).isEmpty()) {
        return {};
    }
    QList<SimpleMissionItem*> waypoints;
    QGeoCoordinate departure = mission->plannedHomePosition();
    for (int i = 1; i < mission->visualItems()->count(); ++i) {
        auto* item = mission->visualItems()->value<SimpleMissionItem*>(i);
        if (!item || item->command() == MAV_CMD_DO_JUMP) {
            return {};
        }
        if (item->command() == MAV_CMD_NAV_WAYPOINT) {
            if (!item->coordinate().isValid()) {
                return {};
            }
            waypoints.append(item);
        } else if (item->isTakeoffItem() && item->coordinate().isValid()) {
            departure = item->coordinate();
        } else if (item->specifiesCoordinate() && item->command() != MAV_CMD_NAV_LAND &&
                   item->command() != MAV_CMD_NAV_LOITER_UNLIM) {
            return {};
        }
    }
    if (waypoints.isEmpty()) {
        return {};
    }
    int start = 0;
    QGeoCoordinate returning = mission->plannedHomePosition();
    const int startSequence = settings.value("startSequence").toInt();
    const int returnSequence = settings.value("returnSequence").toInt();
    bool foundStart = startSequence == 0;
    bool foundReturn = returnSequence == 0;
    for (int i = 0; i < waypoints.count(); ++i) {
        if (waypoints.at(i)->sequenceNumber() == startSequence) {
            start = i;
            foundStart = true;
        }
        if (waypoints.at(i)->sequenceNumber() == returnSequence) {
            returning = waypoints.at(i)->coordinate();
            foundReturn = true;
        }
    }
    if (!foundStart || (settings.value("endBehavior").toInt() == 1 && !foundReturn)) {
        return {};
    }
    QVariantList result;
    const auto append = [&result](const QGeoCoordinate& from, const QGeoCoordinate& to, const QString& kind) {
        if (QGCGeo::geodesicDistance(from, to) >= 0.01) {
            result.append(QVariantMap{{QStringLiteral("coordinate1"), QVariant::fromValue(from)},
                                      {QStringLiteral("coordinate2"), QVariant::fromValue(to)},
                                      {QStringLiteral("kind"), kind}});
        }
    };
    append(mission->plannedHomePosition(), departure, QStringLiteral("takeoff"));
    const auto first = waypoints.at(start)->coordinate();
    append(departure, first, QStringLiteral("start"));
    auto last = first;
    const bool closed = settings.value("closed").toBool();
    const int count = closed ? waypoints.count() : waypoints.count() - start;
    for (int i = 1; i < count; ++i) {
        const auto next = waypoints.at((start + i) % waypoints.count())->coordinate();
        append(last, next, QStringLiteral("body"));
        last = next;
    }
    if (closed) {
        append(last, first, QStringLiteral("closure"));
        last = first;
    } else if (settings.value("loopMode").toInt() == 2 ||
               (settings.value("loopMode").toInt() == 1 && settings.value("loopCount").toInt() > 1)) {
        append(last, first, QStringLiteral("loop"));
    }
    if (settings.value("loopMode").toInt() != 2 && settings.value("endBehavior").toInt() == 1) {
        append(last, returning, QStringLiteral("return"));
    }
    return result;
}

bool CaacRouteEditorController::_previewItemsCurrent() const
{
    auto* mission = _missionController();
    if (!mission || !mission->visualItems()) {
        return false;
    }
    for (const auto& item : _previewItems) {
        if (!item || mission->visualItems()->indexOf(item) < 0) {
            return false;
        }
    }
    return !_previewItems.isEmpty();
}

void CaacRouteEditorController::_clearRotationPreview()
{
    _rotationPreviewActive = false;
    _previewCoordinates.clear();
    _previewItems.clear();
    if (_masterController) {
        _masterController->setProperty("caacRotationPreviewActive", false);
    }
    emit rotationPreviewChanged();
}

bool CaacRouteEditorController::previewRouteOrientation(double heading, const QGeoCoordinate& center)
{
    if (!_canEdit(true) || !std::isfinite(heading) || !center.isValid()) {
        return _fail(tr("Check the rotation centre and angle."));
    }
    if (!_rotationPreviewActive) {
        QList<QGeoCoordinate> coordinates;
        QList<QObject*> items;
        if (!_rotationCoordinates(coordinates, items)) {
            return false;
        }
        _previewCoordinates = coordinates;
        for (auto* object : items) {
            _previewItems.append(qobject_cast<VisualMissionItem*>(object));
        }
        _previewCenter = center;
        _previewBaseOrientation = routeOrientation();
        _previewDirty = _missionController()->dirty();
        beginEdit();
        _rotationPreviewActive = true;
        _masterController->setProperty("caacRotationPreviewActive", true);
        emit rotationPreviewChanged();
    }
    if (!_previewItemsCurrent()) {
        _clearRotationPreview();
        _editDepth = 0;
        clearHistory();
        return _fail(tr("The route changed. Start the orientation preview again."));
    }
    const auto coordinates =
        CaacRouteGeometry::rotate(_previewCoordinates, _previewCenter, heading - _previewBaseOrientation);
    if (!coordinates) {
        return _fail(tr("Check the rotation centre and angle."));
    }
    for (int i = 0; i < _previewItems.size(); ++i) {
        _previewItems[i]->setCoordinate(coordinates->at(i));
    }
    _previewOrientation = CaacRouteGeometry::normalizedAngle(heading);
    _missionController()->setDirty(true);
    return true;
}

bool CaacRouteEditorController::confirmRouteOrientation()
{
    if (!_rotationPreviewActive || !_canEdit(true) || !_previewItemsCurrent()) {
        return false;
    }
    _masterController->setProperty("caacRouteOrientation", _previewOrientation);
    _setCenter(_previewCenter);
    _clearRotationPreview();
    endEdit();
    return true;
}

void CaacRouteEditorController::cancelRouteOrientation()
{
    if (!_rotationPreviewActive) {
        return;
    }
    const bool current = _previewItemsCurrent();
    if (current) {
        for (int i = 0; i < _previewItems.size(); ++i) {
            _previewItems[i]->setCoordinate(_previewCoordinates.at(i));
        }
        _missionController()->setDirty(_previewDirty);
    }
    _clearRotationPreview();
    if (current) {
        endEdit();
    } else {
        _editDepth = 0;
        clearHistory();
    }
}

bool CaacRouteEditorController::setSimulationSettings(const QVariantMap& settings)
{
    if (!_canEdit()) {
        return false;
    }
    const auto error = CaacSimulationController::validateSettings(settings);
    if (!error.isEmpty()) {
        return _fail(error);
    }
    beginEdit();
    const auto finish = qScopeGuard([this]() { endEdit(); });
    _masterController->setProperty("caacSimulationSettings", settings);
    _missionController()->setDirty(true);
    emit simulationSettingsChanged();
    return true;
}

void CaacRouteEditorController::setPlanMasterController(PlanMasterController* controller)
{
    if (_masterController == controller) {
        return;
    }
    cancelRouteOrientation();
    if (_masterController) {
        _masterController->removeEventFilter(this);
        disconnect(_masterController, nullptr, this, nullptr);
        disconnect(_missionController(), nullptr, this, nullptr);
    }
    for (const auto& connection : _itemConnections) {
        disconnect(connection);
    }
    _itemConnections.clear();
    _captureTimer.stop();
    _editDepth = 0;
    _masterController = controller;
    _rotationCenter =
        controller ? controller->property("caacRotationCenter").value<QGeoCoordinate>() : QGeoCoordinate();
    if (controller) {
        _rotationCenter = controller->property("caacRotationCenter").value<QGeoCoordinate>();
        controller->installEventFilter(this);
        connect(controller, &PlanMasterController::managerVehicleChanged, this,
                &CaacRouteEditorController::_watchVehicleHome);
        connect(controller, &PlanMasterController::syncInProgressChanged, this,
                &CaacRouteEditorController::_syncPlannedHomeAltitude);
        connect(controller, &PlanMasterController::offlineChanged, this,
                &CaacRouteEditorController::_syncPlannedHomeAltitude);
        connect(controller->missionController(), &MissionController::plannedHomePositionChanged, this,
                &CaacRouteEditorController::_syncPlannedHomeAltitude, Qt::QueuedConnection);
        connect(controller->missionController(), &MissionController::syncInProgressChanged, this,
                &CaacRouteEditorController::_syncPlannedHomeAltitude);
        connect(controller->missionController(), &MissionController::planViewStateChanged, this,
                &CaacRouteEditorController::selectionChanged);
        connect(controller, &QObject::destroyed, this, [this]() {
            clearHistory();
            emit planMasterControllerChanged();
            emit selectionChanged();
        });
        connect(controller->missionController(), &MissionController::visualItemsReset, this, [this]() {
            if (_rotationPreviewActive) {
                _clearRotationPreview();
            }
            _watchItems();
            QTimer::singleShot(0, this, &CaacRouteEditorController::_syncPlannedHomeAltitude);
            if (!_restoring) {
                _editDepth = 0;
                clearHistory();
            }
        });
        connect(controller->missionController(), &MissionController::globalAltitudeFrameChanged, this,
                &CaacRouteEditorController::_scheduleCapture);
        connect(controller->missionController(), &MissionController::newItemsFromVehicle, this,
                &CaacRouteEditorController::clearHistory);
        _watchItems();
    }
    _watchVehicleHome();
    clearHistory();
    _clearError();
    emit planMasterControllerChanged();
    emit selectionChanged();
    emit rotationCenterChanged();
    emit simulationSettingsChanged();
    emit routeOrientationChanged();
}

SimpleMissionItem* CaacRouteEditorController::currentItem() const
{
    auto* mission = _missionController();
    return mission ? qobject_cast<SimpleMissionItem*>(mission->currentPlanViewItem()) : nullptr;
}

bool CaacRouteEditorController::_fail(const QString& error)
{
    _lastError = error;
    emit lastErrorChanged();
    return false;
}

void CaacRouteEditorController::_clearError()
{
    if (!_lastError.isEmpty()) {
        _lastError.clear();
        emit lastErrorChanged();
    }
}

bool CaacRouteEditorController::_canEdit(bool allowRotationPreview)
{
    _clearError();
    if (!_masterController || !_missionController() || !_missionController()->visualItems() ||
        _missionController()->visualItems()->count() < 1) {
        return _fail(tr("No plan is available."));
    }
    if (_masterController->syncInProgress() || _masterController->property("caacMissionUpdateActive").toBool()) {
        return _fail(tr("Wait for the vehicle transfer to finish before editing."));
    }
    if (_rotationPreviewActive && !allowRotationPreview) {
        return _fail(QCoreApplication::translate(
            "PlanMasterController", "Confirm or cancel the orientation preview before saving or uploading."));
    }
    return true;
}

bool CaacRouteEditorController::_canChangeOrder()
{
    if (!_canEdit()) {
        return false;
    }
    auto* items = _missionController()->visualItems();
    for (int i = 1; i < items->count(); ++i) {
        const auto* item = items->value<SimpleMissionItem*>(i);
        if (item && item->command() == MAV_CMD_DO_JUMP) {
            return _fail(tr("This plan contains jump commands. Use the original mission editor to change its order."));
        }
    }
    return true;
}

bool CaacRouteEditorController::addWaypoint(const QGeoCoordinate& coordinate)
{
    if (!_canChangeOrder()) {
        return false;
    }
    if (!coordinate.isValid()) {
        return _fail(tr("Latitude or longitude is invalid."));
    }
    auto* mission = _missionController();
    const int index = mission->currentPlanViewVIIndex();
    const int insertAt = index >= 0 ? index + 1 : mission->visualItems()->count();
    beginEdit();
    const auto finish = qScopeGuard([this]() { endEdit(); });
    _masterController->setUserSelectedManualCreation(true);
    mission->insertSimpleMissionItem(coordinate, insertAt, true);
    return true;
}

bool CaacRouteEditorController::selectWaypoint(QObject* object)
{
    if (!_canEdit()) {
        return false;
    }
    auto* item = qobject_cast<SimpleMissionItem*>(object);
    if (!item || !item->specifiesCoordinate() || item->isStandaloneCoordinate() ||
        _missionController()->visualItemIndexForObject(item) < 1) {
        return _fail(tr("Select a simple coordinate mission item."));
    }
    _missionController()->setCurrentPlanViewSeqNum(item->sequenceNumber(), true);
    return true;
}

bool CaacRouteEditorController::updateWaypoint(QObject* object, double latitude, double longitude, double altitude,
                                               bool specifySpeed, double speed)
{
    if (!_canEdit()) {
        return false;
    }
    auto* item = qobject_cast<SimpleMissionItem*>(object);
    if (!item || _missionController()->visualItemIndexForObject(item) < 1 || !item->specifiesCoordinate() ||
        item->isStandaloneCoordinate() || !item->specifiesAltitude()) {
        return _fail(tr("This mission item does not support waypoint property editing."));
    }
    const QGeoCoordinate coordinate(latitude, longitude);
    if (!coordinate.isValid() || !std::isfinite(altitude) || altitude < item->altitude()->rawMin().toDouble() ||
        altitude > item->altitude()->rawMax().toDouble()) {
        return _fail(tr("Check latitude, longitude and altitude (metres in the existing altitude frame)."));
    }
    auto* speedSection = item->speedSection();
    if (specifySpeed &&
        (!speedSection || !speedSection->available() || !std::isfinite(speed) || speed <= 0 ||
         speed > std::numeric_limits<float>::max() || speed < speedSection->flightSpeed()->rawMin().toDouble() ||
         speed > speedSection->flightSpeed()->rawMax().toDouble())) {
        return _fail(tr("Flight speed is invalid or unavailable for this mission item."));
    }
    if (speedSection && speedSection->available() && specifySpeed != speedSection->specifyFlightSpeed() &&
        !_canChangeOrder()) {
        return false;
    }
    // Validate every input before changing any Fact; preserve command, frame and other MAVLink parameters.
    beginEdit();
    const auto finish = qScopeGuard([this]() { endEdit(); });
    item->setCoordinate(coordinate);
    item->altitude()->setRawValue(altitude);
    if (speedSection && speedSection->available()) {
        if (specifySpeed) {
            speedSection->flightSpeed()->setRawValue(speed);
        }
        speedSection->setSpecifyFlightSpeed(specifySpeed);
    }
    return true;
}

QVariantMap CaacRouteEditorController::waypointAdvancedProperties(QObject* object) const
{
    const auto* item = qobject_cast<SimpleMissionItem*>(object);
    auto* mission = _missionController();
    if (!item || !mission || mission->visualItemIndexForObject(object) < 1 || item->command() != MAV_CMD_NAV_WAYPOINT) {
        return {};
    }
    const auto& command = item->missionItem();
    const auto settings = item->caacFlightSettings().isEmpty() ? simulationSettings() : item->caacFlightSettings();
    return {{QStringLiteral("supported"), true},
            {QStringLiteral("verticalSpeed"), settings.value("verticalSpeed")},
            {QStringLiteral("turnMode"), settings.value("turnMode")},
            {QStringLiteral("climbMode"), settings.value("climbMode")},
            {QStringLiteral("holdSeconds"), command.param1()},
            {QStringLiteral("specifyYaw"), std::isfinite(command.param4())},
            {QStringLiteral("yaw"), std::isfinite(command.param4()) ? command.param4() : 0}};
}

bool CaacRouteEditorController::updateWaypointProperties(QObject* object, const QVariantMap& properties)
{
    if (!_canEdit()) {
        return false;
    }
    auto* item = qobject_cast<SimpleMissionItem*>(object);
    const auto number = [&properties](const char* key) {
        bool valid = false;
        const double value = properties.value(QLatin1String(key)).toDouble(&valid);
        return valid ? value : std::numeric_limits<double>::quiet_NaN();
    };
    const bool advanced = properties.contains(QStringLiteral("holdSeconds")) ||
                          properties.contains(QStringLiteral("specifyYaw")) ||
                          properties.contains(QStringLiteral("yaw"));
    const double hold = number("holdSeconds");
    const bool specifyYaw = properties.value(QStringLiteral("specifyYaw")).toBool();
    const double yaw = number("yaw");
    const bool legSettingsSpecified = properties.contains(QStringLiteral("verticalSpeed")) ||
                                      properties.contains(QStringLiteral("turnMode")) ||
                                      properties.contains(QStringLiteral("climbMode"));
    const double verticalSpeed = number("verticalSpeed");
    const double turnMode = number("turnMode");
    const double climbMode = number("climbMode");
    if (legSettingsSpecified &&
        (!item || item->command() != MAV_CMD_NAV_WAYPOINT || !std::isfinite(verticalSpeed) || verticalSpeed <= 0 ||
         verticalSpeed > 30 || !std::isfinite(turnMode) || turnMode != std::floor(turnMode) || turnMode < 0 ||
         turnMode > 3 || !std::isfinite(climbMode) || climbMode != std::floor(climbMode) || climbMode < 0 ||
         climbMode > 1)) {
        return _fail(tr("Invalid waypoint simulation settings."));
    }
    if (advanced && (!item || _missionController()->visualItemIndexForObject(item) < 1 ||
                     item->command() != MAV_CMD_NAV_WAYPOINT || !std::isfinite(hold) || hold < 0 ||
                     hold > std::numeric_limits<float>::max() || (specifyYaw && !std::isfinite(yaw)))) {
        return _fail(tr("Enter a nonnegative hold duration and a valid waypoint yaw."));
    }
    const bool vertexSpecified = properties.contains(QStringLiteral("vertexAngle"));
    const auto vertexCoordinate =
        vertexSpecified ? waypointVertexCoordinate(object, number("vertexAngle")) : QGeoCoordinate();
    if (vertexSpecified && !vertexCoordinate.isValid()) {
        return _fail(tr("This angle cannot be reached on the previous leg extension."));
    }
    beginEdit();
    const auto finish = qScopeGuard([this]() { endEdit(); });
    const double altitude = number("altitude");
    const bool specifySpeed = properties.value(QStringLiteral("specifySpeed")).toBool();
    const double speed = number("speed");
    const bool updated =
        vertexSpecified ? updateWaypoint(object, vertexCoordinate.latitude(), vertexCoordinate.longitude(), altitude,
                                         specifySpeed, speed)
        : properties.value(QStringLiteral("relative")).toBool()
            ? updateRelativeWaypoint(object, number("angle"), number("distance"),
                                     properties.value(QStringLiteral("interiorAngle")).toBool(), altitude, specifySpeed,
                                     speed)
            : updateWaypoint(object, number("latitude"), number("longitude"), altitude, specifySpeed, speed);
    if (!updated) {
        return false;
    }
    if (advanced) {
        item->missionItem().setParam1(hold);
        item->missionItem().setParam4(specifyYaw ? std::remainder(yaw, 360) : std::numeric_limits<double>::quiet_NaN());
    }
    if (legSettingsSpecified) {
        item->setCaacFlightSettings({{"verticalSpeed", verticalSpeed},
                                     {"turnMode", static_cast<int>(turnMode)},
                                     {"climbMode", static_cast<int>(climbMode)}});
    }
    return true;
}

VisualMissionItem* CaacRouteEditorController::_previousCoordinateItem(int beforeIndex) const
{
    auto* mission = _missionController();
    auto* model = mission ? mission->visualItems() : nullptr;
    if (!model || beforeIndex < 1 || beforeIndex > model->count()) {
        return nullptr;
    }
    for (int i = beforeIndex - 1; i >= 0; --i) {
        auto* candidate = model->value<VisualMissionItem*>(i);
        if (!candidate) {
            continue;
        }
        // A complex item's entry/exit path cannot be represented by one simple coordinate.
        if (i > 0 && !candidate->isSimpleItem()) {
            return nullptr;
        }
        if (i == 0 || (candidate->specifiesCoordinate() && !candidate->isStandaloneCoordinate())) {
            return candidate->coordinate().isValid() ? candidate : nullptr;
        }
    }
    return nullptr;
}

QVariantMap CaacRouteEditorController::waypointLeg(QObject* object) const
{
    auto* mission = _missionController();
    auto* item = qobject_cast<SimpleMissionItem*>(object);
    if (!mission || !item || !item->specifiesCoordinate() || item->isStandaloneCoordinate() ||
        !item->coordinate().isValid()) {
        return {};
    }
    const int index = mission->visualItemIndexForObject(item);
    auto* previous = _previousCoordinateItem(index);
    if (!previous) {
        return {};
    }
    const double distance = QGCGeo::geodesicDistance(previous->coordinate(), item->coordinate());
    QVariantMap result{{QStringLiteral("distance"), distance},
                       {QStringLiteral("previousNumber"), previous->waypointNumber()}};
    if (distance < 0.001) {
        return result;
    }
    const double bearing = QGCGeo::geodesicAzimuth(previous->coordinate(), item->coordinate());
    result.insert(QStringLiteral("bearing"), bearing);
    auto* incoming = _previousCoordinateItem(mission->visualItemIndexForObject(previous));
    if (incoming && QGCGeo::geodesicDistance(incoming->coordinate(), previous->coordinate()) >= 0.001) {
        result.insert(QStringLiteral("angle"),
                      CaacRouteGeometry::normalizedAngle(
                          bearing - QGCGeo::geodesicAzimuth(previous->coordinate(), incoming->coordinate())));
    }
    return result;
}

QVariantMap CaacRouteEditorController::snapWaypointDrag(QObject* object, const QGeoCoordinate& coordinate,
                                                        double tolerance) const
{
    QVariantMap result{{QStringLiteral("coordinate"), QVariant::fromValue(coordinate)},
                       {QStringLiteral("guides"), QVariantList()}};
    auto* mission = _missionController();
    auto* item = qobject_cast<SimpleMissionItem*>(object);
    const int index = mission && item ? mission->visualItemIndexForObject(item) : -1;
    auto* previous = _previousCoordinateItem(index);
    if (!mission || !item || item->command() != MAV_CMD_NAV_WAYPOINT || !previous || !coordinate.isValid() ||
        !std::isfinite(tolerance) || tolerance <= 0) {
        return result;
    }
    tolerance = std::clamp(tolerance, 0.02, 50.0);
    const auto origin = previous->coordinate();
    const auto project = [&](const QGeoCoordinate& point) {
        const double distance = QGCGeo::geodesicDistance(origin, point);
        if (distance < 0.001) {
            return QPointF();
        }
        const double bearing = qDegreesToRadians(QGCGeo::geodesicAzimuth(origin, point));
        return QPointF(distance * std::sin(bearing), distance * std::cos(bearing));
    };
    const auto unproject = [&](const QPointF& point) {
        auto target = QGCGeo::geodesicDestination(origin, qRadiansToDegrees(std::atan2(point.x(), point.y())),
                                                  std::hypot(point.x(), point.y()));
        target.setAltitude(coordinate.altitude());
        return target;
    };
    const auto raw = project(coordinate);
    const auto* plugin = qobject_cast<CaacCorePlugin*>(QGCCorePlugin::instance());
    const auto preferences = plugin ? plugin->planningPreferences() : QVariantMap();
    const bool horizontalSnap = preferences.value(QStringLiteral("horizontalSnap"), true).toBool();
    const bool verticalSnap = preferences.value(QStringLiteral("verticalSnap"), true).toBool();
    const bool angleSnap = preferences.value(QStringLiteral("angleSnap"), true).toBool();
    const bool symmetrySnap = preferences.value(QStringLiteral("symmetrySnap"), true).toBool();
    const bool midpointSnap = preferences.value(QStringLiteral("midpointSnap"), true).toBool();
    const bool intersectionSnap = preferences.value(QStringLiteral("intersectionSnap"), true).toBool();

    struct Guide
    {
        QPointF anchor;
        QPointF direction;
        QPointF projection;
        double error;
        int kind;
    };

    QList<Guide> candidates;
    const auto addGuide = [&](const QPointF& anchor, const QPointF& vector, int kind, bool cardinal = false) {
        const double length = std::hypot(vector.x(), vector.y());
        if (length < 0.001) {
            return;
        }
        const auto direction = vector / length;
        const auto offset = raw - anchor;
        const auto projection = anchor + direction * QPointF::dotProduct(offset, direction);
        const double error = QLineF(raw, projection).length();
        const double radius = std::hypot(offset.x(), offset.y());
        if (error <= tolerance &&
            (!cardinal || (radius > tolerance * 2 && error <= radius * std::sin(qDegreesToRadians(6.0))))) {
            candidates.append({anchor, direction, projection, error, kind});
        }
    };
    QList<QPointF> references;
    QList<VisualMissionItem*> vertices;
    VisualMissionItem* next = nullptr;
    for (int i = 0; i < mission->visualItems()->count(); ++i) {
        auto* reference = mission->visualItems()->value<VisualMissionItem*>(i);
        if (!reference || !reference->coordinate().isValid() ||
            (i != 0 && (!reference->isSimpleItem() || !reference->specifiesCoordinate() ||
                        reference->isStandaloneCoordinate()))) {
            continue;
        }
        vertices.append(reference);
        if (reference == item) {
            continue;
        }
        const auto point = project(reference->coordinate());
        references.append(point);
        if (horizontalSnap) {
            addGuide(point, QPointF(1, 0), 0, reference == previous);
        }
        if (verticalSnap) {
            addGuide(point, QPointF(0, 1), 0, reference == previous);
        }
        if (i > index && !next) {
            next = reference;
        }
    }
    const bool closed = simulationSettings().value(QStringLiteral("closed")).toBool();
    const int startSequence = simulationSettings().value(QStringLiteral("startSequence")).toInt();
    auto* start = vertices.isEmpty() ? nullptr : vertices.first();
    for (auto* vertex : vertices) {
        if (vertex->sequenceNumber() == startSequence) {
            start = vertex;
            break;
        }
    }
    const int edgeCount = vertices.size() - (closed ? 0 : 1);
    if (midpointSnap) {
        double closest = tolerance;
        QGeoCoordinate midpoint;
        VisualMissionItem* from = nullptr;
        VisualMissionItem* to = nullptr;
        for (int i = 0; i < edgeCount; ++i) {
            auto* a = vertices[i];
            auto* b = i + 1 < vertices.size() ? vertices[i + 1] : start;
            if (a == item || b == item) {
                continue;
            }
            const double distance = QGCGeo::geodesicDistance(a->coordinate(), b->coordinate());
            if (distance < tolerance * 2) {
                continue;
            }
            const auto center = QGCGeo::geodesicDestination(
                a->coordinate(), QGCGeo::geodesicAzimuth(a->coordinate(), b->coordinate()), distance / 2);
            const auto anchor = project(center);
            if (horizontalSnap) {
                addGuide(anchor, QPointF(1, 0), 3);
            }
            if (verticalSnap) {
                addGuide(anchor, QPointF(0, 1), 3);
            }
            const double error = QGCGeo::geodesicDistance(coordinate, center);
            if (error <= closest) {
                closest = error;
                midpoint = center;
                from = a;
                to = b;
            }
        }
        if (midpoint.isValid()) {
            midpoint.setAltitude(coordinate.altitude());
            result[QStringLiteral("coordinate")] = QVariant::fromValue(midpoint);
            result[QStringLiteral("guides")] =
                QVariantList{QVariantMap{{QStringLiteral("from"), QVariant::fromValue(from->coordinate())},
                                         {QStringLiteral("to"), QVariant::fromValue(to->coordinate())},
                                         {QStringLiteral("midpoint"), QVariant::fromValue(midpoint)},
                                         {QStringLiteral("kind"), 3}}};
            return result;
        }
    }
    if (intersectionSnap) {
        double closest = tolerance;
        QGeoCoordinate intersection;
        VisualMissionItem* from = nullptr;
        VisualMissionItem* to = nullptr;
        for (int i = 0; i < edgeCount; ++i) {
            auto* a = vertices[i];
            auto* b = i + 1 < vertices.size() ? vertices[i + 1] : start;
            if (a == item || b == item || a == b) {
                continue;
            }
            const auto anchor = project(a->coordinate());
            const auto vector = project(b->coordinate()) - anchor;
            const double lengthSquared = QPointF::dotProduct(vector, vector);
            if (lengthSquared < 1e-6) {
                continue;
            }
            const double fraction = QPointF::dotProduct(raw - anchor, vector) / lengthSquared;
            if (fraction < 0 || fraction > 1) {
                continue;
            }
            const auto projection = anchor + vector * fraction;
            const double error = QLineF(raw, projection).length();
            if (error <= closest) {
                closest = error;
                intersection = unproject(projection);
                from = a;
                to = b;
            }
        }
        if (intersection.isValid()) {
            result[QStringLiteral("coordinate")] = QVariant::fromValue(intersection);
            result[QStringLiteral("guides")] =
                QVariantList{QVariantMap{{QStringLiteral("from"), QVariant::fromValue(from->coordinate())},
                                         {QStringLiteral("to"), QVariant::fromValue(to->coordinate())},
                                         {QStringLiteral("intersection"), QVariant::fromValue(intersection)},
                                         {QStringLiteral("kind"), 4}}};
            return result;
        }
    }
    const auto perpendicular = [](const QPointF& point) { return QPointF(-point.y(), point.x()); };
    if (angleSnap && std::hypot(raw.x(), raw.y()) > tolerance * 2) {
        const double increment = preferences.value(QStringLiteral("snapAngle"), 15.0).toDouble();
        const double bearing = std::round(qRadiansToDegrees(std::atan2(raw.x(), raw.y())) / increment) * increment;
        const double axis = std::abs(std::remainder(bearing, 180.0));
        if ((verticalSnap || axis > 1e-6) && (horizontalSnap || std::abs(axis - 90) > 1e-6)) {
            const double radians = qDegreesToRadians(bearing);
            addGuide(QPointF(), QPointF(std::sin(radians), std::cos(radians)), 0, true);
        }
    }
    auto* incoming = _previousCoordinateItem(mission->visualItemIndexForObject(previous));
    if (incoming && angleSnap) {
        const auto direction = -project(incoming->coordinate());
        addGuide(QPointF(), perpendicular(direction), 1);
        if (next) {
            addGuide(project(next->coordinate()), direction, 1);
        }
    }
    if (next && symmetrySnap) {
        const auto opposite = project(next->coordinate());
        addGuide(opposite / 2, perpendicular(opposite), 2);
    } else if (incoming && symmetrySnap) {
        const auto opposite = project(incoming->coordinate());
        addGuide(opposite / 2, perpendicular(opposite), 2);
    }
    if (candidates.isEmpty()) {
        return result;
    }
    std::stable_sort(candidates.begin(), candidates.end(), [](const Guide& a, const Guide& b) {
        // Pairwise epsilon equality is not transitive and violates the sort ordering contract.
        return a.error == b.error ? a.kind > b.kind : a.error < b.error;
    });
    auto snapped = candidates.first().projection;
    QList<Guide> active{candidates.first()};
    for (int i = 1; i < candidates.size(); ++i) {
        const auto& a = candidates.first();
        const auto& b = candidates[i];
        const double determinant = a.direction.x() * b.direction.y() - a.direction.y() * b.direction.x();
        if (std::abs(determinant) < 0.5) {
            continue;
        }
        const auto offset = b.anchor - a.anchor;
        const auto intersection =
            a.anchor + a.direction * ((offset.x() * b.direction.y() - offset.y() * b.direction.x()) / determinant);
        if (QLineF(raw, intersection).length() > tolerance ||
            std::any_of(references.cbegin(), references.cend(),
                        [&](const QPointF& point) { return QLineF(point, intersection).length() < tolerance * 0.5; })) {
            continue;
        }
        snapped = intersection;
        active.append(b);
        break;
    }
    QVariantList guides;
    for (const auto& guide : active) {
        const double extent = std::max(QLineF(guide.anchor, snapped).length() * 1.2, tolerance * 8);
        guides.append(QVariantMap{
            {QStringLiteral("from"), QVariant::fromValue(unproject(guide.anchor - guide.direction * extent))},
            {QStringLiteral("to"), QVariant::fromValue(unproject(guide.anchor + guide.direction * extent))},
            {QStringLiteral("kind"), guide.kind}});
    }
    result.insert(QStringLiteral("coordinate"), QVariant::fromValue(unproject(snapped)));
    result.insert(QStringLiteral("guides"), guides);
    return result;
}

QGeoCoordinate CaacRouteEditorController::waypointVertexCoordinate(QObject* object, double angle) const
{
    auto* item = qobject_cast<SimpleMissionItem*>(object);
    if (!item || !std::isfinite(angle) || angle < 0 || angle > 180) {
        return {};
    }
    QVariantMap vertex;
    for (const auto& entry : waypointAngles(simulationSettings().value("closed").toBool())) {
        const auto candidate = entry.toMap();
        if (candidate.value("sequence").toInt() == item->sequenceNumber()) {
            vertex = candidate;
            break;
        }
    }
    if (vertex.isEmpty()) {
        return {};
    }
    if (std::abs(vertex.value("angle").toDouble() - angle) < 1e-8) {
        return item->coordinate();
    }
    if (angle <= 0 || angle >= 180) {
        return {};
    }
    const auto previous = vertex.value("previous").value<QGeoCoordinate>();
    const auto next = vertex.value("next").value<QGeoCoordinate>();
    const double bearing = QGCGeo::geodesicAzimuth(previous, item->coordinate());
    const double side = QGCGeo::geodesicDistance(previous, next);
    const double delta = qDegreesToRadians(std::remainder(QGCGeo::geodesicAzimuth(previous, next) - bearing, 360.0));
    const double cross = std::abs(side * std::sin(delta));
    if (cross < 0.01) {
        return {};
    }
    double distance = side * std::cos(delta) + cross / std::tan(qDegreesToRadians(angle));
    const auto residual = [&](double metres) {
        const auto point = QGCGeo::geodesicDestination(previous, bearing, metres);
        return std::abs(std::remainder(QGCGeo::geodesicAzimuth(point, next) - QGCGeo::geodesicAzimuth(point, previous),
                                       360.0)) -
               angle;
    };
    for (int iteration = 0; iteration < 24; ++iteration) {
        if (!std::isfinite(distance) || distance < 0.01 || distance > 20000000) {
            return {};
        }
        const double error = residual(distance);
        if (std::abs(error) < 1e-7) {
            auto point = QGCGeo::geodesicDestination(previous, bearing, distance);
            point.setAltitude(item->coordinate().altitude());
            return point;
        }
        const double step = std::min(distance / 2, std::max(0.01, distance * 1e-5));
        const double derivative = (residual(distance + step) - residual(distance - step)) / (2 * step);
        if (!std::isfinite(derivative) || std::abs(derivative) < 1e-12) {
            return {};
        }
        distance -= std::clamp(error / derivative, -distance * 0.5, distance * 0.5);
    }
    return {};
}

QVariantList CaacRouteEditorController::waypointAngles(bool closed) const
{
    QVariantList result;
    auto* mission = _missionController();
    if (!mission) {
        return result;
    }
    QList<VisualMissionItem*> points;
    QList<SimpleMissionItem*> waypoints;
    for (int i = 0; i < mission->visualItems()->count(); ++i) {
        auto* point = mission->visualItems()->value<VisualMissionItem*>(i);
        if (point && point->coordinate().isValid() &&
            (i == 0 || (point->isSimpleItem() && point->specifiesCoordinate() && !point->isStandaloneCoordinate()))) {
            points.append(point);
            if (auto* simple = qobject_cast<SimpleMissionItem*>(point);
                simple && simple->command() == MAV_CMD_NAV_WAYPOINT) {
                waypoints.append(simple);
            }
        }
    }
    for (auto* item : waypoints) {
        const int index = points.indexOf(item);
        auto* previous = index > 0 ? points[index - 1] : nullptr;
        auto* next = index + 1 < points.size() ? points[index + 1] : nullptr;
        if (closed && waypoints.size() > 2) {
            if (item == waypoints.first()) {
                previous = waypoints.last();
            }
            if (item == waypoints.last()) {
                next = waypoints.first();
            }
        }
        const auto vertex = item->coordinate();
        if (!previous || !next || QGCGeo::geodesicDistance(vertex, previous->coordinate()) < 0.01 ||
            QGCGeo::geodesicDistance(vertex, next->coordinate()) < 0.01) {
            continue;
        }
        const double from = QGCGeo::geodesicAzimuth(vertex, previous->coordinate());
        const double to = QGCGeo::geodesicAzimuth(vertex, next->coordinate());
        result.append(QVariantMap{{QStringLiteral("sequence"), item->sequenceNumber()},
                                  {QStringLiteral("coordinate"), QVariant::fromValue(vertex)},
                                  {QStringLiteral("previous"), QVariant::fromValue(previous->coordinate())},
                                  {QStringLiteral("next"), QVariant::fromValue(next->coordinate())},
                                  {QStringLiteral("angle"), std::abs(std::remainder(to - from, 360.0))}});
    }
    return result;
}

bool CaacRouteEditorController::applyWaypointRelation(int targetIndex, int referenceIndex, int secondIndex,
                                                      bool extendedLeg, double bearing, double distance,
                                                      bool moveWholeRoute, double takeoffHeight)
{
    if (!_canEdit() || _rotationPreviewActive) {
        return false;
    }
    auto* model = _missionController()->visualItems();
    const auto pointAt = [&](int index) -> VisualMissionItem* {
        if (index < 0 || index >= model->count()) {
            return nullptr;
        }
        auto* point = model->value<VisualMissionItem*>(index);
        return point && point->coordinate().isValid() &&
                       (index == 0 ||
                        (point->isSimpleItem() && point->specifiesCoordinate() && !point->isStandaloneCoordinate()))
                   ? point
                   : nullptr;
    };
    auto* target = pointAt(targetIndex);
    auto* reference = pointAt(referenceIndex);
    auto* simple = qobject_cast<SimpleMissionItem*>(target);
    if (!target || !reference || target == reference ||
        (targetIndex > 0 && (!simple || simple->command() != MAV_CMD_NAV_WAYPOINT)) || !std::isfinite(bearing) ||
        !std::isfinite(distance) || distance < 0 || distance > 20000000) {
        return _fail(tr("Select a simple coordinate reference point."));
    }
    auto anchor = reference->coordinate();
    if (extendedLeg) {
        auto* second = pointAt(secondIndex);
        if (!second || second == target || QGCGeo::geodesicDistance(anchor, second->coordinate()) < 0.01) {
            return _fail(tr("Select a simple coordinate reference point."));
        }
        bearing = QGCGeo::geodesicAzimuth(second->coordinate(), anchor) + 180;
        anchor = second->coordinate();
    }
    auto coordinate = QGCGeo::geodesicDestination(anchor, bearing, distance);
    coordinate.setAltitude(target->coordinate().altitude());
    if (!coordinate.isValid()) {
        return _fail(tr("Enter valid reference and target coordinates."));
    }
    auto settings = simulationSettings();
    if (takeoffHeight != -1) {
        settings.insert(QStringLiteral("takeoffHeight"), takeoffHeight);
        const auto error = CaacSimulationController::validateSettings(settings);
        if (!error.isEmpty()) {
            return _fail(error);
        }
    }
    beginEdit();
    const auto finish = qScopeGuard([this]() { endEdit(); });
    if (moveWholeRoute) {
        if (!translateRoute(target->coordinate(), coordinate)) {
            return false;
        }
    } else if (targetIndex == 0) {
        if (!updatePlannedHome(coordinate.latitude(), coordinate.longitude(), coordinate.altitude(), false)) {
            return false;
        }
    } else {
        target->setCoordinate(coordinate);
    }
    if (takeoffHeight != -1) {
        return setSimulationSettings(settings);
    }
    return true;
}

bool CaacRouteEditorController::updateWaypointLeg(QObject* object, double bearing, double distance)
{
    if (!_canEdit() || _rotationPreviewActive) {
        return false;
    }
    auto* item = qobject_cast<SimpleMissionItem*>(object);
    auto* mission = _missionController();
    const int index = item ? mission->visualItemIndexForObject(item) : -1;
    auto* previous = _previousCoordinateItem(index);
    if (!item || item->command() != MAV_CMD_NAV_WAYPOINT || !previous || !std::isfinite(bearing) ||
        !std::isfinite(distance) || distance < 0 || distance > 20000000) {
        return _fail(tr("Enter a valid bearing and distance in metres for an ordinary waypoint."));
    }
    const auto target = distance == 0 ? std::optional<QGeoCoordinate>(previous->coordinate())
                                      : CaacRouteGeometry::relativePoint(previous->coordinate(), bearing, distance);
    if (!target) {
        return _fail(tr("Enter a valid bearing and distance in metres for an ordinary waypoint."));
    }
    auto coordinate = *target;
    coordinate.setAltitude(item->coordinate().altitude());
    beginEdit();
    const auto finish = qScopeGuard([this]() { endEdit(); });
    item->setCoordinate(coordinate);
    return true;
}

bool CaacRouteEditorController::clearWaypoints()
{
    if (!_canChangeOrder() || _rotationPreviewActive) {
        return false;
    }
    auto* mission = _missionController();
    beginEdit();
    const auto finish = qScopeGuard([this]() { endEdit(); });
    for (int i = mission->visualItems()->count() - 1; i > 0; --i) {
        mission->removeVisualItem(i);
    }
    return true;
}

bool CaacRouteEditorController::reverseWaypoints()
{
    if (!_canChangeOrder()) {
        return false;
    }
    auto* mission = _missionController();
    auto* model = mission->visualItems();
    int first = 1;
    int last = model->count() - 1;
    if (const auto* takeoff = first <= last ? model->value<SimpleMissionItem*>(first) : nullptr;
        takeoff && (takeoff->command() == MAV_CMD_NAV_TAKEOFF || takeoff->command() == MAV_CMD_NAV_VTOL_TAKEOFF)) {
        ++first;
    }
    if (const auto* landing = first <= last ? model->value<SimpleMissionItem*>(last) : nullptr;
        landing && (landing->command() == MAV_CMD_NAV_LAND || landing->command() == MAV_CMD_NAV_VTOL_LAND ||
                    landing->command() == MAV_CMD_NAV_RETURN_TO_LAUNCH)) {
        --last;
    }
    for (int i = first; i <= last; ++i) {
        const auto* item = model->value<SimpleMissionItem*>(i);
        if (!item || item->command() != MAV_CMD_NAV_WAYPOINT) {
            return _fail(tr("Route reversal requires ordinary waypoints only."));
        }
    }
    beginEdit();
    const auto finish = qScopeGuard([this]() { endEdit(); });
    for (int i = first; i < last; ++i) {
        if (!mission->moveVisualItem(last, i)) {
            return false;
        }
    }
    return true;
}

bool CaacRouteEditorController::centerHomeOnRoute()
{
    if (!_canEdit()) {
        return false;
    }
    auto* mission = _missionController();
    auto* model = mission->visualItems();
    QList<QGeoCoordinate> coordinates;
    for (int i = 1; i < model->count(); ++i) {
        auto* item = model->value<SimpleMissionItem*>(i);
        if (item && item->command() == MAV_CMD_NAV_WAYPOINT && item->coordinate().isValid()) {
            coordinates.append(item->coordinate());
        }
    }
    if (coordinates.isEmpty()) {
        return _fail(tr("Add a waypoint before centering the takeoff point."));
    }
    const auto center = QGeoRectangle(coordinates).center();
    const auto previous = mission->plannedHomePosition();
    beginEdit();
    const auto finish = qScopeGuard([this]() { endEdit(); });
    if (!updatePlannedHome(center.latitude(), center.longitude(), previous.altitude(), false)) {
        return false;
    }
    for (int i = 1; i < model->count(); ++i) {
        auto* item = model->value<SimpleMissionItem*>(i);
        if (item && item->isTakeoffItem() && QGCGeo::geodesicDistance(item->coordinate(), previous) < 0.01) {
            auto position = center;
            position.setAltitude(item->coordinate().altitude());
            item->setCoordinate(position);
        }
    }
    return true;
}

bool CaacRouteEditorController::useWaypointAsHome(QObject* object)
{
    if (!_canChangeOrder() || _rotationPreviewActive) {
        return false;
    }
    auto* item = qobject_cast<SimpleMissionItem*>(object);
    auto* mission = _missionController();
    if (!item || mission->visualItemIndexForObject(item) < 1 || item->command() != MAV_CMD_NAV_WAYPOINT) {
        return _fail(tr("Select an ordinary waypoint to use as the takeoff point."));
    }
    const auto position = item->coordinate();
    const double altitude = mission->plannedHomePosition().altitude();
    beginEdit();
    const auto finish = qScopeGuard([this]() { endEdit(); });
    if (!updatePlannedHome(position.latitude(), position.longitude(), altitude, false)) {
        return false;
    }
    mission->removeVisualItem(mission->visualItemIndexForObject(item));
    return true;
}

bool CaacRouteEditorController::useWaypointAsLanding(QObject* object)
{
    if (!_canChangeOrder()) {
        return false;
    }
    auto* point = qobject_cast<SimpleMissionItem*>(object);
    auto* mission = _missionController();
    auto* model = mission->visualItems();
    if (!point || mission->visualItemIndexForObject(point) < 1 || point->command() != MAV_CMD_NAV_WAYPOINT) {
        return _fail(tr("Select an ordinary waypoint as the landing point."));
    }
    for (int i = 1; i < model->count(); ++i) {
        if (!model->value<SimpleMissionItem*>(i)) {
            return _fail(tr("Set the landing point in the original editor for complex missions."));
        }
    }
    const auto coordinate = point->coordinate();
    beginEdit();
    const auto finish = qScopeGuard([this]() { endEdit(); });
    for (int i = model->count() - 1; i > 0; --i) {
        const auto* item = model->value<SimpleMissionItem*>(i);
        if (item->isLandCommand() || item->command() == MAV_CMD_NAV_RETURN_TO_LAUNCH) {
            mission->removeVisualItem(i);
        }
    }
    auto* landing =
        qobject_cast<SimpleMissionItem*>(mission->insertSimpleMissionItem(coordinate, model->count(), true));
    if (!landing) {
        return _fail(tr("Unable to create the landing point."));
    }
    landing->setCommand(MAV_CMD_NAV_LAND);
    landing->setAltitudeFrame(QGroundControlQmlGlobal::AltitudeFrameRelative);
    landing->altitude()->setRawValue(0);
    return true;
}

bool CaacRouteEditorController::updateRelativeWaypoint(QObject* object, double angle, double distance,
                                                       bool interiorAngle, double altitude, bool specifySpeed,
                                                       double speed)
{
    if (!_canEdit()) {
        return false;
    }
    auto* item = qobject_cast<SimpleMissionItem*>(object);
    auto* mission = _missionController();
    const int index = item ? mission->visualItemIndexForObject(item) : -1;
    auto* previous = _previousCoordinateItem(index);
    if (!item || index < 1 || !item->specifiesCoordinate() || item->isStandaloneCoordinate() || !previous) {
        return _fail(tr("This waypoint has no valid simple previous point."));
    }
    std::optional<QGeoCoordinate> coordinate;
    if (interiorAngle) {
        auto* incoming = _previousCoordinateItem(mission->visualItemIndexForObject(previous));
        if (!incoming) {
            return _fail(tr("Interior angle requires an incoming leg; planned home alone has none."));
        }
        coordinate =
            CaacRouteGeometry::interiorAnglePoint(incoming->coordinate(), previous->coordinate(), angle, distance);
    } else {
        coordinate = CaacRouteGeometry::relativePoint(previous->coordinate(), angle, distance);
    }
    if (!coordinate) {
        return _fail(tr("Check the reference point, angle and positive distance in metres."));
    }
    return updateWaypoint(item, coordinate->latitude(), coordinate->longitude(), altitude, specifySpeed, speed);
}

bool CaacRouteEditorController::removeCurrentWaypoint()
{
    if (!_canChangeOrder()) {
        return false;
    }
    auto* item = currentItem();
    if (!item || (item->command() != MAV_CMD_NAV_WAYPOINT && !item->isLandCommand()) ||
        _missionController()->visualItemIndexForObject(item) < 1) {
        return _fail(tr("Select a simple coordinate mission item."));
    }
    beginEdit();
    const auto finish = qScopeGuard([this]() { endEdit(); });
    _missionController()->removeVisualItem(_missionController()->visualItemIndexForObject(item));
    return true;
}

bool CaacRouteEditorController::removeWaypoints(const QVariantList& waypoints)
{
    if (!_canChangeOrder()) {
        return false;
    }
    auto* mission = _missionController();
    QSet<int> indices;
    if (waypoints.isEmpty()) {
        return _fail(tr("Only ordinary waypoints can be removed here."));
    }
    for (const auto& waypoint : waypoints) {
        auto* item = qobject_cast<SimpleMissionItem*>(waypoint.value<QObject*>());
        const int index = item ? mission->visualItemIndexForObject(item) : -1;
        if (!item || index < 1 || item->command() != MAV_CMD_NAV_WAYPOINT || indices.contains(index)) {
            return _fail(tr("Only ordinary waypoints can be removed here."));
        }
        indices.insert(index);
    }
    auto ordered = indices.values();
    std::sort(ordered.begin(), ordered.end(), [](int indexA, int indexB) { return indexA > indexB; });
    beginEdit();
    const auto finish = qScopeGuard([this]() { endEdit(); });
    for (int index : ordered) {
        mission->removeVisualItem(index);
    }
    return true;
}

bool CaacRouteEditorController::moveCurrentWaypoint(int offset)
{
    if (!_canChangeOrder()) {
        return false;
    }
    if (offset != -1 && offset != 1) {
        return _fail(tr("Move one waypoint up or down at a time."));
    }
    const int from = _missionController()->currentPlanViewVIIndex();
    beginEdit();
    const auto finish = qScopeGuard([this]() { endEdit(); });
    if (!_missionController()->moveVisualItem(from, from + offset)) {
        return _fail(tr("Waypoints can only move past adjacent ordinary waypoints."));
    }
    return true;
}

QVariantList CaacRouteEditorController::coordinateItems() const
{
    QVariantList result;
    auto* mission = _missionController();
    if (!mission || !mission->visualItems()) {
        return result;
    }
    for (int i = 0; i < mission->visualItems()->count(); ++i) {
        auto* item = mission->visualItems()->value<VisualMissionItem*>(i);
        if (item &&
            (i == 0 || (item->isSimpleItem() && item->specifiesCoordinate() && !item->isStandaloneCoordinate()))) {
            result.append(
                QVariantMap{{QStringLiteral("index"), i},
                            {QStringLiteral("command"),
                             item->isSimpleItem() ? qobject_cast<SimpleMissionItem*>(item)->command() : 0},
                            {QStringLiteral("label"),
                             i == 0 ? tr("Planned home")
                                    : tr("Waypoint %1 · %2").arg(item->waypointNumber()).arg(item->commandName())},
                            {QStringLiteral("coordinate"), QVariant::fromValue(item->coordinate())}});
        }
    }
    return result;
}

bool CaacRouteEditorController::setRotationCenter(const QGeoCoordinate& coordinate)
{
    if (!_canEdit()) {
        return false;
    }
    if (!coordinate.isValid()) {
        return _fail(tr("Enter a valid rotation centre."));
    }
    beginEdit();
    _setCenter(coordinate);
    _missionController()->setDirty(true);
    endEdit();
    return true;
}

bool CaacRouteEditorController::_rotationCoordinates(QList<QGeoCoordinate>& coordinates, QList<QObject*>& items)
{
    if (!_canEdit()) {
        return false;
    }
    auto* model = _missionController()->visualItems();
    for (int i = 0; i < model->count(); ++i) {
        auto* item = model->value<VisualMissionItem*>(i);
        if (!item || (i > 0 && !item->isSimpleItem())) {
            return _fail(tr("Convert complex survey or landing patterns to simple items before rotating."));
        }
        if (i == 0 || item->specifiesCoordinate()) {
            if (!item->coordinate().isValid()) {
                return _fail(tr("Every route coordinate, including planned home, must be valid."));
            }
            coordinates.append(item->coordinate());
            items.append(item);
        }
    }
    if (items.size() < 2) {
        return _fail(tr("Create at least one coordinate waypoint before rotating."));
    }
    return true;
}

bool CaacRouteEditorController::rotateRoute(double clockwiseDegrees)
{
    return rotateRouteAround(_rotationCenter, clockwiseDegrees);
}

bool CaacRouteEditorController::rotateRouteAround(const QGeoCoordinate& center, double clockwiseDegrees,
                                                  bool includeHome)
{
    QList<QGeoCoordinate> coordinates;
    QList<QObject*> items;
    if (!_rotationCoordinates(coordinates, items)) {
        return false;
    }
    const auto result = CaacRouteGeometry::rotate(coordinates, center, clockwiseDegrees);
    if (!result) {
        return _fail(tr("Check the rotation centre and angle."));
    }
    beginEdit();
    const auto finish = qScopeGuard([this]() { endEdit(); });
    _setCenter(center);
    _masterController->setProperty("caacRouteOrientation",
                                   CaacRouteGeometry::normalizedAngle(routeOrientation() + clockwiseDegrees));
    for (int i = 0; i < items.size(); ++i) {
        auto* item = qobject_cast<VisualMissionItem*>(items[i]);
        if (includeHome || !item->homePosition()) {
            item->setCoordinate(result->at(i));
        }
    }
    _missionController()->setDirty(true);
    return true;
}

bool CaacRouteEditorController::alignRoute(int fromIndex, int toIndex, double targetBearing)
{
    return alignRouteAround(_rotationCenter, fromIndex, toIndex, targetBearing);
}

bool CaacRouteEditorController::alignRouteAround(const QGeoCoordinate& center, int fromIndex, int toIndex,
                                                 double targetBearing, bool includeHome)
{
    if (!_canEdit()) {
        return false;
    }
    auto* model = _missionController()->visualItems();
    if (fromIndex < 0 || toIndex < 0 || fromIndex >= model->count() || toIndex >= model->count()) {
        return _fail(tr("Select two existing coordinate points."));
    }
    auto* a = model->value<VisualMissionItem*>(fromIndex);
    auto* b = model->value<VisualMissionItem*>(toIndex);
    if (!a || !b || (fromIndex > 0 && !a->specifiesCoordinate()) || (toIndex > 0 && !b->specifiesCoordinate())) {
        return _fail(tr("Select two existing coordinate points."));
    }
    if (!includeHome && (fromIndex == 0 || toIndex == 0) &&
        QGCGeo::geodesicDistance(center, _missionController()->plannedHomePosition()) > 0.001) {
        return _fail(tr("When H is fixed, select two waypoints or use H as the rotation center."));
    }
    const auto angle = CaacRouteGeometry::alignmentAngle(a->coordinate(), b->coordinate(), center, targetBearing);
    if (!angle) {
        return _fail(tr("The two points must be distinct and the target bearing must be valid."));
    }
    return rotateRouteAround(center, *angle, includeHome);
}

bool CaacRouteEditorController::insertRelativeWaypoint(int referenceIndex, int afterIndex, double angle,
                                                       double distance, bool interiorAngle)
{
    if (!_canChangeOrder()) {
        return false;
    }
    auto* mission = _missionController();
    auto* model = mission->visualItems();
    if (referenceIndex < 0 || referenceIndex >= model->count() || afterIndex < 0 || afterIndex >= model->count()) {
        return _fail(tr("Select a reference point and an insertion position."));
    }
    auto* reference = model->value<VisualMissionItem*>(referenceIndex);
    if (!reference || (referenceIndex > 0 && (!reference->isSimpleItem() || !reference->specifiesCoordinate() ||
                                              reference->isStandaloneCoordinate()))) {
        return _fail(tr("Select a simple coordinate reference point."));
    }
    std::optional<QGeoCoordinate> coordinate;
    if (interiorAngle) {
        VisualMissionItem* previous = nullptr;
        for (int i = referenceIndex - 1; i >= 0; --i) {
            auto* candidate = model->value<VisualMissionItem*>(i);
            if (candidate && (i == 0 || (candidate->specifiesCoordinate() && !candidate->isStandaloneCoordinate()))) {
                previous = candidate;
                break;
            }
        }
        if (!previous) {
            return _fail(tr("Interior angle requires an incoming leg; planned home alone has none."));
        }
        coordinate =
            CaacRouteGeometry::interiorAnglePoint(previous->coordinate(), reference->coordinate(), angle, distance);
    } else {
        coordinate = CaacRouteGeometry::relativePoint(reference->coordinate(), angle, distance);
    }
    if (!coordinate) {
        return _fail(tr("Check the reference point, angle and positive distance in metres."));
    }
    beginEdit();
    const auto finish = qScopeGuard([this]() { endEdit(); });
    _masterController->setUserSelectedManualCreation(true);
    mission->insertSimpleMissionItem(*coordinate, afterIndex + 1, true);
    return true;
}

bool CaacRouteEditorController::setRelativeEndpoint(int referenceIndex, double angle, double distance, bool landing,
                                                    bool interiorAngle)
{
    if (!_canChangeOrder()) {
        return false;
    }
    auto* mission = _missionController();
    auto* model = mission->visualItems();
    if (referenceIndex < 0 || referenceIndex >= model->count()) {
        return _fail(tr("Select a simple coordinate reference point."));
    }
    auto* reference = model->value<VisualMissionItem*>(referenceIndex);
    if (!reference || (referenceIndex > 0 && (!reference->isSimpleItem() || !reference->specifiesCoordinate() ||
                                              reference->isStandaloneCoordinate()))) {
        return _fail(tr("Select a simple coordinate reference point."));
    }
    std::optional<QGeoCoordinate> coordinate;
    if (interiorAngle) {
        auto* previous = _previousCoordinateItem(referenceIndex);
        if (!previous) {
            return _fail(tr("Interior angle requires an incoming leg; planned home alone has none."));
        }
        coordinate = CaacRouteGeometry::interiorAnglePoint(previous->coordinate(), reference->coordinate(), angle,
                                                           distance == 0 ? 1 : distance);
        if (coordinate && distance == 0) {
            coordinate = reference->coordinate();
        }
    } else {
        coordinate = distance == 0 && std::isfinite(angle) && reference->coordinate().isValid()
                         ? std::optional<QGeoCoordinate>(reference->coordinate())
                         : CaacRouteGeometry::relativePoint(reference->coordinate(), angle, distance);
    }
    if (!coordinate) {
        return _fail(tr("Check the reference point, angle and positive distance in metres."));
    }
    beginEdit();
    const auto finish = qScopeGuard([this]() { endEdit(); });
    _masterController->setUserSelectedManualCreation(true);
    if (!landing) {
        const auto altitude = mission->plannedHomePosition().altitude();
        if (!updatePlannedHome(coordinate->latitude(), coordinate->longitude(), altitude, false)) {
            return false;
        }
        auto takeoffCoordinate = *coordinate;
        takeoffCoordinate.setAltitude(altitude);
        for (int i = 1; i < model->count(); ++i) {
            auto* item = model->value<SimpleMissionItem*>(i);
            if (item && item->isTakeoffItem()) {
                item->setCoordinate(takeoffCoordinate);
            }
        }
        return true;
    }
    for (int i = model->count() - 1; i > 0; --i) {
        auto* item = model->value<SimpleMissionItem*>(i);
        if (item && item->isLandCommand()) {
            item->setCoordinate(*coordinate);
            return true;
        }
    }
    auto* item = qobject_cast<SimpleMissionItem*>(mission->insertSimpleMissionItem(*coordinate, model->count(), true));
    if (!item) {
        return _fail(tr("Unable to create the landing point."));
    }
    item->setCommand(MAV_CMD_NAV_LAND);
    item->setAltitudeFrame(QGroundControlQmlGlobal::AltitudeFrameRelative);
    item->altitude()->setRawValue(0);
    return true;
}

QVariantList CaacRouteEditorController::templateCoordinates(int shape, int referenceIndex,
                                                            const QVariantMap& options) const
{
    auto* mission = _missionController();
    auto* model = mission ? mission->visualItems() : nullptr;
    if (!model || referenceIndex < 0 || referenceIndex >= model->count()) {
        return {};
    }
    auto* reference = model->value<VisualMissionItem*>(referenceIndex);
    if (!reference || (referenceIndex > 0 && (!reference->isSimpleItem() || !reference->specifiesCoordinate() ||
                                              reference->isStandaloneCoordinate()))) {
        return {};
    }
    const double count = options.value(QStringLiteral("count"), 10).toDouble();
    if (!std::isfinite(count) || count < 0 || count > 1000 || std::floor(count) != count) {
        return {};
    }
    const CaacRouteTemplates::Options parameters{
        .shape = static_cast<CaacRouteTemplates::Shape>(shape),
        .size = options.value(QStringLiteral("size"), 100).toDouble(),
        .width = options.value(QStringLiteral("width"), 50).toDouble(),
        .count = static_cast<int>(count),
        .clockwise = options.value(QStringLiteral("clockwise"), true).toBool(),
        .vertexTop = options.value(QStringLiteral("vertexTop"), false).toBool(),
        .start = options.value(QStringLiteral("start"), 0).toInt(),
        .bearing = options.value(QStringLiteral("bearing"), 0).toDouble(),
        .close = options.value(QStringLiteral("close"), false).toBool()};
    const auto coordinates = CaacRouteTemplates::generate(reference->coordinate(), parameters);
    QVariantList result;
    if (coordinates) {
        for (const auto& coordinate : *coordinates) {
            result.append(QVariant::fromValue(coordinate));
        }
    }
    return result;
}

bool CaacRouteEditorController::insertTemplate(int shape, int referenceIndex, const QVariantMap& options)
{
    if (!_canChangeOrder()) {
        return false;
    }
    const auto coordinates = templateCoordinates(shape, referenceIndex, options);
    if (coordinates.isEmpty()) {
        return _fail(tr("Check the template anchor, dimensions and waypoint count (maximum 1000)."));
    }
    beginEdit();
    const auto finish = qScopeGuard([this]() { endEdit(); });
    _masterController->setUserSelectedManualCreation(true);
    _masterController->setProperty("caacRouteOrientation",
                                   CaacRouteGeometry::normalizedAngle(options.value("bearing", 0).toDouble()));
    for (int i = 0; i < coordinates.size(); ++i) {
        _missionController()->insertSimpleMissionItem(coordinates[i].value<QGeoCoordinate>(), referenceIndex + i + 1,
                                                      i == coordinates.size() - 1);
    }
    return true;
}

bool CaacRouteEditorController::translateRoute(const QGeoCoordinate& reference, const QGeoCoordinate& target,
                                               bool includeHome)
{
    QList<QGeoCoordinate> coordinates;
    QList<QObject*> items;
    if (!_rotationCoordinates(coordinates, items)) {
        return false;
    }
    if (!reference.isValid() || !target.isValid()) {
        return _fail(tr("Enter valid reference and target coordinates."));
    }
    QList<QGeoCoordinate> result;
    for (const auto& coordinate : coordinates) {
        auto shifted = QGCGeo::geodesicDestination(target, QGCGeo::geodesicAzimuth(reference, coordinate),
                                                   QGCGeo::geodesicDistance(reference, coordinate));
        shifted.setAltitude(coordinate.altitude());
        if (!shifted.isValid()) {
            return _fail(tr("Unable to translate this route."));
        }
        result.append(shifted);
    }
    beginEdit();
    const auto finish = qScopeGuard([this]() { endEdit(); });
    for (int i = 0; i < items.size(); ++i) {
        auto* item = qobject_cast<VisualMissionItem*>(items[i]);
        if (includeHome || !item->homePosition()) {
            item->setCoordinate(result[i]);
        }
    }
    _missionController()->setDirty(true);
    return true;
}

bool CaacRouteEditorController::moveRouteBy(double bearing, double distance, bool includeHome)
{
    auto* mission = _missionController();
    if (!_canEdit() || !mission) {
        return false;
    }
    const auto reference = mission->plannedHomePosition();
    const auto target = CaacRouteGeometry::relativePoint(reference, bearing, distance);
    return target ? translateRoute(reference, *target, includeHome)
                  : _fail(tr("Enter a valid bearing and positive distance."));
}

bool CaacRouteEditorController::updatePlannedHome(double latitude, double longitude, double amslAltitude,
                                                  bool manualAltitude)
{
    if (!_canEdit()) {
        return false;
    }
    auto* home = _missionController()->visualItems()->value<MissionSettingsItem*>(0);
    const QGeoCoordinate coordinate(latitude, longitude, amslAltitude);
    if (!home || !coordinate.isValid() || !std::isfinite(amslAltitude) ||
        amslAltitude < home->plannedHomePositionAltitude()->rawMin().toDouble() ||
        amslAltitude > home->plannedHomePositionAltitude()->rawMax().toDouble()) {
        return _fail(tr("Check the planned home coordinates and AMSL altitude."));
    }
    beginEdit();
    const auto finish = qScopeGuard([this]() { endEdit(); });
    if (manualAltitude) {
        home->setProperty("caacHomeAltitudeManual", true);
    }
    _missionController()->setHomePosition(coordinate);
    home->plannedHomePositionAltitude()->setRawValue(amslAltitude);
    home->setDirty(true);
    return true;
}

void CaacRouteEditorController::_watchVehicleHome()
{
    for (const auto& connection : _vehicleConnections) {
        disconnect(connection);
    }
    _vehicleConnections.clear();
    auto* vehicle = _masterController ? _masterController->managerVehicle() : nullptr;
    if (vehicle) {
        _vehicleConnections.append(connect(vehicle, &Vehicle::homePositionChanged, this,
                                           &CaacRouteEditorController::_syncPlannedHomeAltitude));
        _vehicleConnections.append(
            connect(vehicle, &Vehicle::coordinateChanged, this, &CaacRouteEditorController::_syncPlannedHomeAltitude));
        if (auto* gps = vehicle->gpsFactGroup()) {
            _vehicleConnections.append(connect(gps->lock(), &Fact::rawValueChanged, this,
                                               &CaacRouteEditorController::_syncPlannedHomeAltitude));
        }
        _vehicleConnections.append(connect(vehicle->altitudeAMSL(), &Fact::rawValueChanged, this,
                                           &CaacRouteEditorController::_syncPlannedHomeAltitude));
    }
    _syncPlannedHomeAltitude();
}

void CaacRouteEditorController::_syncPlannedHomeAltitude()
{
    auto* mission = _missionController();
    auto* vehicle = _masterController ? _masterController->managerVehicle() : nullptr;
    if (_synchronizingHome || _restoring || _rotationPreviewActive || !mission || !vehicle ||
        _masterController->offline() || _masterController->property("flyView").toBool() ||
        _masterController->syncInProgress()) {
        return;
    }
    auto* items = mission->visualItems();
    auto* home = items && items->count() ? items->value<MissionSettingsItem*>(0) : nullptr;
    if (!home || home->property("caacHomeAltitudeManual").toBool()) {
        return;
    }
    const auto planned = mission->plannedHomePosition();
    // Imported or explicitly entered finite heights remain authoritative. Zero is the upstream
    // placeholder used when a first waypoint creates a planned home without an AMSL source.
    if (planned.isValid() && std::isfinite(planned.altitude()) && planned.altitude() != 0) {
        return;
    }
    auto source = vehicle->homePosition();
    if (!source.isValid() || !std::isfinite(source.altitude())) {
        auto* gps = vehicle->gpsFactGroup();
        if (vehicle->airborne() || !gps || gps->lock()->rawValue().toInt() < 3) {
            return;
        }
        source = vehicle->coordinate();
        source.setAltitude(vehicle->altitudeAMSL()->rawValue().toDouble());
    }
    if (!source.isValid() || !std::isfinite(source.altitude()) ||
        source.altitude() < home->plannedHomePositionAltitude()->rawMin().toDouble() ||
        source.altitude() > home->plannedHomePositionAltitude()->rawMax().toDouble()) {
        return;
    }
    // The first-waypoint fallback places home 30 m away. Allow that offset, while leaving
    // a route planned for a different location untouched.
    if (planned.isValid() && planned.distanceTo(source) > 50) {
        return;
    }
    if (!planned.isValid() && items->count() != 1) {
        return;
    }
    auto resolved = planned.isValid() ? planned : source;
    resolved.setAltitude(source.altitude());
    if (resolved == planned) {
        return;
    }
    _synchronizingHome = true;
    const auto finish = qScopeGuard([this]() { _synchronizingHome = false; });
    mission->setHomePosition(resolved);
    home->plannedHomePositionAltitude()->setRawValue(resolved.altitude());
    home->setDirty(true);
}

CaacRouteEditorController::Snapshot CaacRouteEditorController::_snapshot() const
{
    Snapshot result;
    result.center = _rotationCenter;
    result.orientation = _masterController ? _masterController->property("caacRouteOrientation") : QVariant();
    result.simulation =
        _masterController ? _masterController->property("caacSimulationSettings").toMap() : QVariantMap();
    auto* mission = _missionController();
    if (mission && mission->visualItems() && mission->visualItems()->count() > 0) {
        result.manualHomeAltitude = mission->visualItems()->get(0)->property("caacHomeAltitudeManual").toBool();
        mission->save(result.mission);
        result.selection = mission->currentPlanViewVIIndex();
    }
    return result;
}

void CaacRouteEditorController::_scheduleCapture()
{
    if (!_restoring && _editDepth == 0) {
        _captureTimer.start();
    }
}

void CaacRouteEditorController::_watchItems()
{
    for (const auto& connection : _itemConnections) {
        disconnect(connection);
    }
    _itemConnections.clear();
    auto* mission = _missionController();
    if (!mission || !mission->visualItems()) {
        return;
    }
    auto* model = mission->visualItems();
    _itemConnections.append(connect(model, &QmlObjectListModel::countChanged, this, [this]() {
        _watchItems();
        _scheduleCapture();
    }));
    for (int i = 0; i < model->count(); ++i) {
        auto* item = model->value<VisualMissionItem*>(i);
        if (!item) {
            continue;
        }
        _itemConnections.append(
            connect(item, &VisualMissionItem::dirtyChanged, this, &CaacRouteEditorController::_scheduleCapture));
        for (auto* fact : item->findChildren<Fact*>()) {
            _itemConnections.append(
                connect(fact, &Fact::rawValueChanged, this, &CaacRouteEditorController::_scheduleCapture));
        }
        _itemConnections.append(connect(item, &VisualMissionItem::sequenceNumberChanged, this,
                                        &CaacRouteEditorController::coordinateItemsChanged));
        _itemConnections.append(connect(item, &VisualMissionItem::waypointNumberChanged, this,
                                        &CaacRouteEditorController::coordinateItemsChanged));
        _itemConnections.append(connect(item, &VisualMissionItem::coordinateChanged, this, [this]() {
            emit coordinateItemsChanged();
            _scheduleCapture();
        }));
    }
    emit coordinateItemsChanged();
}

void CaacRouteEditorController::_capture()
{
    _captureTimer.stop();
    if (_restoring || _editDepth > 0) {
        return;
    }
    const auto state = _snapshot();
    if (state.mission.isEmpty()) {
        return;
    }
    if (_historyIndex >= 0 && _history[_historyIndex].mission == state.mission &&
        _history[_historyIndex].center == state.center && _history[_historyIndex].simulation == state.simulation &&
        _history[_historyIndex].orientation == state.orientation &&
        _history[_historyIndex].manualHomeAltitude == state.manualHomeAltitude) {
        return;
    }
    _history = _history.mid(0, _historyIndex + 1);
    _history.append(state);
    // Bound memory for long classroom sessions while retaining 100 complete editing operations.
    constexpr int MAX_SNAPSHOTS = 101;
    if (_history.size() > MAX_SNAPSHOTS) {
        _history.removeFirst();
    }
    _historyIndex = _history.size() - 1;
    emit historyChanged();
}

void CaacRouteEditorController::beginEdit()
{
    if (_editDepth == 0) {
        _editSimulationSettings =
            _masterController ? _masterController->property("caacSimulationSettings").toMap() : QVariantMap();
        _editStartPoint = nullptr;
        _editReturnPoint = nullptr;
        auto* mission = _missionController();
        if (mission && !_editSimulationSettings.isEmpty()) {
            for (int i = 1; i < mission->visualItems()->count(); ++i) {
                auto* point = mission->visualItems()->value<SimpleMissionItem*>(i);
                if (!point || point->command() != MAV_CMD_NAV_WAYPOINT) {
                    continue;
                }
                if (point->sequenceNumber() == _editSimulationSettings.value("startSequence").toInt()) {
                    _editStartPoint = point;
                }
                if (point->sequenceNumber() == _editSimulationSettings.value("returnSequence").toInt()) {
                    _editReturnPoint = point;
                }
            }
        }
        _capture();
        if (_historyIndex >= 0 && _missionController()) {
            _history[_historyIndex].selection = _missionController()->currentPlanViewVIIndex();
        }
    }
    ++_editDepth;
}

void CaacRouteEditorController::endEdit()
{
    if (_editDepth > 0) {
        --_editDepth;
        if (_editDepth == 0) {
            if (_masterController && !_editSimulationSettings.isEmpty() &&
                _masterController->property("caacSimulationSettings").toMap() == _editSimulationSettings) {
                auto remapped = _editSimulationSettings;
                if (_editStartPoint) {
                    remapped["startSequence"] = _editStartPoint->sequenceNumber();
                }
                if (_editReturnPoint) {
                    remapped["returnSequence"] = _editReturnPoint->sequenceNumber();
                }
                if (remapped != _editSimulationSettings) {
                    ++_editDepth;
                    _masterController->setProperty("caacSimulationSettings", remapped);
                    --_editDepth;
                    emit simulationSettingsChanged();
                }
            }
            _editStartPoint = nullptr;
            _editReturnPoint = nullptr;
            _watchItems();
            _capture();
        }
    }
}

void CaacRouteEditorController::clearHistory()
{
    _captureTimer.stop();
    _history.clear();
    _historyIndex = -1;
    _capture();
    emit historyChanged();
}

bool CaacRouteEditorController::_restore(int index)
{
    if (!_canEdit() || _editDepth > 0 || index < 0 || index >= _history.size()) {
        return false;
    }
    const auto state = _history[index];
    _restoring = true;
    const auto finish = qScopeGuard([this]() { _restoring = false; });
    QString error;
    if (!_missionController()->load(state.mission, error)) {
        return _fail(tr("Unable to restore the route: %1").arg(error));
    }
    const auto savedHome = state.mission.value(QStringLiteral("plannedHomePosition")).toArray();
    auto* home = _missionController()->visualItems()->value<MissionSettingsItem*>(0);
    // Loading a takeoff coordinate can replace the home altitude with an unset coordinate altitude.
    if (home && savedHome.size() == 3 && savedHome.at(2).isDouble()) {
        home->setProperty("caacHomeAltitudeManual", state.manualHomeAltitude);
        home->plannedHomePositionAltitude()->setRawValue(savedHome.at(2).toDouble());
    }
    _setCenter(state.center);
    _masterController->setProperty("caacSimulationSettings", state.simulation);
    _masterController->setProperty("caacRouteOrientation", state.orientation);
    emit simulationSettingsChanged();
    _historyIndex = index;
    auto* mission = _missionController();
    const int selection = std::clamp(state.selection, 0, mission->visualItems()->count() - 1);
    mission->setCurrentPlanViewSeqNum(mission->visualItems()->value<VisualMissionItem*>(selection)->sequenceNumber(),
                                      true);
    mission->setDirty(true);
    _watchItems();
    emit selectionChanged();
    emit historyChanged();
    return true;
}

bool CaacRouteEditorController::undo()
{
    if (_rotationPreviewActive) {
        cancelRouteOrientation();
        return true;
    }
    _capture();
    return _restore(_historyIndex - 1);
}

bool CaacRouteEditorController::redo()
{
    if (_rotationPreviewActive) {
        cancelRouteOrientation();
        return false;
    }
    _capture();
    return _restore(_historyIndex + 1);
}

void CaacRouteEditorController::_setCenter(const QGeoCoordinate& coordinate)
{
    _rotationCenter = coordinate;
    if (_masterController) {
        _masterController->setProperty("caacRotationCenter", QVariant::fromValue(coordinate));
    }
    emit rotationCenterChanged();
}

bool CaacRouteEditorController::eventFilter(QObject* watched, QEvent* event)
{
    if (watched == _masterController && event->type() == QEvent::DynamicPropertyChange) {
        const auto* change = static_cast<QDynamicPropertyChangeEvent*>(event);
        if (change->propertyName() == "caacRotationCenter") {
            const auto center = watched->property("caacRotationCenter").value<QGeoCoordinate>();
            if (center != _rotationCenter) {
                _rotationCenter = center;
                emit rotationCenterChanged();
                clearHistory();
            }
        } else if (change->propertyName() == "caacRouteOrientation") {
            emit routeOrientationChanged();
            if (!_restoring && _editDepth == 0) {
                clearHistory();
            }
        } else if (change->propertyName() == "caacSimulationSettings") {
            emit simulationSettingsChanged();
            if (!_restoring && _editDepth == 0) {
                clearHistory();
            }
        }
    }
    return QObject::eventFilter(watched, event);
}
