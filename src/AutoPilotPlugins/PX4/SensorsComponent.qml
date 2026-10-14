import QtQuick

import QGroundControl
import QGroundControl.Controls
import QGroundControl.AutoPilotPlugins.PX4

SetupPage {
    pageName: QGroundControl.corePlugin.directMissionItemEditing ? qsTr("飞控校准") : ScreenTools.vehicleComponentName(vehicleComponent)
    pageDescription: QGroundControl.corePlugin.directMissionItemEditing ? "" : vehicleComponent ? vehicleComponent.description : ""
    pageComponent:  pageComponent
    Component {
        id: pageComponent
        SensorsSetup {
            width:      availableWidth
            height:     availableHeight
        }
    }
}
