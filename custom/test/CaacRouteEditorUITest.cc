#include <cmath>
#include <cstdio>
#include <limits>
#include <memory>

#include <QtCore/QByteArray>
#include <QtCore/QFile>
#include <QtCore/QPointer>
#include <QtCore/QScopeGuard>
#include <QtCore/QTemporaryDir>
#include <QtGui/QGuiApplication>
#include <QtGui/QImage>
#include <QtQml/QQmlApplicationEngine>
#include <QtQml/QQmlComponent>
#include <QtQuick/QQuickItem>
#include <QtQuick/QQuickWindow>
#include <QtTest/QTest>

#include "MissionController.h"
#include "PlanMasterController.h"
#include "QGCFileDialogController.h"
#include "QmlUITestBase.h"
#include "SimpleMissionItem.h"

namespace {
void reportInvalidGeometry(QQuickItem* item, int& remaining)
{
    if (!item || remaining <= 0) {
        return;
    }
    const QRectF bounds = item->mapRectToScene(item->boundingRect());
    const auto outsideIntegerRange = [](double value) {
        return !std::isfinite(value) || value <= std::numeric_limits<int>::min() ||
               value >= std::numeric_limits<int>::max();
    };
    if (outsideIntegerRange(bounds.x()) || outsideIntegerRange(bounds.y()) || outsideIntegerRange(bounds.width()) ||
        outsideIntegerRange(bounds.height())) {
        --remaining;
        std::fprintf(stderr, "CAAC invalid scene geometry: %s [%s] visible=%d rect=(%g,%g,%g,%g)\n",
                     item->metaObject()->className(), qPrintable(item->objectName()), item->isVisible(), bounds.x(),
                     bounds.y(), bounds.width(), bounds.height());
        for (auto* parent = item->parentItem(); parent; parent = parent->parentItem()) {
            std::fprintf(stderr, "  parent: %s [%s]\n", parent->metaObject()->className(),
                         qPrintable(parent->objectName()));
        }
    }
    for (auto* child : item->childItems()) {
        reportInvalidGeometry(child, remaining);
    }
}
}  // namespace

#ifdef Q_OS_WIN
#include <QtCore/qt_windows.h>

// DbgHelp requires the Win32 declarations above; keep this include in a separate block.
#include <dbghelp.h>

namespace {
QtMessageHandler previousMessageHandler = nullptr;

void reportFatalStack(QtMsgType type, const QMessageLogContext& context, const QString& message)
{
    if (type == QtFatalMsg) {
        int remaining = 20;
        for (auto* window : QGuiApplication::allWindows()) {
            if (auto* quickWindow = qobject_cast<QQuickWindow*>(window)) {
                reportInvalidGeometry(quickWindow->contentItem(), remaining);
            }
        }
        const HANDLE process = GetCurrentProcess();
        SymSetOptions(SYMOPT_UNDNAME | SYMOPT_DEFERRED_LOADS | SYMOPT_NO_PROMPTS | SYMOPT_FAIL_CRITICAL_ERRORS);
        const QByteArray symbolPath = qgetenv("CAAC_QT_SYMBOL_PATH");
        if (SymInitialize(process, symbolPath.isEmpty() ? nullptr : symbolPath.constData(), TRUE)) {
            void* frames[64] = {};
            const USHORT count = CaptureStackBackTrace(0, 64, frames, nullptr);
            alignas(SYMBOL_INFO) unsigned char symbolBuffer[sizeof(SYMBOL_INFO) + MAX_SYM_NAME] = {};
            auto* symbol = reinterpret_cast<SYMBOL_INFO*>(symbolBuffer);
            symbol->SizeOfStruct = sizeof(SYMBOL_INFO);
            symbol->MaxNameLen = MAX_SYM_NAME;
            std::fputs("CAAC fatal stack:\n", stderr);
            for (USHORT i = 0; i < count; ++i) {
                const auto address = reinterpret_cast<DWORD64>(frames[i]);
                DWORD64 displacement = 0;
                const bool resolved = SymFromAddr(process, address, &displacement, symbol);
                std::fprintf(stderr, "  %u %p %s + %llu", i, frames[i], resolved ? symbol->Name : "?",
                             static_cast<unsigned long long>(displacement));
                IMAGEHLP_LINE64 line = {};
                line.SizeOfStruct = sizeof(line);
                DWORD lineDisplacement = 0;
                if (SymGetLineFromAddr64(process, address, &lineDisplacement, &line)) {
                    std::fprintf(stderr, " (%s:%lu)", line.FileName, line.LineNumber);
                }
                std::fputc('\n', stderr);
            }
            SymCleanup(process);
            std::fflush(stderr);
        }
    }
    if (previousMessageHandler) {
        previousMessageHandler(type, context, message);
    }
}
}  // namespace
#endif

class CaacRouteEditorUITest : public QmlUITestBase
{
    Q_OBJECT

private slots:
    void _testMouseEditing();
};

void CaacRouteEditorUITest::_testMouseEditing()
{
#ifdef Q_OS_WIN
    previousMessageHandler = qInstallMessageHandler(reportFatalStack);
    const auto restoreHandler = qScopeGuard([] { qInstallMessageHandler(previousMessageHandler); });
#endif
    const auto step = [](const char* label) {
        if (UnitTest::isVerbose()) {
            std::fprintf(stderr, "CAAC UI step: %s\n", label);
            std::fflush(stderr);
        }
    };
    step("start UI");
    startUI();
    if (QTest::currentTestFailed()) {
        return;
    }
    step("check empty and scrolling indicators");
    {
        QQmlComponent component(_engine);
        component.setData(R"(
            import QtQuick
            import QGroundControl
            import QGroundControl.Controls
            Item {
                QGCPalette { id: qgcPal }
                QGCFlickable {
                    objectName: "scrollFixture"
                    width: 200; height: 100
                    contentWidth: 0; contentHeight: 0
                    flickableDirection: Flickable.HorizontalAndVerticalFlick
                }
            }
        )",
                          QUrl(QStringLiteral("qrc:/qml/CAAC/Plan/ScrollIndicatorTest.qml")));
        const std::unique_ptr<QObject> fixture(component.create());
        QVERIFY2(fixture, qPrintable(component.errorString()));
        auto* flickable = fixture->findChild<QQuickItem*>(QStringLiteral("scrollFixture"));
        QVERIFY(flickable);
        QList<QQuickItem*> indicators;
        for (auto* child : flickable->childItems()) {
            if (child->property("orientation").isValid() && child->property("showIndicator").isValid()) {
                indicators.append(child);
            }
        }
        QCOMPARE(indicators.size(), 2);
        const auto finiteIndicators = [&] {
            for (auto* indicator : indicators) {
                if (!std::isfinite(indicator->x()) || !std::isfinite(indicator->y()) ||
                    !std::isfinite(indicator->width()) || !std::isfinite(indicator->height())) {
                    return false;
                }
            }
            return true;
        };
        QVERIFY(finiteIndicators());
        for (auto* indicator : indicators) {
            QVERIFY(!indicator->property("showIndicator").toBool());
        }
        flickable->setProperty("contentWidth", 400);
        flickable->setProperty("contentHeight", 200);
        flickable->setProperty("contentX", 20);
        flickable->setProperty("contentY", 30);
        for (auto* indicator : indicators) {
            const bool vertical = indicator->property("orientation").toInt() == 0;
            QTRY_VERIFY_WITH_TIMEOUT(indicator->property("showIndicator").toBool(), TestTimeout::mediumMs());
            QTRY_VERIFY_WITH_TIMEOUT(
                std::abs((vertical ? indicator->height() : indicator->width()) - (vertical ? 50.0 : 100.0)) < 0.01,
                TestTimeout::mediumMs());
            QVERIFY(std::abs((vertical ? indicator->y() : indicator->x()) - (vertical ? 15.0 : 10.0)) < 0.01);
        }
        flickable->setProperty("contentWidth", 0);
        flickable->setProperty("contentHeight", 0);
        QVERIFY(finiteIndicators());
        for (auto* indicator : indicators) {
            QVERIFY(!indicator->property("showIndicator").toBool());
        }
    }
    if (UnitTest::isVerbose()) {
        int remaining = 20;
        reportInvalidGeometry(_rootItem, remaining);
    }
    step("navigate to plan");
    QVERIFY(clickToolSelectDropdownButton(QStringLiteral("toolbar_viewPlan")));
    auto* plan = findVisibleItem(_rootItem, QStringLiteral("mainView_plan"), TestTimeout::mediumMs());
    QVERIFY(plan);
    auto* master = plan->property("_planMasterController").value<PlanMasterController*>();
    QVERIFY(master);
    auto* mission = master->missionController();
    auto* map = findVisibleItem(_rootItem, QStringLiteral("planView_map"));
    QVERIFY(map);
    QVERIFY(findVisibleItem(_rootItem, QStringLiteral("caacRouteEditor")));
    QVERIFY(map->property("directMissionItemEditing").toBool());
    step("center and zoom map");
    const QGeoCoordinate center(30, 120);
    map->setProperty("center", QVariant::fromValue(center));
    map->setProperty("zoomLevel", 16.0);
    QTRY_VERIFY_WITH_TIMEOUT(map->property("mapReady").toBool(), TestTimeout::mediumMs());
    const QPointF aScene = map->mapToScene(QPointF(map->width() * 0.4, map->height() * 0.5));
    QVERIFY(std::isfinite(aScene.x()) && std::isfinite(aScene.y()));
    QVERIFY(_rootItem->boundingRect().contains(aScene));
    const QPoint a = aScene.toPoint();
    const QPoint b = a + QPoint(100, 50);
    step("single click and double-click first waypoint");
    QTest::mouseClick(_window, Qt::LeftButton, Qt::NoModifier, a);
    QCOMPARE(mission->visualItems()->count(), 1);
    QTest::mouseDClick(_window, Qt::LeftButton, Qt::NoModifier, a);
    QTRY_COMPARE_WITH_TIMEOUT(mission->visualItems()->count(), 2, TestTimeout::mediumMs());
    step("double-click second waypoint");
    QTest::mouseDClick(_window, Qt::LeftButton, Qt::NoModifier, b);
    QTRY_COMPARE_WITH_TIMEOUT(mission->visualItems()->count(), 3, TestTimeout::mediumMs());
    QPointer<SimpleMissionItem> first = mission->visualItems()->value<SimpleMissionItem*>(1);
    QVERIFY(first);
    auto* dragger = findVisibleItem(_rootItem, QStringLiteral("missionItemDrag_1"), TestTimeout::mediumMs());
    QVERIFY(dragger);
    QTRY_VERIFY_WITH_TIMEOUT(
        _rootItem->boundingRect().contains(dragger->mapToScene(QPointF(dragger->width() / 2, dragger->height() / 2))),
        TestTimeout::mediumMs());
    const QPoint marker = dragger->mapToScene(QPointF(dragger->width() / 2, dragger->height() / 2)).toPoint();
    step("right-click first waypoint");
    QTest::mouseClick(_window, Qt::RightButton, Qt::NoModifier, marker);
    QVERIFY(waitForDialog(QStringLiteral("Waypoint properties"), TestTimeout::mediumMs()));
    auto* latitude = findVisibleItem(_rootItem, QStringLiteral("caacWaypointLatitude"));
    auto* altitude = findVisibleItem(_rootItem, QStringLiteral("caacWaypointAltitude"));
    QVERIFY(latitude);
    QVERIFY(altitude);
    const auto oldCoordinate = first->coordinate();
    auto* other = mission->visualItems()->value<SimpleMissionItem*>(2);
    QVERIFY(other);
    const auto otherCoordinate = other->coordinate();
    mission->setCurrentPlanViewSeqNum(other->sequenceNumber(), true);
    step("reject invalid properties");
    latitude->setProperty("text", QStringLiteral("30.12345678"));
    altitude->setProperty("text", QStringLiteral("NaN"));
    QVERIFY(acceptDialog());
    QVERIFY(dialogVisible(QStringLiteral("Waypoint properties")));
    QCOMPARE(first->coordinate(), oldCoordinate);
    step("accept valid properties");
    altitude->setProperty("text", QStringLiteral("123.45"));
    QVERIFY(acceptDialog());
    QTRY_VERIFY_WITH_TIMEOUT(!dialogVisible(QStringLiteral("Waypoint properties")), TestTimeout::mediumMs());
    QCOMPARE(first->coordinate().latitude(), 30.12345678);
    QCOMPARE(first->altitude()->rawValue().toDouble(), 123.45);
    QCOMPARE(other->coordinate(), otherCoordinate);

    // Bring the edited waypoint back into view and select the other waypoint before dragging it.
    step("drag waypoint");
    first->setCoordinate(oldCoordinate);
    mission->setCurrentPlanViewSeqNum(mission->visualItems()->value<VisualMissionItem*>(2)->sequenceNumber(), true);
    dragger = findVisibleItem(_rootItem, QStringLiteral("missionItemDrag_1"), TestTimeout::mediumMs());
    QVERIFY(dragger);
    QTRY_VERIFY_WITH_TIMEOUT(
        _rootItem->boundingRect().contains(dragger->mapToScene(QPointF(dragger->width() / 2, dragger->height() / 2))),
        TestTimeout::mediumMs());
    const QPoint from = dragger->mapToScene(QPointF(dragger->width() / 2, dragger->height() / 2)).toPoint();
    QTest::mousePress(_window, Qt::LeftButton, Qt::NoModifier, from);
    for (int i = 1; i <= 5; ++i) {
        QTest::mouseMove(_window, from + QPoint(i * 8, -i * 5));
    }
    const bool waypointMoved = waitForCondition([&] { return first->coordinate().distanceTo(oldCoordinate) > 1; },
                                                TestTimeout::mediumMs(), QStringLiteral("waypoint drag"));
    QTest::mouseRelease(_window, Qt::LeftButton, Qt::NoModifier, from + QPoint(40, -25));
    QVERIFY(waypointMoved);
    QCOMPARE(first->altitude()->rawValue().toDouble(), 123.45);
    QCOMPARE(mission->visualItems()->count(), 3);

    step("pan map");
    const auto beforePan = map->property("center").value<QGeoCoordinate>();
    const QPoint blank = map->mapToScene(QPointF(map->width() * 0.35, map->height() * 0.75)).toPoint();
    QTest::mousePress(_window, Qt::LeftButton, Qt::NoModifier, blank);
    for (int i = 1; i <= 5; ++i) {
        QTest::mouseMove(_window, blank + QPoint(i * 12, 0));
    }
    const bool mapMoved =
        waitForCondition([&] { return map->property("center").value<QGeoCoordinate>().distanceTo(beforePan) > 1; },
                         TestTimeout::mediumMs(), QStringLiteral("map pan"));
    QTest::mouseRelease(_window, Qt::LeftButton, Qt::NoModifier, blank + QPoint(60, 0));
    QVERIFY(mapMoved);
    QCOMPARE(mission->visualItems()->count(), 3);

    step("reorder waypoints");
    QVERIFY(clickButton(QStringLiteral("caacMoveWaypointDown")));
    QCOMPARE(mission->visualItems()->get(2), first.data());
    QVERIFY(clickButton(QStringLiteral("caacMoveWaypointUp")));
    QCOMPARE(mission->visualItems()->get(1), first.data());

    QTemporaryDir temp;
    QVERIFY(temp.isValid());
    const QString file = temp.filePath(QStringLiteral("CAAC航点.plan"));
    step("cancel save");
    QGCFileDialogController::setTestRejectNext();
    QVERIFY(clickButton(QStringLiteral("planToolbar_saveButton")));
    QTRY_VERIFY_WITH_TIMEOUT(!QGCFileDialogController::testHookArmed(), TestTimeout::mediumMs());
    QVERIFY(!QFile::exists(file));
    QVERIFY(master->dirtyForSave());
    step("save plan");
    QGCFileDialogController::setTestNextFileForAccept(file);
    QVERIFY(clickButton(QStringLiteral("planToolbar_saveButton")));
    QTRY_VERIFY_WITH_TIMEOUT(QFile::exists(file), TestTimeout::mediumMs());
    QVERIFY(!QGCFileDialogController::testHookArmed());
    QVERIFY(!master->dirtyForSave());
    const auto saved = master->saveToJson();
    step("delete waypoint and load plan");
    QVERIFY(clickButton(QStringLiteral("caacRemoveWaypoint")));
    QCOMPARE(mission->visualItems()->count(), 2);
    master->removeAll();
    QGCFileDialogController::setTestNextFileForAccept(file);
    QVERIFY(clickButton(QStringLiteral("planToolbar_openButton")));
    QTRY_VERIFY_WITH_TIMEOUT(!QGCFileDialogController::testHookArmed(), TestTimeout::mediumMs());
    QCOMPARE(master->saveToJson(), saved);
    QCOMPARE(mission->visualItems()->count(), 3);

    step("complete");
    const QString screenshot = qEnvironmentVariable("CAAC_UI_SCREENSHOT");
    if (!screenshot.isEmpty()) {
        QVERIFY(_window->grabWindow().save(screenshot));
    }
    master->removeAll();
}

UT_REGISTER_TEST(CaacRouteEditorUITest, TestLabel::Integration, TestLabel::MissionManager)

#include "CaacRouteEditorUITest.moc"
