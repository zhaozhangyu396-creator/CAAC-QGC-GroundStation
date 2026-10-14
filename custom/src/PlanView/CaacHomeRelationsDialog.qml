pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import QGroundControl
import QGroundControl.Controls

QGCPopupDialog {
    id: root
    objectName: "caacHomeRelationsDialog"
    title: qsTr("H point and waypoint relations")
    buttons: Dialog.Ok | Dialog.Cancel
    property var routeController: null
    readonly property var _points: routeController ? routeController.coordinateItems : []
    readonly property var _targets: _points.filter(point => point.index === 0 || point.command === 16)

    function selectedIndex(combo, points) {
        return combo.currentIndex >= 0 && combo.currentIndex < points.length ? points[combo.currentIndex].index : -1
    }
    function number(text) { return text.trim().length ? Number(text) : NaN }
    function cardinalBearing() {
        const offsets = [0, 90, 180, 270]
        const primary = offsets[direction.currentIndex]
        const secondary = offsets[towards.currentIndex]
        const delta = ((secondary - primary + 540) % 360) - 180
        const angle = number(offset.text)
        return Math.abs(delta) === 90 && isFinite(angle) && angle >= 0 && angle <= 90
            ? primary + Math.sign(delta) * angle : NaN
    }
    onAccepted: {
        preventClose = !routeController || !routeController.applyWaypointRelation(
            selectedIndex(target, _targets), selectedIndex(reference, _points), selectedIndex(second, _points),
            mode.currentIndex === 1, mode.currentIndex === 2 ? cardinalBearing() : number(bearing.text),
            number(distance.text), moveAll.checked, number(takeoffHeight.text))
    }

    ColumnLayout {
        width: Math.min(root.maxContentAvailableWidth, ScreenTools.defaultFontPixelWidth * 52)
        spacing: ScreenTools.defaultFontPixelHeight * 0.5
        GridLayout {
            columns: width < ScreenTools.defaultFontPixelWidth * 35 ? 1 : 2
            Layout.fillWidth: true
            QGCLabel { text: qsTr("Takeoff height (relative, m)") }
            QGCTextField {
                id: takeoffHeight
                objectName: "caacRelationTakeoffHeight"
                Layout.fillWidth: true
                text: root.routeController ? root.routeController.simulationSettings.takeoffHeight.toString() : ""
                validator: DoubleValidator { bottom: 0; locale: "C" }
            }
            QGCLabel { text: qsTr("Position editing") }
            QGCComboBox {
                id: mode
                objectName: "caacRelationMode"
                Layout.fillWidth: true
                model: [qsTr("Bearing and distance"), qsTr("Extended waypoint line"), qsTr("Cardinal offset")]
            }
            QGCLabel { text: qsTr("Target point") }
            QGCComboBox {
                id: target
                objectName: "caacRelationTarget"
                Layout.fillWidth: true
                model: root._targets
                textRole: "label"
            }
            QGCLabel { text: qsTr("Reference point") }
            QGCComboBox {
                id: reference
                objectName: "caacRelationReference"
                Layout.fillWidth: true
                model: root._points
                textRole: "label"
                currentIndex: root._points.length > 1 ? 1 : 0
            }
            QGCLabel { text: qsTr("Point B"); visible: mode.currentIndex === 1 }
            QGCComboBox {
                id: second
                objectName: "caacRelationSecond"
                Layout.fillWidth: true
                visible: mode.currentIndex === 1
                model: root._points
                textRole: "label"
                currentIndex: root._points.length > 2 ? 2 : 0
            }
            QGCLabel { text: qsTr("Bearing (degrees)"); visible: mode.currentIndex === 0 }
            QGCTextField {
                id: bearing
                objectName: "caacRelationBearing"
                Layout.fillWidth: true
                visible: mode.currentIndex === 0
                text: "0"
                validator: DoubleValidator { locale: "C" }
            }
            QGCLabel { text: qsTr("Direction"); visible: mode.currentIndex === 2 }
            RowLayout {
                Layout.fillWidth: true
                visible: mode.currentIndex === 2
                QGCComboBox {
                    id: direction
                    objectName: "caacRelationDirection"
                    Layout.fillWidth: true
                    model: [qsTr("North"), qsTr("East"), qsTr("South"), qsTr("West")]
                    currentIndex: 1
                }
                QGCComboBox {
                    id: towards
                    objectName: "caacRelationTowards"
                    Layout.fillWidth: true
                    model: direction.model
                }
                QGCTextField {
                    id: offset
                    objectName: "caacRelationOffset"
                    Layout.preferredWidth: ScreenTools.defaultFontPixelWidth * 8
                    text: "0"
                    showUnits: true
                    unitsLabel: "°"
                    validator: DoubleValidator { bottom: 0; top: 90; locale: "C" }
                }
            }
            QGCLabel { text: qsTr("Distance (m)") }
            QGCTextField {
                id: distance
                objectName: "caacRelationDistance"
                Layout.fillWidth: true
                text: "10"
                validator: DoubleValidator { bottom: 0; locale: "C" }
            }
        }
        QGCCheckBox {
            id: moveAll
            objectName: "caacRelationMoveAll"
            text: qsTr("Move entire route")
        }
        QGCLabel {
            Layout.fillWidth: true
            wrapMode: Text.WordWrap
            text: qsTr("Confirm moves the target point. Extension starts at B and continues in the A to B direction. Cardinal offset turns the first direction towards the second by 0° to 90°.")
        }
        QGCLabel {
            Layout.fillWidth: true
            wrapMode: Text.WordWrap
            text: root.routeController ? root.routeController.lastError : ""
            visible: text.length > 0
        }
    }
}
