import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Dialogs

import QGroundControl
import QGroundControl.Controls

// Provides the standard dialog mechanism for QGC. Works 99% like Qml Dialog.
//
// Example usage:
//      QGCPopupDialogFactory {
//          id: myDialogFactory
//          dialogComponent: myDialogComponent
//      }
//
//      Component {
//          id: myDialogComponent
//
//          QGCPopupDialog {
//              ...
//          }
//      }
//
//      onFoo: myDialogFactory.open()
//      onBar: myDialogFactory.open({ title: "My Title", myProp: someValue })
//
// Notes:
//  * Use QGCPopupDialogFactory to create and open dialogs. The factory handles correct parenting and cleanup of the dialog instances.
//  * The dialog automatically reparents itself to Overlay.overlay on creation, while tracking the original parent's lifetime to prevent orphaned dialogs.
// Differences from standard Qml Dialog:
//  * The QGCPopupDialog object will automatically be destroyed when it closed. You can override this
//      behaviour by setting destroyOnClose to false if it was not created dynamically.
//  * Dialog will automatically close after accepted/rejected signal processing. You can prevent this by setting
//      preventClose = true prior to returning from your signal handlers.
Popup {
    id:                 root
    width:  mainWindow.width
    height: mainWindow.height
    modal:              true
    focus:              true
    margins:            0

    default property alias dialogContent: dialogContentParent.data

    property string title
    property var    buttons:                Dialog.Ok
    property alias  acceptButtonEnabled:    acceptButton.enabled
    property alias  rejectButtonEnabled:    rejectButton.enabled
    property var    dialogProperties
    property bool   destroyOnClose:         true
    property bool   preventClose:           false
    property bool   bypassNavigationCheck:  false
    property bool   closeOnOutsideClick:    false
    property bool   draggable: true
    property point  _dragOffset: Qt.point(0, 0)
    property bool   resizable: false
    property real   preferredDialogWidth: maxContentAvailableWidth * 0.8
    property real   preferredDialogHeight: maxContentAvailableHeight * 0.85
    property real   minimumDialogWidth: ScreenTools.defaultFontPixelWidth * 44
    property real   minimumDialogHeight: ScreenTools.defaultFontPixelHeight * 8
    readonly property real dialogContentWidth: contentFlickable.width
    readonly property real dialogContentHeight: contentFlickable.height
    property size   _resizedSize: Qt.size(0, 0)

    property real maxContentAvailableWidth:    mainWindow.width - _contentMargin * 6
    property real maxContentAvailableHeight:   mainWindow.height - titleRowLayout.height - _contentMargin * 7

    readonly property real headerMinWidth: titleLabel.implicitWidth + rejectButton.width + acceptButton.width + titleRowLayout.spacing * 2

    signal accepted
    signal rejected

    property var    _qgcPal:            QGroundControl.globalPalette
    property real   _frameSize:         ScreenTools.defaultFontPixelWidth
    property real   _contentMargin:     ScreenTools.defaultFontPixelHeight / 2
    property bool   _acceptAllowed:     acceptButton.visible
    property bool   _rejectAllowed:     rejectButton.visible
    property int    _previousValidationErrorCount: 0

    background: QGCMouseArea {
        width:  mainWindow.width
        height: mainWindow.height

        onClicked: (mouse) => {
            if (root.closeOnOutsideClick) {
                const point = mapToItem(root.contentItem, mouse.x, mouse.y)
                if (point.x < mainLayout.x - root._contentMargin
                    || point.x > mainLayout.x + mainLayout.width + root._contentMargin
                    || point.y < mainLayout.y - root._contentMargin
                    || point.y > mainLayout.y + mainLayout.height + root._contentMargin) {
                    root._reject()
                }
            } else if (closePolicy & Popup.CloseOnPressOutside) {
                _reject()
            }
        }
    }

    // We use this to track when the original parent of the dialog is destroyed. This allows us to automatically close the dialog when that happens which prevents
    // orphaned dialogs which cause crashes.
    Connections {
        id: originalParentConnections
        ignoreUnknownSignals: true // Prevents warning from initial connection when parent is null
        onDestroyed: root.close()
    }

    Component.onCompleted: {
        originalParentConnections.target = parent
        parent = Overlay.overlay
    }

    onAboutToShow: {
        _dragOffset = Qt.point(0, 0)
        _resizedSize = Qt.size(0, 0)
        _previousValidationErrorCount = globals.validationErrorCount
        setupDialogButtons(buttons)
    }

    onClosed: {
        globals.validationErrorCount = _previousValidationErrorCount
        Qt.inputMethod.hide()
        if (destroyOnClose) {
            root.destroy()
        }
    }

    onButtonsChanged: setupDialogButtons(buttons)

    function _accept() {
        if (_acceptAllowed && (bypassNavigationCheck || mainWindow.allowViewSwitch(_previousValidationErrorCount))) {
            accepted()
            if (preventClose) {
                preventClose = false
            } else {
                close()
            }
        }
    }

    function _reject() {
        // Dialogs with cancel button are allowed to close with validation errors
        if (_rejectAllowed && ((buttons & Dialog.Cancel) || bypassNavigationCheck || mainWindow.allowViewSwitch(_previousValidationErrorCount))) {
            rejected()
            if (preventClose) {
                preventClose = false
            } else {
                close()
            }
        }
    }

    QGCPalette { id: qgcPal; colorGroupEnabled: parent.enabled }

    function setupDialogButtons(buttons) {
        acceptButton.visible = false
        rejectButton.visible = false
        // Accept role buttons
        if (buttons & Dialog.Ok) {
            acceptButton.text = qsTr("Ok")
            acceptButton.visible = true
        } else if (buttons & Dialog.Open) {
            acceptButton.text = qsTr("Open")
            acceptButton.visible = true
        } else if (buttons & Dialog.Save) {
            acceptButton.text = qsTr("Save")
            acceptButton.visible = true
        } else if (buttons & Dialog.Apply) {
            acceptButton.text = qsTr("Apply")
            acceptButton.visible = true
        } else if (buttons & Dialog.SaveAll) {
            acceptButton.text = qsTr("Save All")
            acceptButton.visible = true
        } else if (buttons & Dialog.Yes) {
            acceptButton.text = qsTr("Yes")
            acceptButton.visible = true
        } else if (buttons & Dialog.YesToAll) {
            acceptButton.text = qsTr("Yes to All")
            acceptButton.visible = true
        } else if (buttons & Dialog.Retry) {
            acceptButton.text = qsTr("Retry")
            acceptButton.visible = true
        } else if (buttons & Dialog.Reset) {
            acceptButton.text = qsTr("Reset")
            acceptButton.visible = true
        } else if (buttons & Dialog.RestoreToDefaults) {
            acceptButton.text = qsTr("Restore to Defaults")
            acceptButton.visible = true
        } else if (buttons & Dialog.Ignore) {
            acceptButton.text = qsTr("Ignore")
            acceptButton.visible = true
        }

        // Reject role buttons
        if (buttons & Dialog.Cancel) {
            rejectButton.text = qsTr("Cancel")
            rejectButton.visible = true
        } else if (buttons & Dialog.Close) {
            rejectButton.text = qsTr("Close")
            rejectButton.visible = true
        } else if (buttons & Dialog.No) {
            rejectButton.text = qsTr("No")
            rejectButton.visible = true
        } else if (buttons & Dialog.NoToAll) {
            rejectButton.text = qsTr("No to All")
            rejectButton.visible = true
        } else if (buttons & Dialog.Abort) {
            rejectButton.text = qsTr("Abort")
            rejectButton.visible = true
        }

        closePolicy = Popup.NoAutoClose
        if (buttons & Dialog.Cancel) {
            closePolicy |= Popup.CloseOnEscape
        }
    }

    function disableAcceptButton() {
        acceptButton.enabled = false
    }

    Rectangle {
        id: dialogFrame
        x:              mainLayout.x - _contentMargin
        y:              mainLayout.y - _contentMargin
        width:          mainLayout.width + _contentMargin * 2
        height:         mainLayout.height + _contentMargin * 2
        color:          _qgcPal.windowShade
        radius:         root.padding / 2
        border.width:   1
        border.color:   _qgcPal.windowShadeLight
    }

    ColumnLayout {
        id:                 mainLayout
        objectName:         "popupDialog_frame"
        x: Math.max(_contentMargin, Math.min((parent.width - width) / 2 + root._dragOffset.x,
            parent.width - width - _contentMargin))
        y: Math.max(_contentMargin, Math.min((parent.height - height) / 2 + root._dragOffset.y,
            parent.height - height - _contentMargin))
        spacing:            _contentMargin

        RowLayout {
            id:                     titleRowLayout
            Layout.fillWidth:       true
            spacing:                _contentMargin

            QGCLabel {
                id:                 titleLabel
                objectName:         "popupDialog_title"
                Layout.fillWidth:   true
                text:               root.title
                font.pointSize:     ScreenTools.mediumFontPointSize
                verticalAlignment:	Text.AlignVCenter
                MouseArea {
                    objectName: "popupDialog_dragHandle"
                    anchors.fill: parent
                    enabled: root.draggable
                    acceptedButtons: Qt.LeftButton
                    cursorShape: pressed ? Qt.ClosedHandCursor : Qt.OpenHandCursor
                    property point _pressPosition
                    property point _initialOffset
                    onPressed: (mouse) => {
                        _pressPosition = mapToItem(root.contentItem, mouse.x, mouse.y)
                        _initialOffset = root._dragOffset
                    }
                    onPositionChanged: (mouse) => {
                        if (pressed) {
                            const position = mapToItem(root.contentItem, mouse.x, mouse.y)
                            const centerX = (root.contentItem.width - mainLayout.width) / 2
                            const centerY = (root.contentItem.height - mainLayout.height) / 2
                            root._dragOffset = Qt.point(
                                Math.max(_contentMargin, Math.min(centerX + _initialOffset.x + position.x - _pressPosition.x,
                                    root.contentItem.width - mainLayout.width - _contentMargin)) - centerX,
                                Math.max(_contentMargin, Math.min(centerY + _initialOffset.y + position.y - _pressPosition.y,
                                    root.contentItem.height - mainLayout.height - _contentMargin)) - centerY)
                        }
                    }
                }
            }

            QGCButton {
                id:                     rejectButton
                objectName:             "popupDialog_rejectButton"
                onClicked:              _reject()
                Layout.minimumWidth:    height * 1.5
            }

            QGCButton {
                id:                     acceptButton
                objectName:             "popupDialog_acceptButton"
                primary:                true
                onClicked:              _accept()
                Layout.minimumWidth:    height * 1.5
            }
        }

        Rectangle {
            id: contentFrame
            Layout.fillWidth:       true
            Layout.preferredWidth: root.resizable
                ? Math.min(maxAvailableWidth, Math.max(minimumWidth, root._resizedSize.width || root.preferredDialogWidth))
                : Math.min(maxAvailableWidth, totalContentWidth)
            Layout.preferredHeight: root.resizable
                ? Math.min(maxAvailableHeight, Math.max(minimumHeight, root._resizedSize.height || root.preferredDialogHeight))
                : Math.min(maxAvailableHeight, totalContentHeight)
            color:                  _qgcPal.window

            readonly property real minimumWidth: Math.max(root.headerMinWidth, root.minimumDialogWidth)
            readonly property real minimumHeight: root.minimumDialogHeight
            property real totalContentWidth:    dialogContentParent.childrenRect.width + _contentMargin * 2
            property real totalContentHeight:   dialogContentParent.childrenRect.height + _contentMargin * 2
            property real maxAvailableWidth:    mainWindow.width - _contentMargin * 4
            property real maxAvailableHeight:   mainWindow.height - titleRowLayout.height - _contentMargin * 5

            QGCFlickable {
                id: contentFlickable
                objectName: "popupDialog_contentFlickable"
                anchors.margins:    _contentMargin
                anchors.fill:       parent
                contentWidth:       dialogContentParent.childrenRect.width
                contentHeight:      dialogContentParent.childrenRect.height

                Item {
                    id:     dialogContentParent
                    focus:  true

                    Keys.onPressed: (event) => {
                        if (event.key === Qt.Key_Escape && _rejectAllowed) {
                            _reject()
                            event.accepted = true
                        }
                    }
                }
            }
        }
    }

    Repeater {
        model: root.resizable ? [Qt.LeftEdge, Qt.RightEdge, Qt.TopEdge, Qt.BottomEdge,
            Qt.LeftEdge | Qt.TopEdge, Qt.RightEdge | Qt.TopEdge,
            Qt.LeftEdge | Qt.BottomEdge, Qt.RightEdge | Qt.BottomEdge] : []

        delegate: MouseArea {
            id: resizeHandle
            required property int modelData
            readonly property bool leftEdge: (modelData & Qt.LeftEdge) !== 0
            readonly property bool rightEdge: (modelData & Qt.RightEdge) !== 0
            readonly property bool topEdge: (modelData & Qt.TopEdge) !== 0
            readonly property bool bottomEdge: (modelData & Qt.BottomEdge) !== 0
            readonly property bool corner: (leftEdge || rightEdge) && (topEdge || bottomEdge)
            readonly property real grip: Math.max(10, root._contentMargin)
            property point pressPosition
            property rect initialFrame
            property real headerHeight

            objectName: "popupDialog_resizeHandle_" + modelData
            x: leftEdge ? dialogFrame.x - grip / 2
                : rightEdge ? dialogFrame.x + dialogFrame.width - grip / 2 : dialogFrame.x + grip / 2
            y: topEdge ? dialogFrame.y - grip / 2
                : bottomEdge ? dialogFrame.y + dialogFrame.height - grip / 2 : dialogFrame.y + grip / 2
            width: leftEdge || rightEdge ? grip : dialogFrame.width - grip
            height: topEdge || bottomEdge ? grip : dialogFrame.height - grip
            z: 100
            acceptedButtons: Qt.LeftButton
            preventStealing: true
            cursorShape: corner ? (leftEdge === topEdge ? Qt.SizeFDiagCursor : Qt.SizeBDiagCursor)
                : leftEdge || rightEdge ? Qt.SizeHorCursor : Qt.SizeVerCursor

            onPressed: (mouse) => {
                pressPosition = mapToItem(root.contentItem, mouse.x, mouse.y)
                initialFrame = Qt.rect(mainLayout.x, mainLayout.y, mainLayout.width, mainLayout.height)
                headerHeight = mainLayout.height - contentFrame.height
            }
            onPositionChanged: (mouse) => {
                if (!pressed) { return }
                const position = mapToItem(root.contentItem, mouse.x, mouse.y)
                const dx = position.x - pressPosition.x
                const dy = position.y - pressPosition.y
                const margin = root._contentMargin
                const minWidth = Math.min(contentFrame.minimumWidth, contentFrame.maxAvailableWidth)
                const minHeight = Math.min(contentFrame.minimumHeight, contentFrame.maxAvailableHeight) + headerHeight
                let left = initialFrame.x
                let top = initialFrame.y
                let right = left + initialFrame.width
                let bottom = top + initialFrame.height
                const maxWidth = contentFrame.maxAvailableWidth
                const maxHeight = contentFrame.maxAvailableHeight + headerHeight
                if (leftEdge) { left = Math.max(margin, right - maxWidth, Math.min(left + dx, right - minWidth)) }
                if (rightEdge) { right = Math.min(root.contentItem.width - margin, left + maxWidth, Math.max(right + dx, left + minWidth)) }
                if (topEdge) { top = Math.max(margin, bottom - maxHeight, Math.min(top + dy, bottom - minHeight)) }
                if (bottomEdge) { bottom = Math.min(root.contentItem.height - margin, top + maxHeight, Math.max(bottom + dy, top + minHeight)) }
                root._resizedSize = Qt.size(right - left, bottom - top - headerHeight)
                root._dragOffset = Qt.point((left + right - root.contentItem.width) / 2,
                    (top + bottom - root.contentItem.height) / 2)
            }

            Text {
                anchors.right: parent.horizontalCenter
                anchors.bottom: parent.verticalCenter
                visible: resizeHandle.rightEdge && resizeHandle.bottomEdge
                text: "◢"
                color: root._qgcPal.text
                font.pixelSize: resizeHandle.grip
            }
        }
    }
}
