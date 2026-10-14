pragma ComponentBehavior: Bound

import QtQuick

import QGroundControl
import QGroundControl.Controls

Item {
    id: root
    required property var mapControl
    property var guides: []
    property var angles: []
    property int _projectionRevision: 0
    z: QGroundControl.zOrderWaypointLines + 2
    QGCPalette { id: palette; colorGroupEnabled: root.enabled }

    function screenPoint(coordinate) {
        const point = mapControl.fromCoordinate(coordinate, false)
        return mapControl.mapToItem(root, point.x, point.y)
    }

    Canvas {
        id: guideCanvas
        objectName: "caacWaypointSnapGuides"
        anchors.fill: parent
        readonly property int guideCount: root.guides.length
        visible: guideCount > 0
        onGuideCountChanged: requestPaint()
        onPaint: {
            const ctx = getContext("2d")
            ctx.clearRect(0, 0, width, height)
            ctx.lineWidth = ScreenTools.defaultFontPixelWidth * 0.2
            ctx.setLineDash([ScreenTools.defaultFontPixelWidth, ScreenTools.defaultFontPixelWidth * 0.6])
            for (const guide of root.guides) {
                const from = root.screenPoint(guide.from)
                const to = root.screenPoint(guide.to)
                if (![from.x, from.y, to.x, to.y].every(isFinite)) { continue }
                ctx.strokeStyle = guide.kind >= 2 ? palette.colorBlue : palette.colorYellowGreen
                ctx.beginPath()
                ctx.moveTo(from.x, from.y)
                ctx.lineTo(to.x, to.y)
                ctx.stroke()
                const marker = guide.intersection || guide.midpoint
                if (marker) {
                    const center = root.screenPoint(marker)
                    ctx.beginPath()
                    ctx.arc(center.x, center.y, ScreenTools.defaultFontPixelWidth * 0.5, 0, Math.PI * 2)
                    ctx.fillStyle = palette.colorBlue
                    ctx.fill()
                }
            }
        }
    }
    onGuidesChanged: guideCanvas.requestPaint()
    on_ProjectionRevisionChanged: guideCanvas.requestPaint()

    Repeater {
        model: root.angles
        delegate: Item {
            id: vertex
            required property var modelData
            readonly property real angle: modelData.angle
            readonly property real _radius: ScreenTools.defaultFontPixelHeight * 1.2
            readonly property point _point: {
                root._projectionRevision
                return root.screenPoint(modelData.coordinate)
            }
            readonly property point _previous: {
                root._projectionRevision
                return root.screenPoint(modelData.previous)
            }
            readonly property point _next: {
                root._projectionRevision
                return root.screenPoint(modelData.next)
            }
            readonly property real _start: Math.atan2(_previous.y - _point.y, _previous.x - _point.x)
            readonly property real _sweep: {
                const end = Math.atan2(_next.y - _point.y, _next.x - _point.x)
                return ((end - _start + Math.PI * 3) % (Math.PI * 2)) - Math.PI
            }
            x: _point.x
            y: _point.y
            visible: isFinite(x) && isFinite(y) && x >= 0 && y >= 0 && x <= root.width && y <= root.height
                && Math.hypot(_previous.x - x, _previous.y - y) > _radius * 2
                && Math.hypot(_next.x - x, _next.y - y) > _radius * 2

            Canvas {
                id: arc
                objectName: "caacWaypointAngleArc_" + vertex.modelData.sequence
                x: -vertex._radius * 2
                y: x
                width: vertex._radius * 4
                height: width
                onPaint: {
                    const ctx = getContext("2d")
                    ctx.clearRect(0, 0, width, height)
                    ctx.strokeStyle = palette.text
                    ctx.lineWidth = ScreenTools.defaultFontPixelWidth * 0.15
                    ctx.beginPath()
                    ctx.arc(width / 2, height / 2, vertex._radius,
                        vertex._start, vertex._start + vertex._sweep, vertex._sweep < 0)
                    ctx.stroke()
                }
            }
            on_StartChanged: arc.requestPaint()
            on_SweepChanged: arc.requestPaint()
            QGCLabel {
                objectName: "caacWaypointAngle_" + vertex.modelData.sequence
                readonly property real angle: vertex.angle
                x: Math.cos(vertex._start + vertex._sweep / 2) * vertex._radius * 2 - width / 2
                y: Math.sin(vertex._start + vertex._sweep / 2) * vertex._radius * 2 - height / 2
                text: vertex.angle.toFixed(1) + "°"
                color: palette.text
                style: Text.Outline
                styleColor: palette.windowShadeDark
                font.bold: true
                font.pointSize: ScreenTools.smallFontPointSize
            }
        }
    }

    Connections {
        target: root.mapControl
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
