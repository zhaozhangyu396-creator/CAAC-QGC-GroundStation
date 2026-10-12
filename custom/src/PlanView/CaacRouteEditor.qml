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
    readonly property real _settingsButtonHeight: settingsButton.visible ? settingsButton.height : 0
    property int activePanel: 3
    readonly property bool relativeEditorVisible: activePanel === 1
    property bool routeDragMode: false
    property point _panelDragOffset: Qt.point(0, 0)
    transform: Translate { x: root._panelDragOffset.x; y: root._panelDragOffset.y }
    onActivePanelChanged: showMissionTree = false
    readonly property var currentItem: editorController.currentItem
    readonly property var routeController: editorController
    readonly property var _managerVehicle: planMasterController ? planMasterController.managerVehicle : null
    readonly property bool _canTransfer: planMasterController && !planMasterController.offline
        && _managerVehicle && _managerVehicle === QGroundControl.multiVehicleManager.activeVehicle
        && _managerVehicle.initialConnectComplete && !planMasterController.syncInProgress
        && !editorController.rotationPreviewActive
    readonly property real _transferHeight: waypointTransfer.visible ? waypointTransfer.height : 0
    property bool _pickingCenter: false
    property bool _pickingHome: false
    property int measurePick: 0
    property var measureA: QtPositioning.coordinate()
    property var measureB: QtPositioning.coordinate()
    onVisibleChanged: { if (!visible) { measurePick = 0; _pickingHome = false } }
    onEnabledChanged: { if (!enabled) { measurePick = 0; _pickingHome = false } }
    property int _pendingOperation: 0
    property var _draggedWaypoint: null
    readonly property var _dragLeg: _draggedWaypoint && editorController.coordinateItems.length > 0
        ? editorController.waypointLeg(_draggedWaypoint) : ({})

    QGCPalette { id: dragPalette; colorGroupEnabled: root.enabled }

    function openAdvancedMissionSettings() {
        root.activePanel = 3
        root.showMissionTree = true
    }
    function openSimulation() {
        const simulation = QGroundControl.corePlugin.simulation
        if (simulation.active) { simulationDialogFactory.open() }
        else if (!simulation.start(root.planMasterController)) {
            QGroundControl.showMessageDialog(mainWindow, qsTr("Simulation flight"), simulation.status)
        }
    }
    Connections {
        target: QGroundControl.corePlugin.simulation
        function onMissionUploaded() { mainWindow.showFlyView() }
    }

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

    CaacWaypointOverlap {
        parent: root.editorMap
        anchors.fill: parent
        visible: root.visible && root.enabled
        z: QGroundControl.zOrderMapItems + 1
        enabled: !waypointPopup.visible && !waypointMenu.visible
        blockedAreas: {
            void root._panelDragOffset
            void root.x
            void root.y
            const point = root.editorMap ? root.mapToItem(root.editorMap, 0, 0) : Qt.point(0, 0)
            return [Qt.rect(point.x, point.y, root.width, root.height)]
        }
        mapControl: root.editorMap
        missionController: root.planMasterController ? root.planMasterController.missionController : null
    }

    CaacWaypointPopup { id: waypointPopup; editor: root }
    CaacWaypointMenu { id: waypointMenu; editor: root }
    property bool showDrawingAids: true
    CaacDrawingAids {
        id: drawingAids
        mapControl: root.editorMap
        showAids: root.visible && root.enabled && root.showDrawingAids
    }
    function closeWaypointProperties() { waypointPopup.close() }
    function addDrawingAid(origin, radius, bearing, axes) { drawingAids.add(origin, radius, bearing, axes) }
    function clearDrawingAids() { drawingAids.clear() }
    readonly property bool hasCoordinateAxes: drawingAids.aids.some(aid => aid.axes)
    function clearCoordinateAxes() { drawingAids.clearAxes() }
    function pickPlannedHome() {
        _pickingCenter = false
        measurePick = 0
        _pickingHome = true
    }
    function centerPlannedHome() {
        if (editorMap && planMasterController && planMasterController.missionController.plannedHomePosition.isValid) {
            editorMap.center = planMasterController.missionController.plannedHomePosition
        }
    }
    function confirmClearWaypoints() { clearWaypointsFactory.open() }
    function confirmGoto(coordinate) {
        const vehicle = QGroundControl.multiVehicleManager.activeVehicle
        if (!vehicle || !vehicle.armed || !vehicle.airborne || !vehicle.supports.guidedMode) { return }
        mainWindow.showFlyView()
        Qt.callLater(() => {
            const guided = globals.guidedControllerFlyView
            if (guided && vehicle === QGroundControl.multiVehicleManager.activeVehicle && guided.confirmDialog) {
                guided.confirmAction(guided.actionGoto, coordinate)
            }
        })
    }
    QGCPopupDialogFactory {
        id: clearWaypointsFactory
        dialogComponent: Component {
            QGCPopupDialog {
                title: qsTr("Clear waypoints")
                buttons: Dialog.Ok | Dialog.Cancel
                QGCLabel {
                    text: qsTr("Remove all mission items and keep the planned takeoff point? This can be undone.")
                    wrapMode: Text.WordWrap
                    width: ScreenTools.defaultFontPixelWidth * 40
                }
                onAccepted: preventClose = !editorController.clearWaypoints()
            }
        }
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
        waypointPopup.close()
        editorController.cancelRouteOrientation()
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
            checked: !root.showMissionTree && root.activePanel === panelIndex
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
            onClicked: {
                root.showMissionTree = false
                root.activePanel = panelIndex
            }
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
                    : root.showMissionTree ? qsTr("Advanced mission settings") : qsTr("Waypoint list")
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
            root.height - panelNavigation.height - listHeader.height - root._settingsButtonHeight - helpLabel.height - root.spacing * 4)
        visible: root.activePanel === 0
        routeController: editorController
        editor: root
    }

    CaacTemplateEditor {
        objectName: "caacTemplateEditorPanel"
        width: root.width
        height: Math.max(ScreenTools.defaultFontPixelHeight * 8,
            root.height - panelNavigation.height - listHeader.height - root._settingsButtonHeight - helpLabel.height - root.spacing * 4)
        visible: root.activePanel === 2
        routeController: editorController
    }

    CaacRelativeEditor {
        width: root.width
        height: Math.max(ScreenTools.defaultFontPixelHeight * 8,
            root.height - panelNavigation.height - listHeader.height - root._settingsButtonHeight - helpLabel.height - root.spacing * 4)
        visible: root.relativeEditorVisible
        routeController: editorController
    }

    QGCListView {
        id: waypointList
        objectName: "caacWaypointList"
        width: root.width
        height: Math.max(ScreenTools.defaultFontPixelHeight * 5,
            root.height - panelNavigation.height - listHeader.height - pointActions.height - root._transferHeight - root._settingsButtonHeight - helpLabel.height
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

            MouseArea {
                anchors.fill: parent
                acceptedButtons: Qt.LeftButton
                enabled: card.index > 0
                onClicked: editorController.selectWaypoint(card.object)
            }

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
                ColumnLayout {
                    visible: card.index > 0
                    spacing: ScreenTools.defaultFontPixelHeight * 0.2
                    QGCButton {
                        objectName: "caacEditWaypoint_" + card.object.sequenceNumber
                        text: qsTr("Edit")
                        heightFactor: 0.2
                        onClicked: root.editWaypoint(card.object)
                    }
                    QGCButton {
                        objectName: "caacRemoveWaypoint_" + card.object.sequenceNumber
                        text: qsTr("Delete")
                        enabled: card.object.command === 16
                        heightFactor: 0.2
                        onClicked: {
                            waypointPopup.close()
                            editorController.cancelRouteOrientation()
                            if (editorController.selectWaypoint(card.object)) { editorController.removeCurrentWaypoint() }
                        }
                    }
                }
            }
        }
    }

    RowLayout {
        id: pointActions
        visible: root.activePanel === 3 && !root.showMissionTree
        width: root.width
        spacing: ScreenTools.defaultFontPixelWidth * 0.3
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

    RowLayout {
        id: waypointTransfer
        objectName: "caacWaypointTransferActions"
        width: root.width
        visible: root.activePanel === 3 && !root.showMissionTree
        spacing: ScreenTools.defaultFontPixelWidth
        QGCButton {
            objectName: "caacReadWaypoints"
            Layout.fillWidth: true
            Layout.minimumWidth: 0
            pointSize: ScreenTools.menuFontPointSize
            text: qsTr("Read waypoints")
            enabled: root._canTransfer
            onClicked: {
                waypointPopup.close()
                if (root.planMasterController.dirtyForSave) { readWaypointsFactory.open() }
                else { root.planMasterController.loadFromVehicle() }
            }
        }
        QGCButton {
            objectName: "caacUploadWaypoints"
            Layout.fillWidth: true
            Layout.minimumWidth: 0
            pointSize: ScreenTools.menuFontPointSize
            primary: true
            text: qsTr("Upload waypoints")
            enabled: root._canTransfer && root.planMasterController.containsItems
            onClicked: { waypointPopup.close(); root.planMasterController.upload() }
        }
    }
    QGCPopupDialogFactory {
        id: readWaypointsFactory
        dialogComponent: Component {
            QGCPopupDialog {
                id: readDialog
                title: qsTr("Read waypoints")
                buttons: Dialog.Yes | Dialog.Cancel
                property var vehicleAtOpen: null
                property var masterAtOpen: null
                Component.onCompleted: { vehicleAtOpen = root._managerVehicle; masterAtOpen = root.planMasterController }
                QGCLabel {
                    width: ScreenTools.defaultFontPixelWidth * 40
                    wrapMode: Text.WordWrap
                    text: qsTr("Reading from the drone will replace the current unsaved plan. Continue?")
                }
                onAccepted: {
                    if (root._canTransfer && root._managerVehicle === vehicleAtOpen && root.planMasterController === masterAtOpen) {
                        root.planMasterController.loadFromVehicle()
                    }
                }
                Connections {
                    target: QGroundControl.multiVehicleManager
                    function onActiveVehicleChanged() { if (readDialog.vehicleAtOpen !== root._managerVehicle) { readDialog.close() } }
                }
            }
        }
    }

    QGCButton {
        id: settingsButton
        objectName: "caacReturnToWaypoints"
        visible: root.showMissionTree
        pointSize: ScreenTools.menuFontPointSize
        width: root.width
        text: qsTr("Return to waypoint list")
        heightFactor: 0.25
        onClicked: {
            root.showMissionTree = false
        }
    }

    QGCLabel {
        id: helpLabel
        width: root.width
        wrapMode: Text.WordWrap
        font.pointSize: ScreenTools.smallFontPointSize
        text: root.showMissionTree ? qsTr("Camera, fence, rally points and other mission commands. Uses the same mission as the four main panels.")
            : root.measurePick > 0 ? qsTr("Click the map to choose point %1. Esc cancels.").arg(root.measurePick === 1 ? "A" : "B")
            : root._pickingHome ? qsTr("Click the map to set the planned takeoff point. Esc cancels.")
            : root._pickingCenter ? qsTr("Click the map to choose a rotation centre. Esc cancels.")
            : editorController.lastError || (root.routeDragMode
                ? qsTr("Left-drag the route to move all points. Turn off route dragging to edit individual points.")
                : qsTr("Double-click to add · Drag to move · Left-click properties · Right-click actions"))
    }

    QGCPopupDialogFactory {
        id: simulationDialogFactory
        dialogComponent: Component { CaacSimulationDialog { editor: root } }
    }
    Connections {
        target: root.editorMap
        function onSimulationRequested() { root.openSimulation() }
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
        enabled: root._pickingCenter || root._pickingHome || root.measurePick > 0
        onActivated: { root._pickingCenter = false; root._pickingHome = false; root.measurePick = 0 }
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
            if (!root.enabled || !root.visible) { return }
            if (root._pickingHome) {
                const coordinate = root.editorMap.toCoordinate(position, false)
                const home = root.planMasterController.missionController.plannedHomePosition
                if (editorController.updatePlannedHome(coordinate.latitude, coordinate.longitude, home.altitude)) {
                    root._pickingHome = false
                }
            } else if (root.measurePick > 0) {
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
            waypointPopup.close()
            editorController.cancelRouteOrientation()
            root._draggedWaypoint = item
            editorController.beginEdit()
        }
        function onMissionItemDragFinished(item) {
            root._draggedWaypoint = null
            editorController.endEdit()
        }
        function onMissionItemEditRequested(item) {
            if (root.enabled && root.visible && root.editorMap.directMissionItemEditing) {
                waypointMenu.show(item)
            }
        }
        function onMissionItemPropertiesRequested(item) {
            if (root.enabled && root.visible && root.editorMap.directMissionItemEditing) {
                waypointPopup.show(item)
            }
        }
    }
}
