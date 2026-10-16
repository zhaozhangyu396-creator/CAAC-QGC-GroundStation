import QtQuick
import QtQuick.Controls
import QtQuick.Dialogs

import QGroundControl
import QGroundControl.FactControls
import QGroundControl.Controls
import "qrc:/qml/QGroundControl/Controls/AirframeCatalog.js" as AirframeCatalog

SetupPage {
    id:             airframePage
    objectName:     "airframeSetup_root"
    pageComponent:  categoryPage

    property string categoryFilter: ""
    readonly property int pendingAutostartId: controller.autostartId
    readonly property var _filteredTypes: controller.airframeTypes.filter(type =>
        type.airframes.some(airframe => airframe.category === categoryFilter))
    property bool _selectionMade: false

    function selectAirframe(airframe) {
        if (airframe) {
            controller.autostartId = airframe.autostartId
            _selectionMade = true
        }
    }

    AirframeComponentController {
        id:         controller
        objectName: "airframeSetup_controller"
    }

    Component {
        id: customFrame
        Column {
            width:          availableWidth
            spacing:        ScreenTools.defaultFontPixelHeight * 4
            Item {
                width:      1
                height:     1
            }
            QGCLabel {
                anchors.horizontalCenter: parent.horizontalCenter
                width:      parent.width * 0.5
                height:     ScreenTools.defaultFontPixelHeight * 4
                wrapMode:   Text.WordWrap
                text:       qsTr("Your vehicle is using a custom airframe configuration. ") +
                            qsTr("This configuration can only be modified through the Parameter Editor.\n\n") +
                            qsTr("If you want to reset your airframe configuration and select a standard configuration, click 'Reset' below.")
            }
            QGCButton {
                text:       qsTr("Reset")
                enabled:    sys_autostart
                anchors.horizontalCenter: parent.horizontalCenter
                property Fact sys_autostart: controller.getParameterFact(-1, "SYS_AUTOSTART")
                onClicked: {
                    if(sys_autostart) {
                        sys_autostart.value = 0
                    }
                }
            }
        }
    }

    Component {
        id: categoryPage

        Column {
            id:     mainColumn
            width:  availableWidth

            property real _minW:        ScreenTools.defaultFontPixelWidth * 30
            property real _boxWidth:    _minW
            property real _boxSpace:    ScreenTools.defaultFontPixelWidth

            readonly property real spacerHeight: ScreenTools.defaultFontPixelHeight

            AirframeCategoryMenu {
                width: parent.width
                category: airframePage.categoryFilter
                onCategorySelected: category => airframePage.categoryFilter = category
            }

            Loader {
                width: parent.width
                active: airframePage.categoryFilter !== "" && controller.showCustomConfigPanel
                visible: active
                sourceComponent: customFrame
            }

            QGCLabel {
                width: parent.width
                visible: airframePage.categoryFilter !== "" && !controller.showCustomConfigPanel
                    && airframePage._filteredTypes.length === 0
                text: qsTr("当前固件没有此类型的可选机架。")
                wrapMode: Text.WordWrap
            }

            onWidthChanged: {
                computeDimensions()
            }

            Component.onCompleted: computeDimensions()

            function computeDimensions() {
                var sw  = 0
                var rw  = 0
                var idx = Math.floor(mainColumn.width / (_minW + ScreenTools.defaultFontPixelWidth))
                if(idx < 1) {
                    _boxWidth = mainColumn.width
                    _boxSpace = 0
                } else {
                    _boxSpace = 0
                    if(idx > 1) {
                        _boxSpace = ScreenTools.defaultFontPixelWidth
                        sw = _boxSpace * (idx - 1)
                    }
                    rw = mainColumn.width - sw
                    _boxWidth = rw / idx
                }
            }

            Item {
                id:             helpApplyRow
                visible:        airframePage.categoryFilter !== "" && !controller.showCustomConfigPanel
                    && airframePage._filteredTypes.length > 0
                anchors.left:   parent.left
                anchors.right:  parent.right
                height:         Math.max(helpText.contentHeight, applyButton.height)

                QGCLabel {
                    id:             helpText
                    width:          parent.width - applyButton.width - 5
                    text:           (controller.currentVehicleName != "" ?
                                         qsTr("You've connected a %1.").arg(controller.currentVehicleName) :
                                         qsTr("Airframe is not set.")) +
                                    qsTr(" To change this configuration, select the desired airframe below then click 'Apply and Restart'.")
                    font.bold:      true
                    wrapMode:       Text.WordWrap
                }

                QGCButton {
                    id:             applyButton
                    objectName:     "airframeSetup_applyButton"
                    anchors.right:  parent.right
                    text:           qsTr("Apply and Restart")
                    primary:        airframePage._selectionMade
                    enabled:        airframePage._selectionMade
                    onClicked:      QGroundControl.showMessageDialog(airframePage, qsTr("Apply and Restart"),
                                                                 qsTr("Clicking 'Apply' will save the changes you have made to your airframe configuration.<br><br>\
                                                                        All vehicle parameters other than Radio Calibration will be reset.<br><br>\
                                                                        Your vehicle will also be restarted in order to complete the process."),
                                                                 Dialog.Apply | Dialog.Cancel,
                                                                 function() { controller.changeAutostart() })

                }
            }

            Item {
                id:             lastSpacer
                visible:        helpApplyRow.visible
                height:         parent.spacerHeight
                width:          10
            }

            Flow {
                id:         flowView
                visible:    helpApplyRow.visible
                width:      parent.width
                spacing:    _boxSpace

                Repeater {
                    model: airframePage._filteredTypes

                    // Outer summary item rectangle
                    Rectangle {
                        id: typeBox
                        objectName: "airframeTypeBox_" + index
                        width:  _boxWidth
                        height: ScreenTools.defaultFontPixelHeight * 14
                        color:  qgcPal.window

                        property string airframeTypeName:     modelData.name
                        readonly property var _airframes: modelData.airframes.filter(airframe =>
                            airframe.category === airframePage.categoryFilter)
                        readonly property bool airframeTypeSelected: _airframes.some(airframe =>
                            airframe.autostartId === controller.autostartId)

                        readonly property real titleHeight: ScreenTools.defaultFontPixelHeight * 1.75
                        readonly property real innerMargin: ScreenTools.defaultFontPixelWidth

                        MouseArea {
                            anchors.fill: parent

                            onClicked: {
                                airframePage.selectAirframe(typeBox._airframes[combo.currentIndex])
                            }
                        }

                        QGCLabel {
                            id:     title
                            text:   modelData.name
                        }

                        Rectangle {
                            anchors.topMargin:  ScreenTools.defaultFontPixelHeight / 2
                            anchors.top:        title.bottom
                            anchors.bottom:     parent.bottom
                            anchors.left:       parent.left
                            anchors.right:      parent.right
                            color:              Qt.lighter(qgcPal.settingsAccent, 1.3)
                            border.color:       typeBox.airframeTypeSelected ? qgcPal.text : "transparent"
                            border.width:       typeBox.airframeTypeSelected ? 1 : 0

                            Image {
                                id:                 image
                                anchors.margins:    innerMargin
                                anchors.top:        parent.top
                                anchors.bottom:     combo.top
                                anchors.left:       parent.left
                                anchors.right:      parent.right
                                fillMode:           Image.PreserveAspectFit
                                smooth:             true
                                mipmap:             true
                                source:             AirframeCatalog.px4Image(modelData.name, modelData.imageResource)
                            }

                            QGCComboBox {
                                id:                 combo
                                objectName:         modelData.name + "ComboBox"
                                anchors.margins:    innerMargin
                                anchors.bottom:     parent.bottom
                                anchors.left:       parent.left
                                anchors.right:      parent.right
                                model:              typeBox._airframes
                                textRole:           "text"
                                currentIndex: {
                                    const selectedIndex = typeBox._airframes.findIndex(airframe =>
                                        airframe.autostartId === controller.autostartId)
                                    return selectedIndex >= 0 ? selectedIndex : 0
                                }

                                onActivated: (index) => {
                                    airframePage.selectAirframe(typeBox._airframes[index])
                                }
                            }
                        }
                    }
                } // Repeater - summary boxes
            } // Flow - summary boxes
        } // Column
    } // Component
} // SetupPage
