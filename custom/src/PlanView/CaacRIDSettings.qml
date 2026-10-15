pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtPositioning

import QGroundControl
import QGroundControl.Controls
import QGroundControl.FactControls

Item {
    id: root
    objectName: "caacRIDSettings"
    readonly property var settings: QGroundControl.settingsManager.remoteIDSettings
    readonly property var vehicle: QGroundControl.multiVehicleManager.activeVehicle
    readonly property var manager: vehicle ? vehicle.remoteIDManager : null
    readonly property bool china: settings.region.rawValue === 2
    readonly property bool eu: settings.region.rawValue === 1
    readonly property var home: vehicle ? vehicle.homePosition
        : globals.planMasterControllerPlanView.missionController.plannedHomePosition
    readonly property var position: settings.locationType.rawValue === 0 ? home
        : QGroundControl.positionManager.gcsPosition
    readonly property string serial: manager ? manager.aircraftSerial : ""
    readonly property var declaration: settings.selfIDType.rawValue === 1 ? settings.selfIDEmergency
        : settings.selfIDType.rawValue === 2 ? settings.selfIDExtended : settings.selfIDFree
    QGCPalette { id: palette }

    component SectionTitle: RowLayout {
        id: heading
        required property string title
        Layout.fillWidth: true
        Rectangle {
            Layout.preferredWidth: ScreenTools.defaultFontPixelWidth * 0.4
            Layout.preferredHeight: ScreenTools.defaultFontPixelHeight * 1.1
            radius: width / 2
            color: palette.settingsAccent
        }
        QGCLabel { text: heading.title; font.bold: true; Layout.fillWidth: true; wrapMode: Text.WordWrap }
    }
    component Divider: Rectangle {
        Layout.fillWidth: true
        Layout.preferredHeight: 1
        color: palette.windowShade
        Layout.topMargin: ScreenTools.defaultFontPixelHeight
        Layout.bottomMargin: Layout.topMargin
    }
    component Choice: RowLayout {
        id: row
        required property string label
        required property Fact setting
        QGCLabel { text: row.label; Layout.preferredWidth: root.width * 0.34; wrapMode: Text.WordWrap }
        FactComboBox {
            objectName: row.objectName + "_value"
            fact: row.setting
            indexModel: false
            Layout.fillWidth: true
            Layout.minimumWidth: 0
        }
    }
    component Entry: RowLayout {
        id: row
        required property string label
        required property Fact setting
        property alias field: editor
        QGCLabel { text: row.label; Layout.preferredWidth: root.width * 0.34; wrapMode: Text.WordWrap }
        FactTextField {
            id: editor
            objectName: row.objectName + "_value"
            fact: row.setting
            Layout.fillWidth: true
            Layout.minimumWidth: 0
        }
    }

    QGCFlickable {
        objectName: "caacRIDScroller"
        anchors.fill: parent
        contentWidth: width
        contentHeight: content.implicitHeight
        clip: true
        ColumnLayout {
            id: content
            width: parent.width
            spacing: ScreenTools.defaultFontPixelHeight * 0.6
            SectionTitle { title: qsTr("Aircraft ID") }
            QGCLabel {
                Layout.fillWidth: true
                wrapMode: Text.WordWrap
                text: qsTr("UA type: %1 · UA ID type: %2")
                    .arg(root.manager && root.serial.length ? root.settings.basicIDUaType.enumStrings[root.manager.aircraftType] : "—")
                    .arg(qsTr("Unique product identifier"))
            }
            QGCLabel { text: qsTr("Airframe code (unique product identifier)"); Layout.fillWidth: true; wrapMode: Text.WordWrap }
            RowLayout {
                Layout.fillWidth: true
                QGCTextField {
                    objectName: "caacRIDAircraftSerial"
                    Layout.fillWidth: true
                    Layout.minimumWidth: 0
                    readOnly: true
                    text: root.serial
                    placeholderText: root.vehicle ? qsTr("Not reported by the aircraft") : qsTr("Connect a flight controller first")
                }
                QGCButton {
                    iconSource: "/InstrumentValueIcons/copy.svg"
                    enabled: root.serial.length > 0
                    onClicked: QGroundControl.copyToClipboard(root.serial)
                    ToolTip.visible: hovered
                    ToolTip.text: qsTr("Copy airframe code")
                }
            }
            Divider { }
            RowLayout {
                Layout.fillWidth: true
                SectionTitle { title: qsTr("UAS code") }
                QGCButton {
                    objectName: "caacRIDEnterUASCode"
                    text: qsTr("Enter code")
                    backgroundColor: palette.window
                    showBorder: true
                    onClicked: mainWindow.caacSettingsPage = "UOM"
                }
            }
            Choice { label: qsTr("UAS code type"); setting: root.settings.basicIDType }
            Entry { id: uas; objectName: "caacRIDUASCode"; label: qsTr("UAS code (required)"); setting: root.settings.basicID }
            FactCheckBox { text: qsTr("Broadcast UAS code"); fact: root.settings.sendBasicID }
            Divider { }
            SectionTitle { title: qsTr("Ground station position") }
            Choice {
                objectName: "caacRIDLocationType"
                label: qsTr("Position type")
                setting: root.settings.locationType
                enabled: root.settings.region.rawValue !== 0
            }
            Entry { label: qsTr("Latitude"); setting: root.settings.latitudeFixed; visible: root.settings.locationType.rawValue === 2 }
            Entry { label: qsTr("Longitude"); setting: root.settings.longitudeFixed; visible: root.settings.locationType.rawValue === 2 }
            Entry { label: qsTr("Altitude"); setting: root.settings.altitudeFixed; visible: root.settings.locationType.rawValue === 2 }
            Repeater {
                model: [
                    {label: qsTr("Latitude"), value: root.position.isValid ? root.position.latitude.toFixed(7) : "—"},
                    {label: qsTr("Longitude"), value: root.position.isValid ? root.position.longitude.toFixed(7) : "—"},
                    {label: qsTr("Altitude"), value: root.position.isValid && isFinite(root.position.altitude) ? root.position.altitude.toFixed(1) : "—"}
                ]
                RowLayout {
                    required property var modelData
                    visible: root.settings.locationType.rawValue !== 2
                    Layout.fillWidth: true
                    QGCLabel { text: parent.modelData.label; Layout.preferredWidth: root.width * 0.34 }
                    QGCTextField { text: parent.modelData.value; readOnly: true; Layout.fillWidth: true; Layout.minimumWidth: 0 }
                }
            }
            Divider { }
            SectionTitle { title: qsTr("Unmanned aircraft classification") }
            Choice { objectName: "caacRIDRegion"; label: qsTr("Region of operation"); setting: root.settings.region }
            Choice { label: qsTr("Classification type"); setting: root.settings.classificationType; visible: root.eu }
            Choice {
                objectName: "caacRIDCategory"
                label: qsTr("Operation category")
                setting: root.china ? root.settings.categoryChina : root.settings.categoryEU
                enabled: root.china || (root.eu && root.settings.classificationType.rawValue === 1)
            }
            Choice {
                objectName: "caacRIDClass"
                label: qsTr("UA class")
                setting: root.china ? root.settings.classChina : root.settings.classEU
                enabled: root.china || (root.eu && root.settings.classificationType.rawValue === 1)
            }
            Divider { }
            SectionTitle { title: qsTr("Flight declaration (optional)") }
            Choice { label: qsTr("Declaration type"); setting: root.settings.selfIDType }
            Entry { objectName: "caacRIDDeclaration"; label: qsTr("Declaration text"); setting: root.declaration }
            FactCheckBox { text: qsTr("Broadcast flight declaration"); fact: root.settings.sendSelfID }
            Divider { visible: !root.china }
            SectionTitle { title: qsTr("Operator ID"); visible: !root.china }
            Entry {
                label: qsTr("Operator ID")
                setting: root.eu ? root.settings.operatorIDEU : root.settings.operatorIDFAA
                visible: !root.china
            }
            FactCheckBox { text: qsTr("Broadcast operator ID"); fact: root.settings.sendOperatorID; visible: !root.china }
        }
    }
}
