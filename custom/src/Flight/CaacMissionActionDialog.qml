pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import QGroundControl
import QGroundControl.Controls

QGCPopupDialog {
    id: root
    property bool editAltitude: false
    readonly property var _vehicle: QGroundControl.multiVehicleManager.activeVehicle
    property var _openedVehicle: null
    property var _openedSourcePlan: null
    readonly property var _master: globals.planMasterControllerFlyView
    readonly property var _mission: _master ? _master.missionController : null
    readonly property var _items: _mission ? _mission.visualItems : null
    property var _choices: []
    property string _error: ""
    title: editAltitude ? qsTr("Change altitude") : qsTr("Jump to waypoint")
    buttons: Dialog.Ok | Dialog.Cancel

    Component.onCompleted: {
        _openedVehicle = _vehicle
        _openedSourcePlan = globals.planMasterControllerPlanView
        // Freeze the choices so a downloaded replacement mission cannot retarget an open dialog.
        const choices = []
        const simulation = QGroundControl.corePlugin.simulation
        if (editAltitude) { choices.push({all: true, label: qsTr("All waypoints")}) }
        if (_items) {
            for (let i = 1; i < _items.count; ++i) {
                const item = _items.get(i)
                if (item.isSimpleItem && item.command === 16) {
                    const number = simulation.waypointNumber(item.sequenceNumber, item.coordinate)
                    const member = {item: item, sequence: item.sequenceNumber}
                    const existing = choices.find(entry => !entry.all && entry.number === number)
                    if (existing) { existing.members.push(member) }
                    else {
                        choices.push({item: item, sequence: item.sequenceNumber, altitude: item.altitude.rawValue,
                            number: number, members: [member], label: qsTr("Waypoint %1").arg(number)})
                    }
                }
            }
        }
        choices.sort((a, b) => a.all ? -1 : b.all ? 1 : a.number - b.number)
        _choices = choices
    }

    function choiceMembers(choice) {
        const groups = choice.all ? _choices.filter(entry => !entry.all) : [choice]
        const members = []
        for (const group of groups) {
            for (const member of group.members) { members.push(member) }
        }
        return members
    }

    function selectedItemIsCurrent() {
        if (!_vehicle || _vehicle !== _openedVehicle || _vehicle.flightPlanUpdateInProgress
                || (editAltitude && globals.planMasterControllerPlanView !== _openedSourcePlan)
                || !_items || waypoint.currentIndex < 0
                || waypoint.currentIndex >= _choices.length || _master.syncInProgress) {
            return false
        }
        const choice = _choices[waypoint.currentIndex]
        const selected = choiceMembers(choice)
        if (!selected.length) { return false }
        for (const entry of selected) {
            let found = false
            for (let i = 1; i < _items.count; ++i) {
                if (_items.get(i) === entry.item && entry.item.sequenceNumber === entry.sequence) {
                    found = true
                    break
                }
            }
            if (!found) { return false }
        }
        return true
    }

    onAccepted: {
        preventClose = false
        if (!selectedItemIsCurrent()) {
            _error = qsTr("Aircraft or mission changed. Close this window and select again.")
            preventClose = true
            return
        }
        const choice = _choices[waypoint.currentIndex]
        if (editAltitude) {
            _error = QGroundControl.corePlugin.updateFlightWaypointAltitudes(_master,
                choiceMembers(choice).map(member => member.item),
                altitude.text.trim().length ? Number(altitude.text) : NaN, _openedSourcePlan)
            preventClose = _error.length > 0
        } else {
            const guided = globals.guidedControllerFlyView
            if (!_vehicle.armed || !_vehicle.airborne) {
                _error = qsTr("Jumping requires an aircraft in flight.")
                preventClose = true
                return
            }
            guided.closeAll()
            guided.executeAction(guided.actionSetWaypoint, choice.sequence)
        }
    }

    ColumnLayout {
        width: Math.min(root.maxContentAvailableWidth, ScreenTools.defaultFontPixelWidth * 55)
        spacing: ScreenTools.defaultFontPixelHeight * 0.5
        QGCLabel { text: qsTr("Target waypoint") }
        QGCComboBox {
            id: waypoint
            objectName: "caacFlightWaypointChoice"
            Layout.fillWidth: true
            model: root._choices
            textRole: "label"
        }
        QGCLabel {
            visible: root.editAltitude
            text: qsTr("Waypoint altitude (m)")
        }
        QGCTextField {
            id: altitude
            objectName: "caacFlightWaypointAltitude"
            Layout.fillWidth: true
            visible: root.editAltitude
            validator: DoubleValidator { locale: "C" }
            text: waypoint.currentIndex >= 0 && waypoint.currentIndex < root._choices.length
                ? (root._choices[waypoint.currentIndex].all ? "" : root._choices[waypoint.currentIndex].altitude.toString()) : ""
        }
        QGCLabel {
            Layout.fillWidth: true
            visible: root.editAltitude
            wrapMode: Text.WordWrap
            text: qsTr("Keeps the waypoint's altitude frame. Pause the mission before saving; saving uploads the complete mission to the aircraft.")
        }
        QGCLabel {
            Layout.fillWidth: true
            visible: root._choices.length === 0 || root._error.length > 0
            wrapMode: Text.WordWrap
            text: root._error || qsTr("No downloaded waypoints are available.")
        }
    }
}
