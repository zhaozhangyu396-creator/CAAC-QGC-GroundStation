import QtQuick
import QtQuick.Controls

import QGroundControl
import QGroundControl.Controls

Rectangle {
    id:         _root
    color:      roundButtons ? "transparent" : qgcPal.windowTransparent
    readonly property real _buttonWidth: {
        void _buttonRevision
        if (roundButtons && horizontal) {
            return Math.min(ScreenTools.defaultFontPixelHeight * 3.6,
                Math.max(0, (maxWidth - flickable.anchors.margins * 2
                    - _buttonSpacing * Math.max(0, _roundColumns - 1)) / Math.max(1, _roundColumns)))
        }
        let textWidth = 0
        for (let i = 0; i < repeater.count; ++i) {
            const button = repeater.itemAt(i)
            if (button && button.visible) {
                textWidth = Math.max(textWidth, button.contentWidth)
            }
        }
        return Math.max(ScreenTools.menuFontPixelWidth * 7, textWidth + ScreenTools.defaultFontPixelWidth * 2)
    }
    width:      horizontal ? Math.min(maxWidth, toolStripColumn.implicitWidth + flickable.anchors.margins * 2) : _buttonWidth
    height:     Math.min(maxHeight, toolStripColumn.height + (flickable.anchors.margins * 2))
    radius:     ScreenTools.defaultFontPixelWidth / 2

    readonly property int _visibleButtonCount: {
        void _buttonRevision
        let count = 0
        for (let i = 0; i < repeater.count; ++i) {
            if (repeater.itemAt(i) && repeater.itemAt(i).visible) { ++count }
        }
        return Math.max(1, count)
    }
    readonly property int _roundColumns: horizontalColumns > 0 ? horizontalColumns : maxWidth < _visibleButtonCount * ScreenTools.defaultFontPixelHeight * 3.6
        + (_visibleButtonCount - 1) * _buttonSpacing + flickable.anchors.margins * 2
        ? Math.ceil(_visibleButtonCount / 2) : _visibleButtonCount

    property alias  model:              repeater.model
    property bool   horizontal: false
    property bool   roundButtons: false
    property int    horizontalColumns: 0
    readonly property real roundButtonDiameter: Math.max(0, Math.min(ScreenTools.defaultFontPixelHeight * 3,
        _buttonWidth - ScreenTools.defaultFontPixelWidth * 1.4))
    property real   maxWidth: Infinity
    property real   maxHeight           ///< Maximum height for control, determines whether text is hidden to make control shorter
    property var    fontSize:           ScreenTools.menuFontPointSize

    property var _dropPanel: dropPanel
    property int _buttonRevision: 0
    readonly property real _buttonSpacing: ScreenTools.defaultFontPixelWidth * (roundButtons ? 0.65 : 0.25)

    function simulateClick(buttonIndex) {
        var button = toolStripColumn.children[buttonIndex]
        if (button.checkable) {
            button.checked = !button.checked
        }
        button.clicked()
    }

    signal dropped(int index)

    DeadMouseArea {
        anchors.fill: parent
    }

    QGCFlickable {
        id:                 flickable
        objectName:          _root.objectName.length > 0 ? _root.objectName + "_flickable" : "toolStrip_flickable"
        anchors.margins:    ScreenTools.defaultFontPixelWidth * 0.4
        anchors.fill:       parent
        contentHeight:      toolStripColumn.height
        contentWidth:       _root.horizontal ? toolStripColumn.implicitWidth : width
        interactive: !(_root.roundButtons && _root.horizontal)
        flickableDirection: _root.horizontal ? Flickable.HorizontalFlick : Flickable.VerticalFlick
        ScrollBar.horizontal: ScrollBar { policy: _root.horizontal && !_root.roundButtons ? ScrollBar.AsNeeded : ScrollBar.AlwaysOff }
        clip:               true

        Grid {
            id:             toolStripColumn
            columns:        _root.horizontal ? (_root.roundButtons ? _root._roundColumns : Math.max(1, repeater.count)) : 1
            width:          _root.horizontal ? implicitWidth : flickable.width
            spacing:        _root._buttonSpacing

            Repeater {
                id: repeater
                onItemAdded: _root._buttonRevision++
                onItemRemoved: _root._buttonRevision++

                ToolStripHoverButton {
                    id:                 buttonTemplate
                    width:              _root._buttonWidth - flickable.anchors.margins * 2
                    height:             _root.roundButtons ? ScreenTools.defaultFontPixelHeight * 4.7 : width
                    roundStyle:         _root.roundButtons
                    radius:             ScreenTools.defaultFontPixelWidth / 2
                    fontPointSize:      _root.roundButtons ? Math.min(_root.fontSize, ScreenTools.smallFontPointSize) : _root.fontSize
                    toolStripAction:    modelData
                    dropPanel:          _dropPanel
                    onDropped: (index) => _root.dropped(index)

                    onCheckedChanged: {
                        // We deal with exclusive check state manually since usinug autoExclusive caused all sorts of crazt problems
                        if (checked) {
                            for (var i=0; i<repeater.count; i++) {
                                if (i != index) {
                                    var button = repeater.itemAt(i)
                                    if (button.checked) {
                                        button.checked = false
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    ToolStripDropPanel {
        id:         dropPanel
        toolStrip:  _root
    }
}
