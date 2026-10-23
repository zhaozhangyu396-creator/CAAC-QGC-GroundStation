import QtQuick
import QtQuick.Layouts

import QGroundControl
import QGroundControl.Controls

Rectangle {
    id: root
    signal titleDragged(point delta)
    readonly property var _settings: QGroundControl.settingsManager.flightMapSettings
    readonly property var _types: QGroundControl.mapEngineManager.mapTypeList(_settings.mapProvider.rawValue)
    readonly property var _providers: QGroundControl.mapEngineManager.mapProviderList
    readonly property var _follow: QGroundControl.settingsManager.flyViewSettings.keepMapCenteredOnVehicle
    implicitHeight: contents.implicitHeight + ScreenTools.defaultFontPixelHeight
    color: palette.windowShadeDark
    radius: ScreenTools.defaultBorderRadius
    QGCPalette { id: palette; colorGroupEnabled: root.enabled }

    ColumnLayout {
        id: contents
        width: parent.width
        spacing: ScreenTools.defaultFontPixelHeight
        Rectangle {
            Layout.fillWidth: true
            implicitHeight: ScreenTools.defaultFontPixelHeight * 2.5
            color: palette.windowShade
            CaacPanelDragHandle {
                objectName: "caacMapSettingsDragHandle"
                anchors.fill: parent
                onMoved: (delta) => root.titleDragged(delta)
            }
            QGCLabel {
                anchors.centerIn: parent
                text: qsTr("Map settings")
                font.bold: true
                font.pointSize: ScreenTools.largeFontPointSize
            }
        }
        GridLayout {
            Layout.fillWidth: true
            Layout.leftMargin: ScreenTools.defaultFontPixelWidth
            Layout.rightMargin: ScreenTools.defaultFontPixelWidth
            columns: 2
            rowSpacing: ScreenTools.defaultFontPixelHeight
            columnSpacing: ScreenTools.defaultFontPixelWidth
            QGCLabel { text: qsTr("Provider") }
            QGCComboBox {
                objectName: "caacMapProvider"
                Layout.fillWidth: true
                Layout.minimumWidth: 0
                model: root._providers
                textTransform: (value) => MapNames.displayName(value)
                currentIndex: root._providers.indexOf(root._settings.mapProvider.rawValue)
                onActivated: (index) => {
                    const provider = root._providers[index]
                    const types = QGroundControl.mapEngineManager.mapTypeList(provider)
                    if (!provider || !types.length) { return }
                    root._settings.mapProvider.rawValue = provider
                    if (types.indexOf(root._settings.mapType.rawValue) < 0) { root._settings.mapType.rawValue = types[0] }
                }
            }
            QGCLabel { text: qsTr("Type") }
            QGCComboBox {
                objectName: "caacMapType"
                Layout.fillWidth: true
                Layout.minimumWidth: 0
                model: root._types.map(value => ({ value: value, label: MapNames.displayName(value) }))
                textRole: "label"
                currentIndex: root._types.indexOf(root._settings.mapType.rawValue)
                onActivated: (index) => { root._settings.mapType.rawValue = root._types[index] }
            }
            QGCLabel { text: qsTr("Lock aircraft") }
            QGCComboBox {
                objectName: "caacMapLockAircraft"
                Layout.fillWidth: true
                Layout.minimumWidth: 0
                model: [qsTr("No"), qsTr("Yes")]
                currentIndex: root._follow.rawValue ? 1 : 0
                onActivated: (index) => { root._follow.rawValue = index === 1 }
            }
            QGCLabel { text: qsTr("Double-click to add") }
            QGCComboBox {
                objectName: "caacMapDoubleClickAdd"
                Layout.fillWidth: true
                Layout.minimumWidth: 0
                model: [qsTr("Disabled"), qsTr("Enabled")]
                currentIndex: QGroundControl.corePlugin.doubleClickAddWaypoint ? 1 : 0
                onActivated: (index) => { QGroundControl.corePlugin.doubleClickAddWaypoint = index === 1 }
            }
            QGCLabel { text: qsTr("Drag waypoints") }
            QGCComboBox {
                objectName: "caacMapDragWaypoints"
                Layout.fillWidth: true
                Layout.minimumWidth: 0
                model: [qsTr("Disabled"), qsTr("Enabled")]
                currentIndex: QGroundControl.corePlugin.dragWaypoints ? 1 : 0
                onActivated: (index) => { QGroundControl.corePlugin.dragWaypoints = index === 1 }
            }
        }
        QGCLabel {
            Layout.fillWidth: true
            Layout.leftMargin: ScreenTools.defaultFontPixelWidth
            Layout.rightMargin: ScreenTools.defaultFontPixelWidth
            text: qsTr("Waypoint switches apply in Plan view. Flight view shows the downloaded mission.")
            font.pointSize: ScreenTools.smallFontPointSize
            wrapMode: Text.WordWrap
        }
    }
}
