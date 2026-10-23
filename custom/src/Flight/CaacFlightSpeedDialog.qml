pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import QGroundControl
import QGroundControl.Controls
import CAAC.Plan

QGCPopupDialog {
    id: root
    objectName: "caacFlightSpeedDialog"
    title: qsTr("Change flight speed")
    buttons: update.busy ? Dialog.NoButton : Dialog.Ok | Dialog.Cancel
    readonly property var vehicle: QGroundControl.multiVehicleManager.activeVehicle
    property var openedVehicle: null
    property var openedMaster: null
    property var waypointChoices: []
    property string errorText: ""
    readonly property var conversion: QGroundControl.unitsConversion
    readonly property bool forwardFlight: vehicle && vehicle.inFwdFlight
    readonly property real minimumSpeed: vehicle && forwardFlight ? vehicle.minimumEquivalentAirspeed() : 0.1
    readonly property real maximumSpeed: !vehicle ? 0 : forwardFlight
        ? vehicle.maximumEquivalentAirspeed() : vehicle.maximumHorizontalSpeedMultirotorMetersSecond()
    readonly property bool canChange: vehicle && vehicle === openedVehicle && vehicle.initialConnectComplete
        && vehicle.armed && vehicle.airborne && vehicle.supports.guidedMode && !update.busy
    Component.onCompleted: {
        openedVehicle = vehicle
        const simulation = QGroundControl.corePlugin.simulation
        const plan = globals.planMasterControllerPlanView
        openedMaster = simulation.active && simulation.vehicle === vehicle && simulation.planningPlan
            ? simulation.planningPlan
            : plan && plan.managerVehicle === vehicle && plan.missionController.uploadConfirmed
                ? plan : globals.planMasterControllerFlyView
        const choices = []
        const items = openedMaster ? openedMaster.missionController.visualItems : null
        if (items) {
            for (let i = 1; i < items.count; ++i) {
                const item = items.get(i)
                if (!item.isSimpleItem || item.command !== 16) { continue }
                const number = openedMaster === globals.planMasterControllerFlyView
                    ? simulation.waypointNumber(item.sequenceNumber, item.coordinate) : item.waypointNumber
                const member = { item: item, sequence: item.sequenceNumber }
                const existing = choices.find(choice => choice.number === number)
                if (existing) { existing.members.push(member) }
                else { choices.push({ number: number, members: [member], label: qsTr("Waypoint %1").arg(number) }) }
            }
        }
        choices.sort((left, right) => left.number - right.number)
        waypointChoices = choices
    }

    function selectedWaypoints() {
        const groups = scope.currentIndex === 2 ? waypointChoices
            : waypoint.currentIndex >= 0 && waypoint.currentIndex < waypointChoices.length
                ? [waypointChoices[waypoint.currentIndex]] : []
        const selected = []
        const items = openedMaster ? openedMaster.missionController.visualItems : null
        if (!items || openedMaster.managerVehicle !== vehicle || openedMaster.syncInProgress) { return [] }
        for (const group of groups) {
            for (const member of group.members) {
                let current = false
                for (let i = 1; i < items.count; ++i) {
                    if (items.get(i) === member.item && member.item.sequenceNumber === member.sequence) {
                        current = true
                        break
                    }
                }
                if (!current) { return [] }
                selected.push(member.item)
            }
        }
        return selected
    }

    function selectionIncludesCurrentLeg(selected) {
        const liveMaster = globals.planMasterControllerFlyView
        const mission = liveMaster.missionController
        const sequence = mission.currentMissionIndex
        const items = mission.visualItems
        let previous = items.count > 0 ? items.get(0) : null
        for (let i = 1; i < items.count; ++i) {
            const item = items.get(i)
            if (item.sequenceNumber <= sequence && sequence <= item.lastSequenceNumber) {
                if (sequence !== item.sequenceNumber || !previous || !previous.isSimpleItem || previous.command !== 16) {
                    return false
                }
                if (openedMaster === liveMaster) { return selected.indexOf(previous) >= 0 }
                const simulation = QGroundControl.corePlugin.simulation
                const number = simulation.active && simulation.vehicle === vehicle
                    ? simulation.waypointNumber(previous.sequenceNumber, previous.coordinate) : previous.waypointNumber
                return selected.some(point => point.waypointNumber === number
                    && point.coordinate.distanceTo(previous.coordinate) < 0.1)
            }
            if (item.specifiesCoordinate && !item.isStandaloneCoordinate) { previous = item }
        }
        return false
    }

    onAccepted: {
        const value = speed.text.trim().length ? conversion.appSettingsSpeedUnitsToMetersSecond(Number(speed.text)) : NaN
        const minimum = vehicle && forwardFlight ? vehicle.minimumEquivalentAirspeed() : 0.1
        const maximum = !vehicle ? 0 : forwardFlight
            ? vehicle.maximumEquivalentAirspeed() : vehicle.maximumHorizontalSpeedMultirotorMetersSecond()
        preventClose = !canChange || !isFinite(value) || !isFinite(minimum) || !isFinite(maximum)
            || maximum < minimum || value < minimum || value > maximum
        if (preventClose) { errorText = qsTr("Enter a valid flight speed within the aircraft limits."); return }
        if (scope.currentIndex !== 0) {
            preventClose = true
            const selected = selectedWaypoints()
            if (!selected.length) {
                errorText = qsTr("Aircraft or mission changed. Close this window and select again.")
                return
            }
            const selectedCurrentLeg = scope.currentIndex === 1 && selectionIncludesCurrentLeg(selected)
            const currentSpeed = scope.currentIndex === 2 || selectedCurrentLeg ? value : NaN
            const expectedCurrentSequence = selectedCurrentLeg
                ? globals.planMasterControllerFlyView.missionController.currentMissionIndex : -1
            errorText = QGroundControl.corePlugin.setFlightWaypointSpeeds(openedMaster, selected, value,
                scope.currentIndex === 2)
            if (errorText.length) { return }
            update.updateSpeeds(openedMaster, QGroundControl.corePlugin.simulation, currentSpeed, expectedCurrentSequence)
            return
        }
        if (forwardFlight) { vehicle.guidedModeChangeEquivalentAirspeedMetersSecond(value) }
        else { vehicle.guidedModeChangeGroundSpeedMetersSecond(value) }
    }
    onRejected: preventClose = update.busy

    CaacMissionUpdateController {
        id: update
        objectName: "caacFlightSpeedUpdate"
        onFinished: (success, message) => {
            if (success) { root.close() }
            else { root.errorText = message }
        }
    }
    ColumnLayout {
        width: Math.min(root.maxContentAvailableWidth, ScreenTools.defaultFontPixelWidth * 45)
        QGCLabel { text: qsTr("Apply to") }
        QGCComboBox {
            id: scope
            objectName: "caacFlightSpeedScope"
            Layout.fillWidth: true
            enabled: !update.busy
            model: [qsTr("Current leg"), qsTr("Selected waypoint"), qsTr("All waypoints")]
        }
        QGCLabel { text: qsTr("Target waypoint"); visible: scope.currentIndex === 1 }
        QGCComboBox {
            id: waypoint
            objectName: "caacFlightSpeedWaypoint"
            Layout.fillWidth: true
            visible: scope.currentIndex === 1
            enabled: !update.busy
            model: root.waypointChoices
            textRole: "label"
        }
        QGCLabel {
            Layout.fillWidth: true
            text: root.forwardFlight ? qsTr("Airspeed") : qsTr("Horizontal speed")
        }
        QGCTextField {
            id: speed
            objectName: "caacFlightSpeedValue"
            Layout.fillWidth: true
            enabled: !update.busy
            validator: DoubleValidator { locale: "C" }
            showUnits: true
            unitsLabel: root.vehicle ? root.vehicle.groundSpeed.units : ""
            text: root.vehicle ? root.conversion.metersSecondToAppSettingsSpeedUnits(
                Math.max(root.minimumSpeed, Math.min(root.maximumSpeed,
                    root.forwardFlight ? root.vehicle.airSpeed.rawValue : root.maximumSpeed / 2))).toFixed(1) : ""
        }
        QGCLabel {
            Layout.fillWidth: true
            wrapMode: Text.WordWrap
            text: scope.currentIndex === 0 ? qsTr("Later waypoint speed commands may override this speed.")
                : scope.currentIndex === 1
                    ? qsTr("Changes the leg leaving the selected waypoint. Other waypoint speeds are preserved.")
                    : qsTr("Changes all waypoint speeds, including the initial leg from takeoff.")
        }
        QGCLabel {
            Layout.fillWidth: true
            wrapMode: Text.WordWrap
            visible: scope.currentIndex !== 0
            text: qsTr("The aircraft will pause, update the mission, and continue from its current target waypoint.")
        }
        QGCLabel {
            Layout.fillWidth: true
            wrapMode: Text.WordWrap
            visible: update.busy
            text: qsTr("Updating flight speeds…")
        }
        QGCLabel {
            Layout.fillWidth: true
            wrapMode: Text.WordWrap
            visible: scope.currentIndex !== 0 && root.waypointChoices.length === 0
            text: qsTr("No downloaded waypoints are available.")
        }
        QGCLabel {
            Layout.fillWidth: true
            wrapMode: Text.WordWrap
            visible: root.errorText.length > 0
            text: root.errorText
        }
    }
}
