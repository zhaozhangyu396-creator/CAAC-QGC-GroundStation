pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import QGroundControl
import QGroundControl.Controls

QGCPopupDialog {
    id: root
    readonly property var _simulation: QGroundControl.corePlugin.simulation
    readonly property var _vehicle: _simulation.vehicle
    readonly property bool _ready: _simulation.active && _vehicle && _vehicle.initialConnectComplete
        && _vehicle === QGroundControl.multiVehicleManager.activeVehicle
    title: qsTr("Simulation emergency")
    buttons: Dialog.Close

    function finishAction(action) {
        if (_ready) {
            action()
            root.close()
        }
    }

    ColumnLayout {
        width: Math.min(root.maxContentAvailableWidth, ScreenTools.defaultFontPixelWidth * 45)
        spacing: ScreenTools.defaultFontPixelHeight * 0.5
        QGCButton {
            objectName: "caacEmergencyHover"
            Layout.fillWidth: true
            text: qsTr("Emergency hover")
            enabled: root._ready && root._vehicle.airborne
            onClicked: root.finishAction(() => root._vehicle.pauseVehicle())
        }
        QGCButton {
            objectName: "caacLand"
            Layout.fillWidth: true
            text: qsTr("Land at current position")
            enabled: root._ready && root._vehicle.airborne
            onClicked: root.finishAction(() => root._vehicle.guidedModeLand())
        }
        QGCButton {
            objectName: "caacEmergencyRestart"
            Layout.fillWidth: true
            text: qsTr("Restart simulation and clear mileage")
            enabled: root._ready
            onClicked: root.finishAction(() => root._simulation.restart())
        }
        QGCButton {
            objectName: "caacDisconnectSimulation"
            Layout.fillWidth: true
            text: qsTr("Disconnect simulation")
            enabled: root._ready
            onClicked: root.finishAction(() => root._simulation.stop())
        }
    }
}
