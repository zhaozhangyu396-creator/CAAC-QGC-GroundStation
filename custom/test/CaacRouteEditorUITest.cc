#include <atomic>
#include <cmath>
#include <cstdio>
#include <limits>
#include <memory>

#include <QtCore/QByteArray>
#include <QtCore/QFile>
#include <QtCore/QLineF>
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

#include "AppSettings.h"
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
    QVERIFY(clickToolSelectDropdownButton(QStringLiteral("toolbar_viewPlan")));
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
    QTRY_VERIFY_WITH_TIMEOUT(graphDomainsValid(), TestTimeout::mediumMs());
    step("double-click second waypoint");
    QTest::mouseDClick(_window, Qt::LeftButton, Qt::NoModifier, b);
    QTRY_COMPARE_WITH_TIMEOUT(mission->visualItems()->count(), 3, TestTimeout::mediumMs());
    QTRY_VERIFY_WITH_TIMEOUT(graphDomainsValid(), TestTimeout::mediumMs());
    QPointer<SimpleMissionItem> first = mission->visualItems()->value<SimpleMissionItem*>(1);
    QVERIFY(first);
    auto* firstNumber = findVisibleItem(_rootItem, QStringLiteral("caacWaypointNumber_1"));
    QVERIFY(firstNumber);
    QCOMPARE(firstNumber->property("text").toString(), QStringLiteral("1"));
    auto* planTree = findItem(_rootItem, QStringLiteral("planView_planTree"));
    QVERIFY(planTree && !planTree->isVisible());
    QVERIFY(clickButton(QStringLiteral("caacMissionSettings")));
    QTRY_VERIFY_WITH_TIMEOUT(planTree->isVisible(), TestTimeout::mediumMs());
    QVERIFY(clickButton(QStringLiteral("caacMissionSettings")));
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
    QTest::mousePress(_window, Qt::RightButton, Qt::NoModifier, marker);
    auto* rightClickGrabber = _window->mouseGrabberItem();
    const bool waypointGrabbed = rightClickGrabber && rightClickGrabber->parentItem() == dragger;
    const QString grabbedItem = rightClickGrabber ? QString::fromLatin1(rightClickGrabber->metaObject()->className()) +
                                                        QLatin1Char(':') + rightClickGrabber->objectName()
                                                  : QStringLiteral("none");
    QTest::mouseRelease(_window, Qt::RightButton, Qt::NoModifier, marker);
    QVERIFY2(waypointGrabbed, qPrintable(QStringLiteral("Waypoint right-click intercepted by ") + grabbedItem));
    QVERIFY(waitForDialog(QCoreApplication::translate("CaacWaypointDialog", "Waypoint properties"),
                          TestTimeout::mediumMs()));
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
    QVERIFY(clickButton(QStringLiteral("caacRemoveWaypoint")));
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
    QVERIFY(clickButton(QStringLiteral("caacRotateRoute")));
    QVERIFY(waitForDialog(QCoreApplication::translate("CaacGeometryDialog", "Route rotation")));
    auto* rotationOperation = findVisibleItem(_rootItem, QStringLiteral("caacGeometryOperation"));
    QVERIFY(rotationOperation);
    QCOMPARE(rotationOperation->property("count").toInt(), 2);
    auto* rotationLatitude = findVisibleItem(_rootItem, QStringLiteral("caacRotationLatitude"));
    auto* rotationLongitude = findVisibleItem(_rootItem, QStringLiteral("caacRotationLongitude"));
    auto* rotationAngle = findVisibleItem(_rootItem, QStringLiteral("caacRotationAngle"));
    QVERIFY(rotationLatitude && rotationLongitude && rotationAngle);
    rotationLatitude->setProperty("text", QStringLiteral("30"));
    rotationLongitude->setProperty("text", QStringLiteral("120"));
    rotationAngle->setProperty("text", QStringLiteral("90"));
    QVERIFY(acceptDialog());
    QVERIFY(editor->canUndo());
    QVERIFY(master->saveToJson() != beforeRotation);
    QVERIFY(clickButton(QStringLiteral("caacUndo")));
    QCOMPARE(master->saveToJson(), beforeRotation);
    QVERIFY(clickButton(QStringLiteral("caacRedo")));
    QVERIFY(master->saveToJson() != beforeRotation);

    step("relative point through native controls");
    QVERIFY(clickButton(QStringLiteral("caacRelativePoint")));
    QVERIFY(editorPanel->property("relativeEditorVisible").toBool());
    QVERIFY(!findVisibleItem(_rootItem, QStringLiteral("caacWaypointList")));
    auto* relativeDistance = findVisibleItem(_rootItem, QStringLiteral("caacRelativeDistance"));
    QVERIFY(relativeDistance);
    auto* panelTitle = findVisibleItem(_rootItem, QStringLiteral("caacPanelTitle"));
    QVERIFY(panelTitle);
    QCOMPARE(panelTitle->property("text").toString(),
             chinese ? QStringLiteral("相对坐标编辑器") : QStringLiteral("Relative coordinate editor"));
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
    QVERIFY(clickButton(QStringLiteral("caacMissionSettings")));
    QVERIFY(!editorPanel->property("relativeEditorVisible").toBool());

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
    QVERIFY(clickButton(QStringLiteral("caacEditWaypoint")));
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
        const auto* rotateButton = findVisibleItem(_rootItem, QStringLiteral("caacRotateRoute"));
        QVERIFY(rotateButton);
        QCOMPARE(rotateButton->property("text").toString(), QStringLiteral("航线旋转"));
        QCOMPARE(QCoreApplication::translate("CaacWaypointDialog", "Waypoint properties"), QStringLiteral("航点属性"));
    }
    step("complete");
    const QString screenshot = qEnvironmentVariable("CAAC_UI_SCREENSHOT");
    if (!screenshot.isEmpty() && chinese) {
        QVERIFY(_window->grabWindow().save(screenshot));
    }
    master->removeAll();
}

UT_REGISTER_TEST(CaacRouteEditorUITest, TestLabel::Integration, TestLabel::MissionManager)

class CaacChineseUITest : public CaacRouteEditorUITest
{
    Q_OBJECT
};

UT_REGISTER_TEST(CaacChineseUITest, TestLabel::Integration, TestLabel::MissionManager)

#include "CaacRouteEditorUITest.moc"
