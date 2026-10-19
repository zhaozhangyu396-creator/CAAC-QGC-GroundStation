pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Layouts
import QtQuick.Controls

import "CompassRotation.js" as CompassRotation

import QGroundControl
import QGroundControl.Controls

Item {
    id: root
    implicitHeight: (showImages && !compassMode ? Math.max(ScreenTools.defaultFontPixelHeight * 8, width * 0.55) + spacing : 0)
        + poseGrid.implicitHeight

    required property var sideStates
    required property var sideVisible
    required property var sideRotating
    property int selectedSide: 0
    property bool showImages: true
    property bool compassMode: false
    property bool motionGuidance: false
    property bool calibrationActive: false
    property real overallProgress: 0
    property real attitudeUpdateTime: 0
    property var vehicle: null
    property var rotationTracker: CompassRotation.createTracker()
    property var rotationProgress: [0, 0, 0, 0, 0, 0]
    property int activeRotationSide: -1
    readonly property int completedSides: sideStates.filter(state => state === VehicleRotationCal.CalState.Completed).length
    readonly property int visibleSides: sideVisible.filter(Boolean).length

    function resetRotation() {
        rotationTracker = CompassRotation.createTracker()
        rotationProgress = rotationTracker.progress.slice()
        activeRotationSide = -1
    }

    function faceProgress(index) {
        if (motionGuidance) { return rotationProgress[index] }
        if (sideStates[index] === VehicleRotationCal.CalState.Completed) { return 1 }
        if (!calibrationActive || sideStates[index] !== VehicleRotationCal.CalState.InProgress) { return 0 }
        const step = Math.floor(100 / Math.max(1, visibleSides))
        const baseline = Math.floor(completedSides * 100 / Math.max(1, visibleSides))
        // PX4 reports overall sample progress; final fitting follows all six sides.
        return Math.max(0, Math.min(0.99, (overallProgress * 100 - baseline) / step))
    }

    onCalibrationActiveChanged: {
        if (calibrationActive) { resetRotation() }
        else { activeRotationSide = -1 }
    }
    onVehicleChanged: resetRotation()
    onCompassModeChanged: resetRotation()

    Timer {
        interval: 100
        repeat: true
        running: root.compassMode && root.motionGuidance && root.calibrationActive
        onTriggered: {
            const vehicle = root.vehicle
            if (!vehicle || vehicle.vehicleLinkManager.communicationLost
                    || root.attitudeUpdateTime <= 0 || Date.now() - root.attitudeUpdateTime > 1000) {
                CompassRotation.interrupt(root.rotationTracker)
                root.activeRotationSide = -1
                return
            }
            if (root.attitudeUpdateTime <= root.rotationTracker.timestamp) { return }
            CompassRotation.sample(root.rotationTracker, Number(vehicle.roll.rawValue),
                Number(vehicle.pitch.rawValue), Number(vehicle.heading.rawValue), root.attitudeUpdateTime)
            root.rotationProgress = root.rotationTracker.progress.slice()
            const side = root.rotationTracker.side
            if (side >= 0 && side !== root.activeRotationSide) { root.selectedSide = side }
            root.activeRotationSide = side
        }
    }
    property string indicatorPrefix: "sensorsCal_"
    readonly property var sides: [
        {key: "downSide", label: qsTr("水平"), image: "VehicleDown", pose: "01"},
        {key: "leftSide", label: qsTr("左侧"), image: "VehicleLeft", pose: "05"},
        {key: "rightSide", label: qsTr("右侧"), image: "VehicleRight", pose: "06"},
        {key: "noseDownSide", label: qsTr("机头朝下"), image: "VehicleNoseDown", pose: "03"},
        {key: "tailDownSide", label: qsTr("机头朝上"), image: "VehicleTailDown", pose: "04"},
        {key: "upsideDownSide", label: qsTr("倒置"), image: "VehicleUpsideDown", pose: "02"}
    ]
    readonly property real spacing: ScreenTools.defaultFontPixelWidth * 0.6

    function imageSource(index) {
        if (compassMode) {
            return "qrc:/CAAC/Calibration/pose-" + sides[index].pose + "-rotation.gif"
        }
        if (QGroundControl.corePlugin.directMissionItemEditing) {
            return "qrc:/CAAC/Calibration/pose-" + sides[index].pose + ".png"
        }
        return "qrc:///qmlimages/" + sides[index].image + (sideRotating[index] ? "Rotate" : "") + ".png"
    }

    function statusText(index) {
        if (compassMode && motionGuidance) {
            return rotationProgress[index] >= 1 ? qsTr("转动已完成")
                : calibrationActive && activeRotationSide === index ? qsTr("请按图转动") : qsTr("待转动")
        }
        switch (sideStates[index]) {
        case VehicleRotationCal.CalState.Incomplete: return qsTr("未完成")
        case VehicleRotationCal.CalState.InProgress: return sideRotating[index] ? qsTr("旋转") : qsTr("保持静止")
        case VehicleRotationCal.CalState.Completed: return qsTr("已完成")
        default: return ""
        }
    }

    function selectActiveSide() {
        const active = sideStates.indexOf(VehicleRotationCal.CalState.InProgress)
        if (active >= 0) { selectedSide = active }
    }

    onSideStatesChanged: selectActiveSide()

    QGCPalette { id: calPalette }

    ColumnLayout {
        anchors.fill: parent
        spacing: root.spacing

        Rectangle {
            Layout.fillWidth: true
            Layout.fillHeight: true
            visible: root.showImages && !root.compassMode
            color: "black"
            radius: ScreenTools.defaultBorderRadius

            Image {
                objectName: root.indicatorPrefix === "sensorsCal_" ? "sensorCalibrationLargePreview" : root.indicatorPrefix + "largePreview"
                anchors.fill: parent
                anchors.margins: root.spacing
                source: root.showImages && !root.compassMode ? root.imageSource(root.selectedSide) : ""
                fillMode: Image.PreserveAspectFit
                smooth: true
            }
        }

        GridLayout {
            id: poseGrid
            Layout.fillWidth: true
            Layout.fillHeight: false
            columns: root.compassMode
                ? Math.max(1, Math.min(3, Math.floor((root.width + root.spacing)
                    / (Math.max(140, ScreenTools.defaultFontPixelWidth * 10) + root.spacing))))
                : root.width < ScreenTools.defaultFontPixelWidth * 24 ? 2
                : root.width < ScreenTools.defaultFontPixelWidth * 60 ? 3 : 6
            columnSpacing: root.spacing
            rowSpacing: root.spacing

            Repeater {
                model: root.sides

                Rectangle {
                    id: card
                    required property int index
                    required property var modelData
                    readonly property real faceProgress: root.faceProgress(index)
                    readonly property int calState: root.compassMode && root.motionGuidance
                        ? faceProgress >= 1 ? VehicleRotationCal.CalState.Completed
                            : root.calibrationActive && root.activeRotationSide === index ? VehicleRotationCal.CalState.InProgress
                            : VehicleRotationCal.CalState.Incomplete
                        : root.sideStates[index]
                    readonly property string calInProgressText: root.sideRotating[index] ? qsTr("旋转") : qsTr("保持静止")
                    readonly property string imageSource: root.imageSource(index)
                    objectName: root.indicatorPrefix + modelData.key
                    visible: root.sideVisible[index]
                    Layout.fillWidth: true
                    Layout.minimumWidth: 0
                    Layout.preferredWidth: 1
                    Layout.preferredHeight: root.compassMode ? cardContent.implicitHeight + root.spacing
                        : root.showImages ? ScreenTools.defaultFontPixelHeight * 6 : cardContent.implicitHeight + root.spacing
                    color: calPalette.windowShade
                    radius: ScreenTools.defaultBorderRadius
                    border.width: root.selectedSide === index ? 2 : 1
                    border.color: calState === VehicleRotationCal.CalState.Completed ? calPalette.colorGreen
                        : calState === VehicleRotationCal.CalState.InProgress ? calPalette.colorBlue
                        : root.selectedSide === index ? calPalette.text : calPalette.buttonBorder

                    ColumnLayout {
                        id: cardContent
                        anchors.fill: parent
                        anchors.margins: root.spacing * 0.5
                        spacing: root.spacing * 0.5

                        Rectangle {
                            Layout.fillWidth: true
                            Layout.fillHeight: true
                            Layout.preferredHeight: root.compassMode ? width * 520 / 600 : -1
                            visible: root.showImages
                            color: "black"
                            radius: ScreenTools.defaultBorderRadius
                            AnimatedImage {
                                paused: !root.compassMode || !root.visible || !card.visible
                                    || card.calState === VehicleRotationCal.CalState.Completed
                                cache: false
                                objectName: "sensorCalibrationPoseImage_" + card.index
                                anchors.fill: parent
                                source: root.showImages ? card.imageSource : ""
                                fillMode: Image.PreserveAspectFit
                                smooth: true
                            }
                        }
                        QGCLabel {
                            Layout.fillWidth: true
                            text: card.modelData.label
                            horizontalAlignment: Text.AlignHCenter
                            font.pointSize: ScreenTools.smallFontPointSize
                            wrapMode: Text.Wrap
                        }
                        ProgressBar {
                            objectName: "sensorCalibrationFaceProgress_" + card.index
                            Layout.fillWidth: true
                            visible: root.compassMode
                            value: card.faceProgress
                            palette.dark: calPalette.mapWidgetBorderLight
                            palette.midlight: calPalette.windowShadeDark
                        }
                        QGCLabel {
                            Layout.fillWidth: true
                            visible: root.compassMode
                            text: (root.motionGuidance ? qsTr("转动进度 %1%") : qsTr("校准进度 %1%"))
                                .arg(Math.floor(card.faceProgress * 100))
                            horizontalAlignment: Text.AlignHCenter
                            font.pointSize: ScreenTools.smallFontPointSize
                        }
                        QGCLabel {
                            Layout.fillWidth: true
                            text: root.statusText(card.index)
                            visible: text !== ""
                            horizontalAlignment: Text.AlignHCenter
                            font.pointSize: ScreenTools.smallFontPointSize
                            wrapMode: Text.Wrap
                        }
                    }

                    MouseArea {
                        anchors.fill: parent
                        onClicked: root.selectedSide = card.index
                    }
                }
            }
        }
    }
}
