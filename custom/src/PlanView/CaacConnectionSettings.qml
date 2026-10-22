pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Layouts

import QGroundControl
import QGroundControl.Controls

Item {
    id: root
    objectName: "caacConnectionSettings"
    readonly property var manager: QGroundControl.linkManager
    property bool advanced: false
    readonly property string port: serialPort.currentIndex >= 0 && serialPort.model.length
        ? serialPort.model[serialPort.currentIndex].path : ""
    readonly property var configuration: {
        const configurations = manager.linkConfigurations
        for (let i = 0; i < configurations.count; ++i) {
            const entry = configurations.get(i)
            if (entry.linkType === LinkConfiguration.TypeSerial && entry.portName === port) { return entry }
        }
        return null
    }
    readonly property bool connected: !!configuration && configuration.linkActive
    QGCPalette { id: palette }

    function refreshPorts(rescan = true) {
        let selected = port
        if (!selected) {
            const configurations = manager.linkConfigurations
            for (let i = 0; i < configurations.count; ++i) {
                const entry = configurations.get(i)
                if (entry.linkType === LinkConfiguration.TypeSerial && entry.linkActive) { selected = entry.portName; break }
            }
        }
        if (rescan) { manager.refreshSerialPorts() }
        const paths = ScreenTools.isSerialAvailable ? manager.serialPorts : []
        const labels = ScreenTools.isSerialAvailable ? manager.serialPortStrings : []
        const entries = [{label: qsTr("选择串口"), path: ""}]
        for (let i = 0; i < paths.length; ++i) { entries.push({label: labels[i] || paths[i], path: paths[i]}) }
        if (selected && !entries.some(entry => entry.path === selected)) { entries.push({label: selected, path: selected}) }
        serialPort.model = entries
        serialPort.currentIndex = Math.max(0, entries.findIndex(entry => entry.path === selected))
    }
    function saveConfiguration(automatic) {
        if (!port || connected) { return configuration }
        const original = configuration && !configuration.dynamic ? configuration : null
        const draft = original ? manager.startConfigurationEditing(original)
            : manager.createConfiguration(LinkConfiguration.TypeSerial, qsTr("串口 %1").arg(port))
        if (!draft) { return null }
        draft.portName = port
        draft.baud = Number(baud.currentText)
        draft.autoConnect = automatic
        if (original) {
            manager.endConfigurationEditing(original, draft)
            return original
        }
        draft.dynamic = false
        manager.endCreateConfiguration(draft)
        return draft
    }
    Component.onCompleted: refreshPorts()
    Connections {
        target: root.manager
        function onCommPortsChanged() { root.refreshPorts(false) }
        function onCommPortStringsChanged() { root.refreshPorts(false) }
    }

    QGCFlickable {
        id: connectionScroller
        objectName: "caacConnectionScroller"
        anchors.fill: parent
        visible: !root.advanced
        contentHeight: connectionColumn.implicitHeight
        clip: true

        ColumnLayout {
            id: connectionColumn
            width: connectionScroller.width
            height: Math.max(implicitHeight, connectionScroller.height)
            spacing: ScreenTools.defaultFontPixelHeight
            RowLayout {
                Layout.fillWidth: true
                Rectangle {
                    Layout.preferredWidth: ScreenTools.defaultFontPixelWidth * 0.4
                    Layout.preferredHeight: ScreenTools.defaultFontPixelHeight
                    color: palette.settingsSectionAccent
                    radius: width / 2
                }
                QGCLabel { text: qsTr("连接设置"); font.bold: true; Layout.fillWidth: true }
            }
            SettingsFormRow {
                Layout.fillWidth: true
                spacing: ScreenTools.defaultFontPixelWidth
                QGCComboBox {
                    id: serialPort
                    objectName: "caacConnectionSerialPort"
                    Layout.fillWidth: true
                    Layout.minimumWidth: 0
                    textRole: "label"
                    model: []
                    enabled: ScreenTools.isSerialAvailable && !root.connected && count > 1
                }
                QGCComboBox {
                    id: baud
                    objectName: "caacConnectionBaudRate"
                    Layout.preferredWidth: ScreenTools.defaultFontPixelWidth * 12
                    model: ScreenTools.isSerialAvailable ? root.manager.serialBaudRates : []
                    currentIndex: Math.max(0, model.indexOf(root.configuration ? root.configuration.baud.toString() : "115200"))
                    enabled: ScreenTools.isSerialAvailable && !root.connected
                }
            }
            RowLayout {
                Layout.fillWidth: true
                spacing: ScreenTools.defaultFontPixelWidth
                QGCButton {
                    objectName: "caacSerialConnectButton"
                    Layout.fillWidth: true
                    Layout.minimumWidth: 0
                    Layout.preferredHeight: ScreenTools.defaultFontPixelHeight * 2.8
                    text: qsTr("连接")
                    pointSize: ScreenTools.defaultFontPointSize * 1.15
                    fontWeight: Font.Bold
                    backgroundColor: Qt.lighter(palette.settingsAccent, 1.3)
                    backRadius: ScreenTools.defaultFontPixelHeight * 0.45
                    showBorder: false
                    enabled: root.port !== "" && !root.connected
                    onClicked: {
                        const configuration = root.saveConfiguration(automatic.checked)
                        if (configuration) { root.manager.createConnectedLink(configuration) }
                    }
                }
                QGCButton {
                    objectName: "caacSerialRefreshButton"
                    Layout.preferredWidth: ScreenTools.defaultFontPixelHeight * 3
                    Layout.preferredHeight: ScreenTools.defaultFontPixelHeight * 2.8
                    iconSource: "/InstrumentValueIcons/reload.svg"
                    backgroundColor: "transparent"
                    showBorder: false
                    enabled: !root.connected
                    Accessible.name: qsTr("刷新串口")
                    onClicked: root.refreshPorts()
                }
            }
            QGCButton {
                objectName: "caacSerialDisconnectButton"
                Layout.fillWidth: true
                Layout.minimumWidth: 0
                text: qsTr("断开连接")
                enabled: root.connected
                onClicked: root.manager.disconnectLinkConfiguration(root.configuration)
            }
            SettingsFormRow {
                QGCCheckBoxSlider {
                    id: automatic
                    objectName: "caacSerialAutoConnect"
                    Layout.fillWidth: true
                    text: qsTr("自动连接")
                    enabled: root.port !== "" && !root.connected
                    Binding on checked { value: !!root.configuration && root.configuration.autoConnect }
                    onClicked: root.saveConfiguration(checked)
                }
            }
            QGCLabel {
                Layout.fillWidth: true
                wrapMode: Text.WordWrap
                visible: serialPort.count <= 1
                text: qsTr("未检测到串口，请连接设备后刷新。")
                color: palette.text
            }
            QGCButton {
                objectName: "caacMoreConnectionSettings"
                text: qsTr("更多连接设置")
                Layout.fillWidth: true
                onClicked: root.advanced = true
            }
            Item { Layout.fillHeight: true }
        }
    }
    ColumnLayout {
        anchors.fill: parent
        visible: root.advanced
        QGCButton {
            objectName: "caacConnectionBack"
            text: qsTr("返回连接设置")
            Layout.fillWidth: true
            onClicked: root.advanced = false
        }
        Loader {
            Layout.fillWidth: true
            Layout.fillHeight: true
            active: root.advanced
            source: "qrc:/qml/QGroundControl/AppSettings/CommLinksSettings.qml"
            onLoaded: item.fillAvailableWidth = true
        }
    }
}
