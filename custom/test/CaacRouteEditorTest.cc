#include <limits>

#include <QtCore/QJsonDocument>
#include <QtCore/QTemporaryDir>
#include <QtTest/QTest>

#include "CaacRouteEditorController.h"
#include "MissionController.h"
#include "MissionItem.h"
#include "MissionTest.h"
#include "PlanMasterController.h"
#include "QmlObjectTreeModel.h"
#include "SimpleMissionItem.h"
#include "SpeedSection.h"

class CaacRouteEditorTest : public OfflineMissionTest
{
    Q_OBJECT

private slots:
    void _testInsertReorderDelete();
    void _testInvalidProperties_data();
    void _testInvalidProperties();
    void _testPlanRoundtrip();
    void _testOrderConstraints();
    void _testStaleItem();
};

void CaacRouteEditorTest::_testInsertReorderDelete()
{
    CaacRouteEditorController editor;
    editor.setPlanMasterController(planController());
    QVERIFY(editor.addWaypoint(QGeoCoordinate(30, 120)));
    auto* a = editor.currentItem();
    QVERIFY(a);
    QVERIFY(editor.updateWaypoint(a, 30, 120, 123, true, 9));
    QVERIFY(editor.addWaypoint(QGeoCoordinate(30.001, 120.001)));
    auto* b = editor.currentItem();
    QVERIFY(editor.selectWaypoint(a));
    QVERIFY(editor.addWaypoint(QGeoCoordinate(30.002, 120.002)));
    auto* c = editor.currentItem();
    auto* items = missionController()->visualItems();
    QCOMPARE(items->get(1), a);
    QCOMPARE(items->get(2), c);
    QCOMPARE(items->get(3), b);
    QVERIFY(editor.moveCurrentWaypoint(-1));
    QCOMPARE(items->get(1), c);
    QCOMPARE(items->get(2), a);
    QCOMPARE(a->altitude()->rawValue().toDouble(), 123.0);
    QCOMPARE(a->speedSection()->specifiedFlightSpeed(), 9.0);
    QCOMPARE(editor.currentItem(), c);
    for (int i = 1; i < items->count(); ++i) {
        const auto* previous = items->value<VisualMissionItem*>(i - 1);
        auto* item = items->value<VisualMissionItem*>(i);
        QCOMPARE(item->sequenceNumber(), previous->lastSequenceNumber() + 1);
        const auto* tree = missionController()->visualItemsTree();
        QCOMPARE(tree->indexForObject(item).row(), i);
    }
    QVERIFY(editor.removeCurrentWaypoint());
    QCOMPARE(items->count(), 3);
    QCOMPARE(items->get(1), a);
    QCOMPARE(items->get(2), b);
    QVERIFY(planController()->dirtyForSave());
}

void CaacRouteEditorTest::_testInvalidProperties_data()
{
    QTest::addColumn<double>("latitude");
    QTest::addColumn<double>("longitude");
    QTest::addColumn<double>("altitude");
    QTest::addColumn<double>("speed");
    const double nan = std::numeric_limits<double>::quiet_NaN();
    QTest::newRow("latitude") << 91.0 << 120.0 << 100.0 << 9.0;
    QTest::newRow("longitude") << 30.0 << 181.0 << 100.0 << 9.0;
    QTest::newRow("nan-coordinate") << nan << 120.0 << 100.0 << 9.0;
    QTest::newRow("nan-altitude") << 30.0 << 120.0 << nan << 9.0;
    QTest::newRow("infinite-altitude") << 30.0 << 120.0 << std::numeric_limits<double>::infinity() << 9.0;
    QTest::newRow("negative-speed") << 30.0 << 120.0 << 100.0 << -1.0;
    QTest::newRow("nan-speed") << 30.0 << 120.0 << 100.0 << nan;
}

void CaacRouteEditorTest::_testInvalidProperties()
{
    QFETCH(double, latitude);
    QFETCH(double, longitude);
    QFETCH(double, altitude);
    QFETCH(double, speed);
    CaacRouteEditorController editor;
    editor.setPlanMasterController(planController());
    QVERIFY(editor.addWaypoint(QGeoCoordinate(31, 121)));
    const auto before = planController()->saveToJson();
    QVERIFY(!editor.updateWaypoint(editor.currentItem(), latitude, longitude, altitude, true, speed));
    QVERIFY(!editor.lastError().isEmpty());
    QCOMPARE(planController()->saveToJson(), before);
}

void CaacRouteEditorTest::_testPlanRoundtrip()
{
    CaacRouteEditorController editor;
    editor.setPlanMasterController(planController());
    missionController()->setHomePosition(QGeoCoordinate(30, 120, 50));
    QVERIFY(editor.addWaypoint(QGeoCoordinate(30.001, 120.001)));
    auto* item = editor.currentItem();
    const auto frame = item->altitudeFrame();
    item->missionItem().setParam1(7);
    item->missionItem().setParam2(3);
    item->missionItem().setParam4(42);
    QVERIFY(editor.updateWaypoint(item, 30.12345678, 120.87654321, 123.45, true, 9));
    QCOMPARE(item->altitudeFrame(), frame);
    QCOMPARE(item->missionItem().param1(), 7.0);
    QCOMPARE(item->missionItem().param2(), 3.0);
    QCOMPARE(item->missionItem().param4(), 42.0);
    QTemporaryDir dir;
    QVERIFY(dir.isValid());
    const QString file = dir.filePath(QStringLiteral("exam.plan"));
    QVERIFY(planController()->saveToFile(file));
    const auto saved = planController()->saveToJson();
    planController()->removeAll();
    planController()->loadFromFile(file);
    QCOMPARE(planController()->saveToJson(), saved);
    const auto* loaded = missionController()->visualItems()->value<SimpleMissionItem*>(1);
    QVERIFY(loaded);
    QCOMPARE(loaded->coordinate().latitude(), 30.12345678);
    QCOMPARE(loaded->coordinate().longitude(), 120.87654321);
}

void CaacRouteEditorTest::_testOrderConstraints()
{
    CaacRouteEditorController editor;
    editor.setPlanMasterController(planController());
    QVERIFY(editor.addWaypoint(QGeoCoordinate(30, 120)));
    auto* waypoint = editor.currentItem();
    missionController()->insertTakeoffItem(QGeoCoordinate(30, 120), 1);
    QVERIFY(editor.selectWaypoint(waypoint));
    const auto before = planController()->saveToJson();
    QVERIFY(!editor.moveCurrentWaypoint(-1));
    QVERIFY(!missionController()->moveVisualItem(0, 1));
    QVERIFY(!missionController()->moveVisualItem(1, 2));
    QCOMPARE(planController()->saveToJson(), before);
    auto* jump = qobject_cast<SimpleMissionItem*>(missionController()->insertSimpleMissionItem({}, -1));
    QVERIFY(jump);
    jump->setCommand(MAV_CMD_DO_JUMP);
    const auto withJump = planController()->saveToJson();
    QVERIFY(!editor.addWaypoint(QGeoCoordinate(30.1, 120)));
    QVERIFY(!editor.removeCurrentWaypoint());
    QCOMPARE(planController()->saveToJson(), withJump);
}

void CaacRouteEditorTest::_testStaleItem()
{
    CaacRouteEditorController editor;
    QVERIFY(!editor.addWaypoint(QGeoCoordinate(30, 120)));
    editor.setPlanMasterController(planController());
    QVERIFY(editor.addWaypoint(QGeoCoordinate(30, 120)));
    QPointer<SimpleMissionItem> old = editor.currentItem();
    planController()->removeAll();
    QVERIFY(!editor.updateWaypoint(old, 31, 121, 100, false, 0));
    editor.setPlanMasterController(nullptr);
    QVERIFY(!editor.currentItem());
    QVERIFY(!editor.removeCurrentWaypoint());
}

UT_REGISTER_TEST(CaacRouteEditorTest, TestLabel::Unit, TestLabel::MissionManager)

#include "CaacRouteEditorTest.moc"
