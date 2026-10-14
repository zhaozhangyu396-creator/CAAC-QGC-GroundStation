import QtQuick
import QtQuick.Layouts

import QGroundControl
import QGroundControl.Controls

Item {
    id:             control
    implicitWidth:  mainLayout.implicitWidth + (_toolsMargin * 2)
    implicitHeight: mainLayout.height + (_toolsMargin * 2)

    property real extraWidth: 0 ///< Extra width to add to the background rectangle
    readonly property real _toolsMargin: ScreenTools.defaultFontPixelWidth * 0.75
    QGCPalette { id: qgcPal }

    property alias factValueGrid:           factValueGrid
    property alias settingsGroup:           factValueGrid.settingsGroup
    property alias specificVehicleForCard:  factValueGrid.specificVehicleForCard

    Rectangle {
        id:         backgroundRect
        width:      control.width + extraWidth
        height:     control.height
        color:      qgcPal.window
        radius:     ScreenTools.defaultFontPixelWidth / 2
        opacity:    0.75
    }

    ColumnLayout {
        id:                 mainLayout
        width: Math.max(0, control.width - _toolsMargin * 2)
        anchors.margins:    _toolsMargin
        anchors.bottom:     parent.bottom
        anchors.left:       parent.left

        RowLayout {
            visible: factValueGrid.settingsUnlocked

            QGCColoredImage {
                source:             "qrc:/InstrumentValueIcons/lock-open.svg"
                mipmap:             true
                width:              ScreenTools.minTouchPixels * 0.75
                height:             width
                sourceSize.width:   width
                color:              qgcPal.text
                fillMode:           Image.PreserveAspectFit

                QGCMouseArea {
                    anchors.fill: parent
                    onClicked:    factValueGrid.settingsUnlocked = false
                }
            }
        }

        RowLayout {
            spacing: ScreenTools.defaultFontPixelWidth
            HorizontalFactValueGrid {
                id: factValueGrid
                Layout.fillWidth: true
                Layout.minimumWidth: 0
            }
            ColumnLayout {
                QGCLabel { text: qsTr("Heading"); font.pointSize: ScreenTools.smallFontPointSize }
                QGCLabel {
                    objectName: "caacDashboardHeadingValue"
                    readonly property var vehicle: QGroundControl.multiVehicleManager.activeVehicle
                    text: vehicle && isFinite(vehicle.heading.rawValue)
                        ? vehicle.heading.valueString + " " + vehicle.heading.units : qsTr("Unavailable")
                    font.pointSize: ScreenTools.menuFontPointSize
                }
            }
        }
    }

    QGCMouseArea {
        id:                         mouseArea
        x:                          mainLayout.x
        y:                          mainLayout.y
        width:                      mainLayout.width
        height:                     mainLayout.height
        acceptedButtons:            Qt.LeftButton | Qt.RightButton
        propagateComposedEvents:    true
        visible:                    !factValueGrid.settingsUnlocked

        onClicked: (mouse) => {
            if (!ScreenTools.isMobile && mouse.button === Qt.RightButton) {
                factValueGrid.settingsUnlocked = true
                mouse.accepted = true
            }
        }

        onPressAndHold: (mouse) => {
            factValueGrid.settingsUnlocked = true
            mouse.accepted = true
        }
    }
}
