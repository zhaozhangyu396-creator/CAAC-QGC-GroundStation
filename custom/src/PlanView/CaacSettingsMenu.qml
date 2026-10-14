pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import QGroundControl
import QGroundControl.Controls

Rectangle {
    id: root
    objectName: "caacSettingsMenu"
    width: Math.min(mainWindow.width * 0.58, ScreenTools.defaultFontPixelWidth * 72)
    height: Math.min(mainWindow.height * 0.8, ScreenTools.defaultFontPixelHeight * 35)
    color: QGroundControl.globalPalette.window
    radius: ScreenTools.defaultBorderRadius
    readonly property real spacingSize: ScreenTools.defaultFontPixelWidth
    function settingsPage(name) {
        if (mainWindow.allowViewSwitch()) {
            mainWindow.closeIndicatorDrawer()
            mainWindow.showSettingsTool(name)
        }
    }
    component NavigationButton: QGCButton {
        id: button
        property string imageSource: "/res/gear-white.svg"
        width: navigation.width
        height: ScreenTools.defaultFontPixelHeight * 3.3
        background: Rectangle {
            color: button.checked ? "#245b46" : button.hovered ? "#39423e" : "transparent"
            radius: ScreenTools.defaultBorderRadius
            Rectangle { anchors.left: parent.left; width: 3; height: parent.height; color: "#32cc7c"; visible: button.checked }
        }
        contentItem: ColumnLayout {
            spacing: ScreenTools.defaultFontPixelWidth * 0.3
            QGCColoredImage {
                source: button.imageSource
                color: button.checked ? "#43db8b" : "white"
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
            Layout.preferredWidth: ScreenTools.defaultFontPixelWidth * 11
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
                    objectName: "toolbar_viewFlightPlan"
                    text: qsTr("Main view")
                    imageSource: "/qmlimages/Plan.svg"
                    onClicked: { mainWindow.closeIndicatorDrawer(); mainWindow.showPlanView() }
                }
                NavigationButton {
                    objectName: "toolbar_connections"
                    text: qsTr("Connection")
                    imageSource: "/InstrumentValueIcons/usb.svg"
                    onClicked: root.settingsPage("Comm Links")
                }
                NavigationButton {
                    objectName: "toolbar_rtk"
                    text: "RTK"
                    imageSource: "/InstrumentValueIcons/globe.svg"
                    onClicked: root.settingsPage("RTK Corrections")
                }
                NavigationButton {
                    objectName: "toolbar_rid"
                    text: "RID"
                    imageSource: "/qmlimages/RidIconManNoID.svg"
                    onClicked: root.settingsPage("Remote ID")
                }
                NavigationButton {
                    objectName: "toolbar_viewConfigure"
                    text: qsTr("Vehicle setup")
                    imageSource: "/res/GearWithPaperPlane.svg"
                    onClicked: {
                        if (mainWindow.allowViewSwitch()) { mainWindow.closeIndicatorDrawer(); mainWindow.showVehicleConfig() }
                    }
                }
                NavigationButton { objectName: "toolbar_preferences"; text: qsTr("Preferences"); checked: true }
                NavigationButton {
                    objectName: "toolbar_viewSettings"
                    text: qsTr("Application settings")
                    onClicked: root.settingsPage("")
                }
                NavigationButton {
                    objectName: "toolbar_simulation"
                    text: QGroundControl.corePlugin.simulation.active ? qsTr("Cancel simulation") : qsTr("Simulation")
                    imageSource: "/InstrumentValueIcons/play.svg"
                    onClicked: { mainWindow.closeIndicatorDrawer(); mainWindow.togglePlanningSimulation() }
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
                    onClicked: {
                        if (mainWindow.allowViewSwitch()) { mainWindow.closeIndicatorDrawer(); mainWindow.showAnalyzeTool() }
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
        Rectangle { Layout.preferredWidth: 1; Layout.fillHeight: true; color: "#444c53" }
        ColumnLayout {
            Layout.fillWidth: true
            Layout.fillHeight: true
            QGCLabel { text: qsTr("Preferences"); font.bold: true }
            CaacPreferences { Layout.fillWidth: true; Layout.fillHeight: true }
            QGCLabel { text: QGroundControl.qgcVersion; font.pointSize: ScreenTools.smallFontPointSize; Layout.fillWidth: true }
        }
    }
}
