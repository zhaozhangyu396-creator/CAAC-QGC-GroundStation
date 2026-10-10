#include "CaacRouteEditorController.h"

#include <cmath>

#include "Fact.h"
#include "MissionController.h"
#include "PlanMasterController.h"
#include "SimpleMissionItem.h"
#include "SpeedSection.h"

CaacRouteEditorController::CaacRouteEditorController(QObject* parent)
    : QObject(parent)
{}

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
        disconnect(_masterController, nullptr, this, nullptr);
        disconnect(_missionController(), nullptr, this, nullptr);
    }
    _masterController = controller;
    if (controller) {
        connect(controller->missionController(), &MissionController::planViewStateChanged, this,
                &CaacRouteEditorController::selectionChanged);
        connect(controller, &QObject::destroyed, this, [this]() {
            emit planMasterControllerChanged();
            emit selectionChanged();
        });
    }
    _clearError();
    emit planMasterControllerChanged();
    emit selectionChanged();
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
    if (!_masterController || !_missionController() || !_missionController()->visualItems()) {
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
    if (specifySpeed && (!speedSection || !speedSection->available() || !std::isfinite(speed) || speed <= 0 ||
                         speed < speedSection->flightSpeed()->rawMin().toDouble() ||
                         speed > speedSection->flightSpeed()->rawMax().toDouble())) {
        return _fail(tr("Flight speed is invalid or unavailable for this mission item."));
    }
    // Validate every input before changing any Fact; preserve command, frame and other MAVLink parameters.
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

bool CaacRouteEditorController::removeCurrentWaypoint()
{
    if (!_canChangeOrder()) {
        return false;
    }
    auto* item = currentItem();
    if (!item || item->command() != MAV_CMD_NAV_WAYPOINT) {
        return _fail(tr("Only ordinary waypoints can be removed here."));
    }
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
    if (!_missionController()->moveVisualItem(from, from + offset)) {
        return _fail(tr("Waypoints can only move past adjacent ordinary waypoints."));
    }
    return true;
}
