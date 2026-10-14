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
    property bool planningOverlay: false
    property bool planningPaneExpanded: false
    property real planningNavigationHeight: 0
    property real reservedBottomHeight: 0
    property var missionControllerOverride: null
    property var customOccluders: tools.visible ? [Qt.rect(tools.x, tools.y, tools.width, tools.height)] : []

    readonly property bool _simulationEmergency: _corePlugin.simulation.emergencyMode
    property Component _emergencyDialog: Component { CaacSimulationEmergencyDialog { } }
    readonly property var _vehicle: QGroundControl.multiVehicleManager.activeVehicle
    readonly property var _corePlugin: QGroundControl.corePlugin
    readonly property var _missionController: missionControllerOverride || (mapControl && mapControl.planMasterController
        ? mapControl.planMasterController.missionController : null)
    readonly property real _margin: ScreenTools.defaultFontPixelWidth
    property point _panelOffset: Qt.point(0, 0)
    readonly property real _panelBottom: !root.planningOverlay && root.occluders
        ? Math.min(root.occluders.bottomRight.y, root.occluders.virtualJoystickRight.y)
        : root.height - Math.max(root.reservedBottomHeight,
            root.planningOverlay ? ScreenTools.defaultFontPixelHeight * 5 * 1.25 + root._margin * 2 : 0)

    function movePanelGroup(delta) {
        const x = Math.max(root._margin, Math.min(tools.x + delta.x, root.width - tools.width - root._margin))
        const bottom = root._panelBottom
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

    component SquareTool: QGCButton {
        id: button
        property string toolIcon: "data"
        Layout.minimumWidth: Layout.preferredWidth
        Layout.maximumWidth: Layout.preferredWidth
        Layout.preferredWidth: ScreenTools.defaultFontPixelHeight * 2.5
        Layout.preferredHeight: Layout.preferredWidth + ScreenTools.defaultFontPixelHeight
        Accessible.name: text
        ToolTip.visible: hovered
        ToolTip.text: text
        background: Rectangle {
            anchors.top: parent.top
            width: button.width
            height: width
            radius: ScreenTools.defaultFontPixelWidth * 0.7
            color: button.checked ? "#245b46" : "#d9232629"
            border.width: 1
            border.color: button.checked ? "#40d58b" : "#a0a5aa"
        }
        leftPadding: 0
        rightPadding: 0
        topPadding: 0
        bottomPadding: 0
        contentItem: Item {
          Canvas {
            anchors.top: parent.top
            width: button.width
            height: width
            property color iconColor: "white"
            onIconColorChanged: requestPaint()
            onPaint: {
                const ctx = getContext("2d")
                ctx.reset()
                ctx.strokeStyle = iconColor
                ctx.fillStyle = iconColor
                ctx.lineWidth = ScreenTools.defaultFontPixelWidth / 3
                if (button.toolIcon === "mission") {
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
                } else if (button.toolIcon === "clearTrajectory") {
                    ctx.beginPath()
                    ctx.moveTo(width * 0.58, height * 0.2)
                    ctx.lineTo(width * 0.79, height * 0.4)
                    ctx.lineTo(width * 0.51, height * 0.69)
                    ctx.lineTo(width * 0.3, height * 0.49)
                    ctx.closePath()
                    ctx.fill()
                    ctx.strokeStyle = "#232629"
                    ctx.beginPath()
                    ctx.moveTo(width * 0.42, height * 0.38)
                    ctx.lineTo(width * 0.64, height * 0.58)
                    ctx.stroke()
                    ctx.strokeStyle = iconColor
                    ctx.beginPath()
                    ctx.moveTo(width * 0.24, height * 0.78)
                    ctx.lineTo(width * 0.8, height * 0.78)
                    ctx.stroke()
                } else if (button.toolIcon === "calculator") {
                    ctx.fillRect(width * 0.24, height * 0.2, width * 0.52, height * 0.6)
                    ctx.strokeStyle = "#232629"
                    ctx.lineWidth = width * 0.045
                    ctx.beginPath()
                    ctx.moveTo(width * 0.3, height * 0.35); ctx.lineTo(width * 0.44, height * 0.35)
                    ctx.moveTo(width * 0.54, height * 0.29); ctx.lineTo(width * 0.67, height * 0.42)
                    ctx.moveTo(width * 0.67, height * 0.29); ctx.lineTo(width * 0.54, height * 0.42)
                    ctx.moveTo(width * 0.3, height * 0.6); ctx.lineTo(width * 0.44, height * 0.6)
                    ctx.moveTo(width * 0.37, height * 0.53); ctx.lineTo(width * 0.37, height * 0.67)
                    ctx.moveTo(width * 0.54, height * 0.56); ctx.lineTo(width * 0.68, height * 0.56)
                    ctx.moveTo(width * 0.54, height * 0.65); ctx.lineTo(width * 0.68, height * 0.65)
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
        QGCLabel {
            anchors.bottom: parent.bottom
            width: parent.width
            text: button.text
            font.pointSize: ScreenTools.smallFontPointSize
            horizontalAlignment: Text.AlignHCenter
            color: "white"
            style: Text.Outline
            styleColor: "#222222"
            fontSizeMode: Text.Fit
            minimumPointSize: ScreenTools.smallFontPointSize * 0.7
        }
    }

    CaacWaypointOverlap {
        visible: !root._simulationEmergency && !root.planningOverlay
        anchors.fill: parent
        z: QGroundControl.zOrderMapItems + 1
        mapControl: root.mapControl
        missionController: root._missionController
        blockedAreas: root.occluders ? root.occluders.all.concat(root.customOccluders) : root.customOccluders
    }

    CaacRouteLegLabels {
        visible: !root._simulationEmergency && !root.planningOverlay
        anchors.fill: parent
        mapControl: root.mapControl
        blockedAreas: root.occluders ? root.occluders.all.concat(root.customOccluders) : root.customOccluders
        warningTopInset: root.occluders ? root.occluders.toolStrip.y + root.occluders.toolStrip.height + root._margin : 0
        segments: root._missionController ? root._missionController.simpleFlightPathSegments : null
    }

    ColumnLayout {
        id: emergencyInstruments
        readonly property real sideInset: root.occluders
            ? root.occluders.toolStrip.x + root.occluders.toolStrip.width + root._margin : root._margin
        readonly property real availableWidth: Math.max(0, root.width - sideInset - root._margin)
        readonly property real maximumDiameter: Math.max(0, root.height
            - 2 * (emergencyControls.height + root._margin) - emergencyValues.implicitHeight - spacing)
        x: sideInset + (availableWidth - width) / 2
        y: Math.max(0, Math.min((root.height - height) / 2, emergencyControls.y - root._margin - height))
        width: availableWidth
        spacing: ScreenTools.defaultFontPixelHeight
        visible: root._simulationEmergency

        CaacAttitudeInstrument {
            objectName: "caacEmergencyAttitude"
            Layout.alignment: Qt.AlignHCenter
            Layout.preferredWidth: Math.min(emergencyInstruments.availableWidth, root.height * 0.7, emergencyInstruments.maximumDiameter)
            Layout.preferredHeight: Layout.preferredWidth
            vehicle: root._vehicle
        }
        RowLayout {
            id: emergencyValues
            Layout.alignment: Qt.AlignHCenter
            Layout.fillWidth: true
            spacing: ScreenTools.defaultFontPixelWidth * 2
            Repeater {
                model: [
                    { key: "Altitude", label: qsTr("Altitude"), fact: root._vehicle ? root._vehicle.altitudeRelative : null },
                    { key: "Speed", label: qsTr("Speed"), fact: root._vehicle ? root._vehicle.groundSpeed : null },
                    { key: "Heading", label: qsTr("Heading"), fact: root._vehicle ? root._vehicle.heading : null }
                ]
                QGCLabel {
                    required property var modelData
                    objectName: "caacEmergency" + modelData.key + "Value"
                    Layout.fillWidth: true
                    horizontalAlignment: Text.AlignHCenter
                    wrapMode: Text.WordWrap
                    font.pointSize: ScreenTools.largeFontPointSize
                    text: modelData.label + ": " + (modelData.fact && isFinite(Number(modelData.fact.rawValue))
                        ? modelData.fact.valueString + " " + modelData.fact.units : "—")
                }
            }
        }
    }

    QGCButton {
        id: emergencyControls
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

    RowLayout {
        id: tools
        readonly property real baseY: root.occluders
            ? root.occluders.toolStrip.y + ScreenTools.defaultFontPixelWidth * 0.7 : root._margin
        x: Math.max(root._margin, Math.min(root._margin + root._panelOffset.x, root.width - width - root._margin))
        y: Math.max(0, Math.min(baseY + root._panelOffset.y,
            root._panelBottom
                - toolbar.height - root._margin * 2))
        readonly property real panelWidth: Math.min(ScreenTools.defaultFontPixelWidth * 46, parent.width / 2)
        width: toolbar.implicitWidth + (panelScroller.visible ? panelWidth + spacing : 0)
        readonly property real availableHeight: root._panelBottom - y - root._margin
        visible: !root._simulationEmergency && (!root.planningPaneExpanded || root.planningOverlay)
            && availableHeight > 0
        spacing: root._margin

        QGCFlickable {
            id: toolbarScroller
            objectName: "caacMainToolsScroller"
            Layout.alignment: Qt.AlignTop
            Layout.preferredWidth: toolbar.implicitWidth
            Layout.preferredHeight: Math.min(toolbar.implicitHeight, tools.availableHeight)
            Layout.fillHeight: false
            contentWidth: width
            contentHeight: toolbar.implicitHeight
            clip: true
          ColumnLayout {
            id: toolbar
            width: parent.width
            spacing: root._margin * 0.5
            SquareTool {
                id: missionButton
                objectName: "caacMissionDetailsButton"
                text: qsTr("Mission details")
                toolIcon: "mission"
                checkable: true
                onCheckedChanged: root.selectPanel(this)
            }
            SquareTool {
                id: dataButton
                objectName: "caacFlightDataButton"
                text: qsTr("Flight data")
                checkable: true
                onCheckedChanged: root.selectPanel(this)
            }
            SquareTool {
                id: mapButton
                objectName: "caacMapSettingsButton"
                text: qsTr("Map settings")
                toolIcon: "map"
                checkable: true
                onCheckedChanged: root.selectPanel(this)
            }
            SquareTool {
                objectName: "caacCalculatorButton"
                text: qsTr("Calculator")
                toolIcon: "calculator"
                onClicked: calculatorFactory.open()
            }
            SquareTool {
                objectName: "caacClearTrajectoryButton"
                text: qsTr("Clear trajectory")
                toolIcon: "clearTrajectory"
                enabled: !!root._vehicle
                onClicked: {
                    if (root._vehicle) { root._vehicle.trajectoryPoints.clear() }
                }
            }
          }
        }

        QGCFlickable {
            id: panelScroller
            objectName: "caacFlyPanelsScroller"
            Layout.alignment: Qt.AlignTop
            Layout.preferredWidth: tools.panelWidth
            Layout.preferredHeight: Math.max(0, Math.min(contentHeight, tools.availableHeight))
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
                        tools.availableHeight))
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
    QGCPopupDialogFactory {
        id: calculatorFactory
        dialogComponent: Component { CaacCalculator { } }
    }
}
