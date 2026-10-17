import QtQuick
import QtQuick.Window

Window {
    id: root

    width: Math.min(Screen.width * 0.8, textMetrics.height * 62) * 0.5
    height: width * 623 / 1107
    x: Screen.virtualX + (Screen.width - width) / 2
    y: Screen.virtualY + (Screen.height - height) / 2
    visible: true
    modality: Qt.ApplicationModal
    flags: Qt.SplashScreen | Qt.FramelessWindowHint | Qt.WindowStaysOnTopHint
    // Brand colors follow the supplied artwork; importing QGC here would initialize services.
    color: "#111519"
    title: qsTr("飞虎教培地面站")
    objectName: "caacStartupWindow"

    required property QtObject startup
    readonly property color brandTextColor: "#ffffff"

    FontMetrics {
        id: textMetrics
        font: Qt.application.font
    }

    SystemPalette {
        id: systemPalette
        colorGroup: SystemPalette.Active
    }

    // Keep the supplied brand artwork intact; its painted progress bar is outside this source clip.
    Item {
        width: parent.width
        height: parent.height * 0.76
        clip: true

        Image {
            width: parent.width
            height: root.height
            source: "qrc:/CAAC/Brand/startup-background.png"
            fillMode: Image.Stretch
            objectName: "caacStartupArtwork"
        }
    }

    Rectangle {
        y: parent.height * 0.72
        width: parent.width
        height: parent.height - y
        gradient: Gradient {
            GradientStop { position: 0; color: "transparent" }
            GradientStop { position: 0.22; color: root.color }
            GradientStop { position: 1; color: root.color }
        }
    }

    Rectangle {
        id: track
        x: (parent.width - width) / 2
        y: parent.height * 0.84
        width: parent.width * 0.305
        height: Math.max(2, textMetrics.height * 0.25)
        radius: height / 2
        color: root.color
        border.color: systemPalette.mid
        objectName: "caacStartupProgressTrack"

        Rectangle {
            x: track.border.width
            y: track.border.width
            width: (track.width - 2 * x) * root.startup.progress / 100
            height: track.height - 2 * y
            radius: track.radius
            color: "#ff9e2b"
            objectName: "caacStartupProgressFill"
        }
    }

    Text {
        anchors.top: track.bottom
        anchors.topMargin: textMetrics.height * 0.55
        anchors.horizontalCenter: parent.horizontalCenter
        width: parent.width * 0.82
        text: root.startup.statusText
        color: root.brandTextColor
        font: Qt.application.font
        horizontalAlignment: Text.AlignHCenter
        wrapMode: Text.WordWrap
        objectName: "caacStartupStatus"
    }

    Rectangle {
        anchors.top: parent.top
        anchors.right: parent.right
        anchors.margins: textMetrics.height * 0.6
        width: textMetrics.height * 2.2
        height: width
        radius: textMetrics.height * 0.25
        color: closeMouse.containsMouse ? systemPalette.dark : "transparent"

        Text {
            anchors.centerIn: parent
            text: "×"
            color: root.brandTextColor
            font.pixelSize: textMetrics.height * 1.4
        }

        MouseArea {
            id: closeMouse
            anchors.fill: parent
            hoverEnabled: true
            onClicked: root.startup.requestClose()
        }

        Accessible.role: Accessible.Button
        Accessible.name: qsTr("关闭")
        Accessible.onPressAction: root.startup.requestClose()
    }

    onClosing: function(close) {
        close.accepted = false
        root.startup.requestClose()
    }
}
