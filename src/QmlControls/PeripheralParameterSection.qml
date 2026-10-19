pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import QGroundControl
import QGroundControl.Controls

ColumnLayout {
    id: root
    required property var vehicle
    required property var controller
    required property string title
    required property var definitions
    property string description: ""
    property bool collapsible: false
    property bool expanded: true
    property string resultText: ""
    property Component preview: null
    readonly property var displayValues: {
        const values = {}
        for (let index = 0; index < rows.count; ++index) {
            const row = root.rowAt(index)
            if (!row) { continue }
            const value = row.edited ? row.draft : row.fact ? Number(row.fact.rawValue) * row.valueScale : null
            values[row.definition.key] = value !== null && String(value).trim().length && Number.isFinite(Number(value))
                ? Number(value) : null
        }
        return values
    }
    readonly property bool ready: !!vehicle && !!controller && controller.vehicle === vehicle
        && vehicle.parameterManager.parametersReady && !vehicle.vehicleLinkManager.communicationLost
    readonly property bool writable: ready && !vehicle.armed && !vehicle.flying
    readonly property bool busy: ready && vehicle.parameterManager.pendingWrites
    Layout.fillWidth: true
    spacing: ScreenTools.defaultFontPixelHeight * 0.4

    function parameter(names) {
        if (!ready) { return null }
        for (const name of names) {
            if (controller.parameterExists(-1, name)) { return controller.getParameterFact(-1, name, false) }
        }
        return null
    }

    function reset() {
        resultText = ""
        if (!acknowledgementTimeout || !rows) { return }
        acknowledgementTimeout.stop()
        for (let i = 0; i < rows.count; ++i) { root.rowAt(i).reset() }
    }

    function rowAt(index: int): ParameterRow {
        return rows.itemAt(index) as ParameterRow
    }

    function stepFor(key, delta) {
        if (!writable || busy) { return }
        for (let i = 0; i < rows.count; ++i) {
            const row = root.rowAt(i)
            if (row.definition.key === key && row.fact && !row.fact.readOnly) { row.step(delta); return }
        }
    }

    function save() {
        if (!writable || busy) { return }
        for (let i = 0; i < rows.count; ++i) { if (!root.rowAt(i).validate()) { return } }
        resultText = ""
        let changed = false
        for (let i = 0; i < rows.count; ++i) { changed = root.rowAt(i).write() || changed }
        if (changed) {
            resultText = qsTr("正在等待飞控确认…")
            acknowledgementTimeout.restart()
        } else {
            resultText = qsTr("参数未更改。")
        }
    }

    function optionText(text) {
        const translations = {"Disabled": qsTr("禁用"), "Enabled": qsTr("启用"), "None": qsTr("无"),
            "AUTO": qsTr("自动识别"), "Auto": qsTr("自动识别"), "GPS 1": qsTr("GPS 1 接口"), "GPS 2": qsTr("GPS 2 接口"),
            "RTL": qsTr("返航"), "Land": qsTr("降落"), "AltHold": qsTr("定高"), "Hold": qsTr("保持"),
            "Report Only": qsTr("仅报告"), "ReportOnly": qsTr("仅报告"), "Terminate": qsTr("终止飞行"),
            "SmartRTL or RTL": qsTr("智能返航或返航"), "SmartRTL or Land": qsTr("智能返航或降落"),
            "Brake or Land": qsTr("制动或降落"), "All": qsTr("全部检查"), "Barometer": qsTr("气压计"),
            "Compass": qsTr("罗盘"), "INS": qsTr("惯性传感器"), "Parameters": qsTr("参数"),
            "RC Channels": qsTr("遥控通道"), "Voltage": qsTr("供电电压"), "Battery": qsTr("电池"),
            "Airspeed": qsTr("空速"), "Logging": qsTr("日志"), "Strict": qsTr("严格"),
            "Default": qsTr("默认"), "Relaxed": qsTr("宽松"), "Disabled/NoAction": qsTr("禁用／无动作"),
            "Report only": qsTr("仅报告"), "Land even in Stabilize": qsTr("所有模式下均降落"),
            "Switch to Land mode if current mode requires position": qsTr("当前模式需要定位时切换降落模式"),
            "Switch to AltHold mode if current mode requires position": qsTr("当前模式需要定位时切换定高模式"),
            "Switch to Land mode from all modes": qsTr("所有模式下均切换降落模式"),
            "Auto DO_LAND_START or RTL": qsTr("按任务降落，否则返航"),
            "Auto DO_LAND_START/DO_RETURN_PATH_START or RTL": qsTr("按任务降落或返航路径执行，否则返航"),
            "Logging Available": qsTr("日志可用"), "Hardware safety switch": qsTr("硬件安全开关"),
            "GPS Configuration": qsTr("GPS 配置"), "GPS lock": qsTr("GPS 定位"),
            "Board voltage": qsTr("飞控供电电压"), "Battery Level": qsTr("电池电量"),
            "System": qsTr("系统"), "Mission": qsTr("任务"), "Rangefinder": qsTr("测距传感器"),
            "Camera": qsTr("相机"), "AuxAuth": qsTr("辅助授权"), "VisualOdometry": qsTr("视觉里程计"),
            "Continue if in Auto on RC failsafe": qsTr("遥控失联时继续自动任务"),
            "Continue if in Auto on GCS failsafe": qsTr("数传失联时继续自动任务"),
            "Continue if in Guided on RC failsafe": qsTr("遥控失联时继续引导模式"),
            "Continue if landing on any failsafe": qsTr("故障保护触发时继续已有降落"),
            "Continue if in pilot controlled modes on GCS failsafe": qsTr("数传失联时继续人工操控模式"),
            "Release Gripper": qsTr("释放夹爪"), "Failsafe enabled in Hold mode": qsTr("保持模式下启用故障保护"),
            "AutoEnableOff": qsTr("关闭自动启用"), "AutoEnableOnTakeoff": qsTr("起飞时自动启用"),
            "AutoEnableDisableFloorOnLanding": qsTr("自动启用，降落时取消高度下限"),
            "AutoEnableOnlyWhenArmed": qsTr("仅在解锁时自动启用"), "Nothing": qsTr("无动作"),
            "HoldAndDisarm": qsTr("保持并上锁"), "SmartRTL or Hold": qsTr("智能返航或保持"),
            "Continue": qsTr("继续"), "ReturnToLaunch": qsTr("返航"), "Glide": qsTr("滑翔"),
            "Deploy Parachute": qsTr("打开降落伞"), "CIRCLE": qsTr("盘旋"), "Disable": qsTr("禁用"),
            "CIRCLE/no change(if already in AUTO|GUIDED|LOITER)": qsTr("盘旋；任务／引导／悬停模式下保持原模式"),
            "FBWA at zero throttle": qsTr("FBWA 模式，油门为零"), "Warn only": qsTr("仅警告"),
            "QLand": qsTr("垂直降落"), "QRTL": qsTr("垂直起降返航"),
            "Fly HOME then land via DO_LAND_START mission item": qsTr("先飞回返航点，再执行任务降落段"),
            "Go directly to landing sequence via DO_LAND_START mission item": qsTr("直接执行任务降落段"),
            "Go directly to landing sequence via DO_RETURN_PATH_START mission item": qsTr("直接执行任务返航路径"),
            "OnlyForGoAround": qsTr("仅用于复飞")}
        return (translations[text] || text).replace(/\s*[（(][^（）()]*[）)]/g, "").trim()
    }

    function acknowledge() {
        for (let i = 0; i < rows.count; ++i) { if (root.rowAt(i).awaiting) { return } }
        acknowledgementTimeout.stop()
        resultText = qsTr("已收到飞控保存确认。")
    }

    onReadyChanged: reset()
    onDefinitionsChanged: resultText = ""

    Timer {
        id: acknowledgementTimeout
        interval: 15000
        onTriggered: root.resultText = qsTr("未收到全部保存确认，请读取参数核对。")
    }

    RowLayout {
        Layout.fillWidth: true
        QGCLabel { text: root.title; visible: !root.collapsible; font.bold: true; wrapMode: Text.WordWrap; Layout.fillWidth: true }
        QGCButton {
            objectName: root.objectName + "_toggle"
            text: (root.expanded ? "▾ " : "▸ ") + root.title
            visible: root.collapsible
            Layout.fillWidth: true
            onClicked: root.expanded = !root.expanded
        }
        QGCButton {
            objectName: root.objectName + "_read"
            text: qsTr("读取")
            visible: root.expanded
            enabled: root.ready && !root.busy
            onClicked: {
                root.reset()
                for (let i = 0; i < rows.count; ++i) {
                    const fact = root.rowAt(i).fact
                    if (fact) { QGroundControl.corePlugin.refreshFlightPreferenceParameter(fact.name) }
                }
            }
        }
    }
    QGCLabel {
        text: root.description
        visible: root.expanded && text.length > 0
        Layout.fillWidth: true
        font.pointSize: ScreenTools.smallFontPointSize
        wrapMode: Text.WordWrap
    }
    Loader {
        Layout.fillWidth: true
        visible: root.expanded && !!root.preview
        sourceComponent: root.preview
    }
    component ParameterRow: ColumnLayout {
        id: row
        required property var modelData
        readonly property var definition: modelData
        readonly property var fact: root.parameter(definition.names)
        readonly property real valueScale: definition.valueScale || (fact && definition.scales && definition.scales[fact.name]
            ? definition.scales[fact.name] : 1)
        readonly property bool bitChoice: definition.bit !== undefined
        readonly property var flags: {
            if (!fact || bitChoice) { return [] }
            if (definition.flags) { return definition.flags }
            return fact.bitmaskValues.map((value, index) => ({value: Number(value), label: root.optionText(fact.bitmaskStrings[index])}))
        }
        readonly property var choices: {
            if (!fact || bitChoice) { return [] }
            if (!fact.enumValues.length) { return definition.options || [] }
            return fact.enumValues.map((value, index) => {
                const translated = definition.options ? definition.options.find(option => Number(option.value) === Number(value)) : null
                return {value: Number(value), label: translated ? translated.label : root.optionText(fact.enumStrings[index])}
            })
        }
        property bool edited: false
        property bool awaiting: false
        property var draft: null
        property real expectedValue: NaN
        objectName: "peripheralParameter_" + definition.key
        Layout.fillWidth: true
        visible: root.expanded
        spacing: ScreenTools.defaultFontPixelHeight * 0.15

        function reset() {
            edited = false
            awaiting = false
            draft = null
            if (input) { input.text = displayValue(); input.clearValidationError() }
        }
        function displayValue() {
            return fact ? Number((Number(fact.rawValue) * valueScale).toFixed(4)).toString() : ""
        }
        function candidateRaw() {
            if (!fact) { return NaN }
            if (!edited) { return Number(fact.rawValue) }
            if (bitChoice) {
                return draft ? Number(fact.rawValue) | (1 << definition.bit)
                             : Number(fact.rawValue) & ~(1 << definition.bit)
            }
            if (flags.length) {
                const known = flags.reduce((mask, flag) => mask | Number(flag.value), 0)
                return (Number(fact.rawValue) & ~known) | (Number(draft) & known)
            }
            return choices.length ? Number(draft) : Number(draft) / valueScale
        }
        function validate() {
            if (!edited) { return true }
            if (!fact || fact.readOnly || !root.writable) { return false }
            const raw = candidateRaw()
            if (!Number.isFinite(raw) || (typeof draft === "string" && !draft.trim().length)) {
                input.showValidationError(qsTr("请输入有效数值。"))
                return false
            }
            const range = definition.ranges ? definition.ranges[fact.name] : null
            if (range && (raw < range[0] || raw > range[1])) {
                input.showValidationError(qsTr("请输入 %1 至 %2 之间的数值。")
                    .arg((range[0] * valueScale).toFixed(2)).arg((range[1] * valueScale).toFixed(2)))
                return false
            }
            const error = fact.validate(fact.rawToCooked(raw).toString(), false)
            if (error.length) { input.showValidationError(error); return false }
            input.clearValidationError()
            return true
        }
        function write() {
            if (!edited || !fact || !root.writable || fact.readOnly) { return false }
            const raw = candidateRaw()
            if (Number(fact.rawValue) === raw) { reset(); return false }
            expectedValue = raw
            awaiting = true
            fact.rawValue = raw
            return true
        }
        function step(delta) {
            const value = edited ? Number(draft) : Number(fact.rawValue) * valueScale
            draft = Number(((Number.isFinite(value) ? value : 0) + delta).toFixed(4)).toString()
            input.text = draft
            edited = true
            root.resultText = ""
        }
        onFactChanged: reset()
        onDefinitionChanged: reset()
        Component.onCompleted: reset()

        QGCLabel { text: row.definition.label; wrapMode: Text.WordWrap; Layout.fillWidth: true }
        QGCCheckBox {
            objectName: row.objectName + "_enable"
            Layout.fillWidth: true
            visible: !!row.definition.enabledValues
            text: row.definition.toggleText || qsTr("启用")
            wrapMode: Text.WordWrap
            checked: !!row.fact && !!row.definition.enabledValues
                && row.definition.enabledValues.includes(row.edited ? Number(row.draft) : Number(row.fact.rawValue))
            enabled: root.writable && !root.busy && !!row.fact && !row.fact.readOnly
            onClicked: {
                row.draft = checked ? row.definition.enabledValue : row.definition.disabledValue
                row.edited = true
                root.resultText = ""
            }
        }
        Flow {
            Layout.fillWidth: true
            visible: row.flags.length > 0
            spacing: ScreenTools.defaultFontPixelWidth
            Repeater {
                model: row.flags
                QGCCheckBox {
                    id: flagChoice
                    required property var modelData
                    readonly property int mask: Number(modelData.value)
                    readonly property int currentMask: row.edited ? Number(row.draft) : row.fact ? Number(row.fact.rawValue) : 0
                    objectName: row.objectName + "_flag_" + mask
                    width: Math.min(implicitWidth, row.width)
                    text: modelData.label
                    wrapMode: Text.WordWrap
                    checked: !!(currentMask & mask)
                    enabled: root.writable && !root.busy && !!row.fact && !row.fact.readOnly
                        && !(row.definition.allFlag && mask !== row.definition.allFlag && (currentMask & row.definition.allFlag))
                    onClicked: {
                        const all = Number(row.definition.allFlag || 0)
                        row.draft = checked && mask === all ? all
                            : checked ? currentMask | mask : currentMask & ~mask
                        row.edited = true
                        root.resultText = ""
                    }
                }
            }
        }
        RowLayout {
            Layout.fillWidth: true
            visible: row.flags.length === 0
            enabled: root.writable && !root.busy && !!row.fact && !row.fact.readOnly
            QGCCheckBox {
                objectName: row.objectName + "_bit"
                visible: row.bitChoice
                enabled: !!row.fact
                text: row.definition.toggleText || qsTr("启用双天线航向融合")
                checked: row.edited ? !!row.draft : !!row.fact && !!(Number(row.fact.rawValue) & (1 << row.definition.bit))
                Layout.fillWidth: true
                onClicked: { row.draft = checked; row.edited = true; root.resultText = "" }
            }
            QGCComboBox {
                id: choice
                objectName: row.objectName + "_choice"
                Layout.fillWidth: true
                Layout.minimumWidth: 0
                visible: !row.bitChoice && row.choices.length > 0
                model: row.choices
                textRole: "label"
                QGCPalette { id: choicePalette; colorGroupEnabled: choice.enabled }
                currentIndex: row.choices.findIndex(entry => Number(entry.value) ===
                    (row.edited ? Number(row.draft) : row.fact ? Number(row.fact.rawValue) : NaN))
                displayText: currentIndex < 0 ? qsTr("当前值：%1").arg(row.fact ? row.fact.rawValue : "—") : currentText
                contentItem: QGCLabel {
                    text: choice.displayText
                    wrapMode: Text.WordWrap
                    color: choicePalette.buttonText
                }
                delegate: ItemDelegate {
                    id: option
                    required property int index
                    required property var modelData
                    objectName: row.objectName + "_option_" + option.index
                    width: choice.width
                    implicitHeight: Math.max(contentItem.implicitHeight + topPadding + bottomPadding,
                        ScreenTools.defaultFontPixelHeight * 1.75)
                    highlighted: choice.highlightedIndex === option.index
                    contentItem: QGCLabel {
                        text: option.modelData.label
                        font: choice.font
                        wrapMode: Text.WordWrap
                        color: choice.currentIndex === option.index ? choicePalette.buttonHighlightText : choicePalette.buttonText
                    }
                    background: Rectangle {
                        color: choice.currentIndex === option.index ? choicePalette.buttonHighlight : choicePalette.button
                        Rectangle {
                            anchors.bottom: parent.bottom
                            width: parent.width
                            height: 1
                            visible: option.index < choice.count - 1
                            color: choicePalette.text
                            opacity: 0.35
                        }
                    }
                }
                onActivated: index => { row.draft = row.choices[index].value; row.edited = true; root.resultText = "" }
            }
            QGCButton {
                objectName: row.objectName + "_minus"
                Layout.preferredWidth: ScreenTools.defaultFontPixelHeight * 1.8
                Layout.maximumWidth: Layout.preferredWidth
                text: "−"
                visible: input.visible
                onClicked: row.step(-Number(row.definition.step || 0.01))
            }
            QGCTextField {
                id: input
                objectName: row.objectName + "_value"
                Layout.fillWidth: true
                Layout.minimumWidth: 0
                visible: !row.bitChoice && row.choices.length === 0
                placeholderText: qsTr("不支持")
                numericValuesOnly: true
                onTextEdited: { row.draft = text; row.edited = true; root.resultText = "" }
            }
            QGCButton {
                objectName: row.objectName + "_plus"
                Layout.preferredWidth: ScreenTools.defaultFontPixelHeight * 1.8
                Layout.maximumWidth: Layout.preferredWidth
                text: "+"
                visible: input.visible
                onClicked: row.step(Number(row.definition.step || 0.01))
            }
            QGCLabel { text: row.definition.units || ""; visible: input.visible }
        }
        QGCLabel {
            Layout.fillWidth: true
            font.pointSize: ScreenTools.smallFontPointSize
            wrapMode: Text.WrapAnywhere
            text: row.fact ? row.fact.name : qsTr("当前飞控未提供此参数。")
            opacity: 0.65
        }
        Connections {
            target: row.fact
            function onRawValueChanged() { if (!row.edited) { input.text = row.displayValue() } }
            function onVehicleUpdated(value) {
                if (row.awaiting && Math.abs(Number(value) - row.expectedValue) <= Math.max(1e-6, Math.abs(row.expectedValue) * 1e-6)) {
                    row.reset()
                    root.acknowledge()
                }
            }
        }
    }
    Repeater {
        id: rows
        model: root.definitions
        ParameterRow { }
    }
    QGCButton {
        objectName: root.objectName + "_save"
        Layout.fillWidth: true
        text: qsTr("确认保存")
        visible: root.expanded
        enabled: root.writable && !root.busy && root.definitions.some(definition => !!root.parameter(definition.names))
        onClicked: root.save()
    }
    QGCLabel {
        objectName: root.objectName + "_result"
        Layout.fillWidth: true
        visible: root.expanded && text.length > 0
        text: root.resultText
        wrapMode: Text.WordWrap
    }
}
