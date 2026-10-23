#include <cmath>
#include <limits>

#include <QtCore/QScopeGuard>
#include <QtCore/QTemporaryDir>
#include <QtCore/QtMath>
#include <QtTest/QSignalSpy>

#include "CaacCorePlugin.h"
#include "CaacRouteEditorController.h"
#include "CaacRouteTemplates.h"
#include "CaacSimulationController.h"
#include "FlightPathSegment.h"
#include "MissionController.h"
#include "MissionItem.h"
#include "MissionSettingsItem.h"
#include "MissionTest.h"
#include "PlanMasterController.h"
#include "QGCGeo.h"
#include "SimpleMissionItem.h"
#include "SpeedSection.h"
#include "Vehicle.h"

class CaacPlanningToolsTest : public OfflineMissionTest
{
    Q_OBJECT
private slots:
    void _defaultPlannedHome();
    void _missingHomeAltitude_data();
    void _missingHomeAltitude();
    void _templateTransactions();
    void _translationPreservesMission();
    void _invalidInputsAreAtomic();
    void _coordinateFormats();
    void _advancedWaypointProperties();
    void _missionSpeedTransaction();
    void _legFlightSpeeds();
    void _scopedFlightSpeeds();
    void _scopedFlightSpeedCommandTypes_data();
    void _scopedFlightSpeedCommandTypes();
    void _orientationPreview();
    void _quickWaypointOperations();
    void _relativeEndpointEditing();
    void _relativeOverwrite();
    void _batchWaypointDeletion();
    void _centerHomeOnRoute();
    void _dragSnapping_data();
    void _dragSnapping();
    void _perpendicularAndSymmetry();
    void _edgeMidpoints();
    void _routeIntersections();
    void _guideIntersections();
    void _pointRelationsAndVertex();
    void _synchronizeWaypointDefaults();
    void _displayNumbersAndLoopSelection();
    void _planningPreferences();
    void _nearlyCoincidentSnapGuides();
};

void CaacPlanningToolsTest::_defaultPlannedHome()
{
    auto* plugin = qobject_cast<CaacCorePlugin*>(QGCCorePlugin::instance());
    QVERIFY(plugin);
    auto* master = planController();
    auto* mission = missionController();
    QVERIFY(!mission->homePositionSet());
    plugin->initializeDefaultPlan(nullptr);
    plugin->initializeDefaultPlan(master);
    const QGeoCoordinate expected(25.137904838950554, 102.72128132348416, 2062.0);
    QCOMPARE(mission->plannedHomePosition(), expected);
    QVERIFY(!mission->dirty());

    const QGeoCoordinate edited(30, 120, 100);
    mission->setHomePosition(edited);
    plugin->initializeDefaultPlan(master);
    QCOMPARE(mission->plannedHomePosition(), edited);

    CaacRouteEditorController editor;
    editor.setPlanMasterController(master);
    QVERIFY(editor.addWaypoint(edited.atDistanceAndAzimuth(100, 90)));
    QTemporaryDir directory;
    QVERIFY(directory.isValid());
    const auto path = directory.filePath(QStringLiteral("home.plan"));
    QVERIFY(master->saveToFile(path));
    const auto saved = master->saveToJson();
    master->removeAll();
    mission->setDirty(true);
    plugin->initializeDefaultPlan(master);
    QCOMPARE(mission->plannedHomePosition(), expected);
    QVERIFY(mission->dirty());
    master->loadFromFile(path);
    plugin->initializeDefaultPlan(master);
    QCOMPARE(master->saveToJson(), saved);

    master->removeAll();
    master->setFlyView(true);
    plugin->initializeDefaultPlan(master);
    QVERIFY(!mission->homePositionSet());
}

void CaacPlanningToolsTest::_missingHomeAltitude_data()
{
    QTest::addColumn<double>("distance");
    QTest::addColumn<double>("altitude");
    QTest::addColumn<double>("terrain");
    QTest::addColumn<bool>("manual");
    QTest::addColumn<double>("expected");
    QTest::newRow("default-field-zero") << 0.0 << 0.0 << qQNaN() << false << 2062.0;
    QTest::newRow("nearby-field-missing") << 30.0 << qQNaN() << qQNaN() << false << 2062.0;
    QTest::newRow("terrain-at-home") << 1000.0 << 0.0 << 1800.0 << false << 1800.0;
    QTest::newRow("unknown-remote-height") << 1000.0 << 0.0 << qQNaN() << false << 0.0;
    QTest::newRow("explicit-sea-level") << 0.0 << 0.0 << 1800.0 << true << 0.0;
    QTest::newRow("existing-positive-height") << 0.0 << 1900.0 << 1800.0 << false << 1900.0;
}

void CaacPlanningToolsTest::_missingHomeAltitude()
{
    QFETCH(double, distance);
    QFETCH(double, altitude);
    QFETCH(double, terrain);
    QFETCH(bool, manual);
    QFETCH(double, expected);
    auto* plugin = qobject_cast<CaacCorePlugin*>(QGCCorePlugin::instance());
    QVERIFY(plugin);
    auto* mission = missionController();
    auto coordinate = CaacCorePlugin::defaultHomePosition().atDistanceAndAzimuth(distance, 90);
    coordinate.setAltitude(altitude);
    mission->setHomePosition(coordinate);
    QVERIFY(mission->insertSimpleMissionItem(coordinate.atDistanceAndAzimuth(30, 0), 1, false));
    auto* home = mission->visualItems()->value<MissionSettingsItem*>(0);
    QVERIFY(home);
    home->setProperty("caacHomeAltitudeManual", manual);
    if (std::isfinite(terrain)) {
        const QList<double> heights{terrain};
        QVERIFY(QMetaObject::invokeMethod(home, "_terrainDataReceived", Qt::DirectConnection, Q_ARG(bool, true),
                                          Q_ARG(QList<double>, heights)));
    }
    plugin->initializeDefaultPlan(planController());
    QCOMPARE(mission->plannedHomePosition().altitude(), expected);
    QCOMPARE(home->plannedHomePositionAltitude()->rawValue().toDouble(), expected);
    QCOMPARE(mission->plannedHomePosition().latitude(), coordinate.latitude());
    QCOMPARE(mission->plannedHomePosition().longitude(), coordinate.longitude());
    QCOMPARE(home->property("caacHomeAltitudeManual").toBool(), manual);
}

void CaacPlanningToolsTest::_nearlyCoincidentSnapGuides()
{
    auto* plugin = qobject_cast<CaacCorePlugin*>(QGCCorePlugin::instance());
    QVERIFY(plugin);
    const auto preferences = plugin->planningPreferences();
    const auto restore = qScopeGuard([plugin, preferences]() { plugin->updatePlanningPreferences(preferences); });
    QVERIFY(plugin->updatePlanningPreferences({{"horizontalSnap", true},
                                               {"verticalSnap", false},
                                               {"angleSnap", true},
                                               {"symmetrySnap", false},
                                               {"midpointSnap", true},
                                               {"intersectionSnap", false}}));
    PlanMasterController master;
    master.start();
    CaacRouteEditorController editor;
    editor.setPlanMasterController(&master);
    const QGeoCoordinate home(0, 0, 20);
    master.missionController()->setHomePosition(home);
    const auto raw = QGCGeo::geodesicDestination(home, 90, 300);
    QVERIFY(editor.addWaypoint(raw));
    auto* item = master.missionController()->visualItems()->value<SimpleMissionItem*>(1);
    QVERIFY(item);
    for (const double offset : {0.25e-6, 0.75e-6, 1.25e-6}) {
        const auto shifted = QGCGeo::geodesicDestination(home, 0, offset);
        QVERIFY(editor.addWaypoint(QGCGeo::geodesicDestination(shifted, 90, 40)));
        QVERIFY(editor.addWaypoint(QGCGeo::geodesicDestination(shifted, 90, 160)));
    }
    const auto original = master.saveToJson();
    for (int i = 0; i < 100; ++i) {
        const auto result = editor.snapWaypointDrag(item, raw, 2);
        const auto snapped = result.value("coordinate").value<QGeoCoordinate>();
        QVERIFY(snapped.isValid());
        QVERIFY(QGCGeo::geodesicDistance(raw, snapped) < 0.1e-6);
        QVERIFY(!result.value("guides").toList().isEmpty());
    }
    QCOMPARE(master.saveToJson(), original);
}

void CaacPlanningToolsTest::_planningPreferences()
{
    auto* plugin = qobject_cast<CaacCorePlugin*>(QGCCorePlugin::instance());
    QVERIFY(plugin);
    const auto original = plugin->planningPreferences();
    const auto restore = qScopeGuard([&]() { plugin->updatePlanningPreferences(original); });
    const QVariantMap disabled{{"horizontalSnap", false}, {"verticalSnap", false}, {"angleSnap", false},
                               {"symmetrySnap", false},   {"midpointSnap", false}, {"intersectionSnap", false},
                               {"snapAngle", 30.0}};
    QSignalSpy changes(plugin, &CaacCorePlugin::mapPreferencesChanged);
    QVERIFY(plugin->updatePlanningPreferences(disabled));
    QCOMPARE(changes.count(), 1);
    QCOMPARE(plugin->planningPreferences(), disabled);
    const CaacCorePlugin reloaded;
    QCOMPARE(reloaded.planningPreferences(), disabled);
    QVERIFY(!plugin->updatePlanningPreferences({{"horizontalSnap", true}, {"snapAngle", 0.0}}));
    QVERIFY(!plugin->updatePlanningPreferences({{"unknownSetting", true}}));
    QVERIFY(!plugin->updatePlanningPreferences({{"angleSnap", QStringLiteral("false")}}));
    QCOMPARE(changes.count(), 1);
    QCOMPARE(plugin->planningPreferences(), disabled);
    PlanMasterController master;
    master.start();
    CaacRouteEditorController editor;
    editor.setPlanMasterController(&master);
    const QGeoCoordinate origin(30, 120, 50);
    master.missionController()->setHomePosition(QGCGeo::geodesicDestination(origin, 180, 300));
    QVERIFY(editor.addWaypoint(origin));
    QVERIFY(editor.addWaypoint(QGCGeo::geodesicDestination(origin, 45, 100)));
    auto* item = master.missionController()->visualItems()->value<SimpleMissionItem*>(2);
    const auto raw = QGCGeo::geodesicDestination(origin, 90.5, 100);
    const auto unsnapped = editor.snapWaypointDrag(item, raw, 2);
    QCOMPARE(unsnapped.value("coordinate").value<QGeoCoordinate>(), raw);
    QVERIFY(unsnapped.value("guides").toList().isEmpty());
    QVERIFY(plugin->updatePlanningPreferences({{"horizontalSnap", true}}));
    const auto snapped = editor.snapWaypointDrag(item, raw, 2);
    QVERIFY(!snapped.value("guides").toList().isEmpty());
    QVERIFY(std::abs(QGCGeo::geodesicAzimuth(origin, snapped.value("coordinate").value<QGeoCoordinate>()) - 90) < 1e-6);
}

void CaacPlanningToolsTest::_displayNumbersAndLoopSelection()
{
    PlanMasterController master;
    master.start();
    CaacRouteEditorController editor;
    editor.setPlanMasterController(&master);
    auto* mission = master.missionController();
    const QGeoCoordinate home(30, 120, 20);
    mission->setHomePosition(home);
    for (int i = 1; i <= 4; ++i) {
        QVERIFY(editor.addWaypoint(QGCGeo::geodesicDestination(home, i * 90, 100)));
    }
    auto settings = editor.simulationSettings();
    settings["startSequence"] = 3;
    settings["returnSequence"] = 4;
    settings["closed"] = true;
    settings["loopMode"] = 1;
    settings["loopCount"] = 4;
    QVERIFY(editor.setSimulationSettings(settings));
    const auto before = master.saveToJson();
    QVERIFY(editor.syncWaypointDefaults({{"specifySpeed", true}, {"speed", 8.0}}));
    for (int i = 1; i <= 4; ++i) {
        auto* point = mission->visualItems()->value<SimpleMissionItem*>(i);
        QCOMPARE(point->waypointNumber(), i);
        QCOMPARE(point->sequenceNumber(), 2 * i - 1);
    }
    QCOMPARE(editor.simulationSettings().value("startSequence").toInt(), 5);
    QCOMPARE(editor.simulationSettings().value("returnSequence").toInt(), 7);
    QVERIFY(editor.setMissionFlightSpeed(true, 8));
    QCOMPARE(mission->visualItems()->value<SimpleMissionItem*>(4)->waypointNumber(), 4);
    QCOMPARE(editor.simulationSettings().value("startSequence").toInt(), 6);
    QCOMPARE(editor.simulationSettings().value("returnSequence").toInt(), 8);
    QVERIFY(editor.undo());
    QVERIFY(editor.undo());
    QCOMPARE(master.saveToJson(), before);
    QCOMPARE(editor.simulationSettings().value("startSequence").toInt(), 3);
    QCOMPARE(editor.simulationSettings().value("returnSequence").toInt(), 4);
}

void CaacPlanningToolsTest::_synchronizeWaypointDefaults()
{
    PlanMasterController master;
    master.start();
    CaacRouteEditorController editor;
    editor.setPlanMasterController(&master);
    auto* mission = master.missionController();
    const QGeoCoordinate home(30, 120, 20);
    mission->setHomePosition(home);
    QVERIFY(editor.addWaypoint(QGCGeo::geodesicDestination(home, 90, 100)));
    QVERIFY(editor.addWaypoint(QGCGeo::geodesicDestination(home, 0, 100)));
    auto* first = mission->visualItems()->value<SimpleMissionItem*>(1);
    auto* second = mission->visualItems()->value<SimpleMissionItem*>(2);
    second->setAltitudeFrame(QGroundControlQmlGlobal::AltitudeFrameAbsolute);
    const auto firstFrame = first->altitudeFrame();
    const auto secondFrame = second->altitudeFrame();
    auto* takeoff = qobject_cast<SimpleMissionItem*>(mission->insertTakeoffItem(home, 1, false));
    QVERIFY(takeoff);
    const auto takeoffAltitude = takeoff->altitude()->rawValue();
    editor.clearHistory();
    const auto original = master.saveToJson();
    QVERIFY(editor.syncWaypointDefaults({{"altitude", 500.0}}));
    QCOMPARE(first->altitude()->rawValue().toDouble(), 500.0);
    QCOMPARE(second->altitude()->rawValue().toDouble(), 500.0);
    QCOMPARE(first->altitudeFrame(), firstFrame);
    QCOMPARE(second->altitudeFrame(), secondFrame);
    QCOMPARE(takeoff->altitude()->rawValue(), takeoffAltitude);
    QCOMPARE(mission->plannedHomePosition(), home);
    QVERIFY(editor.undo());
    QCOMPARE(master.saveToJson(), original);
    QVERIFY(editor.syncWaypointDefaults({{"specifySpeed", true}, {"speed", 8.0}, {"includeMissionSpeed", true}}));
    auto* missionSpeed = mission->visualItems()->value<MissionSettingsItem*>(0)->speedSection();
    QVERIFY(missionSpeed && missionSpeed->specifyFlightSpeed());
    QCOMPARE(missionSpeed->flightSpeed()->rawValue().toDouble(), 8.0);
    for (int i = 2; i <= 3; ++i) {
        auto* item = mission->visualItems()->value<SimpleMissionItem*>(i);
        auto* speed = qobject_cast<SpeedSection*>(item->speedSection());
        QVERIFY(speed && speed->specifyFlightSpeed());
        QCOMPARE(speed->flightSpeed()->rawValue().toDouble(), 8.0);
    }
    const auto synced = master.saveToJson();
    QVERIFY(!editor.syncWaypointDefaults({{"altitude", 400.0}, {"specifySpeed", true}, {"speed", 0.0}}));
    QCOMPARE(master.saveToJson(), synced);
    QVERIFY(!editor.syncWaypointDefaults({{"specifySpeed", true}, {"speed", 0.0}, {"includeMissionSpeed", true}}));
    QVERIFY(!editor.syncWaypointDefaults({{"includeMissionSpeed", true}}));
    QCOMPARE(master.saveToJson(), synced);
    QVERIFY(editor.undo());
    QCOMPARE(master.saveToJson(), original);
}

void CaacPlanningToolsTest::_dragSnapping_data()
{
    QTest::addColumn<QGeoCoordinate>("origin");
    QTest::addColumn<double>("bearing");
    for (const auto& origin :
         {QGeoCoordinate(30, 120, 20), QGeoCoordinate(30, 179.999, 20), QGeoCoordinate(80, -100, 20)}) {
        for (const double bearing : {0.0, 90.0, 180.0, 270.0}) {
            const auto tag = QStringLiteral("%1-%2").arg(origin.longitude()).arg(bearing).toLatin1();
            QTest::newRow(tag.constData()) << origin << bearing;
        }
    }
}

void CaacPlanningToolsTest::_dragSnapping()
{
    QFETCH(QGeoCoordinate, origin);
    QFETCH(double, bearing);
    PlanMasterController master;
    master.start();
    CaacRouteEditorController editor;
    editor.setPlanMasterController(&master);
    auto* mission = master.missionController();
    mission->setHomePosition(QGCGeo::geodesicDestination(origin, 180, 300));
    QVERIFY(editor.addWaypoint(origin));
    QVERIFY(editor.addWaypoint(QGCGeo::geodesicDestination(origin, 45, 100)));
    auto* item = mission->visualItems()->value<SimpleMissionItem*>(2);
    const auto original = master.saveToJson();
    auto raw = QGCGeo::geodesicDestination(origin, bearing + 0.5, 100);
    raw.setAltitude(42);
    const auto snap = editor.snapWaypointDrag(item, raw, 2);
    const auto point = snap.value("coordinate").value<QGeoCoordinate>();
    QVERIFY(!snap.value("guides").toList().isEmpty());
    QVERIFY(std::abs(std::remainder(QGCGeo::geodesicAzimuth(origin, point) - bearing, 360)) < 1e-6);
    QCOMPARE(point.altitude(), 42.0);
    QCOMPARE(master.saveToJson(), original);
    const auto free = QGCGeo::geodesicDestination(origin, 37, 100);
    const auto unsnapped = editor.snapWaypointDrag(item, free, 0.1);
    QCOMPARE(unsnapped.value("coordinate").value<QGeoCoordinate>(), free);
    QVERIFY(unsnapped.value("guides").toList().isEmpty());
    QCOMPARE(editor.snapWaypointDrag(nullptr, raw, 2).value("coordinate").value<QGeoCoordinate>(), raw);
}

void CaacPlanningToolsTest::_edgeMidpoints()
{
    auto* plugin = qobject_cast<CaacCorePlugin*>(QGCCorePlugin::instance());
    QVERIFY(plugin);
    const auto original = plugin->planningPreferences();
    const auto restore = qScopeGuard([&]() { plugin->updatePlanningPreferences(original); });
    QVERIFY(plugin->updatePlanningPreferences({{"horizontalSnap", false},
                                               {"verticalSnap", false},
                                               {"angleSnap", false},
                                               {"symmetrySnap", false},
                                               {"intersectionSnap", false},
                                               {"midpointSnap", true}}));
    for (const auto& origin : {QGeoCoordinate(30, 120, 20), QGeoCoordinate(80, 179.999, 20)}) {
        PlanMasterController master;
        master.start();
        CaacRouteEditorController editor;
        editor.setPlanMasterController(&master);
        auto* mission = master.missionController();
        mission->setHomePosition(origin);
        const auto a = QGCGeo::geodesicDestination(origin, 63, 100);
        const auto b = QGCGeo::geodesicDestination(a, 333, 80);
        const auto c = QGCGeo::geodesicDestination(origin, 333, 80);
        for (const auto& point : {a, b, c}) {
            QVERIFY(editor.addWaypoint(point));
        }
        auto settings = editor.simulationSettings();
        settings["closed"] = true;
        QVERIFY(editor.setSimulationSettings(settings));
        const auto before = master.saveToJson();
        auto* moved = mission->visualItems()->value<SimpleMissionItem*>(1);
        for (const auto& edge : {qMakePair(b, c), qMakePair(c, origin)}) {
            const auto midpoint =
                QGCGeo::geodesicDestination(edge.first, QGCGeo::geodesicAzimuth(edge.first, edge.second),
                                            QGCGeo::geodesicDistance(edge.first, edge.second) / 2);
            auto raw = QGCGeo::geodesicDestination(midpoint, 18, 0.5);
            raw.setAltitude(42);
            const auto result = editor.snapWaypointDrag(moved, raw, 1);
            const auto snapped = result.value("coordinate").value<QGeoCoordinate>();
            QVERIFY(QGCGeo::geodesicDistance(snapped, midpoint) < 0.001);
            QCOMPARE(snapped.altitude(), 42.0);
            QCOMPARE(result.value("guides").toList().first().toMap().value("kind").toInt(), 3);
            QCOMPARE(master.saveToJson(), before);
            QVERIFY(plugin->updatePlanningPreferences({{"midpointSnap", false}}));
            QCOMPARE(editor.snapWaypointDrag(moved, raw, 1).value("coordinate").value<QGeoCoordinate>(), raw);
            QVERIFY(plugin->updatePlanningPreferences({{"midpointSnap", true}}));
        }
    }
}

void CaacPlanningToolsTest::_perpendicularAndSymmetry()
{
    PlanMasterController master;
    master.start();
    CaacRouteEditorController editor;
    editor.setPlanMasterController(&master);
    auto* mission = master.missionController();
    const QGeoCoordinate origin(30, 120, 20);
    mission->setHomePosition(QGCGeo::geodesicDestination(origin, 213, 100));
    QVERIFY(editor.addWaypoint(origin));
    QVERIFY(editor.addWaypoint(QGCGeo::geodesicDestination(origin, 130, 100)));
    auto* item = mission->visualItems()->value<SimpleMissionItem*>(2);
    auto perpendicular = editor.snapWaypointDrag(item, QGCGeo::geodesicDestination(origin, 123.3, 100), 2);
    QVERIFY(!perpendicular.value("guides").toList().isEmpty());
    QCOMPARE(perpendicular.value("guides").toList().first().toMap().value("kind").toInt(), 1);
    const auto point = perpendicular.value("coordinate").value<QGeoCoordinate>();
    QVERIFY(std::abs(std::abs(std::remainder(QGCGeo::geodesicAzimuth(origin, point) -
                                                 QGCGeo::geodesicAzimuth(origin, mission->plannedHomePosition()),
                                             360)) -
                     90) < 1e-6);
    const auto next = QGCGeo::geodesicDestination(origin, 90, 100);
    QVERIFY(editor.addWaypoint(next));
    const auto raw = QGCGeo::geodesicDestination(QGCGeo::geodesicDestination(origin, 90, 49.9), 0, 70);
    const auto symmetric = editor.snapWaypointDrag(item, raw, 1);
    QCOMPARE(symmetric.value("guides").toList().first().toMap().value("kind").toInt(), 2);
    const auto symmetricPoint = symmetric.value("coordinate").value<QGeoCoordinate>();
    QVERIFY(std::abs(QGCGeo::geodesicDistance(origin, symmetricPoint) -
                     QGCGeo::geodesicDistance(next, symmetricPoint)) < 0.01);
}

void CaacPlanningToolsTest::_routeIntersections()
{
    auto* plugin = qobject_cast<CaacCorePlugin*>(QGCCorePlugin::instance());
    QVERIFY(plugin);
    const auto original = plugin->planningPreferences();
    const auto restore = qScopeGuard([&]() { plugin->updatePlanningPreferences(original); });
    QVERIFY(plugin->updatePlanningPreferences({{"horizontalSnap", false},
                                               {"verticalSnap", false},
                                               {"angleSnap", false},
                                               {"symmetrySnap", false},
                                               {"midpointSnap", false},
                                               {"intersectionSnap", true}}));
    for (const auto& origin : {QGeoCoordinate(30, 120, 20), QGeoCoordinate(80, 179.999, 20)}) {
        PlanMasterController master;
        master.start();
        CaacRouteEditorController editor;
        editor.setPlanMasterController(&master);
        auto* mission = master.missionController();
        mission->setHomePosition(origin);
        const auto a = QGCGeo::geodesicDestination(origin, 90, 100);
        const auto b = QGCGeo::geodesicDestination(a, 0, 100);
        const auto c = QGCGeo::geodesicDestination(origin, 0, 100);
        const auto d = QGCGeo::geodesicDestination(origin, 270, 100);
        for (const auto& point : {a, b, c, d}) {
            QVERIFY(editor.addWaypoint(point));
        }
        auto* moved = mission->visualItems()->value<SimpleMissionItem*>(4);
        const auto before = master.saveToJson();
        const auto target = QGCGeo::geodesicDestination(origin, 90, 30);
        auto raw = QGCGeo::geodesicDestination(target, 0, 0.5);
        raw.setAltitude(42);
        const auto result = editor.snapWaypointDrag(moved, raw, 1);
        const auto snapped = result.value("coordinate").value<QGeoCoordinate>();
        QVERIFY(QGCGeo::geodesicDistance(snapped, target) < 0.01);
        QCOMPARE(snapped.altitude(), 42.0);
        QCOMPARE(result.value("guides").toList().first().toMap().value("kind").toInt(), 4);
        QCOMPARE(master.saveToJson(), before);
        QVERIFY(plugin->updatePlanningPreferences({{"intersectionSnap", false}}));
        QCOMPARE(editor.snapWaypointDrag(moved, raw, 1).value("coordinate").value<QGeoCoordinate>(), raw);
        QVERIFY(plugin->updatePlanningPreferences({{"intersectionSnap", true}}));
        const auto outside = QGCGeo::geodesicDestination(origin, 90, 120);
        QCOMPARE(editor.snapWaypointDrag(moved, outside, 1).value("coordinate").value<QGeoCoordinate>(), outside);

        // The closing segment is eligible only when it actually forms part of the route.
        moved = mission->visualItems()->value<SimpleMissionItem*>(2);
        const auto closingTarget = QGCGeo::geodesicDestination(origin, 270, 30);
        raw = QGCGeo::geodesicDestination(closingTarget, 0, 0.5);
        QCOMPARE(editor.snapWaypointDrag(moved, raw, 1).value("coordinate").value<QGeoCoordinate>(), raw);
        auto settings = editor.simulationSettings();
        settings["closed"] = true;
        QVERIFY(editor.setSimulationSettings(settings));
        const auto closed = editor.snapWaypointDrag(moved, raw, 1);
        QVERIFY(QGCGeo::geodesicDistance(closed.value("coordinate").value<QGeoCoordinate>(), closingTarget) < 0.01);
        QCOMPARE(closed.value("guides").toList().first().toMap().value("kind").toInt(), 4);
    }
}

void CaacPlanningToolsTest::_guideIntersections()
{
    auto* plugin = qobject_cast<CaacCorePlugin*>(QGCCorePlugin::instance());
    QVERIFY(plugin);
    const auto preferences = plugin->planningPreferences();
    const auto restore = qScopeGuard([&]() { plugin->updatePlanningPreferences(preferences); });
    QVERIFY(plugin->updatePlanningPreferences({{"horizontalSnap", false},
                                               {"verticalSnap", true},
                                               {"angleSnap", true},
                                               {"snapAngle", 15.0},
                                               {"symmetrySnap", false},
                                               {"midpointSnap", false},
                                               {"intersectionSnap", true}}));
    for (const auto& origin : {QGeoCoordinate(30, 120, 20), QGeoCoordinate(80, 179.999, 20)}) {
        const auto coordinate = [&](double east, double north) {
            return QGCGeo::geodesicDestination(origin, qRadiansToDegrees(std::atan2(east, north)),
                                               std::hypot(east, north));
        };
        const double east = 100 * std::sin(qDegreesToRadians(15.0));
        const double north = 100 * std::cos(qDegreesToRadians(15.0));
        const auto target = coordinate(east, north);
        auto raw = coordinate(east - 0.8, north - 0.1);
        raw.setAltitude(42);
        PlanMasterController master;
        master.start();
        CaacRouteEditorController editor;
        editor.setPlanMasterController(&master);
        auto* mission = master.missionController();
        mission->setHomePosition(coordinate(-100, -100));
        // A closer parallel guide and a nearby route edge must not mask the 15-degree intersection.
        for (const auto& point : {origin, coordinate(100, 100), coordinate(east, -60), coordinate(east - 0.79, -50),
                                  coordinate(-80, north - 0.05), coordinate(80, north - 0.05)}) {
            QVERIFY(editor.addWaypoint(point));
        }
        auto* moved = mission->visualItems()->value<SimpleMissionItem*>(2);
        const auto before = master.saveToJson();
        const auto result = editor.snapWaypointDrag(moved, raw, 1);
        const auto snapped = result.value("coordinate").value<QGeoCoordinate>();
        QVERIFY2(
            QGCGeo::geodesicDistance(snapped, target) < 0.001,
            qPrintable(QStringLiteral("Intersection offset: %1 m").arg(QGCGeo::geodesicDistance(snapped, target))));
        QCOMPARE(snapped.altitude(), 42.0);
        const auto guides = result.value("guides").toList();
        QCOMPARE(guides.size(), 2);
        QCOMPARE(guides.first().toMap().value("intersection").value<QGeoCoordinate>(), snapped);
        QCOMPARE(master.saveToJson(), before);
        QVERIFY(plugin->updatePlanningPreferences({{"intersectionSnap", false}}));
        const auto disabled = editor.snapWaypointDrag(moved, raw, 1);
        QVERIFY(QGCGeo::geodesicDistance(disabled.value("coordinate").value<QGeoCoordinate>(), target) > 0.1);
        QCOMPARE(disabled.value("guides").toList().size(), 1);
        QVERIFY(!disabled.value("guides").toList().first().toMap().contains("intersection"));
        QVERIFY(plugin->updatePlanningPreferences({{"intersectionSnap", true}}));
    }
}

void CaacPlanningToolsTest::_pointRelationsAndVertex()
{
    PlanMasterController master;
    master.start();
    CaacRouteEditorController editor;
    editor.setPlanMasterController(&master);
    auto* mission = master.missionController();
    const QGeoCoordinate home(30, 120, 20);
    mission->setHomePosition(home);
    const auto a = QGCGeo::geodesicDestination(home, 0, 200);
    const auto b = QGCGeo::geodesicDestination(a, 90, 100);
    const auto c = QGCGeo::geodesicDestination(b, 45, 100);
    for (const auto& point : {a, b, c}) {
        QVERIFY(editor.addWaypoint(point));
    }
    editor.clearHistory();
    const auto original = master.saveToJson();
    const auto fixedFirstPoint = mission->visualItems()->value<SimpleMissionItem*>(1)->coordinate();
    QVERIFY(editor.applyWaypointRelation(0, 1, 2, false, 270, 20, false));
    QVERIFY(QGCGeo::geodesicDistance(mission->plannedHomePosition(), QGCGeo::geodesicDestination(a, 270, 20)) < 0.001);
    QCOMPARE(mission->plannedHomePosition().altitude(), 20.0);
    QCOMPARE(mission->visualItems()->value<SimpleMissionItem*>(1)->coordinate(), fixedFirstPoint);
    QVERIFY(editor.undo());
    QCOMPARE(master.saveToJson(), original);
    QVERIFY(editor.applyWaypointRelation(0, 1, 2, true, 0, 10, true));
    const auto extended = QGCGeo::geodesicDestination(b, QGCGeo::geodesicAzimuth(b, a) + 180, 10);
    QVERIFY(QGCGeo::geodesicDistance(mission->plannedHomePosition(), extended) < 0.001);
    QCOMPARE(mission->plannedHomePosition().altitude(), 20.0);
    QVERIFY(QGCGeo::geodesicDistance(mission->visualItems()->value<SimpleMissionItem*>(1)->coordinate(), a) > 1);
    QVERIFY(editor.undo());
    QCOMPARE(master.saveToJson(), original);
    QVERIFY(!editor.applyWaypointRelation(0, 1, 1, true, 0, 10, false));
    QCOMPARE(master.saveToJson(), original);
    auto* item = mission->visualItems()->value<SimpleMissionItem*>(2);
    const auto vertex = editor.waypointAngles(false).last().toMap();
    QVERIFY(std::abs(vertex.value("angle").toDouble() - 135) < 0.01);
    QVariantMap properties{{"vertexAngle", 90.0},  {"altitude", 40.0},   {"specifySpeed", false},
                           {"holdSeconds", 1.0},   {"specifyYaw", true}, {"yaw", 315.0},
                           {"verticalSpeed", 3.0}, {"climbMode", 0},     {"turnMode", 0}};
    QVERIFY(editor.updateWaypointProperties(item, properties));
    QVERIFY(std::abs(editor.waypointAngles(false).last().toMap().value("angle").toDouble() - 90) < 1e-6);
    QVERIFY(std::abs(std::remainder(QGCGeo::geodesicAzimuth(a, item->coordinate()) - 90, 360)) < 1e-6);
    QCOMPARE(item->altitude()->rawValue().toDouble(), 40.0);
    QCOMPARE(item->missionItem().param1(), 1.0);
    QCOMPARE(item->missionItem().param4(), -45.0);
    QCOMPARE(item->caacFlightSettings().value("verticalSpeed").toDouble(), 3.0);
    QJsonArray saved;
    item->save(saved);
    SimpleMissionItem restored(&master, false, true);
    QString error;
    QVERIFY2(restored.load(saved.first().toObject(), item->sequenceNumber(), error), qPrintable(error));
    QCOMPARE(restored.caacFlightSettings(), item->caacFlightSettings());
    const auto edited = master.saveToJson();
    properties["verticalSpeed"] = 0;
    QVERIFY(!editor.updateWaypointProperties(item, properties));
    QCOMPARE(master.saveToJson(), edited);
    QVERIFY(editor.undo());
    QCOMPARE(master.saveToJson(), original);
    QVERIFY(editor.redo());
    QCOMPARE(master.saveToJson(), edited);
    const auto fixedHome = mission->plannedHomePosition();
    QVERIFY(!editor.alignRouteAround(a, 1, 0, 90, false));
    QCOMPARE(master.saveToJson(), edited);
    QVERIFY(editor.rotateRouteAround(a, 45, false));
    QCOMPARE(mission->plannedHomePosition(), fixedHome);
    QVERIFY(editor.undo());
    QCOMPARE(master.saveToJson(), edited);
    QVERIFY(editor.translateRoute(a, QGCGeo::geodesicDestination(a, 180, 10), false));
    QCOMPARE(mission->plannedHomePosition(), fixedHome);
    QVERIFY(editor.undo());
    QCOMPARE(master.saveToJson(), edited);
}

void CaacPlanningToolsTest::_centerHomeOnRoute()
{
    for (const double longitude : {120.0, 179.999}) {
        PlanMasterController master;
        master.start();
        CaacRouteEditorController editor;
        editor.setPlanMasterController(&master);
        auto* mission = master.missionController();
        const QGeoCoordinate home(30, longitude, 20);
        mission->setHomePosition(home);
        const auto empty = master.saveToJson();
        QVERIFY(!editor.centerHomeOnRoute());
        QCOMPARE(master.saveToJson(), empty);
        const auto center = QGCGeo::geodesicDestination(home, 90, 500);
        QList<QGeoCoordinate> points;
        for (double bearing : {45.0, 135.0, 225.0, 315.0}) {
            points.append(QGCGeo::geodesicDestination(center, bearing, 100));
            QVERIFY(editor.addWaypoint(points.last()));
        }
        auto* takeoff = qobject_cast<SimpleMissionItem*>(mission->insertTakeoffItem(home, 1, false));
        QVERIFY(takeoff);
        takeoff->setCoordinate(home);
        const auto takeoffAltitude = takeoff->altitude()->rawValue();
        auto* jump = qobject_cast<SimpleMissionItem*>(
            mission->insertSimpleMissionItem(home, mission->visualItems()->count(), false));
        QVERIFY(jump);
        jump->setCommand(MAV_CMD_DO_JUMP);
        jump->missionItem().setParam1(2);
        const auto jumpCommand = jump->missionItem().command();
        editor.clearHistory();
        const auto original = master.saveToJson();
        QVERIFY(editor.centerHomeOnRoute());
        QVERIFY(QGCGeo::geodesicDistance(mission->plannedHomePosition(), center) < 0.01);
        QCOMPARE(mission->plannedHomePosition().altitude(), 20.0);
        QCOMPARE(takeoff->altitude()->rawValue(), takeoffAltitude);
        QCOMPARE(jump->missionItem().command(), jumpCommand);
        QCOMPARE(jump->missionItem().param1(), 2.0);
        QVERIFY(QGCGeo::geodesicDistance(takeoff->coordinate(), center) < 0.01);
        for (int i = 0; i < points.count(); ++i) {
            QVERIFY(QGCGeo::geodesicDistance(mission->visualItems()->value<SimpleMissionItem*>(i + 2)->coordinate(),
                                             points[i]) < 0.001);
        }
        const auto centered = master.saveToJson();
        QVERIFY(editor.undo());
        QCOMPARE(master.saveToJson(), original);
        QVERIFY(editor.redo());
        QCOMPARE(master.saveToJson(), centered);
    }
}

void CaacPlanningToolsTest::_batchWaypointDeletion()
{
    PlanMasterController master;
    master.start();
    CaacRouteEditorController editor;
    editor.setPlanMasterController(&master);
    auto* mission = master.missionController();
    const QGeoCoordinate home(30, 120, 20);
    mission->setHomePosition(home);
    QList<QGeoCoordinate> coordinates;
    for (double bearing : {0.0, 90.0, 180.0, 270.0}) {
        coordinates.append(QGCGeo::geodesicDestination(home, bearing, 100));
        QVERIFY(editor.addWaypoint(coordinates.last()));
    }
    auto* takeoff = qobject_cast<SimpleMissionItem*>(mission->insertTakeoffItem(home, 1, false));
    QVERIFY(takeoff);
    takeoff->setCoordinate(home);
    auto* landing = qobject_cast<SimpleMissionItem*>(
        mission->insertSimpleMissionItem(home, mission->visualItems()->count(), false));
    QVERIFY(landing);
    landing->setCommand(MAV_CMD_NAV_LAND);
    editor.clearHistory();
    const auto original = master.saveToJson();
    const auto objectAt = [&](int index) { return QVariant::fromValue(mission->visualItems()->get(index)); };
    QVERIFY(!editor.removeWaypoints({objectAt(2), objectAt(0)}));
    QVERIFY(!editor.removeWaypoints({objectAt(2), objectAt(2)}));
    QVERIFY(!editor.removeWaypoints({objectAt(2), objectAt(1)}));
    QCOMPARE(master.saveToJson(), original);
    QVERIFY(editor.removeWaypoints({objectAt(2), objectAt(4)}));
    QCOMPARE(mission->visualItems()->count(), 5);
    QVERIFY(QGCGeo::geodesicDistance(mission->visualItems()->value<SimpleMissionItem*>(2)->coordinate(),
                                     coordinates.at(1)) < 0.001);
    QVERIFY(QGCGeo::geodesicDistance(mission->visualItems()->value<SimpleMissionItem*>(3)->coordinate(),
                                     coordinates.at(3)) < 0.001);
    const auto afterPartial = master.saveToJson();
    QVERIFY(editor.undo());
    QCOMPARE(master.saveToJson(), original);
    QVERIFY(editor.redo());
    QCOMPARE(master.saveToJson(), afterPartial);
    QVERIFY(editor.undo());
    QVariantList all;
    for (int i = 1; i < mission->visualItems()->count(); ++i) {
        auto* item = mission->visualItems()->value<SimpleMissionItem*>(i);
        if (item && item->command() == MAV_CMD_NAV_WAYPOINT) {
            all.append(objectAt(i));
        }
    }
    QVERIFY(editor.removeWaypoints(all));
    QCOMPARE(mission->visualItems()->count(), 3);
    QCOMPARE(mission->visualItems()->value<SimpleMissionItem*>(1)->command(), MAV_CMD_NAV_TAKEOFF);
    QCOMPARE(mission->visualItems()->value<SimpleMissionItem*>(2)->command(), MAV_CMD_NAV_LAND);
    QCOMPARE(mission->plannedHomePosition(), home);
    QVERIFY(editor.undo());
    QCOMPARE(master.saveToJson(), original);
}

void CaacPlanningToolsTest::_relativeOverwrite()
{
    auto* mission = missionController();
    const QGeoCoordinate home(30, 120, 20);
    mission->setHomePosition(home);
    CaacRouteEditorController editor;
    editor.setPlanMasterController(planController());
    QVERIFY(editor.addWaypoint(home.atDistanceAndAzimuth(30, 0)));
    QVERIFY(editor.addWaypoint(home.atDistanceAndAzimuth(60, 0)));
    auto* point = mission->visualItems()->value<SimpleMissionItem*>(2);
    QVERIFY(point);
    point->setAltitudeFrame(QGroundControlQmlGlobal::AltitudeFrameAbsolute);
    point->altitude()->setRawValue(123);
    point->missionItem().setParam1(7);
    point->missionItem().setParam4(45);
    editor.clearHistory();
    const auto before = planController()->saveToJson();
    QVERIFY(editor.insertRelativeWaypoint(0, 2, 90, 50, false, true));
    QCOMPARE(mission->visualItems()->count(), 3);
    QCOMPARE(mission->visualItems()->get(2), point);
    QCOMPARE(point->altitude()->rawValue().toDouble(), 123.0);
    QCOMPARE(point->altitudeFrame(), QGroundControlQmlGlobal::AltitudeFrameAbsolute);
    QCOMPARE(point->missionItem().param1(), 7.0);
    QCOMPARE(point->missionItem().param4(), 45.0);
    QVERIFY(std::abs(QGCGeo::geodesicDistance(home, point->coordinate()) - 50) < 0.01);
    QVERIFY(std::abs(QGCGeo::geodesicAzimuth(home, point->coordinate()) - 90) < 0.01);
    const auto after = planController()->saveToJson();
    QVERIFY(editor.undo());
    QCOMPARE(planController()->saveToJson(), before);
    QVERIFY(editor.redo());
    QCOMPARE(planController()->saveToJson(), after);
    QVERIFY(!editor.insertRelativeWaypoint(0, 0, 90, 50, false, true));
    QVERIFY(!editor.insertRelativeWaypoint(0, 2, 90, 0, false, true));
    QCOMPARE(planController()->saveToJson(), after);
    QVERIFY(editor.insertRelativeWaypoint(1, 2, 180, 25, true, true));
    QCOMPARE(mission->visualItems()->count(), 3);
}

void CaacPlanningToolsTest::_relativeEndpointEditing()
{
    PlanMasterController master;
    master.start();
    CaacRouteEditorController editor;
    editor.setPlanMasterController(&master);
    auto* mission = master.missionController();
    const QGeoCoordinate home(30, 120, 20);
    mission->setHomePosition(home);
    const auto waypoint = QGCGeo::geodesicDestination(home, 90, 100);
    QVERIFY(editor.addWaypoint(waypoint));
    editor.clearHistory();
    const auto original = master.saveToJson();
    QVERIFY(editor.setRelativeEndpoint(1, 180, 50, false));
    auto* reference = mission->visualItems()->value<SimpleMissionItem*>(1);
    QVERIFY(reference);
    QVERIFY(QGCGeo::geodesicDistance(reference->coordinate(), waypoint) < 0.001);
    QVERIFY(std::abs(QGCGeo::geodesicDistance(waypoint, mission->plannedHomePosition()) - 50) < 0.01);
    QVERIFY(std::abs(QGCGeo::geodesicAzimuth(waypoint, mission->plannedHomePosition()) - 180) < 0.01);
    QCOMPARE(mission->plannedHomePosition().altitude(), 20.0);
    const auto firstHome = master.saveToJson();
    QVERIFY(editor.setRelativeEndpoint(1, 90, 25, false));
    QCOMPARE(mission->plannedHomePosition().altitude(), 20.0);
    QVERIFY(editor.undo());
    QCOMPARE(master.saveToJson(), firstHome);
    QVERIFY(editor.undo());
    QCOMPARE(master.saveToJson(), original);
    QVERIFY(editor.setRelativeEndpoint(1, 90, 50, true));
    QCOMPARE(mission->visualItems()->count(), 3);
    auto* landing = mission->visualItems()->value<SimpleMissionItem*>(2);
    QVERIFY(landing);
    QCOMPARE(landing->command(), MAV_CMD_NAV_LAND);
    QCOMPARE(landing->altitude()->rawValue().toDouble(), 0.0);
    QVERIFY(std::abs(QGCGeo::geodesicDistance(waypoint, landing->coordinate()) - 50) < 0.01);
    const auto firstLanding = master.saveToJson();
    QVERIFY(editor.setRelativeEndpoint(1, 180, 25, true));
    QCOMPARE(mission->visualItems()->count(), 3);
    QVERIFY(editor.undo());
    QCOMPARE(master.saveToJson(), firstLanding);
    const auto unchanged = master.saveToJson();
    QVERIFY(!editor.setRelativeEndpoint(1, qQNaN(), 25, true));
    QVERIFY(!editor.setRelativeEndpoint(100, 90, 25, false));
    QCOMPARE(master.saveToJson(), unchanged);
    QVERIFY(editor.undo());
    QCOMPARE(master.saveToJson(), original);
    for (bool isLanding : {false, true}) {
        for (double angle : {90.0, -90.0, 180.0}) {
            QVERIFY(editor.setRelativeEndpoint(1, angle, 50, isLanding, true));
            const auto endpoint = isLanding ? mission->visualItems()->value<SimpleMissionItem*>(2)->coordinate()
                                            : mission->plannedHomePosition();
            const double expectedBearing = angle == 90 ? 0 : angle == -90 ? 180 : 90;
            QVERIFY(std::abs(QGCGeo::geodesicDistance(waypoint, endpoint) - 50) < 0.01);
            QVERIFY(std::abs(std::remainder(QGCGeo::geodesicAzimuth(waypoint, endpoint) - expectedBearing, 360)) <
                    0.01);
            QCOMPARE(mission->plannedHomePosition().altitude(), 20.0);
            QVERIFY(editor.undo());
            QCOMPARE(master.saveToJson(), original);
        }
        QVERIFY(editor.setRelativeEndpoint(1, 90, 0, isLanding, true));
        const auto endpoint = isLanding ? mission->visualItems()->value<SimpleMissionItem*>(2)->coordinate()
                                        : mission->plannedHomePosition();
        QVERIFY(QGCGeo::geodesicDistance(waypoint, endpoint) < 0.001);
        QVERIFY(editor.undo());
        QCOMPARE(master.saveToJson(), original);
        QVERIFY(!editor.setRelativeEndpoint(0, 90, 25, isLanding, true));
        QVERIFY(!editor.setRelativeEndpoint(1, qQNaN(), 25, isLanding, true));
        QVERIFY(!editor.setRelativeEndpoint(1, 90, -1, isLanding, true));
        QCOMPARE(master.saveToJson(), original);
    }
    auto* takeoff = qobject_cast<SimpleMissionItem*>(mission->insertTakeoffItem(home, 1, false));
    QVERIFY(takeoff);
    takeoff->setCoordinate(home);
    takeoff->altitude()->setRawValue(5.0);
    const auto beforeTakeoffEdit = master.saveToJson();
    editor.clearHistory();
    QVERIFY2(editor.setRelativeEndpoint(2, 90, 25, false, true), qPrintable(editor.lastError()));
    QVERIFY(QGCGeo::geodesicDistance(takeoff->coordinate(), mission->plannedHomePosition()) < 0.001);
    QCOMPARE(mission->plannedHomePosition().altitude(), 20.0);
    QCOMPARE(takeoff->altitude()->rawValue().toDouble(), 5.0);
    const auto afterTakeoffEdit = master.saveToJson();
    QVERIFY(editor.undo());
    QCOMPARE(master.saveToJson(), beforeTakeoffEdit);
    QVERIFY(editor.redo());
    QCOMPARE(master.saveToJson(), afterTakeoffEdit);
}

class CaacRouteTemplateGeometryTest : public UnitTest
{
    Q_OBJECT
private slots:
    void _templateGeometry_data();
    void _templateGeometry();
};

void CaacRouteTemplateGeometryTest::_templateGeometry_data()
{
    QTest::addColumn<int>("shape");
    QTest::addColumn<int>("start");
    QTest::addColumn<bool>("variant");
    QTest::addColumn<bool>("clockwise");
    QTest::addColumn<QGeoCoordinate>("anchor");
    for (int shape = 0; shape < 4; ++shape) {
        const int starts = shape == 1 ? 6 : shape == 2 ? 4 : 1;
        for (int start = 0; start < starts; ++start) {
            for (bool variant : {false, true}) {
                if (variant && shape != 1) {
                    continue;
                }
                for (bool clockwise : {false, true}) {
                    if (!clockwise && shape == 2) {
                        continue;
                    }
                    for (const auto& anchor : {QGeoCoordinate(30, 120), QGeoCoordinate(80, 179.999)}) {
                        const auto name = QStringLiteral("%1/%2/%3/%4/%5")
                                              .arg(shape)
                                              .arg(start)
                                              .arg(variant)
                                              .arg(clockwise)
                                              .arg(anchor.longitude())
                                              .toUtf8();
                        QTest::newRow(name.constData()) << shape << start << variant << clockwise << anchor;
                    }
                }
            }
        }
    }
}

void CaacRouteTemplateGeometryTest::_templateGeometry()
{
    QFETCH(int, shape);
    QFETCH(int, start);
    QFETCH(bool, variant);
    QFETCH(bool, clockwise);
    QFETCH(QGeoCoordinate, anchor);
    const CaacRouteTemplates::Options options{.shape = static_cast<CaacRouteTemplates::Shape>(shape),
                                              .size = 100,
                                              .width = 50,
                                              .count = 8,
                                              .clockwise = clockwise,
                                              .vertexTop = variant,
                                              .start = start,
                                              .bearing = 15,
                                              .close = true};
    const auto result = CaacRouteTemplates::generate(anchor, options);
    QVERIFY(result);
    QCOMPARE(result->size(), shape == 0 ? 9 : shape == 1 ? 6 : shape == 2 ? 8 : 5);
    if (shape == 0) {
        QVERIFY(QGCGeo::geodesicDistance(result->first(), result->last()) < 0.001);
        for (const auto& point : *result) {
            QVERIFY(std::abs(QGCGeo::geodesicDistance(anchor, point) - 50) < 0.001);
        }
        const double secondBearing = QGCGeo::geodesicAzimuth(anchor, result->at(1));
        QVERIFY(std::abs(std::remainder(secondBearing - (15 + (clockwise ? 45 : -45)), 360)) < 0.001);
    } else {
        QCOMPARE(result->last(), anchor);
        QVERIFY(QGCGeo::geodesicDistance(anchor, result->first()) > 0.001);
        if (shape == 1 || shape == 3) {
            auto previous = anchor;
            for (const auto& point : *result) {
                QVERIFY(std::abs(QGCGeo::geodesicDistance(previous, point) - 100) < 0.01);
                previous = point;
            }
        }
        if (shape == 2) {
            const double firstBearing = QGCGeo::geodesicAzimuth(anchor, result->first());
            QVERIFY(std::abs(std::remainder(firstBearing - (15 + (start % 2 == 0 ? 90 : -90)), 360)) < 0.001);
            QVERIFY(std::abs(QGCGeo::geodesicDistance(anchor, result->first()) - 100) < 0.001);
        }
    }
}

void CaacPlanningToolsTest::_orientationPreview()
{
    CaacRouteEditorController editor;
    editor.setPlanMasterController(planController());
    const QGeoCoordinate home(30, 120, 20);
    missionController()->setHomePosition(home);
    QVERIFY(editor.addWaypoint(QGCGeo::geodesicDestination(home, 0, 100)));
    QVERIFY(editor.addWaypoint(QGCGeo::geodesicDestination(home, 90, 150)));
    const auto center = editor.currentItem()->coordinate();
    const auto original = planController()->saveToJson();
    missionController()->setDirty(false);
    editor.clearHistory();
    QVERIFY(editor.previewRouteOrientation(56, center));
    const auto firstPreview = planController()->saveToJson();
    QCOMPARE(editor.currentItem()->coordinate(), center);
    QVERIFY(editor.previewRouteOrientation(90, center));
    QVERIFY(editor.previewRouteOrientation(56, center));
    QCOMPARE(planController()->saveToJson(), firstPreview);
    QVERIFY(!editor.previewRouteOrientation(std::numeric_limits<double>::quiet_NaN(), center));
    QCOMPARE(planController()->saveToJson(), firstPreview);
    QVERIFY(editor.rotationPreviewActive());
    QVERIFY(!editor.addWaypoint(home));
    QCOMPARE(planController()->saveToJson(), firstPreview);
    QVERIFY(editor.undo());
    QCOMPARE(planController()->saveToJson(), original);
    QVERIFY(!missionController()->dirty());
    QVERIFY(!editor.canUndo());
    QVERIFY(editor.previewRouteOrientation(90, center));
    QVERIFY(editor.confirmRouteOrientation());
    QCOMPARE(editor.routeOrientation(), 90.0);
    QVERIFY(!editor.rotationPreviewActive());
    const auto confirmed = planController()->saveToJson();
    QVERIFY(editor.undo());
    QCOMPARE(planController()->saveToJson(), original);
    QVERIFY(!editor.canUndo());
    QVERIFY(editor.redo());
    QCOMPARE(planController()->saveToJson(), confirmed);
    QTemporaryDir directory;
    QVERIFY(directory.isValid());
    const auto path = directory.filePath(QStringLiteral("orientation.plan"));
    QVERIFY(planController()->saveToFile(path));
    editor.clearWaypoints();
    planController()->loadFromFile(path);
    QCOMPARE(editor.routeOrientation(), 90.0);
    QCOMPARE(planController()->saveToJson(), confirmed);
}

void CaacPlanningToolsTest::_quickWaypointOperations()
{
    CaacRouteEditorController editor;
    editor.setPlanMasterController(planController());
    const QGeoCoordinate home(30, 120, 20);
    missionController()->setHomePosition(home);
    QVERIFY(editor.addWaypoint(QGCGeo::geodesicDestination(home, 0, 100)));
    auto* first = editor.currentItem();
    QVERIFY(editor.updateWaypoint(first, first->coordinate().latitude(), first->coordinate().longitude(), 65, true, 7));
    const auto frame = first->missionItem().frame();
    QVERIFY(editor.updateWaypointLeg(first, 90, 50));
    const auto leg = editor.waypointLeg(first);
    QVERIFY(std::abs(leg.value("distance").toDouble() - 50) < 0.001);
    QVERIFY(std::abs(leg.value("bearing").toDouble() - 90) < 0.001);
    QCOMPARE(first->altitude()->rawValue().toDouble(), 65.0);
    QCOMPARE(first->missionItem().frame(), frame);
    QCOMPARE(first->speedSection()->flightSpeed()->rawValue().toDouble(), 7.0);
    const auto valid = planController()->saveToJson();
    QVERIFY(!editor.updateWaypointLeg(first, 0, -1));
    QVERIFY(!editor.updateWaypointLeg(first, std::numeric_limits<double>::quiet_NaN(), 1));
    QCOMPARE(planController()->saveToJson(), valid);
    QVERIFY(editor.updateWaypointLeg(first, 90, 0));
    QCOMPARE(first->coordinate().latitude(), home.latitude());
    QCOMPARE(first->coordinate().longitude(), home.longitude());
    QVERIFY(editor.addWaypoint(QGCGeo::geodesicDestination(home, 180, 80)));
    const auto secondCoordinate = editor.currentItem()->coordinate();
    const auto original = planController()->saveToJson();
    editor.clearHistory();
    QVERIFY(editor.reverseWaypoints());
    QCOMPARE(missionController()->visualItems()->value<SimpleMissionItem*>(1)->coordinate(), secondCoordinate);
    QVERIFY(editor.undo());
    QCOMPARE(planController()->saveToJson(), original);
    QVERIFY(editor.useWaypointAsHome(missionController()->visualItems()->value<SimpleMissionItem*>(2)));
    QCOMPARE(missionController()->visualItems()->count(), 2);
    QVERIFY(QGCGeo::geodesicDistance(missionController()->plannedHomePosition(), secondCoordinate) < 0.001);
    QCOMPARE(missionController()->plannedHomePosition().altitude(), 20.0);
    QVERIFY(editor.undo());
    QCOMPARE(planController()->saveToJson(), original);
    QVERIFY(editor.clearWaypoints());
    QCOMPARE(missionController()->visualItems()->count(), 1);
    QCOMPARE(missionController()->plannedHomePosition(), home);
    QVERIFY(editor.undo());
    QCOMPARE(planController()->saveToJson(), original);
}

void CaacPlanningToolsTest::_templateTransactions()
{
    CaacRouteEditorController editor;
    editor.setPlanMasterController(planController());
    missionController()->setHomePosition(QGeoCoordinate(30, 120, 20));
    QVERIFY(editor.addWaypoint(QGeoCoordinate(30.001, 120)));
    QVERIFY(editor.addWaypoint(QGeoCoordinate(30.002, 120)));
    const auto trailingCoordinate = editor.currentItem()->coordinate();
    editor.clearHistory();
    const QVariantMap options{{"size", 100}, {"count", 10}, {"close", true}};
    QVERIFY(editor.insertTemplate(0, 1, options));
    QCOMPARE(missionController()->visualItems()->count(), 14);
    QCOMPARE(missionController()->visualItems()->value<SimpleMissionItem*>(13)->coordinate(), trailingCoordinate);
    QVERIFY(editor.undo());
    QCOMPARE(missionController()->visualItems()->count(), 3);
    QVERIFY(editor.redo());
    QCOMPARE(missionController()->visualItems()->count(), 14);
}

void CaacPlanningToolsTest::_translationPreservesMission()
{
    CaacRouteEditorController editor;
    editor.setPlanMasterController(planController());
    const QGeoCoordinate home(30, 120, 20);
    missionController()->setHomePosition(home);
    auto* takeoff = qobject_cast<SimpleMissionItem*>(missionController()->insertTakeoffItem(home, 1, true));
    QVERIFY(takeoff);
    const auto a = QGCGeo::geodesicDestination(home, 30, 100);
    QVERIFY(editor.addWaypoint(a));
    auto* waypoint = editor.currentItem();
    QVERIFY(waypoint);
    QVERIFY(editor.updateWaypoint(waypoint, a.latitude(), a.longitude(), 65, true, 7));
    auto* landing = qobject_cast<SimpleMissionItem*>(missionController()->insertSimpleMissionItem(home, 3, true));
    QVERIFY(landing);
    landing->setCommand(MAV_CMD_NAV_LAND);
    QList<SimpleMissionItem*> items{takeoff, waypoint, landing};
    QList<int> commands;
    QList<double> altitudes;
    QList<int> frames;
    for (auto* item : items) {
        commands.append(item->command());
        altitudes.append(item->altitude()->rawValue().toDouble());
        frames.append(item->missionItem().frame());
    }
    const auto target = QGCGeo::geodesicDestination(home, 45, 100);
    editor.clearHistory();
    QVERIFY(editor.translateRoute(home, target));
    QVERIFY(QGCGeo::geodesicDistance(missionController()->plannedHomePosition(), target) < 0.001);
    QVERIFY(QGCGeo::geodesicDistance(takeoff->coordinate(), target) < 0.001);
    QVERIFY(QGCGeo::geodesicDistance(landing->coordinate(), target) < 0.001);
    QVERIFY(std::abs(QGCGeo::geodesicDistance(target, waypoint->coordinate()) - 100) < 0.001);
    for (int i = 0; i < items.size(); ++i) {
        QCOMPARE(items[i]->command(), commands[i]);
        QCOMPARE(items[i]->altitude()->rawValue().toDouble(), altitudes[i]);
        QCOMPARE(items[i]->missionItem().frame(), frames[i]);
    }
    QCOMPARE(waypoint->speedSection()->property("specifyFlightSpeed").toBool(), true);
    QVERIFY(editor.undo());
    QVERIFY(QGCGeo::geodesicDistance(missionController()->plannedHomePosition(), home) < 0.001);
    QVERIFY(editor.redo());
    QVERIFY(QGCGeo::geodesicDistance(missionController()->plannedHomePosition(), target) < 0.001);
    QVERIFY(editor.updatePlannedHome(31, 121, 123));
    QCOMPARE(missionController()->plannedHomePosition().altitude(), 123.0);
    QVERIFY(editor.undo());
    QVERIFY(QGCGeo::geodesicDistance(missionController()->plannedHomePosition(), target) < 0.001);
}

void CaacPlanningToolsTest::_invalidInputsAreAtomic()
{
    CaacRouteEditorController editor;
    editor.setPlanMasterController(planController());
    missionController()->setHomePosition(QGeoCoordinate(30, 120, 20));
    QVERIFY(editor.addWaypoint(QGeoCoordinate(30.001, 120)));
    QJsonObject before;
    missionController()->save(before);
    for (const QVariantMap& options : {QVariantMap{{"size", -1}}, QVariantMap{{"count", 2}},
                                       QVariantMap{{"count", 1001}}, QVariantMap{{"count", 5.5}}}) {
        QVERIFY(!editor.insertTemplate(0, 0, options));
    }
    QVERIFY(!editor.insertTemplate(4, 0, {}));
    QVERIFY(!editor.insertTemplate(1, 10, {}));
    QVERIFY(!editor.translateRoute(QGeoCoordinate(), QGeoCoordinate(30, 120)));
    QVERIFY(!editor.moveRouteBy(90, -100));
    QVERIFY(!editor.updatePlannedHome(91, 120, 20));
    QJsonObject after;
    missionController()->save(after);
    QCOMPARE(before, after);
}

void CaacPlanningToolsTest::_coordinateFormats()
{
    const CaacCorePlugin plugin;
    for (int maximum : {90, 180}) {
        for (double value : {0.0, -0.0, 0.5, -0.5, 22.5125727, -113.4427937, 90.0, -90.0, 180.0, -180.0,
                             std::nextafter(90.0, 0.0), std::nextafter(180.0, 0.0)}) {
            const auto dms = plugin.coordinateToDms(value, maximum);
            if (std::abs(value) > maximum) {
                QVERIFY(dms.isEmpty());
                continue;
            }
            QVERIFY(!dms.isEmpty());
            QVERIFY(dms.value("seconds").toDouble() < 60);
            QCOMPARE(dms.value("negative").toBool(), std::signbit(value));
            const double restored =
                plugin.coordinateFromDms(dms.value("degrees").toDouble(), dms.value("minutes").toDouble(),
                                         dms.value("seconds").toDouble(), dms.value("negative").toBool(), maximum);
            QVERIFY(std::abs(restored - value) < 3e-13);
            QCOMPARE(std::signbit(restored), std::signbit(value));
        }
    }
    const double nan = std::numeric_limits<double>::quiet_NaN();
    QVERIFY(plugin.coordinateToDms(nan, 90).isEmpty());
    QVERIFY(plugin.coordinateToDms(0, 360).isEmpty());
    for (const auto& parts : {QList<double>{90, 0, 1},
                              {1, 60, 0},
                              {1, 0, 60},
                              {-1, 0, 0},
                              {1.5, 0, 0},
                              {0, 1.5, 0},
                              {0, 0, -1},
                              {nan, 0, 0},
                              {0, nan, 0},
                              {0, 0, nan}}) {
        QVERIFY(std::isnan(plugin.coordinateFromDms(parts[0], parts[1], parts[2], false, 90)));
    }
}

void CaacPlanningToolsTest::_advancedWaypointProperties()
{
    CaacRouteEditorController editor;
    editor.setPlanMasterController(planController());
    missionController()->setHomePosition(QGeoCoordinate(30, 120, 20));
    QVERIFY(editor.addWaypoint(QGeoCoordinate(30.001, 120)));
    auto* item = editor.currentItem();
    editor.beginEdit();
    item->missionItem().setParam2(3.25);
    item->missionItem().setParam3(8);
    editor.endEdit();
    const auto before = planController()->saveToJson();
    QVariantMap properties{{"latitude", 30.002}, {"longitude", 120.003}, {"altitude", 65}, {"specifySpeed", false},
                           {"holdSeconds", 2.5}, {"specifyYaw", true},   {"yaw", 270}};
    QVERIFY(editor.updateWaypointProperties(item, properties));
    QCOMPARE(item->missionItem().param1(), 2.5);
    QCOMPARE(item->missionItem().param4(), -90);
    QCOMPARE(item->missionItem().param2(), 3.25);
    QCOMPARE(item->missionItem().param3(), 8);
    const auto edited = planController()->saveToJson();
    editor.undo();
    QCOMPARE(planController()->saveToJson(), before);
    editor.redo();
    QCOMPARE(planController()->saveToJson(), edited);
    item = editor.currentItem();
    QVERIFY(item);
    for (const auto& invalid : {QVariantMap{{"holdSeconds", -1}}, QVariantMap{{"holdSeconds", "invalid"}},
                                QVariantMap{{"yaw", std::numeric_limits<double>::quiet_NaN()}},
                                QVariantMap{{"latitude", 91}}, QVariantMap{{"altitude", "invalid"}}}) {
        auto rejected = properties;
        for (auto it = invalid.cbegin(); it != invalid.cend(); ++it) {
            rejected[it.key()] = it.value();
        }
        QVERIFY(!editor.updateWaypointProperties(item, rejected));
        QCOMPARE(planController()->saveToJson(), edited);
    }
    properties["specifyYaw"] = false;
    QVERIFY(editor.updateWaypointProperties(item, properties));
    QVERIFY(std::isnan(item->missionItem().param4()));
    QTemporaryDir directory;
    QVERIFY(directory.isValid());
    const auto file = directory.filePath(QStringLiteral("advanced.plan"));
    QVERIFY(planController()->saveToFile(file));
    const auto saved = planController()->saveToJson();
    planController()->removeAll();
    planController()->loadFromFile(file);
    QCOMPARE(planController()->saveToJson(), saved);
}

void CaacPlanningToolsTest::_missionSpeedTransaction()
{
    CaacRouteEditorController editor;
    editor.setPlanMasterController(planController());
    missionController()->setHomePosition(QGeoCoordinate(30, 120, 20));
    QVERIFY(editor.addWaypoint(QGeoCoordinate(30.001, 120)));
    editor.clearHistory();
    const auto before = planController()->saveToJson();
    for (double invalid : {0.0, -1.0, 1e100, std::numeric_limits<double>::quiet_NaN()}) {
        QVERIFY(!editor.setMissionFlightSpeed(true, invalid));
        QCOMPARE(planController()->saveToJson(), before);
        QVERIFY(!editor.canUndo());
    }
    QVERIFY(editor.setMissionFlightSpeed(true, 12.5));
    const auto edited = planController()->saveToJson();
    auto* settings = missionController()->visualItems()->value<MissionSettingsItem*>(0);
    QVERIFY(settings);
    QList<MissionItem*> commands;
    settings->appendMissionItems(commands, &editor);
    QVERIFY(!commands.isEmpty());
    auto* speedCommand = commands.last();
    QCOMPARE(speedCommand->command(), MAV_CMD_DO_CHANGE_SPEED);
    QCOMPARE(speedCommand->param2(), 12.5);
    QVERIFY(editor.undo());
    QCOMPARE(planController()->saveToJson(), before);
    QVERIFY(editor.redo());
    QCOMPARE(planController()->saveToJson(), edited);
    QTemporaryDir directory;
    QVERIFY(directory.isValid());
    const auto file = directory.filePath(QStringLiteral("mission-speed.plan"));
    QVERIFY(planController()->saveToFile(file));
    planController()->removeAll();
    planController()->loadFromFile(file);
    QCOMPARE(planController()->saveToJson(), edited);
}

void CaacPlanningToolsTest::_scopedFlightSpeeds()
{
    auto* plugin = qobject_cast<CaacCorePlugin*>(QGCCorePlugin::instance());
    QVERIFY(plugin);
    CaacRouteEditorController editor;
    editor.setPlanMasterController(planController());
    auto* mission = missionController();
    const QGeoCoordinate home(30, 120, 100);
    mission->setHomePosition(home);
    for (int i = 1; i <= 4; ++i) {
        QVERIFY(editor.addWaypoint(home.atDistanceAndAzimuth(i * 100, 90)));
    }
    QVERIFY(editor.setMissionFlightSpeed(true, 5));
    auto* first = mission->visualItems()->value<SimpleMissionItem*>(1);
    auto* second = mission->visualItems()->value<SimpleMissionItem*>(2);
    auto* third = mission->visualItems()->value<SimpleMissionItem*>(3);
    QVERIFY(first && second && third);
    first->speedSection()->flightSpeed()->setRawValue(7);
    first->speedSection()->setSpecifyFlightSpeed(true);
    second->speedSection()->flightSpeed()->setRawValue(5);
    second->speedSection()->setSpecifyFlightSpeed(true);
    auto* explicitSpeed = qobject_cast<SimpleMissionItem*>(mission->insertSimpleMissionItem({}, 2, false));
    QVERIFY(explicitSpeed);
    explicitSpeed->setCommand(MAV_CMD_DO_CHANGE_SPEED);
    explicitSpeed->missionItem().setParam1(1);
    explicitSpeed->missionItem().setParam2(4.5);
    explicitSpeed->missionItem().setParam3(-1);
    explicitSpeed->missionItem().setFrame(MAV_FRAME_MISSION);
    explicitSpeed->missionItem().setParam4(0);
    explicitSpeed->missionItem().setParam5(0);
    explicitSpeed->missionItem().setParam6(0);
    explicitSpeed->missionItem().setParam7(0);
    auto* jump =
        qobject_cast<SimpleMissionItem*>(mission->insertSimpleMissionItem({}, mission->visualItems()->count(), false));
    QVERIFY(jump);
    jump->setCommand(MAV_CMD_DO_JUMP);
    jump->missionItem().setParam1(first->sequenceNumber());
    jump->missionItem().setParam2(2);
    auto settings = CaacSimulationController::defaults();
    settings["startSequence"] = second->sequenceNumber();
    settings["returnSequence"] = third->sequenceNumber();
    QVERIFY(editor.setSimulationSettings(settings));
    QTRY_VERIFY_WITH_TIMEOUT(third->simpleFlightPathSegment(), TestTimeout::mediumMs());
    QTRY_COMPARE_WITH_TIMEOUT(first->simpleFlightPathSegment()->flightSpeed(), 4.5, TestTimeout::mediumMs());
    const QVariantList selected{QVariant::fromValue(static_cast<QObject*>(first))};
    const auto before = planController()->saveToJson();
    for (double invalid : {0.0, qQNaN(), 1e100}) {
        QVERIFY(!plugin->setFlightWaypointSpeeds(planController(), selected, invalid, false).isEmpty());
        QCOMPARE(planController()->saveToJson(), before);
    }
    QObject foreign;
    QVERIFY(!plugin->setFlightWaypointSpeeds(planController(), {QVariant::fromValue(&foreign)}, 8, false).isEmpty());
    QCOMPARE(planController()->saveToJson(), before);
    QVERIFY(plugin->setFlightWaypointSpeeds(planController(), selected, 8, false).isEmpty());
    auto* homeItem = mission->visualItems()->value<MissionSettingsItem*>(0);
    QTRY_COMPARE_WITH_TIMEOUT(homeItem->simpleFlightPathSegment()->flightSpeed(), 5.0, TestTimeout::mediumMs());
    QTRY_COMPARE_WITH_TIMEOUT(first->simpleFlightPathSegment()->flightSpeed(), 8.0, TestTimeout::mediumMs());
    QTRY_COMPARE_WITH_TIMEOUT(second->simpleFlightPathSegment()->flightSpeed(), 5.0, TestTimeout::mediumMs());
    QTRY_COMPARE_WITH_TIMEOUT(third->simpleFlightPathSegment()->flightSpeed(), 5.0, TestTimeout::mediumMs());
    QCOMPARE(explicitSpeed->missionItem().param2(), 8.0);
    QCOMPARE(jump->missionItem().param1(), double(first->sequenceNumber()));
    QCOMPARE(jump->missionItem().param2(), 2.0);
    QCOMPARE(editor.simulationSettings().value("startSequence").toInt(), second->sequenceNumber());
    QCOMPARE(editor.simulationSettings().value("returnSequence").toInt(), third->sequenceNumber());

    jump->missionItem().setParam1(first->sequenceNumber() + 1);
    QVERIFY(plugin->setFlightWaypointSpeeds(planController(), selected, 9, true).isEmpty());
    QTRY_COMPARE_WITH_TIMEOUT(homeItem->simpleFlightPathSegment()->flightSpeed(), 9.0, TestTimeout::mediumMs());
    for (auto* item : {first, second, third}) {
        QTRY_COMPARE_WITH_TIMEOUT(item->simpleFlightPathSegment()->flightSpeed(), 9.0, TestTimeout::mediumMs());
    }
    QCOMPARE(explicitSpeed->missionItem().param2(), 9.0);
    QCOMPARE(jump->missionItem().param1(), double(first->sequenceNumber() + 1));
    QCOMPARE(jump->missionItem().param2(), 2.0);
}

void CaacPlanningToolsTest::_scopedFlightSpeedCommandTypes_data()
{
    QTest::addColumn<bool>("fixedWing");
    QTest::newRow("multirotor-groundspeed") << false;
    QTest::newRow("fixedwing-airspeed") << true;
}

void CaacPlanningToolsTest::_scopedFlightSpeedCommandTypes()
{
    QFETCH(bool, fixedWing);
    auto* plugin = qobject_cast<CaacCorePlugin*>(QGCCorePlugin::instance());
    QVERIFY(plugin);
    PlanMasterController plan(MAV_AUTOPILOT_PX4, fixedWing ? MAV_TYPE_FIXED_WING : MAV_TYPE_QUADROTOR);
    plan.setFlyView(false);
    plan.start();
    QCOMPARE(plan.controllerVehicle()->fixedWing(), fixedWing);
    auto* mission = plan.missionController();
    CaacRouteEditorController editor;
    editor.setPlanMasterController(&plan);
    const QGeoCoordinate home(30, 120, 100);
    mission->setHomePosition(home);
    for (int i = 1; i <= 3; ++i) {
        QVERIFY(editor.addWaypoint(home.atDistanceAndAzimuth(i * 100, 90)));
    }
    QVERIFY(editor.setMissionFlightSpeed(true, 5));
    auto* first = mission->visualItems()->value<SimpleMissionItem*>(1);
    auto* second = mission->visualItems()->value<SimpleMissionItem*>(2);
    QVERIFY(first && second);
    const int horizontalType = fixedWing ? 0 : 1;
    QList<SimpleMissionItem*> otherSpeeds;
    const auto insertSpeed = [&](int type, double value, double relative) {
        auto* item = qobject_cast<SimpleMissionItem*>(
            mission->insertSimpleMissionItem({}, mission->visualItems()->indexOf(second), false));
        if (!item) {
            return item;
        }
        item->setCommand(MAV_CMD_DO_CHANGE_SPEED);
        auto& command = item->missionItem();
        command.setFrame(MAV_FRAME_MISSION);
        command.setParam1(type);
        command.setParam2(value);
        command.setParam3(-1);
        command.setParam4(relative);
        command.setParam5(0);
        command.setParam6(0);
        command.setParam7(0);
        return item;
    };
    for (int type : {1 - horizontalType, 2, 3}) {
        auto* item = insertSpeed(type, type + 3, 0);
        QVERIFY(item);
        otherSpeeds.append(item);
    }
    QTRY_VERIFY_WITH_TIMEOUT(first->simpleFlightPathSegment(), TestTimeout::mediumMs());
    QTRY_COMPARE_WITH_TIMEOUT(first->simpleFlightPathSegment()->flightSpeed(), 5.0, TestTimeout::mediumMs());
    const QVariantList selected{QVariant::fromValue(static_cast<QObject*>(second))};
    QVERIFY(plugin->setFlightWaypointSpeeds(&plan, selected, 8, false).isEmpty());
    QTRY_COMPARE_WITH_TIMEOUT(first->simpleFlightPathSegment()->flightSpeed(), 5.0, TestTimeout::mediumMs());
    QTRY_VERIFY_WITH_TIMEOUT(second->simpleFlightPathSegment(), TestTimeout::mediumMs());
    QTRY_COMPARE_WITH_TIMEOUT(second->simpleFlightPathSegment()->flightSpeed(), 8.0, TestTimeout::mediumMs());
    for (const auto* item : otherSpeeds) {
        const auto& command = item->missionItem();
        QCOMPARE(command.param2(), command.param1() + 3);
        QCOMPARE(command.param3(), -1.0);
        QCOMPARE(command.param4(), 0.0);
    }
    auto* relative = insertSpeed(horizontalType, 3, 1);
    QVERIFY(relative);
    const auto before = plan.saveToJson();
    QVERIFY(!plugin->setFlightWaypointSpeeds(&plan, selected, 9, false).isEmpty());
    QCOMPARE(plan.saveToJson(), before);
    QVERIFY(!plugin->setFlightWaypointSpeeds(&plan, selected, 9, true).isEmpty());
    QCOMPARE(plan.saveToJson(), before);
}

void CaacPlanningToolsTest::_legFlightSpeeds()
{
    CaacRouteEditorController editor;
    editor.setPlanMasterController(planController());
    auto* mission = missionController();
    const QGeoCoordinate home(30, 120, 100);
    mission->setHomePosition(home);
    for (int i = 1; i <= 3; ++i) {
        QVERIFY(editor.addWaypoint(home.atDistanceAndAzimuth(i * 100, 45)));
    }
    auto* first = mission->visualItems()->value<SimpleMissionItem*>(1);
    auto* second = mission->visualItems()->value<SimpleMissionItem*>(2);
    QVERIFY(first && second);
    auto* homeItem = mission->visualItems()->value<MissionSettingsItem*>(0);
    auto* vehicle = planController()->controllerVehicle();
    QVERIFY(homeItem && vehicle);
    QVERIFY(vehicle->multiRotor());
    QVERIFY(editor.setMissionFlightSpeed(false, 0));
    QTRY_VERIFY_WITH_TIMEOUT(homeItem->simpleFlightPathSegment(), TestTimeout::mediumMs());
    QTRY_COMPARE_WITH_TIMEOUT(homeItem->simpleFlightPathSegment()->flightSpeed(), vehicle->defaultHoverSpeed(),
                              TestTimeout::mediumMs());
    QVERIFY(std::isfinite(homeItem->simpleFlightPathSegment()->flightSpeed()));
    QVERIFY(homeItem->simpleFlightPathSegment()->flightSpeed() > 0);
    QVERIFY(editor.setMissionFlightSpeed(true, 6));
    QTRY_COMPARE_WITH_TIMEOUT(homeItem->simpleFlightPathSegment()->flightSpeed(), 6.0, TestTimeout::mediumMs());
    first->speedSection()->flightSpeed()->setRawValue(8.5);
    first->speedSection()->setSpecifyFlightSpeed(true);
    QTRY_VERIFY_WITH_TIMEOUT(first->simpleFlightPathSegment(), TestTimeout::mediumMs());
    QTRY_COMPARE_WITH_TIMEOUT(first->simpleFlightPathSegment()->flightSpeed(), 8.5, TestTimeout::mediumMs());
    QTRY_VERIFY_WITH_TIMEOUT(second->simpleFlightPathSegment(), TestTimeout::mediumMs());
    QTRY_COMPARE_WITH_TIMEOUT(second->simpleFlightPathSegment()->flightSpeed(), 8.5, TestTimeout::mediumMs());
    second->speedSection()->flightSpeed()->setRawValue(11);
    second->speedSection()->setSpecifyFlightSpeed(true);
    QTRY_COMPARE_WITH_TIMEOUT(second->simpleFlightPathSegment()->flightSpeed(), 11.0, TestTimeout::mediumMs());
    QCOMPARE(first->simpleFlightPathSegment()->flightSpeed(), 8.5);
    QCOMPARE(homeItem->simpleFlightPathSegment()->flightSpeed(), 6.0);

    auto settings = CaacSimulationController::defaults();
    settings["closed"] = true;
    settings["endBehavior"] = 2;
    const auto preview = editor.simulationPreviewSegments(settings);
    QVERIFY(!preview.isEmpty());
    for (const auto& value : preview) {
        const auto segment = value.toMap();
        const auto from = segment.value("coordinate1").value<QGeoCoordinate>();
        const auto kind = segment.value("kind").toString();
        const double expected = kind == "start"                                              ? 6
                                : kind == "body" && from.distanceTo(first->coordinate()) < 1 ? 8.5
                                                                                             : 11;
        QCOMPARE(segment.value("flightSpeed").toDouble(), expected);
    }
    first->speedSection()->setSpecifyFlightSpeed(false);
    QTRY_COMPARE_WITH_TIMEOUT(first->simpleFlightPathSegment()->flightSpeed(), 6.0, TestTimeout::mediumMs());
    QVERIFY(editor.setMissionFlightSpeed(true, 7));
    QTRY_COMPARE_WITH_TIMEOUT(homeItem->simpleFlightPathSegment()->flightSpeed(), 7.0, TestTimeout::mediumMs());
    QTRY_COMPARE_WITH_TIMEOUT(first->simpleFlightPathSegment()->flightSpeed(), 7.0, TestTimeout::mediumMs());
    QCOMPARE(second->simpleFlightPathSegment()->flightSpeed(), 11.0);
    auto* speedCommand = qobject_cast<SimpleMissionItem*>(mission->insertSimpleMissionItem(QGeoCoordinate(), 2, false));
    QVERIFY(speedCommand);
    speedCommand->setCommand(MAV_CMD_DO_CHANGE_SPEED);
    speedCommand->missionItem().setParam1(1);
    speedCommand->missionItem().setParam2(9.5);
    QTRY_COMPARE_WITH_TIMEOUT(first->simpleFlightPathSegment()->flightSpeed(), 9.5, TestTimeout::mediumMs());
    QCOMPARE(second->simpleFlightPathSegment()->flightSpeed(), 11.0);
    QCOMPARE(homeItem->simpleFlightPathSegment()->flightSpeed(), 7.0);
    const auto changedPreview = editor.simulationPreviewSegments(settings);
    const auto firstLeg = changedPreview.at(1).toMap();
    QCOMPARE(firstLeg.value("coordinate1").value<QGeoCoordinate>(), first->coordinate());
    QCOMPARE(firstLeg.value("flightSpeed").toDouble(), 9.5);

    auto* initialSpeedCommand =
        qobject_cast<SimpleMissionItem*>(mission->insertSimpleMissionItem(QGeoCoordinate(), 1, false));
    QVERIFY(initialSpeedCommand);
    initialSpeedCommand->setCommand(MAV_CMD_DO_CHANGE_SPEED);
    initialSpeedCommand->missionItem().setParam1(1);
    initialSpeedCommand->missionItem().setParam2(4.5);
    QTRY_VERIFY_WITH_TIMEOUT(homeItem->simpleFlightPathSegment(), TestTimeout::mediumMs());
    QTRY_COMPARE_WITH_TIMEOUT(homeItem->simpleFlightPathSegment()->flightSpeed(), 4.5, TestTimeout::mediumMs());
    QCOMPARE(first->simpleFlightPathSegment()->flightSpeed(), 9.5);
    QCOMPARE(second->simpleFlightPathSegment()->flightSpeed(), 11.0);
    initialSpeedCommand->missionItem().setParam2(5.5);
    QTRY_COMPARE_WITH_TIMEOUT(homeItem->simpleFlightPathSegment()->flightSpeed(), 5.5, TestTimeout::mediumMs());
    const auto initialPreview = editor.simulationPreviewSegments(settings);
    QVERIFY(!initialPreview.isEmpty());
    const auto initialLeg = initialPreview.first().toMap();
    QCOMPARE(initialLeg.value("coordinate1").value<QGeoCoordinate>(), home);
    QCOMPARE(initialLeg.value("coordinate2").value<QGeoCoordinate>(), first->coordinate());
    QCOMPARE(initialLeg.value("flightSpeed").toDouble(), 5.5);
    mission->removeVisualItem(1);
    QTRY_VERIFY_WITH_TIMEOUT(homeItem->simpleFlightPathSegment(), TestTimeout::mediumMs());
    QTRY_COMPARE_WITH_TIMEOUT(homeItem->simpleFlightPathSegment()->flightSpeed(), 7.0, TestTimeout::mediumMs());
    QCOMPARE(first->simpleFlightPathSegment()->flightSpeed(), 9.5);
    QCOMPARE(second->simpleFlightPathSegment()->flightSpeed(), 11.0);
}

UT_REGISTER_TEST(CaacPlanningToolsTest, TestLabel::Unit, TestLabel::MissionManager)
UT_REGISTER_TEST(CaacRouteTemplateGeometryTest, TestLabel::Unit, TestLabel::MissionManager)

#include "CaacPlanningToolsTest.moc"
