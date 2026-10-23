pragma ComponentBehavior: Bound

import QtQuick

import QGroundControl
import QGroundControl.Controls

Item {
    id: root
    required property var mapControl
    property var segments: null
    property var previewSegments: []
    property var runtimeMissionController: null
    property var runtimeVehicle: null
    property var blockedAreas: []
    property real warningTopInset: 0
    property int _projectionRevision: 0
    readonly property var _runtimeSegment: {
        if (!runtimeVehicle || !runtimeVehicle.armed || !runtimeVehicle.airborne || !runtimeMissionController) {
            return null
        }
        const sequence = runtimeMissionController.currentMissionIndex
        const items = runtimeMissionController.visualItems
        let previous = items.count > 0 ? items.get(0) : null
        for (let i = 1; i < items.count; ++i) {
            const item = items.get(i)
            if (item.sequenceNumber <= sequence && sequence <= item.lastSequenceNumber) {
                if (!previous || !item.isSimpleItem || sequence !== item.sequenceNumber
                    || (item.command !== 20 && (!item.specifiesCoordinate || item.isStandaloneCoordinate))) {
                    return null
                }
                return { coordinate1: previous.isSimpleItem ? previous.entryCoordinate : previous.exitCoordinate,
                    coordinate2: item.command === 20 ? items.get(0).entryCoordinate : item.entryCoordinate }
            }
            if (item.specifiesCoordinate && !item.isStandaloneCoordinate) { previous = item }
        }
        return null
    }
    readonly property real _runtimeSpeed: {
        if (!_runtimeSegment) { return NaN }
        const simulation = QGroundControl.corePlugin.simulation
        if (simulation.active && simulation.vehicle === runtimeVehicle) {
            const details = simulation.currentMissionDetails
            return details.sequence === runtimeMissionController.currentMissionIndex ? details.speed : NaN
        }
        return runtimeVehicle.commandedFlightSpeedType === (runtimeVehicle.inFwdFlight ? 0 : 1)
            ? runtimeVehicle.commandedFlightSpeed : NaN
    }

    function isRuntimeSegment(segment) {
        if (!_runtimeSegment) { return false }
        if (segment === _runtimeSegment) { return true }
        // The planning preview and the uploaded mission own separate segment objects.
        return segment.coordinate1.distanceTo(_runtimeSegment.coordinate1) < 0.1
            && segment.coordinate2.distanceTo(_runtimeSegment.coordinate2) < 0.1
    }

    readonly property bool terrainCollision: {
        if (segments) {
            for (let i = 0; i < segments.count; ++i) {
                if (segments.get(i).terrainCollision) { return true }
            }
        }
        return false
    }
    Rectangle {
        id: clearanceWarning
        objectName: "caacTerrainClearanceWarning"
        parent: root.mapControl && root.mapControl.parent && root.mapControl.parent.parent
            ? root.mapControl.parent.parent : root
        readonly property real margin: ScreenTools.defaultFontPixelWidth
        readonly property real availableWidth: parent ? parent.width : 0
        readonly property real availableHeight: parent ? parent.height : 0
        property point dragOffset: Qt.point(0, 0)
        x: Math.max(margin, Math.min(availableWidth / 3 - width / 2 + dragOffset.x, availableWidth - width - margin))
        y: Math.max(margin, Math.min(Math.max(root.warningTopInset, ScreenTools.defaultFontPixelHeight * 6)
            + dragOffset.y, availableHeight - height - margin))
        width: Math.min(Math.max(0, availableWidth - margin * 2),
            warning.implicitWidth + ScreenTools.defaultFontPixelWidth * 2)
        height: warning.implicitHeight + ScreenTools.defaultFontPixelHeight
        visible: root.visible && root.terrainCollision
        z: QGroundControl.zOrderTopMost
        color: QGroundControl.globalPalette.windowShadeDark
        border.color: QGroundControl.globalPalette.warningText
        radius: ScreenTools.defaultBorderRadius
        DragHandler {
            target: null
            acceptedButtons: Qt.LeftButton
            property point startOffset
            onActiveChanged: if (active) { startOffset = clearanceWarning.dragOffset }
            onActiveTranslationChanged: if (active) {
                clearanceWarning.dragOffset = Qt.point(startOffset.x + activeTranslation.x, startOffset.y + activeTranslation.y)
            }
        }
        QGCLabel {
            id: warning
            anchors.centerIn: parent
            width: parent.width - ScreenTools.defaultFontPixelWidth * 2
            text: qsTr("True height is too low; adjust the waypoint altitude.")
            color: QGroundControl.globalPalette.warningText
            font.bold: true
            font.pointSize: ScreenTools.menuFontPointSize
            wrapMode: Text.WordWrap
        }
    }

    function screenPoint(coordinate) {
        const point = mapControl.fromCoordinate(coordinate, false)
        return mapControl.mapToItem(root, point.x, point.y)
    }

    function overlapsControls(point, radius) {
        for (const area of blockedAreas) {
            if (area.width <= 0 || area.height <= 0) { continue }
            const dx = Math.max(area.x - point.x, 0, point.x - area.x - area.width)
            const dy = Math.max(area.y - point.y, 0, point.y - area.y - area.height)
            if (Math.hypot(dx, dy) < radius) { return true }
        }
        return false
    }

    component LegLabel: Item {
        id: label
        required property var object
        required property int index
        objectName: "caacRouteLegLabel_" + index
        readonly property var metrics: QGroundControl.corePlugin.routeLegMetrics(object.coordinate1, object.coordinate2)
        readonly property point _from: {
            root._projectionRevision
            return root.screenPoint(object.coordinate1)
        }
        readonly property point _to: {
            root._projectionRevision
            return root.screenPoint(object.coordinate2)
        }
        readonly property point _midpoint: {
            root._projectionRevision
            return metrics.midpoint !== undefined ? root.screenPoint(metrics.midpoint) : Qt.point(-1, -1)
        }
        readonly property real _angle: Math.atan2(_to.y - _from.y, _to.x - _from.x) * 180 / Math.PI
        readonly property real displayedFlightSpeed: isFinite(root._runtimeSpeed) && root._runtimeSpeed > 0
            && root.isRuntimeSegment(object) ? root._runtimeSpeed : object.flightSpeed
        readonly property string legText: metrics.distance !== undefined
            ? qsTr("%1 m  %2°  %3 m/s").arg(metrics.distance.toFixed(1))
                .arg((Math.round(metrics.bearing) % 360).toFixed(0))
                .arg(isFinite(displayedFlightSpeed) && displayedFlightSpeed > 0 ? displayedFlightSpeed.toFixed(1) : "—") : ""
        x: _midpoint.x
        y: _midpoint.y
        visible: metrics.distance !== undefined && isFinite(_midpoint.x) && isFinite(_midpoint.y)
            && _midpoint.x >= 0 && _midpoint.y >= 0 && _midpoint.x <= root.width && _midpoint.y <= root.height
            && !root.overlapsControls(_midpoint, Math.hypot(text.implicitWidth / 2, text.implicitHeight * 1.5))
            && Math.hypot(_to.x - _from.x, _to.y - _from.y) >= ScreenTools.defaultFontPixelHeight * 2
        rotation: _angle > 90 ? _angle - 180 : _angle < -90 ? _angle + 180 : _angle
        z: QGroundControl.zOrderWaypointLines + 2

        Rectangle {
            x: -width / 2
            y: -height - ScreenTools.defaultFontPixelWidth * 0.5
            width: text.implicitWidth + ScreenTools.defaultFontPixelWidth
            height: text.implicitHeight + ScreenTools.defaultFontPixelHeight * 0.25
            radius: ScreenTools.defaultBorderRadius
            color: QGroundControl.globalPalette.windowShadeDark
            opacity: 0.9
            QGCLabel {
                id: text
                anchors.centerIn: parent
                text: label.legText
                color: QGroundControl.globalPalette.text
                font.bold: true
                font.pointSize: ScreenTools.menuFontPointSize
            }
        }
    }

    Repeater {
        model: root.previewSegments.length > 0 ? null : root.segments
        delegate: LegLabel { }
    }
    Repeater {
        model: root.previewSegments
        delegate: LegLabel {
            required property var modelData
            object: modelData
        }
    }

    Connections {
        target: root.mapControl && root.mapControl.center !== undefined ? root.mapControl : null
        function onCenterChanged() { ++root._projectionRevision }
        function onBearingChanged() { ++root._projectionRevision }
        function onZoomLevelChanged() { ++root._projectionRevision }
        function onWidthChanged() { ++root._projectionRevision }
        function onHeightChanged() { ++root._projectionRevision }
    }
    Connections {
        target: root.mapControl && root.mapControl.geoMap !== undefined ? root.mapControl.geoMap.camera : null
        function onScenePoseChanged() { ++root._projectionRevision }
        function onViewportSizeChanged() { ++root._projectionRevision }
        function onVerticalFieldOfViewChanged() { ++root._projectionRevision }
    }
}
