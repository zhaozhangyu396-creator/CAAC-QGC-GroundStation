pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Layouts

import QGroundControl
import QGroundControl.Controls

ColumnLayout {
    id: root
    required property var routeController
    required property var editor
    required property real takeoffHeight
    required property real horizontalSpeed
    readonly property var _settings: routeController.simulationSettings
    property bool _previewReady: false
    property bool draftDirty: false
    readonly property var _previewValues: {
        const settings = Object.assign({}, _settings)
        settings.closed = closed.checked
        settings.loopMode = loop.currentIndex
        const count = number(loopCount.text)
        settings.loopCount = isFinite(count) && count >= 1 && Math.floor(count) === count ? count : _settings.loopCount
        settings.startSequence = start.currentIndex >= 0 ? _points[start.currentIndex].sequence : _settings.startSequence
        settings.endBehavior = ending.currentIndex
        settings.returnSequence = returning.currentIndex >= 0 ? _points[returning.currentIndex].sequence : _settings.returnSequence
        return settings
    }
    function updatePreview() {
        if (_previewReady) { editor.simulationPreviewSettings = visible ? _previewValues : null }
    }
    on_PreviewValuesChanged: updatePreview()
    onVisibleChanged: updatePreview()
    Component.onCompleted: { _previewReady = true; updatePreview() }
    Component.onDestruction: editor.simulationPreviewSettings = null
    readonly property var _items: editor.planMasterController ? editor.planMasterController.missionController.visualItems : null
    readonly property var _points: {
        const points = [{label: qsTr("Takeoff point"), sequence: 0}]
        if (_items) {
            for (let i = 1; i < _items.count; ++i) {
                const item = _items.get(i)
                if (item.isSimpleItem && item.command === 16) {
                    points.push({label: qsTr("Waypoint %1").arg(item.waypointNumber), sequence: item.sequenceNumber})
                }
            }
        }
        return points
    }
    function pointIndex(sequence) { return _points.findIndex(point => point.sequence === sequence) }
    function number(value) { return value.trim().length ? Number(value) : NaN }
    function values() {
        return {horizontalSpeed: horizontalSpeed, verticalSpeed: number(vertical.text),
            yawRate: number(yawRate.text), windSpeed: _settings.windSpeed, windBearing: _settings.windBearing,
            turnMode: turn.currentIndex, climbMode: climb.currentIndex, holdSeconds: (turn.currentIndex === 0 || turn.currentIndex === 3) ? number(hold.text) : _settings.holdSeconds,
            specifyYaw: yawMode.currentIndex === 1, yaw: yawMode.currentIndex === 1 ? number(yaw.text) : _settings.yaw, closed: closed.checked,
            loopMode: loop.currentIndex, loopCount: loop.currentIndex === 1 ? number(loopCount.text) : _settings.loopCount, endBehavior: ending.currentIndex,
            startSequence: start.currentIndex >= 0 ? _points[start.currentIndex].sequence : NaN,
            returnSequence: ending.currentIndex === 0 ? _settings.returnSequence
                : returning.currentIndex >= 0 ? _points[returning.currentIndex].sequence : NaN,
            takeoffHeight: takeoffHeight}
    }
    component NumberField: QGCTextField {
        Layout.fillWidth: true
        Layout.minimumWidth: 0
        validator: DoubleValidator { locale: "C" }
        onTextEdited: root.draftDirty = true
    }
    spacing: ScreenTools.defaultFontPixelHeight * 0.5
    CaacSettingsSection {
        objectName: "caacSimTurnSection"
        title: qsTr("Turn and heading")
        GridLayout {
            columns: root.width < ScreenTools.defaultFontPixelWidth * 35 ? 1 : 2
            Layout.fillWidth: true
            QGCLabel { text: qsTr("Turn mode") }
            QGCComboBox {
                id: turn
                objectName: "caacSimTurnMode"
                Layout.fillWidth: true
                model: [qsTr("Point turn"), qsTr("Coordinated turn"), qsTr("Adaptive coordinated turn"), qsTr("No turn")]
                currentIndex: root._settings.turnMode; onActivated: root.draftDirty = true
            }
            QGCLabel { text: qsTr("Hold duration (s)"); visible: turn.currentIndex === 0 || turn.currentIndex === 3 }
            NumberField {
                id: hold; objectName: "caacSimHoldDuration"; text: root._settings.holdSeconds.toString()
                visible: turn.currentIndex === 0 || turn.currentIndex === 3
            }
            QGCLabel { text: qsTr("Hold heading"); visible: hold.visible }
            QGCComboBox {
                id: yawMode; objectName: "caacSimSpecifyYaw"; Layout.fillWidth: true
                model: [qsTr("Unspecified"), qsTr("Specified")]
                currentIndex: root._settings.specifyYaw ? 1 : 0; onActivated: root.draftDirty = true
                visible: hold.visible
            }
            QGCLabel { text: qsTr("Specified heading (degrees)"); visible: yawMode.visible && yawMode.currentIndex === 1 }
            NumberField {
                id: yaw; objectName: "caacSimYaw"; text: root._settings.yaw.toString()
                visible: yawMode.visible && yawMode.currentIndex === 1
            }
            QGCLabel { text: qsTr("Yaw rate (degrees/s)") }
            NumberField { id: yawRate; objectName: "caacSimYawRate"; text: root._settings.yawRate.toString() }
        }
    }
    CaacSettingsSection {
        objectName: "caacSimClimbSection"
        title: qsTr("Climb settings")
        GridLayout {
            columns: root.width < ScreenTools.defaultFontPixelWidth * 35 ? 1 : 2
            Layout.fillWidth: true
            QGCLabel { text: qsTr("Vertical speed (m/s)") }
            NumberField { id: vertical; objectName: "caacSimVerticalSpeed"; text: root._settings.verticalSpeed.toString() }
            QGCLabel { text: qsTr("Climb mode") }
            QGCComboBox {
                id: climb; objectName: "caacSimClimbMode"; Layout.fillWidth: true
                model: [qsTr("Point climb"), qsTr("Coordinated climb")]
                currentIndex: root._settings.climbMode; onActivated: root.draftDirty = true
            }
        }
    }
    CaacSettingsSection {
        objectName: "caacSimLoopsSection"
        title: qsTr("Closed route and loops")
        QGCCheckBox { id: closed; objectName: "caacSimClosedRoute"; text: qsTr("Close route"); checked: root._settings.closed; onClicked: root.draftDirty = true }
        GridLayout {
            columns: root.width < ScreenTools.defaultFontPixelWidth * 35 ? 1 : 2
            Layout.fillWidth: true
            QGCLabel { text: qsTr("Loop behavior") }
            QGCComboBox {
                id: loop; objectName: "caacSimLoopMode"; Layout.fillWidth: true
                model: [qsTr("No loop"), qsTr("Specified count"), qsTr("Loop forever")]
                currentIndex: root._settings.loopMode; onActivated: root.draftDirty = true
            }
            QGCLabel { text: qsTr("Total route executions"); visible: loop.currentIndex === 1 }
            NumberField {
                id: loopCount; objectName: "caacSimLoopCount"; text: root._settings.loopCount.toString()
                visible: loop.currentIndex === 1
            }
            QGCLabel { text: qsTr("Start point") }
            QGCComboBox {
                id: start; objectName: "caacSimStartPoint"; Layout.fillWidth: true
                model: root._points; textRole: "label"; currentIndex: root.pointIndex(root._settings.startSequence); onActivated: root.draftDirty = true
            }
            QGCLabel { text: qsTr("End behavior") }
            QGCComboBox {
                id: ending; objectName: "caacSimEndBehavior"; Layout.fillWidth: true
                model: [qsTr("Hover in place"), qsTr("Return")]; currentIndex: root._settings.endBehavior; onActivated: root.draftDirty = true
            }
            QGCLabel { text: qsTr("Return point"); visible: ending.currentIndex === 1 }
            QGCComboBox {
                id: returning; objectName: "caacSimReturnPoint"; Layout.fillWidth: true
                visible: ending.currentIndex === 1
                model: root._points; textRole: "label"; currentIndex: root.pointIndex(root._settings.returnSequence); onActivated: root.draftDirty = true
            }
        }
        QGCLabel {
            Layout.fillWidth: true; wrapMode: Text.WordWrap
            text: qsTr("Count 1 executes once. Takeoff runs once; loops begin at the selected waypoint. Returning to a waypoint ends in a hover; returning home lands in this simulation.")
        }
    }
}
