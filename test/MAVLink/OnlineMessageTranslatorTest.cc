#include "OnlineMessageTranslatorTest.h"

#include <memory>

#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>
#include <QtCore/QScopeGuard>
#include <QtCore/QTranslator>
#include <QtNetwork/QNetworkAccessManager>
#include <QtQml/QQmlComponent>
#include <QtQml/QQmlEngine>
#include <QtTest/QSignalSpy>

#include "ColoredSvgImageProvider.h"
#include "Fact.h"
#include "Fixtures/LocalHttpTestServer.h"
#include "HealthAndArmingCheckReport.h"
#include "OnlineMessageTranslator.h"
#include "QGCApplication.h"
#include "StatusTextHandler.h"
#include "VehicleMessageTranslator.h"

namespace {
QByteArray response(const QString& text, int status = 200)
{
    return QJsonDocument(
               QJsonObject{{QStringLiteral("responseStatus"), status},
                           {QStringLiteral("responseData"), QJsonObject{{QStringLiteral("translatedText"), text}}}})
        .toJson(QJsonDocument::Compact);
}
}  // namespace

void OnlineMessageTranslatorTest::_roundTripAndCache()
{
    TestFixtures::LocalHttpTestServer server;
    QVERIFY(server.listen());
    server.installHttpResponder(response(QStringLiteral("自定义传感器 Q G C V 0 X 报告压力 QGCV1X")));
    OnlineMessageTranslator translator;
    translator._testEnabled = true;
    translator._endpoint = QUrl(server.url());
    QSignalSpy finished(translator._network, &QNetworkAccessManager::finished);
    QSignalSpy changed(&translator, &OnlineMessageTranslator::translationsChanged);
    const QString raw = QStringLiteral("Custom sensor GPS reports pressure 12.3 V");
    QCOMPARE(translator.translateUnknown(raw), raw);
    QCOMPARE(translator.translateUnknown(raw), raw);
    QCOMPARE(translator._pending.size(), 1);
    QTRY_COMPARE_WITH_TIMEOUT(changed.count(), 1, TestTimeout::mediumMs());
    QCOMPARE(finished.count(), 1);
    QCOMPARE(translator.translateUnknown(raw), QStringLiteral("自定义传感器 GPS 报告压力 12.3 V"));
    QCOMPARE(translator.translateUnknown(QStringLiteral("Custom sensor EKF reports pressure 9.6 V")),
             QStringLiteral("自定义传感器 EKF 报告压力 9.6 V"));
    QCOMPARE(translator.translateUnknown(QStringLiteral("Custom sensor GPS reports pressure 2.4e-3 mAh")),
             QStringLiteral("自定义传感器 GPS 报告压力 2.4e-3 mAh"));
    QCOMPARE(translator._pending.size(), 0);
    QCOMPARE(finished.count(), 1);
    translator._testEnabled = false;
    translator._settingsChanged();
    QCOMPARE(translator.translateUnknown(raw), raw);
}

void OnlineMessageTranslatorTest::_adjacentTokens()
{
    TestFixtures::LocalHttpTestServer server;
    QVERIFY(server.listen());
    server.installHttpResponder(response(QStringLiteral("自定义压力 QGCV0XQGCV1X")));
    OnlineMessageTranslator translator;
    translator._testEnabled = true;
    translator._endpoint = QUrl(server.url());
    QSignalSpy changed(&translator, &OnlineMessageTranslator::translationsChanged);
    const QString raw = QStringLiteral("Custom pressure SENSOR_ID 2");
    QCOMPARE(translator.translateUnknown(raw), raw);
    QTRY_COMPARE_WITH_TIMEOUT(changed.count(), 1, TestTimeout::mediumMs());
    QCOMPARE(translator.translateUnknown(raw), QStringLiteral("自定义压力 SENSOR_ID 2"));
}

void OnlineMessageTranslatorTest::_invalidResponse_data()
{
    QTest::addColumn<QByteArray>("body");
    QTest::newRow("missing-value") << response(QStringLiteral("压力异常"));
    QTest::newRow("duplicate-value") << response(QStringLiteral("压力 QGCV0X QGCV0X"));
    QTest::newRow("unknown-value") << response(QStringLiteral("压力 QGCV9X"));
    QTest::newRow("remote-markup") << response(QStringLiteral("<img src=x>压力 QGCV0X"));
    QTest::newRow("invented-value") << response(QStringLiteral("压力 100 QGCV0X"));
    QTest::newRow("still-english") << response(QStringLiteral("Pressure QGCV0X"));
    QTest::newRow("quota") << response(QStringLiteral("QUOTA EXCEEDED"), 429);
    QTest::newRow("malformed-json") << QByteArrayLiteral("invalid");
}

void OnlineMessageTranslatorTest::_invalidResponse()
{
    QFETCH(QByteArray, body);
    TestFixtures::LocalHttpTestServer server;
    QVERIFY(server.listen());
    server.installHttpResponder(body);
    OnlineMessageTranslator translator;
    translator._testEnabled = true;
    translator._endpoint = QUrl(server.url());
    QSignalSpy finished(translator._network, &QNetworkAccessManager::finished);
    QSignalSpy changed(&translator, &OnlineMessageTranslator::translationsChanged);
    const QString raw = QStringLiteral("Custom pressure 12.3 V");
    QCOMPARE(translator.translateUnknown(raw), raw);
    QTRY_COMPARE_WITH_TIMEOUT(finished.count(), 1, TestTimeout::mediumMs());
    QCOMPARE(changed.count(), 0);
    QCOMPARE(translator.translateUnknown(raw), raw);
    QCOMPARE(translator._pending.size(), 0);
    QVERIFY(translator._failedUntil.contains(QStringLiteral("Custom pressure QGCV0X")));
}

void OnlineMessageTranslatorTest::_disabledAndBounded()
{
    OnlineMessageTranslator translator;
    const QString raw = QStringLiteral("Unknown pressure warning");
    QCOMPARE(translator.translateUnknown(raw), raw);
    QCOMPARE(translator._pending.size(), 0);
    translator._testEnabled = true;
    for (const QString& text :
         {QStringLiteral("GPS EKF2 RC1_MIN 12.3 V"), QStringLiteral("气压异常"), QString(451, QLatin1Char('x'))}) {
        QCOMPARE(translator.translateUnknown(text), text);
        QCOMPARE(translator._pending.size(), 0);
    }
    for (int i = 0; i < 100; ++i) {
        translator.translateUnknown(raw + QChar('a' + i));
    }
    QCOMPARE(translator._pending.size(), 32);
    translator._testEnabled = false;
    translator._settingsChanged();
    QVERIFY(translator._queue.isEmpty());
    QVERIFY(translator._pending.isEmpty());
}

void OnlineMessageTranslatorTest::_timeout()
{
    TestFixtures::LocalHttpTestServer server;
    QVERIFY(server.listen());
    OnlineMessageTranslator translator;
    translator._testEnabled = true;
    translator._endpoint = QUrl(server.url());
    translator._requestTimeoutMs = 50;
    QSignalSpy finished(translator._network, &QNetworkAccessManager::finished);
    const QString raw = QStringLiteral("Custom sensor timeout warning");
    QCOMPARE(translator.translateUnknown(raw), raw);
    QTRY_COMPARE_WITH_TIMEOUT(finished.count(), 1, TestTimeout::mediumMs());
    QCOMPARE(translator.translateUnknown(raw), raw);
    QVERIFY(translator._active == nullptr);
    QVERIFY(translator._backoffUntil > QDateTime::currentMSecsSinceEpoch());
}

void OnlineMessageTranslatorTest::_longDescription()
{
    TestFixtures::LocalHttpTestServer server;
    QVERIFY(server.listen());
    server.installHttpResponder(response(QStringLiteral("参数说明译文")));
    OnlineMessageTranslator translator;
    translator._testEnabled = true;
    translator._endpoint = QUrl(server.url());
    QSignalSpy finished(translator._network, &QNetworkAccessManager::finished);
    const QString raw = QStringLiteral("The magnetometer accepts measurements only after startup. ").repeated(12);
    QCOMPARE(translator.translateUnknown(raw), raw);
    QVERIFY(translator._queue.size() > 1);
    const auto requestCount = translator._queue.size();
    QTRY_COMPARE_WITH_TIMEOUT(finished.count(), requestCount, TestTimeout::mediumMs());
    QVERIFY(!translator.translateUnknown(raw).contains(QStringLiteral("magnetometer")));
    QVERIFY(translator.translateUnknown(raw).contains(QStringLiteral("参数说明译文")));
}

void OnlineMessageTranslatorTest::_parameterMetadata()
{
    QTranslator chinese;
    QVERIFY(chinese.load(QStringLiteral("qgc_source_zh_CN"), QStringLiteral(":/i18n")));
    QVERIFY(QCoreApplication::installTranslator(&chinese));
    const auto reset = qScopeGuard([&chinese] { QCoreApplication::removeTranslator(&chinese); });
    Fact fact(1, QStringLiteral("EKF2_MAG_CHECK"), FactMetaData::valueTypeInt32);
    const QStringList options{QStringLiteral("Strength (EKF2_MAG_CHK_STR)"),
                              QStringLiteral("Inclination (EKF2_MAG_CHK_INC)"), QStringLiteral("Wait for WMM")};
    fact.setEnumInfo(options, {1, 2, 4});
    fact.setRawValue(2);
    fact.metaData()->setShortDescription(QStringLiteral("Magnetic field strength test selection"));
    auto* translator = OnlineMessageTranslator::instance();
    QCOMPARE(translator->parameterOptions(fact.enumStrings(), 0),
             QStringList({QStringLiteral("磁场强度（EKF2_MAG_CHK_STR）"), QStringLiteral("磁倾角（EKF2_MAG_CHK_INC）"),
                          QStringLiteral("等待世界磁场模型（WMM）")}));
    QCOMPARE(translator->parameterText(fact.shortDescription(), 0), QStringLiteral("磁场有效性检查选项"));
    QCOMPARE(fact.metaData()->shortDescription(), QStringLiteral("Magnetic field strength test selection"));
    QCOMPARE(fact.enumStrings(), options);
    QCOMPARE(fact.enumValues(), QVariantList({1, 2, 4}));
    QCOMPARE(fact.rawValue().toInt(), 2);
    QCOMPARE(fact.enumIndex(), 1);
    VehicleMessageTranslator::setLanguage(QLocale(QLocale::Chinese));
    translator->_testEnabled = true;
    const auto resetOnline = qScopeGuard([translator] {
        translator->_testEnabled = false;
        translator->_settingsChanged();
        VehicleMessageTranslator::setLanguage(qgcApp()->getCurrentLanguage());
    });
    const QString brand = QStringLiteral("Pixhawk/Pixracer/Navio2/Pixhawk2_PM1");
    QCOMPARE(translator->parameterText(brand, 0), brand);
    const QString chineseText = QStringLiteral("当前使用 Pixhawk 飞控");
    QCOMPARE(translator->parameterText(chineseText, 0), chineseText);
    QVERIFY(translator->_pending.isEmpty());
}

void OnlineMessageTranslatorTest::_parameterSelectionSurvivesTranslation()
{
    VehicleMessageTranslator::setLanguage(QLocale(QLocale::English));
    const auto restore = qScopeGuard([] { VehicleMessageTranslator::setLanguage(qgcApp()->getCurrentLanguage()); });
    Fact fact(1, QStringLiteral("CUSTOM_SELECTOR"), FactMetaData::valueTypeInt32);
    const QStringList options{QStringLiteral("Manual roll scale"), QStringLiteral("Manual pitch scale")};
    fact.setEnumInfo(options, {1, 4});
    fact.setRawValue(1);
    QQmlEngine engine;
    engine.addImageProvider(QLatin1String(ColoredSvgImageProvider::ProviderId), new ColoredSvgImageProvider());
    engine.addImportPath(QStringLiteral("qrc:/qml"));
    QQmlComponent component(&engine);
    component.setData("import QtQuick; import QGroundControl.FactControls; FactComboBox { deferred: true }", QUrl());
    std::unique_ptr<QObject> control(
        component.createWithInitialProperties({{QStringLiteral("fact"), QVariant::fromValue(&fact)}}));
    QVERIFY2(control, qPrintable(component.errorString()));
    QTRY_COMPARE(control->property("currentIndex").toInt(), 0);
    QVERIFY(QMetaObject::invokeMethod(control.get(), "activated", Q_ARG(int, 1)));
    QCOMPARE(fact.rawValue().toInt(), 1);
    QTRY_COMPARE(control->property("currentIndex").toInt(), 1);
    VehicleMessageTranslator::setLanguage(QLocale(QLocale::Chinese));
    auto* translator = OnlineMessageTranslator::instance();
    ++translator->_revision;
    emit translator->translationsChanged();
    QTRY_COMPARE(control->property("currentText").toString(), QStringLiteral("手动俯仰输入比例"));
    QCOMPARE(control->property("currentIndex").toInt(), 1);
    QCOMPARE(fact.rawValue().toInt(), 1);
    QCOMPARE(fact.enumStrings(), options);
    QVERIFY(QMetaObject::invokeMethod(control.get(), "applyPendingValue"));
    QCOMPARE(fact.rawValue().toInt(), 4);
}

void OnlineMessageTranslatorTest::_refreshExistingMessages()
{
    TestFixtures::LocalHttpTestServer server;
    QVERIFY(server.listen());
    server.installHttpResponder(response(QStringLiteral("无人机：自定义传感器压力 QGCV0X")));
    auto* translator = OnlineMessageTranslator::instance();
    const QUrl endpoint = translator->_endpoint;
    translator->_endpoint = QUrl(server.url());
    translator->_testEnabled = true;
    translator->_nextRequestAt = 0;
    const auto reset = qScopeGuard([translator, endpoint] {
        translator->_testEnabled = false;
        translator->_settingsChanged();
        translator->_endpoint = endpoint;
        translator->_cache.clear();
    });
    StatusTextHandler handler;
    QSignalSpy newMessage(&handler, &StatusTextHandler::newFormattedMessage);
    QSignalSpy refreshed(&handler, &StatusTextHandler::formattedMessagesChanged);
    QSignalSpy errors(&handler, &StatusTextHandler::newErrorMessage);
    const QString raw = QStringLiteral("无人机：Custom sensor pressure 12.3 V");
    HealthAndArmingCheckProblem problem(raw, {}, QStringLiteral("error"));
    QSignalSpy healthChanged(&problem, &HealthAndArmingCheckProblem::translationChanged);
    handler.handleHTMLEscapedTextMessage(MAV_COMP_ID_AUTOPILOT1, MAV_SEVERITY_CRITICAL, raw, {});
    QCOMPARE(handler.messages().size(), 1);
    QCOMPARE(errors.count(), 1);
    QCOMPARE(handler.criticalMessageCount(), uint32_t(1));
    QTRY_COMPARE_WITH_TIMEOUT(healthChanged.count(), 1, TestTimeout::mediumMs());
    QCOMPARE(refreshed.count(), 1);
    QCOMPARE(newMessage.count(), 1);
    QCOMPARE(handler.messages().size(), 1);
    QCOMPARE(handler.messages().first()->getText(), raw);
    QCOMPARE(handler.criticalMessageCount(), uint32_t(1));
    QCOMPARE(errors.count(), 1);
    QCOMPARE(problem.message(), QStringLiteral("无人机：自定义传感器压力 12.3 V"));
    QVERIFY(handler.formattedMessages().contains(problem.message()));
    QVERIFY(handler.formattedMessages().contains(QStringLiteral("在线翻译 · 原文：") + raw));
}

UT_REGISTER_TEST(OnlineMessageTranslatorTest, TestLabel::Unit)
