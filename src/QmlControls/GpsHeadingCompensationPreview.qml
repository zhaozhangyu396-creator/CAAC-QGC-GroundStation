pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Layouts

import QGroundControl
import QGroundControl.Controls

Rectangle {
    id: root
    objectName: "gpsHeadingCompensationPreview"
    required property var angle
    readonly property bool valid: angle !== null && Number.isFinite(Number(angle))
        && Number(angle) >= 0 && Number(angle) <= 360
    // PX4's body-frame antenna vector is [cos(offset), sin(offset), 0]; positive Y points right.
    readonly property real radians: valid ? Number(angle) * Math.PI / 180 : 0
    readonly property real directionX: Math.sin(radians)
    readonly property real directionY: -Math.cos(radians)
    readonly property real padding: ScreenTools.defaultFontPixelWidth
    implicitHeight: content.implicitHeight + padding * 2
    color: palette.window
    radius: ScreenTools.defaultBorderRadius

    QGCPalette { id: palette }
    ColumnLayout {
        id: content
        x: root.padding
        y: root.padding
        width: root.width - root.padding * 2
        spacing: ScreenTools.defaultFontPixelHeight * 0.3
        QGCLabel {
            Layout.fillWidth: true
            text: qsTr("安装方向关系 · 俯视图")
            font.bold: true
            wrapMode: Text.WordWrap
        }
        Item {
            id: plane
            Layout.fillWidth: true
            Layout.preferredHeight: ScreenTools.defaultFontPixelHeight * 13
            readonly property real reach: Math.max(0, Math.min(width / 2 - ScreenTools.defaultFontPixelHeight * 1.6,
                height / 2 - ScreenTools.defaultFontPixelHeight * 1.2))
            Canvas {
                id: drawing
                anchors.fill: parent
                onWidthChanged: requestPaint()
                onHeightChanged: requestPaint()

                function arrow(context, fromX, fromY, toX, toY, color) {
                    const direction = Math.atan2(toY - fromY, toX - fromX)
                    const head = ScreenTools.defaultFontPixelHeight * 0.5
                    context.strokeStyle = color
                    context.fillStyle = color
                    context.beginPath()
                    context.moveTo(fromX, fromY)
                    context.lineTo(toX, toY)
                    context.stroke()
                    context.beginPath()
                    context.moveTo(toX, toY)
                    context.lineTo(toX - head * Math.cos(direction - Math.PI / 6),
                        toY - head * Math.sin(direction - Math.PI / 6))
                    context.lineTo(toX - head * Math.cos(direction + Math.PI / 6),
                        toY - head * Math.sin(direction + Math.PI / 6))
                    context.closePath()
                    context.fill()
                }

                onPaint: {
                    const context = getContext("2d")
                    context.reset()
                    context.translate(width / 2, height / 2)
                    const fontSize = ScreenTools.defaultFontPixelHeight
                    const arm = plane.reach * 0.65
                    context.strokeStyle = palette.text
                    context.lineWidth = fontSize * 0.25
                    context.globalAlpha = 0.25
                    context.beginPath()
                    context.moveTo(-arm, -arm)
                    context.lineTo(arm, arm)
                    context.moveTo(arm, -arm)
                    context.lineTo(-arm, arm)
                    context.stroke()
                    for (const x of [-arm, arm]) {
                        for (const y of [-arm, arm]) {
                            context.beginPath()
                            context.arc(x, y, fontSize * 0.5, 0, Math.PI * 2)
                            context.stroke()
                        }
                    }
                    context.fillStyle = palette.text
                    context.beginPath()
                    context.ellipse(-fontSize * 0.6, -fontSize, fontSize * 1.2, fontSize * 2)
                    context.fill()
                    context.globalAlpha = 1
                    context.lineWidth = Math.max(1, fontSize * 0.1)
                    arrow(context, 0, 0, 0, -plane.reach, palette.text)
                    if (!root.valid) { return }
                    arrow(context, root.directionX * plane.reach * 0.7, root.directionY * plane.reach * 0.7,
                        -root.directionX * plane.reach * 0.92, -root.directionY * plane.reach * 0.92, palette.colorGreen)
                    if (Number(root.angle) > 0) {
                        const radius = plane.reach * 0.42
                        context.strokeStyle = palette.colorOrange
                        context.beginPath()
                        context.arc(0, 0, radius, -Math.PI / 2, root.radians - Math.PI / 2)
                        context.stroke()
                        const endX = root.directionX * radius
                        const endY = root.directionY * radius
                        arrow(context, endX - Math.cos(root.radians) * fontSize * 0.25,
                            endY - Math.sin(root.radians) * fontSize * 0.25, endX, endY, palette.colorOrange)
                    }
                }
            }
            QGCLabel {
                anchors.top: parent.top
                anchors.horizontalCenter: parent.horizontalCenter
                text: qsTr("机头方向")
                font.bold: true
            }
            Antenna {
                objectName: "gpsHeadingAntennaA"
                caption: qsTr("主")
                x: plane.width / 2 + root.directionX * plane.reach * 0.7 - width / 2
                y: plane.height / 2 + root.directionY * plane.reach * 0.7 - height / 2
            }
            Antenna {
                objectName: "gpsHeadingAntennaB"
                caption: qsTr("次")
                x: plane.width / 2 - root.directionX * plane.reach * 0.7 - width / 2
                y: plane.height / 2 - root.directionY * plane.reach * 0.7 - height / 2
            }
        }
        QGCLabel {
            Layout.fillWidth: true
            color: palette.colorGreen
            horizontalAlignment: Text.AlignHCenter
            wrapMode: Text.WordWrap
            text: qsTr("主：主 GPS 天线　次：次 GPS 天线\n主 → 次：天线指向")
        }
        QGCLabel {
            objectName: "gpsHeadingCompensationAngle"
            Layout.fillWidth: true
            color: palette.colorOrange
            horizontalAlignment: Text.AlignHCenter
            wrapMode: Text.WordWrap
            text: root.valid ? qsTr("安装补偿角：%1°").arg(Number(root.angle).toFixed(1))
                : qsTr("请输入 0～360° 的安装补偿角")
        }
        QGCLabel {
            Layout.fillWidth: true
            font.pointSize: ScreenTools.smallFontPointSize
            wrapMode: Text.WordWrap
            text: qsTr("绿色箭头由主 GPS 天线指向次 GPS 天线。橙色补偿角按飞控定义，从机头顺时针量至“次 → 主”方向。位置及间距仅作示意；随输入预览，确认后保存。")
        }
    }
    component Antenna: Rectangle {
        id: antenna
        required property string caption
        visible: root.valid
        width: ScreenTools.defaultFontPixelHeight * 1.3
        height: width
        radius: width / 2
        color: palette.colorGreen
        border.color: palette.text
        QGCLabel {
            anchors.centerIn: parent
            text: antenna.caption
            color: palette.window
            font.bold: true
        }
    }
    Connections {
        target: root
        function onAngleChanged() { drawing.requestPaint() }
    }
    Connections {
        target: palette
        function onPaletteChanged() { drawing.requestPaint() }
    }
}
