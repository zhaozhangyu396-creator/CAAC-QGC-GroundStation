import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import QGroundControl
import QGroundControl.Controls

Rectangle {
    id:             vehicleConfigView
    objectName:     "vehicleConfig_root"
    color:          qgcPal.window
    z:      QGroundControl.zOrderTopMost
    property bool embedded: false
    property bool _embeddedPageOpened: false
    readonly property real navigationWidth: Math.max(ScreenTools.defaultFontPixelWidth * 22, buttonColumn.implicitWidth) + _horizontalMargin * 2
    property string _offlinePageTitle: ""
    readonly property var _peripheralPages: [
        {key: "gps", label: qsTr("GPS 设置"), icon: "/InstrumentValueIcons/globe.svg"},
        {key: "radar", label: qsTr("雷达设置"), icon: "/qmlimages/SensorsComponentIcon.png"},
        {key: "trim", label: qsTr("飞控微调"), icon: "/qmlimages/TuningComponentIcon.png"}
    ]
    readonly property var _offlineFunctions: [
        ["AirframeComponent", "Airframe", "/qmlimages/AirframeComponentIcon.png"],
        ["SensorsComponent", "Sensors", "/qmlimages/SensorsComponentIcon.png"],
        ["PX4RadioComponent", "Radio", "/qmlimages/RadioComponentIcon.png"],
        ["FlightModesComponent", "Flight Modes", "/qmlimages/FlightModesComponentIcon.png"],
        ["Peripheral", "gps", "/InstrumentValueIcons/globe.svg"],
        ["Peripheral", "radar", "/qmlimages/SensorsComponentIcon.png"],
        ["Peripheral", "trim", "/qmlimages/TuningComponentIcon.png"],
        ["MotorComponent", "Motors", "/qmlimages/MotorComponentIcon.svg"],
        ["PowerComponent", "Power", "/qmlimages/Battery.svg"],
        ["SafetyComponent", "Safety", "/qmlimages/SafetyComponentIcon.png"],
        ["PX4TuningComponent", "PID Tuning", "/qmlimages/TuningComponentIcon.png"],
        ["JoystickComponent", "Joystick", "/qmlimages/Joystick.png"],
        ["PX4FlightBehavior", "Flight Behavior", "/qmlimages/TuningComponentIcon.png"],
        ["VehicleConfigView", "Parameters", "/qmlimages/subMenuButtonImage.png"]
    ]

    function showNavigation() {
        if (mainWindow.allowViewSwitch()) { _embeddedPageOpened = false }
    }

    function showOfflinePage(entry) {
        if (entry[0] === "Peripheral") { showPeripheralPage(entry[1]); return }
        if (!mainWindow.allowViewSwitch()) { return }
        _embeddedPageOpened = true
        _selectedComponentIndex = -1
        _selectedSectionIndex = -1
        _selectedSpecial = "offline"
        _offlinePageTitle = qsTranslate(entry[0], entry[1])
        _messagePanelText = qsTr("请连接 PX4 飞控并完成参数读取后再设置此项。")
        panelLoader.setSourceComponent(entry[1] === "Airframe" ? airframeCatalogComponent
            : entry[1] === "Sensors" ? offlineSensorsComponent : messagePanelComponent)
    }

    function showPeripheralPage(key) {
        if (!mainWindow.allowViewSwitch()) { return }
        _embeddedPageOpened = true
        _selectedSpecial = key
        _selectedComponentIndex = -1
        _selectedSectionIndex = -1
        panelLoader.setSourceComponent(peripheralPageComponent)
    }

    // This need to block click event leakage to underlying map.
    DeadMouseArea {
        anchors.fill: parent
    }

    QGCPalette { id: qgcPal; colorGroupEnabled: true }

    readonly property real      _defaultTextHeight: ScreenTools.defaultFontPixelHeight
    readonly property real      _defaultTextWidth:  ScreenTools.defaultFontPixelWidth
    readonly property real      _horizontalMargin:  _defaultTextWidth / 2
    readonly property real      _verticalMargin:    _defaultTextHeight / 2
    readonly property real      _buttonWidth:       _defaultTextWidth * 18
    readonly property bool      _caacStyle:         QGroundControl.corePlugin.directMissionItemEditing
    readonly property string    _armedVehicleText:  qsTr("This operation cannot be performed while the vehicle is armed.")

    property var    _activeVehicle:                 QGroundControl.multiVehicleManager.activeVehicle
    property bool   _vehicleArmed:                  _activeVehicle ? _activeVehicle.armed : false
    property string _messagePanelText:              qsTr("missing message panel text")
    property bool   _fullParameterVehicleAvailable: _activeVehicle && QGroundControl.multiVehicleManager.parameterReadyVehicleAvailable && !_activeVehicle.parameterManager.missingParameters
    property var    _corePlugin:                    QGroundControl.corePlugin

    // Tree view state
    property int    _selectedComponentIndex: -1     // -1 = summary or special button
    property int    _selectedSectionIndex:   -1
    property string _selectedSpecial:        ""     // "summary", "parameters", "firmware", "opticalflow"
    property bool   _showingPrereqMessage:   false  // Panel area shows prerequisite message instead of selected component
    property var    _expandedComponents:     ({})
    property var    _expandedSearchComponents: ({})
    property string _searchQuery:            ""

    on_SearchQueryChanged: _expandedSearchComponents = ({})

    function _orderedComponents() {
        if (!_fullParameterVehicleAvailable) { return [] }
        const components = _activeVehicle.autopilotPlugin.vehicleComponents
        const order = ["AirframeComponent", "APMAirframeComponent", "SensorsComponent", "APMSensorsComponent",
            "PX4RadioComponent", "APMRadioComponent", "APMESCComponent", "FlightModesComponent", "APMFlightModesComponent",
            "APMServoComponent", "ActuatorComponent", "MotorComponent", "APMMotorComponent", "PowerComponent", "APMPowerComponent",
            "SafetyComponent", "APMFailsafesComponent", "PX4TuningComponent", "JoystickComponent", "PX4FlightBehavior"]
        const entries = components.map((component, index) => ({component: component, componentIndex: index}))
        if (_caacStyle) {
            entries.sort((left, right) => {
                const leftOrder = order.indexOf(left.component.toString().split("(")[0])
                const rightOrder = order.indexOf(right.component.toString().split("(")[0])
                return (leftOrder < 0 ? order.length : leftOrder) - (rightOrder < 0 ? order.length : rightOrder)
            })
            const flightModes = entries.findIndex(entry =>
                ["FlightModesComponent", "APMFlightModesComponent"].indexOf(entry.component.toString().split("(")[0]) >= 0)
            const insertion = flightModes >= 0 ? flightModes + 1 : Math.min(3, entries.length)
            entries.splice(insertion, 0, ..._peripheralPages.map(page => ({component: null, componentIndex: -1, page: page})))
        }
        return entries
    }

    function _setExpanded(compIndex, value) {
        const searching = _searchQuery.trim() !== ""
        const states = Object.assign({}, searching ? _expandedSearchComponents : _expandedComponents)
        states[compIndex] = value
        if (searching) {
            _expandedSearchComponents = states
        } else {
            _expandedComponents = states
        }
    }

    function _isExpanded(compIndex) {
        if (_searchQuery.trim() !== "") {
            return _expandedSearchComponents[compIndex] !== false
        }
        return _expandedComponents[compIndex] === true
    }

    /// Translated display name for a section ID. JSON-driven components translate via the JSON
    /// filename context; hand-coded components provide sectionDisplayName().
    function _sectionDisplayName(component, sectionId) {
        let context = _translationContext(component)
        if (context) {
            return qsTranslate(context, sectionId)
        }
        if (component && typeof component.sectionDisplayName === "function") {
            return component.sectionDisplayName(sectionId)
        }
        return sectionId
    }

    /// Get the section ID for a sidebar entry.
    function _sectionId(compIndex, sectionIndex) {
        if (sectionIndex < 0 || !_fullParameterVehicleAvailable) return ""
        var components = _activeVehicle.autopilotPlugin.vehicleComponents
        if (compIndex < 0 || compIndex >= components.length) return ""
        var secs = components[compIndex].sectionIds
        if (sectionIndex < secs.length) return secs[sectionIndex]
        return ""
    }

    /// Extract the translation context (JSON filename) from a component.
    function _translationContext(component) {
        if (!component || !component.vehicleConfigJson) return ""
        var path = component.vehicleConfigJson.toString()
        var slash = path.lastIndexOf("/")
        return slash >= 0 ? path.substring(slash + 1) : path
    }

    function _componentMatchesSearch(component) {
        if (_searchQuery.trim() === "") return true
        var query = _searchQuery.toLowerCase().trim()
        if (component.name.toLowerCase().indexOf(query) !== -1) return true
        if (ScreenTools.vehicleComponentName(component).toLowerCase().indexOf(query) !== -1) return true
        var context = _translationContext(component)
        var secs = component.sectionIds
        if (secs) {
            for (var i = 0; i < secs.length; i++) {
                if (secs[i].toLowerCase().indexOf(query) !== -1) return true
                let displayName = _sectionDisplayName(component, secs[i])
                if (displayName !== secs[i] && displayName.toLowerCase().indexOf(query) !== -1) {
                    return true
                }
            }
        }
        var keywords = component.sectionKeywords
        if (keywords) {
            for (var key in keywords) {
                var terms = keywords[key]
                for (var j = 0; j < terms.length; j++) {
                    if (terms[j].toLowerCase().indexOf(query) !== -1) return true
                    if (context && qsTranslate(context, terms[j]).toLowerCase().indexOf(query) !== -1) return true
                }
            }
        }
        return false
    }

    function _sectionMatchesSearch(component, sectionId) {
        if (_searchQuery.trim() === "") return true
        var query = _searchQuery.toLowerCase().trim()
        if (sectionId.toLowerCase().indexOf(query) !== -1) return true
        var context = _translationContext(component)
        let displayName = _sectionDisplayName(component, sectionId)
        if (displayName !== sectionId && displayName.toLowerCase().indexOf(query) !== -1) {
            return true
        }
        var keywords = component.sectionKeywords
        if (keywords && keywords[sectionId]) {
            var terms = keywords[sectionId]
            for (var i = 0; i < terms.length; i++) {
                if (terms[i].toLowerCase().indexOf(query) !== -1) return true
                if (context && qsTranslate(context, terms[i]).toLowerCase().indexOf(query) !== -1) return true
            }
        }
        return false
    }

    function _isAirframeComponent(component) {
        if (!component) { return false }
        const className = component.toString().split("(")[0]
        return className === "AirframeComponent" || className === "APMAirframeComponent"
    }

    function _isMergedSafetyComponent(component) {
        return _caacStyle && !!component && ["APMFailsafesComponent", "APMFlightSafetyComponent"].indexOf(
            component.toString().split("(")[0]) >= 0
    }

    function _componentVisible(component) {
        if (_isMergedSafetyComponent(component)) { return false }
        if (!component || (component.setupSource.toString() === "" && !(_caacStyle && _isAirframeComponent(component)))) {
            return false
        }
        return _searchQuery.trim() === "" || _componentMatchesSearch(component)
    }

    function _anyComponentVisible() {
        if (!_fullParameterVehicleAvailable) {
            return false
        }
        return _activeVehicle.autopilotPlugin.vehicleComponents.some(_componentVisible)
    }

    function showSummaryPanel() {
        if (mainWindow.allowViewSwitch()) {
            _embeddedPageOpened = true
            _showSummaryPanel()
        }
    }

    function _showSummaryPanel() {
        _selectedSpecial = "summary"
        _selectedComponentIndex = -1
        _selectedSectionIndex = -1
        if (_fullParameterVehicleAvailable) {
            if (_activeVehicle.autopilotPlugin.vehicleComponents.length === 0) {
                panelLoader.setSourceComponent(noComponentsVehicleSummaryComponent)
            } else {
                panelLoader.setSource("qrc:/qml/QGroundControl/VehicleSetup/VehicleSummary.qml")
            }
        } else if (QGroundControl.multiVehicleManager.parameterReadyVehicleAvailable) {
            panelLoader.setSourceComponent(missingParametersVehicleSummaryComponent)
        } else {
            panelLoader.setSourceComponent(disconnectedVehicleAndParamsSummaryComponent)
        }
    }

    function showPanel(specialName, qmlSource) {
        if (mainWindow.allowViewSwitch()) {
            _embeddedPageOpened = true
            _selectedSpecial = specialName
            _selectedComponentIndex = -1
            _selectedSectionIndex = -1
            panelLoader.setSource(qmlSource)
        }
    }

    function _navigateToComponent(compIndex, sectionIndex) {
        if (!mainWindow.allowViewSwitch()) return
        if (!_fullParameterVehicleAvailable) return
        _embeddedPageOpened = true

        var components = _activeVehicle.autopilotPlugin.vehicleComponents
        if (compIndex < 0 || compIndex >= components.length) return
        var vehicleComponent = components[compIndex]

        if (_isMergedSafetyComponent(vehicleComponent)) {
            mainWindow.caacSettingsPage = "Safety"
            return
        }

        _selectedSpecial = ""

        // If component opts in and root was clicked, auto-select first section
        if (sectionIndex < 0 && vehicleComponent.showFirstSectionOnRootClick && vehicleComponent.sectionIds.length > 0) {
            sectionIndex = 0
            if (_caacStyle) {
                const orientationIndex = vehicleComponent.sectionIds.indexOf("Orientations")
                if (orientationIndex >= 0) { sectionIndex = orientationIndex }
            }
        }
        _selectedSectionIndex = sectionIndex

        if (_caacStyle && _isAirframeComponent(vehicleComponent) && vehicleComponent.setupSource.toString() === "") {
            _selectedComponentIndex = compIndex
            _showingPrereqMessage = false
            panelLoader.setSourceComponent(airframeCatalogComponent, vehicleComponent)
            return
        }

        var autopilotPlugin = _activeVehicle.autopilotPlugin
        var prereq = autopilotPlugin.prerequisiteSetup(vehicleComponent)
        if (prereq !== "") {
            // Selection state still updates so the tree expands and highlights
            // normally; only the panel area shows the prerequisite message
            _selectedComponentIndex = compIndex
            _showingPrereqMessage = true
            _messagePanelText = qsTr("%1 setup must be completed prior to %2 setup.").arg(prereq).arg(vehicleComponent.name)
            panelLoader.setSourceComponent(messagePanelComponent)
            return
        }

        if (_selectedComponentIndex !== compIndex || _showingPrereqMessage) {
            _selectedComponentIndex = compIndex
            _showingPrereqMessage = false
            panelLoader.setSource(vehicleComponent.setupSource, vehicleComponent)
        }

        // Apply section filter
        if (panelLoader.item && typeof panelLoader.item.sectionIdFilter !== "undefined") {
            panelLoader.item.sectionIdFilter = _sectionId(compIndex, sectionIndex)
        }
    }

    function showParametersPanel() {
        showPanel("parameters", "qrc:/qml/QGroundControl/VehicleSetup/SetupParameterEditor.qml")
    }

    function showVehicleComponentPanel(vehicleComponent) {
        if (!mainWindow.allowViewSwitch()) return
        if (!_fullParameterVehicleAvailable) return

        var components = _activeVehicle.autopilotPlugin.vehicleComponents
        for (var i = 0; i < components.length; i++) {
            if (components[i] === vehicleComponent) {
                _navigateToComponent(i, -1)
                return
            }
        }
    }

    Component.onCompleted: _showSummaryPanel()

    Connections {
        target: QGroundControl.corePlugin
        function onShowAdvancedUIChanged(showAdvancedUI) {
            if (!showAdvancedUI) {
                _showSummaryPanel()
            }
        }
    }

    Connections {
        target: QGroundControl.multiVehicleManager
        function onParameterReadyVehicleAvailableChanged(parametersReady) {
            if (parametersReady || _selectedSpecial === "summary" || _selectedSpecial !== "firmware") {
                _showSummaryPanel()
            }
        }
    }

    Connections {
        target: panelLoader
        function onLoaded() {
            if (panelLoader.item && typeof panelLoader.item.sectionIdFilter !== "undefined") {
                panelLoader.item.sectionIdFilter = _sectionId(_selectedComponentIndex, _selectedSectionIndex)
            }
        }
    }

    Component {
        id: noComponentsVehicleSummaryComponent
        Rectangle {
            color: qgcPal.windowShade
            QGCLabel {
                anchors.margins:        _defaultTextWidth * 2
                anchors.fill:           parent
                verticalAlignment:      Text.AlignVCenter
                horizontalAlignment:    Text.AlignHCenter
                wrapMode:               Text.WordWrap
                font.pointSize:         ScreenTools.mediumFontPointSize
                text:                   qsTr("%1 does not currently support configuration of your vehicle. ").arg(QGroundControl.appName) +
                                        "If your vehicle is already configured you can still Fly."
            }
        }
    }

    Component {
        id: disconnectedVehicleAndParamsSummaryComponent
        Rectangle {
            id: disconnectedRect
            color: qgcPal.windowShade
            Column {
                anchors.centerIn:   parent
                spacing:            ScreenTools.defaultFontPixelHeight
                QGCLabel {
                    anchors.horizontalCenter: parent.horizontalCenter
                    width:              disconnectedRect.width - _defaultTextWidth * 4
                    horizontalAlignment: Text.AlignHCenter
                    wrapMode:           Text.WordWrap
                    font.pointSize:     ScreenTools.largeFontPointSize
                    text:               !_activeVehicle
                                            ? qsTr("Vehicle configuration pages will display after you connect your vehicle and parameters have been downloaded.")
                                            : (_activeVehicle.parameterManager.parameterDownloadSkipped
                                                ? qsTr("Parameter download was skipped because the vehicle is armed. Configuration pages will be available after parameters are downloaded.")
                                                : qsTr("Waiting for vehicle parameters to download…"))
                }
                QGCButton {
                    anchors.horizontalCenter: parent.horizontalCenter
                    text:       qsTr("Download Parameters")
                    visible:    _activeVehicle && _activeVehicle.parameterManager.parameterDownloadSkipped
                    enabled:    _activeVehicle && _activeVehicle.parameterManager.parameterDownloadSkipped && _activeVehicle.parameterManager.loadProgress === 0
                    onClicked:  _activeVehicle.parameterManager.refreshAllParameters()
                }
            }
        }
    }

    Component {
        id: missingParametersVehicleSummaryComponent

        Rectangle {
            color: qgcPal.windowShade

            QGCLabel {
                anchors.margins:        _defaultTextWidth * 2
                anchors.fill:           parent
                verticalAlignment:      Text.AlignVCenter
                horizontalAlignment:    Text.AlignHCenter
                wrapMode:               Text.WordWrap
                font.pointSize:         ScreenTools.mediumFontPointSize
                text:                   qsTr("Vehicle did not return the full parameter list. ") +
                                        qsTr("As a result, the configuration pages are not available.")
            }
        }
    }

    Component {
        id: messagePanelComponent

        Item {
            objectName: "vehicleConfig_messagePanel"

            QGCLabel {
                anchors.margins:        _defaultTextWidth * 2
                anchors.fill:           parent
                verticalAlignment:      Text.AlignVCenter
                horizontalAlignment:    Text.AlignHCenter
                wrapMode:               Text.WordWrap
                font.pointSize:         ScreenTools.mediumFontPointSize
                text:                   _messagePanelText
            }
        }
    }

    Component {
        id: airframeCatalogComponent
        AirframeCatalogPage {
            connected: vehicleConfigView._fullParameterVehicleAvailable
        }
    }

    Component {
        id: offlineSensorsComponent
        SensorCalibrationPanel {
            vehicle: null
            sectionId: "Orientations"
            calibrationEnabled: false
            levelEnabled: false
            sideStates: [0, 0, 0, 0, 0, 0]
            sideVisible: [true, true, true, true, true, true]
            sideRotating: [false, false, false, false, false, false]
        }
    }

    Component {
        id: peripheralPageComponent
        VehiclePeripheralSetup { page: vehicleConfigView._selectedSpecial }
    }

    ColumnLayout {
        id:                 leftPanel
        width:              embedded ? parent.width - _horizontalMargin * 2
                                : Math.max(buttonColumn.implicitWidth + _horizontalMargin, ScreenTools.defaultFontPixelWidth * 22)
        visible:            !embedded || !_embeddedPageOpened
        anchors.topMargin:  _verticalMargin
        anchors.top:        parent.top
        anchors.bottom:     parent.bottom
        anchors.leftMargin: _horizontalMargin
        anchors.left:       parent.left
        spacing:            _verticalMargin / 2

        QGCLabel {
            Layout.fillWidth: true
            Layout.leftMargin: _horizontalMargin
            Layout.topMargin: _verticalMargin
            Layout.bottomMargin: _verticalMargin
            text: qsTr("无人机设置")
            font.pointSize: ScreenTools.mediumFontPointSize
            font.bold: true
            visible: vehicleConfigView._caacStyle
        }

        QGCTextField {
            id:                 searchField
            objectName:         "vehicleConfig_searchField"
            Layout.fillWidth:   true
            placeholderText:    qsTr("Search configuration...")
            visible:            _fullParameterVehicleAvailable

            onTextChanged: {
                vehicleConfigView._searchQuery = text
            }
        }

        QGCFlickable {
            objectName:         "vehicleConfig_sidebarFlickable"
            Layout.fillWidth:   true
            Layout.fillHeight:  true
            contentHeight:      buttonColumn.height + _verticalMargin
            flickableDirection:  Flickable.VerticalFlick
            clip:               true

            ColumnLayout {
                id:         buttonColumn
                width:      parent.width
                spacing:    vehicleConfigView.embedded ? ScreenTools.defaultFontPixelHeight * 0.5 : 0

                // Summary button
                ConfigButton {
                    id:                 summaryButton
                    objectName:         "vehicleConfig_summary"
                    icon.source:        "/qmlimages/VehicleSummaryIcon.png"
                    checked:            vehicleConfigView._selectedSpecial === "summary"
                    outlined:           vehicleConfigView.embedded
                    text:               qsTr("Summary")
                    Layout.fillWidth:   true
                    visible:            vehicleConfigView._searchQuery.trim() === ""

                    onClicked: showSummaryPanel()
                }

                // When the component group is empty only this divider shows, not the second one
                SidebarDivider {
                    objectName: "vehicleConfig_summaryDivider"
                    visible:    !vehicleConfigView.embedded && summaryButton.visible &&
                                (vehicleConfigView._anyComponentVisible() || opticalFlowButton.visible ||
                                 parametersButton.visible || firmwareButton.visible)
                }

                // Vehicle component tree
                Repeater {
                    id:     componentRepeater
                    model:  vehicleConfigView._orderedComponents()

                    ColumnLayout {
                        id:             compColumn
                        spacing:        vehicleConfigView.embedded ? ScreenTools.defaultFontPixelHeight * 0.5 : 0
                        Layout.fillWidth: true

                        required property int index
                        required property var modelData
                        readonly property int componentIndex: modelData.componentIndex

                        property var    comp:           modelData.component
                        property string compName:       comp ? comp.name : ""
                        property var    compSectionIds: comp ? comp.sectionIds : []
                        property bool   isSelected:     modelData.page ? vehicleConfigView._selectedSpecial === modelData.page.key
                            : vehicleConfigView._selectedComponentIndex === componentIndex && vehicleConfigView._selectedSpecial === ""
                        property bool   hasSections:    compSectionIds.length > 1
                        property bool   isSearching:    vehicleConfigView._searchQuery.trim() !== ""
                        property bool   matchesSearch:  comp ? vehicleConfigView._componentMatchesSearch(comp) : false
                        property bool   isExpanded:     hasSections && (!isSearching || matchesSearch)
                                                        && vehicleConfigView._isExpanded(componentIndex)

                        visible: modelData.page ? vehicleConfigView._searchQuery.trim() === ""
                            || modelData.page.label.toLowerCase().indexOf(vehicleConfigView._searchQuery.trim().toLowerCase()) >= 0
                            : vehicleConfigView._componentVisible(comp)

                        ConfigButton {
                            Layout.fillWidth:   true
                            objectName:         compColumn.modelData.page ? "vehicleConfig_" + compColumn.modelData.page.key
                                : "vehicleConfig_comp_" + compColumn.compName.replace(/ /g, "")
                            icon.source:        compColumn.modelData.page ? compColumn.modelData.page.icon
                                : compColumn.comp ? compColumn.comp.iconResource : ""
                            setupComplete:      compColumn.comp ? compColumn.comp.setupComplete : true
                            text:               compColumn.modelData.page ? compColumn.modelData.page.label
                                : ScreenTools.vehicleComponentName(compColumn.comp)
                            expandable:         compColumn.hasSections
                            expanded:           compColumn.isExpanded
                            checked:            compColumn.isSelected && vehicleConfigView._selectedSectionIndex === -1
                            outlined:           vehicleConfigView.embedded

                            onClicked: {
                                if (compColumn.modelData.page) {
                                    vehicleConfigView.showPeripheralPage(compColumn.modelData.page.key)
                                    return
                                }
                                if (compColumn.hasSections && compColumn.isSelected) {
                                    vehicleConfigView._setExpanded(compColumn.componentIndex, !compColumn.isExpanded)
                                    return
                                }
                                if (!mainWindow.allowViewSwitch()) { return }
                                vehicleConfigView._navigateToComponent(compColumn.componentIndex, -1)
                                if (compColumn.hasSections) {
                                    vehicleConfigView._setExpanded(compColumn.componentIndex, true)
                                }
                            }

                            onToggleExpand: {
                                vehicleConfigView._setExpanded(compColumn.componentIndex, !compColumn.isExpanded)
                            }
                        }

                        // Section sub-items
                        Repeater {
                            model: compColumn.isExpanded ? compColumn.compSectionIds : []

                            Button {
                                id:             sectionBtn
                                objectName:     "vehicleConfig_section_" + modelData.replace(/ /g, "")
                                Layout.fillWidth: true
                                padding:        ScreenTools.defaultFontPixelWidth * 0.75
                                topPadding:     vehicleConfigView.embedded ? ScreenTools.defaultFontPixelHeight * 0.5 : padding
                                bottomPadding:  vehicleConfigView.embedded ? ScreenTools.defaultFontPixelHeight * 0.5 : padding
                                leftPadding:    ScreenTools.defaultFontPixelWidth * 3
                                hoverEnabled:   !ScreenTools.isMobile

                                property int sectionIndex: index
                                property bool sectionChecked: compColumn.isSelected && vehicleConfigView._selectedSectionIndex === sectionIndex
                                property bool sectionSetupComplete: {
                                    if (!compColumn.comp) {
                                        return true
                                    }
                                    // Referencing comp.setupComplete re-evaluates this binding whenever a
                                    // setup trigger parameter changes, since sectionSetupComplete() is a
                                    // plain function call which QML cannot otherwise track
                                    void compColumn.comp.setupComplete
                                    return typeof compColumn.comp.sectionSetupComplete === "function"
                                               ? compColumn.comp.sectionSetupComplete(modelData)
                                               : true
                                }
                                property bool sectionMatchesSearch: {
                                    if (!compColumn.isSearching) return true
                                    return vehicleConfigView._sectionMatchesSearch(compColumn.comp, modelData)
                                }
                                property bool sectionContentVisible: {
                                    if (!compColumn.isSelected) return true
                                    if (!panelLoader.item) return true
                                    if (typeof panelLoader.item.sectionVisible !== "function") return true
                                    return panelLoader.item.sectionVisible(modelData)
                                }
                                property color textColor: vehicleConfigView._caacStyle && (sectionChecked || pressed) ? qgcPal.text
                                    : sectionChecked || pressed ? qgcPal.buttonHighlightText : qgcPal.buttonText
                                visible: sectionMatchesSearch && sectionContentVisible

                                background: Rectangle {
                                    color: vehicleConfigView.embedded ? "transparent"
                                        : vehicleConfigView._caacStyle ? qgcPal.windowShade : qgcPal.buttonHighlight
                                    opacity: vehicleConfigView.embedded ? 1
                                        : sectionBtn.sectionChecked || sectionBtn.pressed ? 1 : sectionBtn.enabled && sectionBtn.hovered ? 0.2 : 0
                                    border.color: qgcPal.groupBorder
                                    border.width: vehicleConfigView.embedded ? 1 : 0
                                    radius: vehicleConfigView.embedded ? ScreenTools.defaultBorderRadius
                                        : ScreenTools.defaultFontPixelWidth / 2

                                    Rectangle {
                                        anchors.fill: parent
                                        anchors.margins: parent.border.width
                                        visible: vehicleConfigView.embedded
                                        color: vehicleConfigView._caacStyle ? qgcPal.windowShade : qgcPal.buttonHighlight
                                        opacity: sectionBtn.sectionChecked || sectionBtn.pressed ? 1
                                            : sectionBtn.enabled && sectionBtn.hovered ? 0.2 : 0
                                        radius: Math.max(0, parent.radius - parent.border.width)
                                    }
                                }

                                contentItem: RowLayout {
                                    spacing: ScreenTools.defaultFontPixelWidth * 0.5

                                    Rectangle {
                                        width:   ScreenTools.defaultFontPixelWidth
                                        height:  width
                                        radius:  width / 2
                                        color:   sectionBtn.sectionSetupComplete ? qgcPal.colorGreen : qgcPal.colorOrange
                                        visible: !sectionBtn.sectionSetupComplete
                                    }

                                    QGCLabel {
                                        text:  vehicleConfigView._sectionDisplayName(compColumn.comp, modelData)
                                        color: sectionBtn.textColor
                                        font.pointSize: ScreenTools.defaultFontPointSize * 0.9
                                        horizontalAlignment: Text.AlignLeft
                                        Layout.fillWidth: true
                                    }
                                }

                                onClicked: {
                                    vehicleConfigView._navigateToComponent(compColumn.componentIndex, sectionIndex)
                                }
                            }
                        }
                    }
                }

                Repeater {
                    model: vehicleConfigView.embedded && !vehicleConfigView._fullParameterVehicleAvailable
                        ? vehicleConfigView._offlineFunctions : []
                    ConfigButton {
                        required property var modelData
                        objectName: modelData[0] === "Peripheral" ? "vehicleConfig_" + modelData[1]
                            : "vehicleConfig_offline_" + modelData[1].replace(/ /g, "")
                        Layout.fillWidth: true
                        text: modelData[0] === "Peripheral"
                            ? vehicleConfigView._peripheralPages.find(page => page.key === modelData[1]).label
                            : qsTranslate(modelData[0], modelData[1])
                        icon.source: modelData[2]
                        setupComplete: false
                        outlined: vehicleConfigView.embedded
                        onClicked: vehicleConfigView.showOfflinePage(modelData)
                    }
                }

                // Optical Flow (special)
                ConfigButton {
                    id:                 opticalFlowButton
                    visible:            _activeVehicle ? _activeVehicle.flowImageIndex > 0 : false
                    text:               qsTr("Optical Flow")
                    Layout.fillWidth:   true
                    checked:            vehicleConfigView._selectedSpecial === "opticalflow"
                    outlined:           vehicleConfigView.embedded
                    onClicked:          showPanel("opticalflow", "qrc:/qml/QGroundControl/VehicleSetup/OpticalFlowSensor.qml")
                }

                SidebarDivider {
                    objectName: "vehicleConfig_componentsDivider"
                    visible:    !vehicleConfigView.embedded &&
                                (vehicleConfigView._anyComponentVisible() || opticalFlowButton.visible) &&
                                (parametersButton.visible || firmwareButton.visible)
                }

                ConfigButton {
                    id:                 parametersButton
                    objectName:         "vehicleConfig_parametersButton"
                    visible:            QGroundControl.multiVehicleManager.parameterReadyVehicleAvailable &&
                                        !_activeVehicle.usingHighLatencyLink &&
                                        _corePlugin.showAdvancedUI &&
                                        vehicleConfigView._searchQuery.trim() === ""
                    text:               qsTr("Parameters")
                    Layout.fillWidth:   true
                    icon.source:        "/qmlimages/subMenuButtonImage.png"
                    checked:            vehicleConfigView._selectedSpecial === "parameters"
                    outlined:           vehicleConfigView.embedded
                    onClicked:          showPanel("parameters", "qrc:/qml/QGroundControl/VehicleSetup/SetupParameterEditor.qml")
                }

                ConfigButton {
                    id:                 firmwareButton
                    objectName:         "vehicleConfig_firmwareButton"
                    icon.source:        "/qmlimages/FirmwareUpgradeIcon.png"
                    visible:            !ScreenTools.isMobile && _corePlugin.options.showFirmwareUpgrade &&
                                        vehicleConfigView._searchQuery.trim() === ""
                    text:               qsTr("Firmware")
                    Layout.fillWidth:   true
                    checked:            vehicleConfigView._selectedSpecial === "firmware"
                    outlined:           vehicleConfigView.embedded

                    onClicked: showPanel("firmware", "qrc:/qml/QGroundControl/VehicleSetup/FirmwareUpgrade.qml")
                }
            }
        }
    }

    Rectangle {
        id:                     divider
        visible:                !embedded
        anchors.topMargin:      _verticalMargin
        anchors.bottomMargin:   _verticalMargin
        anchors.leftMargin:     _horizontalMargin
        anchors.left:           leftPanel.right
        anchors.top:            parent.top
        anchors.bottom:         parent.bottom
        width:                  1
        color:                  qgcPal.windowShade
    }

    RowLayout {
        id: embeddedBackBar
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: parent.top
        anchors.margins: _horizontalMargin
        visible: embedded && _embeddedPageOpened
        QGCButton {
            objectName: "vehicleConfig_backToMenu"
            text: qsTr("返回无人机设置")
            Layout.fillWidth: true
            onClicked: vehicleConfigView.showNavigation()
        }
    }

    RowLayout {
        id: specialPageHeader
        objectName: "vehicleSetup_header"
        anchors.left: embedded ? parent.left : divider.right
        anchors.right: parent.right
        anchors.top: embeddedBackBar.visible ? embeddedBackBar.bottom : parent.top
        anchors.margins: _defaultTextHeight
        spacing: _defaultTextWidth
        visible: vehicleConfigView._caacStyle && (!embedded || _embeddedPageOpened)
            && panelLoader.item && typeof panelLoader.item.pageName === "undefined"

        Rectangle {
            width: _defaultTextWidth * 0.35
            height: _defaultTextHeight * 1.2
            color: qgcPal.settingsSectionAccent
            radius: width / 2
        }
        QGCLabel {
            Layout.fillWidth: true
            font.pointSize: ScreenTools.mediumFontPointSize
            font.bold: true
            text: {
                if (vehicleConfigView._selectedSpecial === "offline") { return vehicleConfigView._offlinePageTitle }
                if (vehicleConfigView._selectedComponentIndex >= 0 && vehicleConfigView._fullParameterVehicleAvailable) {
                    return ScreenTools.vehicleComponentName(vehicleConfigView._activeVehicle.autopilotPlugin.vehicleComponents[
                        vehicleConfigView._selectedComponentIndex])
                }
                switch (vehicleConfigView._selectedSpecial) {
                case "parameters": return qsTr("Parameters")
                case "firmware": return qsTr("Firmware")
                case "opticalflow": return qsTr("Optical Flow")
                default: return qsTr("Summary")
                }
            }
        }
    }

    Loader {
        id:                     panelLoader
        objectName:             "vehicleConfig_panelLoader"
        visible:                !embedded || _embeddedPageOpened
        anchors.topMargin:      _verticalMargin
        anchors.bottomMargin:   _verticalMargin
        anchors.leftMargin:     _horizontalMargin
        anchors.rightMargin:    _horizontalMargin
        anchors.left:           embedded ? parent.left : divider.right
        anchors.right:          parent.right
        anchors.top:            specialPageHeader.visible ? specialPageHeader.bottom
                                    : embeddedBackBar.visible ? embeddedBackBar.bottom : parent.top
        anchors.bottom:         parent.bottom

        function setSource(source, vehicleComponent) {
            panelLoader.source = ""
            panelLoader.vehicleComponent = vehicleComponent
            panelLoader.source = source
        }

        function setSourceComponent(sourceComponent, vehicleComponent) {
            panelLoader.sourceComponent = undefined
            panelLoader.vehicleComponent = vehicleComponent
            panelLoader.sourceComponent = sourceComponent
        }

        property var vehicleComponent
    }
}
