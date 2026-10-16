import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import QGroundControl
import QGroundControl.FactControls
import QGroundControl.Controls

SetupPage {
    pageComponent:  pageComponent
    Component {
        id: pageComponent

        Item {
            id:     root
            width:  availableWidth
            height: availableHeight

            property string sectionIdFilter: ""

            property real _margins:         ScreenTools.defaultFontPixelHeight / 2
            property var  _switchNameList:  [ "RC_MAP_ARM_SW", "RC_MAP_GEAR_SW", "RC_MAP_KILL_SW", "RC_MAP_LOITER_SW", "RC_MAP_OFFB_SW", "RC_MAP_RETURN_SW" ]
            property var  _switchTHList:    [ "RC_ARMSWITCH_TH", "RC_GEAR_TH", "RC_KILLSWITCH_TH", "RC_LOITER_TH", "RC_OFFB_TH", "RC_RETURN_TH" ]
            readonly property var _switchLabels: ({
                "RC_MAP_ARM_SW": qsTr("Arm switch channel"),
                "RC_MAP_GEAR_SW": qsTr("Landing gear switch channel"),
                "RC_MAP_KILL_SW": qsTr("Emergency Kill switch channel"),
                "RC_MAP_LOITER_SW": qsTr("Loiter switch channel"),
                "RC_MAP_OFFB_SW": qsTr("Offboard switch channel"),
                "RC_MAP_RETURN_SW": qsTr("Return switch channel"),
                "RC_MAP_TRANS_SW": qsTr("VTOL transition switch channel"),
                "RC_MAP_FLAPS": qsTr("Flaps channel")
            })
            readonly property var _modeLabels: ({
                "Unassigned": qsTr("Unassigned"),
                "Manual": qsTr("Manual"),
                "Altitude": qsTr("Altitude"),
                "Position": qsTr("Position"),
                "Mission": qsTr("Mission"),
                "Hold": qsTr("Hold"),
                "Return": qsTr("Return"),
                "Acro": qsTr("Acro"),
                "Offboard": qsTr("Offboard"),
                "Stabilized": qsTr("Stabilized"),
                "Position Slow": qsTr("Position Slow"),
                "Takeoff": qsTr("Takeoff"),
                "Land": qsTr("Land"),
                "Follow Me": qsTr("Follow Me"),
                "Precision Land": qsTr("Precision Land"),
                "Altitude Cruise": qsTr("Altitude Cruise")
            })

            function modeLabels(fact) {
                return fact.enumStrings.map(function(label) {
                    const externalMode = /^External Mode (\d+)$/.exec(label)
                    return externalMode ? qsTr("External Mode %1").arg(externalMode[1])
                        : root._modeLabels[label] || label
                })
            }

            readonly property real _channelComboWidth: ScreenTools.defaultFontPixelWidth * 13

            function channelLabels(fact) {
                return fact.enumStrings.map(function(label) {
                    if (label === "Unassigned") {
                        return qsTr("Unassigned")
                    }
                    const channel = /^Channel (\d+)$/.exec(label)
                    return channel ? qsTr("Channel %1").arg(channel[1]) : label
                })
            }

            Component.onCompleted: {
                if (controller.vehicle.vtol) {
                    _switchNameList.push("RC_MAP_TRANS_SW")
                    _switchTHList.push("RC_TRANS_TH")
                }
                if (controller.vehicle.fixedWing) {
                    _switchNameList.push("RC_MAP_FLAPS")
                    _switchTHList.push("")
                }
                switchRepeater.model = _switchNameList
            }

            PX4SimpleFlightModesController {
                id: controller
            }

            QGCFlickable {
                objectName: "px4FlightModesScroller"
                anchors.fill:   parent
                clip:           true
                contentWidth:   settingsRow.width
                contentHeight:  column1.height

                Column {
                    id:         column1
                    spacing:    _margins

                    QGCLabel {
                        objectName: "px4ModeInputSource"
                        text: qsTr("Input source: %1").arg(controller.joystickInput
                            ? qsTr("USB joystick") : qsTr("Flight controller receiver"))
                    }

                    QGCLabel {
                        width: root.width
                        wrapMode: Text.WordWrap
                        text: qsTr("Yellow indicates the input switch position. Check the top bar for the actual flight mode.")
                        visible: sectionIdFilter === "" || sectionIdFilter === "Flight Modes"
                    }

                    Grid {
                        id:         settingsRow
                        columns: QGroundControl.corePlugin.directMissionItemEditing ? 1 : 2
                        spacing:    _margins

                        Column {
                            id:      flightModeSettingsColumn
                            spacing: _margins
                            visible: sectionIdFilter === "" || sectionIdFilter === "Flight Modes"

                            QGCLabel {
                                id:             flightModeLabel
                                text:           qsTr("Flight Mode Settings")
                                font.bold:      true
                            }

                            Rectangle {
                                id:                 flightModeSettings
                                width:              flightModeColumn.width + (_margins * 2)
                                height:             flightModeColumn.height + ScreenTools.defaultFontPixelHeight
                                color:              qgcPal.windowShade

                                GridLayout {
                                    id:                 flightModeColumn
                                    anchors.margins:    ScreenTools.defaultFontPixelWidth
                                    anchors.left:       parent.left
                                    anchors.top:        parent.top
                                    rows:               7
                                    rowSpacing:         ScreenTools.defaultFontPixelWidth / 2
                                    columnSpacing:      rowSpacing
                                    flow:               GridLayout.TopToBottom

                                    QGCLabel {
                                        Layout.fillWidth:   true
                                        text:               qsTr("Mode Channel")
                                    }

                                    Repeater {
                                        model: 6

                                        QGCLabel {
                                            objectName:         "px4FlightModeSlot_" + (modelData + 1)
                                            Layout.fillWidth:   true
                                            text:               qsTr("Flight Mode %1").arg(modelData + 1)
                                            color:              (controller.activeFlightMode - 1) == index ? "yellow" : qgcPal.text
                                        }
                                    }

                                    FactComboBox {
                                        objectName:         "px4FlightModeChannel"
                                        Layout.fillWidth:   true
                                        fact:               controller.getParameterFact(-1, "RC_MAP_FLTMODE")
                                        model:              root.channelLabels(fact)
                                        indexModel:         false
                                        sizeToContents:     true
                                    }

                                    Repeater {
                                        model: 6

                                        FactComboBox {
                                            objectName:         "px4FlightModeValue_" + (modelData + 1)
                                            Layout.fillWidth:   true
                                            fact:               controller.getParameterFact(-1, "COM_FLTMODE" + (modelData + 1))
                                            model:              root.modeLabels(fact)
                                            indexModel:         false
                                            sizeToContents:     true
                                        }
                                    }
                                }
                            } // Rectangle - Flight Modes
                        } // Column - Flight mode settings

                        Column {
                            id:         column2
                            spacing:    _margins
                            visible:    sectionIdFilter === "" || sectionIdFilter === "Switch Settings"

                            QGCLabel {
                                text:           qsTr("Switch Settings")
                                font.bold:      true
                            }

                            Rectangle {
                                id:     switchSettingsRect
                                width:  switchSettingsGrid.width + (_margins * 2)
                                height: switchSettingsGrid.height + ScreenTools.defaultFontPixelHeight
                                color:  qgcPal.windowShade

                                GridLayout {
                                    id:                 switchSettingsGrid
                                    anchors.margins:    ScreenTools.defaultFontPixelWidth
                                    anchors.left:       parent.left
                                    anchors.top:        parent.top
                                    columns:            QGroundControl.corePlugin.directMissionItemEditing ? 1 : 2
                                    columnSpacing:      ScreenTools.defaultFontPixelWidth

                                    Repeater {
                                        id: switchRepeater

                                        RowLayout {
                                            spacing:            ScreenTools.defaultFontPixelWidth
                                            Layout.fillWidth:   true

                                            property string thFactName:     _switchTHList[index]
                                            property bool   thFactExists:   thFactName !== ""
                                            property Fact   swFact:         controller.getParameterFact(-1, modelData)
                                            property Fact   thFact:         thFactExists ? controller.getParameterFact(-1, thFactName) : null
                                            property real   thValue:        thFactExists ? thFact.rawValue : 0.5
                                            property real   thPWM:          1000 + (1000 * thValue)
                                            property int    swChannel:      swFact.rawValue - 1
                                            property bool   swActive:       swChannel < 0 ?
                                                                                false :
                                                                                (thValue >= 0 ?
                                                                                     (controller.rcChannelValues[swChannel] > thPWM) :
                                                                                     (controller.rcChannelValues[swChannel] <= thPWM))
                                            QGCLabel {
                                                text:               root._switchLabels[modelData] || swFact.shortDescription
                                                Layout.fillWidth:   true
                                                color:              swActive ? "yellow" : qgcPal.text
                                            }

                                            FactComboBox {
                                                objectName:             "px4SwitchChannel_" + modelData
                                                Layout.preferredWidth:  _channelComboWidth
                                                fact:                   swFact
                                                model:                  root.channelLabels(fact)
                                                indexModel:             false
                                            }
                                        }
                                    }
                                }
                            } // Rectangle

                            RCChannelMonitor {
                                objectName: "px4ModeChannelMonitor"
                                width:      switchSettingsRect.width
                                twoColumn:  true
                                channelValuesOverride: controller.rcChannelValues
                            }
                        } // Column - Switch settings
                    } // Row - Settings
                }
            }
        }
    }
}
