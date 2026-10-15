pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import QGroundControl
import QGroundControl.Controls

Item {
    id: root

    required property var sideStates
    required property var sideVisible
    required property var sideRotating
    property var vehicle: globals.activeVehicle
    property bool calibrationActive: false
    property bool calibrationEnabled: true
    property bool compassAvailable: true
    property bool accelAvailable: true
    property bool levelAvailable: true
    property bool levelEnabled: true
    property bool accelComplete: false
    property bool compassComplete: false
    property bool nextEnabled: false
    property bool persistentActions: false
    property bool cancelEnabled: calibrationActive
    property real progress: 0
    property string helpText: ""
    property string statusText: ""
    property string sectionId: ""
    property string currentType: "accel"
    property string cancelObjectName: "sensorsSetup_cancelCalibration"
    property Component additionalControls
    readonly property var idleStates: [0, 0, 0, 0, 0, 0]
    readonly property var allSides: [true, true, true, true, true, true]
    readonly property var stillSides: [false, false, false, false, false, false]
    readonly property real margins: ScreenTools.defaultFontPixelHeight * 0.6
    readonly property real previewHeight: Math.max(ScreenTools.defaultFontPixelHeight * 10,
        Math.min(width * 0.7, scroller.height * 0.64))

    signal startAccel()
    signal startLevel()
    signal startCompass()
    signal next()
    signal cancel()

    function focusSection() {
        let section = accelSection
        switch (sectionId) {
        case "Compass": currentType = "compass"; section = compassSection; break
        case "Level Horizon": currentType = "level"; section = levelSection; break
        case "Gyroscope": currentType = "gyro"; section = extraSection; break
        case "Orientations":
        case "Airspeed": section = extraSection; break
        default: currentType = "accel"; break
        }
        scroller.contentY = Math.max(0, Math.min(section.y, scroller.contentHeight - scroller.height))
    }

    function focusCalibration() {
        const section = currentType === "compass" ? compassSection
            : currentType === "level" ? levelSection : currentType === "gyro" ? extraSection : accelSection
        scroller.contentY = Math.max(0, Math.min(section.y, scroller.contentHeight - scroller.height))
    }

    onSectionIdChanged: Qt.callLater(focusSection)
    onCalibrationActiveChanged: if (calibrationActive) { Qt.callLater(focusCalibration) }
    Component.onCompleted: Qt.callLater(focusSection)

    QGCPalette { id: calPalette }

    component CalibrationButton: IndicatorButton {
        Layout.fillWidth: false
        Layout.alignment: Qt.AlignHCenter
        implicitWidth: contentItem.implicitWidth + leftPadding + rightPadding
        implicitHeight: contentItem.implicitHeight + topPadding + bottomPadding
        _horizontalPadding: ScreenTools.defaultFontPixelWidth
        rightPadding: _horizontalPadding + (enabled && !indicatorGreen
            ? ScreenTools.defaultFontPixelHeight * 0.75 + ScreenTools.defaultFontPixelWidth / 3 : 0)
        heightFactor: 0.3
        fontWeight: Font.Bold
    }

    ColumnLayout {
        anchors.fill: parent
        spacing: root.margins

        RowLayout {
            Layout.fillWidth: true
            visible: root.calibrationActive || root.persistentActions

            ProgressBar {
                objectName: "sensorsSetup_progressBar"
                Layout.fillWidth: true
                value: root.progress
            }
            SettingsActionButton {
                objectName: "sensorsSetup_nextButton"
                text: qsTr("确认当前姿态")
                visible: root.nextEnabled || root.persistentActions
                enabled: root.nextEnabled
                onClicked: root.next()
            }
            SettingsActionButton {
                objectName: root.cancelObjectName
                text: qsTr("取消校准")
                enabled: root.cancelEnabled
                onClicked: root.cancel()
            }
        }

        QGCFlickable {
            id: scroller
            objectName: "sensorsSetup_buttonScroller"
            Layout.fillWidth: true
            Layout.fillHeight: true
            contentHeight: sections.height
            clip: true

            ColumnLayout {
                id: sections
                width: scroller.width
                spacing: root.margins * 2

                QGCLabel {
                    Layout.fillWidth: true
                    text: root.calibrationActive ? root.helpText : qsTr("选择校准项目，按提示摆放无人机。")
                    wrapMode: Text.WordWrap
                }

                ColumnLayout {
                    id: accelSection
                    visible: root.accelAvailable
                    Layout.fillWidth: true
                    spacing: root.margins

                    SectionTitle { title: qsTr("校准加速度计") }
                    QGCLabel {
                        Layout.fillWidth: true
                        text: root.currentType === "accel" && root.calibrationActive ? root.helpText
                            : qsTr("%1 / 6 · 将无人机放在平坦表面，按六种姿态依次保持静止。")
                                .arg(root.currentType === "accel" ? root.sideStates.filter(state => state === 3).length : 0)
                        wrapMode: Text.WordWrap
                    }
                    SensorCalibrationPreview {
                        objectName: root.currentType === "accel" ? "sensorCalibrationPreview" : "sensorCalibrationAccelPreview"
                        Layout.fillWidth: true
                        Layout.preferredHeight: root.previewHeight
                        indicatorPrefix: root.currentType === "accel" ? "sensorsCal_" : "sensorsAccel_"
                        sideStates: root.currentType === "accel" ? root.sideStates : root.idleStates
                        sideVisible: root.currentType === "accel" && root.calibrationActive ? root.sideVisible : root.allSides
                        sideRotating: root.currentType === "accel" ? root.sideRotating : root.stillSides
                    }
                    CalibrationButton {
                        objectName: "sensorsSetup_calibrateAccel"
                        indicatorGreen: root.accelComplete
                        text: qsTr("Start Calibration")
                        enabled: root.calibrationEnabled && !root.calibrationActive && !motorTest.motorsEnabled
                        onClicked: { root.currentType = "accel"; root.startAccel() }
                    }
                }

                ColumnLayout {
                    id: levelSection
                    visible: root.levelAvailable
                    Layout.fillWidth: true
                    spacing: root.margins

                    SectionTitle { title: qsTr("校准水平") }
                    QGCLabel {
                        Layout.fillWidth: true
                        text: qsTr("将无人机水平放置于平坦表面后点击校准。")
                        wrapMode: Text.WordWrap
                    }
                    SettingsActionButton {
                        objectName: "sensorsSetup_calibrateLevel"
                        text: qsTr("校准水平")
                        enabled: root.levelEnabled && root.calibrationEnabled && !root.calibrationActive && !motorTest.motorsEnabled
                        onClicked: { root.currentType = "level"; root.startLevel() }
                    }
                }

                ColumnLayout {
                    id: compassSection
                    Layout.fillWidth: true
                    spacing: root.margins
                    visible: root.compassAvailable

                    SectionTitle { title: qsTr("校准罗盘") }
                    QGCLabel {
                        Layout.fillWidth: true
                        text: root.currentType === "compass" && root.calibrationActive ? root.helpText
                            : qsTr("校准方式由飞控决定，请按姿态提示旋转无人机。")
                        wrapMode: Text.WordWrap
                    }
                    SensorCalibrationPreview {
                        objectName: root.currentType === "compass" ? "sensorCalibrationPreview" : "sensorCalibrationCompassPreview"
                        Layout.fillWidth: true
                        Layout.preferredHeight: root.previewHeight
                        indicatorPrefix: root.currentType === "compass" ? "sensorsCal_" : "sensorsCompass_"
                        sideStates: root.currentType === "compass" ? root.sideStates : root.idleStates
                        sideVisible: root.currentType === "compass" && root.calibrationActive ? root.sideVisible : root.allSides
                        sideRotating: root.currentType === "compass" ? root.sideRotating : root.stillSides
                    }
                    CalibrationButton {
                        objectName: "sensorsSetup_calibrateCompass"
                        indicatorGreen: root.compassComplete
                        text: qsTr("Start Calibration")
                        enabled: root.calibrationEnabled && !root.calibrationActive && !motorTest.motorsEnabled
                        onClicked: { root.currentType = "compass"; root.startCompass() }
                    }
                }

                ColumnLayout {
                    id: extraSection
                    Layout.fillWidth: true
                    spacing: root.margins

                    SectionTitle { title: qsTr("其他传感器设置") }
                    SensorCalibrationPreview {
                        objectName: "sensorCalibrationPreview"
                        Layout.fillWidth: true
                        Layout.preferredHeight: root.previewHeight
                        visible: root.currentType === "gyro"
                        indicatorPrefix: root.currentType === "gyro" ? "sensorsCal_" : "sensorsGyro_"
                        sideStates: root.sideStates
                        sideVisible: root.calibrationActive ? root.sideVisible : [true, false, false, false, false, false]
                        sideRotating: root.stillSides
                    }
                    Loader {
                        Layout.fillWidth: true
                        sourceComponent: root.additionalControls
                        enabled: root.calibrationEnabled && !root.calibrationActive && !motorTest.motorsEnabled
                    }
                    QGCLabel {
                        Layout.fillWidth: true
                        text: root.statusText
                        wrapMode: Text.WordWrap
                        visible: text !== "" && (root.calibrationActive || root.progress > 0)
                    }
                }

                ColumnLayout {
                    Layout.fillWidth: true
                    spacing: root.margins

                    SectionTitle { title: qsTr("电机测试") }
                    SensorCalibrationMotorTest {
                        id: motorTest
                        Layout.fillWidth: true
                        vehicle: root.vehicle
                        calibrationActive: root.calibrationActive
                    }
                }
            }
        }
    }

    component SectionTitle: RowLayout {
        required property string title
        Layout.fillWidth: true
        spacing: root.margins * 0.5
        Rectangle {
            width: ScreenTools.defaultFontPixelWidth * 0.35
            height: ScreenTools.defaultFontPixelHeight
            color: calPalette.settingsAccent
            radius: width * 0.5
        }
        QGCLabel { text: parent.title; Layout.fillWidth: true }
    }
}
