pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import QGroundControl
import QGroundControl.Controls

QGCPopupDialog {
    id: root
    property bool editAltitude: false
    property bool editCurrentAltitude: false
    readonly property var _vehicle: QGroundControl.multiVehicleManager.activeVehicle
    property var _openedVehicle: null
    readonly property var _master: globals.planMasterControllerFlyView
    readonly property var _mission: _master ? _master.missionController : null
    readonly property var _items: _mission ? _mission.visualItems : null
    property var _choices: []
    property string _error: ""
    property bool _changingCurrentAltitude: false
    property real _requestedCurrentAltitude: NaN
    readonly property bool _canChangeCurrentAltitude: _vehicle && _vehicle === _openedVehicle
        && _vehicle.initialConnectComplete && _vehicle.armed && _vehicle.airborne && _vehicle.supports.guidedMode
        && !_vehicle.flightPlanUpdateInProgress && _master && !_master.syncInProgress && !_changingCurrentAltitude
    title: editAltitude ? qsTr("Change altitude") : qsTr("Jump to waypoint")
    buttons: _changingCurrentAltitude ? Dialog.NoButton : Dialog.Ok | Dialog.Cancel

    function finishCurrentAltitudeChange(error) {
        altitudeTimeout.stop()
        _changingCurrentAltitude = false
        _error = error
        if (!error.length) { root.close() }
    }

    function checkCurrentAltitude() {
        if (!_changingCurrentAltitude) { return }
        if (!_vehicle || _vehicle !== _openedVehicle || !_vehicle.armed || !_vehicle.airborne) {
            finishCurrentAltitudeChange(qsTr("Aircraft or mission changed. Close this window and select again."))
        } else if (Math.abs(_vehicle.altitudeRelative.rawValue - _requestedCurrentAltitude) < 0.15) {
            finishCurrentAltitudeChange("")
        }
    }

    on_VehicleChanged: if (_changingCurrentAltitude) {
        finishCurrentAltitudeChange(qsTr("Aircraft or mission changed. Close this window and select again."))
    }

    Component.onCompleted: {
        _openedVehicle = _vehicle
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
        if (editCurrentAltitude && _vehicle) { altitude.text = _vehicle.altitudeRelative.rawValue.toString() }
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
        if (editCurrentAltitude) {
            preventClose = true
            const height = altitude.text.trim().length ? Number(altitude.text) : NaN
            if (!_canChangeCurrentAltitude) {
                _error = qsTr("Aircraft or mission changed. Close this window and select again.")
                return
            }
            _requestedCurrentAltitude = height
            _changingCurrentAltitude = true
            globals.guidedControllerFlyView.closeAll()
            const error = QGroundControl.corePlugin.changeCurrentFlightAltitude(_openedVehicle, height)
            if (error.length) { _changingCurrentAltitude = false; _error = error; return }
            if (!_changingCurrentAltitude) { return }
            _error = ""
            altitudeTimeout.start()
            checkCurrentAltitude()
            return
        }
        if (!selectedItemIsCurrent()) {
            _error = qsTr("Aircraft or mission changed. Close this window and select again.")
            preventClose = true
            return
        }
        const choice = _choices[waypoint.currentIndex]
        if (editAltitude) {
            _error = QGroundControl.corePlugin.updateFlightWaypointAltitudes(_master,
                choiceMembers(choice).map(member => member.item),
                altitude.text.trim().length ? Number(altitude.text) : NaN)
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

    onRejected: preventClose = _changingCurrentAltitude

    Timer {
        id: altitudeTimeout
        interval: 60000
        onTriggered: root.finishCurrentAltitudeChange(qsTr("The aircraft did not reach the requested altitude. Check the flight data and try again."))
    }

    Connections {
        target: root._openedVehicle
        function onMavCommandResult(vehicleId, component, command, result, failure) {
            if (root._changingCurrentAltitude && root._openedVehicle
                && vehicleId === root._openedVehicle.id && command === 192
                && (result !== 0 || failure !== 0)) {
                root.finishCurrentAltitudeChange(qsTr("The aircraft did not accept the altitude change."))
            }
        }
        function onArmedChanged() { root.checkCurrentAltitude() }
        function onAirborneChanged() { root.checkCurrentAltitude() }
    }

    Connections {
        target: root._openedVehicle ? root._openedVehicle.altitudeRelative : null
        function onRawValueChanged() { root.checkCurrentAltitude() }
    }

    ColumnLayout {
        width: Math.min(root.maxContentAvailableWidth, ScreenTools.defaultFontPixelWidth * 55)
        spacing: ScreenTools.defaultFontPixelHeight * 0.5
        QGCLabel { text: qsTr("Target waypoint"); visible: !root.editCurrentAltitude }
        QGCComboBox {
            id: waypoint
            objectName: "caacFlightWaypointChoice"
            Layout.fillWidth: true
            visible: !root.editCurrentAltitude
            model: root._choices
            textRole: "label"
        }
        QGCLabel {
            visible: root.editAltitude
            text: root.editCurrentAltitude ? qsTr("Current flight altitude (m)") : qsTr("Waypoint altitude (m)")
        }
        QGCTextField {
            id: altitude
            objectName: "caacFlightWaypointAltitude"
            Layout.fillWidth: true
            visible: root.editAltitude
            enabled: !root._changingCurrentAltitude
            validator: DoubleValidator { locale: "C" }
            text: !root.editCurrentAltitude && waypoint.currentIndex >= 0 && waypoint.currentIndex < root._choices.length
                ? (root._choices[waypoint.currentIndex].all ? "" : root._choices[waypoint.currentIndex].altitude.toString()) : ""
        }
        QGCLabel {
            Layout.fillWidth: true
            visible: root.editCurrentAltitude
            wrapMode: Text.WordWrap
            text: root._changingCurrentAltitude ? qsTr("Changing current flight altitude…")
                : qsTr("The aircraft will pause the mission and change its current altitude. Waypoint altitudes are unchanged.")
        }
        QGCLabel {
            Layout.fillWidth: true
            visible: root.editAltitude && !root.editCurrentAltitude
            wrapMode: Text.WordWrap
            text: qsTr("Keeps the waypoint's altitude frame. Pause the mission before saving; saving uploads the complete mission to the aircraft.")
        }
        QGCButton {
            objectName: "caacChangeFlightAltitude"
            Layout.fillWidth: true
            visible: root.editAltitude && !root.editCurrentAltitude
            text: qsTr("Change current flight altitude")
            enabled: root._canChangeCurrentAltitude
            onClicked: {
                if (enabled) {
                    root._error = ""
                    root.editCurrentAltitude = true
                    altitude.text = root._vehicle.altitudeRelative.rawValue.toString()
                }
            }
        }
        QGCLabel {
            Layout.fillWidth: true
            visible: (!root.editCurrentAltitude && root._choices.length === 0) || root._error.length > 0
            wrapMode: Text.WordWrap
            text: root._error || qsTr("No downloaded waypoints are available.")
        }
    }
}
