pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtPositioning

import QGroundControl
import QGroundControl.Controls

QGCPopupDialog {
    id: root
    objectName: "caacGeometryDialog"
    title: operation === 2 ? qsTr("Relative coordinate editor") : qsTr("Route rotation")
    buttons: Dialog.Ok | Dialog.Cancel

    property var routeController: null
    property var editor: null
    property int operation: 0
    readonly property var _points: routeController ? routeController.coordinateItems : []
    property string _inputError: ""

    function number(value) { return value.trim().length ? Number(value) : NaN }
    function selectedIndex(combo) {
        return combo.currentIndex >= 0 && combo.currentIndex < _points.length ? _points[combo.currentIndex].index : -1
    }
    function chooseCenter(index) {
        if (index >= 0 && index < _points.length) {
            latitude.text = _points[index].coordinate.latitude.toString()
            longitude.text = _points[index].coordinate.longitude.toString()
        }
    }

    onAccepted: {
        _inputError = ""
        let success = false
        if (operation === 2) {
            success = routeController.insertRelativeWaypoint(selectedIndex(reference), selectedIndex(insertAfter),
                number(relativeAngle.text), number(distance.text), angleType.currentIndex === 1)
        } else {
            const center = QtPositioning.coordinate(number(latitude.text), number(longitude.text))
            success = operation === 0 ? routeController.rotateRouteAround(center, number(rotationAngle.text))
                : routeController.alignRouteAround(center, selectedIndex(pointA), selectedIndex(pointB), number(targetBearing.text))
        }
        preventClose = !success
    }

    ColumnLayout {
        spacing: ScreenTools.defaultFontPixelHeight * 0.5
        width: Math.min(root.maxContentAvailableWidth, ScreenTools.defaultFontPixelWidth * 46)

        QGCComboBox {
            id: operationSelector
            objectName: "caacGeometryOperation"
            Layout.fillWidth: true
            visible: root.operation !== 2
            model: [qsTr("Rotate by angle"), qsTr("Align two points")]
            currentIndex: root.operation === 2 ? -1 : root.operation
            onActivated: root.operation = currentIndex
        }

        GridLayout {
            columns: 2
            Layout.fillWidth: true
            visible: root.operation !== 2

            QGCLabel { text: qsTr("Centre point") }
            QGCComboBox {
                id: centerPoint
                objectName: "caacRotationCenterPoint"
                Layout.fillWidth: true
                model: root._points
                textRole: "label"
                onActivated: root.chooseCenter(currentIndex)
            }
            QGCLabel { text: qsTr("Centre latitude (°)") }
            QGCTextField {
                id: latitude
                objectName: "caacRotationLatitude"
                Layout.fillWidth: true
                text: root.routeController && root.routeController.rotationCenter.isValid
                    ? root.routeController.rotationCenter.latitude.toString()
                    : (root._points.length ? root._points[0].coordinate.latitude.toString() : "")
                validator: DoubleValidator { bottom: -90; top: 90; decimals: 17; locale: "C" }
            }
            QGCLabel { text: qsTr("Centre longitude (°)") }
            QGCTextField {
                id: longitude
                objectName: "caacRotationLongitude"
                Layout.fillWidth: true
                text: root.routeController && root.routeController.rotationCenter.isValid
                    ? root.routeController.rotationCenter.longitude.toString()
                    : (root._points.length ? root._points[0].coordinate.longitude.toString() : "")
                validator: DoubleValidator { bottom: -180; top: 180; decimals: 17; locale: "C" }
            }
            QGCLabel { text: qsTr("Custom centre") }
            QGCButton {
                objectName: "caacPickRotationCenter"
                Layout.fillWidth: true
                text: qsTr("Pick on map")
                onClicked: {
                    root.editor.pickRotationCenter(root.operation)
                    root.close()
                }
            }
            QGCLabel { text: qsTr("Angle (°)"); visible: root.operation === 0 }
            QGCTextField {
                id: rotationAngle
                objectName: "caacRotationAngle"
                Layout.fillWidth: true
                visible: root.operation === 0
                text: "90"
                validator: DoubleValidator { locale: "C" }
            }
            QGCLabel { text: qsTr("Direction point A"); visible: root.operation === 1 }
            QGCComboBox {
                id: pointA
                objectName: "caacAlignmentPointA"
                Layout.fillWidth: true
                visible: root.operation === 1
                model: root._points
                textRole: "label"
            }
            QGCLabel { text: qsTr("Direction point B"); visible: root.operation === 1 }
            QGCComboBox {
                id: pointB
                objectName: "caacAlignmentPointB"
                Layout.fillWidth: true
                visible: root.operation === 1
                model: root._points
                textRole: "label"
                currentIndex: root._points.length > 1 ? 1 : 0
            }
            QGCLabel { text: qsTr("Target bearing (°)"); visible: root.operation === 1 }
            QGCTextField {
                id: targetBearing
                objectName: "caacAlignmentBearing"
                Layout.fillWidth: true
                visible: root.operation === 1
                text: "0"
                validator: DoubleValidator { locale: "C" }
            }
        }

        GridLayout {
            columns: 2
            Layout.fillWidth: true
            visible: root.operation === 2

            QGCLabel { text: qsTr("Reference point") }
            QGCComboBox {
                id: reference
                objectName: "caacRelativeReference"
                Layout.fillWidth: true
                model: root._points
                textRole: "label"
            }
            QGCLabel { text: qsTr("Angle type") }
            QGCComboBox {
                id: angleType
                objectName: "caacRelativeAngleType"
                Layout.fillWidth: true
                model: [qsTr("Bearing"), qsTr("Interior angle")]
            }
            QGCLabel { text: qsTr("Angle (°)") }
            QGCTextField {
                id: relativeAngle
                objectName: "caacRelativeAngle"
                Layout.fillWidth: true
                text: "90"
                validator: DoubleValidator { locale: "C" }
            }
            QGCLabel { text: qsTr("Distance (m)") }
            QGCTextField {
                id: distance
                objectName: "caacRelativeDistance"
                Layout.fillWidth: true
                text: "100"
                validator: DoubleValidator { bottom: 0; locale: "C" }
            }
            QGCLabel { text: qsTr("Insert after") }
            QGCComboBox {
                id: insertAfter
                objectName: "caacRelativeInsertAfter"
                Layout.fillWidth: true
                model: root._points
                textRole: "label"
            }
        }

        QGCLabel {
            Layout.fillWidth: true
            wrapMode: Text.WordWrap
            text: root.operation === 2
                ? qsTr("Bearing: north 0°, east 90°. Interior angle: 90° turns left, 180° continues straight; an incoming leg is required.")
                : qsTr("Positive angles rotate clockwise. A → B follows true-north bearing. Home, takeoff and landing move unless used as the centre; heights and commands stay unchanged.")
        }
        QGCLabel {
            Layout.fillWidth: true
            wrapMode: Text.WordWrap
            text: root.routeController ? root.routeController.lastError : ""
            visible: text.length > 0
        }
    }
}
