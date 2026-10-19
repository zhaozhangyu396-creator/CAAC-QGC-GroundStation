pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Layouts

import QGroundControl
import QGroundControl.Controls

Rectangle {
    id: root
    objectName: "gpsAntennaPositionPreview"
    required property int receiver
    required property var positionX
    required property var positionY
    required property var positionZ
    readonly property real extent: Math.ceil(Math.max(40, Math.abs(positionX || 0),
        Math.abs(positionY || 0), Math.abs(positionZ || 0)) * 1.25 / 50) * 50
    implicitHeight: content.implicitHeight + padding * 2
    property real padding: ScreenTools.defaultFontPixelWidth
    color: palette.windowShade
    border.color: palette.text
    radius: ScreenTools.defaultBorderRadius

    function coordinate(value) {
        return value === null ? "—" : Number(value.toFixed(2)).toString()
    }

    QGCPalette { id: palette }
    ColumnLayout {
        id: content
        x: root.padding
        y: root.padding
        width: root.width - root.padding * 2
        spacing: ScreenTools.defaultFontPixelHeight * 0.4
        QGCLabel {
            Layout.fillWidth: true
            font.bold: true
            wrapMode: Text.WordWrap
            text: qsTr("GPS %1 安装位置实时预览").arg(root.receiver)
        }
        GridLayout {
            Layout.fillWidth: true
            columns: width >= ScreenTools.defaultFontPixelHeight * 10 ? 2 : 1
            columnSpacing: ScreenTools.defaultFontPixelWidth
            rowSpacing: ScreenTools.defaultFontPixelHeight * 0.4
            Projection {
                objectName: "gpsTopProjection"
                caption: qsTr("俯视图")
                horizontalPosition: root.positionY
                verticalPosition: root.positionX === null ? null : -root.positionX
                horizontalAxis: qsTr("Y 右 →")
                verticalAxis: qsTr("↑ X 前")
                markerName: "gpsTopMarker"
            }
            Projection {
                objectName: "gpsSideProjection"
                caption: qsTr("侧视图")
                horizontalPosition: root.positionX
                verticalPosition: root.positionZ
                horizontalAxis: qsTr("X 前 →")
                verticalAxis: qsTr("Z 下 ↓")
                markerName: "gpsSideMarker"
            }
        }
        QGCLabel {
            Layout.fillWidth: true
            wrapMode: Text.WordWrap
            text: qsTr("X：%1　Y：%2　Z：%3 cm")
                .arg(root.coordinate(root.positionX)).arg(root.coordinate(root.positionY)).arg(root.coordinate(root.positionZ))
        }
        QGCLabel {
            Layout.fillWidth: true
            font.pointSize: ScreenTools.smallFontPointSize
            wrapMode: Text.WordWrap
            text: qsTr("十字：机体原点；圆点：GPS。范围 ±%1 cm，自动缩放。随输入预览，确认后保存。")
                .arg(root.extent)
        }
    }

    component Projection: ColumnLayout {
        id: projection
        required property string caption
        required property var horizontalPosition
        required property var verticalPosition
        required property string horizontalAxis
        required property string verticalAxis
        required property string markerName
        Layout.fillWidth: true
        Layout.minimumWidth: 0
        spacing: ScreenTools.defaultFontPixelHeight * 0.2
        QGCLabel { text: projection.caption; Layout.alignment: Qt.AlignHCenter }
        Rectangle {
            id: plane
            Layout.fillWidth: true
            Layout.preferredHeight: ScreenTools.defaultFontPixelHeight * 6
            color: palette.window
            clip: true
            readonly property real reach: Math.max(0, Math.min(width, height) / 2 - ScreenTools.defaultFontPixelHeight)
            Rectangle { anchors.centerIn: parent; width: parent.width; height: 1; color: palette.text; opacity: 0.4 }
            Rectangle { anchors.centerIn: parent; width: 1; height: parent.height; color: palette.text; opacity: 0.4 }
            Rectangle {
                anchors.centerIn: parent
                width: ScreenTools.defaultFontPixelHeight * 1.2
                height: width
                color: palette.windowShade
                border.color: palette.text
                opacity: 0.65
            }
            QGCLabel {
                anchors.top: parent.top
                anchors.horizontalCenter: parent.horizontalCenter
                text: projection.verticalAxis
                font.pointSize: ScreenTools.smallFontPointSize
            }
            QGCLabel {
                anchors.bottom: parent.bottom
                anchors.right: parent.right
                text: projection.horizontalAxis
                font.pointSize: ScreenTools.smallFontPointSize
            }
            Rectangle {
                objectName: projection.markerName
                width: ScreenTools.defaultFontPixelHeight * 0.7
                height: width
                radius: width / 2
                color: palette.colorGreen
                border.color: palette.text
                visible: projection.horizontalPosition !== null && projection.verticalPosition !== null
                x: plane.width / 2 + Number(projection.horizontalPosition) / root.extent * plane.reach - width / 2
                y: plane.height / 2 + Number(projection.verticalPosition) / root.extent * plane.reach - height / 2
            }
        }
    }
}
