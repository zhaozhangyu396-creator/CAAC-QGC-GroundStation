import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Dialogs

import QGroundControl
import QGroundControl.Controls
import QGroundControl.FlyView

Item {
    required property var guidedValueSlider

    id:     control
    width:  parent.width
    height: _statusHeight

    readonly property real _statusHeight: ScreenTools.toolbarHeight
    property alias flightToolStrip: flightActions
    property bool flightSettingsExpanded: false
    signal displayPreFlightChecklist

    property var    _activeVehicle:     QGroundControl.multiVehicleManager.activeVehicle
    property bool   _communicationLost: _activeVehicle ? _activeVehicle.vehicleLinkManager.communicationLost : false
    property color  _mainStatusBGColor: qgcPal.brandPrimary
    property real   _leftRightMargin:   ScreenTools.defaultFontPixelWidth * 0.75
    readonly property real _margins: ScreenTools.defaultFontPixelWidth / 2
    property var    _guidedController:  globals.guidedControllerFlyView

    function dropMainStatusIndicatorTool() {
        mainStatusIndicator.dropMainStatusIndicator();
    }

    QGCPalette { id: qgcPal }

    QGCFlickable {
        anchors.left:       parent.left
        anchors.right:      parent.right
        anchors.top:        parent.top
        height:             control._statusHeight
        contentWidth:       Math.min(toolBarLayout.width, width)
        flickableDirection: Flickable.HorizontalFlick
        interactive:        false

        Row {
            id:         toolBarLayout
            height:     parent.height
            spacing:    0

            Item {
                id:     leftPanel
                width:  Math.min(leftPanelLayout.implicitWidth, control.width * 0.32)
                height: parent.height

                Rectangle {
                    anchors.fill: parent
                    color:          qgcPal.window
                }

                RowLayout {
                    id:         leftPanelLayout
                    height:     parent.height
                    spacing:    ScreenTools.defaultFontPixelWidth * 2
                    scale:      Math.min(1, leftPanel.width / Math.max(1, implicitWidth))
                    transformOrigin: Item.Left

                    RowLayout {
                        id:         mainStatusLayout
                        height:     parent.height
                        spacing:    0

                        QGCToolBarButton {
                            id:                 qgcButton
                            objectName:         "toolbar_qgcLogo"
                            Layout.fillHeight:  true
                            icon.source:        "/res/gear-white.svg"
                            logo:               false
                            onClicked:          mainWindow.showToolSelectDialog()
                        }

                        MainStatusIndicator {
                            id:                 mainStatusIndicator
                            objectName:         "toolbar_mainStatusIndicator"
                            Layout.fillHeight:  true
                            visible: !QGroundControl.corePlugin.directMissionItemEditing
                        }
                    }

                    QGCButton {
                        id:         disconnectButton
                        text:       qsTr("Disconnect")
                        pointSize:  ScreenTools.menuFontPointSize
                        onClicked:  _activeVehicle.closeVehicle()
                        visible:    _activeVehicle && _communicationLost
                    }

                    FlightModeIndicator {
                        objectName:         "toolbar_flightModeIndicator"
                        Layout.fillHeight:  true
                        toolbarButtonStyle: QGroundControl.corePlugin.directMissionItemEditing
                        visible: QGroundControl.corePlugin.directMissionItemEditing || !!_activeVehicle
                    }
                }
            }
            Item {
                id:     centerPanel
                // center panel takes up all remaining space in toolbar between left and right panels
                width:  Math.max(0, control.width - (leftPanel.width + rightPanel.width))
                height: parent.height

                Rectangle {
                    anchors.fill:   parent
                    color:          qgcPal.window
                }

                GuidedActionConfirm {
                    id:                         guidedActionConfirm
                    height:                     parent.height
                    anchors.horizontalCenter:   parent.horizontalCenter
                    guidedController:           control._guidedController
                    registrationEnabled:        control.visible
                    guidedValueSlider:          control.guidedValueSlider
                    messageDisplay:             guidedActionMessageDisplay
                }
            }

            Item {
                id:     rightPanel
                width:  Math.min(flyViewIndicators.implicitWidth, control.width * 0.28)
                height: parent.height

                Rectangle {
                    anchors.fill:   parent
                    color:          qgcPal.window
                }

                FlyViewToolBarIndicators {
                    id:     flyViewIndicators
                    height: parent.height * 0.72
                    anchors.verticalCenter: parent.verticalCenter
                    anchors.right: parent.right
                    scale: Math.min(1, rightPanel.width / Math.max(1, implicitWidth))
                    transformOrigin: Item.Right
                }
            }
        }
    }

    Item {
        id: flightActionHost
        objectName: "caacTopFlightActions"
        visible: !guidedActionConfirm.visible
        x: leftPanel.width
        y: 0
        width: Math.max(0, control.width - leftPanel.width - rightPanel.width)
        height: control._statusHeight

        Rectangle {
            anchors.fill: parent
            color: qgcPal.window
        }

        RowLayout {
            anchors.verticalCenter: parent.verticalCenter
            height: parent.height
            scale: Math.min(1, parent.width / Math.max(1, implicitWidth))
            transformOrigin: Item.Left
            spacing: ScreenTools.defaultFontPixelWidth * 0.5
            QGCButton {
                objectName: "caacFlightSettingsButton"
                text: qsTr("Flight settings")
                checked: control.flightSettingsExpanded
                checkable: true
                heightFactor: 0.25
                backgroundColor: checked ? qgcPal.colorGreen : qgcPal.window
                textColor: qgcPal.text
                onClicked: control.flightSettingsExpanded = checked
            }
            QGCButton {
                objectName: "caacArmMenuButton"
                heightFactor: 0.25
                backgroundColor: qgcPal.window
                textColor: qgcPal.text
                text: control._activeVehicle && control._activeVehicle.armed ? qsTr("Disarm") : qsTr("Arm")
                enabled: !!control._activeVehicle
                onClicked: mainStatusIndicator.dropStatusSection(1, this)
            }
            QGCButton {
                objectName: "caacVehicleMessagesButton"
                heightFactor: 0.25
                backgroundColor: qgcPal.window
                textColor: qgcPal.text
                text: qsTr("Messages")
                enabled: !!control._activeVehicle
                onClicked: mainStatusIndicator.dropStatusSection(2, this)
            }
            QGCButton {
                objectName: "caacSensorStatusButton"
                heightFactor: 0.25
                backgroundColor: qgcPal.window
                textColor: qgcPal.text
                text: qsTr("Sensor status")
                enabled: !!control._activeVehicle
                onClicked: mainStatusIndicator.dropStatusSection(3, this)
            }
            RowLayout {
                spacing: ScreenTools.defaultFontPixelWidth * 0.5
                QGCColoredImage {
                    source: "/InstrumentValueIcons/timer.svg"
                    color: qgcPal.text
                    Layout.preferredWidth: ScreenTools.defaultFontPixelHeight
                    Layout.preferredHeight: Layout.preferredWidth
                    fillMode: Image.PreserveAspectFit
                }
                QGCLabel {
                    objectName: "caacTopFlightTime"
                    readonly property var flightTime: control._activeVehicle ? control._activeVehicle.getFact("flightTime") : null
                    text: flightTime ? flightTime.valueString : "00:00:00"
                    color: qgcPal.text
                    Accessible.name: qsTr("Flight time") + ": " + text
                }
            }
            QGCButton {
                id: connectionButton
                objectName: "caacConnectionStatusButton"
                readonly property bool connected: !!control._activeVehicle && !control._communicationLost
                heightFactor: 0.25
                backgroundColor: qgcPal.window
                text: connected ? qsTr("Connected") : qsTr("Not connected")
                ToolTip.visible: hovered
                ToolTip.text: qsTr("Open connection settings")
                contentItem: RowLayout {
                    spacing: ScreenTools.defaultFontPixelWidth * 0.5
                    Item {
                        Layout.preferredWidth: ScreenTools.defaultFontPixelHeight * 1.2
                        Layout.preferredHeight: Layout.preferredWidth
                        QGCColoredImage {
                            anchors.fill: parent
                            source: "/InstrumentValueIcons/link.svg"
                            color: qgcPal.text
                            fillMode: Image.PreserveAspectFit
                        }
                        Canvas {
                            anchors.fill: parent
                            visible: !connectionButton.connected
                            readonly property color lineColor: qgcPal.text
                            readonly property color coverColor: qgcPal.window
                            onLineColorChanged: requestPaint()
                            onCoverColorChanged: requestPaint()
                            onPaint: {
                                const ctx = getContext("2d")
                                ctx.reset()
                                ctx.beginPath()
                                ctx.moveTo(width * 0.1, height * 0.1)
                                ctx.lineTo(width * 0.9, height * 0.9)
                                ctx.strokeStyle = coverColor
                                ctx.lineWidth = width * 0.22
                                ctx.stroke()
                                ctx.strokeStyle = lineColor
                                ctx.lineWidth = width * 0.1
                                ctx.stroke()
                            }
                        }
                    }
                    QGCLabel { text: connectionButton.text; color: qgcPal.text }
                }
                onClicked: {
                    if (mainWindow.allowViewSwitch()) {
                        mainWindow.closeIndicatorDrawer()
                        mainWindow.showSettingsTool("Comm Links")
                    }
                }
            }
        }
    }

    Rectangle {
        objectName: "caacFlightSettingsPanel"
        parent: control.parent
        visible: control.visible && control.flightSettingsExpanded && !guidedActionConfirm.visible
        x: Math.max(control._margins, (parent.width - width) / 2)
        y: control.y + control.height + ScreenTools.defaultFontPixelHeight * 3.5
        width: flightActions.width + control._margins * 2
        height: flightActions.height + control._margins * 2
        radius: ScreenTools.defaultBorderRadius
        color: qgcPal.window
        z: QGroundControl.zOrderWidgets + 20
        FlyViewToolStrip {
            id: flightActions
            objectName: "flyViewToolStrip"
            horizontal: true
            horizontalColumns: 4
            squareButtons: QGroundControl.corePlugin.directMissionItemEditing
            x: control._margins
            y: control._margins
            maxWidth: Math.min(control.parent.width - control._margins * 4, ScreenTools.defaultFontPixelHeight * 16)
            maxHeight: ScreenTools.defaultFontPixelHeight * 8
            roundButtonHeight: ScreenTools.defaultFontPixelHeight * 3.4
            roundButtonMaximumDiameter: ScreenTools.defaultFontPixelHeight * 2.3
            compactLabels: true
            onActionTriggered: control.flightSettingsExpanded = false
            onDisplayPreFlightChecklist: control.displayPreFlightChecklist()
        }
    }

    // The guided action message display is outside of the GuidedActionConfirm control so that it doesn't end up as
    // part of the Flickable
    Rectangle {
        id:                         guidedActionMessageDisplay
        anchors.top:                control.bottom
        anchors.topMargin:          _margins
        x:                          control.mapFromItem(guidedActionConfirm.parent, guidedActionConfirm.x, 0).x + (guidedActionConfirm.width - guidedActionMessageDisplay.width) / 2
        width:                      messageLabel.contentWidth + (_margins * 2)
        height:                     messageLabel.contentHeight + (_margins * 2)
        color:                      qgcPal.window
        radius:                     ScreenTools.defaultBorderRadius
        visible:                    guidedActionConfirm.visible

        QGCLabel {
            id:         messageLabel
            x:          _margins
            y:          _margins
            width:      ScreenTools.defaultFontPixelWidth * 30
            wrapMode:   Text.WordWrap
            text:       guidedActionConfirm.message
        }

        PropertyAnimation {
            id:         messageOpacityAnimation
            target:     guidedActionMessageDisplay
            property:   "opacity"
            from:       1
            to:         0
            duration:   500
        }

        Timer {
            id:             messageFadeTimer
            interval:       4000
            onTriggered:    messageOpacityAnimation.start()
        }
    }

    ParameterDownloadProgress {
        anchors.fill: parent
    }
}
