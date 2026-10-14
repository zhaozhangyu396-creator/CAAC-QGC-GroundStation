import QtQuick

import QGroundControl
import QGroundControl.Controls

Item {
    id: root
    objectName: "caacAttitudeInstrument"
    implicitWidth: Math.min(mainWindow.width * 0.24, mainWindow.height * 0.32,
        ScreenTools.defaultFontPixelHeight * 12)
    implicitHeight: implicitWidth
    width: implicitWidth
    height: implicitHeight

    property var vehicle: null
    readonly property real rollAngle: vehicle && isFinite(vehicle.roll.rawValue) ? vehicle.roll.rawValue : 0
    readonly property real pitchAngle: vehicle && isFinite(vehicle.pitch.rawValue) ? vehicle.pitch.rawValue : 0
    readonly property real rollIndicatorAngle: rollAngle
    readonly property bool attitudeAvailable: vehicle && isFinite(vehicle.roll.rawValue) && isFinite(vehicle.pitch.rawValue)
    readonly property real headingAngle: vehicle ? vehicle.heading.rawValue : NaN

    QGCPalette { id: palette; colorGroupEnabled: true }

    onRollAngleChanged: dial.requestPaint()
    onPitchAngleChanged: dial.requestPaint()

    onHeadingAngleChanged: dial.requestPaint()
    Canvas {
        id: dial
        anchors.fill: parent
        onWidthChanged: requestPaint()
        onHeightChanged: requestPaint()
        onPaint: {
            const ctx = getContext("2d")
            ctx.reset()
            const r = Math.min(width, height) / 2
            if (r <= 0) { return }
            const inner = r * 0.55
            ctx.translate(width / 2, height / 2)
            ctx.fillStyle = "#1d1b32"
            ctx.beginPath()
            ctx.arc(0, 0, r * 0.985, 0, Math.PI * 2)
            ctx.fill()
            ctx.strokeStyle = "#596071"
            ctx.lineWidth = Math.max(1, r * 0.008)
            ctx.stroke()
            ctx.textAlign = "center"
            ctx.textBaseline = "middle"
            ctx.font = Math.max(7, Math.round(r * 0.085)) + 'px "' + ScreenTools.normalFontFamily + '"'
            for (let bearing = 0; bearing < 360; bearing += 15) {
                ctx.save()
                ctx.rotate(bearing * Math.PI / 180)
                ctx.strokeStyle = bearing % 45 === 0 ? "white" : "#82828e"
                ctx.beginPath()
                ctx.moveTo(0, -r * 0.72)
                ctx.lineTo(0, -r * 0.79)
                ctx.stroke()
                if (r >= 100 || bearing % 45 === 0) {
                    ctx.fillStyle = "white"
                    ctx.fillText(bearing + "°", 0, -r * 0.88)
                }
                ctx.restore()
            }
            const names = ["N", "NE", "E", "SE", "S", "SW", "W", "NW"]
            ctx.font = Math.max(7, Math.round(r * 0.09)) + 'px "' + ScreenTools.normalFontFamily + '"'
            ctx.fillStyle = "white"
            for (let i = 0; i < names.length; ++i) {
                const angle = i * Math.PI / 4
                ctx.fillText(names[i], Math.sin(angle) * r * 0.64, -Math.cos(angle) * r * 0.64)
            }
            ctx.save()
            ctx.beginPath()
            ctx.arc(0, 0, inner, 0, Math.PI * 2)
            ctx.clip()
            ctx.save()
            ctx.rotate(-root.rollAngle * Math.PI / 180)
            ctx.translate(0, root.pitchAngle * inner / 30)
            ctx.fillStyle = "#3da4ef"
            ctx.fillRect(-inner * 6, -inner * 12, inner * 12, inner * 12)
            ctx.fillStyle = "#67b663"
            ctx.fillRect(-inner * 6, 0, inner * 12, inner * 12)
            ctx.strokeStyle = "white"
            ctx.fillStyle = "white"
            ctx.lineWidth = Math.max(1, r * 0.006)
            ctx.font = Math.max(7, Math.round(r * 0.065)) + 'px "' + ScreenTools.normalFontFamily + '"'
            for (let angle = -90; angle <= 90; angle += 5) {
                const y = -angle * inner / 30
                if (Math.abs(y + root.pitchAngle * inner / 30) > inner * 0.85) { continue }
                const halfLine = inner * (angle % 10 === 0 ? 0.3 : 0.16)
                ctx.beginPath()
                ctx.moveTo(-halfLine, y)
                ctx.lineTo(halfLine, y)
                ctx.stroke()
                if (angle !== 0 && angle % 10 === 0) {
                    ctx.fillText(angle + "°", -inner * 0.52, y)
                    ctx.fillText(angle + "°", inner * 0.52, y)
                }
            }
            ctx.restore()
            ctx.strokeStyle = palette.colorRed
            ctx.lineWidth = Math.max(1, r * 0.012)
            ctx.beginPath()
            ctx.moveTo(-inner * 0.45, 0)
            ctx.lineTo(inner * 0.45, 0)
            ctx.stroke()
            ctx.font = 'bold ' + Math.max(8, Math.round(r * 0.11)) + 'px "' + ScreenTools.normalFontFamily + '"'
            ctx.fillStyle = "white"
            ctx.fillText(root.attitudeAvailable ? Math.round(root.pitchAngle).toString() : "—", 0, 0)
            ctx.restore()
            ctx.strokeStyle = "#85888b"
            ctx.lineWidth = Math.max(1, r * 0.027)
            ctx.beginPath()
            ctx.arc(0, 0, inner, 0, Math.PI * 2)
            ctx.stroke()
            ctx.save()
            ctx.rotate((isFinite(root.headingAngle) ? root.headingAngle : 0) * Math.PI / 180)
            ctx.fillStyle = palette.colorRed
            ctx.beginPath()
            ctx.moveTo(0, -r * 0.61)
            ctx.lineTo(-r * 0.07, -r * 0.43)
            ctx.lineTo(0, -r * 0.46)
            ctx.lineTo(r * 0.07, -r * 0.43)
            ctx.closePath()
            ctx.fill()
            ctx.restore()
        }
    }

    Rectangle {
        visible: false
        anchors.horizontalCenter: parent.horizontalCenter
        y: root.height * 0.81
        width: root.width * 0.24
        height: root.height * 0.11
        radius: height / 2
        color: Qt.rgba(0, 0, 0, 0.5)
        QGCLabel {
            objectName: "caacAttitudeHeading"
            anchors.centerIn: parent
            color: "white"
            font.pointSize: ScreenTools.defaultFontPointSize
            text: isFinite(root.headingAngle)
                ? String(Math.round((root.headingAngle % 360 + 360) % 360) % 360 || 360)
                : qsTr("Not available")
        }
    }

    QGCLabel {
        anchors.horizontalCenter: parent.horizontalCenter
        y: root.height * 0.63
        visible: !root.attitudeAvailable
        color: "white"
        font.pointSize: ScreenTools.smallFontPointSize
        text: qsTr("Not available")
    }
}
