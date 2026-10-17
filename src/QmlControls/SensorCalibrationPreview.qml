pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Layouts

import QGroundControl
import QGroundControl.Controls

Item {
    id: root
    implicitHeight: Math.max(ScreenTools.defaultFontPixelHeight * 8, width * 0.55)
        + poseGrid.implicitHeight + spacing

    required property var sideStates
    required property var sideVisible
    required property var sideRotating
    property int selectedSide: 0
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
        if (QGroundControl.corePlugin.directMissionItemEditing) {
            return "qrc:/CAAC/Calibration/pose-" + sides[index].pose + ".png"
        }
        return "qrc:///qmlimages/" + sides[index].image + (sideRotating[index] ? "Rotate" : "") + ".png"
    }

    function statusText(index) {
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
            color: "black"
            radius: ScreenTools.defaultBorderRadius

            Image {
                objectName: root.indicatorPrefix === "sensorsCal_" ? "sensorCalibrationLargePreview" : root.indicatorPrefix + "largePreview"
                anchors.fill: parent
                anchors.margins: root.spacing
                source: root.imageSource(root.selectedSide)
                fillMode: Image.PreserveAspectFit
                smooth: true
            }
        }

        GridLayout {
            id: poseGrid
            Layout.fillWidth: true
            Layout.fillHeight: false
            columns: root.width < ScreenTools.defaultFontPixelWidth * 24 ? 2
                : root.width < ScreenTools.defaultFontPixelWidth * 60 ? 3 : 6
            columnSpacing: root.spacing
            rowSpacing: root.spacing

            Repeater {
                model: root.sides

                Rectangle {
                    id: card
                    required property int index
                    required property var modelData
                    readonly property int calState: root.sideStates[index]
                    readonly property string calInProgressText: root.sideRotating[index] ? qsTr("旋转") : qsTr("保持静止")
                    readonly property string imageSource: root.imageSource(index)
                    objectName: root.indicatorPrefix + modelData.key
                    visible: root.sideVisible[index]
                    Layout.fillWidth: true
                    Layout.minimumWidth: 0
                    Layout.preferredWidth: 1
                    Layout.preferredHeight: ScreenTools.defaultFontPixelHeight * 6
                    color: calPalette.windowShade
                    radius: ScreenTools.defaultBorderRadius
                    border.width: root.selectedSide === index ? 2 : 1
                    border.color: calState === VehicleRotationCal.CalState.Completed ? calPalette.colorGreen
                        : calState === VehicleRotationCal.CalState.InProgress ? calPalette.colorBlue
                        : root.selectedSide === index ? calPalette.text : calPalette.buttonBorder

                    ColumnLayout {
                        anchors.fill: parent
                        anchors.margins: root.spacing * 0.5
                        spacing: root.spacing * 0.5

                        Rectangle {
                            Layout.fillWidth: true
                            Layout.fillHeight: true
                            color: "black"
                            radius: ScreenTools.defaultBorderRadius
                            Image {
                                anchors.fill: parent
                                source: card.imageSource
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
