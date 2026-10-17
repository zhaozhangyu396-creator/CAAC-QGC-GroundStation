#include "APMSensorsCalibrationUITest.h"

#include <QtCore/QDir>
#include <QtCore/QLocale>
#include <QtCore/QScopeGuard>
#include <QtGui/QScreen>
#include <QtQuick/QQuickItem>
#include <QtQuick/QQuickWindow>
#include <QtTest/QSignalSpy>
#include <QtTest/QTest>

#include "AppSettings.h"
#include "MockLink.h"
#include "ParameterManager.h"
#include "QGCApplication.h"
#include "QGCCorePlugin.h"
#include "SettingsManager.h"
#include "Vehicle.h"

UT_REGISTER_TEST(APMSensorsCalibrationUITest, TestLabel::Integration)

void APMSensorsCalibrationUITest::init()
{
    if (!apmFirmwareSupported()) {
        QSKIP("ArduPilot support not registered in this build");
    }
    VehicleConfigUITestBase::init();
}

// ---------------------------------------------------------------------------
// _testCompassCalibration
// ---------------------------------------------------------------------------
void APMSensorsCalibrationUITest::_testCompassCalibration()
{
    runWithMockLink([] { return MockLink::startAPMArduCopterMockLink(); },
                    [&](QPointer<MockLink> /*mockLink*/, Vehicle* vehicle) {
                        resetAPMParamsToUncalibrated(vehicle);
                        if (QTest::currentTestFailed())
                            return;

                        navigateToAPMSensorsPage();
                        if (QTest::currentTestFailed())
                            return;

                        // Both indicators must be orange (not calibrated)
                        verifyAPMCalIndicators(/*compassGreen=*/false, /*accelGreen=*/false, "after param reset");
                        if (QTest::currentTestFailed())
                            return;

                        // -------------------------------------------------------------------
                        // 1. Accel calibration (required before compass on APM)
                        // -------------------------------------------------------------------
                        runAPMFullAccelCal();
                        if (QTest::currentTestFailed())
                            return;

                        waitForParamRefreshQuiet(vehicle);
                        if (QTest::currentTestFailed())
                            return;

                        // Accel indicator should now be green; compass still orange
                        verifyAPMCalIndicators(/*compassGreen=*/false, /*accelGreen=*/true, "after accel cal");
                        if (QTest::currentTestFailed())
                            return;

                        // -------------------------------------------------------------------
                        // 2. Compass calibration
                        // -------------------------------------------------------------------
                        runAPMCompassCal();
                        if (QTest::currentTestFailed())
                            return;

                        waitForParamRefreshQuiet(vehicle);
                        if (QTest::currentTestFailed())
                            return;

                        // Both indicators must be green
                        verifyAPMCalIndicators(/*compassGreen=*/true, /*accelGreen=*/true, "after compass cal");
                    });
}

// ---------------------------------------------------------------------------
// _testCompassCalibrationCancel
// ---------------------------------------------------------------------------
void APMSensorsCalibrationUITest::_testCompassCalibrationCancel()
{
    runWithMockLink(
        [] { return MockLink::startAPMArduCopterMockLink(); },
        [&](QPointer<MockLink> /*mockLink*/, Vehicle* vehicle) {
            resetAPMParamsToUncalibrated(vehicle);
            if (QTest::currentTestFailed())
                return;

            navigateToAPMSensorsPage();
            if (QTest::currentTestFailed())
                return;

            // Must do accel first
            runAPMFullAccelCal();
            if (QTest::currentTestFailed())
                return;
            waitForParamRefreshQuiet(vehicle);
            if (QTest::currentTestFailed())
                return;

            selectAPMSensorSection(QStringLiteral("Compass"));
            if (QTest::currentTestFailed())
                return;
            // Start compass cal
            QVERIFY2(clickButtonScrolled(QStringLiteral("sensorsSetup_calibrateCompass"),
                                         QStringLiteral("sensorsSetup_buttonScroller")),
                     "Failed to click Compass button");
            QVERIFY2(clickButtonScrolled(QStringLiteral("sensorsSetup_confirmOrientationCalibration"),
                                         QStringLiteral("sensorsSetup_buttonScroller")),
                     "Failed to confirm inline compass calibration");

            // Wait for some progress
            QQuickItem* progressBar = findVisibleItem(_rootItem, QStringLiteral("sensorsSetup_progressBar"), 2000);
            QVERIFY2(progressBar, "Progress bar not found");
            QVERIFY2(QTest::qWaitFor([&] { return progressBar->property("value").toDouble() > 0.0; }, 5000),
                     "Compass cal progress never started");

            // Cancel
            QVERIFY2(clickButtonScrolled(QStringLiteral("sensorsSetup_cancelButton"),
                                         QStringLiteral("sensorsSetup_buttonScroller")),
                     "Failed to click Cancel button");

            // After cancel the cancel button should become disabled (cal no longer active)
            QQuickItem* cancelBtn = findVisibleItem(_rootItem, QStringLiteral("sensorsSetup_cancelButton"), 2000);
            QVERIFY2(cancelBtn, "Cancel button not found after cancel");
            QVERIFY2(QTest::qWaitFor([&] { return !cancelBtn->property("enabled").toBool(); }, 5000),
                     "Cancel button still enabled after cancellation");
            if (QGCCorePlugin::instance()->directMissionItemEditing()) {
                QQuickItem* panel = findVisibleItem(_rootItem, QStringLiteral("sensorsSetup_inlinePanel"));
                QVERIFY(panel);
                QTRY_VERIFY_WITH_TIMEOUT(!panel->property("calibrationActive").toBool(), TestTimeout::mediumMs());
                QVERIFY(!findVisibleItem(_rootItem, QStringLiteral("popupDialog_frame"), 0));
            }

            // COMPASS_OFS_X must still be 0 (compass not calibrated)
            ParameterManager* mgr = vehicle->parameterManager();
            QVERIFY2(mgr->parameterExists(ParameterManager::defaultComponentId, QStringLiteral("COMPASS_OFS_X")),
                     "COMPASS_OFS_X parameter not found");
            Fact* compassOfs = mgr->getParameter(ParameterManager::defaultComponentId, QStringLiteral("COMPASS_OFS_X"));
            QVERIFY2(qFuzzyIsNull(compassOfs->rawValue().toFloat()),
                     "COMPASS_OFS_X is non-zero after compass cal cancel");

            // Compass indicator still orange
            verifyAPMCalIndicators(/*compassGreen=*/false, /*accelGreen=*/true, "after compass cal cancel");
        });
}

// ---------------------------------------------------------------------------
// _testCompassCalibrationIgnoresStaleFailedReports
//
// ArduPilot keeps streaming MAG_CAL_REPORT(MAG_CAL_FAILED) after a failed
// onboard compass cal until it is cancelled. Starting a new calibration while
// that stream is active must not instantly complete/fail the new cal.
// ---------------------------------------------------------------------------
void APMSensorsCalibrationUITest::_testCompassCalibrationIgnoresStaleFailedReports()
{
    runWithMockLink(
        [] { return MockLink::startAPMArduCopterMockLink(); },
        [&](QPointer<MockLink> mockLink, Vehicle* vehicle) {
            resetAPMParamsToUncalibrated(vehicle);
            if (QTest::currentTestFailed())
                return;

            navigateToAPMSensorsPage();
            if (QTest::currentTestFailed())
                return;

            // Must do accel first
            runAPMFullAccelCal();
            if (QTest::currentTestFailed())
                return;
            waitForParamRefreshQuiet(vehicle);
            if (QTest::currentTestFailed())
                return;

            // Simulate leftover MAG_CAL_REPORT(FAILED) stream from a previously failed cal
            mockLink->startAPMStaleFailedMagCalReportStreaming();

            selectAPMSensorSection(QStringLiteral("Compass"));
            if (QTest::currentTestFailed())
                return;
            // Start compass cal while the stale stream is active
            QVERIFY2(clickButtonScrolled(QStringLiteral("sensorsSetup_calibrateCompass"),
                                         QStringLiteral("sensorsSetup_buttonScroller")),
                     "Failed to click Compass button");
            QVERIFY2(clickButtonScrolled(QStringLiteral("sensorsSetup_confirmOrientationCalibration"),
                                         QStringLiteral("sensorsSetup_buttonScroller")),
                     "Failed to confirm inline compass calibration");

            // The stale failed reports must not terminate the new calibration: the cancel
            // button stays enabled while a compass cal is active, so it must remain enabled.
            QQuickItem* cancelBtn = findVisibleItem(_rootItem, QStringLiteral("sensorsSetup_cancelButton"), 2000);
            QVERIFY2(cancelBtn, "Cancel button not found");
            QVERIFY2(QTest::qWaitFor([&] { return cancelBtn->property("enabled").toBool(); }, 5000),
                     "Compass cal never became active");

            // Starting the new cal must have cancelled the stale stream on the vehicle
            QVERIFY2(QTest::qWaitFor([&] { return !mockLink->apmStaleFailedMagCalReportStreamingActive(); }, 5000),
                     "Stale failed MAG_CAL_REPORT stream was never cancelled");

            // Calibration must still be running (not instantly completed/failed by a stale report):
            // give the stale stream time to have been (mis)processed, then check cancel still enabled.
            QVERIFY2(!QTest::qWaitFor([&] { return !cancelBtn->property("enabled").toBool(); }, 2000),
                     "Compass cal terminated prematurely - stale MAG_CAL_REPORT was processed");

            // Now let the normal MockLink progress simulation complete the calibration
            if (QGCCorePlugin::instance()->directMissionItemEditing()) {
                QVERIFY(findVisibleItem(_rootItem, QStringLiteral("sensorsSetup_compassCalibrationResult"), 25000));
                QVERIFY(!findVisibleItem(_rootItem, QStringLiteral("popupDialog_acceptButton"), 0));
                QVERIFY(clickButtonScrolled(QStringLiteral("sensorsSetup_acknowledgeCalibration"),
                                            QStringLiteral("sensorsSetup_buttonScroller")));
            } else {
                QVERIFY2(findVisibleItem(_rootItem, QStringLiteral("popupDialog_acceptButton"), 25000),
                         "Post-compass-cal dialog not shown");
                QVERIFY2(clickButton(QStringLiteral("popupDialog_acceptButton")),
                         "Failed to dismiss post-compass-cal dialog");
            }

            waitForParamRefreshQuiet(vehicle);
            if (QTest::currentTestFailed())
                return;

            // Both indicators must be green - the cal completed successfully
            verifyAPMCalIndicators(/*compassGreen=*/true, /*accelGreen=*/true, "after compass cal with stale reports");
        });
}

// ---------------------------------------------------------------------------
// _testCompassCalibrationStartRejected
//
// MAG_CAL_PROGRESS/REPORT are ignored until MAV_CMD_DO_START_MAG_CAL is
// accepted, so a rejected START is the only exit path for a cal that never
// starts. Verify the UI exits cleanly instead of staying in a running state.
// ---------------------------------------------------------------------------
void APMSensorsCalibrationUITest::_testCompassCalibrationStartRejected()
{
    runWithMockLink(
        [] { return MockLink::startAPMArduCopterMockLink(); },
        [&](QPointer<MockLink> mockLink, Vehicle* vehicle) {
            resetAPMParamsToUncalibrated(vehicle);
            if (QTest::currentTestFailed())
                return;

            navigateToAPMSensorsPage();
            if (QTest::currentTestFailed())
                return;

            // Must do accel first
            runAPMFullAccelCal();
            if (QTest::currentTestFailed())
                return;
            waitForParamRefreshQuiet(vehicle);
            if (QTest::currentTestFailed())
                return;

            // Vehicle rejects MAV_CMD_DO_START_MAG_CAL
            mockLink->setAPMMagCalStartFailureMode(true);

            // START is sent with showError=true so the rejection first shows the command
            // error, then _stopCalibration(StopCalibrationFailed) shows the cal failed message.
            const bool inlineCalibration = QGCCorePlugin::instance()->directMissionItemEditing();
            if (!inlineCalibration) {
                expectAppMessage(QRegularExpression(QStringLiteral("command failed")));
                expectAppMessage(QRegularExpression(QStringLiteral("Calibration failed")));
            }

            selectAPMSensorSection(QStringLiteral("Compass"));
            if (QTest::currentTestFailed())
                return;

            QVERIFY2(clickButtonScrolled(QStringLiteral("sensorsSetup_calibrateCompass"),
                                         QStringLiteral("sensorsSetup_buttonScroller")),
                     "Failed to click Compass button");
            QVERIFY2(clickButtonScrolled(QStringLiteral("sensorsSetup_confirmOrientationCalibration"),
                                         QStringLiteral("sensorsSetup_buttonScroller")),
                     "Failed to confirm inline compass calibration");

            // The rejected START must cleanly stop the calibration: the cancel button is only
            // enabled while a cal is active, so it must end up disabled.
            QQuickItem* cancelBtn = findVisibleItem(_rootItem, QStringLiteral("sensorsSetup_cancelButton"), 2000);
            QVERIFY2(cancelBtn, "Cancel button not found");
            QVERIFY2(QTest::qWaitFor([&] { return !cancelBtn->property("enabled").toBool(); }, 10000),
                     "Compass cal still active after START was rejected");

            if (!inlineCalibration) {
                verifyExpectedLogMessage();  // command failed
                verifyExpectedLogMessage();  // Calibration failed
            } else {
                QVERIFY(!findVisibleItem(_rootItem, QStringLiteral("popupDialog_acceptButton"), 0));
            }

            auto* statusLog = findVisibleItem(_rootItem, QStringLiteral("sensorsSetup_statusLog"), 3000);
            QVERIFY2(statusLog, "Calibration failure log is not visible");
            QVERIFY2(
                statusLog->property("text").toString().contains(QStringLiteral("Failed to start compass calibration")),
                "Calibration failure log does not contain the rejected start result");

            waitForParamRefreshQuiet(vehicle);
            if (QTest::currentTestFailed())
                return;

            // Compass indicator still orange, accel green
            verifyAPMCalIndicators(/*compassGreen=*/false, /*accelGreen=*/true, "after rejected compass cal start");
        });
}

// ---------------------------------------------------------------------------
// _testAccelCalibration
// ---------------------------------------------------------------------------
void APMSensorsCalibrationUITest::_testAccelCalibration()
{
    runWithMockLink(
        [] { return MockLink::startAPMArduCopterMockLink(); },
        [&](QPointer<MockLink> /*mockLink*/, Vehicle* vehicle) {
            const QString previewDirectory = qEnvironmentVariable("CAAC_SENSOR_PREVIEW_DIR");
            Fact* languageFact = SettingsManager::instance()->appSettings()->qLocaleLanguage();
            const QVariant savedLanguage = languageFact->rawValue();
            const QSize savedSize = _window->size();
            const auto restorePreview = qScopeGuard([&, this] {
                if (previewDirectory.isEmpty()) {
                    return;
                }
                _window->resize(savedSize);
                if (languageFact->rawValue() != savedLanguage) {
                    qgcApp()->resetRebootMessageDebounce();
                    expectAppMessage(QRegularExpression(
                        QRegularExpression::escape(Fact::tr("Restart application for changes to take effect."))));
                    languageFact->setRawValue(savedLanguage);
                    QVERIFY(acceptDialog(TestTimeout::mediumMs()));
                    verifyExpectedLogMessage();
                }
            });
            if (!previewDirectory.isEmpty()) {
                QVERIFY(QDir().mkpath(previewDirectory));
                const QMargins frame = _window->frameMargins();
                const QSize available =
                    _window->screen() ? _window->screen()->availableGeometry().size() : QSize(1280, 1000);
                const QSize previewSize(qMin(1280, available.width() - frame.left() - frame.right()),
                                        qMin(1000, available.height() - frame.top() - frame.bottom()));
                _window->resize(previewSize);
                QTRY_COMPARE_WITH_TIMEOUT(_window->size(), previewSize, TestTimeout::shortMs());
                ignoreLogMessage("API.QGCApplication", QtWarningMsg,
                                 QRegularExpression(QStringLiteral("Qt lib localization for .* is not present")));
                ignoreLogMessage("API.QGCApplication", QtWarningMsg,
                                 QRegularExpression(QStringLiteral("Error loading source localization for .*")));
                ignoreLogMessage("API.QGCApplication", QtWarningMsg,
                                 QRegularExpression(QStringLiteral("Error loading json localization for .*")));
                if (savedLanguage.toInt() != QLocale::Chinese) {
                    qgcApp()->resetRebootMessageDebounce();
                    expectAppMessage(QRegularExpression(
                        QRegularExpression::escape(Fact::tr("Restart application for changes to take effect."))));
                    languageFact->setRawValue(QLocale::Chinese);
                    QVERIFY(acceptDialog(TestTimeout::mediumMs()));
                    verifyExpectedLogMessage();
                }
            }
            const auto capturePreview = [&](const QString& name) {
                if (previewDirectory.isEmpty()) {
                    return true;
                }
                QQuickItem* scroller = findVisibleItem(_rootItem, QStringLiteral("sensorsSetup_buttonScroller"));
                if (scroller) {
                    scroller->setProperty("contentY", 0.0);
                }
                QSignalSpy rendered(_window, &QQuickWindow::frameSwapped);
                _window->update();
                return waitForCondition([&] { return !rendered.isEmpty(); }, TestTimeout::mediumMs(), name) &&
                       _window->grabWindow().save(QDir(previewDirectory).filePath(name));
            };

            resetAPMParamsToUncalibrated(vehicle);
            if (QTest::currentTestFailed())
                return;

            navigateToAPMSensorsPage();
            if (QTest::currentTestFailed())
                return;

            QVERIFY2(!findVisibleItem(_rootItem, QStringLiteral("sensorsSetup_statusLog"), 0),
                     "Idle calibration log panel should be hidden");

            QVERIFY2(clickButtonScrolled(QStringLiteral("sensorsSetup_setOrientation"),
                                         QStringLiteral("sensorsSetup_buttonScroller")),
                     "Failed to expand flight controller orientation");
            QVERIFY2(findVisibleItem(_rootItem, QStringLiteral("sensorsSetup_boardOrientation"), 3000),
                     "Board orientation Fact control was not shown inline");
            QVERIFY2(!findVisibleItem(_rootItem, QStringLiteral("popupDialog_acceptButton"), 0),
                     "Orientation settings unexpectedly opened a dialog");
            QVERIFY(capturePreview(QStringLiteral("sensors-01-orientation.png")));
            QVERIFY2(clickButtonScrolled(QStringLiteral("sensorsSetup_setOrientation"),
                                         QStringLiteral("sensorsSetup_buttonScroller")),
                     "Failed to collapse flight controller orientation");
            QTRY_VERIFY_WITH_TIMEOUT(!findVisibleItem(_rootItem, QStringLiteral("sensorsSetup_orientationSettings"), 0),
                                     3000);

            selectAPMSensorSection(QStringLiteral("Accelerometer"));
            if (QTest::currentTestFailed())
                return;
            QVERIFY(capturePreview(QStringLiteral("sensors-02-accelerometer.png")));

            QVERIFY2(clickButtonScrolled(QStringLiteral("sensorsSetup_calibrateAccel"),
                                         QStringLiteral("sensorsSetup_buttonScroller")),
                     "Failed to open inline accel confirmation");
            QVERIFY2(clickButtonScrolled(QStringLiteral("sensorsSetup_closeOrientationSettings"),
                                         QStringLiteral("sensorsSetup_buttonScroller")),
                     "Failed to cancel inline accel confirmation");
            QQuickItem* cancelBtn = findVisibleItem(_rootItem, QStringLiteral("sensorsSetup_cancelButton"), 2000);
            QVERIFY2(cancelBtn && !cancelBtn->property("enabled").toBool(),
                     "Cancelling the inline confirmation started calibration");
            QQuickItem* accelBtn = findVisibleItem(_rootItem, QStringLiteral("sensorsSetup_calibrateAccel"), 2000);
            QVERIFY2(accelBtn && accelBtn->property("enabled").toBool(),
                     "Cancelling the inline confirmation did not restore the calibration entry");

            verifyAPMCalIndicators(/*compassGreen=*/false, /*accelGreen=*/false, "after param reset");
            if (QTest::currentTestFailed())
                return;

            runAPMFullAccelCal();
            if (QTest::currentTestFailed())
                return;

            waitForParamRefreshQuiet(vehicle);
            if (QTest::currentTestFailed())
                return;

            // INS_ACCOFFS_X must be non-zero (MockLink writes 0.1 on SUCCESS)
            ParameterManager* mgr = vehicle->parameterManager();
            QVERIFY2(mgr->parameterExists(ParameterManager::defaultComponentId, QStringLiteral("INS_ACCOFFS_X")),
                     "INS_ACCOFFS_X parameter not found");
            Fact* accelOfs = mgr->getParameter(ParameterManager::defaultComponentId, QStringLiteral("INS_ACCOFFS_X"));
            QVERIFY2(QTest::qWaitFor([&] { return !qFuzzyIsNull(accelOfs->rawValue().toFloat()); }, 10000),
                     "INS_ACCOFFS_X still 0 after accel cal — param refresh did not bring back non-zero value");

            // Accel indicator green, compass still orange
            verifyAPMCalIndicators(/*compassGreen=*/false, /*accelGreen=*/true, "after accel cal");
        });
}
