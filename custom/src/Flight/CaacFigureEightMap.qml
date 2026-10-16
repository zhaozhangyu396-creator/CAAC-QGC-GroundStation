pragma ComponentBehavior: Bound

import QtQuick
import QtLocation
import QtPositioning

import QGroundControl
import QGroundControl.Controls

Item {
    id: root

    required property var mapControl

    readonly property var _controller: QGroundControl.corePlugin.figureEight
    readonly property var _fence: _controller ? _controller.fence : null
    readonly property var _referencePath: _controller ? _controller.referencePath : []
    readonly property var _recordedPath: _controller ? _controller.recordedPath : []
    readonly property var _fencePath: {
        const result = []
        if (_fence) {
            for (const coordinate of _fence.path) { result.push(coordinate) }
        }
        if (result.length > 2
            && (result[0].latitude !== result[result.length - 1].latitude
                || result[0].longitude !== result[result.length - 1].longitude)) {
            result.push(result[0])
        }
        return result
    }
    readonly property bool _showReference: !!_controller && _controller.viewActive
        && _controller.showOnMap && _referencePath.length > 1
    readonly property bool _showRecorded: !!_controller && _controller.viewActive
        && _controller.showOnMap && _recordedPath.length > 1
    readonly property bool _showFence: !!_controller && _controller.viewActive
        && !!_fence && _fence.showOnMap && _fencePath.length > 3
    property string _siteKey: ""
    property bool _fitPending: false
    readonly property real _corridorPixels: {
        if (!mapControl || !mapControl.mapReady || !_controller || !_controller.pointA.isValid) { return 0 }
        const zoom = mapControl.zoomLevel
        const origin = mapControl.fromCoordinate(_controller.pointA, false)
        const bearing = _controller.pointA.latitude > 0 ? 180 : 0
        const edge = mapControl.fromCoordinate(_controller.pointA.atDistanceAndAzimuth(_controller.fenceWidth, bearing), false)
        return isFinite(zoom) ? Math.hypot(origin.x - edge.x, origin.y - edge.y) * 2 : 0
    }

    function attach(item) {
        if (!item || item.attachedMap === mapControl) {
            return
        }
        if (item.attachedMap) {
            item.attachedMap.removeMapItem(item)
            item.attachedMap = null
        }
        if (mapControl) {
            mapControl.addMapItem(item)
            item.attachedMap = mapControl
        }
    }

    function updateSite() {
        const key = _referencePath.length > 1
            ? [_controller.pointA.latitude, _controller.pointA.longitude,
                _controller.pointB.latitude, _controller.pointB.longitude].join(",") : ""
        if (key !== _siteKey) {
            _siteKey = key
            _fitPending = key.length > 0
            Qt.callLater(fitTrainingSite)
        }
    }

    function fitTrainingSite() {
        if (!_fitPending || (!_showReference && !_showFence) || !visible || !mapControl || mapControl.mapReady === false
            || !mapControl.visible || mapControl.width <= 0 || mapControl.height <= 0) {
            return
        }
        const fitPath = _showFence ? _fencePath : _referencePath
        const origin = fitPath[0]
        let north = -90
        let south = 90
        let east = -180
        let west = 180
        for (const coordinate of fitPath) {
            // Unwrap around the field, so a date-line crossing stays a local view.
            const longitude = (coordinate.longitude - origin.longitude + 540) % 360 - 180
            north = Math.max(north, coordinate.latitude)
            south = Math.min(south, coordinate.latitude)
            east = Math.max(east, longitude)
            west = Math.min(west, longitude)
        }
        const latitudePadding = Math.max((north - south) / 4, _controller.fenceWidth / 111320)
        const longitudePadding = Math.max((east - west) / 4,
            latitudePadding / Math.max(0.001, Math.cos(origin.latitude * Math.PI / 180)))
        const topLeft = QtPositioning.coordinate(Math.min(90, north + latitudePadding),
            (origin.longitude + west - longitudePadding + 540) % 360 - 180)
        const bottomRight = QtPositioning.coordinate(Math.max(-90, south - latitudePadding),
            (origin.longitude + east + longitudePadding + 540) % 360 - 180)
        const region = QtPositioning.rectangle(topLeft, bottomRight)
        // Release FlyViewMap's initial saved-position binding before fitting changes its center internally.
        mapControl.center = region.center
        mapControl.setVisibleRegion(region)
        _fitPending = false
    }

    onMapControlChanged: {
        attach(outerFenceLine)
        attach(corridorLine)
        attach(referenceLine)
        attach(recordedLine)
        for (let i = 0; markers && i < markers.count; ++i) {
            attach(markers.objectAt(i))
        }
        _fitPending = _referencePath.length > 1
        Qt.callLater(fitTrainingSite)
    }

    onVisibleChanged: if (visible) { Qt.callLater(fitTrainingSite) }
    on_ShowFenceChanged: {
        if (_showFence) {
            _fitPending = true
            Qt.callLater(fitTrainingSite)
        }
    }

    QGCPalette { id: palette; colorGroupEnabled: true }

    MapPolyline {
        id: outerFenceLine

        property var attachedMap: null

        objectName: "caacFigureEightOuterFence"
        visible: root._showFence
        path: root._fencePath
        line.width: ScreenTools.defaultFontPixelWidth * 0.35
        line.color: root._fence && root._fence.applied ? palette.colorGreen : palette.colorBlue
        z: QGroundControl.zOrderWaypointLines + 1

        Component.onCompleted: root.attach(outerFenceLine)
        Component.onDestruction: {
            if (attachedMap) { attachedMap.removeMapItem(outerFenceLine) }
        }
    }

    MapPolyline {
        id: corridorLine
        property var attachedMap: null
        objectName: "caacFigureEightTrainingCorridor"
        visible: root._showReference && root._corridorPixels > 0
        path: root._referencePath
        line.width: Math.max(ScreenTools.defaultFontPixelWidth * 0.4, root._corridorPixels)
        line.color: Qt.tint(palette.colorGreen,
            Qt.rgba(palette.mapWidgetBorderLight.r, palette.mapWidgetBorderLight.g,
                palette.mapWidgetBorderLight.b, 0.65))
        opacity: 0.65
        z: QGroundControl.zOrderWaypointLines + 2
        Component.onCompleted: root.attach(corridorLine)
        Component.onDestruction: {
            if (attachedMap) { attachedMap.removeMapItem(corridorLine) }
        }
    }

    MapPolyline {
        id: referenceLine

        property var attachedMap: null

        objectName: "caacFigureEightReferencePath"
        visible: root._showReference
        path: root._referencePath
        line.width: ScreenTools.defaultFontPixelWidth * 0.4
        line.color: palette.colorRed
        z: QGroundControl.zOrderWaypointLines + 3

        Component.onCompleted: root.attach(referenceLine)
        Component.onDestruction: {
            if (attachedMap) {
                attachedMap.removeMapItem(referenceLine)
            }
        }
    }

    MapPolyline {
        id: recordedLine

        property var attachedMap: null

        objectName: "caacFigureEightRecordedPath"
        visible: root._showRecorded
        path: root._recordedPath
        line.width: ScreenTools.defaultFontPixelWidth * 0.3
        line.color: palette.colorOrange
        z: QGroundControl.zOrderWaypointLines + 4

        Component.onCompleted: root.attach(recordedLine)
        Component.onDestruction: {
            if (attachedMap) {
                attachedMap.removeMapItem(recordedLine)
            }
        }
    }

    Instantiator {
        id: markers

        // Four quarter points on each circle share the tangent point at sample zero.
        model: [0, 30, 60, 90, 150, 180, 210]

        delegate: MapQuickItem {
            id: marker

            required property int modelData
            required property int index
            property var attachedMap: null

            objectName: "caacFigureEightTrainingMarker" + index
            coordinate: root._referencePath.length > modelData
                ? root._referencePath[modelData] : QtPositioning.coordinate()
            visible: root._showReference && coordinate.isValid
            z: QGroundControl.zOrderMapItems + 4
            anchorPoint: Qt.point(symbol.width / 2, symbol.height / 2)

            sourceItem: Rectangle {
                id: symbol

                width: ScreenTools.defaultFontPixelHeight * 0.85
                height: width
                radius: width / 2
                color: palette.colorGreen
                border.color: palette.mapWidgetBorderLight
                border.width: ScreenTools.defaultFontPixelWidth * 0.2
            }

            Component.onCompleted: root.attach(marker)
            Component.onDestruction: {
                if (attachedMap) {
                    attachedMap.removeMapItem(marker)
                }
            }
        }
    }

    Connections {
        target: root._controller
        function onSiteChanged() { root.updateSite() }
        function onSettingsChanged() { Qt.callLater(root.fitTrainingSite) }
        function onViewActiveChanged() {
            if (root._controller.viewActive) {
                root._fitPending = root._referencePath.length > 1
                Qt.callLater(root.fitTrainingSite)
            }
        }
    }

    Connections {
        target: root._fence
        function onGeometryChanged() {
            root._fitPending = root._referencePath.length > 1
            Qt.callLater(root.fitTrainingSite)
        }
    }

    Connections {
        target: root.mapControl
        ignoreUnknownSignals: true
        function onMapReadyChanged() { Qt.callLater(root.fitTrainingSite) }
        function onVisibleChanged() { Qt.callLater(root.fitTrainingSite) }
        function onWidthChanged() { Qt.callLater(root.fitTrainingSite) }
        function onHeightChanged() { Qt.callLater(root.fitTrainingSite) }
    }

    Component.onCompleted: updateSite()
}
