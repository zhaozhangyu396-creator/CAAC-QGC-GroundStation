#include <QtCore/QList>
#include <QtCore/QRegularExpression>
#include <QtCore/QStringList>
#include <QtCore/QTemporaryDir>
#include <QtTest/QSignalSpy>

#include "CaacCorePlugin.h"
#include "Fact.h"
#include "FlightPathSegment.h"
#include "MissionController.h"
#include "MissionItem.h"
#include "MissionManager.h"
#include "MissionSettingsItem.h"
#include "MissionTest.h"
#include "MockLinkMissionItemHandler.h"
#include "PlanMasterController.h"
#include "SimpleMissionItem.h"
#include "SpeedSection.h"
#include "Vehicle.h"

class CaacRouteUploadStateTest : public MissionTest
{
    Q_OBJECT

private slots:
    void _localEditsRetainAcknowledgedLegs();
    void _vehicleChangeClearsAcknowledgedLegs();
    void _wireLoopAndSpeedTypesStayAcknowledged();
    void _flightAltitudeUpdatesRetainAcknowledgedLegs_data();
    void _flightAltitudeUpdatesRetainAcknowledgedLegs();

private:
    void _preparePlan(PlanMasterController& plan);
    static bool _statesMatch(MissionController* mission, const QList<bool>& expected);
    static QString _stateText(MissionController* mission);
};

void CaacRouteUploadStateTest::_preparePlan(PlanMasterController& plan)
{
    QVERIFY(vehicle());
    QVERIFY(waitForInitialConnect());
    QTRY_VERIFY_WITH_TIMEOUT(vehicle()->initialPlanRequestComplete(), TestTimeout::mediumMs());
    plan.setFlyView(false);
    plan.start();
    QTRY_COMPARE_WITH_TIMEOUT(plan.managerVehicle(), vehicle(), TestTimeout::mediumMs());
    QTRY_VERIFY_WITH_TIMEOUT(!plan.syncInProgress(), TestTimeout::mediumMs());
    auto* mission = plan.missionController();
    mission->removeAll();
    mission->setHomePosition(QGeoCoordinate(30, 120, 20));
    auto* settings = mission->visualItems()->value<MissionSettingsItem*>(0);
    QVERIFY(settings && settings->speedSection());
    settings->speedSection()->flightSpeed()->setRawValue(5);
    settings->speedSection()->setSpecifyFlightSpeed(true);
    for (int i = 1; i <= 4; ++i) {
        auto* item = qobject_cast<SimpleMissionItem*>(
            mission->insertSimpleMissionItem(QGeoCoordinate(30 + i * 0.001, 120), i, false));
        QVERIFY(item);
        item->setAltitudeFrame(QGroundControlQmlGlobal::AltitudeFrameRelative);
        item->altitude()->setRawValue(30);
    }
    QTRY_COMPARE_WITH_TIMEOUT(mission->simpleFlightPathSegments()->count(), 4, TestTimeout::mediumMs());
}

bool CaacRouteUploadStateTest::_statesMatch(MissionController* mission, const QList<bool>& expected)
{
    const auto* segments = mission ? mission->simpleFlightPathSegments() : nullptr;
    if (!segments || segments->count() != expected.size()) {
        return false;
    }
    for (int i = 0; i < expected.size(); ++i) {
        const auto* segment = segments->value<FlightPathSegment*>(i);
        if (!segment || segment->uploaded() != expected.at(i)) {
            return false;
        }
    }
    return true;
}

QString CaacRouteUploadStateTest::_stateText(MissionController* mission)
{
    QStringList states;
    auto* segments = mission->simpleFlightPathSegments();
    for (int i = 0; i < segments->count(); ++i) {
        const auto* segment = segments->value<FlightPathSegment*>(i);
        states.append(segment && segment->uploaded() ? QStringLiteral("green") : QStringLiteral("orange"));
    }
    return states.join(QLatin1Char(','));
}

void CaacRouteUploadStateTest::_localEditsRetainAcknowledgedLegs()
{
    PlanMasterController plan;
    _preparePlan(plan);
    if (QTest::currentTestFailed()) {
        return;
    }
    auto* mission = plan.missionController();
    auto* manager = vehicle()->missionManager();
    QSignalSpy sent(manager, &MissionManager::sendComplete);
    QVERIFY(_statesMatch(mission, {false, false, false, false}));
    mission->sendToVehicle();
    QTRY_COMPARE_WITH_TIMEOUT(sent.count(), 1, TestTimeout::longMs());
    QVERIFY(!sent.last().first().toBool());
    QTRY_VERIFY2_WITH_TIMEOUT(_statesMatch(mission, {true, true, true, true}), qPrintable(_stateText(mission)),
                              TestTimeout::mediumMs());

    auto* originalThird = mission->visualItems()->value<SimpleMissionItem*>(3);
    QVERIFY(originalThird);
    auto* appended = qobject_cast<SimpleMissionItem*>(
        mission->insertSimpleMissionItem(QGeoCoordinate(30.005, 120), mission->visualItems()->count(), false));
    QVERIFY(appended);
    appended->altitude()->setRawValue(30);
    QTRY_VERIFY2_WITH_TIMEOUT(_statesMatch(mission, {true, true, true, true, false}), qPrintable(_stateText(mission)),
                              TestTimeout::mediumMs());
    auto* inserted =
        qobject_cast<SimpleMissionItem*>(mission->insertSimpleMissionItem(QGeoCoordinate(30.0015, 120.0003), 2, false));
    QVERIFY(inserted);
    inserted->altitude()->setRawValue(30);
    QTRY_VERIFY2_WITH_TIMEOUT(_statesMatch(mission, {true, false, false, true, true, false}),
                              qPrintable(_stateText(mission)), TestTimeout::mediumMs());
    auto* originalSecond = mission->visualItems()->value<SimpleMissionItem*>(3);
    QVERIFY(originalSecond);
    QVERIFY(mission->isRouteSegmentUploaded(originalSecond->coordinate(), originalThird->coordinate(), 5));
    QVERIFY(!mission->isRouteSegmentUploaded(originalSecond->coordinate(), originalThird->coordinate(), 8));
    QVERIFY(!mission->isRouteSegmentUploaded(originalThird->coordinate(), originalSecond->coordinate(), 5));

    mission->setDirty(false);
    QTemporaryDir directory;
    QVERIFY(directory.isValid());
    QVERIFY(plan.saveToFile(directory.filePath(QStringLiteral("local-edits.plan"))));
    QVERIFY(_statesMatch(mission, {true, false, false, true, true, false}));

    expectLogMessage("API.QGCApplication.AppMessage", QtDebugMsg,
                     QRegularExpression(QStringLiteral("任务传输失败|Mission transfer failed")));
    mockLink()->setMissionItemFailureMode(MockLinkMissionItemHandler::FailWriteFinalAckErrorAck, MAV_MISSION_ERROR);
    mission->sendToVehicle();
    QTRY_COMPARE_WITH_TIMEOUT(sent.count(), 2, TestTimeout::longMs());
    QVERIFY(sent.last().first().toBool());
    verifyExpectedLogMessage();
    QVERIFY(_statesMatch(mission, {true, false, false, true, true, false}));

    mockLink()->setMissionItemFailureMode(MockLinkMissionItemHandler::FailNone, MAV_MISSION_ACCEPTED);
    mission->sendToVehicle();
    originalThird->altitude()->setRawValue(40);
    mission->setDirty(false);
    QTRY_COMPARE_WITH_TIMEOUT(sent.count(), 3, TestTimeout::longMs());
    QVERIFY(!sent.last().first().toBool());
    QTRY_VERIFY2_WITH_TIMEOUT(_statesMatch(mission, {true, true, true, false, false, true}),
                              qPrintable(_stateText(mission)), TestTimeout::mediumMs());
    bool transmittedPreviousAltitude = false;
    for (const auto* item : manager->missionItems()) {
        if (item->command() == MAV_CMD_NAV_WAYPOINT &&
            item->coordinate().distanceTo(originalThird->coordinate()) < 0.02) {
            QCOMPARE(item->param7(), 30.0);
            transmittedPreviousAltitude = true;
        }
    }
    QVERIFY(transmittedPreviousAltitude);

    mission->sendToVehicle();
    QTRY_COMPARE_WITH_TIMEOUT(sent.count(), 4, TestTimeout::longMs());
    QVERIFY(!sent.last().first().toBool());
    QTRY_VERIFY2_WITH_TIMEOUT(_statesMatch(mission, {true, true, true, true, true, true}),
                              qPrintable(_stateText(mission)), TestTimeout::mediumMs());
    const double unchangedAmsl = originalThird->amslEntryAlt();
    originalThird->setAltitudeFrame(QGroundControlQmlGlobal::AltitudeFrameAbsolute);
    originalThird->altitude()->setRawValue(unchangedAmsl);
    QCOMPARE(originalThird->amslEntryAlt(), unchangedAmsl);
    QTRY_VERIFY2_WITH_TIMEOUT(_statesMatch(mission, {true, true, true, false, false, true}),
                              qPrintable(_stateText(mission)), TestTimeout::mediumMs());

    auto* originalFirst = mission->visualItems()->value<SimpleMissionItem*>(1);
    QVERIFY(originalFirst);
    const auto firstCoordinate = originalFirst->coordinate();
    originalFirst->setCoordinate(firstCoordinate.atDistanceAndAzimuth(0.05, 90));
    QTRY_VERIFY2_WITH_TIMEOUT(_statesMatch(mission, {false, false, true, false, false, true}),
                              qPrintable(_stateText(mission)), TestTimeout::mediumMs());
}

void CaacRouteUploadStateTest::_vehicleChangeClearsAcknowledgedLegs()
{
    PlanMasterController plan;
    _preparePlan(plan);
    if (QTest::currentTestFailed()) {
        return;
    }
    auto* mission = plan.missionController();
    const auto home = mission->plannedHomePosition();
    const auto first = mission->visualItems()->value<SimpleMissionItem*>(1)->coordinate();
    QSignalSpy sent(vehicle()->missionManager(), &MissionManager::sendComplete);
    mission->sendToVehicle();
    QTRY_COMPARE_WITH_TIMEOUT(sent.count(), 1, TestTimeout::longMs());
    QVERIFY(!sent.last().first().toBool());
    QTRY_VERIFY_WITH_TIMEOUT(_statesMatch(mission, {true, true, true, true}), TestTimeout::mediumMs());
    QVERIFY(mission->isRouteSegmentUploaded(home, first, 5));
    mission->setDirty(true);
    _disconnectMockLink();
    if (QTest::currentTestFailed()) {
        return;
    }
    QTRY_VERIFY_WITH_TIMEOUT(plan.offline(), TestTimeout::mediumMs());
    QVERIFY(!mission->isRouteSegmentUploaded(home, first, 5));
    QTRY_VERIFY_WITH_TIMEOUT(_statesMatch(mission, {false, false, false, false}), TestTimeout::mediumMs());
    _connectMockLink(MAV_AUTOPILOT_PX4);
    if (QTest::currentTestFailed()) {
        return;
    }
    QTRY_COMPARE_WITH_TIMEOUT(plan.managerVehicle(), vehicle(), TestTimeout::mediumMs());
    QVERIFY(!mission->isRouteSegmentUploaded(home, first, 5));
    QTRY_VERIFY_WITH_TIMEOUT(_statesMatch(mission, {false, false, false, false}), TestTimeout::mediumMs());
}

void CaacRouteUploadStateTest::_wireLoopAndSpeedTypesStayAcknowledged()
{
    PlanMasterController plan;
    _preparePlan(plan);
    if (QTest::currentTestFailed()) {
        return;
    }
    auto* mission = plan.missionController();
    auto* manager = vehicle()->missionManager();
    QVERIFY(vehicle()->multiRotor());
    auto* first = mission->visualItems()->value<SimpleMissionItem*>(1);
    auto* second = mission->visualItems()->value<SimpleMissionItem*>(2);
    auto* fourth = mission->visualItems()->value<SimpleMissionItem*>(4);
    QVERIFY(first && second && fourth && first->speedSection() && fourth->speedSection());
    const auto firstCoordinate = first->coordinate();
    const auto secondCoordinate = second->coordinate();
    const auto fourthCoordinate = fourth->coordinate();
    first->speedSection()->flightSpeed()->setRawValue(7);
    first->speedSection()->setSpecifyFlightSpeed(true);
    // A jump into the first point's speed section must restore 7 rather than inherit 9.
    fourth->speedSection()->flightSpeed()->setRawValue(9);
    fourth->speedSection()->setSpecifyFlightSpeed(true);
    auto* oppositeSpeed =
        qobject_cast<SimpleMissionItem*>(mission->insertSimpleMissionItem(QGeoCoordinate(), 2, false));
    QVERIFY(oppositeSpeed);
    oppositeSpeed->setCommand(MAV_CMD_DO_CHANGE_SPEED);
    oppositeSpeed->missionItem().setFrame(MAV_FRAME_MISSION);
    oppositeSpeed->missionItem().setParam1(0);
    oppositeSpeed->missionItem().setParam2(8);
    oppositeSpeed->missionItem().setParam3(-1);
    oppositeSpeed->missionItem().setParam4(0);
    oppositeSpeed->missionItem().setParam5(0);
    oppositeSpeed->missionItem().setParam6(0);
    oppositeSpeed->missionItem().setParam7(0);
    auto* jump = qobject_cast<SimpleMissionItem*>(
        mission->insertSimpleMissionItem(QGeoCoordinate(), mission->visualItems()->count(), false));
    QVERIFY(jump);
    jump->setCommand(MAV_CMD_DO_JUMP);
    jump->missionItem().setParam1(first->sequenceNumber() + 1);
    jump->missionItem().setParam2(1);
    const auto speedsMatch = [mission](const QList<double>& expected) {
        auto* segments = mission->simpleFlightPathSegments();
        if (segments->count() != expected.size()) {
            return false;
        }
        for (int i = 0; i < expected.size(); ++i) {
            const auto* segment = segments->value<FlightPathSegment*>(i);
            if (!segment || segment->flightSpeed() != expected.at(i)) {
                return false;
            }
        }
        return true;
    };
    QTRY_VERIFY_WITH_TIMEOUT(speedsMatch({5, 7, 7, 7}), TestTimeout::mediumMs());
    QSignalSpy sent(manager, &MissionManager::sendComplete);
    QVERIFY(sent.isValid());
    mission->sendToVehicle();
    QTRY_COMPARE_WITH_TIMEOUT(sent.count(), 1, TestTimeout::longMs());
    QVERIFY(!sent.first().first().toBool());
    QVERIFY(!manager->missionItemsAreDownloaded());
    QTRY_VERIFY2_WITH_TIMEOUT(_statesMatch(mission, {true, true, true, true}), qPrintable(_stateText(mission)),
                              TestTimeout::mediumMs());
    QVERIFY(mission->isRouteSegmentUploaded(fourthCoordinate, secondCoordinate, 7));
    QVERIFY(!mission->isRouteSegmentUploaded(fourthCoordinate, secondCoordinate, 8));
    QVERIFY(!mission->isRouteSegmentUploaded(fourthCoordinate, secondCoordinate, 9));

    const auto uploadedHome = mission->plannedHomePosition();
    const auto movedHome = uploadedHome.atDistanceAndAzimuth(200, 90);
    mission->setHomePosition(movedHome);
    mission->setDirty(true);
    QTRY_VERIFY2_WITH_TIMEOUT(_statesMatch(mission, {false, true, true, true}), qPrintable(_stateText(mission)),
                              TestTimeout::mediumMs());
    QSignalSpy loaded(manager, &MissionManager::newMissionItemsAvailable);
    QVERIFY(loaded.isValid());
    manager->loadFromVehicle();
    QTRY_COMPARE_WITH_TIMEOUT(loaded.count(), 1, TestTimeout::longMs());
    QVERIFY(manager->missionItemsAreDownloaded());
    QCOMPARE(mission->plannedHomePosition(), movedHome);
    QTRY_VERIFY_WITH_TIMEOUT(speedsMatch({5, 7, 7, 7}), TestTimeout::mediumMs());
    QTRY_VERIFY2_WITH_TIMEOUT(_statesMatch(mission, {false, true, true, true}), qPrintable(_stateText(mission)),
                              TestTimeout::mediumMs());
    QVERIFY(!mission->isRouteSegmentUploaded(movedHome, firstCoordinate, 5));
    mission->setHomePosition(uploadedHome);
    QTRY_VERIFY2_WITH_TIMEOUT(_statesMatch(mission, {true, true, true, true}), qPrintable(_stateText(mission)),
                              TestTimeout::mediumMs());
    QVERIFY(mission->isRouteSegmentUploaded(fourthCoordinate, secondCoordinate, 7));
    QVERIFY(!mission->isRouteSegmentUploaded(fourthCoordinate, secondCoordinate, 8));
    QVERIFY(!mission->isRouteSegmentUploaded(fourthCoordinate, secondCoordinate, 9));
    QVERIFY(mission->isRouteSegmentUploaded(firstCoordinate, secondCoordinate, 7));

    SimpleMissionItem* downloadedOppositeSpeed = nullptr;
    for (int i = 1; i < mission->visualItems()->count(); ++i) {
        auto* item = mission->visualItems()->value<SimpleMissionItem*>(i);
        if (item && item->command() == MAV_CMD_DO_CHANGE_SPEED && item->missionItem().param1() == 0 &&
            item->missionItem().param2() == 8) {
            QVERIFY(!downloadedOppositeSpeed);
            downloadedOppositeSpeed = item;
        }
    }
    QVERIFY(downloadedOppositeSpeed);
    downloadedOppositeSpeed->missionItem().setParam1(1);
    QTRY_VERIFY_WITH_TIMEOUT(speedsMatch({5, 8, 8, 8}), TestTimeout::mediumMs());
    QTRY_VERIFY2_WITH_TIMEOUT(_statesMatch(mission, {true, false, false, false}), qPrintable(_stateText(mission)),
                              TestTimeout::mediumMs());
    QVERIFY(!mission->isRouteSegmentUploaded(firstCoordinate, secondCoordinate, 8));
    QVERIFY(!mission->isRouteSegmentUploaded(fourthCoordinate, secondCoordinate, 8));
    QCOMPARE(sent.count(), 1);
    QVERIFY(manager->missionItemsAreDownloaded());
}

void CaacRouteUploadStateTest::_flightAltitudeUpdatesRetainAcknowledgedLegs_data()
{
    QTest::addColumn<bool>("all");
    QTest::addColumn<bool>("failed");
    QTest::addColumn<bool>("draft");
    QTest::newRow("selected-height") << false << false << false;
    QTest::newRow("all-heights") << true << false << false;
    QTest::newRow("rejected-upload") << false << true << false;
    QTest::newRow("preserve-local-height-draft") << true << false << true;
}

void CaacRouteUploadStateTest::_flightAltitudeUpdatesRetainAcknowledgedLegs()
{
    QFETCH(bool, all);
    QFETCH(bool, failed);
    QFETCH(bool, draft);
    PlanMasterController plan;
    _preparePlan(plan);
    if (QTest::currentTestFailed()) {
        return;
    }
    auto* mission = plan.missionController();
    auto* manager = vehicle()->missionManager();
    QSignalSpy sent(manager, &MissionManager::sendComplete);
    mission->sendToVehicle();
    QTRY_COMPARE_WITH_TIMEOUT(sent.count(), 1, TestTimeout::longMs());
    QVERIFY(!sent.last().first().toBool());
    QTRY_VERIFY2_WITH_TIMEOUT(_statesMatch(mission, {true, true, true, true}), qPrintable(_stateText(mission)),
                              TestTimeout::mediumMs());

    PlanMasterController flight;
    flight.setFlyView(true);
    flight.start();
    QTRY_COMPARE_WITH_TIMEOUT(flight.managerVehicle(), vehicle(), TestTimeout::mediumMs());
    QTRY_VERIFY_WITH_TIMEOUT(!flight.syncInProgress(), TestTimeout::mediumMs());
    flight.missionController()->showPlanFromManagerVehicle();
    QVariantList selected;
    for (int i = 1; i < flight.missionController()->visualItems()->count(); ++i) {
        auto* item = flight.missionController()->visualItems()->value<SimpleMissionItem*>(i);
        if (item && item->command() == MAV_CMD_NAV_WAYPOINT && (all || item->waypointNumber() == 2)) {
            selected.append(QVariant::fromValue(static_cast<QObject*>(item)));
        }
    }
    QCOMPARE(selected.size(), all ? 4 : 1);
    auto* localThird = mission->visualItems()->value<SimpleMissionItem*>(3);
    QVERIFY(localThird);
    if (draft) {
        localThird->altitude()->setRawValue(55);
        QVERIFY(mission->dirty());
    }
    if (failed) {
        expectLogMessage("API.QGCApplication.AppMessage", QtDebugMsg,
                         QRegularExpression(QStringLiteral("任务传输失败|Mission transfer failed")));
        mockLink()->setMissionItemFailureMode(MockLinkMissionItemHandler::FailWriteFinalAckErrorAck, MAV_MISSION_ERROR);
    }
    auto* plugin = qobject_cast<CaacCorePlugin*>(QGCCorePlugin::instance());
    QVERIFY(plugin);
    QVERIFY(plugin->updateFlightWaypointAltitudes(&flight, selected, 42, &plan).isEmpty());
    QVERIFY(!mission->uploadConfirmed());
    QTRY_COMPARE_WITH_TIMEOUT(sent.count(), 2, TestTimeout::longMs());
    QCOMPARE(sent.last().first().toBool(), failed);
    if (failed) {
        verifyExpectedLogMessage();
    }
    const QList<bool> expected = failed  ? QList<bool>{true, false, false, true}
                                 : draft ? QList<bool>{true, true, false, false}
                                         : QList<bool>{true, true, true, true};
    QTRY_VERIFY2_WITH_TIMEOUT(_statesMatch(mission, expected), qPrintable(_stateText(mission)),
                              TestTimeout::mediumMs());
    for (int i = 1; i <= 4; ++i) {
        auto* item = mission->visualItems()->value<SimpleMissionItem*>(i);
        QVERIFY(item);
        QCOMPARE(item->altitude()->rawValue().toDouble(), draft && i == 3 ? 55.0 : all || i == 2 ? 42.0 : 30.0);
    }
    if (draft || failed) {
        QVERIFY(mission->dirty());
        QVERIFY(!mission->uploadConfirmed());
    }
}

UT_REGISTER_TEST(CaacRouteUploadStateTest, TestLabel::Unit, TestLabel::MissionManager)

#include "CaacRouteUploadStateTest.moc"
