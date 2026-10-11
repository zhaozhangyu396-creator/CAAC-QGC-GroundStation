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
    property bool showMissionTree: false
    property bool relativeEditorVisible: false
    readonly property var currentItem: editorController.currentItem
    readonly property var routeController: editorController
    property bool _pickingCenter: false
    property int _pendingOperation: 0
    property var _draggedWaypoint: null
    readonly property var _dragLeg: _draggedWaypoint && editorController.coordinateItems.length > 0
        ? editorController.waypointLeg(_draggedWaypoint) : ({})

    QGCPalette { id: dragPalette; colorGroupEnabled: root.enabled }

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

    Rectangle {
        parent: root.editorMap
        visible: root.enabled && root.visible
        z: QGroundControl.zOrderWidgets
        x: ScreenTools.defaultFontPixelWidth * 8
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
                Layout.fillWidth: true
                text: qsTr("Rotate / align")
                heightFactor: 0.25
                leftPadding: ScreenTools.defaultFontPixelWidth
                rightPadding: leftPadding
                onClicked: root.openGeometry(0)
            }
            QGCButton {
                objectName: "caacRelativePoint"
                Layout.fillWidth: true
                text: qsTr("Relative point")
                heightFactor: 0.25
                leftPadding: ScreenTools.defaultFontPixelWidth
                rightPadding: leftPadding
                onClicked: {
                    root.showMissionTree = false
                    root.relativeEditorVisible = true
                }
            }
        }
    }

    Rectangle {
        id: listHeader
        width: root.width
        height: headerContent.implicitHeight + ScreenTools.defaultFontPixelHeight
        color: dragPalette.windowShade
        radius: ScreenTools.defaultBorderRadius

        RowLayout {
            id: headerContent
            anchors.fill: parent
            anchors.margins: ScreenTools.defaultFontPixelHeight * 0.5
            QGCLabel {
                objectName: "caacPanelTitle"
                Layout.fillWidth: true
                text: root.relativeEditorVisible ? qsTr("Relative coordinate editor")
                    : root.showMissionTree ? qsTr("Mission settings") : qsTr("Waypoint list")
                font.bold: true
                wrapMode: Text.WordWrap
                horizontalAlignment: Text.AlignHCenter
            }
            QGCLabel {
                visible: !root.showMissionTree && !root.relativeEditorVisible
                text: qsTr("%1 points").arg(Math.max(0, editorController.coordinateItems.length - 1))
                font.pointSize: ScreenTools.smallFontPointSize
            }
        }
    }

    CaacRelativeEditor {
        width: root.width
        height: Math.max(ScreenTools.defaultFontPixelHeight * 8,
            root.height - listHeader.height - settingsButton.height - helpLabel.height - root.spacing * 3)
        visible: root.relativeEditorVisible
        routeController: editorController
    }

    QGCListView {
        id: waypointList
        objectName: "caacWaypointList"
        width: root.width
        height: Math.max(ScreenTools.defaultFontPixelHeight * 5,
            root.height - listHeader.height - pointActions.height - settingsButton.height - helpLabel.height
            - root.spacing * 4)
        visible: !root.showMissionTree && !root.relativeEditorVisible
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
        visible: !root.relativeEditorVisible
        width: root.width
        spacing: ScreenTools.defaultFontPixelWidth * 0.3
        QGCButton {
            objectName: "caacEditWaypoint"
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
        width: root.width
        text: root.relativeEditorVisible || root.showMissionTree ? qsTr("Waypoint list") : qsTr("Mission settings")
        heightFactor: 0.25
        onClicked: {
            if (root.relativeEditorVisible) {
                root.relativeEditorVisible = false
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
