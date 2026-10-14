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
    QGCPalette { id: palette }
    component UOMTab: TabButton {
        id: tab
        contentItem: QGCLabel { text: tab.text; horizontalAlignment: Text.AlignHCenter; wrapMode: Text.WordWrap }
        background: Rectangle {
            color: palette.windowShadeDark
            Rectangle { anchors.left: parent.left; anchors.right: parent.right; anchors.bottom: parent.bottom; height: 3; color: palette.colorGreen; visible: tab.checked }
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
    component Identifier: ColumnLayout {
        id: entry
        required property string label
        required property string value
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
                    Identifier { label: qsTr("Airframe code (unique product identifier)"); value: root.serial }
                    QGCLabel { text: qsTr("UAS code"); Layout.fillWidth: true }
                    FactTextField { objectName: "caacUOMUASCode"; fact: root.rid.basicID; Layout.fillWidth: true }
                    QGCLabel {
                        Layout.fillWidth: true
                        wrapMode: Text.WordWrap
                        text: qsTr("Complete registration and add the aircraft on the official UOM platform, then enter the UAS code from your aircraft record.")
                    }
                    QGCButton {
                        text: qsTr("Open official UOM platform")
                        Layout.fillWidth: true
                        backgroundColor: palette.colorGreen
                        onClicked: Qt.openUrlExternally("https://uom.caac.gov.cn/")
                    }
                    QGCButton { text: qsTr("Refresh status"); enabled: !!root.vehicle; onClicked: root.refresh() }
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
                        QGCButton { text: qsTr("Read all"); Layout.fillWidth: true; backgroundColor: palette.colorGreen; enabled: !!root.vehicle; onClicked: root.refresh() }
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
                            ColumnLayout {
                                id: datum
                                required property var modelData
                                Layout.fillWidth: true
                                QGCLabel { objectName: "caacUOMTelemetry_" + datum.modelData.key; text: root.telemetryValue(datum.modelData.key); color: palette.colorGreen; Layout.fillWidth: true; horizontalAlignment: Text.AlignHCenter; wrapMode: Text.WordWrap }
                                QGCLabel { text: datum.modelData.label; Layout.fillWidth: true; horizontalAlignment: Text.AlignHCenter; wrapMode: Text.WordWrap; font.pointSize: ScreenTools.smallFontPointSize }
                            }
                        }
                    }
                    Heading { text: qsTr("Data statistics") }
                    QGCLabel { text: qsTr("Communication status: reporting service is not connected"); Layout.fillWidth: true; wrapMode: Text.WordWrap }
                    QGCLabel { text: qsTr("Uploaded packets: 0") }
                    QGCLabel { text: qsTr("Last upload: none") }
                }
            }
        }
    }
}
