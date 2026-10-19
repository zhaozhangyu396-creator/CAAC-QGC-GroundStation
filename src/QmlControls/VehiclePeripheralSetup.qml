pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import QGroundControl
import QGroundControl.Controls

Item {
    id: root
    objectName: "vehiclePeripheralSetup"
    required property string page
    property var vehicle: QGroundControl.multiVehicleManager.activeVehicle
    readonly property string pageName: page === "gps" ? qsTr("GPS 设置")
        : page === "radar" ? qsTr("雷达设置") : qsTr("飞控微调")
    readonly property bool ready: !!vehicle && vehicle.parameterManager.parametersReady
        && !vehicle.vehicleLinkManager.communicationLost
    readonly property var controller: controllerLoader.item
    property int receiver: 1
    property real trimStep: 0.1
    property double now: Date.now()
    readonly property var downward: vehicle ? vehicle.distanceSensors.getFact("rotationPitch270") : null
    readonly property bool distanceFresh: ready && vehicle.distanceSensors.downwardUpdateTime > 0
        && now - vehicle.distanceSensors.downwardUpdateTime < 3000
    readonly property var yawSource: parameter(["EK3_SRC1_YAW"])
    readonly property var yawOffset: parameter(["EKF2_GPS_YAW_OFF"])
    readonly property var rangeType: parameter(["RNGFND1_TYPE"])
    readonly property var protocolChoices: [
        {label: qsTr("禁用"), value: 0}, {label: qsTr("自动识别"), value: 1},
        {label: "u-blox", value: 2}, {label: "NMEA", value: 5}, {label: "DroneCAN", value: 9},
        {label: "MAVLink", value: 14}, {label: qsTr("u-blox 移动基站"), value: 17},
        {label: qsTr("u-blox 移动站"), value: 18}, {label: qsTr("DroneCAN 移动基站"), value: 22},
        {label: qsTr("DroneCAN 移动站"), value: 23}, {label: "Unicore NMEA", value: 24},
        {label: qsTr("Unicore 双天线"), value: 25}, {label: qsTr("Septentrio 双天线"), value: 26}
    ]

    function parameter(names) {
        if (!ready || !controller || controller.vehicle !== vehicle) { return null }
        for (const name of names) {
            if (controller.parameterExists(-1, name)) { return controller.getParameterFact(-1, name, false) }
        }
        return null
    }
    function supported(definitions) {
        return definitions.filter(definition => !!parameter(definition.names))
    }
    function resetController() {
        controllerLoader.sourceComponent = undefined
        if (ready) { controllerLoader.sourceComponent = controllerComponent }
    }
    onVehicleChanged: resetController()
    onReadyChanged: resetController()
    Component.onCompleted: resetController()
    Component { id: controllerComponent; FactPanelController { } }
    Loader { id: controllerLoader }
    QGCPalette { id: palette }
    Timer { interval: 500; repeat: true; running: root.visible; onTriggered: root.now = Date.now() }

    QGCFlickable {
        id: scroller
        objectName: "vehiclePeripheralScroller"
        anchors.fill: parent
        clip: true
        contentWidth: width
        contentHeight: content.implicitHeight
        Rectangle {
            objectName: "vehiclePeripheralContent"
            width: parent.width
            height: content.implicitHeight
            color: palette.window
            ColumnLayout {
                id: content
                width: parent.width
                spacing: ScreenTools.defaultFontPixelHeight
                QGCLabel {
                    text: root.pageName
                    font.pointSize: ScreenTools.mediumFontPointSize
                    font.bold: true
                    Layout.fillWidth: true
                }
                QGCLabel {
                    Layout.fillWidth: true
                    wrapMode: Text.WordWrap
                    text: !root.ready ? qsTr("请连接飞控并等待参数读取完成。不支持的项目不可用。")
                        : root.vehicle.armed || root.vehicle.flying ? qsTr("请先停止飞行并上锁，再修改设置。")
                        : qsTr("编辑后点击确认保存。需要重启的参数保存后会提示重启飞控。")
                    font.pointSize: ScreenTools.smallFontPointSize
                }

                ColumnLayout {
                    visible: root.page === "gps"
                    Layout.fillWidth: true
                    spacing: ScreenTools.defaultFontPixelHeight
                    Rectangle {
                        objectName: "gpsHeadingCard"
                        Layout.fillWidth: true
                        implicitHeight: headingSettings.implicitHeight + ScreenTools.defaultFontPixelHeight
                        color: palette.windowShade
                        radius: ScreenTools.defaultBorderRadius
                        PeripheralParameterSection {
                            id: headingSettings
                            objectName: "gpsHeadingSettings"
                            anchors.centerIn: parent
                            width: parent.width - ScreenTools.defaultFontPixelWidth * 2
                            vehicle: root.vehicle
                            controller: root.controller
                            title: qsTr("双天线定向")
                            description: qsTr("需要支持双天线测向的接收机，并正确配置接收机类型、天线安装位置和安装补偿。勾选后点击确认保存。")
                                + (root.yawSource ? qsTr("APM 启用时选择 GPS 航向，关闭时切回罗盘；也可在下方选择罗盘备用等航向来源。") : "")
                            definitions: root.yawSource
                                ? [{key: "yawSource", label: qsTr("航向来源"), names: ["EK3_SRC1_YAW"],
                                    toggleText: qsTr("启用双天线定向"), enabledValues: [2, 3], enabledValue: 2, disabledValue: 1,
                                    options: [
                                        {label: qsTr("无"), value: 0}, {label: qsTr("罗盘"), value: 1}, {label: qsTr("GPS"), value: 2},
                                        {label: qsTr("GPS，罗盘备用"), value: 3}, {label: qsTr("外部导航"), value: 6}, {label: "GSF", value: 8}]}]
                                : [{key: "yawFusion", label: qsTr("GPS 航向融合"), names: ["EKF2_GPS_CTRL"], bit: 3,
                                    toggleText: qsTr("启用双天线定向")}]
                        }
                    }
                    QGCButton {
                        objectName: "gpsInstallationPosition"
                        text: qsTr("设置 GPS 安装位置")
                        Layout.fillWidth: true
                        onClicked: scroller.contentY = Math.min(offsets.mapToItem(content, Qt.point(0, 0)).y,
                            Math.max(0, scroller.contentHeight - scroller.height))
                    }
                    QGCButton {
                        objectName: "gpsHeadingInstallation"
                        text: qsTr("设置双天线安装补偿")
                        Layout.fillWidth: true
                        onClicked: scroller.contentY = Math.min(compensation.mapToItem(content, Qt.point(0, 0)).y,
                            Math.max(0, scroller.contentHeight - scroller.height))
                    }
                    DualAntennaSummary { vehicle: root.vehicle; compact: true }
                    RowLayout {
                        Layout.fillWidth: true
                        QGCButton {
                            objectName: "gpsReceiver_1"
                            text: qsTr("GPS 1")
                            checked: root.receiver === 1
                            Layout.fillWidth: true
                            onClicked: root.receiver = 1
                        }
                        QGCButton {
                            objectName: "gpsReceiver_2"
                            text: qsTr("GPS 2")
                            checked: root.receiver === 2
                            Layout.fillWidth: true
                            onClicked: root.receiver = 2
                        }
                    }
                    PeripheralParameterSection {
                        objectName: "gpsReceiverSettings"
                        vehicle: root.vehicle
                        controller: root.controller
                        title: qsTr("GPS %1 接收机").arg(root.receiver)
                        description: qsTr("选择实际连接的接收机类型或串口。CAN 接收机也需在飞控中启用对应的 CAN 驱动。")
                        definitions: {
                            const apmNames = root.receiver === 1 ? ["GPS1_TYPE", "GPS_TYPE"] : ["GPS2_TYPE", "GPS_TYPE2"]
                            return [{key: "gpsType", label: qsTr("接收机类型"), names: apmNames, options: root.protocolChoices},
                                {key: "gpsPort", label: qsTr("GPS 串口"), names: ["GPS_" + root.receiver + "_CONFIG"]}]
                        }
                    }
                    PeripheralParameterSection {
                        id: offsets
                        objectName: "gpsAntennaOffsets"
                        vehicle: root.vehicle
                        controller: root.controller
                        title: qsTr("GPS %1 安装位置（天线偏移）").arg(root.receiver)
                        description: root.parameter(["EKF2_GPS_POS_X"])
                            ? qsTr("相对于机体重心：X 向前、Y 向右、Z 向下为正，单位为厘米（cm）。")
                            : qsTr("相对于共同机体原点，通常选机体重心；请与 IMU 安装位置使用同一基准。X 向前、Y 向右、Z 向下为正，单位为厘米（cm）。")
                        preview: GpsAntennaPositionPreview {
                            receiver: root.receiver
                            positionX: offsets.displayValues.gpsX ?? null
                            positionY: offsets.displayValues.gpsY ?? null
                            positionZ: offsets.displayValues.gpsZ ?? null
                        }
                        definitions: [
                            {key: "gpsX", label: qsTr("前后偏移 X"), units: "cm", valueScale: 100, step: 1, names: root.receiver === 1
                                ? ["GPS1_POS_X", "GPS_POS1_X", "EKF2_GPS_POS_X"] : ["GPS2_POS_X", "GPS_POS2_X", "EKF2_GPS2_POS_X"]},
                            {key: "gpsY", label: qsTr("左右偏移 Y"), units: "cm", valueScale: 100, step: 1, names: root.receiver === 1
                                ? ["GPS1_POS_Y", "GPS_POS1_Y", "EKF2_GPS_POS_Y"] : ["GPS2_POS_Y", "GPS_POS2_Y", "EKF2_GPS2_POS_Y"]},
                            {key: "gpsZ", label: qsTr("上下偏移 Z"), units: "cm", valueScale: 100, step: 1, names: root.receiver === 1
                                ? ["GPS1_POS_Z", "GPS_POS1_Z", "EKF2_GPS_POS_Z"] : ["GPS2_POS_Z", "GPS_POS2_Z", "EKF2_GPS2_POS_Z"]}
                        ]
                    }
                    Rectangle {
                        id: compensation
                        objectName: "gpsInstallationCompensation"
                        Layout.fillWidth: true
                        implicitHeight: compensationContent.implicitHeight + ScreenTools.defaultFontPixelHeight
                        color: palette.windowShade
                        radius: ScreenTools.defaultBorderRadius
                        ColumnLayout {
                            id: compensationContent
                            anchors.centerIn: parent
                            width: parent.width - ScreenTools.defaultFontPixelWidth * 2
                            spacing: ScreenTools.defaultFontPixelHeight * 0.5
                            PeripheralParameterSection {
                                id: headingOffsetSettings
                                objectName: "gpsHeadingOffsetSettings"
                                visible: !!root.yawOffset || !root.ready
                                vehicle: root.vehicle
                                controller: root.controller
                                title: qsTr("双天线安装航向补偿")
                                description: qsTr("补偿双天线测向基准方向与机头方向之间的固定夹角，范围 0～360°。0° 表示不加补偿；具体数值按接收机天线方向及飞控约定确定。")
                                preview: GpsHeadingCompensationPreview {
                                    angle: headingOffsetSettings.displayValues.yawOffset ?? null
                                }
                                definitions: [{key: "yawOffset", label: qsTr("安装补偿值"), names: ["EKF2_GPS_YAW_OFF"],
                                    units: "°", step: 1, ranges: {EKF2_GPS_YAW_OFF: [0, 360]}}]
                            }
                            PeripheralParameterSection {
                                objectName: "gpsMovingBaseSettings"
                                visible: !!root.parameter(["GPS1_MB_TYPE", "GPS_MB1_TYPE"])
                                vehicle: root.vehicle
                                controller: root.controller
                                title: qsTr("双天线安装方向补偿")
                                description: qsTr("飞控根据天线基线确定安装方向，可由两路 GPS 安装位置计算。使用自定义基站时，填写主天线相对于第二天线的位置，单位 cm。X 向前、Y 向右、Z 向下为正。")
                                definitions: [
                                    {key: "baseType", label: qsTr("基站位置来源"), names: ["GPS1_MB_TYPE", "GPS_MB1_TYPE"], options: [
                                        {label: qsTr("由两路 GPS 天线偏移计算"), value: 0}, {label: qsTr("自定义相对偏移"), value: 1}]},
                                    {key: "baseX", label: qsTr("相对前后偏移 X"), names: ["GPS1_MB_OFS_X", "GPS_MB1_OFS_X"], units: "cm", valueScale: 100, step: 1},
                                    {key: "baseY", label: qsTr("相对左右偏移 Y"), names: ["GPS1_MB_OFS_Y", "GPS_MB1_OFS_Y"], units: "cm", valueScale: 100, step: 1},
                                    {key: "baseZ", label: qsTr("相对上下偏移 Z"), names: ["GPS1_MB_OFS_Z", "GPS_MB1_OFS_Z"], units: "cm", valueScale: 100, step: 1}
                                ]
                            }
                            QGCLabel {
                                Layout.fillWidth: true
                                visible: root.ready && !root.yawOffset && !root.parameter(["GPS1_MB_TYPE", "GPS_MB1_TYPE"])
                                wrapMode: Text.WordWrap
                                text: qsTr("当前飞控未提供独立的安装航向补偿参数。请设置两根天线相对于机体原点的位置，由飞控根据基线确定安装方向。")
                            }
                        }
                    }
                }

                ColumnLayout {
                    visible: root.page === "radar"
                    Layout.fillWidth: true
                    spacing: ScreenTools.defaultFontPixelHeight
                    QGCLabel {
                        Layout.fillWidth: true
                        wrapMode: Text.WordWrap
                        text: qsTr("将测距传感器朝下安装并避免遮挡。接口与协议请按传感器型号和飞控接线配置。")
                    }
                    Rectangle {
                        Layout.fillWidth: true
                        implicitHeight: distanceContent.implicitHeight + ScreenTools.defaultFontPixelHeight * 2
                        color: palette.windowShade
                        radius: ScreenTools.defaultBorderRadius
                        ColumnLayout {
                            id: distanceContent
                            anchors.centerIn: parent
                            width: parent.width - ScreenTools.defaultFontPixelWidth * 2
                            QGCLabel {
                                objectName: "radarLiveDistance"
                                Layout.fillWidth: true
                                horizontalAlignment: Text.AlignHCenter
                                font.pointSize: ScreenTools.largeFontPointSize
                                text: root.distanceFresh && root.downward && Number.isFinite(Number(root.downward.rawValue))
                                    ? Number(root.downward.rawValue).toFixed(2) + " m" : qsTr("未提供")
                            }
                            QGCLabel { text: qsTr("底部实时测距"); Layout.alignment: Qt.AlignHCenter }
                        }
                    }
                    PeripheralParameterSection {
                        objectName: "radarHardwareSettings"
                        vehicle: root.vehicle
                        controller: root.controller
                        title: qsTr("测距传感器设置")
                        definitions: root.rangeType ? [
                            {key: "rangeType", label: qsTr("传感器类型（0 为禁用）"), names: ["RNGFND1_TYPE"], step: 1},
                            {key: "rangeOrientation", label: qsTr("安装方向"), names: ["RNGFND1_ORIENT"], options: [
                                {label: qsTr("向前"), value: 0}, {label: qsTr("向右"), value: 2}, {label: qsTr("向后"), value: 4},
                                {label: qsTr("向左"), value: 6}, {label: qsTr("向上"), value: 24}, {label: qsTr("向下（底部测距）"), value: 25}]},
                            {key: "rangeMin", label: qsTr("最小有效距离"), names: ["RNGFND1_MIN", "RNGFND1_MIN_CM"],
                                units: "m", scales: {RNGFND1_MIN_CM: 0.01}},
                            {key: "rangeMax", label: qsTr("最大有效距离"), names: ["RNGFND1_MAX", "RNGFND1_MAX_CM"],
                                units: "m", scales: {RNGFND1_MAX_CM: 0.01}}
                        ] : root.supported([
                            {key: "tfminiPort", label: qsTr("TFmini / 雷达串口"), names: ["SENS_TFMINI_CFG"]},
                            {key: "tfminiType", label: qsTr("TFmini / 雷达型号"), names: ["SENS_TFMINI_HW"]},
                            {key: "tf02Enable", label: qsTr("TF02 Pro 驱动"), names: ["SENS_EN_TF02PRO"]},
                            {key: "sfPort", label: qsTr("Lightware 串口"), names: ["SENS_SF0X_CFG"]},
                            {key: "sfType", label: qsTr("Lightware 型号"), names: ["SENS_EN_SF0X"]},
                            {key: "sfI2c", label: qsTr("Lightware I²C 驱动"), names: ["SENS_EN_SF1XX"]},
                            {key: "rangeFusion", label: qsTr("底部测距融合"), names: ["EKF2_RNG_CTRL"], options: [
                                {label: qsTr("禁用"), value: 0}, {label: qsTr("按条件启用"), value: 1}, {label: qsTr("启用"), value: 2}]}
                        ])
                        description: root.rangeType ? qsTr("选择实际传感器型号及向下安装方向后确认保存。")
                            : qsTr("仅显示飞控支持的驱动。测距融合用于估计器，传感器驱动需单独配置；不能用融合开关代替硬件驱动。")
                    }
                }

                ColumnLayout {
                    visible: root.page === "trim"
                    Layout.fillWidth: true
                    spacing: ScreenTools.defaultFontPixelHeight
                    QGCLabel {
                        Layout.fillWidth: true
                        wrapMode: Text.WordWrap
                        text: qsTr("用于减小无人机在无风、姿态模式下的持续飘移。记录飘移方向，降落并上锁后，按对应方向微调并确认保存，再试飞核对效果。")
                    }
                    VehicleSummaryRow {
                        labelText: qsTr("当前横滚角")
                        valueText: root.ready ? Number(root.vehicle.roll.rawValue).toFixed(2) + " °" : qsTr("未提供")
                    }
                    VehicleSummaryRow {
                        labelText: qsTr("当前俯仰角")
                        valueText: root.ready ? Number(root.vehicle.pitch.rawValue).toFixed(2) + " °" : qsTr("未提供")
                    }
                    RowLayout {
                        Layout.fillWidth: true
                        QGCLabel { text: qsTr("每次修正角度"); Layout.fillWidth: true }
                        QGCComboBox {
                            objectName: "trimStepSelector"
                            model: ["0.05°", "0.10°", "0.20°"]
                            currentIndex: 1
                            onActivated: index => root.trimStep = [0.05, 0.1, 0.2][index]
                        }
                    }
                    QGCLabel {
                        text: qsTr("选择实际飘移方向（以机头方向为前），系统施加反向修正。")
                        Layout.fillWidth: true
                        wrapMode: Text.WordWrap
                    }
                    GridLayout {
                        Layout.fillWidth: true
                        columns: 3
                        enabled: trimSettings.writable && !trimSettings.busy
                        Item { Layout.fillWidth: true }
                        QGCButton {
                            objectName: "trimDriftForward"
                            Layout.fillWidth: true
                            text: qsTr("向前飘移")
                            enabled: !!root.parameter(["AHRS_TRIM_Y", "SENS_BOARD_Y_OFF"])
                            onClicked: trimSettings.stepFor("trimPitch", root.trimStep)
                        }
                        Item { Layout.fillWidth: true }
                        QGCButton {
                            objectName: "trimDriftLeft"
                            Layout.fillWidth: true
                            text: qsTr("向左飘移")
                            enabled: !!root.parameter(["AHRS_TRIM_X", "SENS_BOARD_X_OFF"])
                            onClicked: trimSettings.stepFor("trimRoll", root.trimStep)
                        }
                        QGCLabel {
                            Layout.fillWidth: true
                            text: qsTr("↑ 机头")
                            horizontalAlignment: Text.AlignHCenter
                        }
                        QGCButton {
                            objectName: "trimDriftRight"
                            Layout.fillWidth: true
                            text: qsTr("向右飘移")
                            enabled: !!root.parameter(["AHRS_TRIM_X", "SENS_BOARD_X_OFF"])
                            onClicked: trimSettings.stepFor("trimRoll", -root.trimStep)
                        }
                        Item { Layout.fillWidth: true }
                        QGCButton {
                            objectName: "trimDriftBackward"
                            Layout.fillWidth: true
                            text: qsTr("向后飘移")
                            enabled: !!root.parameter(["AHRS_TRIM_Y", "SENS_BOARD_Y_OFF"])
                            onClicked: trimSettings.stepFor("trimPitch", -root.trimStep)
                        }
                        Item { Layout.fillWidth: true }
                    }
                    PeripheralParameterSection {
                        id: trimSettings
                        objectName: "flightControllerTrim"
                        vehicle: root.vehicle
                        controller: root.controller
                        title: qsTr("飘移修正量")
                        description: qsTr("方向按钮只调整下方待保存数值，确认后才写入飞控。也可手动修改修正角度。")
                        definitions: [
                            {key: "trimRoll", label: qsTr("横滚零点"), names: ["AHRS_TRIM_X", "SENS_BOARD_X_OFF"],
                                units: "°", step: 0.1, scales: {AHRS_TRIM_X: 180 / Math.PI},
                                ranges: {AHRS_TRIM_X: [-0.1745, 0.1745], SENS_BOARD_X_OFF: [-45, 45]}},
                            {key: "trimPitch", label: qsTr("俯仰零点"), names: ["AHRS_TRIM_Y", "SENS_BOARD_Y_OFF"],
                                units: "°", step: 0.1, scales: {AHRS_TRIM_Y: 180 / Math.PI},
                                ranges: {AHRS_TRIM_Y: [-0.1745, 0.1745], SENS_BOARD_Y_OFF: [-45, 45]}}
                        ]
                    }
                }
            }
        }
    }
}
