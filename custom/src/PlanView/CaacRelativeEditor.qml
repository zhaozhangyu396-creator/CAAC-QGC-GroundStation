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

    function selectedIndex(combo) {
        return combo.currentIndex >= 0 && combo.currentIndex < _points.length ? _points[combo.currentIndex].index : -1
    }

    function addWaypoint() {
        return routeController.insertRelativeWaypoint(selectedIndex(reference), selectedIndex(insertAfter),
            angle.text.trim().length ? Number(angle.text) : NaN,
            distance.text.trim().length ? Number(distance.text) : NaN, angleType.currentIndex === 1)
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

                QGCLabel { text: qsTr("Reference point") }
                QGCComboBox {
                    id: reference
                    objectName: "caacRelativeReference"
                    Layout.fillWidth: true
                    Layout.minimumWidth: 0
                    model: root._points
                    textRole: "label"
                }
                QGCLabel { text: qsTr("Angle type") }
                QGCComboBox {
                    id: angleType
                    objectName: "caacRelativeAngleType"
                    Layout.fillWidth: true
                    Layout.minimumWidth: 0
                    model: [qsTr("Bearing"), qsTr("Interior angle")]
                }
                QGCLabel { text: qsTr("Angle (°)") }
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
                QGCLabel { text: qsTr("Distance (m)") }
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
                QGCLabel { text: qsTr("Generation method") }
                QGCLabel { text: qsTr("Insert"); Layout.fillWidth: true }
                QGCLabel { text: qsTr("Insert after") }
                QGCComboBox {
                    id: insertAfter
                    objectName: "caacRelativeInsertAfter"
                    Layout.fillWidth: true
                    Layout.minimumWidth: 0
                    model: root._points
                    textRole: "label"
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
                text: qsTr("Press Space to add a waypoint")
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
        text: qsTr("Add")
        enabled: root._points.length > 0
        primary: true
        onClicked: root.addWaypoint()
    }

    Shortcut {
        sequence: "Space"
        enabled: root.visible && root.enabled && addButton.enabled && !angle.activeFocus && !distance.activeFocus
            && !reference.popup.visible && !angleType.popup.visible && !insertAfter.popup.visible
        onActivated: root.addWaypoint()
    }
}
