#include "FirmwarePluginTest.h"

#include <QtCore/QCoreApplication>
#include <QtCore/QScopeGuard>
#include <QtCore/QString>
#include <QtCore/QTranslator>

#include "FirmwarePlugin.h"
#include "PX4FirmwarePlugin.h"
#include "px4_custom_mode.h"

namespace {
class ModeTranslator : public QTranslator
{
public:
    bool isEmpty() const override { return false; }

    QString translate(const char* context, const char* sourceText, const char*, int) const override
    {
        if (QByteArray(context) == "PX4FirmwarePlugin") {
            return QStringLiteral("本地化-%1").arg(QString::fromUtf8(sourceText));
        }
        return {};
    }
};
}  // namespace

void FirmwarePluginTest::_stableFirmwareNotifiedOncePerVersion_data()
{
    QTest::addColumn<QString>("latest");
    QTest::addColumn<QString>("seen");
    QTest::addColumn<bool>("expected");

    QTest::newRow("never seen") << "4.6.2" << "" << true;
    QTest::newRow("already seen") << "4.6.2" << "4.6.2" << false;
    QTest::newRow("newer than seen") << "4.6.3" << "4.6.2" << true;
    QTest::newRow("older than seen") << "4.6.1" << "4.6.2" << false;
    QTest::newRow("corrupt setting") << "4.6.2" << "junk" << true;
    QTest::newRow("unparseable latest") << "junk" << "" << false;
}

void FirmwarePluginTest::_stableFirmwareNotifiedOncePerVersion()
{
    QFETCH(QString, latest);
    QFETCH(QString, seen);
    QFETCH(bool, expected);

    QCOMPARE(FirmwarePlugin::isStableFirmwareVersionUnseen(latest, seen), expected);
}

void FirmwarePluginTest::_px4DynamicModesTranslateAndPreserveCommands_data()
{
    QTest::addColumn<QString>("reportedName");
    QTest::addColumn<QString>("translatedSource");
    QTest::addColumn<uint32_t>("wireMode");
    QTest::addColumn<bool>("selectable");

    QTest::newRow("manual") << QStringLiteral("Manual") << QStringLiteral("Manual") << uint32_t(PX4CustomMode::MANUAL)
                            << true;
    QTest::newRow("stabilized") << QStringLiteral("Stabilized") << QStringLiteral("Stabilized")
                                << uint32_t(PX4CustomMode::STABILIZED) << true;
    QTest::newRow("acro") << QStringLiteral("Acro") << QStringLiteral("Acro") << uint32_t(PX4CustomMode::ACRO) << true;
    QTest::newRow("altitude") << QStringLiteral("Altitude") << QStringLiteral("Altitude")
                              << uint32_t(PX4CustomMode::ALTCTL) << true;
    QTest::newRow("position") << QStringLiteral("Position") << QStringLiteral("Position")
                              << uint32_t(PX4CustomMode::POSCTL_POSCTL) << true;
    QTest::newRow("hold") << QStringLiteral("Hold") << QStringLiteral("Hold") << uint32_t(PX4CustomMode::AUTO_LOITER)
                          << true;
    QTest::newRow("mission") << QStringLiteral("Mission") << QStringLiteral("Mission")
                             << uint32_t(PX4CustomMode::AUTO_MISSION) << true;
    QTest::newRow("return") << QStringLiteral("Return") << QStringLiteral("Return") << uint32_t(PX4CustomMode::AUTO_RTL)
                            << true;
    QTest::newRow("precision landing") << QStringLiteral("Precision Landing") << QStringLiteral("Precision Land")
                                       << uint32_t(PX4CustomMode::AUTO_PRECLAND) << false;
    QTest::newRow("mock mode") << QStringLiteral("MockLink Mode") << QString() << uint32_t(PX4CustomMode::RATTITUDE)
                               << false;
    QTest::newRow("unknown mode retained") << QStringLiteral("Vendor Mode") << QString() << uint32_t(0xFEFE) << true;
    QTest::newRow("offboard removed") << QStringLiteral("Offboard") << QString() << uint32_t(PX4CustomMode::OFFBOARD)
                                      << false;
    QTest::newRow("delayed mock removed") << QStringLiteral("MockLink Mode (delayed)") << QString()
                                          << uint32_t(PX4CustomMode::AUTO_FOLLOW_TARGET) << false;
}

void FirmwarePluginTest::_px4DynamicModesTranslateAndPreserveCommands()
{
    QFETCH(QString, reportedName);
    QFETCH(QString, translatedSource);
    QFETCH(uint32_t, wireMode);
    QFETCH(bool, selectable);
    ModeTranslator translator;
    QVERIFY(QCoreApplication::installTranslator(&translator));
    const auto restoreTranslator = qScopeGuard([&translator] { QCoreApplication::removeTranslator(&translator); });
    PX4FirmwarePlugin plugin;
    FlightModeList modes{{reportedName, uint8_t(0), wireMode, true, true, true, true}};
    plugin.updateAvailableFlightModes(modes);
    const QString expectedName =
        translatedSource.isEmpty() ? reportedName : QStringLiteral("本地化-%1").arg(translatedSource);
    QCOMPARE(plugin.flightMode(MAV_MODE_FLAG_CUSTOM_MODE_ENABLED, wireMode), expectedName);
    QCOMPARE(modes.first().mode_name, expectedName);
    QCOMPARE(modes.first().custom_mode, wireMode);
    QCOMPARE(modes.first().canBeSet, selectable);
    QVERIFY(modes.first().advanced);
    uint8_t baseMode = 0;
    uint32_t customMode = 0;
    QVERIFY(plugin.setFlightMode(expectedName, &baseMode, &customMode));
    QCOMPARE(baseMode, uint8_t(MAV_MODE_FLAG_CUSTOM_MODE_ENABLED));
    QCOMPARE(customMode, wireMode);
    plugin.updateAvailableFlightModes(modes);
    QCOMPARE(plugin.flightMode(baseMode, customMode), expectedName);
}

UT_REGISTER_TEST(FirmwarePluginTest, TestLabel::Unit, TestLabel::Vehicle)
