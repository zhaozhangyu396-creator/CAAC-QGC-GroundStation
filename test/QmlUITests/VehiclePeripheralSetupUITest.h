#pragma once

#include "VehicleConfigUITestBase.h"

class VehiclePeripheralSetupUITest : public VehicleConfigUITestBase
{
    Q_OBJECT

private slots:
    void _settings_data();
    void _settings();

private:
    void _capture(const QString& name);
};
