pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtPositioning

import QGroundControl
import QGroundControl.Controls

ColumnLayout {
    id: root
    required property var routeController
    required property var editor
    readonly property var _points: routeController.coordinateItems
    spacing: ScreenTools.defaultFontPixelHeight * 0.5
    QGCPalette { id: palette; colorGroupEnabled: root.enabled }

    function number(text) { return text.trim().length ? Number(text) : NaN }
    function point(combo) {
        return combo.currentIndex >= 0 && combo.currentIndex < _points.length ? _points[combo.currentIndex] : null
    }
    component PointSelector: QGCComboBox {
        Layout.fillWidth: true
        Layout.minimumWidth: 0
        model: root._points
        textRole: "label"
        currentIndex: root._points.length > 1 ? 1 : 0
    }
    component NumberField: QGCTextField {
        Layout.fillWidth: true
        Layout.minimumWidth: 0
        validator: DoubleValidator { locale: "C" }
    }
    component Card: Rectangle {
        id: card
        required property string title
        default property alias fields: fieldsLayout.data
        Layout.fillWidth: true
        implicitHeight: content.implicitHeight + ScreenTools.defaultFontPixelHeight
        radius: ScreenTools.defaultFontPixelHeight * 0.5
        color: Qt.alpha(palette.windowShadeDark, 0.85)
        border.color: palette.text
        border.width: ScreenTools.defaultFontPixelWidth * 0.1
        data: [ColumnLayout {
            id: content
            x: ScreenTools.defaultFontPixelWidth
            y: ScreenTools.defaultFontPixelHeight * 0.5
            width: card.width - x * 2
            RowLayout {
                Rectangle {
                    width: ScreenTools.defaultFontPixelWidth * 0.3
                    height: heading.implicitHeight
                    radius: width / 2
                    color: palette.colorGreen
                }
                QGCLabel {
                    id: heading
                    Layout.fillWidth: true
                    text: card.title
                    font.bold: true
                    wrapMode: Text.WordWrap
                }
            }
            ColumnLayout { id: fieldsLayout; Layout.fillWidth: true }
        }]
    }

    Card {
        title: qsTr("Drag entire route")
        Switch {
            objectName: "caacRouteDragToggle"
            checked: root.editor.routeDragMode
            onClicked: root.editor.routeDragMode = checked
        }
    }
    Card {
        title: qsTr("Route rotation")
        RowLayout {
            Layout.fillWidth: true
            PointSelector { id: from; objectName: "caacAlignFrom" }
            QGCLabel { text: "→" }
            PointSelector {
                id: to
                objectName: "caacAlignTo"
                currentIndex: root._points.length > 2 ? 2 : 0
            }
        }
        QGCLabel { text: qsTr("Bearing (degrees)") }
        NumberField { id: heading; objectName: "caacAlignHeading"; text: "90" }
        QGCCheckBox { id: alignHome; objectName: "caacAlignIncludeHome"; text: qsTr("Include H point") }
        QGCButton {
            objectName: "caacAlignApply"
            Layout.fillWidth: true
            text: qsTr("Rotate")
            primary: true
            onClicked: {
                const a = root.point(from)
                const b = root.point(to)
                if (a && b) {
                    root.routeController.alignRouteAround(a.coordinate, a.index, b.index,
                        root.number(heading.text), alignHome.checked)
                }
            }
        }
    }
    Card {
        title: qsTr("Route translation")
        QGCLabel { text: qsTr("Reference point") }
        PointSelector { id: reference; objectName: "caacTranslateReference" }
        GridLayout {
            Layout.fillWidth: true
            columns: root.width < ScreenTools.defaultFontPixelWidth * 52 ? 1 : 2
            QGCLabel { text: qsTr("Longitude (°)") }
            CaacCoordinateField {
                id: longitude
                Layout.fillWidth: true
                compact: true
                maximumDegrees: 180
                fieldName: "caacTranslateLongitude"
                value: root.point(reference) ? root.point(reference).coordinate.longitude : NaN
            }
            QGCLabel { text: qsTr("Latitude (°)") }
            CaacCoordinateField {
                id: latitude
                Layout.fillWidth: true
                compact: true
                maximumDegrees: 90
                fieldName: "caacTranslateLatitude"
                value: root.point(reference) ? root.point(reference).coordinate.latitude : NaN
            }
        }
        QGCCheckBox {
            id: translateHome
            objectName: "caacTranslateIncludeHome"
            text: qsTr("Include H point")
            checked: true
        }
        QGCButton {
            objectName: "caacTranslateApply"
            Layout.fillWidth: true
            text: qsTr("Translate route")
            primary: true
            onClicked: {
                const selected = root.point(reference)
                if (selected) {
                    root.routeController.translateRoute(selected.coordinate,
                        QtPositioning.coordinate(latitude.coordinate, longitude.coordinate), translateHome.checked)
                }
            }
        }
    }
    GridLayout {
        Layout.fillWidth: true
        columns: root.width < ScreenTools.defaultFontPixelWidth * 52 ? 1 : 2
        Card {
            title: qsTr("Directional translation")
            QGCLabel { text: qsTr("Distance (m)") }
            NumberField { id: distance; objectName: "caacTranslateDistance"; text: "10" }
            GridLayout {
                Layout.alignment: Qt.AlignHCenter
                columns: 3
                Repeater {
                    model: [{text: "↖", bearing: 315}, {text: "↑", bearing: 0}, {text: "↗", bearing: 45},
                        {text: "←", bearing: 270}, {text: "", bearing: -1}, {text: "→", bearing: 90},
                        {text: "↙", bearing: 225}, {text: "↓", bearing: 180}, {text: "↘", bearing: 135}]
                    delegate: QGCButton {
                        required property var modelData
                        objectName: "caacMoveRouteDirection_" + modelData.bearing
                        text: modelData.text
                        Layout.preferredWidth: ScreenTools.defaultFontPixelHeight * 1.6
                        Layout.preferredHeight: Layout.preferredWidth
                        Layout.minimumWidth: Layout.preferredWidth
                        Layout.maximumWidth: Layout.preferredWidth
                        Layout.minimumHeight: Layout.preferredHeight
                        Layout.maximumHeight: Layout.preferredHeight
                        leftPadding: 0
                        rightPadding: 0
                        topPadding: 0
                        bottomPadding: 0
                        opacity: modelData.bearing < 0 ? 0 : 1
                        enabled: modelData.bearing >= 0
                        onClicked: root.routeController.moveRouteBy(modelData.bearing, root.number(distance.text), translateHome.checked)
                    }
                }
            }
        }
        Card {
            title: qsTr("Rotation around a waypoint")
            QGCLabel { text: qsTr("Centre") }
            PointSelector { id: center; objectName: "caacRotateCenter" }
            QGCLabel { text: qsTr("Angle (degrees)") }
            NumberField { id: angle; objectName: "caacRotateAngle"; text: "0" }
            QGCCheckBox { id: rotateHome; objectName: "caacRotateIncludeHome"; text: qsTr("Include H point") }
            QGCButton {
                objectName: "caacRotateApply"
                Layout.fillWidth: true
                text: qsTr("Rotate")
                primary: true
                onClicked: {
                    const selected = root.point(center)
                    if (selected) {
                        root.routeController.rotateRouteAround(selected.coordinate, root.number(angle.text), rotateHome.checked)
                    }
                }
            }
        }
    }
}
