pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Layouts

import QGroundControl
import QGroundControl.Controls

Item {
    id: root
    required property var routeController
    readonly property var _points: routeController.coordinateItems
    readonly property int _reference: reference.currentIndex >= 0 && reference.currentIndex < _points.length
        ? _points[reference.currentIndex].index : -1
    readonly property var _options: ({
        size: number(size.text), width: number(bowWidth.text), count: number(count.text),
        clockwise: direction.currentIndex === 0, vertexTop: hexType.currentIndex === 1,
        start: tabs.currentIndex === 1 ? hexStart.currentIndex : bowStart.currentIndex,
        bearing: number(bearing.text), close: closeRoute.checked
    })
    readonly property var _preview: routeController.templateCoordinates(tabs.currentIndex, _reference, _options)
    property bool _restoringBearing: false
    function resetBearing() {
        _restoringBearing = true
        bearing.text = Qt.binding(() => root.routeController.routeOrientation.toString())
        _restoringBearing = false
    }
    function cancelOrientation() {
        routeController.cancelRouteOrientation()
        resetBearing()
    }
    onVisibleChanged: { if (!visible) { cancelOrientation() } }
    Component.onDestruction: { if (routeController) { routeController.cancelRouteOrientation() } }
    function number(value) { return value.trim().length ? Number(value) : NaN }

    component NumberField: QGCTextField {
        Layout.fillWidth: true
        Layout.minimumWidth: 0
        validator: DoubleValidator { locale: "C" }
    }

    QGCFlickable {
        objectName: "caacTemplateScroller"
        anchors.top: parent.top
        anchors.bottom: add.top
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottomMargin: ScreenTools.defaultFontPixelHeight
        clip: true
        contentWidth: width
        contentHeight: fields.implicitHeight
        ColumnLayout {
            id: fields
            width: parent.width
            spacing: ScreenTools.defaultFontPixelHeight
            QGCTabBar {
                id: tabs
                objectName: "caacTemplateTabs"
                Layout.fillWidth: true
                QGCTabButton { text: qsTr("Circle") }
                QGCTabButton { text: qsTr("Hexagon") }
                QGCTabButton { text: qsTr("Bow") }
                QGCTabButton { text: qsTr("Star") }
            }
            GridLayout {
                columns: root.width < ScreenTools.defaultFontPixelWidth * 35 ? 1 : 2
                Layout.fillWidth: true
                rowSpacing: ScreenTools.defaultFontPixelHeight * 0.5
                QGCLabel { text: qsTr("Reference point") }
                QGCComboBox {
                    id: reference
                    objectName: "caacTemplateReference"
                    Layout.fillWidth: true
                    Layout.minimumWidth: 0
                    model: root._points
                    textRole: "label"
                    onActivated: root.cancelOrientation()
                }
                QGCLabel { text: tabs.currentIndex === 0 ? qsTr("Diameter (m)") : tabs.currentIndex === 2 ? qsTr("Length (m)") : qsTr("Side length (m)") }
                NumberField { id: size; objectName: "caacTemplateSize"; text: "100" }
                QGCLabel { text: qsTr("Width (m)"); visible: tabs.currentIndex === 2 }
                NumberField { id: bowWidth; objectName: "caacTemplateWidth"; text: "50"; visible: tabs.currentIndex === 2 }
                QGCLabel { text: qsTr("Waypoint count"); visible: tabs.currentIndex === 0 || tabs.currentIndex === 2 }
                NumberField {
                    id: count
                    objectName: "caacTemplateCount"
                    text: tabs.currentIndex === 2 ? "8" : "10"
                    visible: tabs.currentIndex === 0 || tabs.currentIndex === 2
                    validator: IntValidator { bottom: tabs.currentIndex === 2 ? 4 : 3; top: 1000 }
                }
                QGCLabel { text: qsTr("Direction"); visible: tabs.currentIndex !== 2 }
                QGCComboBox {
                    id: direction
                    Layout.fillWidth: true
                    Layout.minimumWidth: 0
                    model: [qsTr("Clockwise"), qsTr("Counterclockwise")]
                    visible: tabs.currentIndex !== 2
                }
                QGCLabel { text: qsTr("Type"); visible: tabs.currentIndex === 1 }
                QGCComboBox {
                    id: hexType
                    objectName: "caacTemplateHexType"
                    Layout.fillWidth: true
                    Layout.minimumWidth: 0
                    model: [qsTr("Flat side on top"), qsTr("Vertex on top")]
                    visible: tabs.currentIndex === 1
                }
                QGCLabel { text: qsTr("Start vertex"); visible: tabs.currentIndex === 1 }
                QGCComboBox {
                    id: hexStart
                    objectName: "caacTemplateHexStart"
                    Layout.fillWidth: true
                    Layout.minimumWidth: 0
                    model: [qsTr("Point A"), qsTr("Point B"), qsTr("Point C"), qsTr("Point D"), qsTr("Point E"), qsTr("Point F")]
                    visible: tabs.currentIndex === 1
                }
                QGCLabel { text: qsTr("Start corner"); visible: tabs.currentIndex === 2 }
                QGCComboBox {
                    id: bowStart
                    objectName: "caacTemplateBowStart"
                    Layout.fillWidth: true
                    Layout.minimumWidth: 0
                    model: [qsTr("Upper left"), qsTr("Upper right"), qsTr("Lower left"), qsTr("Lower right")]
                    visible: tabs.currentIndex === 2
                }
                QGCLabel { text: qsTr("Orientation (°)") }
                NumberField {
                    id: bearing
                    objectName: "caacTemplateBearing"
                    text: root.routeController.routeOrientation.toString()
                    onTextEdited: {
                        const angle = root.number(text)
                        if (!root._restoringBearing && isFinite(angle) && root._points.length > 1
                                && reference.currentIndex >= 0 && reference.currentIndex < root._points.length) {
                            root.routeController.previewRouteOrientation(angle, root._points[reference.currentIndex].coordinate)
                        }
                    }
                }
            }
            RowLayout {
                Layout.fillWidth: true
                visible: root.routeController.rotationPreviewActive
                QGCButton {
                    objectName: "caacConfirmOrientation"
                    Layout.fillWidth: true
                    text: qsTr("Confirm orientation")
                    primary: true
                    enabled: isFinite(root.number(bearing.text))
                    onClicked: { if (root.routeController.confirmRouteOrientation()) { root.resetBearing() } }
                }
                QGCButton {
                    objectName: "caacCancelOrientation"
                    Layout.fillWidth: true
                    text: qsTr("Cancel rotation")
                    onClicked: root.cancelOrientation()
                }
            }
            QGCLabel {
                Layout.fillWidth: true
                wrapMode: Text.WordWrap
                text: qsTr("Editing orientation rotates all loaded route points about the reference point. Confirm to save one undoable change; cancel restores their original positions.")
            }
            QGCCheckBox { id: closeRoute; text: qsTr("Close route with a final waypoint") }
            QGCLabel {
                Layout.fillWidth: true
                text: qsTr("Circle: centre anchor. Other templates: existing first vertex, without a duplicate. Insert after the anchor. Orientation 0°: north up.")
                wrapMode: Text.WordWrap
                font.pointSize: ScreenTools.smallFontPointSize
            }
            Canvas {
                id: preview
                Layout.fillWidth: true
                Layout.preferredHeight: ScreenTools.defaultFontPixelHeight * 8
                property var vertices: root._preview
                property int shape: tabs.currentIndex
                onVerticesChanged: requestPaint()
                onShapeChanged: requestPaint()
                onWidthChanged: requestPaint()
                onHeightChanged: requestPaint()
                onPaint: {
                    const ctx = getContext("2d")
                    ctx.reset()
                    if (!vertices.length || reference.currentIndex < 0 || reference.currentIndex >= root._points.length) {
                        return
                    }
                    const anchor = root._points[reference.currentIndex].coordinate
                    const coordinates = shape === 0 ? vertices : [anchor].concat(vertices)
                    const points = coordinates.map(coordinate => {
                        const leg = QGroundControl.corePlugin.routeLegMetrics(anchor, coordinate)
                        const angle = leg.bearing === undefined ? 0 : leg.bearing * Math.PI / 180
                        const distance = leg.distance === undefined ? 0 : leg.distance
                        return Qt.point(distance * Math.sin(angle), -distance * Math.cos(angle))
                    })
                    const xs = points.map(point => point.x)
                    const ys = points.map(point => point.y)
                    const minX = Math.min.apply(null, xs), maxX = Math.max.apply(null, xs)
                    const minY = Math.min.apply(null, ys), maxY = Math.max.apply(null, ys)
                    const scale = Math.min(width / Math.max(1, maxX - minX), height / Math.max(1, maxY - minY)) * 0.8
                    ctx.strokeStyle = QGroundControl.globalPalette.text
                    ctx.fillStyle = ctx.strokeStyle
                    ctx.lineWidth = 2
                    ctx.beginPath()
                    points.forEach((point, index) => {
                        const x = width / 2 + (point.x - (minX + maxX) / 2) * scale
                        const y = height / 2 + (point.y - (minY + maxY) / 2) * scale
                        if (index === 0) { ctx.moveTo(x, y) } else { ctx.lineTo(x, y) }
                    })
                    ctx.stroke()
                }
            }
        }
    }
    QGCButton {
        id: add
        objectName: "caacTemplateAdd"
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        text: qsTr("Add")
        primary: true
        enabled: root._preview.length > 0 && !root.routeController.rotationPreviewActive
        onClicked: root.routeController.insertTemplate(tabs.currentIndex, root._reference, root._options)
    }
}
