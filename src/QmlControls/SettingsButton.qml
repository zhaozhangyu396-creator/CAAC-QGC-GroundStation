import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import QGroundControl
import QGroundControl.Controls

Button {
    id:             control
    padding:        ScreenTools.defaultFontPixelWidth * 0.75
    hoverEnabled:   !ScreenTools.isMobile
    autoExclusive:  true
    icon.color:     textColor

    readonly property bool vehicleSetupStyle: ScreenTools.isVehicleSetupItem(control)
    readonly property bool settingsMenuStyle: ScreenTools.isSettingsMenuItem(control)
    property color textColor: (vehicleSetupStyle || settingsMenuStyle) && (checked || pressed) ? qgcPal.text
        : checked || pressed ? qgcPal.buttonHighlightText : qgcPal.buttonText
    property bool expandable: false
    property bool expanded:   false

    signal toggleExpand()

    QGCPalette {
        id:                 qgcPal
        colorGroupEnabled:  control.enabled
    }

    background: Rectangle {
        color:      control.settingsMenuStyle ? qgcPal.windowShadeDark
                        : control.vehicleSetupStyle ? qgcPal.windowShade : qgcPal.buttonHighlight
        border.color: control.settingsMenuStyle && checked ? qgcPal.text : "transparent"
        border.width: control.settingsMenuStyle && checked ? 1 : 0
        opacity:    checked || pressed ? 1 : enabled && hovered ? .2 : 0
        radius:     ScreenTools.defaultFontPixelWidth / 2

        Rectangle {
            anchors.left: parent.left
            anchors.verticalCenter: parent.verticalCenter
            width: ScreenTools.defaultFontPixelWidth * 0.35
            height: parent.height * 0.6
            radius: width / 2
            color: qgcPal.window
            border.color: qgcPal.windowShadeLight
            border.width: 1
            visible: control.vehicleSetupStyle && (control.checked || control.pressed)
        }
    }

    contentItem: RowLayout {
        spacing: ScreenTools.defaultFontPixelWidth

        QGCColoredImage {
            source: control.icon.source
            color:  control.icon.color
            width:  ScreenTools.defaultFontPixelHeight
            height: ScreenTools.defaultFontPixelHeight
        }

        QGCLabel {
            id:                     displayText
            Layout.fillWidth:       true
            text:                   control.text
            font.pointSize:         control.vehicleSetupStyle ? ScreenTools.defaultFontPointSize : ScreenTools.menuFontPointSize
            wrapMode:               Text.WordWrap
            color:                  control.textColor
            horizontalAlignment:    QGCLabel.AlignLeft
        }

        QGCColoredImage {
            visible:    control.expandable
            source:     "/InstrumentValueIcons/cheveron-right.svg"
            color:      control.textColor
            width:      ScreenTools.defaultFontPixelHeight * 0.75
            height:     width
            rotation:   control.expanded ? 90 : 0

            MouseArea {
                anchors.fill: parent
                anchors.margins: -ScreenTools.defaultFontPixelWidth
                onClicked: control.toggleExpand()
            }
        }
    }
}
