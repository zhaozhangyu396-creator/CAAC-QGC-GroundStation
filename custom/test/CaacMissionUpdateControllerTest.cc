#include <QtCore/QJsonArray>
#include <QtCore/QRegularExpression>
#include <QtCore/QTemporaryDir>

#include "CaacMissionUpdateController.h"
#include "CaacRouteEditorController.h"
#include "Fact.h"
#include "FirmwarePlugin.h"
#include "MissionController.h"
#include "MissionItem.h"
#include "MissionManager.h"
#include "MissionSettingsItem.h"
#include "MissionTest.h"
#include "MockLinkMissionItemHandler.h"
#include "PlanMasterController.h"
#include "SimpleMissionItem.h"
#include "Vehicle.h"

class CaacMissionUpdateControllerTest : public MissionTest
{
    Q_OBJECT
private slots:
    void _connectedHomeAltitude();
    void _uploadConfirmation_data();
    void _uploadConfirmation();
    void _update_data();
    void _update();
};

void CaacMissionUpdateControllerTest::_connectedHomeAltitude()
{
    QVERIFY(vehicle());
    QVERIFY(waitForInitialConnect());
    QTRY_VERIFY_WITH_TIMEOUT(vehicle()->homePosition().isValid(), TestTimeout::mediumMs());
    PlanMasterController plan;
    plan.setFlyView(false);
    plan.start();
    QTRY_COMPARE_WITH_TIMEOUT(plan.managerVehicle(), vehicle(), TestTimeout::mediumMs());
    QTRY_VERIFY_WITH_TIMEOUT(!plan.syncInProgress(), TestTimeout::mediumMs());
    auto* mission = plan.missionController();
    mission->removeAll();
    CaacRouteEditorController editor;
    editor.setPlanMasterController(&plan);
    const auto reported = vehicle()->homePosition();
    QTRY_COMPARE_WITH_TIMEOUT(mission->plannedHomePosition(), reported, TestTimeout::shortMs());

    auto planned = reported.atDistanceAndAzimuth(30, 0);
    planned.setAltitude(0);
    QVERIFY(editor.updatePlannedHome(planned.latitude(), planned.longitude(), 0, false));
    QTRY_COMPARE_WITH_TIMEOUT(mission->plannedHomePosition().altitude(), reported.altitude(), TestTimeout::shortMs());
    QCOMPARE(mission->plannedHomePosition().latitude(), planned.latitude());
    QCOMPARE(mission->plannedHomePosition().longitude(), planned.longitude());
    QVERIFY(editor.addWaypoint(reported.atDistanceAndAzimuth(40, 90)));
    const auto serialized = plan.saveToJson()
                                .object()
                                .value(QStringLiteral("mission"))
                                .toObject()
                                .value(QStringLiteral("plannedHomePosition"))
                                .toArray();
    QCOMPARE(serialized.at(2).toDouble(), reported.altitude());
    QTemporaryDir directory;
    QVERIFY(directory.isValid());
    const auto file = directory.filePath(QStringLiteral("connected-home.plan"));
    QVERIFY(plan.saveToFile(file));
    plan.loadFromFile(file);
    QCOMPARE(mission->plannedHomePosition().altitude(), reported.altitude());

    auto* home = mission->visualItems()->value<MissionSettingsItem*>(0);
    QVERIFY(home);
    const QList<double> terrain{reported.altitude() + 25};
    QVERIFY(QMetaObject::invokeMethod(home, "_terrainDataReceived", Qt::DirectConnection, Q_ARG(bool, true),
                                      Q_ARG(QList<double>, terrain)));
    QCOMPARE(mission->plannedHomePosition().altitude(), reported.altitude());
    QVERIFY(editor.updatePlannedHome(planned.latitude(), planned.longitude(), 0));
    QVERIFY(plan.saveToFile(file));
    plan.loadFromFile(file);
    vehicle()->homePositionChanged(reported);
    QTRY_VERIFY_WITH_TIMEOUT(mission->visualItems()->get(0)->property("caacHomeAltitudeManual").toBool(),
                             TestTimeout::shortMs());
    QCOMPARE(mission->plannedHomePosition().altitude(), 0.0);
    QVERIFY(editor.updatePlannedHome(planned.latitude(), planned.longitude(), reported.altitude() + 10));
    vehicle()->homePositionChanged(reported);
    QCOMPARE(mission->plannedHomePosition().altitude(), reported.altitude() + 10);

    mission->removeAll();
    auto remote = reported.atDistanceAndAzimuth(1000, 90);
    remote.setAltitude(0);
    mission->setHomePosition(remote);
    vehicle()->homePositionChanged(reported);
    QCOMPARE(mission->plannedHomePosition(), remote);
    editor.setPlanMasterController(nullptr);
}

void CaacMissionUpdateControllerTest::_uploadConfirmation_data()
{
    QTest::addColumn<bool>("editDuringUpload");
    QTest::newRow("acknowledged") << false;
    QTest::newRow("edited-and-saved-before-ack") << true;
}

void CaacMissionUpdateControllerTest::_uploadConfirmation()
{
    QFETCH(bool, editDuringUpload);
    QVERIFY(vehicle());
    QVERIFY(waitForInitialConnect());
    QTRY_VERIFY_WITH_TIMEOUT(vehicle()->initialPlanRequestComplete(), TestTimeout::mediumMs());
    QTRY_VERIFY_WITH_TIMEOUT(vehicle()->homePosition().isValid(), TestTimeout::mediumMs());
    PlanMasterController plan;
    plan.setFlyView(false);
    plan.start();
    QTRY_COMPARE_WITH_TIMEOUT(plan.managerVehicle(), vehicle(), TestTimeout::mediumMs());
    QTRY_VERIFY_WITH_TIMEOUT(!plan.syncInProgress(), TestTimeout::mediumMs());
    auto* mission = plan.missionController();
    mission->removeAll();
    mission->setHomePosition(QGeoCoordinate(30, 120, 20));
    auto* waypoint = mission->insertSimpleMissionItem(QGeoCoordinate(30.001, 120), 1, false);
    QVERIFY(waypoint);
    mission->setDirty(false);
    QVERIFY(!mission->uploadConfirmed());
    QSignalSpy sent(vehicle()->missionManager(), &MissionManager::sendComplete);
    mission->sendToVehicle();
    QVERIFY(!mission->uploadConfirmed());
    if (editDuringUpload) {
        waypoint->setCoordinate(QGeoCoordinate(30.002, 120));
        mission->setDirty(false);
    }
    QTRY_COMPARE_WITH_TIMEOUT(sent.count(), 1, TestTimeout::longMs());
    QVERIFY(!sent.last()[0].toBool());
    QVERIFY(mission->visualItemIndexForObject(waypoint) >= 1);
    QCOMPARE(mission->uploadConfirmed(), !editDuringUpload);
    waypoint->setCoordinate(QGeoCoordinate(30.003, 120));
    QVERIFY(!mission->uploadConfirmed());
}

void CaacMissionUpdateControllerTest::_update_data()
{
    QTest::addColumn<bool>("transferFailure");
    QTest::addColumn<bool>("structuralChange");
    QTest::addColumn<bool>("landingPoint");
    QTest::addColumn<bool>("initiallyPaused");
    QTest::newRow("position-update") << false << false << false << false;
    QTest::newRow("landing-position-update") << false << false << true << false;
    QTest::newRow("paused-position-update-resumes") << false << false << false << true;
    QTest::newRow("paused-landing-position-update-resumes") << false << false << true << true;
    QTest::newRow("transfer-failure-stays-paused") << true << false << false << false;
    QTest::newRow("structural-change-stays-paused") << false << true << false << false;
}

void CaacMissionUpdateControllerTest::_update()
{
    QFETCH(bool, transferFailure);
    QFETCH(bool, structuralChange);
    QFETCH(bool, landingPoint);
    QFETCH(bool, initiallyPaused);
    QVERIFY(vehicle());
    QVERIFY(waitForInitialConnect());
    QTRY_VERIFY_WITH_TIMEOUT(vehicle()->homePosition().isValid(), TestTimeout::mediumMs());
    QTRY_VERIFY_WITH_TIMEOUT(!planController()->syncInProgress(), TestTimeout::mediumMs());
    clearMission();
    missionController()->setHomePosition(QGeoCoordinate(30, 120, 20));
    for (int i = 1; i <= 3; ++i) {
        QVERIFY(missionController()->insertSimpleMissionItem(QGeoCoordinate(30 + i * 0.001, 120), i, false));
    }
    if (landingPoint) {
        auto* land = missionController()->visualItems()->value<SimpleMissionItem*>(3);
        QVERIFY(land);
        land->missionItem().setCommand(MAV_CMD_NAV_LAND);
        land->altitude()->setRawValue(0);
    }
    auto* manager = vehicle()->missionManager();
    QSignalSpy transferErrors(manager, &MissionManager::error);
    QSignalSpy sent(manager, &MissionManager::sendComplete);
    QVERIFY(!missionController()->uploadConfirmed());
    missionController()->sendToVehicle();
    QVERIFY(!missionController()->uploadConfirmed());
    QTRY_COMPARE_WITH_TIMEOUT(sent.count(), 1, TestTimeout::longMs());
    QVERIFY(!sent.last()[0].toBool());
    QVERIFY(missionController()->uploadConfirmed());
    vehicle()->setArmed(true, true);
    QTRY_VERIFY_WITH_TIMEOUT(vehicle()->armed(), TestTimeout::mediumMs());
    vehicle()->setFlightMode(vehicle()->missionFlightMode());
    QTRY_COMPARE_WITH_TIMEOUT(vehicle()->flightMode(), vehicle()->missionFlightMode(), TestTimeout::mediumMs());
    if (initiallyPaused) {
        vehicle()->pauseVehicle();
        QTRY_COMPARE_WITH_TIMEOUT(vehicle()->flightMode(), vehicle()->firmwarePlugin()->pauseFlightMode(),
                                  TestTimeout::mediumMs());
    }
    const auto reportTarget = [&](int sequence) {
        mavlink_message_t report{};
        mavlink_msg_mission_current_pack(vehicle()->id(), vehicle()->defaultComponentId(), &report, sequence,
                                         manager->missionItems().size(), MISSION_STATE_ACTIVE, 1, 0, 0, 0);
        return QMetaObject::invokeMethod(manager, "_mavlinkMessageReceived", Qt::DirectConnection,
                                         Q_ARG(mavlink_message_t, report));
    };
    QVERIFY(reportTarget(1));
    QCOMPARE(manager->currentIndex(), 1);
    const int changedIndex = landingPoint ? 3 : 2;
    auto* waypoint = missionController()->visualItems()->value<SimpleMissionItem*>(changedIndex);
    QVERIFY(waypoint);
    waypoint->setCoordinate(QGeoCoordinate(30.0025, 120.0005));
    QVERIFY(!missionController()->uploadConfirmed());
    if (structuralChange) {
        QVERIFY(missionController()->insertSimpleMissionItem(QGeoCoordinate(30.004, 120), 4, false));
    }
    if (transferFailure) {
        expectLogMessage("API.QGCApplication.AppMessage", QtDebugMsg,
                         QRegularExpression(QStringLiteral("任务传输失败|Mission transfer failed")));
        mockLink()->setMissionItemFailureMode(MockLinkMissionItemHandler::FailWriteFinalAckErrorAck, MAV_MISSION_ERROR);
    }
    CaacMissionUpdateController update;
    QSignalSpy finished(&update, &CaacMissionUpdateController::finished);
    bool pausedBeforeTransfer = false;
    connect(manager, &MissionManager::inProgressChanged, this, [&](bool inProgress) {
        if (inProgress) {
            pausedBeforeTransfer = vehicle()->flightMode() == vehicle()->firmwarePlugin()->pauseFlightMode();
        }
    });
    QVERIFY(update.update(planController(), nullptr));
    if (!transferFailure && !structuralChange) {
        mavlink_message_t command{};
        QTRY_VERIFY_WITH_TIMEOUT(
            mockLink()->lastReceivedMavlinkMessage(MAVLINK_MSG_ID_MISSION_SET_CURRENT, command) || !finished.isEmpty(),
            TestTimeout::mediumMs());
        QVERIFY2(finished.isEmpty(), finished.isEmpty() ? "" : qPrintable(finished.last()[1].toString()));
        mavlink_mission_set_current_t target{};
        mavlink_msg_mission_set_current_decode(&command, &target);
        QCOMPARE(target.seq, 1);
        QCOMPARE(vehicle()->flightMode(), vehicle()->firmwarePlugin()->pauseFlightMode());
        QVERIFY(update.busy());
        QVERIFY(reportTarget(target.seq));
    }
    QTRY_VERIFY_WITH_TIMEOUT(!finished.isEmpty(), TestTimeout::longMs());
    if (transferFailure) {
        QVERIFY2(pausedBeforeTransfer, qPrintable(finished.last()[1].toString()));
        QVERIFY(!transferErrors.isEmpty());
        QVERIFY(finished.last()[1].toString().contains(transferErrors.last()[1].toString()));
        verifyExpectedLogMessage();
    }
    QCOMPARE(finished.last()[0].toBool(), !transferFailure && !structuralChange);
    QCOMPARE(missionController()->uploadConfirmed(), !transferFailure && !structuralChange);
    QVERIFY(!update.busy());
    if (transferFailure || structuralChange) {
        QCOMPARE(vehicle()->flightMode(), vehicle()->firmwarePlugin()->pauseFlightMode());
        QCOMPARE(sent.count(), structuralChange ? 1 : 2);
    } else {
        QVERIFY2(pausedBeforeTransfer, qPrintable(finished.last()[1].toString()));
        QCOMPARE(sent.count(), 2);
        QCOMPARE(vehicle()->flightMode(), vehicle()->missionFlightMode());
        const int offset = vehicle()->firmwarePlugin()->sendHomePositionToVehicle() ? 0 : 1;
        QVERIFY(manager->missionItems()[changedIndex - offset]->coordinate().distanceTo(waypoint->coordinate()) < 0.02);
    }
}

UT_REGISTER_TEST(CaacMissionUpdateControllerTest, TestLabel::Unit, TestLabel::MissionManager)
#include "CaacMissionUpdateControllerTest.moc"
