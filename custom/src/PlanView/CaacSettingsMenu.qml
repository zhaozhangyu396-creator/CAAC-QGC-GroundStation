pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import QGroundControl
import QGroundControl.Controls

Rectangle {
    id: root
    objectName: "caacSettingsMenu"
    width: !submenuExpanded ? ScreenTools.defaultFontPixelWidth * 11 + spacingSize * 2
        : _expandedWidth - (compactDirectory ? 0 : _expandedContentWidth * 0.25)
    height: Math.min(mainWindow.height * 0.8, ScreenTools.defaultFontPixelHeight * 35)
    color: QGroundControl.globalPalette.window
    radius: ScreenTools.defaultBorderRadius
    readonly property real spacingSize: ScreenTools.defaultFontPixelWidth
    readonly property string selectedPage: mainWindow.caacSettingsPage
    property bool submenuExpanded: true
    onSelectedPageChanged: submenuExpanded = true
    function closeCurrentPage(page) {
        if (selectedPage === page && submenuExpanded) { submenuExpanded = false; return true }
        return false
    }
    function togglePage(page) {
        if (!mainWindow.allowViewSwitch() || closeCurrentPage(page)) { return }
        submenuExpanded = true
        mainWindow.caacSettingsPage = page
    }
    readonly property bool compactDirectory: selectedPage === "Analyze"
        && !!submenu.item && !submenu.item._embeddedPageOpened
    readonly property real preferredContentWidth: compactDirectory ? submenu.item.navigationWidth : ScreenTools.defaultFontPixelWidth * 58
    readonly property real _expandedWidth: Math.min(
        mainWindow.width * (mainWindow.width < ScreenTools.defaultFontPixelWidth * 96 ? 0.95 : 0.58),
        preferredContentWidth + ScreenTools.defaultFontPixelWidth * 11 + spacingSize * 4)
    readonly property real _expandedContentWidth: Math.max(0,
        _expandedWidth - ScreenTools.defaultFontPixelWidth * 11 - spacingSize * 4 - 1)
    Component.onCompleted: mainWindow.caacSettingsMenu = root
    Component.onDestruction: {
        mainWindow.caacSettingsMenu = null
        mainWindow.caacVehicleConfigView = null
    }
    function showApplicationSettings() {
        submenuExpanded = true
        if (selectedPage === "ApplicationSettings" && submenu.item) {
            submenu.item.showNavigation()
            if (mainWindow.caacApplicationSettingsPage !== "") {
                submenu.item.showSettingsPage(mainWindow.caacApplicationSettingsPage)
            }
            mainWindow.caacApplicationSettingsPage = ""
        } else {
            mainWindow.caacSettingsPage = "ApplicationSettings"
        }
    }
    function showAnalyzeNavigation() {
        submenuExpanded = true
        if (selectedPage === "Analyze" && submenu.item) {
            submenu.item.showNavigation()
        } else {
            mainWindow.caacSettingsPage = "Analyze"
        }
    }
    component NavigationButton: QGCButton {
        id: button
        property string imageSource: "/res/gear-white.svg"
        width: navigation.width
        height: ScreenTools.defaultFontPixelHeight * 3.3
        background: Rectangle {
            color: button.checked ? QGroundControl.globalPalette.window
                : button.hovered ? QGroundControl.globalPalette.windowShade : "transparent"
            radius: ScreenTools.defaultBorderRadius
            border.color: button.checked ? QGroundControl.globalPalette.text : "transparent"
            border.width: button.checked ? 1 : 0
        }
        contentItem: ColumnLayout {
            spacing: ScreenTools.defaultFontPixelWidth * 0.3
            QGCColoredImage {
                source: button.imageSource
                color: "white"
                Layout.alignment: Qt.AlignHCenter
                Layout.preferredWidth: ScreenTools.defaultFontPixelHeight
                Layout.preferredHeight: Layout.preferredWidth
                fillMode: Image.PreserveAspectFit
            }
            QGCLabel {
                text: button.text
                color: "white"
                Layout.fillWidth: true
                horizontalAlignment: Text.AlignHCenter
                font.pointSize: ScreenTools.smallFontPointSize
                wrapMode: Text.WordWrap
            }
        }
    }
    RowLayout {
        anchors.fill: parent
        anchors.margins: root.spacingSize
        spacing: root.spacingSize
        QGCFlickable {
            objectName: "caacSettingsNavigationScroller"
            consumeWheelEvents: true
            Layout.preferredWidth: ScreenTools.defaultFontPixelWidth * 11
            Layout.minimumWidth: Layout.preferredWidth
            Layout.maximumWidth: Layout.preferredWidth
            Layout.fillHeight: true
            contentWidth: width
            contentHeight: navigation.implicitHeight
            clip: true
            Rectangle { width: parent.width; height: Math.max(navigation.implicitHeight, parent.height); color: "#20262b"; z: -1 }
            Column {
                id: navigation
                width: parent.width
                spacing: ScreenTools.defaultFontPixelWidth * 0.4
                NavigationButton {
                    objectName: "toolbar_simulation"
                    text: QGroundControl.corePlugin.simulation.active ? qsTr("Cancel simulation") : qsTr("Simulation")
                    imageSource: "/InstrumentValueIcons/play.svg"
                    onClicked: { mainWindow.closeIndicatorDrawer(); mainWindow.togglePlanningSimulation() }
                }
                NavigationButton {
                    objectName: "toolbar_connections"
                    text: qsTr("Connection")
                    imageSource: "/InstrumentValueIcons/usb.svg"
                    checked: root.submenuExpanded && root.selectedPage === "Connection"
                    onClicked: root.togglePage("Connection")
                }
                NavigationButton {
                    objectName: "toolbar_viewConfigure"
                    text: qsTr("Vehicle setup")
                    imageSource: "/res/GearWithPaperPlane.svg"
                    checked: root.submenuExpanded && root.selectedPage === "VehicleSetup"
                    onClicked: {
                        if (mainWindow.allowViewSwitch() && !root.closeCurrentPage("VehicleSetup")) { mainWindow.showVehicleConfig() }
                    }
                }
                NavigationButton {
                    objectName: "toolbar_rtk"
                    text: "RTK"
                    imageSource: "/InstrumentValueIcons/globe.svg"
                    checked: root.submenuExpanded && root.selectedPage === "RTK"
                    onClicked: root.togglePage("RTK")
                }
                NavigationButton {
                    objectName: "toolbar_rid"
                    text: "RID"
                    imageSource: "/qmlimages/RidIconManNoID.svg"
                    checked: root.submenuExpanded && root.selectedPage === "RID"
                    onClicked: root.togglePage("RID")
                }
                NavigationButton {
                    objectName: "toolbar_preferences"
                    text: qsTr("Preferences")
                    checked: root.submenuExpanded && root.selectedPage === "Preferences"
                    onClicked: root.togglePage("Preferences")
                }
                NavigationButton {
                    objectName: "toolbar_viewSettings"
                    text: qsTr("Application settings")
                    checked: root.submenuExpanded && root.selectedPage === "ApplicationSettings"
                    onClicked: { if (mainWindow.allowViewSwitch() && !root.closeCurrentPage("ApplicationSettings")) { mainWindow.showSettingsTool() } }
                }
                NavigationButton {
                    objectName: "toolbar_safety"
                    text: qsTr("Safety")
                    imageSource: "/InstrumentValueIcons/shield.svg"
                    checked: root.submenuExpanded && root.selectedPage === "Safety"
                    onClicked: root.togglePage("Safety")
                }
                NavigationButton {
                    objectName: "toolbar_uom"
                    text: "UOM"
                    imageSource: "/InstrumentValueIcons/airplane.svg"
                    checked: root.submenuExpanded && root.selectedPage === "UOM"
                    onClicked: root.togglePage("UOM")
                }
                NavigationButton {
                    objectName: "toolbar_planningStats"
                    text: qsTr("Statistics")
                    imageSource: "/res/chevron-double-right.svg"
                    onClicked: { mainWindow.closeIndicatorDrawer(); mainWindow.togglePlanningStatistics() }
                }
                NavigationButton {
                    objectName: "toolbar_viewAnalyze"
                    text: qsTr("Analyze")
                    imageSource: "/qmlimages/Analyze.svg"
                    visible: QGroundControl.corePlugin.showAdvancedUI
                    checked: root.submenuExpanded && root.selectedPage === "Analyze"
                    onClicked: {
                        if (mainWindow.allowViewSwitch() && !root.closeCurrentPage("Analyze")) { mainWindow.showAnalyzeTool() }
                    }
                }
                NavigationButton {
                    objectName: "toolbar_viewClose"
                    text: qsTr("Close")
                    imageSource: "/res/OpenDoor.svg"
                    onClicked: { if (mainWindow.allowViewSwitch()) { mainWindow.closeIndicatorDrawer(); mainWindow.close() } }
                }
                NavigationButton {
                    objectName: "toolbar_updateButton"
                    text: qsTr("Update")
                    imageSource: "/res/UpdateAvailable.svg"
                    visible: QGroundControl.newStableVersion !== ""
                    onClicked: { mainWindow.closeIndicatorDrawer(); Qt.openUrlExternally(QGroundControl.corePlugin.stableDownloadUrl) }
                }
            }
        }
        Rectangle { visible: root.submenuExpanded; Layout.preferredWidth: 1; Layout.fillHeight: true; color: "#444c53" }
        ColumnLayout {
            objectName: "caacSettingsDetails"
            visible: root.submenuExpanded
            Layout.fillWidth: true
            Layout.minimumWidth: 0
            Layout.fillHeight: true
            QGCLabel {
                text: root.selectedPage === "Connection" ? qsTr("Connection")
                    : root.selectedPage === "RTK" ? "RTK" : root.selectedPage === "RID" ? "RID"
                    : root.selectedPage === "Safety" ? qsTr("Safety") : root.selectedPage === "UOM" ? "UOM"
                    : root.selectedPage === "ApplicationSettings" ? qsTr("Application settings")
                    : root.selectedPage === "Analyze" ? qsTr("Analyze") : qsTr("Preferences")
                // Vehicle setup shares this panel with Preferences.
                visible: root.selectedPage !== "VehicleSetup"
                font.bold: true
            }
            Loader {
                id: submenu
                objectName: "caacSettingsSubmenu"
                Layout.fillWidth: true
                Layout.minimumWidth: 0
                Layout.fillHeight: true
                source: root.selectedPage === "Connection" ? "qrc:/qml/CAAC/Plan/CaacConnectionSettings.qml"
                    : root.selectedPage === "VehicleSetup" ? "qrc:/qml/QGroundControl/VehicleSetup/VehicleConfigView.qml"
                    : root.selectedPage === "RTK" ? "qrc:/qml/QGroundControl/AppSettings/RTKCorrectionsSettings.qml"
                    : root.selectedPage === "RID" ? "qrc:/qml/CAAC/Plan/CaacRIDSettings.qml"
                    : root.selectedPage === "Safety" ? "qrc:/qml/CAAC/Plan/CaacSafetySettings.qml"
                    : root.selectedPage === "UOM" ? "qrc:/qml/CAAC/Plan/CaacUOMSettings.qml"
                    : root.selectedPage === "ApplicationSettings" ? "qrc:/qml/QGroundControl/Controls/AppSettings.qml"
                    : root.selectedPage === "Analyze" ? "qrc:/qml/QGroundControl/AnalyzeView/AnalyzeView.qml"
                    : "qrc:/qml/CAAC/Plan/CaacPreferences.qml"
                onLoaded: {
                    if (root.selectedPage === "RTK") { item.fillAvailableWidth = true }
                    if (root.selectedPage === "VehicleSetup") {
                        item.embedded = true
                        mainWindow.vehicleConfigReady(item)
                    } else {
                        mainWindow.caacVehicleConfigView = null
                    }
                    if (root.selectedPage === "ApplicationSettings") {
                        item.embedded = true
                        if (mainWindow.caacApplicationSettingsPage !== "") {
                            item.showSettingsPage(mainWindow.caacApplicationSettingsPage)
                        }
                        mainWindow.caacApplicationSettingsPage = ""
                    }
                    if (root.selectedPage === "Analyze") { item.embedded = true }
                }
            }
            QGCLabel { text: QGroundControl.qgcVersion; font.pointSize: ScreenTools.smallFontPointSize; Layout.fillWidth: true }
        }
    }
}
