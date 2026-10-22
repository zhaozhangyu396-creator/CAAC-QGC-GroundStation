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
    property bool simulationInputsEdited: false
    function saveSimulationSettings() {
        if (!routeController.setSimulationSettings(simulationSettings.values())) { return false }
        simulationInputsEdited = false
        simulationSettings.draftDirty = false
        return true
    }
    function prepareUpload() {
        if (editor.planMasterController) {
            QGroundControl.corePlugin.initializeDefaultPlan(editor.planMasterController)
        }
        return (!simulationInputsEdited && !simulationSettings.draftDirty) || saveSimulationSettings()
    }
    readonly property var _points: routeController.coordinateItems
    readonly property var _home: _points.length ? _points[0].coordinate : QtPositioning.coordinate()
    readonly property var _missionItems: editor.planMasterController ? editor.planMasterController.missionController.visualItems : null
    readonly property var _speedSection: _missionItems && _missionItems.count ? _missionItems.get(0).speedSection : null
    readonly property string _homePositionKey: _home.isValid ? _home.latitude + ":" + _home.longitude : ""
    readonly property real _availableHomeAltitude: _home.isValid ? _home.altitude : NaN
    function fillHomeAltitude() {
        if (homeAltitude && !homeAltitude.manuallyEdited) {
            homeAltitude.text = isFinite(_availableHomeAltitude) ? _availableHomeAltitude.toString() : ""
        }
    }
    on_AvailableHomeAltitudeChanged: fillHomeAltitude()
    on_HomePositionKeyChanged: {
        if (homeAltitude) { homeAltitude.manuallyEdited = false }
        fillHomeAltitude()
    }
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
                    columns: root.width < ScreenTools.defaultFontPixelWidth * 35
                        || homeLongitude.displayDms || homeLatitude.displayDms ? 1 : 2
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
                    NumberField {
                        id: homeAltitude
                        objectName: "caacHomeAltitude"
                        property bool manuallyEdited: false
                        Component.onCompleted: root.fillHomeAltitude()
                        onTextEdited: manuallyEdited = true
                    }
                }
                QGCButton {
                    objectName: "caacApplyHome"
                    Layout.fillWidth: true
                    text: qsTr("Save home position")
                    primary: true
                    onClicked: root.routeController.updatePlannedHome(homeLatitude.coordinate, homeLongitude.coordinate,
                        root.number(homeAltitude.text), homeAltitude.manuallyEdited)
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
                QGCButton {
                    objectName: "caacSyncWaypointSpeed"
                    Layout.fillWidth: true
                    text: qsTr("Synchronize existing waypoint speeds")
                    enabled: specifySpeed.enabled
                    onClicked: root.routeController.syncWaypointDefaults({specifySpeed: specifySpeed.checked, speed: root.number(missionSpeed.text)})
                }
                QGCLabel { text: qsTr("New waypoint altitude") }
                FactTextField {
                    objectName: "caacDefaultWaypointAltitude"
                    Layout.fillWidth: true
                    fact: QGroundControl.settingsManager.appSettings.defaultMissionItemAltitude
                }
                QGCButton {
                    objectName: "caacSyncWaypointAltitude"
                    Layout.fillWidth: true
                    text: qsTr("Synchronize existing waypoint altitudes")
                    onClicked: root.routeController.syncWaypointDefaults({altitude: QGroundControl.settingsManager.appSettings.defaultMissionItemAltitude.rawValue})
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
                        onTextEdited: root.simulationInputsEdited = true
                        text: root.routeController.simulationSettings.takeoffHeight.toString()
                    }
                    QGCLabel { text: qsTr("Horizontal speed (m/s)") }
                    NumberField {
                        id: simHorizontalSpeed
                        objectName: "caacSimHorizontalSpeed"
                        onTextEdited: root.simulationInputsEdited = true
                        text: root.routeController.simulationSettings.horizontalSpeed.toString()
                    }
                }
                QGCButton {
                    objectName: "caacSaveSimulationSettings"
                    Layout.fillWidth: true
                    text: qsTr("Save simulation settings")
                    onClicked: root.saveSimulationSettings()
                }
            }
            CaacSimulationSettings {
                id: simulationSettings
                Layout.fillWidth: true
                routeController: root.routeController
                editor: root.editor
                takeoffHeight: root.number(simTakeoffHeight.text)
                horizontalSpeed: root.number(simHorizontalSpeed.text)
            }
            CaacSettingsSection {
                objectName: "caacTransformSection"
                title: qsTr("Rotate and translate")
                CaacTransformSettings {
                    Layout.fillWidth: true
                    routeController: root.routeController
                    editor: root.editor
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
