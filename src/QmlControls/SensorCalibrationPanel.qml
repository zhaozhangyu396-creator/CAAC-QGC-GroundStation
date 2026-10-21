pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import QGroundControl
import QGroundControl.Controls

Item {
    id: root
    objectName: "sensorsSetup_inlinePanel"

    required property var sideStates
    required property var sideVisible
    required property var sideRotating
    property var vehicle: globals.activeVehicle
    property bool calibrationActive: false
    property bool calibrationEnabled: true
    property bool orientationAvailable: true
    property bool orientationExpanded: !vehicle
    property bool compassAvailable: true
    property bool compassMotionGuidance: false
    property real compassAttitudeUpdateTime: 0
    property bool accelAvailable: true
    property bool levelAvailable: true
    property bool pressureAvailable: false
    property string pressureHelpText: ""
    property bool compassMotAvailable: false
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
    property Component orientationControls
    property Component additionalControls
    property bool preparationActive: false
    property string preparationText: ""
    property Component preparationControls
    property alias preparationItem: preparationLoader.item
    property string confirmObjectName: "sensorsSetup_confirmCalibration"
    property string closeObjectName: "sensorsSetup_closeCalibration"
    property bool resultVisible: false
    property string resultTitle: ""
    property string resultText: ""
    property Component resultControls
    property string resultObjectName: "sensorsSetup_calibrationResult"
    property string waitingText: ""
    property bool orientationPreviewVisible: true
    property bool motorTestAvailable: true
    readonly property var idleStates: [0, 0, 0, 0, 0, 0]
    readonly property var allSides: [true, true, true, true, true, true]
    readonly property var stillSides: [false, false, false, false, false, false]
    readonly property real margins: ScreenTools.defaultFontPixelHeight * 0.6
    readonly property bool _previewSection: (sectionId === "" || sectionId === "Accelerometer"
        || sectionId === "Compass") && (currentType === "accel" || currentType === "compass")
    readonly property bool _workflowVisible: preparationActive || calibrationActive || resultVisible
        || statusText !== "" || helpText !== "" || waitingText !== ""
        || (orientationPreviewVisible && _previewSection) || (persistentActions && currentType === "accel")

    signal startAccel()
    signal setOrientation()
    signal startLevel()
    signal startCompass()
    signal startPressure()
    signal startCompassMot()
    signal next()
    signal cancel()
    signal confirmPreparation()
    signal dismissPreparation()
    signal acknowledgeResult()

    function matchesSection(id) {
        return sectionId === "" || sectionId === id
    }

    function focusOrientation() {
        orientationExpanded = true
        Qt.callLater(focusSection)
    }

    function focusSection() {
        switch (sectionId) {
        case "Compass": currentType = "compass"; break
        case "Level Horizon": currentType = "level"; break
        case "Gyroscope": currentType = "gyro"; break
        case "Airspeed": currentType = "airspeed"; break
        case "Pressure": currentType = "pressure"; break
        case "CompassMot": currentType = "compassmot"; break
        default: currentType = "accel"; break
        }
        scroller.contentY = 0
    }

    function focusCalibration() {
        scroller.contentY = Math.max(0, Math.min(workflow.y, scroller.contentHeight - scroller.height))
    }

    onSectionIdChanged: {
        if (motorTest) { motorTest.stopTesting() }
        Qt.callLater(focusSection)
    }
    onCalibrationActiveChanged: if (calibrationActive) { Qt.callLater(focusCalibration) }
    onPreparationActiveChanged: if (preparationActive) { Qt.callLater(focusCalibration) }
    onResultVisibleChanged: if (resultVisible) { Qt.callLater(focusCalibration) }
    Component.onCompleted: Qt.callLater(focusSection)

    QGCPalette { id: calPalette }

    component CalibrationButton: IndicatorButton {
        Layout.fillWidth: true
        implicitWidth: contentItem.implicitWidth + leftPadding + rightPadding
        implicitHeight: contentItem.implicitHeight + topPadding + bottomPadding
        _horizontalPadding: ScreenTools.defaultFontPixelWidth
        rightPadding: _horizontalPadding + (enabled && !indicatorGreen
            ? ScreenTools.defaultFontPixelHeight * 0.75 + ScreenTools.defaultFontPixelWidth / 3 : 0)
        heightFactor: 0.3
        fontWeight: Font.Bold
    }

    QGCFlickable {
        id: scroller
        objectName: "sensorsSetup_buttonScroller"
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right
        height: Math.min(root.height, sections.implicitHeight)
        contentWidth: width
        contentHeight: sections.implicitHeight
        flickableDirection: Flickable.VerticalFlick
        clip: true
        onContentHeightChanged: returnToBounds()

        ColumnLayout {
            id: sections
            width: scroller.width
            height: implicitHeight
            spacing: root.margins

            ColumnLayout {
                visible: root.orientationAvailable && root.matchesSection("Orientations")
                Layout.fillWidth: true
                spacing: root.margins
                SectionTitle { title: qsTr("飞控朝向") }
                SettingsActionButton {
                    objectName: "sensorsSetup_setOrientation"
                    Layout.fillWidth: true
                    text: qsTr("飞控朝向")
                    checked: root.orientationExpanded
                    enabled: root.calibrationEnabled && !root.calibrationActive && !root.preparationActive
                        && (!root.vehicle || !root.vehicle.armed) && !motorTest.motorsEnabled
                    onClicked: {
                        root.orientationExpanded = !root.orientationExpanded
                        if (root.orientationExpanded) { root.setOrientation() }
                    }
                }
                Loader {
                    objectName: "sensorsSetup_orientationInlineLoader"
                    Layout.fillWidth: true
                    visible: root.orientationExpanded
                    active: root.orientationExpanded && root.matchesSection("Orientations")
                    enabled: root.calibrationEnabled && !root.calibrationActive
                        && (!root.vehicle || !root.vehicle.armed) && !motorTest.motorsEnabled
                    sourceComponent: root.orientationControls || offlineOrientationControls
                }
            }

            ColumnLayout {
                visible: root.accelAvailable && root.matchesSection("Accelerometer")
                Layout.fillWidth: true
                spacing: root.margins
                SectionTitle { title: qsTr("校准加速度计") }
                QGCLabel {
                    Layout.fillWidth: true
                    text: qsTr("%1 / 6 · 将无人机放在平坦表面，按六种姿态依次保持静止。")
                        .arg(root.currentType === "accel" ? root.sideStates.filter(state => state === 3).length : 0)
                    wrapMode: Text.WordWrap
                }
                CalibrationButton {
                    objectName: "sensorsSetup_calibrateAccel"
                    indicatorGreen: root.accelComplete
                    text: qsTr("Start Calibration")
                    enabled: root.calibrationEnabled && !root.calibrationActive && !root.preparationActive && !motorTest.motorsEnabled
                    onClicked: { root.currentType = "accel"; root.startAccel() }
                }
            }

            ColumnLayout {
                visible: root.levelAvailable && root.matchesSection("Level Horizon")
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
                    Layout.fillWidth: true
                    text: qsTr("校准水平")
                    enabled: root.levelEnabled && root.calibrationEnabled && !root.calibrationActive && !root.preparationActive
                        && !motorTest.motorsEnabled
                    onClicked: { root.currentType = "level"; root.startLevel() }
                }
            }

            ColumnLayout {
                visible: root.compassAvailable && root.matchesSection("Compass")
                Layout.fillWidth: true
                spacing: root.margins
                SectionTitle { title: qsTr("校准罗盘") }
                QGCLabel {
                    Layout.fillWidth: true
                    text: qsTr("校准方式由飞控决定，请按姿态提示旋转无人机。")
                    wrapMode: Text.WordWrap
                }
                CalibrationButton {
                    objectName: "sensorsSetup_calibrateCompass"
                    indicatorGreen: root.compassComplete
                    text: qsTr("Start Calibration")
                    enabled: root.calibrationEnabled && !root.calibrationActive && !root.preparationActive && !motorTest.motorsEnabled
                    onClicked: { root.currentType = "compass"; root.startCompass() }
                }
            }

            ColumnLayout {
                visible: root.pressureAvailable && root.matchesSection("Pressure")
                Layout.fillWidth: true
                spacing: root.margins
                SectionTitle { title: qsTr("Calibrate Barometer") }
                QGCLabel {
                    Layout.fillWidth: true
                    text: root.pressureHelpText
                    wrapMode: Text.WordWrap
                }
                SettingsActionButton {
                    objectName: "sensorsSetup_calibratePressure"
                    Layout.fillWidth: true
                    text: qsTr("Start Calibration")
                    enabled: root.calibrationEnabled && !root.calibrationActive && !root.preparationActive
                        && !motorTest.motorsEnabled
                    onClicked: { root.currentType = "pressure"; root.startPressure() }
                }
            }

            ColumnLayout {
                visible: root.compassMotAvailable && root.matchesSection("CompassMot")
                Layout.fillWidth: true
                spacing: root.margins
                SectionTitle { title: qsTr("Compass Motor Interference Calibration") }
                QGCLabel {
                    Layout.fillWidth: true
                    text: qsTr("Measure compass interference from the motors. Review the setup instructions before starting calibration.")
                    wrapMode: Text.WordWrap
                }
                SettingsActionButton {
                    objectName: "sensorsSetup_calibrateCompassMot"
                    Layout.fillWidth: true
                    text: qsTr("Start Calibration")
                    enabled: root.calibrationEnabled && !root.calibrationActive && !root.preparationActive
                        && !motorTest.motorsEnabled
                    onClicked: { root.currentType = "compassmot"; root.startCompassMot() }
                }
            }

            Loader {
                Layout.fillWidth: true
                Layout.preferredHeight: (item as Item)?.implicitHeight ?? 0
                visible: root.additionalControls !== null
                sourceComponent: root.additionalControls
                enabled: root.calibrationEnabled && !root.calibrationActive && !root.preparationActive && !motorTest.motorsEnabled
            }

            ColumnLayout {
                id: workflow
                objectName: "sensorsSetup_inlineWorkflow"
                Layout.fillWidth: true
                spacing: root.margins
                visible: root._workflowVisible && root.sectionId !== "Orientations"

                ColumnLayout {
                    objectName: "sensorsSetup_inlinePreparation"
                    Layout.fillWidth: true
                    spacing: root.margins
                    visible: root.preparationActive
                    QGCLabel {
                        Layout.fillWidth: true
                        text: root.preparationText
                        wrapMode: Text.WordWrap
                        visible: text !== ""
                    }
                    Loader {
                        id: preparationLoader
                        Layout.fillWidth: true
                        active: root.preparationActive
                        sourceComponent: root.preparationControls
                    }
                    SettingsActionButton {
                        objectName: root.confirmObjectName
                        Layout.fillWidth: true
                        text: qsTr("Start Calibration")
                        enabled: root.calibrationEnabled && !root.calibrationActive
                            && (!root.vehicle || !root.vehicle.armed) && !motorTest.motorsEnabled
                        onClicked: root.confirmPreparation()
                    }
                    SettingsActionButton {
                        objectName: root.closeObjectName
                        Layout.fillWidth: true
                        text: qsTr("Cancel")
                        onClicked: root.dismissPreparation()
                    }
                }

                QGCLabel {
                    objectName: "sensorsSetup_calibrationHelp"
                    Layout.fillWidth: true
                    text: root.waitingText !== "" ? root.waitingText : root.helpText
                    wrapMode: Text.WordWrap
                    visible: text !== "" && !root.preparationActive
                }
                QGCLabel {
                    Layout.fillWidth: true
                    visible: root.currentType === "compass" && (root.calibrationActive || root.resultVisible)
                    text: qsTr("飞控总体进度 %1%").arg(Math.round(root.progress * 100))
                }
                ProgressBar {
                    objectName: "sensorsSetup_progressBar"
                    Layout.fillWidth: true
                    value: root.progress
                    visible: root.calibrationActive || root.resultVisible || root.progress > 0
                }
                SettingsActionButton {
                    objectName: "sensorsSetup_nextButton"
                    Layout.fillWidth: true
                    text: root.currentType === "compassmot" ? qsTr("Finish Calibration") : qsTr("确认当前姿态")
                    visible: root.nextEnabled || (root.persistentActions && root.currentType === "accel")
                    enabled: root.nextEnabled && root.waitingText === ""
                    onClicked: root.next()
                }
                SettingsActionButton {
                    objectName: root.cancelObjectName
                    Layout.fillWidth: true
                    text: qsTr("取消校准")
                    visible: root.calibrationActive || (root.persistentActions && root.currentType === "accel")
                    enabled: root.cancelEnabled && root.waitingText === ""
                    onClicked: root.cancel()
                }
                QGCLabel {
                    Layout.fillWidth: true
                    visible: root.currentType === "compass" && root.orientationPreviewVisible
                    text: root.compassMotionGuidance
                        ? qsTr("按六个姿态分别绕竖直轴转动一周。分面进度表示姿态转动完成度；罗盘校准结果以飞控反馈为准。")
                        : qsTr("按动图转动无人机，各面进度由飞控反馈更新。")
                    wrapMode: Text.WordWrap
                }
                SensorCalibrationPreview {
                    objectName: "sensorCalibrationPreview"
                    Layout.fillWidth: true
                    Layout.preferredHeight: implicitHeight
                    visible: root.orientationPreviewVisible && root._previewSection && !root.preparationActive
                    compassMode: root.currentType === "compass"
                    motionGuidance: root.compassMotionGuidance
                    calibrationActive: root.calibrationActive
                    overallProgress: root.progress
                    attitudeUpdateTime: root.compassAttitudeUpdateTime
                    vehicle: root.vehicle
                    sideStates: root.sideStates
                    sideVisible: root.currentType === "compass" && root.compassMotionGuidance ? root.allSides
                        : root.calibrationActive ? root.sideVisible : root.allSides
                    sideRotating: root.sideRotating
                }
                QGCLabel {
                    objectName: "sensorsSetup_statusLog"
                    Layout.fillWidth: true
                    text: root.statusText
                    wrapMode: Text.WordWrap
                    visible: text !== ""
                }
                ColumnLayout {
                    objectName: root.resultObjectName
                    Layout.fillWidth: true
                    spacing: root.margins
                    visible: root.resultVisible
                    QGCLabel {
                        Layout.fillWidth: true
                        text: root.resultTitle
                        wrapMode: Text.WordWrap
                        font.bold: true
                        visible: text !== ""
                    }
                    Loader {
                        Layout.fillWidth: true
                        active: root.resultVisible
                        sourceComponent: root.resultControls
                    }
                    QGCLabel {
                        Layout.fillWidth: true
                        text: root.resultText
                        wrapMode: Text.WordWrap
                        visible: text !== ""
                    }
                    SettingsActionButton {
                        objectName: "sensorsSetup_acknowledgeCalibration"
                        Layout.fillWidth: true
                        text: qsTr("Close")
                        onClicked: root.acknowledgeResult()
                    }
                }
            }

            ColumnLayout {
                Layout.fillWidth: true
                spacing: root.margins
                visible: root.motorTestAvailable && (root.sectionId === "" || root.sectionId === "Orientations")
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

    Component {
        id: offlineOrientationControls
        ColumnLayout {
            objectName: "sensorsSetup_orientationControls"
            QGCComboBox {
                Layout.fillWidth: true
                Layout.minimumWidth: 0
                model: [qsTr("默认朝向")]
                enabled: false
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
            color: calPalette.settingsSectionAccent
            radius: width * 0.5
        }
        QGCLabel { text: parent.title; Layout.fillWidth: true; wrapMode: Text.WordWrap }
    }
}
