pragma ComponentBehavior: Bound

import QtQuick

import QGroundControl
import QGroundControl.Controls

Item {
    id: root
    required property var mapControl
    property var segments: null
    property int _projectionRevision: 0

    Repeater {
        model: root.segments
        delegate: Item {
            id: label
            required property var object
            required property int index
            objectName: "caacRouteLegLabel_" + index
            readonly property var metrics: QGroundControl.corePlugin.routeLegMetrics(object.coordinate1, object.coordinate2)
            readonly property point _from: {
                root._projectionRevision
                return root.mapControl.fromCoordinate(object.coordinate1, false)
            }
            readonly property point _to: {
                root._projectionRevision
                return root.mapControl.fromCoordinate(object.coordinate2, false)
            }
            readonly property point _midpoint: {
                root._projectionRevision
                return metrics.midpoint !== undefined ? root.mapControl.fromCoordinate(metrics.midpoint, false) : Qt.point(-1, -1)
            }
            readonly property real _angle: Math.atan2(_to.y - _from.y, _to.x - _from.x) * 180 / Math.PI
            readonly property string legText: metrics.distance !== undefined
                ? qsTr("%1 m  %2°").arg(metrics.distance.toFixed(1)).arg((Math.round(metrics.bearing * 10) / 10 % 360).toFixed(1)) : ""
            x: _midpoint.x
            y: _midpoint.y
            visible: metrics.distance !== undefined && isFinite(_midpoint.x) && isFinite(_midpoint.y)
                && _midpoint.x >= 0 && _midpoint.y >= 0 && _midpoint.x <= root.mapControl.width && _midpoint.y <= root.mapControl.height
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
    }

    Connections {
        target: root.mapControl
        function onCenterChanged() { ++root._projectionRevision }
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
