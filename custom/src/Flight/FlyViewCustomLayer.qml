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
    property var flightToolStrip
    property var mapControl
    property var customOccluders: tools.visible ? [Qt.rect(tools.x, tools.y, tools.width, tools.height)] : []

    readonly property bool _simulationEmergency: _corePlugin.simulation.emergencyMode
    property Component _emergencyDialog: Component { CaacSimulationEmergencyDialog { } }
    readonly property var _vehicle: QGroundControl.multiVehicleManager.activeVehicle
    readonly property var _corePlugin: QGroundControl.corePlugin
    readonly property var _missionController: mapControl && mapControl.planMasterController
        ? mapControl.planMasterController.missionController : null
    readonly property real _margin: ScreenTools.defaultFontPixelWidth
    readonly property bool _canCenter: _vehicle && _vehicle.coordinate.isValid && mapControl
    property point _panelOffset: Qt.point(0, 0)

    function movePanelGroup(delta) {
        const x = Math.max(root._margin, Math.min(tools.x + delta.x, root.width - tools.width - root._margin))
        const bottom = root.occluders ? Math.min(root.occluders.bottomRight.y, root.occluders.virtualJoystickRight.y) : root.height
        const y = Math.max(root._margin, Math.min(tools.y + delta.y, bottom - toolbar.height - root._margin * 2))
        _panelOffset = Qt.point(_panelOffset.x + x - tools.x, _panelOffset.y + y - tools.y)
    }

    QGCPalette { id: caacPalette; colorGroupEnabled: true }
    function selectPanel(button) {
        if (button.checked) {
            for (const panelButton of [missionButton, dataButton, mapButton]) {
                if (panelButton !== button) { panelButton.checked = false }
            }
        }
    }

    component RoundTool: QGCButton {
        id: button
        property string toolIcon: "data"
        Layout.minimumWidth: Layout.preferredWidth
        Layout.maximumWidth: Layout.preferredWidth
        Layout.preferredWidth: root.flightToolStrip ? root.flightToolStrip.roundButtonDiameter : ScreenTools.defaultFontPixelHeight * 3
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
                if (button.toolIcon === "center") {
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
                } else if (button.toolIcon === "mission") {
                    ctx.beginPath()
                    ctx.moveTo(width / 2, height / 6)
                    ctx.lineTo(width * 4 / 5, height * 5 / 6)
                    ctx.lineTo(width / 2, height * 2 / 3)
                    ctx.lineTo(width / 5, height * 5 / 6)
                    ctx.closePath()
                    ctx.stroke()
                } else if (button.toolIcon === "map") {
                    ctx.beginPath()
                    ctx.moveTo(width / 6, height / 4)
                    ctx.lineTo(width * 2 / 5, height / 6)
                    ctx.lineTo(width * 3 / 5, height / 4)
                    ctx.lineTo(width * 5 / 6, height / 6)
                    ctx.lineTo(width * 5 / 6, height * 3 / 4)
                    ctx.lineTo(width * 3 / 5, height * 5 / 6)
                    ctx.lineTo(width * 2 / 5, height * 3 / 4)
                    ctx.lineTo(width / 6, height * 5 / 6)
                    ctx.closePath()
                    ctx.moveTo(width * 2 / 5, height / 6)
                    ctx.lineTo(width * 2 / 5, height * 3 / 4)
                    ctx.moveTo(width * 3 / 5, height / 4)
                    ctx.lineTo(width * 3 / 5, height * 5 / 6)
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

    CaacWaypointOverlap {
        visible: !root._simulationEmergency
        anchors.fill: parent
        z: QGroundControl.zOrderMapItems + 1
        mapControl: root.mapControl
        missionController: root._missionController
        blockedAreas: root.occluders ? root.occluders.all.concat(root.customOccluders) : root.customOccluders
    }

    CaacRouteLegLabels {
        visible: !root._simulationEmergency
        anchors.fill: parent
        mapControl: root.mapControl
        blockedAreas: root.occluders ? root.occluders.all.concat(root.customOccluders) : root.customOccluders
        warningTopInset: root.occluders ? root.occluders.toolStrip.y + root.occluders.toolStrip.height + root._margin : 0
        segments: root._missionController ? root._missionController.simpleFlightPathSegments : null
    }

    CaacAttitudeInstrument {
        objectName: "caacEmergencyAttitude"
        readonly property real sideInset: root.occluders
            ? root.occluders.toolStrip.x + root.occluders.toolStrip.width + root._margin : root._margin
        readonly property real availableWidth: Math.max(0, root.width - sideInset - root._margin)
        x: sideInset + (availableWidth - width) / 2
        y: (root.height - height) / 2
        width: Math.min(availableWidth, root.height * 0.7)
        height: width
        vehicle: root._vehicle
        visible: root._simulationEmergency
    }

    QGCButton {
        objectName: "caacSimulationManagement"
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        anchors.margins: root._margin
        visible: root._simulationEmergency
        text: qsTr("Simulation controls")
        onClicked: {
            const dialog = root._emergencyDialog.createObject(mainWindow)
            if (dialog) { dialog.open() }
        }
    }

    ColumnLayout {
        id: tools
        readonly property real baseY: root.occluders
            ? root.occluders.toolStrip.y + ScreenTools.defaultFontPixelWidth * 0.7 : root._margin
        x: Math.max(root._margin, Math.min(root.width - width + root._panelOffset.x, root.width - width))
        y: Math.max(0, Math.min(baseY + root._panelOffset.y,
            (root.occluders ? Math.min(root.occluders.bottomRight.y, root.occluders.virtualJoystickRight.y) : root.height)
                - toolbar.height - root._margin * 2))
        width: root.flightToolStrip ? root.flightToolStrip.width : Math.min(ScreenTools.defaultFontPixelWidth * 46, parent.width / 2)
        readonly property real availableHeight: (root.occluders ? Math.min(root.occluders.bottomRight.y, root.occluders.virtualJoystickRight.y) : root.height) - y - root._margin
        visible: !root._simulationEmergency && availableHeight > toolbar.height
        spacing: root._margin

        RowLayout {
            id: toolbar
            Layout.fillWidth: true
            Layout.leftMargin: ScreenTools.defaultFontPixelWidth * 0.7
            Layout.rightMargin: ScreenTools.defaultFontPixelWidth * 0.7
            RoundTool {
                id: missionButton
                objectName: "caacMissionDetailsButton"
                text: qsTr("Mission details")
                toolIcon: "mission"
                checkable: true
                onCheckedChanged: root.selectPanel(this)
            }
            Item { Layout.fillWidth: true }
            RoundTool {
                id: dataButton
                objectName: "caacFlightDataButton"
                text: qsTr("Flight data")
                checkable: true
                onCheckedChanged: root.selectPanel(this)
            }
            Item { Layout.fillWidth: true }
            RoundTool {
                id: mapButton
                objectName: "caacMapSettingsButton"
                text: qsTr("Map settings")
                toolIcon: "map"
                checkable: true
                onCheckedChanged: root.selectPanel(this)
            }
            Item { Layout.fillWidth: true }
            RoundTool {
                objectName: "caacCenterVehicleButton"
                text: qsTr("Center aircraft")
                toolIcon: "center"
                enabled: root._canCenter
                onClicked: {
                    if (root._canCenter) {
                        root.mapControl.center = root._vehicle.coordinate
                    }
                }
            }
        }

        QGCFlickable {
            objectName: "caacFlyPanelsScroller"
            Layout.fillWidth: true
            Layout.preferredHeight: Math.max(0, Math.min(contentHeight, tools.availableHeight - toolbar.height - root._margin))
            visible: missionButton.checked || dataButton.checked || mapButton.checked
            contentHeight: panels.implicitHeight
            contentWidth: width
            clip: true
            Column {
                id: panels
                width: parent.width
                spacing: root._margin
                CaacMissionDetails {
                    objectName: "caacMissionDetailsPanel"
                    width: parent.width
                    height: implicitHeight
                    visible: missionButton.checked
                    vehicle: root._vehicle
                    missionController: root._missionController
                    onTitleDragged: (delta) => root.movePanelGroup(delta)
                }
                CaacFlyDataPanel {
                    objectName: "caacFlyDataPanel"
                    width: parent.width
                    height: Math.min(implicitHeight, Math.max(ScreenTools.defaultFontPixelHeight * 10,
                        tools.availableHeight - toolbar.height - root._margin))
                    visible: dataButton.checked
                    vehicle: root._vehicle
                    distance: root._corePlugin.flightDistance
                    onTitleDragged: (delta) => root.movePanelGroup(delta)
                }
                CaacMapSettings {
                    objectName: "caacMapSettingsPanel"
                    width: parent.width
                    height: implicitHeight
                    visible: mapButton.checked
                    onTitleDragged: (delta) => root.movePanelGroup(delta)
                }
            }
        }
    }
}
