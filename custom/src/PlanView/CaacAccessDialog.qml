pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import QGroundControl
import QGroundControl.Controls

QGCPopupDialog {
    id: root
    objectName: "caacAccessDialog"
    property bool changePassword: false
    readonly property var access: QGroundControl.corePlugin.accessController
    readonly property bool settingPassword: changePassword || !access.configured
    property string errorText: ""
    signal accessGranted()
    title: settingPassword ? qsTr("Set access password") : qsTr("Enter access password")
    buttons: Dialog.Ok | Dialog.Cancel
    bypassNavigationCheck: true
    acceptButtonEnabled: password.text.length > 0 && (!settingPassword || confirmation.text.length > 0)
    onAccepted: {
        if (settingPassword) {
            if (password.text !== confirmation.text) { errorText = qsTr("The passwords do not match."); preventClose = true; return }
            preventClose = !access.setPassword(current.text, password.text)
        } else { preventClose = !access.verify(password.text) }
        if (preventClose) { errorText = qsTr("Incorrect password or unable to save the password."); return }
        password.text = ""
        confirmation.text = ""
        current.text = ""
        if (!changePassword) { accessGranted() }
    }
    ColumnLayout {
        width: Math.min(root.maxContentAvailableWidth, ScreenTools.defaultFontPixelWidth * 42)
        QGCLabel { text: qsTr("Vehicle setup and analysis require this password each time they are opened."); Layout.fillWidth: true; wrapMode: Text.WordWrap }
        QGCLabel { text: qsTr("Current password"); visible: root.changePassword && root.access.configured }
        QGCTextField { id: current; objectName: "caacAccessCurrentPassword"; Layout.fillWidth: true; echoMode: TextInput.Password; visible: root.changePassword && root.access.configured }
        QGCLabel { text: root.settingPassword ? qsTr("New password") : qsTr("Access password") }
        QGCTextField { id: password; objectName: "caacAccessPassword"; Layout.fillWidth: true; echoMode: TextInput.Password; maximumLength: 128 }
        QGCLabel { text: qsTr("Confirm password"); visible: root.settingPassword }
        QGCTextField { id: confirmation; objectName: "caacAccessConfirmation"; Layout.fillWidth: true; echoMode: TextInput.Password; maximumLength: 128; visible: root.settingPassword }
        QGCLabel { text: root.errorText; Layout.fillWidth: true; wrapMode: Text.WordWrap; visible: root.errorText.length > 0 }
    }
}
