#include <cmath>

#include <QtCore/QJsonArray>
#include <QtCore/QMetaMethod>
#include <QtCore/QRegularExpression>
#include <QtCore/QScopeGuard>
#include <QtCore/QTemporaryDir>

#include "CaacMissionUpdateController.h"
#include "CaacMissionUpdateUtils.h"
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
#include "SpeedSection.h"
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
    void _speedMapping();
    void _speedUpdate_data();
    void _speedUpdate();
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

void CaacMissionUpdateControllerTest::_speedMapping()
{
    using namespace CaacMissionUpdateUtils;
    const auto nav = [](int sequence) {
        mavlink_mission_item_int_t item{};
        item.seq = sequence;
        item.command = MAV_CMD_NAV_WAYPOINT;
        item.frame = MAV_FRAME_GLOBAL_RELATIVE_ALT;
        item.autocontinue = true;
        item.x = 300000000 + sequence * 1000;
        item.y = 1200000000;
        item.z = 30;
        return item;
    };
    const auto speed = [](int sequence, float value) {
        mavlink_mission_item_int_t item{};
        item.seq = sequence;
        item.command = MAV_CMD_DO_CHANGE_SPEED;
        item.frame = MAV_FRAME_MISSION;
        item.autocontinue = true;
        item.param1 = 1;
        item.param2 = value;
        item.param3 = -1;
        return item;
    };
    const auto jump = [](int sequence, int target) {
        mavlink_mission_item_int_t item{};
        item.seq = sequence;
        item.command = MAV_CMD_DO_JUMP;
        item.frame = MAV_FRAME_MISSION;
        item.autocontinue = true;
        item.param1 = target;
        item.param2 = 3;
        return item;
    };
    const Mission previous{{0, nav(0)}, {1, speed(1, 5)}, {2, nav(2)}, {3, nav(3)}, {4, jump(4, 1)}};
    auto moved = previous.value(2);
    moved.seq = 3;
    auto last = previous.value(3);
    last.seq = 5;
    auto first = previous.value(0);
    first.seq = 1;
    const Mission changed{{0, speed(0, 6)}, {1, first}, {2, speed(2, 8)}, {3, moved},
                          {4, speed(4, 6)}, {5, last},  {6, jump(6, 2)}};
    const auto mapping = speedUpdateMapping(previous, changed);
    QVERIFY(mapping.valid);
    QCOMPARE(mapping.oldToNew.value(0), 1);
    QCOMPARE(mapping.oldToNew.value(1), 3);
    QCOMPARE(mapping.oldToNew.value(2), 3);
    QCOMPARE(mapping.oldToNew.value(3), 5);
    QCOMPARE(mapping.oldToNew.value(4), 6);
    QCOMPARE(mapping.jumpTargetToNew.value(1), 2);
    QCOMPARE(mapping.newToOld.value(4), 3);

    auto invalid = changed;
    invalid[6].param1 = 0;
    QVERIFY(!speedUpdateMapping(previous, invalid).valid);
    QVERIFY(speedUpdateMapping(previous, invalid, false).valid);
    invalid = changed;
    invalid[3].z += 1;
    QVERIFY(!speedUpdateMapping(previous, invalid).valid);
    invalid = changed;
    invalid[4].param4 = 1;
    QVERIFY(!speedUpdateMapping(previous, invalid).valid);
    invalid = changed;
    invalid[4].param1 = 2;
    QVERIFY(!speedUpdateMapping(previous, invalid).valid);

    auto secondWithoutSpeed = previous.value(2);
    secondWithoutSpeed.seq = 1;
    auto thirdWithoutSpeed = previous.value(3);
    thirdWithoutSpeed.seq = 2;
    const Mission removed{{0, previous.value(0)}, {1, secondWithoutSpeed}, {2, thirdWithoutSpeed}, {3, jump(3, 1)}};
    const auto removedMapping = speedUpdateMapping(previous, removed);
    QVERIFY(removedMapping.valid);
    QCOMPARE(removedMapping.oldToNew.value(3), 2);
    QCOMPARE(removedMapping.jumpTargetToNew.value(1), 1);
}

void CaacMissionUpdateControllerTest::_speedUpdate_data()
{
    QTest::addColumn<bool>("structuralChange");
    QTest::addColumn<bool>("futureOnly");
    QTest::addColumn<bool>("forceCurrent");
    QTest::addColumn<bool>("downloadLoop");
    QTest::addColumn<int>("expectationMode");
    QTest::newRow("insert-speeds-preserves-later-target") << false << false << false << false << 0;
    QTest::newRow("future-speed-keeps-current-override") << false << true << false << false << 0;
    QTest::newRow("explicit-current-speed-replaces-override") << false << false << true << false << 1;
    QTest::newRow("selected-target-advances-while-pausing") << false << false << true << false << 2;
    QTest::newRow("all-waypoints-force-survives-target-advance") << false << false << true << false << 3;
    QTest::newRow("downloaded-px4-loop-preserves-target-and-progress") << false << false << false << true << 0;
    QTest::newRow("unrelated-structure-change-stays-paused") << true << false << false << false << 0;
}

void CaacMissionUpdateControllerTest::_speedUpdate()
{
    QFETCH(bool, structuralChange);
    QFETCH(bool, futureOnly);
    QFETCH(bool, forceCurrent);
    QFETCH(bool, downloadLoop);
    QFETCH(int, expectationMode);
    QVERIFY(vehicle());
    QVERIFY(waitForInitialConnect());
    QTRY_VERIFY_WITH_TIMEOUT(vehicle()->homePosition().isValid(), TestTimeout::mediumMs());
    QTRY_VERIFY_WITH_TIMEOUT(!planController()->syncInProgress(), TestTimeout::mediumMs());
    clearMission();
    auto* mission = missionController();
    mission->setHomePosition(QGeoCoordinate(30, 120, 20));
    for (int i = 1; i <= 3; ++i) {
        QVERIFY(mission->insertSimpleMissionItem(QGeoCoordinate(30 + i * 0.001, 120), i, false));
    }
    auto* first = mission->visualItems()->value<SimpleMissionItem*>(1);
    auto* third = mission->visualItems()->value<SimpleMissionItem*>(3);
    QVERIFY(first && third);
    QVERIFY(first->speedSection() && first->speedSection()->available());
    QVERIFY(third->speedSection() && third->speedSection()->available());
    if (forceCurrent) {
        first->speedSection()->flightSpeed()->setRawValue(5);
        first->speedSection()->setSpecifyFlightSpeed(true);
    }
    if (downloadLoop) {
        auto* jump = qobject_cast<SimpleMissionItem*>(mission->insertSimpleMissionItem(QGeoCoordinate(), 4, false));
        QVERIFY(jump);
        jump->setCommand(MAV_CMD_DO_JUMP);
        jump->missionItem().setParam1(first->sequenceNumber());
        jump->missionItem().setParam2(2);
    }
    auto* manager = vehicle()->missionManager();
    QSignalSpy sent(manager, &MissionManager::sendComplete);
    mission->sendToVehicle();
    QTRY_COMPARE_WITH_TIMEOUT(sent.count(), 1, TestTimeout::longMs());
    QVERIFY(!sent.last()[0].toBool());
    const int offset = vehicle()->firmwarePlugin()->sendHomePositionToVehicle() ? 0 : 1;
    if (downloadLoop) {
        QCOMPARE(offset, 1);
        QCOMPARE(manager->missionItems().last()->command(), MAV_CMD_DO_JUMP);
        QCOMPARE(manager->missionItems().last()->param1(), 0.0);
        QSignalSpy downloaded(mission, &MissionController::newItemsFromVehicle);
        mission->loadFromVehicle();
        QTRY_COMPARE_WITH_TIMEOUT(downloaded.count(), 1, TestTimeout::longMs());
        QTRY_VERIFY_WITH_TIMEOUT(!planController()->syncInProgress(), TestTimeout::mediumMs());
        QCOMPARE(manager->missionItems().last()->param1(), 1.0);
        QCOMPARE(manager->missionItems().last()->sequenceNumber(), 3);
    }
    // The static active-vehicle fixture uses fly view, which rebuilds visual items after every upload.
    first = mission->visualItems()->value<SimpleMissionItem*>(1);
    third = mission->visualItems()->value<SimpleMissionItem*>(3);
    QVERIFY(first && third);
    QCOMPARE(mission->visualItemIndexForObject(first), 1);
    QCOMPARE(mission->visualItemIndexForObject(third), 3);
    QVERIFY(first->speedSection() && first->speedSection()->available());
    QVERIFY(third->speedSection() && third->speedSection()->available());
    vehicle()->setArmed(true, true);
    QTRY_VERIFY_WITH_TIMEOUT(vehicle()->armed(), TestTimeout::mediumMs());
    vehicle()->setFlightMode(vehicle()->missionFlightMode());
    QTRY_COMPARE_WITH_TIMEOUT(vehicle()->flightMode(), vehicle()->missionFlightMode(), TestTimeout::mediumMs());
    const bool advanceWhilePausing = expectationMode == 2 || expectationMode == 3;
    auto* initialTarget = advanceWhilePausing ? mission->visualItems()->value<SimpleMissionItem*>(2) : third;
    QVERIFY(initialTarget);
    const int previousTarget = initialTarget->sequenceNumber() - offset;
    const int expectedCurrentSequence = expectationMode == 1 || expectationMode == 2 ? previousTarget + offset : -1;
    const auto reportTarget = [&](int sequence) {
        mavlink_message_t report{};
        mavlink_msg_mission_current_pack(vehicle()->id(), vehicle()->defaultComponentId(), &report, sequence,
                                         manager->missionItems().size(), MISSION_STATE_ACTIVE, 1, 0, 0, 0);
        return QMetaObject::invokeMethod(manager, "_mavlinkMessageReceived", Qt::DirectConnection,
                                         Q_ARG(mavlink_message_t, report));
    };
    if (downloadLoop) {
        QVERIFY(reportTarget(manager->missionItems().last()->sequenceNumber()));
        QVERIFY(reportTarget(0));
        QCOMPARE(manager->remainingLoops(), 2);
    }
    QVERIFY(reportTarget(previousTarget));
    QCOMPARE(manager->currentIndex(), previousTarget);
    QList<mavlink_command_long_t> speedCommands;
    mavlink_message_t receiveMessage{};
    mavlink_status_t receiveStatus{};
    QObject speedResponder;
    auto* link = mockLink();
    const auto queuedWriteSignal = QMetaMethod::fromSignal(&MockLink::writeBytesQueuedSignal);
    const int queuedWriteIndex = link->metaObject()->indexOfSlot("_writeBytesQueued(QByteArray)");
    QVERIFY(queuedWriteIndex >= 0);
    const auto queuedWriteSlot = link->metaObject()->method(queuedWriteIndex);
    QVERIFY(QObject::disconnect(link, &MockLink::writeBytesQueuedSignal, link, nullptr));
    const auto restoreWrites = qScopeGuard([&]() {
        QObject::disconnect(link, &MockLink::writeBytesQueuedSignal, &speedResponder, nullptr);
        QObject::connect(link, queuedWriteSignal, link, queuedWriteSlot, Qt::QueuedConnection);
    });
    // The generic MockLink rejects speed changes; supply just that aircraft response without changing its mission path.
    connect(
        link, &MockLink::writeBytesQueuedSignal, &speedResponder,
        [&](const QByteArray& bytes) {
            for (const char byte : bytes) {
                mavlink_message_t message{};
                mavlink_status_t status{};
                if (mavlink_frame_char_buffer(&receiveMessage, &receiveStatus, static_cast<uint8_t>(byte), &message,
                                              &status) != MAVLINK_FRAMING_OK) {
                    continue;
                }
                mavlink_command_long_t command{};
                if (message.msgid == MAVLINK_MSG_ID_COMMAND_LONG) {
                    mavlink_msg_command_long_decode(&message, &command);
                }
                if (message.msgid == MAVLINK_MSG_ID_COMMAND_LONG && command.command == MAV_CMD_DO_CHANGE_SPEED) {
                    speedCommands.append(command);
                    const bool valid = command.target_system == vehicle()->id() &&
                                       command.target_component == vehicle()->defaultComponentId() &&
                                       command.param1 == 1 && command.param4 == 0 && std::isfinite(command.param2) &&
                                       command.param2 > 0 && command.param2 <= 100;
                    link->sendUnexpectedCommandAck(MAV_CMD_DO_CHANGE_SPEED,
                                                   valid ? MAV_RESULT_ACCEPTED : MAV_RESULT_DENIED);
                } else {
                    uint8_t buffer[MAVLINK_MAX_PACKET_LEN]{};
                    const int length = mavlink_msg_to_send_buffer(buffer, &message);
                    QVERIFY(QMetaObject::invokeMethod(
                        link, "_writeBytesQueued", Qt::QueuedConnection,
                        Q_ARG(QByteArray, QByteArray(reinterpret_cast<char*>(buffer), length))));
                }
            }
        },
        Qt::QueuedConnection);
    vehicle()->guidedModeChangeGroundSpeedMetersSecond(8);
    QTRY_COMPARE_WITH_TIMEOUT(vehicle()->commandedFlightSpeed(), 8.0, TestTimeout::mediumMs());
    if (forceCurrent) {
        QCOMPARE(first->speedSection()->flightSpeed()->rawValue().toDouble(), 5.0);
    } else if (futureOnly) {
        third->speedSection()->flightSpeed()->setRawValue(9);
        third->speedSection()->setSpecifyFlightSpeed(true);
    } else {
        auto* settings = mission->visualItems()->value<MissionSettingsItem*>(0);
        QVERIFY(settings && settings->speedSection());
        settings->speedSection()->flightSpeed()->setRawValue(6);
        settings->speedSection()->setSpecifyFlightSpeed(true);
        first->speedSection()->flightSpeed()->setRawValue(7);
        first->speedSection()->setSpecifyFlightSpeed(true);
        QTRY_VERIFY_WITH_TIMEOUT(third->sequenceNumber() - offset > previousTarget, TestTimeout::mediumMs());
        if (downloadLoop) {
            auto* jump = mission->visualItems()->value<SimpleMissionItem*>(4);
            QVERIFY(jump && jump->command() == MAV_CMD_DO_JUMP);
            jump->missionItem().setParam1(first->sequenceNumber());
        }
    }
    if (structuralChange) {
        QVERIFY(mission->insertSimpleMissionItem(QGeoCoordinate(30.004, 120), mission->visualItems()->count(), false));
    }
    const int expectedTarget = third->sequenceNumber() - offset;
    const auto expectedTargetCoordinate = third->coordinate();
    const int expectedLoopTarget = first->sequenceNumber() - offset;
    CaacMissionUpdateController update;
    QSignalSpy finished(&update, &CaacMissionUpdateController::finished);
    QSignalSpy speedResults(vehicle(), &Vehicle::mavCommandResult);
    bool speedAcceptedBeforeResume = false;
    connect(vehicle(), &Vehicle::flightModeChanged, &update, [&](const QString& mode) {
        if (mode != vehicle()->missionFlightMode()) {
            return;
        }
        for (const auto& result : speedResults) {
            if (result[2].toInt() == MAV_CMD_DO_CHANGE_SPEED && result[3].toInt() == MAV_RESULT_ACCEPTED) {
                speedAcceptedBeforeResume = true;
            }
        }
    });
    bool pausedBeforeTransfer = false;
    connect(manager, &MissionManager::inProgressChanged, &update, [&](bool inProgress) {
        if (inProgress) {
            pausedBeforeTransfer = vehicle()->flightMode() == vehicle()->firmwarePlugin()->pauseFlightMode();
        }
    });
    speedCommands.clear();
    QVERIFY(update.updateSpeeds(planController(), nullptr, forceCurrent ? 5.0 : qQNaN(), expectedCurrentSequence));
    if (advanceWhilePausing) {
        // Advance before the pause acknowledgement, while the controller has not captured its restore target.
        QVERIFY(reportTarget(expectedTarget));
    }
    if (!structuralChange) {
        mavlink_message_t command{};
        QTRY_VERIFY_WITH_TIMEOUT(
            mockLink()->lastReceivedMavlinkMessage(MAVLINK_MSG_ID_MISSION_SET_CURRENT, command) || !finished.isEmpty(),
            TestTimeout::mediumMs());
        QVERIFY2(finished.isEmpty(), finished.isEmpty() ? "" : qPrintable(finished.last()[1].toString()));
        mavlink_mission_set_current_t target{};
        mavlink_msg_mission_set_current_decode(&command, &target);
        QCOMPARE(target.seq, expectedTarget);
        QCOMPARE(vehicle()->flightMode(), vehicle()->firmwarePlugin()->pauseFlightMode());
        QCOMPARE(speedCommands.count(), 0);
        QVERIFY(update.busy());
        QVERIFY(reportTarget(target.seq));
    }
    QTRY_VERIFY_WITH_TIMEOUT(!finished.isEmpty(), TestTimeout::longMs());
    QCOMPARE(finished.last()[0].toBool(), !structuralChange);
    QVERIFY(!update.busy());
    if (structuralChange) {
        QCOMPARE(sent.count(), 1);
        QCOMPARE(vehicle()->flightMode(), vehicle()->firmwarePlugin()->pauseFlightMode());
        QCOMPARE(speedCommands.count(), 0);
    } else {
        QVERIFY(pausedBeforeTransfer);
        QCOMPARE(sent.count(), 2);
        QCOMPARE(vehicle()->flightMode(), vehicle()->missionFlightMode());
        QCOMPARE(speedCommands.count(), 1);
        const double expectedSpeed = expectationMode == 2 || futureOnly ? 8.0 : forceCurrent ? 5.0 : 7.0;
        QCOMPARE(speedCommands.last().param2, static_cast<float>(expectedSpeed));
        QCOMPARE(vehicle()->commandedFlightSpeed(), expectedSpeed);
        QVERIFY(speedAcceptedBeforeResume);
        QCOMPARE(manager->currentIndex(), expectedTarget);
        QVERIFY(manager->missionItems()[expectedTarget]->coordinate().distanceTo(expectedTargetCoordinate) < 0.02);
        if (downloadLoop) {
            QCOMPARE(manager->remainingLoops(), 2);
            QCOMPARE(manager->missionItems().last()->command(), MAV_CMD_DO_JUMP);
            QCOMPARE(manager->missionItems().last()->param1(), static_cast<double>(expectedLoopTarget));
            QCOMPARE(manager->missionItems().last()->param2(), 1.0);
        }
    }
}

UT_REGISTER_TEST(CaacMissionUpdateControllerTest, TestLabel::Unit, TestLabel::MissionManager)
#include "CaacMissionUpdateControllerTest.moc"
