pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import QGC
import QGroundControl
import QGroundControl.Controls

ColumnLayout {
    id: root
    objectName: "caacRouteEditor"
    implicitHeight: controls.implicitHeight + helpLabel.implicitHeight + spacing
    spacing: ScreenTools.defaultFontPixelHeight * 0.25

    property var editorMap: null
    property var planMasterController: null
    readonly property var currentItem: editorController.currentItem

    function editWaypoint(item) {
        if (editorController.selectWaypoint(item)) {
            waypointDialogFactory.open()
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

    RowLayout {
        id: controls
        Layout.fillWidth: true
        spacing: ScreenTools.defaultFontPixelWidth * 0.25

        QGCButton {
            objectName: "caacEditWaypoint"
            Layout.fillWidth: true
            text: qsTr("Edit")
            enabled: root.currentItem && root.currentItem.specifiesCoordinate
            onClicked: root.editWaypoint(root.currentItem)
        }
        QGCButton {
            objectName: "caacMoveWaypointUp"
            text: qsTr("Up")
            enabled: root.currentItem !== null
            onClicked: editorController.moveCurrentWaypoint(-1)
        }
        QGCButton {
            objectName: "caacMoveWaypointDown"
            text: qsTr("Down")
            enabled: root.currentItem !== null
            onClicked: editorController.moveCurrentWaypoint(1)
        }
        QGCButton {
            objectName: "caacRemoveWaypoint"
            text: qsTr("Delete")
            enabled: root.currentItem !== null
            onClicked: editorController.removeCurrentWaypoint()
        }
    }

    QGCLabel {
        id: helpLabel
        Layout.fillWidth: true
        wrapMode: Text.WordWrap
        font.pointSize: ScreenTools.smallFontPointSize
        text: editorController.lastError || qsTr("Double-click empty map to insert after selection. Drag with left button; right-click a waypoint to edit. Save/load in Plan File.")
    }

    QGCPopupDialogFactory {
        id: waypointDialogFactory
        dialogComponent: Component {
            CaacWaypointDialog {
                routeController: editorController
                waypoint: root.currentItem
            }
        }
    }

    Connections {
        target: root.editorMap

        function onMapDoubleClicked(position) {
            if (root.enabled && root.visible && root.editorMap.directMissionItemEditing) {
                editorController.addWaypoint(root.editorMap.toCoordinate(position, false))
            }
        }
        function onMissionItemEditRequested(item) {
            if (root.enabled && root.visible && root.editorMap.directMissionItemEditing) {
                root.editWaypoint(item)
            }
        }
    }
}
