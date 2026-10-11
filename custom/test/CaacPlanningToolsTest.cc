#include <cmath>
#include <limits>

#include <QtCore/QTemporaryDir>

#include "CaacCorePlugin.h"
#include "CaacRouteEditorController.h"
#include "CaacRouteTemplates.h"
#include "MissionController.h"
#include "MissionItem.h"
#include "MissionSettingsItem.h"
#include "MissionTest.h"
#include "PlanMasterController.h"
#include "QGCGeo.h"
#include "SimpleMissionItem.h"
#include "SpeedSection.h"

class CaacPlanningToolsTest : public OfflineMissionTest
{
    Q_OBJECT
private slots:
    void _templateTransactions();
    void _translationPreservesMission();
    void _invalidInputsAreAtomic();
    void _coordinateFormats();
    void _advancedWaypointProperties();
};

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

UT_REGISTER_TEST(CaacPlanningToolsTest, TestLabel::Unit, TestLabel::MissionManager)
UT_REGISTER_TEST(CaacRouteTemplateGeometryTest, TestLabel::Unit, TestLabel::MissionManager)

#include "CaacPlanningToolsTest.moc"
