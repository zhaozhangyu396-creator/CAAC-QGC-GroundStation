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
    readonly property bool _compactToolbar: QGroundControl.corePlugin.directMissionItemEditing
    readonly property real _toolbarScale: _compactToolbar ? 1.15 : 1
    readonly property real _iconExtent: ScreenTools.defaultFontPixelHeight * (_compactToolbar ? 0.85 : 1) * _toolbarScale
    readonly property real _fontPointSize: (_compactToolbar ? ScreenTools.smallFontPointSize : ScreenTools.defaultFontPointSize) * _toolbarScale
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

    component LinkStrength: RowLayout {
        id: indicator
        required property string iconSource
        required property string label
        property bool available: false
        property real percent: 0
        property string valueText: "--"
        spacing: ScreenTools.defaultFontPixelWidth * 0.55
        Accessible.name: label + ": " + valueText
        QGCColoredImage {
            source: indicator.iconSource
            color: qgcPal.text
            Layout.preferredWidth: control._iconExtent
            Layout.preferredHeight: Layout.preferredWidth
            fillMode: Image.PreserveAspectFit
        }
        SignalStrength {
            size: control._iconExtent
            percent: indicator.available ? indicator.percent : 0
            opacity: indicator.available ? 1 : 0.5
        }
        QGCLabel {
            objectName: indicator.objectName + "Value"
            text: indicator.valueText
            font.pointSize: control._fontPointSize
        }
    }

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
                    spacing:    ScreenTools.defaultFontPixelWidth * (control._compactToolbar ? 0.35 : 2)
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
                            iconExtent:         control._compactToolbar ? control._iconExtent : Math.min(ScreenTools.defaultFontPixelHeight * 2, height * 0.75)
                            _horizontalMargin:  ScreenTools.defaultFontPixelWidth * (control._compactToolbar ? 0.6 : 1)
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
                        toolbarScale: control._toolbarScale
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
                width:  Math.min(rightIndicators.implicitWidth, control.width * 0.48)
                height: parent.height

                Rectangle {
                    anchors.fill:   parent
                    color:          qgcPal.window
                }

                RowLayout {
                    id: rightIndicators
                    height: parent.height * 0.72
                    anchors.verticalCenter: parent.verticalCenter
                    anchors.right: parent.right
                    scale: Math.min(1, rightPanel.width / Math.max(1, implicitWidth))
                    transformOrigin: Item.Right
                    spacing: ScreenTools.defaultFontPixelWidth * (control._compactToolbar ? 1.25 : 1)
                    RowLayout {
                        spacing: ScreenTools.defaultFontPixelWidth * 0.7
                        QGCColoredImage {
                            source: "/InstrumentValueIcons/timer.svg"
                            color: qgcPal.text
                            Layout.preferredWidth: control._iconExtent
                            Layout.preferredHeight: Layout.preferredWidth
                            fillMode: Image.PreserveAspectFit
                        }
                        QGCLabel {
                            objectName: "caacTopFlightTime"
                            readonly property var flightTime: control._activeVehicle ? control._activeVehicle.getFact("flightTime") : null
                            text: flightTime ? flightTime.valueString : "00:00:00"
                            color: qgcPal.text
                            font.pointSize: control._fontPointSize
                            Accessible.name: qsTr("Flight time") + ": " + text
                        }
                    }
                    QGCButton {
                        id: connectionButton
                        objectName: "caacConnectionStatusButton"
                        readonly property bool connected: !!control._activeVehicle && !control._communicationLost
                        heightFactor: 0.25
                        _horizontalPadding: ScreenTools.defaultFontPixelWidth * (control._compactToolbar ? 0.6 : 2)
                        implicitWidth: contentItem.implicitWidth + leftPadding + rightPadding
                        backgroundColor: qgcPal.window
                        text: connected ? qsTr("Connected") : qsTr("Not connected")
                        ToolTip.visible: hovered
                        ToolTip.text: qsTr("Open connection settings")
                        contentItem: RowLayout {
                            spacing: ScreenTools.defaultFontPixelWidth * 0.7
                            Item {
                                Layout.preferredWidth: control._iconExtent
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
                            QGCLabel { text: connectionButton.text; color: qgcPal.text; font.pointSize: control._fontPointSize }
                        }
                        onClicked: {
                            if (mainWindow.allowViewSwitch()) {
                                mainWindow.showToolSelectDialog("Connection")
                            }
                        }
                    }
                    LinkStrength {
                        objectName: "caacRCRSSIIndicator"
                        visible: QGroundControl.corePlugin.directMissionItemEditing
                        iconSource: "/qmlimages/RC.svg"
                        label: qsTr("RC signal strength")
                        readonly property var fact: control._activeVehicle ? control._activeVehicle.rcRSSI : null
                        available: !!fact && !control._communicationLost && fact.rawValue >= 0 && fact.rawValue <= 100
                        percent: available ? Number(fact.rawValue) : 0
                        valueText: available ? Math.round(percent) + "%" : "--"
                    }
                    LinkStrength {
                        objectName: "caacTelemetryRSSIIndicator"
                        visible: QGroundControl.corePlugin.directMissionItemEditing
                        iconSource: "/qmlimages/TelemRSSI.svg"
                        label: qsTr("Telemetry signal strength")
                        readonly property var radio: control._activeVehicle ? control._activeVehicle.radioStatus : null
                        available: !!radio && radio.telemetryAvailable && !control._communicationLost && isFinite(Number(radio.lrssi.rawValue))
                        percent: available ? Math.max(0, Math.min(100, (Number(radio.lrssi.rawValue) + 100) * 2)) : 0
                        valueText: available ? radio.lrssi.valueString + " " + radio.lrssi.units : "--"
                    }
                    FlyViewToolBarIndicators {
                        id: flyViewIndicators
                        Layout.fillHeight: true
                        hideSignalStrengthIndicators: QGroundControl.corePlugin.directMissionItemEditing
                    }
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
                id: flightSettingsButton
                objectName: "caacFlightSettingsButton"
                text: qsTr("Flight settings")
                checked: control.flightSettingsExpanded
                checkable: true
                heightFactor: 0.25
                pointSize: control._fontPointSize
                _horizontalPadding: ScreenTools.defaultFontPixelWidth * (control._compactToolbar ? 0.6 : 2)
                implicitWidth: contentItem.implicitWidth + leftPadding + rightPadding
                backgroundColor: checked ? qgcPal.colorGreen : qgcPal.window
                textColor: qgcPal.text
                onClicked: control.flightSettingsExpanded = checked
            }
            RowLayout {
                objectName: "caacArmStatusIndicator"
                spacing: ScreenTools.defaultFontPixelWidth * 0.35
                Accessible.name: qsTr("Propeller lock status") + ": " + armStatus.text
                QGCColoredImage {
                    visible: !!control._activeVehicle
                    source: control._activeVehicle && control._activeVehicle.armed ? "/qmlimages/Armed.svg" : "/qmlimages/Disarmed.svg"
                    objectName: "caacArmStatusIcon"
                    color: !control._activeVehicle ? qgcPal.text : control._activeVehicle.armed ? qgcPal.colorGreen : qgcPal.colorRed
                    Layout.preferredWidth: control._iconExtent
                    Layout.preferredHeight: Layout.preferredWidth
                    fillMode: Image.PreserveAspectFit
                }
                QGCLabel {
                    id: armStatus
                    objectName: "caacArmStatusValue"
                    font.pointSize: control._fontPointSize
                    text: !control._activeVehicle ? "--" : control._activeVehicle.armed ? qsTr("Armed") : qsTr("Disarmed")
                }
            }
            QGCButton {
                objectName: "caacSensorStatusButton"
                heightFactor: 0.25
                pointSize: control._fontPointSize
                _horizontalPadding: ScreenTools.defaultFontPixelWidth * (control._compactToolbar ? 0.6 : 2)
                implicitWidth: contentItem.implicitWidth + leftPadding + rightPadding
                backgroundColor: qgcPal.window
                textColor: qgcPal.text
                text: qsTr("Sensor status")
                enabled: !!control._activeVehicle
                onClicked: mainStatusIndicator.dropStatusSection(3, this)
            }

        }
    }

    Rectangle {
        objectName: "caacFlightSettingsPanel"
        parent: control.parent
        visible: control.visible && control.flightSettingsExpanded && !guidedActionConfirm.visible
        x: Math.max(control._margins, Math.min(flightSettingsButton.mapToItem(parent, 0, 0).x,
            parent.width - width - control._margins))
        y: control.mapToItem(parent, 0, control.height).y + control._margins
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
            maxHeight: Math.max(0, control.parent.height - parent.y - control._margins * 3)
            roundButtonHeight: ScreenTools.defaultFontPixelHeight * 3.9
            roundButtonMaximumDiameter: ScreenTools.defaultFontPixelHeight * 2.3
            compactLabels: true
            onActionTriggered: { if (!QGroundControl.corePlugin.directMissionItemEditing) { control.flightSettingsExpanded = false } }
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
