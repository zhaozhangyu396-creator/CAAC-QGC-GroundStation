#include <QtCore/QDir>
#include <QtCore/QPointer>
#include <QtCore/QScopeGuard>
#include <QtCore/QTemporaryDir>
#include <QtGui/QClipboard>
#include <QtGui/QGuiApplication>
#include <QtGui/QImage>
#include <QtQuick/QQuickItem>
#include <QtQuick/QQuickWindow>
#include <QtTest/QSignalSpy>

#include "CaacActivationController.h"
#include "CaacActivationTestSupport.h"
#include "LinkManager.h"
#include "QGCCorePlugin.h"
#include "QmlUITestBase.h"
#include "UnitTest.h"

class CaacActivationUITest : public UnitTest
{
    Q_OBJECT
private slots:
    void _lockedUntilValidActivation();
};

void CaacActivationUITest::_lockedUntilValidActivation()
{
    QTemporaryDir dir;
    QVERIFY(dir.isValid());
    const auto key = CaacActivationTestSupport::publicKey();
    const QByteArray identity("activation-native-ui-machine");
    CaacActivationController controller(key, identity, dir.filePath(QStringLiteral("activation.license")));
    QObject gateLifetime;
    const bool previousQuit = QGuiApplication::quitOnLastWindowClosed();
    QGuiApplication::setQuitOnLastWindowClosed(false);
    const auto restoreQuit =
        qScopeGuard([previousQuit]() { QGuiApplication::setQuitOnLastWindowClosed(previousQuit); });
    int starts = 0;
    QVERIFY(controller.showWindow(QGCCorePlugin::instance(), &gateLifetime, [&starts]() { ++starts; }));
    QPointer<QQuickWindow> window;
    for (auto* candidate : QGuiApplication::topLevelWindows()) {
        if (candidate->objectName() == QStringLiteral("caacActivationWindow")) {
            window = qobject_cast<QQuickWindow*>(candidate);
            break;
        }
    }
    QVERIFY(window);
    QVERIFY(QTest::qWaitForWindowExposed(window));
    const auto find = [&window](const char* name) {
        return QmlUITestBase::findItem(window->contentItem(), QLatin1String(name));
    };
    const auto click = [&window](QQuickItem* item) {
        if (!item || !item->isVisible() || !item->isEnabled()) {
            return false;
        }
        QTest::mouseClick(window, Qt::LeftButton, Qt::NoModifier,
                          item->mapToScene(QPointF(item->width() / 2, item->height() / 2)).toPoint());
        return true;
    };
    const auto preview = [&window](const QString& name) {
        const auto path = qEnvironmentVariable("CAAC_ACTIVATION_PREVIEW_DIR");
        if (path.isEmpty()) {
            return true;
        }
        QDir().mkpath(path);
        QSignalSpy frame(window, &QQuickWindow::frameSwapped);
        window->update();
        if (!frame.wait(TestTimeout::mediumMs())) {
            return false;
        }
        return window->grabWindow().save(path + QLatin1Char('/') + name + QStringLiteral(".png"));
    };
    QCOMPARE(starts, 0);
    QVERIFY(LinkManager::instance()->links().isEmpty());
    QVERIFY(!find("mainView_plan"));
    auto* input = find("caacActivationCodeInput");
    auto* submit = find("caacActivationSubmit");
    QVERIFY(input && submit);
    QVERIFY(!submit->isEnabled());
    QCOMPARE(find("caacActivationMachineCode")->property("text").toString(), controller.machineCode());
    QVERIFY(click(find("caacActivationCopyMachineCode")));
    QCOMPARE(QGuiApplication::clipboard()->text(), controller.machineCode());
    QVERIFY(preview(QStringLiteral("activation-locked")));
    QVERIFY(input->setProperty("text", QStringLiteral("wrong-code")));
    QVERIFY(click(submit));
    QVERIFY(!controller.activated());
    QCOMPARE(starts, 0);
    QVERIFY(find("caacActivationError")->isVisible());
    auto* error = find("caacActivationError");
    const auto errorBounds = error->mapRectToScene(error->boundingRect());
    QVERIFY(QRectF(0, 0, window->width(), window->height()).contains(errorBounds));
    for (auto* ancestor = error->parentItem(); ancestor; ancestor = ancestor->parentItem()) {
        if (ancestor->clip()) {
            QVERIFY(ancestor->mapRectToScene(ancestor->boundingRect()).contains(errorBounds));
        }
    }
    QVERIFY(preview(QStringLiteral("activation-invalid")));
    QVERIFY(input->setProperty("text", CaacActivationTestSupport::activationCode(identity)));
    QVERIFY(click(submit));
    QTRY_COMPARE_WITH_TIMEOUT(starts, 1, TestTimeout::mediumMs());
    QVERIFY(controller.activated());
    QTRY_VERIFY_WITH_TIMEOUT(window.isNull(), TestTimeout::mediumMs());
    CaacActivationController restart(key, identity, dir.filePath(QStringLiteral("activation.license")));
    QVERIFY(restart.activated());
    QVERIFY(!restart.showWindow(QGCCorePlugin::instance(), this, [&starts]() { ++starts; }));
    QCOMPARE(starts, 1);
}

UT_REGISTER_TEST(CaacActivationUITest, TestLabel::Integration, TestLabel::Utilities)

#include "CaacActivationUITest.moc"
