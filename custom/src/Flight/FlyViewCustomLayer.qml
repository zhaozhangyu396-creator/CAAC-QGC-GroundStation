import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import QGroundControl
import QGroundControl.Controls
import CAAC.Plan

Item {
    id: root
    objectName: "caacFlyLayer"

    property var occluders
    property var mapControl
    property var customOccluders: tools.visible ? [Qt.rect(tools.x, tools.y, tools.width, tools.height)] : []

    readonly property var _vehicle: QGroundControl.multiVehicleManager.activeVehicle
    readonly property var _corePlugin: QGroundControl.corePlugin
    readonly property real _margin: ScreenTools.defaultFontPixelWidth
    readonly property bool _canCenter: _vehicle && _vehicle.coordinate.isValid && mapControl

    QGCPalette { id: caacPalette; colorGroupEnabled: true }

    component RoundTool: QGCButton {
        id: button
        property bool centerIcon: false
        Layout.preferredWidth: ScreenTools.defaultFontPixelHeight * 3
        Layout.preferredHeight: Layout.preferredWidth
        Accessible.name: text
        ToolTip.visible: hovered
        ToolTip.text: text
        background: Rectangle {
            radius: width / 2
            color: button.checked ? caacPalette.colorYellowGreen : (caacPalette.globalTheme === QGCPalette.Dark ? caacPalette.text : caacPalette.button)
            opacity: button.enabled ? 1 : 0.5
        }
        contentItem: Canvas {
            property color iconColor: caacPalette.globalTheme === QGCPalette.Dark ? caacPalette.windowShadeDark : caacPalette.text
            onIconColorChanged: requestPaint()
            onPaint: {
                const ctx = getContext("2d")
                ctx.reset()
                ctx.strokeStyle = iconColor
                ctx.fillStyle = iconColor
                ctx.lineWidth = ScreenTools.defaultFontPixelWidth / 3
                if (button.centerIcon) {
                    ctx.beginPath()
                    ctx.arc(width / 2, height / 2, width / 5, 0, 2 * Math.PI)
                    ctx.moveTo(width / 2, height / 8)
                    ctx.lineTo(width / 2, height * 3 / 8)
                    ctx.moveTo(width / 2, height * 5 / 8)
                    ctx.lineTo(width / 2, height * 7 / 8)
                    ctx.moveTo(width / 8, height / 2)
                    ctx.lineTo(width * 3 / 8, height / 2)
                    ctx.moveTo(width * 5 / 8, height / 2)
                    ctx.lineTo(width * 7 / 8, height / 2)
                    ctx.stroke()
                } else {
                    ctx.strokeRect(width / 5, height / 5, width * 3 / 5, height * 3 / 5)
                    for (let i = 0; i < 3; ++i) {
                        ctx.fillRect(width * (0.3 + i * 0.16), height * (0.55 - i * 0.1), width * 0.08, height * (0.15 + i * 0.1))
                    }
                }
            }
        }
    }

    ColumnLayout {
        id: tools
        anchors.right: parent.right
        anchors.rightMargin: ScreenTools.defaultFontPixelHeight * 3 + root._margin
        y: root.occluders ? root.occluders.topRight.y + root.occluders.topRight.height + root._margin : root._margin
        width: Math.min(ScreenTools.defaultFontPixelWidth * 46, parent.width / 3)
        readonly property real availableHeight: (root.occluders ? Math.min(root.occluders.bottomRight.y, root.occluders.virtualJoystickRight.y) : root.height) - y - root._margin
        visible: availableHeight > toolbar.height
        spacing: root._margin

        RowLayout {
            id: toolbar
            Layout.fillWidth: true
            RoundTool {
                id: dataButton
                objectName: "caacFlightDataButton"
                text: qsTr("Flight data")
                checkable: true
            }
            Item { Layout.fillWidth: true }
            RoundTool {
                objectName: "caacCenterVehicleButton"
                text: qsTr("Center aircraft")
                centerIcon: true
                enabled: root._canCenter
                onClicked: {
                    if (root._canCenter) {
                        root.mapControl.center = root._vehicle.coordinate
                    }
                }
            }
        }

        CaacFlyDataPanel {
            objectName: "caacFlyDataPanel"
            Layout.fillWidth: true
            Layout.preferredHeight: Math.min(implicitHeight, tools.availableHeight - toolbar.height - root._margin)
            visible: dataButton.checked
            vehicle: root._vehicle
            distance: root._corePlugin.flightDistance
        }
    }
}
