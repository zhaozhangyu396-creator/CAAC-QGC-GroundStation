#include <cmath>
#include <limits>

#include <QtCore/QFile>
#include <QtCore/QTemporaryDir>

#include "CaacRouteEditorController.h"
#include "CaacSimulationController.h"
#include "CaacSimulationEngine.h"
#include "MissionController.h"
#include "MissionTest.h"
#include "PlanMasterController.h"
#include "QGCGeo.h"

namespace {
mavlink_mission_item_int_t waypoint(int sequence, const QGeoCoordinate& position, float altitude)
{
    mavlink_mission_item_int_t item{};
    item.seq = static_cast<uint16_t>(sequence);
    item.command = MAV_CMD_NAV_WAYPOINT;
    item.frame = MAV_FRAME_GLOBAL_RELATIVE_ALT;
    item.x = static_cast<int32_t>(position.latitude() * 1e7);
    item.y = static_cast<int32_t>(position.longitude() * 1e7);
    item.z = altitude;
    item.param4 = std::numeric_limits<float>::quiet_NaN();
    item.autocontinue = 1;
    return item;
}
}  // namespace

class CaacSimulationEngineTest : public OfflineMissionTest
{
    Q_OBJECT
private slots:
    void _climbGeometry();
    void _attitudeRespondsToMotion();
    void _turnModes();
    void _loopsAndReturn();
    void _windComponents();
    void _rejectInvalidMissionAtomically();
    void _settingsHistoryAndFile();
    void _guidedTargetPreservesMission();
    void _manualFlightWithoutMission();
};

void CaacSimulationEngineTest::_manualFlightWithoutMission()
{
    CaacSimulationEngine engine;
    const QGeoCoordinate home(30, 120, 20);
    engine.initialize(home, {.horizontalSpeed = 10, .verticalSpeed = 10});
    QVERIFY(!engine.loaded());
    QVERIFY(!engine.state().airborne);
    for (int i = 0; i < 20; ++i) {
        engine.advanceHoldAltitude(22, 0.1);
    }
    QCOMPARE(engine.state().position.altitude(), 22.0);
    QVERIFY(engine.state().airborne);
    auto target = QGCGeo::geodesicDestination(home, 90, 5);
    target.setAltitude(22);
    for (int i = 0; i < 20; ++i) {
        engine.advanceGuidedTarget(target, 0.1);
    }
    QVERIFY(QGCGeo::geodesicDistance(engine.state().position, target) < 0.01);
    bool landed = false;
    for (int i = 0; i < 100 && !landed; ++i) {
        landed = engine.advanceReturn(0.1);
    }
    QVERIFY(landed);
    QVERIFY(!engine.state().airborne);
    QVERIFY(QGCGeo::geodesicDistance(engine.state().position, home) < 0.01);
    QCOMPARE(engine.state().position.altitude(), 20.0);
    for (int i = 0; i < 20; ++i) {
        engine.advanceHoldAltitude(22, 0.1);
    }
    for (int i = 0; i < 20; ++i) {
        landed = engine.advanceLanding(0.1);
    }
    QVERIFY(landed);
    QVERIFY(!engine.state().airborne);
}

void CaacSimulationEngineTest::_guidedTargetPreservesMission()
{
    CaacSimulationEngine engine;
    QString error;
    const QGeoCoordinate home(30, 120, 20);
    QVERIFY(engine.load({{0, waypoint(0, home, 2)}}, home, {}, error));
    engine.setAltitude(22);
    auto target = QGCGeo::geodesicDestination(home, 45, 2);
    target.setAltitude(22);
    for (int i = 0; i < 20; ++i) {
        engine.advanceGuidedTarget(target, 0.1);
    }
    QVERIFY(QGCGeo::geodesicDistance(engine.state().position, target) < 0.001);
    QCOMPARE(engine.state().sequence, 0);
    QVERIFY(!engine.state().finished);
    QCOMPARE(engine.state().position.altitude(), 22.0);
    engine.advanceGuidedTarget(QGeoCoordinate(), 0.1);
    QCOMPARE(engine.state().position, target);
    engine.pause();
    QCOMPARE(engine.state().northSpeed, 0.0);
    QCOMPARE(engine.state().eastSpeed, 0.0);
}

void CaacSimulationEngineTest::_attitudeRespondsToMotion()
{
    const QGeoCoordinate home(30, 120, 20);
    for (double bearing : {90.0, 270.0}) {
        for (double altitude : {2.0, 10.0}) {
            CaacSimulationEngine engine;
            QString error;
            const auto target = QGCGeo::geodesicDestination(home, bearing, 100);
            QVERIFY(engine.load({{0, waypoint(0, target, static_cast<float>(altitude))}}, home,
                                {.horizontalSpeed = 10, .verticalSpeed = 10, .turnMode = 1}, error));
            engine.setAltitude(25);
            engine.advance(0.1);
            QVERIFY(std::isfinite(engine.state().roll) && std::isfinite(engine.state().pitch));
            QVERIFY(bearing == 90 ? engine.state().roll > 0 : engine.state().roll < 0);
            QVERIFY(altitude == 10 ? engine.state().pitch > 0 : engine.state().pitch < 0);
            QVERIFY(std::abs(engine.state().roll) <= 35);
            QVERIFY(std::abs(engine.state().pitch) <= 30);
            engine.pause();
            QCOMPARE(engine.state().roll, 0.0);
            QCOMPARE(engine.state().pitch, 0.0);
        }
    }
}

void CaacSimulationEngineTest::_climbGeometry()
{
    const QGeoCoordinate home(30, 120, 20);
    const auto target = QGCGeo::geodesicDestination(home, 0, 100);
    for (int climbMode : {0, 1}) {
        CaacSimulationEngine engine;
        QString error;
        QVERIFY(engine.load({{0, waypoint(0, target, 50)}}, home,
                            {.horizontalSpeed = 10, .verticalSpeed = 2, .turnMode = 3, .climbMode = climbMode}, error));
        engine.advance(1);
        QCOMPARE(engine.state().position.altitude(), 22.0);
        const auto distance = QGCGeo::geodesicDistance(home, engine.state().position);
        QVERIFY(std::abs(distance - (climbMode == 0 ? 0 : 4)) < 0.02);
    }
}

void CaacSimulationEngineTest::_turnModes()
{
    const QGeoCoordinate home(30, 120, 20);
    const auto target = QGCGeo::geodesicDestination(home, 90, 100);
    for (int turnMode : {0, 1, 2, 3}) {
        CaacSimulationEngine engine;
        QString error;
        QVERIFY(engine.load({{0, waypoint(0, target, 5)}}, home,
                            {.horizontalSpeed = 10, .verticalSpeed = 10, .yawRate = 60, .turnMode = turnMode}, error));
        engine.advance(0.1);
        const auto distance = QGCGeo::geodesicDistance(home, engine.state().position);
        if (turnMode == 0) {
            QVERIFY(distance < 0.01);
        } else {
            QVERIFY(distance > 0.9);
        }
        const double expectedHeading = turnMode == 3 ? 0 : turnMode == 2 ? 1 : 6;
        QVERIFY(std::abs(engine.state().heading - expectedHeading) < 0.01);
    }
}

void CaacSimulationEngineTest::_loopsAndReturn()
{
    const QGeoCoordinate home(30, 120, 20);
    auto first = waypoint(0, home, 2);
    first.param1 = 0.5;
    const auto second = waypoint(1, QGCGeo::geodesicDestination(home, 0, 5), 2);
    auto jump = waypoint(2, home, 0);
    jump.command = MAV_CMD_DO_JUMP;
    jump.param1 = 0;
    jump.param2 = 1;
    auto returning = waypoint(3, home, 0);
    returning.command = MAV_CMD_NAV_RETURN_TO_LAUNCH;
    QString error;
    CaacSimulationEngine engine;
    const CaacSimulationEngine::Options options{.horizontalSpeed = 10, .verticalSpeed = 10, .turnMode = 3};
    QVERIFY(engine.load({{0, first}, {1, second}, {2, jump}, {3, returning}}, home, options, error));
    int passes = 0;
    int previous = -1;
    for (int tick = 0; tick < 300 && !engine.state().finished; ++tick) {
        engine.advance(0.1);
        if (engine.state().sequence == 1 && previous != 1) {
            ++passes;
        }
        previous = engine.state().sequence;
    }
    QCOMPARE(passes, 2);
    QVERIFY(engine.state().finished && !engine.state().airborne);
    QVERIFY(QGCGeo::geodesicDistance(engine.state().position, home) < 0.01);
    QCOMPARE(engine.state().position.altitude(), home.altitude());
    jump.param2 = -1;
    QVERIFY(engine.load({{0, first}, {1, second}, {2, jump}}, home, options, error));
    for (int tick = 0; tick < 500; ++tick) {
        engine.advance(0.1);
    }
    QVERIFY(!engine.state().finished);
    const auto position = engine.state().position;
    engine.pause();
    QCOMPARE(engine.state().position, position);
    QCOMPARE(engine.state().northSpeed, 0.0);
    QCOMPARE(engine.state().eastSpeed, 0.0);
    QVERIFY(engine.jump(1));
    QVERIFY(!engine.jump(100));
}

void CaacSimulationEngineTest::_windComponents()
{
    const QGeoCoordinate home(30, 120, 20);
    QString error;
    CaacSimulationEngine engine;
    QVERIFY(engine.load({{0, waypoint(0, QGCGeo::geodesicDestination(home, 45, 100), 2)}}, home,
                        {.horizontalSpeed = 10, .verticalSpeed = 10, .windSpeed = 5, .windBearing = 270, .turnMode = 3},
                        error));
    auto state = engine.state();
    state.position.setAltitude(22);
    state.airborne = true;
    engine.restoreState(state);
    engine.advance(1);
    QVERIFY(std::abs(engine.state().northSpeed - 10 / std::sqrt(2.0)) < 0.02);
    QVERIFY(std::abs(engine.state().eastSpeed - (10 / std::sqrt(2.0) - 5)) < 0.02);
}

void CaacSimulationEngineTest::_rejectInvalidMissionAtomically()
{
    const QGeoCoordinate home(30, 120, 20);
    QString error;
    CaacSimulationEngine engine;
    QVERIFY(engine.load({{0, waypoint(0, home, 2)}}, home, {}, error));
    const auto before = engine.state().position;
    auto invalid = waypoint(0, home, 2);
    invalid.command = MAV_CMD_DO_JUMP;
    invalid.param1 = std::numeric_limits<float>::max();
    QVERIFY(!engine.load({{0, invalid}}, home, {}, error));
    QCOMPARE(engine.state().position, before);
    invalid = waypoint(0, home, 2);
    invalid.frame = MAV_FRAME_GLOBAL_TERRAIN_ALT;
    QVERIFY(!engine.load({{0, invalid}}, home, {}, error));
    invalid.command = MAV_CMD_DO_SET_ROI_LOCATION;
    QVERIFY(!engine.load({{0, invalid}}, home, {}, error));
    QVERIFY(!engine.load({}, home, {}, error));
    QVERIFY(!engine.load({{0, waypoint(0, home, 2)}}, home, {.verticalSpeed = 0}, error));
}

void CaacSimulationEngineTest::_settingsHistoryAndFile()
{
    CaacRouteEditorController editor;
    editor.setPlanMasterController(planController());
    missionController()->setHomePosition(QGeoCoordinate(30, 120, 20));
    QVERIFY(editor.addWaypoint(QGeoCoordinate(30.001, 120)));
    const auto before = planController()->saveToJson();
    editor.clearHistory();
    auto settings = CaacSimulationController::defaults();
    settings["loopMode"] = 1;
    settings["loopCount"] = 3;
    settings["turnMode"] = 3;
    settings["climbMode"] = 0;
    settings["closed"] = true;
    QVERIFY(editor.setSimulationSettings(settings));
    const auto after = planController()->saveToJson();
    QVERIFY(before != after);
    QVERIFY(editor.undo());
    QCOMPARE(planController()->saveToJson(), before);
    QVERIFY(editor.redo());
    QCOMPARE(editor.simulationSettings(), settings);
    auto invalid = settings;
    invalid["verticalSpeed"] = 0;
    QVERIFY(!editor.setSimulationSettings(invalid));
    QCOMPARE(planController()->saveToJson(), after);
    invalid["verticalSpeed"] = std::numeric_limits<double>::quiet_NaN();
    QVERIFY(!editor.setSimulationSettings(invalid));
    QTemporaryDir directory;
    QVERIFY(directory.isValid());
    const auto path = directory.filePath("simulation.plan");
    QVERIFY(planController()->saveToFile(path));
    planController()->removeAll();
    planController()->loadFromFile(path);
    QCOMPARE(editor.simulationSettings(), settings);
    QCOMPARE(planController()->saveToJson(), after);
}

UT_REGISTER_TEST(CaacSimulationEngineTest, TestLabel::Unit, TestLabel::MissionManager)
#include "CaacSimulationEngineTest.moc"
