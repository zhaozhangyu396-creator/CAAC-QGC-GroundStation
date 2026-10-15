pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Layouts
import QtPositioning

import QGroundControl
import QGroundControl.Controls
import QGroundControl.FlightMap
import QGroundControl.PlanView

ColumnLayout {
    id: root
    objectName: "caacGeoFenceSettings"
    required property var actionFact
    readonly property var master: globals.planMasterControllerPlanView
    readonly property var controller: master ? master.geoFenceController : null
    readonly property var vehicle: QGroundControl.multiVehicleManager.activeVehicle
    readonly property bool transferReady: !!controller && !!vehicle && controller.supported && !controller.syncInProgress
    property bool drawing: false
    property int previousAction: 1
    Layout.fillWidth: true
    spacing: ScreenTools.defaultFontPixelHeight * 0.5
    QGCPalette { id: palette }

    function fitPreview() {
        const map = previewLoader.item
        if (!controller || !map || !map.mapReady) { return }
        const coordinates = []
        for (let i = 0; i < controller.polygons.count; ++i) {
            const polygon = controller.polygons.get(i)
            for (const coordinate of polygon.path) { if (coordinate.isValid) { coordinates.push(coordinate) } }
        }
        for (let i = 0; i < controller.circles.count; ++i) {
            const circle = controller.circles.get(i)
            if (!circle.center.isValid) { continue }
            for (const azimuth of [0, 90, 180, 270]) {
                coordinates.push(circle.center.atDistanceAndAzimuth(circle.radius.rawValue, azimuth))
            }
        }
        map.fitCoordinates(coordinates)
    }
    function drawFence() {
        const map = previewLoader.item
        if (!controller || !map || !map.mapReady) { return }
        drawing = true
        mainWindow.caacFenceVisible = true
        const topLeft = map.toCoordinate(Qt.point(map.width * 0.25, map.height * 0.25), false)
        const bottomRight = map.toCoordinate(Qt.point(map.width * 0.75, map.height * 0.75), false)
        if (topLeft.isValid && bottomRight.isValid) { controller.addInclusionPolygon(topLeft, bottomRight) }
    }
    Connections {
        target: root.controller
        function onContainsItemsChanged() { Qt.callLater(root.fitPreview) }
        function onSyncInProgressChanged() { if (!root.controller.syncInProgress) { Qt.callLater(root.fitPreview) } }
    }
    RowLayout {
        Layout.fillWidth: true
        Rectangle {
            Layout.preferredWidth: ScreenTools.defaultFontPixelWidth * 0.4
            Layout.preferredHeight: ScreenTools.defaultFontPixelHeight
            color: palette.settingsAccent
            radius: width / 2
        }
        QGCLabel { text: qsTr("电子围栏"); font.bold: true; Layout.fillWidth: true }
    }
    SettingsFormRow {
        QGCCheckBoxSlider {
            objectName: "caacFenceEnabled"
            Layout.fillWidth: true
            text: qsTr("围栏开关")
            enabled: !!root.actionFact && !!root.vehicle && !root.vehicle.armed
            Binding on checked { value: !!root.actionFact && Number(root.actionFact.rawValue) !== 0 }
            onClicked: {
                if (!root.actionFact) { return }
                if (checked) {
                    const values = root.actionFact.enumValues.map(value => Number(value))
                    const desired = values.indexOf(root.previousAction) >= 0 ? root.previousAction : values.find(value => value > 0)
                    if (desired !== undefined) { root.actionFact.rawValue = desired }
                } else {
                    root.previousAction = Number(root.actionFact.rawValue)
                    root.actionFact.rawValue = 0
                }
            }
        }
    }
    SettingsFormRow {
        QGCCheckBoxSlider {
            objectName: "caacFenceShowOnMap"
            Layout.fillWidth: true
            text: qsTr("显示在地图上")
            Binding on checked { value: mainWindow.caacFenceVisible }
            onClicked: mainWindow.caacFenceVisible = checked
        }
    }
    RowLayout {
        Layout.fillWidth: true
        QGCButton {
            objectName: "caacFenceDraw"
            text: qsTr("绘制电子围栏")
            Layout.fillWidth: true
            enabled: !!root.controller && !root.controller.syncInProgress && !!previewLoader.item && previewLoader.item.mapReady
            onClicked: root.drawFence()
        }
        QGCButton {
            objectName: "caacFenceRead"
            text: qsTr("读取")
            Layout.fillWidth: true
            enabled: root.transferReady
            onClicked: { root.drawing = false; root.controller.loadFromVehicle() }
        }
        QGCButton {
            objectName: "caacFenceWrite"
            text: qsTr("写入")
            Layout.fillWidth: true
            enabled: root.transferReady && !root.vehicle.armed && root.controller.containsItems
            onClicked: { root.drawing = false; root.controller.sendToVehicle() }
        }
    }
    RowLayout {
        Layout.fillWidth: true
        QGCLabel {
            Layout.fillWidth: true
            wrapMode: Text.WordWrap
            text: root.controller && root.controller.dirty ? qsTr("围栏预览（尚未写入飞控）") : qsTr("已保存围栏预览（地图）")
        }
        QGCButton {
            objectName: "caacFenceCenterMainMap"
            text: qsTr("主地图定位到此位置")
            _horizontalPadding: ScreenTools.defaultFontPixelWidth * 0.5
            enabled: !!root.controller && root.controller.containsItems && !!previewLoader.item
            onClicked: mainWindow.centerPlanMap(previewLoader.item.center, previewLoader.item.zoomLevel)
        }
    }
    Rectangle {
        Layout.fillWidth: true
        Layout.preferredHeight: ScreenTools.defaultFontPixelHeight * 16
        color: palette.windowShadeDark
        border.color: palette.groupBorder
        border.width: 1
        radius: ScreenTools.defaultBorderRadius
        clip: true
        Loader {
            id: previewLoader
            anchors.fill: parent
            active: !!root.controller
            sourceComponent: FencePreview { }
        }
        QGCLabel {
            anchors.centerIn: parent
            width: parent.width - ScreenTools.defaultFontPixelWidth * 2
            horizontalAlignment: Text.AlignHCenter
            wrapMode: Text.WordWrap
            visible: !root.drawing && (!root.controller || !root.controller.containsItems)
            text: qsTr("绘制或读取围栏后将在此显示地图预览")
        }
    }
    QGCLabel {
        Layout.fillWidth: true
        wrapMode: Text.WordWrap
        visible: root.drawing
        text: qsTr("拖动顶点调整围栏；点击边中点可加点。完成后点击写入，将围栏发送到已连接的飞控。")
    }
    component FencePreview: FlightMap {
        id: preview
        objectName: "caacFencePreviewMap"
        mapName: "CaacFencePreview"
        property rect centerViewport: Qt.rect(0, 0, width, height)
        planView: true
        allowGCSLocationCenter: false
        allowVehicleLocationCenter: false
        opacity: root.drawing || root.controller.containsItems ? 1 : 0
        zoomLevel: 16
        Component.onCompleted: {
            const home = root.master.missionController.plannedHomePosition
            if (home.isValid) { center = home }
        }
        onMapReadyChanged: if (mapReady) { Qt.callLater(root.fitPreview) }
        function fitCoordinates(coordinates) { fit.fitMapViewportToAllCoordinates(coordinates) }
        MapFitFunctions {
            id: fit
            map: preview
            planMasterController: root.master
            usePlannedHomePosition: true
        }
        GeoFenceMapVisuals {
            map: preview
            myGeoFenceController: root.controller
            homePosition: root.master.missionController.plannedHomePosition
            planView: true
            interactive: root.drawing
        }
    }
}
