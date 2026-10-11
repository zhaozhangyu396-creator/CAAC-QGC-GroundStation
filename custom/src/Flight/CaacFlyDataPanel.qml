import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import QGroundControl
import QGroundControl.Controls

Rectangle {
    id: root
    implicitHeight: ScreenTools.defaultFontPixelHeight * 17
    color: caacPalette.windowShadeDark
    radius: ScreenTools.defaultFontPixelWidth
    clip: true

    property var vehicle: null
    property var distance: null
    property alias currentPage: pages.currentIndex
    signal titleDragged(point delta)

    readonly property real _margin: ScreenTools.defaultFontPixelWidth * 2
    property string _messages: ""

    onVehicleChanged: _messages = vehicle ? vehicle.formattedMessages : ""

    function formatMessages(messages) {
        const style = color => "color: " + color + "; font: " + ScreenTools.defaultFontPointSize.toFixed(0) + "pt monospace;"
        return messages.replace(/<#E>/g, style(caacPalette.warningText))
                       .replace(/<#I>/g, style(caacPalette.colorOrange))
                       .replace(/<#N>/g, style(caacPalette.text))
    }

    function factValue(fact, digits, unit) {
        if (!fact || !isFinite(Number(fact.rawValue))) {
            return qsTr("Unavailable")
        }
        return Number(fact.rawValue).toFixed(digits) + unit
    }

    QGCPalette { id: caacPalette; colorGroupEnabled: root.enabled }

    Rectangle {
        id: titleBar
        width: parent.width
        height: ScreenTools.defaultFontPixelHeight * 2.5
        color: caacPalette.windowShade
        CaacPanelDragHandle {
            objectName: "caacFlightDataDragHandle"
            anchors.fill: parent
            onMoved: (delta) => root.titleDragged(delta)
        }
        QGCLabel {
            anchors.centerIn: parent
            text: qsTr("Flight data")
            font.bold: true
            font.pointSize: ScreenTools.largeFontPointSize
        }
    }

    component DataGrid: GridLayout {
        property var entries: []
        columns: 2
        rowSpacing: root._margin
        columnSpacing: root._margin
        Repeater {
            model: entries
            ColumnLayout {
                required property var modelData
                Layout.fillWidth: true
                Layout.fillHeight: true
                QGCLabel {
                    objectName: modelData.key ? "caac" + modelData.key + "DistanceValue" : ""
                    Layout.alignment: Qt.AlignHCenter
                    text: modelData.value
                    font.bold: true
                    font.pointSize: ScreenTools.largeFontPointSize
                }
                QGCLabel {
                    Layout.fillWidth: true
                    text: modelData.label
                    horizontalAlignment: Text.AlignHCenter
                    wrapMode: Text.WordWrap
                }
            }
        }
    }

    SwipeView {
        id: pages
        anchors.top: titleBar.bottom
        anchors.bottom: pagination.top
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.margins: root._margin
        clip: true
        onCurrentIndexChanged: pagination.currentIndex = currentIndex

        Item {
            DataGrid {
                anchors.fill: parent
                entries: [
                    { label: qsTr("Flight time"), value: root.vehicle ? root.vehicle.getFact("flightTime").valueString : qsTr("Unavailable") },
                    { label: qsTr("Satellites"), value: root.vehicle && root.vehicle.gps.count.rawValue >= 0 && root.vehicle.gps.count.rawValue < 255 ? root.vehicle.gps.count.valueString : qsTr("Unavailable") },
                    { label: qsTr("AMSL altitude (m)"), value: root.factValue(root.vehicle ? root.vehicle.altitudeAMSL : null, 1, "") },
                    { label: qsTr("Airspeed (m/s)"), value: root.factValue(root.vehicle ? root.vehicle.airSpeed : null, 1, "") }
                ]
            }
        }
        ScrollView {
            id: logView
            contentWidth: availableWidth
            TextArea {
                objectName: "caacFlightLog"
                readOnly: true
                textFormat: TextEdit.RichText
                text: root._messages.length > 0 ? root.formatMessages(root._messages) : qsTr("No flight messages")
                wrapMode: TextEdit.Wrap
                color: caacPalette.text
                font.pointSize: ScreenTools.defaultFontPointSize
                background: null
            }
        }
        Item {
            objectName: "caacDirectionalDistancePage"
            DataGrid {
                anchors.fill: parent
                entries: [
                    { key: "North", label: qsTr("North distance (m)"), value: root.distance ? root.distance.north.toFixed(1) : qsTr("Unavailable") },
                    { key: "South", label: qsTr("South distance (m)"), value: root.distance ? root.distance.south.toFixed(1) : qsTr("Unavailable") },
                    { key: "East", label: qsTr("East distance (m)"), value: root.distance ? root.distance.east.toFixed(1) : qsTr("Unavailable") },
                    { key: "West", label: qsTr("West distance (m)"), value: root.distance ? root.distance.west.toFixed(1) : qsTr("Unavailable") }
                ]
            }
        }
    }

    PageIndicator {
        id: pagination
        objectName: "caacFlightDataPagination"
        anchors.bottom: parent.bottom
        anchors.bottomMargin: root._margin / 2
        anchors.horizontalCenter: parent.horizontalCenter
        count: pages.count
        interactive: true
        delegate: Rectangle {
            required property int index
            implicitWidth: ScreenTools.defaultFontPixelWidth
            implicitHeight: implicitWidth
            radius: width / 2
            color: caacPalette.text
            opacity: index === pagination.currentIndex ? 1 : 0.3
        }
        onCurrentIndexChanged: pages.currentIndex = currentIndex
    }

    Connections {
        target: root.vehicle
        function onNewFormattedMessage(message) { root._messages = message + root._messages }
        function onFormattedMessagesChanged() { root._messages = root.vehicle ? root.vehicle.formattedMessages : "" }
    }
}
