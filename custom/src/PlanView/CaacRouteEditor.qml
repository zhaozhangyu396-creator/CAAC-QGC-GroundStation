pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtLocation
import QtPositioning

import QGC
import QGroundControl
import QGroundControl.Controls
import QGroundControl.FlightMap
import QGroundControl.PlanView
import CAAC.Plan

Column {
    id: root
    objectName: "caacRouteEditor"
    spacing: ScreenTools.defaultFontPixelHeight * 0.25

    property var editorMap: null
    property var planMasterController: null
    property bool showMissionTree: false
    readonly property real _settingsButtonHeight: settingsButton.visible ? settingsButton.height : 0
    property int activePanel: -1
    readonly property bool panelsCollapsed: activePanel < 0 && !showMissionTree
    readonly property real navigationHeight: panelNavigation.height
    readonly property bool relativeEditorVisible: activePanel === 1
    property bool routeDragMode: false
    property point _panelDragOffset: Qt.point(0, 0)
    transform: Translate { x: root._panelDragOffset.x; y: root._panelDragOffset.y }
    onActivePanelChanged: showMissionTree = false
    readonly property var currentItem: editorController.currentItem
    readonly property var routeController: editorController
    property var simulationPreviewSettings: null
    property int _flightSpeedRevision: 0
    function prepareUpload() { return globalSettings.prepareUpload() }
    function requestWaypointUpload() {
        if (root._canTransfer && !root._deletedWaypointsPrompt) { updateWaypointsFactory.open() }
    }
    readonly property var simulationPreviewSegments: {
        void editorController.coordinateItems
        void root._flightSpeedRevision
        return editorController.simulationPreviewSegments(simulationPreviewSettings || editorController.simulationSettings)
    }
    readonly property bool simulationPreviewActive: simulationPreviewSegments.length > 0
        && (simulationPreviewSettings !== null || editorController.hasSimulationSettings)

    Connections {
        target: root.planMasterController ? root.planMasterController.missionController : null
        function onMissionTimeChanged() { ++root._flightSpeedRevision }
    }

    function attachPreviewItem(item) {
        if (editorMap && item.attachedMap !== editorMap) {
            if (item.attachedMap) { item.attachedMap.removeMapItem(item) }
            editorMap.addMapItem(item)
            item.attachedMap = editorMap
        }
    }
    onEditorMapChanged: {
        for (let i = 0; i < previewItems.count; ++i) {
            const item = previewItems.objectAt(i)
            if (item) { item.attach() }
        }
    }

    Instantiator {
        id: previewItems
        model: root.simulationPreviewActive ? root.simulationPreviewSegments : []
        delegate: Item {
            id: preview
            required property var modelData
            required property int index
            function attach() { root.attachPreviewItem(line) }
            MapPolyline {
                id: line
                objectName: "caacSimulationPreview_" + preview.modelData.kind + (preview.modelData.kind === "body" ? "_" + preview.index : "")
                property var attachedMap: null
                path: [preview.modelData.coordinate1, preview.modelData.coordinate2]
                visible: root.visible && root.enabled
                line.color: {
                    if (Boolean(preview.modelData.terrainCollision)) {
                        return QGroundControl.globalPalette.colorRed
                    }
                    const mission = root.planMasterController ? root.planMasterController.missionController : null
                    if (!mission) { return QGroundControl.globalPalette.mapMissionTrajectory }
                    void mission.uploadedRouteRevision
                    return mission.isRouteSegmentUploaded(preview.modelData.coordinate1, preview.modelData.coordinate2, preview.modelData.flightSpeed)
                        ? QGroundControl.globalPalette.colorGreen : QGroundControl.globalPalette.mapMissionTrajectory
                }
                line.width: ScreenTools.defaultFontPixelWidth * 0.3
                z: QGroundControl.zOrderWaypointLines
                Component.onDestruction: { if (attachedMap) { attachedMap.removeMapItem(line) } }
            }
            Component.onCompleted: attach()
        }
    }
    property var selectedWaypointItems: []
    readonly property int selectedWaypointCount: selectedWaypointItems.length
    readonly property var _selectableWaypoints: {
        const items = []
        const model = planMasterController ? planMasterController.missionController.visualItems : null
        if (model) {
            for (let i = 1; i < model.count; ++i) {
                const item = model.get(i)
                if (item.isSimpleItem && item.command === 16) { items.push(item) }
            }
        }
        return items
    }
    readonly property bool allWaypointsSelected: _selectableWaypoints.length > 0
        && _selectableWaypoints.every(item => selectedWaypointItems.indexOf(item) >= 0)
    on_SelectableWaypointsChanged: {
        selectedWaypointItems = selectedWaypointItems.filter(item => _selectableWaypoints.indexOf(item) >= 0)
    }

    function selectWaypointForDeletion(item, selected) {
        const items = selectedWaypointItems.filter(point => point !== item)
        if (selected && _selectableWaypoints.indexOf(item) >= 0) { items.push(item) }
        selectedWaypointItems = items
    }
    property var _deletedWaypointsPrompt: null
    property int _deletionSourceRevision: 0
    readonly property bool canDeleteWaypoints: !missionUpdate.busy && !_deletedWaypointsPrompt
        && root.planMasterController && !root.planMasterController.syncInProgress

    function removeWaypoint(item) {
        if (!canDeleteWaypoints) { return false }
        waypointPopup.close()
        editorController.cancelRouteOrientation()
        if (!editorController.selectWaypoint(item) || !editorController.removeCurrentWaypoint()) { return false }
        offerDeletedWaypointUpload()
        return true
    }

    function removeSelectedWaypoints() {
        if (!canDeleteWaypoints) { return false }
        waypointPopup.close()
        editorController.cancelRouteOrientation()
        if (!editorController.removeWaypoints(selectedWaypointItems)) { return false }
        selectedWaypointItems = []
        offerDeletedWaypointUpload()
        return true
    }

    function offerDeletedWaypointUpload() {
        const vehicle = root._managerVehicle
        if (!root._canTransfer || !vehicle || !vehicle.armed || !vehicle.airborne || root._deletedWaypointsPrompt) { return }
        root._deletedWaypointsPrompt = deletedWaypointsFactory.open({
            vehicleAtOpen: vehicle,
            masterAtOpen: root.planMasterController,
            revisionAtOpen: root._deletionSourceRevision
        })
    }

    Connections {
        target: editorController
        function onHistoryChanged() { ++root._deletionSourceRevision }
    }
    readonly property var _managerVehicle: planMasterController ? planMasterController.managerVehicle : null
    readonly property bool _canTransfer: !missionUpdate.busy && planMasterController && !planMasterController.offline
        && _managerVehicle && _managerVehicle === QGroundControl.multiVehicleManager.activeVehicle
        && _managerVehicle.initialConnectComplete && !planMasterController.syncInProgress
        && !editorController.rotationPreviewActive
    readonly property real _transferHeight: (waypointTransfer.visible ? waypointTransfer.height : 0)
        + (fileActions.visible ? fileActions.height + root.spacing : 0)
    property bool _pickingCenter: false
    property bool _pickingHome: false
    property int measurePick: 0
    readonly property bool routeInsertionEnabled: visible && enabled && !_pickingCenter && !_pickingHome
        && measurePick === 0 && !routeDragMode && !missionUpdate.busy
    property var measureA: QtPositioning.coordinate()
    property var measureB: QtPositioning.coordinate()
    onVisibleChanged: {
        if (!visible) { measurePick = 0; _pickingHome = false }
        else { activePanel = -1; showMissionTree = false }
    }
    onEnabledChanged: { if (!enabled) { measurePick = 0; _pickingHome = false } }
    property int _pendingOperation: 0
    property var _draggedWaypoint: null
    property var _dragStartCoordinate: QtPositioning.coordinate()
    property var _dragGuides: []
    function snapDraggedWaypoint(item, coordinate) {
        const point = editorMap.fromCoordinate(coordinate, false)
        const pixels = ScreenTools.defaultFontPixelHeight * 0.75
        if (item && item.command === 16) {
            const capture = editorController.snapDrawingAidIntersection(point, drawingAids.screenSegments(), pixels)
            if (capture.point !== undefined) {
                const snapped = editorMap.toCoordinate(capture.point, false)
                if (snapped && snapped.isValid) {
                    snapped.altitude = coordinate.altitude
                    const guides = capture.guides
                    guides[0].intersection = snapped
                    _dragGuides = guides
                    return snapped
                }
            }
        }
        const nearby = editorMap.toCoordinate(Qt.point(point.x + pixels, point.y), false)
        const tolerance = QGroundControl.corePlugin.routeLegMetrics(coordinate, nearby).distance
        const result = editorController.snapWaypointDrag(item, coordinate, tolerance)
        _dragGuides = result.guides || []
        return result.coordinate || coordinate
    }
    Binding {
        target: root.editorMap
        property: "missionItemDragEditor"
        value: root.routeInsertionEnabled ? root : null
    }
    Binding {
        target: root.editorMap
        property: "missionItemDragBlocked"
        value: missionUpdate.busy
    }
    readonly property var _dragLeg: _draggedWaypoint && editorController.coordinateItems.length > 0
        ? editorController.waypointLeg(_draggedWaypoint) : ({})

    QGCPalette { id: dragPalette; colorGroupEnabled: root.enabled }

    function openAdvancedMissionSettings() {
        root.activePanel = 3
        root.showMissionTree = true
    }
    function openHomeRelations() { homeRelationsFactory.open() }
    function openSimulation() {
        const simulation = QGroundControl.corePlugin.simulation
        if (simulation.active) { simulation.stop() }
        else {
            if (!simulation.start(root.planMasterController)) {
                QGroundControl.showMessageDialog(mainWindow, qsTr("Simulation flight"), simulation.status)
            } else if (root.editorMap && root.planMasterController.missionController.visualItems.count === 1) {
                root.editorMap.center = root.planMasterController.missionController.plannedHomePosition
            }
        }
    }
    Connections {
        target: QGroundControl.corePlugin.simulation
        function onMissionUploaded() { mainWindow.showFlyView() }
    }

    Rectangle {
        parent: root.parent
        x: root.x
        y: root.y + panelNavigation.height
        width: root.width
        height: Math.max(0, root.height - panelNavigation.height)
        transform: Translate { x: root._panelDragOffset.x; y: root._panelDragOffset.y }
        z: -1
        visible: root.enabled && root.visible && !root.showMissionTree && !root.panelsCollapsed
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
    function centerPlannedHome() { editorController.centerHomeOnRoute() }
    function confirmClearWaypoints() { if (root.canDeleteWaypoints) { clearWaypointsFactory.open() } }
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
        previewSegments: root.simulationPreviewActive ? root.simulationPreviewSegments : []
        runtimeMissionController: globals.planMasterControllerFlyView.missionController
        runtimeVehicle: root.planMasterController && root.planMasterController.missionController.uploadConfirmed
            ? QGroundControl.multiVehicleManager.activeVehicle : null
    }

    CaacWaypointGuides {
        parent: root.editorMap
        anchors.fill: parent
        visible: root.enabled && root.visible
        mapControl: root.editorMap
        guides: root._draggedWaypoint ? root._dragGuides : []
        angles: {
            editorController.coordinateItems
            return editorController.waypointAngles((root.simulationPreviewSettings || editorController.simulationSettings).closed)
        }
    }

    CaacMeasureMap {
        mapControl: root.editorMap
        pointA: root.measureA
        pointB: root.measureB
        showMeasurement: root.enabled && root.visible
        onPointMoved: (label, coordinate) => {
            if (label === "A") { root.measureA = coordinate }
            else { root.measureB = coordinate }
        }
        onSettingsRequested: root.showMeasurementSettings()
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
                    .arg(root._draggedWaypoint.waypointNumber).arg(root._dragLeg.distance.toFixed(2))
                    + "\n" + (root._dragLeg.angle !== undefined
                        ? qsTr("Interior angle: %1°").arg(root._dragLeg.angle.toFixed(0))
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

    function openMeasurement() {
        clearMeasurement()
        _pickingHome = false
        pickMeasurement(1)
    }
    function showMeasurementSettings() { measureDialogFactory.open() }
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
        enabled: root.enabled && root.visible && root.editorMap && root.editorMap.directMissionItemEditing && !missionUpdate.busy
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

    Menu {
        id: mapContextMenu
        objectName: "caacMapContextMenu"
        parent: root.editorMap
        font.pointSize: ScreenTools.menuFontPointSize
        palette.window: dragPalette.windowShadeDark
        palette.windowText: dragPalette.text
        palette.text: dragPalette.text
        palette.base: dragPalette.windowShadeDark
        palette.highlight: dragPalette.buttonHighlight
        palette.highlightedText: dragPalette.buttonHighlightText
        MenuItem {
            objectName: "caacMapClearWaypoints"
            text: qsTr("Clear waypoints")
            enabled: !missionUpdate.busy && root.planMasterController
                && !root.planMasterController.syncInProgress && root.planMasterController.missionController.visualItems.count > 1
            onTriggered: root.confirmClearWaypoints()
        }
    }
    Rectangle {
        objectName: "caacMapEditTools"
        parent: root.editorMap && root.editorMap.mapToolsContainer ? root.editorMap.mapToolsContainer
            : root.editorMap ? root.editorMap.parent : null
        visible: root.enabled && root.visible
        z: QGroundControl.zOrderWidgets + 4
        Layout.alignment: Qt.AlignHCenter
        Layout.topMargin: ScreenTools.defaultFontPixelHeight * 0.4
        implicitWidth: ScreenTools.defaultFontPixelHeight * 1.25
        implicitHeight: rail.implicitHeight
        width: implicitWidth
        height: implicitHeight
        color: "#bb202326"
        radius: ScreenTools.defaultFontPixelWidth
        Column {
            id: rail
            width: parent.width
            component RailButton: QGCButton {
                id: button
                property string toolKind
                width: rail.width
                height: width
                leftPadding: 0
                rightPadding: 0
                topPadding: 0
                bottomPadding: 0
                Accessible.name: text
                ToolTip.visible: hovered
                ToolTip.text: text
                background: Rectangle { color: button.hovered && button.enabled ? "#39423e" : "transparent"; radius: ScreenTools.defaultBorderRadius }
                contentItem: Canvas {
                    opacity: button.enabled ? 1 : 0.35
                    onPaint: {
                        const ctx = getContext("2d")
                        ctx.reset()
                        ctx.strokeStyle = "#ffffff"
                        ctx.lineWidth = width * 0.05
                        ctx.lineCap = "round"
                        if (button.toolKind === "center") {
                            ctx.beginPath(); ctx.arc(width / 2, height / 2, width * 0.18, 0, Math.PI * 2)
                            for (let i = 0; i < 4; ++i) {
                                const angle = i * Math.PI / 2
                                ctx.moveTo(width / 2 + Math.cos(angle) * width * 0.22, height / 2 + Math.sin(angle) * width * 0.22)
                                ctx.lineTo(width / 2 + Math.cos(angle) * width * 0.34, height / 2 + Math.sin(angle) * width * 0.34)
                            }
                            ctx.stroke()
                        } else if (button.toolKind === "undo" || button.toolKind === "redo") {
                            if (button.toolKind === "redo") { ctx.translate(width, 0); ctx.scale(-1, 1) }
                            ctx.beginPath()
                            ctx.moveTo(width * 0.28, height * 0.35)
                            ctx.lineTo(width * 0.61, height * 0.35)
                            ctx.bezierCurveTo(width * 0.86, height * 0.35, width * 0.86, height * 0.68, width * 0.61, height * 0.68)
                            ctx.lineTo(width * 0.35, height * 0.68)
                            ctx.moveTo(width * 0.4, height * 0.22)
                            ctx.lineTo(width * 0.27, height * 0.35)
                            ctx.lineTo(width * 0.4, height * 0.48)
                            ctx.stroke()
                        } else {
                            ctx.beginPath()
                            ctx.moveTo(width * 0.27, height / 2); ctx.lineTo(width * 0.73, height / 2)
                            if (button.toolKind === "zoomIn") { ctx.moveTo(width / 2, height * 0.27); ctx.lineTo(width / 2, height * 0.73) }
                            ctx.stroke()
                        }
                    }
                }
            }
            RailButton {
                id: centerButton
                objectName: "caacHomeCenterButton"
                text: qsTr("Click: center map on takeoff point. Double-click: center map on aircraft.")
                toolKind: "center"
                readonly property var vehicle: QGroundControl.multiVehicleManager.activeVehicle
                readonly property var home: root.planMasterController ? root.planMasterController.missionController.plannedHomePosition : null
                enabled: (home && home.isValid) || (vehicle && vehicle.coordinate.isValid)
                onClicked: singleClick.restart()
                Timer {
                    id: singleClick
                    objectName: "caacMapCenterSingleClickTimer"
                    interval: Qt.styleHints.mouseDoubleClickInterval
                    onTriggered: {
                        if (centerButton.home && centerButton.home.isValid) {
                            root.editorMap.center = centerButton.home
                        }
                    }
                }
                MouseArea {
                    anchors.fill: parent
                    acceptedButtons: Qt.LeftButton
                    onClicked: singleClick.restart()
                    onDoubleClicked: {
                        singleClick.stop()
                        if (centerButton.vehicle && centerButton.vehicle.coordinate.isValid) {
                            root.editorMap.center = centerButton.vehicle.coordinate
                        }
                    }
                }
            }
            RailButton { objectName: "caacUndo"; text: qsTr("Undo"); toolKind: "undo"; enabled: editorController.canUndo; onClicked: editorController.undo() }
            RailButton { objectName: "caacRedo"; text: qsTr("重做（Ctrl+X）"); toolKind: "redo"; enabled: editorController.canRedo; onClicked: editorController.redo() }
            RailButton { objectName: "caacMapZoomIn"; text: qsTr("Zoom in"); toolKind: "zoomIn"; enabled: root.editorMap.zoomLevel < root.editorMap.maximumZoomLevel; onClicked: root.editorMap.zoomLevel = Math.min(root.editorMap.maximumZoomLevel, root.editorMap.zoomLevel + 1) }
            RailButton { objectName: "caacMapZoomOut"; text: qsTr("Zoom out"); toolKind: "zoomOut"; enabled: root.editorMap.zoomLevel > root.editorMap.minimumZoomLevel; onClicked: root.editorMap.zoomLevel = Math.max(root.editorMap.minimumZoomLevel, root.editorMap.zoomLevel - 1) }
        }
    }

    RowLayout {
        id: panelNavigation
        width: root.width
        spacing: ScreenTools.defaultFontPixelWidth
        readonly property real labelHeight: Math.max(globalNavigation.labelHeight, relativeNavigation.labelHeight,
            templateNavigation.labelHeight, waypointNavigation.labelHeight)
        component PanelButton: QGCButton {
            id: button
            required property int panelIndex
            Layout.fillWidth: true
            Layout.minimumWidth: iconSize
            Layout.preferredWidth: Math.max(iconSize, buttonLabel.implicitWidth)
            readonly property real iconSize: Math.max(0, Math.min(ScreenTools.defaultFontPixelHeight * 2.5,
                (root.width - panelNavigation.spacing * 3) / 4))
            readonly property real labelHeight: buttonLabel.implicitHeight
            Layout.preferredHeight: iconSize + panelNavigation.labelHeight
            checked: !root.showMissionTree && root.activePanel === panelIndex
            leftPadding: 0
            rightPadding: 0
            topPadding: 0
            bottomPadding: 0
            Accessible.name: text
            ToolTip.visible: hovered
            ToolTip.text: text
            background: Rectangle {
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.top: parent.top
                width: button.iconSize
                height: width
                radius: ScreenTools.defaultFontPixelWidth * 0.7
                color: button.checked ? "#245b46" : "#d9232629"
                border.width: 1
                border.color: button.checked ? "#40d58b" : "#a0a5aa"
            }
            contentItem: Item {
                Canvas {
                    anchors.top: parent.top
                    anchors.horizontalCenter: parent.horizontalCenter
                    width: button.iconSize
                    height: width
                    onPaint: {
                        const ctx = getContext("2d")
                        ctx.reset()
                        ctx.strokeStyle = "#ffffff"
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
            }
            QGCLabel {
                id: buttonLabel
                anchors.top: parent.top
                anchors.topMargin: button.iconSize
                anchors.horizontalCenter: parent.horizontalCenter
                width: parent.width
                text: button.text
                horizontalAlignment: Text.AlignHCenter
                wrapMode: Text.Wrap
                font.pointSize: ScreenTools.defaultFontPointSize
                color: "white"
                style: Text.Outline
                styleColor: "#222222"
            }
            onClicked: {
                const wasActive = !root.showMissionTree && root.activePanel === panelIndex
                root.showMissionTree = false
                root.activePanel = wasActive ? -1 : panelIndex
            }
        }
        PanelButton { id: globalNavigation; objectName: "caacGlobalSettingsButton"; panelIndex: 0; text: qsTr("Global settings") }
        PanelButton { id: relativeNavigation; objectName: "caacRelativeEditorButton"; panelIndex: 1; text: qsTr("Relative coordinate editor") }
        PanelButton { id: templateNavigation; objectName: "caacTemplateEditorButton"; panelIndex: 2; text: qsTr("Template") }
        PanelButton { id: waypointNavigation; objectName: "caacWaypointListButton"; panelIndex: 3; text: qsTr("Points") }
    }

    Rectangle {
        id: listHeader
        visible: !root.panelsCollapsed
        width: root.width
        height: headerContent.implicitHeight + ScreenTools.defaultFontPixelHeight
        color: dragPalette.windowShade
        radius: ScreenTools.defaultBorderRadius

        CaacPanelDragHandle {
            objectName: "caacPlanningPanelDragHandle"
            anchors.fill: parent
            enabled: false
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
                    : root.showMissionTree ? qsTr("Advanced mission settings") : qsTr("Waypoint settings")
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
        id: globalSettings
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

    RowLayout {
        id: waypointBatchActions
        visible: root.activePanel === 3 && !root.showMissionTree
        width: root.width
        spacing: ScreenTools.defaultFontPixelWidth * 0.5
        QGCButton {
            objectName: "caacSelectAllWaypoints"
            Layout.fillWidth: true
            Layout.minimumWidth: 0
            pointSize: ScreenTools.smallFontPointSize
            text: root.allWaypointsSelected ? qsTr("Deselect all") : qsTr("Select all")
            enabled: root._selectableWaypoints.length > 0
            onClicked: root.selectedWaypointItems = root.allWaypointsSelected ? [] : root._selectableWaypoints.slice()
        }
        QGCButton {
            objectName: "caacDeleteSelectedWaypoints"
            Layout.fillWidth: true
            Layout.minimumWidth: 0
            pointSize: ScreenTools.smallFontPointSize
            text: qsTr("Delete selected (%1)").arg(root.selectedWaypointCount)
            enabled: root.selectedWaypointCount > 0 && root.canDeleteWaypoints
            onClicked: root.removeSelectedWaypoints()
        }
    }

    QGCListView {
        id: waypointList
        objectName: "caacWaypointList"
        width: root.width
        height: Math.max(ScreenTools.defaultFontPixelHeight * 1.5,
            root.height - panelNavigation.height - listHeader.height - waypointBatchActions.height - pointActions.height - root._transferHeight - root._settingsButtonHeight - helpLabel.height
            - root.spacing * 6)
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
            readonly property bool _batchSelected: root.selectedWaypointItems.indexOf(object) >= 0
            objectName: "caacWaypointCard_" + object.sequenceNumber
            width: waypointList.width
            height: _display ? cardContent.implicitHeight + ScreenTools.defaultFontPixelHeight : 0
            visible: _display
            radius: ScreenTools.defaultBorderRadius
            color: _batchSelected || object.isCurrentItem ? dragPalette.buttonHighlight : dragPalette.windowShade
            readonly property color _textColor: _batchSelected || object.isCurrentItem ? dragPalette.buttonHighlightText : dragPalette.text

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
                QGCCheckBox {
                    objectName: "caacSelectWaypoint_" + card.object.sequenceNumber
                    visible: card.index > 0 && card.object.command === 16
                    checked: card._batchSelected
                    onClicked: root.selectWaypointForDeletion(card.object, checked)
                }
                QGCLabel {
                    objectName: "caacWaypointNumber_" + card.object.sequenceNumber
                    text: card.object.waypointNumber
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
                                ? qsTr("%1°").arg(card._leg.angle.toFixed(0)) : "—") : ""
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
                        enabled: (card.object.command === 16 || card.object.isLandCommand) && root.canDeleteWaypoints
                        heightFactor: 0.2
                        onClicked: root.removeWaypoint(card.object)
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

    PlanToolBarIndicators {
        id: fileActions
        objectName: "caacWaypointFileActions"
        width: root.width
        visible: root.activePanel === 3 && !root.showMissionTree
        compact: true
        planMasterController: root.planMasterController
        onToolbarButtonClicked: waypointPopup.close()
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
            heightFactor: 0.25
            text: qsTr("Read waypoints")
            enabled: root._canTransfer
            onClicked: {
                waypointPopup.close()
                if (root.planMasterController.dirtyForSave) { readWaypointsFactory.open() }
                else { root.planMasterController.loadFromVehicle() }
            }
        }
    }
    CaacMissionUpdateController {
        id: missionUpdate
        objectName: "caacMissionUpdateController"
        onFinished: (success, message) => {
            if (!success) { QGroundControl.showMessageDialog(mainWindow, qsTr("Update waypoints"), message) }
        }
    }
    QGCPopupDialogFactory {
        id: deletedWaypointsFactory
        dialogComponent: Component {
            QGCPopupDialog {
                id: deletionDialog
                objectName: "caacUploadDeletedWaypointsDialog"
                title: qsTr("Upload waypoint deletion")
                buttons: Dialog.Yes | Dialog.No
                property var vehicleAtOpen: null
                property var masterAtOpen: null
                property int revisionAtOpen: -1
                readonly property bool sourceCurrent: root._canTransfer
                    && vehicleAtOpen && vehicleAtOpen.armed && vehicleAtOpen.airborne
                    && vehicleAtOpen === QGroundControl.multiVehicleManager.activeVehicle
                    && root._managerVehicle === vehicleAtOpen && root.planMasterController === masterAtOpen
                    && root._deletionSourceRevision === revisionAtOpen
                acceptButtonEnabled: sourceCurrent
                onSourceCurrentChanged: if (visible && !sourceCurrent) { close() }
                onOpened: if (!sourceCurrent) { close() }
                onClosed: if (root._deletedWaypointsPrompt === deletionDialog) { root._deletedWaypointsPrompt = null }
                QGCLabel {
                    width: Math.min(deletionDialog.maxContentAvailableWidth, ScreenTools.defaultFontPixelWidth * 42)
                    wrapMode: Text.WordWrap
                    text: qsTr("Upload the updated route and waypoint information? The aircraft will pause for the update. It will resume only if the mission was running. Choosing No keeps the edits locally.")
                }
                onAccepted: {
                    if (sourceCurrent) {
                        missionUpdate.updateDeletedWaypoints(masterAtOpen, QGroundControl.corePlugin.simulation)
                    }
                }
            }
        }
    }
    QGCPopupDialogFactory {
        id: updateWaypointsFactory
        dialogComponent: Component {
            QGCPopupDialog {
                id: updateDialog
                objectName: "caacUpdateWaypointsDialog"
                title: qsTr("Update waypoints")
                buttons: Dialog.Yes | Dialog.Cancel
                property var vehicleAtOpen: null
                property var masterAtOpen: null
                Component.onCompleted: { vehicleAtOpen = root._managerVehicle; masterAtOpen = root.planMasterController }
                QGCLabel {
                    width: Math.min(updateDialog.maxContentAvailableWidth, ScreenTools.defaultFontPixelWidth * 42)
                    wrapMode: Text.WordWrap
                    text: qsTr("Upload the new waypoint positions? The aircraft will pause, upload, and continue from its previous target waypoint. Cancelling keeps the edits locally.")
                }
                onAccepted: {
                    if (root._managerVehicle === vehicleAtOpen && root.planMasterController === masterAtOpen) {
                        missionUpdate.update(masterAtOpen, QGroundControl.corePlugin.simulation)
                    }
                }
                Connections {
                    target: QGroundControl.multiVehicleManager
                    function onActiveVehicleChanged() {
                        if (updateDialog.vehicleAtOpen !== root._managerVehicle) { updateDialog.close() }
                    }
                }
            }
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
        text: qsTr("Exit advanced mission settings")
        heightFactor: 0.25
        onClicked: {
            root.showMissionTree = false
        }
    }

    QGCLabel {
        id: helpLabel
        visible: !root.panelsCollapsed
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
        id: homeRelationsFactory
        dialogComponent: Component { CaacHomeRelationsDialog { routeController: editorController } }
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
        objectName: "caacRouteUndoShortcut"
        sequences: [StandardKey.Undo]
        context: Qt.ApplicationShortcut
        enabled: root.visible && root.enabled && editorController.canUndo
        onActivated: {
            waypointPopup.fineTuning = false
            waypointPopup.close()
            editorController.undo()
        }
    }

    Shortcut {
        objectName: "caacRouteRedoShortcut"
        sequence: "Ctrl+X"
        context: Qt.ApplicationShortcut
        enabled: root.visible && root.enabled && editorController.canRedo
            && (!mainWindow.activeFocusItem || typeof mainWindow.activeFocusItem.cut !== "function")
        onActivated: {
            waypointPopup.fineTuning = false
            waypointPopup.close()
            editorController.redo()
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
                if (editorController.updatePlannedHome(coordinate.latitude, coordinate.longitude, home.altitude, false)) {
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
            } else if (root._pickingCenter) {
                if (editorController.setRotationCenter(root.editorMap.toCoordinate(position, false))) {
                    root._pickingCenter = false
                    root.openGeometry(root._pendingOperation)
                }
            }
        }
        function onMapRightClicked(position) {
            if (root.enabled && root.visible && root.editorMap.directMissionItemEditing && !missionUpdate.busy) {
                waypointPopup.close()
                mapContextMenu.x = Math.max(0, Math.min(position.x, root.editorMap.width - mapContextMenu.width))
                mapContextMenu.y = Math.max(0, Math.min(position.y, root.editorMap.height - mapContextMenu.implicitHeight))
                mapContextMenu.open()
            }
        }
        function onMissionItemDragStarted(item) {
            waypointPopup.close()
            if (item && item.sequenceNumber === 0) {
                root.planMasterController.caacHomeDragActive = true
            }
            editorController.cancelRouteOrientation()
            root._dragStartCoordinate = item ? QtPositioning.coordinate(item.coordinate.latitude, item.coordinate.longitude, item.coordinate.altitude)
                : QtPositioning.coordinate()
            root._draggedWaypoint = item
            root._dragGuides = []
            editorController.beginEdit()
        }
        function onMissionItemDragFinished(item) {
            root.planMasterController.caacHomeDragActive = false
            if (item && item.sequenceNumber === 0 && item.coordinate.isValid) {
                editorController.refreshPlannedHomeAltitude()
                item.dirty = true
                root.planMasterController.missionController.setCurrentPlanViewSeqNum(0, true)
            }
            root._draggedWaypoint = null
            root._dragGuides = []
            editorController.endEdit()
            const vehicle = root._managerVehicle
            if (item && (item.command === 16 || item.command === 21) && root._dragStartCoordinate.isValid
                && root._dragStartCoordinate.distanceTo(item.coordinate) > 0.01 && vehicle && vehicle.armed
                && (vehicle.flightMode === vehicle.missionFlightMode || vehicle.flightMode === vehicle.pauseFlightMode)
                && !missionUpdate.busy && !root._deletedWaypointsPrompt) {
                updateWaypointsFactory.open()
            }
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
