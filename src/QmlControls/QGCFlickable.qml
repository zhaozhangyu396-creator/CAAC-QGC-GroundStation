import QtQuick
import QtQuick.Controls

import QGroundControl
import QGroundControl.Controls

/// QGC version of Flickable control that shows horizontal/vertial scroll indicators
Flickable {
    id:                     root
    boundsBehavior:         Flickable.StopAtBounds
    clip:                   true
    maximumFlickVelocity:   (ScreenTools.realPixelDensity * 25.4) * 8   // About two inches per second

    property color indicatorColor: qgcPal.text
    property bool consumeWheelEvents: false

    WheelHandler {
        enabled: root.consumeWheelEvents
        target: null
        acceptedDevices: PointerDevice.Mouse | PointerDevice.TouchPad
        onWheel: (event) => {
            const delta = event.pixelDelta.y !== 0 ? event.pixelDelta.y
                : event.angleDelta.y / 120 * ScreenTools.defaultFontPixelHeight * 3
            root.cancelFlick()
            root.contentY = Math.max(root.originY, Math.min(root.contentY - delta,
                root.originY + Math.max(0, root.contentHeight - root.height)))
            event.accepted = true
        }
    }

    Component.onCompleted: {
        var indicatorComponent = Qt.createComponent("QGCFlickableScrollIndicator.qml")
        indicatorComponent.createObject(root, { orientation: QGCFlickableScrollIndicator.Horizontal })
        indicatorComponent = Qt.createComponent("QGCFlickableScrollIndicator.qml")
        indicatorComponent.createObject(root, { orientation: QGCFlickableScrollIndicator.Vertical })
    }
}
