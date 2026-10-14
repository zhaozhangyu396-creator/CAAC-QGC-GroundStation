pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import QGroundControl
import QGroundControl.Controls

QGCPopupDialog {
    id: root
    objectName: "caacFlightSpeedDialog"
    title: qsTr("Change flight speed")
    buttons: Dialog.Ok | Dialog.Cancel
    readonly property var vehicle: QGroundControl.multiVehicleManager.activeVehicle
    property var openedVehicle: null
    property string errorText: ""
    readonly property var conversion: QGroundControl.unitsConversion
    readonly property bool forwardFlight: vehicle && vehicle.inFwdFlight
    readonly property real minimumSpeed: vehicle && forwardFlight ? vehicle.minimumEquivalentAirspeed() : 0.1
    readonly property real maximumSpeed: !vehicle ? 0 : forwardFlight
        ? vehicle.maximumEquivalentAirspeed() : vehicle.maximumHorizontalSpeedMultirotorMetersSecond()
    readonly property bool canChange: vehicle && vehicle === openedVehicle && vehicle.initialConnectComplete
        && vehicle.armed && vehicle.airborne && vehicle.supports.guidedMode
    Component.onCompleted: openedVehicle = vehicle

    onAccepted: {
        const value = speed.text.trim().length ? conversion.appSettingsSpeedUnitsToMetersSecond(Number(speed.text)) : NaN
        const minimum = vehicle && forwardFlight ? vehicle.minimumEquivalentAirspeed() : 0.1
        const maximum = !vehicle ? 0 : forwardFlight
            ? vehicle.maximumEquivalentAirspeed() : vehicle.maximumHorizontalSpeedMultirotorMetersSecond()
        preventClose = !canChange || !isFinite(value) || !isFinite(minimum) || !isFinite(maximum)
            || maximum < minimum || value < minimum || value > maximum
        if (preventClose) { errorText = qsTr("Enter a valid flight speed within the aircraft limits."); return }
        if (forwardFlight) { vehicle.guidedModeChangeEquivalentAirspeedMetersSecond(value) }
        else { vehicle.guidedModeChangeGroundSpeedMetersSecond(value) }
    }
    ColumnLayout {
        width: Math.min(root.maxContentAvailableWidth, ScreenTools.defaultFontPixelWidth * 45)
        QGCLabel {
            Layout.fillWidth: true
            text: root.forwardFlight ? qsTr("Airspeed") : qsTr("Horizontal speed")
        }
        QGCTextField {
            id: speed
            objectName: "caacFlightSpeedValue"
            Layout.fillWidth: true
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
            text: qsTr("Later waypoint speed commands may override this speed.")
        }
        QGCLabel {
            Layout.fillWidth: true
            wrapMode: Text.WordWrap
            visible: root.errorText.length > 0
            text: root.errorText
        }
    }
}
