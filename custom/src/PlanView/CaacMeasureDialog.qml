pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtPositioning

import QGroundControl
import QGroundControl.Controls

QGCPopupDialog {
    id: root
    objectName: "caacMeasureDialog"
    title: qsTr("A/B measurement")
    buttons: Dialog.Close
    required property var editor
    property string _error: ""
    readonly property var _metrics: QGroundControl.corePlugin.routeLegMetrics(editor.measureA, editor.measureB)

    function applyCoordinates() {
        const a = QtPositioning.coordinate(aLatitude.coordinate, aLongitude.coordinate)
        const b = QtPositioning.coordinate(bLatitude.coordinate, bLongitude.coordinate)
        if (!a.isValid || !b.isValid) {
            _error = qsTr("Enter valid coordinates for both points.")
            return
        }
        _error = ""
        editor.measureA = a
        editor.measureB = b
    }

    ColumnLayout {
        width: Math.min(root.maxContentAvailableWidth, ScreenTools.defaultFontPixelWidth * 80)
        spacing: ScreenTools.defaultFontPixelHeight * 0.5
        GridLayout {
            columns: root.maxContentAvailableWidth < ScreenTools.defaultFontPixelWidth * 68 ? 1 : 2
            Layout.fillWidth: true
            QGCLabel { text: qsTr("A latitude") }
            CaacCoordinateField {
                id: aLatitude
                Layout.fillWidth: true
                fieldName: "caacMeasureALatitude"
                maximumDegrees: 90
                textContextMenuEnabled: false
                value: root.editor.measureA.latitude
            }
            QGCLabel { text: qsTr("A longitude") }
            CaacCoordinateField {
                id: aLongitude
                Layout.fillWidth: true
                fieldName: "caacMeasureALongitude"
                maximumDegrees: 180
                textContextMenuEnabled: false
                value: root.editor.measureA.longitude
            }
            QGCLabel { text: qsTr("B latitude") }
            CaacCoordinateField {
                id: bLatitude
                Layout.fillWidth: true
                fieldName: "caacMeasureBLatitude"
                maximumDegrees: 90
                textContextMenuEnabled: false
                value: root.editor.measureB.latitude
            }
            QGCLabel { text: qsTr("B longitude") }
            CaacCoordinateField {
                id: bLongitude
                Layout.fillWidth: true
                fieldName: "caacMeasureBLongitude"
                maximumDegrees: 180
                textContextMenuEnabled: false
                value: root.editor.measureB.longitude
            }
        }
        RowLayout {
            Layout.fillWidth: true
            QGCButton {
                objectName: "caacMeasurePickA"
                Layout.fillWidth: true
                text: qsTr("Pick A on map")
                enabled: root.editor.editorMap && root.editor.editorMap.directMissionItemEditing
                onClicked: { root.editor.pickMeasurement(1); root.close() }
            }
            QGCButton {
                objectName: "caacMeasurePickB"
                Layout.fillWidth: true
                text: qsTr("Pick B on map")
                enabled: root.editor.editorMap && root.editor.editorMap.directMissionItemEditing
                onClicked: { root.editor.pickMeasurement(2); root.close() }
            }
        }
        QGCButton {
            objectName: "caacMeasureApply"
            Layout.fillWidth: true
            text: qsTr("Measure entered coordinates")
            onClicked: root.applyCoordinates()
        }
        QGCLabel {
            objectName: "caacMeasureResult"
            Layout.fillWidth: true
            text: root.editor.measureA.isValid && root.editor.measureB.isValid
                ? qsTr("Distance: %1 m · Bearing A → B: %2°")
                    .arg(root._metrics.distance !== undefined ? root._metrics.distance.toFixed(2) : "0.00")
                    .arg(root._metrics.bearing !== undefined ? root._metrics.bearing.toFixed(2) : "—")
                : qsTr("Select or enter points A and B.")
            wrapMode: Text.WordWrap
            font.bold: true
        }
        QGCLabel {
            Layout.fillWidth: true
            visible: root._error.length > 0
            text: root._error
            wrapMode: Text.WordWrap
        }
        QGCLabel {
            Layout.fillWidth: true
            text: qsTr("WGS84 horizontal distance and true-north bearing. Measurement does not change the mission.")
            font.pointSize: ScreenTools.smallFontPointSize
            wrapMode: Text.WordWrap
        }
        QGCButton {
            objectName: "caacMeasureClear"
            Layout.fillWidth: true
            text: qsTr("Clear measurement")
            onClicked: { root._error = ""; root.editor.clearMeasurement(); root.close() }
        }
    }
}
