pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import QGroundControl
import QGroundControl.Controls

ColumnLayout {
    id: root
    objectName: "caacBatteryProtection"
    required property var lowThreshold
    required property var criticalThreshold
    required property var actionFact
    required property var cellsFact
    required property var emptyVoltageFact
    required property var chargedVoltageFact
    required property bool ready
    readonly property real cells: cellsFact ? Number(cellsFact.rawValue) : 0
    readonly property real emptyVoltage: emptyVoltageFact ? Number(emptyVoltageFact.rawValue) * cells : NaN
    readonly property real chargedVoltage: chargedVoltageFact ? Number(chargedVoltageFact.rawValue) * cells : NaN
    readonly property bool available: ready && cells > 0 && isFinite(emptyVoltage) && chargedVoltage > emptyVoltage
    readonly property int returnAction: supportsAction(1) ? 1 : supportsAction(3) ? 3 : -1
    property int draftAction: actionFact ? Number(actionFact.rawValue) : -1
    property bool actionEdited: false
    onActionFactChanged: { actionEdited = false; draftAction = actionFact ? Number(actionFact.rawValue) : -1 }
    Layout.fillWidth: true
    spacing: ScreenTools.defaultFontPixelHeight
    QGCPalette { id: batteryPalette }

    function supportsAction(value) {
        return !!actionFact && actionFact.enumValues.some(entry => Number(entry) === value)
    }
    function voltage(threshold) {
        return available && threshold ? emptyVoltage + Number(threshold.rawValue) * (chargedVoltage - emptyVoltage) : NaN
    }
    function refresh() {
        for (const fact of [lowThreshold, criticalThreshold, actionFact, cellsFact, emptyVoltageFact, chargedVoltageFact]) {
            if (fact) { QGroundControl.corePlugin.refreshFlightPreferenceParameter(fact.name) }
        }
    }
    onEmptyVoltageChanged: Qt.callLater(() => { lowCard.refreshValue(); criticalCard.refreshValue() })
    onChargedVoltageChanged: Qt.callLater(() => { lowCard.refreshValue(); criticalCard.refreshValue() })
    Connections {
        target: root.actionFact
        function onRawValueChanged() { if (!root.actionEdited) { root.draftAction = Number(root.actionFact.rawValue) } }
    }

    component BatteryCard: Rectangle {
        id: card
        required property bool critical
        required property var threshold
        property bool edited: false
        readonly property var actions: critical
            ? [{label: qsTr("悬停"), value: -1}, {label: qsTr("返航"), value: root.returnAction}, {label: qsTr("降落"), value: 2}]
            : [{label: qsTr("警告"), value: 0}, {label: qsTr("悬停"), value: -1}, {label: qsTr("返航"), value: -1}]
        readonly property real draftVoltage: Number(voltageField.text)
        Layout.fillWidth: true
        implicitHeight: fields.implicitHeight + ScreenTools.defaultFontPixelHeight
        radius: ScreenTools.defaultBorderRadius
        color: "transparent"
        border.color: batteryPalette.groupBorder
        border.width: 1
        function refreshValue() {
            if (voltageField && !edited) {
                const current = root.voltage(threshold)
                voltageField.text = isFinite(current) ? current.toFixed(2) : ""
            }
        }
        function rawThreshold() { return (draftVoltage - root.emptyVoltage) / (root.chargedVoltage - root.emptyVoltage) }
        function validate() {
            if (!edited) { return true }
            const raw = rawThreshold()
            if (!threshold || !root.available || !voltageField.text.trim().length || !isFinite(raw)
                || raw < Number(threshold.rawMin) || raw > Number(threshold.rawMax)) {
                voltageField.showValidationError(qsTr("请输入飞控允许范围内的电压门限。"))
                return false
            }
            voltageField.clearValidationError()
            return true
        }
        function changeVoltage(delta) {
            if (!threshold || !root.available) { return }
            const minimum = root.emptyVoltage + Number(threshold.rawMin) * (root.chargedVoltage - root.emptyVoltage)
            const maximum = root.emptyVoltage + Number(threshold.rawMax) * (root.chargedVoltage - root.emptyVoltage)
            voltageField.text = Math.max(minimum, Math.min(maximum, draftVoltage + delta)).toFixed(2)
            edited = true
        }
        onThresholdChanged: { edited = false; refreshValue() }
        Component.onCompleted: refreshValue()
        Connections {
            target: card.threshold
            function onRawValueChanged() { card.refreshValue() }
        }
        ColumnLayout {
            id: fields
            x: ScreenTools.defaultFontPixelWidth
            y: ScreenTools.defaultFontPixelHeight * 0.5
            width: parent.width - x * 2
            spacing: ScreenTools.defaultFontPixelHeight * 0.5
            GridLayout {
                Layout.fillWidth: true
                columns: card.width < ScreenTools.defaultFontPixelWidth * 42 ? 1 : 2
                QGCLabel {
                    text: card.critical ? qsTr("飞行器严重低电量时") : qsTr("飞行器低电量时")
                    color: card.critical ? batteryPalette.colorRed : batteryPalette.colorOrange
                    wrapMode: Text.WordWrap
                    Layout.fillWidth: true
                }
                RowLayout {
                    Layout.fillWidth: true
                    Repeater {
                        model: card.actions
                        QGCButton {
                            id: actionButton
                            required property var modelData
                            required property int index
                            objectName: (card.critical ? "caacCriticalBatteryAction_" : "caacLowBatteryAction_") + index
                            Layout.fillWidth: true
                            Layout.minimumWidth: 0
                            _horizontalPadding: ScreenTools.defaultFontPixelWidth * 0.5
                            text: modelData.label
                            checked: card.critical ? modelData.value >= 0 && root.draftAction === modelData.value : index === 0
                            enabled: card.critical && root.ready && modelData.value >= 0 && root.supportsAction(modelData.value)
                            textColor: checked ? "white" : batteryPalette.text
                            background: Rectangle {
                                implicitWidth: ScreenTools.implicitButtonWidth
                                implicitHeight: ScreenTools.implicitButtonHeight
                                radius: ScreenTools.defaultBorderRadius
                                color: actionButton.checked ? (card.critical ? batteryPalette.colorRed : batteryPalette.colorOrange) : batteryPalette.window
                                border.color: batteryPalette.windowShadeLight
                                border.width: 1
                            }
                            ToolTip.visible: hovered
                            ToolTip.text: enabled ? modelData.label : qsTr("PX4 不支持单独配置此动作。")
                            onClicked: { root.draftAction = modelData.value; root.actionEdited = true }
                        }
                    }
                }
            }
            SettingsFormRow {
                Layout.fillWidth: true
                QGCLabel { text: qsTr("电压门限"); Layout.fillWidth: true }
                QGCButton {
                    objectName: card.critical ? "caacCriticalVoltageMinus" : "caacLowVoltageMinus"
                    text: "−"
                    _horizontalPadding: ScreenTools.defaultFontPixelWidth
                    enabled: root.available && !!card.threshold
                    onClicked: card.changeVoltage(-0.1)
                }
                QGCTextField {
                    id: voltageField
                    objectName: card.critical ? "caacCriticalBatteryVoltage" : "caacLowBatteryVoltage"
                    Layout.preferredWidth: ScreenTools.defaultFontPixelWidth * 9
                    Layout.minimumWidth: 0
                    enabled: root.available && !!card.threshold
                    horizontalAlignment: Text.AlignHCenter
                    placeholderText: "—"
                    validator: DoubleValidator { locale: "C"; bottom: 0 }
                    onTextEdited: card.edited = true
                }
                QGCLabel { text: "V" }
                QGCButton {
                    objectName: card.critical ? "caacCriticalVoltagePlus" : "caacLowVoltagePlus"
                    text: "+"
                    _horizontalPadding: ScreenTools.defaultFontPixelWidth
                    enabled: root.available && !!card.threshold
                    onClicked: card.changeVoltage(0.1)
                }
            }
        }
    }
    RowLayout {
        Layout.fillWidth: true
        QGCLabel { text: qsTr("电池设置"); font.bold: true; Layout.fillWidth: true }
        QGCButton { text: qsTr("读取参数"); enabled: root.ready; onClicked: root.refresh() }
    }
    BatteryCard { id: lowCard; critical: false; threshold: root.lowThreshold }
    BatteryCard { id: criticalCard; critical: true; threshold: root.criticalThreshold }
    QGCLabel {
        Layout.fillWidth: true
        wrapMode: Text.WordWrap
        font.pointSize: ScreenTools.smallFontPointSize
        text: root.available
            ? qsTr("电压按电池节数及满／空电压换算；PX4 仍按剩余电量比例触发保护。低电量阶段固定警告，不支持的动作禁用。")
            : qsTr("连接飞控，并在电源设置中配置电池节数及满／空电压后，可使用 V 设置门限。")
    }
    QGCLabel {
        Layout.fillWidth: true
        wrapMode: Text.WordWrap
        text: root.actionFact ? qsTr("当前严重低电量动作：%1").arg(root.actionFact.enumStringValue) : qsTr("保护动作参数不可用")
    }
    QGCButton {
        objectName: "caacConfirmBatteryProtection"
        text: qsTr("确认电池设置")
        Layout.fillWidth: true
        enabled: root.available && !!root.lowThreshold && !!root.criticalThreshold
        onClicked: {
            if (!lowCard.validate() || !criticalCard.validate()) { return }
            const low = lowCard.edited ? lowCard.rawThreshold() : Number(root.lowThreshold.rawValue)
            const critical = criticalCard.edited ? criticalCard.rawThreshold() : Number(root.criticalThreshold.rawValue)
            if (low <= critical) {
                mainWindow.showMessageDialog(qsTr("电池设置"), qsTr("低电量电压门限必须高于严重低电量门限。"))
                return
            }
            if (lowCard.edited) { root.lowThreshold.rawValue = low; lowCard.edited = false }
            if (criticalCard.edited) { root.criticalThreshold.rawValue = critical; criticalCard.edited = false }
            if (root.actionEdited && root.supportsAction(root.draftAction)) {
                root.actionFact.rawValue = root.draftAction
                root.actionEdited = false
            }
            lowCard.refreshValue()
            criticalCard.refreshValue()
        }
    }
}
