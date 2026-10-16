import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import QGroundControl
import QGroundControl.Controls

Button {
    id:             control
    padding:        ScreenTools.defaultFontPixelWidth * 0.75
    leftPadding:    outlined ? ScreenTools.defaultFontPixelWidth : padding
    rightPadding:   leftPadding
    topPadding:     outlined ? ScreenTools.defaultFontPixelHeight * 0.5 : padding
    bottomPadding:  topPadding
    hoverEnabled:   !ScreenTools.isMobile
    autoExclusive:  true
    icon.color:     textColor

    readonly property bool vehicleSetupStyle: ScreenTools.isVehicleSetupItem(control)
    readonly property bool settingsMenuStyle: ScreenTools.isSettingsMenuItem(control)
    property color textColor: (vehicleSetupStyle || settingsMenuStyle) && (checked || pressed) ? qgcPal.text
        : checked || pressed ? qgcPal.buttonHighlightText : qgcPal.buttonText
    property bool expandable: false
    property bool expanded:   false
    property bool outlined:   false

    signal toggleExpand()

    QGCPalette {
        id:                 qgcPal
        colorGroupEnabled:  control.enabled
    }

    background: Rectangle {
        readonly property color fillColor: control.settingsMenuStyle ? qgcPal.windowShadeDark
                        : control.vehicleSetupStyle ? qgcPal.windowShade : qgcPal.buttonHighlight
        readonly property real highlightOpacity: control.checked || control.pressed
                        ? 1 : control.enabled && control.hovered ? 0.2 : 0
        color:      control.outlined
                        ? Qt.rgba(fillColor.r, fillColor.g, fillColor.b, fillColor.a * highlightOpacity) : fillColor
        border.color: control.settingsMenuStyle && control.checked ? qgcPal.text
                        : control.outlined ? qgcPal.groupBorder : "transparent"
        border.width: control.outlined || (control.settingsMenuStyle && control.checked) ? 1 : 0
        opacity:    control.outlined ? 1 : highlightOpacity
        radius:     control.outlined ? ScreenTools.defaultBorderRadius : ScreenTools.defaultFontPixelWidth / 2

        Rectangle {
            anchors.left: parent.left
            anchors.verticalCenter: parent.verticalCenter
            width: ScreenTools.defaultFontPixelWidth * 0.35
            height: parent.height * 0.6
            radius: width / 2
            color: qgcPal.settingsSectionAccent
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
            font.bold:              control.outlined && control.vehicleSetupStyle
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
