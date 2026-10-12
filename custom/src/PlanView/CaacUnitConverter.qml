pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import QGroundControl
import QGroundControl.Controls

QGCPopupDialog {
    id: root
    objectName: "caacUnitConverterDialog"
    title: qsTr("Unit conversion")
    buttons: Dialog.Close
    readonly property var _units: quantity.currentIndex === 0
        ? [{name: "m", scale: 1}, {name: "km", scale: 1000}, {name: "ft", scale: 0.3048}, {name: "NM", scale: 1852}]
        : quantity.currentIndex === 1
        ? [{name: "m/s", scale: 1}, {name: "km/h", scale: 1 / 3.6}, {name: "kn", scale: 1852 / 3600}, {name: "mph", scale: 1609.344 / 3600}]
        : [{name: "°", scale: Math.PI / 180}, {name: "rad", scale: 1}]
    readonly property real result: value.text.trim().length && value.acceptableInput
        && from.currentIndex >= 0 && from.currentIndex < _units.length && to.currentIndex >= 0 && to.currentIndex < _units.length
        ? Number(value.text) * _units[from.currentIndex].scale / _units[to.currentIndex].scale : NaN

    ColumnLayout {
        width: Math.min(root.maxContentAvailableWidth, ScreenTools.defaultFontPixelWidth * 46)
        spacing: ScreenTools.defaultFontPixelHeight * 0.5
        QGCLabel { text: qsTr("Quantity") }
        QGCComboBox {
            id: quantity
            objectName: "caacConvertQuantity"
            Layout.fillWidth: true
            model: [qsTr("Length"), qsTr("Speed"), qsTr("Angle")]
        }
        QGCLabel { text: qsTr("Input value") }
        QGCTextField {
            id: value
            objectName: "caacConvertValue"
            Layout.fillWidth: true
            text: "100"
            validator: DoubleValidator { locale: "C" }
        }
        QGCLabel { text: qsTr("From unit") }
        QGCComboBox {
            id: from
            objectName: "caacConvertFrom"
            Layout.fillWidth: true
            model: root._units
            textRole: "name"
            onModelChanged: currentIndex = 0
        }
        QGCLabel { text: qsTr("To unit") }
        QGCComboBox {
            id: to
            objectName: "caacConvertTo"
            Layout.fillWidth: true
            model: root._units
            textRole: "name"
            onModelChanged: currentIndex = 1
        }
        QGCLabel {
            objectName: "caacConvertResult"
            Layout.fillWidth: true
            font.bold: true
            font.pointSize: ScreenTools.largeFontPointSize
            wrapMode: Text.WrapAnywhere
            text: isFinite(root.result) ? Number(root.result.toPrecision(12)).toString() + " " + root._units[to.currentIndex].name
                : qsTr("Enter a finite value and select valid units.")
        }
    }
}
