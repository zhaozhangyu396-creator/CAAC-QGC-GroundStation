pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import QGroundControl
import QGroundControl.Controls

QGCPopupDialog {
    id: root
    objectName: "caacFigureEightDialog"
    title: qsTr("八字飞行 · %1").arg(root.pageNames[Math.max(0, pages.currentIndex)])
    buttons: Dialog.Close
    closeOnOutsideClick: true

    property int initialPage: 0
    readonly property var pageNames: [qsTr("场地列表"), qsTr("考试偏好"), qsTr("标准设置"), qsTr("训练监测"), qsTr("电子围栏")]

    readonly property var controller: QGroundControl.corePlugin.figureEight
    readonly property var fence: controller.fence
    readonly property var _activeVehicle: QGroundControl.multiVehicleManager.activeVehicle
    readonly property var appSettings: QGroundControl.settingsManager.appSettings
    property var limitsDraft: ({})
    property string _loadedLimits: ""
    property int _loadedStandard: -1
    readonly property var categories: [qsTr("视距内"), qsTr("超视距"), qsTr("教员"), qsTr("自定义")]
    readonly property var limitFields: [
        {key: "horizontal", label: qsTr("最大水平偏差（米）")},
        {key: "vertical", label: qsTr("最大高度偏差（米）")},
        {key: "heading", label: qsTr("最大航向偏差（度）")},
        {key: "maxDuration", label: qsTr("训练最长时间（秒）")},
        {key: "minSpeed", label: qsTr("训练最低速度（米/秒）")},
        {key: "maxSpeed", label: qsTr("训练最高速度（米/秒）")}
    ]

    function number(text) { return text.trim().length ? Number(text) : NaN }

    function refreshLimits(force) {
        const signature = JSON.stringify(controller.limits)
        if (force || signature !== _loadedLimits || _loadedStandard !== controller.standard) {
            limitsDraft = Object.assign({}, controller.limits)
            _loadedLimits = signature
            _loadedStandard = controller.standard
        }
    }

    function editLimit(key, value) {
        const updated = Object.assign({}, limitsDraft)
        updated[key] = value
        limitsDraft = updated
    }

    function metric(key, decimals, suffix) {
        if (!controller.metrics.telemetryValid && !controller.recordedPath.length
                && ["speed", "altitudeError", "horizontalError", "headingError"].indexOf(key) >= 0) {
            return "--"
        }
        const value = controller.metrics[key]
        return typeof value === "number" && isFinite(value) ? value.toFixed(decimals) + suffix : "--"
    }

    QGCPalette { id: palette; colorGroupEnabled: true }

    ColumnLayout {
        width: Math.min(root.maxContentAvailableWidth, ScreenTools.defaultFontPixelWidth * 92)
        spacing: ScreenTools.defaultFontPixelHeight * 0.5

        RowLayout {
            Layout.fillWidth: true
            QGCLabel {
                Layout.fillWidth: true
                wrapMode: Text.WordWrap
                text: root.categories[root.controller.standard]
                    + (root.controller.standard === 2 ? qsTr(" · 后退八字") : qsTr(" · 前进八字"))
            }
            QGCLabel {
                objectName: "caacFigureEightSessionState"
                text: root.controller.phase
                color: root.controller.running ? palette.colorGreen : palette.text
            }
        }

        CaacFigureEightIllustration {
            objectName: "caacFigureEightPreview"
            Layout.alignment: Qt.AlignHCenter
            Layout.preferredWidth: Math.min(root.maxContentAvailableWidth, ScreenTools.defaultFontPixelHeight * 24)
            Layout.preferredHeight: Layout.preferredWidth * 327 / 480
        }

        QGCComboBox {
            id: pages
            objectName: "caacFigureEightPage"
            Layout.fillWidth: true
            model: root.pageNames
            currentIndex: Math.max(0, Math.min(root.initialPage, root.pageNames.length - 1))
        }

        StackLayout {
            Layout.fillWidth: true
            currentIndex: pages.currentIndex
            implicitHeight: currentIndex >= 0 && currentIndex < root.pageNames.length
                ? [sitePage, preferencesPage, standardsPage, monitorPage, fencePage][currentIndex].implicitHeight : 0

            ColumnLayout {
                id: sitePage
                spacing: ScreenTools.defaultFontPixelHeight * 0.4
                enabled: !root.controller.running
                QGCComboBox {
                    objectName: "caacFigureEightSites"
                    Layout.fillWidth: true
                    model: [qsTr("临时场地")].concat(root.controller.sites.map(site => site.name))
                    currentIndex: root.controller.selectedSite + 1
                    onActivated: (index) => root.controller.selectedSite = index - 1
                }
                QGCLabel {
                    Layout.fillWidth: true
                    wrapMode: Text.WordWrap
                    text: qsTr("将具备 RTK 定位的无人机放到 A 筒位置，点击标定 A 点；再移至 B 筒位置，点击标定 B 点。")
                }
                QGCLabel {
                    objectName: "caacFigureEightCalibrationStatus"
                    Layout.fillWidth: true
                    visible: !!root._activeVehicle && root._activeVehicle.initialConnectComplete
                    wrapMode: Text.WordWrap
                    text: root.controller.calibrationStatus
                    color: root.controller.calibrationReady ? palette.colorGreen : palette.text
                }
                RowLayout {
                    Layout.fillWidth: true
                    spacing: ScreenTools.defaultFontPixelWidth
                    Repeater {
                        model: ["A", "B"]
                        delegate: Rectangle {
                            id: calibrationCard
                            required property string modelData
                            readonly property bool captured: modelData === "A"
                                ? root.controller.pointA.isValid : root.controller.pointB.isValid
                            Layout.fillWidth: true
                            implicitHeight: calibrationContent.implicitHeight + ScreenTools.defaultFontPixelHeight
                            color: palette.windowShade
                            radius: ScreenTools.defaultBorderRadius
                            ColumnLayout {
                                id: calibrationContent
                                anchors.fill: parent
                                anchors.margins: ScreenTools.defaultFontPixelHeight * 0.5
                                QGCLabel {
                                    Layout.fillWidth: true
                                    text: calibrationCard.modelData === "A" ? qsTr("A 筒 · %1").arg(calibrationCard.captured ? qsTr("已标定") : qsTr("未标定"))
                                        : qsTr("B 筒 · %1").arg(calibrationCard.captured ? qsTr("已标定") : qsTr("未标定"))
                                    font.bold: true
                                }
                                QGCButton {
                                    objectName: "caacFigureEightCapture" + calibrationCard.modelData
                                    Layout.fillWidth: true
                                    text: calibrationCard.modelData === "A" ? qsTr("标定 A 点") : qsTr("标定 B 点")
                                    enabled: root.controller.calibrationReady
                                    onClicked: {
                                        if (calibrationCard.modelData === "A") { root.controller.capturePointA() }
                                        else { root.controller.capturePointB() }
                                    }
                                }
                            }
                        }
                    }
                }
                QGCLabel {
                    objectName: "caacFigureEightGeneratedSite"
                    Layout.fillWidth: true
                    wrapMode: Text.WordWrap
                    text: root.controller.referencePath.length
                        ? qsTr("已自动在地图上生成八字训练轨迹。A/B 间距 %1 米 · 单圆直径 %2 米。")
                            .arg((QGroundControl.corePlugin.routeLegMetrics(root.controller.pointA, root.controller.pointB).distance).toFixed(2))
                            .arg((QGroundControl.corePlugin.routeLegMetrics(root.controller.pointA, root.controller.pointB).distance / 2).toFixed(2))
                        : qsTr("两点标定完成后，自动按实际间距和方位在地图上生成八字训练轨迹。")
                }
                QGCLabel {
                    Layout.fillWidth: true
                    wrapMode: Text.WordWrap
                    text: qsTr("小型、中型多旋翼标准单圆直径为 12 米；按图示双圆布局，A/B 外侧端点间距为 24 米。")
                }
                RowLayout {
                    Layout.fillWidth: true
                    QGCTextField { id: siteName; Layout.fillWidth: true; placeholderText: qsTr("场地名称") }
                    QGCButton {
                        objectName: "caacFigureEightSaveSite"
                        text: qsTr("保存场地")
                        enabled: root.controller.referencePath.length > 0 && siteName.text.trim().length > 0
                        onClicked: root.controller.saveSite(siteName.text, root.controller.pointA, root.controller.pointB)
                    }
                }
                RowLayout {
                    Layout.fillWidth: true
                    QGCButton { text: qsTr("导入场地"); onClicked: importDialog.openForLoad() }
                    QGCButton { text: qsTr("导出场地"); onClicked: exportDialog.openForSave() }
                    QGCButton {
                        text: qsTr("删除所选场地")
                        enabled: root.controller.selectedSite >= 0
                        onClicked: root.controller.removeSite(root.controller.selectedSite)
                    }
                }
            }

            ColumnLayout {
                id: preferencesPage
                spacing: ScreenTools.defaultFontPixelHeight * 0.5
                QGCLabel { text: qsTr("考试类别") }
                QGCComboBox {
                    objectName: "caacFigureEightStandard"
                    Layout.fillWidth: true
                    enabled: !root.controller.running
                    model: root.categories
                    currentIndex: root.controller.standard
                    onActivated: (index) => root.controller.standard = index
                }
                QGCLabel { text: qsTr("考试科目") }
                QGCComboBox {
                    objectName: "caacFigureEightExerciseMode"
                    Layout.fillWidth: true
                    enabled: !root.controller.running
                    model: [qsTr("单项八字飞行"), qsTr("单项原地自旋"), qsTr("原地自旋 → 八字飞行")]
                    currentIndex: root.controller.exerciseMode
                    onActivated: (index) => root.controller.exerciseMode = index
                }
                QGCLabel {
                    Layout.fillWidth: true
                    wrapMode: Text.WordWrap
                    text: root.controller.standard === 0
                        ? qsTr("视距内：前进水平八字，可使用姿态和位置修正辅助。")
                        : root.controller.standard === 1
                        ? qsTr("超视距：姿态辅助模式下前进水平八字，不使用位置修正辅助，速度无明显变化。")
                        : root.controller.standard === 2
                        ? qsTr("教员：后退水平八字，航向按运动切线反向评估，速度基本无变化。")
                        : qsTr("自定义：根据训练需要调整偏差和提醒阈值。")
                }
                QGCLabel {
                    Layout.fillWidth: true
                    wrapMode: Text.WordWrap
                    text: qsTr("默认适用于小型、中型多旋翼，一级风（0–4 米/秒）。切换类别会保留各类别自己的设置。")
                }
                QGCLabel {
                    Layout.fillWidth: true
                    wrapMode: Text.WordWrap
                    text: qsTr("原地自旋每圈 5–30 秒。教员类别先顺时针、再逆时针，随后进入后退八字。")
                }
                QGCLabel { text: qsTr("地面风速来源") }
                QGCComboBox {
                    objectName: "caacFigureEightWindSource"
                    Layout.fillWidth: true
                    enabled: !root.controller.running
                    model: [qsTr("手动输入"), qsTr("地面风速传感器")]
                    currentIndex: root.controller.windSource
                    onActivated: (index) => root.controller.windSource = index
                }
                RowLayout {
                    Layout.fillWidth: true
                    visible: root.controller.windSource === 0
                    QGCLabel { Layout.fillWidth: true; text: qsTr("手动地面风速（米/秒）") }
                    QGCTextField {
                        objectName: "caacFigureEightManualWind"
                        Layout.preferredWidth: ScreenTools.defaultFontPixelWidth * 12
                        text: String(root.controller.manualWindSpeed)
                        validator: DoubleValidator { bottom: 0; top: 100; locale: "C" }
                        onEditingFinished: {
                            const value = root.number(text)
                            if (isFinite(value) && value >= 0 && value <= 100) { root.controller.manualWindSpeed = value }
                            text = Qt.binding(() => String(root.controller.manualWindSpeed))
                        }
                    }
                }
                QGCLabel {
                    objectName: "caacFigureEightWindState"
                    Layout.fillWidth: true
                    wrapMode: Text.WordWrap
                    text: root.controller.windValid
                        ? qsTr("当前风速 %1 米/秒 · 第 %2 级 · 误差容差 ×%3")
                            .arg(root.controller.windSpeed.toFixed(2)).arg(root.controller.windLevel)
                            .arg(root.controller.windFactor.toFixed(4))
                        : qsTr("等待有效的地面风速传感器数据；当前不可开始考评。")
                    color: root.controller.windValid ? palette.text : palette.warningText
                }
                QGCLabel {
                    objectName: "caacFigureEightEffectiveLimits"
                    Layout.fillWidth: true
                    wrapMode: Text.WordWrap
                    text: root.controller.windValid
                        ? qsTr("当前考评容差：水平 ±%1 米，高度 ±%2 米，航向 %3°。")
                            .arg(Number(root.controller.effectiveLimits.horizontal).toFixed(3))
                            .arg(Number(root.controller.effectiveLimits.vertical).toFixed(3))
                            .arg(Number(root.controller.effectiveLimits.heading).toFixed(3)) : ""
                }
                QGCLabel {
                    Layout.fillWidth: true
                    wrapMode: Text.WordWrap
                    text: qsTr("风等级每升一级，允许误差比前一级增加 15%。本软件按 ≤4、>4 且 ≤8、>8 米/秒分级，交界值采用较严格一级。")
                }
                QGCLabel {
                    Layout.fillWidth: true
                    wrapMode: Text.WordWrap
                    visible: root.controller.windSource === 1
                    text: qsTr("传感器接入接口使用米/秒。超过 10 秒无有效数据会结束考评记录；历史读数不作为新考评的风速。")
                }
            }

            ColumnLayout {
                id: standardsPage
                enabled: !root.controller.running
                spacing: ScreenTools.defaultFontPixelHeight * 0.4
                GridLayout {
                    columns: 2
                    Layout.fillWidth: true
                    Repeater {
                        model: root.limitFields
                        delegate: RowLayout {
                            id: fieldRow
                            required property var modelData
                            Layout.columnSpan: 2
                            Layout.fillWidth: true
                            QGCLabel { Layout.fillWidth: true; text: fieldRow.modelData.label; wrapMode: Text.WordWrap }
                            QGCTextField {
                                objectName: "caacFigureEightLimit_" + fieldRow.modelData.key
                                Layout.preferredWidth: ScreenTools.defaultFontPixelWidth * 12
                                text: root.limitsDraft[fieldRow.modelData.key] === undefined ? ""
                                    : String(root.limitsDraft[fieldRow.modelData.key])
                                validator: DoubleValidator { bottom: 0; locale: "C" }
                                onTextEdited: root.editLimit(fieldRow.modelData.key, root.number(text))
                            }
                        }
                    }
                }
                RowLayout {
                    Layout.fillWidth: true
                    QGCLabel { Layout.fillWidth: true; text: qsTr("地图训练走廊半宽（米）") }
                    QGCTextField {
                        objectName: "caacFigureEightCorridorWidth"
                        Layout.preferredWidth: ScreenTools.defaultFontPixelWidth * 12
                        enabled: !root.controller.running
                        text: String(root.controller.fenceWidth)
                        validator: DoubleValidator { bottom: 0.1; top: 20; locale: "C" }
                        onEditingFinished: {
                            const value = root.number(text)
                            if (isFinite(value) && value >= 0.1 && value <= 20) { root.controller.fenceWidth = value }
                            text = Qt.binding(() => String(root.controller.fenceWidth))
                        }
                    }
                }
                QGCCheckBox {
                    objectName: "caacFigureEightShowOnMap"
                    text: qsTr("在地图上显示八字航迹和训练轨迹")
                    checked: root.controller.showOnMap
                    onClicked: root.controller.showOnMap = checked
                }
                QGCCheckBox {
                    objectName: "caacFigureEightDurationReminder"
                    text: qsTr("启用训练时间提醒")
                    checked: !!root.limitsDraft.durationEnabled
                    onClicked: root.editLimit("durationEnabled", checked)
                }
                QGCCheckBox {
                    objectName: "caacFigureEightSpeedReminder"
                    text: qsTr("启用训练速度提醒")
                    checked: !!root.limitsDraft.speedEnabled
                    onClicked: root.editLimit("speedEnabled", checked)
                }
                QGCLabel {
                    Layout.fillWidth: true
                    wrapMode: Text.WordWrap
                    text: qsTr("180 秒、0.3–3 米/秒来自参考视频，仅作训练提醒，默认关闭；不是已核实的 CAAC 统一判定限值。")
                }
                RowLayout {
                    QGCButton {
                        objectName: "caacFigureEightSaveLimits"
                        text: qsTr("保存当前类别标准")
                        onClicked: {
                            if (root.controller.setLimits(root.limitsDraft)) { root.refreshLimits(true) }
                        }
                    }
                    QGCButton {
                        objectName: "caacFigureEightResetLimits"
                        text: qsTr("恢复默认标准")
                        onClicked: { root.controller.resetLimits(); root.refreshLimits(true) }
                    }
                }
            }

            ColumnLayout {
                id: monitorPage
                spacing: ScreenTools.defaultFontPixelHeight * 0.5
                QGCLabel {
                    objectName: "caacFigureEightStartError"
                    Layout.fillWidth: true
                    visible: root.controller.lastError.length > 0
                    text: root.controller.lastError
                    color: palette.warningText
                    wrapMode: Text.WordWrap
                }
                GridLayout {
                    columns: 2
                    Layout.fillWidth: true
                    Repeater {
                        model: [
                            {label: qsTr("记录时间"), key: "elapsed", decimals: 1, suffix: qsTr(" 秒")},
                            {label: qsTr("测得速度"), key: "speed", decimals: 2, suffix: qsTr(" 米/秒")},
                            {label: qsTr("水平偏差"), key: "horizontalError", decimals: 2, suffix: qsTr(" 米")},
                            {label: qsTr("高度偏差"), key: "altitudeError", decimals: 2, suffix: qsTr(" 米")},
                            {label: qsTr("航向偏差"), key: "headingError", decimals: 1, suffix: "°"},
                            {label: qsTr("完成圈数"), key: "completedLaps", decimals: 0, suffix: ""},
                            {label: qsTr("自旋角度"), key: "spinAngle", decimals: 1, suffix: "°"},
                            {label: qsTr("超差采样数"), key: "violationSamples", decimals: 0, suffix: ""}
                        ]
                        delegate: RowLayout {
                            id: metricRow
                            required property var modelData
                            Layout.columnSpan: 2
                            Layout.fillWidth: true
                            QGCLabel { Layout.fillWidth: true; text: metricRow.modelData.label }
                            QGCLabel {
                                objectName: "caacFigureEightMetric_" + metricRow.modelData.key
                                text: root.metric(metricRow.modelData.key, metricRow.modelData.decimals, metricRow.modelData.suffix)
                            }
                        }
                    }
                }
                RowLayout {
                    Layout.fillWidth: true
                    QGCButton {
                        objectName: "caacFigureEightStart"
                        text: qsTr("开始考试")
                        enabled: !root.controller.running
                        onClicked: root.controller.start()
                    }
                    QGCButton {
                        objectName: "caacFigureEightStop"
                        text: qsTr("结束考试")
                        enabled: root.controller.running
                        onClicked: root.controller.stop()
                    }
                    QGCButton {
                        objectName: "caacFigureEightClear"
                        text: qsTr("清空记录")
                        enabled: !root.controller.running
                        onClicked: root.controller.clearRecording()
                    }
                }
                QGCLabel {
                    Layout.fillWidth: true
                    wrapMode: Text.WordWrap
                    text: qsTr("飞机起飞并获得有效遥测后开始记录。红线为参考航迹，浅绿色为训练走廊，橙线为实际轨迹；关闭窗口后继续记录。")
                }
            }

            ColumnLayout {
                id: fencePage
                spacing: ScreenTools.defaultFontPixelHeight * 0.5
                QGCLabel {
                    Layout.fillWidth: true
                    wrapMode: Text.WordWrap
                    text: qsTr("沿双圆走廊的外边界向外扩展，生成包围整个训练场的电子围栏。")
                }
                RowLayout {
                    Layout.fillWidth: true
                    QGCLabel { Layout.fillWidth: true; text: qsTr("走廊外边界至围栏的距离（米）") }
                    QGCTextField {
                        objectName: "caacFigureEightFenceMargin"
                        Layout.preferredWidth: ScreenTools.defaultFontPixelWidth * 12
                        enabled: !root.fence.busy && !root.controller.running
                        text: String(root.fence.margin)
                        validator: DoubleValidator { bottom: 0; top: 100; locale: "C" }
                        onEditingFinished: {
                            const value = root.number(text)
                            if (isFinite(value) && value >= 0 && value <= 100) { root.fence.margin = value }
                            text = Qt.binding(() => String(root.fence.margin))
                        }
                    }
                }
                QGCCheckBox {
                    objectName: "caacFigureEightFenceShowOnMap"
                    text: qsTr("在地图上显示电子围栏")
                    checked: root.fence.showOnMap
                    onClicked: root.fence.showOnMap = checked
                }
                RowLayout {
                    Layout.fillWidth: true
                    QGCButton {
                        objectName: "caacFigureEightFenceApply"
                        text: qsTr("写入并启用")
                        enabled: root.fence.canApply
                        onClicked: root.fence.apply(false)
                    }
                    QGCButton {
                        objectName: "caacFigureEightFenceRead"
                        text: qsTr("读取飞控状态")
                        enabled: root.fence.canRead
                        onClicked: root.fence.readFromVehicle()
                    }
                }
                QGCLabel {
                    objectName: "caacFigureEightFenceStatus"
                    Layout.fillWidth: true
                    wrapMode: Text.WordWrap
                    text: root.fence.status
                    color: root.fence.applied ? palette.colorGreen : palette.text
                }
                QGCLabel {
                    Layout.fillWidth: true
                    wrapMode: Text.WordWrap
                    visible: root.fence.actionDescription.length > 0
                    text: root.fence.actionDescription
                }
                QGCButton {
                    objectName: "caacFigureEightFenceReplace"
                    text: qsTr("备份并替换当前围栏")
                    visible: root.fence.replacementRequired
                    enabled: root.fence.canApply
                    onClicked: root.fence.apply(true)
                }
                QGCLabel {
                    Layout.fillWidth: true
                    wrapMode: Text.WordWrap
                    visible: root.fence.backupFile.length > 0
                    text: qsTr("原围栏备份：%1").arg(root.fence.backupFile)
                    font.pointSize: ScreenTools.smallFontPointSize
                }
            }

        }

        QGCCheckBox {
            objectName: "caacFigureEightVoiceEnabled"
            text: root.appSettings.audioMuted.rawValue
                ? qsTr("启用考试语音（取消全局静音）") : qsTr("启用考试语音提示")
            checked: root.controller.voiceEnabled && !root.appSettings.audioMuted.rawValue
            onClicked: {
                root.controller.voiceEnabled = checked
                if (checked) { root.appSettings.audioMuted.rawValue = false }
            }
        }
        QGCButton {
            objectName: "caacFigureEightTestVoice"
            text: qsTr("试听考试语音")
            enabled: root.controller.voiceEnabled && !root.appSettings.audioMuted.rawValue
            onClicked: root.controller.testVoice()
        }
        QGCLabel {
            objectName: "caacFigureEightAnnouncement"
            Layout.fillWidth: true
            wrapMode: Text.WordWrap
            visible: root.controller.lastAnnouncement.length > 0
            text: qsTr("最近提示：%1").arg(root.controller.lastAnnouncement)
            font.bold: true
        }
        QGCLabel {
            objectName: "caacFigureEightStatus"
            Layout.fillWidth: true
            wrapMode: Text.WordWrap
            text: root.controller.lastError || root.controller.status
            color: root.controller.lastError ? palette.warningText : palette.text
        }
    }

    QGCFileDialog {
        id: importDialog
        title: qsTr("导入八字飞行场地")
        nameFilters: [qsTr("八字场地 (*.json)")]
        onAcceptedForLoad: (file) => root.controller.importSites(file)
    }
    QGCFileDialog {
        id: exportDialog
        title: qsTr("导出八字飞行场地")
        nameFilters: [qsTr("八字场地 (*.json)")]
        defaultSuffix: "json"
        onAcceptedForSave: (file) => root.controller.exportSites(file)
    }
    Connections {
        target: root.controller
        function onSettingsChanged() { root.refreshLimits() }
    }
    Component.onCompleted: refreshLimits()
}
