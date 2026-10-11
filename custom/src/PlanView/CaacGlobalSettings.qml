pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Layouts
import QtPositioning

import QGroundControl
import QGroundControl.Controls
import QGroundControl.FactControls

Item {
    id: root
    required property var routeController
    required property var editor
    readonly property var _points: routeController.coordinateItems
    readonly property var _home: _points.length ? _points[0].coordinate : QtPositioning.coordinate()
    function number(value) { return value.trim().length ? Number(value) : NaN }
    component NumberField: QGCTextField {
        Layout.fillWidth: true
        Layout.minimumWidth: 0
        validator: DoubleValidator { locale: "C" }
    }
    QGCFlickable {
        objectName: "caacGlobalSettingsScroller"
        anchors.fill: parent
        contentWidth: width
        contentHeight: fields.implicitHeight
        clip: true
        ColumnLayout {
            id: fields
            width: parent.width
            spacing: ScreenTools.defaultFontPixelHeight
            QGCLabel { text: qsTr("Planned home"); font.bold: true }
            GridLayout {
                columns: root.width < ScreenTools.defaultFontPixelWidth * 35 ? 1 : 2
                Layout.fillWidth: true
                rowSpacing: ScreenTools.defaultFontPixelHeight * 0.5
                QGCLabel { text: qsTr("Longitude (°)") }
                NumberField { id: homeLongitude; objectName: "caacHomeLongitude"; text: root._home.isValid ? root._home.longitude.toString() : "" }
                QGCLabel { text: qsTr("Latitude (°)") }
                NumberField { id: homeLatitude; objectName: "caacHomeLatitude"; text: root._home.isValid ? root._home.latitude.toString() : "" }
                QGCLabel { text: qsTr("Home AMSL altitude (m)") }
                NumberField { id: homeAltitude; objectName: "caacHomeAltitude"; text: root._home.isValid ? root._home.altitude.toString() : "" }
            }
            QGCButton {
                objectName: "caacApplyHome"
                Layout.fillWidth: true
                text: qsTr("Save home position")
                primary: true
                onClicked: root.routeController.updatePlannedHome(root.number(homeLatitude.text), root.number(homeLongitude.text), root.number(homeAltitude.text))
            }
            QGCLabel { text: qsTr("Basic settings"); font.bold: true }
            QGCLabel { text: qsTr("New waypoint altitude") }
            FactTextField {
                Layout.fillWidth: true
                fact: QGroundControl.settingsManager.appSettings.defaultMissionItemAltitude
            }
            QGCLabel {
                Layout.fillWidth: true
                text: qsTr("Default height applies to new waypoints. Existing waypoint heights retain their mission frames.")
                font.pointSize: ScreenTools.smallFontPointSize
                wrapMode: Text.WordWrap
            }
            QGCLabel { text: qsTr("Route rotation"); font.bold: true }
            QGCButton {
                objectName: "caacGlobalRotate"
                Layout.fillWidth: true
                text: qsTr("Rotate / align two points")
                onClicked: root.editor.openGeometry(0)
            }
            QGCLabel { text: qsTr("Route translation"); font.bold: true }
            GridLayout {
                columns: root.width < ScreenTools.defaultFontPixelWidth * 35 ? 1 : 2
                Layout.fillWidth: true
                rowSpacing: ScreenTools.defaultFontPixelHeight * 0.5
                QGCLabel { text: qsTr("Translation method") }
                QGCComboBox {
                    id: method
                    objectName: "caacTranslateMethod"
                    Layout.fillWidth: true
                    Layout.minimumWidth: 0
                    model: [qsTr("Specified coordinates"), qsTr("Precise movement")]
                }
                QGCLabel { text: qsTr("Reference point"); visible: method.currentIndex === 0 }
                QGCComboBox {
                    id: reference
                    objectName: "caacTranslateReference"
                    Layout.fillWidth: true
                    Layout.minimumWidth: 0
                    model: root._points
                    textRole: "label"
                    visible: method.currentIndex === 0
                }
                QGCLabel { text: qsTr("Longitude (°)"); visible: method.currentIndex === 0 }
                NumberField {
                    id: longitude
                    objectName: "caacTranslateLongitude"
                    text: reference.currentIndex >= 0 && reference.currentIndex < root._points.length ? root._points[reference.currentIndex].coordinate.longitude.toString() : ""
                    visible: method.currentIndex === 0
                }
                QGCLabel { text: qsTr("Latitude (°)"); visible: method.currentIndex === 0 }
                NumberField {
                    id: latitude
                    objectName: "caacTranslateLatitude"
                    text: reference.currentIndex >= 0 && reference.currentIndex < root._points.length ? root._points[reference.currentIndex].coordinate.latitude.toString() : ""
                    visible: method.currentIndex === 0
                }
                QGCLabel { text: qsTr("Bearing (°)"); visible: method.currentIndex === 1 }
                NumberField { id: angle; objectName: "caacTranslateBearing"; text: "0"; visible: method.currentIndex === 1 }
                QGCLabel { text: qsTr("Distance (m)"); visible: method.currentIndex === 1 }
                NumberField { id: distance; objectName: "caacTranslateDistance"; text: "0"; visible: method.currentIndex === 1 }
            }
            QGCButton {
                objectName: "caacTranslateApply"
                Layout.fillWidth: true
                text: qsTr("Translate route")
                enabled: root._points.length > 1
                primary: true
                onClicked: {
                    if (method.currentIndex === 1) {
                        root.routeController.moveRouteBy(root.number(angle.text), root.number(distance.text))
                    } else if (reference.currentIndex >= 0 && reference.currentIndex < root._points.length) {
                        root.routeController.translateRoute(root._points[reference.currentIndex].coordinate,
                            QtPositioning.coordinate(root.number(latitude.text), root.number(longitude.text)))
                    }
                }
            }
            QGCCheckBox {
                objectName: "caacRouteDragToggle"
                text: qsTr("Drag entire route")
                checked: root.editor.routeDragMode
                onClicked: root.editor.routeDragMode = checked
            }
            QGCLabel {
                Layout.fillWidth: true
                text: qsTr("Translation and route dragging include home, takeoff and landing points. Complex patterns must first be converted to simple items.")
                font.pointSize: ScreenTools.smallFontPointSize
                wrapMode: Text.WordWrap
            }
        }
    }
}
