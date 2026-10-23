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
    readonly property FactPanelController parameterController: controllerLoader.item as FactPanelController
    property bool advancedFence: false
    QGCPalette { id: palette }
    function resetController() {
        controllerLoader.sourceComponent = null
        if (vehicle && ready) { controllerLoader.sourceComponent = controllerComponent }
    }
    function parameter(names) {
        const controller = root.parameterController
        if (!controller || controller.vehicle !== vehicle || !ready) { return null }
        for (const name of names) {
            if (controller.parameterExists(-1, name)) { return controller.getParameterFact(-1, name, false) }
        }
        return null
    }
    function supported(definitions) {
        return definitions.filter(definition => !!parameter(definition.names))
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
        QGCLabel {
            text: row.definition.label
            Layout.preferredWidth: row.definition.names[0] === "RTL_TYPE"
                ? Math.min(implicitWidth, root.width * 0.46) : root.width * 0.46
            wrapMode: Text.WordWrap
        }
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
            wrapCurrentText: true
            currentIndex: row.fact ? row.fact.enumIndex : -1
            textTransform: (text) => {
                const translated = row.fact ? FailsafeEnums.displayString(row.fact.name, text) : text
                return translated !== text ? translated
                    : OnlineMessageTranslator.parameterText(text, OnlineMessageTranslator.revision)
            }
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
                    Rectangle {
                        objectName: option.objectName + "_separator"
                        anchors.bottom: parent.bottom
                        width: parent.width
                        height: 1
                        visible: option.index < choice.count - 1
                        color: choicePalette.text
                        opacity: 0.35
                    }
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
        function rowAt(index: int): ParameterRow { return rows.itemAt(index) as ParameterRow }
        visible: definitions.length > 0
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
                        const row = section.rowAt(i)
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
            objectName: "caacSafetyConfirm_" + (section.definitions.length ? section.definitions[0].names[0] : section.title)
            Layout.fillWidth: true
            text: qsTr("Confirm")
            showBorder: true
            enabled: root.vehicle && root.ready
            onClicked: {
                if (!root.parameterController || root.parameterController.vehicle !== root.vehicle) { return }
                for (let i = 0; i < rows.count; ++i) { if (!section.rowAt(i).validate()) { return } }
                for (let i = 0; i < rows.count; ++i) { section.rowAt(i).write() }
            }
        }
        Rectangle { Layout.fillWidth: true; Layout.preferredHeight: 1; color: palette.windowShade; Layout.topMargin: ScreenTools.defaultFontPixelHeight }
    }
    component AdditionalSafetySection: PeripheralParameterSection {
        required property var availableDefinitions
        vehicle: root.vehicle
        controller: root.parameterController
        definitions: root.supported(availableDefinitions)
        visible: definitions.length > 0
        collapsible: true
        expanded: false
        Layout.bottomMargin: ScreenTools.defaultFontPixelHeight
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
                objectName: "caacReturnSettings"
                title: qsTr("返航设置")
                definitions: root.supported([
                    {label: qsTr("返航高度"), names: ["RTL_RETURN_ALT", "RTL_ALT_M", "RTL_ALT", "RTL_ALTITUDE"]},
                    {label: qsTr("返航方式"), names: ["RTL_TYPE"]},
                    {label: qsTr("返航点上方悬停时间"), names: ["RTL_LOIT_TIME", "RTL_LAND_DELAY"]},
                    {label: qsTr("最终降落阶段高度"), names: ["RTL_ALT_FINAL_M", "RTL_ALT_FINAL", "RTL_DESCEND_ALT"]},
                    {label: qsTr("最终降落速度"), names: ["LAND_SPD_MS", "LAND_SPEED", "MPC_LAND_SPEED"]},
                    {label: qsTr("返航盘旋半径"), names: ["RTL_RADIUS"]},
                    {label: qsTr("返航后自动降落"), names: ["RTL_AUTOLAND"]}
                ])
            }
            SafetySection {
                title: qsTr("Slow descent")
                definitions: [{label: qsTr("Slow descent function"), names: ["SLOW_DESC_EN", "CAAC_SLOW_DESC"]}]
            }
            SafetySection {
                title: qsTr("RC connection loss protection")
                definitions: [
                    {label: qsTr("RC loss action"), names: ["NAV_RCL_ACT", "FS_THR_ENABLE", "THR_FAILSAFE"]},
                    {label: qsTr("Trigger time"), names: ["COM_RC_LOSS_T", "RC_FS_TIMEOUT"]}
                ]
            }
            SafetySection {
                title: qsTr("Data link loss protection")
                definitions: [
                    {label: qsTr("Data link loss action"), names: ["NAV_DLL_ACT", "FS_GCS_ENABLE", "FS_GCS_ENABL"]},
                    {label: qsTr("Trigger time"), names: ["COM_DL_LOSS_T", "FS_GCS_TIMEOUT"]}
                ]
            }
            SafetySection {
                objectName: "caacAdditionalPx4SafetySettings"
                title: qsTr("附加安全设置")
                definitions: root.supported([
                    {label: qsTr("紧急电量门限"), names: ["BAT_EMERGEN_THR"]},
                    {label: qsTr("降落后自动上锁延时"), names: ["COM_DISARM_LAND"]}
                ])
                visible: definitions.length > 0
            }
            FactCheckBox {
                objectName: "caacSafetyObstacleDistanceOverlay"
                Layout.fillWidth: true
                text: qsTr("显示障碍物距离")
                fact: QGroundControl.settingsManager.flyViewSettings.showObstacleDistanceOverlay
            }
            CaacBatteryProtection {
                id: batteryProtection
                visible: !!root.parameter(["BAT_LOW_THR"])
                lowThreshold: root.parameter(["BAT_LOW_THR"])
                criticalThreshold: root.parameter(["BAT_CRIT_THR"])
                actionFact: root.parameter(["COM_LOW_BAT_ACT"])
                cellsFact: root.parameter(["BAT1_N_CELLS", "BAT_N_CELLS"])
                emptyVoltageFact: root.parameter(["BAT1_V_EMPTY", "BAT_V_EMPTY"])
                chargedVoltageFact: root.parameter(["BAT1_V_CHARGED", "BAT_V_CHARGED"])
                ready: root.ready && !!root.vehicle && !root.vehicle.armed
            }
            Rectangle {
                objectName: "caacBatteryFenceDivider"
                visible: batteryProtection.visible
                Layout.fillWidth: true
                Layout.preferredHeight: 1
                Layout.topMargin: ScreenTools.defaultFontPixelHeight * 0.5
                Layout.bottomMargin: ScreenTools.defaultFontPixelHeight * 0.5
                color: palette.groupBorder
            }
            CaacGeoFenceSettings {
                actionFact: root.parameter(["GF_ACTION", "FENCE_ACTION"])
                enabledFact: root.parameter(["FENCE_ENABLE"])
            }
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
            AdditionalSafetySection {
                objectName: "caacFenceAdditionalSettings"
                title: qsTr("围栏附加设置")
                availableDefinitions: [
                    {key: "fenceType", label: qsTr("围栏类型"), names: ["FENCE_TYPE"], flags: [
                        {label: qsTr("最高高度"), value: 1}, {label: qsTr("以返航点为中心的圆形"), value: 2},
                        {label: qsTr("包含／排除区域"), value: 4}, {label: qsTr("最低高度"), value: 8}]},
                    {key: "fenceMinimumAltitude", label: qsTr("最低高度"), names: ["FENCE_ALT_MIN"], units: "m"},
                    {key: "fenceMargin", label: qsTr("安全边距"), names: ["FENCE_MARGIN"], units: "m"},
                    {key: "fenceAutomaticEnable", label: qsTr("自动启用围栏"), names: ["FENCE_AUTOENABLE"]},
                    {key: "fenceReturnAltitude", label: qsTr("越界后的返航高度"), names: ["FENCE_RET_ALT"], units: "m"},
                    {key: "fenceRallyPoint", label: qsTr("越界后前往最近集结点"), names: ["FENCE_RET_RALLY"]}
                ]
            }
            AdditionalSafetySection {
                objectName: "caacArmingSafetySettings"
                title: qsTr("解锁检查")
                availableDefinitions: [
                    {key: "armingChecks", label: qsTr("解锁前检查项目"), names: ["ARMING_CHECK"], allFlag: 1, step: 1},
                    {key: "armingSkipChecks", label: qsTr("允许跳过的检查"), names: ["ARMING_SKIPCHK"], step: 1}
                ]
            }
            Repeater {
                model: 16
                AdditionalSafetySection {
                    required property int index
                    readonly property string batteryPrefix: index === 0 ? "BATT" : "BATT" + (index + 1)
                    readonly property var monitor: root.parameter([batteryPrefix + "_MONITOR"])
                    visible: definitions.length > 0 && (!monitor || Number(monitor.rawValue) !== 0)
                    objectName: "caacBatterySafety_" + (index + 1)
                    title: qsTr("电池 %1 保护").arg(index + 1)
                    availableDefinitions: [
                        {key: batteryPrefix + "LowAction", label: qsTr("低电量行为"), names: [batteryPrefix + "_FS_LOW_ACT"], step: 1},
                        {key: batteryPrefix + "CriticalAction", label: qsTr("严重低电量行为"), names: [batteryPrefix + "_FS_CRT_ACT"], step: 1},
                        {key: batteryPrefix + "LowVoltage", label: qsTr("低电压门限"), names: [batteryPrefix + "_LOW_VOLT"], units: "V", step: 0.1},
                        {key: batteryPrefix + "CriticalVoltage", label: qsTr("严重低电压门限"), names: [batteryPrefix + "_CRT_VOLT"], units: "V", step: 0.1},
                        {key: batteryPrefix + "LowCapacity", label: qsTr("低剩余容量门限"), names: [batteryPrefix + "_LOW_MAH"], units: "mAh", step: 10},
                        {key: batteryPrefix + "CriticalCapacity", label: qsTr("严重低剩余容量门限"), names: [batteryPrefix + "_CRT_MAH"], units: "mAh", step: 10}
                    ]
                }
            }
            AdditionalSafetySection {
                objectName: "caacNavigationFailsafeSettings"
                title: qsTr("导航与姿态故障保护")
                availableDefinitions: [
                    {key: "ekfThreshold", label: qsTr("EKF 失效门限"), names: ["FS_EKF_THRESH"], step: 0.1},
                    {key: "ekfAction", label: qsTr("EKF 失效行为"), names: ["FS_EKF_ACTION"], step: 1},
                    {key: "deadReckoningTimeout", label: qsTr("推算导航超时"), names: ["FS_DR_TIMEOUT"], units: "s", step: 1},
                    {key: "deadReckoningAction", label: qsTr("推算导航失效行为"), names: ["FS_DR_ENABLE"], step: 1},
                    {key: "crashCheck", label: qsTr("碰撞保护"), names: ["FS_CRASH_CHECK"], step: 1},
                    {key: "vibrationCheck", label: qsTr("振动保护"), names: ["FS_VIBE_ENABLE"], step: 1}
                ]
            }
            AdditionalSafetySection {
                objectName: "caacOtherFailsafeSettings"
                title: qsTr("其他失联与故障保护")
                availableDefinitions: [
                    {key: "throttlePwm", label: qsTr("油门失联 PWM 门限"), names: ["FS_THR_VALUE", "THR_FS_VALUE"], units: "µs", step: 1},
                    {key: "failsafeOptions", label: qsTr("故障保护选项"), names: ["FS_OPTIONS"], step: 1},
                    {key: "shortFailsafe", label: qsTr("短时失联行为"), names: ["FS_SHORT_ACTN"], step: 1},
                    {key: "longFailsafe", label: qsTr("长时失联行为"), names: ["FS_LONG_ACTN"], step: 1},
                    {key: "longFailsafeTimeout", label: qsTr("长时失联超时"), names: ["FS_LONG_TIMEOUT"], units: "s", step: 1},
                    {key: "transitionFailsafe", label: qsTr("垂直起降转换失败行为"), names: ["Q_TRANS_FAIL_ACT"], step: 1},
                    {key: "transitionTimeout", label: qsTr("垂直起降转换超时"), names: ["Q_TRANS_FAIL"], units: "s", step: 1},
                    {key: "genericTimeout", label: qsTr("失联超时"), names: ["FS_TIMEOUT"], units: "s", step: 1},
                    {key: "genericAction", label: qsTr("失联行为"), names: ["FS_ACTION"], step: 1}
                ]
            }
        }
    }
}
