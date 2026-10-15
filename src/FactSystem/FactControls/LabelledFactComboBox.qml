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
    property alias label:                   label.text
    property alias fact:                    _comboBox.fact
    property alias indexModel:              _comboBox.indexModel
    property var   comboBox:                _comboBox
    property real  comboBoxPreferredWidth:  -1

    spacing: ScreenTools.defaultFontPixelWidth * 2

    signal activated(int index)

    QGCLabel {
        id:                 label
        Layout.fillWidth:   true
        Layout.minimumWidth: control.settingsViewport !== null ? 0 : implicitWidth
        wrapMode: control.settingsViewport !== null ? Text.WordWrap : Text.NoWrap
    }

    FactComboBox {
        id:                     _comboBox
        Layout.preferredWidth:  comboBoxPreferredWidth
        Layout.minimumWidth: control.settingsViewport !== null ? 0 : -1
        Layout.maximumWidth: control.settingsViewport !== null ? Math.max(0, control.settingsViewport.width * 0.6) : Number.POSITIVE_INFINITY
        sizeToContents:         true

        onActivated: (index) => { parent.activated(index) }
    }
}
