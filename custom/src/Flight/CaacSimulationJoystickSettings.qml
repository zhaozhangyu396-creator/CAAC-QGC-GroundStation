pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import QGroundControl
import QGroundControl.Controls

ColumnLayout {
    id: root
    readonly property var _simulation: QGroundControl.corePlugin.simulation
    readonly property var _joystick: joystickManager.activeJoystick
    readonly property var _settings: _joystick ? _joystick.settings : null
    readonly property bool _calibrated: _settings && _settings.calibrated.rawValue
    property bool expanded: false
    signal calibrationRequested()

    function configure(fact, value) {
        _settings.simulationModeSwitchEnabled.rawValue = false
        fact.rawValue = value
    }

    QGCButton {
        objectName: "caacSimulationJoystickMappingToggle"
        Layout.fillWidth: true
        text: (root.expanded ? "▾ " : "▸ ") + qsTr("USB joystick channel mapping")
        onClicked: root.expanded = !root.expanded
    }
    ColumnLayout {
        Layout.fillWidth: true
        visible: root.expanded
        spacing: ScreenTools.defaultFontPixelHeight * 0.5
        QGCLabel {
            Layout.fillWidth: true
            wrapMode: Text.WordWrap
            text: root._joystick ? root._joystick.name : qsTr("Connect a USB joystick to view its channels.")
        }
        QGCLabel {
            Layout.fillWidth: true
            wrapMode: Text.WordWrap
            text: qsTr("Calibrate and enable the joystick in Aircraft settings → Joystick. Attitude training does not hold horizontal position; center throttle holds altitude.")
        }
        QGCButton {
            Layout.fillWidth: true
            text: qsTr("Joystick calibration and axis mapping")
            onClicked: root.calibrationRequested()
        }
        QGCCheckBox {
            objectName: "caacSimulationJoystickEnabled"
            text: qsTr("Enable USB joystick")
            enabled: root._calibrated
            checked: joystickManager.activeJoystickEnabledForActiveVehicle
            onClicked: joystickManager.activeJoystickEnabledForActiveVehicle = checked
        }
        QGCLabel {
            objectName: "caacSimulationJoystickChannelMonitor"
            Layout.fillWidth: true
            wrapMode: Text.WordWrap
            font.pointSize: ScreenTools.smallFontPointSize
            text: root._simulation.joystickChannels.length
                ? root._simulation.joystickChannels.map((value, index) => "CH" + (index + 1) + ": " + value).join("   ")
                : qsTr("Channel values appear after calibration and enabling the joystick.")
        }
        RowLayout {
            Layout.fillWidth: true
            QGCLabel { text: qsTr("Mode switch channel") }
            QGCComboBox {
                objectName: "caacSimulationModeSwitchAxis"
                Layout.fillWidth: true
                enabled: !!root._settings
                model: [qsTr("Select channel")].concat(Array.from({length: root._joystick ? root._joystick.axisCount : 0}, (_, i) => "CH" + (i + 1)))
                currentIndex: root._settings ? Number(root._settings.simulationModeSwitchAxis.rawValue) + 1 : 0
                onActivated: (index) => root.configure(root._settings.simulationModeSwitchAxis, index - 1)
            }
        }
        RowLayout {
            Layout.fillWidth: true
            QGCComboBox {
                objectName: "caacSimulationModeSwitchPositions"
                Layout.fillWidth: true
                enabled: !!root._settings
                model: [qsTr("Two positions"), qsTr("Three positions")]
                currentIndex: root._settings ? Number(root._settings.simulationModeSwitchPositions.rawValue) - 2 : 1
                onActivated: (index) => root.configure(root._settings.simulationModeSwitchPositions, index + 2)
            }
            QGCCheckBox {
                objectName: "caacSimulationModeSwitchReversed"
                text: qsTr("Reverse")
                enabled: !!root._settings
                checked: root._settings ? root._settings.simulationModeSwitchReversed.rawValue : false
                onClicked: root.configure(root._settings.simulationModeSwitchReversed, checked)
            }
        }
        Repeater {
            model: root._settings ? [
                {label: qsTr("Low position"), fact: root._settings.simulationModeLow},
                {label: qsTr("Middle position"), fact: root._settings.simulationModeMiddle},
                {label: qsTr("High position"), fact: root._settings.simulationModeHigh}
            ] : []
            RowLayout {
                id: modeRow
                required property var modelData
                required property int index
                Layout.fillWidth: true
                visible: index !== 1 || (root._settings && Number(root._settings.simulationModeSwitchPositions.rawValue) === 3)
                QGCLabel { text: modeRow.modelData.label }
                QGCComboBox {
                    objectName: "caacSimulationModePosition" + modeRow.index
                    Layout.fillWidth: true
                    readonly property var modeFact: modeRow.modelData.fact
                    model: [qsTr("Hover"), qsTr("Attitude mode"), qsTr("Mission"), qsTr("Automatic return"), qsTr("Land")]
                    currentIndex: modeFact ? Number(modeFact.rawValue) : 0
                    onActivated: (index) => root.configure(modeFact, index)
                }
            }
        }
        QGCCheckBox {
            objectName: "caacSimulationModeSwitchEnabled"
            text: qsTr("Enable simulation mode switch")
            enabled: !!root._settings && root._simulation.modeSwitchUnavailableReason.length === 0
            checked: root._settings ? root._settings.simulationModeSwitchEnabled.rawValue : false
            onClicked: {
                if (checked && root._simulation.active
                    && root._simulation.vehicle === QGroundControl.multiVehicleManager.activeVehicle) {
                    joystickManager.activeJoystickEnabledForActiveVehicle = true
                }
                root._settings.simulationModeSwitchEnabled.rawValue = checked
            }
        }
        QGCLabel {
            objectName: "caacSimulationModeSwitchUnavailableReason"
            Layout.fillWidth: true
            visible: text.length > 0
            wrapMode: Text.WordWrap
            text: root._simulation.modeSwitchUnavailableReason
        }
        QGCLabel {
            Layout.fillWidth: true
            wrapMode: Text.WordWrap
            font.pointSize: ScreenTools.smallFontPointSize
            text: qsTr("USB channels are numbered from CH1. Choose a switch axis separate from the four control axes. Enable, then move the switch to change modes. The initial position only establishes a baseline. Mapping controls the built-in simulator only.")
        }
        QGCLabel {
            objectName: "caacSimulationModeSwitchPosition"
            Layout.fillWidth: true
            text: root._simulation.modeSwitchPosition < 0 ? qsTr("Switch position not acquired")
                : qsTr("Current position: %1").arg([qsTr("Low position"), qsTr("Middle position"), qsTr("High position")][root._simulation.modeSwitchPosition])
        }
    }
}
