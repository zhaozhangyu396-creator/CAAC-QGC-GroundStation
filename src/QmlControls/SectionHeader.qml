import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import QGroundControl
import QGroundControl.Controls

CheckBox {
    id:             control
    focusPolicy:    Qt.ClickFocus
    checked:        true
    leftPadding:    0

    property var            color:          qgcPal.text
    property bool           showSpacer:     true
    property ButtonGroup    buttonGroup:    null
    readonly property bool vehicleSetupStyle: ScreenTools.isVehicleSetupItem(control)

    property real _sectionSpacer: ScreenTools.defaultFontPixelWidth / 2  // spacing between section headings

    onButtonGroupChanged: {
        if (buttonGroup) {
            buttonGroup.addButton(control)
        }
    }

    QGCPalette { id: qgcPal; colorGroupEnabled: enabled }

    contentItem: ColumnLayout {
        Item {
            Layout.preferredHeight: control._sectionSpacer
            width:                  1
            visible:                control.showSpacer
        }

        QGCLabel {
            text:               control.text
            color:              control.color
            Layout.fillWidth:   true
            leftPadding:        control.vehicleSetupStyle ? ScreenTools.defaultFontPixelWidth : 0

            Rectangle {
                anchors.left: parent.left
                anchors.verticalCenter: parent.verticalCenter
                width: ScreenTools.defaultFontPixelWidth * 0.35
                height: parent.height
                color: qgcPal.colorGreen
                radius: width / 2
                visible: control.vehicleSetupStyle
            }

            QGCColoredImage {
                anchors.right:          parent.right
                anchors.verticalCenter: parent.verticalCenter
                width:                  parent.height / 2
                height:                 width
                source:                 "/qmlimages/arrow-down.png"
                color:                  qgcPal.text
                visible:                !control.checked
            }
        }

        Rectangle {
            Layout.fillWidth:   true
            height:             1
            color:              control.vehicleSetupStyle ? qgcPal.groupBorder : qgcPal.text
        }
    }

    indicator: Item {}
}
