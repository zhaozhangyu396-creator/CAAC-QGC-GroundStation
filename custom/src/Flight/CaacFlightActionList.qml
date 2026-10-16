import QtQuick
import QtQml.Models

import QGroundControl
import QGroundControl.Controls
import QGroundControl.FlyView
import CAAC.Plan

ToolStripActionList {
    id: root
    signal displayPreFlightChecklist
    readonly property var _guided: globals.guidedControllerFlyView
    readonly property var _vehicle: QGroundControl.multiVehicleManager.activeVehicle
    readonly property var _master: globals.planMasterControllerFlyView
    readonly property bool _hasMission: _vehicle && _master && !_master.syncInProgress
        && _master.missionController.visualItems.count > 1
    readonly property var _simulation: QGroundControl.corePlugin.simulation
    readonly property bool _simulatedVehicle: _vehicle && _simulation.active && _simulation.vehicle === _vehicle
    property Component _missionDialog: Component { CaacMissionActionDialog { } }
    property Component _speedDialog: Component { CaacFlightSpeedDialog { } }
    property var _pendingVehicle: null
    property bool _pendingAltitude: false
    on_GuidedChanged: disableAutomaticConfirmations()
    Component.onCompleted: disableAutomaticConfirmations()
    on_VehicleChanged: {
        _pendingVehicle = null
        if (root._guided && root._guided.confirmDialog && root._guided.guidedValueSlider) { root._guided.closeAll() }
    }

    function disableAutomaticConfirmations() {
        if (_guided) {
            _guided.automaticMissionPopupsAllowed = false
            if (_guided.confirmDialog && _guided.guidedValueSlider) { _guided.closeAll() }
        }
    }

    function openMissionDialog(editAltitude) {
        if (!_vehicle || !_master || _master.syncInProgress) {
            return
        }
        _pendingVehicle = _vehicle
        _pendingAltitude = editAltitude
        const error = QGroundControl.corePlugin.refreshFlightMission(_master)
        if (error.length) {
            _pendingVehicle = null
            QGroundControl.showMessageDialog(mainWindow, qsTr("Waypoint operations"), error)
        }
    }

    property Connections _transferConnections: Connections {
        target: root._master ? root._master.missionController : null
        function onNewItemsFromVehicle() {
            Qt.callLater(() => {
                if (root._pendingVehicle && root._pendingVehicle === root._vehicle && !root._master.syncInProgress) {
                    root._pendingVehicle = null
                    if (root._master.missionController.visualItems.count > 1) {
                        const dialog = root._missionDialog.createObject(mainWindow, {editAltitude: root._pendingAltitude})
                        if (dialog) { dialog.open() }
                    }
                }
            })
        }
    }

    model: [
        ToolStripAction {
            objectName: "caacTakeoff"
            text: qsTr("One-click takeoff")
            iconSource: "/res/takeoff.svg"
            enabled: root._vehicle && root._guided.showTakeoff
            onTriggered: {
                if (enabled) {
                    root._guided.closeAll()
                    if (root._simulatedVehicle) { root._vehicle.guidedModeTakeoff(root._simulation.takeoffHeight) }
                    else if (root._vehicle.supports.guidedTakeoffWithAltitude) {
                        root._vehicle.guidedModeTakeoff(root._vehicle.minimumTakeoffAltitudeMeters())
                    } else { root._vehicle.startTakeoff() }
                }
            }
        },
        ToolStripAction {
            objectName: "caacReturnHome"
            text: qsTr("One-click return")
            iconSource: "/res/rtl.svg"
            enabled: root._vehicle && root._guided.showRTL
            onTriggered: {
                if (enabled) {
                    root._guided.closeAll()
                    root._vehicle.guidedModeRTL(false)
                }
            }
        },
        ToolStripAction {
            objectName: "caacStartMission"
            text: qsTr("Start route")
            iconSource: "/InstrumentValueIcons/play.svg"
            enabled: root._hasMission && (root._guided.showStartMission || root._guided.showContinueMission
                || (root._simulatedVehicle && root._simulation.missionFinished && root._vehicle.armed
                    && root._vehicle.airborne && root._vehicle.flightMode !== root._vehicle.missionFlightMode))
            onTriggered: {
                if (enabled) {
                    root._guided.closeAll()
                    root._vehicle.startMission()
                }
            }
        },
        ToolStripAction {
            objectName: "caacPauseMission"
            text: qsTr("Pause route")
            iconSource: "/res/pause-mission.svg"
            enabled: root._vehicle && root._guided.showPause
            onTriggered: {
                if (enabled) {
                    root._guided.closeAll()
                    root._vehicle.pauseVehicle()
                }
            }
        },
        ToolStripAction {
            objectName: "caacJumpWaypoint"
            text: qsTr("Jump to waypoint")
            iconSource: "/res/waypoint.svg"
            enabled: root._hasMission && root._vehicle.armed && root._vehicle.airborne
            onTriggered: { if (enabled) { root.openMissionDialog(false) } }
        },
        ToolStripAction {
            objectName: "caacModifyWaypointAltitude"
            text: qsTr("Change altitude")
            iconSource: "/InstrumentValueIcons/arrow-thick-up.svg"
            enabled: root._hasMission
            onTriggered: { if (enabled) { root.openMissionDialog(true) } }
        },
        ToolStripAction {
            objectName: "caacChangeFlightSpeed"
            text: qsTr("Change flight speed")
            iconSource: "/InstrumentValueIcons/arrow-thick-right.svg"
            enabled: root._vehicle && root._vehicle.initialConnectComplete && root._vehicle.armed
                && root._vehicle.airborne && root._vehicle.supports.guidedMode
            onTriggered: {
                if (enabled) { root._guided.closeAll(); _speedDialog.createObject(mainWindow).open() }
            }
        },
        ToolStripAction {
            objectName: "caacSimulationEmergency"
            text: root._simulation.emergencyMode ? qsTr("Exit emergency") : qsTr("Simulation emergency")
            iconSource: "/InstrumentValueIcons/hand-stop.svg"
            enabled: root._simulatedVehicle && root._vehicle.initialConnectComplete
            onTriggered: {
                if (enabled) {
                    root._guided.closeAll()
                    root._simulation.emergencyMode = !root._simulation.emergencyMode
                }
            }
        }
    ]
}
