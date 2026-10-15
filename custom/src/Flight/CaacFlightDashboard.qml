pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import QGroundControl
import QGroundControl.Controls

Item {
    id: root
    objectName: "caacFlightDashboard"
    required property var vehicle
    property bool compact: false
    property real availableWidth: parent.width
    property real topInset: 0
    property real bottomInset: 0
    property real sizeFactor: 1
    property point panelOffset: Qt.point(0, 0)
    readonly property real margin: ScreenTools.defaultFontPixelWidth
    readonly property var battery: vehicle && vehicle.batteries.count ? vehicle.batteries.get(0) : null
    readonly property real fontScale: Math.max(0.7, Math.min(1.4, width / (ScreenTools.defaultFontPixelWidth * 38)))
    readonly property real baseWidth: Math.min(ScreenTools.defaultFontPixelHeight * 18,
        availableWidth * (compact ? 0.3 : 0.48))
    width: Math.max(0, Math.min(parent.width - margin * 2, Math.max(ScreenTools.defaultFontPixelWidth * 18, baseWidth * sizeFactor)))
    height: instrument.height + values.height + margin
    x: Math.max(margin, Math.min((compact ? margin : availableWidth - width - margin)
        + panelOffset.x, parent.width - width - margin))
    y: Math.max(topInset + margin, Math.min(parent.height - bottomInset - height - margin + panelOffset.y,
        parent.height - height - margin))

    function formatted(fact) {
        return fact && isFinite(Number(fact.rawValue)) ? fact.valueString + " " + fact.units : "—"
    }
    CaacAttitudeInstrument {
        id: instrument
        anchors.horizontalCenter: parent.horizontalCenter
        width: Math.max(0, Math.min(root.width * 0.82, root.parent.height * (root.compact ? 0.2 : 0.35) * root.sizeFactor,
            root.parent.height - root.topInset - root.bottomInset - values.height - root.margin * 3))
        height: width
        vehicle: root.vehicle
    }
    Rectangle {
        id: values
        objectName: "caacDashboardValues"
        anchors.top: instrument.bottom
        anchors.topMargin: root.margin
        width: root.width
        height: grid.implicitHeight + root.margin
        color: "#d9222222"
        radius: ScreenTools.defaultBorderRadius
        GridLayout {
            id: grid
            x: root.margin * 0.5
            y: root.margin * 0.5
            width: parent.width - root.margin
            columns: 4
            rowSpacing: root.margin * 0.5
            columnSpacing: root.margin * 0.4
            Repeater {
                model: [
                    {key: "Altitude", label: qsTr("Altitude"), fact: root.vehicle ? root.vehicle.altitudeRelative : null},
                    {key: "Speed", label: qsTr("Speed"), fact: root.vehicle ? root.vehicle.groundSpeed : null},
                    {key: "GPS", label: "GPS", fact: null},
                    {key: "Pitch", label: qsTr("Pitch"), fact: root.vehicle ? root.vehicle.pitch : null},
                    {key: "Voltage", label: qsTr("Voltage"), fact: root.battery ? root.battery.voltage : null},
                    {key: "Heading", label: qsTr("Heading"), fact: root.vehicle ? root.vehicle.heading : null},
                    {key: "HomeDistance", label: qsTr("Home distance"), fact: root.vehicle ? root.vehicle.distanceToHome : null},
                    {key: "Roll", label: qsTr("Roll"), fact: root.vehicle ? root.vehicle.roll : null}
                ]
                delegate: ColumnLayout {
                    id: cell
                    required property var modelData
                    Layout.fillWidth: true
                    Layout.minimumWidth: 0
                    spacing: 0
                    QGCLabel {
                        Layout.fillWidth: true
                        text: cell.modelData.label
                        color: "white"
                        horizontalAlignment: Text.AlignHCenter
                        font.pointSize: ScreenTools.smallFontPointSize * root.fontScale
                    }
                    QGCLabel {
                        objectName: "caacDashboard" + cell.modelData.key + "Value"
                        Layout.fillWidth: true
                        color: "white"
                        horizontalAlignment: Text.AlignHCenter
                        font.pointSize: ScreenTools.smallFontPointSize * root.fontScale
                        text: cell.modelData.key === "GPS" ? (root.vehicle
                            ? root.vehicle.gps.lock.enumStringValue + " · " + root.vehicle.gps.count.valueString : "—")
                            : root.formatted(cell.modelData.fact)
                    }
                }
            }
        }
    }
    MouseArea {
        objectName: "caacDashboardDragArea"
        anchors.fill: parent
        acceptedButtons: Qt.LeftButton
        preventStealing: true
        cursorShape: pressed ? Qt.ClosedHandCursor : Qt.OpenHandCursor
        property point pressPosition
        property point pressOffset
        onPressed: (mouse) => {
            pressPosition = mapToItem(root.parent, mouse.x, mouse.y)
            pressOffset = root.panelOffset
        }
        onPositionChanged: (mouse) => {
            if (pressed) {
                const position = mapToItem(root.parent, mouse.x, mouse.y)
                root.panelOffset = Qt.point(pressOffset.x + position.x - pressPosition.x,
                    pressOffset.y + position.y - pressPosition.y)
            }
        }
        onWheel: (wheel) => {
            root.sizeFactor = Math.max(0.6, Math.min(2.5, root.sizeFactor * (wheel.angleDelta.y > 0 ? 1.1 : 1 / 1.1)))
            wheel.accepted = true
        }
    }
    QGCButton {
        objectName: "caacDashboardZoomButton"
        anchors.right: parent.right
        anchors.top: parent.top
        text: root.sizeFactor > 1 ? "↙" : "↗"
        heightFactor: 0.2
        leftPadding: root.margin * 0.4
        rightPadding: leftPadding
        ToolTip.visible: hovered
        ToolTip.text: qsTr("Enlarge or restore dashboard")
        onClicked: root.sizeFactor = root.sizeFactor > 1 ? 1 : 1.8
    }
}
