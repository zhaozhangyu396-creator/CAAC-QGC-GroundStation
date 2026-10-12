pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Layouts

import QGroundControl
import QGroundControl.Controls

ColumnLayout {
    id: root
    required property string title
    default property alias content: fields.data
    Layout.fillWidth: true
    spacing: 0
    data: [Rectangle {
        Layout.fillWidth: true
        implicitHeight: heading.implicitHeight + ScreenTools.defaultFontPixelHeight
        color: QGroundControl.globalPalette.windowShadeDark
        QGCLabel {
            id: heading
            anchors.verticalCenter: parent.verticalCenter
            x: ScreenTools.defaultFontPixelWidth
            width: parent.width - x * 2
            text: root.title
            font.bold: true
            font.pointSize: ScreenTools.menuFontPointSize
            wrapMode: Text.WordWrap
        }
        Rectangle {
            anchors.bottom: parent.bottom
            width: parent.width
            height: ScreenTools.defaultFontPixelHeight * 0.1
            color: QGroundControl.globalPalette.buttonHighlight
        }
    }, Rectangle {
        Layout.fillWidth: true
        implicitHeight: fields.implicitHeight + ScreenTools.defaultFontPixelHeight
        color: QGroundControl.globalPalette.windowShade
        border.color: QGroundControl.globalPalette.windowShadeDark
        border.width: ScreenTools.defaultFontPixelWidth * 0.15
        ColumnLayout {
            id: fields
            x: ScreenTools.defaultFontPixelWidth
            y: ScreenTools.defaultFontPixelHeight * 0.5
            width: parent.width - x * 2
            spacing: ScreenTools.defaultFontPixelHeight * 0.6
        }
    }
    ]
}
