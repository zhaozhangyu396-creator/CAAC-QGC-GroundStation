import QtQuick
import QtQuick.Controls
import QtQuick.Dialogs
import QtQuick.Layouts

import QGroundControl
import QGroundControl.FactControls
import QGroundControl.Controls
import QGroundControl.VehicleSetup

SetupPage {
    id: radioPage
    pageComponent: pageComponent

    readonly property var _additionalChannelLabels: ({
        "RC_MAP_AUX1": qsTr("AUX1 Passthrough RC channel"),
        "RC_MAP_AUX2": qsTr("AUX2 Passthrough RC channel"),
        "RC_MAP_PARAM1": qsTr("PARAM1 tuning channel"),
        "RC_MAP_PARAM2": qsTr("PARAM2 tuning channel"),
        "RC_MAP_PARAM3": qsTr("PARAM3 tuning channel"),
        "RC_MAP_PAY_SW": qsTr("Payload Power Switch RC channel"),
        "RC_MAP_FLAPS": qsTr("Flaps channel")
    })

    function channelLabels(fact) {
        if (!fact) { return [] }
        return fact.enumStrings.map(function(label) {
            if (label === "Unassigned") {
                return qsTr("Unassigned")
            }
            const channel = /^Channel (\d+)$/.exec(label)
            return channel ? qsTr("Channel %1").arg(channel[1]) : label
        })
    }

    Component {
        id: pageComponent

        RemoteControlCalibration {
            id: remoteControlCalibration

            useDeadband: false

            controller: RadioComponentController {
                statusText: remoteControlCalibration.statusText
                cancelButton: remoteControlCalibration.cancelButton
                nextButton: remoteControlCalibration.nextButton
                joystickMode: false

                onThrottleReversedCalFailure: QGroundControl.showMessageDialog(radioPage, qsTr("Throttle channel reversed"), qsTr("Calibration failed. The throttle channel on your transmitter is reversed. You must correct this on your transmitter in order to complete calibration."))
            }

            Component.onCompleted: controller.start()

            additionalSetupComponent: ColumnLayout {
                Layout.minimumWidth: remoteControlCalibration.vehicleSetupStyle ? 0 : -1
                spacing: ScreenTools.defaultFontPixelHeight / 2

                ColumnLayout {
                    id: switchSettings
                    Layout.fillWidth: true
                    Layout.minimumWidth: remoteControlCalibration.vehicleSetupStyle ? 0 : -1

                    Repeater {
                        model: QGroundControl.multiVehicleManager.activeVehicle.px4Firmware ?
                                    (QGroundControl.multiVehicleManager.activeVehicle.multiRotor ?
                                        [ "RC_MAP_AUX1", "RC_MAP_AUX2", "RC_MAP_PARAM1", "RC_MAP_PARAM2", "RC_MAP_PARAM3", "RC_MAP_PAY_SW"] :
                                        [ "RC_MAP_FLAPS", "RC_MAP_AUX1", "RC_MAP_AUX2", "RC_MAP_PARAM1", "RC_MAP_PARAM2", "RC_MAP_PARAM3", "RC_MAP_PAY_SW"]) :
                                    0

                        LabelledFactComboBox {
                            objectName: "radioAdditionalChannel_" + modelData
                            Layout.fillWidth: true
                            label: radioPage._additionalChannelLabels[modelData] || OnlineMessageTranslator.parameterText(fact.shortDescription, OnlineMessageTranslator.revision)
                            fact: controller.getParameterFact(-1, modelData)
                            model: radioPage.channelLabels(fact)
                            indexModel: false
                        }
                    }
                }

                Rectangle {
                    Layout.fillWidth: true
                    implicitHeight: 1
                    color: qgcPal.text
                }

                GridLayout {
                    Layout.fillWidth: true
                    columns: ScreenTools.isVehicleSetupItem(remoteControlCalibration) ? 1 : 3
                    rowSpacing: ScreenTools.defaultFontPixelWidth
                    columnSpacing: ScreenTools.defaultFontPixelWidth

                    QGCButton {
                        id: bindButton
                        text: qsTr("Spektrum Bind")
                        onClicked: spektrumBindDialogFactory.open()
                    }

                    QGCButton {
                        text: qsTr("CRSF Bind")
                        onClicked: QGroundControl.showMessageDialog(radioPage, qsTr("CRSF Bind"),
                                                                qsTr("Click Ok to place your CRSF receiver in the bind mode."),
                                                                Dialog.Ok | Dialog.Cancel,
                                                                function() { controller.crsfBindMode() })
                    }

                    QGCButton {
                        text: qsTr("Copy Trims")
                        onClicked: QGroundControl.showMessageDialog(radioPage, qsTr("Copy Trims"),
                                                                qsTr("Center your sticks and move throttle all the way down, then press Ok to copy trims. After pressing Ok, reset the trims on your radio back to zero."),
                                                                Dialog.Ok | Dialog.Cancel,
                                                                function() { controller.copyTrims() })
                    }
                }

                QGCPopupDialogFactory {
                    id: spektrumBindDialogFactory

                    dialogComponent: spektrumBindDialogComponent
                }

                Component {
                    id: spektrumBindDialogComponent

                    QGCPopupDialog {
                        title: qsTr("Spektrum Bind")
                        buttons: Dialog.Ok | Dialog.Cancel

                        onAccepted: { controller.spektrumBindMode(radioGroup.checkedButton.bindMode) }

                        ButtonGroup { id: radioGroup }

                        ColumnLayout {
                            spacing: ScreenTools.defaultFontPixelHeight / 2

                            QGCLabel {
                                wrapMode: Text.WordWrap
                                text: qsTr("Click Ok to place your Spektrum receiver in the bind mode.")
                            }

                            QGCLabel {
                                wrapMode: Text.WordWrap
                                text: qsTr("Select the specific receiver type below:")
                            }

                            QGCRadioButton {
                                text: qsTr("DSM2 Mode")
                                ButtonGroup.group: radioGroup
                                property int bindMode: RadioComponentController.DSM2
                            }

                            QGCRadioButton {
                                text: qsTr("DSMX (7 channels or less)")
                                ButtonGroup.group: radioGroup
                                property int bindMode: RadioComponentController.DSMX7
                            }

                            QGCRadioButton {
                                checked: true
                                text: qsTr("DSMX (8 channels or more)")
                                ButtonGroup.group: radioGroup
                                property int bindMode: RadioComponentController.DSMX8
                            }
                        }
                    }
                }
            }
        }
    }
}
