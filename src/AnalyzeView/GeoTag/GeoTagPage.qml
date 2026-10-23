import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import QGroundControl
import QGroundControl.AnalyzeView
import QGroundControl.Controls

AnalyzePage {
    id:                 geoTagPage
    objectName:         "geoTag_page"
    pageComponent:      pageComponent
    pageDescription:    qsTr("Tag images from a survey mission with GPS coordinates from your flight log.")

    readonly property real _margin: ScreenTools.defaultFontPixelWidth

    QGCPalette { id: qgcPal; colorGroupEnabled: true }

    Component {
        id: pageComponent

        QGCFlickable {
            id: geoTagScroll
            objectName: "geoTag_scroller"
            width: availableWidth
            height: availableHeight
            contentWidth: width
            contentHeight: geoTagContent.height
            flickableDirection: Flickable.VerticalFlick

            ColumnLayout {
                id: geoTagContent
                objectName: "geoTag_content"
                spacing: _margin * 2
                width: geoTagScroll.width

                // Status Card
                Rectangle {
                    Layout.fillWidth:       true
                    Layout.preferredHeight: statusColumn.height + _margin * 2
                    color:                  qgcPal.windowShade
                    radius:                 ScreenTools.defaultFontPixelWidth / 2
                    visible:                GeoTagController.inProgress || GeoTagController.errorMessage || GeoTagController.taggedCount > 0

                    ColumnLayout {
                        id:                 statusColumn
                        anchors.left:       parent.left
                        anchors.right:      parent.right
                        anchors.top:        parent.top
                        anchors.margins:    _margin
                        spacing:            _margin

                        RowLayout {
                            Layout.fillWidth:   true
                            spacing:            _margin
                            visible:            GeoTagController.inProgress

                            BusyIndicator {
                                Layout.preferredWidth:  ScreenTools.defaultFontPixelHeight * 2
                                Layout.preferredHeight: ScreenTools.defaultFontPixelHeight * 2
                                running:                GeoTagController.inProgress
                            }

                            ColumnLayout {
                                Layout.fillWidth: true
                                spacing: _margin / 2

                                QGCLabel {
                                    Layout.fillWidth: true
                                    Layout.minimumWidth: 0
                                    wrapMode: Text.WordWrap
                                    text:               qsTr("Geotagging in progress...")
                                    font.bold:          true
                                }

                                ProgressBar {
                                    Layout.fillWidth:   true
                                    from:               0
                                    to:                 100
                                    value:              GeoTagController.progress
                                }
                            }
                        }

                        QGCLabel {
                            Layout.fillWidth:       true
                            text:                   GeoTagController.errorMessage
                            color:                  qgcPal.colorRed
                            font.bold:              true
                            wrapMode:               Text.WordWrap
                            horizontalAlignment:    Text.AlignHCenter
                            visible:                GeoTagController.errorMessage && !GeoTagController.inProgress
                        }

                        QGCLabel {
                            Layout.fillWidth:       true
                            Layout.minimumWidth:    0
                            wrapMode:               Text.WordWrap
                            text: {
                                if (GeoTagController.taggedCount > 0 && !GeoTagController.inProgress) {
                                    let msg = qsTr("Successfully tagged %1 images").arg(GeoTagController.taggedCount)
                                    let details = []
                                    if (GeoTagController.skippedCount > 0) {
                                        details.push(qsTr("%1 skipped").arg(GeoTagController.skippedCount))
                                    }
                                    if (GeoTagController.failedCount > 0) {
                                        details.push(qsTr("%1 failed").arg(GeoTagController.failedCount))
                                    }
                                    if (details.length > 0) {
                                        msg += " (" + details.join(", ") + ")"
                                    }
                                    return msg
                                }
                                return ""
                            }
                            color:                  GeoTagController.failedCount > 0 ? qgcPal.colorOrange : qgcPal.colorGreen
                            font.bold:              true
                            horizontalAlignment:    Text.AlignHCenter
                            visible:                GeoTagController.taggedCount > 0 && !GeoTagController.inProgress
                        }
                    }
                }

                // Step 1: Log File
                Rectangle {
                    Layout.fillWidth:       true
                    Layout.preferredHeight: step1Column.height + _margin * 2
                    color:                  qgcPal.windowShade
                    radius:                 ScreenTools.defaultFontPixelWidth / 2

                    ColumnLayout {
                        id:                 step1Column
                        anchors.left:       parent.left
                        anchors.right:      parent.right
                        anchors.top:        parent.top
                        anchors.margins:    _margin
                        spacing:            _margin

                        RowLayout {
                            Layout.fillWidth: true
                            spacing: _margin

                            Rectangle {
                                Layout.preferredWidth:  ScreenTools.defaultFontPixelHeight * 1.5
                                Layout.preferredHeight: ScreenTools.defaultFontPixelHeight * 1.5
                                radius:                 height / 2
                                color:                  GeoTagController.logFile ? qgcPal.colorGreen : qgcPal.button

                                QGCLabel {
                                    anchors.centerIn:   parent
                                    text:               GeoTagController.logFile ? "\u2713" : "1"
                                    color:              GeoTagController.logFile ? "white" : qgcPal.buttonText
                                    font.bold:          true
                                }
                            }

                            QGCLabel {
                                Layout.fillWidth: true
                                Layout.minimumWidth: 0
                                wrapMode: Text.WordWrap
                                text:       qsTr("Select Flight Log")
                                font.bold:  true
                            }
                        }

                        RowLayout {
                            Layout.fillWidth: true
                            spacing: _margin

                            QGCButton {
                                text:               qsTr("Browse...")
                                enabled:            !GeoTagController.inProgress
                                onClicked:          openLogFile.openForLoad()

                                QGCFileDialog {
                                    id:             openLogFile
                                    title:          qsTr("Select Flight Log")
                                    nameFilters:    [qsTr("Flight logs (*.ulg *.bin)"), qsTr("ULog (*.ulg)"), qsTr("DataFlash (*.bin)"), qsTr("All Files (*)")]
                                    defaultSuffix:  "ulg"
                                    onAcceptedForLoad: (file) => {
                                        GeoTagController.logFile = file
                                        close()
                                    }
                                }
                            }

                            QGCLabel {
                                objectName: "geoTag_logPath"
                                Layout.fillWidth:   true
                                Layout.minimumWidth: 0
                                wrapMode: Text.Wrap
                                text:               GeoTagController.logFile ? GeoTagController.logFile : qsTr("No file selected")
                                opacity:            GeoTagController.logFile ? 1.0 : 0.5
                            }
                        }
                    }
                }

                // Step 2: Image Directory
                Rectangle {
                    Layout.fillWidth:       true
                    Layout.preferredHeight: step2Column.height + _margin * 2
                    color:                  qgcPal.windowShade
                    radius:                 ScreenTools.defaultFontPixelWidth / 2

                    ColumnLayout {
                        id:                 step2Column
                        anchors.left:       parent.left
                        anchors.right:      parent.right
                        anchors.top:        parent.top
                        anchors.margins:    _margin
                        spacing:            _margin

                        RowLayout {
                            Layout.fillWidth: true
                            spacing: _margin

                            Rectangle {
                                Layout.preferredWidth:  ScreenTools.defaultFontPixelHeight * 1.5
                                Layout.preferredHeight: ScreenTools.defaultFontPixelHeight * 1.5
                                radius:                 height / 2
                                color:                  GeoTagController.imageDirectory ? qgcPal.colorGreen : qgcPal.button

                                QGCLabel {
                                    anchors.centerIn:   parent
                                    text:               GeoTagController.imageDirectory ? "\u2713" : "2"
                                    color:              GeoTagController.imageDirectory ? "white" : qgcPal.buttonText
                                    font.bold:          true
                                }
                            }

                            QGCLabel {
                                Layout.fillWidth: true
                                Layout.minimumWidth: 0
                                wrapMode: Text.WordWrap
                                text:       qsTr("Select Image Folder")
                                font.bold:  true
                            }
                        }

                        RowLayout {
                            Layout.fillWidth: true
                            spacing: _margin

                            QGCButton {
                                text:               qsTr("Browse...")
                                enabled:            !GeoTagController.inProgress
                                onClicked:          selectImageDir.openForLoad()

                                QGCFileDialog {
                                    id:             selectImageDir
                                    title:          qsTr("Select Image Folder")
                                    selectFolder:   true
                                    onAcceptedForLoad: (file) => {
                                        GeoTagController.imageDirectory = file
                                        close()
                                    }
                                }
                            }

                            QGCLabel {
                                objectName: "geoTag_imagePath"
                                Layout.fillWidth:   true
                                Layout.minimumWidth: 0
                                wrapMode: Text.Wrap
                                text:               GeoTagController.imageDirectory ? GeoTagController.imageDirectory : qsTr("No folder selected")
                                opacity:            GeoTagController.imageDirectory ? 1.0 : 0.5
                            }
                        }
                    }
                }

                // Step 3: Save Directory (Optional)
                Rectangle {
                    Layout.fillWidth:       true
                    Layout.preferredHeight: step3Column.height + _margin * 2
                    color:                  qgcPal.windowShade
                    radius:                 ScreenTools.defaultFontPixelWidth / 2

                    ColumnLayout {
                        id:                 step3Column
                        anchors.left:       parent.left
                        anchors.right:      parent.right
                        anchors.top:        parent.top
                        anchors.margins:    _margin
                        spacing:            _margin

                        RowLayout {
                            Layout.fillWidth: true
                            spacing: _margin

                            Rectangle {
                                Layout.preferredWidth:  ScreenTools.defaultFontPixelHeight * 1.5
                                Layout.preferredHeight: ScreenTools.defaultFontPixelHeight * 1.5
                                radius:                 height / 2
                                color:                  qgcPal.button

                                QGCLabel {
                                    anchors.centerIn:   parent
                                    text:               "3"
                                    color:              qgcPal.buttonText
                                    font.bold:          true
                                }
                            }

                            QGCLabel {
                                Layout.fillWidth: true
                                Layout.minimumWidth: 0
                                wrapMode: Text.WordWrap
                                text:       qsTr("Output Folder (Optional)")
                                font.bold:  true
                            }
                        }

                        RowLayout {
                            Layout.fillWidth: true
                            spacing: _margin

                            QGCButton {
                                text:               qsTr("Browse...")
                                enabled:            !GeoTagController.inProgress
                                onClicked:          selectDestDir.openForLoad()

                                QGCFileDialog {
                                    id:             selectDestDir
                                    title:          qsTr("Select Output Folder")
                                    selectFolder:   true
                                    onAcceptedForLoad: (file) => {
                                        GeoTagController.saveDirectory = file
                                        close()
                                    }
                                }
                            }

                            QGCLabel {
                                objectName: "geoTag_outputPath"
                                Layout.fillWidth: true
                                Layout.minimumWidth: 0
                                wrapMode: Text.Wrap
                                text: {
                                    if (GeoTagController.saveDirectory) {
                                        return GeoTagController.saveDirectory
                                    } else if (GeoTagController.imageDirectory) {
                                        return GeoTagController.imageDirectory + "/TAGGED"
                                    }
                                    return qsTr("Default: /TAGGED subfolder")
                                }
                                opacity:            GeoTagController.saveDirectory ? 1.0 : 0.5
                            }
                        }
                    }
                }

                // Advanced Options
                Rectangle {
                    Layout.fillWidth:       true
                    Layout.preferredHeight: advancedColumn.height + _margin * 2
                    color:                  qgcPal.windowShade
                    radius:                 ScreenTools.defaultFontPixelWidth / 2

                    ColumnLayout {
                        id:                 advancedColumn
                        anchors.left:       parent.left
                        anchors.right:      parent.right
                        anchors.top:        parent.top
                        anchors.margins:    _margin
                        spacing:            _margin

                        QGCLabel {
                            text:       qsTr("Advanced Options")
                            font.bold:  true
                        }

                        GridLayout {
                            id: offsetLayout
                            Layout.fillWidth: true
                            Layout.minimumWidth: 0
                            columns: width >= ScreenTools.defaultFontPixelWidth * 65 ? 3 : 2
                            columnSpacing: _margin
                            rowSpacing: _margin / 2

                            QGCLabel {
                                Layout.fillWidth: true
                                Layout.minimumWidth: 0
                                wrapMode: Text.WordWrap
                                text: qsTr("Time Offset (seconds):")
                            }

                            QGCTextField {
                                id:                     timeOffsetField
                                objectName:             "geoTag_timeOffset"
                                Layout.minimumWidth:    0
                                Layout.preferredWidth:  ScreenTools.defaultFontPixelWidth * 10
                                text:                   GeoTagController.timeOffsetSecs.toFixed(1)
                                enabled:                !GeoTagController.inProgress
                                inputMethodHints:       Qt.ImhFormattedNumbersOnly
                                validator:              DoubleValidator { bottom: -3600; top: 3600; decimals: 1 }
                                onEditingFinished:      GeoTagController.timeOffsetSecs = parseFloat(text) || 0
                            }

                            QGCLabel {
                                objectName: "geoTag_offsetHelp"
                                Layout.fillWidth: true
                                Layout.minimumWidth: 0
                                Layout.columnSpan: offsetLayout.columns === 2 ? 2 : 1
                                wrapMode: Text.WordWrap
                                text:               qsTr("Adjust if camera clock differs from flight log")
                                opacity:            0.7
                                font.pointSize:     ScreenTools.smallFontPointSize
                            }
                        }

                        GridLayout {
                            Layout.fillWidth: true
                            Layout.minimumWidth: 0
                            columns: width >= ScreenTools.defaultFontPixelWidth * 80 ? 2 : 1
                            columnSpacing: _margin
                            rowSpacing: _margin / 2

                            QGCCheckBox {
                                id:         previewCheckbox
                                objectName: "geoTag_preview"
                                Layout.fillWidth: true
                                Layout.minimumWidth: 0
                                wrapMode: Text.WordWrap
                                text:       qsTr("Preview mode (don't write files)")
                                checked:    GeoTagController.previewMode
                                enabled:    !GeoTagController.inProgress
                                onClicked:  GeoTagController.previewMode = checked
                            }

                            QGCLabel {
                                objectName: "geoTag_previewHelp"
                                Layout.fillWidth: true
                                Layout.minimumWidth: 0
                                wrapMode: Text.WordWrap
                                text:               qsTr("Verify time offset before committing")
                                opacity:            0.7
                                font.pointSize:     ScreenTools.smallFontPointSize
                            }
                        }
                    }
                }

                // Action Button
                QGCButton {
                    objectName: "geoTag_start"
                    Layout.alignment: Qt.AlignHCenter
                    Layout.minimumWidth: 0
                    Layout.maximumWidth: geoTagContent.width
                    Layout.preferredWidth: ScreenTools.defaultFontPixelWidth * 20
                    wrapMode: Text.WordWrap
                    text: {
                        if (GeoTagController.inProgress) {
                            return qsTr("Cancel")
                        } else if (GeoTagController.previewMode) {
                            return qsTr("Preview")
                        } else {
                            return qsTr("Start Tagging")
                        }
                    }
                    enabled:                (GeoTagController.logFile && GeoTagController.imageDirectory) || GeoTagController.inProgress
                    onClicked: {
                        if (GeoTagController.inProgress) {
                            GeoTagController.cancelTagging()
                        } else {
                            GeoTagController.startTagging()
                        }
                    }
                }

                // Image List
                Rectangle {
                    objectName: "geoTag_endContent"
                    Layout.fillWidth: true
                    Layout.minimumWidth: 0
                    Layout.preferredHeight: imageHeader.implicitHeight + ScreenTools.defaultFontPixelHeight * 10 + _margin * 3
                    color:                  qgcPal.windowShade
                    radius:                 ScreenTools.defaultFontPixelWidth / 2
                    visible:                GeoTagController.imageModel.count > 0

                    ColumnLayout {
                        anchors.fill:       parent
                        anchors.margins:    _margin
                        spacing:            _margin / 2

                        ColumnLayout {
                            id: imageHeader
                            Layout.fillWidth: true
                            spacing: _margin

                            QGCLabel {
                                Layout.fillWidth: true
                                Layout.minimumWidth: 0
                                wrapMode: Text.WordWrap
                                text:       qsTr("Images (%1)").arg(GeoTagController.imageModel.count)
                                font.bold:  true
                            }

                            // Legend
                            Flow {
                                Layout.fillWidth: true
                                Layout.minimumWidth: 0
                                spacing: _margin

                                Row {
                                    spacing: _margin / 4
                                    Rectangle { width: 10; height: 10; radius: 2; color: qgcPal.text; opacity: 0.5 }
                                    QGCLabel { text: qsTr("Pending"); font.pointSize: ScreenTools.smallFontPointSize }
                                }
                                Row {
                                    spacing: _margin / 4
                                    Rectangle { width: 10; height: 10; radius: 2; color: qgcPal.colorBlue }
                                    QGCLabel { text: qsTr("Processing"); font.pointSize: ScreenTools.smallFontPointSize }
                                }
                                Row {
                                    spacing: _margin / 4
                                    Rectangle { width: 10; height: 10; radius: 2; color: qgcPal.colorGreen }
                                    QGCLabel { text: qsTr("Tagged"); font.pointSize: ScreenTools.smallFontPointSize }
                                }
                                Row {
                                    spacing: _margin / 4
                                    Rectangle { width: 10; height: 10; radius: 2; color: qgcPal.colorOrange }
                                    QGCLabel { text: qsTr("Skipped"); font.pointSize: ScreenTools.smallFontPointSize }
                                }
                                Row {
                                    spacing: _margin / 4
                                    Rectangle { width: 10; height: 10; radius: 2; color: qgcPal.colorRed }
                                    QGCLabel { text: qsTr("Failed"); font.pointSize: ScreenTools.smallFontPointSize }
                                }
                            }
                        }

                        Rectangle {
                            Layout.fillWidth:   true
                            height:             1
                            color:              qgcPal.text
                            opacity:            0.2
                        }

                        QGCListView {
                            id:                 imageListView
                            Layout.fillWidth:   true
                            Layout.fillHeight:  true
                            model:              GeoTagController.imageModel

                            delegate: Rectangle {
                                width:      imageListView.width
                                height:     Math.max(ScreenTools.defaultFontPixelHeight * 2, imageRow.implicitHeight + _margin)
                                color:      index % 2 === 0 ? "transparent" : qgcPal.window
                                radius:     ScreenTools.defaultFontPixelWidth / 4

                                GridLayout {
                                    id: imageRow
                                    anchors.left: parent.left
                                    anchors.right: parent.right
                                    anchors.top: parent.top
                                    anchors.margins: _margin / 2
                                    columns: width >= ScreenTools.defaultFontPixelWidth * 70 ? 4 : 2
                                    columnSpacing: _margin
                                    rowSpacing: _margin / 2

                                    // Status indicator
                                    Rectangle {
                                        Layout.preferredWidth:  ScreenTools.defaultFontPixelHeight * 0.8
                                        Layout.preferredHeight: ScreenTools.defaultFontPixelHeight * 0.8
                                        radius:                 ScreenTools.defaultFontPixelWidth / 4
                                        color: {
                                            switch (model.status) {
                                            case 0: return qgcPal.text    // Pending
                                            case 1: return qgcPal.colorBlue   // Processing
                                            case 2: return qgcPal.colorGreen  // Tagged
                                            case 3: return qgcPal.colorOrange // Skipped
                                            case 4: return qgcPal.colorRed    // Failed
                                            default: return qgcPal.text
                                            }
                                        }
                                        opacity: model.status === 0 ? 0.5 : 1.0

                                        // Processing animation
                                        SequentialAnimation on opacity {
                                            running: model.status === 1
                                            loops: Animation.Infinite
                                            NumberAnimation { to: 0.3; duration: 500 }
                                            NumberAnimation { to: 1.0; duration: 500 }
                                        }
                                    }

                                    // File name
                                    QGCLabel {
                                        Layout.fillWidth: true
                                        Layout.minimumWidth: 0
                                        wrapMode: Text.Wrap
                                        text:               model.fileName
                                    }

                                    // Coordinate (if tagged)
                                    QGCLabel {
                                        Layout.fillWidth: true
                                        Layout.minimumWidth: 0
                                        Layout.columnSpan: imageRow.columns === 2 ? 2 : 1
                                        wrapMode: Text.Wrap
                                        text: {
                                            if (model.coordinate && model.coordinate.isValid) {
                                                return model.coordinate.latitude.toFixed(6) + ", " + model.coordinate.longitude.toFixed(6)
                                            }
                                            return ""
                                        }
                                        font.pointSize:     ScreenTools.smallFontPointSize
                                        opacity:            0.7
                                        visible:            model.status === 2
                                    }

                                    // Status text or error
                                    QGCLabel {
                                        Layout.fillWidth: true
                                        Layout.minimumWidth: 0
                                        Layout.columnSpan: imageRow.columns === 2 ? 2 : 1
                                        wrapMode: Text.WordWrap
                                        text:               model.errorMessage ? model.errorMessage : model.statusString
                                        font.pointSize:     ScreenTools.smallFontPointSize
                                        color:              model.status === 4 ? qgcPal.colorRed : (model.status === 3 ? qgcPal.colorOrange : qgcPal.text)
                                        horizontalAlignment: Text.AlignRight
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}
