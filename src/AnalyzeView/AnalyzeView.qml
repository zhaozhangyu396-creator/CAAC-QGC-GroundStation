import QtQuick
import QtQuick.Controls

import QGroundControl
import QGroundControl.Controls

Rectangle {
    id:     _root
    objectName: "analyzeView_root"
    color:  qgcPal.window
    z:      QGroundControl.zOrderTopMost

    readonly property real  _defaultTextHeight:     ScreenTools.defaultFontPixelHeight
    readonly property real  _defaultTextWidth:      ScreenTools.defaultFontPixelWidth
    readonly property real  _horizontalMargin:      _defaultTextWidth / 2
    readonly property real  _verticalMargin:        _defaultTextHeight / 2

    property var  _activeVehicle: QGroundControl.multiVehicleManager.activeVehicle
    property var  _currentPage:   null
    property var  _currentItem:   null
    property bool embedded: false
    property bool _embeddedPageOpened: false
    readonly property real navigationWidth: Math.max(ScreenTools.defaultFontPixelWidth * 22, buttonColumn._maxButtonWidth) + _horizontalMargin * 2
    onEmbeddedChanged: if (embedded) { showNavigation() }

    function showNavigation() {
        if (mainWindow.allowViewSwitch()) { _embeddedPageOpened = false }
    }
    // Destroy the in-panel page item while panelContainer is still alive, before the
    // loader clears the source and tears down this component.
    Component.onDestruction: mainWindow.destroyInPanelAnalyzePage()

    function _loadPage(source) {
        // Clear reference before calling mainWindow.createAnalyzePage (which destroys the old item).
        _currentItem = null
        if (source !== "") {
            // mainWindow creates and owns the item (QObject parent = mainWindow) so it
            // survives AnalyzeView being unloaded in the popped-out case.
            // anchors.fill: parent in AnalyzePage.qml automatically fills panelContainer.
            _currentItem = mainWindow.createAnalyzePage(source)
            if (_currentItem) {
                _currentItem.parent = panelContainer
            }
        } else {
            mainWindow.destroyInPanelAnalyzePage()
        }
    }

    function _updatePanelSource() {
        if (_currentPage) {
            if (_currentPage.requiresVehicle && !_activeVehicle) {
                _loadPage("")
            } else {
                _loadPage(_currentPage.url)
            }
        }
    }

    on_ActiveVehicleChanged: {
        if (_currentPage && _currentPage.requiresVehicle) {
            _loadPage("")
            if (_activeVehicle) {
                Qt.callLater(_updatePanelSource)
            }
        }
    }

    // This need to block click event leakage to underlying map.
    DeadMouseArea {
        anchors.fill: parent
    }

    QGCFlickable {
        id:                 buttonScroll
        objectName:         "analyze_buttonList"
        visible:            !embedded || !_embeddedPageOpened
        width:              buttonColumn.width
        anchors.topMargin:  _defaultTextHeight / 2
        anchors.top:        parent.top
        anchors.bottom:     parent.bottom
        anchors.leftMargin: _horizontalMargin
        anchors.left:       parent.left
        contentHeight:      buttonColumn.height
        flickableDirection: Flickable.VerticalFlick
        clip:               true

        Column {
            id:         buttonColumn
            width:      _root.embedded ? _root.width - _horizontalMargin * 2 : _maxButtonWidth
            spacing:    _defaultTextHeight / 2

            property real _maxButtonWidth: {
                var maxW = 0
                for (var i = 0; i < buttonRepeater.count; i++) {
                    var item = buttonRepeater.itemAt(i)
                    if (item) maxW = Math.max(maxW, item.implicitWidth)
                }
                return maxW
            }

            Repeater {
                id:     buttonRepeater
                model:  QGroundControl.corePlugin ? QGroundControl.corePlugin.analyzePages : []

                Component.onCompleted: {
                    if (count > 0) {
                        _currentPage = QGroundControl.corePlugin.analyzePages[0]
                        panelContainer.title = _currentPage.title
                        _updatePanelSource()
                    }
                }

                SubMenuButton {
                    objectName:         "analyzeButton_" + modelData.title
                    imageResource:      modelData.icon
                    text:               modelData.title
                    width:              buttonColumn.width
                    checked:            _currentPage === modelData

                    onClicked: {
                        if (!mainWindow.allowViewSwitch()) {
                            return
                        }
                        _currentPage        = modelData
                        _embeddedPageOpened = true
                        panelContainer.title = modelData.title
                        _updatePanelSource()
                    }
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
        anchors.left:           buttonScroll.right
        anchors.top:            parent.top
        anchors.bottom:         parent.bottom
        width:                  1
        color:                  qgcPal.windowShade
    }

    QGCButton {
        id: embeddedBackButton
        objectName: "analyze_backToMenu"
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: parent.top
        anchors.margins: _horizontalMargin
        visible: embedded && _embeddedPageOpened
        text: qsTr("返回分析")
        onClicked: _root.showNavigation()
    }

    Item {
        id:                     panelContainer
        objectName:             "analyze_panel"
        visible:                !embedded || _embeddedPageOpened
        anchors.topMargin:      _verticalMargin
        anchors.bottomMargin:   _verticalMargin
        anchors.leftMargin:     _horizontalMargin
        anchors.rightMargin:    _horizontalMargin
        anchors.left:           embedded ? parent.left : divider.right
        anchors.right:          parent.right
        anchors.top:            embeddedBackButton.visible ? embeddedBackButton.bottom : parent.top
        anchors.bottom:         parent.bottom

        property string title

        Connections {
            target: _currentItem

            function onPopout() {
                var existingItem = _currentItem
                var pageTitle = panelContainer.title
                var pageSource = _currentPage.url
                var requiresVehicle = _currentPage ? _currentPage.requiresVehicle : false
                // Clear references before handing item to the popup window.
                _currentItem = null
                // Tell mainWindow this item has moved to a popup so it is not destroyed
                // when AnalyzeView is torn down.
                mainWindow.analyzePageMovedToPopup()
                existingItem.visible = false
                // Hand the existing item to the popout window.
                mainWindow.createWindowedAnalyzePage(pageTitle, pageSource, requiresVehicle, existingItem)
                // Create a fresh in-panel instance.
                _loadPage(pageSource)
            }
        }
    }

    QGCLabel {
        anchors.centerIn:   panelContainer
        text:               qsTr("Requires a connected vehicle")
        visible:            panelContainer.visible && _currentPage && _currentPage.requiresVehicle && !_activeVehicle
    }
}
