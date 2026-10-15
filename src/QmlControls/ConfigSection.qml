import QtQuick
import QtQuick.Layouts

import QGroundControl
import QGroundControl.Controls

ColumnLayout {
    id: control
    spacing: ScreenTools.defaultFontPixelHeight / 3

    default property alias contentItem: _controlsColumn.data

    property string heading
    property string iconSource
    readonly property bool vehicleSetupStyle: ScreenTools.isVehicleSetupItem(control)

    property real _margins: ScreenTools.defaultFontPixelHeight / 2

    QGCPalette { id: qgcPal; colorGroupEnabled: true }

    RowLayout {
        Layout.fillWidth: true
        visible: control.heading !== ""
        spacing: ScreenTools.defaultFontPixelWidth
        Rectangle {
            width: ScreenTools.defaultFontPixelWidth * 0.35
            height: ScreenTools.defaultFontPixelHeight
            color: qgcPal.settingsAccent
            radius: width / 2
            visible: control.vehicleSetupStyle
        }
        QGCLabel {
            Layout.fillWidth: true
            text: control.heading
            font.pointSize: control.vehicleSetupStyle ? ScreenTools.defaultFontPointSize : ScreenTools.largeFontPointSize
            wrapMode: Text.WordWrap
        }
    }

    Rectangle {
        implicitWidth: _contentRow.implicitWidth + _margins * 2
        implicitHeight: _contentRow.implicitHeight + _margins * 2
        Layout.fillWidth: true
        color: qgcPal.windowShade
        radius: ScreenTools.defaultFontPixelHeight / 4

        RowLayout {
            id: _contentRow
            y: _margins
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.leftMargin: _margins
            anchors.rightMargin: _margins
            spacing: ScreenTools.defaultFontPixelWidth * 2

            QGCColoredImage {
                Layout.preferredWidth: ScreenTools.defaultFontPixelHeight * 3
                Layout.preferredHeight: ScreenTools.defaultFontPixelHeight * 1.5
                source: control.iconSource
                color: qgcPal.text
                fillMode: Image.PreserveAspectFit
                visible: control.iconSource !== ""
            }

            ColumnLayout {
                id: _controlsColumn
                Layout.fillWidth: true
                spacing: ScreenTools.defaultFontPixelHeight / 2
            }
        }
    }
}
