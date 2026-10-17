#include "APMSensorsComponent.h"

#include "ParameterManager.h"
#include "QGCCorePlugin.h"
#include "Vehicle.h"

APMSensorsComponent::APMSensorsComponent(Vehicle* vehicle, AutoPilotPlugin* autopilot, QObject* parent)
    : VehicleComponent(vehicle, autopilot, AutoPilotPlugin::KnownSensorsVehicleComponent, parent)
{}

QStringList APMSensorsComponent::sectionIds() const
{
    if (!QGCCorePlugin::instance()->directMissionItemEditing()) {
        return {};
    }
    return {QStringLiteral("Orientations"),  QStringLiteral("Accelerometer"), QStringLiteral("Compass"),
            QStringLiteral("Level Horizon"), QStringLiteral("Gyroscope"),     QStringLiteral("Pressure"),
            QStringLiteral("CompassMot")};
}

QString APMSensorsComponent::sectionDisplayName(const QString& sectionId) const
{
    if (sectionId == QStringLiteral("Orientations")) {
        return tr("飞控朝向");
    }
    if (sectionId == QStringLiteral("Accelerometer")) {
        return tr("Accelerometer");
    }
    if (sectionId == QStringLiteral("Compass")) {
        return tr("Compass");
    }
    if (sectionId == QStringLiteral("Level Horizon")) {
        return tr("Level Horizon");
    }
    if (sectionId == QStringLiteral("Gyroscope")) {
        return tr("Gyroscope");
    }
    if (sectionId == QStringLiteral("Pressure")) {
        return tr("Pressure");
    }
    if (sectionId == QStringLiteral("CompassMot")) {
        return tr("Compass Motor Interference Calibration");
    }
    return sectionId;
}

bool APMSensorsComponent::sectionSetupComplete(const QString& sectionId) const
{
    if (!_vehicle) {
        return false;
    }
    if (sectionId == QStringLiteral("Accelerometer")) {
        return !accelSetupNeeded();
    }
    if (sectionId == QStringLiteral("Compass")) {
        return !compassSetupNeeded();
    }
    return true;
}

QStringList APMSensorsComponent::setupCompleteChangedTriggerList() const
{
    static const QStringList triggers = {
        QStringLiteral("COMPASS_DEV_ID"), QStringLiteral("COMPASS_DEV_ID2"), QStringLiteral("COMPASS_DEV_ID3"),
        QStringLiteral("COMPASS_USE"),    QStringLiteral("COMPASS_USE2"),    QStringLiteral("COMPASS_USE3"),
        QStringLiteral("COMPASS_OFS_X"),  QStringLiteral("COMPASS_OFS_Y"),   QStringLiteral("COMPASS_OFS_Z"),
        QStringLiteral("COMPASS_OFS2_X"), QStringLiteral("COMPASS_OFS2_Y"),  QStringLiteral("COMPASS_OFS2_Z"),
        QStringLiteral("COMPASS_OFS3_X"), QStringLiteral("COMPASS_OFS3_Y"),  QStringLiteral("COMPASS_OFS3_Z"),
        QStringLiteral("INS_ACCOFFS_X"),  QStringLiteral("INS_ACCOFFS_Y"),   QStringLiteral("INS_ACCOFFS_Z")};

    return triggers;
}

bool APMSensorsComponent::compassSetupNeeded() const
{
    ParameterManager* parameters = _vehicle ? _vehicle->parameterManager() : nullptr;
    if (!parameters) {
        return true;
    }
    static const QStringList rgDevicesIds = {QStringLiteral("COMPASS_DEV_ID"), QStringLiteral("COMPASS_DEV_ID2"),
                                             QStringLiteral("COMPASS_DEV_ID3")};

    static const QStringList rgCompassUse = {QStringLiteral("COMPASS_USE"), QStringLiteral("COMPASS_USE2"),
                                             QStringLiteral("COMPASS_USE3")};

    static const QList<QStringList> rgOffsets = {
        {QStringLiteral("COMPASS_OFS_X"), QStringLiteral("COMPASS_OFS_Y"), QStringLiteral("COMPASS_OFS_Z")},
        {QStringLiteral("COMPASS_OFS2_X"), QStringLiteral("COMPASS_OFS2_Y"), QStringLiteral("COMPASS_OFS2_Z")},
        {QStringLiteral("COMPASS_OFS3_X"), QStringLiteral("COMPASS_OFS3_Y"), QStringLiteral("COMPASS_OFS3_Z")}};

    for (qsizetype i = 0; i < rgDevicesIds.length(); i++) {
        const Fact* deviceId = parameters->getParameter(ParameterManager::defaultComponentId, rgDevicesIds[i]);
        if (!deviceId) {
            return true;
        }
        if (deviceId->rawValue().toInt() == 0) {
            continue;
        }
        const Fact* useCompass = parameters->getParameter(ParameterManager::defaultComponentId, rgCompassUse[i]);
        if (!useCompass) {
            return true;
        }
        if (useCompass->rawValue().toInt() == 0) {
            continue;
        }

        const QStringList& offsets = rgOffsets[i];
        for (const QString& offset : offsets) {
            const Fact* offsetFact = parameters->getParameter(ParameterManager::defaultComponentId, offset);
            if (!offsetFact || offsetFact->rawValue().toFloat() == 0.0f) {
                return true;
            }
        }
    }

    return false;
}

bool APMSensorsComponent::accelSetupNeeded() const
{
    ParameterManager* parameters = _vehicle ? _vehicle->parameterManager() : nullptr;
    if (!parameters) {
        return true;
    }
    // The best we can do is test the first accel which will always be there. We don't have enough information to know
    // whether any of the other accels are available.
    static const QStringList rgOffsets = {QStringLiteral("INS_ACCOFFS_X"), QStringLiteral("INS_ACCOFFS_Y"),
                                          QStringLiteral("INS_ACCOFFS_Z")};

    int zeroCount = 0;
    for (const QString& offset : rgOffsets) {
        const Fact* offsetFact = parameters->getParameter(ParameterManager::defaultComponentId, offset);
        if (!offsetFact) {
            return true;
        }
        if (offsetFact->rawValue().toFloat() == 0.0f) {
            zeroCount++;
        }
    }

    return (zeroCount == rgOffsets.count());
}
