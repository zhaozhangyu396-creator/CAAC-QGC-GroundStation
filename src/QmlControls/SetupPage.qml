import QtQuick
import QtQuick.Controls
import QtQuick.Dialogs
import QtQuick.Layouts

import QGroundControl
import QGroundControl.FactControls
import QGroundControl.Controls

/// Base view control for all Setup pages
Item {
    id:             setupView
    enabled:        !_disableDueToArmed && !_disableDueToFlying

    property alias  pageComponent:          pageLoader.sourceComponent
    property string pageName:               ScreenTools.vehicleComponentName(vehicleComponent)
    property string pageDescription:        vehicleComponent ? vehicleComponent.description : ""
    property real   availableWidth:         width - pageLoader.x - (vehicleSetupStyle ? _margins : 0)
    property real   availableHeight:        height - pageLoader.y
    property bool   showAdvanced:           false
    property alias  advanced:               advancedCheckBox.checked
    property string sectionIdFilter:        ""
    readonly property bool vehicleSetupStyle: ScreenTools.isVehicleSetupItem(setupView)

    function sectionVisible(sectionId) {
        if (pageLoader.item && typeof pageLoader.item.sectionVisible === "function") {
            return pageLoader.item.sectionVisible(sectionId)
        }
        return true
    }

    onSectionIdFilterChanged: {
        if (pageLoader.item && typeof pageLoader.item.sectionIdFilter !== "undefined") {
            pageLoader.item.sectionIdFilter = sectionIdFilter
        }
    }

    property bool   _vehicleArmed:          globals.activeVehicle ? globals.activeVehicle.armed : false
    property bool   _vehicleAirborne:       globals.activeVehicle ? globals.activeVehicle.airborne : false
    property bool   _disableDueToArmed:     vehicleComponent ? (!vehicleComponent.allowSetupWhileArmed && _vehicleArmed) : false
    property bool   _disableDueToFlying:    vehicleComponent ? (!vehicleComponent.allowSetupWhileFlying && _vehicleAirborne) : false
    property string _disableReason:         _disableDueToArmed ? qsTr("armed") : qsTr("flying")
    property real   _margins:               ScreenTools.defaultFontPixelHeight * 0.5

    Component.onCompleted: {
        if(pageLoader.item && pageLoader.item.setupPageCompleted) {
            pageLoader.item.setupPageCompleted()
        }
        if (pageLoader.item && typeof pageLoader.item.sectionIdFilter !== "undefined") {
            pageLoader.item.sectionIdFilter = sectionIdFilter
        }
    }

    QGCFlickable {
        objectName:     "vehicleSetup_scroller"
        anchors.fill:   parent
        contentWidth:   Math.max(availableWidth, pageLoader.x + pageLoader.item.width)
        contentHeight:  Math.max(availableHeight, pageLoader.y + pageLoader.item.height)
        clip:           true

        RowLayout {
            id:                 headingRow
            width:              availableWidth
            spacing:            _margins
            layoutDirection:    Qt.RightToLeft
            visible:            vehicleSetupStyle || showAdvanced || (pageDescription !== "" && !ScreenTools.isShortScreen)
            x:                  vehicleSetupStyle ? _margins : 0

            QGCCheckBox {
                id:         advancedCheckBox
                text:       qsTr("Advanced")
                visible:    showAdvanced
            }

            ColumnLayout {
                spacing:            _margins
                Layout.fillWidth:   true

                RowLayout {
                    objectName: "vehicleSetup_header"
                    Layout.fillWidth: true
                    Layout.topMargin: setupView._margins
                    Layout.bottomMargin: setupView._margins
                    visible: setupView.vehicleSetupStyle && setupView.pageName !== ""
                    spacing: ScreenTools.defaultFontPixelWidth
                    Rectangle {
                        width: ScreenTools.defaultFontPixelWidth * 0.35
                        height: ScreenTools.defaultFontPixelHeight * 1.2
                        radius: width / 2
                        color: qgcPal.settingsAccent
                    }
                    QGCLabel {
                        objectName: "vehicleSetup_title"
                        Layout.fillWidth: true
                        text: setupView.pageName
                        font.pointSize: ScreenTools.mediumFontPointSize
                        font.bold: true
                        wrapMode: Text.WordWrap
                    }
                }

                QGCLabel {
                    Layout.fillWidth:   true
                    wrapMode:           Text.WordWrap
                    text:               pageDescription
                    visible:            pageDescription !== "" && !ScreenTools.isShortScreen
                }

                QGCLabel {
                    Layout.fillWidth:   true
                    wrapMode:           Text.WordWrap
                    text:               qsTr("Disabled while the vehicle is %1").arg(_disableReason)
                    color:              qgcPal.warningText
                    visible:            !setupView.enabled && !ScreenTools.isShortScreen
                }
            }
        }

        Loader {
            id:                 pageLoader
            objectName:         "setupPage_contentLoader"
            x:                  vehicleSetupStyle ? _margins : 0
            anchors.topMargin:  _margins
            anchors.top:        headingRow.bottom
        }

        // Overlay to display when vehicle is armed and this setup page needs
        // to be disabled
        Rectangle {
            visible:            !setupView.enabled
            anchors.fill:       parent
            color:              "black"
            opacity:            0.5
        }
    }
}
