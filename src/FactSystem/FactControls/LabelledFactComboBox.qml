import QtQuick
import QtQuick.Layouts

import QGroundControl
import QGroundControl.Controls
import QGroundControl.FactControls

GridLayout {
    id: control
    Layout.minimumWidth: settingsViewport !== null ? 0 : -1
    columns: _stacked ? 1 : 2
    columnSpacing: ScreenTools.defaultFontPixelWidth * 2
    rowSpacing: ScreenTools.defaultFontPixelHeight * 0.25

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

    property alias spacing: control.columnSpacing
    readonly property bool _stacked: settingsViewport !== null
        && settingsViewport.width < ScreenTools.defaultFontPixelWidth * 40

    signal activated(int index)

    QGCLabel {
        id:                 label
        Layout.fillWidth:   true
        Layout.minimumWidth: control.settingsViewport !== null ? 0 : implicitWidth
        wrapMode: control.settingsViewport !== null ? Text.WordWrap : Text.NoWrap
    }

    FactComboBox {
        id:                     _comboBox
        Layout.fillWidth: control._stacked
        Layout.preferredWidth:  comboBoxPreferredWidth
        Layout.minimumWidth: control.settingsViewport !== null ? 0 : -1
        Layout.maximumWidth: control.settingsViewport !== null
            ? Math.max(0, control._stacked
                ? control.settingsViewport.width - ScreenTools.defaultFontPixelWidth * 2
                : control.settingsViewport.width * 0.6)
            : Number.POSITIVE_INFINITY
        sizeToContents:         true

        onActivated: (index) => { parent.activated(index) }
    }
}
