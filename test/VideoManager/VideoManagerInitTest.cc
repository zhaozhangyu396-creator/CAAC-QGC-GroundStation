#include "VideoManagerInitTest.h"

#ifdef QGC_GST_STREAMING

#include <QtCore/QRegularExpression>
#include <QtCore/QScopeGuard>
#include <QtQuick/QQuickWindow>

#include "SettingsManager.h"
#include "VideoManager.h"
#include "VideoSettings.h"

void VideoManagerInitTest::init()
{
    UnitTest::init();

    static const QRegularExpression sGStreamerCriticalRe(
        QStringLiteral("cannot register existing type|"
                       "g_type_add_interface_static.*G_TYPE_IS_INSTANTIATABLE|"
                       "g_once_init_leave.*result != 0|"
                       "GStreamer initialization failed"));
    // These backend startup diagnostics are platform/runtime dependent and not
    // tied to a single deterministic call site in this fixture.
    ignoreLogMessage("Video.GStreamer.GStreamerLogging", QtCriticalMsg, sGStreamerCriticalRe);
}

void VideoManagerInitTest::_testQmlReadyBeforeBackendReady()
{
    VideoManager videoManager;
    QQuickWindow mainWindow;
    videoManager._mainWindow = &mainWindow;

    int createReceiversCount = 0;
    videoManager._createVideoReceiversForTest = [&createReceiversCount]() {
        ++createReceiversCount;
    };

    videoManager._initState = VideoManager::InitState::Pending;

    videoManager._initAfterQmlIsReady();
    QCOMPARE(videoManager._initState, VideoManager::InitState::QmlReady);
    QCOMPARE(createReceiversCount, 0);

    videoManager._onBackendInitComplete(true);
    QCOMPARE(videoManager._initState, VideoManager::InitState::Running);
    QCOMPARE(createReceiversCount, 1);

    expectLogMessage("Video.VideoManager", QtWarningMsg, QRegularExpression(QStringLiteral("_onBackendInitComplete: unexpected state")));
    videoManager._onBackendInitComplete(true);
    verifyExpectedLogMessage();
    QCOMPARE(createReceiversCount, 1);
}

void VideoManagerInitTest::_testBackendReadyBeforeQmlReady()
{
    VideoManager videoManager;
    QQuickWindow mainWindow;
    videoManager._mainWindow = &mainWindow;

    int createReceiversCount = 0;
    videoManager._createVideoReceiversForTest = [&createReceiversCount]() {
        ++createReceiversCount;
    };

    videoManager._initState = VideoManager::InitState::Pending;

    videoManager._onBackendInitComplete(true);
    QCOMPARE(videoManager._initState, VideoManager::InitState::BackendReady);
    QCOMPARE(createReceiversCount, 0);

    videoManager._initAfterQmlIsReady();
    QCOMPARE(videoManager._initState, VideoManager::InitState::Running);
    QCOMPARE(createReceiversCount, 1);

    expectLogMessage("Video.VideoManager", QtWarningMsg, QRegularExpression(QStringLiteral("_initAfterQmlIsReady: unexpected state")));
    videoManager._initAfterQmlIsReady();
    verifyExpectedLogMessage();
    QCOMPARE(createReceiversCount, 1);
}

void VideoManagerInitTest::_testBackendInitFailure()
{
    VideoManager videoManager;
    QQuickWindow mainWindow;
    videoManager._mainWindow = &mainWindow;

    int createReceiversCount = 0;
    videoManager._createVideoReceiversForTest = [&createReceiversCount]() {
        ++createReceiversCount;
    };

    videoManager._initState = VideoManager::InitState::Pending;

    expectLogMessage("Video.VideoManager", QtCriticalMsg, QRegularExpression(QStringLiteral("video initialization failed")));
    videoManager._onBackendInitComplete(false);
    verifyExpectedLogMessage();
    QCOMPARE(videoManager._initState, VideoManager::InitState::Failed);
    QCOMPARE(createReceiversCount, 0);

    expectLogMessage("Video.VideoManager", QtWarningMsg, QRegularExpression(QStringLiteral("QML ready but video init failed")));
    videoManager._initAfterQmlIsReady();
    verifyExpectedLogMessage();
    QCOMPARE(videoManager._initState, VideoManager::InitState::Failed);
    QCOMPARE(createReceiversCount, 0);
}

void VideoManagerInitTest::_testDisabledBackend_data()
{
    QTest::addColumn<bool>("qmlReady");
    QTest::addColumn<bool>("lateSuccess");
    QTest::newRow("backend-pending-late-success") << false << true;
    QTest::newRow("backend-pending-late-failure") << false << false;
    QTest::newRow("qml-ready-late-success") << true << true;
    QTest::newRow("qml-ready-late-failure") << true << false;
}

void VideoManagerInitTest::_testDisabledBackend()
{
    QFETCH(bool, qmlReady);
    QFETCH(bool, lateSuccess);
    auto* settings = SettingsManager::instance()->videoSettings();
    const QVariant previousSource = settings->videoSource()->rawValue();
    const QVariant previousEnabled = settings->streamEnabled()->rawValue();
    const auto restoreSettings = qScopeGuard([settings, previousSource, previousEnabled]() {
        settings->videoSource()->setRawValue(previousSource);
        settings->streamEnabled()->setRawValue(previousEnabled);
    });
    settings->videoSource()->setRawValue(VideoSettings::videoSourceUDPH264);
    settings->streamEnabled()->setRawValue(true);

    VideoManager videoManager;
    QQuickWindow mainWindow;
    videoManager._backendDisabledForTests = false;
    videoManager._initState = qmlReady ? VideoManager::InitState::QmlReady : VideoManager::InitState::Pending;
    int createReceiversCount = 0;
    videoManager._createVideoReceiversForTest = [&createReceiversCount]() { ++createReceiversCount; };
    QVERIFY(videoManager.hasVideo());

    videoManager.disableVideoForSession();
    QVERIFY(!videoManager.hasVideo());
    QVERIFY(!videoManager.waitForVideoBackendReady());
    videoManager.startVideoBackendInit();
    videoManager.init(&mainWindow);
    videoManager._onBackendInitComplete(lateSuccess);
    videoManager._initAfterQmlIsReady();
    QCOMPARE(videoManager._initState, VideoManager::InitState::Disabled);
    QCOMPARE(createReceiversCount, 0);
    QVERIFY(videoManager._videoReceivers.isEmpty());
    QCOMPARE(settings->videoSource()->rawValue(), QVariant(VideoSettings::videoSourceUDPH264));
    QCOMPARE(settings->streamEnabled()->rawValue(), QVariant(true));
}

#else

void VideoManagerInitTest::init()
{
    UnitTest::init();
    QSKIP("GStreamer not enabled");
}

void VideoManagerInitTest::_testQmlReadyBeforeBackendReady()
{
    QSKIP("GStreamer not enabled");
}

void VideoManagerInitTest::_testBackendReadyBeforeQmlReady()
{
    QSKIP("GStreamer not enabled");
}

void VideoManagerInitTest::_testBackendInitFailure()
{
    QSKIP("GStreamer not enabled");
}

void VideoManagerInitTest::_testDisabledBackend_data()
{
    QSKIP("GStreamer not enabled");
}

void VideoManagerInitTest::_testDisabledBackend()
{
    QSKIP("GStreamer not enabled");
}

#endif

UT_REGISTER_TEST(VideoManagerInitTest, TestLabel::Unit)
