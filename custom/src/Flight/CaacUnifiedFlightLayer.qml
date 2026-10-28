pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Layouts

import QGroundControl
import QGroundControl.Controls
import QGroundControl.FlyView

Item {
    id: root
    objectName: "caacUnifiedFlightLayer"
    required property var mapControl
    property bool planningPaneExpanded: false
    property real planningPanelWidth: 0
    property real planningNavigationHeight: 0
    readonly property real _toolsMargin: ScreenTools.defaultFontPixelWidth * 0.75
    readonly property bool _emergency: QGroundControl.corePlugin.simulation.emergencyMode
    readonly property bool _figureEightView: QGroundControl.corePlugin.figureEight.viewActive
    readonly property real telemetryHeight: footer.visible ? footer.height + root._toolsMargin * 2 : 0
    readonly property var mapToolsContainer: customLayer.item ? customLayer.item.mapToolsContainer : null
    readonly property real flightActionsLeft: customLayer.item ? customLayer.item.flightActionsLeft : NaN
    QGCPalette { id: qgcPal; colorGroupEnabled: true }

    Loader {
        id: customLayer
        anchors.fill: parent
        source: "qrc:/Custom/qml/QGroundControl/FlyView/FlyViewCustomLayer.qml"
        onLoaded: {
            item.mapControl = Qt.binding(() => root.mapControl)
            item.planningOverlay = true
            item.planningPaneExpanded = Qt.binding(() => root.planningPaneExpanded)
            item.planningNavigationHeight = Qt.binding(() => root.planningNavigationHeight)
            item.reservedBottomHeight = Qt.binding(() => root.telemetryHeight)
            item.missionControllerOverride = globals.planMasterControllerFlyView.missionController
        }
    }
    CaacFlightTelemetryBar {
        id: footer
        height: implicitHeight
        width: Math.max(0, Math.min(ScreenTools.defaultFontPixelWidth * 100,
            root.width - (root.planningPaneExpanded ? root.planningPanelWidth : 0) - root._toolsMargin * 2))
        x: (root.width - (root.planningPaneExpanded ? root.planningPanelWidth : 0) - width) / 2
        anchors.bottom: parent.bottom
        anchors.bottomMargin: root._toolsMargin
        visible: !root._emergency && !root._figureEightView
        vehicle: QGroundControl.multiVehicleManager.activeVehicle
        missionController: globals.planMasterControllerFlyView.missionController
    }
    CaacFlightDashboard {
        id: dashboard
        z: -1
        visible: !root._emergency && !root._figureEightView
        compact: root.planningPaneExpanded
        availableWidth: root.width - (root.planningPaneExpanded ? root.planningPanelWidth : 0)
        topInset: !root.planningPaneExpanded
            && dashboard.x + dashboard.width > root.width - root.planningPanelWidth
            ? root.planningNavigationHeight : 0
        bottomInset: root.telemetryHeight
        vehicle: QGroundControl.multiVehicleManager.activeVehicle
    }
}
