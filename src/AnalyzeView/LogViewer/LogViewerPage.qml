import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtLocation
import QtPositioning

import QGroundControl
import QGroundControl.AnalyzeView
import QGroundControl.Controls
import QGroundControl.FlightMap
import QGroundControl.LogViewer

AnalyzePage {
    id: logViewerPage
    objectName: "logViewer_page"
    pageComponent: pageComponent
    pageDescription: qsTr("Open and inspect DataFlash (.bin), PX4 ULog (.ulg), and telemetry (.tlog) logs in a unified workflow.")
    allowPopout: true

    Component {
        id: pageComponent

        QGCFlickable {
            id: logViewerScroll
            objectName: "logViewer_scroller"
            width: availableWidth
            height: availableHeight
            contentWidth: width
            contentHeight: logViewerContent.height
            flickableDirection: Flickable.VerticalFlick

            ColumnLayout {
                id: logViewerContent
                objectName: "logViewer_content"
                width: logViewerScroll.width
                height: Math.max(implicitHeight, logViewerScroll.height)
                spacing: ScreenTools.defaultFontPixelHeight

                property string pendingBinFile: ""

                readonly property bool _xAxisShowLocalTime: QGroundControl.settingsManager.logViewerSettings.xAxisShowLocalTime.rawValue

                readonly property bool isFirmwareLog: logViewerController.sourceType === LogViewerController.Bin
                                                 || logViewerController.sourceType === LogViewerController.ULog

                // Cancel any in-flight async parse before QML starts tearing down the tree.
                // Without this, the background thread can emit signals (parseProgressChanged,
                // parseFileFinished) into partially-destroyed QML objects, causing a
                // QQmlData::disconnectNotifiers crash.
                Component.onDestruction: logParser.clear()

                function clearLoadedLogState(clearControllerState) {
                    replayController.isPlaying = false
                    replayController.link = null
                    logParser.clear()
                    logViewerController.setPlottableFields([])
                    logViewerController.clearSelection()
                    _parametersTab.applyFilter()
                    logViewerChart.clearMarker()
                    logViewerChart.refreshBinChart()
                    _mapTab._sharedCursorT = NaN
                    _mapTab._sharedZoomMinX = NaN
                    _mapTab._sharedZoomMaxX = NaN
                    if (clearControllerState) {
                        logViewerController.clear()
                    }
                }

                function loadBinFile(file) {
                    if (logViewerController.hasLoadedLog) {
                        // Match explicit "Clear" behavior before loading replacement .bin file.
                        clearLoadedLogState(true)
                    }
                    pendingBinFile = file
                    logParser.startParsingAsync(file)
                }

                LogViewerController {
                    id: logViewerController
                    objectName: "logViewer_controller"
                }

                LogFileParser {
                    id: logParser
                    objectName: "logViewer_parser"
                }

                Connections {
                    target: logParser
                    ignoreUnknownSignals: true

                    function onParseFileFinished(filePath, ok, errorMessage) {
                        if (filePath !== pendingBinFile) {
                            return
                        }

                        if (!ok) {
                            QGroundControl.showMessageDialog(logViewerPage, qsTr("Log Viewer"), errorMessage)
                            pendingBinFile = ""
                            return
                        }

                        fieldsPanel.rebuildGroupedFields()
                        _parametersTab.applyFilter()
                        logViewerController.clearSelection()
                        logViewerChart.clearMarker()
                        logViewerChart.refreshBinChart()
                        Qt.callLater(logViewerChart.centerCursor)

                        const lowerPath = filePath.toLowerCase()
                        if (lowerPath.endsWith(".ulg")) {
                            logViewerController.openULogFile(filePath)
                        } else {
                            logViewerController.openBinLog(filePath)
                        }
                        pendingBinFile = ""
                    }
                }

                LogReplayLinkController {
                    id: replayController
                }

                Flow {
                    id: openToolbar
                    Layout.fillWidth: true
                    Layout.minimumWidth: 0
                    spacing: ScreenTools.defaultFontPixelWidth

                    QGCButton {
                        objectName: "logViewer_openBin"
                        width: Math.min(implicitWidth, openToolbar.width)
                        wrapMode: Text.WordWrap
                        text: qsTr("Open .bin")
                        visible: QGroundControl.apmFirmwareSupported
                        onClicked: {
                            openDialog.nameFilters = ["DataFlash Logs (*.bin *.BIN *.log *.LOG)"]
                            openDialog.openForLoad()
                        }
                    }

                    QGCButton {
                        objectName: "logViewer_openULog"
                        width: Math.min(implicitWidth, openToolbar.width)
                        wrapMode: Text.WordWrap
                        text: qsTr("Open .ulg")
                        onClicked: {
                            openDialog.nameFilters = ["PX4 ULog Files (*.ulg *.ULG)"]
                            openDialog.openForLoad()
                        }
                    }

                    QGCButton {
                        objectName: "logViewer_openTLog"
                        width: Math.min(implicitWidth, openToolbar.width)
                        wrapMode: Text.WordWrap
                        text: qsTr("Open .tlog")
                        onClicked: {
                            const activeVehicle = QGroundControl.multiVehicleManager.activeVehicle
                            if (activeVehicle && !activeVehicle.isOfflineEditingVehicle) {
                                QGroundControl.showMessageDialog(logViewerPage, qsTr("Log Viewer"), qsTr("Close active vehicle connections before starting telemetry replay."))
                                return
                            }
                            openDialog.nameFilters = ["Telemetry Logs (*.tlog *.TLOG)"]
                            openDialog.openForLoad()
                        }
                    }

                    QGCButton {
                        objectName: "logViewer_clear"
                        width: Math.min(implicitWidth, openToolbar.width)
                        wrapMode: Text.WordWrap
                        text: qsTr("Clear")
                        enabled: logViewerController.hasLoadedLog
                        onClicked: {
                            clearLoadedLogState(true)
                        }
                    }

                    QGCLabel {
                        objectName: "logViewer_fileName"
                        width: Math.min(implicitWidth, openToolbar.width)
                        wrapMode: Text.Wrap
                        text: logViewerController.hasLoadedLog ? logViewerController.currentLogPath.replace(/.*[/\\]/, "") : qsTr("No log selected")
                    }

                    QGCLabel {
                        width: Math.min(implicitWidth, openToolbar.width)
                        wrapMode: Text.Wrap
                        visible: logViewerController.hasLoadedLog
                        text: qsTr("Start time:")
                    }

                    QGCLabel {
                        width: Math.min(implicitWidth, openToolbar.width)
                        wrapMode: Text.Wrap
                        readonly property bool _hasStartTime: logParser.startTime
                                                              && !isNaN(logParser.startTime.getTime())
                                                              && logParser.startTime.getTime() > 0
                        visible: logViewerController.hasLoadedLog
                        text: _hasStartTime
                                 ? Qt.formatDateTime(logParser.startTime, Qt.locale().dateTimeFormat(Locale.ShortFormat))
                                 : qsTr("N/A")
                    }

                    QGCLabel {
                        width: Math.min(implicitWidth, openToolbar.width)
                        wrapMode: Text.Wrap
                        visible: logViewerController.hasLoadedLog && logParser.detectedVehicleType.length > 0
                        text: qsTr("Vehicle:")
                    }

                    QGCLabel {
                        width: Math.min(implicitWidth, openToolbar.width)
                        wrapMode: Text.Wrap
                        visible: logViewerController.hasLoadedLog && logParser.detectedVehicleType.length > 0
                        text: logParser.detectedVehicleType
                    }
                }

                RowLayout {
                    Layout.fillWidth: true
                    visible: logParser.parsing
                    spacing: ScreenTools.defaultFontPixelWidth

                    QGCLabel {
                        text: qsTr("Loading...")
                    }

                    QGCSlider {
                        Layout.fillWidth: true
                        from: 0
                        to: 1
                        value: logParser.parseProgress
                        enabled: false
                    }
                }

                GridLayout {
                    id: replayControls
                    Layout.fillWidth: true
                    Layout.minimumWidth: 0
                    columns: width >= ScreenTools.defaultFontPixelWidth * 70 ? 5 : 3
                    visible: logViewerController.sourceType === LogViewerController.TLog && replayController.link
                    columnSpacing: ScreenTools.defaultFontPixelWidth
                    rowSpacing: ScreenTools.defaultFontPixelWidth

                    QGCButton {
                        text: replayController.isPlaying ? qsTr("Pause") : qsTr("Play")
                        onClicked: replayController.isPlaying = !replayController.isPlaying
                    }

                    QGCComboBox {
                        textRole: "text"
                        currentIndex: 3

                        model: ListModel {
                            ListElement { text: "0.1x";  value: 0.1  }
                            ListElement { text: "0.25x"; value: 0.25 }
                            ListElement { text: "0.5x";  value: 0.5  }
                            ListElement { text: "1x";    value: 1.0  }
                            ListElement { text: "2x";    value: 2.0  }
                            ListElement { text: "5x";    value: 5.0  }
                            ListElement { text: "10x";   value: 10.0 }
                        }

                        onActivated: (index) => replayController.playbackSpeed = model.get(index).value
                    }

                    QGCLabel { text: replayController.playheadTime }

                    Slider {
                        id: _replaySlider
                        Layout.fillWidth: true
                        Layout.minimumWidth: 0
                        Layout.columnSpan: replayControls.columns === 3 ? 2 : 1
                        from: 0
                        to: 100

                        property bool _internalUpdate: false

                        Connections {
                            target: replayController
                            function onPercentCompleteChanged(percentComplete) {
                                _replaySlider._internalUpdate = true
                                _replaySlider.value = percentComplete
                                _replaySlider._internalUpdate = false
                            }
                        }

                        onValueChanged: {
                            if (!_internalUpdate) {
                                replayController.percentComplete = value
                            }
                        }
                    }

                    QGCLabel { text: replayController.totalTime }
                }

                GridLayout {
                    id: mainTabBar
                    objectName: "logViewer_tabs"
                    Layout.fillWidth: true
                    Layout.minimumWidth: 0
                    columns: width >= _tabWidth * 4 ? 4 : width >= _tabWidth * 2 ? 2 : 1
                    columnSpacing: 0
                    rowSpacing: 0
                    property int currentIndex: 0
                    readonly property real _tabWidth: Math.max(chartTab.implicitWidth, mapTab.implicitWidth,
                                                              parametersTab.implicitWidth, messagesTab.implicitWidth)
                    ButtonGroup { id: mainTabButtons }

                    QGCTabButton {
                        id: chartTab
                        objectName: "logViewer_tabChart"
                        Layout.minimumWidth: 0
                        Layout.preferredWidth: 1
                        text: qsTr("Charting")
                        ButtonGroup.group: mainTabButtons
                        checked: true
                        onClicked: mainTabBar.currentIndex = 0
                    }
                    QGCTabButton {
                        id: mapTab
                        objectName: "logViewer_tabMap"
                        Layout.minimumWidth: 0
                        Layout.preferredWidth: 1
                        text: qsTr("Map")
                        ButtonGroup.group: mainTabButtons
                        onClicked: mainTabBar.currentIndex = 1
                    }
                    QGCTabButton {
                        id: parametersTab
                        objectName: "logViewer_tabParameters"
                        Layout.minimumWidth: 0
                        Layout.preferredWidth: 1
                        text: qsTr("Parameters")
                        ButtonGroup.group: mainTabButtons
                        onClicked: mainTabBar.currentIndex = 2
                    }
                    QGCTabButton {
                        id: messagesTab
                        objectName: "logViewer_tabMessages"
                        Layout.minimumWidth: 0
                        Layout.preferredWidth: 1
                        text: qsTr("Messages")
                        ButtonGroup.group: mainTabButtons
                        onClicked: mainTabBar.currentIndex = 3
                    }
                }

                StackLayout {
                    objectName: "logViewer_endContent"
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    Layout.minimumWidth: 0
                    Layout.minimumHeight: ScreenTools.defaultFontPixelHeight * 12
                    currentIndex: mainTabBar.currentIndex

                    // ---- Tab 0: Charting ----
                    GridLayout {
                        id: chartingLayout
                        columns: logViewerContent.width >= ScreenTools.defaultFontPixelWidth * 100 ? 2 : 1
                        columnSpacing: ScreenTools.defaultFontPixelWidth
                        rowSpacing: ScreenTools.defaultFontPixelHeight

                        // Left panel: stats + fields list
                        LogViewerFieldsPanel {
                            id: fieldsPanel
                            objectName: "logViewer_fields"
                            Layout.fillWidth: true
                            Layout.fillHeight: true
                            Layout.maximumWidth: chartingLayout.columns === 1 ? Infinity : logViewerContent.width * 0.4
                            Layout.preferredHeight: chartingLayout.columns === 1 ? ScreenTools.defaultFontPixelHeight * 14 : -1
                            Layout.minimumHeight: ScreenTools.defaultFontPixelHeight * 10
                            visible: isFirmwareLog
                            logParser: logParser
                            logViewerController: logViewerController
                            onClearSelectedRequested: logViewerChart.refreshBinChart()
                        }

                        // Right panel: chart + MAVLink inspector
                        Rectangle {
                            Layout.fillWidth: true
                            Layout.fillHeight: true
                            Layout.minimumWidth: 0
                            Layout.minimumHeight: ScreenTools.defaultFontPixelHeight * 12
                            color: qgcPal.windowShadeDark
                            radius: ScreenTools.defaultFontPixelWidth * 0.5

                            ColumnLayout {
                                anchors.fill: parent
                                anchors.margins: ScreenTools.defaultFontPixelWidth
                                spacing: ScreenTools.defaultFontPixelHeight * 0.5

                                LogViewerChart {
                                    id: logViewerChart
                                    Layout.fillWidth: true
                                    Layout.fillHeight: true
                                    visible: isFirmwareLog
                                    logParser: logParser
                                    logViewerController: logViewerController
                                    xAxisShowLocalTime: _xAxisShowLocalTime

                                    onCursorMoved: (t) => {
                                        _mapTab._markerVisible = true
                                        _mapTab._markerCoord   = logParser.gpsCoordAt(t)
                                        _mapTab._sharedCursorT = t
                                        if (_altChart.visible) _altChart.setSharedCursor(t)
                                    }
                                    onZoomApplied: (minX, maxX) => {
                                        _mapTab._setSharedZoom(minX, maxX)
                                        if (_altChart.visible) _altChart.setSharedZoom(minX, maxX)
                                    }
                                }

                                Loader {
                                    Layout.fillWidth: true
                                    Layout.fillHeight: true
                                    active: logViewerController.sourceType === LogViewerController.TLog
                                    source: "qrc:/qml/QGroundControl/AnalyzeView/MAVLinkInspector/MAVLinkInspectorPage.qml"
                                }
                            }
                        }
                    }

                    // ---- Tab 1: Map ----
                    Item {
                        id: _mapTab
                        Layout.fillWidth: true
                        Layout.fillHeight: true

                        // GPS path data
                        readonly property var _gpsPath: logParser.parseComplete ? logParser.gpsPath() : []
                        readonly property int _pathLen: (_gpsPath && _gpsPath.length) ? _gpsPath.length : 0
                        readonly property bool _hasPath: _pathLen >= 2
                        readonly property string _altFieldName: _hasPath ? logParser.gpsAltitudeFieldName() : ""
                        readonly property bool _hasAltField: _altFieldName.length > 0
                        readonly property bool _showAltChart: _hasAltField && _hasPath

                        // Shared cursor state (driven by altitude chart, displayed on map)
                        property bool _markerVisible: false
                        property var _markerCoord: ({})

                        // Collapsed state for the altitude chart, so the map can use the full tab height
                        property bool _altChartCollapsed: false

                        // Last shared cursor/zoom from either chart; reapplied when the altitude chart becomes visible.
                        property real _sharedCursorT: NaN
                        property real _sharedZoomMinX: NaN
                        property real _sharedZoomMaxX: NaN

                        function _setSharedZoom(minX, maxX) {
                            _sharedZoomMinX = minX
                            _sharedZoomMaxX = maxX
                            // Mirror LogViewerBaseChart._applyZoomInternal, which recenters an out-of-range cursor
                            if (!isNaN(_sharedCursorT) && (_sharedCursorT < minX || _sharedCursorT > maxX)) {
                                _sharedCursorT = (minX + maxX) / 2
                                _markerCoord = logParser.gpsCoordAt(_sharedCursorT)
                            }
                        }

                        ColumnLayout {
                            anchors.fill: parent
                            spacing: 0

                            // ---- Map ----
                            Item {
                                Layout.fillWidth: true
                                Layout.fillHeight: true

                                FlightMap {
                                    id: _flightMap
                                    anchors.fill: parent
                                    mapName: "LogViewerMap"
                                    allowGCSLocationCenter: true

                                    readonly property var _path: _mapTab._gpsPath

                                    function _fitPath() {
                                        const p = _mapTab._gpsPath
                                        if (!_mapTab._hasPath) return
                                        var minLat = p[0].latitude, maxLat = minLat
                                        var minLon = p[0].longitude, maxLon = minLon
                                        for (var i = 1; i < p.length; i++) {
                                            var c = p[i]
                                            if (c.latitude  < minLat) minLat = c.latitude
                                            if (c.latitude  > maxLat) maxLat = c.latitude
                                            if (c.longitude < minLon) minLon = c.longitude
                                            if (c.longitude > maxLon) maxLon = c.longitude
                                        }
                                        setVisibleRegion(QtPositioning.rectangle(
                                            QtPositioning.coordinate(maxLat, minLon),
                                            QtPositioning.coordinate(minLat, maxLon)))
                                    }

                                    Connections {
                                        target: logParser
                                        function onParseCompleteChanged() {
                                            if (logParser.parseComplete) Qt.callLater(_flightMap._fitPath)
                                        }
                                    }

                                    MapPolyline {
                                        line.width: 3
                                        line.color: QGroundControl.globalPalette.colorRed
                                        path: _flightMap._path
                                    }

                                    // Position dot driven by altitude chart cursor
                                    MapQuickItem {
                                        readonly property var _coord: _mapTab._markerCoord
                                        visible: _mapTab._markerVisible && _mapTab._hasPath
                                                      && _coord && _coord.latitude !== undefined
                                        coordinate: (_coord && _coord.latitude !== undefined)
                                                      ? QtPositioning.coordinate(_coord.latitude, _coord.longitude)
                                                      : QtPositioning.coordinate(0, 0)
                                        anchorPoint: Qt.point(_posDot.width / 2, _posDot.height / 2)
                                        sourceItem: Rectangle {
                                            id: _posDot
                                            width: ScreenTools.defaultFontPixelHeight * 1.2
                                            height: width
                                            radius: width / 2
                                            color: QGroundControl.globalPalette.colorYellow
                                            border.color: "white"
                                            border.width: 2
                                        }
                                    }

                                    MapScale {
                                        anchors.margins: ScreenTools.defaultFontPixelWidth
                                        anchors.left: parent.left
                                        anchors.bottom: parent.bottom
                                        mapControl: _flightMap
                                    }
                                }

                                QGCLabel {
                                    anchors.centerIn: parent
                                    visible: logParser.parseComplete && !_mapTab._hasPath
                                    text: qsTr("No GPS data found in this log")
                                    font.italic: true
                                }

                                QGCLabel {
                                    anchors.centerIn: parent
                                    visible: !logParser.parseComplete
                                    text: qsTr("Load a log file to view the flight path")
                                    font.italic: true
                                }

                                QGCButton {
                                    anchors.margins: ScreenTools.defaultFontPixelWidth
                                    anchors.right: parent.right
                                    anchors.bottom: parent.bottom
                                    scale: 0.75
                                    transformOrigin: Item.BottomRight
                                    visible: _mapTab._showAltChart
                                    iconSource: _mapTab._altChartCollapsed ? "/res/chevron-double-up.svg" : "/res/chevron-double-down.svg"
                                    Accessible.name: _mapTab._altChartCollapsed ? qsTr("Expand altitude chart") : qsTr("Collapse altitude chart")
                                    ToolTip.text: Accessible.name
                                    ToolTip.visible: hovered
                                    onClicked: _mapTab._altChartCollapsed = !_mapTab._altChartCollapsed
                                }
                            }

                            // ---- Altitude chart ----
                            LogViewerAltChart {
                                id: _altChart
                                Layout.fillWidth: true
                                Layout.preferredHeight: ScreenTools.defaultFontPixelHeight * 14
                                visible: _mapTab._showAltChart && !_mapTab._altChartCollapsed
                                logParser: logParser
                                altFieldName: _mapTab._altFieldName
                                xAxisShowLocalTime: _xAxisShowLocalTime

                                onMarkerChanged: (t) => {
                                    _mapTab._markerVisible = true
                                    _mapTab._markerCoord   = logParser.gpsCoordAt(t)
                                    _mapTab._sharedCursorT = t
                                    logViewerChart.setSharedCursor(t)
                                }
                                onMarkerCleared: {
                                    _mapTab._markerVisible = false
                                }
                                onZoomApplied: (minX, maxX) => {
                                    _mapTab._setSharedZoom(minX, maxX)
                                    logViewerChart.setSharedZoom(minX, maxX)
                                }
                            }

                            // Catch up on cursor/zoom changes that occurred while this chart was hidden.
                            Connections {
                                target: _altChart
                                function onVisibleChanged() {
                                    if (!_altChart.visible) {
                                        return
                                    }
                                    if (!isNaN(_mapTab._sharedZoomMinX)) {
                                        _altChart.setSharedZoom(_mapTab._sharedZoomMinX, _mapTab._sharedZoomMaxX)
                                    }
                                    if (!isNaN(_mapTab._sharedCursorT)) {
                                        _altChart.setSharedCursor(_mapTab._sharedCursorT)
                                    }
                                }
                            }
                        }
                    }

                    // ---- Tab 2: Parameters ----
                    LogViewerParametersTab {
                        id: _parametersTab
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                        logParser: logParser
                    }

                    // ---- Tab 3: Messages ----
                    LogViewerMessagesTab {
                        id: _messagesTab
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                        logParser: logParser
                    }
                }

                QGCFileDialog {
                    id: openDialog
                    title: qsTr("Select log file")
                    folder: QGroundControl.settingsManager.appSettings.logSavePath
                    selectFolder: false

                    onAcceptedForLoad: (file) => {
                        const fileLower = file.toLowerCase()
                        if (fileLower.endsWith(".tlog")) {
                            if (logViewerController.hasLoadedLog) {
                                clearLoadedLogState(true)
                            }
                            const replayLink = QGroundControl.linkManager.startLogReplay(file)
                            if (!replayLink) {
                                QGroundControl.showMessageDialog(
                                    logViewerPage,
                                    qsTr("Log Viewer"),
                                    qsTr("Failed to start telemetry replay for the selected .tlog file.")
                                )
                                close()
                                return
                            }
                            replayController.link = replayLink
                            logViewerController.openTLog(file)
                        } else {
                            loadBinFile(file)
                        }
                        close()
                    }
                }
            }
        }
    }
}
