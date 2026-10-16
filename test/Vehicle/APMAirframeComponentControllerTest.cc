#include "APMAirframeComponentControllerTest.h"

#include <QtCore/QFile>
#include <QtCore/QRegularExpression>
#include <QtCore/QTemporaryDir>
#include <QtGui/QGuiApplication>

#include "APMAirframeComponentController.h"
#include "Fact.h"

APMAirframeComponentControllerTest::APMAirframeComponentControllerTest()
{
    setWaitForParameters(true);
}

void APMAirframeComponentControllerTest::_frameClassImagesAndDefaults_data()
{
    QTest::addColumn<bool>("copter");
    QTest::addColumn<int>("frameClass");
    QTest::addColumn<QVariantList>("frameTypes");
    QTest::addColumn<int>("defaultFrameType");
    QTest::addColumn<QString>("defaultImage");
    QTest::addColumn<QString>("category");

    QTest::newRow("quad-prefers-x") << true << 1 << QVariantList{0, 1} << 1 << QStringLiteral("QuadRotorX")
                                    << QStringLiteral("multirotor");
    QTest::newRow("quad-without-x") << true << 1 << QVariantList{0} << 0 << QStringLiteral("QuadRotorPlus")
                                    << QStringLiteral("multirotor");
    QTest::newRow("quad-without-supported-type")
        << true << 1 << QVariantList{99} << -1 << QStringLiteral("QuadRotorX") << QStringLiteral("multirotor");
    QTest::newRow("rover-without-frame-type")
        << false << 1 << QVariantList{} << -1 << QStringLiteral("Rover") << QStringLiteral("rover");
    QTest::newRow("boat-without-frame-type")
        << false << 2 << QVariantList{} << -1 << QStringLiteral("Boat") << QStringLiteral("other");
    QTest::newRow("unmapped-rover-class")
        << false << 3 << QVariantList{} << -1 << QStringLiteral("AirframeUnknown") << QStringLiteral("rover");
}

void APMAirframeComponentControllerTest::_frameClassImagesAndDefaults()
{
    QFETCH(bool, copter);
    QFETCH(int, frameClass);
    QFETCH(QVariantList, frameTypes);
    QFETCH(int, defaultFrameType);
    QFETCH(QString, defaultImage);
    QFETCH(QString, category);

    Fact frameType(1, QStringLiteral("FRAME_TYPE"), FactMetaData::valueTypeInt32, this);
    QStringList frameTypeNames;
    for (const QVariant& value : frameTypes) {
        frameTypeNames.append(value.toString());
    }
    frameType.setEnumInfo(frameTypeNames, frameTypes);
    QSignalSpy writeSpy(&frameType, &Fact::containerRawValueChanged);
    QVERIFY(writeSpy.isValid());

    APMFrameClass frame(QStringLiteral("Frame"), copter, frameClass, copter ? &frameType : nullptr, this);
    QCOMPARE(frame.category(), category);
    QCOMPARE(frame._defaultFrameType, defaultFrameType);
    QCOMPARE(frame._imageResourceDefault, QStringLiteral("/qmlimages/Airframe/%1.svg").arg(defaultImage));
    QVERIFY2(QFile::exists(QStringLiteral(":") + frame._imageResourceDefault), qPrintable(frame._imageResourceDefault));
    QVERIFY2(QFile::exists(QStringLiteral(":") + frame.imageResource()), qPrintable(frame.imageResource()));
    QCOMPARE(frame._frameTypeSupported, defaultFrameType != -1);
    if (defaultFrameType != -1) {
        QVERIFY(frameTypes.contains(defaultFrameType));
    }
    QCOMPARE(writeSpy.count(), 0);
}

void APMAirframeComponentControllerTest::_downloadCompleteSlotsRestoreCursor()
{
    // The test explicitly injects download and parse errors; all resulting showAppMessage logs are expected.
    ignoreLogMessage("API.QGCApplication.AppMessage", QtDebugMsg,
                     QRegularExpression("Param file.*failed"));
    // Invalid JSON parse warning from APMAirframeComponentController is expected.
    ignoreLogMessage("AutoPilotPlugins.APMAirframeComponentController", QtWarningMsg,
                     QRegularExpression("Unable to open json document"));
    // Empty URL warning from QGCFileDownload when download_url is missing from JSON.
    ignoreLogMessage("Utilities.QGCFileDownload", QtWarningMsg,
                     QRegularExpression("Empty URL provided"));
    APMAirframeComponentController controller(this);

    // Failure path for github metadata download should restore wait cursor.
    QGuiApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    controller._githubJsonDownloadComplete(false, QString(), QStringLiteral("simulated download error"));
    QVERIFY(QGuiApplication::overrideCursor() == nullptr);

    // Success path with invalid JSON should also restore wait cursor.
    QTemporaryDir tempDir;
    QVERIFY(tempDir.isValid());
    const QString invalidJsonFile = tempDir.path() + QStringLiteral("/invalid.json");
    QFile jsonFile(invalidJsonFile);
    QVERIFY(jsonFile.open(QIODevice::WriteOnly | QIODevice::Truncate));
    jsonFile.write("{ invalid json");
    jsonFile.close();

    QGuiApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    controller._githubJsonDownloadComplete(true, invalidJsonFile, QString());
    QVERIFY(QGuiApplication::overrideCursor() == nullptr);

    // Success path with missing download_url should fail start and restore wait cursor.
    const QString missingDownloadUrlJsonFile = tempDir.path() + QStringLiteral("/missing_download_url.json");
    QFile missingUrlFile(missingDownloadUrlJsonFile);
    QVERIFY(missingUrlFile.open(QIODevice::WriteOnly | QIODevice::Truncate));
    missingUrlFile.write("{}");
    missingUrlFile.close();

    QGuiApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    controller._githubJsonDownloadComplete(true, missingDownloadUrlJsonFile, QString());
    QVERIFY(QGuiApplication::overrideCursor() == nullptr);

    // Parameter file download failure should restore wait cursor.
    QGuiApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    controller._paramFileDownloadComplete(false, QString(), QStringLiteral("simulated param download error"));
    QVERIFY(QGuiApplication::overrideCursor() == nullptr);
}

UT_REGISTER_TEST(APMAirframeComponentControllerTest, TestLabel::Integration, TestLabel::Vehicle)
