#include <atomic>
#include <cmath>
#include <cstdio>
#include <limits>
#include <memory>

#include <QtCore/QByteArray>
#include <QtCore/QElapsedTimer>
#include <QtCore/QFile>
#include <QtCore/QFileInfo>
#include <QtCore/QLineF>
#include <QtCore/QPointer>
#include <QtCore/QProcess>
#include <QtCore/QScopeGuard>
#include <QtCore/QTemporaryDir>
#include <QtCore/QUuid>
#include <QtGui/QFontMetricsF>
#include <QtGui/QGuiApplication>
#include <QtGui/QImage>
#include <QtNetwork/QLocalServer>
#include <QtNetwork/QLocalSocket>
#include <QtQml/QQmlApplicationEngine>
#include <QtQml/QQmlComponent>
#include <QtQml/QQmlExpression>
#include <QtQuick/QQuickItem>
#include <QtQuick/QQuickWindow>
#include <QtTest/QTest>

#include "AppSettings.h"
#include "CaacCorePlugin.h"
#include "CaacRouteEditorController.h"
#include "Fact.h"
#include "MissionController.h"
#include "PlanMasterController.h"
#include "QGCApplication.h"
#include "QGCFileDialogController.h"
#include "QGCGeo.h"
#include "QGCMapEngineManager.h"
#include "QmlUITestBase.h"
#include "SettingsManager.h"
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
    static std::atomic_bool reportedInvalidPath = false;
    const bool invalidPath =
        type == QtWarningMsg && message.startsWith(QStringLiteral("QPainterPath::moveTo: Adding point with invalid"));
    if (type == QtFatalMsg) {
        int remaining = 20;
        for (auto* window : QGuiApplication::allWindows()) {
            if (auto* quickWindow = qobject_cast<QQuickWindow*>(window)) {
                reportInvalidGeometry(quickWindow->contentItem(), remaining);
            }
        }
    }
    if (type == QtFatalMsg || (invalidPath && !reportedInvalidPath.exchange(true))) {
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
            std::fputs(type == QtFatalMsg ? "CAAC fatal stack:\n" : "CAAC invalid path stack:\n", stderr);
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
    void _testMouseEditing_data();
    void _testMouseEditing();
    void _homeCenterWindowFocus();
    void _planningUtilities();
};

void CaacRouteEditorUITest::_testMouseEditing_data()
{
    QTest::addColumn<bool>("chinese");
    if (inherits("CaacChineseUITest")) {
        QTest::newRow("Simplified-Chinese") << true;
    } else {
        QTest::newRow("English-regression") << false;
    }
}

void CaacRouteEditorUITest::_testMouseEditing()
{
    QFETCH(bool, chinese);
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
    step("map display names preserve provider identifiers");
    {
        QQmlComponent component(_engine);
        component.setData(R"(
            import QtQuick
            import QGroundControl.Controls
            QGCComboBox {
                textTransform: (value) => MapNames.displayName(value)
            }
        )",
                          QUrl(QStringLiteral("qrc:/qml/CAAC/Plan/MapTranslationTest.qml")));
        const std::unique_ptr<QObject> combo(component.create());
        QVERIFY2(combo, qPrintable(component.errorString()));
        for (const auto& key : QGCMapEngineManager::mapList()) {
            combo->setProperty("model", QStringList{key});
            combo->setProperty("currentIndex", 0);
            QCOMPARE(combo->property("currentText").toString(), key);
            auto* content = combo->property("contentItem").value<QObject*>();
            QVERIFY(content);
            const QString label = content->property("text").toString();
            if (chinese) {
                QVERIFY2(label.contains(QRegularExpression(QStringLiteral("[\\x{4e00}-\\x{9fff}]"))),
                         qPrintable(QStringLiteral("Untranslated map type: %1 → %2").arg(key, label)));
            }
        }
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
    QVERIFY(clickToolSelectDropdownButton(QStringLiteral("toolbar_viewFlightPlan")));
    auto* plan = findVisibleItem(_rootItem, QStringLiteral("mainView_plan"), TestTimeout::mediumMs());
    QVERIFY(plan);
    auto* master = plan->property("_planMasterController").value<PlanMasterController*>();
    QVERIFY(master);
    auto* mission = master->missionController();
    QVERIFY(!mission->planFileGroupIndex().isValid());
    QVERIFY(mission->defaultsGroupIndex().isValid());
    QVERIFY(mission->missionGroupIndex().isValid());
    auto* map = findVisibleItem(_rootItem, QStringLiteral("planView_map"));
    QVERIFY(map);
    QVERIFY(!findVisibleItem(_rootItem, QStringLiteral("planView_statistics"), 0));
    QVERIFY(clickToolSelectDropdownButton(QStringLiteral("toolbar_planningStats")));
    auto* terrainGraph = findVisibleItem(_rootItem, QStringLiteral("planTerrainGraph"));
    QVERIFY(terrainGraph);
    const auto graphDomainsValid = [&] {
        for (const auto* name : {"axisX", "axisY"}) {
            auto* axis = terrainGraph->property(name).value<QObject*>();
            if (!axis) {
                return false;
            }
            const double minimum = axis->property("min").toDouble();
            const double maximum = axis->property("max").toDouble();
            if (!std::isfinite(minimum) || !std::isfinite(maximum) || maximum <= minimum) {
                return false;
            }
        }
        return true;
    };
    QTRY_VERIFY_WITH_TIMEOUT(graphDomainsValid(), TestTimeout::mediumMs());
    QVERIFY(clickToolSelectDropdownButton(QStringLiteral("toolbar_planningStats")));
    QVERIFY(findVisibleItem(_rootItem, QStringLiteral("caacRouteEditor")));
    QVERIFY(clickButton(QStringLiteral("caacWaypointListButton")));
    QVERIFY(map->property("directMissionItemEditing").toBool());
    step("center and zoom map");
    const QGeoCoordinate center(30, 120);
    map->setProperty("center", QVariant::fromValue(center));
    map->setProperty("zoomLevel", 16.0);
    QTRY_VERIFY_WITH_TIMEOUT(map->property("mapReady").toBool(), TestTimeout::mediumMs());
    const auto mapViewport = map->property("centerViewport").toRectF();
    QVERIFY(mapViewport.isValid());
    const QPointF aScene = map->mapToScene(
        QPointF(mapViewport.x() + mapViewport.width() * 0.4, mapViewport.y() + mapViewport.height() * 0.6));
    QVERIFY(std::isfinite(aScene.x()) && std::isfinite(aScene.y()));
    QVERIFY(_rootItem->boundingRect().contains(aScene));
    const QPoint a = aScene.toPoint();
    const QPoint b = a + QPoint(100, 50);
    step("single click and double-click first waypoint");
    QTest::mouseClick(_window, Qt::LeftButton, Qt::NoModifier, a);
    QCOMPARE(mission->visualItems()->count(), 1);
    QTest::mouseDClick(_window, Qt::LeftButton, Qt::NoModifier, a);
    QTRY_COMPARE_WITH_TIMEOUT(mission->visualItems()->count(), 2, TestTimeout::mediumMs());
    QTRY_VERIFY_WITH_TIMEOUT(graphDomainsValid(), TestTimeout::mediumMs());
    step("double-click second waypoint");
    QTest::mouseDClick(_window, Qt::LeftButton, Qt::NoModifier, b);
    QTRY_COMPARE_WITH_TIMEOUT(mission->visualItems()->count(), 3, TestTimeout::mediumMs());
    QTRY_VERIFY_WITH_TIMEOUT(graphDomainsValid(), TestTimeout::mediumMs());
    QPointer<SimpleMissionItem> first = mission->visualItems()->value<SimpleMissionItem*>(1);
    QVERIFY(first);
    QCOMPARE(mission->simpleFlightPathSegments()->count(), 2);
    auto* legLabel = findVisibleItem(_rootItem, QStringLiteral("caacRouteLegLabel_1"));
    QVERIFY(legLabel);
    const auto initialLegText = legLabel->property("legText").toString();
    QVERIFY(initialLegText.contains(QStringLiteral(" m")) && initialLegText.contains(QChar(0x00b0)));
    auto* plugin = qobject_cast<CaacCorePlugin*>(QGCCorePlugin::instance());
    QVERIFY(plugin);
    const auto restoreMapPreferences = qScopeGuard([plugin]() {
        plugin->setDoubleClickAddWaypoint(true);
        plugin->setDragWaypoints(true);
    });
    plugin->setDoubleClickAddWaypoint(false);
    QTest::mouseDClick(_window, Qt::LeftButton, Qt::NoModifier, b + QPoint(50, 0));
    QCOMPARE(mission->visualItems()->count(), 3);
    plugin->setDoubleClickAddWaypoint(true);
    auto* firstNumber = findVisibleItem(_rootItem, QStringLiteral("caacWaypointNumber_1"));
    QVERIFY(firstNumber);
    QCOMPARE(firstNumber->property("text").toString(), QStringLiteral("1"));
    auto* planTree = findItem(_rootItem, QStringLiteral("planView_planTree"));
    QVERIFY(planTree && !planTree->isVisible());
    QVERIFY(!findVisibleItem(_rootItem, QStringLiteral("caacMissionSettings")));
    QVERIFY(clickButton(QStringLiteral("caacGlobalSettingsButton")));
    QVERIFY(clickButtonScrolled(QStringLiteral("caacMissionSettings"), QStringLiteral("caacGlobalSettingsScroller")));
    QTRY_VERIFY_WITH_TIMEOUT(planTree->isVisible(), TestTimeout::mediumMs());
    QVERIFY(!findVisibleItem(_rootItem, QStringLiteral("caacEditWaypoint")));
    QVERIFY(clickButton(QStringLiteral("caacWaypointListButton")));
    QTRY_VERIFY_WITH_TIMEOUT(!planTree->isVisible(), TestTimeout::mediumMs());
    auto* dragger = findVisibleItem(_rootItem, QStringLiteral("missionItemDrag_1"), TestTimeout::mediumMs());
    QVERIFY(dragger);
    QTRY_VERIFY_WITH_TIMEOUT(
        _rootItem->boundingRect().contains(dragger->mapToScene(QPointF(dragger->width() / 2, dragger->height() / 2))),
        TestTimeout::mediumMs());
    // The native settings panel can move the camera; settle the map before sending a screen-space click.
    map->setProperty("center", QVariant::fromValue(first->coordinate()));
    auto* indicator = dragger->property("itemIndicator").value<QQuickItem*>();
    QVERIFY(indicator);
    const auto markerAnchor = [&]() {
        return dragger->mapToScene(indicator->property("anchorPoint").toPointF() +
                                   QPointF(dragger->property("_touchMarginHorizontal").toDouble(),
                                           dragger->property("_touchMarginVertical").toDouble()));
    };
    const QPointF expectedMarker = map->mapToScene(QPointF(map->width() / 2, map->height() / 2));
    QTRY_VERIFY_WITH_TIMEOUT(QLineF(markerAnchor(), expectedMarker).length() < 1.0, TestTimeout::mediumMs());
    const QPoint marker = dragger->mapToScene(QPointF(dragger->width() / 2, dragger->height() / 2)).toPoint();
    step("right-click first waypoint");
    plugin->setDragWaypoints(false);
    QTest::mousePress(_window, Qt::RightButton, Qt::NoModifier, marker);
    auto* rightClickGrabber = _window->mouseGrabberItem();
    const bool waypointGrabbed = rightClickGrabber && rightClickGrabber->parentItem() == dragger;
    const QString grabbedItem = rightClickGrabber ? QString::fromLatin1(rightClickGrabber->metaObject()->className()) +
                                                        QLatin1Char(':') + rightClickGrabber->objectName()
                                                  : QStringLiteral("none");
    QTest::mouseRelease(_window, Qt::RightButton, Qt::NoModifier, marker);
    QVERIFY2(waypointGrabbed, qPrintable(QStringLiteral("Waypoint right-click intercepted by ") + grabbedItem));
    QVERIFY(findVisibleItem(_rootItem, QStringLiteral("caacContextDeleteWaypoint")));
    QTest::keyClick(_window, Qt::Key_Escape);
    QTest::mouseClick(_window, Qt::LeftButton, Qt::NoModifier, marker);
    QVERIFY(findVisibleItem(_rootItem, QStringLiteral("caacWaypointQuickBearing")));
    QTest::keyClick(_window, Qt::Key_Escape);
    QVERIFY(clickButton(QStringLiteral("caacEditWaypoint_1")));
    QVERIFY(waitForDialog(QCoreApplication::translate("CaacWaypointDialog", "Waypoint properties"),
                          TestTimeout::mediumMs()));
    auto* latitude = findVisibleItem(_rootItem, QStringLiteral("caacWaypointLatitude"));
    auto* altitude = findVisibleItem(_rootItem, QStringLiteral("caacWaypointAltitude"));
    QVERIFY(latitude);
    QVERIFY(altitude);
    plugin->setDragWaypoints(true);
    const auto oldCoordinate = first->coordinate();
    auto* other = mission->visualItems()->value<SimpleMissionItem*>(2);
    QVERIFY(other);
    const auto otherCoordinate = other->coordinate();
    mission->setCurrentPlanViewSeqNum(other->sequenceNumber(), true);
    step("reject invalid properties");
    latitude->setProperty("text", QStringLiteral("30.12345678"));
    altitude->setProperty("text", QStringLiteral("NaN"));
    QVERIFY(acceptDialog());
    QVERIFY(dialogVisible(QCoreApplication::translate("CaacWaypointDialog", "Waypoint properties")));
    QCOMPARE(first->coordinate(), oldCoordinate);
    step("accept valid properties");
    altitude->setProperty("text", QStringLiteral("123.45"));
    QVERIFY(acceptDialog());
    QTRY_VERIFY_WITH_TIMEOUT(!dialogVisible(QCoreApplication::translate("CaacWaypointDialog", "Waypoint properties")),
                             TestTimeout::mediumMs());
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
    auto* dragValues = findVisibleItem(_rootItem, QStringLiteral("caacWaypointDragValues"));
    const QString initialFeedback = dragValues ? dragValues->property("text").toString() : QString();
    QTest::mouseMove(_window, from + QPoint(55, -35));
    const bool feedbackChanged =
        dragValues && waitForCondition([&] { return dragValues->property("text").toString() != initialFeedback; },
                                       TestTimeout::mediumMs(), QStringLiteral("live drag distance and angle"));
    QTest::mouseRelease(_window, Qt::LeftButton, Qt::NoModifier, from + QPoint(55, -35));
    QVERIFY(waypointMoved);
    QVERIFY(dragValues);
    QVERIFY(feedbackChanged);
    QTRY_VERIFY_WITH_TIMEOUT(
        findVisibleItem(_rootItem, QStringLiteral("caacRouteLegLabel_1"), 0) &&
            findVisibleItem(_rootItem, QStringLiteral("caacRouteLegLabel_1"), 0)->property("legText").toString() !=
                initialLegText,
        TestTimeout::mediumMs());
    QTRY_VERIFY_WITH_TIMEOUT(!dragValues->isVisible(), TestTimeout::mediumMs());
    QCOMPARE(first->altitude()->rawValue().toDouble(), 123.45);
    QCOMPARE(mission->visualItems()->count(), 3);

    step("pan map");
    auto* background = findVisibleItem(_rootItem, QStringLiteral("caacMapMouseArea"));
    QVERIFY(background);
    const auto beforePan = map->property("center").value<QGeoCoordinate>();
    // Keep the drag above the terrain graph and away from the waypoint markers and tool strips.
    const QPoint blank = map->mapToScene(QPointF(map->width() * 0.2, map->height() * 0.4)).toPoint();
    QTest::mousePress(_window, Qt::LeftButton, Qt::NoModifier, blank);
    const bool backgroundGrabbed = _window->mouseGrabberItem() == background;
    const QString grabber = _window->mouseGrabberItem()
                                ? QString::fromLatin1(_window->mouseGrabberItem()->metaObject()->className()) +
                                      QLatin1Char(':') + _window->mouseGrabberItem()->objectName()
                                : QStringLiteral("none");
    for (int i = 1; i <= 5; ++i) {
        QTest::mouseMove(_window, blank + QPoint(i * 12, 0));
    }
    const bool mapMoved =
        waitForCondition([&] { return map->property("center").value<QGeoCoordinate>().distanceTo(beforePan) > 1; },
                         TestTimeout::mediumMs(), QStringLiteral("map pan"));
    QTest::mouseRelease(_window, Qt::LeftButton, Qt::NoModifier, blank + QPoint(60, 0));
    QVERIFY2(backgroundGrabbed, qPrintable(QStringLiteral("Map press intercepted by ") + grabber));
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
    QVERIFY(clickButtonScrolled(
        QStringLiteral("caacRemoveWaypoint_%1").arg(mission->currentPlanViewItem()->sequenceNumber()),
        QStringLiteral("caacWaypointList")));
    QCOMPARE(mission->visualItems()->count(), 2);
    master->removeAll();
    QTRY_VERIFY_WITH_TIMEOUT(graphDomainsValid(), TestTimeout::mediumMs());
    QGCFileDialogController::setTestNextFileForAccept(file);
    QVERIFY(clickButton(QStringLiteral("planToolbar_openButton")));
    QTRY_VERIFY_WITH_TIMEOUT(!QGCFileDialogController::testHookArmed(), TestTimeout::mediumMs());
    QCOMPARE(master->saveToJson(), saved);
    QCOMPARE(mission->visualItems()->count(), 3);

    step("rotate and undo through native controls");
    auto* editorPanel = findVisibleItem(_rootItem, QStringLiteral("caacRouteEditor"));
    QVERIFY(editorPanel);
    auto* editor = editorPanel->property("routeController").value<CaacRouteEditorController*>();
    QVERIFY(editor);
    const auto beforeRotation = master->saveToJson();
    QVERIFY(clickButton(QStringLiteral("caacGlobalSettingsButton")));
    auto* rotationCenter =
        findVisibleItemScrolled(QStringLiteral("caacRotateCenter"), QStringLiteral("caacGlobalSettingsScroller"));
    auto* rotationAngle =
        findVisibleItemScrolled(QStringLiteral("caacRotateAngle"), QStringLiteral("caacGlobalSettingsScroller"));
    auto* includeHome =
        findVisibleItemScrolled(QStringLiteral("caacRotateIncludeHome"), QStringLiteral("caacGlobalSettingsScroller"));
    QVERIFY(rotationCenter && rotationAngle && includeHome);
    QVERIFY(rotationCenter->setProperty("currentIndex", 0));
    QVERIFY(rotationAngle->setProperty("text", QStringLiteral("90")));
    QVERIFY(includeHome->setProperty("checked", true));
    const auto homeBeforeRotation = mission->plannedHomePosition();
    const auto firstBeforeRotation = mission->visualItems()->value<SimpleMissionItem*>(1)->coordinate();
    QVERIFY(clickButtonScrolled(QStringLiteral("caacRotateApply"), QStringLiteral("caacGlobalSettingsScroller")));
    QVERIFY(editor->canUndo());
    QVERIFY(master->saveToJson() != beforeRotation);
    QCOMPARE(mission->plannedHomePosition(), homeBeforeRotation);
    const auto firstAfterRotation = mission->visualItems()->value<SimpleMissionItem*>(1)->coordinate();
    QVERIFY(std::abs(QGCGeo::geodesicDistance(homeBeforeRotation, firstBeforeRotation) -
                     QGCGeo::geodesicDistance(homeBeforeRotation, firstAfterRotation)) < 1e-5);
    QVERIFY(std::abs(std::remainder(QGCGeo::geodesicAzimuth(homeBeforeRotation, firstAfterRotation) -
                                        QGCGeo::geodesicAzimuth(homeBeforeRotation, firstBeforeRotation) - 90,
                                    360)) < 1e-5);
    QVERIFY(clickButton(QStringLiteral("caacUndo")));
    QCOMPARE(master->saveToJson(), beforeRotation);
    QVERIFY(clickButton(QStringLiteral("caacRedo")));
    QVERIFY(master->saveToJson() != beforeRotation);

    step("relative point through native controls");
    QVERIFY(clickButton(QStringLiteral("caacRelativeEditorButton")));
    QVERIFY(editorPanel->property("relativeEditorVisible").toBool());
    QVERIFY(!findVisibleItem(_rootItem, QStringLiteral("caacWaypointList")));
    auto* relativeDistance = findVisibleItem(_rootItem, QStringLiteral("caacRelativeDistance"));
    QVERIFY(relativeDistance);
    auto* panelTitle = findVisibleItem(_rootItem, QStringLiteral("caacPanelTitle"));
    QVERIFY(panelTitle);
    QCOMPARE(panelTitle->property("text").toString(),
             chinese ? QStringLiteral("相对坐标编辑") : QStringLiteral("Relative coordinate editor"));
    const auto beforeInvalidInsert = master->saveToJson();
    QVERIFY(clickButton(QStringLiteral("caacRelativeAdd")));
    QCOMPARE(master->saveToJson(), beforeInvalidInsert);
    QVERIFY(!editor->lastError().isEmpty());
    relativeDistance->setProperty("text", QStringLiteral("100"));
    const int beforeInsert = mission->visualItems()->count();
    QVERIFY(clickButton(QStringLiteral("caacRelativeAdd")));
    QCOMPARE(mission->visualItems()->count(), beforeInsert + 1);
    QVERIFY(clickButton(QStringLiteral("caacUndo")));
    QCOMPARE(mission->visualItems()->count(), beforeInsert);
    relativeDistance->setProperty("focus", false);
    _window->requestActivate();
    QVERIFY(QTest::qWaitForWindowActive(_window));
    _rootItem->forceActiveFocus();
    QTest::keyClick(_window, Qt::Key_Space);
    QTRY_COMPARE_WITH_TIMEOUT(mission->visualItems()->count(), beforeInsert + 1, TestTimeout::mediumMs());
    QVERIFY(clickButton(QStringLiteral("caacUndo")));
    QCOMPARE(mission->visualItems()->count(), beforeInsert);
    const QString relativeScreenshot = qEnvironmentVariable("CAAC_RELATIVE_UI_SCREENSHOT");
    if (!relativeScreenshot.isEmpty() && chinese) {
        QVERIFY(_window->grabWindow().save(relativeScreenshot));
    }
    const auto beforeAdvancedNavigation = master->saveToJson();
    QVERIFY(!findVisibleItem(_rootItem, QStringLiteral("caacMissionSettings")));
    QVERIFY(clickButton(QStringLiteral("caacGlobalSettingsButton")));
    QVERIFY(clickButtonScrolled(QStringLiteral("caacMissionSettings"), QStringLiteral("caacGlobalSettingsScroller")));
    QTRY_VERIFY_WITH_TIMEOUT(planTree->isVisible(), TestTimeout::mediumMs());
    QVERIFY(!editorPanel->property("relativeEditorVisible").toBool());
    QCOMPARE(panelTitle->property("text").toString(),
             chinese ? QStringLiteral("高级任务设置") : QStringLiteral("Advanced mission settings"));
    auto* returnToWaypoints = findVisibleItem(_rootItem, QStringLiteral("caacReturnToWaypoints"));
    QVERIFY(returnToWaypoints);
    QCOMPARE(returnToWaypoints->property("text").toString(),
             chinese ? QStringLiteral("退出高级任务设置") : QStringLiteral("Exit advanced mission settings"));
    QVERIFY(clickButton(QStringLiteral("caacReturnToWaypoints")));
    QTRY_VERIFY_WITH_TIMEOUT(!planTree->isVisible(), TestTimeout::mediumMs());
    QCOMPARE(master->saveToJson(), beforeAdvancedNavigation);

    step("edit existing waypoint by distance and angle");
    auto* target = mission->visualItems()->value<SimpleMissionItem*>(2);
    auto* reference = mission->visualItems()->value<SimpleMissionItem*>(1);
    QVERIFY(target && reference);
    QVERIFY(editor->selectWaypoint(target));
    const auto beforeRelativeEdit = master->saveToJson();
    const auto referenceCoordinate = reference->coordinate();
    const double targetAltitude = target->altitude()->rawValue().toDouble();
    const auto targetFrame = target->altitudeFrame();
    const int targetCommand = target->command();
    QVERIFY(clickButton(QStringLiteral("caacEditWaypoint_%1").arg(editor->currentItem()->sequenceNumber())));
    QVERIFY(waitForDialog(QCoreApplication::translate("CaacWaypointDialog", "Waypoint properties")));
    auto* positionMode = findVisibleItem(_rootItem, QStringLiteral("caacWaypointPositionMode"));
    QVERIFY(positionMode && positionMode->isEnabled());
    positionMode->setProperty("currentIndex", 1);
    auto* relativeAngle = findVisibleItem(_rootItem, QStringLiteral("caacWaypointRelativeAngle"));
    auto* existingDistance = findVisibleItem(_rootItem, QStringLiteral("caacWaypointRelativeDistance"));
    auto* existingAngleType = findVisibleItem(_rootItem, QStringLiteral("caacWaypointAngleType"));
    QVERIFY(relativeAngle && existingDistance && existingAngleType);
    existingAngleType->setProperty("currentIndex", 0);
    relativeAngle->setProperty("text", QStringLiteral("90"));
    existingDistance->setProperty("text", QStringLiteral("150"));
    QVERIFY(acceptDialog());
    QTRY_VERIFY_WITH_TIMEOUT(!dialogVisible(QCoreApplication::translate("CaacWaypointDialog", "Waypoint properties")),
                             TestTimeout::mediumMs());
    QVERIFY(std::abs(QGCGeo::geodesicDistance(referenceCoordinate, target->coordinate()) - 150) < 1e-6);
    QVERIFY(std::abs(QGCGeo::geodesicAzimuth(referenceCoordinate, target->coordinate()) - 90) < 1e-7);
    QCOMPARE(target->altitude()->rawValue().toDouble(), targetAltitude);
    QCOMPARE(target->altitudeFrame(), targetFrame);
    QCOMPARE(target->command(), targetCommand);
    QCOMPARE(reference->coordinate(), referenceCoordinate);
    QVERIFY(clickButton(QStringLiteral("caacUndo")));
    QCOMPARE(master->saveToJson(), beforeRelativeEdit);
    if (chinese) {
        const auto* rotateButton = findVisibleItem(_rootItem, QStringLiteral("caacGlobalSettingsButton"));
        QVERIFY(rotateButton);
        QCOMPARE(rotateButton->property("text").toString(), QStringLiteral("全局设置"));
        QCOMPARE(QCoreApplication::translate("CaacWaypointDialog", "Waypoint properties"), QStringLiteral("航点属性"));
    }
    step("coordinate format preserves precision");
    QVERIFY(editor->selectWaypoint(mission->visualItems()->value<SimpleMissionItem*>(1)));
    const auto beforeDms = master->saveToJson();
    QVERIFY(clickButton(QStringLiteral("caacEditWaypoint_%1").arg(editor->currentItem()->sequenceNumber())));
    QVERIFY(waitForDialog(QCoreApplication::translate("CaacWaypointDialog", "Waypoint properties")));
    QVERIFY(clickButtonScrolled(QStringLiteral("caacWaypointLatitudeFormatToggle"),
                                QStringLiteral("popupDialog_contentFlickable")));
    QVERIFY(clickButtonScrolled(QStringLiteral("caacWaypointLongitudeFormatToggle"),
                                QStringLiteral("popupDialog_contentFlickable")));
    for (const auto* name :
         {"caacWaypointLatitudeDegrees", "caacWaypointLatitudeMinutes", "caacWaypointLatitudeSeconds",
          "caacWaypointLongitudeDegrees", "caacWaypointLongitudeMinutes", "caacWaypointLongitudeSeconds"}) {
        auto* field = findVisibleItemScrolled(QLatin1String(name), QStringLiteral("popupDialog_contentFlickable"));
        QVERIFY(field);
        const auto textFits = [field] {
            const QFontMetricsF metrics(field->property("font").value<QFont>());
            return field->width() >= metrics.horizontalAdvance(field->property("text").toString()) +
                                         field->property("leftPadding").toDouble() +
                                         field->property("rightPadding").toDouble();
        };
        QTRY_VERIFY_WITH_TIMEOUT(textFits(), TestTimeout::mediumMs());
    }
    QVERIFY(acceptDialog());
    QCOMPARE(master->saveToJson(), beforeDms);

    step("edit DMS and advanced waypoint parameters atomically");
    QVERIFY(clickButton(QStringLiteral("caacEditWaypoint_%1").arg(editor->currentItem()->sequenceNumber())));
    QVERIFY(waitForDialog(QCoreApplication::translate("CaacWaypointDialog", "Waypoint properties")));
    QVERIFY(clickButtonScrolled(QStringLiteral("caacWaypointLatitudeFormatToggle"),
                                QStringLiteral("popupDialog_contentFlickable")));
    auto* degrees = findVisibleItemScrolled(QStringLiteral("caacWaypointLatitudeDegrees"),
                                            QStringLiteral("popupDialog_contentFlickable"));
    auto* minutes = findVisibleItemScrolled(QStringLiteral("caacWaypointLatitudeMinutes"),
                                            QStringLiteral("popupDialog_contentFlickable"));
    auto* seconds = findVisibleItemScrolled(QStringLiteral("caacWaypointLatitudeSeconds"),
                                            QStringLiteral("popupDialog_contentFlickable"));
    auto* hemisphere = findVisibleItemScrolled(QStringLiteral("caacWaypointLatitudeHemisphere"),
                                               QStringLiteral("popupDialog_contentFlickable"));
    auto* hold = findVisibleItemScrolled(QStringLiteral("caacWaypointHoldDuration"),
                                         QStringLiteral("popupDialog_contentFlickable"));
    auto* yawMode =
        findVisibleItemScrolled(QStringLiteral("caacWaypointYawMode"), QStringLiteral("popupDialog_contentFlickable"));
    QVERIFY(degrees && minutes && seconds && hemisphere && hold && yawMode);
    degrees->setProperty("text", QStringLiteral("0"));
    minutes->setProperty("text", QStringLiteral("60"));
    seconds->setProperty("text", QStringLiteral("0"));
    hemisphere->setProperty("currentIndex", 1);
    hold->setProperty("text", QStringLiteral("2.5"));
    yawMode->setProperty("currentIndex", 1);
    auto* yaw =
        findVisibleItemScrolled(QStringLiteral("caacWaypointYaw"), QStringLiteral("popupDialog_contentFlickable"));
    QVERIFY(yaw);
    yaw->setProperty("text", QStringLiteral("270"));
    QVERIFY(acceptDialog());
    QVERIFY(dialogVisible(QCoreApplication::translate("CaacWaypointDialog", "Waypoint properties")));
    QCOMPARE(master->saveToJson(), beforeDms);
    minutes->setProperty("text", QStringLiteral("30"));
    const QString advancedScreenshot = qEnvironmentVariable("CAAC_UI_ADVANCED_SCREENSHOT");
    if (chinese && !advancedScreenshot.isEmpty()) {
        QVERIFY(_window->grabWindow().save(advancedScreenshot));
    }
    QVERIFY(acceptDialog());
    auto* dmsPoint = mission->visualItems()->value<SimpleMissionItem*>(1);
    QCOMPARE(dmsPoint->coordinate().latitude(), -0.5);
    QCOMPARE(dmsPoint->missionItem().param1(), 2.5);
    QCOMPARE(dmsPoint->missionItem().param4(), -90);
    QVERIFY(clickButton(QStringLiteral("caacUndo")));
    QCOMPARE(master->saveToJson(), beforeDms);

    step("generate template through native controls");
    const auto beforeTemplate = master->saveToJson();
    const int beforeTemplateCount = mission->visualItems()->count();
    QVERIFY(clickButton(QStringLiteral("caacTemplateEditorButton")));
    QVERIFY(findVisibleItem(_rootItem, QStringLiteral("caacTemplateSize")));
    QVERIFY(clickButton(QStringLiteral("caacTemplateAdd")));
    QCOMPARE(mission->visualItems()->count(), beforeTemplateCount + 10);
    QVERIFY(clickButton(QStringLiteral("caacUndo")));
    QCOMPARE(master->saveToJson(), beforeTemplate);

    step("precise translation through native controls");
    QVERIFY(clickButton(QStringLiteral("caacGlobalSettingsButton")));
    auto* translateDistance = findItem(_rootItem, QStringLiteral("caacTranslateDistance"));
    QVERIFY(translateDistance);
    QVERIFY(scrollIntoView(translateDistance, QStringLiteral("caacGlobalSettingsScroller")));
    translateDistance->setProperty("text", QStringLiteral("100"));
    auto* translateApply = findItem(_rootItem, QStringLiteral("caacMoveRouteDirection_45"));
    QVERIFY(translateApply);
    QVERIFY(scrollIntoView(translateApply, QStringLiteral("caacGlobalSettingsScroller")));
    const auto beforeTranslation = master->saveToJson();
    const auto beforeTranslationHome = mission->plannedHomePosition();
    QVERIFY(clickButton(QStringLiteral("caacMoveRouteDirection_45")));
    QVERIFY(std::abs(QGCGeo::geodesicDistance(beforeTranslationHome, mission->plannedHomePosition()) - 100) < 0.01);
    QVERIFY(std::abs(std::remainder(QGCGeo::geodesicAzimuth(beforeTranslationHome, mission->plannedHomePosition()) - 45,
                                    360)) < 0.001);
    QVERIFY(master->saveToJson() != beforeTranslation);
    QVERIFY(clickButton(QStringLiteral("caacUndo")));
    QCOMPARE(master->saveToJson(), beforeTranslation);

    step("drag entire route in one undo step");
    auto* dragToggle = findVisibleItem(_rootItem, QStringLiteral("caacRouteDragToggle"));
    QVERIFY(dragToggle);
    QVERIFY(scrollIntoView(dragToggle, QStringLiteral("caacGlobalSettingsScroller")));
    QVERIFY(clickButton(QStringLiteral("caacRouteDragToggle")));
    QVERIFY(editorPanel->property("routeDragMode").toBool());
    const auto beforeRouteDrag = master->saveToJson();
    const auto mapCenterBeforeDrag = map->property("center").value<QGeoCoordinate>();
    const auto routePress = map->mapToScene(QPointF(map->width() * 0.3, map->height() * 0.5)).toPoint();
    QTest::mousePress(_window, Qt::LeftButton, Qt::NoModifier, routePress);
    for (int i = 1; i <= 5; ++i) {
        QTest::mouseMove(_window, routePress + QPoint(i * 10, i * 4));
    }
    QTest::mouseRelease(_window, Qt::LeftButton, Qt::NoModifier, routePress + QPoint(50, 20));
    QTRY_VERIFY_WITH_TIMEOUT(master->saveToJson() != beforeRouteDrag, TestTimeout::mediumMs());
    QCOMPARE(map->property("center").value<QGeoCoordinate>(), mapCenterBeforeDrag);
    QVERIFY(clickButton(QStringLiteral("caacUndo")));
    QCOMPARE(master->saveToJson(), beforeRouteDrag);
    QVERIFY(scrollIntoView(dragToggle, QStringLiteral("caacGlobalSettingsScroller")));
    QVERIFY(clickButton(QStringLiteral("caacRouteDragToggle")));
    QVERIFY(!editorPanel->property("routeDragMode").toBool());

    step("drag planning panel title");
    auto* planningHandle = findVisibleItem(_rootItem, QStringLiteral("caacPlanningPanelDragHandle"));
    QVERIFY(planningHandle);
    const auto panelBeforeDrag = planningHandle->mapToScene(QPointF(0, 0));
    const auto panelPress =
        planningHandle->mapToScene(QPointF(planningHandle->width() / 2, planningHandle->height() / 2)).toPoint();
    QTest::mousePress(_window, Qt::LeftButton, Qt::NoModifier, panelPress);
    for (int i = 1; i <= 4; ++i) {
        QTest::mouseMove(_window, panelPress - QPoint(i * 10, 0));
    }
    QTest::mouseRelease(_window, Qt::LeftButton, Qt::NoModifier, panelPress - QPoint(40, 0));
    QTRY_VERIFY_WITH_TIMEOUT(planningHandle->mapToScene(QPointF(0, 0)).x() < panelBeforeDrag.x() - 10,
                             TestTimeout::mediumMs());
    QVERIFY(clickButton(QStringLiteral("caacWaypointListButton")));
    QCOMPARE(editorPanel->property("activePanel").toInt(), 3);
    QVERIFY(clickButton(QStringLiteral("caacGlobalSettingsButton")));
    QCOMPARE(editorPanel->property("activePanel").toInt(), 0);
    QVERIFY(clickButton(QStringLiteral("caacWaypointListButton")));
    QCOMPARE(editorPanel->property("activePanel").toInt(), 3);
    step("complete");
    const QString screenshot = qEnvironmentVariable("CAAC_UI_SCREENSHOT");
    if (!screenshot.isEmpty() && chinese) {
        QVERIFY(_window->grabWindow().save(screenshot));
    }
    master->removeAll();
}

void CaacRouteEditorUITest::_homeCenterWindowFocus()
{
    startUI();
    if (QTest::currentTestFailed()) {
        return;
    }
    QVERIFY(clickToolSelectDropdownButton(QStringLiteral("toolbar_viewFlightPlan")));
    auto* plan = findVisibleItem(_rootItem, QStringLiteral("mainView_plan"), TestTimeout::mediumMs());
    QVERIFY(plan);
    auto* master = plan->property("_planMasterController").value<PlanMasterController*>();
    QVERIFY(master);
    auto* mission = master->missionController();
    auto* map = findVisibleItem(_rootItem, QStringLiteral("planView_map"));
    QVERIFY(map);
    map->setProperty("center", QVariant::fromValue(QGeoCoordinate(30, 120)));
    map->setProperty("zoomLevel", 16.0);
    QTRY_VERIFY_WITH_TIMEOUT(map->property("mapReady").toBool(), TestTimeout::mediumMs());
    QVERIFY(clickItemFraction(QStringLiteral("planView_map"), 0.4, 0.5));
    QTest::mouseDClick(_window, Qt::LeftButton, Qt::NoModifier,
                       map->mapToScene(QPointF(map->width() * 0.4, map->height() * 0.5)).toPoint());
    QTRY_COMPARE_WITH_TIMEOUT(mission->visualItems()->count(), 2, TestTimeout::mediumMs());
    QTest::mouseDClick(_window, Qt::LeftButton, Qt::NoModifier,
                       map->mapToScene(QPointF(map->width() * 0.5, map->height() * 0.55)).toPoint());
    QTRY_COMPARE_WITH_TIMEOUT(mission->visualItems()->count(), 3, TestTimeout::mediumMs());

#if defined(Q_OS_WIN) || (defined(Q_OS_LINUX) && !defined(Q_OS_ANDROID))
    QProcess otherApplication;
    const auto stopOtherApplication = [&] {
        if (otherApplication.state() != QProcess::NotRunning) {
            otherApplication.terminate();
            if (!otherApplication.waitForFinished(TestTimeout::shortMs())) {
                otherApplication.kill();
                otherApplication.waitForFinished(TestTimeout::shortMs());
            }
        }
    };
    const auto cleanupOtherApplication = qScopeGuard(stopOtherApplication);
    QString helper = QCoreApplication::applicationDirPath() + QStringLiteral("/CaacFocusTestWindow");
#ifdef Q_OS_WIN
    helper += QStringLiteral(".exe");
#endif
    QVERIFY(QFileInfo::exists(helper));
    const auto changeFocus = [&] {
        QLocalServer control;
        control.setSocketOptions(QLocalServer::UserAccessOption);
        const QString serverName = QStringLiteral("caac-focus-") + QUuid::createUuid().toString(QUuid::Id128);
        QVERIFY(control.listen(serverName));
        _window->requestActivate();
        QTRY_VERIFY_WITH_TIMEOUT(QGuiApplication::focusWindow() == _window && _window->isActive(),
                                 TestTimeout::mediumMs());
        otherApplication.start(helper, {serverName, QString::number(_window->winId())});
        QVERIFY(otherApplication.waitForStarted(TestTimeout::mediumMs()));
        QTRY_VERIFY_WITH_TIMEOUT(control.hasPendingConnections(), TestTimeout::mediumMs());
        std::unique_ptr<QLocalSocket> connection(control.nextPendingConnection());
        QVERIFY(connection);
        QTRY_VERIFY_WITH_TIMEOUT(QGuiApplication::applicationState() == Qt::ApplicationInactive && !_window->isActive(),
                                 TestTimeout::mediumMs());
        QCOMPARE(connection->write("return\n"), 7);
        QVERIFY(connection->flush());
        QTRY_VERIFY_WITH_TIMEOUT(QGuiApplication::applicationState() == Qt::ApplicationActive && _window->isActive(),
                                 TestTimeout::mediumMs());
        stopOtherApplication();
    };
#else
    // Other native platforms retain the second-window regression until an external focus helper is available.
    QQuickWindow otherWindow;
    otherWindow.setTitle(QStringLiteral("Map focus regression"));
    otherWindow.resize(320, 240);
    otherWindow.show();
    const auto changeFocus = [&] {
        otherWindow.requestActivate();
        QTRY_VERIFY_WITH_TIMEOUT(otherWindow.isActive() && !_window->isActive(), TestTimeout::mediumMs());
        _window->requestActivate();
        QTRY_VERIFY_WITH_TIMEOUT(_window->isActive() && !otherWindow.isActive(), TestTimeout::mediumMs());
    };
#endif
    QVERIFY(!findVisibleItem(_rootItem, QStringLiteral("planToolStrip_roiButton")));
    const auto beforeCenter = master->saveToJson();
    const auto home = mission->plannedHomePosition();
    QVERIFY(home.isValid());
    const auto firstCoordinate = mission->visualItems()->value<SimpleMissionItem*>(1)->coordinate();
    const auto secondCoordinate = mission->visualItems()->value<SimpleMissionItem*>(2)->coordinate();
    const QGeoCoordinate routeCenter((firstCoordinate.latitude() + secondCoordinate.latitude()) / 2,
                                     (firstCoordinate.longitude() + secondCoordinate.longitude()) / 2);
    const auto initialZoom = map->property("zoomLevel").toDouble();
    auto centeredPlan = beforeCenter;
    for (int i = 0; i < 3; ++i) {
        map->setProperty("center", QVariant::fromValue(home.atDistanceAndAzimuth(500, 90)));
        auto* panel = findVisibleItem(_rootItem, QStringLiteral("caacRouteEditor"));
        auto* point = findVisibleItem(_rootItem, QStringLiteral("missionItemDrag_1"));
        QVERIFY(panel && point);
        map->setProperty("center", QVariant::fromValue(firstCoordinate));
        const auto middle = map->mapToScene(QPointF(map->width() / 2, map->height() / 2));
        QTRY_VERIFY_WITH_TIMEOUT(
            QLineF(point->mapToScene(QPointF(point->width() / 2, point->height() / 2)), middle).length() < 1,
            TestTimeout::mediumMs());
        const auto cameraAtMenu = map->property("center").value<QGeoCoordinate>();
        QTest::mouseClick(_window, Qt::RightButton, Qt::NoModifier, middle.toPoint());
        QVERIFY(clickButton(QStringLiteral("caacTakeoffPointButton")));
        QVERIFY(clickButton(QStringLiteral("caacContextCenterHome")));
        QTRY_VERIFY_WITH_TIMEOUT(mission->plannedHomePosition().distanceTo(routeCenter) < 0.1, TestTimeout::mediumMs());
        QCOMPARE(map->property("center").value<QGeoCoordinate>(), cameraAtMenu);
        QCOMPARE(mission->plannedHomePosition().altitude(), home.altitude());
        QCOMPARE(mission->visualItems()->value<SimpleMissionItem*>(1)->coordinate(), firstCoordinate);
        QCOMPARE(mission->visualItems()->value<SimpleMissionItem*>(2)->coordinate(), secondCoordinate);
        if (i == 0) {
            centeredPlan = master->saveToJson();
        }
        QCOMPARE(map->property("zoomLevel").toDouble(), initialZoom);
        QVERIFY(map->property("directMissionItemEditing").toBool());
        changeFocus();
        if (QTest::currentTestFailed()) {
            return;
        }
        QVERIFY(clickItemFraction(QStringLiteral("planView_map"), 0.45, 0.35));
        QCOMPARE(master->saveToJson(), centeredPlan);
        QVERIFY(!mission->property("isROIActive").toBool());
    }
    QTest::keyClick(_window, Qt::Key_Z, Qt::ControlModifier);
    QTRY_COMPARE_WITH_TIMEOUT(master->saveToJson(), beforeCenter, TestTimeout::mediumMs());
    QTest::mouseDClick(_window, Qt::LeftButton, Qt::NoModifier,
                       map->mapToScene(QPointF(map->width() * 0.45, map->height() * 0.35)).toPoint());
    QTRY_COMPARE_WITH_TIMEOUT(mission->visualItems()->count(), 4, TestTimeout::mediumMs());
    changeFocus();
    if (QTest::currentTestFailed()) {
        return;
    }
    QVERIFY(!mission->property("isROIActive").toBool());
    QCOMPARE(mission->visualItems()->count(), 4);
    master->removeAll();
}

void CaacRouteEditorUITest::_planningUtilities()
{
    startUI();
    if (QTest::currentTestFailed()) {
        return;
    }
    QVERIFY(clickToolSelectDropdownButton(QStringLiteral("toolbar_viewFlightPlan")));
    auto* plan = findVisibleItem(_rootItem, QStringLiteral("mainView_plan"));
    QVERIFY(plan);
    auto* master = plan->property("_planMasterController").value<PlanMasterController*>();
    QVERIFY(master);
    auto* panel = findVisibleItem(_rootItem, QStringLiteral("caacRouteEditor"));
    QVERIFY(panel);
    auto* editor = qobject_cast<CaacRouteEditorController*>(panel->property("routeController").value<QObject*>());
    QVERIFY(editor);
    master->missionController()->setHomePosition(QGeoCoordinate(30, 120, 20));
    QVERIFY(editor->addWaypoint(QGeoCoordinate(30.001, 120)));
    auto* map = findVisibleItem(_rootItem, QStringLiteral("planView_map"));
    QVERIFY(map);
    map->setProperty("center", QVariant::fromValue(QGeoCoordinate(30, 120)));
    map->setProperty("zoomLevel", 16.0);
    QTRY_VERIFY_WITH_TIMEOUT(map->property("mapReady").toBool(), TestTimeout::mediumMs());
    const auto before = master->saveToJson();
    QVERIFY(clickButton(QStringLiteral("caacGlobalSettingsButton")));
    QVERIFY(clickButton(QStringLiteral("caacUnitConverter")));
    auto* converterHandle = findVisibleItem(_rootItem, QStringLiteral("popupDialog_dragHandle"));
    auto* converterFrame = findVisibleItem(_rootItem, QStringLiteral("popupDialog_frame"));
    QVERIFY(converterHandle && converterFrame);
    const auto converterFrameBefore = converterFrame->mapToScene(QPointF());
    const auto converterPress =
        converterHandle->mapToScene(QPointF(converterHandle->width() / 2, converterHandle->height() / 2)).toPoint();
    QTest::mousePress(_window, Qt::LeftButton, Qt::NoModifier, converterPress);
    for (int step = 1; step <= 4; ++step) {
        QTest::mouseMove(_window, converterPress + QPoint(step * 8, step * 3));
    }
    QTest::mouseRelease(_window, Qt::LeftButton, Qt::NoModifier, converterPress + QPoint(32, 12));
    QTRY_VERIFY_WITH_TIMEOUT(converterFrame->mapToScene(QPointF()).x() > converterFrameBefore.x() + 5,
                             TestTimeout::mediumMs());
    QVERIFY(_rootItem->boundingRect().contains(converterFrame->mapRectToScene(converterFrame->boundingRect())));
    auto* value = findVisibleItem(_rootItem, QStringLiteral("caacConvertValue"));
    auto* quantity = findVisibleItem(_rootItem, QStringLiteral("caacConvertQuantity"));
    auto* from = findVisibleItem(_rootItem, QStringLiteral("caacConvertFrom"));
    auto* to = findVisibleItem(_rootItem, QStringLiteral("caacConvertTo"));
    auto* result = findVisibleItem(_rootItem, QStringLiteral("caacConvertResult"));
    QVERIFY(value && quantity && from && to && result);
    value->setProperty("text", QStringLiteral("1000"));
    QTRY_COMPARE_WITH_TIMEOUT(result->property("text").toString(), QStringLiteral("1 km"), TestTimeout::mediumMs());
    quantity->setProperty("currentIndex", 1);
    from->setProperty("currentIndex", 1);
    to->setProperty("currentIndex", 0);
    value->setProperty("text", QStringLiteral("36"));
    QTRY_COMPARE_WITH_TIMEOUT(result->property("text").toString(), QStringLiteral("10 m/s"), TestTimeout::mediumMs());
    value->setProperty("text", QString());
    QTRY_COMPARE_WITH_TIMEOUT(
        result->property("text").toString(),
        QCoreApplication::translate("CaacUnitConverter", "Enter a finite value and select valid units."),
        TestTimeout::mediumMs());
    QVERIFY(rejectDialog());
    QVERIFY(clickButton(QStringLiteral("caacMeasureTool")));
    QTRY_COMPARE_WITH_TIMEOUT(panel->property("measurePick").toInt(), 1, TestTimeout::mediumMs());
    QVERIFY(!findVisibleItem(_rootItem, QStringLiteral("caacMeasureDialog"), 0));
    QVERIFY(clickItemFraction(QStringLiteral("planView_map"), 0.3, 0.4));
    QTRY_COMPARE_WITH_TIMEOUT(panel->property("measurePick").toInt(), 2, TestTimeout::mediumMs());
    QVERIFY(clickItemFraction(QStringLiteral("planView_map"), 0.4, 0.4));
    QTRY_COMPARE_WITH_TIMEOUT(panel->property("measurePick").toInt(), 0, TestTimeout::mediumMs());
    QVERIFY(!findVisibleItem(_rootItem, QStringLiteral("caacMeasureDialog"), 0));
    auto* markerA = findVisibleItem(_rootItem, QStringLiteral("caacMeasureMarkerA"));
    QVERIFY(markerA);
    for (const auto& label : {QStringLiteral("A"), QStringLiteral("B")}) {
        auto* marker = findVisibleItem(_rootItem, QStringLiteral("caacMeasureMarker") + label);
        auto* drag = findVisibleItem(_rootItem, QStringLiteral("caacMeasureDrag") + label);
        QVERIFY(marker && drag);
        const auto original = panel->property(qPrintable(QStringLiteral("measure") + label)).value<QGeoCoordinate>();
        const auto mapCenter = map->property("center").value<QGeoCoordinate>();
        const auto press = marker->mapToScene(QPointF(marker->width() / 2, marker->height() / 2)).toPoint();
        QElapsedTimer held;
        held.start();
        QTest::mousePress(_window, Qt::LeftButton, Qt::NoModifier, press);
        QTest::mouseMove(_window, press + QPoint(1, 1));
        QTRY_VERIFY_WITH_TIMEOUT(held.elapsed() >= drag->property("pressAndHoldInterval").toInt(),
                                 TestTimeout::mediumMs());
        QVERIFY(drag->property("dragging").toBool());
        for (int step = 1; step <= 5; ++step) {
            const QPoint position = press + QPoint(step * 8, step * 3);
            QTest::mouseMove(_window, position);
            QTRY_VERIFY_WITH_TIMEOUT(
                QLineF(marker->mapToScene(QPointF(marker->width() / 2, marker->height() / 2)), position).length() < 2,
                TestTimeout::mediumMs());
        }
        QTest::mouseRelease(_window, Qt::LeftButton, Qt::NoModifier, press + QPoint(40, 15));
        QVERIFY(QGCGeo::geodesicDistance(
                    original, panel->property(qPrintable(QStringLiteral("measure") + label)).value<QGeoCoordinate>()) >
                0.1);
        QVERIFY(QGCGeo::geodesicDistance(mapCenter, map->property("center").value<QGeoCoordinate>()) < 0.02);
        QVERIFY(!findVisibleItem(_rootItem, QStringLiteral("caacMeasureDialog"), 0));
        QCOMPARE(master->saveToJson(), before);
    }
    QVERIFY(findVisibleItem(_rootItem, QStringLiteral("caacMeasureMapValues")));
    QTest::mouseClick(_window, Qt::RightButton, Qt::NoModifier,
                      markerA->mapToScene(QPointF(markerA->width() / 2, markerA->height() / 2)).toPoint());
    auto* format = findVisibleItem(_rootItem, QStringLiteral("caacMeasureALatitudeFormatToggle"));
    QVERIFY(format);
    QVERIFY(scrollIntoView(format, QStringLiteral("popupDialog_contentFlickable")));
    QVERIFY(clickButton(QStringLiteral("caacMeasureALatitudeFormatToggle")));
    QVERIFY(findVisibleItem(_rootItem, QStringLiteral("caacMeasureALatitudeDegrees")));
    QVERIFY(clickButton(QStringLiteral("caacMeasureALatitudeFormatToggle")));
    const auto a = panel->property("measureA").value<QGeoCoordinate>();
    const auto b = panel->property("measureB").value<QGeoCoordinate>();
    QVERIFY(a.isValid() && b.isValid());
    auto* measured = findVisibleItem(_rootItem, QStringLiteral("caacMeasureResult"));
    QVERIFY(measured);
    const auto expected = QCoreApplication::translate("CaacMeasureDialog", "Distance: %1 m · Bearing A → B: %2°")
                              .arg(QGCGeo::geodesicDistance(a, b), 0, 'f', 2)
                              .arg(QGCGeo::geodesicAzimuth(a, b), 0, 'f', 2);
    QTRY_COMPARE_WITH_TIMEOUT(measured->property("text").toString(), expected, TestTimeout::mediumMs());
    QCOMPARE(master->saveToJson(), before);
    auto* aLatitude = findVisibleItem(_rootItem, QStringLiteral("caacMeasureALatitude"));
    QVERIFY(aLatitude);
    QQmlExpression contextMenu(qmlContext(aLatitude), aLatitude, QStringLiteral("ContextMenu.menu === null"));
    QVERIFY(contextMenu.evaluate().toBool());
    QVERIFY(!contextMenu.hasError());
    QTest::mouseClick(_window, Qt::RightButton, Qt::NoModifier,
                      aLatitude->mapToScene(QPointF(aLatitude->width() / 2, aLatitude->height() / 2)).toPoint());
    QVERIFY(contextMenu.evaluate().toBool());
    aLatitude->setProperty("text", QStringLiteral("91"));
    auto* apply = findVisibleItem(_rootItem, QStringLiteral("caacMeasureApply"));
    QVERIFY(apply);
    QVERIFY(scrollIntoView(apply, QStringLiteral("popupDialog_contentFlickable")));
    QVERIFY(clickButton(QStringLiteral("caacMeasureApply")));
    QCOMPARE(panel->property("measureA").value<QGeoCoordinate>(), a);
    QCOMPARE(master->saveToJson(), before);
    QVERIFY(rejectDialog());
    const auto screenshot = qEnvironmentVariable("CAAC_MEASURE_UI_SCREENSHOT");
    if (!screenshot.isEmpty() && inherits("CaacChineseUITest")) {
        QSignalSpy frame(_window, &QQuickWindow::frameSwapped);
        _window->update();
        QVERIFY(frame.wait(TestTimeout::mediumMs()));
        QVERIFY(_window->grabWindow().save(screenshot));
    }
    QTest::mouseClick(_window, Qt::RightButton, Qt::NoModifier,
                      markerA->mapToScene(QPointF(markerA->width() / 2, markerA->height() / 2)).toPoint());
    auto* clear = findVisibleItem(_rootItem, QStringLiteral("caacMeasureClear"));
    QVERIFY(clear);
    QVERIFY(scrollIntoView(clear, QStringLiteral("popupDialog_contentFlickable")));
    QVERIFY(clickButton(QStringLiteral("caacMeasureClear")));
    QTRY_VERIFY_WITH_TIMEOUT(!findVisibleItem(_rootItem, QStringLiteral("caacMeasureDialog"), 0),
                             TestTimeout::mediumMs());
    QVERIFY(!panel->property("measureA").value<QGeoCoordinate>().isValid());
    QVERIFY(!panel->property("measureB").value<QGeoCoordinate>().isValid());
    QVERIFY(!findVisibleItem(_rootItem, QStringLiteral("caacMeasureMapValues"), 0));
    QCOMPARE(master->saveToJson(), before);
    auto* specify = findVisibleItem(_rootItem, QStringLiteral("caacSpecifyMissionSpeed"));
    QVERIFY(specify);
    QVERIFY(scrollIntoView(specify, QStringLiteral("caacGlobalSettingsScroller")));
    QVERIFY(clickButton(QStringLiteral("caacSpecifyMissionSpeed")));
    auto* speed = findVisibleItem(_rootItem, QStringLiteral("caacMissionSpeed"));
    QVERIFY(speed);
    speed->setProperty("text", QStringLiteral("8.5"));
    apply = findVisibleItem(_rootItem, QStringLiteral("caacApplyMissionSpeed"));
    QVERIFY(apply);
    QVERIFY(scrollIntoView(apply, QStringLiteral("caacGlobalSettingsScroller")));
    QVERIFY(clickButton(QStringLiteral("caacApplyMissionSpeed")));
    QVERIFY(master->saveToJson() != before);
    QVERIFY(clickButton(QStringLiteral("caacUndo")));
    QCOMPARE(master->saveToJson(), before);
    master->removeAll();
}

UT_REGISTER_TEST(CaacRouteEditorUITest, TestLabel::Integration, TestLabel::MissionManager)

class CaacChineseUITest : public CaacRouteEditorUITest
{
    Q_OBJECT
};

UT_REGISTER_TEST(CaacChineseUITest, TestLabel::Integration, TestLabel::MissionManager)

#include "CaacRouteEditorUITest.moc"
