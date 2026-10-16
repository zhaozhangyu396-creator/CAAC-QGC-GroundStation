import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import QGroundControl
import QGroundControl.Controls

ColumnLayout {
    id:         root
    spacing:    ScreenTools.defaultFontPixelWidth / 4

    property var model
    property int currentPageIndex: 0

    property real _availableHeight: availableHeight
    property real _availableWidth:  availableWidth
    width: ScreenTools.isVehicleSetupItem(root) ? _availableWidth : implicitWidth
    readonly property bool compactNavigation: ScreenTools.isVehicleSetupItem(root)
        && _availableWidth < ScreenTools.defaultFontPixelWidth * 80
    onCompactNavigationChanged: if (!compactNavigation) {
        Qt.callLater(() => { tabBar.currentIndex = root.currentPageIndex })
    }
    onCurrentPageIndexChanged: if (!compactNavigation) { tabBar.currentIndex = currentPageIndex }

    FactPanelController {
        id:         controller
    }

    QGCTabBar {
        id: tabBar
        Layout.fillWidth: true
        Layout.minimumWidth: ScreenTools.isVehicleSetupItem(root) ? 0 : -1
        visible: !root.compactNavigation
        currentIndex: root.currentPageIndex

        Repeater {
            model: root.model
            QGCTabButton {
                objectName: "pidTuning_tab_" + buttonText.replace(/ /g, "")
                text: buttonText
                width: ScreenTools.isVehicleSetupItem(root) ? tabBar.width / root.model.count : implicitWidth
                onClicked: root.currentPageIndex = index
            }
        }
    }

    QGCComboBox {
        objectName: "pidTuning_sectionMenu"
        Layout.fillWidth: true
        Layout.minimumWidth: ScreenTools.isVehicleSetupItem(root) ? 0 : -1
        visible: root.compactNavigation
        model: root.model
        textRole: "buttonText"
        currentIndex: root.currentPageIndex
        onActivated: (index) => { root.currentPageIndex = index }
    }

    Loader {
        id:     loader
        Layout.fillWidth: ScreenTools.isVehicleSetupItem(root)
        Layout.minimumWidth: ScreenTools.isVehicleSetupItem(root) ? 0 : -1
        source: root.model && root.currentPageIndex >= 0 && root.currentPageIndex < root.model.count
            ? root.model.get(root.currentPageIndex).tuningPage : ""

        property bool useAutoTuning:    true
        property real availableWidth:   _availableWidth
        property real availableHeight:  _availableHeight - loader.y
    }
}
