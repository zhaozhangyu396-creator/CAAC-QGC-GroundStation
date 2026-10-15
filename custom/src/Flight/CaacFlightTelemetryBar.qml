pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import QGroundControl
import QGroundControl.Controls

Rectangle {
    id: root
    objectName: "caacFlightTelemetryBar"
    required property var vehicle
    required property var missionController
    readonly property real margin: ScreenTools.defaultFontPixelWidth
    readonly property real telemetryPointSize: ScreenTools.smallFontPointSize * 1.1
        * Math.max(0.7, Math.min(1, width / (ScreenTools.defaultFontPixelWidth * 85)))
    readonly property var battery: vehicle && vehicle.batteries.count ? vehicle.batteries.get(0) : null
    readonly property real headingAngle: vehicle && isFinite(Number(vehicle.heading.rawValue))
        ? (Number(vehicle.heading.rawValue) % 360 + 360) % 360 : NaN
    implicitHeight: ScreenTools.defaultFontPixelHeight * 3.1
    color: "#cf222222"
    radius: ScreenTools.defaultFontPixelHeight
    readonly property var target: {
        if (!vehicle || !missionController) { return null }
        const sequence = missionController.currentMissionIndex
        for (let i = 1; i < missionController.visualItems.count; ++i) {
            const item = missionController.visualItems.get(i)
            if (item.specifiesCoordinate && item.sequenceNumber <= sequence && sequence <= item.lastSequenceNumber) { return item }
        }
        return null
    }
    readonly property var targetMetrics: vehicle && target
        ? QGroundControl.corePlugin.routeLegMetrics(vehicle.coordinate, target.coordinate) : ({})
    function formatted(fact) {
        return fact && isFinite(Number(fact.rawValue)) ? fact.valueString + " " + fact.units : "—"
    }
    RowLayout {
        anchors.fill: parent
        anchors.margins: root.margin * 0.5
        spacing: root.margin * 0.4
        Repeater {
            model: [
                {key: "Battery", label: qsTr("Battery"), fact: null},
                {key: "Altitude", label: qsTr("Flight altitude"), fact: root.vehicle ? root.vehicle.altitudeRelative : null},
                {key: "HomeDistance", label: qsTr("Home distance"), fact: root.vehicle ? root.vehicle.distanceToHome : null},
                {key: "Heading", label: "", fact: null},
                {key: "HorizontalSpeed", label: qsTr("Horizontal speed"), fact: root.vehicle ? root.vehicle.groundSpeed : null},
                {key: "VerticalSpeed", label: qsTr("Vertical speed"), fact: root.vehicle ? root.vehicle.climbRate : null},
                {key: "TargetDistance", label: qsTr("Target distance"), fact: null}
            ]
            delegate: Item {
                id: cell
                required property var modelData
                Layout.fillWidth: true
                Layout.minimumWidth: 0
                Layout.fillHeight: true
                ColumnLayout {
                    anchors.fill: parent
                    visible: cell.modelData.key !== "Heading"
                    spacing: 0
                    QGCLabel {
                        objectName: "caacTelemetry" + cell.modelData.key + "Value"
                        Layout.fillWidth: true
                        Layout.alignment: Qt.AlignVCenter
                        horizontalAlignment: Text.AlignHCenter
                        color: "white"
                        font.pointSize: root.telemetryPointSize
                        font.weight: Font.DemiBold
                        fontSizeMode: Text.HorizontalFit
                        minimumPointSize: ScreenTools.smallFontPointSize * 0.7
                        text: cell.modelData.key === "Battery" ? (root.battery
                            ? root.formatted(root.battery.voltage) + " · " + root.formatted(root.battery.percentRemaining) : "— · —")
                            : cell.modelData.key === "TargetDistance" ? (root.targetMetrics.distance !== undefined
                                ? QGroundControl.unitsConversion.metersToAppSettingsHorizontalDistanceUnits(root.targetMetrics.distance).toFixed(1)
                                    + " " + (root.vehicle ? root.vehicle.distanceToHome.units : "") : "—")
                                : root.formatted(cell.modelData.fact)
                    }
                    QGCLabel {
                        Layout.fillWidth: true
                        horizontalAlignment: Text.AlignHCenter
                        color: "#c8c8c8"
                        text: cell.modelData.label
                        font.pointSize: root.telemetryPointSize
                        font.weight: Font.DemiBold
                    }
                }
                Canvas {
                    id: compass
                    objectName: "caacTelemetryHeadingArrow"
                    readonly property real headingAngle: root.headingAngle
                    anchors.centerIn: parent
                    width: parent.height * 1.2
                    height: width
                    visible: cell.modelData.key === "Heading"
                    onHeadingAngleChanged: requestPaint()
                    onWidthChanged: requestPaint()
                    onPaint: {
                        const ctx = getContext("2d")
                        ctx.reset()
                        const r = width / 2
                        if (r <= 0) { return }
                        ctx.translate(r, r)
                        ctx.fillStyle = "#242424"
                        ctx.beginPath(); ctx.arc(0, 0, r * 0.98, 0, Math.PI * 2); ctx.fill()
                        ctx.strokeStyle = "#747474"; ctx.lineWidth = 1; ctx.stroke()
                        for (const fraction of [0.81, 0.68, 0.5]) {
                            ctx.beginPath(); ctx.arc(0, 0, r * fraction, 0, Math.PI * 2); ctx.stroke()
                        }
                        ctx.translate(0, -r * 0.16)
                        ctx.rotate((isFinite(root.headingAngle) ? root.headingAngle : 0) * Math.PI / 180)
                        ctx.fillStyle = isFinite(root.headingAngle) ? "white" : "#888888"
                        ctx.beginPath()
                        ctx.moveTo(0, -r * 0.4); ctx.lineTo(r * 0.27, r * 0.24)
                        ctx.lineTo(0, r * 0.12); ctx.lineTo(-r * 0.27, r * 0.24)
                        ctx.closePath(); ctx.fill()
                    }
                    Rectangle {
                        objectName: "caacTelemetryHeadingFrame"
                        anchors.horizontalCenter: parent.horizontalCenter
                        y: parent.height * 0.69
                        width: headingValue.implicitWidth + ScreenTools.defaultFontPixelWidth * 0.6
                        height: headingValue.implicitHeight + ScreenTools.defaultFontPixelHeight * 0.18
                        color: "black"
                        border.color: "white"
                        border.width: 1
                        radius: 2
                        z: 1
                        QGCLabel {
                            id: headingValue
                            objectName: "caacTelemetryHeadingValue"
                            anchors.centerIn: parent
                            text: isFinite(root.headingAngle) ? root.headingAngle.toFixed(1) + "°" : "—"
                            color: "white"
                            font.pointSize: ScreenTools.smallFontPointSize * 0.935
                            font.weight: Font.DemiBold
                        }
                    }
                }
            }
        }
    }
}
