import QGroundControl
import QGroundControl.Controls
import QtGraphs
import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Window

Item {
    id: root

    property var axisX: null
    property var axisY: null
    readonly property bool expanded: expandedWindow.visible
    property bool followLatest: true
    property alias graphView: graph
    property var legendModel: []
    property bool running: false
    readonly property var seriesList: graph.seriesList
    property alias theme: graph.theme
    readonly property int timeDecimals: Math.max(0, Math.min(4, -Math.floor(Math.log10(xTickInterval))))
    property string title: ""
    readonly property real xTickInterval: niceTickInterval(axisX, graph.plotArea.width, ScreenTools.defaultFontPixelWidth * 10)
    readonly property real yTickInterval: niceTickInterval(axisY, graph.plotArea.height, ScreenTools.defaultFontPixelHeight * 3)

    signal clearRequested
    signal restoreRequested
    signal toggleRunningRequested

    function addSeries(series) {
        graph.addSeries(series);
    }

    function closeExpanded() {
        if (expandedWindow) {
            expandedWindow.hide();
        }
    }

    function inPlot(x, y) {
        const area = graph.plotArea;
        return area.width > 0 && area.height > 0 && x >= area.x && x <= area.x + area.width && y >= area.y && y <= area.y + area.height;
    }

    function niceTickInterval(axis, pixels, labelSpacing) {
        if (!axis || !isFinite(axis.min) || !isFinite(axis.max) || axis.max <= axis.min) {
            return 1;
        }
        const sections = Math.max(1, Math.floor(pixels / labelSpacing));
        const rough = (axis.max - axis.min) / sections;
        const magnitude = Math.pow(10, Math.floor(Math.log10(rough)));
        const normalized = rough / magnitude;
        return (normalized <= 1 ? 1 : normalized <= 2 ? 2 : normalized <= 5 ? 5 : 10) * magnitude;
    }

    function openExpanded() {
        expandedWindow.width = Math.max(expandedWindow.minimumWidth, Math.min(expandedWindow.maximumWidth, expandedWindow.width));
        expandedWindow.height = Math.max(expandedWindow.minimumHeight, Math.min(expandedWindow.maximumHeight, expandedWindow.height));
        expandedWindow.x = expandedWindow.screen.virtualX + (expandedWindow.screen.width - expandedWindow.width) / 2;
        expandedWindow.y = expandedWindow.screen.virtualY + (expandedWindow.screen.height - expandedWindow.height) / 2;
        expandedWindow.show();
        expandedWindow.requestActivate();
    }

    function pan(dx, dy) {
        const area = graph.plotArea;
        if (!axisX || !axisY || area.width <= 0 || area.height <= 0) {
            return;
        }
        const xShift = -dx * (axisX.max - axisX.min) / area.width;
        const yShift = dy * (axisY.max - axisY.min) / area.height;
        followLatest = false;
        setAxisRange(axisX, axisX.min + xShift, axisX.max + xShift);
        setAxisRange(axisY, axisY.min + yShift, axisY.max + yShift);
    }

    function removeSeries(series) {
        graph.removeSeries(series);
    }

    function setAxisRange(axis, minimum, maximum) {
        if (!axis || !isFinite(minimum) || !isFinite(maximum) || maximum <= minimum) {
            return;
        }
        // Keep the intermediate range valid even when panning beyond the previous range.
        if (minimum >= axis.max) {
            axis.max = maximum;
            axis.min = minimum;
        } else {
            axis.min = minimum;
            axis.max = maximum;
        }
    }

    function zoom(x, y, delta) {
        if (!axisX || !axisY || !inPlot(x, y) || delta === 0) {
            return;
        }
        const area = graph.plotArea;
        const factor = Math.pow(1.2, Math.max(-20, Math.min(20, -delta / 120)));
        const xRatio = (x - area.x) / area.width;
        const yRatio = 1 - (y - area.y) / area.height;
        const oldXSpan = axisX.max - axisX.min;
        const oldYSpan = axisY.max - axisY.min;
        if (oldXSpan <= 0 || oldYSpan <= 0) {
            return;
        }
        const xSpan = Math.max(0.05, Math.min(180, oldXSpan * factor));
        const ySpan = Math.max(0.001, Math.min(1e9, oldYSpan * factor));
        const xMinimum = axisX.min + xRatio * (oldXSpan - xSpan);
        const yMinimum = axisY.min + yRatio * (oldYSpan - ySpan);
        followLatest = false;
        setAxisRange(axisX, xMinimum, xMinimum + xSpan);
        setAxisRange(axisY, yMinimum, yMinimum + ySpan);
    }

    objectName: "pidTuning_chart"

    Component.onDestruction: closeExpanded()
    onVisibleChanged: if (!visible) {
        closeExpanded();
    }

    QGCPalette {
        id: chartPalette
    }

    Item {
        id: dock

        anchors.fill: parent

        QGCLabel {
            anchors.centerIn: parent
            horizontalAlignment: Text.AlignHCenter
            text: qsTr("Chart opened in a separate window.")
            visible: root.expanded
            width: parent.width * 0.8
            wrapMode: Text.WordWrap
        }
    }

    GraphsView {
        id: graph

        anchors.fill: parent
        axisX: root.axisX
        axisXSmoothing: 0
        axisY: root.axisY
        axisYSmoothing: 0
        objectName: "pidTuning_graph"
        // Move the view itself so its series, subscriptions and collected points stay intact.
        parent: root.expanded ? expandedHost : dock

    }

    // Keep pointer input outside the GraphsView 3D viewport.
    MouseArea {
        parent: graph.parent
        z: graph.z + 1
        objectName: "pidTuning_chartInput"
        property bool dragging: false
        property var lastPoint
        property var startPoint

        ToolTip.text: qsTr("Double-click to enlarge. Drag to pan; scroll to zoom.")
        ToolTip.visible: containsMouse && !pressed && !root.expanded
        acceptedButtons: Qt.LeftButton
        anchors.fill: graph
        cursorShape: pressed ? Qt.ClosedHandCursor : Qt.OpenHandCursor
        hoverEnabled: true
        preventStealing: true

        onCanceled: {
            lastPoint = undefined;
            dragging = false;
        }
        onDoubleClicked: {
            if (root.expanded) {
                root.restoreRequested();
            } else {
                root.openExpanded();
            }
        }
        onPositionChanged: mouse => {
            if (!pressed || !lastPoint) {
                return;
            }
            if (!dragging && Math.hypot(mouse.x - startPoint.x, mouse.y - startPoint.y) < Qt.styleHints.startDragDistance) {
                return;
            }
            dragging = true;
            root.pan(mouse.x - lastPoint.x, mouse.y - lastPoint.y);
            lastPoint = Qt.point(mouse.x, mouse.y);
        }
        onPressed: mouse => {
            if (!root.inPlot(mouse.x, mouse.y)) {
                mouse.accepted = false;
                return;
            }
            startPoint = Qt.point(mouse.x, mouse.y);
            lastPoint = startPoint;
            dragging = false;
        }
        onReleased: {
            lastPoint = undefined;
            dragging = false;
        }
        onWheel: wheel => {
            if (!root.inPlot(wheel.x, wheel.y)) {
                wheel.accepted = false;
                return;
            }
            root.zoom(wheel.x, wheel.y, wheel.angleDelta.y || wheel.pixelDelta.y);
            wheel.accepted = true;
        }
    }

    QGCButton {
        anchors.right: parent.right
        anchors.top: parent.top
        objectName: "pidTuning_restore"
        text: qsTr("Restore live view")
        visible: !root.expanded && !root.followLatest

        onClicked: root.restoreRequested()
    }

    Window {
        id: expandedWindow

        color: chartPalette.window
        flags: Qt.Window
        height: Math.min(maximumHeight, Math.min(Screen.height, Screen.desktopAvailableHeight) * 0.8)
        maximumHeight: Math.max(1, Math.min(Screen.height, Screen.desktopAvailableHeight) - ScreenTools.defaultFontPixelHeight * 3)
        maximumWidth: Math.max(1, Math.min(Screen.width, Screen.desktopAvailableWidth) - ScreenTools.defaultFontPixelHeight * 2)
        minimumHeight: Math.min(maximumHeight, ScreenTools.defaultFontPixelHeight * 20)
        minimumWidth: Math.min(maximumWidth, ScreenTools.defaultFontPixelWidth * 45)
        objectName: "pidTuning_chartWindow"
        screen: root.Window.window ? root.Window.window.screen : Qt.application.screens[0]
        title: root.title
        transientParent: root.Window.window
        visible: false
        width: Math.min(maximumWidth, Math.min(Screen.width, Screen.desktopAvailableWidth) * 0.8)

        onClosing: close => {
            close.accepted = false;
            root.closeExpanded();
        }

        ColumnLayout {
            anchors.fill: parent
            anchors.margins: ScreenTools.defaultFontPixelHeight / 2
            spacing: ScreenTools.defaultFontPixelHeight / 2

            QGCLabel {
                Layout.fillWidth: true
                text: qsTr("Drag to pan; scroll to zoom. Double-click to restore live view.")
                wrapMode: Text.WordWrap
            }

            Item {
                id: expandedHost

                Layout.fillHeight: true
                Layout.fillWidth: true
                Layout.minimumHeight: 0
            }

            Flow {
                Layout.fillWidth: true
                spacing: ScreenTools.defaultFontPixelWidth

                Repeater {
                    model: root.legendModel

                    Row {
                        spacing: ScreenTools.defaultFontPixelWidth / 2

                        Rectangle {
                            anchors.verticalCenter: parent.verticalCenter
                            color: modelData.color
                            height: ScreenTools.defaultFontPixelHeight / 3
                            width: ScreenTools.defaultFontPixelHeight
                        }

                        QGCLabel {
                            text: modelData.name
                        }
                    }
                }
            }

            Flow {
                Layout.fillWidth: true
                spacing: ScreenTools.defaultFontPixelWidth

                QGCButton {
                    enabled: !root.followLatest
                    objectName: "pidTuning_expandedRestore"
                    text: qsTr("Restore live view")

                    onClicked: root.restoreRequested()
                }

                QGCButton {
                    objectName: "pidTuning_expandedClear"
                    text: qsTr("Clear")

                    onClicked: root.clearRequested()
                }

                QGCButton {
                    objectName: "pidTuning_expandedToggle"
                    text: root.running ? qsTr("Stop") : qsTr("Start")

                    onClicked: root.toggleRunningRequested()
                }

                QGCButton {
                    objectName: "pidTuning_expandedClose"
                    text: qsTr("Close")

                    onClicked: root.closeExpanded()
                }
            }
        }
    }
}
