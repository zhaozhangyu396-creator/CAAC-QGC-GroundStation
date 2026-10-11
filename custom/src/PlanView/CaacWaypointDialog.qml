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
    readonly property var _leg: routeController && routeController.coordinateItems.length > 0
        ? routeController.waypointLeg(waypoint) : ({})

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
        if (!routeController) {
            preventClose = true
        } else if (positionMode.currentIndex === 1) {
            preventClose = !routeController.updateRelativeWaypoint(waypoint, number(relativeAngle.text),
                number(relativeDistance.text), angleType.currentIndex === 1,
                number(altitude.text), specifySpeed.checked, number(speed.text))
        } else {
            preventClose = !routeController.updateWaypoint(waypoint, number(latitude.text),
                number(longitude.text), number(altitude.text), specifySpeed.checked, number(speed.text))
        }
    }

    ColumnLayout {
        spacing: ScreenTools.defaultFontPixelHeight * 0.5

        QGCLabel {
            text: root.waypoint ? qsTr("Waypoint %1").arg(root.waypoint.sequenceNumber) : ""
            font.bold: true
        }

        GridLayout {
            columns: 2
            columnSpacing: ScreenTools.defaultFontPixelWidth

            QGCLabel { text: qsTr("Position editing") }
            QGCComboBox {
                id: positionMode
                objectName: "caacWaypointPositionMode"
                Layout.fillWidth: true
                model: [qsTr("Coordinates"), qsTr("Relative to previous waypoint")]
                enabled: root._leg.distance !== undefined
            }
            QGCLabel { text: qsTr("Latitude (degrees)"); visible: positionMode.currentIndex === 0 }
            QGCTextField {
                id: latitude
                objectName: "caacWaypointLatitude"
                Layout.preferredWidth: ScreenTools.defaultFontPixelWidth * 20
                text: root.waypoint ? root.waypoint.coordinate.latitude.toString() : ""
                visible: positionMode.currentIndex === 0
                validator: DoubleValidator { bottom: -90; top: 90; decimals: 17; locale: "C" }
            }
            QGCLabel { text: qsTr("Longitude (degrees)"); visible: positionMode.currentIndex === 0 }
            QGCTextField {
                id: longitude
                objectName: "caacWaypointLongitude"
                Layout.fillWidth: true
                text: root.waypoint ? root.waypoint.coordinate.longitude.toString() : ""
                visible: positionMode.currentIndex === 0
                validator: DoubleValidator { bottom: -180; top: 180; decimals: 17; locale: "C" }
            }
            QGCLabel { text: qsTr("Angle type"); visible: positionMode.currentIndex === 1 }
            QGCComboBox {
                id: angleType
                objectName: "caacWaypointAngleType"
                Layout.fillWidth: true
                visible: positionMode.currentIndex === 1
                model: root._leg.angle !== undefined ? [qsTr("Bearing"), qsTr("Interior angle")] : [qsTr("Bearing")]
                currentIndex: root._leg.angle !== undefined ? 1 : 0
            }
            QGCLabel { text: qsTr("Distance from previous waypoint (m)"); visible: positionMode.currentIndex === 1 }
            QGCTextField {
                id: relativeDistance
                objectName: "caacWaypointRelativeDistance"
                Layout.preferredWidth: ScreenTools.defaultFontPixelWidth * 20
                Layout.fillWidth: true
                visible: positionMode.currentIndex === 1
                text: root._leg.distance !== undefined ? root._leg.distance.toFixed(3) : ""
                validator: DoubleValidator { bottom: 0; locale: "C" }
            }
            QGCLabel { text: qsTr("Angle (degrees)"); visible: positionMode.currentIndex === 1 }
            QGCTextField {
                id: relativeAngle
                objectName: "caacWaypointRelativeAngle"
                Layout.fillWidth: true
                visible: positionMode.currentIndex === 1
                text: angleType.currentIndex === 1 ? root._leg.angle.toFixed(6)
                    : root._leg.bearing !== undefined ? root._leg.bearing.toFixed(6) : "0"
                validator: DoubleValidator { locale: "C" }
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
            Layout.maximumWidth: ScreenTools.defaultFontPixelWidth * 55
            visible: positionMode.currentIndex === 1
            wrapMode: Text.WordWrap
            text: root._leg.angle !== undefined
                ? qsTr("At the previous waypoint: interior angle 90° turns left, 180° continues straight. Negative angles are supported.")
                : qsTr("No incoming leg at the previous point: use a bearing (true north 0°, east 90°).")
        }
        QGCLabel {
            Layout.fillWidth: true
            wrapMode: Text.WordWrap
            text: root.routeController ? root.routeController.lastError : ""
            visible: text.length > 0
        }
    }
}
