pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Controls
import QtLocation

import QGroundControl
import QGroundControl.Controls

Item {
    id: root
    required property var mapControl
    property var aids: []
    property bool showAids: true
    property int _nextId: 0

    function attach(item) {
        if (item && mapControl && item.attachedMap !== mapControl) {
            if (item.attachedMap) { item.attachedMap.removeMapItem(item) }
            mapControl.addMapItem(item)
            item.attachedMap = mapControl
        }
    }
    function add(origin, radius, bearing, axes) {
        if (!origin || !origin.isValid) { return }
        const kind = axes ? "axes" : radius > 0 ? "circle" : "line"
        aids = aids.filter(aid => axes ? !aid.axes : aid.kind !== kind || aid.origin.latitude !== origin.latitude
            || aid.origin.longitude !== origin.longitude)
            .concat([{id: ++_nextId, origin: origin, radius: radius, bearing: bearing, axes: axes, kind: kind}])
    }
    function clear() { aids = [] }
    function clearAxes() { aids = aids.filter(aid => !aid.axes) }
    function remove(id) { aids = aids.filter(aid => aid.id !== id) }
    function move(id, coordinate) {
        const aid = aids.find(item => item.id === id)
        if (aid && coordinate.isValid) { aid.origin = coordinate }
    }

    Instantiator {
        model: root.aids
        delegate: Item {
            id: guide
            required property var modelData
            MapCircle {
                id: circle
                objectName: "caacAuxiliaryCircle"
                property var attachedMap: null
                center: guide.modelData.origin
                radius: guide.modelData.radius
                visible: root.showAids && radius > 0
                color: "transparent"
                border.color: QGroundControl.globalPalette.colorYellowGreen
                border.width: ScreenTools.defaultFontPixelWidth * 0.2
                z: QGroundControl.zOrderWaypointLines - 1
                Component.onCompleted: root.attach(circle)
                Component.onDestruction: { if (attachedMap) { attachedMap.removeMapItem(circle) } }
            }
            Instantiator {
                model: !guide.modelData.axes && isFinite(guide.modelData.bearing) ? [guide.modelData.bearing] : []
                delegate: MapPolyline {
                    id: line
                    objectName: "caacAuxiliaryLine"
                    required property real modelData
                    property var attachedMap: null
                    path: [guide.modelData.origin.atDistanceAndAzimuth(1000, modelData + 180),
                        guide.modelData.origin, guide.modelData.origin.atDistanceAndAzimuth(1000, modelData)]
                    visible: root.showAids
                    line.width: ScreenTools.defaultFontPixelWidth * 0.2
                    line.color: QGroundControl.globalPalette.colorYellowGreen
                    z: QGroundControl.zOrderWaypointLines - 1
                    Component.onCompleted: root.attach(line)
                    Component.onDestruction: { if (attachedMap) { attachedMap.removeMapItem(line) } }
                }
            }
            MapQuickItem {
                id: axes
                objectName: "caacCoordinateAxes"
                property var attachedMap: null
                coordinate: guide.modelData.origin
                anchorPoint: Qt.point(graphic.width / 2, graphic.height / 2)
                visible: root.showAids && guide.modelData.axes
                z: QGroundControl.zOrderWaypointIndicators + 1
                autoFadeIn: false
                sourceItem: Item {
                    id: graphic
                    objectName: "caacCoordinateAxesGraphic"
                    width: ScreenTools.defaultFontPixelHeight * 7
                    height: width
                    rotation: root.mapControl ? -root.mapControl.bearing : 0
                    Rectangle {
                        anchors.centerIn: parent
                        width: parent.width - ScreenTools.defaultFontPixelHeight * 2
                        height: ScreenTools.defaultFontPixelWidth * 0.2
                        color: QGroundControl.globalPalette.colorYellowGreen
                    }
                    Rectangle {
                        anchors.centerIn: parent
                        width: ScreenTools.defaultFontPixelWidth * 0.2
                        height: parent.height - ScreenTools.defaultFontPixelHeight * 2
                        color: QGroundControl.globalPalette.colorYellowGreen
                    }
                    Rectangle {
                        anchors.centerIn: parent
                        width: ScreenTools.defaultFontPixelWidth
                        height: width
                        radius: width / 2
                        color: QGroundControl.globalPalette.colorYellowGreen
                    }
                    Repeater {
                        model: ["N", "E", "S", "W"]
                        delegate: QGCLabel {
                            required property string modelData
                            required property int index
                            objectName: "caacAxesDirection_" + modelData
                            text: modelData
                            font.bold: true
                            color: QGroundControl.globalPalette.colorYellowGreen
                            style: Text.Outline
                            styleColor: QGroundControl.globalPalette.windowShadeDark
                            rotation: root.mapControl ? root.mapControl.bearing : 0
                            x: index === 1 ? graphic.width - width : index === 3 ? 0 : (graphic.width - width) / 2
                            y: index === 0 ? 0 : index === 2 ? graphic.height - height : (graphic.height - height) / 2
                        }
                    }
                    MouseArea {
                        id: axisMouse
                        anchors.fill: parent
                        acceptedButtons: Qt.LeftButton | Qt.RightButton
                        preventStealing: true
                        cursorShape: Qt.SizeAllCursor
                        property point _pressPoint
                        property point _originPoint
                        property bool _leftDrag: false
                        onPressed: (mouse) => {
                            _leftDrag = mouse.button === Qt.LeftButton
                            _pressPoint = mapToItem(root.mapControl, mouse.x, mouse.y)
                            _originPoint = root.mapControl.fromCoordinate(axes.coordinate, false)
                        }
                        onPositionChanged: (mouse) => {
                            if (!_leftDrag || !pressed || !root.mapControl) { return }
                            const current = mapToItem(root.mapControl, mouse.x, mouse.y)
                            const point = Qt.point(_originPoint.x + current.x - _pressPoint.x,
                                _originPoint.y + current.y - _pressPoint.y)
                            const coordinate = root.mapControl.toCoordinate(point, false)
                            if (coordinate.isValid) { axes.coordinate = coordinate }
                        }
                        onReleased: (mouse) => {
                            if (_leftDrag) { root.move(guide.modelData.id, axes.coordinate) }
                            else if (mouse.button === Qt.RightButton) {
                                const point = mapToItem(root.mapControl, mouse.x, mouse.y)
                                axisMenu.x = point.x
                                axisMenu.y = point.y
                                axisMenu.open()
                            }
                            _leftDrag = false
                        }
                        onCanceled: { _leftDrag = false; root.move(guide.modelData.id, axes.coordinate) }
                    }
                }
                Menu {
                    id: axisMenu
                    parent: root.mapControl
                    font.pointSize: ScreenTools.menuFontPointSize
                    MenuItem {
                        objectName: "caacDeleteCoordinateAxes"
                        text: qsTr("Delete coordinate axes")
                        onTriggered: root.remove(guide.modelData.id)
                    }
                }
                Component.onCompleted: { if (guide.modelData.axes) { root.attach(axes) } }
                Component.onDestruction: { if (attachedMap) { attachedMap.removeMapItem(axes) } }
            }
        }
    }
}
