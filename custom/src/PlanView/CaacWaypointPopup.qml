pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import QGroundControl
import QGroundControl.Controls

Popup {
    id: root
    objectName: "caacWaypointPopup"
    required property var editor
    property var waypoint: null
    property bool fineTuning: false
    readonly property var controller: editor.routeController
    readonly property var _leg: waypoint && controller.coordinateItems.length > 0 ? controller.waypointLeg(waypoint) : ({})
    readonly property bool _editable: _leg.distance !== undefined && waypoint && waypoint.command === 16
    readonly property point _position: {
        if (!waypoint || !editor.editorMap) { return Qt.point(0, 0) }
        void editor.editorMap.center
        void editor.editorMap.zoomLevel
        return editor.editorMap.fromCoordinate(waypoint.coordinate, false)
    }
    parent: editor.editorMap
    width: Math.min(ScreenTools.defaultFontPixelWidth * 35, parent ? parent.width : 0)
    readonly property real _gap: ScreenTools.defaultFontPixelHeight * 1.5
    readonly property int _placement: !parent ? 0
        : _position.y + _gap + height <= parent.height ? 0
        : _position.y - height - _gap >= 0 ? 1
        : _position.x + _gap + width <= parent.width ? 2 : 3
    x: parent ? Math.max(0, Math.min(_placement === 2 ? _position.x + _gap
        : _placement === 3 ? _position.x - width - _gap : _position.x - width / 2, parent.width - width)) : 0
    y: parent ? Math.max(0, Math.min(_placement === 0 ? _position.y + _gap
        : _placement === 1 ? _position.y - height - _gap : _position.y - height / 2, parent.height - height)) : 0
    padding: ScreenTools.defaultFontPixelHeight
    focus: true
    modal: false
    closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutside
    background: Rectangle {
        color: palette.window
        radius: ScreenTools.defaultBorderRadius * 2
        Canvas {
            readonly property bool side: root._placement >= 2
            readonly property int placement: root._placement
            width: ScreenTools.defaultFontPixelHeight * (side ? 1 : 2)
            height: ScreenTools.defaultFontPixelHeight * (side ? 2 : 1)
            x: side ? (placement === 2 ? -width : parent.width)
                : Math.max(parent.radius, Math.min(root._position.x - root.x - width / 2, parent.width - width - parent.radius))
            y: side ? Math.max(parent.radius, Math.min(root._position.y - root.y - height / 2, parent.height - height - parent.radius))
                : placement === 0 ? -height : parent.height
            onPlacementChanged: requestPaint()
            onWidthChanged: requestPaint()
            onHeightChanged: requestPaint()
            onPaint: {
                const ctx = getContext("2d")
                ctx.reset()
                ctx.fillStyle = palette.window
                ctx.beginPath()
                if (placement === 0) {
                    ctx.moveTo(0, height); ctx.lineTo(width / 2, 0); ctx.lineTo(width, height)
                } else if (placement === 1) {
                    ctx.moveTo(0, 0); ctx.lineTo(width / 2, height); ctx.lineTo(width, 0)
                } else if (placement === 2) {
                    ctx.moveTo(width, 0); ctx.lineTo(0, height / 2); ctx.lineTo(width, height)
                } else {
                    ctx.moveTo(0, 0); ctx.lineTo(width, height / 2); ctx.lineTo(0, height)
                }
                ctx.closePath()
                ctx.fill()
            }
        }
    }
    QGCPalette { id: palette; colorGroupEnabled: root.enabled }

    component QuickNumberField: QGCTextField {
        id: field
        font.pointSize: ScreenTools.menuFontPointSize
        color: palette.text
        horizontalAlignment: TextInput.AlignHCenter
        rightPadding: ScreenTools.defaultFontPixelWidth * 3
        background: Rectangle {
            color: palette.windowShadeDark
            radius: ScreenTools.defaultBorderRadius
            implicitHeight: ScreenTools.defaultFontPixelHeight * 3
            implicitWidth: ScreenTools.implicitTextFieldWidth
            QGCLabel {
                anchors.right: parent.right
                anchors.rightMargin: ScreenTools.defaultFontPixelWidth
                anchors.verticalCenter: parent.verticalCenter
                text: field.unitsLabel
                color: palette.text
            }
        }
    }

    function show(item) {
        if (visible) { close() }
        controller.cancelRouteOrientation()
        if (!controller.selectWaypoint(item)) { return }
        waypoint = item
        syncFields()
        open()
    }
    function syncFields() {
        bearing.text = _leg.bearing === undefined ? "0" : _leg.bearing.toFixed(2)
        distance.text = _leg.distance === undefined ? "0" : _leg.distance.toFixed(2)
    }
    function apply() {
        if (!visible || !waypoint) { return false }
        const angle = bearing.text.trim().length ? Number(bearing.text) : NaN
        const metres = distance.text.trim().length ? Number(distance.text) : NaN
        return controller.updateWaypointLeg(waypoint, angle, metres)
    }
    function nudge(key) {
        if (!fineTuning || !_editable) { return false }
        let angle = _leg.bearing === undefined ? Number(bearing.text) : _leg.bearing
        let metres = _leg.distance
        if (key === Qt.Key_Up) { angle += 0.01 }
        else if (key === Qt.Key_Down) { angle -= 0.01 }
        else if (key === Qt.Key_Right) { metres += 0.01 }
        else if (key === Qt.Key_Left) { metres = Math.max(0, metres - 0.01) }
        else { return false }
        if (controller.updateWaypointLeg(waypoint, angle, metres)) {
            syncFields()
            if (metres < 0.001) { bearing.text = ((angle % 360 + 360) % 360).toFixed(2) }
        }
        return true
    }
    onFineTuningChanged: {
        if (fineTuning) { controller.beginEdit(); keyTarget.forceActiveFocus() }
        else { controller.endEdit() }
    }
    onClosed: { fineTuning = false; waypoint = null }

    Connections {
        target: root.editor.planMasterController ? root.editor.planMasterController.missionController : null
        function onVisualItemsReset() { root.close() }
    }
    Connections {
        target: root.editor
        function onVisibleChanged() { if (!root.editor.visible) { root.close() } }
        function onEnabledChanged() { if (!root.editor.enabled) { root.close() } }
    }

    contentItem: FocusScope {
        id: keyTarget
        objectName: "caacWaypointPopupContent"
        readonly property rect popupGeometry: Qt.rect(root.x, root.y, root.width, root.height)
        implicitHeight: fields.implicitHeight
        Keys.onPressed: (event) => { if (root.nudge(event.key)) { event.accepted = true } }
        ColumnLayout {
            id: fields
            width: parent.width
            spacing: ScreenTools.defaultFontPixelHeight * 0.5
            GridLayout {
                Layout.fillWidth: true
                columns: 2
                columnSpacing: ScreenTools.defaultFontPixelWidth
                QGCLabel { text: qsTr("Bearing"); font.pointSize: ScreenTools.menuFontPointSize }
                QuickNumberField {
                    id: bearing
                    objectName: "caacWaypointQuickBearing"
                    Layout.fillWidth: true
                    enabled: root._editable
                    unitsLabel: "°"
                    inputMethodHints: Qt.ImhFormattedNumbersOnly
                    Keys.forwardTo: [keyTarget]
                    onTextEdited: root.apply()
                    onEditingFinished: { if (root.apply()) { root.syncFields() } }
                }
                QGCLabel { text: qsTr("Distance"); font.pointSize: ScreenTools.menuFontPointSize }
                QuickNumberField {
                    id: distance
                    objectName: "caacWaypointQuickDistance"
                    Layout.fillWidth: true
                    enabled: root._editable
                    unitsLabel: "m"
                    inputMethodHints: Qt.ImhFormattedNumbersOnly
                    Keys.forwardTo: [keyTarget]
                    onTextEdited: root.apply()
                    onEditingFinished: { if (root.apply()) { root.syncFields() } }
                }
                QGCLabel { text: qsTr("Fine tuning"); font.pointSize: ScreenTools.menuFontPointSize }
                QGCButton {
                    objectName: "caacWaypointFineTuning"
                    pointSize: ScreenTools.menuFontPointSize
                    Layout.fillWidth: true
                    enabled: root._editable
                    text: root.fineTuning ? qsTr("On") : qsTr("Off")
                    onClicked: root.fineTuning = !root.fineTuning
                }
            }
            QGCLabel {
                Layout.fillWidth: true
                opacity: root.fineTuning ? 1 : 0
                text: qsTr("Up/down: bearing ±0.01°. Left/right: distance ±0.01 m.")
                wrapMode: Text.WordWrap
                font.pointSize: ScreenTools.smallFontPointSize
            }
            Rectangle { Layout.fillWidth: true; height: 1; color: palette.text; opacity: 0.25 }
            QGCButton {
                objectName: "caacWaypointQuickConfirm"
                pointSize: ScreenTools.menuFontPointSize
                Layout.fillWidth: true
                enabled: root._editable
                text: qsTr("Confirm")
                onClicked: {
                    if (root.apply()) { root.close() }
                }
            }
            QGCButton {
                objectName: "caacWaypointQuickDelete"
                pointSize: ScreenTools.menuFontPointSize
                textColor: palette.colorRed
                Layout.fillWidth: true
                enabled: root._editable
                text: qsTr("Delete")
                onClicked: {
                    root.fineTuning = false
                    if (root.controller.selectWaypoint(root.waypoint) && root.controller.removeCurrentWaypoint()) { root.close() }
                }
            }
            QGCLabel {
                Layout.fillWidth: true
                visible: text.length > 0
                text: root.controller.lastError
                color: palette.warningText
                wrapMode: Text.WordWrap
            }
        }
    }
}
