pragma ComponentBehavior: Bound

import QtQuick

import QGroundControl
import QGroundControl.Controls
import QGroundControl.PlanView

/// Picker for mission item markers drawn too close together to click individually
DropPanel {
    id: root

    modal: false

    required property var groupItems

    signal itemSelected(int sequenceNumber)

    sourceComponent: Component {
        Item {
            implicitWidth: itemListView.width
            implicitHeight: itemListView.height

            QGCListView {
                id: itemListView

                objectName: "missionItemSelectionList"
                width: Math.min(Math.max(contentItem.childrenRect.width, _itemExtent), _maxWidth)
                height: _itemExtent
                orientation: ListView.Horizontal
                model: root.groupItems
                cacheBuffer: width * 2
                reuseItems: true
                currentIndex: -1

                readonly property real _itemExtent: Math.max(ScreenTools.minTouchPixels, ScreenTools.defaultFontPixelHeight * 2.5)
                readonly property real _maxWidth: root.dropViewPort.width * 0.4

                function _scrollBy(delta) {
                    const currentTarget = wheelScrollAnimation.running ? wheelScrollAnimation.to : contentX
                    const target = Math.max(0, Math.min(currentTarget - delta, Math.max(0, contentWidth - width)))
                    wheelScrollAnimation.stop()
                    wheelScrollAnimation.from = contentX
                    wheelScrollAnimation.to = target
                    wheelScrollAnimation.start()
                }

                delegate: Item {
                    id: itemDelegate

                    objectName: "missionItemSelection_" + modelData.sequenceNumber
                    width: Math.max(itemListView._itemExtent, itemLabel.width + ScreenTools.defaultFontPixelWidth)
                    height: itemListView._itemExtent

                    required property var modelData

                    readonly property bool _isPlannedLanding: QGroundControl.corePlugin.directMissionItemEditing
                        && modelData.isSimpleItem && modelData.isLandCommand
                    readonly property bool _usesAbbreviation: _isPlannedLanding || modelData.abbreviation !== ""
                    readonly property int _displaySequenceNumber: {
                        if (!modelData.flyView || !QGroundControl.corePlugin.directMissionItemEditing) { return modelData.sequenceNumber }
                        const simulation = QGroundControl.corePlugin.simulation
                        void simulation.waypointNumberRevision
                        return simulation.waypointNumber(modelData.sequenceNumber, modelData.coordinate)
                    }
                    readonly property string _supplementaryLabel: _isPlannedLanding ? qsTr("Planned landing point")
                        : (!_usesAbbreviation ? "" : `${modelData.abbreviation} (${_displaySequenceNumber})`)

                    MissionItemIndexLabel {
                        id: itemLabel
                        anchors.centerIn: parent
                        checked: itemDelegate.modelData.isCurrentItem || itemDelegate.modelData.hasCurrentChildItem
                        plannedLanding: itemDelegate._isPlannedLanding
                        label: itemDelegate.modelData.abbreviation
                        index: itemDelegate._usesAbbreviation ? -1 : itemDelegate._displaySequenceNumber
                        small: false
                        supplementaryLabel: itemDelegate._supplementaryLabel
                    }

                    MouseArea {
                        anchors.fill: parent
                        onClicked: {
                            root.close()
                            root.itemSelected(itemDelegate.modelData.sequenceNumber)
                        }
                    }
                }

                NumberAnimation {
                    id: wheelScrollAnimation

                    target: itemListView
                    property: "contentX"
                    duration: 120
                    easing.type: Easing.OutCubic
                }

                WheelHandler {
                    acceptedDevices: PointerDevice.Mouse | PointerDevice.TouchPad
                    orientation: Qt.Vertical
                    target: null
                    onWheel: event => {
                        itemListView._scrollBy(event.pixelDelta.y || event.angleDelta.y)
                        event.accepted = true
                    }
                }

                WheelHandler {
                    acceptedDevices: PointerDevice.Mouse | PointerDevice.TouchPad
                    orientation: Qt.Horizontal
                    target: null
                    onWheel: event => {
                        itemListView._scrollBy(event.pixelDelta.x || event.angleDelta.x)
                        event.accepted = true
                    }
                }

                onMovementStarted: wheelScrollAnimation.stop()
            }
        }
    }
}
