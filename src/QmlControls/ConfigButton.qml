import QGroundControl
import QGroundControl.Controls

SettingsButton {
    icon.color: outlined || setupComplete ? textColor : "red"

    property bool setupComplete: true
}
