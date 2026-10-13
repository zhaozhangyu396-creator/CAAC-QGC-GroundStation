pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Controls

import QGroundControl
import QGroundControl.Controls

Item {
    id: root
    objectName: "caacWaypointOverlap"
    required property var mapControl
    required property var missionController
    property var blockedAreas: []
    readonly property real _radius: ScreenTools.defaultFontPixelHeight * 1.4
    readonly property var _labels: {
        if (!hover.hovered || !mapControl || !missionController || !missionController.visualItems) {
            return []
        }
        void mapControl.center
        void mapControl.zoomLevel
        void mapControl.width
        void mapControl.height
        const items = missionController.visualItems
        const position = hover.point.position
        if (blockedAreas.some(area => area.width > 0 && area.height > 0 && position.x >= area.x
                && position.x <= area.x + area.width && position.y >= area.y && position.y <= area.y + area.height)) {
            return []
        }
        const points = []
        let closest = null
        let distance = root._radius
        for (let i = 0; i < items.count; ++i) {
            const item = items.get(i)
            if (!item || !item.coordinate.isValid || (i > 0 && (!item.isSimpleItem
                    || !item.specifiesCoordinate || item.isStandaloneCoordinate))) {
                continue
            }
            const projected = mapControl.fromCoordinate(item.coordinate, false)
            const point = mapControl.mapToItem(root, projected.x, projected.y)
            if (!isFinite(point.x) || !isFinite(point.y)) { continue }
            const candidate = {point: point, coordinate: item.coordinate,
                label: i === 0 ? qsTr("Takeoff point") : item.sequenceNumber.toString()}
            points.push(candidate)
            const offset = Math.hypot(point.x - position.x, point.y - position.y)
            if (offset <= distance) { closest = candidate; distance = offset }
        }
        if (!closest) { return [] }
        return points.filter(candidate => candidate.coordinate.distanceTo(closest.coordinate) < 0.01)
            .map(candidate => candidate.label)
    }
    QGCPalette { id: palette; colorGroupEnabled: root.enabled }
    HoverHandler { id: hover; acceptedDevices: PointerDevice.Mouse | PointerDevice.TouchPad }
    ToolTip {
        id: tip
        objectName: "caacWaypointOverlapTip"
        enabled: false
        closePolicy: Popup.NoAutoClose
        visible: root.visible && hover.hovered && root._labels.length > 1
        text: qsTr("Overlapping waypoints: %1").arg(root._labels.join(", "))
        x: Math.min(hover.point.position.x + ScreenTools.defaultFontPixelWidth, root.width - width)
        y: Math.max(0, hover.point.position.y - height - ScreenTools.defaultFontPixelHeight * 0.5)
        background: Rectangle {
            color: palette.windowShadeDark
            radius: ScreenTools.defaultBorderRadius
            border.color: palette.text
        }
        contentItem: QGCLabel {
            objectName: "caacWaypointOverlapText"
            text: tip.text
            color: palette.text
            font.pointSize: ScreenTools.menuFontPointSize
        }
        font.pointSize: ScreenTools.menuFontPointSize
        delay: 0
        timeout: -1
    }
}
