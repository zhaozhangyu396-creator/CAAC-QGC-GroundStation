pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import QGroundControl
import QGroundControl.Controls
import QGroundControl.FactControls

Item {
    id: root
    objectName: "caacUOMSettings"
    readonly property var vehicle: QGroundControl.multiVehicleManager.activeVehicle
    readonly property var rid: QGroundControl.settingsManager.remoteIDSettings
    readonly property string serial: vehicle ? vehicle.remoteIDManager.aircraftSerial : ""
    readonly property string flightControllerSN: vehicle && vehicle.vehicleUID > 0 ? vehicle.vehicleUIDStr : ""
    readonly property bool ready: QGroundControl.multiVehicleManager.parameterReadyVehicleAvailable
    property bool registrationNoticeExpanded: false
    QGCPalette { id: palette }
    component UOMTab: TabButton {
        id: tab
        contentItem: QGCLabel { text: tab.text; horizontalAlignment: Text.AlignHCenter; wrapMode: Text.WordWrap }
        background: Rectangle {
            border.color: tab.checked ? palette.text : "transparent"
            border.width: tab.checked ? 1 : 0
            color: palette.windowShadeDark
            Rectangle { anchors.left: parent.left; anchors.right: parent.right; anchors.bottom: parent.bottom; height: 3; color: palette.window; visible: tab.checked }
        }
    }
    function resetController() {
        controllerLoader.sourceComponent = null
        if (vehicle && ready) { controllerLoader.sourceComponent = controllerComponent }
    }
    function parameter(names) {
        const controller = controllerLoader.item
        if (!controller || controller.vehicle !== vehicle || !ready) { return null }
        for (const name of names) {
            if (controller.parameterExists(-1, name)) { return controller.getParameterFact(-1, name, false) }
        }
        return null
    }
    readonly property var modelFact: parameter(["PRODUCT_MODEL", "UAS_MODEL", "DRONE_MODEL"])
    readonly property var factoryFact: parameter(["PRODUCT_SN", "UAS_SERIAL", "DRONE_SN"])
    readonly property string productModel: modelFact ? modelFact.valueString : ""
    readonly property string factoryCode: factoryFact ? factoryFact.valueString : ""
    onVehicleChanged: resetController()
    onReadyChanged: resetController()
    Component.onCompleted: resetController()
    Component { id: controllerComponent; FactPanelController { } }
    Loader { id: controllerLoader }
    function refresh() {
        QGroundControl.corePlugin.refreshAircraftIdentity()
        for (const fact of [modelFact, factoryFact]) {
            if (fact) { QGroundControl.corePlugin.refreshFlightPreferenceParameter(fact.name) }
        }
    }
    function factText(name) {
        const fact = vehicle ? vehicle.getFact(name) : null
        return fact && isFinite(Number(fact.rawValue)) ? fact.valueString + " " + fact.units : "—"
    }
    function telemetryValue(key) {
        if (key === "mode") { return vehicle ? vehicle.flightMode : qsTr("No telemetry") }
        if (key === "longitude") { return vehicle && vehicle.coordinate.isValid ? vehicle.coordinate.longitude.toFixed(7) : "—" }
        if (key === "latitude") { return vehicle && vehicle.coordinate.isValid ? vehicle.coordinate.latitude.toFixed(7) : "—" }
        const names = {heading: "heading", amsl: "altitudeAMSL", relative: "altitudeRelative", speed: "groundSpeed", climb: "climbRate"}
        return factText(names[key])
    }
    component Heading: QGCLabel { Layout.fillWidth: true; font.bold: true; wrapMode: Text.WordWrap }
    component Identifier: SettingsFormRow {
        id: entry
        required property string label
        required property string value
        Layout.fillWidth: true
        ColumnLayout {
            Layout.fillWidth: true
            QGCLabel { text: entry.label; Layout.fillWidth: true; wrapMode: Text.WordWrap }
            RowLayout {
                Layout.fillWidth: true
                QGCTextField { text: entry.value; readOnly: true; placeholderText: qsTr("No data"); Layout.fillWidth: true; Layout.minimumWidth: 0 }
                QGCButton {
                    iconSource: "/InstrumentValueIcons/copy.svg"
                    enabled: entry.value.length > 0
                    onClicked: QGroundControl.copyToClipboard(entry.value)
                }
            }
        }
    }
    ColumnLayout {
        anchors.fill: parent
        TabBar {
            id: tabs
            Layout.fillWidth: true
            UOMTab { objectName: "caacUOMRegistrationTab"; text: qsTr("UOM registration") }
            UOMTab { objectName: "caacUOMSerialTab"; text: qsTr("Aircraft SN") }
            UOMTab { objectName: "caacUOMTelemetryTab"; text: qsTr("UOM dynamic data") }
        }
        QGCFlickable {
            objectName: "caacUOMScroller"
            Layout.fillWidth: true
            Layout.fillHeight: true
            contentWidth: width
            contentHeight: content.implicitHeight
            clip: true
            ColumnLayout {
                id: content
                width: parent.width
                spacing: ScreenTools.defaultFontPixelHeight * 0.6
                ColumnLayout {
                    Layout.fillWidth: true
                    visible: tabs.currentIndex === 0
                    Heading { text: qsTr("Registration and activation") }
                    QGCButton {
                        objectName: "caacUOMRegistrationNoticeToggle"
                        Layout.fillWidth: true
                        text: qsTr("首次使用前进行实名登记和激活") + (root.registrationNoticeExpanded ? "  ▴" : "  ▾")
                        textColor: "#ffb547"
                        wrapMode: Text.WordWrap
                        checked: root.registrationNoticeExpanded
                        onClicked: root.registrationNoticeExpanded = !root.registrationNoticeExpanded
                    }
                    Rectangle {
                        id: registrationNoticePreview
                        objectName: "caacUOMRegistrationNoticePreview"
                        Layout.fillWidth: true
                        implicitHeight: root.registrationNoticeExpanded
                            ? registrationNotice.implicitHeight + ScreenTools.defaultFontPixelHeight * 4
                            : Math.min(registrationNotice.implicitHeight + ScreenTools.defaultFontPixelHeight * 4,
                                ScreenTools.defaultFontPixelHeight * 12)
                        clip: true
                        color: "#292416"
                        border.color: "#997127"
                        radius: ScreenTools.defaultBorderRadius
                        QGCLabel {
                            id: registrationNotice
                            objectName: "caacUOMRegistrationNotice"
                            x: ScreenTools.defaultFontPixelWidth
                            y: ScreenTools.defaultFontPixelHeight
                            width: parent.width - x * 2
                            textFormat: Text.RichText
                            wrapMode: Text.WordWrap
                            color: "#d5d9db"
                            onLinkActivated: (link) => Qt.openUrlExternally(link)
                            text: qsTr("<h3><font color='#ffdf75'>重要提示</font></h3>"
                                + "<p><b><font color='#ffc36b'>首次使用前须进行实名登记和激活。</font></b>请在民用无人机外包装及民用无人机系统显著位置知悉该要求，并认真阅读以下说明。</p>"
                                + "<p><b><font color='#ffc36b'>a）法律法规条款及内容</font></b><br>国家关于民用无人机实名登记的主要规定包括但不限于：<br>"
                                + "1）《无人驾驶航空器飞行管理暂行条例》第二章第十条：<b><font color='#ffc36b'>民用无人驾驶航空器所有者应当依法进行实名登记</font></b>，具体办法由国务院民用航空主管部门会同有关部门制定。<br>"
                                + "2）《民用无人驾驶航空器运行安全管理规则》第92.205条：所有人应当按照规定在<b><font color='#ffc36b'>民用无人驾驶航空器综合管理平台（UOM）</font></b>进行实名登记，<b><font color='#ffc36b'>取得登记标志后方可激活使用</font></b>民用无人驾驶航空器。</p>"
                                + "<p><b><font color='#ffc36b'>b）法律责任与风险</font></b><br>违反实名登记规定可能承担相应法律责任。依据《无人驾驶航空器飞行管理暂行条例》第四十七条：民用无人驾驶航空器<b><font color='#ff8178'>未经实名登记实施飞行活动</font></b>的，由公安机关责令改正，可以处<b><font color='#ff8178'>200元以下</font></b>的罚款；情节严重的，处<b><font color='#ff8178'>2000元以上2万元以下</font></b>的罚款。请务必先完成实名登记并激活后再开展飞行活动。</p>"
                                + "<p><b><font color='#ffc36b'>c）实名登记指导流程</font></b><br>建议按以下步骤完成登记：<br>"
                                + "1）打开 UOM 官网（<a href='https://uom.caac.gov.cn/'>https://uom.caac.gov.cn/</a>）或 UOM App；<br>2）注册账号并登录；<br>3）进入实名登记，按要求填写所有者身份与联系信息、航空器信息及使用用途；<br>4）提交并取得登记标志后，返回本软件查询状态并完成激活。</p>"
                                + "<p><b><font color='#ffc36b'>d）取消激活与所有者变更提示</font></b><br>在完成民用无人机<b><font color='#ffc36b'>取消激活</font></b>操作前，所有者<b><font color='#ff8178'>不应进行转卖、转让、赠与等涉及所有者变更的行为</font></b>。所有权变更、报废、失事或退出使用时，原所有者应及时办理注销登记，新所有者须重新实名登记。<br>"
                                + "<b><font color='#ff8178'>如果原所有者未按规定取消激活，可能面临侵权责任、合同纠纷等民事法律风险。在情节严重的情况下，原所有者还可能因民用无人机被他人用于非法活动而承担相应的刑事责任。</font></b></p>")
                        }
                        Rectangle {
                            anchors.left: parent.left
                            anchors.right: parent.right
                            anchors.bottom: parent.bottom
                            height: ScreenTools.defaultFontPixelHeight * 6
                            visible: !root.registrationNoticeExpanded
                            gradient: Gradient {
                                GradientStop { position: 0; color: "#00292416" }
                                GradientStop { position: 1; color: "#000000" }
                            }
                        }
                        QGCButton {
                            objectName: "caacUOMRegistrationNoticeArrow"
                            anchors.horizontalCenter: parent.horizontalCenter
                            anchors.bottom: parent.bottom
                            text: root.registrationNoticeExpanded ? "⌃" : "⌄"
                            textColor: "#ffb547"
                            Accessible.name: root.registrationNoticeExpanded ? qsTr("收起实名登记提示") : qsTr("展开实名登记提示")
                            background: Item { }
                            heightFactor: 0.2
                            onClicked: root.registrationNoticeExpanded = !root.registrationNoticeExpanded
                        }
                    }
                    Identifier { label: qsTr("Airframe code (unique product identifier)"); value: root.serial }
                    SettingsFormRow {
                        ColumnLayout {
                            Layout.fillWidth: true
                            QGCLabel { text: qsTr("UAS code"); Layout.fillWidth: true }
                            FactTextField { objectName: "caacUOMUASCode"; fact: root.rid.basicID; Layout.fillWidth: true }
                        }
                    }
                    QGCLabel {
                        Layout.fillWidth: true
                        wrapMode: Text.WordWrap
                        text: qsTr("Complete registration and add the aircraft on the official UOM platform, then enter the UAS code from your aircraft record.")
                    }
                    SettingsActionButton {
                        text: qsTr("Open official UOM platform")
                        backgroundColor: palette.window
                        showBorder: true
                        onClicked: Qt.openUrlExternally("https://uom.caac.gov.cn/")
                    }
                    QGCButton { objectName: "caacUOMRefreshStatus"; text: qsTr("Refresh status"); enabled: !!root.vehicle; onClicked: root.refresh() }
                }
                ColumnLayout {
                    Layout.fillWidth: true
                    visible: tabs.currentIndex === 1
                    Heading { text: qsTr("Factory serial numbers") }
                    Identifier { label: qsTr("Product model"); value: root.productModel }
                    Identifier { label: qsTr("Factory code"); value: root.factoryCode }
                    Identifier { label: qsTr("Airframe code (unique product identifier)"); value: root.serial }
                    Identifier { label: qsTr("Flight controller SN"); value: root.flightControllerSN }
                    RowLayout {
                        Layout.fillWidth: true
                        QGCButton {
                            objectName: "caacUOMCopyAll"
                            text: qsTr("Copy all")
                            Layout.fillWidth: true
                            enabled: root.productModel.length + root.factoryCode.length + root.serial.length + root.flightControllerSN.length > 0
                            onClicked: QGroundControl.copyToClipboard([
                                qsTr("Product model") + ": " + root.productModel,
                                qsTr("Factory code") + ": " + root.factoryCode,
                                qsTr("Airframe code (unique product identifier)") + ": " + root.serial,
                                qsTr("Flight controller SN") + ": " + root.flightControllerSN
                            ].join("\n"))
                        }
                        QGCButton { objectName: "caacUOMReadAll"; text: qsTr("Read all"); Layout.fillWidth: true; backgroundColor: palette.window; showBorder: true; enabled: !!root.vehicle; onClicked: root.refresh() }
                    }
                }
                ColumnLayout {
                    Layout.fillWidth: true
                    visible: tabs.currentIndex === 2
                    Heading { text: qsTr("Basic settings") }
                    Identifier { label: qsTr("Aircraft type"); value: root.vehicle ? root.vehicle.vehicleTypeString : "" }
                    Identifier { label: qsTr("Airframe code"); value: root.serial }
                    Heading { text: qsTr("Dynamic data") }
                    GridLayout {
                        Layout.fillWidth: true
                        columns: 3
                        Repeater {
                            model: [
                                {key: "heading", label: qsTr("Heading")},
                                {key: "mode", label: qsTr("Flight status")},
                                {key: "amsl", label: qsTr("Altitude AMSL")},
                                {key: "relative", label: qsTr("Relative altitude")},
                                {key: "speed", label: qsTr("Horizontal speed")},
                                {key: "climb", label: qsTr("Vertical speed")},
                                {key: "longitude", label: qsTr("Longitude")},
                                {key: "latitude", label: qsTr("Latitude")}
                            ]
                            SettingsFormRow {
                                id: datum
                                required property var modelData
                                Layout.fillWidth: true
                                Layout.fillHeight: true
                                ColumnLayout {
                                    Layout.fillWidth: true
                                    QGCLabel { objectName: "caacUOMTelemetry_" + datum.modelData.key; text: root.telemetryValue(datum.modelData.key); color: palette.text; Layout.fillWidth: true; horizontalAlignment: Text.AlignHCenter; wrapMode: Text.WordWrap }
                                    QGCLabel { text: datum.modelData.label; Layout.fillWidth: true; horizontalAlignment: Text.AlignHCenter; wrapMode: Text.WordWrap; font.pointSize: ScreenTools.smallFontPointSize }
                                }
                            }
                        }
                    }
                    Heading { text: qsTr("Data statistics") }
                    SettingsFormRow { QGCLabel { text: qsTr("Communication status: reporting service is not connected"); Layout.fillWidth: true; wrapMode: Text.WordWrap } }
                    SettingsFormRow { QGCLabel { text: qsTr("Uploaded packets: 0"); Layout.fillWidth: true } }
                    SettingsFormRow { QGCLabel { text: qsTr("Last upload: none"); Layout.fillWidth: true } }
                }
            }
        }
    }
}
