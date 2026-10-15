import QtQuick
import QtQuick.Layouts

import QGroundControl
import QGroundControl.Controls

ColumnLayout {
    id:                 control
    spacing:            _margins / 2
    implicitWidth:      _contentLayout.implicitWidth + (_margins * 2)
    implicitHeight:     _contentLayout.implicitHeight + (_margins * 2)
    Layout.minimumWidth: settingsMenuStyle ? 0 : -1

    default property alias contentItem: _contentLayout.data

    property alias contentSpacing: _contentLayout.spacing

    property string defaultBorderColor  : QGroundControl.globalPalette.groupBorder
    property string outerBorderColor    : defaultBorderColor

    property string defaultHeadingPointSize:    ScreenTools.defaultFontPointSize + 1
    property string headingPointSize:           defaultHeadingPointSize

    property string heading
    property string headingDescription
    property bool   showTopDivider:     false
    property bool   showDividers:       true
    property bool   showBorder:         !settingsMenuStyle
    readonly property bool vehicleSetupStyle: ScreenTools.isVehicleSetupItem(control)
    readonly property bool settingsMenuStyle: ScreenTools.isSettingsMenuItem(control)

    property real _margins: ScreenTools.defaultFontPixelHeight / 2

    // We work with a y sorted list of children for divider visibility checks
    property var _ySortedChildren: {
        let arr = []
        for (let c of _contentLayout.children)
            arr.push(c)
        arr.sort((a, b) => a.y - b.y)
        return arr
    }

    Rectangle {
        objectName: "settingsGroupTopDivider"
        Layout.fillWidth: true
        Layout.bottomMargin: control._margins
        implicitHeight: 1
        color: QGroundControl.globalPalette.groupBorder
        visible: control.showTopDivider
    }

    ColumnLayout {
        Layout.leftMargin:  _margins
        Layout.fillWidth:   true
        spacing:            0
        visible:            heading !== ""

        RowLayout {
            Layout.fillWidth: true
            spacing: ScreenTools.defaultFontPixelWidth
            Rectangle {
                width: ScreenTools.defaultFontPixelWidth * 0.35
                height: ScreenTools.defaultFontPixelHeight
                color: QGroundControl.globalPalette.settingsAccent
                radius: width / 2
                visible: control.vehicleSetupStyle || control.settingsMenuStyle
            }
            QGCLabel {
                Layout.fillWidth: true
                text: control.heading
                font.pointSize: control.headingPointSize
                font.bold: true
                wrapMode: Text.WordWrap
            }
        }

        QGCLabel {
            Layout.fillWidth:   true
            text:               headingDescription
            wrapMode:           Text.WordWrap
            font.pointSize:     ScreenTools.smallFontPointSize
            visible:            headingDescription !== ""
        }
    }

    Rectangle {
        id:                 outerRect
        Layout.fillWidth:   true
        Layout.minimumWidth: control.settingsMenuStyle ? 0 : -1
        implicitWidth:      _contentLayout.implicitWidth + (control.settingsMenuStyle ? ScreenTools.defaultFontPixelWidth * 2 : showBorder ? _margins * 2 : 0)
        implicitHeight:     _contentLayout.implicitHeight + (control.settingsMenuStyle || showBorder ? _margins * 2 : 0)
        color:              control.settingsMenuStyle ? QGroundControl.globalPalette.window
                                : control.vehicleSetupStyle ? QGroundControl.globalPalette.windowShade : "transparent"
        border.color:       outerBorderColor
        border.width:       showBorder && !control.vehicleSetupStyle ? 1 : 0
        radius:             ScreenTools.defaultFontPixelHeight / 2

        Repeater {
            model: showDividers && !control.settingsMenuStyle ? _ySortedChildren.length : 0

            Rectangle {
                x:          showBorder ? _margins : 0
                y:          _contentItem ? (_contentItem.y + _contentItem.height + _margins + (showBorder ? _margins : 0)) : 0
                width:      parent.width - (showBorder ? _margins * 2 : 0)
                height:     1
                color:      QGroundControl.globalPalette.groupBorder
                visible:    _contentItem ? _isContentItemVisible() : false

                property var _contentItem: index < _ySortedChildren.length ? _ySortedChildren[index] : undefined

                function _isRepeater(item) {
                    return item && item.toString().startsWith("QQuickRepeater");
                }

                function _isContentItemVisible() {
                    if (!_contentItem || !_contentItem.visible || _isRepeater(_contentItem)) {
                        return false
                    }
                    // Any children after this one visually from top to bottom must be visible to show divider
                    for (let i = index + 1; i < _ySortedChildren.length; ++i) {
                        if (!_ySortedChildren[i] || _isRepeater(_ySortedChildren[i])) {
                            continue
                        }
                        if (_ySortedChildren[i].visible) {
                            return true
                        }
                    }
                    return false
                }
            }
        }

        Repeater {
            model: control.settingsMenuStyle ? _contentLayout.children : []

            Rectangle {
                required property var modelData
                objectName: "settingsFormRowFrame"
                x: 0
                y: _contentLayout.y + modelData.y - control._margins
                width: outerRect.width
                height: modelData.height + control._margins * 2
                visible: modelData.visible && modelData.height > 0
                color: "transparent"
                border.color: QGroundControl.globalPalette.groupBorder
                border.width: 1
                radius: ScreenTools.defaultBorderRadius
            }
        }

        ColumnLayout {
            id:                 _contentLayout
            x:                  control.settingsMenuStyle ? ScreenTools.defaultFontPixelWidth : showBorder ? _margins : 0
            y:                  control.settingsMenuStyle || showBorder ? _margins : 0
            width:              parent.width - x * 2
            spacing:            _margins * (control.settingsMenuStyle ? 3.2 : showDividers ? 2 : 1)
        }
    }
}
