import QtQuick
import QtQuick.Controls
import QGroundControl.Controls

MenuItem {
    font.family: ScreenTools.normalFontFamily
    font.pointSize: ScreenTools.menuFontPointSize
    // MenuItem doesn't support !visible so we have to hack it in
    height: visible ? implicitHeight : 0
}
