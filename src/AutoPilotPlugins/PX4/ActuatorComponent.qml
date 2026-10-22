import QtQuick
import QtQuick.Controls
import QtQuick.Dialogs
import QtQuick.Layouts

import QGroundControl
import QGroundControl.Controls
import QGroundControl.FactControls
import QGroundControl.AutoPilotPlugins.PX4

SetupPage {
    id:             actuatorPage
    pageComponent:  pageComponent
    showAdvanced:   true

    property var actuators:       globals.activeVehicle.actuators

    property var _showAdvanced:              advanced
    readonly property real _margins:         ScreenTools.defaultFontPixelHeight
    readonly property bool _constrainedWidth: QGroundControl.corePlugin.directMissionItemEditing

    function actuatorLabel(value) {
        const label = String(value)
        switch (label) {
        case "Motors": return qsTr("Motors")
        case "Servos": return qsTr("Servos")
        case "ESCs": return qsTr("ESCs")
        case "Function": return qsTr("Function")
        case "Configure": return qsTr("Configure")
        case "Position X": return qsTr("Position X")
        case "Position Y": return qsTr("Position Y")
        case "Position Z": return qsTr("Position Z")
        case "Direction CCW": return qsTr("Direction CCW")
        case "Axis X": return qsTr("Axis X")
        case "Axis Y": return qsTr("Axis Y")
        case "Axis Z": return qsTr("Axis Z")
        case "Min": return qsTr("Min")
        case "Max": return qsTr("Max")
        case "Disarmed": return qsTr("Disarmed")
        case "Failsafe": return qsTr("Failsafe")
        case "Bitrate": return qsTr("Bitrate")
        case "Trim": return qsTr("Trim")
        case "Reversible": return qsTr("Reversible")
        }
        const channel = /^(Motor|Servo|ESC) (\d+)$/.exec(label)
        if (channel) {
            switch (channel[1]) {
            case "Motor": return qsTr("Motor %1").arg(channel[2])
            case "Servo": return qsTr("Servo %1").arg(channel[2])
            case "ESC": return qsTr("ESC %1").arg(channel[2])
            }
        }
        return label
    }

    component ActuatorChannelGrid: GridLayout {
        id: root
        required property var group
        required property bool showAdvanced
        required property var labelText
        property bool constrainedWidth: QGroundControl.corePlugin.directMissionItemEditing
        readonly property int visibleColumnCount: {
            let count = 0
            for (let i = 0; i < group.channelConfigs.count; ++i) {
                const config = group.channelConfigs.get(i)
                if (config.visible && (showAdvanced || !config.advanced)) { ++count }
            }
            return count
        }
        readonly property bool stacked: constrainedWidth
            && width < ScreenTools.defaultFontPixelWidth * (10 + 10 * visibleColumnCount)
        columns: stacked ? 1 : 1 + group.channelConfigs.count
        rows: stacked ? group.channels.count * (group.channelConfigs.count + 1) : 1 + group.channels.count
        columnSpacing: ScreenTools.defaultFontPixelWidth
        rowSpacing: ScreenTools.defaultFontPixelHeight * 0.5
        Layout.fillWidth: constrainedWidth
        Layout.minimumWidth: 0

        QGCLabel { visible: !root.stacked; text: ""; Layout.row: 0; Layout.column: 0 }
        Repeater {
            model: root.group.channelConfigs
            QGCLabel {
                text: root.labelText(object.label)
                visible: !root.stacked && object.visible && (root.showAdvanced || !object.advanced)
                Layout.row: 0
                Layout.column: 1 + index
                Layout.fillWidth: root.constrainedWidth
                Layout.minimumWidth: 0
                wrapMode: Text.WordWrap
            }
        }
        Repeater {
            model: root.group.channels
            QGCLabel {
                objectName: root.objectName + "_channel_" + index
                text: root.labelText(object.label) + ":"
                font.bold: root.stacked
                Layout.row: root.stacked ? index * (root.group.channelConfigs.count + 1) : 1 + index
                Layout.column: 0
                Layout.fillWidth: root.constrainedWidth
                Layout.minimumWidth: 0
                wrapMode: Text.WordWrap
            }
        }
        Repeater {
            model: root.group.channels
            Repeater {
                property int channelIndex: index
                model: object.configInstances
                Item {
                    id: cell
                    objectName: root.objectName + "_cell_" + channelIndex + "_" + index
                    visible: object.config.visible && (root.showAdvanced || !object.config.advanced)
                        && (typeof object.visible === "undefined" || object.visible)
                    enabled: typeof object.enabled === "undefined" || object.enabled
                    Layout.row: root.stacked ? channelIndex * (root.group.channelConfigs.count + 1) + 1 + index
                                            : 1 + channelIndex
                    Layout.column: root.stacked ? 0 : 1 + index
                    Layout.fillWidth: root.constrainedWidth
                    Layout.minimumWidth: 0
                    implicitWidth: root.constrainedWidth ? ScreenTools.defaultFontPixelWidth * 10 : field.implicitWidth
                    implicitHeight: Math.max(label.visible ? label.implicitHeight : 0, field.implicitHeight)
                    QGCLabel {
                        id: label
                        text: root.labelText(object.config.label)
                        visible: root.stacked
                        width: Math.min(cell.width * 0.4, ScreenTools.defaultFontPixelWidth * 10)
                        anchors.verticalCenter: parent.verticalCenter
                        wrapMode: Text.WordWrap
                    }
                    ActuatorFact {
                        id: field
                        objectName: cell.objectName + "_fact"
                        fact: object.fact
                        constrainedWidth: root.constrainedWidth
                        x: label.visible ? label.width + ScreenTools.defaultFontPixelWidth : 0
                        width: root.constrainedWidth ? Math.max(0, cell.width - x) : implicitWidth
                        height: implicitHeight
                        anchors.verticalCenter: parent.verticalCenter
                    }
                }
            }
        }
    }

    Component {
        id: pageComponent

        Grid {
            objectName: "actuatorSetup_layout"
            columns: QGroundControl.corePlugin.directMissionItemEditing ? 1 : 2
            width: QGroundControl.corePlugin.directMissionItemEditing ? availableWidth : implicitWidth
            spacing:                        ScreenTools.defaultFontPixelWidth * 4
            property var _leftColumnWidth:  QGroundControl.corePlugin.directMissionItemEditing ? availableWidth
                                               : Math.max(actuatorTesting.implicitWidth, mixerUi.implicitWidth) + (_margins * 2)

            ColumnLayout {
                spacing:                    ScreenTools.defaultFontPixelHeight
                width:                      _leftColumnWidth

                // mixer ui
                RowLayout {
                    width:                      _leftColumnWidth
                    visible:                    actuators.mixer.groups.count > 0
                    QGCLabel {
                        text:                   qsTr("Geometry") + (actuators.mixer.title ? ": " + actuators.mixer.title : "")
                        font.pointSize:         ScreenTools.mediumFontPointSize
                        Layout.fillWidth:       true
                    }
                    QGCLabel {
                        text:                   "<a href='"+actuators.mixer.helpUrl+"'>?</a>"
                        font.pointSize:         ScreenTools.mediumFontPointSize
                        visible:                actuators.mixer.helpUrl
                        textFormat:             Text.RichText
                        onLinkActivated: (link) => {
                            Qt.openUrlExternally(link);
                        }
                    }
                }

                Rectangle {
                    objectName: "actuatorSetup_geometry"
                    implicitWidth:          _leftColumnWidth
                    implicitHeight:         mixerUi.height + (_margins * 2)
                    color:                  qgcPal.windowShade
                    visible:                actuators.mixer.groups.count > 0

                    Column {
                        id:                 mixerUi
                        width: actuatorPage._constrainedWidth ? Math.max(0, parent.width - actuatorPage._margins * 2) : implicitWidth
                        spacing:            _margins
                        anchors {
                            left:           parent.left
                            leftMargin:     _margins
                            verticalCenter: parent.verticalCenter
                        }
                        enabled:            !safetySwitch.checked && !actuators.motorAssignmentActive
                        Repeater {
                            model:          actuators.mixer.groups
                            ColumnLayout {
                                property var mixerGroup: object
                                width: actuatorPage._constrainedWidth ? mixerUi.width : implicitWidth

                                RowLayout {
                                    Layout.fillWidth: actuatorPage._constrainedWidth
                                    QGCLabel {
                                        text:                    actuatorPage.actuatorLabel(mixerGroup.label)
                                        Layout.fillWidth: actuatorPage._constrainedWidth
                                        Layout.minimumWidth: 0
                                        wrapMode: Text.WordWrap
                                        font.bold:               true
                                        rightPadding:            ScreenTools.defaultFontPixelWidth * 3
                                    }
                                    ActuatorFact {
                                        property var countParam: mixerGroup.countParam
                                        visible:                 countParam != null
                                        fact:                    countParam ? countParam.fact : null
                                    }
                                }

                                ActuatorChannelGrid {
                                    objectName: "actuatorSetup_geometryGrid_" + index
                                    group: mixerGroup
                                    showAdvanced: actuatorPage._showAdvanced
                                    labelText: actuatorPage.actuatorLabel
                                }


                                // extra group config params
                                Repeater {
                                    model: mixerGroup.configParams

                                    RowLayout {
                                        Layout.fillWidth: actuatorPage._constrainedWidth
                                        spacing:     ScreenTools.defaultFontPixelWidth
                                        QGCLabel {
                                            text:    actuatorPage.actuatorLabel(object.label) + ":"
                                            Layout.preferredWidth: actuatorPage._constrainedWidth ? parent.width * 0.4 : -1
                                            Layout.minimumWidth: 0
                                            wrapMode: Text.WordWrap
                                            visible: _showAdvanced || !object.advanced
                                        }
                                        ActuatorFact {
                                            fact: object.fact
                                            visible: _showAdvanced || !object.advanced
                                        }
                                    }
                                }
                            }
                        }
                    }
                }

                // actuator image
                Image {
                    id:                     actuatorImage
                    source:                 "image://actuators/geometry"+refreshFlag
                    sourceSize.width:       imageSize
                    sourceSize.height:      imageSize
                    Layout.preferredWidth:  imageSize
                    Layout.preferredHeight: imageSize
                    Layout.alignment:       Qt.AlignHCenter
                    visible:                actuators.isMultirotor
                    cache:                  false

                    property var refreshFlag:         actuators.imageRefreshFlag
                    readonly property real imageSize: actuatorPage._constrainedWidth
                        ? Math.min(availableWidth, 9 * ScreenTools.defaultFontPixelHeight) : 9 * ScreenTools.defaultFontPixelHeight

                    MouseArea {
                        anchors.fill:  parent
                        onClicked: (mouse) => {
                            if (mouse.button == Qt.LeftButton) {
                                actuators.imageClicked(Qt.size(width, height), mouse.x, mouse.y);
                            }
                        }
                    }
                }

                // actuator testing
                QGCLabel {
                    text:               qsTr("Actuator Testing")
                    font.pointSize:     ScreenTools.mediumFontPointSize
                }

                Rectangle {
                    implicitWidth:            _leftColumnWidth
                    implicitHeight:           actuatorTesting.height + (_margins * 2)
                    color:                    qgcPal.windowShade

                    Column {
                        id:                   actuatorTesting
                        width: actuatorPage._constrainedWidth ? Math.max(0, parent.width - actuatorPage._margins * 2) : implicitWidth
                        spacing:              _margins
                        anchors {
                            left:             parent.left
                            leftMargin:       _margins
                            verticalCenter:   parent.verticalCenter
                        }

                        QGCLabel {
                            text: qsTr("Configure some outputs in order to test them.")
                            width: actuatorPage._constrainedWidth ? parent.width : implicitWidth
                            wrapMode: Text.WordWrap
                            visible: actuators.actuatorTest.actuators.count == 0
                        }

                        RowLayout {
                            width: actuatorPage._constrainedWidth ? actuatorTesting.width : implicitWidth
                            spacing: ScreenTools.defaultFontPixelWidth
                            visible: actuators.actuatorTest.actuators.count > 0

                            QGCCheckBoxSlider {
                                id:      safetySwitch
                                enabled: !actuators.motorAssignmentActive &&  !actuators.actuatorTest.hadFailure
                                Connections {
                                    target: actuators.actuatorTest
                                    onHadFailureChanged: {
                                        if (actuators.actuatorTest.hadFailure) {
                                            safetySwitch.checked = false;
                                            safetySwitch.switchUpdated();
                                        }
                                    }
                                }
                                onClicked: {
                                    switchUpdated();
                                }
                                function switchUpdated() {
                                    if (!checked) {
                                        for (var channelIdx=0; channelIdx<sliderRepeater.count; channelIdx++) {
                                            sliderRepeater.itemAt(channelIdx).stop();
                                        }
                                        if (allMotorsLoader.item != null)
                                            allMotorsLoader.item.stop();
                                    }
                                    actuators.actuatorTest.setActive(checked);
                                }
                            }

                            QGCLabel {
                                Layout.fillWidth: actuatorPage._constrainedWidth
                                Layout.minimumWidth: 0
                                wrapMode: Text.WordWrap
                                color:  qgcPal.warningText
                                text: safetySwitch.checked ? qsTr("Careful: Actuator sliders are enabled") : qsTr("Propellers are removed - Enable sliders")
                            }
                        } // Row

                        Flow {
                            width: actuatorPage._constrainedWidth ? actuatorTesting.width : implicitWidth
                            spacing: ScreenTools.defaultFontPixelWidth * 2
                            enabled: safetySwitch.checked

                            // (optional) slider for all motors
                            Loader {
                                id:                allMotorsLoader
                                sourceComponent:   actuators.actuatorTest.allMotorsActuator ?  allMotorsComponent : null
                                Layout.alignment:  Qt.AlignTop
                            }
                            Component {
                                id:                allMotorsComponent
                                ActuatorSlider {
                                    channel:       actuators.actuatorTest.allMotorsActuator
                                    rightPadding:  ScreenTools.defaultFontPixelWidth * 3
                                    onActuatorValueChanged: (value, sliderValue) => {
                                        stopTimer();
                                        for (var channelIdx=0; channelIdx<sliderRepeater.count; channelIdx++) {
                                            var channelSlider = sliderRepeater.itemAt(channelIdx);
                                            if (channelSlider.channel.isMotor) {
                                                channelSlider.value = sliderValue;
                                            }
                                        }
                                    }
                                }
                            }

                            // all channels
                            Repeater {
                                id:         sliderRepeater
                                model:      actuators.actuatorTest.actuators

                                ActuatorSlider {
                                    channel: object
                                    onActuatorValueChanged: (value) =>{
                                        if (isNaN(value)) {
                                            actuators.actuatorTest.stopControl(index);
                                            stop();
                                        } else {
                                            actuators.actuatorTest.setChannelTo(index, value);
                                        }
                                    }
                                }
                            } // Repeater
                        } // Row

                        // actuator actions
                        Column {
                            visible: actuators.actuatorActions.count > 0
                            enabled: !safetySwitch.checked && !actuators.motorAssignmentActive
                            Row {
                                spacing: ScreenTools.defaultFontPixelWidth * 2
                                Repeater {
                                    model: actuators.actuatorActions

                                    QGCButton {
                                        property var actionGroup: object
                                        text:          actionGroup.label
                                        onClicked:     actionMenu.popup()
                                        QGCMenu {
                                            id:                 actionMenu

                                            Instantiator {
                                                model:              actionGroup.actions
                                                QGCMenuItem {
                                                    text:           object.label
                                                    onTriggered:    object.trigger()
                                                }
                                                onObjectAdded:      (index, object) => actionMenu.insertItem(index, object)
                                                onObjectRemoved:    (index, object) => actionMenu.removeItem(object)
                                            }
                                        }
                                    }
                                }
                            }

                        } // Column

                    } // Column
                } // Rectangle
            }

            // Right column
            Column {
                id: outputColumn
                width: actuatorPage._constrainedWidth ? availableWidth : implicitWidth
                QGCLabel {
                    text:               qsTr("Actuator Outputs")
                    font.pointSize:     ScreenTools.mediumFontPointSize
                    bottomPadding:      ScreenTools.defaultFontPixelHeight
                }
                QGCLabel {
                    text:          qsTr("One or more actuator still needs to be assigned to an output.")
                    width: actuatorPage._constrainedWidth ? parent.width : implicitWidth
                    wrapMode: Text.WordWrap
                    visible:       actuators.hasUnsetRequiredFunctions
                    color:         qgcPal.warningText
                    bottomPadding: ScreenTools.defaultFontPixelHeight
                }


                // actuator output selection tabs
                QGCTabBar {
                    id: outputTabs
                    width: actuatorPage._constrainedWidth ? outputColumn.width : implicitWidth
                    Repeater {
                        model: actuators.actuatorOutputs
                        QGCTabButton {
                            text:      '   ' + object.label + '   '
                            width:     actuatorPage._constrainedWidth ? outputTabs.width / Math.max(1, outputTabs.count) : implicitWidth
                        }
                    }
                    onCurrentIndexChanged: {
                        actuators.selectActuatorOutput(currentIndex)
                    }
                }

                // actuator outputs
                Rectangle {
                    id:                             selActuatorOutput
                    objectName: "actuatorSetup_outputs"
                    width: actuatorPage._constrainedWidth ? outputColumn.width : implicitWidth
                    implicitWidth:                  actuatorGroupColumn.width + (_margins * 2)
                    implicitHeight:                 actuatorGroupColumn.height + (_margins * 2)
                    color:                          qgcPal.windowShade

                    property var actuatorOutput:    actuators.selectedActuatorOutput

                    Column {
                        id:               actuatorGroupColumn
                        width: actuatorPage._constrainedWidth ? Math.max(0, parent.width - actuatorPage._margins * 2) : implicitWidth
                        spacing:          _margins
                        anchors.centerIn: parent

                        // Motor assignment
                        Row {
                            visible:           actuators.isMultirotor
                            enabled:           !safetySwitch.checked
                            anchors.right:     parent.right
                            spacing:           _margins
                            QGCButton {
                                text:          qsTr("Identify & Assign Motors")
                                visible:       !actuators.motorAssignmentActive && selActuatorOutput.actuatorOutput.groupsVisible
                                enabled:       actuators.motorAssignmentEnabled
                                onClicked: {
                                    var success = actuators.initMotorAssignment()
                                    if (success) {
                                        motorAssignmentConfirmDialog.open()
                                    } else {
                                        motorAssignmentFailureDialog.open()
                                    }
                                }
                                MessageDialog {
                                    id:         motorAssignmentConfirmDialog
                                    visible:    false
                                    //icon:       StandardIcon.Warning
                                    buttons:    MessageDialog.Yes | MessageDialog.No
                                    title:      qsTr("Motor Order Identification and Assignment")
                                    text:       actuators.motorAssignmentMessage
                                    onButtonClicked: function (button, role) {
                                        switch (button) {
                                        case MessageDialog.Yes:
                                            actuators.startMotorAssignment()
                                            break;
                                        }
                                    }
                                }
                                MessageDialog {
                                    id:         motorAssignmentFailureDialog
                                    visible:    false
                                    //icon:       StandardIcon.Critical
                                    buttons:    MessageDialog.Ok
                                    title:      qsTr("Error")
                                    text:       actuators.motorAssignmentMessage
                                }
                            }
                            QGCButton {
                                text:          qsTr("Spin Motor Again")
                                visible:       actuators.motorAssignmentActive
                                onClicked: {
                                    actuators.spinCurrentMotor()
                                }
                            }
                            QGCButton {
                                text:          qsTr("Abort")
                                visible:       actuators.motorAssignmentActive
                                onClicked: {
                                    actuators.abortMotorAssignment()
                                }
                            }
                        }

                        Column {
                            width: actuatorPage._constrainedWidth ? actuatorGroupColumn.width : implicitWidth
                            enabled:          !safetySwitch.checked && !actuators.motorAssignmentActive
                            spacing:          _margins

                            RowLayout {
                                width: actuatorPage._constrainedWidth ? parent.width : implicitWidth
                                property var enableParam:     selActuatorOutput.actuatorOutput.enableParam
                                QGCLabel {
                                    visible:                  parent.enableParam != null
                                    text:                     parent.enableParam ? actuatorPage.actuatorLabel(parent.enableParam.label) + ":" : ""
                                    Layout.preferredWidth: actuatorPage._constrainedWidth ? parent.width * 0.4 : -1
                                    Layout.minimumWidth: 0
                                    wrapMode: Text.WordWrap
                                }
                                ActuatorFact {
                                    visible:                  parent.enableParam != null
                                    fact:                     parent.enableParam ?  parent.enableParam.fact : null
                                }
                            }


                            Repeater {
                                model: selActuatorOutput.actuatorOutput.subgroups

                                ColumnLayout {
                                    property var subgroup: object
                                    width: actuatorPage._constrainedWidth ? actuatorGroupColumn.width : implicitWidth
                                    visible:               selActuatorOutput.actuatorOutput.groupsVisible

                                    RowLayout {
                                        Layout.fillWidth: actuatorPage._constrainedWidth
                                        visible: subgroup.label != ""
                                        QGCLabel {
                                            text:                    actuatorPage.actuatorLabel(subgroup.label)
                                            Layout.fillWidth: actuatorPage._constrainedWidth
                                            Layout.minimumWidth: 0
                                            wrapMode: Text.WordWrap
                                            font.bold:               true
                                            rightPadding:            ScreenTools.defaultFontPixelWidth * 3
                                        }
                                        ActuatorFact {
                                            property var primaryParam: subgroup.primaryParam
                                            visible:                   primaryParam != null
                                            fact:                      primaryParam ? primaryParam.fact : null
                                        }
                                    }

                                    ActuatorChannelGrid {
                                        objectName: "actuatorSetup_outputGrid_" + index
                                        group: subgroup
                                        showAdvanced: actuatorPage._showAdvanced
                                        labelText: actuatorPage.actuatorLabel
                                    }


                                    // extra subgroup config params
                                    Repeater {
                                        model: subgroup.configParams

                                        RowLayout {
                                            Layout.fillWidth: actuatorPage._constrainedWidth
                                            QGCLabel {
                                                text: actuatorPage.actuatorLabel(object.label) + ":"
                                                Layout.preferredWidth: actuatorPage._constrainedWidth ? parent.width * 0.4 : -1
                                                Layout.minimumWidth: 0
                                                wrapMode: Text.WordWrap
                                            }
                                            ActuatorFact {
                                                fact: object.fact
                                            }
                                        }
                                    }

                                }
                            } // subgroup Repeater

                            // extra actuator config params
                            Repeater {
                                model: selActuatorOutput.actuatorOutput.configParams

                                RowLayout {
                                    width: actuatorPage._constrainedWidth ? parent.width : implicitWidth
                                    QGCLabel {
                                        text: actuatorPage.actuatorLabel(object.label) + ":"
                                        Layout.preferredWidth: actuatorPage._constrainedWidth ? parent.width * 0.4 : -1
                                        Layout.minimumWidth: 0
                                        wrapMode: Text.WordWrap
                                    }
                                    ActuatorFact {
                                        fact: object.fact
                                    }
                                }
                            }

                            // notes
                            Repeater {
                                model: selActuatorOutput.actuatorOutput.notes
                                ColumnLayout {
                                    width: actuatorPage._constrainedWidth ? parent.width : implicitWidth
                                    spacing: ScreenTools.defaultFontPixelHeight
                                    QGCLabel {
                                        Layout.fillWidth: actuatorPage._constrainedWidth
                                        Layout.minimumWidth: 0
                                        wrapMode: Text.WordWrap
                                        text:       modelData
                                        color:      qgcPal.warningText
                                    }
                                }
                            }
                        }
                    }
                } // Rectangle
            } // Column
        } // Row

    }
}
