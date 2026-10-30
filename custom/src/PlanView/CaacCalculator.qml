pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import QGroundControl
import QGroundControl.Controls
import "CaacCalculatorMath.js" as Calculator

QGCPopupDialog {
    id: root
    objectName: "caacCalculatorDialog"
    title: qsTr("Angle calculator")
    buttons: Dialog.Close
    bypassNavigationCheck: true
    property var windResult: null
    property var timeResult: null
    property var trigResult: null
    property var triangleResult: null
    property var knownSides: ({})
    property bool fillingTriangle: false
    readonly property real cardSpacing: ScreenTools.defaultFontPixelWidth
    readonly property color resultColor: calculatorPalette.globalTheme === QGCPalette.Light
        ? Qt.darker(calculatorPalette.colorGreen, 1.4) : calculatorPalette.colorGreen
    readonly property color errorColor: calculatorPalette.globalTheme === QGCPalette.Light
        ? Qt.darker(calculatorPalette.warningText, 1.25) : Qt.lighter(calculatorPalette.warningText, 1.3)
    readonly property color angleColor: calculatorPalette.globalTheme === QGCPalette.Light
        ? Qt.darker(calculatorPalette.colorYellow, 1.8) : calculatorPalette.colorYellow
    QGCPalette { id: calculatorPalette; colorGroupEnabled: root.enabled }
    readonly property var windDirections: [
        {label: qsTr("Select wind direction"), angle: 0},
        {label: qsTr("NNE · 22.5°"), angle: 22.5}, {label: qsTr("NE · 45°"), angle: 45},
        {label: qsTr("ENE · 67.5°"), angle: 67.5}, {label: qsTr("E · 90°"), angle: 90},
        {label: qsTr("ESE · 112.5°"), angle: 112.5}, {label: qsTr("SE · 135°"), angle: 135},
        {label: qsTr("SSE · 157.5°"), angle: 157.5}, {label: qsTr("S · 180°"), angle: 180},
        {label: qsTr("SSW · 202.5°"), angle: 202.5}, {label: qsTr("SW · 225°"), angle: 225},
        {label: qsTr("WSW · 247.5°"), angle: 247.5}, {label: qsTr("W · 270°"), angle: 270},
        {label: qsTr("WNW · 292.5°"), angle: 292.5}, {label: qsTr("NW · 315°"), angle: 315},
        {label: qsTr("NNW · 337.5°"), angle: 337.5}, {label: qsTr("N · 360°"), angle: 360}
    ]
    readonly property var relativeWind: [
        {label: qsTr("Wind from front right"), angle: 45},
        {label: qsTr("Wind from front"), angle: 0},
        {label: qsTr("Wind from right"), angle: 90},
        {label: qsTr("Wind from rear right"), angle: 135},
        {label: qsTr("Wind from rear"), angle: 180},
        {label: qsTr("Wind from rear left"), angle: 225},
        {label: qsTr("Wind from left"), angle: 270},
        {label: qsTr("Wind from front left"), angle: 315}
    ]
    function formatted(value) { return Number(value.toPrecision(12)).toString() }
    function errorText(result) {
        if (!result || result.ok) { return "" }
        switch (result.error) {
        case "wind": return qsTr("Enter a wind direction from 0° to 360°.")
        case "time": return qsTr("Enter whole hours from 0 to 23 and minutes from 0 to 59.")
        case "domain": return qsTr("The input for arcsin and arccos must be between -1 and 1.")
        case "tangent": return qsTr("The tangent is undefined at this angle.")
        case "known": return qsTr("Enter exactly two sides, or one side and an angle.")
        case "angle": return qsTr("The acute angle must be greater than 0° and less than 90°.")
        case "hypotenuse": return qsTr("The hypotenuse must be longer than either leg.")
        case "sides": return qsTr("Enter finite, positive side lengths.")
        default: return qsTr("Enter a finite number.")
        }
    }
    function sideEdited(key, text) {
        if (fillingTriangle) { return }
        const known = Object.assign({}, knownSides)
        if (text.trim().length) { known[key] = Calculator.parsed(text) }
        else { delete known[key] }
        knownSides = known
        triangleResult = null
    }
    function solveTriangle() {
        triangleResult = Calculator.triangle(knownSides)
        if (!triangleResult.ok) { return }
        fillingTriangle = true
        hypotenuse.text = formatted(triangleResult.c)
        verticalSide.text = formatted(triangleResult.a)
        horizontalSide.text = formatted(triangleResult.b)
        theta.text = formatted(triangleResult.theta)
        fillingTriangle = false
    }
    function clearTriangle() {
        fillingTriangle = true
        for (const field of [hypotenuse, verticalSide, horizontalSide, theta]) { field.text = "" }
        knownSides = ({})
        triangleResult = null
        fillingTriangle = false
    }
    component Card: Rectangle {
        id: card
        property string heading
        default property alias fields: contents.data
        Layout.fillWidth: true
        Layout.minimumWidth: 0
        Layout.alignment: Qt.AlignTop
        implicitHeight: contents.implicitHeight + root.cardSpacing * 2
        color: calculatorPalette.windowShade
        radius: ScreenTools.defaultBorderRadius
        border.width: 1
        border.color: calculatorPalette.groupBorder
        ColumnLayout {
            id: contents
            x: root.cardSpacing
            y: root.cardSpacing
            width: parent.width - root.cardSpacing * 2
            spacing: root.cardSpacing
            QGCLabel { text: card.heading; font.bold: true; Layout.fillWidth: true; wrapMode: Text.WordWrap }
        }
    }
    component NumberField: QGCTextField {
        Layout.fillWidth: true
        Layout.minimumWidth: 0
        rightPadding: leftPadding
        validator: DoubleValidator { locale: "C" }
    }
    component CalculateButton: QGCButton {
        primary: true
        text: qsTr("Calculate")
    }
    component ResultRow: RowLayout {
        id: resultRow
        property string resultText: ""
        property bool valid: false
        Layout.fillWidth: true
        visible: resultText.length > 0
        QGCLabel {
            Layout.fillWidth: true
            Layout.minimumWidth: 0
            wrapMode: Text.WrapAnywhere
            text: resultRow.resultText
            color: resultRow.valid ? root.resultColor : root.errorColor
        }
        QGCButton {
            objectName: resultRow.objectName + "Copy"
            text: qsTr("Copy")
            visible: resultRow.valid
            heightFactor: 0.15
            onClicked: QGroundControl.copyToClipboard(resultRow.resultText)
        }
    }
    ColumnLayout {
        width: Math.min(root.maxContentAvailableWidth, ScreenTools.defaultFontPixelWidth * 115)
        spacing: root.cardSpacing
        GridLayout {
            id: upperCards
            Layout.fillWidth: true
            columns: width >= ScreenTools.defaultFontPixelWidth * 85 ? 3 : 1
            columnSpacing: root.cardSpacing
            rowSpacing: root.cardSpacing
            Card {
                heading: qsTr("Wind direction to heading")
                QGCLabel { text: qsTr("Wind direction (degrees)"); Layout.fillWidth: true; wrapMode: Text.WordWrap }
                QGCComboBox {
                    id: windPreset
                    objectName: "caacCalculatorWindPreset"
                    Layout.fillWidth: true
                    model: root.windDirections
                    textRole: "label"
                    onActivated: {
                        wind.text = root.windDirections[currentIndex].angle.toString()
                        root.windResult = null
                    }
                }
                NumberField {
                    id: wind
                    objectName: "caacCalculatorWindValue"
                    text: "0"
                    onTextEdited: { windPreset.currentIndex = 0; root.windResult = null }
                }
                QGCLabel { text: qsTr("Relative wind direction") }
                QGCComboBox {
                    id: relative
                    objectName: "caacCalculatorRelativeWind"
                    Layout.fillWidth: true
                    model: root.relativeWind
                    textRole: "label"
                    onActivated: root.windResult = null
                }
                CalculateButton {
                    objectName: "caacCalculatorWindCalculate"
                    onClicked: root.windResult = Calculator.windHeading(Calculator.parsed(wind.text), root.relativeWind[relative.currentIndex].angle)
                }
                QGCLabel { text: qsTr("True north is 0°; angles increase clockwise."); Layout.fillWidth: true; wrapMode: Text.WordWrap }
                ResultRow {
                    objectName: "caacCalculatorWindResult"
                    valid: root.windResult !== null && root.windResult.ok
                    resultText: valid ? qsTr("Heading: %1°").arg(root.formatted(root.windResult.value)) : root.errorText(root.windResult)
                }
            }
            Card {
                heading: qsTr("Time to angle")
                QGCLabel { text: qsTr("Hours (0–23)") }
                NumberField { id: hours; objectName: "caacCalculatorHours"; text: "0"; onTextEdited: root.timeResult = null }
                QGCLabel { text: qsTr("Minutes (0–59)") }
                NumberField { id: minutes; objectName: "caacCalculatorMinutes"; text: "0"; onTextEdited: root.timeResult = null }
                CalculateButton {
                    objectName: "caacCalculatorTimeCalculate"
                    onClicked: root.timeResult = Calculator.timeAngle(Calculator.parsed(hours.text), Calculator.parsed(minutes.text))
                }
                QGCLabel { text: qsTr("24 hours = 360°; 1 hour = 15°."); Layout.fillWidth: true; wrapMode: Text.WordWrap }
                ResultRow {
                    objectName: "caacCalculatorTimeResult"
                    valid: root.timeResult !== null && root.timeResult.ok
                    resultText: valid ? qsTr("Angle: %1°").arg(root.formatted(root.timeResult.value)) : root.errorText(root.timeResult)
                }
            }
            Card {
                heading: qsTr("Trigonometric functions")
                QGCLabel { text: qsTr("Function") }
                QGCComboBox {
                    id: operation
                    objectName: "caacCalculatorTrigFunction"
                    Layout.fillWidth: true
                    model: ["sin", "cos", "tan", "arcsin", "arccos", "arctan"]
                    onActivated: root.trigResult = null
                }
                QGCLabel { text: qsTr("Input value") }
                NumberField { id: trigInput; objectName: "caacCalculatorTrigValue"; text: "30"; onTextEdited: root.trigResult = null }
                QGCCheckBox { id: radians; objectName: "caacCalculatorRadians"; text: qsTr("Radian mode"); onToggled: root.trigResult = null }
                CalculateButton {
                    objectName: "caacCalculatorTrigCalculate"
                    onClicked: root.trigResult = Calculator.trigonometry(operation.currentIndex, Calculator.parsed(trigInput.text), radians.checked)
                }
                ResultRow {
                    objectName: "caacCalculatorTrigResult"
                    valid: root.trigResult !== null && root.trigResult.ok
                    resultText: valid ? operation.currentText + "(" + trigInput.text + (operation.currentIndex < 3 ? (radians.checked ? " rad" : "°") : "")
                        + ") = " + root.formatted(root.trigResult.value) + (operation.currentIndex >= 3 ? (radians.checked ? " rad" : "°") : "") : root.errorText(root.trigResult)
                }
            }
        }
        Card {
            heading: qsTr("Right triangle")
            QGCLabel { text: qsTr("Enter exactly two sides, or one side and an angle."); Layout.fillWidth: true; wrapMode: Text.WordWrap }
            GridLayout {
                Layout.fillWidth: true
                columns: width >= ScreenTools.defaultFontPixelWidth * 65 ? 2 : 1
                columnSpacing: root.cardSpacing
                ColumnLayout {
                    Layout.fillWidth: true
                    Layout.minimumWidth: 0
                    Layout.preferredWidth: parent.width / parent.columns
                    GridLayout {
                        Layout.fillWidth: true
                        columns: 2
                        QGCLabel { text: qsTr("Hypotenuse c"); Layout.fillWidth: true }
                        QGCLabel { text: qsTr("Vertical leg a"); Layout.fillWidth: true }
                        NumberField { id: hypotenuse; objectName: "caacCalculatorSideC"; onTextChanged: root.sideEdited("c", text) }
                        NumberField { id: verticalSide; objectName: "caacCalculatorSideA"; onTextChanged: root.sideEdited("a", text) }
                        QGCLabel { text: qsTr("Horizontal leg b"); Layout.fillWidth: true }
                        QGCLabel { text: qsTr("Angle θ (degrees)"); Layout.fillWidth: true }
                        NumberField { id: horizontalSide; objectName: "caacCalculatorSideB"; onTextChanged: root.sideEdited("b", text) }
                        NumberField { id: theta; objectName: "caacCalculatorTheta"; onTextChanged: root.sideEdited("theta", text) }
                    }
                    RowLayout {
                        CalculateButton { objectName: "caacCalculatorTriangleCalculate"; onClicked: root.solveTriangle() }
                        QGCButton { objectName: "caacCalculatorTriangleClear"; text: qsTr("Clear"); onClicked: root.clearTriangle() }
                    }
                }
                Canvas {
                    id: triangleCanvas
                    objectName: "caacCalculatorTriangleDiagram"
                    Layout.fillWidth: true
                    Layout.minimumWidth: 0
                    Layout.preferredWidth: parent.width / parent.columns
                    Layout.preferredHeight: ScreenTools.defaultFontPixelHeight * 9
                    property var solution: root.triangleResult
                    onSolutionChanged: requestPaint()
                    onWidthChanged: requestPaint()
                    Connections {
                        target: calculatorPalette
                        function onPaletteChanged() { triangleCanvas.requestPaint() }
                    }
                    onPaint: {
                        const ctx = getContext("2d")
                        ctx.reset()
                        const margin = Math.min(width, height) * 0.2
                        const a = solution && solution.ok ? solution.a : 3
                        const b = solution && solution.ok ? solution.b : 4
                        const scale = Math.min((width - margin * 2) / b, (height - margin * 2) / a)
                        const left = margin, bottom = height - margin, right = left + b * scale, top = bottom - a * scale
                        ctx.strokeStyle = root.resultColor
                        ctx.lineWidth = 2
                        ctx.beginPath(); ctx.moveTo(left, top); ctx.lineTo(left, bottom); ctx.lineTo(right, bottom); ctx.closePath(); ctx.stroke()
                        ctx.font = Math.max(10, ScreenTools.defaultFontPixelHeight * 0.55) + 'px "' + ScreenTools.normalFontFamily + '"'
                        ctx.fillStyle = calculatorPalette.text
                        ctx.textAlign = "center"
                        ctx.fillText(solution && solution.ok ? "a = " + root.formatted(a) : "a", left - margin * 0.45, (top + bottom) / 2)
                        ctx.fillText(solution && solution.ok ? "b = " + root.formatted(b) : "b", (left + right) / 2, bottom + margin * 0.65)
                        ctx.fillText(solution && solution.ok ? "c = " + root.formatted(solution.c) : "c", (left + right) / 2 + margin * 0.35, (top + bottom) / 2)
                        ctx.fillStyle = root.angleColor
                        ctx.fillText("90°", left + margin * 0.35, bottom - margin * 0.2)
                        ctx.fillText(solution && solution.ok ? root.formatted(solution.theta) + "°" : "θ", right - margin * 0.4, bottom - margin * 0.2)
                        ctx.fillText(solution && solution.ok ? root.formatted(solution.phi) + "°" : "φ", left + margin * 0.4, top + margin * 0.25)
                    }
                }
            }
            ResultRow {
                objectName: "caacCalculatorTriangleResult"
                valid: root.triangleResult !== null && root.triangleResult.ok
                resultText: valid ? "c=" + root.formatted(root.triangleResult.c) + ", a=" + root.formatted(root.triangleResult.a)
                    + ", b=" + root.formatted(root.triangleResult.b) + ", θ=" + root.formatted(root.triangleResult.theta)
                    + "°, φ=" + root.formatted(root.triangleResult.phi) + "°" : root.errorText(root.triangleResult)
            }
        }
    }
}
