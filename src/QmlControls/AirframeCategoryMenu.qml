pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Layouts

import QGroundControl
import QGroundControl.Controls

import "AirframeCatalog.js" as AirframeCatalog

ColumnLayout {
    id: root
    objectName: "airframeCategoryMenu"
    spacing: ScreenTools.defaultFontPixelHeight * 0.5

    property string category: ""
    signal categorySelected(string category)

    QGCPalette { id: palette }

    QGCButton {
        objectName: "airframeCategory_back"
        Layout.fillWidth: true
        visible: root.category !== ""
        text: qsTr("返回机架类型")
        onClicked: root.categorySelected("")
    }

    QGCLabel {
        objectName: "airframeCategoryTitle"
        Layout.fillWidth: true
        text: root.category === "" ? qsTr("机架类型") : AirframeCatalog.categoryName(root.category)
        font.pointSize: ScreenTools.mediumFontPointSize
        font.bold: true
        wrapMode: Text.WordWrap
    }

    Repeater {
        model: root.category === "" ? AirframeCatalog.categories() : []

        QGCButton {
            id: categoryButton
            required property var modelData
            objectName: "airframeCategory_" + modelData.key
            Layout.fillWidth: true
            Layout.minimumWidth: 0
            leftPadding: ScreenTools.defaultFontPixelWidth
            rightPadding: leftPadding
            topPadding: ScreenTools.defaultFontPixelHeight * 0.4
            bottomPadding: topPadding
            backgroundColor: Qt.lighter(palette.settingsAccent, 1.3)
            showBorder: true
            Accessible.name: modelData.name

            contentItem: RowLayout {
                spacing: ScreenTools.defaultFontPixelWidth
                Image {
                    source: categoryButton.modelData.image
                    Layout.preferredWidth: ScreenTools.defaultFontPixelHeight * 2.5
                    Layout.preferredHeight: Layout.preferredWidth
                    fillMode: Image.PreserveAspectFit
                    sourceSize.width: width
                    sourceSize.height: height
                }
                QGCLabel {
                    Layout.fillWidth: true
                    text: categoryButton.modelData.name
                    wrapMode: Text.WordWrap
                }
                QGCLabel { text: "›"; font.pointSize: ScreenTools.mediumFontPointSize }
            }
            onClicked: root.categorySelected(modelData.key)
        }
    }
}
