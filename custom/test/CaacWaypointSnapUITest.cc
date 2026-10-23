#include <cmath>
#include <cstdio>

#include <QtCore/QLineF>
#include <QtCore/QScopeGuard>
#include <QtCore/QVariant>
#include <QtCore/QtMath>
#include <QtQuick/QQuickItem>
#include <QtQuick/QQuickWindow>
#include <QtTest/QTest>

#include "CaacCorePlugin.h"
#include "CaacRouteEditorController.h"
#include "Fact.h"
#include "MissionController.h"
#include "PlanMasterController.h"
#include "QGCGeo.h"
#include "QmlUITestBase.h"
#include "SimpleMissionItem.h"

class CaacWaypointSnapUITest : public QmlUITestBase
{
    Q_OBJECT

private slots:
    void _dragToGuideIntersection_data();
    void _dragToGuideIntersection();
};

void CaacWaypointSnapUITest::_dragToGuideIntersection_data()
{
    QTest::addColumn<bool>("enabled");
    QTest::newRow("capture-enabled") << true;
    QTest::newRow("capture-disabled") << false;
}

void CaacWaypointSnapUITest::_dragToGuideIntersection()
{
    QFETCH(bool, enabled);
    startUI();
    if (QTest::currentTestFailed()) {
        return;
    }
    auto* plugin = qobject_cast<CaacCorePlugin*>(QGCCorePlugin::instance());
    QVERIFY(plugin);
    const auto preferences = plugin->planningPreferences();
    const bool dragWaypoints = plugin->dragWaypoints();
    const auto restorePreferences = qScopeGuard([&]() {
        plugin->updatePlanningPreferences(preferences);
        plugin->setDragWaypoints(dragWaypoints);
    });
    plugin->setDragWaypoints(true);
    QVERIFY(plugin->updatePlanningPreferences({{"horizontalSnap", false},
                                               {"verticalSnap", true},
                                               {"angleSnap", true},
                                               {"snapAngle", 15.0},
                                               {"symmetrySnap", false},
                                               {"midpointSnap", false},
                                               {"intersectionSnap", enabled}}));
    QVERIFY(QMetaObject::invokeMethod(_window, "showPlanView"));
    auto* plan = findVisibleItem(_rootItem, QStringLiteral("mainView_plan"));
    auto* panel = findVisibleItem(_rootItem, QStringLiteral("caacRouteEditor"));
    auto* map = findVisibleItem(_rootItem, QStringLiteral("planView_map"));
    QVERIFY(plan && panel && map);
    auto* master = plan->property("_planMasterController").value<PlanMasterController*>();
    auto* editor = panel->property("routeController").value<CaacRouteEditorController*>();
    QVERIFY(master && editor);
    master->removeAll();
    auto* mission = master->missionController();
    QVERIFY(mission);
    const QGeoCoordinate origin(30, 120, 20);
    const auto coordinate = [&](double east, double north) {
        return QGCGeo::geodesicDestination(origin, qRadiansToDegrees(std::atan2(east, north)), std::hypot(east, north));
    };
    const double east = 100 * std::sin(qDegreesToRadians(15.0));
    const double north = 100 * std::cos(qDegreesToRadians(15.0));
    const auto target = coordinate(east, north);
    const auto initial = coordinate(east + 6, north);
    const auto raw = coordinate(east - 0.8, north - 0.1);
    mission->setHomePosition(coordinate(-100, -100));
    for (const auto& point : {origin, initial, coordinate(east, -60), coordinate(east - 0.79, -50),
                              coordinate(-80, north - 0.05), coordinate(80, north - 0.05)}) {
        QVERIFY(editor->addWaypoint(point));
    }
    QCOMPARE(mission->visualItems()->count(), 7);
    auto* moved = mission->visualItems()->value<SimpleMissionItem*>(2);
    QVERIFY(moved);
    moved->altitude()->setRawValue(42);
    panel->setProperty("activePanel", -1);
    QVERIFY(map->property("directMissionItemEditing").toBool());
    map->setProperty("center", QVariant::fromValue(target));
    map->setProperty("zoomLevel", 20.0);
    QTRY_VERIFY_WITH_TIMEOUT(map->property("mapReady").toBool(), TestTimeout::mediumMs());
    auto* guides = findItem(_rootItem, QStringLiteral("caacWaypointSnapGuides"));
    QVERIFY(guides && guides->property("guideCount").isValid());

    moved->setCoordinate(initial);
    auto* dragger = findVisibleItem(_rootItem, QStringLiteral("missionItemDrag_2"), TestTimeout::mediumMs());
    QVERIFY(dragger);
    auto* indicator = dragger->property("itemIndicator").value<QQuickItem*>();
    QVERIFY(indicator);
    const auto anchor = [&]() {
        return dragger->mapToScene(indicator->property("anchorPoint").toPointF() +
                                   QPointF(dragger->property("_touchMarginHorizontal").toDouble(),
                                           dragger->property("_touchMarginVertical").toDouble()));
    };
    QPointF initialPoint;
    QVERIFY(QMetaObject::invokeMethod(map, "fromCoordinate", Q_RETURN_ARG(QPointF, initialPoint),
                                      Q_ARG(QGeoCoordinate, initial), Q_ARG(bool, false)));
    QTRY_VERIFY_WITH_TIMEOUT(QLineF(anchor(), map->mapToScene(initialPoint)).length() < 1, TestTimeout::mediumMs());
    QPointF rawPoint;
    QVERIFY(QMetaObject::invokeMethod(map, "fromCoordinate", Q_RETURN_ARG(QPointF, rawPoint),
                                      Q_ARG(QGeoCoordinate, raw), Q_ARG(bool, false)));
    const QPoint from = anchor().toPoint();
    const QPoint to = map->mapToScene(rawPoint).toPoint();
    QVERIFY(_rootItem->boundingRect().contains(from));
    QVERIFY(_rootItem->boundingRect().contains(to));
    QVERIFY(QLineF(from, to).length() > 20);
    QTest::mousePress(_window, Qt::LeftButton, Qt::NoModifier, from);
    bool released = false;
    const auto releaseMouse = qScopeGuard([&]() {
        if (!released) {
            QTest::mouseRelease(_window, Qt::LeftButton, Qt::NoModifier, to);
        }
    });
    for (int step = 1; step <= 8; ++step) {
        QTest::mouseMove(_window, from + (to - from) * step / 8);
    }
    QTRY_VERIFY_WITH_TIMEOUT(_window->mouseGrabberItem() && dragger->isAncestorOf(_window->mouseGrabberItem()),
                             TestTimeout::mediumMs());
    QTRY_COMPARE_WITH_TIMEOUT(guides->property("guideCount").toInt(), enabled ? 2 : 1, TestTimeout::mediumMs());
    if (enabled) {
        QTRY_VERIFY_WITH_TIMEOUT(QGCGeo::geodesicDistance(moved->coordinate(), target) < 0.01, TestTimeout::mediumMs());
    } else {
        QVERIFY(QGCGeo::geodesicDistance(moved->coordinate(), target) > 0.1);
    }
    const auto beforeRelease = moved->coordinate();
    QTest::mouseRelease(_window, Qt::LeftButton, Qt::NoModifier, to);
    released = true;
    QTRY_COMPARE_WITH_TIMEOUT(guides->property("guideCount").toInt(), 0, TestTimeout::mediumMs());
    const double error = QGCGeo::geodesicDistance(moved->coordinate(), target);
    std::fprintf(stderr, "CAAC guide intersection %s: release offset=%.6f m, altitude=%.3f m\n",
                 enabled ? "enabled" : "disabled", error, moved->altitude()->rawValue().toDouble());
    QVERIFY(QGCGeo::geodesicDistance(moved->coordinate(), beforeRelease) < 0.001);
    QCOMPARE(moved->altitude()->rawValue().toDouble(), 42.0);
    if (enabled) {
        QVERIFY(error < 0.01);
    } else {
        QVERIFY(error > 0.1);
    }
}

UT_REGISTER_TEST(CaacWaypointSnapUITest, TestLabel::Integration, TestLabel::MissionManager)

#include "CaacWaypointSnapUITest.moc"
