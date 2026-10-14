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
    readonly property var _vertex: {
        if (!routeController || !waypoint) { return ({}) }
        routeController.coordinateItems
        return routeController.waypointAngles(routeController.simulationSettings.closed)
            .find(point => point.sequence === waypoint.sequenceNumber) || ({})
    }

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
                if (number(verticalSpeed.text) !== Number(_advanced.verticalSpeed)
                    || climbMode.currentIndex !== _advanced.climbMode || turnMode.currentIndex !== _advanced.turnMode) {
                    properties.verticalSpeed = number(verticalSpeed.text)
                    properties.climbMode = climbMode.currentIndex
                    properties.turnMode = turnMode.currentIndex
                }
            }
            if (positionMode.currentIndex === 2) { properties.vertexAngle = number(vertexAngle.text) }
            preventClose = !routeController.updateWaypointProperties(waypoint, properties)
        }
    }

    ColumnLayout {
        width: Math.min(root.maxContentAvailableWidth, ScreenTools.defaultFontPixelWidth * 80)
        spacing: ScreenTools.defaultFontPixelHeight * 0.5

        QGCLabel {
            text: root.waypoint ? qsTr("Waypoint %1").arg(root.waypoint.waypointNumber) : ""
            font.bold: true
        }

        GridLayout {
            id: waypointFields
            Layout.fillWidth: true
            columns: width < ScreenTools.defaultFontPixelWidth * 68 ? 1 : 2
            columnSpacing: ScreenTools.defaultFontPixelWidth

            QGCLabel { text: qsTr("Position editing") }
            QGCComboBox {
                id: positionMode
                objectName: "caacWaypointPositionMode"
                Layout.fillWidth: true
                model: [qsTr("Coordinates"), qsTr("Relative to previous waypoint"), qsTr("Current vertex angle")]
                enabled: root._leg.distance !== undefined
            }
            QGCLabel { text: qsTr("Latitude (degrees)"); visible: positionMode.currentIndex === 0 }
            CaacCoordinateField {
                id: latitude
                fieldName: "caacWaypointLatitude"
                maximumDegrees: 90
                compact: true
                Layout.fillWidth: true
                value: root.waypoint ? root.waypoint.coordinate.latitude : NaN
                visible: positionMode.currentIndex === 0
            }
            QGCLabel { text: qsTr("Longitude (degrees)"); visible: positionMode.currentIndex === 0 }
            CaacCoordinateField {
                id: longitude
                fieldName: "caacWaypointLongitude"
                maximumDegrees: 180
                compact: true
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
            QGCLabel { text: qsTr("Bearing (degrees)") }
            QGCTextField {
                objectName: "caacWaypointBearing"
                Layout.fillWidth: true
                text: root._leg.bearing !== undefined ? root._leg.bearing.toFixed(6) : ""
                enabled: root._leg.bearing !== undefined
                validator: DoubleValidator { locale: "C" }
                onTextEdited: { positionMode.currentIndex = 1; angleType.currentIndex = 0; relativeAngle.text = text }
            }
            QGCLabel {
                text: qsTr("Interior angle at previous waypoint (degrees)")
                Layout.fillWidth: true
                Layout.minimumWidth: 0
                Layout.preferredWidth: ScreenTools.defaultFontPixelWidth * 24
                wrapMode: Text.WordWrap
            }
            QGCTextField {
                objectName: "caacWaypointInteriorAngle"
                Layout.fillWidth: true
                text: root._leg.angle !== undefined ? Math.min(root._leg.angle, 360 - root._leg.angle).toFixed(6) : ""
                enabled: root._leg.angle !== undefined
                validator: DoubleValidator { bottom: 0; top: 180; locale: "C" }
                onTextEdited: {
                    positionMode.currentIndex = 1
                    angleType.currentIndex = 1
                    relativeAngle.text = root._leg.angle > 180 ? (360 - Number(text)).toString() : text
                }
            }
            QGCLabel {
                text: qsTr("Current vertex angle (degrees)")
                Layout.fillWidth: true
                Layout.minimumWidth: 0
                Layout.preferredWidth: ScreenTools.defaultFontPixelWidth * 24
                wrapMode: Text.WordWrap
            }
            QGCTextField {
                id: vertexAngle
                objectName: "caacWaypointVertexAngle"
                Layout.fillWidth: true
                text: root._vertex.angle !== undefined ? root._vertex.angle.toFixed(6) : ""
                enabled: root._vertex.angle !== undefined
                validator: DoubleValidator { bottom: 0; top: 180; locale: "C" }
                onTextEdited: positionMode.currentIndex = 2
            }
            QGCLabel { text: qsTr("Distance from previous waypoint (m)") }
            QGCTextField {
                id: relativeDistance
                objectName: "caacWaypointRelativeDistance"
                Layout.preferredWidth: ScreenTools.defaultFontPixelWidth * 20
                Layout.fillWidth: true
                text: root._leg.distance !== undefined ? root._leg.distance.toFixed(3) : ""
                enabled: root._leg.distance !== undefined
                validator: DoubleValidator { bottom: 0; locale: "C" }
                onTextEdited: positionMode.currentIndex = 1
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
            QGCLabel { text: qsTr("Vertical speed (m/s)"); visible: root._advanced.supported === true }
            QGCTextField {
                id: verticalSpeed
                objectName: "caacWaypointVerticalSpeed"
                Layout.fillWidth: true
                visible: root._advanced.supported === true
                text: root._advanced.verticalSpeed !== undefined ? root._advanced.verticalSpeed.toString() : ""
                validator: DoubleValidator { bottom: 0; top: 30; locale: "C" }
            }
            QGCLabel { text: qsTr("Altitude change mode"); visible: root._advanced.supported === true }
            QGCComboBox {
                id: climbMode
                objectName: "caacWaypointClimbMode"
                Layout.fillWidth: true
                visible: root._advanced.supported === true
                model: [qsTr("Change altitude before the leg"), qsTr("Change altitude along the leg")]
                currentIndex: root._advanced.climbMode !== undefined ? root._advanced.climbMode : 1
            }
            QGCLabel { text: qsTr("Turn mode"); visible: root._advanced.supported === true }
            QGCComboBox {
                id: turnMode
                objectName: "caacWaypointTurnMode"
                Layout.fillWidth: true
                visible: root._advanced.supported === true
                model: [qsTr("Point turn"), qsTr("Coordinated turn"), qsTr("Adaptive coordinated turn"), qsTr("No turn")]
                currentIndex: root._advanced.turnMode !== undefined ? root._advanced.turnMode : 1
            }
            QGCLabel { text: qsTr("Hold duration (s)"); visible: root._advanced.supported === true }
            QGCTextField {
                id: holdDuration
                objectName: "caacWaypointHoldDuration"
                Layout.fillWidth: true
                visible: root._advanced.supported === true
                text: root._advanced.holdSeconds !== undefined ? root._advanced.holdSeconds.toString() : ""
                validator: DoubleValidator { bottom: 0; locale: "C" }
            }
            QGCLabel { text: qsTr("Next leg heading"); visible: root._advanced.supported === true }
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
            text: qsTr("The current vertex angle matches the map. Editing it moves this point along the previous leg extension. Vertical speed, climb and turn modes apply to the built-in simulation.")
        }
        QGCLabel {
            Layout.fillWidth: true
            wrapMode: Text.WordWrap
            text: root.routeController ? root.routeController.lastError : ""
            visible: text.length > 0
        }
    }
}
