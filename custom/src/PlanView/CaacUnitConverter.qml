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
    readonly property bool coordinates: quantity.currentIndex === 3
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
            model: [qsTr("Length"), qsTr("Speed"), qsTr("Angle"), qsTr("Latitude / longitude")]
        }
        ColumnLayout {
            Layout.fillWidth: true
            visible: root.coordinates
            spacing: ScreenTools.defaultFontPixelHeight * 0.5
            QGCLabel { text: qsTr("Longitude") }
            CaacCoordinateField {
                id: longitude
                Layout.fillWidth: true
                maximumDegrees: 180
                fieldName: "caacConvertLongitude"
                compact: true
                value: 0
            }
            QGCLabel { text: qsTr("Latitude") }
            CaacCoordinateField {
                id: latitude
                Layout.fillWidth: true
                maximumDegrees: 90
                fieldName: "caacConvertLatitude"
                compact: true
                value: 0
            }
            QGCLabel {
                objectName: "caacConvertCoordinateResult"
                Layout.fillWidth: true
                wrapMode: Text.WrapAnywhere
                font.bold: true
                readonly property bool valid: isFinite(longitude.coordinate) && Math.abs(longitude.coordinate) <= 180
                    && isFinite(latitude.coordinate) && Math.abs(latitude.coordinate) <= 90
                function formatted(value, limit) {
                    const dms = QGroundControl.corePlugin.coordinateToDms(value, limit)
                    if (dms.degrees === undefined) { return "" }
                    const direction = limit === 90 ? (dms.negative ? qsTr("South") : qsTr("North"))
                        : (dms.negative ? qsTr("West") : qsTr("East"))
                    return dms.degrees + "° " + dms.minutes + "′ " + Number(dms.seconds.toFixed(6)) + "″ " + direction
                }
                text: valid ? qsTr("Decimal degrees") + "\n" + qsTr("Longitude") + ": " + longitude.coordinate.toFixed(9)
                    + "°\n" + qsTr("Latitude") + ": " + latitude.coordinate.toFixed(9) + "°\n\n"
                    + qsTr("DMS") + "\n" + qsTr("Longitude") + ": " + formatted(longitude.coordinate, 180)
                    + "\n" + qsTr("Latitude") + ": " + formatted(latitude.coordinate, 90)
                    : qsTr("Enter a valid longitude (−180° to 180°) and latitude (−90° to 90°).")
            }
        }
        QGCLabel { text: qsTr("Input value"); visible: !root.coordinates }
        QGCTextField {
            id: value
            visible: !root.coordinates
            objectName: "caacConvertValue"
            Layout.fillWidth: true
            text: "100"
            validator: DoubleValidator { locale: "C" }
        }
        QGCLabel { text: qsTr("From unit"); visible: !root.coordinates }
        QGCComboBox {
            id: from
            visible: !root.coordinates
            objectName: "caacConvertFrom"
            Layout.fillWidth: true
            model: root._units
            textRole: "name"
            onModelChanged: currentIndex = 0
        }
        QGCLabel { text: qsTr("To unit"); visible: !root.coordinates }
        QGCComboBox {
            id: to
            visible: !root.coordinates
            objectName: "caacConvertTo"
            Layout.fillWidth: true
            model: root._units
            textRole: "name"
            onModelChanged: currentIndex = 1
        }
        QGCLabel {
            objectName: "caacConvertResult"
            visible: !root.coordinates
            Layout.fillWidth: true
            font.bold: true
            font.pointSize: ScreenTools.largeFontPointSize
            wrapMode: Text.WrapAnywhere
            text: isFinite(root.result) ? Number(root.result.toPrecision(12)).toString() + " " + root._units[to.currentIndex].name
                : qsTr("Enter a finite value and select valid units.")
        }
    }
}
