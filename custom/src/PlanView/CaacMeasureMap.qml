pragma ComponentBehavior: Bound

import QtQuick
import QtLocation
import QtPositioning

import QGroundControl
import QGroundControl.Controls

Item {
    id: root
    required property var mapControl
    required property var pointA
    required property var pointB
    property bool showMeasurement: true
    readonly property var _metrics: QGroundControl.corePlugin.routeLegMetrics(pointA, pointB)
    signal pointMoved(string label, var coordinate)
    signal settingsRequested()

    function attach(item) {
        if (item && mapControl && item.attachedMap !== mapControl) {
            if (item.attachedMap) { item.attachedMap.removeMapItem(item) }
            mapControl.addMapItem(item)
            item.attachedMap = mapControl
        }
    }
    onMapControlChanged: {
        attach(measureLine)
        attach(measureValues)
        for (let i = 0; markers && i < markers.count; ++i) {
            const item = markers.objectAt(i)
            if (item) { attach(item) }
        }
    }

    MapPolyline {
        id: measureLine
        property var attachedMap: null
        visible: root.showMeasurement && root.pointA.isValid && root.pointB.isValid
        path: visible ? [root.pointA, root.pointB] : []
        line.width: ScreenTools.defaultFontPixelWidth * 0.4
        line.color: QGroundControl.globalPalette.colorYellowGreen
        z: QGroundControl.zOrderWaypointLines + 3
        Component.onCompleted: root.attach(measureLine)
        Component.onDestruction: { if (attachedMap) { attachedMap.removeMapItem(measureLine) } }
    }
    Instantiator {
        id: markers
        model: ["A", "B"]
        delegate: MapQuickItem {
            id: marker
            required property var modelData
            property var attachedMap: null
            coordinate: modelData === "A" ? root.pointA : root.pointB
            visible: root.showMeasurement && coordinate.isValid
            z: QGroundControl.zOrderMapItems + 4
            anchorPoint: Qt.point(symbol.width / 2, symbol.height / 2)
            sourceItem: Rectangle {
                id: symbol
                objectName: "caacMeasureMarker" + marker.modelData
                width: ScreenTools.defaultFontPixelHeight * 1.8
                height: width
                radius: width / 2
                color: QGroundControl.globalPalette.window
                border.color: QGroundControl.globalPalette.colorYellowGreen
                border.width: ScreenTools.defaultFontPixelWidth * 0.2
                QGCLabel {
                    anchors.centerIn: parent
                    text: marker.modelData
                    font.bold: true
                }
                MouseArea {
                    id: dragArea
                    objectName: "caacMeasureDrag" + marker.modelData
                    anchors.fill: parent
                    acceptedButtons: Qt.LeftButton | Qt.RightButton
                    preventStealing: true
                    property bool dragging: false
                    onPressed: (mouse) => { dragging = mouse.button === Qt.LeftButton }
                    onPressAndHold: dragging = !!(pressedButtons & Qt.LeftButton)
                    onPositionChanged: (mouse) => {
                        if (!dragging || !(pressedButtons & Qt.LeftButton) || !root.mapControl) { return }
                        const position = mapToItem(root.mapControl, mouse.x, mouse.y)
                        const coordinate = root.mapControl.toCoordinate(position, false)
                        if (coordinate.isValid) { root.pointMoved(marker.modelData, coordinate) }
                    }
                    onReleased: dragging = false
                    onCanceled: dragging = false
                    onClicked: (mouse) => { if (mouse.button === Qt.RightButton) { root.settingsRequested() } }
                }
            }
            Component.onCompleted: root.attach(marker)
            Component.onDestruction: { if (attachedMap) { attachedMap.removeMapItem(marker) } }
        }
    }
    MapQuickItem {
        id: measureValues
        property var attachedMap: null
        visible: root.showMeasurement && root.pointA.isValid && root.pointB.isValid
        coordinate: visible ? QtPositioning.coordinate((root.pointA.latitude + root.pointB.latitude) / 2,
            ((root.pointA.longitude + (((root.pointB.longitude - root.pointA.longitude + 540) % 360) - 180) / 2 + 540) % 360) - 180) : QtPositioning.coordinate()
        anchorPoint: Qt.point(values.width / 2, values.height + ScreenTools.defaultFontPixelHeight)
        z: QGroundControl.zOrderMapItems + 3
        sourceItem: Rectangle {
            id: values
            color: QGroundControl.globalPalette.window
            radius: ScreenTools.defaultBorderRadius
            width: Math.min(valueText.implicitWidth + ScreenTools.defaultFontPixelWidth * 2,
                root.mapControl ? root.mapControl.width : 0)
            height: valueText.implicitHeight + ScreenTools.defaultFontPixelHeight
            QGCLabel {
                id: valueText
                objectName: "caacMeasureMapValues"
                anchors.centerIn: parent
                width: parent.width - ScreenTools.defaultFontPixelWidth * 2
                wrapMode: Text.WordWrap
                text: qsTr("A → B: %1 m · %2°")
                    .arg(root._metrics.distance !== undefined ? root._metrics.distance.toFixed(2) : "0.00")
                    .arg(root._metrics.bearing !== undefined ? root._metrics.bearing.toFixed(2) : "—")
            }
        }
        Component.onCompleted: root.attach(measureValues)
        Component.onDestruction: { if (attachedMap) { attachedMap.removeMapItem(measureValues) } }
    }
}
