import QtQuick
import QtQuick.Layouts

import QGroundControl
import QGroundControl.Controls

Rectangle {
    id: control

    default property alias content: contents.data
    property alias spacing: contents.spacing
    property real verticalPadding: ScreenTools.defaultFontPixelHeight * 0.5

    Layout.fillWidth: true
    Layout.minimumWidth: 0
    implicitWidth: contents.implicitWidth + ScreenTools.defaultFontPixelWidth * 2
    implicitHeight: contents.implicitHeight + verticalPadding * 2
    color: "transparent"
    border.color: QGroundControl.globalPalette.groupBorder
    border.width: 1
    radius: ScreenTools.defaultBorderRadius

    data: RowLayout {
        id: contents
        x: ScreenTools.defaultFontPixelWidth
        y: control.verticalPadding
        width: control.width - x * 2
        spacing: ScreenTools.defaultFontPixelWidth
    }
}
