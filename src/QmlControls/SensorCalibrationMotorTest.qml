pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import QGroundControl
import QGroundControl.Controls

ColumnLayout {
    id: root

    property var vehicle: null
    property bool calibrationActive: false
    readonly property int motorCount: vehicle ? Math.max(0, vehicle.motorCount) : 0
    readonly property bool hasActuatorControls: !!(vehicle && vehicle.actuators)
    readonly property bool canTest: vehicle && !vehicle.armed && !vehicle.airborne
        && QGroundControl.multiVehicleManager.parameterReadyVehicleAvailable && !calibrationActive
        && motorCount > 0 && !hasActuatorControls
    readonly property bool motorsEnabled: safetyCheck.checked && canTest
    spacing: ScreenTools.defaultFontPixelHeight * 0.6

    function runMotor(index, percent) {
        if (!motorsEnabled) { return }
        if (index < 0) {
            for (let motor = 1; motor <= motorCount; ++motor) {
                vehicle.motorTest(motor, percent, percent > 0 ? 3 : 0, true)
            }
        } else {
            vehicle.motorTest(index + 1, percent, percent > 0 ? 3 : 0, true)
        }
    }

    function resetSliders() {
        for (let index = 0; index < sliders.count; ++index) {
            const item = sliders.itemAt(index)
            if (item) { item.throttle = 0 }
        }
    }

    onCanTestChanged: if (!canTest) { safetyCheck.checked = false; resetSliders() }
    onVehicleChanged: { safetyCheck.checked = false; resetSliders() }

    QGCPalette { id: motorPalette }

    QGCLabel {
        Layout.fillWidth: true
        text: qsTr("手动控制电机输出，用于检查电机顺序与方向。")
        wrapMode: Text.WordWrap
    }
    QGCLabel {
        Layout.fillWidth: true
        text: qsTr("请先拆除所有螺旋桨。每次测试输出持续 3 秒。")
        color: motorPalette.warningText
        wrapMode: Text.WordWrap
    }
    QGCLabel {
        Layout.fillWidth: true
        visible: root.hasActuatorControls
        text: qsTr("请在执行器设置中使用飞控提供的电机测试。")
        wrapMode: Text.WordWrap
    }
    QGCLabel {
        Layout.fillWidth: true
        visible: root.motorCount === 0
        text: qsTr("飞控尚未提供电机数量，请在电机或执行器设置中确认。")
        wrapMode: Text.WordWrap
    }
    QGCCheckBox {
        id: safetyCheck
        objectName: "sensorsSetup_enableMotorTest"
        text: qsTr("已拆除螺旋桨，启用电机测试")
        enabled: root.canTest
        onToggled: {
            if (!checked) {
                if (root.vehicle && root.canTest) {
                    for (let motor = 1; motor <= root.motorCount; ++motor) {
                        root.vehicle.motorTest(motor, 0, 0, true)
                    }
                }
                root.resetSliders()
            }
        }
    }

    Rectangle {
        Layout.fillWidth: true
        Layout.preferredHeight: ScreenTools.defaultFontPixelHeight * 10
        color: motorPalette.windowShade
        radius: ScreenTools.defaultBorderRadius
        visible: root.motorCount > 0

        QGCFlickable {
            anchors.fill: parent
            anchors.margins: ScreenTools.defaultFontPixelWidth
            contentWidth: sliderRow.width
            contentHeight: height
            clip: true

            Row {
                id: sliderRow
                height: parent.height
                spacing: ScreenTools.defaultFontPixelWidth
                enabled: root.motorsEnabled

                Repeater {
                    id: sliders
                    model: root.motorCount > 0 ? root.motorCount + 1 : 0

                    ColumnLayout {
                        id: motorColumn
                        required property int index
                        property alias throttle: throttleSlider.value
                        width: ScreenTools.defaultFontPixelWidth * 5
                        height: sliderRow.height

                        QGCLabel {
                            Layout.alignment: Qt.AlignHCenter
                            text: throttleSlider.value.toFixed(0) + "%"
                        }
                        QGCSlider {
                            id: throttleSlider
                            objectName: "sensorsSetup_motorSlider_" + motorColumn.index
                            Layout.fillHeight: true
                            Layout.alignment: Qt.AlignHCenter
                            orientation: Qt.Vertical
                            from: 0
                            to: 100
                            stepSize: 1
                            onMoved: root.runMotor(motorColumn.index - 1, Math.round(value))
                        }
                        QGCLabel {
                            Layout.alignment: Qt.AlignHCenter
                            text: motorColumn.index === 0 ? qsTr("全部") : qsTr("电机 %1").arg(motorColumn.index)
                        }
                    }
                }
            }
        }
    }
    QGCButton {
        Layout.fillWidth: true
        text: qsTr("停止所有电机")
        enabled: root.motorsEnabled
        onClicked: { root.runMotor(-1, 0); root.resetSliders() }
    }
}
