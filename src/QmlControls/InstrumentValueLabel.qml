import QtQuick
import QtQuick.Layouts
import QtQuick.Controls

import QGroundControl
import QGroundControl.Controls

ColumnLayout {
    property var    instrumentValueData:        null

    property var    _rgFontSizes:               [ ScreenTools.defaultFontPointSize, ScreenTools.smallFontPointSize, ScreenTools.mediumFontPointSize, ScreenTools.largeFontPointSize ]
    property var    _rgFontSizeRatios:          [ 1, ScreenTools.smallFontPointRatio, ScreenTools.mediumFontPointRatio, ScreenTools.largeFontPointRatio ]
    property real   _doubleDescent:             ScreenTools.defaultFontDescent * 2
    property real   _tightDefaultFontHeight:    ScreenTools.defaultFontPixelHeight - _doubleDescent
    property var    _rgFontSizeTightHeights:    [ _tightDefaultFontHeight * _rgFontSizeRatios[0] + 2, _tightDefaultFontHeight * _rgFontSizeRatios[1] + 2, _tightDefaultFontHeight * _rgFontSizeRatios[2] + 2, _tightDefaultFontHeight * _rgFontSizeRatios[3] + 2 ]
    property real   _tightHeight:               _rgFontSizeTightHeights[instrumentValueData.factValueGrid.fontSize]
    property bool   _iconVisible:               instrumentValueData.rangeType === InstrumentValueData.IconSelectRange || instrumentValueData.icon
    property var    _color:                     instrumentValueData.isValidColor(instrumentValueData.currentColor) ? instrumentValueData.currentColor : qgcPal.text
    readonly property string _label: {
        if (instrumentValueData.factGroupName === "Vehicle") {
            switch (instrumentValueData.factName.toLowerCase()) {
            case "altituderelative": return qsTr("Relative altitude")
            case "distancetohome": return qsTr("Distance to home")
            case "climbrate": return qsTr("Vertical speed")
            case "groundspeed": return qsTr("Ground speed")
            case "airspeed": return qsTr("Airspeed")
            case "flighttime": return qsTr("Flight time")
            case "flightdistance": return qsTr("Flight distance")
            case "heading": return qsTr("Heading")
            }
        }
        return instrumentValueData.text
    }

    QGCPalette { id: qgcPal; colorGroupEnabled: enabled }

    QGCColoredImage {
        id:                         valueIcon
        Layout.alignment:           Qt.AlignVCenter
        height:                     _tightHeight * 0.75
        width:                      _tightHeight * 0.85
        sourceSize.height:          height
        fillMode:                   Image.PreserveAspectFit
        mipmap:                     true
        smooth:                     true
        color:                      _color
        opacity:                    instrumentValueData.currentOpacity
        visible:                    false

        readonly property string iconPrefix: "/InstrumentValueIcons/"

        function updateIcon() {
            if (instrumentValueData.rangeType === InstrumentValueData.IconSelectRange) {
                valueIcon.source = instrumentValueData.currentIcon != "" ? iconPrefix + instrumentValueData.currentIcon : "";
            } else if (instrumentValueData.icon) {
                valueIcon.source = instrumentValueData.icon != "" ? iconPrefix + instrumentValueData.icon : "";
            } else {
                valueIcon.source = ""
            }
        }

        Connections {
            target:                 instrumentValueData
            function onRangeTypeChanged() { valueIcon.updateIcon() }
            function onCurrentIconChanged() { valueIcon.updateIcon() }
            function onIconChanged() { valueIcon.updateIcon() }
        }
        Component.onCompleted:      updateIcon();
    }

    QGCLabel {
        Layout.alignment:   Qt.AlignVCenter
        height:             _tightHeight
        font.pointSize:     ScreenTools.smallFontPointSize
        text:               _label
        color:              _color
        opacity:            instrumentValueData.currentOpacity
        visible:            _label.length > 0
    }
}
