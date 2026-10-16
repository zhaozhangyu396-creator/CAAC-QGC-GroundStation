pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Window

import QGroundControl
import QGroundControl.Controls

ApplicationWindow {
    id: root
    objectName: "caacActivationWindow"
    required property var activation
    visible: true
    title: qsTr("Ground station activation")
    width: Math.min(760, Screen.width * 0.9)
    height: Math.min(650, Screen.height * 0.9)
    minimumWidth: 480
    minimumHeight: 480
    color: "#1d242a"

    QGCPalette { id: palette }

    footer: Rectangle {
        implicitHeight: footerContent.implicitHeight + ScreenTools.defaultFontPixelHeight * 1.5
        color: root.color
        ColumnLayout {
            id: footerContent
            width: parent.width - ScreenTools.defaultFontPixelHeight * 3
            anchors.centerIn: parent
            spacing: ScreenTools.defaultFontPixelHeight * 0.6
            QGCLabel {
                Layout.fillWidth: true
                text: qsTr("Permanent license · Offline activation · Bound to this computer")
                color: "#c9ced3"
                font.pointSize: ScreenTools.smallFontPointSize
                wrapMode: Text.WordWrap
            }
            QGCLabel {
                objectName: "caacActivationError"
                Layout.fillWidth: true
                text: root.activation.errorMessage
                visible: text.length > 0
                color: "#ffb4a9"
                wrapMode: Text.WordWrap
            }
            RowLayout {
                Layout.alignment: Qt.AlignHCenter
                spacing: ScreenTools.defaultFontPixelWidth * 2
                QGCButton {
                    objectName: "caacActivationExit"
                    text: qsTr("Exit")
                    font.bold: true
                    onClicked: Qt.quit()
                }
                QGCButton {
                    objectName: "caacActivationSubmit"
                    text: qsTr("Activate and start")
                    primary: true
                    font.bold: true
                    enabled: codeInput.text.trim().length > 0 && !root.activation.activated
                    onClicked: root.activation.activate(codeInput.text)
                }
            }
        }
    }

    ScrollView {
        anchors.fill: parent
        anchors.margins: ScreenTools.defaultFontPixelHeight * 1.5
        contentWidth: availableWidth
        clip: true

        ColumnLayout {
            width: parent.width
            spacing: ScreenTools.defaultFontPixelHeight

            RowLayout {
                Layout.fillWidth: true
                Rectangle {
                    Layout.preferredWidth: 4
                    Layout.preferredHeight: ScreenTools.defaultFontPixelHeight * 1.5
                    color: palette.settingsSectionAccent
                }
                QGCLabel {
                    text: qsTr("Software activation")
                    font.bold: true
                    font.pointSize: ScreenTools.largeFontPointSize
                    color: "white"
                }
            }
            QGCLabel {
                Layout.fillWidth: true
                text: qsTr("Activate this computer to use the ground station.")
                color: "#c9ced3"
                wrapMode: Text.WordWrap
            }

            Rectangle {
                Layout.fillWidth: true
                implicitHeight: machineColumn.implicitHeight + ScreenTools.defaultFontPixelHeight * 1.5
                radius: 6
                color: "#272e35"
                border.color: "#59616a"
                ColumnLayout {
                    id: machineColumn
                    anchors.fill: parent
                    anchors.margins: ScreenTools.defaultFontPixelHeight * 0.75
                    spacing: ScreenTools.defaultFontPixelHeight * 0.6
                    QGCLabel { text: qsTr("Machine code"); color: "white"; font.bold: true }
                    QGCTextField {
                        objectName: "caacActivationMachineCode"
                        Layout.fillWidth: true
                        readOnly: true
                        text: root.activation.machineCode
                        selectByMouse: true
                        horizontalAlignment: TextInput.AlignHCenter
                        font.pointSize: ScreenTools.smallFontPointSize
                    }
                    QGCButton {
                        objectName: "caacActivationCopyMachineCode"
                        Layout.alignment: Qt.AlignHCenter
                        text: qsTr("Copy machine code")
                        font.bold: true
                        enabled: root.activation.machineCode.length > 0
                        onClicked: root.activation.copyMachineCode()
                    }
                }
            }

            QGCLabel {
                Layout.fillWidth: true
                text: qsTr("Send the machine code to your software provider to receive an activation code.")
                wrapMode: Text.WordWrap
                color: "#c9ced3"
            }
            QGCLabel { text: qsTr("Activation code"); color: "white"; font.bold: true }
            TextArea {
                id: codeInput
                objectName: "caacActivationCodeInput"
                Layout.fillWidth: true
                Layout.preferredHeight: ScreenTools.defaultFontPixelHeight * 4
                placeholderText: qsTr("Paste the complete activation code")
                wrapMode: TextEdit.WrapAnywhere
                selectByMouse: true
                color: "white"
                placeholderTextColor: "#aab1b8"
                font.pointSize: ScreenTools.defaultFontPointSize
                padding: ScreenTools.defaultFontPixelHeight * 0.6
                background: Rectangle { color: "#272e35"; border.color: "#8cb3be"; radius: 6 }
            }
        }
    }
}
