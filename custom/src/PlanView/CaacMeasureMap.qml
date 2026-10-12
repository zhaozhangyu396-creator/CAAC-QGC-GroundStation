pragma ComponentBehavior: Bound

import QtQuick
import QtLocation

import QGroundControl
import QGroundControl.Controls

Item {
    id: root
    required property var mapControl
    required property var pointA
    required property var pointB
    property bool showMeasurement: true

    function attach(item) {
        if (item && mapControl && item.attachedMap !== mapControl) {
            if (item.attachedMap) { item.attachedMap.removeMapItem(item) }
            mapControl.addMapItem(item)
            item.attachedMap = mapControl
        }
    }
    onMapControlChanged: {
        attach(measureLine)
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
        model: [{point: root.pointA, label: "A"}, {point: root.pointB, label: "B"}].filter(p => p.point.isValid)
        delegate: MapQuickItem {
            id: marker
            required property var modelData
            property var attachedMap: null
            coordinate: modelData.point
            visible: root.showMeasurement
            z: QGroundControl.zOrderMapItems + 2
            anchorPoint: Qt.point(symbol.width / 2, symbol.height / 2)
            sourceItem: Rectangle {
                id: symbol
                width: ScreenTools.defaultFontPixelHeight * 1.8
                height: width
                radius: width / 2
                color: QGroundControl.globalPalette.window
                border.color: QGroundControl.globalPalette.colorYellowGreen
                border.width: ScreenTools.defaultFontPixelWidth * 0.2
                QGCLabel {
                    anchors.centerIn: parent
                    text: marker.modelData.label
                    font.bold: true
                }
            }
            Component.onCompleted: root.attach(marker)
            Component.onDestruction: { if (attachedMap) { attachedMap.removeMapItem(marker) } }
        }
    }
}
