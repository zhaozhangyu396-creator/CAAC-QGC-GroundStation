pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import QGroundControl
import QGroundControl.Controls

Item {
    id: root
    required property var routeController
    readonly property bool _compact: width < ScreenTools.defaultFontPixelWidth * 38
    readonly property var _points: routeController ? routeController.coordinateItems : []
    readonly property var _waypoints: _points.filter(point => point.index > 0 && point.command === 16)
    property var _referenceItem: null
    property var _insertAfterItem: null
    property var _overwriteItem: null

    component FieldLabel: QGCLabel {
        Layout.fillWidth: root._compact
        Layout.minimumWidth: 0
        Layout.preferredWidth: root._compact ? -1 : root.width * 0.35
        Layout.maximumWidth: root._compact ? Number.POSITIVE_INFINITY : root.width * 0.35
        wrapMode: Text.WordWrap
    }

    function selectedIndex(combo) {
        return combo.currentIndex >= 0 && combo.currentIndex < _points.length ? _points[combo.currentIndex].index : -1
    }

    function itemAt(index) {
        const plan = routeController ? routeController.planMasterController : null
        return plan && index >= 0 ? plan.missionController.visualItems.get(index) : null
    }

    function itemIndex(item) {
        const index = item ? _points.findIndex(point => itemAt(point.index) === item) : -1
        return index >= 0 ? index : 0
    }

    function restoreSelections() {
        reference.currentIndex = itemIndex(_referenceItem)
        insertAfter.currentIndex = itemIndex(_insertAfterItem)
        const index = _waypoints.findIndex(point => itemAt(point.index) === _overwriteItem)
        overwritePoint.currentIndex = index >= 0 ? index : (_waypoints.length ? 0 : -1)
    }

    function addWaypoint() {
        if (target.currentIndex > 0) {
            return routeController.setRelativeEndpoint(selectedIndex(reference),
                angle.text.trim().length ? Number(angle.text) : NaN,
                distance.text.trim().length ? Number(distance.text) : NaN, target.currentIndex === 2,
                angleType.currentIndex === 1)
        }
        if (generationMethod.currentIndex === 1) {
            const index = overwritePoint.currentIndex
            return routeController.insertRelativeWaypoint(selectedIndex(reference),
                index >= 0 && index < _waypoints.length ? _waypoints[index].index : -1,
                angle.text.trim().length ? Number(angle.text) : NaN,
                distance.text.trim().length ? Number(distance.text) : NaN, angleType.currentIndex === 1, true)
        }
        const afterIndex = selectedIndex(insertAfter)
        const added = routeController.insertRelativeWaypoint(selectedIndex(reference), afterIndex,
            angle.text.trim().length ? Number(angle.text) : NaN,
            distance.text.trim().length ? Number(distance.text) : NaN, angleType.currentIndex === 1)
        if (added) {
            _referenceItem = itemAt(afterIndex + 1)
            _insertAfterItem = _referenceItem
            restoreSelections()
        }
        return added
    }

    QGCPalette { id: qgcPal; colorGroupEnabled: root.enabled }

    QGCFlickable {
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: addButton.top
        anchors.bottomMargin: ScreenTools.defaultFontPixelHeight
        contentHeight: fields.implicitHeight
        contentWidth: width
        clip: true

        ColumnLayout {
            id: fields
            width: parent.width
            spacing: ScreenTools.defaultFontPixelHeight

            QGCLabel {
                Layout.fillWidth: true
                text: qsTr("Basic settings")
                font.bold: true
            }
            GridLayout {
                Layout.fillWidth: true
                columns: root._compact ? 1 : 2
                rowSpacing: ScreenTools.defaultFontPixelHeight
                columnSpacing: ScreenTools.defaultFontPixelWidth

                FieldLabel { text: qsTr("Target point") }
                QGCComboBox {
                    id: target
                    objectName: "caacRelativeTarget"
                    Layout.fillWidth: true
                    Layout.minimumWidth: 0
                    model: [qsTr("Waypoint"), qsTr("Takeoff point"), qsTr("Landing point")]
                }
                FieldLabel { text: qsTr("Reference point") }
                QGCComboBox {
                    id: reference
                    objectName: "caacRelativeReference"
                    Layout.fillWidth: true
                    Layout.minimumWidth: 0
                    model: root._points
                    textRole: "label"
                    onModelChanged: Qt.callLater(root.restoreSelections)
                    onActivated: root._referenceItem = root.itemAt(root.selectedIndex(reference))
                }
                FieldLabel { text: qsTr("Angle type") }
                QGCComboBox {
                    id: angleType
                    objectName: "caacRelativeAngleType"
                    Layout.fillWidth: true
                    Layout.minimumWidth: 0
                    model: [qsTr("Bearing"), qsTr("Interior angle")]
                }
                FieldLabel { text: qsTr("Angle (°)") }
                QGCTextField {
                    id: angle
                    objectName: "caacRelativeAngle"
                    Layout.fillWidth: true
                    Layout.minimumWidth: 0
                    text: "0"
                    color: qgcPal.text
                    selectionColor: qgcPal.buttonHighlight
                    selectedTextColor: qgcPal.buttonHighlightText
                    background: Rectangle {
                        color: qgcPal.windowShadeDark
                        radius: ScreenTools.defaultBorderRadius
                        implicitWidth: ScreenTools.implicitTextFieldWidth
                        implicitHeight: ScreenTools.implicitTextFieldHeight
                    }
                    validator: DoubleValidator { locale: "C" }
                }
                FieldLabel { text: qsTr("Distance (m)") }
                QGCTextField {
                    id: distance
                    objectName: "caacRelativeDistance"
                    Layout.fillWidth: true
                    Layout.minimumWidth: 0
                    text: "0"
                    color: qgcPal.text
                    selectionColor: qgcPal.buttonHighlight
                    selectedTextColor: qgcPal.buttonHighlightText
                    background: Rectangle {
                        color: qgcPal.windowShadeDark
                        radius: ScreenTools.defaultBorderRadius
                        implicitWidth: ScreenTools.implicitTextFieldWidth
                        implicitHeight: ScreenTools.implicitTextFieldHeight
                    }
                    validator: DoubleValidator { bottom: 0; locale: "C" }
                }
            }
            QGCLabel {
                Layout.fillWidth: true
                text: qsTr("Advanced settings")
                font.bold: true
            }
            GridLayout {
                Layout.fillWidth: true
                columns: root._compact ? 1 : 2
                rowSpacing: ScreenTools.defaultFontPixelHeight
                columnSpacing: ScreenTools.defaultFontPixelWidth
                FieldLabel { text: qsTr("Generation method") }
                QGCComboBox {
                    id: generationMethod
                    objectName: "caacRelativeGenerationMethod"
                    visible: target.currentIndex === 0
                    Layout.fillWidth: true
                    Layout.minimumWidth: 0
                    model: [qsTr("Insert"), qsTr("Overwrite")]
                }
                QGCLabel { text: qsTr("Set position"); visible: target.currentIndex > 0; Layout.fillWidth: true }
                FieldLabel { text: qsTr("Insert after"); visible: insertAfter.visible }
                QGCComboBox {
                    id: insertAfter
                    objectName: "caacRelativeInsertAfter"
                    visible: target.currentIndex === 0 && generationMethod.currentIndex === 0
                    Layout.fillWidth: true
                    Layout.minimumWidth: 0
                    model: root._points
                    textRole: "label"
                    onModelChanged: Qt.callLater(root.restoreSelections)
                    onActivated: root._insertAfterItem = root.itemAt(root.selectedIndex(insertAfter))
                }
                FieldLabel { text: qsTr("Overwrite waypoint"); visible: overwritePoint.visible }
                QGCComboBox {
                    id: overwritePoint
                    objectName: "caacRelativeOverwritePoint"
                    visible: target.currentIndex === 0 && generationMethod.currentIndex === 1
                    Layout.fillWidth: true
                    Layout.minimumWidth: 0
                    model: root._waypoints
                    textRole: "label"
                    onModelChanged: Qt.callLater(root.restoreSelections)
                    onActivated: (index) => {
                        root._overwriteItem = index >= 0 && index < root._waypoints.length
                            ? root.itemAt(root._waypoints[index].index) : null
                    }
                }
            }
            QGCLabel {
                Layout.fillWidth: true
                wrapMode: Text.WordWrap
                font.pointSize: ScreenTools.smallFontPointSize
                text: qsTr("Bearing: north 0°, east 90°. Interior angle: 90° turns left, 180° continues straight; an incoming leg is required.")
            }
            QGCLabel {
                Layout.fillWidth: true
                wrapMode: Text.WordWrap
                text: target.currentIndex > 0 ? qsTr("Set the selected endpoint from the reference point, angle and distance.")
                    : generationMethod.currentIndex === 1 ? qsTr("Overwrite changes the selected waypoint position and preserves its altitude and settings.")
                    : qsTr("Press Space to add a waypoint")
                font.pointSize: ScreenTools.smallFontPointSize
            }
        }
    }

    QGCButton {
        id: addButton
        objectName: "caacRelativeAdd"
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        text: target.currentIndex > 0 ? (target.currentIndex === 1 ? qsTr("Set takeoff point") : qsTr("Set landing point"))
            : generationMethod.currentIndex === 1 ? qsTr("Overwrite") : qsTr("Add")
        enabled: root._points.length > 0 && (target.currentIndex > 0 || generationMethod.currentIndex === 0 || root._waypoints.length > 0)
        primary: true
        onClicked: root.addWaypoint()
    }

    Shortcut {
        sequence: "Space"
        enabled: root.visible && root.enabled && addButton.enabled && !angle.activeFocus && !distance.activeFocus
            && !target.popup.visible && !reference.popup.visible && !angleType.popup.visible && !insertAfter.popup.visible
            && !generationMethod.popup.visible && !overwritePoint.popup.visible
        onActivated: root.addWaypoint()
    }
}
