import QtQuick
import QtQuick.Controls
import QtQuick.Dialogs
import QtQuick.Layouts

import QGroundControl
import QGroundControl.Controls
import QGroundControl.FactControls

Item {
    id:         _root
    objectName: "parameterEditor_root"

    property Fact   _editorDialogFact: Fact { }
    property int    _rowHeight:         ScreenTools.defaultFontPixelHeight * 2
    property int    _rowWidth:          10 // Dynamic adjusted at runtime
    property bool   _searchFilter:      searchText.text.trim() != "" || controller.showModifiedOnly || controller.showFavoritesOnly  ///< true: showing results of search
    property var    _searchResults      ///< List of parameter names from search results
    property var    _activeVehicle:     QGroundControl.multiVehicleManager.activeVehicle
    property bool   _showRCToParam:     _activeVehicle ? _activeVehicle.px4Firmware : false
    property var    _appSettings:       QGroundControl.settingsManager.appSettings
    property var    _controller:        controller
    property var    _favorites:         controller.favoriteParameterNames
    property real   _margins:           ScreenTools.defaultFontPixelHeight / 2
    readonly property bool _compactLayout: width < ScreenTools.defaultFontPixelWidth * 80
    readonly property var _categoryChoices: _choices(controller.categories)
    readonly property var _groupChoices: _choices(controller.currentCategory ? controller.currentCategory.groups : null)

    function _choices(objects) {
        const choices = []
        if (objects) {
            for (let index = 0; index < objects.count; index++) {
                const object = objects.get(index)
                choices.push({text: object.name, object: object})
            }
        }
        return choices
    }

    ParameterEditorController {
        id: controller
    }

    Timer {
        id:         clearTimer
        interval:   100;
        running:    false;
        repeat:     false
        onTriggered: {
            searchText.text = ""
            controller.searchText = ""
        }
    }

    QGCMenu {
        id:                 toolsMenu
        QGCMenuItem {
            text: qsTr("Hide read-only")
            visible: _root._compactLayout
            checkable: true
            checked: controller.hideReadOnly
            onTriggered: controller.hideReadOnly = checked
        }
        QGCMenuItem {
            text:           qsTr("Refresh")
            onTriggered:	controller.refresh()
        }
        QGCMenuItem {
            text:           qsTr("Reset all to firmware's defaults")
            onTriggered:    QGroundControl.showMessageDialog(_root, qsTr("Reset All"),
                                                         qsTr("Select Reset to reset all parameters to their defaults.\n\nNote that this will also completely reset everything, including UAVCAN nodes, all vehicle settings, setup and calibrations."),
                                                         Dialog.Cancel | Dialog.Reset,
                                                         function() { controller.resetAllToDefaults() })
        }
        QGCMenuItem {
            text:           qsTr("Reset to vehicle's configuration defaults")
            visible:        !_activeVehicle.apmFirmware
            onTriggered:    QGroundControl.showMessageDialog(_root, qsTr("Reset All"),
                                                         qsTr("Select Reset to reset all parameters to the vehicle's configuration defaults."),
                                                         Dialog.Cancel | Dialog.Reset,
                                                         function() { controller.resetAllToVehicleConfiguration() })
        }
        QGCMenuSeparator { }
        QGCMenuItem {
            objectName:     "parameterEditor_toolLoadFromFile"
            text:           qsTr("Load from file for review...")
            onTriggered: {
                fileDialog.title =          qsTr("Load Parameters")
                fileDialog.openForLoad()
            }
        }
        QGCMenuItem {
            text:           qsTr("Save to file...")
            onTriggered: {
                fileDialog.title =          qsTr("Save Parameters")
                fileDialog.openForSave()
            }
        }
        QGCMenuSeparator { }
        QGCMenuItem {
            text:           qsTr("Clear all favorites")
            onTriggered:    controller.clearAllFavorites()
        }
        QGCMenuSeparator { visible: _showRCToParam }
        QGCMenuItem {
            text:           qsTr("Clear all RC to Param")
            onTriggered:	_activeVehicle.clearAllParamMapRC()
            visible:        _showRCToParam
        }
        QGCMenuSeparator { }
        QGCMenuItem {
            text:           qsTr("Reboot Vehicle")
            onTriggered:    QGroundControl.showMessageDialog(_root, qsTr("Reboot Vehicle"),
                                                         qsTr("Select Ok to reboot vehicle."),
                                                         Dialog.Cancel | Dialog.Ok,
                                                         function() { _activeVehicle.rebootVehicle() })
        }
    }


    QGCFileDialog {
        id:             fileDialog
        folder:         _appSettings.parameterSavePath
        nameFilters:    [ qsTr("Parameter Files (*.%1)").arg(_appSettings.parameterFileExtension), qsTr("Mission Planner Files (*.param)"), qsTr("All Files (*)") ]

        onAcceptedForSave: (file) => {
            controller.saveToFile(file)
            close()
        }

        onAcceptedForLoad: (file) => {
            close()
            if (controller.buildDiffFromFile(file)) {
                parameterDiffDialogFactory.open()
            }
        }
    }

    QGCPopupDialogFactory {
        id: editorDialogFactory

        dialogComponent: editorDialogComponent
    }

    Component {
        id: editorDialogComponent

        ParameterEditorDialog {
            fact:           _editorDialogFact
            showRCToParam:  _showRCToParam
        }
    }

    QGCPopupDialogFactory {
        id: parameterDiffDialogFactory

        dialogComponent: parameterDiffDialog
    }

    Component {
        id: parameterDiffDialog

        ParameterDiffDialog {
            paramController: _controller
        }
    }

    GridLayout {
        id:             header
        anchors.left:   parent.left
        anchors.right:  parent.right
        columns:        _root._compactLayout ? 3 : 4
        columnSpacing:  ScreenTools.defaultFontPixelWidth
        rowSpacing:     _margins / 2

        QGCTextField {
            id:                     searchText
            objectName:             "parameterEditor_search"
            Layout.fillWidth:       true
            Layout.minimumWidth:    0
            placeholderText:        qsTr("Search")
            onDisplayTextChanged:   controller.searchText = displayText
        }

        QGCButton {
            objectName: "parameterEditor_clearSearch"
            text: qsTr("Clear")
            onClicked: {
                if(ScreenTools.isMobile) {
                    Qt.inputMethod.hide();
                }
                clearTimer.start()
            }
        }

        QGCCheckBox {
            text:       qsTr("Hide read-only")
            visible:    !_root._compactLayout
            checked:    controller.hideReadOnly
            onClicked:  controller.hideReadOnly = checked
        }

        QGCButton {
            Layout.alignment:   Qt.AlignRight
            objectName:         "parameterEditor_toolsButton"
            text:               qsTr("Tools")
            onClicked:          toolsMenu.popup()
        }
    }

    QGCTabBar {
        id:             tabBar
        anchors.left:   parent.left
        anchors.right:  parent.right
        anchors.top:        header.bottom
        anchors.topMargin:  _margins

        QGCTabButton { objectName: "parameterEditor_fullListTab"; text: qsTr("Full List"); Layout.minimumWidth: 0 }
        QGCTabButton { objectName: "parameterEditor_modifiedTab"; text: qsTr("Modified"); Layout.minimumWidth: 0 }
        QGCTabButton { objectName: "parameterEditor_favoritesTab"; text: qsTr("Favorites"); Layout.minimumWidth: 0 }

        onCurrentIndexChanged: {
            controller.showModifiedOnly  = (currentIndex === 1)
            controller.showFavoritesOnly = (currentIndex === 2)
        }
    }

    GridLayout {
        id: compactSelectors
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: tabBar.bottom
        anchors.topMargin: _margins
        columns: 4
        columnSpacing: ScreenTools.defaultFontPixelWidth
        rowSpacing: _margins
        visible: _root._compactLayout && !_searchFilter

        QGCLabel {
            text: qsTr("Category")
        }

        QGCComboBox {
            objectName: "parameterEditor_categorySelector"
            Layout.fillWidth: true
            Layout.minimumWidth: 0
            font.pointSize: ScreenTools.defaultFontPointSize
            textRole: "text"
            model: _root._categoryChoices
            currentIndex: _root._categoryChoices.findIndex(choice => choice.object === controller.currentCategory)
            onActivated: index => controller.currentCategory = model[index].object
        }

        QGCLabel {
            text: qsTr("Group")
        }

        QGCComboBox {
            objectName: "parameterEditor_groupSelector"
            Layout.fillWidth: true
            Layout.minimumWidth: 0
            font.pointSize: ScreenTools.defaultFontPointSize
            textRole: "text"
            model: _root._groupChoices
            currentIndex: _root._groupChoices.findIndex(choice => choice.object === controller.currentGroup)
            onActivated: index => controller.currentGroup = model[index].object
        }
    }

    /// Group buttons
    QGCFlickable {
        id :                groupScroll
        objectName:         "parameterEditor_categoryTree"
        width:              ScreenTools.defaultFontPixelWidth * 25
        anchors.top:        tabBar.bottom
        anchors.topMargin:  _margins
        anchors.bottom:     parent.bottom
        clip:               true
        pixelAligned:       true
        contentHeight:      groupedViewCategoryColumn.height
        flickableDirection: Flickable.VerticalFlick
        visible:            !_searchFilter && !_root._compactLayout

        ColumnLayout {
            id:             groupedViewCategoryColumn
            anchors.left:   parent.left
            anchors.right:  parent.right
            spacing:        Math.ceil(ScreenTools.defaultFontPixelHeight * 0.25)

            Repeater {
                model: controller.categories

                Column {
                    Layout.fillWidth:   true
                    spacing:            Math.ceil(ScreenTools.defaultFontPixelHeight * 0.25)


                    SectionHeader {
                        id:             categoryHeader
                        anchors.left:   parent.left
                        anchors.right:  parent.right
                        text:           object.name
                        checked:        object == controller.currentCategory

                        onCheckedChanged: {
                            if (checked) {
                                controller.currentCategory  = object
                            }
                        }
                    }

                    Repeater {
                        model: categoryHeader.checked ? object.groups : 0

                        QGCButton {
                            width:          ScreenTools.defaultFontPixelWidth * 25
                            text:           object.name
                            height:         _rowHeight
                            checked:        object == controller.currentGroup
                            autoExclusive:  true

                            onClicked: {
                                if (!checked) _rowWidth = 10
                                checked = true
                                controller.currentGroup = object
                            }
                        }
                    }
                }
            }
        }
    }

    HorizontalHeaderView {
        id:                 headerView
        anchors.left:       tableView.left
        anchors.right:      tableView.right
        anchors.top:        compactSelectors.visible ? compactSelectors.bottom : tabBar.bottom
        anchors.topMargin:  _margins
        syncView:           tableView
        clip:               true

        delegate: Rectangle {
            implicitWidth:  column === 0 ? ScreenTools.implicitCheckBoxHeight + ScreenTools.defaultFontPixelWidth
                                         : headerLabel.contentWidth + ScreenTools.defaultFontPixelWidth
            implicitHeight: headerLabel.contentHeight + ScreenTools.defaultFontPixelHeight * 0.5
            color:          qgcPal.windowShade

            QGCLabel {
                id:                     headerLabel
                anchors.left:           parent.left
                anchors.leftMargin:     ScreenTools.defaultFontPixelWidth / 2
                anchors.verticalCenter: parent.verticalCenter
                text:                   display
                font.bold:              true
            }

            // Top border
            Rectangle {
                anchors.top:    parent.top
                width:          parent.width
                height:         1
                color:          qgcPal.groupBorder
            }

            // Left border
            Rectangle {
                anchors.left:   parent.left
                height:         parent.height
                width:          1
                color:          qgcPal.groupBorder
            }

            // Right border (last column only)
            Rectangle {
                anchors.right:  parent.right
                height:         parent.height
                width:          1
                color:          qgcPal.groupBorder
                visible:        column == 3
            }

            // Bottom border
            Rectangle {
                anchors.bottom: parent.bottom
                width:          parent.width
                height:         1
                color:          qgcPal.groupBorder
            }
        }
    }

    TableView {
        id:                 tableView
        objectName:         "parameterEditor_table"
        anchors.leftMargin: _searchFilter || _root._compactLayout ? 0 : ScreenTools.defaultFontPixelWidth
        anchors.top:        headerView.bottom
        anchors.bottom:     parent.bottom
        anchors.left:       _searchFilter || _root._compactLayout ? parent.left : groupScroll.right
        anchors.right:      parent.right
        columnSpacing:      0
        rowSpacing:         0
        model:              controller.parameters
        columnWidthProvider: column => {
            const favoriteWidth = Math.max(ScreenTools.implicitCheckBoxHeight + ScreenTools.defaultFontPixelWidth,
                                           ScreenTools.defaultFontPixelHeight * 2.5)
            if (column === 0) { return favoriteWidth }
            if (_root._compactLayout && (column === 1 || column === 2)) {
                const availableWidth = Math.max(1, tableView.width - favoriteWidth)
                const nameWidth = Math.floor(availableWidth * 0.56)
                return column === 1 ? nameWidth : availableWidth - nameWidth
            }
            return ScreenTools.defaultFontPixelWidth * (column === 1 ? 22 : column === 2 ? 26 : 48)
        }
        bottomMargin:       horizontalScrollBar.visible ? horizontalScrollBar.height : 0
        clip:               true

        ScrollBar.horizontal: ScrollBar {
            id: horizontalScrollBar
            objectName: "parameterEditor_horizontalScrollBar"
            policy: ScrollBar.AlwaysOn
            visible: tableView.contentWidth > tableView.width
            active: visible
        }

        ScrollBar.vertical: ScrollBar {
            policy: ScrollBar.AsNeeded
        }

        // Qt is supposed to adjust column widths automatically when larger widths come into view.
        // But it doesn't work. So we have to do it force a layout manually when we scroll.
        Timer {
            id:             forceLayoutTimer
            interval:       500
            repeat:         false
            onTriggered:    tableView.forceLayout()
        }

        onWidthChanged: forceLayoutTimer.start()
        onTopRowChanged: forceLayoutTimer.start()
        onLeftColumnChanged: forceLayoutTimer.start()
        onModelChanged: {
            positionViewAtRow(0, TableView.AlignLeft | TableView.AlignTop)
            forceLayoutTimer.start()
        }

        delegate: Rectangle {
            objectName: "parameterEditor_cell_" + row + "_" + column
            implicitWidth:  tableView.columnWidthProvider(column)
            implicitHeight: Math.max(ScreenTools.implicitCheckBoxHeight,
                                     column === 1 ? nameLabel.implicitHeight : label.implicitHeight)
                            + ScreenTools.defaultFontPixelHeight * 0.5
            color:          row % 2 === 0 ? "transparent" : qgcPal.windowShade
            clip:           true

            // Bottom grid line
            Rectangle {
                anchors.bottom: parent.bottom
                width:          parent.width
                height:         1
                color:          qgcPal.groupBorder
            }

            // Left grid line
            Rectangle {
                anchors.left:   parent.left
                height:         parent.height
                width:          1
                color:          qgcPal.groupBorder
            }

            // Right grid line (last column only)
            Rectangle {
                anchors.right:  parent.right
                height:         parent.height
                width:          1
                color:          qgcPal.groupBorder
                visible:        column == 3
            }

            QGCCheckBox {
                objectName:             "parameterEditor_favorite_" + fact.name
                visible:                column === 0
                anchors.centerIn:       parent
                checked:                _root._favorites.indexOf(fact.name) >= 0
                z:                      1
                onClicked:              controller.toggleFavorite(fact.name)
            }

            QGCLabel {
                id:                     nameLabel
                visible:                column === 1
                anchors.left:           parent.left
                anchors.leftMargin:     ScreenTools.defaultFontPixelWidth / 2
                anchors.right:          parent.right
                anchors.rightMargin:    ScreenTools.defaultFontPixelWidth / 2
                                        + (lockIcon.visible ? lockIcon.width + ScreenTools.defaultFontPixelWidth / 3 : 0)
                anchors.verticalCenter: parent.verticalCenter
                text:                   column === 1 ? display : ""
                wrapMode:               Text.Wrap
            }

            QGCColoredImage {
                id:                 lockIcon
                visible:            column === 1 && fact.readOnly
                source:             "qrc:/InstrumentValueIcons/lock-closed.svg"
                color:              qgcPal.text
                width:              ScreenTools.defaultFontPixelHeight * 0.8
                height:             width
                sourceSize.width:   width
                anchors.right:      parent.right
                anchors.rightMargin: ScreenTools.defaultFontPixelWidth / 2
                anchors.verticalCenter: parent.verticalCenter
            }

            QGCLabel {
                id:                 label
                objectName:         "parameterEditor_valueText_" + fact.name + "_" + column
                visible:            column !== 0 && column !== 1
                anchors.left:       parent.left
                anchors.leftMargin: ScreenTools.defaultFontPixelWidth / 2
                anchors.verticalCenter: parent.verticalCenter
                width:              parent.width - ScreenTools.defaultFontPixelWidth
                text:               column == 2 ? col1String() : OnlineMessageTranslator.parameterText(display, OnlineMessageTranslator.revision)
                color:              column == 2 && fact.defaultValueAvailable && !fact.valueEqualsDefault ? qgcPal.modifiedParamValue : qgcPal.text
                font.bold:          column == 2 && fact.defaultValueAvailable && !fact.valueEqualsDefault
                wrapMode:           Text.Wrap

                function col1String() {
                    if (fact.enumStrings.length === 0) {
                        return fact.valueString + " " + fact.units
                    }
                    if (fact.bitmaskStrings.length != 0) {
                        return OnlineMessageTranslator.parameterOptions(fact.selectedBitmaskStrings, OnlineMessageTranslator.revision).join(',')
                    }
                    return OnlineMessageTranslator.parameterText(fact.enumStringValue, OnlineMessageTranslator.revision)
                }
            }

            QGCMouseArea {
                objectName: "parameterEditor_edit_" + fact.name + "_" + column
                anchors.fill: parent
                visible:      column !== 0
                onClicked: mouse => {
                    _editorDialogFact = fact
                    editorDialogFactory.open()
                }
            }
        }
    }

    Rectangle {
        objectName: "parameterEditor_tableFrame"
        anchors.left: tableView.left
        anchors.right: tableView.right
        anchors.top: headerView.top
        anchors.bottom: tableView.bottom
        color: "transparent"
        border.color: qgcPal.groupBorder
        border.width: 1
        z: 2
    }

}
