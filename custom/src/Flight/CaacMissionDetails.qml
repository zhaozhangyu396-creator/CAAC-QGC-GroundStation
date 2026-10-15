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
    readonly property var _simulationDetails: {
        const simulation = QGroundControl.corePlugin.simulation
        if (!vehicle || !simulation.active || simulation.vehicle !== vehicle) { return ({}) }
        const details = simulation.currentMissionDetails
        return details.sequence === root._sequence ? details : ({})
    }
    readonly property real _speed: {
        if (isFinite(_simulationDetails.speed)) { return _simulationDetails.speed }
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
    readonly property int _remainingLoops: {
        if (!vehicle || !missionController) { return -2 }
        const simulation = QGroundControl.corePlugin.simulation
        return simulation.active && simulation.vehicle === vehicle
            ? simulation.remainingLoops : missionController.remainingMissionLoops
    }
    readonly property string _targetNumber: {
        if (!targetItem) { return _unavailable }
        const simulation = QGroundControl.corePlugin.simulation
        void simulation.waypointNumberRevision
        return simulation.waypointNumber(targetItem.sequenceNumber, targetItem.coordinate).toString()
    }
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
                    { key: "Target", label: qsTr("Target waypoint"), value: root._targetNumber },
                    { key: "Speed", label: qsTr("Waypoint speed"), value: isFinite(root._speed) ? root._speed.toFixed(1) + " m/s" : root._unavailable },
                    { key: "Altitude", label: qsTr("Target altitude"), value: root.targetItem && root.targetItem.isSimpleItem && root.targetItem.specifiesAltitude ? root.targetItem.altitude.rawValue.toFixed(1) + " m" : root._unavailable },
                    { key: "Heading", label: qsTr("Route heading"), value: root.targetItem && isFinite(root.targetItem.missionVehicleYaw) ? root.targetItem.missionVehicleYaw.toFixed(1) + "°" : root._unavailable },
                    { key: "RemainingLoops", label: qsTr("剩余圈数"), value: root._remainingLoops === -1 ? qsTr("无限循环") : root._remainingLoops >= 0 ? root._remainingLoops.toString() : root._unavailable },
                    { key: "Climb", label: qsTr("Climb mode"), value: root._simulationDetails.climbMode === 0 ? qsTr("Change altitude before the leg")
                        : root._simulationDetails.climbMode === 1 ? qsTr("Change altitude along the leg") : root._unavailable }
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
            text: qsTr("Mission parameters; simulation speed and climb mode use the uploaded configuration. Altitude uses its mission frame. Unreported aircraft fields are unavailable.")
            font.pointSize: ScreenTools.smallFontPointSize
            wrapMode: Text.WordWrap
        }
    }
}
