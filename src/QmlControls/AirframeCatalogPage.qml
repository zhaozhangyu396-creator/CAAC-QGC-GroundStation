pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Layouts

import QGroundControl
import QGroundControl.Controls

import "AirframeCatalog.js" as AirframeCatalog

Item {
    id: root
    objectName: "airframeCatalogPage"

    property string pageName: qsTr("机架")
    property string category: ""
    property string selectedModel: ""
    property bool connected: false

    QGCPalette { id: palette }

    QGCFlickable {
        objectName: "airframeCatalogScroller"
        anchors.fill: parent
        contentWidth: width
        contentHeight: content.implicitHeight
        flickableDirection: Flickable.VerticalFlick
        clip: true

        ColumnLayout {
            id: content
            width: parent.width
            spacing: ScreenTools.defaultFontPixelHeight * 0.5

            AirframeCategoryMenu {
                Layout.fillWidth: true
                category: root.category
                onCategorySelected: (category) => {
                    root.category = category
                    root.selectedModel = ""
                }
            }

            QGCLabel {
                Layout.fillWidth: true
                visible: root.category !== ""
                text: root.connected ? qsTr("当前飞控未提供此类型的机架配置。") : qsTr("连接飞控后可选择并应用机架。")
                wrapMode: Text.WordWrap
            }

            Repeater {
                model: root.category === "" ? [] : AirframeCatalog.models(root.category)

                Rectangle {
                    id: modelCard
                    required property var modelData
                    objectName: "airframeCatalogModel_" + modelData.key
                    Layout.fillWidth: true
                    implicitHeight: modelContent.implicitHeight + ScreenTools.defaultFontPixelWidth * 2
                    color: Qt.lighter(palette.settingsAccent, 1.3)
                    border.color: root.selectedModel === modelData.key ? palette.text : palette.groupBorder
                    border.width: 1
                    radius: ScreenTools.defaultBorderRadius

                    ColumnLayout {
                        id: modelContent
                        anchors.left: parent.left
                        anchors.right: parent.right
                        anchors.top: parent.top
                        anchors.margins: ScreenTools.defaultFontPixelWidth
                        QGCLabel {
                            Layout.fillWidth: true
                            text: modelCard.modelData.name
                            font.bold: true
                            wrapMode: Text.WordWrap
                        }
                        Image {
                            objectName: "airframeCatalogImage_" + modelCard.modelData.key
                            Layout.fillWidth: true
                            Layout.preferredHeight: ScreenTools.defaultFontPixelHeight * 10
                            fillMode: Image.PreserveAspectFit
                            source: modelCard.modelData.image
                            sourceSize.width: width
                            sourceSize.height: height
                        }
                    }
                    MouseArea {
                        anchors.fill: parent
                        onClicked: root.selectedModel = modelCard.modelData.key
                    }
                }
            }
        }
    }
}
