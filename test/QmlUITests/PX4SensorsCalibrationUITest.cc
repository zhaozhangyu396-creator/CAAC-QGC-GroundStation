#include "PX4SensorsCalibrationUITest.h"

#include <algorithm>

#include <QtCore/QElapsedTimer>
#include <QtQuick/QQuickItem>
#include <QtQuick/QQuickItemGrabResult>
#include <QtQuick/QQuickWindow>
#include <QtTest/QSignalSpy>
#include <QtTest/QTest>

#include "Fact.h"
#include "MockLink.h"
#include "MultiVehicleManager.h"
#include "ParameterManager.h"
#include "QGCCorePlugin.h"
#include "SensorsComponentController.h"
#include "Vehicle.h"

UT_REGISTER_TEST(PX4SensorsCalibrationUITest, TestLabel::Integration)

void PX4SensorsCalibrationUITest::_testMotorSafetyGate()
{
    if (!QGCCorePlugin::instance()->directMissionItemEditing()) {
        QSKIP("The motor slider interface is specific to the CAAC interface");
    }
    runWithMockLink([] { return MockLink::startPX4MockLink(); },
                    [&](QPointer<MockLink>, Vehicle* vehicle) {
                        _navigateToSensorsPanel();
                        if (QTest::currentTestFailed()) {
                            return;
                        }
                        QVERIFY(!findVisibleItem(_rootItem, QStringLiteral("sensorsSetup_enableMotorTest"), 0));
                        QVERIFY(clickSidebarButton(QStringLiteral("vehicleConfig_comp_Motors")));
                        auto* safetyCheck = findVisibleItem(_rootItem, QStringLiteral("sensorsSetup_enableMotorTest"));
                        auto* slider = findVisibleItem(_rootItem, QStringLiteral("sensorsSetup_motorSlider_1"));
                        QVERIFY(safetyCheck);
                        QVERIFY(slider);
                        QVERIFY(!safetyCheck->property("checked").toBool());
                        QVERIFY(!slider->isEnabled());
                        QVERIFY(clickButtonScrolled(QStringLiteral("sensorsSetup_enableMotorTest"),
                                                    QStringLiteral("vehicleSetup_scroller")));
                        QTRY_VERIFY(slider->isEnabled());
                        QSignalSpy commands(vehicle, &Vehicle::mavCommandResult);
                        slider->setProperty("value", 20);
                        QVERIFY(QMetaObject::invokeMethod(slider, "moved"));
                        QTRY_VERIFY_WITH_TIMEOUT(std::any_of(commands.cbegin(), commands.cend(),
                                                             [](const QList<QVariant>& result) {
                                                                 return result.at(2).toInt() == MAV_CMD_DO_MOTOR_TEST &&
                                                                        result.at(3).toInt() == MAV_RESULT_ACCEPTED;
                                                             }),
                                                 5000);
                        vehicle->setArmed(true, false);
                        QTRY_VERIFY_WITH_TIMEOUT(vehicle->armed(), 5000);
                        QTRY_VERIFY(!safetyCheck->property("checked").toBool());
                        QVERIFY(!slider->isEnabled());
                        QCOMPARE(slider->property("value").toInt(), 0);
                        vehicle->setArmed(false, false);
                        QTRY_VERIFY_WITH_TIMEOUT(!vehicle->armed(), 5000);
                        QVERIFY(!safetyCheck->property("checked").toBool());
                        QVERIFY(!slider->isEnabled());
                        QVERIFY(clickButtonScrolled(QStringLiteral("sensorsSetup_enableMotorTest"),
                                                    QStringLiteral("vehicleSetup_scroller")));
                        QTRY_VERIFY(slider->isEnabled());
                        slider->setProperty("value", 20);
                        QVERIFY(QMetaObject::invokeMethod(slider, "moved"));
                        _navigateToSensorsPanel();
                        if (QTest::currentTestFailed()) {
                            return;
                        }
                        clickSidebarButton(QStringLiteral("vehicleConfig_section_Accelerometer"));
                        if (QTest::currentTestFailed()) {
                            return;
                        }
                        auto* calibrateAccel =
                            findVisibleItem(_rootItem, QStringLiteral("sensorsSetup_calibrateAccel"));
                        QVERIFY(calibrateAccel);
                        QTRY_VERIFY(calibrateAccel->isEnabled());
                        QVERIFY(!findVisibleItem(_rootItem, QStringLiteral("sensorsSetup_enableMotorTest"), 0));
                        clickSidebarButton(QStringLiteral("vehicleConfig_section_Orientations"));
                        if (QTest::currentTestFailed()) {
                            return;
                        }
                        QVERIFY(!findVisibleItem(_rootItem, QStringLiteral("sensorsSetup_enableMotorTest"), 0));
                        QVERIFY(clickSidebarButton(QStringLiteral("vehicleConfig_comp_Motors")));
                        safetyCheck = findVisibleItem(_rootItem, QStringLiteral("sensorsSetup_enableMotorTest"));
                        slider = findVisibleItem(_rootItem, QStringLiteral("sensorsSetup_motorSlider_1"));
                        QVERIFY(safetyCheck);
                        QVERIFY(slider);
                        QVERIFY(!safetyCheck->property("checked").toBool());
                        QVERIFY(!slider->isEnabled());
                        QCOMPARE(slider->property("value").toInt(), 0);
                    });
}

namespace {

struct PoseInfo
{
    MockLinkPX4Calibration::Pose pose;
    const char* objectName;   ///< VehicleRotationCal objectName in SensorsSetup.qml
    const char* rotateImage;  ///< Image shown while rotating on this side (mag cal)
    const char* stillImage;   ///< Image shown while holding still on this side (accel cal)
    const char* customImage;
};

constexpr PoseInfo kPoses[MockLinkPX4Calibration::kSideCount] = {
    {MockLinkPX4Calibration::Pose::RightSideUp, "sensorsCal_downSide", "VehicleDownRotate.png", "VehicleDown.png",
     "pose-01.png"},
    {MockLinkPX4Calibration::Pose::UpsideDown, "sensorsCal_upsideDownSide", "VehicleUpsideDownRotate.png",
     "VehicleUpsideDown.png", "pose-02.png"},
    {MockLinkPX4Calibration::Pose::NoseDown, "sensorsCal_noseDownSide", "VehicleNoseDownRotate.png",
     "VehicleNoseDown.png", "pose-03.png"},
    {MockLinkPX4Calibration::Pose::TailDown, "sensorsCal_tailDownSide", "VehicleTailDownRotate.png",
     "VehicleTailDown.png", "pose-04.png"},
    {MockLinkPX4Calibration::Pose::Left, "sensorsCal_leftSide", "VehicleLeftRotate.png", "VehicleLeft.png",
     "pose-05.png"},
    {MockLinkPX4Calibration::Pose::Right, "sensorsCal_rightSide", "VehicleRightRotate.png", "VehicleRight.png",
     "pose-06.png"},
};

const char* poseImage(const PoseInfo& info, bool rotating)
{
    return QGCCorePlugin::instance()->directMissionItemEditing() ? info.customImage
           : rotating                                            ? info.rotateImage
                                                                 : info.stillImage;
}

bool waitForCalState(QQuickItem* item, SensorsComponentController::SideCalState expected, int timeoutMs)
{
    return QTest::qWaitFor([&] { return item->property("calState").toInt() == static_cast<int>(expected); }, timeoutMs);
}

const char* calStateName(int state)
{
    switch (static_cast<SensorsComponentController::SideCalState>(state)) {
        case SensorsComponentController::SideCalStateIdle:
            return "Idle";
        case SensorsComponentController::SideCalStateIncomplete:
            return "Incomplete";
        case SensorsComponentController::SideCalStateInProgress:
            return "InProgress";
        case SensorsComponentController::SideCalStateCompleted:
            return "Completed";
    }
    return "Unknown";
}

}  // namespace

void PX4SensorsCalibrationUITest::_navigateToSensorsPanel()
{
    navigateToConfigureView();
    if (QTest::currentTestFailed())
        return;

    QQuickItem* sensorsBtn = clickSidebarButton(QStringLiteral("vehicleConfig_comp_Sensors"));
    if (QTest::currentTestFailed())
        return;

    // CAAC opens flight controller orientation before accelerometer calibration;
    // the stock interface opens the first firmware section (Compass).
    QVERIFY2(sensorsBtn->property("expanded").toBool(),
             "Sensors section tree not expanded after clicking Sensors button");
    const auto initialSection = QGCCorePlugin::instance()->directMissionItemEditing()
                                    ? QStringLiteral("vehicleConfig_section_Orientations")
                                    : QStringLiteral("vehicleConfig_section_Compass");
    QQuickItem* selectedSection = findItem(_rootItem, initialSection);
    QVERIFY2(selectedSection, "Initial sensor section button not visible after clicking Sensors button");
    QVERIFY2(QTest::qWaitFor([&] { return selectedSection->property("sectionChecked").toBool(); }, 3000),
             "Initial sensor section not selected after clicking Sensors button");

    const auto initialAction = QGCCorePlugin::instance()->directMissionItemEditing()
                                   ? QStringLiteral("sensorsSetup_setOrientation")
                                   : QStringLiteral("sensorsSetup_calibrateCompass");
    QVERIFY2(findVisibleItem(_rootItem, initialAction, 3000),
             "Selected sensor section action not found after opening Sensors page");
    if (QGCCorePlugin::instance()->directMissionItemEditing()) {
        QVERIFY(!findVisibleItem(_rootItem, QStringLiteral("sensorsSetup_calibrateCompass"), 0));
        QVERIFY(!findVisibleItem(_rootItem, QStringLiteral("sensorsSetup_calibrateAccel"), 0));
        QVERIFY(!findVisibleItem(_rootItem, QStringLiteral("sensorCalibrationPreview"), 0));
    }
    QVERIFY2(!findVisibleItem(_rootItem, QStringLiteral("sensorsSetup_statusLog"), 0),
             "Idle calibration log panel should be hidden");

    // The prerequisite-setup message panel ("%1 setup must be completed prior to %2
    // setup.") must not be shown: SYS_AUTOSTART is preserved across the param reset
    // so Airframe setup remains complete
    QVERIFY2(!findVisibleItem(_rootItem, QStringLiteral("vehicleConfig_messagePanel"), 0),
             "Prerequisite setup message panel shown instead of Sensors page");
}

void PX4SensorsCalibrationUITest::_verifyAllPosesState(int expectedState, const char* context)
{
    for (const PoseInfo& info : kPoses) {
        QQuickItem* side = findVisibleItem(_rootItem, QLatin1String(info.objectName), 5000);
        QVERIFY2(side, qPrintable(QStringLiteral("Pose indicator not visible (%1): %2")
                                      .arg(QLatin1String(context), QLatin1String(info.objectName))));
        const int state = side->property("calState").toInt();
        QVERIFY2(state == expectedState,
                 qPrintable(QStringLiteral("Pose in state %1, expected %2 (%3): %4")
                                .arg(QLatin1String(calStateName(state)), QLatin1String(calStateName(expectedState)),
                                     QLatin1String(context), QLatin1String(info.objectName))));
    }
}

void PX4SensorsCalibrationUITest::_verifySensorsSetupStates(const SensorsSetupStates& expected, const char* context)
{
    struct ButtonCheck
    {
        const char* objectName;
        const char* propertyName;
        bool expectedComplete;
    };

    const ButtonCheck checks[] = {
        {"vehicleConfig_comp_Sensors", "setupComplete", expected.sensorsComplete},
        {"vehicleConfig_section_Compass", "sectionSetupComplete", expected.compassComplete},
        {"vehicleConfig_section_Gyroscope", "sectionSetupComplete", expected.gyroscopeComplete},
        {"vehicleConfig_section_Accelerometer", "sectionSetupComplete", expected.accelerometerComplete},
        {"vehicleConfig_section_LevelHorizon", "sectionSetupComplete", expected.levelHorizonComplete},
        {"vehicleConfig_section_Orientations", "sectionSetupComplete", expected.orientationsComplete},
    };

    for (const ButtonCheck& check : checks) {
        QQuickItem* btn = findItem(_rootItem, QLatin1String(check.objectName));
        QVERIFY2(btn, qPrintable(QStringLiteral("Button not found (%1): %2")
                                     .arg(QLatin1String(context), QLatin1String(check.objectName))));
        const QVariant complete = btn->property(check.propertyName);
        QVERIFY2(complete.isValid(), qPrintable(QStringLiteral("Button has no %1 property (%2): %3")
                                                    .arg(QLatin1String(check.propertyName), QLatin1String(context),
                                                         QLatin1String(check.objectName))));
        // Wait briefly for bindings to settle on the expected value
        (void) QTest::qWaitFor([&] { return btn->property(check.propertyName).toBool() == check.expectedComplete; },
                               3000);
        QVERIFY2(btn->property(check.propertyName).toBool() == check.expectedComplete,
                 qPrintable(QStringLiteral("Button setup complete state is %1, expected %2 (%3): %4")
                                .arg(btn->property(check.propertyName).toBool())
                                .arg(check.expectedComplete)
                                .arg(QLatin1String(context), QLatin1String(check.objectName))));
    }
}

void PX4SensorsCalibrationUITest::_startCalibration(const QString& calibrateButtonObjectName)
{
    auto* vehicle = MultiVehicleManager::instance()->activeVehicle();
    QVERIFY(vehicle);
    QSignalSpy messages(vehicle, &Vehicle::mavlinkMessageReceived);
    const auto hasCalibrationAck = [&](bool acceptedOnly) {
        return std::any_of(messages.cbegin(), messages.cend(), [acceptedOnly](const QList<QVariant>& arguments) {
            const auto message = arguments.constFirst().value<mavlink_message_t>();
            if (message.msgid != MAVLINK_MSG_ID_COMMAND_ACK) {
                return false;
            }
            mavlink_command_ack_t ack{};
            mavlink_msg_command_ack_decode(&message, &ack);
            return ack.command == MAV_CMD_PREFLIGHT_CALIBRATION && (!acceptedOnly || ack.result == MAV_RESULT_ACCEPTED);
        });
    };
    QVERIFY2(clickButtonScrolled(calibrateButtonObjectName, QStringLiteral("sensorsSetup_buttonScroller")),
             qPrintable(QStringLiteral("Failed to click calibrate button: %1").arg(calibrateButtonObjectName)));

    if (QGCCorePlugin::instance()->directMissionItemEditing()) {
        QVERIFY(!findVisibleItem(_rootItem, QStringLiteral("sensorsSetup_inlinePreparation"), 0));
        QVERIFY(!findVisibleItem(_rootItem, QStringLiteral("sensorsSetup_preparationBoardOrientation"), 0));
        QVERIFY(!findVisibleItem(_rootItem, QStringLiteral("popupDialog_frame"), 0));
    } else {
        QVERIFY2(findVisibleItem(_rootItem, QStringLiteral("popupDialog_acceptButton"), 3000),
                 "Pre-calibration dialog Ok button not found");
        QVERIFY2(clickButton(QStringLiteral("popupDialog_acceptButton")), "Failed to accept pre-calibration dialog");
    }
    QTRY_VERIFY_WITH_TIMEOUT(hasCalibrationAck(true), TestTimeout::mediumMs());
}

void PX4SensorsCalibrationUITest::_testMagCalibration()
{
    runWithMockLink(
        [] { return MockLink::startPX4MockLink(); },
        [&](QPointer<MockLink> mockLink, Vehicle* vehicle) {
            resetParamsToFirmwareDefaults(vehicle, QStringLiteral("CAL_MAG0_ID"));
            if (QTest::currentTestFailed())
                return;

            _navigateToSensorsPanel();
            if (QTest::currentTestFailed())
                return;

            _verifySensorsSetupStates(
                {
                    .sensorsComplete = false,
                    .compassComplete = false,
                    .gyroscopeComplete = false,
                    .accelerometerComplete = false,
                    .levelHorizonComplete = false,
                    .orientationsComplete = true,
                },
                "after param reset");
            if (QTest::currentTestFailed())
                return;

            clickSidebarButton(QStringLiteral("vehicleConfig_section_Orientations"));
            if (QTest::currentTestFailed())
                return;
            QVERIFY(clickButtonScrolled(QStringLiteral("sensorsSetup_setOrientation"),
                                        QStringLiteral("sensorsSetup_buttonScroller")));
            auto* boardOrientation = findVisibleItem(_rootItem, QStringLiteral("sensorsSetup_boardOrientation"));
            QVERIFY(boardOrientation);
            QCOMPARE(boardOrientation->property("fact").value<Fact*>(),
                     vehicle->parameterManager()->getParameter(ParameterManager::defaultComponentId,
                                                               QStringLiteral("SENS_BOARD_ROT")));
            QVERIFY(!findVisibleItem(_rootItem, QStringLiteral("popupDialog_acceptButton"), 0));
            QVERIFY(clickButtonScrolled(QStringLiteral("sensorsSetup_setOrientation"),
                                        QStringLiteral("sensorsSetup_buttonScroller")));
            QVERIFY(!findVisibleItem(_rootItem, QStringLiteral("sensorsSetup_orientationControls"), 0));

            // Compass shows the uploaded rotation animations and independent face progress.
            clickSidebarButton(QStringLiteral("vehicleConfig_section_Compass"));
            if (QTest::currentTestFailed())
                return;

            _verifyAllPosesState(SensorsComponentController::SideCalStateIdle, "idle preview");
            if (QTest::currentTestFailed())
                return;
            if (QGCCorePlugin::instance()->directMissionItemEditing()) {
                QVERIFY(!findVisibleItem(_rootItem, QStringLiteral("sensorsSetup_calibrateAccel"), 0));
                QVERIFY(!findVisibleItem(_rootItem, QStringLiteral("sensorsSetup_setOrientation"), 0));
            }

            if (auto* preview = findVisibleItem(_rootItem, QStringLiteral("sensorCalibrationPreview"), 0)) {
                QVERIFY(preview->property("compassMode").toBool());
                QVERIFY(!preview->property("motionGuidance").toBool());
                QVERIFY(!findVisibleItem(preview, QStringLiteral("sensorCalibrationLargePreview"), 0));
                for (int index = 0; index < MockLinkPX4Calibration::kSideCount; ++index) {
                    const PoseInfo& info = kPoses[index];
                    auto* side = findVisibleItem(preview, QLatin1String(info.objectName));
                    QVERIFY(side);
                    QVERIFY(side->width() > 0);
                    QVERIFY(side->height() > 0);
                    const auto sideBounds = side->mapRectToItem(preview, side->boundingRect());
                    QVERIFY(sideBounds.left() >= -1);
                    QVERIFY(sideBounds.top() >= -1);
                    QVERIFY(sideBounds.right() <= preview->width() + 1);
                    QVERIFY(sideBounds.bottom() <= preview->height() + 1);
                    auto* animation =
                        findVisibleItem(preview, QStringLiteral("sensorCalibrationPoseImage_%1").arg(index));
                    QVERIFY(animation);
                    QTRY_COMPARE_WITH_TIMEOUT(animation->property("status").toInt(), 1, 3000);
                    QVERIFY(animation->property("frameCount").toInt() > 1);
                    QVERIFY(animation->property("playing").toBool());
                    const int initialFrame = animation->property("currentFrame").toInt();
                    QTRY_VERIFY_WITH_TIMEOUT(animation->property("currentFrame").toInt() != initialFrame, 1000);
                    QVERIFY(animation->property("source").toUrl().toString().endsWith(QStringLiteral("-rotation.gif")));
                    auto* faceProgress = findVisibleItem(
                        side, QStringLiteral("sensorCalibrationFaceProgress_%1").arg(side->property("index").toInt()));
                    QVERIFY(faceProgress);
                    QCOMPARE(faceProgress->property("value").toDouble(), 0.0);
                }
                if (const auto path = qgetenv("CAAC_CALIBRATION_UI_PREVIEW"); !path.isEmpty()) {
                    const auto capture = preview->grabToImage();
                    QVERIFY(capture);
                    QSignalSpy ready(capture.data(), &QQuickItemGrabResult::ready);
                    QTRY_VERIFY_WITH_TIMEOUT(!ready.isEmpty(), 3000);
                    QVERIFY(capture->saveToFile(QString::fromUtf8(path)));
                }
            }

            _startCalibration(QStringLiteral("sensorsSetup_calibrateCompass"));
            if (QTest::currentTestFailed())
                return;

            // MockLink responds with "[cal] calibration started: 2 mag" which makes all
            // six pose indicators visible and Incomplete (red, not yet visited)
            for (const PoseInfo& info : kPoses) {
                QQuickItem* side = findVisibleItem(_rootItem, QLatin1String(info.objectName), 5000);
                QVERIFY2(
                    side,
                    qPrintable(QStringLiteral("Pose indicator not visible: %1").arg(QLatin1String(info.objectName))));
                QVERIFY2(waitForCalState(side, SensorsComponentController::SideCalStateIncomplete, 5000),
                         qPrintable(QStringLiteral("Pose not Incomplete at start (state %1): %2")
                                        .arg(QLatin1String(calStateName(side->property("calState").toInt())),
                                             QLatin1String(info.objectName))));
            }

            QQuickItem* progressBar = findVisibleItem(_rootItem, QStringLiteral("sensorsSetup_progressBar"), 2000);
            QVERIFY2(progressBar, "Progress bar not visible during calibration");
            QVERIFY2(qFuzzyIsNull(progressBar->property("value").toDouble()),
                     "Progress bar not at 0 at calibration start");

            QQuickItem* cancelButton =
                findVisibleItem(_rootItem, QStringLiteral("sensorsSetup_cancelCalibration"), 2000);
            QVERIFY2(cancelButton, "Cancel button not visible during calibration");

            // ---------------------------------------------------------------------
            // Place the vehicle into each pose and watch the UI track the side
            // ---------------------------------------------------------------------
            double lastProgress = 0.0;
            int sidesDone = 0;

            for (const PoseInfo& info : kPoses) {
                const QString sideName = QLatin1String(info.objectName);
                QQuickItem* side = findVisibleItem(_rootItem, sideName);
                QVERIFY2(side, qPrintable(QStringLiteral("Pose indicator disappeared: %1").arg(sideName)));

                mockLink->setCalibrationPose(info.pose);

                // Orientation detected: the side goes InProgress with the rotate animation
                QVERIFY2(waitForCalState(side, SensorsComponentController::SideCalStateInProgress, 5000),
                         qPrintable(QStringLiteral("Side never went in-progress: %1").arg(sideName)));
                const QString rotationImage =
                    QGCCorePlugin::instance()->directMissionItemEditing()
                        ? QString::fromLatin1(info.customImage)
                              .replace(QStringLiteral(".png"), QStringLiteral("-rotation.gif"))
                        : QString::fromLatin1(poseImage(info, true));
                if (auto* large = findVisibleItem(_rootItem, QStringLiteral("sensorCalibrationLargePreview"), 0)) {
                    QTRY_VERIFY_WITH_TIMEOUT(large->property("source").toUrl().toString().endsWith(rotationImage),
                                             3000);
                }
                if (QGCCorePlugin::instance()->directMissionItemEditing()) {
                    QTRY_VERIFY_WITH_TIMEOUT(side->property("faceProgress").toDouble() > 0.0, 1500);
                    QVERIFY(side->property("faceProgress").toDouble() < 1.0);
                }
                QCOMPARE(side->property("calInProgressText").toString(),
                         QGCCorePlugin::instance()->directMissionItemEditing() ? QStringLiteral("旋转")
                                                                               : QStringLiteral("Rotate"));
                if (sidesDone == 0) {
                    if (const auto path = qgetenv("CAAC_CALIBRATION_PROGRESS_PREVIEW"); !path.isEmpty()) {
                        auto* preview = findVisibleItem(_rootItem, QStringLiteral("sensorCalibrationPreview"));
                        QVERIFY(preview);
                        const auto capture = preview->grabToImage();
                        QVERIFY(capture);
                        QSignalSpy ready(capture.data(), &QQuickItemGrabResult::ready);
                        QTRY_VERIFY_WITH_TIMEOUT(!ready.isEmpty(), 3000);
                        QVERIFY(capture->saveToFile(QString::fromUtf8(path)));
                    }
                }
                QVERIFY2(side->property("imageSource").toString().endsWith(rotationImage),
                         qPrintable(QStringLiteral("Wrong rotate image for %1: %2")
                                        .arg(sideName, side->property("imageSource").toString())));

                // Side completes and marks itself done (green/Completed)
                QVERIFY2(waitForCalState(side, SensorsComponentController::SideCalStateCompleted, 5000),
                         qPrintable(QStringLiteral("Side never completed: %1").arg(sideName)));

                if (QGCCorePlugin::instance()->directMissionItemEditing()) {
                    QCOMPARE(side->property("faceProgress").toDouble(), 1.0);
                }
                sidesDone++;

                // Progress bar must move forward with each side and stay within the
                // band expected for the number of completed sides
                const double progress = progressBar->property("value").toDouble();
                QVERIFY2(progress > lastProgress,
                         qPrintable(QStringLiteral("Progress did not advance after %1: %2 -> %3")
                                        .arg(sideName)
                                        .arg(lastProgress)
                                        .arg(progress)));
                if (sidesDone < MockLinkPX4Calibration::kSideCount) {
                    constexpr double sideCount = MockLinkPX4Calibration::kSideCount;
                    QVERIFY2(
                        (progress >= ((sidesDone - 1) / sideCount)) && (progress <= ((sidesDone / sideCount) + 0.01)),
                        qPrintable(QStringLiteral("Progress out of expected band after %1 sides: %2")
                                       .arg(sidesDone)
                                       .arg(progress)));
                }
                lastProgress = progress;

                // Previously completed sides must remain marked complete
                for (const PoseInfo& doneInfo : kPoses) {
                    if (&doneInfo == &info)
                        break;
                    QQuickItem* doneSide = findVisibleItem(_rootItem, QLatin1String(doneInfo.objectName));
                    QVERIFY2(doneSide && (doneSide->property("calState").toInt() ==
                                          SensorsComponentController::SideCalStateCompleted),
                             qPrintable(QStringLiteral("Previously completed side no longer marked complete: %1")
                                            .arg(QLatin1String(doneInfo.objectName))));
                }
            }

            // ---------------------------------------------------------------------
            // Calibration complete: progress reaches 100% and the result stays in Compass.
            // ---------------------------------------------------------------------
            QVERIFY2(
                QTest::qWaitFor([&] { return qFuzzyCompare(progressBar->property("value").toDouble(), 1.0); }, 5000),
                qPrintable(QStringLiteral("Progress bar never reached 1.0: %1")
                               .arg(progressBar->property("value").toDouble())));

            QVERIFY2(findVisibleItem(_rootItem, QStringLiteral("sensorsSetup_orientationRebootMessage"), 5000),
                     "Compass completion reboot reminder not shown inline");
            QVERIFY2(vehicle->rebootRequired(), "Compass calibration did not latch rebootRequired");
            QVERIFY(!findVisibleItem(_rootItem, QStringLiteral("popupDialog_acceptButton"), 0));
            if (QGCCorePlugin::instance()->directMissionItemEditing()) {
                auto* workflow = findVisibleItem(_rootItem, QStringLiteral("sensorsSetup_inlineWorkflow"));
                QVERIFY(workflow);
                QVERIFY(findVisibleItem(workflow, QStringLiteral("sensorsSetup_calibrationResult")));
                QVERIFY(findVisibleItem(workflow, QStringLiteral("sensorsSetup_orientationRebootMessage")));
                QVERIFY(!findVisibleItem(workflow, QStringLiteral("sensorsSetup_boardOrientation"), 0));
                QVERIFY(!findVisibleItem(_rootItem, QStringLiteral("popupDialog_frame"), 0));
                auto* compassSection = findItem(_rootItem, QStringLiteral("vehicleConfig_section_Compass"));
                QVERIFY(compassSection);
                QVERIFY(compassSection->property("sectionChecked").toBool());
                QVERIFY(!findVisibleItem(_rootItem, QStringLiteral("sensorsSetup_setOrientation"), 0));
            }

            // Calibration no longer active: the cancel button row hides
            QVERIFY2(QTest::qWaitFor([&] { return !cancelButton->isVisible(); }, 5000),
                     "Cancel button still visible after calibration completed");

            waitForParamRefreshQuiet(vehicle);
            if (QTest::currentTestFailed())
                return;

            // All sides must remain marked complete (green) after calibration ends and
            // the post-calibration parameter refresh settles
            _verifyAllPosesState(SensorsComponentController::SideCalStateCompleted, "after calibration finished");
            if (QTest::currentTestFailed())
                return;

            // Switching the preview to a different sensor resets the completed sides:
            // the Accelerometer preview must show all poses in the neutral Idle state
            clickSidebarButton(QStringLiteral("vehicleConfig_section_Accelerometer"));
            if (QTest::currentTestFailed())
                return;

            _verifyAllPosesState(SensorsComponentController::SideCalStateIdle, "after section switch");
            if (QTest::currentTestFailed())
                return;

            // The completed mag calibration set CAL_MAG0_ID on the vehicle: only the
            // Compass section shows setup complete, everything else still requires config
            _verifySensorsSetupStates(
                {
                    .sensorsComplete = false,
                    .compassComplete = true,
                    .gyroscopeComplete = false,
                    .accelerometerComplete = false,
                    .levelHorizonComplete = false,
                    .orientationsComplete = true,
                },
                "after mag calibration");
        });
}

void PX4SensorsCalibrationUITest::_runCalibrationCancelTest(const QString& sectionObjectName,
                                                            const QString& calibrateButtonObjectName)
{
    runWithMockLink(
        [] { return MockLink::startPX4MockLink(); },
        [&](QPointer<MockLink> mockLink, Vehicle* vehicle) {
            resetParamsToFirmwareDefaults(vehicle, QStringLiteral("CAL_MAG0_ID"));
            if (QTest::currentTestFailed())
                return;

            _navigateToSensorsPanel();
            if (QTest::currentTestFailed())
                return;

            _verifySensorsSetupStates(
                {
                    .sensorsComplete = false,
                    .compassComplete = false,
                    .gyroscopeComplete = false,
                    .accelerometerComplete = false,
                    .levelHorizonComplete = false,
                    .orientationsComplete = true,
                },
                "after param reset");
            if (QTest::currentTestFailed())
                return;

            // Select the sensor section so its calibrate button is at the top of the page
            clickSidebarButton(sectionObjectName);
            if (QTest::currentTestFailed())
                return;

            _startCalibration(calibrateButtonObjectName);
            if (QTest::currentTestFailed())
                return;

            // Start calibrating one side
            const PoseInfo& info = kPoses[0];
            QQuickItem* side = findVisibleItem(_rootItem, QLatin1String(info.objectName), 5000);
            QVERIFY2(side, "Pose indicator not visible after calibration start");

            mockLink->setCalibrationPose(info.pose);
            QVERIFY2(waitForCalState(side, SensorsComponentController::SideCalStateInProgress, 5000),
                     "Side never went in-progress");

            // Cancel mid-calibration: MockLink responds with "[cal] calibration cancelled"
            QQuickItem* cancelButton = findVisibleItem(_rootItem, QStringLiteral("sensorsSetup_cancelCalibration"));
            QVERIFY2(cancelButton, "Cancel button not visible during calibration");
            if (QGCCorePlugin::instance()->directMissionItemEditing()) {
                QVERIFY2(clickButtonScrolled(QStringLiteral("sensorsSetup_cancelCalibration"),
                                             QStringLiteral("sensorsSetup_buttonScroller")),
                         "Failed to click Cancel inside the selected sensor section");
                QVERIFY(!findVisibleItem(_rootItem, QStringLiteral("popupDialog_frame"), 0));
                QVERIFY(!findVisibleItem(_rootItem, QStringLiteral("popupDialog_acceptButton"), 0));
                QVERIFY(!findVisibleItem(_rootItem, QStringLiteral("popupDialog_rejectButton"), 0));
            } else {
                QVERIFY2(clickButton(QStringLiteral("sensorsSetup_cancelCalibration")), "Failed to click Cancel");
            }

            // The calibration-active UI (progress bar + cancel button row) hides once
            // the controller processes the cancel response
            QVERIFY2(QTest::qWaitFor([&] { return !cancelButton->isVisible(); }, 5000),
                     "Cancel button still visible after cancel");

            // Cancelled calibration discards results: the partially calibrated side
            // returns to the neutral Idle state, not red and not green
            QVERIFY2(waitForCalState(side, SensorsComponentController::SideCalStateIdle, 5000),
                     qPrintable(QStringLiteral("Side not Idle after cancel (state %1)")
                                    .arg(QLatin1String(calStateName(side->property("calState").toInt())))));

            // The calibrate button is clickable again
            QVERIFY2(
                findVisibleItem(_rootItem, calibrateButtonObjectName, 3000),
                qPrintable(
                    QStringLiteral("Calibrate button not available after cancel: %1").arg(calibrateButtonObjectName)));

            waitForParamRefreshQuiet(vehicle);
            if (QTest::currentTestFailed())
                return;

            // The cancelled calibration discarded its results: every sensor still
            // requires config
            _verifySensorsSetupStates(
                {
                    .sensorsComplete = false,
                    .compassComplete = false,
                    .gyroscopeComplete = false,
                    .accelerometerComplete = false,
                    .levelHorizonComplete = false,
                    .orientationsComplete = true,
                },
                "after calibration cancelled");
        });
}

void PX4SensorsCalibrationUITest::_testMagCalibrationCancel()
{
    _runCalibrationCancelTest(QStringLiteral("vehicleConfig_section_Compass"),
                              QStringLiteral("sensorsSetup_calibrateCompass"));
}

void PX4SensorsCalibrationUITest::_testAccelCalibration()
{
    runWithMockLink(
        [] { return MockLink::startPX4MockLink(); },
        [&](QPointer<MockLink> mockLink, Vehicle* vehicle) {
            resetParamsToFirmwareDefaults(vehicle, QStringLiteral("CAL_MAG0_ID"));
            if (QTest::currentTestFailed())
                return;

            _navigateToSensorsPanel();
            if (QTest::currentTestFailed())
                return;

            _verifySensorsSetupStates(
                {
                    .sensorsComplete = false,
                    .compassComplete = false,
                    .gyroscopeComplete = false,
                    .accelerometerComplete = false,
                    .levelHorizonComplete = false,
                    .orientationsComplete = true,
                },
                "after param reset");
            if (QTest::currentTestFailed())
                return;

            // Select the Accelerometer section: the idle orientation preview appears with
            // all six poses in the neutral Idle state
            clickSidebarButton(QStringLiteral("vehicleConfig_section_Accelerometer"));
            if (QTest::currentTestFailed())
                return;

            _verifyAllPosesState(SensorsComponentController::SideCalStateIdle, "idle preview");
            if (QTest::currentTestFailed())
                return;

            _startCalibration(QStringLiteral("sensorsSetup_calibrateAccel"));
            if (QTest::currentTestFailed())
                return;

            // MockLink responds with "[cal] calibration started: 2 accel" which makes all
            // six pose indicators visible and Incomplete (red, not yet visited)
            for (const PoseInfo& info : kPoses) {
                QQuickItem* side = findVisibleItem(_rootItem, QLatin1String(info.objectName), 5000);
                QVERIFY2(
                    side,
                    qPrintable(QStringLiteral("Pose indicator not visible: %1").arg(QLatin1String(info.objectName))));
                QVERIFY2(waitForCalState(side, SensorsComponentController::SideCalStateIncomplete, 5000),
                         qPrintable(QStringLiteral("Pose not Incomplete at start (state %1): %2")
                                        .arg(QLatin1String(calStateName(side->property("calState").toInt())),
                                             QLatin1String(info.objectName))));
            }

            QQuickItem* progressBar = findVisibleItem(_rootItem, QStringLiteral("sensorsSetup_progressBar"), 2000);
            QVERIFY2(progressBar, "Progress bar not visible during calibration");
            QVERIFY2(qFuzzyIsNull(progressBar->property("value").toDouble()),
                     "Progress bar not at 0 at calibration start");

            QQuickItem* cancelButton =
                findVisibleItem(_rootItem, QStringLiteral("sensorsSetup_cancelCalibration"), 2000);
            QVERIFY2(cancelButton, "Cancel button not visible during calibration");

            // ---------------------------------------------------------------------
            // Place the vehicle into each pose and watch the UI track the side
            // ---------------------------------------------------------------------
            int sidesDone = 0;

            for (const PoseInfo& info : kPoses) {
                const QString sideName = QLatin1String(info.objectName);
                QQuickItem* side = findVisibleItem(_rootItem, sideName);
                QVERIFY2(side, qPrintable(QStringLiteral("Pose indicator disappeared: %1").arg(sideName)));

                mockLink->setCalibrationPose(info.pose);

                // Orientation detected: the side goes InProgress. Unlike mag cal the
                // vehicle must be held still, so the static (non-rotating) image stays up
                QVERIFY2(waitForCalState(side, SensorsComponentController::SideCalStateInProgress, 5000),
                         qPrintable(QStringLiteral("Side never went in-progress: %1").arg(sideName)));
                QCOMPARE(side->property("calInProgressText").toString(),
                         QGCCorePlugin::instance()->directMissionItemEditing() ? QStringLiteral("保持静止")
                                                                               : QStringLiteral("Hold Still"));
                QVERIFY2(side->property("imageSource").toString().endsWith(QLatin1String(poseImage(info, false))),
                         qPrintable(QStringLiteral("Wrong hold-still image for %1: %2")
                                        .arg(sideName, side->property("imageSource").toString())));

                // Side completes and marks itself done (green/Completed)
                QVERIFY2(waitForCalState(side, SensorsComponentController::SideCalStateCompleted, 5000),
                         qPrintable(QStringLiteral("Side never completed: %1").arg(sideName)));

                sidesDone++;

                // The firmware reports progress in 17% jumps after each side (see PX4
                // accel_calibration_worker())
                if (sidesDone < MockLinkPX4Calibration::kSideCount) {
                    const double progress = progressBar->property("value").toDouble();
                    const double expected = (17.0 * sidesDone) / 100.0;
                    QVERIFY2(qAbs(progress - expected) < 0.005,
                             qPrintable(QStringLiteral("Unexpected progress after %1 sides: %2, expected %3")
                                            .arg(sidesDone)
                                            .arg(progress)
                                            .arg(expected)));
                }

                // Previously completed sides must remain marked complete
                for (const PoseInfo& doneInfo : kPoses) {
                    if (&doneInfo == &info)
                        break;
                    QQuickItem* doneSide = findVisibleItem(_rootItem, QLatin1String(doneInfo.objectName));
                    QVERIFY2(doneSide && (doneSide->property("calState").toInt() ==
                                          SensorsComponentController::SideCalStateCompleted),
                             qPrintable(QStringLiteral("Previously completed side no longer marked complete: %1")
                                            .arg(QLatin1String(doneInfo.objectName))));
                }
            }

            // ---------------------------------------------------------------------
            // Calibration complete: progress hits 100% and the calibration-active UI
            // (progress bar + cancel button row) hides. Unlike mag cal there is no
            // completion dialog.
            // ---------------------------------------------------------------------
            QVERIFY2(
                QTest::qWaitFor([&] { return qFuzzyCompare(progressBar->property("value").toDouble(), 1.0); }, 5000),
                qPrintable(QStringLiteral("Progress bar never reached 1.0: %1")
                               .arg(progressBar->property("value").toDouble())));

            QVERIFY2(QTest::qWaitFor([&] { return !cancelButton->isVisible(); }, 5000),
                     "Cancel button still visible after calibration completed");

            waitForParamRefreshQuiet(vehicle);
            if (QTest::currentTestFailed())
                return;

            // All sides must remain marked complete (green) after calibration ends and
            // the post-calibration parameter refresh settles
            _verifyAllPosesState(SensorsComponentController::SideCalStateCompleted, "after calibration finished");
            if (QTest::currentTestFailed())
                return;

            // Switching the preview to a different sensor resets the completed sides:
            // the Compass preview must show all poses in the neutral Idle state
            clickSidebarButton(QStringLiteral("vehicleConfig_section_Compass"));
            if (QTest::currentTestFailed())
                return;

            _verifyAllPosesState(SensorsComponentController::SideCalStateIdle, "after section switch");
            if (QTest::currentTestFailed())
                return;

            // The completed accel calibration set CAL_ACC0_ID on the vehicle: only the
            // Accelerometer section shows setup complete. Level Horizon still requires
            // config because the gyro is uncalibrated.
            _verifySensorsSetupStates(
                {
                    .sensorsComplete = false,
                    .compassComplete = false,
                    .gyroscopeComplete = false,
                    .accelerometerComplete = true,
                    .levelHorizonComplete = false,
                    .orientationsComplete = true,
                },
                "after accel calibration");
        });
}

void PX4SensorsCalibrationUITest::_testAccelCalibrationCancel()
{
    _runCalibrationCancelTest(QStringLiteral("vehicleConfig_section_Accelerometer"),
                              QStringLiteral("sensorsSetup_calibrateAccel"));
}
