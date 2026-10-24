import QtQuick
import QtQuick.Controls
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
    readonly property bool _hasMission: _vehicle && !_vehicle.flightPlanUpdateInProgress
        && _master && !_master.syncInProgress
        && _master.missionController.visualItems.count > 1
    readonly property var _simulation: QGroundControl.corePlugin.simulation
    readonly property bool _simulatedVehicle: _vehicle && _simulation.active && _simulation.vehicle === _vehicle
    readonly property bool _hasCurrentMissionItem: {
        if (!_hasMission) { return false }
        const mission = _master.missionController
        const sequence = mission.currentMissionIndex
        for (let index = 1; index < mission.visualItems.count; ++index) {
            const item = mission.visualItems.get(index)
            if (sequence >= item.sequenceNumber && sequence <= item.lastSequenceNumber) { return true }
        }
        return false
    }
    readonly property bool _canContinueMission: _hasCurrentMissionItem && _vehicle.armed && _vehicle.airborne
        && _vehicle.flightMode === _vehicle.pauseFlightMode && (!_simulatedVehicle || _simulation.missionReady)
    property Component _missionDialog: Component { CaacMissionActionDialog { } }
    property Component _speedDialog: Component { CaacFlightSpeedDialog { } }
    property var _landingUploadPrompt: null
    property Component _landingUploadDialog: Component {
        QGCPopupDialog {
            id: landingDialog
            property var vehicleAtOpen
            property var sourcePlanAtOpen
            property string fingerprintAtOpen
            property string error: ""
            title: qsTr("Upload landing point")
            buttons: Dialog.Ok | Dialog.Cancel
            onAccepted: {
                if (!vehicleAtOpen || vehicleAtOpen !== root._vehicle
                    || sourcePlanAtOpen !== root.returnSourcePlan()
                    || !fingerprintAtOpen.length
                    || fingerprintAtOpen !== QGroundControl.corePlugin.landingPointUploadFingerprint(sourcePlanAtOpen)
                    || !QGroundControl.corePlugin.landingPointNeedsUpload(sourcePlanAtOpen)) {
                    error = qsTranslate("CaacMissionActionDialog", "Aircraft or mission changed. Close this window and select again.")
                    preventClose = true
                    return
                }
                QGroundControl.corePlugin.uploadAndReturnToDesignatedLanding(sourcePlanAtOpen, root._simulation)
            }
            onClosed: if (root._landingUploadPrompt === landingDialog) { root._landingUploadPrompt = null }
            QGCLabel {
                objectName: "caacPendingLandingUploadMessage"
                width: Math.min(landingDialog.maxContentAvailableWidth, ScreenTools.defaultFontPixelWidth * 55)
                wrapMode: Text.WordWrap
                text: landingDialog.error || qsTr("The landing point has not been uploaded. Upload the current route and return to this landing point?")
            }
        }
    }
    property var _pendingVehicle: null
    property bool _pendingAltitude: false
    on_GuidedChanged: disableAutomaticConfirmations()
    Component.onCompleted: disableAutomaticConfirmations()
    Component.onDestruction: if (_landingUploadPrompt) { _landingUploadPrompt.close() }
    on_VehicleChanged: {
        _pendingVehicle = null
        if (_landingUploadPrompt) { _landingUploadPrompt.close() }
        if (root._guided && root._guided.confirmDialog && root._guided.guidedValueSlider) { root._guided.closeAll() }
    }

    function disableAutomaticConfirmations() {
        if (_guided) {
            _guided.automaticMissionPopupsAllowed = false
            if (_guided.confirmDialog && _guided.guidedValueSlider) { _guided.closeAll() }
        }
    }

    function openMissionDialog(editAltitude) {
        if (!_vehicle || _vehicle.flightPlanUpdateInProgress || !_master || _master.syncInProgress) {
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

    function returnSourcePlan() {
        return root._simulatedVehicle ? root._simulation.planningPlan : globals.planMasterControllerPlanView
    }

    function returnToLanding() {
        const sourcePlan = root.returnSourcePlan()
        if (QGroundControl.corePlugin.landingPointNeedsUpload(sourcePlan)) {
            if (root._landingUploadPrompt) { return }
            const dialog = root._landingUploadDialog.createObject(mainWindow, {
                vehicleAtOpen: root._vehicle,
                sourcePlanAtOpen: sourcePlan,
                fingerprintAtOpen: QGroundControl.corePlugin.landingPointUploadFingerprint(sourcePlan)
            })
            if (dialog) { root._landingUploadPrompt = dialog; dialog.open() }
        } else {
            QGroundControl.corePlugin.returnToDesignatedLanding(root._master, root._simulation)
        }
    }

    property Connections _transferConnections: Connections {
        target: root._master ? root._master.missionController : null
        function onNewItemsFromVehicle() {
            Qt.callLater(() => {
                if (root._pendingVehicle && root._pendingVehicle === root._vehicle
                        && !root._vehicle.flightPlanUpdateInProgress && !root._master.syncInProgress) {
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
            enabled: root._vehicle && !root._vehicle.flightPlanUpdateInProgress && root._guided.showRTL
            onTriggered: {
                if (enabled) {
                    root._guided.closeAll()
                    root.returnToLanding()
                }
            }
        },
        ToolStripAction {
            objectName: "caacStartMission"
            text: qsTr("Start route")
            iconSource: "/InstrumentValueIcons/play.svg"
            enabled: root._hasMission && root._guided && (root._guided.showStartMission || root._canContinueMission
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
                && !root._vehicle.flightPlanUpdateInProgress
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
