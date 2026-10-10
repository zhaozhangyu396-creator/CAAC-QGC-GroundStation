import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import QGroundControl
import QGroundControl.Controls

QGCPopupDialog {
    id: root
    objectName: "caacWaypointDialog"
    title: qsTr("Waypoint properties")
    buttons: Dialog.Ok | Dialog.Cancel

    property var routeController: null
    property var waypoint: null
    readonly property var _speedSection: waypoint ? waypoint.speedSection : null

    function number(text) {
        return text.trim().length ? Number(text) : NaN
    }

    function altitudeFrameLabel() {
        if (!waypoint) {
            return ""
        }
        switch (waypoint.altitudeFrame) {
        case QGroundControl.AltitudeFrameRelative: return qsTr("Relative to home")
        case QGroundControl.AltitudeFrameAbsolute: return qsTr("AMSL")
        case QGroundControl.AltitudeFrameTerrain: return qsTr("Terrain frame")
        case QGroundControl.AltitudeFrameCalcAboveTerrain: return qsTr("Above terrain")
        default: return qsTr("Existing mission frame")
        }
    }

    onAccepted: {
        preventClose = !routeController || !routeController.updateWaypoint(waypoint, number(latitude.text),
            number(longitude.text), number(altitude.text), specifySpeed.checked, number(speed.text))
    }

    ColumnLayout {
        spacing: ScreenTools.defaultFontPixelHeight * 0.5

        GridLayout {
            columns: 2
            columnSpacing: ScreenTools.defaultFontPixelWidth

            QGCLabel { text: qsTr("Latitude (degrees)") }
            QGCTextField {
                id: latitude
                objectName: "caacWaypointLatitude"
                Layout.preferredWidth: ScreenTools.defaultFontPixelWidth * 20
                text: root.waypoint ? root.waypoint.coordinate.latitude.toString() : ""
                validator: DoubleValidator { bottom: -90; top: 90; decimals: 17; locale: "C" }
            }
            QGCLabel { text: qsTr("Longitude (degrees)") }
            QGCTextField {
                id: longitude
                objectName: "caacWaypointLongitude"
                Layout.fillWidth: true
                text: root.waypoint ? root.waypoint.coordinate.longitude.toString() : ""
                validator: DoubleValidator { bottom: -180; top: 180; decimals: 17; locale: "C" }
            }
            QGCLabel { text: qsTr("Altitude (m)") }
            QGCTextField {
                id: altitude
                objectName: "caacWaypointAltitude"
                Layout.fillWidth: true
                text: root.waypoint ? root.waypoint.altitude.rawValue.toString() : ""
                validator: DoubleValidator { locale: "C" }
            }
            QGCLabel { text: qsTr("Altitude frame") }
            QGCLabel { text: root.altitudeFrameLabel() }
            QGCCheckBox {
                id: specifySpeed
                objectName: "caacWaypointSpecifySpeed"
                text: qsTr("Set flight speed (m/s)")
                enabled: root._speedSection && root._speedSection.available
                checked: root._speedSection ? root._speedSection.specifyFlightSpeed : false
            }
            QGCTextField {
                id: speed
                objectName: "caacWaypointSpeed"
                Layout.fillWidth: true
                enabled: specifySpeed.checked && specifySpeed.enabled
                text: root._speedSection ? root._speedSection.flightSpeed.rawValue.toString() : ""
                validator: DoubleValidator { bottom: 0; locale: "C" }
            }
        }
        QGCLabel {
            Layout.fillWidth: true
            wrapMode: Text.WordWrap
            text: root.routeController ? root.routeController.lastError : ""
            visible: text.length > 0
        }
    }
}
