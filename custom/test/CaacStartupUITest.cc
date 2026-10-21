#include <QtCore/QPointer>
#include <QtCore/QScopeGuard>
#include <QtGui/QGuiApplication>
#include <QtQuick/QQuickItem>
#include <QtQuick/QQuickWindow>
#include <QtTest/QSignalSpy>

#include "CaacStartupController.h"
#include "LinkManager.h"
#include "QmlUITestBase.h"
#include "UnitTest.h"

class CaacStartupUITest : public UnitTest
{
    Q_OBJECT
private slots:
    void _readyMilestones_data();
    void _readyMilestones();
    void _failureAndCancellationDoNotReveal();
};

void CaacStartupUITest::_readyMilestones_data()
{
    QTest::addColumn<QList<int>>("order");
    QTest::addColumn<bool>("videoAvailable");
    const auto addOrder = [](const char* name, const QList<int>& order) {
        QTest::newRow(name) << order << true;
        QTest::newRow(qPrintable(QString::fromLatin1(name) + QStringLiteral("-video-unavailable"))) << order << false;
    };
    addOrder("backend-services-frame", {0, 1, 2});
    addOrder("backend-frame-services", {0, 2, 1});
    addOrder("services-backend-frame", {1, 0, 2});
    addOrder("services-frame-backend", {1, 2, 0});
    addOrder("frame-backend-services", {2, 0, 1});
    addOrder("frame-services-backend", {2, 1, 0});
}

void CaacStartupUITest::_readyMilestones()
{
    QFETCH(QList<int>, order);
    QFETCH(bool, videoAvailable);
    const bool previousQuit = QGuiApplication::quitOnLastWindowClosed();
    QGuiApplication::setQuitOnLastWindowClosed(false);
    const auto restoreQuit =
        qScopeGuard([previousQuit]() { QGuiApplication::setQuitOnLastWindowClosed(previousQuit); });
    CaacStartupController startup;
    QVERIFY(startup.show());
    QPointer<QQuickWindow> window;
    for (auto* candidate : QGuiApplication::topLevelWindows()) {
        if (candidate->objectName() == QStringLiteral("caacStartupWindow")) {
            window = qobject_cast<QQuickWindow*>(candidate);
            break;
        }
    }
    QVERIFY(window);
    QVERIFY(QTest::qWaitForWindowExposed(window));
    QVERIFY(LinkManager::instance()->links().isEmpty());
    auto* artwork = QmlUITestBase::findItem(window->contentItem(), QStringLiteral("caacStartupArtwork"));
    QVERIFY(artwork);
    QCOMPARE(artwork->property("source").toUrl(), QUrl(QStringLiteral("qrc:/CAAC/Brand/startup-background.png")));
    QVERIFY(artwork->parentItem()->clip());
    QVERIFY(artwork->parentItem()->height() > 0);
    QVERIFY(artwork->parentItem()->height() < artwork->height() * 0.8);
    startup.setStage(CaacStartupController::Stage::FirstFrame);
    QCOMPARE(startup.progress(), 95);
    QSignalSpy finished(&startup, &CaacStartupController::finishedChanged);
    const auto mark = [&startup, videoAvailable](int milestone) {
        if (milestone == 0) {
            startup.completeVideoInitialization(videoAvailable);
        } else if (milestone == 1) {
            startup.markServicesReady();
        } else {
            startup.markMainFrameReady();
        }
    };
    for (int i = 0; i < 2; ++i) {
        mark(order[i]);
        QSignalSpy frame(window, &QQuickWindow::frameSwapped);
        window->update();
        QVERIFY(frame.wait(TestTimeout::mediumMs()));
        QCOMPARE(startup.progress(), 95);
        QVERIFY(!startup.finished());
        QVERIFY(window->isVisible());
    }
    mark(order[2]);
    QCOMPARE(startup.progress(), 100);
    QCOMPARE(startup.videoAvailable(), videoAvailable);
    QVERIFY(!startup.failed());
    QCOMPARE(startup.statusText(),
             videoAvailable ? QStringLiteral("系统已就绪") : QStringLiteral("系统已就绪，图传暂不可用"));
    QVERIFY(!startup.finished());
    QTRY_VERIFY_WITH_TIMEOUT(startup.finished(), TestTimeout::mediumMs());
    QCOMPARE(finished.count(), 1);
    QVERIFY(!window->isVisible());
    startup.close();
    QVERIFY(window.isNull());
}

void CaacStartupUITest::_failureAndCancellationDoNotReveal()
{
    CaacStartupController failed;
    failed.setStage(CaacStartupController::Stage::VideoBackend);
    failed.fail(QStringLiteral("startup failure"));
    failed.completeVideoInitialization(false);
    failed.markServicesReady();
    failed.markMainFrameReady();
    QVERIFY(failed.failed());
    QVERIFY(!failed.finished());
    QCOMPARE(failed.progress(), 45);
    QCOMPARE(failed.statusText(), QStringLiteral("startup failure"));

    CaacStartupController cancelled;
    QSignalSpy close(&cancelled, &CaacStartupController::closeRequested);
    cancelled.requestClose();
    cancelled.requestClose();
    QCOMPARE(close.count(), 1);
    cancelled.completeVideoInitialization(false);
    cancelled.markServicesReady();
    cancelled.markMainFrameReady();
    QVERIFY(!cancelled.finished());
    QCOMPARE(cancelled.progress(), 0);
}

UT_REGISTER_TEST(CaacStartupUITest, TestLabel::Integration, TestLabel::Utilities)

#include "CaacStartupUITest.moc"
