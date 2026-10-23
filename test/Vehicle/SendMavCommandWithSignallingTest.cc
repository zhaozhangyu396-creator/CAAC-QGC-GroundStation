#include "SendMavCommandWithSignallingTest.h"

#include <limits>
#include <optional>

#include <QtCore/QElapsedTimer>
#include <QtCore/QRegularExpression>
#include <QtTest/QSignalSpy>

#include "Fixtures/RAIIFixtures.h"
#include "MavCommandQueue.h"
#include "MissionManager.h"
#include "MultiVehicleManager.h"

namespace {
bool _waitForExpectedCommandResult(QSignalSpy& spyResult, MAV_CMD expectedCommand, QList<QVariant>& arguments)
{
    const int commandResultTimeoutMs = TestTimeout::longMs();
    QElapsedTimer elapsedTimer;
    elapsedTimer.start();

    while (true) {
        const int elapsed = static_cast<int>(elapsedTimer.elapsed());
        const int remainingMs = commandResultTimeoutMs - elapsed;
        if (remainingMs <= 0) {
            return false;
        }

        if (spyResult.isEmpty() && !spyResult.wait(remainingMs)) {
            return false;
        }

        arguments = spyResult.takeFirst();
        if (arguments.count() == 5 && arguments.at(2).toInt() == expectedCommand) {
            return true;
        }

        qCDebug(UnitTestLog) << "Received response to command" << arguments.at(2).toInt() << ", ignoring, waiting for:" << expectedCommand;
    }
}
}

SendMavCommandWithSignallingTest::TestCase_t SendMavCommandWithSignallingTest::_rgTestCases[] = {
    {MockLink::MAV_CMD_MOCKLINK_ALWAYS_RESULT_ACCEPTED, MAV_RESULT_ACCEPTED, Vehicle::MavCmdResultCommandResultOnly, 1},
    {MockLink::MAV_CMD_MOCKLINK_ALWAYS_RESULT_FAILED, MAV_RESULT_FAILED, Vehicle::MavCmdResultCommandResultOnly, 1},
    {MockLink::MAV_CMD_MOCKLINK_SECOND_ATTEMPT_RESULT_ACCEPTED, MAV_RESULT_ACCEPTED,
     Vehicle::MavCmdResultCommandResultOnly, 2, true},
    {MockLink::MAV_CMD_MOCKLINK_SECOND_ATTEMPT_RESULT_FAILED, MAV_RESULT_FAILED, Vehicle::MavCmdResultCommandResultOnly,
     2, true},
    {MockLink::MAV_CMD_MOCKLINK_NO_RESPONSE, MAV_RESULT_FAILED, Vehicle::MavCmdResultFailureNoResponseToCommand,
     MavCommandQueue::kMaxRetryCount, true},
    {MockLink::MAV_CMD_MOCKLINK_NO_RESPONSE_NO_RETRY, MAV_RESULT_FAILED,
     Vehicle::MavCmdResultFailureNoResponseToCommand, 1, true},
    {MockLink::MAV_CMD_MOCKLINK_RESULT_IN_PROGRESS_ACCEPTED, MAV_RESULT_ACCEPTED,
     Vehicle::MavCmdResultCommandResultOnly, 1},
    {MockLink::MAV_CMD_MOCKLINK_RESULT_IN_PROGRESS_FAILED, MAV_RESULT_FAILED, Vehicle::MavCmdResultCommandResultOnly,
     1},
    {MockLink::MAV_CMD_MOCKLINK_RESULT_IN_PROGRESS_NO_ACK, MAV_RESULT_FAILED,
     Vehicle::MavCmdResultFailureNoResponseToCommand, 1, true},
};

void SendMavCommandWithSignallingTest::_testCaseWorker(TestCase_t& testCase)
{
    std::optional<TestFixtures::MavCommandAckTimeoutFixture> shortAckTimeout;
    if (testCase.expectAckTimeout) {
        shortAckTimeout.emplace();
    }

    MultiVehicleManager* vehicleMgr = MultiVehicleManager::instance();
    Vehicle* vehicle = vehicleMgr->activeVehicle();
    QSignalSpy spyResult(vehicle, &Vehicle::mavCommandResult);
    _mockLink->clearReceivedMavCommandCounts();
    vehicle->sendMavCommand(MAV_COMP_ID_AUTOPILOT1, testCase.command, true /* showError */);
    QList<QVariant> arguments;
    QVERIFY(_waitForExpectedCommandResult(spyResult, testCase.command, arguments));
    QCOMPARE(arguments.count(), 5);
    QCOMPARE(arguments.at(0).toInt(), vehicle->id());
    QCOMPARE(arguments.at(1).toInt(), MAV_COMP_ID_AUTOPILOT1);
    QCOMPARE(arguments.at(2).toInt(), testCase.command);
    QCOMPARE(arguments.at(3).toInt(), testCase.expectedCommandResult);
    QCOMPARE(arguments.at(4).value<Vehicle::MavCmdResultFailureCode_t>(), testCase.expectedFailureCode);
    QCOMPARE(vehicle->_findMavCommandListEntryIndex(MAV_COMP_ID_AUTOPILOT1,
                                                    MockLink::MAV_CMD_MOCKLINK_ALWAYS_RESULT_ACCEPTED),
             -1);
    QCOMPARE(_mockLink->receivedMavCommandCount(testCase.command), testCase.expectedSendCount);
}

void SendMavCommandWithSignallingTest::_performTestCases()
{
    // Some test cases send showError=true and get a failed result, producing showAppMessage debug logs.
    ignoreLogMessage("API.QGCApplication.AppMessage", QtDebugMsg,
                     QRegularExpression("command failed|did not respond to command"));
    // No-response test cases exhaust command retries, producing MavCommandQueue warnings.
    ignoreLogMessage("Vehicle.MavCommandQueue", QtWarningMsg,
                     QRegularExpression("Giving up sending command after max retries:"));
    int index = 0;
    for (TestCase_t& testCase : _rgTestCases) {
        qCDebug(UnitTestLog) << "Testing case" << index++;
        _testCaseWorker(testCase);
    }
}

void SendMavCommandWithSignallingTest::_duplicateCommand()
{
    // Sending a duplicate command produces a showAppMessage debug log.
    ignoreLogMessage("API.QGCApplication.AppMessage", QtDebugMsg,
                     QRegularExpression("Unable to send command"));
    MultiVehicleManager* vehicleMgr = MultiVehicleManager::instance();
    Vehicle* vehicle = vehicleMgr->activeVehicle();
    vehicle->sendMavCommand(MAV_COMP_ID_AUTOPILOT1, MockLink::MAV_CMD_MOCKLINK_NO_RESPONSE, true /* showError */);
    QVERIFY_TRUE_WAIT(_mockLink->receivedMavCommandCount(MockLink::MAV_CMD_MOCKLINK_NO_RESPONSE) == 1,
                      TestTimeout::shortMs());
    QSignalSpy spyResult(vehicle, &Vehicle::mavCommandResult);
    vehicle->sendMavCommand(MAV_COMP_ID_AUTOPILOT1, MockLink::MAV_CMD_MOCKLINK_NO_RESPONSE, true /* showError */);
    // Duplicate command returns immediately
    QCOMPARE(spyResult.count(), 1);
    QList<QVariant> arguments = spyResult.takeFirst();
    QCOMPARE(arguments.count(), 5);
    QCOMPARE(arguments.at(0).toInt(), vehicle->id());
    QCOMPARE(arguments.at(2).toInt(), (int)MockLink::MAV_CMD_MOCKLINK_NO_RESPONSE);
    QCOMPARE(arguments.at(3).toInt(), (int)MAV_RESULT_FAILED);
    QCOMPARE(arguments.at(4).value<Vehicle::MavCmdResultFailureCode_t>(), Vehicle::MavCmdResultFailureDuplicateCommand);
    QCOMPARE(_mockLink->receivedMavCommandCount(MockLink::MAV_CMD_MOCKLINK_NO_RESPONSE), 1);
    QVERIFY(vehicle->_findMavCommandListEntryIndex(MAV_COMP_ID_AUTOPILOT1, MockLink::MAV_CMD_MOCKLINK_NO_RESPONSE) !=
            -1);
}

void SendMavCommandWithSignallingTest::_acknowledgeSpeedCommand(int targetComponent, MAV_RESULT result)
{
    mavlink_message_t message{};
    message.compid = targetComponent;
    mavlink_command_ack_t ack{};
    ack.command = MAV_CMD_DO_CHANGE_SPEED;
    ack.result = result;
    _vehicle->_mavCmdQueue->handleCommandAck(message, ack);
}

void SendMavCommandWithSignallingTest::_commandedFlightSpeedConfirmation()
{
    // Control ACK delivery so an in-flight request and its rejected duplicate remain distinguishable.
    _mockLink->setCommLost(true);
    QSignalSpy changed(_vehicle, &Vehicle::commandedFlightSpeedChanged);
    QSignalSpy results(_vehicle, &Vehicle::mavCommandResult);
    QVERIFY(changed.isValid() && results.isValid());
    const int component = _vehicle->defaultComponentId();
    QVERIFY(qIsNaN(_vehicle->commandedFlightSpeed()));
    QCOMPARE(_vehicle->commandedFlightSpeedType(), -1);
    _vehicle->sendMavCommand(component, MAV_CMD_DO_CHANGE_SPEED, false, 1, 8, -1, 0);
    QVERIFY(qIsNaN(_vehicle->commandedFlightSpeed()));
    _acknowledgeSpeedCommand(component, MAV_RESULT_IN_PROGRESS);
    QVERIFY(qIsNaN(_vehicle->commandedFlightSpeed()));
    QCOMPARE(changed.count(), 0);
    QCOMPARE(results.count(), 0);

    _vehicle->sendMavCommand(component, MAV_CMD_DO_CHANGE_SPEED, false, 1, 9, -1, 0);
    QCOMPARE(results.count(), 1);
    QCOMPARE(results.last().at(4).toInt(), int(Vehicle::MavCmdResultFailureDuplicateCommand));
    QVERIFY(qIsNaN(_vehicle->commandedFlightSpeed()));
    _acknowledgeSpeedCommand(component, MAV_RESULT_ACCEPTED);
    QCOMPARE(_vehicle->commandedFlightSpeed(), 8.0);
    QCOMPARE(_vehicle->commandedFlightSpeedType(), 1);
    QCOMPARE(changed.count(), 1);
    QCOMPARE(results.count(), 2);
    QCOMPARE(results.last().at(3).toInt(), int(MAV_RESULT_ACCEPTED));

    _vehicle->sendMavCommand(component, MAV_CMD_DO_CHANGE_SPEED, false, 1, 10, -1, 0);
    _acknowledgeSpeedCommand(component, MAV_RESULT_DENIED);
    QCOMPARE(_vehicle->commandedFlightSpeed(), 8.0);
    QCOMPARE(changed.count(), 1);
    QCOMPARE(results.count(), 3);
    QCOMPARE(results.last().at(3).toInt(), int(MAV_RESULT_DENIED));

    _vehicle->sendMavCommand(component, MAV_CMD_DO_CHANGE_SPEED, false, 0, 8, -1, 0);
    _acknowledgeSpeedCommand(component, MAV_RESULT_ACCEPTED);
    QCOMPARE(_vehicle->commandedFlightSpeed(), 8.0);
    QCOMPARE(_vehicle->commandedFlightSpeedType(), 0);
    QCOMPARE(changed.count(), 2);

    _vehicle->sendMavCommand(component, MAV_CMD_DO_CHANGE_SPEED, false, 0, 2, -1, 1);
    _acknowledgeSpeedCommand(component, MAV_RESULT_DENIED);
    QCOMPARE(_vehicle->commandedFlightSpeed(), 8.0);
    QCOMPARE(changed.count(), 2);
    _vehicle->sendMavCommand(component, MAV_CMD_DO_CHANGE_SPEED, false, 0, -1, -1, 0);
    _acknowledgeSpeedCommand(component, MAV_RESULT_ACCEPTED);
    QCOMPARE(_vehicle->commandedFlightSpeed(), 8.0);
    QCOMPARE(changed.count(), 2);
    _vehicle->sendMavCommand(component, MAV_CMD_DO_CHANGE_SPEED, false, 0, -2, -1, 0);
    _acknowledgeSpeedCommand(component, MAV_RESULT_DENIED);
    QCOMPARE(_vehicle->commandedFlightSpeed(), 8.0);
    QCOMPARE(changed.count(), 2);
    _vehicle->sendMavCommand(component, MAV_CMD_DO_CHANGE_SPEED, false, 0, -2, -1, 0);
    _acknowledgeSpeedCommand(component, MAV_RESULT_ACCEPTED);
    QVERIFY(qIsNaN(_vehicle->commandedFlightSpeed()));
    QCOMPARE(_vehicle->commandedFlightSpeedType(), -1);
    QCOMPARE(changed.count(), 3);
}

void SendMavCommandWithSignallingTest::_commandedFlightSpeedFilter_data()
{
    QTest::addColumn<int>("componentOffset");
    QTest::addColumn<float>("speedType");
    QTest::addColumn<float>("speed");
    QTest::addColumn<float>("relative");
    QTest::addColumn<bool>("invalidate");
    QTest::newRow("other-component") << 1 << 1.0f << 8.0f << 0.0f << false;
    QTest::newRow("vertical-speed") << 0 << 2.0f << 8.0f << 0.0f << false;
    QTest::newRow("relative-speed") << 0 << 1.0f << 8.0f << 1.0f << true;
    QTest::newRow("negative-relative-speed") << 0 << 1.0f << -3.0f << 1.0f << true;
    QTest::newRow("other-relative-type") << 0 << 0.0f << 8.0f << 1.0f << false;
    QTest::newRow("reset-speed") << 0 << 1.0f << -2.0f << 0.0f << true;
    QTest::newRow("other-reset-type") << 0 << 0.0f << -2.0f << 0.0f << false;
    QTest::newRow("unchanged-speed") << 0 << 1.0f << -1.0f << 0.0f << false;
    QTest::newRow("unchanged-relative-speed") << 0 << 1.0f << -1.0f << 1.0f << false;
    QTest::newRow("zero-speed") << 0 << 1.0f << 0.0f << 0.0f << false;
    QTest::newRow("nan-speed") << 0 << 1.0f << std::numeric_limits<float>::quiet_NaN() << 0.0f << false;
    QTest::newRow("infinite-speed") << 0 << 1.0f << std::numeric_limits<float>::infinity() << 0.0f << false;
}

void SendMavCommandWithSignallingTest::_commandedFlightSpeedFilter()
{
    QFETCH(int, componentOffset);
    QFETCH(float, speedType);
    QFETCH(float, speed);
    QFETCH(float, relative);
    QFETCH(bool, invalidate);
    _mockLink->setCommLost(true);
    _vehicle->sendMavCommand(_vehicle->defaultComponentId(), MAV_CMD_DO_CHANGE_SPEED, false, 1, 8, -1, 0);
    _acknowledgeSpeedCommand(_vehicle->defaultComponentId(), MAV_RESULT_ACCEPTED);
    QCOMPARE(_vehicle->commandedFlightSpeed(), 8.0);
    const int component = _vehicle->defaultComponentId() + componentOffset;
    QSignalSpy changed(_vehicle, &Vehicle::commandedFlightSpeedChanged);
    QSignalSpy results(_vehicle, &Vehicle::mavCommandResult);
    _vehicle->sendMavCommand(component, MAV_CMD_DO_CHANGE_SPEED, false, speedType, speed, -1, relative);
    _acknowledgeSpeedCommand(component, MAV_RESULT_ACCEPTED);
    QCOMPARE(results.count(), 1);
    QCOMPARE(results.last().at(3).toInt(), int(MAV_RESULT_ACCEPTED));
    if (invalidate) {
        QVERIFY(qIsNaN(_vehicle->commandedFlightSpeed()));
    } else {
        QCOMPARE(_vehicle->commandedFlightSpeed(), 8.0);
    }
    QCOMPARE(_vehicle->commandedFlightSpeedType(), invalidate ? -1 : 1);
    QCOMPARE(changed.count(), invalidate ? 1 : 0);
}

void SendMavCommandWithSignallingTest::_commandedFlightSpeedMissionLifecycle()
{
    auto* mission = _vehicle->missionManager();
    QList<MissionItem*> items;
    for (int i = 0; i < 8; ++i) {
        const bool speed = i == 2 || i == 4 || i == 6;
        items.append(new MissionItem(i, speed ? MAV_CMD_DO_CHANGE_SPEED : MAV_CMD_NAV_WAYPOINT,
                                     speed ? MAV_FRAME_MISSION : MAV_FRAME_GLOBAL_RELATIVE_ALT,
                                     speed ? (i == 4 ? 0 : 1) : 0, speed ? i + 3 : 0, speed ? -1 : 0, 0,
                                     speed ? 0 : 30.0 + i * 0.001, speed ? 0 : 120, speed ? 0 : 30, true, false));
    }
    QSignalSpy sent(mission, &MissionManager::sendComplete);
    mission->writeMissionItems(items);
    QTRY_VERIFY_WITH_TIMEOUT(!sent.isEmpty(), TestTimeout::mediumMs());
    QCOMPARE(sent.last().first().toBool(), false);
    QCOMPARE(mission->missionItems().count(), 7);
    QCOMPARE(mission->missionItems().at(1)->sequenceNumber(), 1);
    QCOMPARE(mission->missionItems().at(1)->command(), MAV_CMD_DO_CHANGE_SPEED);
    _mockLink->setCommLost(true);
    const auto report = [&](int index) {
        mavlink_message_t message{};
        mavlink_msg_mission_current_pack(_vehicle->id(), _vehicle->defaultComponentId(), &message, index, 7,
                                         MISSION_STATE_ACTIVE, 1, 0, 0, 0);
        return QMetaObject::invokeMethod(mission, "_mavlinkMessageReceived", Qt::DirectConnection,
                                         Q_ARG(mavlink_message_t, message));
    };
    const auto acceptSpeed = [&]() {
        _vehicle->sendMavCommand(_vehicle->defaultComponentId(), MAV_CMD_DO_CHANGE_SPEED, false, 1, 8, -1, 0);
        _acknowledgeSpeedCommand(_vehicle->defaultComponentId(), MAV_RESULT_ACCEPTED);
    };
    QVERIFY(report(1));
    acceptSpeed();
    QCOMPARE(_vehicle->commandedFlightSpeed(), 8.0);
    QVERIFY(report(2));
    QCOMPARE(_vehicle->commandedFlightSpeed(), 8.0);  // Do not reapply the old current item's speed.
    QVERIFY(report(4));
    QCOMPARE(_vehicle->commandedFlightSpeed(), 8.0);  // A later airspeed command does not replace groundspeed.
    QVERIFY(report(5));
    QVERIFY(qIsNaN(_vehicle->commandedFlightSpeed()));
    acceptSpeed();
    QVERIFY(report(6));
    QCOMPARE(_vehicle->commandedFlightSpeed(), 8.0);
    QVERIFY(report(2));
    QVERIFY(qIsNaN(_vehicle->commandedFlightSpeed()));  // A backward jump starts a different execution pass.

    _vehicle->_updateArmed(true);
    acceptSpeed();
    _vehicle->_updateArmed(false);
    QVERIFY(qIsNaN(_vehicle->commandedFlightSpeed()));
    QCOMPARE(_vehicle->commandedFlightSpeedType(), -1);

    acceptSpeed();
    mission->sendComplete(true);
    QCOMPARE(_vehicle->commandedFlightSpeed(), 8.0);
    mission->sendComplete(false);
    QVERIFY(qIsNaN(_vehicle->commandedFlightSpeed()));
    acceptSpeed();
    mission->removeAllComplete(true);
    QCOMPARE(_vehicle->commandedFlightSpeed(), 8.0);
    mission->removeAllComplete(false);
    QVERIFY(qIsNaN(_vehicle->commandedFlightSpeed()));
    acceptSpeed();
    mission->newMissionItemsAvailable(false);
    QVERIFY(qIsNaN(_vehicle->commandedFlightSpeed()));
}

UT_REGISTER_TEST(SendMavCommandWithSignallingTest, TestLabel::Integration, TestLabel::Vehicle)
