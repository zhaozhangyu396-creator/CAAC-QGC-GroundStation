#include "CaacRouteEditorController.h"

#include <algorithm>
#include <cmath>
#include <limits>

#include <QtCore/QEvent>
#include <QtCore/QScopeGuard>

#include "CaacRouteGeometry.h"
#include "CaacRouteTemplates.h"
#include "Fact.h"
#include "MissionController.h"
#include "MissionSettingsItem.h"
#include "PlanMasterController.h"
#include "QGCGeo.h"
#include "SimpleMissionItem.h"
#include "SpeedSection.h"

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

PlanMasterController* CaacRouteEditorController::planMasterController() const
{
    return _masterController.data();
}

MissionController* CaacRouteEditorController::_missionController() const
{
    return _masterController ? _masterController->missionController() : nullptr;
}

void CaacRouteEditorController::setPlanMasterController(PlanMasterController* controller)
{
    if (_masterController == controller) {
        return;
    }
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
        connect(controller->missionController(), &MissionController::planViewStateChanged, this,
                &CaacRouteEditorController::selectionChanged);
        connect(controller, &QObject::destroyed, this, [this]() {
            clearHistory();
            emit planMasterControllerChanged();
            emit selectionChanged();
        });
        connect(controller->missionController(), &MissionController::visualItemsReset, this, [this]() {
            _watchItems();
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
    clearHistory();
    _clearError();
    emit planMasterControllerChanged();
    emit selectionChanged();
    emit rotationCenterChanged();
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

bool CaacRouteEditorController::_canEdit()
{
    _clearError();
    if (!_masterController || !_missionController() || !_missionController()->visualItems() ||
        _missionController()->visualItems()->count() < 1) {
        return _fail(tr("No plan is available."));
    }
    if (_masterController->syncInProgress()) {
        return _fail(tr("Wait for the vehicle transfer to finish before editing."));
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
    return {{QStringLiteral("supported"), true},
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
    if (advanced && (!item || _missionController()->visualItemIndexForObject(item) < 1 ||
                     item->command() != MAV_CMD_NAV_WAYPOINT || !std::isfinite(hold) || hold < 0 ||
                     hold > std::numeric_limits<float>::max() || (specifyYaw && !std::isfinite(yaw)))) {
        return _fail(tr("Enter a nonnegative hold duration and a valid waypoint yaw."));
    }
    beginEdit();
    const auto finish = qScopeGuard([this]() { endEdit(); });
    const double altitude = number("altitude");
    const bool specifySpeed = properties.value(QStringLiteral("specifySpeed")).toBool();
    const double speed = number("speed");
    const bool updated =
        properties.value(QStringLiteral("relative")).toBool()
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
                       {QStringLiteral("previousNumber"), previous->sequenceNumber()}};
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
    if (!item || item->command() != MAV_CMD_NAV_WAYPOINT) {
        return _fail(tr("Only ordinary waypoints can be removed here."));
    }
    beginEdit();
    const auto finish = qScopeGuard([this]() { endEdit(); });
    _missionController()->removeVisualItem(_missionController()->visualItemIndexForObject(item));
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
                            {QStringLiteral("label"),
                             i == 0 ? tr("Planned home")
                                    : tr("Waypoint %1 · %2").arg(item->sequenceNumber()).arg(item->commandName())},
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

bool CaacRouteEditorController::rotateRouteAround(const QGeoCoordinate& center, double clockwiseDegrees)
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
    for (int i = 0; i < items.size(); ++i) {
        auto* item = qobject_cast<VisualMissionItem*>(items[i]);
        item->setCoordinate(result->at(i));
    }
    _missionController()->setDirty(true);
    return true;
}

bool CaacRouteEditorController::alignRoute(int fromIndex, int toIndex, double targetBearing)
{
    return alignRouteAround(_rotationCenter, fromIndex, toIndex, targetBearing);
}

bool CaacRouteEditorController::alignRouteAround(const QGeoCoordinate& center, int fromIndex, int toIndex,
                                                 double targetBearing)
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
    const auto angle = CaacRouteGeometry::alignmentAngle(a->coordinate(), b->coordinate(), center, targetBearing);
    if (!angle) {
        return _fail(tr("The two points must be distinct and the target bearing must be valid."));
    }
    return rotateRouteAround(center, *angle);
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
    for (int i = 0; i < coordinates.size(); ++i) {
        _missionController()->insertSimpleMissionItem(coordinates[i].value<QGeoCoordinate>(), referenceIndex + i + 1,
                                                      i == coordinates.size() - 1);
    }
    return true;
}

bool CaacRouteEditorController::translateRoute(const QGeoCoordinate& reference, const QGeoCoordinate& target)
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
        qobject_cast<VisualMissionItem*>(items[i])->setCoordinate(result[i]);
    }
    _missionController()->setDirty(true);
    return true;
}

bool CaacRouteEditorController::moveRouteBy(double bearing, double distance)
{
    auto* mission = _missionController();
    if (!_canEdit() || !mission) {
        return false;
    }
    const auto reference = mission->plannedHomePosition();
    const auto target = CaacRouteGeometry::relativePoint(reference, bearing, distance);
    return target ? translateRoute(reference, *target) : _fail(tr("Enter a valid bearing and positive distance."));
}

bool CaacRouteEditorController::updatePlannedHome(double latitude, double longitude, double amslAltitude)
{
    if (!_canEdit()) {
        return false;
    }
    auto* home = _missionController()->visualItems()->value<MissionSettingsItem*>(0);
    const QGeoCoordinate coordinate(latitude, longitude);
    if (!home || !coordinate.isValid() || !std::isfinite(amslAltitude) ||
        amslAltitude < home->plannedHomePositionAltitude()->rawMin().toDouble() ||
        amslAltitude > home->plannedHomePositionAltitude()->rawMax().toDouble()) {
        return _fail(tr("Check the planned home coordinates and AMSL altitude."));
    }
    beginEdit();
    const auto finish = qScopeGuard([this]() { endEdit(); });
    _missionController()->setHomePosition(coordinate);
    home->plannedHomePositionAltitude()->setRawValue(amslAltitude);
    return true;
}

CaacRouteEditorController::Snapshot CaacRouteEditorController::_snapshot() const
{
    Snapshot result;
    result.center = _rotationCenter;
    auto* mission = _missionController();
    if (mission && mission->visualItems() && mission->visualItems()->count() > 0) {
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
        _history[_historyIndex].center == state.center) {
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
    _setCenter(state.center);
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
    _capture();
    return _restore(_historyIndex - 1);
}

bool CaacRouteEditorController::redo()
{
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
        }
    }
    return QObject::eventFilter(watched, event);
}
