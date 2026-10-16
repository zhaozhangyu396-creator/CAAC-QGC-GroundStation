import QtQuick
import QtQuick.Layouts

import QGroundControl
import QGroundControl.Controls
import QGroundControl.FactControls

RowLayout {
    id: control
    readonly property var settingsViewport: {
        for (let item = parent; item; item = item.parent) {
            if (item.objectName === "settingsPageFlickable") { return item }
        }
        return null
    }
    property string label:                   fact ? fact.shortDescription : ""
    property alias  fact:                    _factTextField.fact
    property real   textFieldPreferredWidth: -1
    property alias  textFieldUnitsLabel:     _factTextField.unitsLabel
    property alias  textFieldShowUnits:      _factTextField.showUnits
    property alias  textFieldShowHelp:       _factTextField.showHelp
    property alias  textField:               _factTextField

    spacing: ScreenTools.defaultFontPixelWidth * 2

    QGCLabel {
        Layout.fillWidth:    true
        Layout.minimumWidth: control.settingsViewport !== null ? 0 : implicitWidth
        wrapMode: control.settingsViewport !== null ? Text.WordWrap : Text.NoWrap
        text:                label
        visible:             label !== ""
    }

    FactTextField {
        id:                     _factTextField
        Layout.preferredWidth:  textFieldPreferredWidth
        Layout.minimumWidth: control.settingsViewport !== null ? 0 : -1
        Layout.maximumWidth: control.settingsViewport !== null
            ? Math.max(0, control.settingsViewport.width * 0.6) : Number.POSITIVE_INFINITY
    }
}
