#include <QtCore/QRegularExpression>

#include "CaacMissionUpdateController.h"
#include "FirmwarePlugin.h"
#include "MissionController.h"
#include "MissionItem.h"
#include "MissionManager.h"
#include "MissionTest.h"
#include "MockLinkMissionItemHandler.h"
#include "PlanMasterController.h"
#include "SimpleMissionItem.h"
#include "Vehicle.h"

class CaacMissionUpdateControllerTest : public MissionTest
{
    Q_OBJECT
private slots:
    void _update_data();
    void _update();
};

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
    missionController()->sendToVehicle();
    QTRY_COMPARE_WITH_TIMEOUT(sent.count(), 1, TestTimeout::longMs());
    QVERIFY(!sent.last()[0].toBool());
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
