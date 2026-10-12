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
    readonly property bool attitudeAvailable: vehicle && isFinite(vehicle.roll.rawValue) && isFinite(vehicle.pitch.rawValue)
    readonly property real headingAngle: vehicle ? vehicle.heading.rawValue : NaN

    QGCPalette { id: palette; colorGroupEnabled: true }

    onRollAngleChanged: dial.requestPaint()
    onPitchAngleChanged: dial.requestPaint()

    Canvas {
        id: dial
        anchors.fill: parent
        onWidthChanged: requestPaint()
        onHeightChanged: requestPaint()
        onPaint: {
            const ctx = getContext("2d")
            ctx.reset()
            const radius = Math.min(width, height) / 2
            const edge = radius * 0.985
            ctx.translate(width / 2, height / 2)
            ctx.save()
            ctx.beginPath()
            ctx.arc(0, 0, edge, 0, Math.PI * 2)
            ctx.clip()
            ctx.save()
            ctx.rotate(-root.rollAngle * Math.PI / 180)
            ctx.translate(0, root.pitchAngle * radius / 40)
            const sky = ctx.createLinearGradient(0, -radius, 0, radius)
            sky.addColorStop(0, Qt.lighter(palette.brandPrimary, 1.15))
            sky.addColorStop(1, Qt.darker(palette.brandPrimary, 1.25))
            ctx.fillStyle = sky
            ctx.fillRect(-radius * 4, -radius * 8, radius * 8, radius * 8)
            const ground = ctx.createLinearGradient(0, 0, 0, radius)
            ground.addColorStop(0, palette.colorYellowGreen)
            ground.addColorStop(1, palette.colorGreen)
            ctx.fillStyle = ground
            ctx.fillRect(-radius * 4, 0, radius * 8, radius * 8)
            ctx.strokeStyle = "white"
            ctx.fillStyle = "white"
            ctx.lineWidth = radius * 0.009
            ctx.font = Math.round(radius * 0.09) + 'px "' + ScreenTools.normalFontFamily + '"'
            ctx.textAlign = "center"
            ctx.textBaseline = "middle"
            for (let angle = -90; angle <= 90; angle += 5) {
                const y = -angle * radius / 40
                if (Math.abs(y + root.pitchAngle * radius / 40) > radius * 0.55) { continue }
                const halfLine = radius * (angle % 10 === 0 ? 0.19 : 0.14)
                ctx.beginPath()
                ctx.moveTo(-halfLine, y)
                ctx.lineTo(halfLine, y)
                ctx.stroke()
                if (angle % 10 === 0) {
                    ctx.fillText(String(angle), -radius * 0.34, y)
                    ctx.fillText(String(angle), radius * 0.34, y)
                }
            }
            ctx.restore()
            ctx.strokeStyle = "white"
            ctx.fillStyle = "white"
            ctx.font = Math.round(radius * 0.09) + 'px "' + ScreenTools.normalFontFamily + '"'
            ctx.textAlign = "center"
            ctx.textBaseline = "middle"
            ctx.rotate(-root.rollAngle * Math.PI / 180)
            for (const angle of [-60, -45, -30, -20, -10, 0, 10, 20, 30, 45, 60]) {
                ctx.save()
                ctx.rotate(angle * Math.PI / 180)
                ctx.beginPath()
                ctx.moveTo(0, -radius * 0.85)
                ctx.lineTo(0, -radius * (angle % 30 === 0 ? 0.76 : 0.81))
                ctx.lineWidth = radius * 0.016
                ctx.stroke()
                if (angle % 30 === 0) {
                    ctx.fillText(String(Math.abs(angle)), 0, -radius * 0.69)
                }
                ctx.restore()
            }
            ctx.restore()
            ctx.strokeStyle = "white"
            ctx.lineWidth = radius * 0.025
            ctx.beginPath()
            ctx.arc(0, 0, edge - ctx.lineWidth / 2, 0, Math.PI * 2)
            ctx.stroke()
            ctx.fillStyle = palette.colorRed
            ctx.beginPath()
            ctx.moveTo(0, -radius * 0.94)
            ctx.lineTo(-radius * 0.045, -radius * 0.83)
            ctx.lineTo(0, -radius * 0.85)
            ctx.lineTo(radius * 0.045, -radius * 0.83)
            ctx.closePath()
            ctx.fill()
            ctx.lineWidth = radius * 0.009
            ctx.stroke()
            ctx.strokeStyle = palette.colorRed
            ctx.lineWidth = radius * 0.026
            ctx.lineCap = "round"
            ctx.beginPath()
            ctx.moveTo(-radius * 0.72, 0)
            ctx.lineTo(-radius * 0.35, 0)
            ctx.moveTo(-radius * 0.26, radius * 0.08)
            ctx.lineTo(0, 0)
            ctx.lineTo(radius * 0.26, radius * 0.08)
            ctx.moveTo(radius * 0.35, 0)
            ctx.lineTo(radius * 0.72, 0)
            ctx.stroke()
        }
    }

    Rectangle {
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
