import QtQuick
import QtQuick.Controls
import QtLocation
import QtPositioning

import QGroundControl
import QGroundControl.Controls
import QGroundControl.FlightMap

/// Base component for mission item map visuals providing common drag/indicator infrastructure.
/// Subclasses must provide an indicatorComponent.
Item {
    id: control

    property var map ///< Map control to place item in
    property var vehicle ///< Vehicle associated with this item
    property bool interactive: true
    property MissionItemIndicatorGroup indicatorGroup

    /// Subclasses must set this to their indicator Component
    property Component indicatorComponent

    property var _missionItem: object
    property bool _itemVisualShowing: false
    property bool _dragAreaShowing: false

    signal clicked(int sequenceNumber)

    function hideItemVisuals() {
        _hideItemVisuals()
    }

    function showItemVisuals() {
        _showItemVisuals()
    }

    function hideDragArea() {
        if (_dragAreaShowing) {
            dragAreaLoader.active = false
            _dragAreaShowing = false
        }
    }

    function showDragArea() {
        if (!_dragAreaShowing) {
            dragAreaLoader.active = true
            _dragAreaShowing = true
        }
    }

    function updateDragArea() {
        const directDrag = map.directMissionItemEditing && (_missionItem.homePosition
                           || (_missionItem.isSimpleItem && !_missionItem.isStandaloneCoordinate))
        if ((_missionItem.isCurrentItem || directDrag) && map.planView
            && _missionItem.specifiesCoordinate && itemVisualLoader.item) {
            showDragArea()
        } else {
            hideDragArea()
        }
    }

    function _hideItemVisuals() {
        if (_itemVisualShowing) {
            itemVisualLoader.active = false
            _itemVisualShowing = false
        }
    }

    function _showItemVisuals() {
        if (!_itemVisualShowing) {
            itemVisualLoader.active = true
            _itemVisualShowing = true
        }
    }

    Component.onCompleted: {
        showItemVisuals()
        updateDragArea()
    }

    Connections {
        target: control.map
        function onDirectMissionItemEditingChanged() { control.updateDragArea() }
    }

    Connections {
        target: _missionItem

        function onIsCurrentItemChanged() { updateDragArea() }
        function onSpecifiesCoordinateChanged() { updateDragArea() }
    }

    Loader {
        id: dragAreaLoader

        asynchronous: true
        active: false

        sourceComponent: dragAreaComponent

        onLoaded: {
            if (item) {
                item.parent = map
            }
        }
    }

    Loader {
        id: itemVisualLoader

        asynchronous: true
        active: false

        sourceComponent: control.indicatorComponent

        onLoaded: {
            if (item) {
                item.parent = map
                map.addMapItem(item)
                updateDragArea()
            }
        }
    }

    // Control which is used to drag items
    Component {
        id: dragAreaComponent

        MissionItemIndicatorDrag {
            objectName: "missionItemDrag_" + control._missionItem.sequenceNumber
            gesturePriority: control._missionItem.sequenceNumber
            mapControl: control.map
            itemIndicator: itemVisualLoader.item
            itemCoordinate: _missionItem.coordinate
            visible: control.interactive && itemVisualLoader.item && itemVisualLoader.item.visible
            editOnRightClick: control.map.directMissionItemEditing
            dragEnabled: !control.map.missionItemDragBlocked
                && (!control.map.directMissionItemEditing || QGroundControl.corePlugin.dragWaypoints)
            coordinateFilter: function(coordinate) {
                const editor = control.map.missionItemDragEditor
                return control.map.directMissionItemEditing && editor
                    ? editor.snapDraggedWaypoint(control._missionItem, coordinate) : coordinate
            }
            onRightClicked: control.map.missionItemEditRequested(control._missionItem)
            onDragStart: {
                if (control.map.directMissionItemEditing) {
                    control.map.missionItemDragStarted(control._missionItem)
                }
                control.clicked(control._missionItem.sequenceNumber)
            }
            onDragStop: {
                if (control.map.directMissionItemEditing) {
                    control.map.missionItemDragFinished(control._missionItem)
                }
            }
            onClicked: {
                if (control.map.directMissionItemEditing) {
                    control.clicked(control._missionItem.sequenceNumber)
                    return
                }
                const indicator = itemVisualLoader.item
                if (indicator && indicator.activate) {
                    indicator.activate()
                } else {
                    control.clicked(control._missionItem.sequenceNumber)
                }
            }
            onDoubleClicked: {
                if (control.map.directMissionItemEditing) {
                    control.map.missionItemPropertiesRequested(control._missionItem)
                }
            }
            onItemCoordinateChanged: {
                if (!_missionItem || !itemCoordinate.isValid) { return }
                _missionItem.coordinate = itemCoordinate
            }
        }
    }
}
