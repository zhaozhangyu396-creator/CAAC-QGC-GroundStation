import QtQuick
import QtQuick.Controls

import QGroundControl
import QGroundControl.Controls

GroupBox {
    id: control
    readonly property bool vehicleSetupStyle: ScreenTools.isVehicleSetupItem(control)
    readonly property bool settingsMenuStyle: ScreenTools.isSettingsMenuItem(control)

    QGCPalette { id: qgcPal; colorGroupEnabled: enabled }

    background: Rectangle {
        y:      control.topPadding - control.padding
        width:  parent.width
        height: parent.height - control.topPadding + control.padding
        color:  control.settingsMenuStyle ? qgcPal.window : qgcPal.windowShade
        radius: control.vehicleSetupStyle ? ScreenTools.defaultBorderRadius : 0
    }

    label: QGCLabel {
        width:  control.availableWidth
        text:   control.title
        leftPadding: control.vehicleSetupStyle || control.settingsMenuStyle ? ScreenTools.defaultFontPixelWidth : 0
        font.bold: control.settingsMenuStyle

        Rectangle {
            anchors.left: parent.left
            anchors.verticalCenter: parent.verticalCenter
            width: ScreenTools.defaultFontPixelWidth * 0.35
            height: parent.height
            color: qgcPal.settingsSectionAccent
            radius: width / 2
            visible: control.vehicleSetupStyle || control.settingsMenuStyle
        }
    }
}
