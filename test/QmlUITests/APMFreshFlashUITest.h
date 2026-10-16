#pragma once

#include "VehicleConfigUITestBase.h"

/// UI test that connects an ArduPilot MockLink started with the
/// OptionAPMStartFreshParams option (freshly flashed firmware state: no
/// airframe selected, compass/accel/radio uncalibrated) and verifies that
/// Frame, Sensors and Radio all report setup required, that Frame is the
/// required prerequisite for the Sensors and Radio pages, that selecting
/// airframe categories does not write frame parameters, that selecting
/// a frame class unblocks them, and that completing accel + compass
/// calibration clears Sensors setup while Radio remains incomplete.
class APMFreshFlashUITest : public VehicleConfigUITestBase
{
    Q_OBJECT

public:
    APMFreshFlashUITest() = default;

protected slots:
    void init() override;

private slots:
    void _testFreshFlashSetupState();

private:
    /// Click the given component button in the config sidebar and verify
    /// whether the prerequisite-setup message panel is shown.
    void _verifyFramePrereq(const QString &compObjectName, bool expectPrereqShown);

    /// Verify setup state and the neutral embedded or standalone status icon color.
    void _verifySetupIndicator(const QString& compObjectName, bool expectSetupComplete);
};
