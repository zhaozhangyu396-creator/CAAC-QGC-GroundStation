pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Layouts

import QGroundControl
import QGroundControl.Controls

Rectangle {
    id: root
    property var missionController: null
    property var vehicle: null
    signal titleDragged(point delta)
    readonly property var _items: missionController ? missionController.visualItems : null
    readonly property int _sequence: vehicle && missionController ? missionController.currentMissionIndex : -1
    readonly property var targetItem: {
        if (!_items || _sequence < 0) {
            return null
        }
        for (let i = 1; i < _items.count; ++i) {
            const item = _items.get(i)
            if (item.sequenceNumber <= _sequence && _sequence <= item.lastSequenceNumber) {
                return item
            }
        }
        return null
    }
    readonly property real _speed: {
        let speed = NaN
        if (_items && targetItem) {
            for (let i = 0; i < _items.count; ++i) {
                const item = _items.get(i)
                if (item.sequenceNumber > targetItem.sequenceNumber) {
                    break
                }
                if (isFinite(item.specifiedFlightSpeed)) {
                    speed = item.specifiedFlightSpeed
                }
            }
        }
        return speed
    }
    readonly property string _unavailable: qsTr("Unavailable")
    implicitHeight: contents.implicitHeight + ScreenTools.defaultFontPixelHeight
    color: palette.windowShadeDark
    radius: ScreenTools.defaultBorderRadius
    QGCPalette { id: palette; colorGroupEnabled: root.enabled }

    ColumnLayout {
        id: contents
        width: parent.width
        spacing: ScreenTools.defaultFontPixelHeight
        Rectangle {
            Layout.fillWidth: true
            implicitHeight: ScreenTools.defaultFontPixelHeight * 2.5
            color: palette.windowShade
            CaacPanelDragHandle {
                objectName: "caacMissionDetailsDragHandle"
                anchors.fill: parent
                onMoved: (delta) => root.titleDragged(delta)
            }
            QGCLabel {
                anchors.centerIn: parent
                text: qsTr("Mission details")
                font.bold: true
                font.pointSize: ScreenTools.largeFontPointSize
            }
        }
        GridLayout {
            Layout.fillWidth: true
            Layout.leftMargin: ScreenTools.defaultFontPixelWidth
            Layout.rightMargin: ScreenTools.defaultFontPixelWidth
            columns: 3
            rowSpacing: ScreenTools.defaultFontPixelHeight
            columnSpacing: ScreenTools.defaultFontPixelWidth
            Repeater {
                model: [
                    { key: "Target", label: qsTr("Target waypoint"), value: root.targetItem ? root.targetItem.sequenceNumber.toString() : root._unavailable },
                    { key: "Speed", label: qsTr("Waypoint speed"), value: isFinite(root._speed) ? root._speed.toFixed(1) + " m/s" : root._unavailable },
                    { key: "Altitude", label: qsTr("Target altitude"), value: root.targetItem && root.targetItem.isSimpleItem && root.targetItem.specifiesAltitude ? root.targetItem.altitude.rawValue.toFixed(1) + " m" : root._unavailable },
                    { key: "Heading", label: qsTr("Route heading"), value: root.targetItem && isFinite(root.targetItem.missionVehicleYaw) ? root.targetItem.missionVehicleYaw.toFixed(1) + "°" : root._unavailable },
                    { key: "StayHeading", label: qsTr("Stay heading"), value: root._unavailable },
                    { key: "Climb", label: qsTr("Climb mode"), value: root._unavailable }
                ]
                ColumnLayout {
                    required property var modelData
                    Layout.fillWidth: true
                    QGCLabel {
                        Layout.fillWidth: true
                        text: modelData.label
                        horizontalAlignment: Text.AlignHCenter
                        wrapMode: Text.WordWrap
                        font.pointSize: ScreenTools.smallFontPointSize
                    }
                    QGCLabel {
                        objectName: "caacMission" + modelData.key
                        Layout.fillWidth: true
                        text: modelData.value
                        horizontalAlignment: Text.AlignHCenter
                        wrapMode: Text.WordWrap
                        font.bold: true
                    }
                }
            }
        }
        QGCLabel {
            Layout.fillWidth: true
            Layout.leftMargin: ScreenTools.defaultFontPixelWidth
            Layout.rightMargin: ScreenTools.defaultFontPixelWidth
            text: qsTr("Downloaded mission parameters; altitude uses its mission frame. Unreported fields are unavailable.")
            font.pointSize: ScreenTools.smallFontPointSize
            wrapMode: Text.WordWrap
        }
    }
}
