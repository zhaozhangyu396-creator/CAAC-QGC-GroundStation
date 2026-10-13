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
    title: editAltitude ? qsTr("Change altitude") : qsTr("Jump to waypoint")
    buttons: Dialog.Ok | Dialog.Cancel

    Component.onCompleted: {
        _openedVehicle = _vehicle
        // Freeze the choices so a downloaded replacement mission cannot retarget an open dialog.
        const choices = []
        if (editAltitude) { choices.push({all: true, label: qsTr("All waypoints")}) }
        if (_items) {
            for (let i = 1; i < _items.count; ++i) {
                const item = _items.get(i)
                if (item.isSimpleItem && item.command === 16) {
                    choices.push({item: item, sequence: item.sequenceNumber, altitude: item.altitude.rawValue,
                        label: qsTr("Waypoint %1").arg(item.sequenceNumber)})
                }
            }
        }
        _choices = choices
    }

    function selectedItemIsCurrent() {
        if (!_vehicle || _vehicle !== _openedVehicle || !_items || waypoint.currentIndex < 0
                || waypoint.currentIndex >= _choices.length || _master.syncInProgress) {
            return false
        }
        const choice = _choices[waypoint.currentIndex]
        const selected = choice.all ? _choices.filter(entry => !entry.all) : [choice]
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
            const height = altitude.text.trim().length ? Number(altitude.text) : NaN
            const settings = QGroundControl.settingsManager.flyViewSettings
            if (!_vehicle || _vehicle !== _openedVehicle || !globals.guidedControllerFlyView.showChangeAlt) {
                _error = qsTr("Aircraft or mission changed. Close this window and select again.")
                preventClose = true
                return
            }
            if (!isFinite(height) || height < settings.guidedMinimumAltitude.rawValue
                    || height > settings.guidedMaximumAltitude.rawValue) {
                _error = qsTr("Altitude must be between %1 and %2 metres.")
                    .arg(settings.guidedMinimumAltitude.rawValue).arg(settings.guidedMaximumAltitude.rawValue)
                preventClose = true
                return
            }
            globals.guidedControllerFlyView.closeAll()
            _vehicle.guidedModeChangeAltitude(height - _vehicle.altitudeRelative.rawValue, false)
            return
        }
        if (!selectedItemIsCurrent()) {
            _error = qsTr("Aircraft or mission changed. Close this window and select again.")
            preventClose = true
            return
        }
        const choice = _choices[waypoint.currentIndex]
        if (editAltitude) {
            const selected = choice.all ? _choices.filter(entry => !entry.all) : [choice]
            _error = QGroundControl.corePlugin.updateFlightWaypointAltitudes(_master, selected.map(entry => entry.item),
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
            validator: DoubleValidator { locale: "C" }
            text: root.editCurrentAltitude && root._vehicle ? root._vehicle.altitudeRelative.rawValue.toString()
                : waypoint.currentIndex >= 0 && waypoint.currentIndex < root._choices.length
                ? (root._choices[waypoint.currentIndex].all ? "" : root._choices[waypoint.currentIndex].altitude.toString()) : ""
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
            enabled: root._vehicle && root._vehicle === root._openedVehicle
                && globals.guidedControllerFlyView.showChangeAlt
            onClicked: {
                if (enabled) {
                    root.editCurrentAltitude = true
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
