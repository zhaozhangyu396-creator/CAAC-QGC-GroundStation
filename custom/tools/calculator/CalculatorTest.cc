#include <cmath>
#include <limits>
#include <numbers>

#include <QtCore/QDir>
#include <QtGui/QClipboard>
#include <QtTest/QTest>
#include <QtWidgets/QApplication>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QTabWidget>

#include "CalculatorEngine.h"
#include "CalculatorWindow.h"

class CalculatorTest : public QObject
{
    Q_OBJECT

private slots:
    void calculations_data();
    void calculations();
    void invalidInputs_data();
    void invalidInputs();
    void bundledFont();
    void standaloneWorkflow();
};

void CalculatorTest::calculations_data()
{
    QTest::addColumn<QString>("function");
    QTest::addColumn<QVariantList>("arguments");
    QTest::addColumn<QString>("key");
    QTest::addColumn<double>("expected");
    QTest::newRow("wind-wrap") << QStringLiteral("windHeading") << QVariantList{0, 45} << QStringLiteral("value")
                               << 315.0;
    QTest::newRow("wind-north") << QStringLiteral("windHeading") << QVariantList{360, 0} << QStringLiteral("value")
                                << 0.0;
    QTest::newRow("time") << QStringLiteral("timeAngle") << QVariantList{6, 30} << QStringLiteral("value") << 97.5;
    QTest::newRow("sin-degrees") << QStringLiteral("trigonometry") << QVariantList{0, 30, false}
                                 << QStringLiteral("value") << 0.5;
    QTest::newRow("cos-radians") << QStringLiteral("trigonometry") << QVariantList{1, std::numbers::pi, true}
                                 << QStringLiteral("value") << -1.0;
    QTest::newRow("tan-degrees") << QStringLiteral("trigonometry") << QVariantList{2, 45, false}
                                 << QStringLiteral("value") << 1.0;
    QTest::newRow("arcsin-degrees") << QStringLiteral("trigonometry") << QVariantList{3, 0.5, false}
                                    << QStringLiteral("value") << 30.0;
    QTest::newRow("arccos-degrees") << QStringLiteral("trigonometry") << QVariantList{4, 0.5, false}
                                    << QStringLiteral("value") << 60.0;
    QTest::newRow("arctan-radians") << QStringLiteral("trigonometry") << QVariantList{5, 1, true}
                                    << QStringLiteral("value") << std::numbers::pi / 4;
    QTest::newRow("triangle-two-legs") << QStringLiteral("triangle") << QVariantList{QVariantMap{{"a", 3}, {"b", 4}}}
                                       << QStringLiteral("c") << 5.0;
    QTest::newRow("triangle-hypotenuse") << QStringLiteral("triangle") << QVariantList{QVariantMap{{"c", 5}, {"a", 3}}}
                                         << QStringLiteral("b") << 4.0;
    QTest::newRow("triangle-angle") << QStringLiteral("triangle") << QVariantList{QVariantMap{{"c", 10}, {"theta", 30}}}
                                    << QStringLiteral("a") << 5.0;
}

void CalculatorTest::calculations()
{
    QFETCH(QString, function);
    QFETCH(QVariantList, arguments);
    QFETCH(QString, key);
    QFETCH(double, expected);
    CalculatorEngine engine;
    QVERIFY(engine.isReady());
    const auto result = engine.calculate(function, arguments);
    QVERIFY2(result.value(QStringLiteral("ok")).toBool(), qPrintable(result.value(QStringLiteral("error")).toString()));
    QVERIFY(std::abs(result.value(key).toDouble() - expected) < 1e-10);
}

void CalculatorTest::invalidInputs_data()
{
    QTest::addColumn<QString>("function");
    QTest::addColumn<QVariantList>("arguments");
    QTest::addColumn<QString>("error");
    QTest::newRow("wind-range") << QStringLiteral("windHeading") << QVariantList{361, 0} << QStringLiteral("wind");
    QTest::newRow("wind-nan") << QStringLiteral("windHeading")
                              << QVariantList{std::numeric_limits<double>::quiet_NaN(), 0} << QStringLiteral("wind");
    QTest::newRow("fractional-hour") << QStringLiteral("timeAngle") << QVariantList{1.5, 0} << QStringLiteral("time");
    QTest::newRow("minute-range") << QStringLiteral("timeAngle") << QVariantList{0, 60} << QStringLiteral("time");
    QTest::newRow("undefined-tangent") << QStringLiteral("trigonometry") << QVariantList{2, 90, false}
                                       << QStringLiteral("tangent");
    QTest::newRow("inverse-domain") << QStringLiteral("trigonometry") << QVariantList{3, 2, false}
                                    << QStringLiteral("domain");
    QTest::newRow("triangle-missing") << QStringLiteral("triangle") << QVariantList{QVariantMap{{"a", 3}}}
                                      << QStringLiteral("known");
    QTest::newRow("triangle-impossible") << QStringLiteral("triangle") << QVariantList{QVariantMap{{"a", 5}, {"c", 3}}}
                                         << QStringLiteral("hypotenuse");
    QTest::newRow("triangle-angle-range")
        << QStringLiteral("triangle") << QVariantList{QVariantMap{{"a", 3}, {"theta", 90}}} << QStringLiteral("angle");
}

void CalculatorTest::invalidInputs()
{
    QFETCH(QString, function);
    QFETCH(QVariantList, arguments);
    QFETCH(QString, error);
    CalculatorEngine engine;
    QVERIFY(engine.isReady());
    QVERIFY(std::isnan(engine.parsed(QString())));
    QVERIFY(std::isnan(engine.parsed(QStringLiteral("abc"))));
    const auto result = engine.calculate(function, arguments);
    QVERIFY(!result.value(QStringLiteral("ok")).toBool());
    QCOMPARE(result.value(QStringLiteral("error")).toString(), error);
}

void CalculatorTest::bundledFont()
{
    CalculatorWindow window;
    QVERIFY(window.isReady());
    const QFontMetrics metrics(window.font());
    for (const QChar character : QStringLiteral("飞虎角度计算器风向时间三角函数直角斜边复制结果0123456789θφ°～")) {
        QVERIFY2(metrics.inFont(character), qPrintable(QStringLiteral("Missing glyph: %1").arg(character)));
    }
}

void CalculatorTest::standaloneWorkflow()
{
    CalculatorWindow window;
    QVERIFY(window.isReady());
    window.show();
    auto* tabs = window.findChild<QTabWidget*>(QStringLiteral("calculatorTabs"));
    QVERIFY(tabs);
    QCOMPARE(tabs->count(), 4);
    const auto field = [&window](const char* name) { return window.findChild<QLineEdit*>(QString::fromLatin1(name)); };
    const auto edit = [&field](const char* name, const QString& text) {
        auto* input = field(name);
        if (!input) {
            return false;
        }
        input->setFocus();
        input->selectAll();
        QTest::keyClicks(input, text);
        return true;
    };
    const auto click = [&window](const char* name) {
        auto* button = window.findChild<QPushButton*>(QString::fromLatin1(name));
        if (!button) {
            return false;
        }
        button->click();
        return true;
    };
    const auto result = [&window](const char* name) {
        const auto* label = window.findChild<QLabel*>(QString::fromLatin1(name));
        return label ? label->text() : QString();
    };
    const auto screenshot = [&window](int page) {
        const QString directory = qEnvironmentVariable("CAAC_CALCULATOR_SCREENSHOT_DIR");
        if (directory.isEmpty()) {
            return true;
        }
        QDir().mkpath(directory);
        QCoreApplication::processEvents();
        return window.grab().save(directory + QStringLiteral("/calculator-%1.png").arg(page));
    };

    QVERIFY(click("windCalculate"));
    QCOMPARE(result("windResult"), QStringLiteral("航向角：315°"));
    QVERIFY(click("windCopy"));
    QCOMPARE(QApplication::clipboard()->text(), result("windResult"));
    QVERIFY(screenshot(0));
    QVERIFY(edit("wind", QStringLiteral("361")));
    QVERIFY(click("windCalculate"));
    QVERIFY(result("windResult").contains(QStringLiteral("0°～360°")));

    tabs->setCurrentIndex(1);
    QVERIFY(edit("hours", QStringLiteral("6")));
    QVERIFY(edit("minutes", QStringLiteral("30")));
    QVERIFY(click("timeCalculate"));
    QCOMPARE(result("timeResult"), QStringLiteral("角度：97.5°"));
    QVERIFY(screenshot(1));

    tabs->setCurrentIndex(2);
    QVERIFY(click("trigCalculate"));
    QVERIFY(result("trigResult").endsWith(QStringLiteral("= 0.5")));
    QVERIFY(screenshot(2));

    tabs->setCurrentIndex(3);
    QVERIFY(edit("side_c", QStringLiteral("5")));
    QVERIFY(edit("side_a", QStringLiteral("3")));
    QVERIFY(click("triangleCalculate"));
    QCOMPARE(field("side_b")->text(), QStringLiteral("4"));
    QVERIFY(result("triangleResult").contains(QStringLiteral("b = 4")));
    QVERIFY(click("triangleCopy"));
    QCOMPARE(QApplication::clipboard()->text(), result("triangleResult"));
    QVERIFY(click("triangleCalculate"));
    QCOMPARE(field("side_b")->text(), QStringLiteral("4"));
    QVERIFY(screenshot(3));
    window.resize(window.minimumSize());
    QVERIFY(screenshot(4));
    QVERIFY(click("triangleClear"));
    QVERIFY(field("side_c")->text().isEmpty());
    QVERIFY(edit("side_c", QStringLiteral("10")));
    QVERIFY(edit("side_theta", QStringLiteral("30")));
    QVERIFY(click("triangleCalculate"));
    QCOMPARE(field("side_a")->text(), QStringLiteral("5"));
}

QTEST_MAIN(CalculatorTest)
#include "CalculatorTest.moc"
