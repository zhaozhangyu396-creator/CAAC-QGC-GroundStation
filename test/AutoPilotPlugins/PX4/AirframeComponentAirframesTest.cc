#include "AirframeComponentAirframesTest.h"

#include <QtTest/QTest>

#include "AirframeComponentAirframes.h"
#include "PX4AirframeLoader.h"

UT_REGISTER_TEST(AirframeComponentAirframesTest, TestLabel::Unit)

void AirframeComponentAirframesTest::_sortedTypesStandardFramesFirst()
{
    // Unit test builds always load the resource based airframe metadata; the
    // load is guarded so repeated calls are no-ops
    PX4AirframeLoader::loadAirframeMetaData();
    QVERIFY2(!AirframeComponentAirframes::get().isEmpty(), "Airframe metadata failed to load");

    const QList<AirframeComponentAirframes::AirframeType_t*> sorted = AirframeComponentAirframes::sortedTypes();
    QCOMPARE(sorted.count(), AirframeComponentAirframes::get().count());

    // The standard airframes show first so users don't have to scroll past the
    // exotic frames to find a regular quad
    const QStringList priorityNames = {
        QStringLiteral("Quadrotor x"),
        QStringLiteral("Standard Plane"),
        QStringLiteral("Standard VTOL"),
    };
    QVERIFY(sorted.count() > priorityNames.count());
    for (int i = 0; i < priorityNames.count(); i++) {
        QCOMPARE(sorted.at(i)->name, priorityNames.at(i));
    }

    // The remaining groups follow in QMap key order (case-sensitive
    // alphabetical) with no duplicates of the priority entries
    for (int i = priorityNames.count(); i < sorted.count(); i++) {
        QVERIFY2(!priorityNames.contains(sorted.at(i)->name),
                 qPrintable(QStringLiteral("Priority group duplicated in remainder: %1").arg(sorted.at(i)->name)));
        if (i > priorityNames.count()) {
            QVERIFY2(sorted.at(i - 1)->name < sorted.at(i)->name,
                     qPrintable(QStringLiteral("Remainder not alphabetical: '%1' before '%2'")
                                    .arg(sorted.at(i - 1)->name, sorted.at(i)->name)));
        }
    }
}

void AirframeComponentAirframesTest::_categoriesPreserveVehicleClasses()
{
    PX4AirframeLoader::loadAirframeMetaData();
    QMap<int, const AirframeComponentAirframes::AirframeInfo_t*> airframes;
    for (const auto* type : AirframeComponentAirframes::sortedTypes()) {
        for (const auto* airframe : type->rgAirframeInfo) {
            QVERIFY2(!airframe->vehicleClass.isEmpty(), qPrintable(airframe->name));
            QVERIFY2(!airframe->category.isEmpty(), qPrintable(airframe->name));
            airframes.insert(airframe->autostartId, airframe);
        }
    }

    // The XML repeats Simulation across three classes; the merged group must retain each entry's class.
    struct ExpectedAirframe
    {
        int id;
        const char* vehicleClass;
        const char* category;
    };

    const ExpectedAirframe expected[] = {
        {1001, "Copter", "multirotor"},
        {1100, "Copter", "multirotor"},
        {1101, "Plane", "fixedwing"},
        {1102, "VTOL", "vtol"},
        {1103, "VTOL", "vtol"},
        {60000, "Underwater Robot", "other"},
        {60002, "Underwater Robot", "other"},
        {70000, "Spacecraft", "other"},
        {16001, "Copter", "other"},
        {17002, "Autogyro", "other"},
        {18001, "Balloon", "airship"},
    };
    for (const auto& entry : expected) {
        const auto* airframe = airframes.value(entry.id, nullptr);
        QVERIFY2(airframe, qPrintable(QStringLiteral("Missing metadata entry %1").arg(entry.id)));
        QCOMPARE(airframe->vehicleClass, QLatin1String(entry.vehicleClass));
        QCOMPARE(airframe->category, QLatin1String(entry.category));
    }
}
