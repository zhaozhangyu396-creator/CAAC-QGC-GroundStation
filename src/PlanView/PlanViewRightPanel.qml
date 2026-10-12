import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import QGroundControl
import QGroundControl.Controls

Item {
    required property var editorMap
    required property var planMasterController

    signal editingLayerChangeRequested(int layer)

    id: root
    z: QGroundControl.zOrderWidgets + 1

    readonly property bool showNativeEditor: !customEditor.enabled || !customEditor.editorItem || customEditor.editorItem.showMissionTree !== false
    property var  _missionController: planMasterController.missionController
    property real _toolsMargin:       ScreenTools.defaultFontPixelWidth * 0.75

    function selectNextNotReady() {
        for (var i = 0; i < _missionController.visualItems.count; i++) {
            var vmi = _missionController.visualItems.get(i)
            if (vmi.readyForSaveState === VisualMissionItem.NotReadyForSaveData) {
                _missionController.setCurrentPlanViewSeqNum(vmi.sequenceNumber, true)
                break
            }
        }
    }

    QGCPalette { id: qgcPal }

    Rectangle {
        id:             rightPanelBackground
        anchors.fill:   parent
        color:          qgcPal.window
        opacity:        0.85
        visible: !customEditor.enabled || !customEditor.editorItem || customEditor.editorItem.showMissionTree !== false
    }


    // Open/Close panel
    Item {
        id:                     panelOpenCloseButton
        anchors.right:          parent.left
        anchors.verticalCenter: parent.verticalCenter
        width:                  toggleButtonRect.width - toggleButtonRect.radius
        height:                 toggleButtonRect.height
        clip:                   true

        property bool _expanded: root.anchors.right == root.parent.right

        Rectangle {
            id:             toggleButtonRect
            width:          ScreenTools.defaultFontPixelWidth * 2.25
            height:         width * 3
            radius:         ScreenTools.defaultBorderRadius
            color:          rightPanelBackground.color
            opacity:        rightPanelBackground.opacity

            QGCLabel {
                id:                 toggleButtonLabel
                anchors.centerIn:   parent
                text:               panelOpenCloseButton._expanded ? ">" : "<"
                color:              qgcPal.buttonText
            }

        }

        QGCMouseArea {
            anchors.fill: parent

            onClicked: {
                if (panelOpenCloseButton._expanded) {
                    // Close panel
                    root.anchors.right = undefined
                    root.anchors.left = root.parent.right
                } else {
                    // Open panel
                    root.anchors.left = undefined
                    root.anchors.right = root.parent.right
                }
            }
        }
    }

    //-------------------------------------------------------
    // Right Panel Controls
    Item {
        anchors.fill: rightPanelBackground

        DeadMouseArea {
            anchors.fill:   parent
            enabled: rightPanelBackground.visible
        }

        PlanTreeView {
            id:                     planTreeView
            objectName:             "planView_planTree"
            anchors.top:            customEditor.bottom
            anchors.left:           parent.left
            anchors.right:          parent.right
            anchors.bottom:         parent.bottom
            visible:                !customEditor.enabled || !customEditor.editorItem
                                    || customEditor.editorItem.showMissionTree !== false
            editorMap:              root.editorMap
            planMasterController:   root.planMasterController
            onEditingLayerChangeRequested: (layer) => root.editingLayerChangeRequested(layer)
        }

        Loader {
            id: customEditor
            anchors.top: parent.top
            anchors.left: parent.left
            anchors.right: parent.right
            source: QGroundControl.corePlugin.planViewEditor
            readonly property var editorItem: item
            height: !enabled ? 0 : editorItem && editorItem.showMissionTree === false
                ? parent.height : editorItem ? editorItem.implicitHeight : 0
            enabled: root.editorMap.directMissionItemEditing
            visible: enabled
            onLoaded: {
                item.editorMap = root.editorMap
                item.planMasterController = root.planMasterController
            }
        }
    }

    function selectLayer(nodeType) {
        // Ensure panel is open
        if (!panelOpenCloseButton._expanded) {
            root.anchors.left = undefined
            root.anchors.right = root.parent.right
        }
        planTreeView.selectLayer(nodeType)
    }
}
