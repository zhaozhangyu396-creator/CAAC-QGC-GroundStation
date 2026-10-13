#include <cmath>
#include <utility>

#include <QtCore/QLineF>
#include <QtCore/QRectF>
#include <QtGui/QImage>
#include <QtQml/QQmlExpression>
#include <QtQuick/QQuickItem>
#include <QtQuick/QQuickWindow>
#include <QtTest/QSignalSpy>
#include <QtTest/QTest>

#include "AppSettings.h"
#include "CaacRouteEditorController.h"
#include "Fact.h"
#include "FlightPathSegment.h"
#include "MissionController.h"
#include "PlanMasterController.h"
#include "QGCGeo.h"
#include "QmlUITestBase.h"
#include "SettingsManager.h"
#include "SimpleMissionItem.h"

class CaacPlanningInteractionUITest : public QmlUITestBase
{
    Q_OBJECT
private slots:
    void _pointMenusAndOrientation();
    void _liveRoutePreview();
    void _homeRelativeEditing();
};

void CaacPlanningInteractionUITest::_homeRelativeEditing()
{
    startUI();
    if (QTest::currentTestFailed()) {
        return;
    }
    QVERIFY(clickToolSelectDropdownButton(QStringLiteral("toolbar_viewPlan")));
    auto* plan = findVisibleItem(_rootItem, QStringLiteral("mainView_plan"));
    auto* map = findVisibleItem(_rootItem, QStringLiteral("planView_map"));
    auto* panel = findVisibleItem(_rootItem, QStringLiteral("caacRouteEditor"));
    QVERIFY(plan && map && panel);
    auto* master = plan->property("_planMasterController").value<PlanMasterController*>();
    auto* editor = panel->property("routeController").value<CaacRouteEditorController*>();
    QVERIFY(master && editor);
    auto* mission = master->missionController();
    const QGeoCoordinate home(30, 120, 20);
    mission->setHomePosition(home);
    const auto reference = QGCGeo::geodesicDestination(home, 90, 100);
    QVERIFY(editor->addWaypoint(reference));
    QVERIFY(editor->addWaypoint(QGCGeo::geodesicDestination(home, 180, 130)));
    const auto original = master->saveToJson();
    map->setProperty("zoomLevel", 18);
    const auto midpoint = QGCGeo::geodesicDestination(home, 90, 50);
    map->setProperty("center", QVariant::fromValue(midpoint));
    auto* split = findVisibleItem(_rootItem, QStringLiteral("caacHomeSplitButton"), TestTimeout::mediumMs());
    QVERIFY(split);
    QTRY_VERIFY_WITH_TIMEOUT(QLineF(split->mapToScene(QPointF(split->width() / 2, split->height() / 2)),
                                    map->mapToScene(QPointF(map->width() / 2, map->height() / 2)))
                                     .length() < 1,
                             TestTimeout::mediumMs());
    QVERIFY(clickButton(QStringLiteral("caacHomeSplitButton")));
    QTRY_COMPARE_WITH_TIMEOUT(mission->visualItems()->count(), 4, TestTimeout::mediumMs());
    auto* inserted = mission->visualItems()->value<SimpleMissionItem*>(1);
    QVERIFY(inserted && inserted->command() == MAV_CMD_NAV_WAYPOINT);
    QVERIFY(QGCGeo::geodesicDistance(inserted->coordinate(), midpoint) < 0.01);
    QVERIFY(QGCGeo::geodesicDistance(mission->visualItems()->value<SimpleMissionItem*>(2)->coordinate(), reference) <
            0.01);
    QCOMPARE(mission->plannedHomePosition().altitude(), 20.0);
    QVERIFY(clickButton(QStringLiteral("caacUndo")));
    QCOMPARE(master->saveToJson(), original);
    map->setProperty("center", QVariant::fromValue(home));
    auto* dragger = findVisibleItem(_rootItem, QStringLiteral("missionItemDrag_0"), TestTimeout::mediumMs());
    QVERIFY(dragger);
    const auto position = [&]() { return dragger->mapToScene(QPointF(dragger->width() / 2, dragger->height() / 2)); };
    QTRY_VERIFY_WITH_TIMEOUT(
        QLineF(position(), map->mapToScene(QPointF(map->width() / 2, map->height() / 2))).length() < 1,
        TestTimeout::mediumMs());
    QVERIFY(!_window->grabWindow().isNull());
    QTest::mouseClick(_window, Qt::LeftButton, Qt::NoModifier, position().toPoint());
    auto* combo = findVisibleItem(_rootItem, QStringLiteral("caacHomeRelativeReference"));
    auto* bearing = findVisibleItem(_rootItem, QStringLiteral("caacWaypointQuickBearing"));
    auto* distance = findVisibleItem(_rootItem, QStringLiteral("caacWaypointQuickDistance"));
    QVERIFY(combo && bearing && distance);
    QCOMPARE(combo->property("count").toInt(), 2);
    QCOMPARE(combo->property("currentIndex").toInt(), 0);
    QVERIFY(std::abs(distance->property("text").toString().toDouble() - 100) < 0.01);
    QVERIFY(!findVisibleItem(_rootItem, QStringLiteral("caacWaypointQuickDelete"), 0));
    combo->setProperty("currentIndex", 1);
    QVERIFY(QMetaObject::invokeMethod(combo, "activated", Q_ARG(int, 1)));
    QVERIFY(std::abs(distance->property("text").toString().toDouble() - 130) < 0.01);
    QCOMPARE(master->saveToJson(), original);
    combo->setProperty("currentIndex", 0);
    QVERIFY(QMetaObject::invokeMethod(combo, "activated", Q_ARG(int, 0)));
    QVERIFY(clickButton(QStringLiteral("caacWaypointFineTuning")));
    _window->requestActivate();
    QTRY_VERIFY_WITH_TIMEOUT(_window->isActive(), TestTimeout::mediumMs());
    QTest::keyClick(_window, Qt::Key_Right);
    QTRY_VERIFY_WITH_TIMEOUT(
        std::abs(QGCGeo::geodesicDistance(reference, mission->plannedHomePosition()) - 100.01) < 0.001,
        TestTimeout::mediumMs());
    QCOMPARE(mission->plannedHomePosition().altitude(), 20.0);
    bearing->setProperty("text", QStringLiteral("238"));
    distance->setProperty("text", QStringLiteral("28.1"));
    QVERIFY(clickButton(QStringLiteral("caacWaypointQuickConfirm")));
    QVERIFY(QGCGeo::geodesicDistance(mission->plannedHomePosition(),
                                     QGCGeo::geodesicDestination(reference, 238, 28.1)) < 0.01);
    QCOMPARE(mission->plannedHomePosition().altitude(), 20.0);
    QCOMPARE(mission->visualItems()->value<SimpleMissionItem*>(1)->coordinate().latitude(), reference.latitude());
    QVERIFY(clickButton(QStringLiteral("caacUndo")));
    QCOMPARE(master->saveToJson(), original);
}

void CaacPlanningInteractionUITest::_liveRoutePreview()
{
    startUI();
    if (QTest::currentTestFailed()) {
        return;
    }
    QVERIFY(clickToolSelectDropdownButton(QStringLiteral("toolbar_viewPlan")));
    auto* plan = findVisibleItem(_rootItem, QStringLiteral("mainView_plan"));
    auto* panel = findVisibleItem(_rootItem, QStringLiteral("caacRouteEditor"));
    QVERIFY(plan && panel);
    auto* master = plan->property("_planMasterController").value<PlanMasterController*>();
    auto* editor = panel->property("routeController").value<CaacRouteEditorController*>();
    QVERIFY(master && editor);
    const QGeoCoordinate home(30, 120, 20);
    master->missionController()->setHomePosition(home);
    QList<QGeoCoordinate> points;
    for (int bearing : {0, 90, 180}) {
        points.append(QGCGeo::geodesicDestination(home, bearing, 40));
        QVERIFY(editor->addWaypoint(points.last()));
    }
    const auto original = master->saveToJson();
    QVERIFY(!editor->hasSimulationSettings());
    QVERIFY(clickButton(QStringLiteral("caacGlobalSettingsButton")));
    QVERIFY(clickButtonScrolled(QStringLiteral("caacSimClosedRoute"), QStringLiteral("caacGlobalSettingsScroller")));
    const auto matches = [](QQuickItem* line, const QGeoCoordinate& from, const QGeoCoordinate& to) {
        if (!line) {
            return false;
        }
        QQmlExpression expression(qmlContext(line), line,
                                  QStringLiteral("path.length === 2 && Math.abs(path[0].latitude - %1) < 1e-8 && "
                                                 "Math.abs(path[0].longitude - %2) < 1e-8 && "
                                                 "Math.abs(path[1].latitude - %3) < 1e-8 && "
                                                 "Math.abs(path[1].longitude - %4) < 1e-8")
                                      .arg(from.latitude(), 0, 'g', 17)
                                      .arg(from.longitude(), 0, 'g', 17)
                                      .arg(to.latitude(), 0, 'g', 17)
                                      .arg(to.longitude(), 0, 'g', 17));
        const bool value = expression.evaluate().toBool();
        return !expression.hasError() && value;
    };
    QTRY_VERIFY_WITH_TIMEOUT(matches(findVisibleItem(_rootItem, QStringLiteral("caacSimulationPreview_closure"), 0),
                                     points.last(), points.first()),
                             TestTimeout::mediumMs());
    auto* start = findVisibleItem(_rootItem, QStringLiteral("caacSimStartPoint"));
    auto* returning = findVisibleItem(_rootItem, QStringLiteral("caacSimReturnPoint"));
    auto* loop = findVisibleItem(_rootItem, QStringLiteral("caacSimLoopMode"));
    QVERIFY(start && returning && loop);
    QVERIFY(loop->setProperty("currentIndex", 1));
    auto* count = findVisibleItem(_rootItem, QStringLiteral("caacSimLoopCount"));
    QVERIFY(count);
    QVERIFY(start->setProperty("currentIndex", 2));
    QVERIFY(returning->setProperty("currentIndex", 3));
    QTRY_VERIFY_WITH_TIMEOUT(
        matches(findVisibleItem(_rootItem, QStringLiteral("caacSimulationPreview_start"), 0), home, points.at(1)),
        TestTimeout::mediumMs());
    QTRY_VERIFY_WITH_TIMEOUT(matches(findVisibleItem(_rootItem, QStringLiteral("caacSimulationPreview_closure"), 0),
                                     points.first(), points.at(1)),
                             TestTimeout::mediumMs());
    QTRY_VERIFY_WITH_TIMEOUT(matches(findVisibleItem(_rootItem, QStringLiteral("caacSimulationPreview_return"), 0),
                                     points.at(1), points.last()),
                             TestTimeout::mediumMs());
    QVERIFY(clickButtonScrolled(QStringLiteral("caacSimClosedRoute"), QStringLiteral("caacGlobalSettingsScroller")));
    QVERIFY(loop->setProperty("currentIndex", 1));
    QVERIFY(count->setProperty("text", QStringLiteral("2")));
    QTRY_VERIFY_WITH_TIMEOUT(!findVisibleItem(_rootItem, QStringLiteral("caacSimulationPreview_closure"), 0),
                             TestTimeout::mediumMs());
    QTRY_VERIFY_WITH_TIMEOUT(matches(findVisibleItem(_rootItem, QStringLiteral("caacSimulationPreview_loop"), 0),
                                     points.last(), points.at(1)),
                             TestTimeout::mediumMs());
    QCOMPARE(master->saveToJson(), original);
    QVERIFY(!editor->hasSimulationSettings());
    auto* item = master->missionController()->visualItems()->value<SimpleMissionItem*>(2);
    QVERIFY(item);
    const auto moved = QGCGeo::geodesicDestination(home, 100, 60);
    item->setCoordinate(moved);
    QTRY_VERIFY_WITH_TIMEOUT(
        matches(findVisibleItem(_rootItem, QStringLiteral("caacSimulationPreview_start"), 0), home, moved),
        TestTimeout::mediumMs());
    QTRY_VERIFY_WITH_TIMEOUT(
        matches(findVisibleItem(_rootItem, QStringLiteral("caacSimulationPreview_loop"), 0), points.last(), moved),
        TestTimeout::mediumMs());
    QVERIFY(clickButtonScrolled(QStringLiteral("caacSaveSimulationSettings"),
                                QStringLiteral("caacGlobalSettingsScroller")));
    QVERIFY(editor->hasSimulationSettings());
    QCOMPARE(editor->simulationSettings().value("startSequence").toInt(), 2);
    QVERIFY(clickButton(QStringLiteral("caacWaypointListButton")));
    QTRY_VERIFY_WITH_TIMEOUT(
        matches(findVisibleItem(_rootItem, QStringLiteral("caacSimulationPreview_loop"), 0), points.last(), moved),
        TestTimeout::mediumMs());
}

void CaacPlanningInteractionUITest::_pointMenusAndOrientation()
{
    startUI();
    if (QTest::currentTestFailed()) {
        return;
    }
    QVERIFY(clickToolSelectDropdownButton(QStringLiteral("toolbar_viewPlan")));
    auto* plan = findVisibleItem(_rootItem, QStringLiteral("mainView_plan"));
    auto* map = findVisibleItem(_rootItem, QStringLiteral("planView_map"));
    auto* panel = findVisibleItem(_rootItem, QStringLiteral("caacRouteEditor"));
    QVERIFY(plan && map && panel);
    auto* master = plan->property("_planMasterController").value<PlanMasterController*>();
    auto* editor = panel->property("routeController").value<CaacRouteEditorController*>();
    QVERIFY(master && editor);
    for (const auto* name : {"caacReadWaypoints", "caacUploadWaypoints"}) {
        auto* button = findVisibleItem(_rootItem, QLatin1String(name));
        QVERIFY(button && !button->isEnabled());
    }
    auto* mission = master->missionController();
    const QGeoCoordinate home(30, 120, 20);
    mission->setHomePosition(home);
    const auto a = QGCGeo::geodesicDestination(home, 90, 7.1);
    QVERIFY(editor->addWaypoint(a));
    QVERIFY(editor->addWaypoint(a));
    QTRY_COMPARE_WITH_TIMEOUT(mission->simpleFlightPathSegments()->count(), 2, TestTimeout::mediumMs());
    auto* firstLeg = mission->simpleFlightPathSegments()->value<FlightPathSegment*>(0);
    QVERIFY(firstLeg);
    QVERIFY(QGCGeo::geodesicDistance(firstLeg->coordinate1(), home) < 0.01);
    QVERIFY(QGCGeo::geodesicDistance(firstLeg->coordinate2(), a) < 0.01);
    QVERIFY(firstLeg->setProperty("terrainCollision", true));
    QVERIFY(findVisibleItem(_rootItem, QStringLiteral("caacTerrainClearanceWarning")));
    QVERIFY(firstLeg->setProperty("terrainCollision", false));
    QTRY_VERIFY_WITH_TIMEOUT(!findVisibleItem(_rootItem, QStringLiteral("caacTerrainClearanceWarning"), 0),
                             TestTimeout::mediumMs());
    map->setProperty("zoomLevel", 18);
    map->setProperty("center", QVariant::fromValue(home));
    auto* homeDragger = findVisibleItem(_rootItem, QStringLiteral("missionItemDrag_0"), TestTimeout::mediumMs());
    QVERIFY(homeDragger);
    const auto beforeHomeDrag = master->saveToJson();
    const auto neighborBeforeHomeDrag = mission->visualItems()->value<SimpleMissionItem*>(2)->coordinate();
    const auto homeMapCenter = map->property("center").value<QGeoCoordinate>();
    QTRY_VERIFY_WITH_TIMEOUT(
        QLineF(homeDragger->mapToScene(QPointF(homeDragger->width() / 2, homeDragger->height() / 2)),
               map->mapToScene(QPointF(map->width() / 2, map->height() / 2)))
                .length() < 1,
        TestTimeout::mediumMs());
    QVERIFY(!_window->grabWindow().isNull());
    const auto homeScenePosition = [homeDragger]() {
        for (auto* ancestor = homeDragger; ancestor; ancestor = ancestor->parentItem()) {
            ancestor->ensurePolished();
        }
        return homeDragger->mapToScene(QPointF(homeDragger->width() / 2, homeDragger->height() / 2));
    };
    QPointF lastHomePosition(qQNaN(), qQNaN());
    QVERIFY(waitForCondition(
        [&]() {
            const auto position = homeScenePosition();
            const bool stable = position == lastHomePosition;
            lastHomePosition = position;
            return stable;
        },
        TestTimeout::shortMs(), QStringLiteral("Home drag position settled")));
    const auto homePress = homeScenePosition().toPoint();
    QTest::mousePress(_window, Qt::LeftButton, Qt::NoModifier, homePress);
    auto* homeGrabber = _window->mouseGrabberItem();
    QVERIFY(homeGrabber);
    QTRY_VERIFY_WITH_TIMEOUT(homeGrabber->property("pressed").toBool(), TestTimeout::mediumMs());
    QCOMPARE(homeGrabber->parentItem()->property("gestureTarget").value<QObject*>(), homeDragger);
    for (int step = 1; step <= 5; ++step) {
        QTest::mouseMove(_window, homePress + QPoint(step * 8, step * 4));
    }
    QTRY_VERIFY_WITH_TIMEOUT(QGCGeo::geodesicDistance(mission->plannedHomePosition(), home) > 1,
                             TestTimeout::mediumMs());
    QTest::mouseRelease(_window, Qt::LeftButton, Qt::NoModifier, homePress + QPoint(40, 20));
    QTRY_VERIFY_WITH_TIMEOUT(QGCGeo::geodesicDistance(mission->plannedHomePosition(), home) > 1,
                             TestTimeout::mediumMs());
    QCOMPARE(mission->plannedHomePosition().altitude(), home.altitude());
    QCOMPARE(mission->visualItems()->value<SimpleMissionItem*>(2)->coordinate(), neighborBeforeHomeDrag);
    QVERIFY(QGCGeo::geodesicDistance(homeMapCenter, map->property("center").value<QGeoCoordinate>()) < 0.02);
    QVERIFY(clickButton(QStringLiteral("caacUndo")));
    QCOMPARE(master->saveToJson(), beforeHomeDrag);
    mission->setCurrentPlanViewSeqNum(1, true);
    map->setProperty("center", QVariant::fromValue(a));
    auto* dragger = findVisibleItem(_rootItem, QStringLiteral("missionItemDrag_2"), TestTimeout::mediumMs());
    QVERIFY(dragger);
    auto* indicator = dragger->property("itemIndicator").value<QQuickItem*>();
    QVERIFY(indicator);
    QTRY_VERIFY_WITH_TIMEOUT(findVisibleItem(_rootItem, QStringLiteral("missionItemMapLabel_2"), 0),
                             TestTimeout::mediumMs());
    QTRY_VERIFY_WITH_TIMEOUT(!findVisibleItem(_rootItem, QStringLiteral("missionItemMapLabel_1"), 0),
                             TestTimeout::mediumMs());
    const auto markerPosition = [&]() {
        return dragger->mapToScene(indicator->property("anchorPoint").toPointF() +
                                   QPointF(dragger->property("_touchMarginHorizontal").toDouble(),
                                           dragger->property("_touchMarginVertical").toDouble()));
    };
    const auto middle = map->mapToScene(QPointF(map->width() / 2, map->height() / 2));
    QTRY_VERIFY_WITH_TIMEOUT(QLineF(markerPosition(), middle).length() < 1, TestTimeout::mediumMs());
    QTest::mouseMove(_window, markerPosition().toPoint());
    auto* overlap = findVisibleItem(_rootItem, QStringLiteral("caacWaypointOverlapText"), TestTimeout::mediumMs());
    QVERIFY(overlap);
    const auto labels = overlap->property("text").toString();
    QVERIFY(labels.contains(QStringLiteral("1")) && labels.contains(QStringLiteral("2")));
    QVERIFY(!labels.contains(QCoreApplication::translate("CaacWaypointOverlap", "Takeoff point")));
    QTest::mousePress(_window, Qt::LeftButton, Qt::NoModifier, markerPosition().toPoint());
    auto* waypointGrabber = _window->mouseGrabberItem();
    QVERIFY(waypointGrabber);
    QCOMPARE(waypointGrabber->parentItem()->property("gestureTarget").value<QObject*>(), dragger);
    QTest::mouseRelease(_window, Qt::LeftButton, Qt::NoModifier, markerPosition().toPoint());
    QVERIFY(findVisibleItem(_rootItem, QStringLiteral("caacWaypointQuickBearing")));
    QVERIFY(findVisibleItem(_rootItem, QStringLiteral("caacWaypointQuickDistance")));
    auto* popup = findVisibleItem(_rootItem, QStringLiteral("caacWaypointPopupContent"));
    QVERIFY(popup);
    const auto popupGeometry = [&]() { return popup->property("popupGeometry").toRectF(); };
    QVERIFY(!_window->grabWindow().isNull());
    const auto initialPopupGeometry = popupGeometry();
    for (int toggle = 0; toggle < 2; ++toggle) {
        QVERIFY(clickButton(QStringLiteral("caacWaypointFineTuning")));
        QVERIFY(!_window->grabWindow().isNull());
        QCOMPARE(popupGeometry(), initialPopupGeometry);
    }
    const auto selected = mission->currentPlanViewItem()->sequenceNumber();
    auto* waypoint = editor->currentItem();
    QVERIFY(waypoint);
    const double initialDistance = editor->waypointLeg(waypoint).value("distance").toDouble();
    QVERIFY(clickButton(QStringLiteral("caacWaypointFineTuning")));
    _window->requestActivate();
    QTRY_VERIFY_WITH_TIMEOUT(_window->isActive(), TestTimeout::mediumMs());
    QTest::keyClick(_window, Qt::Key_Right);
    QTRY_VERIFY_WITH_TIMEOUT(
        std::abs(editor->waypointLeg(waypoint).value("distance").toDouble() - initialDistance - 0.01) < 0.001,
        TestTimeout::mediumMs());
    const auto initialBearing = editor->waypointLeg(waypoint).value("bearing").toDouble();
    QTest::keyClick(_window, Qt::Key_Up);
    QTRY_VERIFY_WITH_TIMEOUT(
        std::abs(std::remainder(editor->waypointLeg(waypoint).value("bearing").toDouble() - initialBearing - 0.01,
                                360)) < 0.001,
        TestTimeout::mediumMs());
    QTest::keyClick(_window, Qt::Key_Down);
    QVERIFY(std::abs(std::remainder(editor->waypointLeg(waypoint).value("bearing").toDouble() - initialBearing, 360)) <
            0.001);
    QTest::keyClick(_window, Qt::Key_Left);
    QVERIFY(std::abs(editor->waypointLeg(waypoint).value("distance").toDouble() - initialDistance) < 0.001);
    if (const auto screenshot = qgetenv("CAAC_POINT_UI_SCREENSHOT"); !screenshot.isEmpty()) {
        QVERIFY(_window->grabWindow().save(QString::fromUtf8(screenshot)));
    }
    auto* quickBearing = findVisibleItem(_rootItem, QStringLiteral("caacWaypointQuickBearing"));
    auto* quickDistance = findVisibleItem(_rootItem, QStringLiteral("caacWaypointQuickDistance"));
    QVERIFY(quickBearing && quickDistance);
    quickBearing->setProperty("text", QStringLiteral("90.44"));
    quickDistance->setProperty("text", QStringLiteral("360.65"));
    QVERIFY(clickButton(QStringLiteral("caacWaypointQuickConfirm")));
    QVERIFY(std::abs(editor->waypointLeg(waypoint).value("distance").toDouble() - 360.65) < 0.001);
    QVERIFY(std::abs(editor->waypointLeg(waypoint).value("bearing").toDouble() - 90.44) < 0.001);
    QVERIFY(!findVisibleItem(_rootItem, QStringLiteral("caacWaypointQuickBearing"), 0));
    map->setProperty("center", QVariant::fromValue(a));
    QSignalSpy hoverFrame(_window, &QQuickWindow::frameSwapped);
    _window->update();
    QVERIFY(hoverFrame.wait(TestTimeout::mediumMs()));
    QTest::mouseMove(_window, middle.toPoint());
    QTRY_VERIFY_WITH_TIMEOUT(!findVisibleItem(_rootItem, QStringLiteral("caacWaypointOverlapText"), 0),
                             TestTimeout::mediumMs());
    map->setProperty("center", QVariant::fromValue(waypoint->coordinate()));
    QTRY_VERIFY_WITH_TIMEOUT(QLineF(markerPosition(), middle).length() < 1, TestTimeout::mediumMs());
    QTest::mouseClick(_window, Qt::RightButton, Qt::NoModifier, markerPosition().toPoint());
    QVERIFY(findVisibleItem(_rootItem, QStringLiteral("caacContextDeleteWaypoint")));
    auto* gotoButton = findVisibleItem(_rootItem, QStringLiteral("caacContextGoto"));
    QVERIFY(gotoButton && !gotoButton->isEnabled());
    if (const auto screenshot = qgetenv("CAAC_POINT_MENU_SCREENSHOT"); !screenshot.isEmpty()) {
        QVERIFY(_window->grabWindow().save(QString::fromUtf8(screenshot)));
    }
    QVERIFY(clickButton(QStringLiteral("caacDrawingAidButton")));
    for (const auto* name : {"caacAuxiliaryCircleRadius", "caacAuxiliaryLineBearing"}) {
        auto* field = findVisibleItem(_rootItem, QLatin1String(name));
        QVERIFY(field);
        QCOMPARE(field->property("placeholderTextColor").value<QColor>(), field->property("color").value<QColor>());
    }
    QVERIFY(clickButton(QStringLiteral("caacAuxiliaryCircleRadius")));
    QTest::keyClick(_window, '5');
    QTest::keyClick(_window, '0');
    QTest::keyClick(_window, Qt::Key_Return);
    auto* circle = findVisibleItem(_rootItem, QStringLiteral("caacAuxiliaryCircle"));
    QVERIFY(circle);
    QCOMPARE(circle->property("radius").toDouble(), 50.0);
    QTest::mouseClick(_window, Qt::RightButton, Qt::NoModifier, markerPosition().toPoint());
    QVERIFY(clickButton(QStringLiteral("caacDrawingAidButton")));
    QVERIFY(clickButton(QStringLiteral("caacAuxiliaryLineBearing")));
    QTest::keyClick(_window, '9');
    QTest::keyClick(_window, '0');
    QTest::keyClick(_window, Qt::Key_Return);
    QVERIFY(findVisibleItem(_rootItem, QStringLiteral("caacAuxiliaryLine")));
    QTest::keyClick(_window, Qt::Key_Escape);
    QTest::keyClick(_window, Qt::Key_Escape);
    QTest::mouseClick(_window, Qt::RightButton, Qt::NoModifier, markerPosition().toPoint());
    QVERIFY(clickButton(QStringLiteral("caacPlaceCoordinateAxes")));
    auto* axes = findVisibleItem(_rootItem, QStringLiteral("caacCoordinateAxes"));
    auto* graphic = findVisibleItem(_rootItem, QStringLiteral("caacCoordinateAxesGraphic"));
    QVERIFY(axes && graphic);
    QVERIFY(graphic->width() < map->width() / 2);
    for (const auto& direction : {QStringLiteral("N"), QStringLiteral("S"), QStringLiteral("W"), QStringLiteral("E")}) {
        auto* label = findVisibleItem(_rootItem, QStringLiteral("caacAxesDirection_") + direction);
        QVERIFY(label);
        QCOMPARE(label->property("text").toString(), direction);
    }
    const auto axesOrigin = axes->property("coordinate").value<QGeoCoordinate>();
    const auto mapCenter = map->property("center").value<QGeoCoordinate>();
    const auto beforeAxesDrag = master->saveToJson();
    const auto armPosition = [&]() {
        return graphic->mapToScene(QPointF(graphic->width() * 0.75, graphic->height() / 2)).toPoint();
    };
    const auto arm = armPosition();
    QTest::mousePress(_window, Qt::LeftButton, Qt::NoModifier, arm);
    QTest::mouseMove(_window, arm + QPoint(60, 40));
    QTest::mouseRelease(_window, Qt::LeftButton, Qt::NoModifier, arm + QPoint(60, 40));
    QTRY_VERIFY_WITH_TIMEOUT(
        QGCGeo::geodesicDistance(axesOrigin, axes->property("coordinate").value<QGeoCoordinate>()) > 1,
        TestTimeout::mediumMs());
    QCOMPARE(master->saveToJson(), beforeAxesDrag);
    QVERIFY(QGCGeo::geodesicDistance(mapCenter, map->property("center").value<QGeoCoordinate>()) < 0.02);
    if (const auto screenshot = qgetenv("CAAC_AXES_UI_SCREENSHOT"); !screenshot.isEmpty()) {
        QVERIFY(_window->grabWindow().save(QString::fromUtf8(screenshot)));
    }
    QTest::mouseClick(_window, Qt::RightButton, Qt::NoModifier, armPosition());
    QVERIFY(clickButton(QStringLiteral("caacDeleteCoordinateAxes")));
    QTRY_VERIFY_WITH_TIMEOUT(!panel->property("hasCoordinateAxes").toBool(), TestTimeout::mediumMs());

    QVERIFY(clickButton(QStringLiteral("caacEditWaypoint_%1").arg(selected)));
    QVERIFY(waitForDialog(QCoreApplication::translate("CaacWaypointDialog", "Waypoint properties")));
    QVERIFY(rejectDialog());
    QVERIFY(findVisibleItem(_rootItem, QStringLiteral("caacRemoveWaypoint_%1").arg(selected)));

    const auto beforeBatchDelete = master->saveToJson();
    QVERIFY(!findVisibleItem(_rootItem, QStringLiteral("caacSelectWaypoint_0"), 0));
    QVERIFY(clickButton(QStringLiteral("caacSelectAllWaypoints")));
    QCOMPARE(panel->property("selectedWaypointCount").toInt(), 2);
    QVERIFY(clickButtonScrolled(QStringLiteral("caacSelectWaypoint_1"), QStringLiteral("caacWaypointList")));
    QCOMPARE(panel->property("selectedWaypointCount").toInt(), 1);
    QVERIFY(clickButton(QStringLiteral("caacDeleteSelectedWaypoints")));
    QCOMPARE(mission->visualItems()->count(), 2);
    QCOMPARE(mission->plannedHomePosition(), home);
    QVERIFY(clickButton(QStringLiteral("caacUndo")));
    QCOMPARE(master->saveToJson(), beforeBatchDelete);
    QCOMPARE(panel->property("selectedWaypointCount").toInt(), 0);
    QVERIFY(clickButton(QStringLiteral("caacSelectAllWaypoints")));
    QVERIFY(clickButton(QStringLiteral("caacDeleteSelectedWaypoints")));
    QCOMPARE(mission->visualItems()->count(), 1);
    QVERIFY(clickButton(QStringLiteral("caacUndo")));
    QCOMPARE(master->saveToJson(), beforeBatchDelete);
    QVERIFY(clickButton(QStringLiteral("caacRelativeEditorButton")));
    auto* relativeTarget = findVisibleItem(_rootItem, QStringLiteral("caacRelativeTarget"));
    auto* relativeReference = findVisibleItem(_rootItem, QStringLiteral("caacRelativeReference"));
    auto* relativeDistance = findVisibleItem(_rootItem, QStringLiteral("caacRelativeDistance"));
    QVERIFY(relativeTarget && relativeReference && relativeDistance);
    auto* relativeAngleType = findVisibleItem(_rootItem, QStringLiteral("caacRelativeAngleType"));
    auto* relativeAngle = findVisibleItem(_rootItem, QStringLiteral("caacRelativeAngle"));
    QVERIFY(relativeAngleType && relativeAngle);
    auto* relativeInsertAfter = findVisibleItem(_rootItem, QStringLiteral("caacRelativeInsertAfter"));
    QVERIFY(relativeInsertAfter);
    const auto beforeRelativeChain = master->saveToJson();
    const int beforeRelativeCount = mission->visualItems()->count();
    QVERIFY(relativeReference->setProperty("currentIndex", 0));
    QVERIFY(relativeInsertAfter->setProperty("currentIndex", 0));
    QVERIFY(relativeAngle->setProperty("text", QStringLiteral("270")));
    QVERIFY(relativeDistance->setProperty("text", QStringLiteral("90")));
    QVERIFY(clickButton(QStringLiteral("caacRelativeAdd")));
    QTRY_COMPARE_WITH_TIMEOUT(relativeReference->property("currentIndex").toInt(), 1, TestTimeout::mediumMs());
    QTRY_COMPARE_WITH_TIMEOUT(relativeInsertAfter->property("currentIndex").toInt(), 1, TestTimeout::mediumMs());
    QCOMPARE(mission->visualItems()->count(), beforeRelativeCount + 1);
    const auto firstRelative = mission->visualItems()->value<SimpleMissionItem*>(1)->coordinate();
    QVERIFY(std::abs(QGCGeo::geodesicDistance(home, firstRelative) - 90) < 0.01);
    const auto beforeInvalidRelative = master->saveToJson();
    QVERIFY(relativeDistance->setProperty("text", QStringLiteral("0")));
    QVERIFY(clickButton(QStringLiteral("caacRelativeAdd")));
    QCOMPARE(master->saveToJson(), beforeInvalidRelative);
    QCOMPARE(relativeReference->property("currentIndex").toInt(), 1);
    QCOMPARE(relativeInsertAfter->property("currentIndex").toInt(), 1);
    QVERIFY(relativeAngle->setProperty("text", QStringLiteral("0")));
    QVERIFY(relativeDistance->setProperty("text", QStringLiteral("30")));
    QVERIFY(clickButton(QStringLiteral("caacRelativeAdd")));
    QTRY_COMPARE_WITH_TIMEOUT(relativeReference->property("currentIndex").toInt(), 2, TestTimeout::mediumMs());
    QTRY_COMPARE_WITH_TIMEOUT(relativeInsertAfter->property("currentIndex").toInt(), 2, TestTimeout::mediumMs());
    QCOMPARE(mission->visualItems()->count(), beforeRelativeCount + 2);
    const auto secondRelative = mission->visualItems()->value<SimpleMissionItem*>(2)->coordinate();
    QVERIFY(std::abs(QGCGeo::geodesicDistance(firstRelative, secondRelative) - 30) < 0.01);
    QVERIFY(std::abs(std::remainder(QGCGeo::geodesicAzimuth(firstRelative, secondRelative), 360)) < 0.02);
    QVERIFY(clickButton(QStringLiteral("caacUndo")));
    QVERIFY(clickButton(QStringLiteral("caacUndo")));
    QCOMPARE(master->saveToJson(), beforeRelativeChain);
    const auto beforeEndpoint = master->saveToJson();
    for (int targetIndex : {1, 2}) {
        QVERIFY(relativeAngleType->setProperty("currentIndex", 1));
        QVERIFY(relativeTarget->setProperty("currentIndex", targetIndex));
        QVERIFY(QMetaObject::invokeMethod(relativeTarget, "activated", Q_ARG(int, targetIndex)));
        QVERIFY(relativeAngleType->isEnabled());
        QCOMPARE(relativeAngleType->property("currentIndex").toInt(), 1);
        QVERIFY(relativeReference->setProperty("currentIndex", 1));
        QVERIFY(QMetaObject::invokeMethod(relativeReference, "activated", Q_ARG(int, 1)));
        QVERIFY(relativeAngle->setProperty("text", QStringLiteral("90")));
        QVERIFY(relativeDistance->setProperty("text", QStringLiteral("25")));
        QVERIFY(clickButton(QStringLiteral("caacRelativeAdd")));
        QVERIFY(master->saveToJson() != beforeEndpoint);
        const auto endpoint =
            targetIndex == 1
                ? mission->plannedHomePosition()
                : mission->visualItems()->value<SimpleMissionItem*>(mission->visualItems()->count() - 1)->coordinate();
        const auto reference = mission->visualItems()->value<SimpleMissionItem*>(1)->coordinate();
        QVERIFY(std::abs(QGCGeo::geodesicDistance(reference, endpoint) - 25) < 0.01);
        QVERIFY(std::abs(std::remainder(QGCGeo::geodesicAzimuth(reference, endpoint), 360)) < 0.02);
        QVERIFY(clickButton(QStringLiteral("caacUndo")));
        QCOMPARE(master->saveToJson(), beforeEndpoint);
    }
    QVERIFY(clickButton(QStringLiteral("caacTemplateEditorButton")));
    auto* bearing =
        findVisibleItemScrolled(QStringLiteral("caacTemplateBearing"), QStringLiteral("caacTemplateScroller"));
    QVERIFY(bearing);
    const auto original = master->saveToJson();
    const auto enterHeading = [&](const QString& value) {
        bearing->forceActiveFocus();
        QTest::keyClick(_window, Qt::Key_A, Qt::ControlModifier);
        for (const auto character : value) {
            QTest::keyClick(_window, character.toLatin1());
        }
    };
    enterHeading(QStringLiteral("56"));
    QTRY_VERIFY_WITH_TIMEOUT(editor->rotationPreviewActive(), TestTimeout::mediumMs());
    const auto preview = master->saveToJson();
    QVERIFY(preview != original);
    enterHeading(QStringLiteral("90"));
    enterHeading(QStringLiteral("56"));
    QCOMPARE(master->saveToJson(), preview);
    QVERIFY(clickButtonScrolled(QStringLiteral("caacCancelOrientation"), QStringLiteral("caacTemplateScroller")));
    QCOMPARE(master->saveToJson(), original);
    bearing = findVisibleItemScrolled(QStringLiteral("caacTemplateBearing"), QStringLiteral("caacTemplateScroller"));
    QVERIFY(bearing);
    enterHeading(QStringLiteral("90"));
    QVERIFY(clickButtonScrolled(QStringLiteral("caacConfirmOrientation"), QStringLiteral("caacTemplateScroller")));
    QCOMPARE(editor->routeOrientation(), 90.0);
    QVERIFY(!editor->rotationPreviewActive());
    QVERIFY(clickButton(QStringLiteral("caacUndo")));
    QCOMPARE(master->saveToJson(), original);
    QVERIFY(clickButton(QStringLiteral("caacGlobalSettingsButton")));
    for (const auto* name : {"caacHomeSection", "caacBasicSection", "caacSimTurnSection", "caacSimClimbSection",
                             "caacSimLoopsSection", "caacSimWindSection", "caacMapEditingSection"}) {
        QVERIFY(findItem(_rootItem, QLatin1String(name)));
    }
    const auto beforeHomeCoordinates = master->saveToJson();
    for (const auto& entry :
         {std::pair<QString, QString>{QStringLiteral("caacHomeLongitude"), QStringLiteral("8.54525860115325")},
          {QStringLiteral("caacHomeLatitude"), QStringLiteral("47.3982382440215")}}) {
        auto* decimal = findVisibleItemScrolled(entry.first, QStringLiteral("caacGlobalSettingsScroller"));
        QVERIFY(decimal);
        QVERIFY(decimal->setProperty("text", entry.second));
        QVERIFY(clickButtonScrolled(entry.first + QStringLiteral("FormatToggle"),
                                    QStringLiteral("caacGlobalSettingsScroller")));
        auto* direction = findVisibleItem(_rootItem, entry.first + QStringLiteral("Hemisphere"));
        QVERIFY(direction);
        QVERIFY(direction->setProperty("currentIndex", 1));
        QVERIFY(clickButtonScrolled(entry.first + QStringLiteral("FormatToggle"),
                                    QStringLiteral("caacGlobalSettingsScroller")));
        decimal = findVisibleItem(_rootItem, entry.first);
        QVERIFY(decimal);
        QVERIFY(std::abs(decimal->property("text").toString().toDouble() + entry.second.toDouble()) < 1e-10);
        QVERIFY(clickButtonScrolled(entry.first + QStringLiteral("FormatToggle"),
                                    QStringLiteral("caacGlobalSettingsScroller")));
    }
    QVERIFY(clickButtonScrolled(QStringLiteral("caacApplyHome"), QStringLiteral("caacGlobalSettingsScroller")));
    QVERIFY(std::abs(mission->plannedHomePosition().longitude() + 8.54525860115325) < 1e-10);
    QVERIFY(std::abs(mission->plannedHomePosition().latitude() + 47.3982382440215) < 1e-10);
    QCOMPARE(mission->plannedHomePosition().altitude(), 20.0);
    const auto validDmsHome = master->saveToJson();
    auto* minutes = findVisibleItem(_rootItem, QStringLiteral("caacHomeLatitudeMinutes"));
    QVERIFY(minutes);
    QVERIFY(minutes->setProperty("text", QStringLiteral("60")));
    QVERIFY(clickButtonScrolled(QStringLiteral("caacApplyHome"), QStringLiteral("caacGlobalSettingsScroller")));
    QCOMPARE(master->saveToJson(), validDmsHome);
    QVERIFY(clickButton(QStringLiteral("caacUndo")));
    QCOMPARE(master->saveToJson(), beforeHomeCoordinates);
    QVERIFY(!findItem(_rootItem, QStringLiteral("caacSimFlightSection")));
    QVERIFY(!findItem(_rootItem, QStringLiteral("caacSimActionsSection")));
    QVERIFY(!findItem(_rootItem, QStringLiteral("caacOpenSimulation")));
    auto* horizontal =
        findVisibleItemScrolled(QStringLiteral("caacSimHorizontalSpeed"), QStringLiteral("caacGlobalSettingsScroller"));
    QVERIFY(horizontal);
    auto* basicSection = findItem(_rootItem, QStringLiteral("caacBasicSection"));
    auto* basicAncestor = horizontal->parentItem();
    while (basicAncestor && basicAncestor != basicSection) {
        basicAncestor = basicAncestor->parentItem();
    }
    QCOMPARE(basicAncestor, basicSection);
    auto* takeoffHeight =
        findVisibleItemScrolled(QStringLiteral("caacSimTakeoffHeight"), QStringLiteral("caacGlobalSettingsScroller"));
    QVERIFY(takeoffHeight);
    QVERIFY(takeoffHeight->setProperty("text", QStringLiteral("15")));
    horizontal->setProperty("text", QStringLiteral("7.5"));
    QVERIFY(clickButtonScrolled(QStringLiteral("caacSaveSimulationSettings"),
                                QStringLiteral("caacGlobalSettingsScroller")));
    QCOMPARE(editor->simulationSettings().value("horizontalSpeed").toDouble(), 7.5);
    QCOMPARE(editor->simulationSettings().value("takeoffHeight").toDouble(), 15.0);
    auto* windSection = findItem(_rootItem, QStringLiteral("caacSimWindSection"));
    auto* saveSettings = findVisibleItem(_rootItem, QStringLiteral("caacSaveSimulationSettings"));
    QVERIFY(windSection && saveSettings);
    auto* ancestor = saveSettings->parentItem();
    while (ancestor && ancestor != windSection) {
        ancestor = ancestor->parentItem();
    }
    QCOMPARE(ancestor, windSection);

    QVERIFY(
        clickButtonScrolled(QStringLiteral("caacGlobalShowDrawingAids"), QStringLiteral("caacGlobalSettingsScroller")));
    QVERIFY(!panel->property("showDrawingAids").toBool());
    QVERIFY(
        clickButtonScrolled(QStringLiteral("caacGlobalShowDrawingAids"), QStringLiteral("caacGlobalSettingsScroller")));
    QVERIFY(panel->property("showDrawingAids").toBool());
    if (const auto screenshot = qgetenv("CAAC_GLOBAL_UI_SCREENSHOT"); !screenshot.isEmpty()) {
        QVERIFY(scrollIntoView(horizontal, QStringLiteral("caacGlobalSettingsScroller")));
        QVERIFY(_window->grabWindow().save(QString::fromUtf8(screenshot)));
    }
    QVERIFY(clickToolSelectDropdownButton(QStringLiteral("toolbar_viewSettings")));
    QVERIFY(findVisibleItem(_rootItem, QStringLiteral("settingsButton_General")));
    for (const auto* name : {"settingsButton_Help", "settingsButton_PX4 Log Transfer", "settingsButton_Remote ID"}) {
        QVERIFY(!findItem(_rootItem, QLatin1String(name)));
    }
    QCOMPARE(SettingsManager::instance()->appSettings()->preferredVehicleClass()->label(), QStringLiteral("首选机型"));
    QCOMPARE(QCoreApplication::translate("Maps.SettingsUI.json", "Tokens"), QStringLiteral("访问令牌"));
}

UT_REGISTER_TEST(CaacPlanningInteractionUITest, TestLabel::Integration, TestLabel::MissionManager)

#include "CaacPlanningInteractionUITest.moc"
