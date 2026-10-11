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
    readonly property var _advanced: routeController && waypoint ? routeController.waypointAdvancedProperties(waypoint) : ({})

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
        } else {
            const properties = {
                relative: positionMode.currentIndex === 1,
                latitude: latitude.coordinate, longitude: longitude.coordinate,
                angle: number(relativeAngle.text), distance: number(relativeDistance.text),
                interiorAngle: angleType.currentIndex === 1,
                altitude: number(altitude.text), specifySpeed: specifySpeed.checked, speed: number(speed.text)
            }
            if (_advanced.supported) {
                properties.holdSeconds = number(holdDuration.text)
                properties.specifyYaw = yawMode.currentIndex === 1
                properties.yaw = number(yaw.text)
            }
            preventClose = !routeController.updateWaypointProperties(waypoint, properties)
        }
    }

    ColumnLayout {
        width: Math.min(root.maxContentAvailableWidth, ScreenTools.defaultFontPixelWidth * 80)
        spacing: ScreenTools.defaultFontPixelHeight * 0.5

        QGCLabel {
            text: root.waypoint ? qsTr("Waypoint %1").arg(root.waypoint.sequenceNumber) : ""
            font.bold: true
        }

        GridLayout {
            id: waypointFields
            Layout.fillWidth: true
            columns: root.maxContentAvailableWidth < ScreenTools.defaultFontPixelWidth * 68 ? 1 : 2
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
            CaacCoordinateField {
                id: latitude
                fieldName: "caacWaypointLatitude"
                maximumDegrees: 90
                Layout.fillWidth: true
                value: root.waypoint ? root.waypoint.coordinate.latitude : NaN
                visible: positionMode.currentIndex === 0
            }
            QGCLabel { text: qsTr("Longitude (degrees)"); visible: positionMode.currentIndex === 0 }
            CaacCoordinateField {
                id: longitude
                fieldName: "caacWaypointLongitude"
                maximumDegrees: 180
                Layout.fillWidth: true
                value: root.waypoint ? root.waypoint.coordinate.longitude : NaN
                visible: positionMode.currentIndex === 0
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
            QGCLabel { text: qsTr("Advanced settings"); font.bold: true; visible: root._advanced.supported === true; Layout.columnSpan: waypointFields.columns }
            QGCLabel { text: qsTr("Hold duration (s)"); visible: root._advanced.supported === true }
            QGCTextField {
                id: holdDuration
                objectName: "caacWaypointHoldDuration"
                Layout.fillWidth: true
                visible: root._advanced.supported === true
                text: root._advanced.holdSeconds !== undefined ? root._advanced.holdSeconds.toString() : ""
                validator: DoubleValidator { bottom: 0; locale: "C" }
            }
            QGCLabel { text: qsTr("Waypoint yaw"); visible: root._advanced.supported === true }
            QGCComboBox {
                id: yawMode
                objectName: "caacWaypointYawMode"
                Layout.fillWidth: true
                visible: root._advanced.supported === true
                model: [qsTr("Unspecified"), qsTr("Specified")]
                currentIndex: root._advanced.specifyYaw ? 1 : 0
            }
            QGCLabel { text: qsTr("Yaw (°)"); visible: root._advanced.supported === true && yawMode.currentIndex === 1 }
            QGCTextField {
                id: yaw
                objectName: "caacWaypointYaw"
                Layout.fillWidth: true
                visible: root._advanced.supported === true && yawMode.currentIndex === 1
                text: root._advanced.yaw !== undefined ? root._advanced.yaw.toString() : "0"
                validator: DoubleValidator { locale: "C" }
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
