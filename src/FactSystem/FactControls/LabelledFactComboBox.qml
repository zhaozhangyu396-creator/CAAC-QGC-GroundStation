import QtQuick
import QtQuick.Layouts

import QGroundControl
import QGroundControl.Controls
import QGroundControl.FactControls

RowLayout {
    id: control
    readonly property bool settingsMenuStyle: ScreenTools.isSettingsMenuItem(control)
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
        Layout.minimumWidth: control.settingsMenuStyle ? 0 : implicitWidth
        wrapMode: control.settingsMenuStyle ? Text.WordWrap : Text.NoWrap
    }

    FactComboBox {
        id:                     _comboBox
        Layout.preferredWidth:  comboBoxPreferredWidth
        Layout.minimumWidth: control.settingsMenuStyle ? 0 : implicitWidth
        Layout.maximumWidth: control.settingsMenuStyle ? Math.max(0, control.width * 0.6) : Number.POSITIVE_INFINITY
        sizeToContents:         true

        onActivated: (index) => { parent.activated(index) }
    }
}
