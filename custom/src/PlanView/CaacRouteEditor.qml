pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import QGC
import QGroundControl
import QGroundControl.Controls

Column {
    id: root
    objectName: "caacRouteEditor"
    spacing: ScreenTools.defaultFontPixelHeight * 0.25

    property var editorMap: null
    property var planMasterController: null
    readonly property var currentItem: editorController.currentItem
    readonly property var routeController: editorController
    property bool _pickingCenter: false
    property int _pendingOperation: 0

    function pickRotationCenter(operation) {
        _pendingOperation = operation
        _pickingCenter = true
    }

    function openGeometry(operation) {
        geometryDialogFactory.open({operation: operation})
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

        property point _pressPosition
        property point _lastPosition
        property bool _dragged: false
        readonly property var _styleHints: Qt.styleHints

        onPressed: (mouse) => {
            _pressPosition = Qt.point(mouse.x, mouse.y)
            _lastPosition = _pressPosition
            _dragged = false
        }
        onPositionChanged: (mouse) => {
            if (!pressed) {
                return
            }
            if (!_dragged && Math.hypot(mouse.x - _pressPosition.x, mouse.y - _pressPosition.y)
                >= _styleHints.startDragDistance) {
                _dragged = true
                root.editorMap.mapPanStart()
            }
            if (_dragged) {
                root.editorMap.pan(_lastPosition.x - mouse.x, _lastPosition.y - mouse.y)
                _lastPosition = Qt.point(mouse.x, mouse.y)
            }
        }
        onReleased: {
            if (_dragged) {
                root.editorMap.mapPanStop()
            }
        }
        onCanceled: {
            if (_dragged) {
                root.editorMap.mapPanStop()
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
                root.editorMap.mapDoubleClicked(Qt.point(mouse.x, mouse.y))
            }
        }
    }

    QGCLabel {
        width: root.width
        text: qsTr("CAAC route workbench")
        font.bold: true
    }

    RowLayout {
        width: root.width
        QGCButton {
            objectName: "caacUndo"
            Layout.fillWidth: true
            text: qsTr("Undo")
            enabled: editorController.canUndo
            onClicked: editorController.undo()
        }
        QGCButton {
            objectName: "caacRedo"
            Layout.fillWidth: true
            text: qsTr("Redo")
            enabled: editorController.canRedo
            onClicked: editorController.redo()
        }
    }

    GridLayout {
        id: controls
        width: root.width
        columns: 2
        columnSpacing: ScreenTools.defaultFontPixelWidth * 0.25
        rowSpacing: ScreenTools.defaultFontPixelHeight * 0.25

        QGCButton {
            objectName: "caacEditWaypoint"
            Layout.fillWidth: true
            text: qsTr("Edit")
            enabled: root.currentItem && root.currentItem.specifiesCoordinate
            onClicked: root.editWaypoint(root.currentItem)
        }
        QGCButton {
            objectName: "caacRemoveWaypoint"
            Layout.fillWidth: true
            text: qsTr("Delete")
            enabled: root.currentItem !== null
            onClicked: editorController.removeCurrentWaypoint()
        }
        QGCButton {
            objectName: "caacMoveWaypointUp"
            Layout.fillWidth: true
            text: qsTr("Up")
            enabled: root.currentItem !== null
            onClicked: editorController.moveCurrentWaypoint(-1)
        }
        QGCButton {
            objectName: "caacMoveWaypointDown"
            Layout.fillWidth: true
            text: qsTr("Down")
            enabled: root.currentItem !== null
            onClicked: editorController.moveCurrentWaypoint(1)
        }
        QGCButton {
            objectName: "caacRotateRoute"
            Layout.fillWidth: true
            text: qsTr("Rotate / align")
            onClicked: root.openGeometry(0)
        }
        QGCButton {
            objectName: "caacRelativePoint"
            Layout.fillWidth: true
            text: qsTr("Relative point")
            onClicked: root.openGeometry(2)
        }
    }

    QGCLabel {
        id: helpLabel
        width: root.width
        wrapMode: Text.WordWrap
        font.pointSize: ScreenTools.smallFontPointSize
        text: root._pickingCenter ? qsTr("Click the map to choose a rotation centre. Esc cancels.")
            : editorController.lastError || qsTr("Double-click to add · Drag to move · Right-click to edit")
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
        enabled: root._pickingCenter
        onActivated: root._pickingCenter = false
    }

    Connections {
        target: root.editorMap

        function onMapDoubleClicked(position) {
            if (root.enabled && root.visible && root.editorMap.directMissionItemEditing && !root._pickingCenter) {
                editorController.addWaypoint(root.editorMap.toCoordinate(position, false))
            }
        }
        function onMapClicked(position) {
            if (root._pickingCenter) {
                if (editorController.setRotationCenter(root.editorMap.toCoordinate(position, false))) {
                    root._pickingCenter = false
                    root.openGeometry(root._pendingOperation)
                }
            }
        }
        function onMissionItemDragStarted(item) { editorController.beginEdit() }
        function onMissionItemDragFinished(item) { editorController.endEdit() }
        function onMissionItemEditRequested(item) {
            if (root.enabled && root.visible && root.editorMap.directMissionItemEditing) {
                root.editWaypoint(item)
            }
        }
    }
}
