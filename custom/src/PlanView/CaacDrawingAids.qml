pragma ComponentBehavior: Bound

import QtQuick
import QtLocation

import QGroundControl
import QGroundControl.Controls

Item {
    id: root
    required property var mapControl
    property var aids: []
    property bool showAids: true

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
        aids = aids.filter(aid => aid.kind !== kind || aid.origin.latitude !== origin.latitude
            || aid.origin.longitude !== origin.longitude)
            .concat([{origin: origin, radius: radius, bearing: bearing, axes: axes, kind: kind}])
    }
    function clear() { aids = [] }

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
                model: guide.modelData.axes ? [0, 90] : isFinite(guide.modelData.bearing) ? [guide.modelData.bearing] : []
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
        }
    }
}
