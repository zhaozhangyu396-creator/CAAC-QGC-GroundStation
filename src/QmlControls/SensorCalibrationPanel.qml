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
    readonly property bool _previewSection: sectionId === "" || sectionId === "Accelerometer"
        || sectionId === "Compass" || sectionId === "Gyroscope"
    readonly property bool _workflowVisible: preparationActive || calibrationActive || resultVisible
        || statusText !== "" || helpText !== "" || waitingText !== ""
        || (orientationPreviewVisible && _previewSection) || persistentActions

    signal startAccel()
    signal setOrientation()
    signal startLevel()
    signal startCompass()
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
        anchors.fill: parent
        contentWidth: width
        contentHeight: sections.height
        flickableDirection: Flickable.VerticalFlick
        clip: true

        ColumnLayout {
            id: sections
            width: scroller.width
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

            Loader {
                Layout.fillWidth: true
                visible: item && item.implicitHeight > 0
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
                ProgressBar {
                    objectName: "sensorsSetup_progressBar"
                    Layout.fillWidth: true
                    value: root.progress
                    visible: root.calibrationActive || root.persistentActions || root.resultVisible || root.progress > 0
                }
                SettingsActionButton {
                    objectName: "sensorsSetup_nextButton"
                    Layout.fillWidth: true
                    text: qsTr("确认当前姿态")
                    visible: root.nextEnabled || root.persistentActions
                    enabled: root.nextEnabled && root.waitingText === ""
                    onClicked: root.next()
                }
                SettingsActionButton {
                    objectName: root.cancelObjectName
                    Layout.fillWidth: true
                    text: qsTr("取消校准")
                    visible: root.calibrationActive || root.persistentActions
                    enabled: root.cancelEnabled && root.waitingText === ""
                    onClicked: root.cancel()
                }
                SensorCalibrationPreview {
                    objectName: "sensorCalibrationPreview"
                    Layout.fillWidth: true
                    Layout.preferredHeight: implicitHeight
                    visible: root.orientationPreviewVisible && root._previewSection && !root.preparationActive
                    sideStates: root.sideStates
                    sideVisible: root.calibrationActive ? root.sideVisible
                        : root.currentType === "gyro" ? [true, false, false, false, false, false] : root.allSides
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
