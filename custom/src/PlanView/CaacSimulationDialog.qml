pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import QGroundControl
import QGroundControl.Controls

QGCPopupDialog {
    id: root
    required property var editor
    readonly property var _simulation: QGroundControl.corePlugin.simulation
    title: qsTr("Simulation flight")
    buttons: Dialog.Close
    Connections {
        target: root._simulation
        function onMissionUploaded() { root.close() }
    }
    ColumnLayout {
        width: Math.min(root.maxContentAvailableWidth, ScreenTools.defaultFontPixelWidth * 65)
        spacing: ScreenTools.defaultFontPixelHeight * 0.5
        QGCLabel {
            objectName: "caacSimulationStatus"
            Layout.fillWidth: true
            wrapMode: Text.WordWrap
            text: root._simulation.status || qsTr("Ready to simulate")
        }
        QGCButton {
            objectName: "caacUploadSimulation"
            Layout.fillWidth: true
            text: qsTr("Upload simulation mission")
            enabled: root._simulation.vehicle !== null && !root.editor.planMasterController.syncInProgress
            onClicked: root._simulation.upload(root.editor.planMasterController)
        }
        QGCButton {
            objectName: "caacViewSimulation"
            Layout.fillWidth: true
            text: qsTr("View simulation flight")
            enabled: root._simulation.vehicle !== null
            onClicked: { root.close(); mainWindow.showFlyView() }
        }
        QGCButton {
            objectName: "caacRestartSimulation"
            Layout.fillWidth: true
            text: qsTr("Restart simulation and clear mileage")
            enabled: root._simulation.vehicle !== null
            onClicked: root._simulation.restart()
        }
        QGCButton {
            objectName: "caacStopSimulation"
            Layout.fillWidth: true
            text: qsTr("Disconnect simulation")
            enabled: root._simulation.active
            onClicked: root._simulation.stop()
        }
    }
}
