import QtQuick
import QtLocation

import QGroundControl
import QGroundControl.Controls

/// Use to drag a MissionItemIndicator
Rectangle {
    id:             itemDragger
    x:              _itemIndicatorX - _touchMarginHorizontal
    y:              _itemIndicatorY - _touchMarginVertical
    width:          _itemIndicatorWidth + (_touchMarginHorizontal * 2)
    height:         _itemIndicatorHeight + (_touchMarginVertical * 2)
    color:          "transparent"
    z:              QGroundControl.zOrderMapItems + 1    // Above item icons

    // Properties which must be specific by consumer
    property var mapControl     ///< Map control which contains this item
    property var itemIndicator  ///< The mission item indicator to drag around
    property var itemCoordinate ///< Coordinate we are updating during drag
    property bool editOnRightClick: false
    property bool dragEnabled: true
    property var coordinateFilter: null
    readonly property bool missionItemDragger: true
    property Item gestureTarget: itemDragger
    property int gesturePriority: -1
    property bool _externalDragActive: false

    signal clicked
    signal rightClicked
    signal doubleClicked
    signal dragStart
    signal dragStop

    property bool   _preventCoordinateBindingLoop:  false

    property real _itemIndicatorX:          itemIndicator ? itemIndicator.x : 0
    property real _itemIndicatorY:          itemIndicator ? itemIndicator.y : 0
    property real _itemIndicatorWidth:      itemIndicator ? itemIndicator.width : 0
    property real _itemIndicatorHeight:     itemIndicator ? itemIndicator.height : 0
    property bool _mobile:                  ScreenTools.isMobile
    property real _touchWidth:              Math.max(_itemIndicatorWidth, ScreenTools.minTouchPixels)
    property real _touchHeight:             Math.max(_itemIndicatorHeight, ScreenTools.minTouchPixels)
    property real _touchMarginHorizontal:   _mobile ? (_touchWidth - _itemIndicatorWidth) / 2 : 0
    property real _touchMarginVertical:     _mobile ? (_touchHeight - _itemIndicatorHeight) / 2 : 0
    property bool _dragStartSignalled:      false

    onXChanged: if (!editOnRightClick) { liveDrag() }
    onYChanged: if (!editOnRightClick) { liveDrag() }

    function liveDrag(pointerPoint = null) {
        if (!itemDragger._preventCoordinateBindingLoop &&
                ((itemDrag.drag.active && gestureTarget === itemDragger) || _externalDragActive)) {
            if (!mapControl || !itemIndicator || !itemCoordinate) { return }
            itemDragger._preventCoordinateBindingLoop = true
            try {
                const point = pointerPoint || Qt.point(itemDragger.x + _touchMarginHorizontal + itemIndicator.anchorPoint.x,
                    itemDragger.y + _touchMarginVertical + itemIndicator.anchorPoint.y)
                let coordinate = mapControl.toCoordinate(point, false /* clipToViewPort */)
                if (typeof coordinateFilter === "function") {
                    coordinate = coordinateFilter(coordinate)
                }
                if (!coordinate || !coordinate.isValid) { return }
                coordinate.altitude = itemCoordinate.altitude
                if (coordinate !== itemCoordinate) { itemCoordinate = coordinate }
            } finally {
                itemDragger._preventCoordinateBindingLoop = false
            }
        }
    }

    Drag.active: itemDrag.drag.active || _externalDragActive

    QGCMouseArea {
        id:                 itemDrag
        anchors.fill:       parent
        drag.target:        itemDragger.gestureTarget && itemDragger.gestureTarget.dragEnabled && (pressedButtons & Qt.LeftButton) ? itemDragger.gestureTarget : null
        acceptedButtons:    itemDragger.editOnRightClick ? Qt.LeftButton | Qt.RightButton : Qt.LeftButton
        drag.minimumX:      0
        drag.minimumY:      0
        drag.maximumX:      itemDragger.parent.width - parent.width
        drag.maximumY:      itemDragger.parent.height - parent.height
        preventStealing:    true
        enabled:            itemDragger.visible
        cursorShape:        drag.active ? Qt.ClosedHandCursor : Qt.OpenHandCursor
        property point _anchorOffset: Qt.point(0, 0)
        property point _lastPointerPosition: Qt.point(-1, -1)

        function updatePointerCoordinate(position) {
            const target = itemDragger.gestureTarget
            if (!itemDragger.editOnRightClick || !target || !target.mapControl || !target.itemIndicator) { return }
            const point = mapToItem(target.mapControl, position)
            if (Math.hypot(point.x - _lastPointerPosition.x, point.y - _lastPointerPosition.y) < 0.01) { return }
            _lastPointerPosition = point
            target.liveDrag(Qt.point(point.x - _anchorOffset.x, point.y - _anchorOffset.y))
        }

        onPositionChanged: (mouse) => {
            if (drag.active && (pressedButtons & Qt.LeftButton)) {
                updatePointerCoordinate(Qt.point(mouse.x, mouse.y))
            }
        }

        onPressed: (mouse) => {
            itemDragger.gestureTarget = itemDragger
            if (!itemDragger.editOnRightClick || !itemDragger.itemIndicator) { return }
            const point = itemDrag.mapToItem(itemDragger.mapControl, Qt.point(mouse.x, mouse.y))
            const ownCenter = itemDragger.itemCoordinate && itemDragger.itemCoordinate.isValid
                ? itemDragger.mapControl.fromCoordinate(itemDragger.itemCoordinate, false) : null
            let nearestDistance = ownCenter ? Math.pow(point.x - ownCenter.x, 2) + Math.pow(point.y - ownCenter.y, 2) : Infinity
            // Expanded hit areas must not intercept a nearer visible marker.
            for (const other of itemDragger.mapControl.children) {
                if (other === itemDragger || other.missionItemDragger !== true || !other.visible || !other.enabled || !other.itemIndicator
                    || !other.itemCoordinate || !other.itemCoordinate.isValid || other.gesturePriority < 0
                    || point.x < other.x || point.y < other.y || point.x > other.x + other.width || point.y > other.y + other.height) {
                    continue
                }
                const center = itemDragger.mapControl.fromCoordinate(other.itemCoordinate, false)
                const distance = Math.pow(point.x - center.x, 2) + Math.pow(point.y - center.y, 2)
                if (distance + 0.01 < nearestDistance || (Math.abs(distance - nearestDistance) <= 0.01
                    && other.gesturePriority > itemDragger.gestureTarget.gesturePriority)) {
                    nearestDistance = distance
                    itemDragger.gestureTarget = other
                }
            }
            const target = itemDragger.gestureTarget
            if (target && target.itemCoordinate && target.itemCoordinate.isValid) {
                const center = target.mapControl.fromCoordinate(target.itemCoordinate, false)
                _anchorOffset = Qt.point(point.x - center.x, point.y - center.y)
            }
            _lastPointerPosition = point
        }

        onClicked: (mouse) => {
            focus = true
            if (mouse.button === Qt.RightButton) {
                if (itemDragger.gestureTarget) { itemDragger.gestureTarget.rightClicked() }
            } else {
                if (itemDragger.gestureTarget) { itemDragger.gestureTarget.clicked() }
            }
        }

        onDoubleClicked: (mouse) => {
            if (mouse.button === Qt.LeftButton && itemDragger.editOnRightClick && itemDragger.gestureTarget) {
                itemDragger.gestureTarget.doubleClicked()
            }
        }

        property bool dragActive: drag.active
        onDragActiveChanged: {
            const target = itemDragger.gestureTarget
            if (!target) { return }
            target._externalDragActive = dragActive && target !== itemDragger
            if (dragActive) {
                focus = true
                if (!_dragStartSignalled) {
                    _dragStartSignalled = true
                    target.dragStart()
                }
                _lastPointerPosition = Qt.point(-1, -1)
                updatePointerCoordinate(Qt.point(mouseX, mouseY))
            } else {
                _dragStartSignalled = false
                target.dragStop()
            }
        }
    }
}
