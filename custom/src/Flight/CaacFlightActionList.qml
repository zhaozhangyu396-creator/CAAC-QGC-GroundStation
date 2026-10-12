import QtQuick
import QtQml.Models

import QGroundControl
import QGroundControl.Controls
import QGroundControl.FlyView
import QGroundControl.Viewer3D
import CAAC.Plan

ToolStripActionList {
    id: root
    signal displayPreFlightChecklist
    readonly property var _guided: globals.guidedControllerFlyView
    readonly property var _vehicle: QGroundControl.multiVehicleManager.activeVehicle
    readonly property var _master: globals.planMasterControllerFlyView
    readonly property bool _hasMission: _vehicle && _master && !_master.syncInProgress
        && _master.missionController.visualItems.count > 1
    property Component _missionDialog: Component { CaacMissionActionDialog { } }
    property var _pendingVehicle: null
    property bool _pendingAltitude: false
    on_VehicleChanged: {
        _pendingVehicle = null
        if (root._guided && root._guided.confirmDialog && root._guided.guidedValueSlider) { root._guided.closeAll() }
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
        Viewer3DShowAction { },
        PreFlightCheckListShowAction { onTriggered: root.displayPreFlightChecklist() },
        GuidedActionTakeoff { },
        GuidedActionLand { },
        GuidedActionRTL { },
        ToolStripAction {
            objectName: "caacStartMission"
            text: root._guided.showContinueMission ? qsTr("Continue\nroute") : qsTr("Start\nroute")
            iconSource: "/res/takeoff.svg"
            enabled: root._hasMission && (root._guided.showStartMission || root._guided.showContinueMission)
            onTriggered: {
                if (enabled) {
                    root._guided.closeAll()
                    root._guided.confirmAction(root._guided.showContinueMission
                        ? root._guided.actionContinueMission : root._guided.actionStartMission)
                }
            }
        },
        GuidedActionPause {
            objectName: "caacPauseMission"
            text: qsTr("Pause\nroute")
            visible: true
        },
        ToolStripAction {
            objectName: "caacJumpWaypoint"
            text: qsTr("Jump to\nwaypoint")
            iconSource: "/res/waypoint.svg"
            enabled: root._hasMission && root._vehicle.armed && root._vehicle.airborne
            onTriggered: { if (enabled) { root.openMissionDialog(false) } }
        },
        ToolStripAction {
            objectName: "caacModifyWaypointAltitude"
            text: qsTr("Waypoint\naltitude")
            iconSource: "/InstrumentValueIcons/arrow-thick-up.svg"
            enabled: root._hasMission
            onTriggered: { if (enabled) { root.openMissionDialog(true) } }
        },
        FlyViewAdditionalActionsButton { },
        FlyViewGripperButton { }
    ]
}
