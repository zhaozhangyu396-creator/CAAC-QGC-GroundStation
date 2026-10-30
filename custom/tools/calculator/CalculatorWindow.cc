#include "CalculatorWindow.h"

#include <algorithm>
#include <array>
#include <cmath>
#include <memory>

#include <QtGui/QClipboard>
#include <QtGui/QFontDatabase>
#include <QtGui/QPainter>
#include <QtWidgets/QApplication>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QScrollArea>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QVBoxLayout>

namespace {
QString formatted(double value)
{
    return QString::number(value, 'g', 12);
}

QString errorText(const QVariantMap& result)
{
    const QString error = result.value(QStringLiteral("error")).toString();
    if (error == QStringLiteral("wind")) {
        return CalculatorWindow::tr("请输入 0°～360° 的风向。");
    }
    if (error == QStringLiteral("time")) {
        return CalculatorWindow::tr("请输入 0～23 的整数小时和 0～59 的整数分钟。");
    }
    if (error == QStringLiteral("domain")) {
        return CalculatorWindow::tr("arcsin 和 arccos 的输入值必须在 -1～1 之间。");
    }
    if (error == QStringLiteral("tangent")) {
        return CalculatorWindow::tr("此角度的正切值无定义。");
    }
    if (error == QStringLiteral("known")) {
        return CalculatorWindow::tr("请只填写两条边，或一条边和一个角度；重新计算可先清空。");
    }
    if (error == QStringLiteral("angle")) {
        return CalculatorWindow::tr("锐角必须大于 0° 且小于 90°。");
    }
    if (error == QStringLiteral("hypotenuse")) {
        return CalculatorWindow::tr("斜边必须大于任一直角边。");
    }
    if (error == QStringLiteral("sides")) {
        return CalculatorWindow::tr("请输入有限的正数边长。");
    }
    if (error == QStringLiteral("engine")) {
        return CalculatorWindow::tr("无法加载计算模块，请重新安装。");
    }
    return CalculatorWindow::tr("请输入有限数值。");
}

class ResultBox : public QWidget
{
public:
    explicit ResultBox(const QString& name, QWidget* parent)
        : QWidget(parent)
    {
        auto* layout = new QVBoxLayout(this);
        layout->setContentsMargins(0, 0, 0, 0);
        _label = new QLabel(this);
        _label->setObjectName(name + QStringLiteral("Result"));
        _label->setWordWrap(true);
        _label->setTextFormat(Qt::PlainText);
        _label->setTextInteractionFlags(Qt::TextSelectableByMouse | Qt::TextSelectableByKeyboard);
        layout->addWidget(_label);
        _copy = new QPushButton(CalculatorWindow::tr("复制结果"), this);
        _copy->setObjectName(name + QStringLiteral("Copy"));
        layout->addWidget(_copy, 0, Qt::AlignLeft);
        connect(_copy, &QPushButton::clicked, this, [this]() { QApplication::clipboard()->setText(_label->text()); });
        clear();
    }

    void showResult(const QVariantMap& result, const QString& success)
    {
        const bool valid = result.value(QStringLiteral("ok")).toBool();
        _label->setText(valid ? success : errorText(result));
        _label->setStyleSheet(valid ? QStringLiteral("color: #67e5a2;") : QStringLiteral("color: #ffcf6b;"));
        _copy->setVisible(valid);
        setVisible(true);
    }

    void clear()
    {
        _label->clear();
        setVisible(false);
    }

private:
    QLabel* _label = nullptr;
    QPushButton* _copy = nullptr;
};

QVBoxLayout* addPage(QTabWidget* tabs, const QString& title, const QString& description)
{
    auto* scroll = new QScrollArea(tabs);
    scroll->setWidgetResizable(true);
    scroll->setFrameShape(QFrame::NoFrame);
    auto* page = new QWidget(scroll);
    auto* layout = new QVBoxLayout(page);
    const int unit = page->fontMetrics().height();
    layout->setContentsMargins(unit, unit, unit, unit);
    layout->setSpacing(unit);
    auto* heading = new QLabel(title, page);
    QFont font = heading->font();
    font.setBold(true);
    font.setPointSizeF(font.pointSizeF() * 1.3);
    heading->setFont(font);
    layout->addWidget(heading);
    auto* hint = new QLabel(description, page);
    hint->setWordWrap(true);
    layout->addWidget(hint);
    scroll->setWidget(page);
    tabs->addTab(scroll, title);
    return layout;
}

QLineEdit* numberField(const QString& name, const QString& value, QWidget* parent)
{
    auto* field = new QLineEdit(value, parent);
    field->setObjectName(name);
    field->setClearButtonEnabled(true);
    field->setInputMethodHints(Qt::ImhFormattedNumbersOnly);
    field->setMinimumWidth(field->fontMetrics().horizontalAdvance(QStringLiteral("-123456789.123")));
    field->setMinimumHeight(field->fontMetrics().height() * 2);
    return field;
}

QFormLayout* formLayout()
{
    auto* form = new QFormLayout;
    form->setFieldGrowthPolicy(QFormLayout::AllNonFixedFieldsGrow);
    form->setRowWrapPolicy(QFormLayout::WrapLongRows);
    return form;
}

QPushButton* calculateButton(const QString& name, QVBoxLayout* layout)
{
    auto* button = new QPushButton(CalculatorWindow::tr("计算"));
    button->setObjectName(name);
    button->setProperty("calculate", true);
    layout->addWidget(button, 0, Qt::AlignLeft);
    return button;
}

class TriangleDiagram : public QWidget
{
public:
    explicit TriangleDiagram(QWidget* parent)
        : QWidget(parent)
    {
        setMinimumHeight(fontMetrics().height() * 11);
        setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        setAccessibleName(CalculatorWindow::tr("直角三角形示意图"));
    }

    void setSolution(const QVariantMap& result)
    {
        _solution = result;
        update();
    }

protected:
    void paintEvent(QPaintEvent*) override
    {
        QPainter painter(this);
        painter.setRenderHint(QPainter::Antialiasing);
        const bool valid = _solution.value(QStringLiteral("ok")).toBool();
        const double a = valid ? _solution.value(QStringLiteral("a")).toDouble() : 3;
        const double b = valid ? _solution.value(QStringLiteral("b")).toDouble() : 4;
        const double largest = std::max(a, b);
        const double unit = fontMetrics().height();
        const QRectF area = QRectF(rect()).adjusted(unit * 3, unit * 2, -unit * 3, -unit * 2);
        const double scale = std::min(area.width() / (b / largest), area.height() / (a / largest));
        const QPointF left(area.left(), area.bottom());
        const QPointF right(left.x() + b / largest * scale, left.y());
        const QPointF top(left.x(), left.y() - a / largest * scale);
        painter.setPen(QPen(QColor(QStringLiteral("#67e5a2")), 2));
        painter.drawPolygon(QPolygonF{top, left, right});
        painter.setPen(palette().color(QPalette::WindowText));
        painter.drawText((top + left) / 2 + QPointF(-unit * 1.5, 0), QStringLiteral("a"));
        painter.drawText((left + right) / 2 + QPointF(0, unit), QStringLiteral("b"));
        painter.drawText((top + right) / 2 + QPointF(unit / 2, 0), QStringLiteral("c"));
        painter.setPen(QColor(QStringLiteral("#ffcf6b")));
        painter.drawText(left + QPointF(unit / 2, -unit / 2), QStringLiteral("90°"));
        painter.drawText(right + QPointF(-unit * 3, -unit / 2),
                         valid ? formatted(_solution.value(QStringLiteral("theta")).toDouble()) + QStringLiteral("°")
                               : QStringLiteral("θ"));
        painter.drawText(top + QPointF(unit / 2, unit), QStringLiteral("φ"));
    }

private:
    QVariantMap _solution;
};
}  // namespace

CalculatorWindow::CalculatorWindow(QWidget* parent)
    : QWidget(parent)
{
    Q_INIT_RESOURCE(calculator_font);
    static const int FONT_ID = QFontDatabase::addApplicationFont(QStringLiteral(":/calculator/CalculatorSans.otf"));
    const QStringList families = QFontDatabase::applicationFontFamilies(FONT_ID);
    QFont applicationFont = font();
    _fontReady = !families.isEmpty();
    if (_fontReady) {
        applicationFont.setFamily(families.first());
    }
    applicationFont.setPointSizeF(std::max(11.0, applicationFont.pointSizeF()));
    setFont(applicationFont);
    setWindowTitle(tr("飞虎角度计算器"));
    setStyleSheet(QStringLiteral(
        "QWidget { background: #22262b; color: #f1f3f5; }"
        "QLineEdit, QComboBox { background: #303740; border: 1px solid #59636e; border-radius: 4px; padding: 7px; }"
        "QPushButton { background: #394550; border: 1px solid #65717d; border-radius: 4px; padding: 8px 22px; }"
        "QPushButton:hover { background: #465664; }"
        "QPushButton:focus, QLineEdit:focus, QComboBox:focus { border: 1px solid #67e5a2; }"
        "QPushButton[calculate=true] { background: #176a45; }"
        "QPushButton[calculate=true]:hover { background: #20875a; }"
        "QTabBar::tab { background: #303740; padding: 10px 12px; }"
        "QTabBar::tab:selected { background: #176a45; }"
        "QTabWidget::pane { border: 1px solid #59636e; }"));
    const int unit = fontMetrics().height();
    resize(unit * 37, unit * 30);
    setMinimumSize(unit * 21, unit * 21);
    auto* layout = new QVBoxLayout(this);
    _tabs = new QTabWidget(this);
    _tabs->setObjectName(QStringLiteral("calculatorTabs"));
    layout->addWidget(_tabs);
    _addWindPage();
    _addTimePage();
    _addTrigonometryPage();
    _addTrianglePage();
}

bool CalculatorWindow::isReady() const
{
    return _engine.isReady() && _fontReady;
}

void CalculatorWindow::_addWindPage()
{
    auto* page = addPage(_tabs, tr("风向角度"), tr("正北为 0°，角度顺时针增加。根据来风方向计算所需航向角。"));
    auto* form = formLayout();
    auto* preset = new QComboBox(this);
    preset->setObjectName(QStringLiteral("windPreset"));
    preset->addItem(tr("选择预设风向"), 0);
    const QStringList directions = {tr("北东北"), tr("东北"), tr("东东北"), tr("东"),   tr("东东南"), tr("东南"),
                                    tr("南东南"), tr("南"),   tr("南西南"), tr("西南"), tr("西西南"), tr("西"),
                                    tr("西西北"), tr("西北"), tr("北西北"), tr("北")};
    for (int index = 0; index < directions.size(); ++index) {
        const double angle = (index + 1) * 22.5;
        preset->addItem(directions[index] + QStringLiteral(" · %1°").arg(formatted(angle)), angle);
    }
    auto* wind = numberField(QStringLiteral("wind"), QStringLiteral("0"), this);
    auto* relative = new QComboBox(this);
    relative->setObjectName(QStringLiteral("relativeWind"));
    const QStringList labels = {tr("正右前侧来风"), tr("正前侧来风"),   tr("正右侧来风"), tr("正右后侧来风"),
                                tr("正后侧来风"),   tr("正左后侧来风"), tr("正左侧来风"), tr("正左前侧来风")};
    const std::array angles{45, 0, 90, 135, 180, 225, 270, 315};
    for (int index = 0; index < labels.size(); ++index) {
        relative->addItem(labels[index], angles[index]);
    }
    form->addRow(tr("预设风向"), preset);
    form->addRow(tr("风向角度（°）"), wind);
    form->addRow(tr("飞行方式"), relative);
    page->addLayout(form);
    auto* button = calculateButton(QStringLiteral("windCalculate"), page);
    auto* result = new ResultBox(QStringLiteral("wind"), this);
    page->addWidget(result);
    page->addStretch();
    connect(preset, &QComboBox::activated, this, [wind, preset, result]() {
        wind->setText(formatted(preset->currentData().toDouble()));
        result->clear();
    });
    connect(wind, &QLineEdit::textEdited, this, [preset, result]() {
        preset->setCurrentIndex(0);
        result->clear();
    });
    connect(relative, &QComboBox::activated, result, [result]() { result->clear(); });
    const auto calculate = [this, wind, relative, result]() {
        const auto value =
            _engine.calculate(QStringLiteral("windHeading"), {_engine.parsed(wind->text()), relative->currentData()});
        result->showResult(value, tr("航向角：%1°").arg(formatted(value.value(QStringLiteral("value")).toDouble())));
    };
    connect(button, &QPushButton::clicked, this, calculate);
    connect(wind, &QLineEdit::returnPressed, this, calculate);
}

void CalculatorWindow::_addTimePage()
{
    auto* page = addPage(_tabs, tr("时间角度"), tr("24 小时对应 360°，1 小时对应 15°，1 分钟对应 0.25°。"));
    auto* form = formLayout();
    auto* hours = numberField(QStringLiteral("hours"), QStringLiteral("0"), this);
    auto* minutes = numberField(QStringLiteral("minutes"), QStringLiteral("0"), this);
    form->addRow(tr("时（0～23）"), hours);
    form->addRow(tr("分（0～59）"), minutes);
    page->addLayout(form);
    auto* button = calculateButton(QStringLiteral("timeCalculate"), page);
    auto* result = new ResultBox(QStringLiteral("time"), this);
    page->addWidget(result);
    page->addStretch();
    const auto calculate = [this, hours, minutes, result]() {
        const auto value = _engine.calculate(QStringLiteral("timeAngle"),
                                             {_engine.parsed(hours->text()), _engine.parsed(minutes->text())});
        result->showResult(value, tr("角度：%1°").arg(formatted(value.value(QStringLiteral("value")).toDouble())));
    };
    connect(button, &QPushButton::clicked, this, calculate);
    for (auto* field : {hours, minutes}) {
        connect(field, &QLineEdit::textEdited, result, [result]() { result->clear(); });
        connect(field, &QLineEdit::returnPressed, this, calculate);
    }
}

void CalculatorWindow::_addTrigonometryPage()
{
    auto* page = addPage(_tabs, tr("三角函数"), tr("支持 sin、cos、tan 及反三角函数。默认使用角度，可切换弧度模式。"));
    auto* form = formLayout();
    auto* operation = new QComboBox(this);
    operation->setObjectName(QStringLiteral("operation"));
    operation->addItems({QStringLiteral("sin"), QStringLiteral("cos"), QStringLiteral("tan"), QStringLiteral("arcsin"),
                         QStringLiteral("arccos"), QStringLiteral("arctan")});
    auto* input = numberField(QStringLiteral("trigInput"), QStringLiteral("30"), this);
    auto* radians = new QCheckBox(tr("弧度模式"), this);
    radians->setObjectName(QStringLiteral("radians"));
    form->addRow(tr("函数"), operation);
    form->addRow(tr("输入值"), input);
    form->addRow(radians);
    page->addLayout(form);
    auto* button = calculateButton(QStringLiteral("trigCalculate"), page);
    auto* result = new ResultBox(QStringLiteral("trig"), this);
    page->addWidget(result);
    page->addStretch();
    const auto calculate = [this, operation, input, radians, result]() {
        const auto value =
            _engine.calculate(QStringLiteral("trigonometry"),
                              {operation->currentIndex(), _engine.parsed(input->text()), radians->isChecked()});
        const QString unit = radians->isChecked() ? QStringLiteral(" rad") : QStringLiteral("°");
        const bool inverse = operation->currentIndex() >= 3;
        result->showResult(
            value, QStringLiteral("%1(%2%3) = %4%5")
                       .arg(operation->currentText(), input->text(), inverse ? QString() : unit,
                            formatted(value.value(QStringLiteral("value")).toDouble()), inverse ? unit : QString()));
    };
    connect(button, &QPushButton::clicked, this, calculate);
    connect(input, &QLineEdit::returnPressed, this, calculate);
    connect(input, &QLineEdit::textEdited, result, [result]() { result->clear(); });
    connect(operation, &QComboBox::activated, result, [result]() { result->clear(); });
    connect(radians, &QCheckBox::toggled, result, [result]() { result->clear(); });
}

void CalculatorWindow::_addTrianglePage()
{
    auto* page = addPage(_tabs, tr("直角三角形"), tr("请只填写两条边，或一条边和夹角 θ。所有边长使用相同单位。"));
    auto* form = formLayout();
    const std::array<QString, 4> keys{QStringLiteral("c"), QStringLiteral("a"), QStringLiteral("b"),
                                      QStringLiteral("theta")};
    const QStringList labels{tr("斜边 c"), tr("垂直直角边 a"), tr("水平直角边 b"), tr("夹角 θ（°）")};
    std::array<QLineEdit*, 4> fields{};
    for (int index = 0; index < 4; ++index) {
        fields[index] = numberField(QStringLiteral("side_") + keys[index], QString(), this);
        form->addRow(labels[index], fields[index]);
    }
    page->addLayout(form);
    auto* buttons = new QHBoxLayout;
    auto* calculate = new QPushButton(tr("计算"), this);
    calculate->setProperty("calculate", true);
    calculate->setObjectName(QStringLiteral("triangleCalculate"));
    auto* clear = new QPushButton(tr("清空"), this);
    clear->setObjectName(QStringLiteral("triangleClear"));
    buttons->addWidget(calculate);
    buttons->addWidget(clear);
    buttons->addStretch();
    page->addLayout(buttons);
    auto* result = new ResultBox(QStringLiteral("triangle"), this);
    page->addWidget(result);
    auto* diagram = new TriangleDiagram(this);
    page->addWidget(diagram, 1);
    auto known = std::make_shared<QVariantMap>();
    for (int index = 0; index < 4; ++index) {
        connect(fields[index], &QLineEdit::textEdited, this,
                [this, known, key = keys[index], result, diagram](const QString& text) {
                    if (text.trimmed().isEmpty()) {
                        known->remove(key);
                    } else {
                        known->insert(key, _engine.parsed(text));
                    }
                    result->clear();
                    diagram->setSolution({});
                });
    }
    const auto solve = [this, known, fields, result, diagram, keys]() {
        const auto value = _engine.calculate(QStringLiteral("triangle"), {*known});
        if (value.value(QStringLiteral("ok")).toBool()) {
            for (int index = 0; index < 4; ++index) {
                fields[index]->setText(formatted(value.value(keys[index]).toDouble()));
            }
        }
        result->showResult(value, QStringLiteral("c = %1，a = %2，b = %3\nθ = %4°，φ = %5°")
                                      .arg(formatted(value.value(QStringLiteral("c")).toDouble()),
                                           formatted(value.value(QStringLiteral("a")).toDouble()),
                                           formatted(value.value(QStringLiteral("b")).toDouble()),
                                           formatted(value.value(QStringLiteral("theta")).toDouble()),
                                           formatted(value.value(QStringLiteral("phi")).toDouble())));
        diagram->setSolution(value);
    };
    connect(calculate, &QPushButton::clicked, this, solve);
    for (auto* field : fields) {
        connect(field, &QLineEdit::returnPressed, this, solve);
    }
    connect(clear, &QPushButton::clicked, this, [known, fields, result, diagram]() {
        known->clear();
        for (auto* field : fields) {
            field->clear();
        }
        result->clear();
        diagram->setSolution({});
    });
}
