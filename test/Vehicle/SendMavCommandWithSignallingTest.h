#pragma once

#include "BaseClasses/VehicleTest.h"
#include "Vehicle.h"

class SendMavCommandWithSignallingTest : public VehicleTestNoInitialConnect
{
    Q_OBJECT

public:
    explicit SendMavCommandWithSignallingTest(QObject* parent = nullptr)
        : VehicleTestNoInitialConnect(parent)
    {
        setAutopilotType(MAV_AUTOPILOT_INVALID);
    }

private slots:
    void _performTestCases();
    void _retryHasFullAckWindow();
    void _duplicateCommand();
    void _commandedFlightSpeedConfirmation();
    void _commandedFlightSpeedFilter_data();
    void _commandedFlightSpeedFilter();
    void _commandedFlightSpeedMissionLifecycle();

private:
    struct TestCase_t
    {
        MAV_CMD command;
        MAV_RESULT expectedCommandResult;
        Vehicle::MavCmdResultFailureCode_t expectedFailureCode;
        int expectedSendCount;
        bool expectAckTimeout = false;  ///< Outcome depends on the ack timeout expiring at least once
    };

    void _testCaseWorker(TestCase_t& testCase);
    void _acknowledgeSpeedCommand(int targetComponent, MAV_RESULT result);

    static TestCase_t _rgTestCases[];
};
