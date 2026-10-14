import QtQuick
import QtQuick.Controls

import QGroundControl
import QGroundControl.Controls

Button {
    id:             control
    objectName:     toolStripAction ? toolStripAction.objectName : ""
    width:          contentLayoutItem.contentWidth + (contentMargins * 2)
    height:         width
    hoverEnabled:   !ScreenTools.isMobile
    enabled:        toolStripAction ? toolStripAction.enabled : true
    visible:        toolStripAction ? toolStripAction.visible : true
    imageSource:    (toolStripAction && modelData) ? (toolStripAction.showAlternateIcon ? modelData.alternateIconSource : modelData.iconSource) : ""
    text:           toolStripAction ? toolStripAction.text : ""
    checked:        toolStripAction ? toolStripAction.checked : false
    checkable:      toolStripAction ? (toolStripAction.dropPanelComponent || (modelData && modelData.checkable)) : false

    property var    toolStripAction:    undefined
    property var    dropPanel:          undefined
    property alias  radius:             buttonBkRect.radius
    property alias  fontPointSize:      innerText.font.pointSize
    property alias  imageSource:        innerImage.source
    property alias  contentWidth:       innerText.contentWidth

    property bool roundStyle: false
    property bool squareStyle: false
    property bool compactLabel: false
    property real maximumRoundDiameter: Infinity
    property bool doubleClickRequired: false
    readonly property real roundDiameter: Math.max(0, Math.min(ScreenTools.defaultFontPixelHeight * 3,
        width - contentMargins * 2, maximumRoundDiameter))
    property bool forceImageScale11: false
    property real imageScale:        forceImageScale11 && (text == "") ? 0.8 : 0.6
    property real contentMargins:    roundStyle ? ScreenTools.defaultFontPixelWidth * 0.3 : innerText.height * 0.1

    property color _currentContentColor: squareStyle ? "white" : roundStyle ? (qgcPal.globalTheme === QGCPalette.Dark ? qgcPal.windowShadeDark : qgcPal.text) : (checked || pressed) ? qgcPal.buttonHighlightText : qgcPal.text
    property color _currentContentColorSecondary:  (checked || pressed) ? qgcPal.text : qgcPal.buttonHighlight

    signal dropped(int index)

    onCheckedChanged: { if (toolStripAction) toolStripAction.checked = checked }

    function triggerAction() {
        if (mainWindow.allowViewSwitch()) {
            dropPanel.hide()
            if (!toolStripAction.dropPanelComponent) {
                toolStripAction.triggered(this)
            } else if (checked) {
                var panelEdgeTopPoint = mapToItem(_root, width, 0)
                dropPanel.show(panelEdgeTopPoint, toolStripAction.dropPanelComponent, this)
                checked = true
                control.dropped(index)
            }
        } else if (checkable) {
            checked = !checked
        }
    }

    onClicked: { if (!doubleClickRequired) { triggerAction() } }
    Connections {
        target: control.doubleClickRequired ? control : null
        function onDoubleClicked() { control.triggerAction() }
    }

    QGCPalette { id: qgcPal; colorGroupEnabled: control.roundStyle || control.enabled }

    contentItem: Item {
        id:                 contentLayoutItem
        anchors.fill:       parent
        anchors.margins:    contentMargins

        Column {
            anchors.horizontalCenter: parent.horizontalCenter
            y: control.compactLabel ? (parent.height - height + (control.roundStyle ? control.roundDiameter * 0.225 : 0)) / 2
                : control.roundStyle ? control.roundDiameter * 0.225 : (parent.height - height) / 2
            spacing:            control.roundStyle ? control.roundDiameter * 0.3 : 0

            Image {
                id:                         innerImageColorful
                height:                     control.roundStyle ? control.roundDiameter * 0.55 : contentLayoutItem.height * imageScale
                width:                      control.roundStyle ? height : contentLayoutItem.width  * imageScale
                smooth:                     true
                mipmap:                     true
                fillMode:                   Image.PreserveAspectFit
                antialiasing:               true
                sourceSize.height:          height
                sourceSize.width:           width
                anchors.horizontalCenter:   parent.horizontalCenter
                source:                     control.imageSource
                visible:                    source != "" && !!modelData && modelData.fullColorIcon
            }

            QGCColoredImage {
                id:                         innerImage
                objectName:                 control.objectName + "_icon"
                height:                     control.roundStyle ? control.roundDiameter * 0.55 : contentLayoutItem.height * imageScale
                width:                      control.roundStyle ? height : contentLayoutItem.width  * imageScale
                smooth:                     true
                mipmap:                     true
                color:                      _currentContentColor
                fillMode:                   Image.PreserveAspectFit
                antialiasing:               true
                sourceSize.height:          height
                sourceSize.width:           width
                anchors.horizontalCenter:   parent.horizontalCenter
                visible:                    source != "" && !(modelData && modelData.fullColorIcon)

                Rectangle {
                    anchors.centerIn: parent
                    width: control.roundDiameter
                    height: width
                    radius: control.squareStyle ? ScreenTools.defaultFontPixelWidth * 0.7 : width / 2
                    visible: control.roundStyle
                    z: -1
                    color: control.squareStyle ? (control.checked || control.pressed ? "#245b46" : "#d9232629") : control.checked || control.pressed ? qgcPal.buttonHighlight
                        : (qgcPal.globalTheme === QGCPalette.Dark ? qgcPal.text : qgcPal.button)
                    opacity: control.enabled ? 1 : 0.45
                    border.width: control.squareStyle ? 1 : control.hovered && control.enabled ? ScreenTools.defaultFontPixelWidth * 0.2 : 0
                    border.color: control.squareStyle ? (control.checked || control.hovered ? "#40d58b" : "#a0a5aa") : qgcPal.buttonHighlight
                }

                QGCColoredImage {
                    id:                         innerImageSecondColor
                    source:                     modelData ? modelData.alternateIconSource : ""
                    height:                     control.roundStyle ? control.roundDiameter * 0.55 : contentLayoutItem.height * imageScale
                    width:                      control.roundStyle ? height : contentLayoutItem.width  * imageScale
                    smooth:                     true
                    mipmap:                     true
                    color:                      _currentContentColorSecondary
                    fillMode:                   Image.PreserveAspectFit
                    antialiasing:               true
                    sourceSize.height:          height
                    sourceSize.width:           width
                    anchors.horizontalCenter:   parent.horizontalCenter
                    visible:                    source != "" && !!modelData && modelData.biColorIcon
                }
            }

            QGCLabel {
                id:                         innerText
                objectName:                 control.objectName + "_label"
                style:                      control.roundStyle ? Text.Outline : Text.Normal
                styleColor:                 qgcPal.windowShadeDark
                text:                       control.text
                width:                      control.roundStyle ? control.width - control.contentMargins * 2 : implicitWidth
                wrapMode:                   control.roundStyle && !control.compactLabel ? Text.Wrap : Text.NoWrap
                maximumLineCount:           control.compactLabel ? 1 : control.roundStyle ? 2 : 2147483647
                fontSizeMode:               control.compactLabel ? Text.Fit : Text.FixedSize
                minimumPointSize:           ScreenTools.smallFontPointSize * 0.6
                elide:                      control.roundStyle ? Text.ElideRight : Text.ElideNone
                horizontalAlignment:        Text.AlignHCenter
                font.pointSize:             ScreenTools.menuFontPointSize
                color:                      control.roundStyle ? qgcPal.text : _currentContentColor
                anchors.horizontalCenter:   parent.horizontalCenter
                font.bold:                  !innerImage.visible && !innerImageColorful.visible
                opacity:                    control.roundStyle && !control.enabled ? 0.45 : !innerImage.visible ? 0.8 : 1.0
            }
        }
    }

    background: Rectangle {
        id:     buttonBkRect
        color:  control.roundStyle ? "transparent" : (control.checked || control.pressed) ?
                    qgcPal.buttonHighlight :
                    ((control.enabled && control.hovered) ? qgcPal.toolStripHoverColor : "transparent")
    }
}
