import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import QGroundControl
import QGroundControl.FactControls
import QGroundControl.Controls

SetupPage {
    id:             safetyPage
    pageComponent:  safetyPageComponent

    Component {
        id: safetyPageComponent

        Flow {
            id:         flowLayout
            width:      availableWidth
            spacing:    _margins

            FactPanelController { id: controller; }

            QGCPalette { id: ggcPal; colorGroupEnabled: true }

            property real _margins:     ScreenTools.defaultFontPixelHeight
            property bool _showIcon:    !ScreenTools.isTinyScreen

            property Fact _hSvMan:          controller.getParameterFact(-1, "H_SV_MAN")
            property Fact _hSwType:         controller.getParameterFact(-1, "H_SW_TYPE")
            property Fact _hSwColDir:       controller.getParameterFact(-1, "H_SW_COL_DIR")
            property Fact _hSwLinSvo:       controller.getParameterFact(-1, "H_SW_LIN_SVO")
            property Fact _hFlybarMode:     controller.getParameterFact(-1, "H_FLYBAR_MODE")
            property Fact _hCycMax:         controller.getParameterFact(-1, "H_CYC_MAX")
            property Fact _hColMax:         controller.getParameterFact(-1, "H_COL_MAX")
            property Fact _hColAngMax:      controller.getParameterFact(-1, "H_COL_ANG_MAX")
            property Fact _hColMin:         controller.getParameterFact(-1, "H_COL_MIN")
            property Fact _hColAngMin:      controller.getParameterFact(-1, "H_COL_ANG_MIN")
            property Fact _hColZeroThrst:   controller.getParameterFact(-1, "H_COL_ZERO_THRST")
            property Fact _hColLandMin:     controller.getParameterFact(-1, "H_COL_LAND_MIN")

            property Fact _hRscMode:        controller.getParameterFact(-1, "H_RSC_MODE")
            property Fact _hRscCritical:    controller.getParameterFact(-1, "H_RSC_CRITICAL")
            property Fact _hRscRampTime:    controller.getParameterFact(-1, "H_RSC_RAMP_TIME")
            property Fact _hRscRunupTime:   controller.getParameterFact(-1, "H_RSC_RUNUP_TIME")
            property Fact _hRscCldwnTime:   controller.getParameterFact(-1, "H_RSC_CLDWN_TIME")
            property Fact _hRscSetpoint:    controller.getParameterFact(-1, "H_RSC_SETPOINT")
            property Fact _hRscIdle:        controller.getParameterFact(-1, "H_RSC_IDLE")
            property Fact _hRscThrcrv0:     controller.getParameterFact(-1, "H_RSC_THRCRV_0")
            property Fact _hRscThrcrv25:    controller.getParameterFact(-1, "H_RSC_THRCRV_25")
            property Fact _hRscThrcrv50:    controller.getParameterFact(-1, "H_RSC_THRCRV_50")
            property Fact _hRscThrcrv75:    controller.getParameterFact(-1, "H_RSC_THRCRV_75")
            property Fact _hRscThrcrv100:   controller.getParameterFact(-1, "H_RSC_THRCRV_100")

            property Fact _hRscGovComp:     controller.getParameterFact(-1, "H_RSC_GOV_COMP")
            property Fact _hRscGovDroop:    controller.getParameterFact(-1, "H_RSC_GOV_DROOP")
            property Fact _hRscGovFf:       controller.getParameterFact(-1, "H_RSC_GOV_FF")
            property Fact _hRscGovRange:    controller.getParameterFact(-1, "H_RSC_GOV_RANGE")
            property Fact _hRscGovRpm:      controller.getParameterFact(-1, "H_RSC_GOV_RPM")
            property Fact _hRscGovTorque:   controller.getParameterFact(-1, "H_RSC_GOV_TORQUE")

            property Fact _imStbCol1:      controller.getParameterFact(-1, "IM_STB_COL_1")
            property Fact _imStbCol2:      controller.getParameterFact(-1, "IM_STB_COL_2")
            property Fact _imStbCol3:      controller.getParameterFact(-1, "IM_STB_COL_3")
            property Fact _imStbCol4:      controller.getParameterFact(-1, "IM_STB_COL_4")
            property Fact _hTailType:       controller.getParameterFact(-1, "H_TAIL_TYPE")
            property Fact _hTailSpeed:      controller.getParameterFact(-1, "H_TAIL_SPEED")
            property Fact _hGyrGain:        controller.getParameterFact(-1, "H_GYR_GAIN")
            property Fact _hGyrGainAcro:    controller.getParameterFact(-1, "H_GYR_GAIN_ACRO")
            property Fact _hColYaw:         controller.getParameterFact(-1, "H_COLYAW")

            QGCGroupBox {
                title: qsTr("Servo Setup")

                GridLayout {
                    columns: 6

                    QGCLabel { text: qsTr("Servo") }
                    QGCLabel { text: qsTr("Function") }
                    QGCLabel { text: qsTr("Min") }
                    QGCLabel { text: qsTr("Max") }
                    QGCLabel { text: qsTr("Trim") }
                    QGCLabel { text: qsTr("Reversed") }

                    QGCLabel { text: qsTr("1") }
                    FactComboBox {
                        fact:               controller.getParameterFact(-1, "SERVO1_FUNCTION")
                        indexModel:         false
                        Layout.fillWidth:   true
                    }
                    FactTextField {
                        fact:               controller.getParameterFact(-1, "SERVO1_MIN")
                        Layout.fillWidth:   true
                    }
                    FactTextField {
                        fact:               controller.getParameterFact(-1, "SERVO1_MAX")
                        Layout.fillWidth:   true
                    }
                    FactTextField {
                        fact:               controller.getParameterFact(-1, "SERVO1_TRIM")
                        Layout.fillWidth:   true
                    }
                    FactCheckBox {
                        fact:               controller.getParameterFact(-1, "SERVO1_REVERSED")
                        Layout.fillWidth:   true
                    }

                    QGCLabel { text: qsTr("2") }
                    FactComboBox {
                        fact:               controller.getParameterFact(-1, "SERVO2_FUNCTION")
                        indexModel:         false
                        Layout.fillWidth:   true
                    }
                    FactTextField {
                        fact:               controller.getParameterFact(-1, "SERVO2_MIN")
                        Layout.fillWidth:   true
                    }
                    FactTextField {
                        fact:               controller.getParameterFact(-1, "SERVO2_MAX")
                        Layout.fillWidth:   true
                    }
                    FactTextField {
                        fact:               controller.getParameterFact(-1, "SERVO2_TRIM")
                        Layout.fillWidth:   true
                    }
                    FactCheckBox {
                        fact:               controller.getParameterFact(-1, "SERVO2_REVERSED")
                        Layout.fillWidth:   true
                    }

                    QGCLabel { text: qsTr("3") }
                    FactComboBox {
                        fact:               controller.getParameterFact(-1, "SERVO3_FUNCTION")
                        indexModel:         false
                        Layout.fillWidth:   true
                    }
                    FactTextField {
                        fact:               controller.getParameterFact(-1, "SERVO3_MIN")
                        Layout.fillWidth:   true
                    }
                    FactTextField {
                        fact:               controller.getParameterFact(-1, "SERVO3_MAX")
                        Layout.fillWidth:   true
                    }
                    FactTextField {
                        fact:               controller.getParameterFact(-1, "SERVO3_TRIM")
                        Layout.fillWidth:   true
                    }
                    FactCheckBox {
                        fact:               controller.getParameterFact(-1, "SERVO3_REVERSED")
                        Layout.fillWidth:   true
                    }

                    QGCLabel { text: qsTr("4") }
                    FactComboBox {
                        fact:               controller.getParameterFact(-1, "SERVO4_FUNCTION")
                        indexModel:         false
                        Layout.fillWidth:   true
                    }
                    FactTextField {
                        fact:               controller.getParameterFact(-1, "SERVO4_MIN")
                        Layout.fillWidth:   true
                    }
                    FactTextField {
                        fact:               controller.getParameterFact(-1, "SERVO4_MAX")
                        Layout.fillWidth:   true
                    }
                    FactTextField {
                        fact:               controller.getParameterFact(-1, "SERVO4_TRIM")
                        Layout.fillWidth:   true
                    }
                    FactCheckBox {
                        fact:               controller.getParameterFact(-1, "SERVO4_REVERSED")
                        Layout.fillWidth:   true
                    }

                    QGCLabel { text: qsTr("5") }
                    FactComboBox {
                        fact:               controller.getParameterFact(-1, "SERVO5_FUNCTION")
                        indexModel:         false
                        Layout.fillWidth:   true
                    }
                    FactTextField {
                        fact:               controller.getParameterFact(-1, "SERVO5_MIN")
                        Layout.fillWidth:   true
                    }
                    FactTextField {
                        fact:               controller.getParameterFact(-1, "SERVO5_MAX")
                        Layout.fillWidth:   true
                    }
                    FactTextField {
                        fact:               controller.getParameterFact(-1, "SERVO5_TRIM")
                        Layout.fillWidth:   true
                    }
                    FactCheckBox {
                        fact:               controller.getParameterFact(-1, "SERVO5_REVERSED")
                        Layout.fillWidth:   true
                    }

                    QGCLabel { text: qsTr("6") }
                    FactComboBox {
                        fact:               controller.getParameterFact(-1, "SERVO6_FUNCTION")
                        indexModel:         false
                        Layout.fillWidth:   true
                    }
                    FactTextField {
                        fact:               controller.getParameterFact(-1, "SERVO6_MIN")
                        Layout.fillWidth:   true
                    }
                    FactTextField {
                        fact:               controller.getParameterFact(-1, "SERVO6_MAX")
                        Layout.fillWidth:   true
                    }
                    FactTextField {
                        fact:               controller.getParameterFact(-1, "SERVO6_TRIM")
                        Layout.fillWidth:   true
                    }
                    FactCheckBox {
                        fact:               controller.getParameterFact(-1, "SERVO6_REVERSED")
                        Layout.fillWidth:   true
                    }

                    QGCLabel { text: qsTr("7") }
                    FactComboBox {
                        fact:               controller.getParameterFact(-1, "SERVO7_FUNCTION")
                        indexModel:         false
                        Layout.fillWidth:   true
                    }
                    FactTextField {
                        fact:               controller.getParameterFact(-1, "SERVO7_MIN")
                        Layout.fillWidth:   true
                    }
                    FactTextField {
                        fact:               controller.getParameterFact(-1, "SERVO7_MAX")
                        Layout.fillWidth:   true
                    }
                    FactTextField {
                        fact:               controller.getParameterFact(-1, "SERVO7_TRIM")
                        Layout.fillWidth:   true
                    }
                    FactCheckBox {
                        fact:               controller.getParameterFact(-1, "SERVO7_REVERSED")
                        Layout.fillWidth:   true
                    }

                    QGCLabel { text: qsTr("8") }
                    FactComboBox {
                        fact:               controller.getParameterFact(-1, "SERVO8_FUNCTION")
                        indexModel:         false
                        Layout.fillWidth:   true
                    }
                    FactTextField {
                        fact:               controller.getParameterFact(-1, "SERVO8_MIN")
                        Layout.fillWidth:   true
                    }
                    FactTextField {
                        fact:               controller.getParameterFact(-1, "SERVO8_MAX")
                        Layout.fillWidth:   true
                    }
                    FactTextField {
                        fact:               controller.getParameterFact(-1, "SERVO8_TRIM")
                        Layout.fillWidth:   true
                    }
                    FactCheckBox {
                        fact:               controller.getParameterFact(-1, "SERVO8_REVERSED")
                        Layout.fillWidth:   true
                    }
                }
            }

            QGCGroupBox {
                title: qsTr("Swashplate Setup")

                GridLayout {
                    columns: 2

                    QGCLabel { text: OnlineMessageTranslator.parameterText(_hSvMan.shortDescription, OnlineMessageTranslator.revision) }
                    FactComboBox {
                        fact:       _hSvMan
                        indexModel: false
                    }

                    QGCLabel { text: OnlineMessageTranslator.parameterText(_hSwType.shortDescription, OnlineMessageTranslator.revision) }
                    FactComboBox {
                        fact:       _hSwType
                        indexModel: false
                    }

                    QGCLabel { text: OnlineMessageTranslator.parameterText(_hSwColDir.shortDescription, OnlineMessageTranslator.revision) }
                    FactComboBox {
                        fact:       _hSwColDir
                        indexModel: false
                    }

                    QGCLabel { text: OnlineMessageTranslator.parameterText(_hSwLinSvo.shortDescription, OnlineMessageTranslator.revision) }
                    FactComboBox {
                        fact:       _hSwLinSvo
                        indexModel: false
                    }

                    QGCLabel { text: OnlineMessageTranslator.parameterText(_hFlybarMode.shortDescription, OnlineMessageTranslator.revision) }
                    FactComboBox {
                        fact:       _hFlybarMode
                        indexModel: false
                    }

                    QGCLabel { text: OnlineMessageTranslator.parameterText(_hCycMax.shortDescription, OnlineMessageTranslator.revision) }
                    FactTextField { fact: _hCycMax }

                    QGCLabel { text: OnlineMessageTranslator.parameterText(_hColMax.shortDescription, OnlineMessageTranslator.revision) }
                    FactTextField { fact: _hColMax }

                    QGCLabel { text: OnlineMessageTranslator.parameterText(_hColAngMax.shortDescription, OnlineMessageTranslator.revision) }
                    FactTextField { fact: _hColAngMax }

                    QGCLabel { text: OnlineMessageTranslator.parameterText(_hColMin.shortDescription, OnlineMessageTranslator.revision) }
                    FactTextField { fact: _hColMin }

                    QGCLabel { text: OnlineMessageTranslator.parameterText(_hColAngMin.shortDescription, OnlineMessageTranslator.revision) }
                    FactTextField { fact: _hColAngMin }

                    QGCLabel { text: OnlineMessageTranslator.parameterText(_hColZeroThrst.shortDescription, OnlineMessageTranslator.revision) }
                    FactTextField { fact: _hColZeroThrst }

                    QGCLabel { text: OnlineMessageTranslator.parameterText(_hColLandMin.shortDescription, OnlineMessageTranslator.revision) }
                    FactTextField { fact: _hColLandMin }
                }
            }

            QGCGroupBox {
                title: qsTr("Throttle Settings")

                GridLayout {
                    columns: 2

                    QGCLabel { text: OnlineMessageTranslator.parameterText(_hRscMode.shortDescription, OnlineMessageTranslator.revision) }
                    FactComboBox {
                        fact:       _hRscMode
                        indexModel: false
                    }

                    QGCLabel { text: OnlineMessageTranslator.parameterText(_hRscCritical.shortDescription, OnlineMessageTranslator.revision) }
                    FactTextField { fact: _hRscCritical }

                    QGCLabel { text: OnlineMessageTranslator.parameterText(_hRscRampTime.shortDescription, OnlineMessageTranslator.revision) }
                    FactTextField { fact: _hRscRampTime }

                    QGCLabel { text: OnlineMessageTranslator.parameterText(_hRscRunupTime.shortDescription, OnlineMessageTranslator.revision) }
                    FactTextField { fact: _hRscRunupTime }

                    QGCLabel { text: OnlineMessageTranslator.parameterText(_hRscCldwnTime.shortDescription, OnlineMessageTranslator.revision) }
                    FactTextField { fact: _hRscCldwnTime }

                    QGCLabel { text: OnlineMessageTranslator.parameterText(_hRscSetpoint.shortDescription, OnlineMessageTranslator.revision) }
                    FactTextField { fact: _hRscSetpoint }

                    QGCLabel { text: OnlineMessageTranslator.parameterText(_hRscIdle.shortDescription, OnlineMessageTranslator.revision) }
                    FactTextField { fact: _hRscIdle }

                    QGCLabel { text: OnlineMessageTranslator.parameterText(_hRscThrcrv0.shortDescription, OnlineMessageTranslator.revision) }
                    FactTextField { fact: _hRscThrcrv0 }

                    QGCLabel { text: OnlineMessageTranslator.parameterText(_hRscThrcrv25.shortDescription, OnlineMessageTranslator.revision) }
                    FactTextField { fact: _hRscThrcrv25 }

                    QGCLabel { text: OnlineMessageTranslator.parameterText(_hRscThrcrv50.shortDescription, OnlineMessageTranslator.revision) }
                    FactTextField { fact: _hRscThrcrv50 }

                    QGCLabel { text: OnlineMessageTranslator.parameterText(_hRscThrcrv75.shortDescription, OnlineMessageTranslator.revision) }
                    FactTextField { fact: _hRscThrcrv75 }

                    QGCLabel { text: OnlineMessageTranslator.parameterText(_hRscThrcrv100.shortDescription, OnlineMessageTranslator.revision) }
                    FactTextField { fact: _hRscThrcrv100 }
                }
            }

            QGCGroupBox {
                title: qsTr("Governor Settings")

                GridLayout {
                    columns: 2

                    QGCLabel { text: OnlineMessageTranslator.parameterText(_hRscGovComp.shortDescription, OnlineMessageTranslator.revision) }
                    FactTextField { fact: _hRscGovComp }

                    QGCLabel { text: OnlineMessageTranslator.parameterText(_hRscGovDroop.shortDescription, OnlineMessageTranslator.revision) }
                    FactTextField { fact: _hRscGovDroop }

                    QGCLabel { text: OnlineMessageTranslator.parameterText(_hRscGovFf.shortDescription, OnlineMessageTranslator.revision) }
                    FactTextField { fact: _hRscGovFf }

                    QGCLabel { text: OnlineMessageTranslator.parameterText(_hRscGovRange.shortDescription, OnlineMessageTranslator.revision) }
                    FactTextField { fact: _hRscGovRange }

                    QGCLabel { text: OnlineMessageTranslator.parameterText(_hRscGovRpm.shortDescription, OnlineMessageTranslator.revision) }
                    FactTextField { fact: _hRscGovRpm }

                    QGCLabel { text: OnlineMessageTranslator.parameterText(_hRscGovTorque.shortDescription, OnlineMessageTranslator.revision) }
                    FactTextField { fact: _hRscGovTorque }
                }
            }

            QGCGroupBox {
                title: qsTr("Miscellaneous Settings")

                GridLayout {
                    columns: 2

                    QGCLabel { text: qsTr("* Stabilize Collective Curve *") }
                    QGCLabel { text: qsTr("") }

                    QGCLabel { text: OnlineMessageTranslator.parameterText(_imStbCol1.shortDescription, OnlineMessageTranslator.revision) }
                    FactTextField { fact: _imStbCol1 }

                    QGCLabel { text: OnlineMessageTranslator.parameterText(_imStbCol2.shortDescription, OnlineMessageTranslator.revision) }
                    FactTextField { fact: _imStbCol2 }

                    QGCLabel { text: OnlineMessageTranslator.parameterText(_imStbCol3.shortDescription, OnlineMessageTranslator.revision) }
                    FactTextField { fact: _imStbCol3 }

                    QGCLabel { text: OnlineMessageTranslator.parameterText(_imStbCol4.shortDescription, OnlineMessageTranslator.revision) }
                    FactTextField { fact: _imStbCol4 }

                    QGCLabel { text: qsTr("* Tail & Gyros *") }
                    QGCLabel { text: qsTr("") }

                    QGCLabel { text: OnlineMessageTranslator.parameterText(_hTailType.shortDescription, OnlineMessageTranslator.revision) }
                    FactComboBox {
                        fact:       _hTailType
                        indexModel: false
                    }

                    QGCLabel { text: OnlineMessageTranslator.parameterText(_hTailSpeed.shortDescription, OnlineMessageTranslator.revision) }
                    FactTextField { fact: _hTailSpeed }

                    QGCLabel { text: OnlineMessageTranslator.parameterText(_hGyrGain.shortDescription, OnlineMessageTranslator.revision) }
                    FactTextField { fact: _hGyrGain }

                    QGCLabel { text: OnlineMessageTranslator.parameterText(_hGyrGainAcro.shortDescription, OnlineMessageTranslator.revision) }
                    FactTextField { fact: _hGyrGainAcro }

                    QGCLabel { text: OnlineMessageTranslator.parameterText(_hColYaw.shortDescription, OnlineMessageTranslator.revision) }
                    FactTextField { fact: _hColYaw }
                }
            }
        } // Flow
    } // Component
} // SetupView
