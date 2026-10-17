import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import QGroundControl
import QGroundControl.Controls

AbstractButton   {
    id:         control
    checkable:  true
    padding:    0

    readonly property bool settingsMenuStyle: ScreenTools.isSettingsMenuItem(control)
    readonly property bool vehicleSetupStyle: ScreenTools.isVehicleSetupItem(control)
    property bool _showBorder:      settingsMenuStyle || qgcPal.globalTheme === QGCPalette.Light
    property int  _sliderInset:     2
    property bool _showHighlight:   enabled && (pressed || checked)

    QGCPalette { id: qgcPal; colorGroupEnabled: control.enabled }

    contentItem: Item {
        implicitWidth:  (label.visible ? label.implicitWidth + ScreenTools.defaultFontPixelWidth : 0) + indicator.width
        implicitHeight: Math.max(label.contentHeight, indicator.height)

        QGCLabel {
            id:             label
            anchors.left:   parent.left
            width: control.settingsMenuStyle || control.vehicleSetupStyle
                ? Math.max(0, parent.width - indicator.width - ScreenTools.defaultFontPixelWidth) : implicitWidth
            wrapMode: control.settingsMenuStyle || control.vehicleSetupStyle ? Text.WordWrap : Text.NoWrap
            text:           visible ? control.text : "X"
            visible:        control.text !== ""
        }

        Rectangle {
            id:                     indicator
            anchors.right:          parent.right
            anchors.verticalCenter: parent.verticalCenter
            height:                 ScreenTools.defaultFontPixelHeight * (control.settingsMenuStyle ? 1.25 : 1)
            width:                  height * 2.6
            radius:                 height / 2
            color:                  control.settingsMenuStyle ? (checked ? qgcPal.window : qgcPal.windowShade)
                                        : checked ? qgcPal.buttonHighlight : qgcPal.button
            border.width:           _showBorder ? 1 : 0
            border.color:           qgcPal.buttonBorder

            Rectangle {
                anchors.fill:   parent
                color:          control.settingsMenuStyle ? qgcPal.window : qgcPal.buttonHighlight
                opacity:        _showHighlight ? 1 : control.enabled && control.hovered ? .2 : 0
                radius:         parent.radius
            }

            Rectangle {
                id: thumb
                anchors.verticalCenter: parent.verticalCenter
                x:                      checked ? indicator.width - width - _sliderInset : _sliderInset
                height:                 parent.height - (_sliderInset * 2)
                width:                  height
                radius:                 height / 2
                color:                  qgcPal.buttonText
            }
            QGCLabel {
                objectName: control.objectName + "_state"
                anchors.verticalCenter: parent.verticalCenter
                x: control.checked ? control._sliderInset : thumb.width + control._sliderInset * 2
                width: indicator.width - thumb.width - control._sliderInset * 3
                text: control.checked ? qsTr("ON") : qsTr("OFF")
                color: control.checked && !control.settingsMenuStyle ? qgcPal.buttonHighlightText : qgcPal.buttonText
                font.pointSize: ScreenTools.smallFontPointSize
                font.bold: true
                horizontalAlignment: Text.AlignHCenter
            }
        }
    }
}
