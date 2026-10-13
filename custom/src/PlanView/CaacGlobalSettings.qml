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
    readonly property var _missionItems: editor.planMasterController ? editor.planMasterController.missionController.visualItems : null
    readonly property var _speedSection: _missionItems && _missionItems.count ? _missionItems.get(0).speedSection : null
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
            CaacSettingsSection {
                objectName: "caacToolsSection"
                title: qsTr("Planning tools")
                QGCButton {
                    objectName: "caacMissionSettings"
                    Layout.fillWidth: true
                    text: qsTr("Advanced mission settings")
                    onClicked: root.editor.openAdvancedMissionSettings()
                }
                QGCButton {
                    objectName: "caacMeasureTool"
                    Layout.fillWidth: true
                    text: qsTr("A/B measurement")
                    onClicked: root.editor.openMeasurement()
                }
                QGCButton {
                    objectName: "caacUnitConverter"
                    Layout.fillWidth: true
                    text: qsTr("Unit conversion")
                    onClicked: root.editor.openUnitConverter()
                }
            }
            CaacSettingsSection {
                objectName: "caacHomeSection"
                title: qsTr("Planned home")
                GridLayout {
                    columns: root.width < ScreenTools.defaultFontPixelWidth * 35 ? 1 : 2
                    Layout.fillWidth: true
                    rowSpacing: ScreenTools.defaultFontPixelHeight * 0.5
                    QGCLabel { text: qsTr("Longitude (°)") }
                    CaacCoordinateField {
                        id: homeLongitude
                        fieldName: "caacHomeLongitude"
                        maximumDegrees: 180
                        compact: true
                        Layout.fillWidth: true
                        Layout.minimumWidth: 0
                        value: root._home.isValid ? root._home.longitude : NaN
                    }
                    QGCLabel { text: qsTr("Latitude (°)") }
                    CaacCoordinateField {
                        id: homeLatitude
                        fieldName: "caacHomeLatitude"
                        maximumDegrees: 90
                        compact: true
                        Layout.fillWidth: true
                        Layout.minimumWidth: 0
                        value: root._home.isValid ? root._home.latitude : NaN
                    }
                    QGCLabel { text: qsTr("Home AMSL altitude (m)") }
                    NumberField { id: homeAltitude; objectName: "caacHomeAltitude"; text: root._home.isValid ? root._home.altitude.toString() : "" }
                }
                QGCButton {
                    objectName: "caacApplyHome"
                    Layout.fillWidth: true
                    text: qsTr("Save home position")
                    primary: true
                    onClicked: root.routeController.updatePlannedHome(homeLatitude.coordinate, homeLongitude.coordinate, root.number(homeAltitude.text))
                }
            }
            CaacSettingsSection {
                objectName: "caacBasicSection"
                title: qsTr("Basic settings")
                QGCCheckBox {
                    id: specifySpeed
                    objectName: "caacSpecifyMissionSpeed"
                    text: qsTr("Specify initial mission speed")
                    checked: root._speedSection ? root._speedSection.specifyFlightSpeed : false
                    enabled: root._speedSection && root._speedSection.available
                }
                NumberField {
                    id: missionSpeed
                    objectName: "caacMissionSpeed"
                    visible: specifySpeed.checked
                    text: root._speedSection ? root._speedSection.flightSpeed.rawValue.toString() : ""
                    showUnits: true
                    unitsLabel: "m/s"
                }
                QGCButton {
                    objectName: "caacApplyMissionSpeed"
                    Layout.fillWidth: true
                    text: qsTr("Save mission speed")
                    enabled: specifySpeed.enabled
                    onClicked: root.routeController.setMissionFlightSpeed(specifySpeed.checked, root.number(missionSpeed.text))
                }
                QGCLabel {
                    Layout.fillWidth: true
                    text: qsTr("Initial mission speed; later waypoint speed commands override it. Vertical speed is separate.")
                    font.pointSize: ScreenTools.smallFontPointSize
                    wrapMode: Text.WordWrap
                }
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
                GridLayout {
                    columns: root.width < ScreenTools.defaultFontPixelWidth * 35 ? 1 : 2
                    Layout.fillWidth: true
                    QGCLabel { text: qsTr("Takeoff height (relative, m)") }
                    NumberField {
                        id: simTakeoffHeight
                        objectName: "caacSimTakeoffHeight"
                        text: root.routeController.simulationSettings.takeoffHeight.toString()
                    }
                    QGCLabel { text: qsTr("Horizontal speed (m/s)") }
                    NumberField {
                        id: simHorizontalSpeed
                        objectName: "caacSimHorizontalSpeed"
                        text: root.routeController.simulationSettings.horizontalSpeed.toString()
                    }
                }
            }
            CaacSimulationSettings {
                Layout.fillWidth: true
                routeController: root.routeController
                editor: root.editor
                takeoffHeight: root.number(simTakeoffHeight.text)
                horizontalSpeed: root.number(simHorizontalSpeed.text)
            }
            CaacSettingsSection {
                objectName: "caacRotationSection"
                title: qsTr("Route rotation")
                QGCButton {
                    objectName: "caacGlobalRotate"
                    Layout.fillWidth: true
                    text: qsTr("Rotate / align two points")
                    onClicked: root.editor.openGeometry(1)
                }
            }
            CaacSettingsSection {
                objectName: "caacTranslationSection"
                title: qsTr("Route translation")
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
            CaacSettingsSection {
                objectName: "caacMapEditingSection"
                title: qsTr("Map editing and drawing aids")
                QGCCheckBox {
                    objectName: "caacGlobalDoubleClickAdd"
                    text: qsTr("Double-click to add waypoints")
                    checked: QGroundControl.corePlugin.doubleClickAddWaypoint
                    onClicked: QGroundControl.corePlugin.doubleClickAddWaypoint = checked
                }
                QGCCheckBox {
                    objectName: "caacGlobalDragWaypoints"
                    text: qsTr("Drag waypoints with the left mouse button")
                    checked: QGroundControl.corePlugin.dragWaypoints
                    onClicked: QGroundControl.corePlugin.dragWaypoints = checked
                }
                QGCCheckBox {
                    objectName: "caacGlobalShowDrawingAids"
                    text: qsTr("Show drawing aids")
                    checked: root.editor.showDrawingAids
                    onClicked: root.editor.showDrawingAids = checked
                }
                QGCButton {
                    objectName: "caacGlobalClearDrawingAids"
                    Layout.fillWidth: true
                    text: qsTr("Clear drawing aids")
                    onClicked: root.editor.clearDrawingAids()
                }
            }
        }
    }
}
