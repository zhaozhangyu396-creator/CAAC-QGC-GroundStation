#include "VehicleMessageTranslatorTest.h"

#include <cstring>

#include <QtCore/QCoreApplication>
#include <QtCore/QFile>
#include <QtCore/QLocale>
#include <QtCore/QRegularExpression>
#include <QtCore/QScopeGuard>
#include <QtCore/QSet>
#include <QtCore/QXmlStreamReader>
#include <QtTest/QSignalSpy>

#include "HealthAndArmingCheckReport.h"
#include "MAVLinkLib.h"
#include "QGCApplication.h"
#include "StatusTextHandler.h"
#include "VehicleMessageCatalog.h"
#include "VehicleMessageTranslator.h"

void VehicleMessageTranslatorTest::init()
{
    UnitTest::init();
    QVERIFY(_translator.load(QStringLiteral("qgc_source_zh_CN"), QStringLiteral(":/i18n")));
    QVERIFY(QCoreApplication::installTranslator(&_translator));
    VehicleMessageTranslator::setLanguage(QLocale(QLocale::Chinese));
}

void VehicleMessageTranslatorTest::cleanup()
{
    VehicleMessageTranslator::setLanguage(QLocale(QLocale::English));
    QCoreApplication::removeTranslator(&_translator);
    UnitTest::cleanup();
}

void VehicleMessageTranslatorTest::_translations_data()
{
    QTest::addColumn<QString>("source");
    QTest::addColumn<QString>("expected");
    QTest::newRow("screenshot") << QStringLiteral("Preflight Fail: Strong magnetic interference")
                                << QStringLiteral("飞行前检查失败：磁场干扰过强");
    QTest::newRow("case-and-spacing") << QStringLiteral(" preflight fail: strong magnetic interference ")
                                      << QStringLiteral(" 飞行前检查失败：磁场干扰过强 ");
    QTest::newRow("apm-compass") << QStringLiteral("PreArm: Compass 2 not healthy")
                                 << QStringLiteral("解锁前检查：罗盘 2 状态异常");
    QTest::newRow("gps-distance") << QStringLiteral("PreArm: GPS positions differ by 12.3m")
                                  << QStringLiteral("解锁前检查：GPS 位置相差 12.3 m");
    QTest::newRow("thresholds") << QStringLiteral("PreArm: Check mag field: 550, max 530, min 120")
                                << QStringLiteral("解锁前检查：检查磁场强度：550，最大值 530，最小值 120");
    QTest::newRow("parameter-identifiers") << QStringLiteral("PreArm: RC1_MIN is greater than RC1_TRIM")
                                           << QStringLiteral("解锁前检查：RC1_MIN 大于 RC1_TRIM");
    QTest::newRow("voltage-values") << QStringLiteral("PreArm: Board (4.6v) out of range 4.8-5.4v")
                                    << QStringLiteral("解锁前检查：飞控供电电压 4.6 V 超出 4.8～5.4 V 范围");
    QTest::newRow("battery-fault") << QStringLiteral("Battery 1: under voltage. Land now!")
                                   << QStringLiteral("电池 1：电压过低。请立即降落！");
    QTest::newRow("log-rate") << QStringLiteral("PreArm: Main loop slow (200Hz < 400Hz)")
                              << QStringLiteral("解锁前检查：主循环频率过低（200 Hz < 400 Hz）");
    QTest::newRow("escaped-log-rate") << QStringLiteral("PreArm: Main loop slow (200Hz &lt; 400Hz)")
                                      << QStringLiteral("解锁前检查：主循环频率过低（200 Hz < 400 Hz）");
    QTest::newRow("literal-placeholder") << QStringLiteral("Bad parameter: CUSTOM_%2")
                                         << QStringLiteral("参数无效：CUSTOM_%2");
    QTest::newRow("component-number") << QStringLiteral("Vehicle 2: Preflight Fail: GPS fix too low")
                                      << QStringLiteral("无人机 2：飞行前检查失败：GPS 定位等级不足");
    QTest::newRow("calibration") << QStringLiteral("[cal] calibration done: mag") << QStringLiteral("校准已完成：罗盘");
    QTest::newRow("calibration-start") << QStringLiteral("[cal] calibration started: 2 gyro")
                                       << QStringLiteral("校准已开始（协议 2）：陀螺仪");
    QTest::newRow("multiline") << QStringLiteral("- Strong magnetic interference\n- RC not found")
                               << QStringLiteral("- 磁场干扰过强\n- 未检测到遥控信号");
    QTest::newRow("rich-text")
        << QStringLiteral("<b title=\"Strong magnetic interference\">Strong magnetic interference</b><br/>Low battery")
        << QStringLiteral("<b title=\"Strong magnetic interference\">磁场干扰过强</b><br/>电池电量不足");
    QTest::newRow("parameter-link")
        << QStringLiteral("Set <a href=\"param://COM_ARM_MAG_STR?text=Low battery&amp;x=%2\">COM_ARM_MAG_STR</a> to 0")
        << QStringLiteral(
               "将 <a href=\"param://COM_ARM_MAG_STR?text=Low battery&amp;x=%2\">COM_ARM_MAG_STR</a> 设置为 0");
    QTest::newRow("already-chinese") << QStringLiteral("飞行前检查失败：磁场干扰过强")
                                     << QStringLiteral("飞行前检查失败：磁场干扰过强");
    QTest::newRow("unknown-firmware") << QStringLiteral("VendorCustom: status CODE_001=4")
                                      << QStringLiteral("VendorCustom: status CODE_001=4");
    QTest::newRow("unknown-reason-retained")
        << QStringLiteral("PreArm: VENDOR_SENSOR_X=4") << QStringLiteral("解锁前检查：VENDOR_SENSOR_X=4");
}

void VehicleMessageTranslatorTest::_translations()
{
    QFETCH(QString, source);
    QFETCH(QString, expected);
    QCOMPARE(VehicleMessageTranslator::translate(source), expected);
}

void VehicleMessageTranslatorTest::_rawMessages_data()
{
    QTest::addColumn<QString>("source");
    QTest::addColumn<QString>("expected");
    QTest::newRow("single-message") << QStringLiteral("Preflight Fail: Strong magnetic interference")
                                    << QStringLiteral("飞行前检查失败：磁场干扰过强");
    QTest::newRow("chunked-message") << QStringLiteral("Preflight Fail: GPS Horizontal Position Error too high")
                                     << QStringLiteral("飞行前检查失败：GPS 水平位置误差过大");
    QTest::newRow("calibration-protocol")
        << QStringLiteral("[cal] calibration done: mag") << QStringLiteral("校准已完成：罗盘");
}

void VehicleMessageTranslatorTest::_rawMessages()
{
    QFETCH(QString, source);
    QFETCH(QString, expected);
    StatusTextHandler handler;
    QSignalSpy raw(&handler, &StatusTextHandler::textMessageReceived);
    QSignalSpy error(&handler, &StatusTextHandler::newErrorMessage);
    connect(&handler, &StatusTextHandler::textMessageReceived, &handler,
            [&handler](MAV_COMPONENT compId, MAV_SEVERITY severity, const QString& text, const QString& description) {
                handler.handleHTMLEscapedTextMessage(compId, severity, text.toHtmlEscaped(), description);
            });
    const QByteArray bytes = source.toUtf8();
    for (qsizetype offset = 0; offset <= bytes.size(); offset += MAVLINK_MSG_STATUSTEXT_FIELD_TEXT_LEN) {
        char chunk[MAVLINK_MSG_STATUSTEXT_FIELD_TEXT_LEN] = {};
        const qsizetype length = qMin<qsizetype>(sizeof(chunk), bytes.size() - offset);
        if (length > 0) {
            std::memcpy(chunk, bytes.constData() + offset, length);
        }
        mavlink_message_t message{};
        mavlink_msg_statustext_pack(1, MAV_COMP_ID_AUTOPILOT1, &message, MAV_SEVERITY_CRITICAL, chunk, 42,
                                    static_cast<uint8_t>(offset / sizeof(chunk)));
        handler.mavlinkMessageReceived(message);
    }
    QCOMPARE(raw.count(), 1);
    QCOMPARE(raw.first()[2].toString(), source);
    QCOMPARE(error.count(), 1);
    QCOMPARE(error.first()[0].toString(), source.toHtmlEscaped());
    QCOMPARE(handler.messages().size(), 1);
    QCOMPARE(handler.messages().first()->getText(), source.toHtmlEscaped());
    QCOMPARE(handler.criticalMessageCount(), uint32_t(1));
    QVERIFY(handler.formattedMessages().contains(expected));
}

void VehicleMessageTranslatorTest::_catalogIntegrity()
{
    QSet<QString> exactSources;
    for (const char* source : VehicleMessageCatalog::messages) {
        const QString key = QString::fromUtf8(source).simplified().toCaseFolded();
        QVERIFY2(!exactSources.contains(key), source);
        exactSources.insert(key);
        QVERIFY2(!_translator.translate("VehicleMessages", source).isEmpty(), source);
    }
    static const QRegularExpression placeholder(QStringLiteral("%([1-9][0-9]?)"));
    for (const auto& rule : VehicleMessageCatalog::rules) {
        const QRegularExpression expression(QString::fromUtf8(rule.expression));
        QVERIFY2(expression.isValid(), qPrintable(expression.errorString()));
        const QString translated = _translator.translate("VehicleMessages", rule.source);
        QVERIFY2(!translated.isEmpty(), rule.source);
        QSet<int> sourcePlaceholders;
        auto matches = placeholder.globalMatch(QString::fromUtf8(rule.source));
        while (matches.hasNext()) {
            sourcePlaceholders.insert(matches.next().captured(1).toInt());
        }
        QSet<int> translatedPlaceholders;
        matches = placeholder.globalMatch(translated);
        while (matches.hasNext()) {
            translatedPlaceholders.insert(matches.next().captured(1).toInt());
        }
        QCOMPARE(sourcePlaceholders, translatedPlaceholders);
        QCOMPARE(sourcePlaceholders.size(), expression.captureCount());
    }
}

void VehicleMessageTranslatorTest::_parameterCatalogIntegrity()
{
    QSet<QString> allSources;
    static const QRegularExpression parameterId(
        QStringLiteral(R"((?<![A-Za-z0-9_])[A-Z][A-Z0-9]*_[A-Z0-9_]+(?![A-Za-z0-9_]))"));
    const auto ids = [](const QString& text) {
        QSet<QString> result;
        auto matches = parameterId.globalMatch(text);
        while (matches.hasNext()) {
            result.insert(matches.next().captured());
        }
        return result;
    };
    for (int i = 0; i < 8; ++i) {
        QTranslator compiled;
        QVERIFY(compiled.load(QStringLiteral("qgc_parameters_%1_zh_CN").arg(i), QStringLiteral(":/i18n")));
        const QString relative = QStringLiteral("../../translations/qgc_parameters_%1_zh_CN.ts").arg(i);
        QFile file(QFINDTESTDATA(relative));
        QVERIFY(file.open(QIODevice::ReadOnly));
        QXmlStreamReader xml(&file);
        QString source;
        QString translated;
        while (!xml.atEnd()) {
            xml.readNext();
            if (xml.isStartElement() && xml.name() == QLatin1String("source")) {
                source = xml.readElementText();
            } else if (xml.isStartElement() && xml.name() == QLatin1String("translation")) {
                translated = xml.readElementText();
            } else if (xml.isEndElement() && xml.name() == QLatin1String("message")) {
                QVERIFY2(!allSources.contains(source), qPrintable(source));
                allSources.insert(source);
                QVERIFY2(!translated.isEmpty(), qPrintable(source));
                QCOMPARE(compiled.translate("VehicleParameters", source.toUtf8().constData()), translated);
                QCOMPARE(VehicleMessageTranslator::translateLocal(source), translated);
                if (source.size() > 100) {
                    QCOMPARE(ids(source), ids(translated));
                }
            }
        }
        QVERIFY(!xml.hasError());
    }
    QCOMPARE(allSources.size(), 16638);
}

void VehicleMessageTranslatorTest::_languageSwitch()
{
    const QString text = QStringLiteral("Preflight Fail: Strong magnetic interference");
    const QString translated = QStringLiteral("飞行前检查失败：磁场干扰过强");
    QCOMPARE(VehicleMessageTranslator::translate(text), translated);
    VehicleMessageTranslator::setLanguage(QLocale(QLocale::English));
    QVERIFY(QCoreApplication::removeTranslator(&_translator));
    const auto restore = qScopeGuard([this]() {
        QCoreApplication::installTranslator(&_translator);
        VehicleMessageTranslator::setLanguage(QLocale(QLocale::Chinese));
    });
    QCOMPARE(VehicleMessageTranslator::translate(text), text);
    QCOMPARE(VehicleMessageTranslator::translate(QStringLiteral("- Low battery\n  Low battery")),
             QStringLiteral("- Low battery\n  Low battery"));
    QVERIFY(QCoreApplication::installTranslator(&_translator));
    QCOMPARE(VehicleMessageTranslator::translate(text), translated);
}

void VehicleMessageTranslatorTest::_healthProblemAndDialog()
{
    HealthAndArmingCheckProblem problem(
        QStringLiteral("Strong magnetic interference"),
        QStringLiteral("Set <a href=\"param://COM_ARM_MAG_STR\">COM_ARM_MAG_STR</a> to 0"), QStringLiteral("error"));
    QCOMPARE(problem.message(), QStringLiteral("磁场干扰过强"));
    QCOMPARE(problem.description(),
             QStringLiteral("将 <a href=\"param://COM_ARM_MAG_STR\">COM_ARM_MAG_STR</a> 设置为 0"));
    QCOMPARE(problem.severity(), QStringLiteral("error"));
    expectAppMessage(QRegularExpression(QStringLiteral("飞行前检查失败：磁场干扰过强")));
    qgcApp()->showAppMessage(QStringLiteral("Preflight Fail: Strong magnetic interference"));
    verifyExpectedLogMessage();
}

UT_REGISTER_TEST(VehicleMessageTranslatorTest, TestLabel::Unit)
