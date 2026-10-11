import QtQuick
import QtQuick.Layouts

import QGroundControl
import QGroundControl.Controls

RowLayout {
    id: root
    required property int maximumDegrees
    required property string fieldName
    property real value: NaN
    property bool displayDms: false
    property real _canonicalValue: NaN
    property bool _dirty: false
    property bool _ready: false
    property bool _rendering: false
    readonly property real coordinate: !_dirty ? _canonicalValue : displayDms
        ? QGroundControl.corePlugin.coordinateFromDms(number(degrees.text), number(minutes.text), number(seconds.text), hemisphere.currentIndex === 1, maximumDegrees)
        : number(decimal.text)
    spacing: ScreenTools.defaultFontPixelWidth * 0.5

    function number(text) { return text.trim().length ? Number(text) : NaN }
    function markEdited() { if (_ready && !_rendering) { _dirty = true } }
    function render() {
        _rendering = true
        decimal.text = isFinite(_canonicalValue) ? _canonicalValue.toString() : ""
        const dms = QGroundControl.corePlugin.coordinateToDms(_canonicalValue, maximumDegrees)
        degrees.text = dms.degrees !== undefined ? dms.degrees.toString() : ""
        minutes.text = dms.minutes !== undefined ? dms.minutes.toString() : ""
        seconds.text = dms.seconds !== undefined ? dms.seconds.toFixed(9) : ""
        hemisphere.currentIndex = dms.negative ? 1 : 0
        _dirty = false
        _rendering = false
    }
    function toggleFormat() {
        const current = coordinate
        if (!isFinite(current) || Math.abs(current) > maximumDegrees) {
            return
        }
        _canonicalValue = current
        displayDms = !displayDms
        render()
    }
    onValueChanged: { if (_ready) { _canonicalValue = value; render() } }
    Component.onCompleted: { _canonicalValue = value; render(); _ready = true }

    QGCTextField {
        id: decimal
        objectName: root.fieldName
        Layout.fillWidth: true
        Layout.minimumWidth: 0
        visible: !root.displayDms
        validator: DoubleValidator { bottom: -root.maximumDegrees; top: root.maximumDegrees; decimals: 17; locale: "C" }
        onTextChanged: root.markEdited()
    }
    QGCTextField {
        id: degrees
        objectName: root.fieldName + "Degrees"
        Layout.fillWidth: true
        Layout.minimumWidth: ScreenTools.defaultFontPixelWidth * 7
        visible: root.displayDms
        showUnits: true
        unitsLabel: "°"
        validator: IntValidator { bottom: 0; top: root.maximumDegrees }
        onTextChanged: root.markEdited()
    }
    QGCTextField {
        id: minutes
        objectName: root.fieldName + "Minutes"
        Layout.fillWidth: true
        Layout.minimumWidth: ScreenTools.defaultFontPixelWidth * 6
        visible: root.displayDms
        showUnits: true
        unitsLabel: "′"
        validator: IntValidator { bottom: 0; top: 59 }
        onTextChanged: root.markEdited()
    }
    QGCTextField {
        id: seconds
        objectName: root.fieldName + "Seconds"
        Layout.fillWidth: true
        Layout.minimumWidth: ScreenTools.defaultFontPixelWidth * 16
        visible: root.displayDms
        showUnits: true
        unitsLabel: "″"
        validator: DoubleValidator { bottom: 0; top: 60; decimals: 9; locale: "C" }
        onTextChanged: root.markEdited()
    }
    QGCComboBox {
        id: hemisphere
        objectName: root.fieldName + "Hemisphere"
        Layout.minimumWidth: ScreenTools.defaultFontPixelWidth * 10
        visible: root.displayDms
        model: root.maximumDegrees === 90 ? [qsTr("North"), qsTr("South")] : [qsTr("East"), qsTr("West")]
        onCurrentIndexChanged: root.markEdited()
    }
    QGCButton {
        objectName: root.fieldName + "FormatToggle"
        text: "⇄"
        Accessible.name: qsTr("Switch coordinate format")
        enabled: isFinite(root.coordinate) && Math.abs(root.coordinate) <= root.maximumDegrees
        onClicked: root.toggleFormat()
    }
}
