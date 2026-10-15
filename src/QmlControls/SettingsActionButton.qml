import QtQuick
import QtQuick.Layouts

import QGroundControl.Controls

QGCButton {
    Layout.fillWidth: false
    Layout.alignment: Qt.AlignHCenter
    implicitWidth: contentItem.implicitWidth + leftPadding + rightPadding
    implicitHeight: contentItem.implicitHeight + topPadding + bottomPadding
    _horizontalPadding: ScreenTools.defaultFontPixelWidth
    heightFactor: 0.3
    fontWeight: Font.Bold
}
