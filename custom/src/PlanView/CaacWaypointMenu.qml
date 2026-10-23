pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import QGroundControl
import QGroundControl.Controls

Menu {
    id: root
    objectName: "caacWaypointContextMenu"
    required property var editor
    property var waypoint: null
    readonly property var controller: editor.routeController
    readonly property var _vehicle: QGroundControl.multiVehicleManager.activeVehicle
    parent: editor.editorMap
    font.pointSize: ScreenTools.menuFontPointSize
    width: ScreenTools.defaultFontPixelWidth * 28
    palette.window: colors.windowShadeDark
    palette.windowText: colors.text
    palette.text: colors.text
    palette.base: colors.windowShadeDark
    palette.highlight: colors.buttonHighlight
    palette.highlightedText: colors.buttonHighlightText
    QGCPalette { id: colors; colorGroupEnabled: root.enabled }

    Component.onCompleted: {
        if (itemAt(0)) { itemAt(0).objectName = "caacDrawingAidButton" }
        if (itemAt(5)) { itemAt(5).objectName = "caacTakeoffPointButton" }
    }

    function show(item) {
        editor.closeWaypointProperties()
        controller.cancelRouteOrientation()
        if (!item || (!item.homePosition && !controller.selectWaypoint(item))) { return }
        waypoint = item
        const point = editor.editorMap.fromCoordinate(item.coordinate, false)
        x = Math.max(0, Math.min(point.x, parent.width - width))
        y = Math.max(0, Math.min(point.y, parent.height - implicitHeight))
        open()
    }
    Menu {
        title: qsTr("Drawing aids")
        palette: root.palette
        font.pointSize: ScreenTools.menuFontPointSize
        width: root.width
        MenuItem {
            implicitHeight: ScreenTools.defaultFontPixelHeight * 3
            contentItem: RowLayout {
                QGCLabel { text: qsTr("Circle") }
                QGCTextField {
                    objectName: "caacAuxiliaryCircleRadius"
                    Layout.fillWidth: true
                    placeholderText: qsTr("Radius")
                    color: colors.textFieldText
                    placeholderTextColor: colors.textFieldText
                    showUnits: true
                    font.pointSize: ScreenTools.menuFontPointSize
                    unitsLabel: "m"
                    inputMethodHints: Qt.ImhFormattedNumbersOnly
                    onEditingFinished: {
                        const radius = text.trim().length ? Number(text) : NaN
                        if (root.waypoint && isFinite(radius) && radius > 0 && radius <= 20000000) {
                            root.editor.addDrawingAid(root.waypoint.coordinate, radius, NaN, false)
                        }
                    }
                }
            }
        }
        MenuItem {
            implicitHeight: ScreenTools.defaultFontPixelHeight * 3
            contentItem: RowLayout {
                QGCLabel { text: qsTr("Guide line") }
                QGCTextField {
                    objectName: "caacAuxiliaryLineBearing"
                    Layout.fillWidth: true
                    placeholderText: qsTr("Bearing")
                    color: colors.textFieldText
                    placeholderTextColor: colors.textFieldText
                    showUnits: true
                    font.pointSize: ScreenTools.menuFontPointSize
                    unitsLabel: "°"
                    inputMethodHints: Qt.ImhFormattedNumbersOnly
                    onEditingFinished: {
                        const angle = text.trim().length ? Number(text) : NaN
                        if (root.waypoint && isFinite(angle)) {
                            root.editor.addDrawingAid(root.waypoint.coordinate, 0, angle, false)
                        }
                    }
                }
            }
        }
        MenuItem { text: qsTr("Clear drawing aids"); onTriggered: root.editor.clearDrawingAids() }
    }
    MenuItem {
        objectName: "caacPlaceCoordinateAxes"
        text: qsTr("Place coordinate axes")
        onTriggered: { if (root.waypoint) { root.editor.addDrawingAid(root.waypoint.coordinate, 0, NaN, true) } }
    }
    MenuSeparator { }
    MenuItem {
        objectName: "caacContextDeleteWaypoint"
        visible: root.waypoint && !root.waypoint.homePosition
        text: root.waypoint ? qsTr("Delete waypoint %1").arg(root.waypoint.waypointNumber) : qsTr("Delete")
        enabled: root.waypoint && (root.waypoint.command === 16 || root.waypoint.isLandCommand) && root.editor.canDeleteWaypoints
        onTriggered: root.editor.removeWaypoint(root.waypoint)
    }
    MenuItem {
        objectName: "caacContextClearWaypoints"
        text: qsTr("Clear waypoints")
        onTriggered: root.editor.confirmClearWaypoints()
    }
    Menu {
        title: qsTr("Takeoff point")
        palette: root.palette
        font.pointSize: ScreenTools.menuFontPointSize
        MenuItem {
            objectName: "caacContextPickHome"
            text: qsTr("Set takeoff point on map")
            onTriggered: root.editor.pickPlannedHome()
        }
        MenuItem {
            objectName: "caacContextCenterHome"
            text: qsTr("Center takeoff point")
            onTriggered: root.editor.centerPlannedHome()
        }
        MenuItem {
            objectName: "caacContextHomeRelations"
            text: qsTr("H point and waypoint relations")
            onTriggered: root.editor.openHomeRelations()
        }
        MenuItem {
            objectName: "caacContextWaypointHome"
            visible: root.waypoint && !root.waypoint.homePosition
            text: root.waypoint ? qsTr("Use waypoint %1 as takeoff point").arg(root.waypoint.waypointNumber) : ""
            enabled: root.waypoint && root.waypoint.command === 16
            onTriggered: root.controller.useWaypointAsHome(root.waypoint)
        }
    }
    MenuItem {
        objectName: "caacContextWaypointLanding"
        text: qsTr("Set landing point")
        visible: root.waypoint && !root.waypoint.homePosition
        enabled: root.waypoint && root.waypoint.command === 16
        onTriggered: root.controller.useWaypointAsLanding(root.waypoint)
    }
    MenuSeparator { }
    MenuItem {
        objectName: "caacContextGoto"
        text: qsTr("Fly to this point")
        enabled: root.waypoint && root._vehicle && root._vehicle.armed && root._vehicle.airborne && root._vehicle.supports.guidedMode
        onTriggered: root.editor.confirmGoto(root.waypoint.coordinate)
    }
    MenuSeparator { }
    MenuItem {
        objectName: "caacContextReverse"
        text: qsTr("Reverse route")
        onTriggered: root.controller.reverseWaypoints()
    }
    MenuItem {
        objectName: "caacContextDeleteAxes"
        text: qsTr("Delete coordinate axes")
        visible: root.editor.hasCoordinateAxes
        enabled: root.editor.hasCoordinateAxes
        onTriggered: root.editor.clearCoordinateAxes()
    }
    Connections {
        target: root.editor.planMasterController ? root.editor.planMasterController.missionController : null
        function onVisualItemsReset() { root.close(); root.waypoint = null }
    }
    Connections {
        target: root.editor
        function onVisibleChanged() { if (!root.editor.visible) { root.close() } }
        function onEnabledChanged() { if (!root.editor.enabled) { root.close() } }
    }
}
