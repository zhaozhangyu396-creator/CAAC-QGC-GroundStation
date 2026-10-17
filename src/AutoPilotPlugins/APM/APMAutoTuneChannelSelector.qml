import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import QGroundControl
import QGroundControl.FactControls
import QGroundControl.Controls

ColumnLayout {
    id:     root
    Layout.minimumWidth: vehicleSetupStyle ? 0 : -1

    required property var controller
    readonly property bool vehicleSetupStyle: ScreenTools.isVehicleSetupItem(root)

    property Fact _ch7Opt:  controller.getParameterFact(-1, "RC7_OPTION")
    property Fact _ch8Opt:  controller.getParameterFact(-1, "RC8_OPTION")
    property Fact _ch9Opt:  controller.getParameterFact(-1, "RC9_OPTION")
    property Fact _ch10Opt: controller.getParameterFact(-1, "RC10_OPTION")
    property Fact _ch11Opt: controller.getParameterFact(-1, "RC11_OPTION")
    property Fact _ch12Opt: controller.getParameterFact(-1, "RC12_OPTION")

    readonly property int   _firstOptionChannel:    7
    readonly property int   _lastOptionChannel:     12

    property int    _autoTuneSwitchChannelIndex:    0
    readonly property int _autoTuneOption:          17

    Component.onCompleted: calcAutoTuneChannel()

    function calcAutoTuneChannel() {
        _autoTuneSwitchChannelIndex = 0
        for (let channel = _firstOptionChannel; channel <= _lastOptionChannel; channel++) {
            let optionFact = controller.getParameterFact(-1, "RC" + channel + "_OPTION")
            if (optionFact.value === _autoTuneOption) {
                _autoTuneSwitchChannelIndex = channel - _firstOptionChannel + 1
                break
            }
        }
    }

    function setChannelAutoTuneOption(channel) {
        for (let optionChannel = _firstOptionChannel; optionChannel <= _lastOptionChannel; optionChannel++) {
            let optionFact = controller.getParameterFact(-1, "RC" + optionChannel + "_OPTION")
            if (optionFact.value === _autoTuneOption) {
                optionFact.value = 0
            }
        }
        if (channel !== 0) {
            let optionFact = controller.getParameterFact(-1, "RC" + channel + "_OPTION")
            optionFact.value = _autoTuneOption
        }
    }

    Connections { target: _ch7Opt;  function onValueChanged() { calcAutoTuneChannel() } }
    Connections { target: _ch8Opt;  function onValueChanged() { calcAutoTuneChannel() } }
    Connections { target: _ch9Opt;  function onValueChanged() { calcAutoTuneChannel() } }
    Connections { target: _ch10Opt; function onValueChanged() { calcAutoTuneChannel() } }
    Connections { target: _ch11Opt; function onValueChanged() { calcAutoTuneChannel() } }
    Connections { target: _ch12Opt; function onValueChanged() { calcAutoTuneChannel() } }

    GridLayout {
        id: channelRow
        Layout.fillWidth: true
        Layout.minimumWidth: root.vehicleSetupStyle ? 0 : -1
        columns: _stacked ? 1 : 2
        columnSpacing: ScreenTools.defaultFontPixelWidth
        rowSpacing: ScreenTools.defaultFontPixelHeight * 0.25
        readonly property bool _stacked: root.vehicleSetupStyle
            && width < ScreenTools.defaultFontPixelWidth * 40

        QGCLabel {
            Layout.fillWidth: true
            Layout.minimumWidth: root.vehicleSetupStyle ? 0 : implicitWidth
            text: qsTr("Channel for AutoTune switch:")
            wrapMode: root.vehicleSetupStyle ? Text.WordWrap : Text.NoWrap
        }

        QGCComboBox {
            objectName: "apmAutoTuneChannelComboBox"
            Layout.fillWidth: channelRow._stacked
            Layout.preferredWidth: ScreenTools.defaultFontPixelWidth * 30
            Layout.minimumWidth: root.vehicleSetupStyle ? 0 : -1
            Layout.maximumWidth: root.vehicleSetupStyle && !channelRow._stacked
                ? Math.max(0, channelRow.width * 0.6) : Number.POSITIVE_INFINITY
            sizeToContents: true
            model: [qsTr("None"), qsTr("Channel 7"), qsTr("Channel 8"), qsTr("Channel 9"), qsTr("Channel 10"), qsTr("Channel 11"), qsTr("Channel 12")]
            currentIndex: root._autoTuneSwitchChannelIndex

            onActivated: (index) => {
                let channel = index
                if (channel > 0) {
                    channel += 6
                }
                root.setChannelAutoTuneOption(channel)
            }
        }
    }
}
