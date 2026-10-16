import QtQuick
import QtQuick.Controls
import QtQuick.Dialogs
import QtLocation
import QtPositioning
import QtQuick.Layouts
import QtQuick.Window

import QGroundControl
import QGroundControl.FlightMap
import QGroundControl.Controls
import QGroundControl.FactControls
import QGroundControl.FlyView
import QGroundControl.Geo
import QGroundControl.Toolbar
import CAAC.Plan

Item {
    id: _root

    readonly property int   _decimalPlaces: 8
    readonly property real  _margin: ScreenTools.defaultFontPixelHeight * 0.5
    readonly property real  _toolsMargin: ScreenTools.defaultFontPixelWidth * 0.75
    readonly property real  _rightPanelWidth: Math.min(width / 3, ScreenTools.defaultFontPixelWidth
        * (QGroundControl.corePlugin.directMissionItemEditing ? 46 : 30))

    property var    _planMasterController: planMasterController
    property var    _missionController: _planMasterController.missionController
    property var    _geoFenceController: _planMasterController.geoFenceController
    property var    _rallyPointController: _planMasterController.rallyPointController
    property var    _visualItems: _missionController.visualItems
    property bool   _singleComplexItem: _missionController.complexMissionItems.length === 1
    property int    _editingLayer: _layerMission
    property var    _appSettings: QGroundControl.settingsManager.appSettings
    property var    _planViewSettings: QGroundControl.settingsManager.planViewSettings
    readonly property bool unifiedFlightPlan: QGroundControl.corePlugin.directMissionItemEditing
    readonly property bool _simulationEmergency: unifiedFlightPlan && QGroundControl.corePlugin.simulation.emergencyMode
    function collapsePlanningPanels() { rightPanel.collapsePanels() }
    function togglePlanningSimulation() { editorMap.simulationRequested() }
    property bool unifiedStatisticsVisible: false
    property bool safetyFenceVisible: true
    function centerMap(coordinate, zoom) {
        if (coordinate && coordinate.isValid) { editorMap.center = coordinate; editorMap.zoomLevel = zoom }
    }
    function togglePlanningStatistics() {
        if (unifiedFlightPlan) { unifiedStatisticsVisible = !unifiedStatisticsVisible }
        else { _planViewSettings.showMissionItemStatus.rawValue = !_planViewSettings.showMissionItemStatus.rawValue }
    }
    property bool   _promptForPlanUsageShowing: false
    property bool   _addWaypointOnClick: false

    readonly property int _layerMission: PlanEditLayers.layerMission
    readonly property int _layerFence: PlanEditLayers.layerFence
    readonly property int _layerRally: PlanEditLayers.layerRally

    onVisibleChanged: {
        if(visible) {
            editorMap.zoomLevel = QGroundControl.flightMapZoom
            editorMap.center    = QGroundControl.flightMapPosition
        }
    }

    Connections {
        target: planToolBar
        function onToolbarButtonClicked() {
            _addWaypointOnClick = false
        }
    }

    function mapCenter() {
        var coordinate = editorMap.center
        coordinate.latitude  = coordinate.latitude.toFixed(_decimalPlaces)
        coordinate.longitude = coordinate.longitude.toFixed(_decimalPlaces)
        coordinate.altitude  = coordinate.altitude.toFixed(_decimalPlaces)
        return coordinate
    }

    MapFitFunctions {
        id: mapFitFunctions  // The name for this id cannot be changed without breaking references outside of this code. Beware!
        map: editorMap
        usePlannedHomePosition: true
        planMasterController: _planMasterController
    }

    PlanMasterController {
        id: planMasterController
        flyView: false

        Component.onCompleted: {
            _planMasterController.start()
            _missionController.setCurrentPlanViewSeqNum(0, true)
        }

        onPromptForPlanUsageOnVehicleChange: {
            if (QGroundControl.corePlugin.keepCurrentPlanOnVehicleChange(_planMasterController)) {
                return
            }
            if (!_promptForPlanUsageShowing) {
                _promptForPlanUsageShowing = true
                promptForPlanUsageOnVehicleChangePopupFactory.open()
            }
        }

        function waitingOnIncompleteDataMessage(save) {
            var saveOrUpload = save ? qsTr("Save") : qsTr("Upload")
            QGroundControl.showMessageDialog(_root, qsTr("Unable to %1").arg(saveOrUpload), qsTr("Plan has incomplete items. Complete all items and %1 again.").arg(saveOrUpload))
        }

        function waitingOnTerrainDataMessage(save) {
            var saveOrUpload = save ? qsTr("Save") : qsTr("Upload")
            QGroundControl.showMessageDialog(_root, qsTr("Unable to %1").arg(saveOrUpload), qsTr("Plan is waiting on terrain data from server for correct altitude values."))
        }

        function checkReadyForSaveUpload(save) {
            if (readyForSaveState() == VisualMissionItem.NotReadyForSaveData) {
                waitingOnIncompleteDataMessage(save)
                return false
            } else if (readyForSaveState() == VisualMissionItem.NotReadyForSaveTerrain) {
                waitingOnTerrainDataMessage(save)
                return false
            }
            return true
        }

        function upload() {
            if (_missionController.sendToVehiclePreCheck() === MissionController.SendToVehiclePreCheckStateActiveMission) {
                QGroundControl.showMessageDialog(_root, qsTr("上传航线"), qsTr("当前航点任务正在执行，请先暂停当前任务，再上传新航线。"))
                return
            }
            if (!rightPanel.prepareUpload()) { return }
            if (!checkReadyForSaveUpload(false /* save */)) {
                return
            }
            const simulation = QGroundControl.corePlugin.simulation
            if (simulation && simulation.active && simulation.vehicle === _planMasterController.managerVehicle) {
                simulation.upload(_planMasterController)
                return
            }
            switch (_missionController.sendToVehiclePreCheck()) {
                case MissionController.SendToVehiclePreCheckStateOk: sendToVehicle()
                    break
                case MissionController.SendToVehiclePreCheckStateActiveMission: QGroundControl.showMessageDialog(_root, qsTr("Send To Vehicle"), qsTr("Current mission must be paused prior to uploading a new Plan"))
                    break
                case MissionController.SendToVehiclePreCheckStateFirwmareVehicleMismatch: QGroundControl.showMessageDialog(_root, qsTr("Plan Upload"),
                                                 qsTr("This Plan was created for a different firmware or vehicle type than the firmware/vehicle type of vehicle you are uploading to. " +
                                                      "This can lead to errors or incorrect behavior. " +
                                                      "It is recommended to recreate the Plan for the correct firmware/vehicle type.\n\n" +
                                                      "Click 'Ok' to upload the Plan anyway."),
                                                 Dialog.Ok | Dialog.Cancel,
                                                 function() { _planMasterController.sendToVehicle() })
                    break
            }
        }

        function loadFromSelectedFile() {
            fileDialog.title =          qsTr("Select Plan File")
            fileDialog.planFiles =      true
            fileDialog.nameFilters =    _planMasterController.loadNameFilters
            fileDialog.openForLoad()
        }

        function saveToSelectedFile() {
            if (!checkReadyForSaveUpload(true /* save */)) {
                return
            }
            fileDialog.title =          qsTr("Save Plan")
            fileDialog.planFiles =      true
            fileDialog.nameFilters =    _planMasterController.saveNameFilters
            fileDialog.openForSave()
        }

        function fitViewportToItems() {
            mapFitFunctions.fitMapViewportToMissionItems()
        }

        function saveKmlToSelectedFile() {
            if (!checkReadyForSaveUpload(true /* save */)) {
                return
            }
            fileDialog.title =          qsTr("Save KML")
            fileDialog.planFiles =      false
            fileDialog.nameFilters =    ShapeFileHelper.fileDialogKMLFilters
            fileDialog.openForSave()
        }
    }

    Connections {
        target: _missionController

        function onNewItemsFromVehicle() {
            if (_visualItems && _visualItems.count !== 1) {
                mapFitFunctions.fitMapViewportToMissionItems()
            }
            _missionController.setCurrentPlanViewSeqNum(0, true)
        }
    }

    function insertSimpleItemAfterCurrent(coordinate) {
        var nextIndex = _missionController.currentPlanViewVIIndex + 1
        _missionController.insertSimpleMissionItem(coordinate, nextIndex, true /* makeCurrentItem */)
    }

    function insertComplexItemAfterCurrent(complexItemName) {
        var nextIndex = _missionController.currentPlanViewVIIndex + 1
        _missionController.insertComplexMissionItem(complexItemName, mapCenter(), nextIndex, true /* makeCurrentItem */)
    }

    function insertTakeoffItemAfterCurrent() {
        var nextIndex = _missionController.currentPlanViewVIIndex + 1
        _missionController.insertTakeoffItem(mapCenter(), nextIndex, true /* makeCurrentItem */)
    }

    function insertVTOLMulticopterTakeoffItemAfterCurrent() {
        var nextIndex = _missionController.currentPlanViewVIIndex + 1
        _missionController.insertVTOLMulticopterTakeoffItem(mapCenter(), nextIndex, true /* makeCurrentItem */)
    }

    function insertLandItemAfterCurrent() {
        var nextIndex = _missionController.currentPlanViewVIIndex + 1
        _missionController.insertLandItem(mapCenter(), nextIndex, true /* makeCurrentItem */)
    }

    function _landButtonText() {
        // Must mirror MissionController::insertLandItem: only fixed-wing/VTOL get landing patterns
        if (!_planMasterController.controllerVehicle.fixedWing && !_planMasterController.controllerVehicle.vtol) {
            return qsTr("Return")
        }
        if (_missionController.isInsertLandValid && _missionController.hasLandItem) {
            return qsTr("Alt Land")
        }
        return qsTr("Land")
    }

    QGCFileDialog {
        id: fileDialog
        folder: _appSettings ? _appSettings.missionSavePath : ""

        property bool planFiles: true    ///< true: working with plan files, false: working with kml file

        onAcceptedForSave: (file) => {
            if (planFiles) {
                if (_planMasterController.saveToFile(file)) {
                    close()
                }
            } else {
                _planMasterController.saveToKml(file)
                close()
            }
        }

        onAcceptedForLoad: (file) => {
            _planMasterController.loadFromFile(file)
            _planMasterController.fitViewportToItems()
            _missionController.setCurrentPlanViewSeqNum(0, true)
            close()
        }
    }

    PlanViewToolBar {
        id: planToolBar
        visible: !_root.unifiedFlightPlan
        planMasterController: _planMasterController
        showRallyPointsHelp: _editingLayer === _layerRally
    }
    FlyViewToolBar {
        visible: _root.unifiedFlightPlan
        guidedValueSlider: unifiedValueSlider
    }
    GuidedValueSlider {
        id: unifiedValueSlider
        objectName: "guidedValueSlider"
        visible: false
        anchors.right: parent.right
        anchors.top: planToolBar.bottom
        anchors.bottom: parent.bottom
        z: QGroundControl.zOrderTopMost
    }
    Binding {
        target: globals.guidedControllerFlyView
        property: "guidedValueSlider"
        value: unifiedValueSlider
        when: _root.unifiedFlightPlan
    }

    Item {
        id: mainPlanViewArea
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: planToolBar.bottom
        anchors.bottom: parent.bottom

        FlightMap {
            id: editorMap
            objectName: "planView_map"
            visible: !_root._simulationEmergency
            property var planMasterController: _root._planMasterController
            readonly property var mapToolsContainer: _root.unifiedFlightPlan ? unifiedFlightLayer.mapToolsContainer : null
            anchors.fill: parent
            mapName: "MissionEditor"
            allowGCSLocationCenter: true
            allowVehicleLocationCenter: true
            planView: true
            directMissionItemEditing: QGroundControl.corePlugin.directMissionItemEditing
                                      && _editingLayer === _layerMission

            zoomLevel: QGroundControl.flightMapZoom
            center: QGroundControl.flightMapPosition

            // This is the center rectangle of the map which is not obscured by tools
            property rect centerViewport: Qt.rect(_leftToolWidth + _margin,  _margin, editorMap.width - _leftToolWidth - _rightToolWidth - (_margin * 2), (missionStatus.visible ? missionStatus.y : height - _margin) - _margin)

            property real _leftToolWidth: toolStrip.x + toolStrip.width
            property real _rightToolWidth: rightPanel.width + rightPanel.anchors.rightMargin
            property real _nonInteractiveOpacity: 0.5

            // Initial map position duplicates Fly view position
            Component.onCompleted: editorMap.center = QGroundControl.flightMapPosition

            onZoomLevelChanged: {
                QGroundControl.flightMapZoom = editorMap.zoomLevel
            }
            onCenterChanged: {
                QGroundControl.flightMapPosition = editorMap.center
            }

            onMapClicked: (mouse) => {
                // Take focus to close any previous editing
                editorMap.focus = true

                // Collapse layer switcher on any map click
                layerSwitcher.expanded = false
                collapseTimer.stop()
                collapsePlanningPanels()

                if (!mainWindow.allowViewSwitch()) {
                    return
                }
                var coordinate = editorMap.toCoordinate(Qt.point(mouse.x, mouse.y), false /* clipToViewPort */)
                coordinate.latitude = coordinate.latitude.toFixed(_decimalPlaces)
                coordinate.longitude = coordinate.longitude.toFixed(_decimalPlaces)
                coordinate.altitude = coordinate.altitude.toFixed(_decimalPlaces)

                switch (_editingLayer) {
                case _layerMission:
                    if (editorMap.directMissionItemEditing) {
                        break
                    }
                    if (_planMasterController.showCreateFromTemplate) {
                        _missionController.setHomePosition(coordinate)
                    } else if (_addWaypointOnClick) {
                        insertSimpleItemAfterCurrent(coordinate)
                    }
                    break
                case _layerRally:
                    if (_rallyPointController.supported) {
                        _rallyPointController.addPoint(coordinate)
                    }
                    break
                }
            }

            MissionItemIndicatorGroup {
                id: _missionItemIndicatorGroup

                map: editorMap
                missionItems: _root._missionController.visualItems
                onItemSelected: (sequenceNumber) => {
                    _root._missionController.setCurrentPlanViewSeqNum(sequenceNumber, false)
                }
            }

            // Add the mission item visuals to the map
            Repeater {
                model: _missionController.visualItems
                delegate: MissionItemMapVisual {
                    map: editorMap
                    opacity: _editingLayer == _layerMission ? 1 : editorMap._nonInteractiveOpacity
                    interactive: _editingLayer == _layerMission
                    vehicle: _planMasterController.controllerVehicle
                    indicatorGroup: _missionItemIndicatorGroup
                    onClicked: (sequenceNumber) => {
                        _root._missionController.setCurrentPlanViewSeqNum(sequenceNumber, false)
                    }
                }
            }

            // Add lines between waypoints
            MapPolyline {
                id: flightTrajectory
                objectName: "caacFlightTrajectory"
                readonly property var vehicle: QGroundControl.multiVehicleManager.activeVehicle
                line.width: ScreenTools.defaultFontPixelWidth * 0.3
                line.color: QGroundControl.globalPalette.colorRed
                z: QGroundControl.zOrderTrajectoryLines
                visible: _root.unifiedFlightPlan
                onVehicleChanged: path = vehicle ? vehicle.trajectoryPoints.list() : []
                Component.onCompleted: path = vehicle ? vehicle.trajectoryPoints.list() : []
                Connections {
                    target: flightTrajectory.vehicle ? flightTrajectory.vehicle.trajectoryPoints : null
                    function onPointAdded(coordinate) { flightTrajectory.addCoordinate(coordinate) }
                    function onUpdateLastPoint(coordinate) {
                        if (flightTrajectory.pathLength() > 0) {
                            flightTrajectory.replaceCoordinate(flightTrajectory.pathLength() - 1, coordinate)
                        }
                    }
                    function onPointsCleared() { flightTrajectory.path = [] }
                }
            }
            MissionLineView {
                showSpecialVisual: _missionController.isROIBeginCurrentItem
                model: rightPanel.simulationPreviewActive ? null : _missionController.simpleFlightPathSegments
                opacity: _editingLayer == _layerMission ? 1 : editorMap._nonInteractiveOpacity
            }

            // Direction arrows in waypoint lines
            MapItemView {
                model: !editorMap.directMissionItemEditing && _editingLayer == _layerMission
                    && !rightPanel.simulationPreviewActive ? _missionController.directionArrows : undefined

                delegate: MapLineArrow {
                    fromCoord: object ? object.coordinate1 : undefined
                    toCoord: object ? object.coordinate2 : undefined
                    arrowPosition: 3
                    mapControl: editorMap
                    z: QGroundControl.zOrderWaypointLines + 1
                }
            }

            HoverHandler {
                id: routeHover
                acceptedDevices: PointerDevice.Mouse | PointerDevice.TouchPad
            }

            MapQuickItem {
                id: routeSplitItem
                readonly property var points: rightPanel.routeController ? rightPanel.routeController.coordinateItems : []
                readonly property var segments: {
                    const model = _missionController.visualItems
                    const legs = []
                    let previous = null
                    for (const point of points) {
                        if (point.index < 0 || point.index >= model.count || !point.coordinate.isValid) { continue }
                        if (point.index === 0) { previous = point; continue }
                        const item = model.get(point.index)
                        if (item.isTakeoffItem) { previous = point; continue }
                        if (item.command !== 16) { continue }
                        if (previous) { legs.push({ from: previous, to: point, after: point.index - 1 }) }
                        previous = point
                    }
                    if (rightPanel.simulationPreviewActive) {
                        const previewLegs = []
                        for (const segment of rightPanel.simulationPreviewSegments) {
                            const from = points.find(point => point.coordinate.isValid
                                && point.coordinate.distanceTo(segment.coordinate1) < 0.01)
                            const to = points.find(point => point.coordinate.isValid
                                && point.coordinate.distanceTo(segment.coordinate2) < 0.01)
                            if (!from || !to || from.index === to.index) { continue }
                            const after = to.index === 0 || segment.kind === "closure" || segment.kind === "loop"
                                || segment.kind === "return" ? from.index : to.index - 1
                            previewLegs.push({from: from, to: to, after: after})
                        }
                        return previewLegs
                    }
                    return legs
                }
                readonly property var hoveredSegment: {
                    void editorMap.center
                    void editorMap.zoomLevel
                    void editorMap.bearing
                    if (!routeHover.hovered || !rightPanel.routeInsertionEnabled || _editingLayer !== _layerMission) { return null }
                    const pointer = routeHover.point.position
                    const radius = ScreenTools.defaultFontPixelWidth
                    let nearest = null
                    let closest = radius
                    for (const segment of segments) {
                        const from = editorMap.fromCoordinate(segment.from.coordinate, false)
                        const to = editorMap.fromCoordinate(segment.to.coordinate, false)
                        const dx = to.x - from.x
                        const dy = to.y - from.y
                        const length = Math.hypot(dx, dy)
                        if (length <= _missionItemIndicatorGroup.groupingDistance * 2) { continue }
                        const ratio = ((pointer.x - from.x) * dx + (pointer.y - from.y) * dy) / (length * length)
                        if (ratio < 0 || ratio > 1) { continue }
                        const offset = Math.hypot(pointer.x - from.x - ratio * dx, pointer.y - from.y - ratio * dy)
                        if (offset <= closest) { closest = offset; nearest = segment }
                    }
                    return nearest
                }
                readonly property var metrics: hoveredSegment
                    ? QGroundControl.corePlugin.routeLegMetrics(hoveredSegment.from.coordinate, hoveredSegment.to.coordinate) : ({})
                coordinate: metrics.midpoint || QtPositioning.coordinate()
                anchorPoint.x: sourceItem.width / 2
                anchorPoint.y: sourceItem.height / 2
                z: QGroundControl.zOrderWaypointLines + 1
                visible: !!hoveredSegment && metrics.distance > 0
                sourceItem: SplitIndicator {
                    objectName: "caacRouteSplitButton"
                    enabled: !_planMasterController.syncInProgress
                    onClicked: {
                        const segment = routeSplitItem.hoveredSegment
                        if (segment && routeSplitItem.metrics.distance > 0) {
                            rightPanel.routeController.insertRelativeWaypoint(segment.from.index, segment.after,
                                routeSplitItem.metrics.bearing, routeSplitItem.metrics.distance / 2, false)
                        }
                    }
                }
            }

            // UI for splitting the current segment
            MapQuickItem {
                id: splitSegmentItem

                property real _screenLegLength: 0

                anchorPoint.x: sourceItem.width / 2
                anchorPoint.y: sourceItem.height / 2
                z: QGroundControl.zOrderWaypointLines + 1
                visible: !QGroundControl.corePlugin.directMissionItemEditing
                         && _editingLayer == _layerMission
                         && !rightPanel.simulationPreviewActive
                         && _screenLegLength > _missionItemIndicatorGroup.groupingDistance * 2

                sourceItem: SplitIndicator {
                    onClicked: _missionController.insertSimpleMissionItem(splitSegmentItem.coordinate,
                                                                           _missionController.currentPlanViewVIIndex,
                                                                           true /* makeCurrentItem */)
                }

                function _updateScreenLegLength() {
                    const segment = _root._missionController.splitSegment
                    if (segment && segment.coordinate1.isValid && segment.coordinate2.isValid) {
                        const fromPoint = editorMap.fromCoordinate(segment.coordinate1, false /* clipToViewPort */)
                        const toPoint = editorMap.fromCoordinate(segment.coordinate2, false /* clipToViewPort */)
                        _screenLegLength = Math.hypot(toPoint.x - fromPoint.x, toPoint.y - fromPoint.y)
                    } else {
                        _screenLegLength = 0
                    }
                }

                function _updateSplitCoord() {
                    if (_missionController.splitSegment) {
                        var distance = _missionController.splitSegment.coordinate1.distanceTo(_missionController.splitSegment.coordinate2)
                        var azimuth = _missionController.splitSegment.coordinate1.azimuthTo(_missionController.splitSegment.coordinate2)
                        splitSegmentItem.coordinate = _missionController.splitSegment.coordinate1.atDistanceAndAzimuth(distance / 2, azimuth)
                    } else {
                        coordinate = QtPositioning.coordinate()
                    }
                    _updateScreenLegLength()
                }

                Connections {
                    target: _missionController
                    function onSplitSegmentChanged()  { splitSegmentItem._updateSplitCoord() }
                }

                Connections {
                    target: _missionController.splitSegment
                    function onCoordinate1Changed()   { splitSegmentItem._updateSplitCoord() }
                    function onCoordinate2Changed()   { splitSegmentItem._updateSplitCoord() }
                }

                Connections {
                    target: editorMap
                    function onCenterChanged() { splitSegmentItem._updateScreenLegLength() }
                    function onZoomLevelChanged() { splitSegmentItem._updateScreenLegLength() }
                }
            }

            // Add the vehicles to the map
            MapItemView {
                model: QGroundControl.multiVehicleManager.vehicles
                delegate: VehicleMapItem {
                    objectName: "planView_vehicle_" + (vehicle ? vehicle.id : "")
                    vehicle: object
                    readonly property var planningSimulation: QGroundControl.corePlugin.directMissionItemEditing
                        ? QGroundControl.corePlugin.simulation : null
                    readonly property bool previewHome: vehicle && planningSimulation && planningSimulation.vehicle === vehicle
                        && !planningSimulation.hasFlown && !vehicle.airborne && !vehicle.armed
                        && _missionController.plannedHomePosition.isValid
                    coordinate: previewHome ? _missionController.plannedHomePosition
                        : vehicle ? vehicle.coordinate : QtPositioning.coordinate()
                    map: editorMap
                    size: ScreenTools.defaultFontPixelHeight * 3
                    z: QGroundControl.zOrderMapItems + (previewHome ? 0.75 : -1)
                }
            }

            GeoFenceMapVisuals {
                objectName: "planView_geoFenceMapVisuals"
                visible: !_root.unifiedFlightPlan || _root.safetyFenceVisible || _editingLayer === _layerFence
                map: editorMap
                myGeoFenceController: _geoFenceController
                interactive: _editingLayer == _layerFence
                homePosition: _missionController.plannedHomePosition
                planView: true
                opacity: _editingLayer != _layerFence ? editorMap._nonInteractiveOpacity : 1
            }

            RallyPointMapVisuals {
                objectName: "planView_rallyPointMapVisuals"
                map: editorMap
                myRallyPointController: _rallyPointController
                interactive: _editingLayer == _layerRally
                planView: true
                opacity: _editingLayer != _layerRally ? editorMap._nonInteractiveOpacity : 1
            }

        }

        //-----------------------------------------------------------
        // Left tool strip
        ToolStrip {
            id: toolStrip
            fontSize: ScreenTools.menuFontPointSize * (editorMap.directMissionItemEditing ? 0.9 : 1)
            anchors.margins: _toolsMargin
            anchors.left: parent.left
            anchors.top: parent.top
            z: QGroundControl.zOrderWidgets
            maxHeight: parent.height - toolStrip.y
            visible: _editingLayer == _layerMission && !editorMap.directMissionItemEditing

            property bool _isMissionLayer: _editingLayer == _layerMission

            Binding {
                target: waypointButton
                property: "checked"
                value: _addWaypointOnClick
            }

            ToolStripActionList {
                id: toolStripActionList
                model: [
                    ToolStripAction {
                        objectName: "planToolStrip_takeoffButton"
                        text: qsTr("Takeoff")
                        iconSource: "/res/takeoff.svg"
                        enabled: _missionController.isInsertTakeoffValid
                        visible: toolStrip._isMissionLayer && !_planMasterController.controllerVehicle.rover
                        dropPanelComponent: _planMasterController.controllerVehicle.supports.vtolMulticopterTakeoff
                                            ? vtolTakeoffDropPanel
                                            : null
                        onTriggered: {
                            insertTakeoffItemAfterCurrent()
                        }
                    },
                    ToolStripAction {
                        objectName: "planToolStrip_patternButton"
                        text: _singleComplexItem ? _missionController.complexMissionItems[0].translatedName : qsTr("Pattern")
                        iconSource: "/qmlimages/MapDrawShape.svg"
                        enabled: _missionController.flyThroughCommandsAllowed
                        visible: toolStrip._isMissionLayer
                        dropPanelComponent: _singleComplexItem ? undefined : patternDropPanel
                        onTriggered: {
                            if (_singleComplexItem) {
                                insertComplexItemAfterCurrent(_missionController.complexMissionItems[0].canonicalName)
                            }
                        }
                    },
                    ToolStripAction {
                        id: waypointButton
                        objectName: "planToolStrip_waypointButton"
                        text: qsTr("Waypoint")
                        iconSource: "/res/waypoint.svg"
                        enabled: _missionController.flyThroughCommandsAllowed
                        visible: false
                        checkable: true
                        onTriggered: {
                            _addWaypointOnClick = !_addWaypointOnClick
                            if (_addWaypointOnClick) {
                                // Arming an insert tool leaves template-creation mode, otherwise
                                // map clicks keep moving the home position instead of inserting
                                _planMasterController.userSelectedManualCreation = true
                            }
                        }
                    },
                    ToolStripAction {
                        objectName: "planToolStrip_homeCenterButton"
                        text: qsTr("Takeoff\ncenter")
                        iconSource: "/qmlimages/roi.svg"
                        enabled: _missionController.plannedHomePosition.isValid
                        visible: toolStrip._isMissionLayer
                        onTriggered: {
                            _addWaypointOnClick = false
                            if (editorMap.directMissionItemEditing && rightPanel.routeController) {
                                rightPanel.routeController.centerHomeOnRoute()
                            } else if (_missionController.plannedHomePosition.isValid) {
                                editorMap.center = _missionController.plannedHomePosition
                            }
                        }
                    },
                    ToolStripAction {
                        objectName: "planToolStrip_simulationButton"
                        readonly property bool simulationActive: QGroundControl.corePlugin.directMissionItemEditing
                            && QGroundControl.corePlugin.simulation.active
                        text: simulationActive ? qsTr("Cancel simulation") : qsTr("Simulation")
                        iconSource: simulationActive ? "/res/XDelete.svg" : "/InstrumentValueIcons/play.svg"
                        visible: QGroundControl.corePlugin.directMissionItemEditing && toolStrip._isMissionLayer
                        enabled: visible
                        onTriggered: editorMap.simulationRequested()
                    },
                    ToolStripAction {
                        objectName: "planToolStrip_landButton"
                        text: _landButtonText()
                        iconSource: "/res/rtl.svg"
                        enabled: _missionController.isInsertLandValid
                        visible: toolStrip._isMissionLayer
                        onTriggered: {
                            insertLandItemAfterCurrent()
                        }
                    },
                    ToolStripAction {
                        text: qsTr("Stats")
                        iconSource: "/res/chevron-double-right.svg"
                        visible: missionStatus.hidden && QGroundControl.corePlugin.options.showMissionStatus
                        onTriggered: missionStatus.showMissionStatus()
                    }
                ]
            }

            model: toolStripActionList.model
        }

        MapScale {
            anchors.margins: _toolsMargin
            anchors.left: editorMap.directMissionItemEditing ? undefined : toolStrip.right
            anchors.right: editorMap.directMissionItemEditing
                ? (rightPanel.panelsCollapsed ? parent.right : rightPanel.left) : undefined
            anchors.top: editorMap.directMissionItemEditing ? undefined : parent.top
            anchors.bottom: editorMap.directMissionItemEditing ? parent.bottom : undefined
            z: QGroundControl.zOrderWidgets + 4
            mapControl: editorMap
            autoHide: true
        }

        PlanViewRightPanel {
            id: rightPanel
            visible: !_root._simulationEmergency
            anchors.top: parent.top
            anchors.bottom: parent.bottom
            anchors.right: parent.right
            width: _rightPanelWidth
            planMasterController: _planMasterController
            editorMap: editorMap
            onEditingLayerChangeRequested: (layer) => _editingLayer = layer
        }

        // Layer switching icons — only active icon visible; click to expand choices leftward
        Item {
            id:                     layerSwitcher
            objectName:             "planView_layerSwitcher"
            anchors.right:          rightPanel.left
            anchors.rightMargin:    _toolsMargin
            anchors.top:            parent.top
            anchors.topMargin:      _toolsMargin
            width:                  layerRow.width
            height:                 _layerButtonSize
            z:                      QGroundControl.zOrderWidgets
            visible:                rightPanel.showNativeEditor && !_planMasterController.showCreateFromTemplate

            property bool   expanded: false
            property real   _layerButtonSize: ScreenTools.defaultFontPixelHeight * 2.0
            property real   _spacing: ScreenTools.defaultFontPixelHeight * 0.25

            Timer {
                id: collapseTimer
                interval: 5000
                onTriggered: layerSwitcher.expanded = false
            }

            function toggle() {
                expanded = !expanded
                if (expanded) {
                    collapseTimer.restart()
                } else {
                    collapseTimer.stop()
                }
            }

            function choose(nodeType) {
                expanded = false
                collapseTimer.stop()
                rightPanel.selectLayer(nodeType)
            }

            // Row laid out right-to-left: active icon on the right, choices expand left
            Row {
                id:             layerRow
                anchors.right:  parent.right
                spacing:        layerSwitcher._spacing
                layoutDirection: Qt.RightToLeft

                // Active layer button (always visible)
                Rectangle {
                    objectName: "layerSwitcher_activeButton"
                    width:  layerSwitcher._layerButtonSize
                    height: width
                    radius: ScreenTools.defaultBorderRadius
                    color:  QGroundControl.globalPalette.buttonHighlight

                    QGCColoredImage {
                        anchors.centerIn:   parent
                        width:              parent.width * 0.6
                        height:             width
                        source:             PlanEditLayers.layerInfos.find(l => l.layer === _editingLayer)?.icon ?? "/res/waypoint.svg"
                        color:              QGroundControl.globalPalette.buttonHighlightText
                    }

                    QGCMouseArea {
                        anchors.fill: parent
                        onClicked:    layerSwitcher.toggle()
                    }
                }

                // Choice buttons (only layers that are NOT the current one)
                Repeater {
                    model: PlanEditLayers.layerInfos.filter(l => l.layer !== _editingLayer)

                    Rectangle {
                        required property var modelData
                        objectName: "layerSwitcher_choice_" + modelData.nodeType
                        width:   choiceRow.implicitWidth + layerSwitcher._spacing * 2
                        height:  layerSwitcher._layerButtonSize
                        radius:  ScreenTools.defaultBorderRadius
                        color:   QGroundControl.globalPalette.button
                        visible: opacity > 0
                        opacity: layerSwitcher.expanded ? 1 : 0

                        Behavior on opacity { NumberAnimation { duration: 150 } }

                        Row {
                            id:                     choiceRow
                            anchors.centerIn:       parent
                            spacing:                layerSwitcher._spacing

                            QGCColoredImage {
                                anchors.verticalCenter: parent.verticalCenter
                                width:                  layerSwitcher._layerButtonSize * 0.6
                                height:                 width
                                source:                 modelData.icon
                                color:                  QGroundControl.globalPalette.buttonText
                            }

                            QGCLabel {
                                anchors.verticalCenter: parent.verticalCenter
                                text:                   modelData.name
                                color:                  QGroundControl.globalPalette.buttonText
                            }
                        }

                        QGCMouseArea {
                            anchors.fill: parent
                            onClicked:    layerSwitcher.choose(modelData.nodeType)
                        }
                    }
                }
            }
        }

        Rectangle {
            anchors.fill: parent
            color: "black"
            objectName: "caacEmergencyBlackout"
            visible: _root._simulationEmergency
            z: QGroundControl.zOrderWidgets + 2
        }
        CaacUnifiedFlightLayer {
            id: unifiedFlightLayer
            anchors.fill: parent
            visible: _root.unifiedFlightPlan
            z: QGroundControl.zOrderWidgets + 3
            mapControl: editorMap
            planningPaneExpanded: !rightPanel.panelsCollapsed
            planningPanelWidth: rightPanel.width
            planningNavigationHeight: rightPanel.navigationHeight
        }

        RowLayout {
            id: missionStatus
            objectName: "planView_statistics"
            z: QGroundControl.zOrderWidgets + 4
            anchors.margins: _toolsMargin
            anchors.left: _root.unifiedFlightPlan ? undefined : _calcLeftAnchor()
            anchors.right: _root.unifiedFlightPlan ? undefined : rightPanel.left
            anchors.bottom: _root.unifiedFlightPlan ? undefined : parent.bottom
            width: _root.unifiedFlightPlan ? Math.max(0, rightPanel.x - _toolsMargin * 2) : implicitWidth
            x: Math.max(-width + ScreenTools.defaultFontPixelWidth * 8,
                Math.min(_toolsMargin + _dragOffset.x, parent.width - ScreenTools.defaultFontPixelWidth * 8))
            y: Math.max(_toolsMargin, Math.min(parent.height - unifiedFlightLayer.telemetryHeight - height - _toolsMargin + _dragOffset.y,
                parent.height - height - _toolsMargin))
            property point _dragOffset: Qt.point(0, 0)
            spacing: 0
            anchors.bottomMargin: _root.unifiedFlightPlan ? unifiedFlightLayer.telemetryHeight + _toolsMargin : _toolsMargin
            visible: !_root._simulationEmergency && !hidden && _editingLayer == _layerMission && QGroundControl.corePlugin.options.showMissionStatus

            readonly property bool hidden: _root.unifiedFlightPlan ? !_root.unifiedStatisticsVisible : !_planViewSettings.showMissionItemStatus.rawValue

            function showMissionStatus() {
                if (_root.unifiedFlightPlan) { _root.unifiedStatisticsVisible = true }
                else { _planViewSettings.showMissionItemStatus.rawValue = true }
            }

            function _calcLeftAnchor() {
                let bottomOfToolStrip = toolStrip.y + toolStrip.height
                let largestStatsHeight = Math.max(terrainStatus.height, missionStats.height)
                if (toolStrip.visible && bottomOfToolStrip + largestStatsHeight > parent.height - missionStatus.anchors.margins) {
                    return toolStrip.right
                }
                return parent.left
            }

            function _toggleMissionStatusVisibility() {
                _root.togglePlanningStatistics()
            }

            DragHandler {
                objectName: "caacStatisticsDragHandler"
                enabled: _root.unifiedFlightPlan
                target: null
                acceptedButtons: Qt.LeftButton
                property point _startOffset
                onActiveChanged: if (active) { _startOffset = missionStatus._dragOffset }
                onActiveTranslationChanged: if (active) {
                    missionStatus._dragOffset = Qt.point(_startOffset.x + activeTranslation.x, _startOffset.y + activeTranslation.y)
                }
            }
            TapHandler {
                enabled: _root.unifiedFlightPlan
                acceptedButtons: Qt.RightButton
                onTapped: (eventPoint) => statisticsMenu.popup(eventPoint.position.x, eventPoint.position.y)
            }
            Menu {
                id: statisticsMenu
                MenuItem {
                    objectName: "caacStatisticsClose"
                    text: qsTr("关闭")
                    onTriggered: _root.unifiedStatisticsVisible = false
                }
            }

            ColumnLayout {
                id: missionStatsButtonLayout
                Layout.alignment: Qt.AlignBottom
                spacing: 0

                property real _buttonImplicitWidth: ScreenTools.defaultFontPixelHeight * 1.5
                property real _buttonImageMargins: _buttonImplicitWidth * 0.15

                Rectangle {
                    id: terrainButton
                    implicitWidth: missionStatsButtonLayout._buttonImplicitWidth
                    implicitHeight: implicitWidth
                    color: checked ? QGroundControl.globalPalette.buttonHighlight : QGroundControl.globalPalette.button

                    property bool checked: true

                    QGCColoredImage {
                        anchors.margins: missionStatsButtonLayout._buttonImageMargins
                        anchors.fill: parent
                        source: "/res/terrain.svg"
                        color: parent.checked ? QGroundControl.globalPalette.buttonHighlightText : QGroundControl.globalPalette.buttonText
                    }

                    QGCMouseArea {
                        anchors.fill: parent
                        onClicked: {
                            terrainButton.checked = true
                            missionStatsButton.checked = false
                        }
                    }
                }

                Rectangle {
                    id: missionStatsButton
                    implicitWidth: missionStatsButtonLayout._buttonImplicitWidth
                    implicitHeight: implicitWidth
                    color: checked ? QGroundControl.globalPalette.buttonHighlight : QGroundControl.globalPalette.button

                    property bool checked: false

                    QGCColoredImage {
                        anchors.margins: missionStatsButtonLayout._buttonImageMargins
                        anchors.fill: parent
                        source: "/res/sliders.svg"
                        color: parent.checked ? QGroundControl.globalPalette.buttonHighlightText : QGroundControl.globalPalette.buttonText
                    }

                    QGCMouseArea {
                        anchors.fill: parent
                        onClicked: {
                            missionStatsButton.checked = true
                            terrainButton.checked = false
                        }
                    }
                }

                Rectangle {
                    id: bottomStatusOpenCloseButton
                    implicitWidth: missionStatsButtonLayout._buttonImplicitWidth
                    implicitHeight: implicitWidth
                    color: QGroundControl.globalPalette.button

                    QGCColoredImage {
                        anchors.margins: missionStatsButtonLayout._buttonImageMargins
                        anchors.fill: parent
                        source: "/res/chevron-double-left.svg"
                        color: QGroundControl.globalPalette.buttonText
                    }

                    QGCMouseArea {
                        anchors.fill: parent
                        onClicked: missionStatus._toggleMissionStatusVisibility()
                    }
                }
            }

            TerrainStatus {
                id: terrainStatus
                Layout.alignment: Qt.AlignBottom
                Layout.fillWidth: true
                height: ScreenTools.defaultFontPixelHeight * 7
                missionController: _missionController
                visible: terrainButton.checked
                onSetCurrentSeqNum: _missionController.setCurrentPlanViewSeqNum(seqNum, true)
            }

            MissionStats {
                id: missionStats
                Layout.alignment: Qt.AlignBottom
                Layout.fillWidth: true
                visible: missionStatsButton.checked
                planMasterController: _root._planMasterController
            }
        }
    }

        //- ToolStrip ToolStripDropPanel Components

    Component {
        id: patternDropPanel

        ColumnLayout {
            spacing: ScreenTools.defaultFontPixelWidth * 0.5

            QGCLabel { text: qsTr("Create complex pattern:") }

            Repeater {
                model: _missionController.complexMissionItems

                QGCButton {
                    text: modelData.translatedName
                    Layout.fillWidth: true

                    onClicked: {
                        insertComplexItemAfterCurrent(modelData.canonicalName)
                        dropPanel.hide()
                    }
                }
            }
        } // Column
    }

    Component {
        id: vtolTakeoffDropPanel

        ColumnLayout {
            spacing: ScreenTools.defaultFontPixelWidth * 0.5

            QGCLabel { text: qsTr("Choose takeoff mode:") }

            QGCButton {
                objectName: "planTakeoff_vtolButton"
                text: qsTr("VTOL takeoff")
                Layout.fillWidth: true
                primary: true

                onClicked: {
                    insertTakeoffItemAfterCurrent()
                    dropPanel.hide()
                }
            }

            QGCButton {
                objectName: "planTakeoff_mcButton"
                text: qsTr("Multicopter takeoff")
                Layout.fillWidth: true

                onClicked: {
                    insertVTOLMulticopterTakeoffItemAfterCurrent()
                    dropPanel.hide()
                }
            }
        }
    }

    QGCPopupDialogFactory {
        id: promptForPlanUsageOnVehicleChangePopupFactory

        dialogComponent: promptForPlanUsageOnVehicleChangePopupComponent
    }

    Component {
        id: promptForPlanUsageOnVehicleChangePopupComponent
        QGCPopupDialog {
            title: _planMasterController.managerVehicle.isOfflineEditingVehicle ? qsTr("Plan View - Vehicle Disconnected") : qsTr("Plan View - Vehicle Changed")
            buttons: Dialog.NoButton

            ColumnLayout {
                QGCLabel {
                    Layout.maximumWidth: parent.width
                    wrapMode: QGCLabel.WordWrap
                    text: _planMasterController.managerVehicle.isOfflineEditingVehicle ?
                                                qsTr("The vehicle associated with the plan in the Plan View is no longer available. What would you like to do with that plan?") : qsTr("The plan being worked on in the Plan View is not from the current vehicle. What would you like to do with that plan?")
                }

                QGCButton {
                    Layout.fillWidth: true
                    text: (_planMasterController.dirtyForSave) ?
                                            (_planMasterController.managerVehicle.isOfflineEditingVehicle ?
                                                 qsTr("Discard Unsaved Changes") : qsTr("Discard Unsaved Changes, Load New Plan From Vehicle")) : qsTr("Load New Plan From Vehicle")
                    onClicked: {
                        _planMasterController.showPlanFromManagerVehicle()
                        _promptForPlanUsageShowing = false
                        close();
                    }
                }

                QGCButton {
                    Layout.fillWidth: true
                    text: _planMasterController.managerVehicle.isOfflineEditingVehicle ?
                                            qsTr("Keep Current Plan") : qsTr("Keep Current Plan, Don't Update From Vehicle")
                    onClicked: {
                        _promptForPlanUsageShowing = false
                        close()
                    }
                }
            }
        }
    }

}
