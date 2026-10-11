#include <cmath>

#include <QtCore/QRegularExpression>
#include <QtTest/QSignalSpy>

#include "CaacDirectionalDistance.h"
#include "CaacFlightDistance.h"
#include "QGCGeo.h"
#include "UnitTest.h"
#include "Vehicle.h"
#include "VehicleLinkManager.h"

class CaacFlightDistanceTest : public UnitTest
{
    Q_OBJECT

private slots:
    void _components_data();
    void _components();
    void _flightLifecycle();
    void _brokenTrack();
    void _vehicleSignalsAndIsolation();
};

void CaacFlightDistanceTest::_components_data()
{
    QTest::addColumn<QGeoCoordinate>("start");
    QTest::addColumn<double>("bearing");
    QTest::addColumn<double>("north");
    QTest::addColumn<double>("south");
    QTest::addColumn<double>("east");
    QTest::addColumn<double>("west");
    QTest::newRow("north") << QGeoCoordinate(22.5, 113.4) << 0.0 << 100.0 << 0.0 << 0.0 << 0.0;
    QTest::newRow("south") << QGeoCoordinate(22.5, 113.4) << 180.0 << 0.0 << 100.0 << 0.0 << 0.0;
    QTest::newRow("east") << QGeoCoordinate(22.5, 113.4) << 90.0 << 0.0 << 0.0 << 100.0 << 0.0;
    QTest::newRow("west") << QGeoCoordinate(22.5, 113.4) << 270.0 << 0.0 << 0.0 << 0.0 << 100.0;
    QTest::newRow("northeast") << QGeoCoordinate(22.5, 113.4) << 45.0 << 70.710678 << 0.0 << 70.710678 << 0.0;
    QTest::newRow("southwest") << QGeoCoordinate(-35, 18) << 225.0 << 0.0 << 70.710678 << 0.0 << 70.710678;
    QTest::newRow("dateline") << QGeoCoordinate(0, 179.9999) << 90.0 << 0.0 << 0.0 << 100.0 << 0.0;
    QTest::newRow("high-latitude") << QGeoCoordinate(85, -179.9999) << 315.0 << 70.710678 << 0.0 << 0.0 << 70.710678;
}

void CaacFlightDistanceTest::_components()
{
    QFETCH(QGeoCoordinate, start);
    QFETCH(double, bearing);
    QFETCH(double, north);
    QFETCH(double, south);
    QFETCH(double, east);
    QFETCH(double, west);
    CaacDirectionalDistance distance;
    distance.setAirborne(true, start);
    QVERIFY(distance.updateCoordinate(QGCGeo::geodesicDestination(start, bearing, 100)));
    const auto& totals = distance.totals();
    QVERIFY(std::abs(totals.north - north) < 0.0001);
    QVERIFY(std::abs(totals.south - south) < 0.0001);
    QVERIFY(std::abs(totals.east - east) < 0.0001);
    QVERIFY(std::abs(totals.west - west) < 0.0001);
}

void CaacFlightDistanceTest::_flightLifecycle()
{
    const QGeoCoordinate home(0, 0, 50);
    const auto north = QGCGeo::geodesicDestination(home, 0, 100);
    CaacDirectionalDistance distance;
    QVERIFY(!distance.updateCoordinate(north));
    distance.setAirborne(true, home);
    QVERIFY(distance.updateCoordinate(north));
    distance.setAirborne(true, north);  // Repeated state must not reset a flight.
    QVERIFY(distance.updateCoordinate(home));
    QVERIFY(std::abs(distance.totals().north - 100) < 0.0001);
    QVERIFY(std::abs(distance.totals().south - 100) < 0.0001);
    auto higher = home;
    higher.setAltitude(150);
    QVERIFY(!distance.updateCoordinate(higher));
    distance.setAirborne(false, higher);
    QVERIFY(!distance.updateCoordinate(north));
    QVERIFY(std::abs(distance.totals().north - 100) < 0.0001);
    distance.setAirborne(true, home);
    QCOMPARE(distance.totals().north, 0);
    QCOMPARE(distance.totals().south, 0);
    QVERIFY(distance.updateCoordinate(north));
    distance.reset();
    QCOMPARE(distance.totals().north, 0);
    QVERIFY(!distance.updateCoordinate(home));  // Restart also discards the old position baseline.
    QVERIFY(distance.updateCoordinate(north));
    QVERIFY(std::abs(distance.totals().north - 100) < 0.0001);
}

void CaacFlightDistanceTest::_brokenTrack()
{
    const QGeoCoordinate home(30, 120);
    const auto east = QGCGeo::geodesicDestination(home, 90, 100);
    CaacDirectionalDistance distance;
    distance.setAirborne(true, QGeoCoordinate());
    QVERIFY(!distance.updateCoordinate(home));
    QVERIFY(distance.updateCoordinate(east));
    QVERIFY(!distance.updateCoordinate(QGeoCoordinate()));
    QVERIFY(!distance.updateCoordinate(home));
    QVERIFY(distance.updateCoordinate(east));
    QVERIFY(std::abs(distance.totals().east - 200) < 0.0001);
    QVERIFY(distance.totals().west < 0.0001);
    distance.breakTrack();
    QVERIFY(!distance.updateCoordinate(home));
    QVERIFY(distance.totals().west < 0.0001);
}

void CaacFlightDistanceTest::_vehicleSignalsAndIsolation()
{
    ignoreLogMessage("Utilities.QGCSensors", QtWarningMsg,
                     QRegularExpression(QStringLiteral("Failed to connect to pressure backend")));
    ignoreLogMessage("Utilities.QGCSensors", QtWarningMsg,
                     QRegularExpression(QStringLiteral("Error Initializing Pressure Sensor")));
    Vehicle first(MAV_AUTOPILOT_GENERIC, MAV_TYPE_QUADROTOR);
    Vehicle second(MAV_AUTOPILOT_GENERIC, MAV_TYPE_QUADROTOR);
    CaacFlightDistance firstDistance(&first);
    CaacFlightDistance secondDistance(&second);
    QSignalSpy changed(&firstDistance, &CaacFlightDistance::distanceChanged);
    QVERIFY(changed.isValid());
    const QGeoCoordinate home(0, 0);
    const auto north = QGCGeo::geodesicDestination(home, 0, 100);
    first.coordinateChanged(home);
    first.coordinateChanged(north);
    QCOMPARE(firstDistance.north(), 0);
    first._setUnderway(true);
    first.coordinateChanged(home);
    first.coordinateChanged(north);
    QVERIFY(std::abs(firstDistance.north() - 100) < 0.0001);
    QCOMPARE(secondDistance.north(), 0);
    QVERIFY(changed.count() >= 2);
    first.vehicleLinkManager()->communicationLostChanged(true);
    first.coordinateChanged(home);
    first.vehicleLinkManager()->communicationLostChanged(false);
    first.coordinateChanged(home);
    first.coordinateChanged(north);
    QVERIFY(std::abs(firstDistance.north() - 200) < 0.0001);
    QCOMPARE(firstDistance.south(), 0);
    firstDistance.resetForSimulationRestart();
    QCOMPARE(firstDistance.north(), 0);
    first.coordinateChanged(home);
    first.coordinateChanged(north);
    QVERIFY(std::abs(firstDistance.north() - 100) < 0.0001);
    first._setUnderway(false);
    first.coordinateChanged(home);
    QCOMPARE(firstDistance.south(), 0);
}

UT_REGISTER_TEST(CaacFlightDistanceTest, TestLabel::Unit, TestLabel::Vehicle)

#include "CaacFlightDistanceTest.moc"
