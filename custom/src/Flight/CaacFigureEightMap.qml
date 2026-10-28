pragma ComponentBehavior: Bound

import QtQuick
import QtPositioning

import QGroundControl
import QGroundControl.Controls

Item {
    id: root

    required property var mapControl
    property rect viewport: Qt.rect(0, 0, width, height)

    anchors.fill: parent
    visible: !!_controller && _controller.viewActive && _referencePath.length > 1

    readonly property var _controller: QGroundControl.corePlugin.figureEight
    readonly property var _vehicle: QGroundControl.multiVehicleManager.activeVehicle
    readonly property var _fence: _controller ? _controller.fence : null
    readonly property var _referencePath: _controller ? _controller.referencePath : []
    readonly property var _recordedPath: _controller ? _controller.recordedPath : []
    readonly property var _fencePath: {
        const result = []
        if (_fence) {
            for (const coordinate of _fence.path) { result.push(coordinate) }
        }
        if (result.length > 2) { result.push(result[0]) }
        return result
    }
    readonly property bool _showReference: !!_controller && _controller.showOnMap && _referencePath.length > 1
    readonly property bool _showRecorded: !!_controller && _controller.showOnMap && _recordedPath.length > 1
    readonly property bool _showFence: !!_fence && _fence.showOnMap && _fencePath.length > 3
    readonly property var _origin: _referencePath.length ? _referencePath[0] : QtPositioning.coordinate()
    readonly property real _siteBearing: _controller && _controller.pointA.isValid && _controller.pointB.isValid
        ? QGroundControl.corePlugin.routeLegMetrics(_controller.pointA, _controller.pointB).bearing : 90
    readonly property color _grassColor: Qt.darker(Qt.tint(palette.colorYellowGreen,
        Qt.alpha(palette.colorGreen, 0.2)), 1.25)
    readonly property rect _bounds: {
        const path = _showFence ? _fencePath : _referencePath
        if (!path.length) { return Qt.rect(-1, -1, 2, 2) }
        let left = Infinity
        let right = -Infinity
        let top = Infinity
        let bottom = -Infinity
        for (const coordinate of path) {
            const point = localPoint(coordinate)
            left = Math.min(left, point.x)
            right = Math.max(right, point.x)
            top = Math.min(top, point.y)
            bottom = Math.max(bottom, point.y)
        }
        const padding = _showFence ? 0 : _controller.fenceWidth
        return Qt.rect(left - padding, top - padding, right - left + padding * 2, bottom - top + padding * 2)
    }
    readonly property real _inset: ScreenTools.defaultFontPixelHeight * 2
    readonly property real _scale: Math.max(0, Math.min(
        (field.width - _inset * 2) / Math.max(1, _bounds.width),
        (field.height - _inset * 2) / Math.max(1, _bounds.height)))

    // Use the same site-relative transform for the reference, telemetry and aircraft heading.
    function localPoint(coordinate) {
        if (!_origin.isValid || !coordinate.isValid) { return Qt.point(0, 0) }
        const metrics = QGroundControl.corePlugin.routeLegMetrics(_origin, coordinate)
        if (!isFinite(metrics.distance) || !isFinite(metrics.bearing)) { return Qt.point(0, 0) }
        const angle = (metrics.bearing - _siteBearing) * Math.PI / 180
        return Qt.point(metrics.distance * Math.cos(angle), metrics.distance * Math.sin(angle))
    }

    function project(coordinate) {
        const point = localPoint(coordinate)
        return Qt.point(field.width / 2 + (point.x - _bounds.x - _bounds.width / 2) * _scale,
            field.height / 2 + (point.y - _bounds.y - _bounds.height / 2) * _scale)
    }

    QGCPalette { id: palette; colorGroupEnabled: true }

    // Cover the map's tiles and unrelated mission items while keeping flight widgets above the field.
    Rectangle {
        objectName: "caacFigureEightFieldBackground"
        parent: root.mapControl || root
        anchors.fill: parent
        visible: root.visible
        color: root._grassColor
        z: QGroundControl.zOrderTopMost
    }

    Binding {
        target: root.mapControl
        property: "enabled"
        when: root.visible && !!root.mapControl
        value: false
        restoreMode: Binding.RestoreBindingOrValue
    }

    Item {
        id: field
        objectName: "caacFigureEightFieldViewport"
        x: root.viewport.x
        y: root.viewport.y
        width: Math.max(0, root.viewport.width)
        height: Math.max(0, root.viewport.height)
        clip: true

        component TrainingLine: Canvas {
            id: trainingLine
            property var path: []
            property real strokeWidth: ScreenTools.defaultFontPixelWidth * 0.4
            property color strokeColor: palette.colorRed
            readonly property var pixels: path.map(coordinate => root.project(coordinate))
            anchors.fill: parent
            onPixelsChanged: requestPaint()
            onStrokeWidthChanged: requestPaint()
            onStrokeColorChanged: requestPaint()
            onVisibleChanged: if (visible) { requestPaint() }
            onPaint: {
                const ctx = getContext("2d")
                ctx.reset()
                if (pixels.length < 2) { return }
                ctx.beginPath()
                ctx.moveTo(pixels[0].x, pixels[0].y)
                for (let i = 1; i < pixels.length; ++i) { ctx.lineTo(pixels[i].x, pixels[i].y) }
                ctx.strokeStyle = strokeColor
                ctx.lineWidth = strokeWidth
                ctx.lineJoin = "round"
                ctx.lineCap = "round"
                ctx.stroke()
            }
        }

        TrainingLine {
            objectName: "caacFigureEightOuterFence"
            visible: root._showFence
            path: root._fencePath
            strokeWidth: ScreenTools.defaultFontPixelWidth * 0.35
            strokeColor: root._fence && root._fence.applied ? palette.colorGreen : palette.colorBlue
        }

        TrainingLine {
            objectName: "caacFigureEightTrainingCorridor"
            visible: root._showReference
            path: root._referencePath
            strokeWidth: Math.max(ScreenTools.defaultFontPixelWidth * 0.4,
                root._controller.fenceWidth * root._scale * 2)
            strokeColor: Qt.tint(palette.colorGreen, Qt.alpha(palette.mapWidgetBorderLight, 0.65))
            opacity: 0.8
        }

        TrainingLine {
            objectName: "caacFigureEightReferencePath"
            visible: root._showReference
            path: root._referencePath
        }

        TrainingLine {
            objectName: "caacFigureEightRecordedPath"
            visible: root._showRecorded
            path: root._recordedPath
            strokeWidth: ScreenTools.defaultFontPixelWidth * 0.3
            strokeColor: palette.colorOrange
        }

        Repeater {
            // Four quarter points on each circle share the tangent point at sample zero.
            model: [0, 30, 60, 90, 150, 180, 210]
            delegate: Rectangle {
                id: marker
                required property int modelData
                required property int index
                readonly property var coordinate: root._referencePath.length > modelData
                    ? root._referencePath[modelData] : QtPositioning.coordinate()
                readonly property point position: root.project(coordinate)
                objectName: "caacFigureEightTrainingMarker" + index
                visible: root._showReference && coordinate.isValid
                x: position.x - width / 2
                y: position.y - height / 2
                width: ScreenTools.defaultFontPixelHeight * 0.85
                height: width
                radius: width / 2
                color: palette.colorGreen
                border.color: palette.mapWidgetBorderLight
                border.width: ScreenTools.defaultFontPixelWidth * 0.2
            }
        }

        Repeater {
            model: [qsTr("A筒"), qsTr("B筒")]
            delegate: Rectangle {
                id: markerLabel
                required property int index
                required property string modelData
                readonly property point position: root.project(index === 0
                    ? root._controller.pointA : root._controller.pointB)
                objectName: "caacFigureEightPointLabel" + index
                visible: root._showReference
                x: Math.max(0, Math.min(field.width - width, position.x - width / 2))
                y: Math.max(0, position.y - height - ScreenTools.defaultFontPixelHeight)
                width: markerText.implicitWidth + ScreenTools.defaultFontPixelWidth * 4
                height: markerText.implicitHeight + ScreenTools.defaultFontPixelHeight
                radius: ScreenTools.defaultBorderRadius
                color: Qt.alpha(palette.mapWidgetBorderDark, 0.78)
                QGCLabel {
                    id: markerText
                    anchors.centerIn: parent
                    text: markerLabel.modelData
                    color: palette.mapWidgetBorderLight
                    font.pointSize: ScreenTools.largeFontPointSize
                    font.bold: true
                }
            }
        }

        Image {
            objectName: "caacFigureEightAircraft"
            readonly property var coordinate: root._vehicle ? root._vehicle.coordinate : QtPositioning.coordinate()
            readonly property point position: root.project(coordinate)
            visible: !!root._vehicle && coordinate.isValid
            x: position.x - width / 2
            y: position.y - height / 2
            width: ScreenTools.defaultFontPixelHeight * 2
            height: width
            source: root._vehicle ? root._vehicle.vehicleImageOpaque : ""
            sourceSize.width: width
            fillMode: Image.PreserveAspectFit
            mipmap: true
            rotation: root._vehicle && isFinite(root._vehicle.heading.rawValue)
                ? root._vehicle.heading.rawValue - root._siteBearing + 90 : 0
        }

        QGCLabel {
            objectName: "caacFigureEightTakeoffLabel"
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.bottom: parent.bottom
            anchors.bottomMargin: ScreenTools.defaultFontPixelHeight * 0.25
            visible: root._showReference
            text: qsTr("起飞位置")
            color: palette.mapWidgetBorderLight
            font.pointSize: ScreenTools.largeFontPointSize
        }
    }
}
