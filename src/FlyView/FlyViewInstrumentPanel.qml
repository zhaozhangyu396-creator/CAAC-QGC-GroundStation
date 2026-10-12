import QtQuick

import QGroundControl
import QGroundControl.Controls
import CAAC.Plan

CaacAttitudeInstrument {
    z: QGroundControl.zOrderWidgets
    vehicle: globals.activeVehicle
    property var missionController: _missionController
    readonly property real extraInset: 0
    readonly property real extraValuesWidth: 0
}
