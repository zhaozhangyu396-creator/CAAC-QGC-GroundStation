import QtQuick
import QtQuick.Window
import QtQuick.Controls
import QtQuick.Templates as T

import QGroundControl
import QGroundControl.Controls

T.ComboBox {
    property bool sizeToContents: false
    property bool wrapCurrentText: false
    property string alternateText: ""
    property var textTransform: null

    function displayString(value) {
        return textTransform ? textTransform(value) : value
    }

    id: control
    padding: ScreenTools.comboBoxPadding
    spacing: ScreenTools.defaultFontPixelWidth
    font.pointSize: ScreenTools.menuFontPointSize
    font.family: ScreenTools.normalFontFamily
    implicitWidth: Math.max(background.implicitWidth,
                            (control.sizeToContents ? _largestTextWidth : contentItem.implicitWidth) + leftPadding + rightPadding + padding)
    implicitHeight: Math.max(background.implicitHeight,
                             Math.max(wrapCurrentText ? contentItem.contentHeight : contentItem.implicitHeight,
                                      indicator ? indicator.implicitHeight : 0) + topPadding + bottomPadding)
    baselineOffset: contentItem.y + text.baselineOffset
    leftPadding: padding + (!control.mirrored || !indicator || !indicator.visible ? 0 : indicator.width + spacing)
    rightPadding: padding + (control.mirrored || !indicator || !indicator.visible ? 0 : indicator.width)

    property real _popupWidth: width
    readonly property real _windowWidth: Window.width
    readonly property real _windowHeight: Window.height
    property real _largestTextWidth: 0
    property bool _onCompleted: false
    property bool _showBorder: qgcPal.globalTheme === QGCPalette.Light
    property bool _showHighlight: enabled && pressed

    QGCPalette { id: qgcPal; colorGroupEnabled: enabled }

    TextMetrics {
        id: textMetrics
        font.family: control.font.family
        font.pointSize: control.font.pointSize
    }

    ItemDelegate {
        id: itemDelegateMetrics
        visible: false
        font.family: control.font.family
        font.pointSize: control.font.pointSize
    }

    function _calcPopupWidth() {
        if (_onCompleted && sizeToContents && control.count > 0) {
            _largestTextWidth = 0
            for (let i = 0; i < control.count; i++) {
                textMetrics.text = control.displayString(control.textAt(i))
                _largestTextWidth = Math.max(textMetrics.width, _largestTextWidth)
            }
            _popupWidth = _largestTextWidth + itemDelegateMetrics.leftPadding + itemDelegateMetrics.rightPadding
        }
    }

    onModelChanged: _calcPopupWidth()
    onCountChanged: _calcPopupWidth()
    onTextTransformChanged: _calcPopupWidth()
    onFontChanged: _calcPopupWidth()

    Component.onCompleted: {
        _onCompleted = true
        _calcPopupWidth()
    }

    // The items in the popup
    delegate: ItemDelegate {
        width: _popupWidth
        height: control.wrapCurrentText
                ? Math.max(contentItem.implicitHeight + topPadding + bottomPadding,
                           Math.round(popupItemMetrics.height * 1.75))
                : Math.round(popupItemMetrics.height * 1.75)

        property string _text: control.textRole ?
                                    (model.hasOwnProperty(control.textRole) ? model[control.textRole] : modelData[control.textRole]) :
                                    modelData

        TextMetrics {
            id: popupItemMetrics
            font: control.font
            text: control.displayString(_text)
        }

        contentItem: Text {
            text: control.displayString(_text)
            font: control.font
            color: control.currentIndex === index ? qgcPal.buttonHighlightText : qgcPal.buttonText
            verticalAlignment: Text.AlignVCenter
            wrapMode: control.wrapCurrentText ? Text.WordWrap : Text.NoWrap
        }

        background: Rectangle {
            color: control.currentIndex === index ? qgcPal.buttonHighlight : qgcPal.button
            Rectangle {
                anchors.bottom: parent.bottom
                width: parent.width
                height: 1
                visible: index < control.count - 1
                color: qgcPal.text
                opacity: 0.35
            }
        }

        highlighted: control.highlightedIndex === index
    }

    indicator: QGCColoredImage {
        anchors.rightMargin: control.padding
        anchors.right: parent.right
        anchors.verticalCenter: parent.verticalCenter
        height: ScreenTools.defaultFontPixelWidth
        width: height
        source: "/qmlimages/arrow-down.png"
        color: qgcPal.buttonText
    }

    // The label of the button
    contentItem: QGCLabel {
        id: text
        text: control.alternateText === "" ? control.displayString(control.currentText) : control.alternateText
        font: control.font
        color: qgcPal.buttonText
        elide: control.wrapCurrentText ? Text.ElideNone : Text.ElideRight
        wrapMode: control.wrapCurrentText ? Text.WordWrap : Text.NoWrap
    }

    background: Rectangle {
        color: qgcPal.button
        border.color: qgcPal.buttonBorder
        border.width: _showBorder ? 1 : 0
        radius: ScreenTools.defaultBorderRadius

        Rectangle {
            anchors.fill: parent
            color: qgcPal.buttonHighlight
            opacity: _showHighlight ? 1 : control.enabled && control.hovered ? .2 : 0
            radius: parent.radius
        }
    }

    popup: T.Popup {
        x: Math.max(-_controlPos.x, Math.min(control.width - _popupWidth, control._windowWidth - _controlPos.x - _popupWidth))
        y: _openAbove ? -height : control.height
        width: _popupWidth
        height: Math.min(contentItem.implicitHeight, _openAbove ? _spaceAbove : _spaceBelow)
        topMargin: 6
        bottomMargin: 6

        readonly property point _controlPos:    control.mapToItem(null, 0, 0)
        readonly property real  _spaceBelow:    Math.max(0, control._windowHeight - _controlPos.y - control.height - bottomMargin)
        readonly property real  _spaceAbove:    Math.max(0, _controlPos.y - topMargin)
        readonly property bool  _openAbove:     contentItem.implicitHeight > _spaceBelow && _spaceAbove > _spaceBelow

        contentItem: ListView {
            clip: true
            implicitHeight: contentHeight
            model: control.delegateModel
            currentIndex: control.highlightedIndex
            highlightMoveDuration: 0

            Rectangle {
                z: 10
                width: parent.width
                height: parent.height
                color: "transparent"
                border.color: qgcPal.text
            }

            T.ScrollIndicator.vertical: ScrollIndicator { }
        }

        background: Rectangle {
            color: qgcPal.window
        }
    }
}
