pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import QGroundControl
import QGroundControl.Controls
import QGroundControl.FactControls

import "CaacFailsafeEnumTranslations.js" as FailsafeEnums

Item {
    id: root
    objectName: "caacSafetySettings"
    readonly property var vehicle: QGroundControl.multiVehicleManager.activeVehicle
    readonly property bool ready: QGroundControl.multiVehicleManager.parameterReadyVehicleAvailable
    property bool advancedFence: false
    QGCPalette { id: palette }
    function resetController() {
        controllerLoader.sourceComponent = null
        if (vehicle && ready) { controllerLoader.sourceComponent = controllerComponent }
    }
    function parameter(names) {
        const controller = controllerLoader.item
        if (!controller || controller.vehicle !== vehicle || !ready) { return null }
        for (const name of names) {
            if (controller.parameterExists(-1, name)) { return controller.getParameterFact(-1, name, false) }
        }
        return null
    }
    onVehicleChanged: resetController()
    onReadyChanged: resetController()
    Component.onCompleted: resetController()
    Component { id: controllerComponent; FactPanelController { } }
    Loader { id: controllerLoader }

    component ParameterRow: SettingsFormRow {
        id: row
        required property var definition
        readonly property var fact: root.parameter(definition.names)
        readonly property bool centimetersPerSecond: !!fact && fact.enumStrings.length === 0
            && fact.units.toLowerCase() === "cm/s"
        readonly property string displayUnits: centimetersPerSecond ? "m/s" : fact ? fact.units : ""
        property bool edited: false
        property var draftValue: null
        onCentimetersPerSecondChanged: {
            if (value && !edited) { value.text = displayText() }
        }
        onFactChanged: { edited = false; draftValue = null; if (value) { value.text = displayText() } }
        objectName: "caacSafety_" + definition.names[0]
        Layout.fillWidth: true
        function displayText() {
            if (!fact) { return "" }
            if (!centimetersPerSecond) { return fact.valueString }
            const decimals = Math.min(12, Math.max(2, fact.decimalPlaces + 2))
            return Number((Number(fact.value) / 100).toFixed(decimals)).toString()
        }
        function parameterValue() {
            if (!centimetersPerSecond || typeof draftValue !== "string" || draftValue.trim() === "") {
                return draftValue
            }
            const numeric = Number(draftValue)
            return Number.isFinite(numeric) ? Number((numeric * 100).toPrecision(15)).toString() : draftValue
        }
        function validate() {
            if (!edited || !fact) { return true }
            const candidate = parameterValue()
            let error = fact.validate(candidate, false)
            if (error.length && centimetersPerSecond && Number.isFinite(Number(candidate))
                    && (Number(candidate) < Number(fact.min) || Number(candidate) > Number(fact.max))) {
                error = qsTranslate("FactMetaData", "Value must be within %1 and %2")
                    .arg(Number((Number(fact.min) / 100).toPrecision(15)))
                    .arg(Number((Number(fact.max) / 100).toPrecision(15))) + " m/s"
            }
            if (error.length) { value.showValidationError(error); return false }
            value.clearValidationError()
            return true
        }
        function write() {
            if (edited && fact && root.ready) {
                fact.value = parameterValue()
                edited = false
                value.text = displayText()
            }
        }
        QGCLabel { text: row.definition.label; Layout.preferredWidth: root.width * 0.46; wrapMode: Text.WordWrap }
        QGCTextField {
            id: value
            objectName: row.objectName + "_value"
            Layout.fillWidth: true
            Layout.minimumWidth: 0
            enabled: !!row.fact
            visible: !row.fact || row.fact.enumStrings.length === 0
            text: row.displayText()
            placeholderText: qsTr("Unavailable")
            onTextEdited: { row.edited = true; row.draftValue = text }
        }
        QGCComboBox {
            id: choice
            objectName: row.objectName + "_choice"
            Layout.fillWidth: true
            Layout.minimumWidth: 0
            visible: !!row.fact && row.fact.enumStrings.length > 0
            model: row.fact ? row.fact.enumStrings : []
            currentIndex: row.fact ? row.fact.enumIndex : -1
            textTransform: (text) => row.fact ? FailsafeEnums.displayString(row.fact.name, text) : text
            QGCPalette { id: choicePalette; colorGroupEnabled: choice.enabled }
            contentItem: QGCLabel {
                objectName: row.objectName + "_choiceText"
                text: choice.displayString(choice.currentText)
                font: choice.font
                color: choicePalette.buttonText
                wrapMode: Text.WordWrap
            }
            delegate: ItemDelegate {
                id: option
                required property int index
                required property string modelData
                objectName: row.objectName + "_option_" + option.index
                width: choice._popupWidth
                implicitHeight: Math.max(option.contentItem.implicitHeight + option.topPadding + option.bottomPadding,
                                         ScreenTools.defaultFontPixelHeight * 1.75)
                highlighted: choice.highlightedIndex === option.index
                contentItem: QGCLabel {
                    objectName: option.objectName + "_text"
                    text: choice.displayString(option.modelData)
                    font: choice.font
                    color: choice.currentIndex === option.index ? choicePalette.buttonHighlightText : choicePalette.buttonText
                    wrapMode: Text.WordWrap
                }
                background: Rectangle {
                    color: choice.currentIndex === option.index ? choicePalette.buttonHighlight : choicePalette.button
                }
            }
            onActivated: (index) => { row.edited = true; row.draftValue = row.fact.enumValues[index] }
        }
        QGCLabel { objectName: row.objectName + "_units"; text: row.displayUnits }
        Connections {
            target: row.fact
            function onValueChanged() { if (!row.edited) { value.text = row.displayText() } }
            function onCookedValuesChanged() { if (!row.edited) { value.text = row.displayText() } }
        }
    }
    component SafetySection: ColumnLayout {
        id: section
        required property string title
        required property var definitions
        Layout.fillWidth: true
        Layout.bottomMargin: ScreenTools.defaultFontPixelHeight
        RowLayout {
            Layout.fillWidth: true
            Rectangle { Layout.preferredWidth: ScreenTools.defaultFontPixelWidth * 0.4; Layout.preferredHeight: ScreenTools.defaultFontPixelHeight; color: palette.settingsSectionAccent; radius: width / 2 }
            QGCLabel { text: section.title; font.bold: true; Layout.fillWidth: true; wrapMode: Text.WordWrap }
            QGCButton {
                text: qsTr("Read parameters")
                enabled: root.vehicle && root.ready
                heightFactor: 0.2
                onClicked: {
                    for (let i = 0; i < rows.count; ++i) {
                        const row = rows.itemAt(i)
                        if (row.fact) {
                            row.edited = false
                            QGroundControl.corePlugin.refreshFlightPreferenceParameter(row.fact.name)
                        }
                    }
                }
            }
        }
        Repeater {
            id: rows
            model: section.definitions
            ParameterRow { required property var modelData; definition: modelData }
        }
        QGCButton {
            objectName: "caacSafetyConfirm_" + section.definitions[0].names[0]
            Layout.fillWidth: true
            text: qsTr("Confirm")
            backgroundColor: palette.window
            showBorder: true
            enabled: root.vehicle && root.ready
            onClicked: {
                if (!controllerLoader.item || controllerLoader.item.vehicle !== root.vehicle) { return }
                for (let i = 0; i < rows.count; ++i) { if (!rows.itemAt(i).validate()) { return } }
                for (let i = 0; i < rows.count; ++i) { rows.itemAt(i).write() }
            }
        }
        Rectangle { Layout.fillWidth: true; Layout.preferredHeight: 1; color: palette.windowShade; Layout.topMargin: ScreenTools.defaultFontPixelHeight }
    }
    QGCFlickable {
        objectName: "caacSafetyScroller"
        anchors.fill: parent
        contentWidth: width
        contentHeight: content.implicitHeight
        clip: true
        ColumnLayout {
            id: content
            width: parent.width
            spacing: ScreenTools.defaultFontPixelHeight * 0.5
            QGCLabel {
                Layout.fillWidth: true
                wrapMode: Text.WordWrap
                text: qsTr("Read the connected aircraft parameters, edit their values, then confirm to write them. Unsupported parameters are unavailable.")
                font.pointSize: ScreenTools.smallFontPointSize
            }
            SafetySection {
                title: qsTr("Flight safety settings")
                definitions: [
                    {label: qsTr("Return altitude"), names: ["RTL_RETURN_ALT", "RTL_ALT", "RTL_ALT_M"]},
                    {label: qsTr("Horizontal speed (route)"), names: ["MPC_XY_CRUISE", "WPNAV_SPEED", "WP_SPD"]},
                    {label: qsTr("Ascent speed (route)"), names: ["MPC_Z_V_AUTO_UP", "WPNAV_SPEED_UP", "WP_SPD_UP"]},
                    {label: qsTr("Descent speed (route)"), names: ["MPC_Z_V_AUTO_DN", "WPNAV_SPEED_DN", "WP_SPD_DN"]},
                    {label: qsTr("Horizontal speed (manual)"), names: ["MPC_VEL_MANUAL", "LOIT_SPEED", "LOIT_SPEED_MS"]},
                    {label: qsTr("Ascent speed (manual)"), names: ["MPC_Z_VEL_MAX_UP", "PILOT_SPEED_UP", "PILOT_SPD_UP"]},
                    {label: qsTr("Descent speed (manual)"), names: ["MPC_Z_VEL_MAX_DN", "PILOT_SPEED_DN", "PILOT_SPD_DN"]},
                    {label: qsTr("Altitude limit"), names: ["GF_MAX_VER_DIST", "FENCE_ALT_MAX"]},
                    {label: qsTr("Radar detection distance"), names: ["CP_DIST", "AVOID_DIST_MAX"]},
                    {label: qsTr("Obstacle braking distance"), names: ["CP_STOP_DIST", "AVOID_MARGIN"]},
                    {label: qsTr("Obstacle avoidance behavior"), names: ["COM_OBS_AVOID", "OA_TYPE"]}
                ]
            }
            SafetySection {
                title: qsTr("Slow descent")
                definitions: [{label: qsTr("Slow descent function"), names: ["SLOW_DESC_EN", "CAAC_SLOW_DESC"]}]
            }
            SafetySection {
                title: qsTr("RC connection loss protection")
                definitions: [
                    {label: qsTr("RC loss action"), names: ["NAV_RCL_ACT", "FS_THR_ENABLE"]},
                    {label: qsTr("Trigger time"), names: ["COM_RC_LOSS_T", "RC_FS_TIMEOUT"]}
                ]
            }
            SafetySection {
                title: qsTr("Data link loss protection")
                definitions: [
                    {label: qsTr("Data link loss action"), names: ["NAV_DLL_ACT", "FS_GCS_ENABLE"]},
                    {label: qsTr("Trigger time"), names: ["COM_DL_LOSS_T", "FS_GCS_TIMEOUT"]}
                ]
            }
            CaacBatteryProtection {
                lowThreshold: root.parameter(["BAT_LOW_THR"])
                criticalThreshold: root.parameter(["BAT_CRIT_THR"])
                actionFact: root.parameter(["COM_LOW_BAT_ACT"])
                cellsFact: root.parameter(["BAT1_N_CELLS", "BAT_N_CELLS"])
                emptyVoltageFact: root.parameter(["BAT1_V_EMPTY", "BAT_V_EMPTY"])
                chargedVoltageFact: root.parameter(["BAT1_V_CHARGED", "BAT_V_CHARGED"])
                ready: root.ready && !!root.vehicle && !root.vehicle.armed
            }
            CaacGeoFenceSettings { actionFact: root.parameter(["GF_ACTION"]) }
            QGCButton {
                text: qsTr("更多围栏参数")
                Layout.fillWidth: true
                onClicked: root.advancedFence = !root.advancedFence
            }
            SafetySection {
                visible: root.advancedFence
                title: qsTr("围栏范围设置")
                definitions: [
                    {label: qsTr("Fence action"), names: ["GF_ACTION", "FENCE_ACTION"]},
                    {label: qsTr("Horizontal limit"), names: ["GF_MAX_HOR_DIST", "FENCE_RADIUS"]},
                    {label: qsTr("Altitude limit"), names: ["GF_MAX_VER_DIST", "FENCE_ALT_MAX"]}
                ]
            }
        }
    }
}
