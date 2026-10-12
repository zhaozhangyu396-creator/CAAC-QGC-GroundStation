pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtPositioning

import QGC
import QGroundControl
import QGroundControl.Controls
import CAAC.Plan

Column {
    id: root
    objectName: "caacRouteEditor"
    spacing: ScreenTools.defaultFontPixelHeight * 0.25

    property var editorMap: null
    property var planMasterController: null
    property bool showMissionTree: false
    property int activePanel: 3
    readonly property bool relativeEditorVisible: activePanel === 1
    property bool routeDragMode: false
    property point _panelDragOffset: Qt.point(0, 0)
    transform: Translate { x: root._panelDragOffset.x; y: root._panelDragOffset.y }
    onActivePanelChanged: showMissionTree = false
    readonly property var currentItem: editorController.currentItem
    readonly property var routeController: editorController
    property bool _pickingCenter: false
    property int measurePick: 0
    property var measureA: QtPositioning.coordinate()
    property var measureB: QtPositioning.coordinate()
    onVisibleChanged: { if (!visible) { measurePick = 0 } }
    onEnabledChanged: { if (!enabled) { measurePick = 0 } }
    property int _pendingOperation: 0
    property var _draggedWaypoint: null
    readonly property var _dragLeg: _draggedWaypoint && editorController.coordinateItems.length > 0
        ? editorController.waypointLeg(_draggedWaypoint) : ({})

    QGCPalette { id: dragPalette; colorGroupEnabled: root.enabled }

    Rectangle {
        parent: root.parent
        x: root.x
        y: root.y
        width: root.width
        height: root.height
        transform: Translate { x: root._panelDragOffset.x; y: root._panelDragOffset.y }
        z: -1
        visible: root.enabled && root.visible && !root.showMissionTree
        color: dragPalette.window
        opacity: 0.85
        DeadMouseArea { anchors.fill: parent }
    }

    function movePanel(delta) {
        if (!editorMap) {
            return
        }
        const position = mapToItem(editorMap, 0, 0)
        const x = Math.max(0, Math.min(position.x + delta.x, editorMap.width - width))
        const y = Math.max(0, Math.min(position.y + delta.y, editorMap.height - panelNavigation.height - listHeader.height))
        _panelDragOffset = Qt.point(_panelDragOffset.x + x - position.x, _panelDragOffset.y + y - position.y)
    }

    CaacRouteLegLabels {
        parent: root.editorMap
        anchors.fill: parent
        visible: root.enabled && root.visible
        mapControl: root.editorMap
        segments: root.planMasterController ? root.planMasterController.missionController.simpleFlightPathSegments : null
    }

    CaacMeasureMap {
        mapControl: root.editorMap
        pointA: root.measureA
        pointB: root.measureB
        showMeasurement: root.enabled && root.visible
    }

    Rectangle {
        parent: root.editorMap
        objectName: "caacWaypointDragFeedback"
        visible: root.enabled && root.visible && root._draggedWaypoint !== null
        z: QGroundControl.zOrderMapItems + 2
        color: dragPalette.window
        border.color: dragPalette.text
        radius: ScreenTools.defaultFontPixelWidth * 0.5
        width: dragFeedback.implicitWidth + ScreenTools.defaultFontPixelWidth * 2
        height: dragFeedback.implicitHeight + ScreenTools.defaultFontPixelHeight
        readonly property point _position: root.editorMap && root._draggedWaypoint
            ? root.editorMap.fromCoordinate(root._draggedWaypoint.coordinate, false) : Qt.point(0, 0)
        x: root.editorMap ? Math.max(0, Math.min(_position.x + ScreenTools.defaultFontPixelWidth * 2,
            root.editorMap.width - width)) : 0
        y: root.editorMap ? Math.max(0, Math.min(_position.y - height - ScreenTools.defaultFontPixelHeight,
            root.editorMap.height - height)) : 0

        QGCLabel {
            id: dragFeedback
            objectName: "caacWaypointDragValues"
            anchors.centerIn: parent
            text: !root._draggedWaypoint ? ""
                : root._dragLeg.distance === undefined ? qsTr("No valid previous waypoint")
                : qsTr("Waypoint %1 → %2 · Distance: %3 m").arg(root._dragLeg.previousNumber)
                    .arg(root._draggedWaypoint.sequenceNumber).arg(root._dragLeg.distance.toFixed(2))
                    + "\n" + (root._dragLeg.angle !== undefined
                        ? qsTr("Interior angle: %1°").arg(root._dragLeg.angle.toFixed(2))
                        : qsTr("Interior angle: unavailable (no incoming leg)"))
                    + "\n" + (root._dragLeg.bearing !== undefined
                        ? qsTr("Bearing: %1°").arg(root._dragLeg.bearing.toFixed(2))
                        : qsTr("Bearing: unavailable (coincident points)"))
        }
    }

    function pickRotationCenter(operation) {
        measurePick = 0
        _pendingOperation = operation
        _pickingCenter = true
    }

    function openGeometry(operation) {
        geometryDialogFactory.open({operation: operation})
    }

    function openMeasurement() { measureDialogFactory.open() }
    function openUnitConverter() { unitDialogFactory.open() }
    function pickMeasurement(point) {
        _pickingCenter = false
        measurePick = point
    }
    function clearMeasurement() {
        measurePick = 0
        measureA = QtPositioning.coordinate()
        measureB = QtPositioning.coordinate()
    }

    function editWaypoint(item) {
        if (editorController.selectWaypoint(item)) {
            waypointDialogFactory.open({ waypoint: item })
        }
    }

    CaacRouteEditorController {
        id: editorController
        objectName: "caacRouteEditorController"
        planMasterController: root.planMasterController
    }

    MouseArea {
        id: mapMouse
        objectName: "caacMapMouseArea"
        parent: root.editorMap
        anchors.fill: parent
        enabled: root.enabled && root.visible && root.editorMap && root.editorMap.directMissionItemEditing
        acceptedButtons: Qt.LeftButton
        z: root.routeDragMode || root.measurePick > 0 ? QGroundControl.zOrderMapItems + 3 : 0

        property point _pressPosition
        property point _lastPosition
        property bool _dragged: false
        property bool _routeDragging: false
        property var _lastCoordinate
        readonly property var _styleHints: Qt.styleHints

        onPressed: (mouse) => {
            _pressPosition = Qt.point(mouse.x, mouse.y)
            _lastPosition = _pressPosition
            _dragged = false
            _routeDragging = false
            _lastCoordinate = root.editorMap.toCoordinate(_pressPosition, false)
        }
        onPositionChanged: (mouse) => {
            if (!pressed) {
                return
            }
            if (!_dragged && Math.hypot(mouse.x - _pressPosition.x, mouse.y - _pressPosition.y)
                >= _styleHints.startDragDistance) {
                _dragged = true
                if (root.routeDragMode && root.measurePick === 0) {
                    _routeDragging = true
                    editorController.beginEdit()
                } else {
                    root.editorMap.mapPanStart()
                }
            }
            if (_dragged) {
                if (_routeDragging) {
                    const coordinate = root.editorMap.toCoordinate(Qt.point(mouse.x, mouse.y), false)
                    if (editorController.translateRoute(_lastCoordinate, coordinate)) {
                        _lastCoordinate = coordinate
                    }
                } else {
                    root.editorMap.pan(_lastPosition.x - mouse.x, _lastPosition.y - mouse.y)
                }
                _lastPosition = Qt.point(mouse.x, mouse.y)
            }
        }
        onReleased: {
            if (_dragged) {
                if (_routeDragging) {
                    editorController.endEdit()
                    _routeDragging = false
                } else {
                    root.editorMap.mapPanStop()
                }
            }
        }
        onCanceled: {
            if (_dragged) {
                if (_routeDragging) {
                    editorController.endEdit()
                    _routeDragging = false
                } else {
                    root.editorMap.mapPanStop()
                }
            }
            _dragged = false
        }
        onClicked: (mouse) => {
            if (!_dragged) {
                root.editorMap.mapClicked(Qt.point(mouse.x, mouse.y))
            }
        }
        onDoubleClicked: (mouse) => {
            if (!_dragged) {
                if (root.measurePick > 0) {
                    root.editorMap.mapClicked(Qt.point(mouse.x, mouse.y))
                } else {
                    root.editorMap.mapDoubleClicked(Qt.point(mouse.x, mouse.y))
                }
            }
        }
    }

    Rectangle {
        parent: root.editorMap
        visible: root.enabled && root.visible
        z: QGroundControl.zOrderWidgets
        x: (root.editorMap ? root.editorMap._leftToolWidth : 0) + ScreenTools.defaultFontPixelWidth
        y: ScreenTools.defaultFontPixelHeight * 3
        width: root.editorMap ? Math.max(0, Math.min(toolRow.implicitWidth,
            root.editorMap.width - root.width - x - ScreenTools.defaultFontPixelWidth)) : 0
        height: toolRow.implicitHeight
        color: dragPalette.window
        radius: ScreenTools.defaultBorderRadius

        RowLayout {
            id: toolRow
            anchors.fill: parent
            spacing: ScreenTools.defaultFontPixelWidth * 0.5

            QGCButton {
                objectName: "caacUndo"
                pointSize: ScreenTools.menuFontPointSize
                Layout.fillWidth: true
                text: qsTr("Undo")
                heightFactor: 0.25
                leftPadding: ScreenTools.defaultFontPixelWidth
                rightPadding: leftPadding
                enabled: editorController.canUndo
                onClicked: editorController.undo()
            }
            QGCButton {
                objectName: "caacRedo"
                pointSize: ScreenTools.menuFontPointSize
                Layout.fillWidth: true
                text: qsTr("Redo")
                heightFactor: 0.25
                leftPadding: ScreenTools.defaultFontPixelWidth
                rightPadding: leftPadding
                enabled: editorController.canRedo
                onClicked: editorController.redo()
            }
            QGCButton {
                objectName: "caacRotateRoute"
                pointSize: ScreenTools.menuFontPointSize
                Layout.fillWidth: true
                text: qsTr("Rotate / align")
                heightFactor: 0.25
                leftPadding: ScreenTools.defaultFontPixelWidth
                rightPadding: leftPadding
                onClicked: root.openGeometry(0)
            }
            QGCButton {
                objectName: "caacRelativePoint"
                pointSize: ScreenTools.menuFontPointSize
                Layout.fillWidth: true
                text: qsTr("Relative point")
                heightFactor: 0.25
                leftPadding: ScreenTools.defaultFontPixelWidth
                rightPadding: leftPadding
                onClicked: {
                    root.showMissionTree = false
                    root.activePanel = 1
                }
            }
        }
    }

    RowLayout {
        id: panelNavigation
        width: root.width
        spacing: ScreenTools.defaultFontPixelWidth
        component PanelButton: QGCButton {
            id: button
            required property int panelIndex
            Layout.fillWidth: true
            Layout.minimumWidth: 0
            Layout.preferredHeight: Math.min(ScreenTools.defaultFontPixelHeight * 3,
                (root.width - panelNavigation.spacing * 3) / 4)
            checked: root.activePanel === panelIndex
            Accessible.name: text
            ToolTip.visible: hovered
            ToolTip.text: text
            background: Rectangle {
                anchors.centerIn: parent
                width: Math.min(parent.width, parent.height)
                height: width
                radius: width / 2
                color: button.checked ? dragPalette.colorYellowGreen : "#ffffff"
            }
            contentItem: Canvas {
                onPaint: {
                    const ctx = getContext("2d")
                    ctx.reset()
                    ctx.strokeStyle = "#111111"
                    ctx.fillStyle = ctx.strokeStyle
                    ctx.lineWidth = ScreenTools.defaultFontPixelWidth / 3
                    if (button.panelIndex === 0) {
                        ctx.beginPath()
                        ctx.moveTo(width / 2, height / 6)
                        ctx.lineTo(width * 5 / 6, height * 5 / 12)
                        ctx.lineTo(width / 2, height * 2 / 3)
                        ctx.lineTo(width / 6, height * 5 / 12)
                        ctx.closePath()
                        ctx.moveTo(width / 6, height * 7 / 12)
                        ctx.lineTo(width / 2, height * 5 / 6)
                        ctx.lineTo(width * 5 / 6, height * 7 / 12)
                        ctx.stroke()
                    } else if (button.panelIndex === 1) {
                        ctx.beginPath()
                        ctx.moveTo(width / 4, height / 4)
                        ctx.lineTo(width / 4, height * 2 / 3)
                        ctx.bezierCurveTo(width / 4, height, width / 2, height, width / 2, height * 2 / 3)
                        ctx.lineTo(width / 2, height / 3)
                        ctx.bezierCurveTo(width / 2, 0, width * 3 / 4, 0, width * 3 / 4, height / 3)
                        ctx.lineTo(width * 3 / 4, height * 3 / 4)
                        ctx.stroke()
                        for (const point of [Qt.point(width / 4, height / 4), Qt.point(width * 3 / 4, height * 3 / 4)]) {
                            ctx.beginPath()
                            ctx.arc(point.x, point.y, width / 12, 0, Math.PI * 2)
                            ctx.fill()
                        }
                    } else if (button.panelIndex === 2) {
                        const size = width / 4
                        ctx.strokeRect(width / 6, height / 6, size, size)
                        ctx.strokeRect(width * 7 / 12, height / 6, size, size)
                        ctx.strokeRect(width / 6, height * 7 / 12, size, size)
                        ctx.beginPath()
                        ctx.moveTo(width * 7 / 12, height * 17 / 24)
                        ctx.lineTo(width * 5 / 6, height * 17 / 24)
                        ctx.moveTo(width * 17 / 24, height * 7 / 12)
                        ctx.lineTo(width * 17 / 24, height * 5 / 6)
                        ctx.stroke()
                    } else {
                        for (let i = 0; i < 3; ++i) {
                            const y = height * (0.25 + i * 0.25)
                            ctx.beginPath()
                            ctx.arc(width / 5, y, width / 25, 0, Math.PI * 2)
                            ctx.fill()
                            ctx.beginPath()
                            ctx.moveTo(width * 2 / 5, y)
                            ctx.lineTo(width * 4 / 5, y)
                            ctx.stroke()
                        }
                    }
                }
            }
            onClicked: root.activePanel = panelIndex
        }
        PanelButton { objectName: "caacGlobalSettingsButton"; panelIndex: 0; text: qsTr("Global") }
        PanelButton { objectName: "caacRelativeEditorButton"; panelIndex: 1; text: qsTr("Relative") }
        PanelButton { objectName: "caacTemplateEditorButton"; panelIndex: 2; text: qsTr("Template") }
        PanelButton { objectName: "caacWaypointListButton"; panelIndex: 3; text: qsTr("Points") }
    }

    Rectangle {
        id: listHeader
        width: root.width
        height: headerContent.implicitHeight + ScreenTools.defaultFontPixelHeight
        color: dragPalette.windowShade
        radius: ScreenTools.defaultBorderRadius

        CaacPanelDragHandle {
            objectName: "caacPlanningPanelDragHandle"
            anchors.fill: parent
            onMoved: (delta) => root.movePanel(delta)
        }

        RowLayout {
            id: headerContent
            anchors.fill: parent
            anchors.margins: ScreenTools.defaultFontPixelHeight * 0.5
            QGCLabel {
                objectName: "caacPanelTitle"
                font.pointSize: ScreenTools.menuFontPointSize
                Layout.fillWidth: true
                text: root.activePanel === 0 ? qsTr("Global settings") : root.activePanel === 2 ? qsTr("Template editor")
                    : root.relativeEditorVisible ? qsTr("Relative coordinate editor")
                    : root.showMissionTree ? qsTr("Mission settings") : qsTr("Waypoint list")
                font.bold: true
                wrapMode: Text.WordWrap
                horizontalAlignment: Text.AlignHCenter
            }
            QGCLabel {
                visible: !root.showMissionTree && root.activePanel === 3
                text: qsTr("%1 points").arg(Math.max(0, editorController.coordinateItems.length - 1))
                font.pointSize: ScreenTools.smallFontPointSize
            }
        }
    }

    CaacGlobalSettings {
        objectName: "caacGlobalSettingsPanel"
        width: root.width
        height: Math.max(ScreenTools.defaultFontPixelHeight * 8,
            root.height - panelNavigation.height - listHeader.height - settingsButton.height - helpLabel.height - root.spacing * 4)
        visible: root.activePanel === 0
        routeController: editorController
        editor: root
    }

    CaacTemplateEditor {
        objectName: "caacTemplateEditorPanel"
        width: root.width
        height: Math.max(ScreenTools.defaultFontPixelHeight * 8,
            root.height - panelNavigation.height - listHeader.height - settingsButton.height - helpLabel.height - root.spacing * 4)
        visible: root.activePanel === 2
        routeController: editorController
    }

    CaacRelativeEditor {
        width: root.width
        height: Math.max(ScreenTools.defaultFontPixelHeight * 8,
            root.height - panelNavigation.height - listHeader.height - settingsButton.height - helpLabel.height - root.spacing * 4)
        visible: root.relativeEditorVisible
        routeController: editorController
    }

    QGCListView {
        id: waypointList
        objectName: "caacWaypointList"
        width: root.width
        height: Math.max(ScreenTools.defaultFontPixelHeight * 5,
            root.height - panelNavigation.height - listHeader.height - pointActions.height - settingsButton.height - helpLabel.height
            - root.spacing * 5)
        visible: !root.showMissionTree && root.activePanel === 3
        clip: true
        spacing: ScreenTools.defaultFontPixelHeight * 0.4
        model: root.planMasterController ? root.planMasterController.missionController.visualItems : null
        currentIndex: root.planMasterController ? root.planMasterController.missionController.currentPlanViewVIIndex : -1
        onCurrentIndexChanged: {
            if (currentIndex >= 0) {
                positionViewAtIndex(currentIndex, ListView.Contain)
            }
        }

        delegate: Rectangle {
            id: card
            required property var object
            required property int index
            readonly property bool _display: index === 0 || (object.isSimpleItem && object.specifiesCoordinate
                && !object.isStandaloneCoordinate)
            readonly property var _leg: editorController.coordinateItems.length > 0
                ? editorController.waypointLeg(object) : ({})
            readonly property var _speed: index > 0 && object.isSimpleItem ? object.speedSection : null
            objectName: "caacWaypointCard_" + object.sequenceNumber
            width: waypointList.width
            height: _display ? cardContent.implicitHeight + ScreenTools.defaultFontPixelHeight : 0
            visible: _display
            radius: ScreenTools.defaultBorderRadius
            color: object.isCurrentItem ? dragPalette.buttonHighlight : dragPalette.windowShade
            readonly property color _textColor: object.isCurrentItem ? dragPalette.buttonHighlightText : dragPalette.text

            RowLayout {
                id: cardContent
                anchors.fill: parent
                anchors.margins: ScreenTools.defaultFontPixelHeight * 0.5
                spacing: ScreenTools.defaultFontPixelWidth
                QGCLabel {
                    objectName: "caacWaypointNumber_" + card.object.sequenceNumber
                    text: card.object.sequenceNumber
                    font.pointSize: ScreenTools.largeFontPointSize
                    font.bold: true
                    color: card._textColor
                    Layout.minimumWidth: ScreenTools.defaultFontPixelWidth * 2
                }
                ColumnLayout {
                    Layout.fillWidth: true
                    spacing: ScreenTools.defaultFontPixelHeight * 0.2
                    QGCLabel {
                        Layout.fillWidth: true
                        text: card.index === 0 ? qsTr("Planned home") : card.object.commandName
                        color: card._textColor
                        elide: Text.ElideRight
                    }
                    QGCLabel {
                        Layout.fillWidth: true
                        visible: card.index > 0
                        font.pointSize: ScreenTools.smallFontPointSize
                        color: card._textColor
                        text: card.index > 0 ? qsTr("Altitude: %1 m · Speed: %2").arg(card.object.altitude.rawValue.toFixed(1))
                            .arg(card._speed && card._speed.specifyFlightSpeed
                                ? qsTr("%1 m/s").arg(card._speed.flightSpeed.rawValue.toFixed(1)) : qsTr("Default")) : ""
                        wrapMode: Text.WordWrap
                    }
                    QGCLabel {
                        Layout.fillWidth: true
                        visible: card._leg.distance !== undefined
                        font.pointSize: ScreenTools.smallFontPointSize
                        color: card._textColor
                        text: card._leg.distance !== undefined ? qsTr("Distance: %1 m · Angle: %2")
                            .arg(card._leg.distance.toFixed(1)).arg(card._leg.angle !== undefined
                                ? qsTr("%1°").arg(card._leg.angle.toFixed(1)) : "—") : ""
                        wrapMode: Text.WordWrap
                    }
                }
            }

            MouseArea {
                anchors.fill: parent
                acceptedButtons: Qt.LeftButton | Qt.RightButton
                enabled: card.index > 0
                onClicked: (mouse) => {
                    if (mouse.button === Qt.RightButton) {
                        root.editWaypoint(card.object)
                    } else {
                        editorController.selectWaypoint(card.object)
                    }
                }
            }
        }
    }

    RowLayout {
        id: pointActions
        visible: root.activePanel === 3
        width: root.width
        spacing: ScreenTools.defaultFontPixelWidth * 0.3
        QGCButton {
            objectName: "caacEditWaypoint"
            pointSize: ScreenTools.menuFontPointSize
            Layout.fillWidth: true
            Layout.minimumWidth: 0
            text: qsTr("Edit")
            leftPadding: ScreenTools.defaultFontPixelWidth * 0.5
            rightPadding: leftPadding
            heightFactor: 0.25
            enabled: root.currentItem && root.currentItem.specifiesCoordinate
            onClicked: root.editWaypoint(root.currentItem)
        }
        QGCButton {
            objectName: "caacRemoveWaypoint"
            pointSize: ScreenTools.menuFontPointSize
            Layout.fillWidth: true
            Layout.minimumWidth: 0
            text: qsTr("Delete")
            leftPadding: ScreenTools.defaultFontPixelWidth * 0.5
            rightPadding: leftPadding
            heightFactor: 0.25
            enabled: root.currentItem !== null
            onClicked: editorController.removeCurrentWaypoint()
        }
        QGCButton {
            objectName: "caacMoveWaypointUp"
            pointSize: ScreenTools.menuFontPointSize
            Layout.preferredWidth: ScreenTools.defaultFontPixelWidth * 4
            Layout.maximumWidth: Layout.preferredWidth
            text: "↑"
            leftPadding: ScreenTools.defaultFontPixelWidth * 0.5
            rightPadding: leftPadding
            heightFactor: 0.25
            ToolTip.visible: hovered
            ToolTip.text: qsTr("Up")
            enabled: root.currentItem !== null
            onClicked: editorController.moveCurrentWaypoint(-1)
        }
        QGCButton {
            objectName: "caacMoveWaypointDown"
            pointSize: ScreenTools.menuFontPointSize
            Layout.preferredWidth: ScreenTools.defaultFontPixelWidth * 4
            Layout.maximumWidth: Layout.preferredWidth
            text: "↓"
            leftPadding: ScreenTools.defaultFontPixelWidth * 0.5
            rightPadding: leftPadding
            heightFactor: 0.25
            ToolTip.visible: hovered
            ToolTip.text: qsTr("Down")
            enabled: root.currentItem !== null
            onClicked: editorController.moveCurrentWaypoint(1)
        }
    }

    QGCButton {
        id: settingsButton
        objectName: "caacMissionSettings"
        pointSize: ScreenTools.menuFontPointSize
        width: root.width
        text: root.activePanel !== 3 || root.showMissionTree ? qsTr("Waypoint list") : qsTr("Mission settings")
        heightFactor: 0.25
        onClicked: {
            if (root.activePanel !== 3) {
                root.activePanel = 3
            } else {
                root.showMissionTree = !root.showMissionTree
            }
        }
    }

    QGCLabel {
        id: helpLabel
        width: root.width
        wrapMode: Text.WordWrap
        font.pointSize: ScreenTools.smallFontPointSize
        text: root.measurePick > 0 ? qsTr("Click the map to choose point %1. Esc cancels.").arg(root.measurePick === 1 ? "A" : "B")
            : root._pickingCenter ? qsTr("Click the map to choose a rotation centre. Esc cancels.")
            : editorController.lastError || (root.routeDragMode
                ? qsTr("Left-drag the route to move all points. Turn off route dragging to edit individual points.")
                : qsTr("Double-click to add · Drag to move · Right-click to edit"))
    }

    QGCPopupDialogFactory {
        id: measureDialogFactory
        dialogComponent: Component { CaacMeasureDialog { editor: root } }
    }
    QGCPopupDialogFactory {
        id: unitDialogFactory
        dialogComponent: Component { CaacUnitConverter { } }
    }
    QGCPopupDialogFactory {
        id: waypointDialogFactory
        dialogComponent: Component {
            CaacWaypointDialog {
                routeController: editorController
            }
        }
    }

    QGCPopupDialogFactory {
        id: geometryDialogFactory
        dialogComponent: Component {
            CaacGeometryDialog {
                routeController: editorController
                editor: root
            }
        }
    }

    Shortcut {
        sequence: "Esc"
        enabled: root._pickingCenter || root.measurePick > 0
        onActivated: { root._pickingCenter = false; root.measurePick = 0 }
    }

    Connections {
        target: root.editorMap

        function onWidthChanged() { root.movePanel(Qt.point(0, 0)) }
        function onHeightChanged() { root.movePanel(Qt.point(0, 0)) }
        function onDirectMissionItemEditingChanged() {
            if (!root.editorMap.directMissionItemEditing) { root.measurePick = 0 }
        }

        function onMapDoubleClicked(position) {
            if (root.enabled && root.visible && root.editorMap.directMissionItemEditing && !root._pickingCenter && root.measurePick === 0
                && QGroundControl.corePlugin.doubleClickAddWaypoint) {
                editorController.addWaypoint(root.editorMap.toCoordinate(position, false))
            }
        }
        function onMapClicked(position) {
            if (root.enabled && root.visible && root.measurePick > 0) {
                const coordinate = root.editorMap.toCoordinate(position, false)
                if (!coordinate.isValid) { return }
                if (root.measurePick === 1) {
                    root.measureA = coordinate
                    root.measurePick = root.measureB.isValid ? 0 : 2
                } else {
                    root.measureB = coordinate
                    root.measurePick = 0
                }
                if (root.measurePick === 0) { root.openMeasurement() }
            } else if (root._pickingCenter) {
                if (editorController.setRotationCenter(root.editorMap.toCoordinate(position, false))) {
                    root._pickingCenter = false
                    root.openGeometry(root._pendingOperation)
                }
            }
        }
        function onMissionItemDragStarted(item) {
            root._draggedWaypoint = item
            editorController.beginEdit()
        }
        function onMissionItemDragFinished(item) {
            root._draggedWaypoint = null
            editorController.endEdit()
        }
        function onMissionItemEditRequested(item) {
            if (root.enabled && root.visible && root.editorMap.directMissionItemEditing) {
                root.editWaypoint(item)
            }
        }
    }
}
