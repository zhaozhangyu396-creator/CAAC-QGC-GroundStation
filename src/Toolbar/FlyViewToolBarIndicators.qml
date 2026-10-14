import QtQuick

import QGroundControl
import QGroundControl.Controls
import QGroundControl.Toolbar

Item {
    id: control
    objectName:    "flyViewToolBarIndicators"
    implicitWidth: mainLayout.width + _widthMargin

    property var  _activeVehicle:           QGroundControl.multiVehicleManager.activeVehicle
    property bool hideSignalStrengthIndicators: false
    function indicatorVisible(source, item) {
        return !!item && item.showIndicator && (!hideSignalStrengthIndicators
            || (!source.toString().endsWith("/RCRSSIIndicator.qml") && !source.toString().endsWith("/TelemetryRSSIIndicator.qml")))
    }
    property real _toolIndicatorMargins:    ScreenTools.defaultFontPixelHeight * (QGroundControl.corePlugin.directMissionItemEditing ? 0.25 : 0.66)
    property real _widthMargin:             _toolIndicatorMargins * 2

    Row {
        id:                 mainLayout
        anchors.margins:    _toolIndicatorMargins
        anchors.left:       parent.left
        anchors.top:        parent.top
        anchors.bottom:     parent.bottom
        spacing:            ScreenTools.defaultFontPixelWidth * (QGroundControl.corePlugin.directMissionItemEditing ? 0.65 : 1.75)

        Repeater {
            id:     appRepeater
            model:  QGroundControl.corePlugin.toolBarIndicators
            Loader {
                anchors.top:        parent.top
                anchors.bottom:     parent.bottom
                source:             modelData
                visible:            control.indicatorVisible(source, item)
            }
        }

        Repeater {
            id:     toolIndicatorsRepeater
            model:  _activeVehicle ? _activeVehicle.toolIndicators : []

            Loader {
                anchors.top:        parent.top
                anchors.bottom:     parent.bottom
                source:             modelData
                visible:            control.indicatorVisible(source, item)
            }
        }
    }
}
