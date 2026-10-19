pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Layouts

import QGroundControl
import QGroundControl.Controls

ColumnLayout {
    id: root
    objectName: "dualAntennaSummary"
    property var vehicle: QGroundControl.multiVehicleManager.activeVehicle
    property bool compact: false
    readonly property bool connected: !!vehicle && !vehicle.vehicleLinkManager.communicationLost
    readonly property bool ready: connected && vehicle.parameterManager.parametersReady
    readonly property var controller: controllerLoader.item
    readonly property var yawSource: parameter(["EK3_SRC1_YAW", "EKF2_GPS_CTRL"])
    readonly property var movingBaseType: parameter(["GPS1_MB_TYPE", "GPS_MB1_TYPE"])
    readonly property var yawOffset: parameter(["EKF2_GPS_YAW_OFF"])
    property double now: Date.now()
    property double primaryReport: 0
    property double secondaryReport: 0
    readonly property bool primaryFresh: connected && primaryReport > 0 && now - primaryReport < 5000
    readonly property bool secondaryFresh: connected && secondaryReport > 0 && now - secondaryReport < 5000
    readonly property int yawReceiver: primaryFresh && Number.isFinite(Number(vehicle.gps.yaw.rawValue)) ? 1
        : secondaryFresh && Number.isFinite(Number(vehicle.gps2.yaw.rawValue)) ? 2 : 0
    readonly property real measuredYaw: yawReceiver === 1 ? Number(vehicle.gps.yaw.rawValue)
        : yawReceiver === 2 ? Number(vehicle.gps2.yaw.rawValue) : NaN
    readonly property string fusionText: {
        if (!yawSource) { return qsTr("不支持") }
        const value = Number(yawSource.rawValue)
        if (yawSource.name === "EKF2_GPS_CTRL") { return value & 8 ? qsTr("已启用") : qsTr("未启用") }
        if (value === 2) { return qsTr("GPS 航向") }
        if (value === 3) { return qsTr("GPS 航向，罗盘备用") }
        return qsTr("未使用 GPS 航向（%1）").arg(value)
    }
    readonly property real antennaSeparation: {
        const axes = ["X", "Y", "Z"]
        let squared = 0
        for (const axis of axes) {
            let delta
            if (movingBaseType && Number(movingBaseType.rawValue) === 1) {
                const offset = parameter(["GPS1_MB_OFS_" + axis, "GPS_MB1_OFS_" + axis])
                if (!offset) { return NaN }
                delta = Number(offset.rawValue)
            } else {
                const first = parameter(["GPS1_POS_" + axis, "GPS_POS1_" + axis])
                const second = parameter(["GPS2_POS_" + axis, "GPS_POS2_" + axis])
                if (!first || !second) { return NaN }
                delta = Number(second.rawValue) - Number(first.rawValue)
            }
            squared += delta * delta
        }
        return Math.sqrt(squared)
    }
    Layout.fillWidth: true
    spacing: ScreenTools.defaultFontPixelHeight * 0.25

    function parameter(names) {
        if (!ready || !controller || controller.vehicle !== vehicle) { return null }
        for (const name of names) {
            if (controller.parameterExists(-1, name)) { return controller.getParameterFact(-1, name, false) }
        }
        return null
    }
    function number(value, units, decimals) {
        return Number.isFinite(Number(value)) ? Number(value).toFixed(decimals) + " " + units : qsTr("未提供")
    }
    function heading(value) {
        return number((Math.round(Number(value) * 10) / 10) % 360, "°", 1)
    }
    function reset() {
        primaryReport = 0
        secondaryReport = 0
        controllerLoader.sourceComponent = undefined
        if (ready) { controllerLoader.sourceComponent = controllerComponent }
    }
    onVehicleChanged: reset()
    onReadyChanged: reset()
    Component.onCompleted: reset()
    Component { id: controllerComponent; FactPanelController { } }
    Loader { id: controllerLoader }
    CompassHeadingController { id: compass; vehicle: root.vehicle }
    Timer { interval: 500; running: root.visible; repeat: true; onTriggered: root.now = Date.now() }
    Connections {
        target: root.vehicle ? root.vehicle.gps : null
        function onPositionReported() { root.primaryReport = Date.now(); root.now = Date.now() }
    }
    Connections {
        target: root.vehicle ? root.vehicle.gps2 : null
        function onPositionReported() { root.secondaryReport = Date.now(); root.now = Date.now() }
    }

    QGCLabel { text: root.compact ? qsTr("GPS 状态") : qsTr("双天线定向"); font.bold: true; Layout.fillWidth: true }
    VehicleSummaryRow {
        objectName: "dualAntenna_fusion"
        visible: !root.compact
        labelText: qsTr("航向融合策略")
        valueText: root.fusionText
    }
    VehicleSummaryRow {
        objectName: "dualAntenna_compassHeading"
        visible: !root.compact
        labelText: qsTr("罗盘航向读数")
        valueText: root.connected ? root.heading(compass.heading.rawValue) : qsTr("未提供")
    }
    VehicleSummaryRow {
        objectName: "dualAntenna_measuredYaw"
        labelText: qsTr("双天线测得航向")
        valueText: root.heading(root.measuredYaw)
    }
    VehicleSummaryRow {
        objectName: "dualAntenna_satellites"
        labelText: qsTr("GPS 1 / 2 卫星数")
        valueText: (root.primaryFresh ? root.vehicle.gps.count.valueString : "—") + " / "
            + (root.secondaryFresh ? root.vehicle.gps2.count.valueString : "—")
    }
    VehicleSummaryRow {
        objectName: "dualAntenna_fix"
        labelText: qsTr("GPS 1 / 2 定位状态")
        valueText: root.fixText(1) + " / " + root.fixText(2)
    }
    VehicleSummaryRow {
        objectName: "dualAntenna_separation"
        visible: !root.compact
        labelText: qsTr("配置天线间距")
        valueText: root.number(root.antennaSeparation, "m", 3)
    }
    VehicleSummaryRow {
        visible: !root.compact && !!root.yawOffset
        labelText: qsTr("双天线航向偏移")
        valueText: root.yawOffset ? root.number(root.yawOffset.rawValue, "°", 1) : qsTr("未提供")
    }
    QGCLabel {
        Layout.fillWidth: true
        visible: !root.compact
        font.pointSize: ScreenTools.smallFontPointSize
        wrapMode: Text.WordWrap
        text: qsTr("罗盘航向由主路磁场和姿态数据计算，以磁北为基准。双天线航向来自 GPS 测向数据，优先显示 GPS1，有效数据缺失时使用 GPS2。配置间距不代表实时测量距离。")
        opacity: 0.7
    }

    function fixText(receiver) {
        const fresh = receiver === 1 ? primaryFresh : secondaryFresh
        if (!fresh) { return qsTr("未提供") }
        const gps = receiver === 1 ? vehicle.gps : vehicle.gps2
        const descriptions = [qsTr("无接收机"), qsTr("未定位"), qsTr("二维定位"), qsTr("三维定位"),
            qsTr("差分定位"), qsTr("RTK 浮点"), qsTr("RTK 固定"), qsTr("静态定位"), qsTr("PPP 定位")]
        const value = Number(gps.lock.rawValue)
        return descriptions[value] || qsTr("未知（%1）").arg(value)
    }
}
