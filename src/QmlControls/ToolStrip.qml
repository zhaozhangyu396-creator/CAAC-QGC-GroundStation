import QtQuick
import QtQuick.Controls

import QGroundControl
import QGroundControl.Controls

Rectangle {
    id:         _root
    color:      qgcPal.windowTransparent
    readonly property real _buttonWidth: {
        void _buttonRevision
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

    property alias  model:              repeater.model
    property bool   horizontal: false
    property real   maxWidth: Infinity
    property real   maxHeight           ///< Maximum height for control, determines whether text is hidden to make control shorter
    property var    fontSize:           ScreenTools.menuFontPointSize

    property var _dropPanel: dropPanel
    property int _buttonRevision: 0

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
        flickableDirection: _root.horizontal ? Flickable.HorizontalFlick : Flickable.VerticalFlick
        ScrollBar.horizontal: ScrollBar { policy: _root.horizontal ? ScrollBar.AsNeeded : ScrollBar.AlwaysOff }
        clip:               true

        Grid {
            id:             toolStripColumn
            columns:        _root.horizontal ? Math.max(1, repeater.count) : 1
            width:          _root.horizontal ? implicitWidth : flickable.width
            spacing:        ScreenTools.defaultFontPixelWidth * 0.25

            Repeater {
                id: repeater
                onItemAdded: _root._buttonRevision++
                onItemRemoved: _root._buttonRevision++

                ToolStripHoverButton {
                    id:                 buttonTemplate
                    width:              _root._buttonWidth - flickable.anchors.margins * 2
                    height:             width
                    radius:             ScreenTools.defaultFontPixelWidth / 2
                    fontPointSize:      _root.fontSize
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
