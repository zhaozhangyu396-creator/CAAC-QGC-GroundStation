import QtQuick
import QtQuick.Layouts

import QGroundControl
import QGroundControl.Controls
import QGroundControl.FactControls

GridLayout {
    id: control
    columns: _stacked ? 1 : 2
    columnSpacing: ScreenTools.defaultFontPixelWidth * 2
    rowSpacing: ScreenTools.defaultFontPixelHeight * 0.25
    Layout.minimumWidth: _responsive ? 0 : -1

    readonly property var settingsViewport: {
        for (let item = parent; item; item = item.parent) {
            if (item.objectName === "settingsPageFlickable") { return item }
        }
        return null
    }
    property string label:                   fact ? OnlineMessageTranslator.parameterText(fact.shortDescription, OnlineMessageTranslator.revision) : ""
    property alias  fact:                    _factTextField.fact
    property real   textFieldPreferredWidth: -1
    property alias  textFieldUnitsLabel:     _factTextField.unitsLabel
    property alias  textFieldShowUnits:      _factTextField.showUnits
    property alias  textFieldShowHelp:       _factTextField.showHelp
    property alias  textField:               _factTextField

    property alias spacing: control.columnSpacing
    readonly property bool vehicleSetupStyle: ScreenTools.isVehicleSetupItem(control)
    readonly property bool _responsive: settingsViewport !== null || vehicleSetupStyle
    readonly property bool _stacked: _responsive
        && (label === "" || width < ScreenTools.defaultFontPixelWidth * 40)

    QGCLabel {
        Layout.fillWidth:    true
        Layout.minimumWidth: control._responsive ? 0 : implicitWidth
        wrapMode: control._responsive ? Text.WordWrap : Text.NoWrap
        text:                label
        visible:             label !== ""
    }

    FactTextField {
        id:                     _factTextField
        objectName: fact ? "factTextField_" + fact.name : ""
        Layout.fillWidth: control._stacked
        Layout.preferredWidth:  textFieldPreferredWidth
        Layout.minimumWidth: control._responsive ? 0 : -1
        Layout.maximumWidth: control._responsive && !control._stacked
            ? Math.max(0, control.width * 0.6) : Number.POSITIVE_INFINITY
    }
}
