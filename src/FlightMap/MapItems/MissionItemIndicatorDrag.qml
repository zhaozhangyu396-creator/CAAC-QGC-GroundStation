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
    readonly property bool missionItemDragger: true
    property var gestureTarget: itemDragger
    property int gesturePriority: -1
    property bool _externalDragActive: false

    signal clicked
    signal rightClicked
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

    onXChanged: liveDrag()
    onYChanged: liveDrag()

    function liveDrag() {
        if (!itemDragger._preventCoordinateBindingLoop && (itemDrag.drag.active || _externalDragActive)) {
            var point = Qt.point(itemDragger.x + _touchMarginHorizontal + itemIndicator.anchorPoint.x, itemDragger.y + _touchMarginVertical + itemIndicator.anchorPoint.y)
            var coordinate = mapControl.toCoordinate(point, false /* clipToViewPort */)
            itemDragger._preventCoordinateBindingLoop = true
            coordinate.altitude = itemCoordinate.altitude
            itemCoordinate = coordinate
            itemDragger._preventCoordinateBindingLoop = false
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

        onPressed: (mouse) => {
            itemDragger.gestureTarget = itemDragger
            if (!itemDragger.editOnRightClick || !itemDragger.itemIndicator) { return }
            const point = itemDrag.mapToItem(itemDragger.mapControl, Qt.point(mouse.x, mouse.y))
            const ownCenter = itemDragger.mapControl.fromCoordinate(itemDragger.itemCoordinate, false)
            let nearestDistance = Math.pow(point.x - ownCenter.x, 2) + Math.pow(point.y - ownCenter.y, 2)
            // Expanded hit areas must not intercept a nearer visible marker.
            for (const other of itemDragger.mapControl.children) {
                if (other === itemDragger || other.missionItemDragger !== true || !other.visible || !other.enabled || !other.itemIndicator
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
        }

        onClicked: (mouse) => {
            focus = true
            if (mouse.button === Qt.RightButton) {
                if (itemDragger.gestureTarget) { itemDragger.gestureTarget.rightClicked() }
            } else {
                if (itemDragger.gestureTarget) { itemDragger.gestureTarget.clicked() }
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
            } else {
                _dragStartSignalled = false
                target.dragStop()
            }
        }
    }
}
