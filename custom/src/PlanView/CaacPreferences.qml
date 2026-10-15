pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import QGroundControl
import QGroundControl.Controls
import QGroundControl.FactControls

Item {
    id: root
    objectName: "caacPreferences"
    readonly property var plugin: QGroundControl.corePlugin
    readonly property var settings: plugin.planningPreferences
    readonly property var vehicle: QGroundControl.multiVehicleManager.activeVehicle
    readonly property bool parametersReady: QGroundControl.multiVehicleManager.parameterReadyVehicleAvailable
    QGCPalette { id: palette; colorGroupEnabled: root.enabled }
    function resetController() {
        if (controllerLoader) {
            controllerLoader.sourceComponent = null
            if (vehicle && parametersReady) { controllerLoader.sourceComponent = controllerComponent }
        }
    }
    onVehicleChanged: resetController()
    onParametersReadyChanged: resetController()
    Component.onCompleted: resetController()
    Component { id: controllerComponent; FactPanelController { } }
    Loader { id: controllerLoader }
    function parameter(names) {
        const controller = controllerLoader.item
        if (!controller || controller.vehicle !== vehicle || !parametersReady) { return null }
        for (const name of names) {
            if (controller.parameterExists(-1, name)) { return controller.getParameterFact(-1, name, false) }
        }
        return null
    }
    component SnapSwitch: RowLayout {
        id: row
        required property string settingKey
        required property string label
        Layout.fillWidth: true
        QGCLabel { text: row.label; Layout.fillWidth: true; wrapMode: Text.WordWrap }
        QGCCheckBoxSlider {
            objectName: "caacPreference_" + row.settingKey
            checked: root.settings[row.settingKey]
            onClicked: {
                const change = ({})
                change[row.settingKey] = checked
                root.plugin.updatePlanningPreferences(change)
            }
        }
    }
    component PreferenceTab: TabButton {
        id: button
        topPadding: ScreenTools.defaultFontPixelHeight * 0.5
        bottomPadding: topPadding
        contentItem: QGCLabel {
            text: button.text
            horizontalAlignment: Text.AlignHCenter
            wrapMode: Text.WordWrap
        }
        background: Rectangle {
            color: button.checked ? palette.windowShadeDark : palette.windowShade
            border.color: button.checked ? palette.text : "transparent"
            border.width: button.checked ? 1 : 0
            Rectangle {
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.bottom: parent.bottom
                height: 3
                color: palette.window
                visible: button.checked
            }
        }
    }
    component ParameterField: ColumnLayout {
        id: row
        required property string label
        required property var names
        readonly property var fact: root.parameter(names)
        property bool edited: false
        Layout.fillWidth: true
        QGCLabel { text: row.label; font.bold: true }
        QGCLabel {
            text: row.fact ? row.fact.name + " · " + row.fact.units : qsTr("This parameter is unavailable on the connected aircraft.")
            Layout.fillWidth: true
            wrapMode: Text.WordWrap
            font.pointSize: ScreenTools.smallFontPointSize
        }
        RowLayout {
            Layout.fillWidth: true
            QGCTextField {
                id: value
                Layout.fillWidth: true
                Layout.minimumWidth: 0
                enabled: !!row.fact
                text: row.fact ? row.fact.valueString : ""
                onTextEdited: row.edited = true
            }
            QGCButton {
                text: qsTr("Read")
                enabled: !!row.fact
                heightFactor: 0.15
                onClicked: {
                    row.edited = false
                    value.text = row.fact.valueString
                    root.plugin.refreshFlightPreferenceParameter(row.fact.name)
                }
            }
            QGCButton {
                text: qsTr("Write")
                enabled: !!row.fact && row.edited
                heightFactor: 0.15
                onClicked: {
                    const error = row.fact.validate(value.text, false)
                    if (error.length) { value.showValidationError(error); return }
                    value.clearValidationError()
                    row.fact.value = value.text
                    row.edited = false
                }
            }
        }
        Connections {
            target: row.fact
            function onValueChanged() { if (!row.edited) { value.text = row.fact.valueString } }
        }
    }
    ColumnLayout {
        anchors.fill: parent
        spacing: ScreenTools.defaultFontPixelHeight * 0.5
        TabBar {
            id: tabs
            Layout.fillWidth: true
            background: Rectangle { color: palette.windowShade }
            PreferenceTab { objectName: "caacPreferencesPlanningTab"; text: qsTr("Waypoint drawing") }
            PreferenceTab { objectName: "caacPreferencesUITab"; text: qsTr("Ground station UI") }
            PreferenceTab { objectName: "caacPreferencesFlightTab"; text: qsTr("Flight parameters") }
        }
        QGCFlickable {
            objectName: "caacPreferencesScroller"
            Layout.fillWidth: true
            Layout.fillHeight: true
            clip: true
            contentWidth: width
            contentHeight: contents.implicitHeight
            ColumnLayout {
                id: contents
                width: parent.width
                spacing: ScreenTools.defaultFontPixelHeight
                ColumnLayout {
                    visible: tabs.currentIndex === 0
                    Layout.fillWidth: true
                    spacing: ScreenTools.defaultFontPixelHeight * 0.6
                    Rectangle {
                        objectName: "caacDrawingSettingsFrame"
                        Layout.fillWidth: true
                        implicitHeight: drawingSettings.implicitHeight + ScreenTools.defaultFontPixelHeight
                        color: palette.window
                        border.color: palette.groupBorder
                        border.width: 1
                        radius: ScreenTools.defaultBorderRadius
                        ColumnLayout {
                            id: drawingSettings
                            x: ScreenTools.defaultFontPixelWidth
                            y: ScreenTools.defaultFontPixelHeight * 0.5
                            width: parent.width - x * 2
                            spacing: ScreenTools.defaultFontPixelHeight * 0.6
                            QGCLabel { text: qsTr("Drawing"); font.bold: true }
                            SnapSwitch { settingKey: "horizontalSnap"; label: qsTr("Horizontal alignment") }
                            SnapSwitch { settingKey: "verticalSnap"; label: qsTr("Vertical alignment") }
                            SnapSwitch { settingKey: "angleSnap"; label: qsTr("Angle snapping") }
                            RowLayout {
                                Layout.fillWidth: true
                                QGCLabel { text: qsTr("Snap angle (degrees)"); Layout.fillWidth: true }
                                QGCTextField {
                                    objectName: "caacPreferenceSnapAngle"
                                    Layout.preferredWidth: ScreenTools.defaultFontPixelWidth * 10
                                    validator: DoubleValidator { bottom: 1; top: 90; locale: "C" }
                                    text: root.settings.snapAngle.toString()
                                    onEditingFinished: root.plugin.updatePlanningPreferences({snapAngle: Number(text)})
                                }
                            }
                            SnapSwitch { settingKey: "symmetrySnap"; label: qsTr("Perpendicular bisector snapping") }
                            SnapSwitch { settingKey: "midpointSnap"; label: qsTr("Edge midpoint snapping") }
                            SnapSwitch { settingKey: "intersectionSnap"; label: qsTr("Route intersection snapping") }
                            QGCLabel {
                                Layout.fillWidth: true
                                wrapMode: Text.WordWrap
                                text: qsTr("Alignment uses nearby waypoints. Angle snapping includes perpendicular guides and multiples of the selected angle.")
                                font.pointSize: ScreenTools.smallFontPointSize
                            }
                        }
                    }
                    QGCLabel { text: qsTr("Tools"); font.bold: true }
                    QGCButton {
                        objectName: "caacPreferencesCalculator"
                        text: qsTr("Angle calculator")
                        onClicked: calculatorFactory.open()
                    }
                }
                ColumnLayout {
                    visible: tabs.currentIndex === 1
                    Layout.fillWidth: true
                    QGCLabel { text: qsTr("Theme"); font.bold: true }
                    RowLayout {
                        QGCButton {
                            objectName: "caacPreferenceThemeDark"
                            text: qsTr("Dark")
                            checked: QGroundControl.settingsManager.appSettings.indoorPalette.rawValue === 1
                            backgroundColor: checked ? palette.window : palette.button
                            showBorder: checked
                            onClicked: QGroundControl.settingsManager.appSettings.indoorPalette.rawValue = 1
                        }
                        QGCButton {
                            objectName: "caacPreferenceThemeLight"
                            text: qsTr("Light")
                            checked: QGroundControl.settingsManager.appSettings.indoorPalette.rawValue === 0
                            backgroundColor: checked ? palette.window : palette.button
                            showBorder: checked
                            onClicked: QGroundControl.settingsManager.appSettings.indoorPalette.rawValue = 0
                        }
                    }

                }
                ColumnLayout {
                    objectName: "caacPreferenceFlightParameters"
                    visible: tabs.currentIndex === 2
                    Layout.fillWidth: true
                    QGCLabel { text: qsTr("Flight parameters"); font.bold: true }
                    QGCLabel {
                        Layout.fillWidth: true
                        wrapMode: Text.WordWrap
                        text: root.parametersReady ? qsTr("Values use the aircraft parameter units and limits.") : qsTr("Connect an aircraft to read or write flight parameters.")
                    }
                    ParameterField { label: qsTr("Waypoint acceptance radius"); names: ["NAV_ACC_RAD", "WPNAV_RADIUS"] }
                    ParameterField { label: qsTr("Horizontal cruise speed"); names: ["MPC_XY_CRUISE", "WPNAV_SPEED"] }
                    ParameterField { label: qsTr("Horizontal acceleration"); names: ["MPC_ACC_HOR", "WPNAV_ACCEL"] }
                }
            }
        }
    }
    QGCPopupDialogFactory { id: calculatorFactory; dialogComponent: Component { CaacCalculator { } } }
}
