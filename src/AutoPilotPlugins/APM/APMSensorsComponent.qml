import QtQuick
import QtQuick.Controls
import QtQuick.Dialogs
import QtQuick.Layouts

import QGroundControl
import QGroundControl.FactControls
import QGroundControl.Controls

SetupPage {
    id:             sensorsPage
    pageComponent:  sensorsPageComponent

    component SensorSectionDivider: Rectangle {
        width: parent.width
        height: ScreenTools.defaultFontPixelHeight * 0.06
        color: QGroundControl.globalPalette.groupBorder
    }

    Component {
        id:             sensorsPageComponent

        Item {
            width:  availableWidth
            height: availableHeight

            // Help text which is shown both in the status text area prior to pressing a cal button and in the
            // pre-calibration dialog.

            readonly property string orientationHelpSet:    qsTr("If mounted in the direction of flight, select None.")
            readonly property string orientationHelpCal:    qsTr("Before calibrating make sure rotation settings are correct. ") + orientationHelpSet
            readonly property string compassRotationText:   qsTr("If the compass or GPS module is mounted in flight direction, leave the default value (None)")

            readonly property string compassHelp:   qsTr("For Compass calibration you will need to rotate your vehicle through a number of positions.")
            readonly property string gyroHelp:      qsTr("For Gyroscope calibration you will need to place your vehicle on a surface and leave it still.")
            readonly property string accelHelp:     qsTr("For Accelerometer calibration you will need to place your vehicle on all six sides on a perfectly level surface and hold it still in each orientation for a few seconds.")
            readonly property string levelHelp:     qsTr("To level the horizon you need to place the vehicle in its level flight position and press OK.")

            readonly property string statusTextAreaDefaultText: ""

            // Used to pass help text to the preCalibrationDialog dialog
            property string preCalibrationDialogHelp

            property string _postCalibrationDialogText
            property var    _postCalibrationDialogParams

            readonly property string _badCompassCalText: qsTr("The calibration for Compass %1 appears to be poor. ") +
                                                         qsTr("Check the compass position within your vehicle and re-do the calibration.")

            readonly property int sideBarH1PointSize:  ScreenTools.mediumFontPointSize
            readonly property int mainTextH1PointSize: ScreenTools.mediumFontPointSize // Seems to be unused

            property Fact noFact: Fact { }

            property bool accelCalNeeded:                   controller.accelSetupNeeded
            property bool compassCalNeeded:                 controller.compassSetupNeeded

            property Fact boardRot:                         controller.getParameterFact(-1, "AHRS_ORIENTATION")

            readonly property int _calTypeCompass:  1   ///< Calibrate compass
            readonly property int _calTypeAccel:    2   ///< Calibrate accel
            readonly property int _calTypeSet:      3   ///< Set orientations only
            readonly property int _buttonWidth:     ScreenTools.defaultFontPixelWidth * 15

            property bool   _orientationSettingsExpanded:  false
            property bool   _orientationSettingsShowCompass: true
            property string _orientationSettingsHelp:       orientationHelpSet
            property int    _orientationSettingsCalType:    _calTypeSet
            property string _orientationSettingsTitle:      qsTr("飞控朝向")
            property bool   _calibrationButtonsEnabled:     true
            readonly property bool _canConfigure:          globals.activeVehicle && !globals.activeVehicle.armed && !controller.calibrationActive
            property real   _margins:                       ScreenTools.defaultFontPixelHeight / 2
            property bool   _compassAutoRotAvailable:       controller.parameterExists(-1, "COMPASS_AUTO_ROT")
            property Fact   _compassAutoRotFact:            controller.getParameterFact(-1, "COMPASS_AUTO_ROT", false /* reportMissing */)
            property bool   _compassAutoRot:                _compassAutoRotAvailable ? _compassAutoRotFact.rawValue == 2 : false
            property bool   _doSimpleAccelCal:              false
            property var    _gcsPosition:                    QGroundControl.positionManager.gcsPosition
            property var    _mapPosition:                    QGroundControl.flightMapPosition
            readonly property bool _inlineCalibration: QGroundControl.corePlugin.directMissionItemEditing
            property bool _inlinePreparationActive: false
            property string _inlinePreparationType: ""
            property string _inlinePreparationHelp: ""
            property string _inlineResultType: ""
            property string _inlineMessage: ""

            function sectionVisible(sectionId) {
                if (sectionId === "Gyroscope") {
                    return globals.activeVehicle && (globals.activeVehicle.multiRotor
                        || globals.activeVehicle.rover || globals.activeVehicle.sub)
                }
                if (sectionId === "CompassMot") {
                    return globals.activeVehicle && globals.activeVehicle.supports.motorInterference
                }
                return true
            }

            function prepareInlineCalibration(type, help) {
                if (!_canConfigure || !_calibrationButtonsEnabled) { return }
                _inlineResultType = ""
                _inlineMessage = ""
                _inlinePreparationType = type
                _inlinePreparationHelp = help
                _inlinePreparationActive = true
                caacPanel.currentType = type
            }

            function confirmInlineCalibration() {
                if (!_canConfigure || !_calibrationButtonsEnabled) { return }
                if (_orientationSettingsExpanded && _orientationSettingsCalType !== _calTypeSet) {
                    if (caacPanel.preparationItem) { caacPanel.preparationItem.confirmCalibration() }
                    return
                }
                _inlinePreparationActive = false
                switch (_inlinePreparationType) {
                case "level": controller.levelHorizon(); break
                case "gyro": controller.calibrateGyro(); break
                case "pressure": controller.calibratePressure(); break
                case "compassmot": controller.calibrateMotorInterference(); break
                }
            }

            function startInlineSensorCalibration(type) {
                if (!_canConfigure || !_calibrationButtonsEnabled) { return }
                _inlinePreparationActive = false
                _orientationSettingsExpanded = false
                _inlineResultType = ""
                _inlineMessage = ""
                caacPanel.currentType = type
                if (type === "accel") {
                    controller.calibrateAccel(_doSimpleAccelCal)
                } else if (type === "compass") {
                    controller.calibrateCompass()
                } else if (type === "pressure") {
                    controller.calibratePressure()
                }
            }

            function showOrientationSettings(calType) {
                if (!_canConfigure || !_calibrationButtonsEnabled) {
                    return
                }
                if (calType === _calTypeSet && _orientationSettingsExpanded && _orientationSettingsCalType === _calTypeSet) {
                    _orientationSettingsExpanded = false
                    return
                }
                _orientationSettingsExpanded = false
                _inlinePreparationActive = false
                _inlineResultType = ""
                _inlineMessage = ""
                _doSimpleAccelCal = false
                _orientationSettingsCalType = calType
                _orientationSettingsShowCompass = calType !== _calTypeAccel
                _orientationSettingsHelp = calType === _calTypeSet ? orientationHelpSet : orientationHelpCal
                _orientationSettingsTitle = calType === _calTypeCompass ? qsTr("Calibrate Compass")
                    : calType === _calTypeAccel ? qsTr("Calibrate Accelerometer") : qsTr("飞控朝向")
                _orientationSettingsExpanded = true
                buttonFlickable.contentY = 0
            }

            on_CanConfigureChanged: {
                if (!_canConfigure) {
                    _orientationSettingsExpanded = false
                }
            }

            Connections {
                target: sensorsPage
                function onSectionIdFilterChanged() {
                    if (_inlineCalibration && !controller.calibrationActive) {
                        _inlinePreparationActive = false
                        _inlinePreparationType = ""
                        _inlinePreparationHelp = ""
                        _orientationSettingsExpanded = false
                        _inlineResultType = ""
                        _inlineMessage = ""
                        statusTextArea.text = statusTextAreaDefaultText
                        orientationCalAreaHelpText.text = ""
                        progressBar.value = 0
                    }
                }
            }

            // Maps the APM controller per-side done/inProgress bools to a VehicleRotationCal.CalState
            function sideCalState(done, inProgress) {
                if (inProgress) {
                    return VehicleRotationCal.CalState.InProgress
                }
                return done ? VehicleRotationCal.CalState.Completed : VehicleRotationCal.CalState.Incomplete
            }

            function compassLabel(index) {
                let label = qsTr("Compass %1 ").arg(index+1)
                let addOpenParan = true
                let addComma = false
                if (sensorParams.compassPrimaryFactAvailable) {
                    label += sensorParams.rgCompassPrimary[index] ? qsTr("(primary") : qsTr("(secondary")
                    addComma = true
                    addOpenParan = false
                }
                if (sensorParams.rgCompassExternalParamAvailable[index]) {
                    if (addOpenParan) {
                        label += "("
                    }
                    if (addComma) {
                        label += qsTr(", ")
                    }
                    label += sensorParams.rgCompassExternal[index] ? qsTr("external") : qsTr("internal")
                }
                label += ")"
                return label
            }

            APMSensorParams {
                id:                     sensorParams
                factPanelController:    controller
            }

            APMSensorsComponentController {
                id:                         controller
                inlineCalibration:          _inlineCalibration
                statusLog:                  statusTextArea
                progressBar:                progressBar
                nextButton:                 nextButton
                cancelButton:               cancelButton
                orientationCalAreaHelpText: orientationCalAreaHelpText

                property var rgCompassCalFitness: [ controller.compass1CalFitness, controller.compass2CalFitness, controller.compass3CalFitness ]

                onResetStatusTextArea: statusLog.text = statusTextAreaDefaultText

                onWaitingForCancelChanged: {
                    if (controller.waitingForCancel && !_inlineCalibration) {
                        waitForCancelDialogFactory.open()
                    }
                }

                onCalibrationComplete: (calType) => {
                    if (_inlineCalibration) {
                        _singleCompassSettingsComponentShowPriority = true
                        _inlineResultType = calType === MAVLink.CalibrationMag ? "compass"
                            : calType === MAVLink.CalibrationAccel ? "accel" : "other"
                        return
                    }
                    switch (calType) {
                    case MAVLink.CalibrationMag:
                        _singleCompassSettingsComponentShowPriority = true
                        postOnboardCompassCalibrationFactory.open()
                        break
                    case MAVLink.CalibrationAccel:
                        postCalibrationFactory.open()
                        break
                    }
                }

                onSetAllCalButtonsEnabled: (enabled) => {
                    _calibrationButtonsEnabled = enabled
                }

                onCalibrationActiveChanged: {
                    if (controller.calibrationActive) {
                        globals.navigationBlockedReason = qsTr("Complete or cancel the current calibration first")
                    } else {
                        globals.navigationBlockedReason = ""
                    }
                }
            }

            Component.onDestruction: globals.navigationBlockedReason = ""

            QGCPalette { id: qgcPal; colorGroupEnabled: true }

            QGCPopupDialogFactory {
                id: waitForCancelDialogFactory

                dialogComponent: waitForCancelDialogComponent
            }

            Component {
                id: waitForCancelDialogComponent

                QGCSimpleMessageDialog {
                    title:      qsTr("Calibration Cancel")
                    text:       qsTr("Waiting for Vehicle to response to Cancel. This may take a few seconds.")
                    buttons:    0

                    Connections {
                        target: controller

                        onWaitingForCancelChanged: {
                            if (!controller.waitingForCancel) {
                                close()
                            }
                        }
                    }
                }
            }

            Component {
                id: singleCompassOnboardResultsComponent

                Column {
                    anchors.left:   parent ? parent.left : undefined
                    anchors.right:  parent ? parent.right : undefined
                    spacing:        Math.round(ScreenTools.defaultFontPixelHeight / 2)
                    visible:        sensorParams.rgCompassAvailable[index] && sensorParams.rgCompassUseFact[index].value

                    property int _index: index

                    property real greenMaxThreshold:   8 * (sensorParams.rgCompassExternal[index] ? 1 : 2)
                    property real yellowMaxThreshold:  15 * (sensorParams.rgCompassExternal[index] ? 1 : 2)
                    property real fitnessRange:        25 * (sensorParams.rgCompassExternal[index] ? 1 : 2)

                    Item {
                        anchors.left:   parent.left
                        anchors.right:  parent.right
                        height:         ScreenTools.defaultFontPixelHeight

                        Row {
                            id:             fitnessRow
                            anchors.fill:   parent

                            Rectangle {
                                width:  parent.width * (greenMaxThreshold / fitnessRange)
                                height: parent.height
                                color:  "green"
                            }
                            Rectangle {
                                width:  parent.width * ((yellowMaxThreshold - greenMaxThreshold) / fitnessRange)
                                height: parent.height
                                color:  "yellow"
                            }
                            Rectangle {
                                width:  parent.width * ((fitnessRange - yellowMaxThreshold) / fitnessRange)
                                height: parent.height
                                color:  "red"
                            }
                        }

                        Rectangle {
                            height:                 fitnessRow.height * 0.66
                            width:                  height
                            anchors.verticalCenter: fitnessRow.verticalCenter
                            x:                      (fitnessRow.width * (Math.min(Math.max(controller.rgCompassCalFitness[index], 0.0), fitnessRange) / fitnessRange)) - (width / 2)
                            radius:                 height / 2
                            color:                  "white"
                            border.color:           "black"
                        }
                    }

                    Loader {
                        anchors.leftMargin: ScreenTools.defaultFontPixelWidth * 2
                        anchors.left:       parent.left
                        anchors.right:      parent.right
                        sourceComponent:    singleCompassSettingsComponent

                        property int index: _index
                    }
                }
            }

            QGCPopupDialogFactory {
                id: postOnboardCompassCalibrationFactory

                dialogComponent: postOnboardCompassCalibrationComponent
            }

            Component {
                id: postOnboardCompassCalibrationComponent

                QGCPopupDialog {
                    id:         postOnboardCompassCalibrationDialog
                    title:      qsTr("Calibration complete")
                    buttons:    Dialog.Ok

                    Column {
                        objectName: "postOnboardCompassCalibrationDialog"
                        width:      40 * ScreenTools.defaultFontPixelWidth
                        spacing:    ScreenTools.defaultFontPixelHeight

                        Repeater {
                            model:      3
                            delegate:   singleCompassOnboardResultsComponent
                        }

                        QGCLabel {
                            anchors.left:   parent.left
                            anchors.right:  parent.right
                            wrapMode:       Text.WordWrap
                            text:           qsTr("Shown in the indicator bars is the quality of the calibration for each compass.\n\n") +
                                            qsTr("- Green indicates a well functioning compass.\n") +
                                            qsTr("- Yellow indicates a questionable compass or calibration.\n") +
                                            qsTr("- Red indicates a compass which should not be used.\n\n") +
                                            qsTr("YOU MUST REBOOT YOUR VEHICLE AFTER EACH CALIBRATION.")
                        }
                    }
                }
            }

            QGCPopupDialogFactory {
                id: postCalibrationFactory

                dialogComponent: postCalibrationComponent
            }

            Component {
                id: postCalibrationComponent

                QGCPopupDialog {
                    id:     postCalibrationDialog
                    title:  qsTr("Calibration complete")

                    Column {
                        objectName: "postCalibrationDialog"
                        width:      40 * ScreenTools.defaultFontPixelWidth
                        spacing:    ScreenTools.defaultFontPixelHeight

                        QGCLabel {
                            anchors.left:   parent.left
                            anchors.right:  parent.right
                            wrapMode:       Text.WordWrap
                            text:           qsTr("YOU MUST REBOOT YOUR VEHICLE AFTER EACH CALIBRATION.")
                        }
                    }
                }
            }

            property bool _singleCompassSettingsComponentShowPriority: true
            Component {
                id: singleCompassSettingsComponent

                Column {
                    width: parent ? parent.width : 0
                    spacing: Math.round(ScreenTools.defaultFontPixelHeight / 2)
                    visible: sensorParams.rgCompassAvailable[index]

                    QGCLabel {
                        width: parent.width
                        wrapMode: Text.WordWrap
                        text: compassLabel(index)
                    }
                    APMSensorIdDecoder {
                        width: parent.width
                        wrapMode: Text.WordWrap
                        fact: sensorParams.rgCompassId[index]
                    }

                    Column {
                        anchors.leftMargin: ScreenTools.defaultFontPixelWidth
                        anchors.left:       parent.left
                        anchors.right:      parent.right
                        spacing:            Math.round(ScreenTools.defaultFontPixelHeight / 4)

                        Column {
                            width: parent.width
                            spacing: ScreenTools.defaultFontPixelWidth

                            FactCheckBox {
                                id:         useCompassCheckBox
                                width:      parent.width
                                wrapMode:   Text.WordWrap
                                text:       qsTr("Use Compass")
                                fact:       sensorParams.rgCompassUseFact[index]
                                visible:    sensorParams.rgCompassUseParamAvailable[index] && !sensorParams.rgCompassPrimary[index]
                            }

                            QGCComboBox {
                                width:      parent.width
                                model:      [ qsTr("Priority 1"), qsTr("Priority 2"), qsTr("Priority 3"), qsTr("Not Set") ]
                                visible:    _singleCompassSettingsComponentShowPriority && sensorParams.compassPrioFactsAvailable && useCompassCheckBox.visible && useCompassCheckBox.checked

                                property int _compassIndex: index

                                function selectPriorityfromParams() {
                                    currentIndex = 3
                                    let compassId = sensorParams.rgCompassId[_compassIndex].rawValue
                                    for (let prioIndex=0; prioIndex<3; prioIndex++) {
                                        if (compassId == sensorParams.rgCompassPrio[prioIndex].rawValue) {
                                            currentIndex = prioIndex
                                            break
                                        }
                                    }
                                }

                                Component.onCompleted: selectPriorityfromParams()

                                onActivated: (index) => {
                                    if (index == 3) {
                                        // User cannot select Not Set
                                        selectPriorityfromParams()
                                    } else {
                                        sensorParams.rgCompassPrio[index].rawValue = sensorParams.rgCompassId[_compassIndex].rawValue
                                    }
                                }
                            }
                        }

                        Column {
                            width: parent.width
                            visible: !_compassAutoRot && sensorParams.rgCompassExternal[index] && sensorParams.rgCompassRotParamAvailable[index]

                            QGCLabel { text: qsTr("Orientation:") }

                            FactComboBox {
                                id: compassOrientation
                                width:      parent.width
                                indexModel: false
                                fact:       sensorParams.rgCompassRotFact[index]
                                deferred: _inlineCalibration
                            }
                            SettingsActionButton {
                                objectName: "sensorsSetup_saveCompassOrientation_" + index
                                width: parent.width
                                visible: _inlineCalibration
                                text: qsTr("Confirm and Save")
                                enabled: compassOrientation.modified && _canConfigure
                                onClicked: compassOrientation.applyPendingValue()
                            }
                        }
                    }
                }
            }

            Component {
                id: orientationSettingsComponent

                Column {
                    id: orientationSettingsColumn
                    objectName: "sensorsSetup_orientationSettings"
                    width: parent ? parent.width : 0
                    spacing: ScreenTools.defaultFontPixelHeight
                    enabled: _canConfigure && _calibrationButtonsEnabled

                    function compassMask() {
                        let mask = 0
                        mask |= (0 + (sensorParams.rgCompassPrio[0].rawValue !== 0)) << 0
                        mask |= (0 + (sensorParams.rgCompassPrio[1].rawValue !== 0)) << 1
                        mask |= (0 + (sensorParams.rgCompassPrio[2].rawValue !== 0)) << 2
                        return mask
                    }

                    function confirmCalibration() {
                        if (!_canConfigure || !_calibrationButtonsEnabled) {
                            return
                        }
                        if (_orientationSettingsCalType === _calTypeAccel) {
                            _orientationSettingsExpanded = false
                            controller.calibrateAccel(_doSimpleAccelCal)
                        } else if (_orientationSettingsCalType === _calTypeCompass) {
                            if (!northCalibrationCheckBox.checked) {
                                _orientationSettingsExpanded = false
                                controller.calibrateCompass()
                            } else {
                                let lat = parseFloat(northCalLat.text)
                                let lon = parseFloat(northCalLon.text)
                                if (useMapPositionCheckbox.checked) {
                                    lat = _mapPosition.latitude
                                    lon = _mapPosition.longitude
                                }
                                if (useGcsPositionCheckbox.checked) {
                                    lat = _gcsPosition.latitude
                                    lon = _gcsPosition.longitude
                                }
                                if (isNaN(lat) || isNaN(lon)) {
                                    return
                                }
                                let mask = compassMask()
                                _orientationSettingsExpanded = false
                                controller.calibrateCompassNorth(lat, lon, mask)
                            }
                        }
                    }

                    QGCLabel {
                        width: parent.width
                        text: _orientationSettingsTitle
                        font.bold: true
                        wrapMode: Text.WordWrap
                    }

                    QGCLabel {
                        width:      parent.width
                        wrapMode:   Text.WordWrap
                        text:       _orientationSettingsHelp
                    }

                    Column {
                        width: parent.width
                        QGCLabel { text: qsTr("Autopilot Rotation:") }

                        FactComboBox {
                            id: boardOrientation
                            objectName: "sensorsSetup_boardOrientation"
                            width:      parent.width
                            indexModel: false
                            fact:       boardRot
                            deferred: _inlineCalibration
                        }
                        SettingsActionButton {
                            objectName: "sensorsSetup_saveBoardOrientation"
                            width: parent.width
                            visible: _inlineCalibration
                            text: qsTr("Confirm and Save")
                            enabled: boardOrientation.modified && _canConfigure
                            onClicked: boardOrientation.applyPendingValue()
                        }
                    }

                    Column {
                        width: parent.width
                        visible: _orientationSettingsCalType == _calTypeAccel
                        spacing: ScreenTools.defaultFontPixelHeight

                        QGCLabel {
                            width:      parent.width
                            wrapMode:   Text.WordWrap
                            text: qsTr("Simple accelerometer calibration is less precise but allows calibrating without rotating the vehicle. Check this if you have a large/heavy vehicle.")
                        }

                        QGCCheckBox {
                            width: parent.width
                            wrapMode: Text.WordWrap
                            text: qsTr("Simple Accelerometer Calibration")
                            checked: _doSimpleAccelCal
                            onClicked: _doSimpleAccelCal = checked
                        }
                    }

                    Repeater {
                        model:      _orientationSettingsShowCompass ? 3 : 0
                        delegate:   singleCompassSettingsComponent
                    }

                    QGCLabel {
                        id:         magneticDeclinationLabel
                        width:      parent.width
                        visible:    globals.activeVehicle && globals.activeVehicle.sub && _orientationSettingsShowCompass
                        text:       qsTr("Magnetic Declination")
                    }

                    Column {
                        visible:            magneticDeclinationLabel.visible
                        anchors.margins:    ScreenTools.defaultFontPixelWidth
                        anchors.left:       parent.left
                        anchors.right:      parent.right
                        spacing:            ScreenTools.defaultFontPixelHeight

                        QGCCheckBox {
                            id:                           manualMagneticDeclinationCheckBox
                            width:                        parent.width
                            wrapMode:                     Text.WordWrap
                            text:                         qsTr("Manual Magnetic Declination")
                            property Fact autoDecFact:    controller.getParameterFact(-1, "COMPASS_AUTODEC")
                            property int manual:          0
                            property int automatic:       1

                            checked:    autoDecFact.rawValue === manual
                            onClicked:  autoDecFact.value = (checked ? manual : automatic)
                        }

                        FactTextField {
                            width:      parent.width
                            fact:       sensorParams.declinationFact
                            enabled:    manualMagneticDeclinationCheckBox.checked
                        }
                    }

                    Item { height: ScreenTools.defaultFontPixelHeight; width: 10 } // spacer

                    QGCLabel {
                        id:         northCalibrationLabel
                        width:      parent.width
                        visible:    _orientationSettingsShowCompass
                        wrapMode:   Text.WordWrap
                        text:       qsTr("Fast compass calibration given vehicle position and yaw. This ") +
                                    qsTr("results in zero diagonal and off-diagonal elements, so is only ") +
                                    qsTr("suitable for vehicles where the field is close to spherical. It is ") +
                                    qsTr("useful for large vehicles where moving the vehicle to calibrate it ") +
                                    qsTr("is difficult. Point the vehicle North before using it.")
                    }

                    Column {
                        visible:            northCalibrationLabel.visible
                        anchors.margins:    ScreenTools.defaultFontPixelWidth
                        anchors.left:       parent.left
                        anchors.right:      parent.right
                        spacing:            ScreenTools.defaultFontPixelHeight

                        QGCCheckBox {
                            id:             northCalibrationCheckBox
                            width:          parent.width
                            visible:        northCalibrationLabel.visible
                            wrapMode:       Text.WordWrap
                            text:           qsTr("Fast Calibration")
                        }

                        QGCLabel {
                            id:         northCalibrationManualPosition
                            width:      parent.width
                            visible:    northCalibrationCheckBox.checked && (!globals.activeVehicle || !globals.activeVehicle.coordinate.isValid)
                            wrapMode:   Text.WordWrap
                            text:       qsTr("Vehicle has no Valid positon, please provide it")
                        }

                        QGCCheckBox {
                            visible:    northCalibrationManualPosition.visible && _gcsPosition.isValid
                            id:         useGcsPositionCheckbox
                            width:      parent.width
                            wrapMode:   Text.WordWrap
                            text:       qsTr("Use GCS position instead")
                            checked:    _gcsPosition.isValid
                        }
                        QGCCheckBox {
                            visible:    northCalibrationManualPosition.visible && !_gcsPosition.isValid
                            id:         useMapPositionCheckbox
                            width:      parent.width
                            wrapMode:   Text.WordWrap
                            text:       qsTr("Use current map position instead")
                        }

                        QGCLabel {
                            width:      parent.width
                            visible:    useMapPositionCheckbox.checked
                            wrapMode:   Text.WordWrap
                            //: %1 is latitude, %2 is longitude
                            text:       qsTr("Lat: %1 Lon: %2").arg(_mapPosition.latitude.toFixed(4)).arg(_mapPosition.longitude.toFixed(4))
                        }

                        FactTextField {
                            id:         northCalLat
                            width:      parent.width
                            visible:    !useGcsPositionCheckbox.checked && !useMapPositionCheckbox.checked && northCalibrationCheckBox.checked
                            text:       "0.00"
                            textColor:  isNaN(parseFloat(text)) ? qgcPal.warningText: qgcPal.textFieldText
                            enabled:    !useGcsPositionCheckbox.checked
                        }
                        FactTextField {
                            id:         northCalLon
                            width:      parent.width
                            visible:    !useGcsPositionCheckbox.checked && !useMapPositionCheckbox.checked && northCalibrationCheckBox.checked
                            text:       "0.00"
                            textColor:  isNaN(parseFloat(text)) ? qgcPal.warningText: qgcPal.textFieldText
                            enabled:    !useGcsPositionCheckbox.checked
                        }

                    }

                    QGCButton {
                        objectName: "sensorsSetup_confirmOrientationCalibration"
                        width: parent.width
                        visible: !_inlineCalibration && _orientationSettingsCalType !== _calTypeSet
                        text: qsTr("开始校准")
                        onClicked: orientationSettingsColumn.confirmCalibration()
                    }

                    QGCButton {
                        objectName: "sensorsSetup_closeOrientationSettings"
                        width: parent.width
                        visible: !_inlineCalibration
                        text: _orientationSettingsCalType === _calTypeSet ? qsTr("Close") : qsTr("Cancel")
                        onClicked: _orientationSettingsExpanded = false
                    }
                }
            }

            QGCPopupDialogFactory {
                id: compassMotDialogFactory

                dialogComponent: compassMotDialogComponent
            }

            Component {
                id: compassMotDialogComponent

                QGCPopupDialog {
                    title:      qsTr("Compass Motor Interference Calibration")
                    buttons:    Dialog.Cancel | Dialog.Ok

                    onAccepted: controller.calibrateMotorInterference()

                    Column {
                        width:      40 * ScreenTools.defaultFontPixelWidth
                        spacing:    ScreenTools.defaultFontPixelHeight

                        QGCLabel {
                            anchors.left:   parent.left
                            anchors.right:  parent.right
                            wrapMode:       Text.WordWrap
                            text:           qsTr("This is recommended for vehicles that have only an internal compass and on vehicles where there is significant interference on the compass from the motors, power wires, etc. ") +
                                            qsTr("CompassMot only works well if you have a battery current monitor because the magnetic interference is linear with current drawn. ") +
                                            qsTr("It is technically possible to set-up CompassMot using throttle but this is not recommended.")
                        }

                        QGCLabel {
                            anchors.left:   parent.left
                            anchors.right:  parent.right
                            wrapMode:       Text.WordWrap
                            text:           qsTr("Disconnect your props, flip them over and rotate them one position around the frame. ") +
                                            qsTr("In this configuration they should push the copter down into the ground when the throttle is raised.")
                        }

                        QGCLabel {
                            anchors.left:   parent.left
                            anchors.right:  parent.right
                            wrapMode:       Text.WordWrap
                            text:           qsTr("Secure the copter (perhaps with tape) so that it does not move.")
                        }

                        QGCLabel {
                            anchors.left:   parent.left
                            anchors.right:  parent.right
                            wrapMode:       Text.WordWrap
                            text:           qsTr("Turn on your transmitter and keep throttle at zero.")
                        }

                        QGCLabel {
                            anchors.left:   parent.left
                            anchors.right:  parent.right
                            wrapMode:       Text.WordWrap
                            text:           qsTr("Click Ok to start CompassMot calibration.")
                        }
                    }
                }
            }

            SensorCalibrationPanel {
                id: caacPanel
                anchors.fill: parent
                visible: _inlineCalibration
                sectionId: sensorsPage.sectionIdFilter
                calibrationActive: controller.calibrationActive
                calibrationEnabled: _canConfigure && _calibrationButtonsEnabled
                accelComplete: !accelCalNeeded
                compassComplete: !compassCalNeeded
                levelEnabled: !controller.accelSetupNeeded
                pressureAvailable: true
                compassMotAvailable: sectionVisible("CompassMot")
                pressureHelpText: qsTr("Pressure calibration will set the %1 to zero at the current pressure reading. %2")
                    .arg(globals.activeVehicle && globals.activeVehicle.sub ? qsTr("depth") : qsTr("altitude"))
                    .arg(globals.activeVehicle && globals.activeVehicle.fixedWing
                        ? qsTr("To calibrate the airspeed sensor shield it from the wind. Do not touch the sensor or obstruct any holes during the calibration.") : "")
                persistentActions: true
                motorTestAvailable: false
                nextEnabled: nextButton.enabled
                cancelEnabled: cancelButton.enabled
                cancelObjectName: "sensorsSetup_cancelButton"
                progress: progressBar.value
                compassMotionGuidance: true
                compassAttitudeUpdateTime: controller.attitudeUpdateTime
                orientationPreviewVisible: caacPanel.currentType === "compass"
                    || controller.showOrientationCalArea || !controller.calibrationActive
                helpText: orientationCalAreaHelpText.text
                statusText: _inlineMessage !== "" ? _inlineMessage : statusTextArea.text
                waitingText: controller.waitingForCancel
                    ? qsTr("Waiting for Vehicle to response to Cancel. This may take a few seconds.") : ""
                orientationControls: orientationSettingsComponent
                preparationActive: _inlinePreparationActive
                    || (_orientationSettingsExpanded && _orientationSettingsCalType !== _calTypeSet)
                preparationText: _inlinePreparationActive ? _inlinePreparationHelp : ""
                preparationControls: _orientationSettingsExpanded && _orientationSettingsCalType !== _calTypeSet
                    ? orientationSettingsComponent : null
                confirmObjectName: "sensorsSetup_confirmOrientationCalibration"
                closeObjectName: "sensorsSetup_closeOrientationSettings"
                resultVisible: _inlineResultType !== ""
                resultTitle: qsTr("Calibration complete")
                resultText: _inlineResultType === "accel" || _inlineResultType === "compass"
                    ? qsTr("YOU MUST REBOOT YOUR VEHICLE AFTER EACH CALIBRATION.") : ""
                resultObjectName: _inlineResultType === "compass" ? "sensorsSetup_compassCalibrationResult"
                    : "sensorsSetup_calibrationResult"
                resultControls: _inlineResultType === "compass" ? inlineCompassResults : null
                sideStates: [sideCalState(controller.orientationCalDownSideDone, controller.orientationCalDownSideInProgress),
                    sideCalState(controller.orientationCalLeftSideDone, controller.orientationCalLeftSideInProgress),
                    sideCalState(controller.orientationCalRightSideDone, controller.orientationCalRightSideInProgress),
                    sideCalState(controller.orientationCalNoseDownSideDone, controller.orientationCalNoseDownSideInProgress),
                    sideCalState(controller.orientationCalTailDownSideDone, controller.orientationCalTailDownSideInProgress),
                    sideCalState(controller.orientationCalUpsideDownSideDone, controller.orientationCalUpsideDownSideInProgress)]
                sideVisible: [controller.orientationCalDownSideVisible, controller.orientationCalLeftSideVisible,
                    controller.orientationCalRightSideVisible, controller.orientationCalNoseDownSideVisible,
                    controller.orientationCalTailDownSideVisible, controller.orientationCalUpsideDownSideVisible]
                sideRotating: [controller.orientationCalDownSideRotate, controller.orientationCalLeftSideRotate,
                    controller.orientationCalRightSideRotate, controller.orientationCalNoseDownSideRotate,
                    controller.orientationCalTailDownSideRotate, controller.orientationCalUpsideDownSideRotate]
                onSetOrientation: showOrientationSettings(_calTypeSet)
                onStartAccel: startInlineSensorCalibration("accel")
                onStartCompass: {
                    if (controller.accelSetupNeeded) {
                        _inlineMessage = qsTr("Accelerometer must be calibrated prior to Compass.")
                    } else {
                        startInlineSensorCalibration("compass")
                    }
                }
                onStartLevel: prepareInlineCalibration("level", levelHelp)
                onStartPressure: startInlineSensorCalibration("pressure")
                onStartCompassMot: prepareInlineCalibration("compassmot",
                    qsTr("This is recommended for vehicles that have only an internal compass and on vehicles where there is significant interference on the compass from the motors, power wires, etc. ")
                    + qsTr("CompassMot only works well if you have a battery current monitor because the magnetic interference is linear with current drawn. ")
                    + qsTr("It is technically possible to set-up CompassMot using throttle but this is not recommended.")
                    + "\n\n" + qsTr("Disconnect your props, flip them over and rotate them one position around the frame. ")
                    + qsTr("In this configuration they should push the copter down into the ground when the throttle is raised.")
                    + "\n\n" + qsTr("Secure the copter (perhaps with tape) so that it does not move.")
                    + "\n\n" + qsTr("Turn on your transmitter and keep throttle at zero."))
                onNext: controller.nextClicked()
                onCancel: controller.cancelCalibration()
                onConfirmPreparation: confirmInlineCalibration()
                onDismissPreparation: {
                    _inlinePreparationActive = false
                    _orientationSettingsExpanded = false
                }
                onAcknowledgeResult: _inlineResultType = ""
                additionalControls: Component {
                    ColumnLayout {
                        spacing: ScreenTools.defaultFontPixelHeight / 2
                        SettingsActionButton {
                            objectName: "sensorsSetup_calibrateGyro"
                            Layout.fillWidth: true
                            text: qsTr("Gyro")
                            visible: caacPanel.matchesSection("Gyroscope") && sectionVisible("Gyroscope")
                            onClicked: prepareInlineCalibration("gyro", gyroHelp)
                        }
                    }
                }
            }

            Component {
                id: inlineCompassResults
                Column {
                    width: parent ? parent.width : 0
                    spacing: ScreenTools.defaultFontPixelHeight
                    Repeater {
                        model: 3
                        delegate: singleCompassOnboardResultsComponent
                    }
                    QGCLabel {
                        width: parent.width
                        wrapMode: Text.WordWrap
                        text: qsTr("Shown in the indicator bars is the quality of the calibration for each compass.\n\n")
                            + qsTr("- Green indicates a well functioning compass.\n")
                            + qsTr("- Yellow indicates a questionable compass or calibration.\n")
                            + qsTr("- Red indicates a compass which should not be used.\n\n")
                    }
                }
            }

            QGCFlickable {
                id:             buttonFlickable
                objectName:     "sensorsSetup_buttonScroller"
                visible:        !_inlineCalibration
                anchors.left:   parent.left
                anchors.top:    parent.top
                anchors.bottom: parent.bottom
                width:          _orientationSettingsExpanded ? parent.width : Math.min(_buttonWidth, parent.width)
                contentHeight:  nextCancelColumn.y + nextCancelColumn.height + _margins

                // Calibration button column - Calibratin buttons are kept in a separate column from Next/Cancel buttons
                // so we can enable/disable them all as a group
                Column {
                    id:                 buttonColumn
                    width:              buttonFlickable.width
                    enabled:            _canConfigure && _calibrationButtonsEnabled
                    spacing:            _margins
                    Layout.alignment:   Qt.AlignLeft | Qt.AlignTop

                    QGCButton {
                        objectName: "sensorsSetup_setOrientation"
                        width:      buttonColumn.width
                        text:       qsTr("飞控朝向")
                        onClicked:  showOrientationSettings(_calTypeSet)
                    }

                    Loader {
                        id: orientationSettingsLoader
                        width: parent.width
                        active: !_inlineCalibration && _orientationSettingsExpanded
                        visible: active
                        sourceComponent: orientationSettingsComponent
                    }

                    SensorSectionDivider { }

                    IndicatorButton {
                        objectName:     "sensorsSetup_calibrateAccel"
                        width:          buttonColumn.width
                        text:           qsTr("Accelerometer")
                        indicatorGreen: !accelCalNeeded

                        onClicked: function () {
                            showOrientationSettings(_calTypeAccel)
                        }
                    }

                    SensorSectionDivider { }

                    IndicatorButton {
                        objectName:     "sensorsSetup_calibrateCompass"
                        width:          buttonColumn.width
                        text:           qsTr("Compass")
                        indicatorGreen: !compassCalNeeded

                        onClicked: {
                            if (controller.accelSetupNeeded) {
                                QGroundControl.showMessageDialog(sensorsPage, qsTr("Calibrate Compass"), qsTr("Accelerometer must be calibrated prior to Compass."))
                            } else {
                                showOrientationSettings(_calTypeCompass)
                            }
                        }
                    }

                    SensorSectionDivider { }

                    QGCButton {
                        width:  buttonColumn.width
                        text:   _levelHorizonText

                        readonly property string _levelHorizonText: qsTr("Level Horizon")

                        onClicked: {
                            if (controller.accelSetupNeeded) {
                                QGroundControl.showMessageDialog(sensorsPage, _levelHorizonText, qsTr("Accelerometer must be calibrated prior to Level Horizon."))
                            } else {
                                QGroundControl.showMessageDialog(sensorsPage, _levelHorizonText,
                                                             qsTr("To level the horizon you need to place the vehicle in its level flight position and press Ok."),
                                                             Dialog.Cancel | Dialog.Ok,
                                                             function() { controller.levelHorizon() })
                            }
                        }
                    }

                    SensorSectionDivider {
                        visible: gyroCalibrationButton.visible
                    }

                    QGCButton {
                        id:         gyroCalibrationButton
                        width:      buttonColumn.width
                        text:       qsTr("Gyro")
                        visible:    globals.activeVehicle && (globals.activeVehicle.multiRotor | globals.activeVehicle.rover | globals.activeVehicle.sub)
                        onClicked:  QGroundControl.showMessageDialog(sensorsPage, qsTr("Calibrate Gyro"),
                                                                 qsTr("For Gyroscope calibration you will need to place your vehicle on a surface and leave it still.\n\nClick Ok to start calibration."),
                                                                 Dialog.Cancel | Dialog.Ok,
                                                                 function() { controller.calibrateGyro() })
                    }

                    SensorSectionDivider { }

                    QGCButton {
                        width:      buttonColumn.width
                        text:       _calibratePressureText
                        onClicked:  QGroundControl.showMessageDialog(sensorsPage, _calibratePressureText,
                                                                 qsTr("Pressure calibration will set the %1 to zero at the current pressure reading. %2").arg(_altText).arg(_helpTextFW),
                                                                 Dialog.Cancel | Dialog.Ok,
                                                                 function() { controller.calibratePressure() })

                        readonly property string _altText:                  globals.activeVehicle.sub ? qsTr("depth") : qsTr("altitude")
                        readonly property string _helpTextFW:               globals.activeVehicle.fixedWing ? qsTr("To calibrate the airspeed sensor shield it from the wind. Do not touch the sensor or obstruct any holes during the calibration.") : ""
                        readonly property string _calibratePressureText:    globals.activeVehicle.fixedWing ? qsTr("Baro/Airspeed") : qsTr("Pressure")
                    }

                    SensorSectionDivider {
                        visible: compassMotButton.visible
                    }

                    QGCButton {
                        id:         compassMotButton
                        width:      buttonColumn.width
                        text:       qsTr("CompassMot")
                        visible:    globals.activeVehicle ? globals.activeVehicle.supports.motorInterference : false
                        onClicked:  compassMotDialogFactory.open()
                    }
                } // Column - Cal Buttons

                Column {
                    id:                 nextCancelColumn
                    width:              buttonColumn.width
                    anchors.topMargin:  buttonColumn.spacing
                    anchors.top:        buttonColumn.bottom
                    anchors.left:       buttonColumn.left
                    spacing:            buttonColumn.spacing

                    QGCButton {
                        id:         nextButton
                        objectName: "sensorsSetup_nextButton"
                        width:      buttonColumn.width
                        text:       qsTr("Next")
                        enabled:    false
                        onClicked:  controller.nextClicked()
                    }

                    QGCButton {
                        id:             cancelButton
                        objectName:     "sensorsSetup_cancelButton"
                        width:          buttonColumn.width
                        text:           qsTr("Cancel")
                        enabled:        false
                        onClicked:      controller.cancelCalibration()
                    }
                }
            } // QGCFlickable - buttons

            /// Right column - cal area
            Column {
                visible:            !_inlineCalibration && !_orientationSettingsExpanded
                anchors.leftMargin: _orientationSettingsExpanded ? 0 : _margins
                anchors.top:        parent.top
                anchors.bottom:     parent.bottom
                anchors.left:       buttonFlickable.right
                anchors.right:      parent.right

                ProgressBar {
                    id:             progressBar
                    objectName:     "sensorsSetup_progressBar"
                    anchors.left:   parent.left
                    anchors.right:  parent.right
                }

                Item { height: ScreenTools.defaultFontPixelHeight; width: 10 } // spacer

                Item {
                    id:     centerPanel
                    width:  parent.width
                    height: parent.height - y
                    visible: controller.calibrationActive || controller.showOrientationCalArea || statusTextArea.text.length > 0

                    TextArea {
                        id:             statusTextArea
                        objectName:     "sensorsSetup_statusLog"
                        anchors.fill:   parent
                        readOnly:       true
                        visible:        !orientationCalArea.visible && text.length > 0
                        text:           statusTextAreaDefaultText
                        color:          qgcPal.text
                        background:     Rectangle { color: qgcPal.windowShade }
                    }

                    Rectangle {
                        id:             orientationCalArea
                        anchors.fill:   parent
                        visible:        controller.showOrientationCalArea
                        color:          qgcPal.windowShade

                        QGCLabel {
                            id:                 orientationCalAreaHelpText
                            anchors.margins:    ScreenTools.defaultFontPixelWidth
                            anchors.top:        orientationCalArea.top
                            anchors.left:       orientationCalArea.left
                            width:              parent.width
                            wrapMode:           Text.WordWrap
                            font.pointSize:     ScreenTools.mediumFontPointSize
                        }

                        SensorCalibrationPreview {
                            objectName: "sensorCalibrationPreview"
                            visible: QGroundControl.corePlugin.directMissionItemEditing
                            anchors.top: orientationCalAreaHelpText.bottom
                            anchors.bottom: parent.bottom
                            anchors.left: parent.left
                            anchors.right: parent.right
                            anchors.margins: ScreenTools.defaultFontPixelWidth / 2
                            sideStates: [sideCalState(controller.orientationCalDownSideDone, controller.orientationCalDownSideInProgress),
                                sideCalState(controller.orientationCalLeftSideDone, controller.orientationCalLeftSideInProgress),
                                sideCalState(controller.orientationCalRightSideDone, controller.orientationCalRightSideInProgress),
                                sideCalState(controller.orientationCalNoseDownSideDone, controller.orientationCalNoseDownSideInProgress),
                                sideCalState(controller.orientationCalTailDownSideDone, controller.orientationCalTailDownSideInProgress),
                                sideCalState(controller.orientationCalUpsideDownSideDone, controller.orientationCalUpsideDownSideInProgress)]
                            sideVisible: [controller.orientationCalDownSideVisible, controller.orientationCalLeftSideVisible,
                                controller.orientationCalRightSideVisible, controller.orientationCalNoseDownSideVisible,
                                controller.orientationCalTailDownSideVisible, controller.orientationCalUpsideDownSideVisible]
                            sideRotating: [controller.orientationCalDownSideRotate, controller.orientationCalLeftSideRotate,
                                controller.orientationCalRightSideRotate, controller.orientationCalNoseDownSideRotate,
                                controller.orientationCalTailDownSideRotate, controller.orientationCalUpsideDownSideRotate]
                        }

                        Flow {
                            visible: !QGroundControl.corePlugin.directMissionItemEditing
                            anchors.topMargin:  ScreenTools.defaultFontPixelWidth
                            anchors.top:        orientationCalAreaHelpText.bottom
                            anchors.bottom:     parent.bottom
                            anchors.left:       parent.left
                            anchors.right:      parent.right
                            spacing:            ScreenTools.defaultFontPixelWidth

                            property real indicatorWidth:   (width / 3) - (spacing * 2)
                            property real indicatorHeight:  (height / 2) - spacing

                            VehicleRotationCal {
                                objectName:         "sensorsCal_downSide"
                                width:              parent.indicatorWidth
                                height:             parent.indicatorHeight
                                visible:            controller.orientationCalDownSideVisible
                                calState:           sideCalState(controller.orientationCalDownSideDone, controller.orientationCalDownSideInProgress)
                                calInProgressText:  controller.orientationCalDownSideRotate ? qsTr("Rotate") : qsTr("Hold Still")
                                imageSource:        "qrc:///qmlimages/VehicleDown.png"
                            }
                            VehicleRotationCal {
                                objectName:         "sensorsCal_leftSide"
                                width:              parent.indicatorWidth
                                height:             parent.indicatorHeight
                                visible:            controller.orientationCalLeftSideVisible
                                calState:           sideCalState(controller.orientationCalLeftSideDone, controller.orientationCalLeftSideInProgress)
                                calInProgressText:  controller.orientationCalLeftSideRotate ? qsTr("Rotate") : qsTr("Hold Still")
                                imageSource:        "qrc:///qmlimages/VehicleLeft.png"
                            }
                            VehicleRotationCal {
                                objectName:         "sensorsCal_rightSide"
                                width:              parent.indicatorWidth
                                height:             parent.indicatorHeight
                                visible:            controller.orientationCalRightSideVisible
                                calState:           sideCalState(controller.orientationCalRightSideDone, controller.orientationCalRightSideInProgress)
                                calInProgressText:  controller.orientationCalRightSideRotate ? qsTr("Rotate") : qsTr("Hold Still")
                                imageSource:        "qrc:///qmlimages/VehicleRight.png"
                            }
                            VehicleRotationCal {
                                objectName:         "sensorsCal_noseDownSide"
                                width:              parent.indicatorWidth
                                height:             parent.indicatorHeight
                                visible:            controller.orientationCalNoseDownSideVisible
                                calState:           sideCalState(controller.orientationCalNoseDownSideDone, controller.orientationCalNoseDownSideInProgress)
                                calInProgressText:  controller.orientationCalNoseDownSideRotate ? qsTr("Rotate") : qsTr("Hold Still")
                                imageSource:        "qrc:///qmlimages/VehicleNoseDown.png"
                            }
                            VehicleRotationCal {
                                objectName:         "sensorsCal_tailDownSide"
                                width:              parent.indicatorWidth
                                height:             parent.indicatorHeight
                                visible:            controller.orientationCalTailDownSideVisible
                                calState:           sideCalState(controller.orientationCalTailDownSideDone, controller.orientationCalTailDownSideInProgress)
                                calInProgressText:  controller.orientationCalTailDownSideRotate ? qsTr("Rotate") : qsTr("Hold Still")
                                imageSource:        "qrc:///qmlimages/VehicleTailDown.png"
                            }
                            VehicleRotationCal {
                                objectName:         "sensorsCal_upsideDownSide"
                                width:              parent.indicatorWidth
                                height:             parent.indicatorHeight
                                visible:            controller.orientationCalUpsideDownSideVisible
                                calState:           sideCalState(controller.orientationCalUpsideDownSideDone, controller.orientationCalUpsideDownSideInProgress)
                                calInProgressText:  controller.orientationCalUpsideDownSideRotate ? qsTr("Rotate") : qsTr("Hold Still")
                                imageSource:        "qrc:///qmlimages/VehicleUpsideDown.png"
                            }
                        }
                    }
                } // Item - Cal display area
            } // Column - cal display
        } // Row
    } // Component - sensorsPageComponent
} // SetupPage
