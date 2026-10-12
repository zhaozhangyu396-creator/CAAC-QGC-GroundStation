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
    ColumnLayout {
        width: Math.min(root.maxContentAvailableWidth, ScreenTools.defaultFontPixelWidth * 65)
        spacing: ScreenTools.defaultFontPixelHeight * 0.5
        QGCLabel {
            Layout.fillWidth: true
            wrapMode: Text.WordWrap
            text: qsTr("Built-in kinematic training, separate from PX4 SITL. Uploads a copy of the saved planning settings and route to an isolated simulation aircraft. Disconnect other aircraft first.")
        }
        QGCLabel {
            objectName: "caacSimulationStatus"
            Layout.fillWidth: true
            wrapMode: Text.WordWrap
            text: root._simulation.status || qsTr("Ready to simulate")
        }
        QGCButton {
            objectName: "caacUploadSimulation"
            Layout.fillWidth: true
            text: qsTr("Upload and execute simulation mission")
            enabled: !root._simulation.active
            onClicked: root._simulation.start(root.editor.planMasterController)
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
            text: qsTr("Stop simulation")
            enabled: root._simulation.active
            onClicked: root._simulation.stop()
        }
    }
}
