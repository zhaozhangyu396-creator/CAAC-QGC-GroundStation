#include <cmath>
#include <limits>

#include <QtCore/QJsonDocument>
#include <QtCore/QScopeGuard>
#include <QtCore/QTemporaryDir>
#include <QtGraphs/QLineSeries>
#include <QtTest/QSignalSpy>
#include <QtTest/QTest>

#include "CaacRouteEditorController.h"
#include "CaacRouteGeometry.h"
#include "FlightPathSegment.h"
#include "MissionController.h"
#include "MissionItem.h"
#include "MissionTest.h"
#include "PlanMasterController.h"
#include "QGCGeo.h"
#include "QmlObjectTreeModel.h"
#include "SimpleMissionItem.h"
#include "SpeedSection.h"
#include "TerrainProfile.h"

class CaacRouteEditorTest : public OfflineMissionTest
{
    Q_OBJECT

private slots:
    void _testInsertReorderDelete();
    void _testWaypointLabelsFollowSpeedCommands();
    void _testInvalidProperties_data();
    void _testInvalidProperties();
    void _testPlanRoundtrip();
    void _testOrderConstraints();
    void _testStaleItem();
    void _testUnknownTerrainProfile();
    void _testGeodesicGeometry_data();
    void _testGeodesicGeometry();
    void _testRelativeAngles();
    void _testRotatePreservesMission();
    void _testHistoryTransactions();
    void _testRelativeInsertion();
    void _testRelativeUpdate_data();
    void _testRelativeUpdate();
    void _testCenterFileRoundtrip();
    void _testWaypointLanding();
};

void CaacRouteEditorTest::_testWaypointLanding()
{
    CaacRouteEditorController editor;
    editor.setPlanMasterController(planController());
    missionController()->setHomePosition(QGeoCoordinate(30, 120, 20));
    QVERIFY(editor.addWaypoint(QGeoCoordinate(30.001, 120)));
    auto* first = editor.currentItem();
    QVERIFY(editor.addWaypoint(QGeoCoordinate(30.002, 120)));
    auto* second = editor.currentItem();
    const auto original = planController()->saveToJson();
    QVERIFY(editor.useWaypointAsLanding(first));
    auto* model = missionController()->visualItems();
    QCOMPARE(model->count(), 4);
    auto* landing = model->value<SimpleMissionItem*>(3);
    QVERIFY(landing && landing->isLandCommand());
    QVERIFY(landing->coordinate().distanceTo(first->coordinate()) < 0.01);
    QCOMPARE(landing->altitude()->rawValue().toDouble(), 0.0);
    const auto firstLanding = planController()->saveToJson();
    QVERIFY(editor.useWaypointAsLanding(second));
    QCOMPARE(model->count(), 4);
    landing = model->value<SimpleMissionItem*>(3);
    QVERIFY(landing && landing->isLandCommand());
    QVERIFY(landing->coordinate().distanceTo(second->coordinate()) < 0.01);
    QVERIFY(editor.undo());
    QCOMPARE(planController()->saveToJson(), firstLanding);
    QVERIFY(editor.undo());
    QCOMPARE(planController()->saveToJson(), original);
}

void CaacRouteEditorTest::_testWaypointLabelsFollowSpeedCommands()
{
    CaacRouteEditorController editor;
    editor.setPlanMasterController(planController());
    missionController()->setHomePosition(QGeoCoordinate(30, 120, 50));
    QVERIFY(editor.addWaypoint(QGeoCoordinate(30.001, 120)));
    auto* first = editor.currentItem();
    QVERIFY(editor.addWaypoint(QGeoCoordinate(30.002, 120)));
    auto* second = editor.currentItem();
    QVERIFY(first && second);
    const int originalNumber = second->sequenceNumber();
    const auto secondCoordinate = second->coordinate();
    QSignalSpy labelsChanged(&editor, &CaacRouteEditorController::coordinateItemsChanged);
    QVERIFY(labelsChanged.isValid());
    const auto label = [&editor]() { return editor.coordinateItems().at(2).toMap().value("label").toString(); };
    QString observedLabel = label();
    connect(&editor, &CaacRouteEditorController::coordinateItemsChanged, this, [&]() { observedLabel = label(); });
    QVERIFY(editor.updateWaypoint(first, 30.001, 120, 50, true, 5));
    QCOMPARE(second->sequenceNumber(), originalNumber + 1);
    QCOMPARE(second->waypointNumber(), 2);
    QVERIFY(!labelsChanged.isEmpty());
    QCOMPARE(observedLabel, QCoreApplication::translate("CaacRouteEditorController", "Waypoint %1 · %2")
                                .arg(second->waypointNumber())
                                .arg(second->commandName()));
    QCOMPARE(second->coordinate(), secondCoordinate);
    labelsChanged.clear();
    QVERIFY(editor.updateWaypoint(first, 30.001, 120, 50, false, 5));
    QCOMPARE(second->sequenceNumber(), originalNumber);
    QVERIFY(!labelsChanged.isEmpty());
    QCOMPARE(observedLabel, QCoreApplication::translate("CaacRouteEditorController", "Waypoint %1 · %2")
                                .arg(second->waypointNumber())
                                .arg(second->commandName()));
}

void CaacRouteEditorTest::_testGeodesicGeometry_data()
{
    QTest::addColumn<QGeoCoordinate>("center");
    QTest::addColumn<double>("angle");
    for (const auto& center :
         {QGeoCoordinate(0, 0), QGeoCoordinate(30, 120), QGeoCoordinate(85, 179.999), QGeoCoordinate(-80, -179.999)}) {
        for (double angle : {90.0, -90.0, 123.456, 360.0, -810.0}) {
            const QByteArray name =
                QStringLiteral("%1/%2/%3").arg(center.latitude()).arg(center.longitude()).arg(angle).toUtf8();
            QTest::newRow(name.constData()) << center << angle;
        }
    }
}

void CaacRouteEditorTest::_testGeodesicGeometry()
{
    QFETCH(QGeoCoordinate, center);
    QFETCH(double, angle);
    auto a = QGCGeo::geodesicDestination(center, 0, 100);
    a.setAltitude(123.45);
    const auto result = CaacRouteGeometry::rotate({center, a}, center, angle);
    QVERIFY(result);
    QCOMPARE(result->at(0), center);
    QCOMPARE(result->at(1).altitude(), 123.45);
    QVERIFY(std::abs(QGCGeo::geodesicDistance(center, result->at(1)) - 100.0) < 1e-6);
    QVERIFY(std::abs(std::remainder(QGCGeo::geodesicAzimuth(center, result->at(1)) - angle, 360.0)) < 1e-7);
    const auto restored = CaacRouteGeometry::rotate(*result, center, -angle);
    QVERIFY(restored);
    QVERIFY(QGCGeo::geodesicDistance(restored->at(1), a) < 1e-6);
    const auto b = QGCGeo::geodesicDestination(a, 37, 500);
    const auto alignedAngle = CaacRouteGeometry::alignmentAngle(a, b, center, 123);
    QVERIFY(alignedAngle);
    const auto aligned = CaacRouteGeometry::rotate({a, b}, center, *alignedAngle);
    QVERIFY(aligned);
    QVERIFY(std::abs(QGCGeo::geodesicAzimuth(aligned->at(0), aligned->at(1)) - 123) < 1e-7);
}

void CaacRouteEditorTest::_testRelativeAngles()
{
    const QGeoCoordinate origin(0, 0, 25);
    const auto north = CaacRouteGeometry::relativePoint(origin, 0, 100);
    const auto east = CaacRouteGeometry::relativePoint(origin, 90, 100);
    QVERIFY(north && east);
    // Independent WGS84 reference values for a 100 m meridian/equator segment.
    QVERIFY(std::abs(north->latitude() - 0.0009043694770496278) < 1e-12);
    QVERIFY(std::abs(east->longitude() - 0.0008983152841195215) < 1e-12);
    QCOMPARE(north->altitude(), 25.0);
    for (const auto& pair : {std::pair{90.0, 270.0}, std::pair{180.0, 0.0}, std::pair{-90.0, 90.0}}) {
        const auto point = CaacRouteGeometry::interiorAnglePoint(origin, *north, pair.first, 100);
        QVERIFY(point);
        QVERIFY(std::abs(std::remainder(QGCGeo::geodesicAzimuth(*north, *point) - pair.second, 360.0)) < 1e-7);
    }
    QVERIFY(!CaacRouteGeometry::relativePoint(origin, 0, -1));
    QVERIFY(!CaacRouteGeometry::relativePoint(origin, std::numeric_limits<double>::infinity(), 100));
    QVERIFY(!CaacRouteGeometry::interiorAnglePoint(origin, origin, 90, 100));
    QVERIFY(!CaacRouteGeometry::rotate({origin, QGeoCoordinate()}, origin, 90));
    QVERIFY(!CaacRouteGeometry::alignmentAngle(origin, origin, origin, 90));
}

void CaacRouteEditorTest::_testRotatePreservesMission()
{
    CaacRouteEditorController editor;
    editor.setPlanMasterController(planController());
    const QGeoCoordinate home(30, 120, 50);
    missionController()->setHomePosition(home);
    auto* takeoff = missionController()->insertTakeoffItem(QGeoCoordinate(30.001, 120), 1);
    QVERIFY(editor.addWaypoint(QGeoCoordinate(30.002, 120)));
    auto* point = editor.currentItem();
    QVERIFY(editor.updateWaypoint(point, 30.002, 120, 123.45, true, 9));
    point->missionItem().setParam1(7);
    point->missionItem().setParam2(3);
    point->missionItem().setParam4(42);
    auto* land =
        qobject_cast<SimpleMissionItem*>(missionController()->insertSimpleMissionItem(QGeoCoordinate(30.003, 120), -1));
    QVERIFY(land);
    land->setCommand(MAV_CMD_NAV_LAND);
    const auto original = planController()->saveToJson();
    const auto pivot = point->coordinate();
    const auto originalTakeoff = takeoff->coordinate();
    const auto originalLand = land->coordinate();
    const int command = point->command();
    const auto frame = point->altitudeFrame();
    QVERIFY(editor.rotateRouteAround(pivot, 90));
    QCOMPARE(point->coordinate(), pivot);
    QCOMPARE(point->command(), command);
    QCOMPARE(point->altitudeFrame(), frame);
    QCOMPARE(point->altitude()->rawValue().toDouble(), 123.45);
    QCOMPARE(point->speedSection()->specifiedFlightSpeed(), 9.0);
    QCOMPARE(point->missionItem().param1(), 7.0);
    QCOMPARE(point->missionItem().param2(), 3.0);
    QCOMPARE(point->missionItem().param4(), 42.0);
    QVERIFY(takeoff->coordinate().distanceTo(originalTakeoff) > 1);
    QVERIFY(land->coordinate().distanceTo(originalLand) > 1);
    QVERIFY(missionController()->plannedHomePosition().distanceTo(home) > 1);
    QVERIFY(editor.undo());
    QCOMPARE(planController()->saveToJson(), original);
    QVERIFY(editor.redo());
    const auto rotated = planController()->saveToJson();
    QVERIFY(!editor.rotateRouteAround(QGeoCoordinate(), 90));
    QCOMPARE(planController()->saveToJson(), rotated);
}

void CaacRouteEditorTest::_testHistoryTransactions()
{
    CaacRouteEditorController editor;
    editor.setPlanMasterController(planController());
    const auto empty = planController()->saveToJson();
    QVERIFY(editor.addWaypoint(QGeoCoordinate(30, 120)));
    const auto added = planController()->saveToJson();
    QVERIFY(editor.updateWaypoint(editor.currentItem(), 30, 120, 123, true, 9));
    const auto updated = planController()->saveToJson();
    editor.beginEdit();
    for (int i = 1; i <= 5; ++i) {
        editor.currentItem()->setCoordinate(QGeoCoordinate(30 + i * 0.001, 120));
        QCoreApplication::processEvents();
    }
    editor.endEdit();
    const auto dragged = planController()->saveToJson();
    QVERIFY(editor.undo());
    QCOMPARE(planController()->saveToJson(), updated);
    QVERIFY(editor.undo());
    QCOMPARE(planController()->saveToJson(), added);
    QVERIFY(editor.undo());
    QCOMPARE(planController()->saveToJson(), empty);
    QVERIFY(!editor.canUndo());
    QVERIFY(editor.redo());
    QVERIFY(editor.redo());
    QVERIFY(editor.redo());
    QCOMPARE(planController()->saveToJson(), dragged);
    QVERIFY(!editor.canRedo());
    QVERIFY(editor.undo());
    QVERIFY(editor.addWaypoint(QGeoCoordinate(31, 121)));
    QVERIFY(!editor.canRedo());
    // Native QGC property edits are captured even when the mission is already dirty.
    auto* item = editor.currentItem();
    const auto beforeNative = planController()->saveToJson();
    item->altitude()->setRawValue(140);
    QVERIFY(editor.undo());
    QCOMPARE(planController()->saveToJson(), beforeNative);
    QVERIFY(planController()->dirtyForSave());
}

void CaacRouteEditorTest::_testRelativeInsertion()
{
    CaacRouteEditorController editor;
    editor.setPlanMasterController(planController());
    missionController()->setHomePosition(QGeoCoordinate(0, 0, 25));
    const auto initial = planController()->saveToJson();
    QVERIFY(!editor.insertRelativeWaypoint(0, 0, 90, 100, true));
    QCOMPARE(planController()->saveToJson(), initial);
    QVERIFY(editor.insertRelativeWaypoint(0, 0, 0, 100, false));
    QVERIFY(editor.insertRelativeWaypoint(1, 1, 90, 100, true));
    auto* reference = missionController()->visualItems()->value<VisualMissionItem*>(1);
    auto* added = missionController()->visualItems()->value<VisualMissionItem*>(2);
    QVERIFY(std::abs(QGCGeo::geodesicAzimuth(reference->coordinate(), added->coordinate()) - 270) < 1e-7);
    const auto saved = planController()->saveToJson();
    QVERIFY(!editor.insertRelativeWaypoint(1, 999, 90, 100, false));
    QCOMPARE(planController()->saveToJson(), saved);
}

void CaacRouteEditorTest::_testRelativeUpdate_data()
{
    QTest::addColumn<double>("angle");
    QTest::addColumn<double>("bearing");
    QTest::newRow("left") << 90.0 << 270.0;
    QTest::newRow("straight") << 180.0 << 0.0;
    QTest::newRow("negative-right") << -90.0 << 90.0;
}

void CaacRouteEditorTest::_testRelativeUpdate()
{
    QFETCH(double, angle);
    QFETCH(double, bearing);
    CaacRouteEditorController editor;
    editor.setPlanMasterController(planController());
    missionController()->setHomePosition(QGeoCoordinate(0, 0, 25));
    QVERIFY(editor.insertRelativeWaypoint(0, 0, 0, 100, false));
    auto* previous = editor.currentItem();
    QVERIFY(!editor.waypointLeg(previous).contains(QStringLiteral("angle")));
    const auto firstPlan = planController()->saveToJson();
    QVERIFY(!editor.updateRelativeWaypoint(previous, angle, 150, true, 75, false, 0));
    QCOMPARE(planController()->saveToJson(), firstPlan);
    QVERIFY(editor.insertRelativeWaypoint(1, 1, 180, 100, true));
    auto* point = editor.currentItem();
    QVERIFY(
        editor.updateWaypoint(point, point->coordinate().latitude(), point->coordinate().longitude(), 123, true, 9));
    point->missionItem().setParam1(7);
    const auto original = planController()->saveToJson();
    const auto originalPrevious = previous->coordinate();
    const int number = point->sequenceNumber();
    const auto frame = point->altitudeFrame();
    QVERIFY(editor.updateRelativeWaypoint(point, angle, 150, true, 123, true, 9));
    QVERIFY(std::abs(QGCGeo::geodesicAzimuth(previous->coordinate(), point->coordinate()) - bearing) < 1e-7);
    const auto leg = editor.waypointLeg(point);
    QVERIFY(std::abs(leg.value(QStringLiteral("distance")).toDouble() - 150) < 1e-6);
    QVERIFY(std::abs(leg.value(QStringLiteral("angle")).toDouble() - CaacRouteGeometry::normalizedAngle(angle)) < 1e-7);
    QCOMPARE(previous->coordinate(), originalPrevious);
    QCOMPARE(point->sequenceNumber(), number);
    QCOMPARE(point->altitudeFrame(), frame);
    QCOMPARE(point->altitude()->rawValue().toDouble(), 123.0);
    QCOMPARE(point->speedSection()->specifiedFlightSpeed(), 9.0);
    QCOMPARE(point->missionItem().param1(), 7.0);
    const auto updated = planController()->saveToJson();
    QVERIFY(!editor.updateRelativeWaypoint(point, angle, -1, true, 123, true, 9));
    QVERIFY(!editor.updateRelativeWaypoint(point, angle, 150, true, std::numeric_limits<double>::quiet_NaN(), true, 9));
    QCOMPARE(planController()->saveToJson(), updated);
    QVERIFY(editor.undo());
    QCOMPARE(planController()->saveToJson(), original);
    QVERIFY(editor.redo());
    QCOMPARE(planController()->saveToJson(), updated);
    point = editor.currentItem();
    QVERIFY(editor.updateRelativeWaypoint(point, 45, 200, false, 123, true, 9));
    QVERIFY(std::abs(editor.waypointLeg(point).value(QStringLiteral("bearing")).toDouble() - 45) < 1e-7);
    point->setCoordinate(missionController()->visualItems()->value<VisualMissionItem*>(1)->coordinate());
    const auto coincident = editor.waypointLeg(point);
    QVERIFY(coincident.value(QStringLiteral("distance")).toDouble() < 0.001);
    QVERIFY(!coincident.contains(QStringLiteral("bearing")));
    QVERIFY(!coincident.contains(QStringLiteral("angle")));
}

void CaacRouteEditorTest::_testCenterFileRoundtrip()
{
    CaacRouteEditorController editor;
    editor.setPlanMasterController(planController());
    missionController()->setHomePosition(QGeoCoordinate(30, 120, 50));
    QVERIFY(editor.addWaypoint(QGeoCoordinate(30.001, 120)));
    QTemporaryDir directory;
    QVERIFY(directory.isValid());
    const auto legacyFile = directory.filePath(QStringLiteral("legacy.plan"));
    QVERIFY(planController()->saveToFile(legacyFile));
    const QGeoCoordinate center(30.1234567890123, 120.9876543210987);
    QVERIFY(editor.setRotationCenter(center));
    const auto file = directory.filePath(QStringLiteral("center.plan"));
    QVERIFY(planController()->saveToFile(file));
    const auto saved = planController()->saveToJson();
    planController()->removeAll();
    planController()->loadFromFile(file);
    QCOMPARE(editor.rotationCenter(), center);
    QCOMPARE(planController()->saveToJson(), saved);
    QVERIFY(!editor.canUndo());
    planController()->loadFromFile(legacyFile);
    QVERIFY(!editor.rotationCenter().isValid());
    QVERIFY(!editor.canUndo());
}

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
    QVERIFY(!editor.updateWaypoint(waypoint, 31, 121, 100, true, 9));
    QCOMPARE(planController()->saveToJson(), withJump);
}

void CaacRouteEditorTest::_testStaleItem()
{
    CaacRouteEditorController editor;
    QVERIFY(!editor.addWaypoint(QGeoCoordinate(30, 120)));
    PlanMasterController unstarted;
    editor.setPlanMasterController(&unstarted);
    QVERIFY(!editor.addWaypoint(QGeoCoordinate(30, 120)));
    editor.setPlanMasterController(planController());
    QVERIFY(editor.addWaypoint(QGeoCoordinate(30, 120)));
    QPointer<SimpleMissionItem> old = editor.currentItem();
    planController()->removeAll();
    QVERIFY(!editor.updateWaypoint(old, 31, 121, 100, false, 0));
    QVERIFY(editor.waypointLeg(old).isEmpty());
    QVERIFY(!editor.updateRelativeWaypoint(old, 90, 100, false, 100, false, 0));
    editor.setPlanMasterController(nullptr);
    QVERIFY(!editor.currentItem());
    QVERIFY(!editor.removeCurrentWaypoint());
}

void CaacRouteEditorTest::_testUnknownTerrainProfile()
{
    CaacRouteEditorController editor;
    editor.setPlanMasterController(planController());
    const QGeoCoordinate a(30, 120);
    const QGeoCoordinate b(30.001, 120);
    const QGeoCoordinate c(30.002, 120);
    QVERIFY(editor.addWaypoint(b));
    auto* home = missionController()->visualItems()->value<VisualMissionItem*>(0);
    auto* waypoint = editor.currentItem();
    QVERIFY(home && waypoint);
    const double nan = std::numeric_limits<double>::quiet_NaN();
    FlightPathSegment first(FlightPathSegment::SegmentTypeGeneric, a, nan, b, nan, false, nullptr);
    FlightPathSegment second(FlightPathSegment::SegmentTypeGeneric, b, nan, c, nan, false, nullptr);
    auto* originalHomeSegment = home->simpleFlightPathSegment();
    auto* originalWaypointSegment = waypoint->simpleFlightPathSegment();
    const auto restoreSegments = qScopeGuard([&] {
        home->setSimpleFlighPathSegment(originalHomeSegment);
        waypoint->setSimpleFlighPathSegment(originalWaypointSegment);
    });
    home->setSimpleFlighPathSegment(&first);
    waypoint->setSimpleFlighPathSegment(&second);
    const auto saved = planController()->saveToJson();
    TerrainProfile profile;
    profile.setMissionController(missionController());
    QVERIFY(profile.setProperty("minAMSLAlt", nan));
    QLineSeries terrain;
    QLineSeries flight;
    QLineSeries missing;
    QLineSeries collision;
    profile.updateSeries(&terrain, &flight, &missing, &collision);
    QCOMPARE(missing.count(), 5);
    // The renderer can break a line at an interior NaN, but its initial moveTo needs a finite point.
    QVERIFY(std::isfinite(missing.at(0).x()) && std::isfinite(missing.at(0).y()));
    QCOMPARE(missing.at(0), QPointF(0, 0));
    QCOMPARE(missing.at(1), QPointF(first.totalDistance(), 0));
    QVERIFY(std::isnan(missing.at(2).y()));
    QCOMPARE(missing.at(3), QPointF(first.totalDistance(), 0));
    QCOMPARE(missing.at(4), QPointF(first.totalDistance() + second.totalDistance(), 0));
    QVERIFY(terrain.points().isEmpty());
    QVERIFY(flight.points().isEmpty());
    QVERIFY(collision.points().isEmpty());
    QVERIFY(profile.setProperty("minAMSLAlt", -20.0));
    QVERIFY(profile.setProperty("verticalScale", 2.0));
    profile.updateSeries(&terrain, &flight, &missing, &collision);
    QCOMPARE(missing.at(0).y(), -40.0);
    QVERIFY(std::isnan(missing.at(2).y()));
    QCOMPARE(planController()->saveToJson(), saved);
}

UT_REGISTER_TEST(CaacRouteEditorTest, TestLabel::Unit, TestLabel::MissionManager)

#include "CaacRouteEditorTest.moc"
