import QtQuick
import QtQuick.Layouts

import QGroundControl
import QGroundControl.Controls

QGCPopupDialog {
    id: messageDialog

    property string text:           ""
    property var    acceptFunction: null        // Mainly used by MainRootWindow.showMessage to specify accept function in call
    property var    closeFunction:  null

    onAccepted: {
        if (acceptFunction) {
            acceptFunction()
        }
    }

    onClosed: {
        if (closeFunction) {
            closeFunction()
        }
    }

    ColumnLayout {
        QGCLabel {
            id:                     label
            Layout.preferredWidth:  Math.max(mainWindow.width / (ScreenTools.isMobile ? 2 : 3), headerMinWidth)
            text:                   OnlineMessageTranslator.displayText(messageDialog.text, OnlineMessageTranslator.revision)
            wrapMode:               Text.WordWrap
            linkColor:              color
            onLinkActivated:        (link) => Qt.openUrlExternally(link)
        }
    }
}
