pragma ComponentBehavior: Bound

import QtQuick

import QGroundControl
import QGroundControl.Controls

Item {
    id: root

    QGCPalette { id: palette; colorGroupEnabled: true }

    Canvas {
        id: field
        anchors.fill: parent
        readonly property color grassColor: Qt.darker(Qt.tint(palette.colorYellowGreen,
            Qt.alpha(palette.colorGreen, 0.2)), 1.25)
        readonly property color grassHighlight: Qt.lighter(grassColor, 1.25)
        readonly property color corridorColor: Qt.tint(palette.colorGreen, Qt.rgba(1, 1, 1, 0.65))
        readonly property color referenceColor: palette.colorRed
        readonly property color markerColor: palette.colorGreen
        readonly property color markerBorder: palette.mapWidgetBorderLight
        onWidthChanged: requestPaint()
        onHeightChanged: requestPaint()
        onGrassColorChanged: requestPaint()
        onCorridorColorChanged: requestPaint()
        onReferenceColorChanged: requestPaint()
        onMarkerColorChanged: requestPaint()
        onPaint: {
            const ctx = getContext("2d")
            ctx.reset()
            ctx.scale(width / 480, height / 327)
            ctx.beginPath()
            ctx.roundedRect(0, 0, 480, 327, 12, 12)
            ctx.clip()
            ctx.fillStyle = grassColor
            ctx.fillRect(0, 0, 480, 327)
            ctx.strokeStyle = grassHighlight
            ctx.lineWidth = 0.6
            for (let i = 0; i < 2200; ++i) {
                const x = (i * 73.173) % 480
                const y = (i * 47.357) % 327
                ctx.beginPath()
                ctx.moveTo(x, y)
                ctx.lineTo(x + 0.8, y - 1.7)
                ctx.stroke()
            }
            for (const center of [150, 331]) {
                ctx.beginPath()
                ctx.arc(center, 164, 90.5, 0, 2 * Math.PI)
                ctx.strokeStyle = corridorColor
                ctx.globalAlpha = 0.8
                ctx.lineWidth = 32
                ctx.stroke()
                ctx.globalAlpha = 1
                ctx.strokeStyle = referenceColor
                ctx.lineWidth = 1.8
                ctx.stroke()
            }
            for (const x of [59.5, 421.5]) {
                ctx.beginPath()
                ctx.arc(x, 164, 11, 0, 2 * Math.PI)
                ctx.fillStyle = markerColor
                ctx.fill()
                ctx.lineWidth = 3
                ctx.strokeStyle = markerBorder
                ctx.stroke()
            }
        }
    }

    Repeater {
        model: [qsTr("A筒"), qsTr("B筒")]
        delegate: Rectangle {
            id: markerLabel
            required property int index
            required property string modelData
            x: root.width * (index === 0 ? 17 : 380) / 480
            y: root.height * 76 / 327
            width: root.width * 84 / 480
            height: root.height * 53 / 327
            radius: ScreenTools.defaultBorderRadius
            color: Qt.alpha(palette.mapWidgetBorderDark, 0.78)
            QGCLabel {
                anchors.centerIn: parent
                text: markerLabel.modelData
                color: palette.mapWidgetBorderLight
                font.pointSize: Math.max(ScreenTools.smallFontPointSize, root.height * 27 / 327
                    * ScreenTools.defaultFontPointSize / ScreenTools.defaultFontPixelHeight)
                font.bold: true
            }
        }
    }

    QGCLabel {
        anchors.horizontalCenter: parent.horizontalCenter
        y: root.height * 268 / 327
        text: qsTr("起飞位置")
        color: palette.mapWidgetBorderLight
        font.pointSize: Math.max(ScreenTools.smallFontPointSize, root.height * 27 / 327
            * ScreenTools.defaultFontPointSize / ScreenTools.defaultFontPixelHeight)
    }
    QGCLabel {
        anchors.right: parent.right
        anchors.top: parent.top
        anchors.margins: ScreenTools.defaultFontPixelWidth
        text: qsTr("场地示意")
        color: palette.mapWidgetBorderLight
        font.pointSize: ScreenTools.smallFontPointSize
        opacity: 0.8
    }
}
