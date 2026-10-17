import QtQuick
import QtQuick.Controls
import QtQuick.Dialogs
import QtQuick.Layouts

import QGroundControl
import QGroundControl.FactControls
import QGroundControl.Controls

import "qrc:/qml/QGroundControl/Controls/AirframeCatalog.js" as AirframeCatalog

SetupPage {
    id:             airframePage
    objectName:     "apmAirframePage"
    pageComponent:  pageComponent
    property string category: ""

    Component {
        id: pageComponent

        ColumnLayout {
            id:     mainColumn
            width:  availableWidth

            property real _minW:                ScreenTools.defaultFontPixelWidth * 20
            property real _boxWidth:            _minW
            property real _boxSpace:            ScreenTools.defaultFontPixelWidth
            property real _margins:             ScreenTools.defaultFontPixelWidth
            property Fact _frameClass:          controller.getParameterFact(-1, "FRAME_CLASS", false)
            property Fact _frameType:           controller.getParameterFact(-1, "FRAME_TYPE", false)    // FRAME_TYPE is not available on all Rover versions
            property bool _frameTypeAvailable:  !!_frameType && !!controller.vehicle && controller.vehicle.multiRotor
            readonly property var _frameClasses: {
                const frames = []
                for (let i = 0; i < controller.frameClassModel.count; ++i) {
                    const frame = controller.frameClassModel.get(i)
                    if (frame.category === airframePage.category) {
                        frames.push(frame)
                    }
                }
                return frames
            }

            readonly property real spacerHeight: ScreenTools.defaultFontPixelHeight

            onWidthChanged:         computeDimensions()
            Component.onCompleted:  computeDimensions()

            function computeDimensions() {
                let sw  = 0
                let rw  = 0
                let idx = Math.floor(mainColumn.width / (_minW + ScreenTools.defaultFontPixelWidth))
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

            APMAirframeComponentController { id: controller; }

            AirframeCategoryMenu {
                Layout.fillWidth: true
                category: airframePage.category
                onCategorySelected: (category) => { airframePage.category = category }
            }

            QGCLabel {
                objectName: "apmAirframeCategoryUnavailable"
                Layout.fillWidth: true
                visible: airframePage.category !== "" && mainColumn._frameClasses.length === 0
                text: qsTr("当前固件未提供此类机架。")
                wrapMode: Text.WordWrap
            }

            QGCLabel {
                id:                 helpText
                Layout.fillWidth:   true
                visible:            mainColumn._frameClasses.length > 0
                text:               (!_frameClass || _frameClass.rawValue === 0 ?
                                         qsTr("Airframe is currently not set.") :
                                         qsTr("Currently set to frame class '%1'").arg(_frameClass.enumStringValue) +
                                         (_frameTypeAvailable ?  qsTr(" and frame type '%2'").arg(_frameType.enumStringValue) : "") +
                                         qsTr(".", "period for end of sentence")) +
                                    qsTr(" To change this configuration, select the desired frame class below and then reboot the vehicle.")
                font.bold:          true
                wrapMode:           Text.WordWrap
            }

            QGCLabel {
                objectName: "apmAirframeMotorNumberingNote"
                Layout.fillWidth: true
                visible: mainColumn._frameClasses.some(frame => {
                    const resource = _frameClass && frame.frameClass === _frameClass.rawValue
                        ? frame.imageResource : frame.imageResourceDefault
                    return frame.category === "multirotor" && AirframeCatalog.apmImage(resource) !== resource
                })
                text: qsTr("图片中的电机编号适用于 PX4；当前飞控编号以电机页面为准。")
                wrapMode: Text.WordWrap
            }

            Item {
                id:             lastSpacer
                height:         parent.spacerHeight
                width:          10
            }

            Flow {
                id:                 flowView
                Layout.fillWidth:   true
                spacing:            _boxSpace

                Repeater {
                    model: mainColumn._frameClasses

                    // Outer summary item rectangle
                    Rectangle {
                        id:         outerRect
                        objectName: "apmAirframeBox_" + frame.name
                        width:      _boxWidth
                        height:     ScreenTools.defaultFontPixelHeight * 14
                        color:      qgcPal.window

                        required property var modelData
                        readonly property var frame: modelData
                        readonly property bool selected: !!_frameClass && frame.frameClass === _frameClass.rawValue
                        readonly property real titleHeight: ScreenTools.defaultFontPixelHeight * 1.75
                        readonly property real innerMargin: ScreenTools.defaultFontPixelWidth

                        MouseArea {
                            anchors.fill: parent
                            onClicked: {
                                if (!_frameClass || !controller.vehicle || controller.vehicle.armed || controller.vehicle.airborne) {
                                    return
                                }
                                if (!outerRect.selected || !combo.valid) {
                                    if (_frameTypeAvailable && outerRect.frame.defaultFrameType !== -1) {
                                        _frameType.rawValue = outerRect.frame.defaultFrameType
                                    }
                                    _frameClass.rawValue = outerRect.frame.frameClass
                                }
                            }
                        }

                        QGCLabel {
                            id:     title
                            text:   outerRect.frame.name
                        }

                        Rectangle {
                            id:                 imageComboRect
                            anchors.topMargin:  ScreenTools.defaultFontPixelHeight / 2
                            anchors.top:        title.bottom
                            anchors.bottom:     parent.bottom
                            anchors.left:       parent.left
                            anchors.right:      parent.right
                            color:              Qt.lighter(qgcPal.settingsAccent, 1.3)
                            border.color:       qgcPal.text
                            border.width:       outerRect.selected ? 1 : 0
                            opacity:            combo.valid ? 1.0 : 0.5

                            ColumnLayout {
                                anchors.margins:    innerMargin
                                anchors.fill:       parent
                                spacing:            innerMargin

                                Image {
                                    id:                 image
                                    objectName:         "apmAirframeImage_" + outerRect.frame.name
                                    Layout.fillWidth:   true
                                    Layout.fillHeight:  true
                                    fillMode:           Image.PreserveAspectFit
                                    smooth:             true
                                    antialiasing:       true
                                    sourceSize.width:   width
                                    source:             AirframeCatalog.apmImage(outerRect.selected
                                                            ? outerRect.frame.imageResource : outerRect.frame.imageResourceDefault)
                                }

                                QGCLabel {
                                    text:           qsTr("Frame Type")
                                    font.pointSize: ScreenTools.smallFontPointSize
                                    color:          qgcPal.buttonHighlightText
                                    visible:        outerRect.selected && outerRect.frame.frameTypeSupported
                                }

                                QGCComboBox {
                                    id:                 combo
                                    Layout.fillWidth:   true
                                    model:              outerRect.frame.frameTypeEnumStrings
                                    visible:            outerRect.selected && outerRect.frame.frameTypeSupported
                                    onActivated: (index) => {
                                        if (_frameType && controller.vehicle && !controller.vehicle.armed && !controller.vehicle.airborne) {
                                            _frameType.rawValue = outerRect.frame.frameTypeEnumValues[index]
                                        }
                                    }

                                    property bool valid: true

                                    function checkFrameType(value) {
                                        return !!_frameType && value === _frameType.rawValue
                                    }

                                    function selectFrameType() {
                                        let index = outerRect.frame.frameTypeEnumValues.findIndex(checkFrameType)
                                        if (index == -1 && combo.visible) {
                                            // Frame Class/Type is set to an invalid combination
                                            combo.valid = false
                                        } else {
                                            combo.currentIndex = index
                                            combo.valid = true
                                        }
                                    }

                                    Component.onCompleted: selectFrameType()
                                    onVisibleChanged: selectFrameType()

                                    Connections {
                                        target:                 _frameTypeAvailable ? _frameType : null
                                        ignoreUnknownSignals:   true
                                        function onRawValueChanged(value) { combo.selectFrameType() }
                                    }
                                }
                            }
                        }

                        QGCLabel {
                            anchors.fill:   imageComboRect
                            text:           qsTr("Invalid setting for FRAME_TYPE. Click to Reset.")
                            wrapMode:       Text.WordWrap
                            visible:        !combo.valid
                        }
                    }
                } // Repeater - summary boxes
            } // Flow - summary boxes
        } // Column
    } // Component
} // SetupPage
