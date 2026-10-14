import QtQuick
import QtQuick.Controls

import QGroundControl
import QGroundControl.Controls

GroupBox {
    id: control
    readonly property bool vehicleSetupStyle: ScreenTools.isVehicleSetupItem(control)

    QGCPalette { id: qgcPal; colorGroupEnabled: enabled }

    background: Rectangle {
        y:      control.topPadding - control.padding
        width:  parent.width
        height: parent.height - control.topPadding + control.padding
        color:  qgcPal.windowShade
        radius: control.vehicleSetupStyle ? ScreenTools.defaultBorderRadius : 0
    }

    label: QGCLabel {
        width:  control.availableWidth
        text:   control.title
        leftPadding: control.vehicleSetupStyle ? ScreenTools.defaultFontPixelWidth : 0

        Rectangle {
            anchors.left: parent.left
            anchors.verticalCenter: parent.verticalCenter
            width: ScreenTools.defaultFontPixelWidth * 0.35
            height: parent.height
            color: qgcPal.colorGreen
            radius: width / 2
            visible: control.vehicleSetupStyle
        }
    }
}
