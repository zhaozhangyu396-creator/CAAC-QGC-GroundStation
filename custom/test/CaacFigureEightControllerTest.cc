#include <algorithm>
#include <cmath>
#include <limits>
#include <numbers>

#include <QtCore/QCryptographicHash>
#include <QtCore/QFile>
#include <QtCore/QJsonArray>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>
#include <QtCore/QSettings>
#include <QtCore/QTemporaryDir>
#include <QtTest/QSignalSpy>

#include "CaacExamAudio.h"
#include "CaacFigureEightController.h"
#include "LinkInterface.h"
#include "MissionManager.h"
#include "MultiVehicleManager.h"
#include "QGCGeo.h"
#include "TrajectoryPoints.h"
#include "Vehicle.h"
#include "VehicleLinkManager.h"
#include "VehicleTestManualConnect.h"

namespace {
bool takeoffAtExamHeight(Vehicle* vehicle)
{
    if (!vehicle) {
        return false;
    }
    // The plain MockLink fixture expects relative param7; the PX4 guided API sends AMSL.
    vehicle->sendMavCommand(vehicle->compId(), MAV_CMD_NAV_TAKEOFF, false, NAN, NAN, 0, NAN, NAN, NAN, 1.75);
    return true;
}

bool reportSpeed(Vehicle* vehicle, MockLink* link, double horizontalSpeed = 0, double verticalSpeed = 0)
{
    if (!vehicle || !link) {
        return false;
    }
    mavlink_message_t speed{};
    mavlink_msg_vfr_hud_pack(vehicle->id(), vehicle->compId(), &speed, 0, horizontalSpeed, 0, 0, 0, verticalSpeed);
    return QMetaObject::invokeMethod(vehicle, "_mavlinkMessageReceived", Qt::DirectConnection,
                                     Q_ARG(LinkInterface*, link), Q_ARG(mavlink_message_t, speed));
}

bool reportGps(Vehicle* vehicle, MockLink* link, const QGeoCoordinate& coordinate, uint8_t fixType,
               bool secondary = false)
{
    if (!vehicle || !link) {
        return false;
    }
    mavlink_message_t message{};
    if (secondary) {
        mavlink_gps2_raw_t gps{};
        gps.lat = qRound64(coordinate.latitude() * 1e7);
        gps.lon = qRound64(coordinate.longitude() * 1e7);
        gps.fix_type = fixType;
        mavlink_msg_gps2_raw_encode(vehicle->id(), vehicle->compId(), &message, &gps);
    } else {
        mavlink_gps_raw_int_t gps{};
        gps.lat = qRound64(coordinate.latitude() * 1e7);
        gps.lon = qRound64(coordinate.longitude() * 1e7);
        gps.fix_type = fixType;
        mavlink_msg_gps_raw_int_encode(vehicle->id(), vehicle->compId(), &message, &gps);
    }
    return QMetaObject::invokeMethod(vehicle, "_mavlinkMessageReceived", Qt::DirectConnection,
                                     Q_ARG(LinkInterface*, link), Q_ARG(mavlink_message_t, message));
}

bool reportPosition(Vehicle* vehicle, MockLink* link, CaacFigureEightController& controller,
                    const QGeoCoordinate& coordinate, double horizontalSpeed = 0, double verticalSpeed = 0,
                    bool quaternionStream = true)
{
    if (!vehicle || !link) {
        return false;
    }
    const double yaw = vehicle->heading()->rawValue().toDouble() * std::numbers::pi / 180;
    mavlink_message_t attitude{};
    mavlink_msg_attitude_pack(vehicle->id(), vehicle->compId(), &attitude, 0, 0, 0, yaw, 0, 0, 0);
    if (!QMetaObject::invokeMethod(vehicle, "_mavlinkMessageReceived", Qt::DirectConnection,
                                   Q_ARG(LinkInterface*, link), Q_ARG(mavlink_message_t, attitude)) ||
        !reportSpeed(vehicle, link, horizontalSpeed, verticalSpeed)) {
        return false;
    }
    if (quaternionStream) {
        const float offset[] = {1, 0, 0, 0};
        mavlink_msg_attitude_quaternion_pack(vehicle->id(), vehicle->compId(), &attitude, 0, std::cos(yaw / 2), 0, 0,
                                             std::sin(yaw / 2), 0, 0, 0, offset);
        if (!QMetaObject::invokeMethod(vehicle, "_mavlinkMessageReceived", Qt::DirectConnection,
                                       Q_ARG(LinkInterface*, link), Q_ARG(mavlink_message_t, attitude))) {
            return false;
        }
    }
    mavlink_message_t position{};
    mavlink_msg_global_position_int_pack(
        vehicle->id(), vehicle->compId(), &position, 0, qRound64(coordinate.latitude() * 1e7),
        qRound64(coordinate.longitude() * 1e7), qRound64(vehicle->altitudeAMSL()->rawValue().toDouble() * 1000),
        qRound64(vehicle->altitudeRelative()->rawValue().toDouble() * 1000), 0, 0, 0, 0);
    return QMetaObject::invokeMethod(vehicle, "_mavlinkMessageReceived", Qt::DirectConnection,
                                     Q_ARG(LinkInterface*, link), Q_ARG(mavlink_message_t, position)) &&
           QMetaObject::invokeMethod(&controller, "_sample", Qt::DirectConnection);
}

bool reportReferencePosition(Vehicle* vehicle, MockLink* link, CaacFigureEightController& controller, int index,
                             double speed = 1)
{
    if (!vehicle) {
        return false;
    }
    const auto path = controller.referencePath();
    if (index < 0 || index >= path.size()) {
        return false;
    }
    const auto previous = path[std::max(0, index - 1)].value<QGeoCoordinate>();
    const auto next = path[std::min(qsizetype(index + 1), path.size() - 1)].value<QGeoCoordinate>();
    const double heading = QGCGeo::geodesicAzimuth(previous, next) + (controller.standard() == 2 ? 180 : 0);
    vehicle->heading()->setRawValue(std::fmod(heading, 360));
    return reportPosition(vehicle, link, controller, path[index].value<QGeoCoordinate>(), speed);
}

bool reportTakeoffReference(Vehicle* vehicle, MockLink* link, bool reverse = false, double distance = 8)
{
    if (!vehicle || !link) {
        return false;
    }
    const auto centre = vehicle->coordinate();
    const double heading = reverse ? 180 : 0;
    const auto takeoff = QGCGeo::geodesicDestination(centre, heading + 180, distance);
    const auto baseMode = vehicle->baseMode();
    const auto customMode = vehicle->customMode();
    const auto receive = [vehicle, link](const mavlink_message_t& message) {
        return QMetaObject::invokeMethod(vehicle, "_mavlinkMessageReceived", Qt::DirectConnection,
                                         Q_ARG(LinkInterface*, link), Q_ARG(mavlink_message_t, message));
    };
    const auto reportArmed = [&](bool armed) {
        mavlink_message_t message{};
        const auto mode =
            static_cast<uint8_t>((baseMode & ~MAV_MODE_FLAG_SAFETY_ARMED) | (armed ? MAV_MODE_FLAG_SAFETY_ARMED : 0));
        mavlink_msg_heartbeat_pack(vehicle->id(), vehicle->compId(), &message, MAV_TYPE_QUADROTOR,
                                   vehicle->firmwareType(), mode, customMode, MAV_STATE_ACTIVE);
        return receive(message);
    };
    const auto reportCoordinate = [&](const QGeoCoordinate& coordinate) {
        mavlink_message_t message{};
        mavlink_msg_global_position_int_pack(
            vehicle->id(), vehicle->compId(), &message, 0, qRound64(coordinate.latitude() * 1e7),
            qRound64(coordinate.longitude() * 1e7), qRound64(vehicle->altitudeAMSL()->rawValue().toDouble() * 1000),
            qRound64(vehicle->altitudeRelative()->rawValue().toDouble() * 1000), 0, 0, 0, 0);
        return receive(message);
    };
    link->setVehicleAttitudeOverrideDeg(0, 0, heading);
    vehicle->heading()->setRawValue(heading);
    return reportArmed(false) && reportCoordinate(takeoff) && reportArmed(true) && reportCoordinate(centre);
}
}  // namespace

class CaacFigureEightControllerTest : public VehicleTestManualConnect
{
    Q_OBJECT

private slots:
    void _referenceGeometry_data();
    void _referenceGeometry();
    void _settingsAndValidation();
    void _sitePersistenceAndImport();
    void _windScaling_data();
    void _windScaling();
    void _groundWindLifecycle();
    void _rtkCalibration();
    void _rtkCalibrationFreshnessAndVehicleSwitch();
    void _manualSessionDoesNotChangeMission();
    void _phasesAndVoice();
    void _spinDirections_data();
    void _spinDirections();
    void _centreQualification_data();
    void _centreQualification();
    void _eightStartsEitherSide_data();
    void _eightStartsEitherSide();
    void _rightCircleDeparture_data();
    void _rightCircleDeparture();
    void _headingEvaluation();
    void _minimumFigureEightSpeed();
    void _voiceResources();
    void _voiceQueue();

private:
    void _ignoreMissingPressureBackend();
};

void CaacFigureEightControllerTest::_ignoreMissingPressureBackend()
{
    // The airborne transition probes the optional local pressure sensor on headless hosts.
    ignoreLogMessage("Utilities.QGCSensors", QtWarningMsg,
                     QRegularExpression(QStringLiteral("Failed to connect to pressure backend")));
    ignoreLogMessage("Utilities.QGCSensors", QtWarningMsg,
                     QRegularExpression(QStringLiteral("Error Initializing Pressure Sensor")));
}

void CaacFigureEightControllerTest::_rtkCalibration()
{
    QSettings().remove(QStringLiteral("CAAC/FigureEight"));
    CaacFigureEightController controller;
    QVERIFY(!controller.calibrationReady());
    QVERIFY(!controller.capturePointA());
    QVERIFY(!controller.pointA().isValid());
    _connectMockLink();
    QVERIFY(vehicle());
    QVERIFY(waitForInitialConnect());
    QTRY_COMPARE_WITH_TIMEOUT(vehicle()->_findMavCommandListEntryIndex(vehicle()->compId(), MAV_CMD_REQUEST_MESSAGE),
                              -1, TestTimeout::mediumMs());
    mockLink()->setCommLost(true);
    const QGeoCoordinate pointA(30, 120);
    const auto pointB = QGCGeo::geodesicDestination(pointA, 90, 24);
    const auto previousMode = vehicle()->flightMode();
    const auto previousMission = vehicle()->missionManager()->missionItems();
    QSignalSpy changed(&controller, &CaacFigureEightController::siteChanged);
    QVERIFY(changed.isValid());
    QVERIFY(reportGps(vehicle(), mockLink(), pointA, GPS_FIX_TYPE_RTK_FLOAT));
    QVERIFY(!controller.calibrationReady());
    QVERIFY(!controller.capturePointA());
    QVERIFY(reportGps(vehicle(), mockLink(), pointA, GPS_FIX_TYPE_RTK_FIXED, true));
    QVERIFY(!controller.calibrationReady());
    QVERIFY(reportGps(vehicle(), mockLink(), pointA, GPS_FIX_TYPE_RTK_FIXED));
    QVERIFY(controller.calibrationReady());
    QVERIFY(!vehicle()->armed());
    QVERIFY(!vehicle()->airborne());
    QVERIFY(controller.capturePointA());
    QCOMPARE(controller.pointA(), pointA);
    QVERIFY(!controller.pointB().isValid());
    QVERIFY(controller.referencePath().isEmpty());
    QVERIFY(!controller.capturePointB());
    QCOMPARE(controller.pointA(), pointA);
    QVERIFY(!controller.pointB().isValid());
    controller.setShowOnMap(false);
    QVERIFY(reportGps(vehicle(), mockLink(), pointB, GPS_FIX_TYPE_RTK_FIXED));
    QVERIFY(controller.capturePointB());
    QVERIFY(controller.showOnMap());
    QCOMPARE(controller.referencePath().size(), 241);
    QVERIFY(QGCGeo::geodesicDistance(controller.pointB(), pointB) < 0.01);
    QCOMPARE(changed.size(), 2);
    const auto path = controller.referencePath();
    QVERIFY(QGCGeo::geodesicDistance(path[60].value<QGeoCoordinate>(), pointA) < 0.01);
    QVERIFY(QGCGeo::geodesicDistance(path[180].value<QGeoCoordinate>(), pointB) < 0.01);
    QVERIFY(reportGps(vehicle(), mockLink(), pointB, GPS_FIX_TYPE_RTK_FLOAT));
    QVERIFY(!controller.capturePointA());
    QCOMPARE(controller.referencePath(), path);
    QVERIFY(reportGps(vehicle(), mockLink(), pointB, GPS_FIX_TYPE_RTK_FIXED));
    mavlink_message_t inAir{};
    mavlink_msg_extended_sys_state_pack(vehicle()->id(), vehicle()->compId(), &inAir, MAV_VTOL_STATE_UNDEFINED,
                                        MAV_LANDED_STATE_IN_AIR);
    _ignoreMissingPressureBackend();
    QVERIFY(QMetaObject::invokeMethod(vehicle(), "_mavlinkMessageReceived", Qt::DirectConnection,
                                      Q_ARG(LinkInterface*, mockLink()), Q_ARG(mavlink_message_t, inAir)));
    QVERIFY(vehicle()->airborne());
    QVERIFY(!controller.calibrationReady());
    QVERIFY(!controller.capturePointA());
    QCOMPARE(controller.referencePath(), path);
    QCOMPARE(vehicle()->flightMode(), previousMode);
    QCOMPARE(vehicle()->missionManager()->missionItems(), previousMission);
}

void CaacFigureEightControllerTest::_rtkCalibrationFreshnessAndVehicleSwitch()
{
    QSettings().remove(QStringLiteral("CAAC/FigureEight"));
    _connectMockLink();
    QVERIFY(vehicle());
    QVERIFY(waitForInitialConnect());
    QTRY_COMPARE_WITH_TIMEOUT(vehicle()->_findMavCommandListEntryIndex(vehicle()->compId(), MAV_CMD_REQUEST_MESSAGE),
                              -1, TestTimeout::mediumMs());
    mockLink()->setCommLost(true);
    // Keep link health independent of GPS freshness while the normal mock stream is paused.
    QTimer heartbeats;
    connect(&heartbeats, &QTimer::timeout, this, [this]() {
        if (!vehicle() || !mockLink()) {
            return;
        }
        mavlink_message_t heartbeat{};
        mavlink_msg_heartbeat_pack(vehicle()->id(), vehicle()->compId(), &heartbeat, MAV_TYPE_QUADROTOR,
                                   MAV_AUTOPILOT_PX4, 0, 0, MAV_STATE_STANDBY);
        vehicle()->vehicleLinkManager()->mavlinkMessageReceived(mockLink(), heartbeat);
    });
    heartbeats.start(200);
    CaacFigureEightController controller;
    const QGeoCoordinate pointA(30, 120);
    const auto pointB = QGCGeo::geodesicDestination(pointA, 90, 24);
    QVERIFY(reportGps(vehicle(), mockLink(), pointB, GPS_FIX_TYPE_RTK_FIXED));
    QVERIFY(controller.capturePointB());
    QVERIFY(!controller.pointA().isValid());
    QVERIFY(controller.pointB().isValid());
    auto* manager = MultiVehicleManager::instance();
    manager->setActiveVehicle(nullptr);
    QTRY_VERIFY_WITH_TIMEOUT(!controller.pointB().isValid(), TestTimeout::mediumMs());
    QVERIFY(!controller.calibrationReady());
    manager->setActiveVehicle(vehicle());
    QTRY_COMPARE_WITH_TIMEOUT(manager->activeVehicle(), vehicle(), TestTimeout::mediumMs());
    QVERIFY(reportGps(vehicle(), mockLink(), pointA, GPS_FIX_TYPE_RTK_FIXED));
    QVERIFY(controller.capturePointA());
    QVERIFY(reportGps(vehicle(), mockLink(), pointB, GPS_FIX_TYPE_RTK_FIXED));
    QVERIFY(controller.capturePointB());
    const auto path = controller.referencePath();
    manager->setActiveVehicle(nullptr);
    QTRY_COMPARE_WITH_TIMEOUT(manager->activeVehicle(), nullptr, TestTimeout::mediumMs());
    QCOMPARE(controller.referencePath(), path);
    manager->setActiveVehicle(vehicle());
    QTRY_COMPARE_WITH_TIMEOUT(manager->activeVehicle(), vehicle(), TestTimeout::mediumMs());
    QVERIFY(reportGps(vehicle(), mockLink(), pointB, GPS_FIX_TYPE_RTK_FIXED));
    QVERIFY(controller.capturePointB());
    QVERIFY(!controller.pointA().isValid());
    QVERIFY(controller.referencePath().isEmpty());
    QVERIFY(controller.calibrationReady());
    QTRY_VERIFY_WITH_TIMEOUT(!controller.calibrationReady(), TestTimeout::mediumMs());
    QVERIFY(!vehicle()->vehicleLinkManager()->communicationLost());
    QVERIFY(controller.calibrationStatus().contains(QStringLiteral("实时")));
    QVERIFY(!controller.capturePointA());
    QVERIFY(!controller.pointA().isValid());
}

void CaacFigureEightControllerTest::_windScaling_data()
{
    QTest::addColumn<double>("speed");
    QTest::addColumn<int>("level");
    QTest::addColumn<double>("factor");
    QTest::newRow("calm") << 0.0 << 1 << 1.0;
    QTest::newRow("level-one-boundary") << 4.0 << 1 << 1.0;
    QTest::newRow("level-two-start") << 4.0001 << 2 << 1.15;
    QTest::newRow("level-two-boundary") << 8.0 << 2 << 1.15;
    QTest::newRow("level-three-start") << 8.0001 << 3 << 1.3225;
}

void CaacFigureEightControllerTest::_windScaling()
{
    QFETCH(double, speed);
    QFETCH(int, level);
    QFETCH(double, factor);
    QSettings().remove(QStringLiteral("CAAC/FigureEight"));
    CaacFigureEightController controller;
    const auto base = controller.limits();
    QSignalSpy windChanged(&controller, &CaacFigureEightController::windChanged);
    QVERIFY(windChanged.isValid());
    controller.setManualWindSpeed(speed);
    QVERIFY(controller.windValid());
    QCOMPARE(controller.windLevel(), level);
    QVERIFY(std::abs(controller.windFactor() - factor) < 1e-10);
    QCOMPARE(controller.limits(), base);
    auto effective = controller.effectiveLimits();
    QVERIFY(std::abs(effective.value(QStringLiteral("horizontal")).toDouble() - 2 * factor) < 1e-10);
    QVERIFY(std::abs(effective.value(QStringLiteral("vertical")).toDouble() - factor) < 1e-10);
    QVERIFY(std::abs(effective.value(QStringLiteral("heading")).toDouble() - 30 * factor) < 1e-10);
    QCOMPARE(effective.value(QStringLiteral("maxSpeed")), base.value(QStringLiteral("maxSpeed")));
    QCOMPARE(effective.value(QStringLiteral("maxDuration")), base.value(QStringLiteral("maxDuration")));
    controller.setStandard(2);
    QVERIFY(std::abs(controller.effectiveLimits().value(QStringLiteral("heading")).toDouble() - 25 * factor) < 1e-10);
    QVERIFY(!windChanged.isEmpty());
    controller.setManualWindSpeed(std::numeric_limits<double>::quiet_NaN());
    QCOMPARE(controller.manualWindSpeed(), speed);
    controller.setManualWindSpeed(-1);
    QCOMPARE(controller.manualWindSpeed(), speed);
    controller.setManualWindSpeed(101);
    QCOMPARE(controller.manualWindSpeed(), speed);
    CaacFigureEightController restored;
    QCOMPARE(restored.manualWindSpeed(), speed);
    QCOMPARE(restored.standard(), 2);
    QCOMPARE(restored.windLevel(), level);
}

void CaacFigureEightControllerTest::_groundWindLifecycle()
{
    QSettings().remove(QStringLiteral("CAAC/FigureEight"));
    CaacFigureEightController controller;
    const QGeoCoordinate pointA(30, 120);
    QVERIFY(controller.setPoints(pointA, QGCGeo::geodesicDestination(pointA, 90, 24)));
    controller.setWindSource(1);
    QVERIFY(!controller.windValid());
    QCOMPARE(controller.windLevel(), 0);
    QCOMPARE(controller.windFactor(), 0);
    QVERIFY(!controller.start());
    QVERIFY(controller.updateGroundWind(6));
    QVERIFY(controller.windValid());
    QCOMPARE(controller.windSpeed(), 6);
    QCOMPARE(controller.windLevel(), 2);
    QCOMPARE(controller.limits().value(QStringLiteral("horizontal")).toDouble(), 2.0);
    QVERIFY(!controller.updateGroundWind(std::numeric_limits<double>::quiet_NaN()));
    QVERIFY(!controller.windValid());
    QVERIFY(controller.updateGroundWind(9));
    CaacFigureEightController restored;
    QCOMPARE(restored.windSource(), 1);
    QVERIFY(!restored.windValid());
    QVERIFY(std::isnan(restored.groundWindSpeed()));
    controller.setWindSource(0);
    QVERIFY(controller.windValid());
    QCOMPARE(controller.windLevel(), 1);
}

void CaacFigureEightControllerTest::_referenceGeometry_data()
{
    QTest::addColumn<QGeoCoordinate>("anchor");
    QTest::addColumn<double>("bearing");
    QTest::newRow("east") << QGeoCoordinate(30, 120) << 90.0;
    QTest::newRow("north") << QGeoCoordinate(30, 120) << 0.0;
    QTest::newRow("antimeridian") << QGeoCoordinate(0, 179.99995) << 90.0;
    QTest::newRow("high-latitude") << QGeoCoordinate(85, -120) << 235.0;
}

void CaacFigureEightControllerTest::_referenceGeometry()
{
    QFETCH(QGeoCoordinate, anchor);
    QFETCH(double, bearing);
    const auto pointB = QGCGeo::geodesicDestination(anchor, bearing, 24);
    const auto path = CaacFigureEightController::referenceCoordinates(anchor, pointB);
    QCOMPARE(path.size(), 241);
    const auto centre = QGCGeo::geodesicDestination(anchor, bearing, 12);
    QVERIFY(QGCGeo::geodesicDistance(path.first(), centre) < 0.01);
    QVERIFY(QGCGeo::geodesicDistance(path.last(), centre) < 0.01);
    QVERIFY(QGCGeo::geodesicDistance(path[120], centre) < 0.01);
    QVERIFY(QGCGeo::geodesicDistance(path[60], anchor) < 0.01);
    QVERIFY(QGCGeo::geodesicDistance(path[180], pointB) < 0.01);
    QVERIFY(std::abs(QGCGeo::pathLength(path) - 24 * std::numbers::pi) < 0.02);
    for (int circle = 0; circle < 2; ++circle) {
        const auto circleCentre = QGCGeo::geodesicDestination(anchor, bearing, circle == 0 ? 6 : 18);
        for (int i = circle * 120; i <= (circle + 1) * 120; ++i) {
            QVERIFY(std::abs(QGCGeo::geodesicDistance(path[i], circleCentre) - 6) < 0.01);
        }
    }
    QVERIFY(CaacFigureEightController::referenceCoordinates(anchor, anchor).isEmpty());
    QVERIFY(CaacFigureEightController::referenceCoordinates({}, pointB).isEmpty());
}

void CaacFigureEightControllerTest::_settingsAndValidation()
{
    QSettings().remove(QStringLiteral("CAAC/FigureEight"));
    CaacFigureEightController controller;
    QCOMPARE(controller.standard(), 0);
    QCOMPARE(controller.limits().value(QStringLiteral("heading")).toDouble(), 30.0);
    QVERIFY(!controller.limits().value(QStringLiteral("speedEnabled")).toBool());
    QVERIFY(!controller.limits().value(QStringLiteral("durationEnabled")).toBool());
    controller.setStandard(2);
    QCOMPARE(controller.limits().value(QStringLiteral("heading")).toDouble(), 25.0);
    const auto before = controller.limits();
    QVERIFY(!controller.setLimits({{QStringLiteral("horizontal"), std::numeric_limits<double>::quiet_NaN()}}));
    QVERIFY(!controller.setLimits({{QStringLiteral("minSpeed"), 5.0}, {QStringLiteral("maxSpeed"), 2.0}}));
    QVERIFY(!controller.setLimits({{QStringLiteral("speedEnabled"), QStringLiteral("yes")}}));
    QVERIFY(!controller.setLimits({{QStringLiteral("unknown"), 1}}));
    QCOMPARE(controller.limits(), before);
    QVERIFY(controller.setLimits({{QStringLiteral("horizontal"), 3.0}, {QStringLiteral("speedEnabled"), true}}));
    QVERIFY(controller.lastError().isEmpty());
    controller.setStandard(0);
    QCOMPARE(controller.limits().value(QStringLiteral("horizontal")).toDouble(), 2.0);
    QVERIFY(controller.setLimits({{QStringLiteral("horizontal"), 4.0}}));
    controller.setStandard(2);
    QCOMPARE(controller.limits().value(QStringLiteral("horizontal")).toDouble(), 3.0);
    {
        CaacFigureEightController restored;
        QCOMPARE(restored.standard(), 2);
        QCOMPARE(restored.limits().value(QStringLiteral("horizontal")).toDouble(), 3.0);
        QVERIFY(restored.limits().value(QStringLiteral("speedEnabled")).toBool());
        restored.setStandard(0);
        QCOMPARE(restored.limits().value(QStringLiteral("horizontal")).toDouble(), 4.0);
    }
    controller.resetLimits();
    QCOMPARE(controller.limits(), before);
    QVERIFY(!controller.start());
    QVERIFY(!controller.running());
    QVERIFY(!controller.lastError().isEmpty());
}

void CaacFigureEightControllerTest::_sitePersistenceAndImport()
{
    QSettings().remove(QStringLiteral("CAAC/FigureEight"));
    QTemporaryDir directory;
    QVERIFY(directory.isValid());
    const QGeoCoordinate pointA(30, 120);
    const auto pointB = QGCGeo::geodesicDestination(pointA, 90, 24);
    CaacFigureEightController controller;
    QVERIFY(controller.saveSite(QStringLiteral("训练场"), pointA, pointB));
    QCOMPARE(controller.sites().size(), 1);
    controller.setFenceWidth(2.0);
    QVERIFY(controller.setLimits({{QStringLiteral("horizontal"), 2.5}}));
    {
        CaacFigureEightController restored;
        QCOMPARE(restored.sites(), controller.sites());
        QCOMPARE(restored.selectedSite(), 0);
        QCOMPARE(restored.pointA(), pointA);
        QVERIFY(QGCGeo::geodesicDistance(restored.pointB(), pointB) < 0.001);
        QCOMPARE(restored.limits().value(QStringLiteral("horizontal")).toDouble(), 2.5);
        QCOMPARE(restored.fenceWidth(), 2.0);
    }
    const auto filename = directory.filePath(QStringLiteral("sites.json"));
    QVERIFY(controller.exportSites(filename));
    controller.removeSite(0);
    QCOMPARE(controller.sites().size(), 0);
    QVERIFY(controller.referencePath().isEmpty());
    QVERIFY(controller.importSites(filename));
    QCOMPARE(controller.sites().size(), 1);
    controller.selectSite(0);
    QCOMPARE(controller.referencePath().size(), 241);
    controller.selectSite(-1);
    QCOMPARE(controller.selectedSite(), -1);
    QCOMPARE(controller.referencePath().size(), 241);
    const auto before = controller.sites();
    QJsonArray invalidSites = QJsonArray::fromVariantList(before);
    invalidSites.append(QJsonObject{{QStringLiteral("name"), QStringLiteral("invalid")}});
    QFile invalidFile(directory.filePath(QStringLiteral("invalid.json")));
    QVERIFY(invalidFile.open(QIODevice::WriteOnly));
    const QJsonObject invalid{{QStringLiteral("fileType"), QStringLiteral("CAACFigureEightSites")},
                              {QStringLiteral("version"), 1},
                              {QStringLiteral("sites"), invalidSites}};
    QVERIFY(invalidFile.write(QJsonDocument(invalid).toJson()) > 0);
    invalidFile.close();
    QVERIFY(!controller.importSites(invalidFile.fileName()));
    QCOMPARE(controller.sites(), before);
    QVERIFY(!controller.saveSite(QString(), pointA, pointB));
    QVERIFY(!controller.setPoints(pointA, pointA));
    QCOMPARE(controller.sites(), before);
}

void CaacFigureEightControllerTest::_manualSessionDoesNotChangeMission()
{
    QSettings().remove(QStringLiteral("CAAC/FigureEight"));
    _connectMockLink();
    QVERIFY(vehicle());
    QVERIFY(waitForInitialConnect());
    QTRY_VERIFY_WITH_TIMEOUT(vehicle()->coordinate().isValid(), TestTimeout::mediumMs());
    _ignoreMissingPressureBackend();
    QVERIFY(takeoffAtExamHeight(vehicle()));
    QTRY_VERIFY_WITH_TIMEOUT(vehicle()->armed(), TestTimeout::mediumMs());
    QTRY_VERIFY_WITH_TIMEOUT(vehicle()->airborne(), TestTimeout::mediumMs());
    QTRY_VERIFY_WITH_TIMEOUT(std::abs(vehicle()->altitudeRelative()->rawValue().toDouble() - 1.75) < 0.01,
                             TestTimeout::mediumMs());
    QVERIFY(reportTakeoffReference(vehicle(), mockLink()));
    CaacFigureEightController controller;
    QTimer speedTelemetry;
    connect(&speedTelemetry, &QTimer::timeout, this, [this]() { reportSpeed(vehicle(), mockLink()); });
    speedTelemetry.start(100);
    QVERIFY(reportSpeed(vehicle(), mockLink()));
    const auto centre = vehicle()->coordinate();
    QVERIFY(controller.setPoints(QGCGeo::geodesicDestination(centre, 270, 12),
                                 QGCGeo::geodesicDestination(centre, 90, 12)));
    auto* mission = vehicle()->missionManager();
    const auto previousItems = mission->missionItems();
    const auto previousMode = vehicle()->flightMode();
    QSignalSpy uploaded(mission, &MissionManager::sendComplete);
    QVERIFY(uploaded.isValid());
    QTRY_COMPARE_WITH_TIMEOUT(controller.lastAnnouncement(), QStringLiteral("可以开始考试"), TestTimeout::mediumMs());
    QVERIFY2(controller.start(), qPrintable(controller.lastError()));
    QVERIFY(controller.running());
    QTRY_COMPARE_WITH_TIMEOUT(controller.phase(), QStringLiteral("八字飞行"), TestTimeout::longMs());
    QTRY_VERIFY_WITH_TIMEOUT(!controller.recordedPath().isEmpty(), TestTimeout::mediumMs());
    QTRY_VERIFY_WITH_TIMEOUT(controller.metrics().value(QStringLiteral("elapsed")).toDouble() > 0.2,
                             TestTimeout::mediumMs());
    QVERIFY(controller.metrics().value(QStringLiteral("telemetryValid")).toBool());
    QCOMPARE(controller.metrics().value(QStringLiteral("completedLaps")).toInt(), 0);
    QVERIFY(!controller.setPoints(centre, centre));
    QVERIFY(!controller.setLimits({{QStringLiteral("horizontal"), 3.0}}));
    const auto reference = controller.referencePath();
    const auto report = [&](int index) { return reportReferencePosition(vehicle(), mockLink(), controller, index); };
    QVERIFY(report(1));
    auto* trajectory = vehicle()->property("trajectoryPoints").value<TrajectoryPoints*>();
    QVERIFY(trajectory);
    QVERIFY(!trajectory->list().isEmpty());
    const auto metricsBeforeClear = controller.metrics();
    const auto pointABeforeClear = controller.pointA();
    const auto pointBBeforeClear = controller.pointB();
    controller.clearTrajectory();
    QVERIFY(controller.recordedPath().isEmpty());
    QVERIFY(trajectory->list().isEmpty());
    QVERIFY(controller.running());
    QCOMPARE(controller.metrics(), metricsBeforeClear);
    QCOMPARE(controller.referencePath(), reference);
    QCOMPARE(controller.pointA(), pointABeforeClear);
    QCOMPARE(controller.pointB(), pointBBeforeClear);
    QVERIFY(report(2));
    QVERIFY(!controller.recordedPath().isEmpty());
    QVERIFY(!trajectory->list().isEmpty());
    // Two visits to the same circle must not stand in for traversing both halves of an eight.
    for (int repeat = 0; repeat < 2; ++repeat) {
        for (int i = 1; i <= 120; ++i) {
            QVERIFY(report(i));
        }
    }
    QCOMPARE(controller.metrics().value(QStringLiteral("completedLaps")).toInt(), 0);
    controller.stop();
    QVERIFY(controller.start());
    QTRY_COMPARE_WITH_TIMEOUT(controller.phase(), QStringLiteral("八字飞行"), TestTimeout::longMs());
    for (int i = 1; i < reference.size(); ++i) {
        QVERIFY(report(i));
    }
    QVERIFY(controller.metrics().value(QStringLiteral("progress")).toDouble() > 0.99);
    QCOMPARE(controller.metrics().value(QStringLiteral("completedLaps")).toInt(), 1);
    controller.stop();
    QVERIFY(controller.start());
    QTRY_COMPARE_WITH_TIMEOUT(controller.phase(), QStringLiteral("八字飞行"), TestTimeout::longMs());
    for (int i = 1; i <= 60; ++i) {
        QVERIFY(report(i));
    }
    for (int i = 59; i >= 0; --i) {
        QVERIFY(report(i));
    }
    QCOMPARE(controller.metrics().value(QStringLiteral("completedLaps")).toInt(), 0);
    vehicle()->armedChanged(false);
    QVERIFY(!controller.running());
    QVERIFY(!controller.recordedPath().isEmpty());
    controller.setWindSource(1);
    QVERIFY(controller.updateGroundWind(6));
    QVERIFY(controller.start());
    QVERIFY(controller.updateGroundWind(9));
    QVERIFY(controller.running());
    QCOMPARE(controller.metrics().value(QStringLiteral("windSource")).toInt(), 1);
    QVERIFY(std::abs(controller.metrics().value(QStringLiteral("windFactor")).toDouble() - 1.3225) < 1e-10);
    QTRY_VERIFY_WITH_TIMEOUT(!controller.running(), TestTimeout::longMs());
    QVERIFY(!controller.windValid());
    QVERIFY(controller.status().contains(QStringLiteral("风速")));
    QVERIFY(!controller.metrics().value(QStringLiteral("windValid")).toBool());
    QCOMPARE(uploaded.count(), 0);
    QCOMPARE(mission->missionItems(), previousItems);
    QCOMPARE(vehicle()->flightMode(), previousMode);
}

void CaacFigureEightControllerTest::_phasesAndVoice()
{
    QSettings().remove(QStringLiteral("CAAC/FigureEight"));
    _connectMockLink();
    QVERIFY(vehicle());
    QVERIFY(waitForInitialConnect());
    _ignoreMissingPressureBackend();
    QVERIFY(takeoffAtExamHeight(vehicle()));
    QTRY_VERIFY_WITH_TIMEOUT(vehicle()->airborne(), TestTimeout::mediumMs());
    mockLink()->setVehicleAttitudeOverrideDeg(0, 0, 0);
    QVERIFY(reportTakeoffReference(vehicle(), mockLink()));
    CaacFigureEightController controller;
    controller.setExerciseMode(1);
    const auto centre = vehicle()->coordinate();
    QVERIFY(controller.setPoints(QGCGeo::geodesicDestination(centre, 270, 12),
                                 QGCGeo::geodesicDestination(centre, 90, 12)));
    QTimer speedTelemetry;
    connect(&speedTelemetry, &QTimer::timeout, this, [this]() { reportSpeed(vehicle(), mockLink()); });
    speedTelemetry.start(100);
    QVERIFY(reportSpeed(vehicle(), mockLink()));
    controller.setVoiceEnabled(false);
    QSignalSpy speech(&controller, &CaacFigureEightController::announcementRequested);
    QSignalSpy cleared(&controller, &CaacFigureEightController::alarmsCleared);
    QVERIFY(speech.isValid());
    QVERIFY(cleared.isValid());
    QTRY_COMPARE_WITH_TIMEOUT(controller.lastAnnouncement(), QStringLiteral("可以开始考试"), TestTimeout::mediumMs());
    QCOMPARE(speech.size(), 0);
    QVERIFY(controller.start());
    QCOMPARE(controller.phase(), QStringLiteral("中心筒就位"));
    QCOMPARE(controller.lastAnnouncement(), QStringLiteral("请将无人机飞至中心筒上方一点五米处，并保持对位悬停。"));
    QTRY_COMPARE_WITH_TIMEOUT(controller.phase(), QStringLiteral("原地自旋"), TestTimeout::longMs());
    const auto turnTo = [&](double heading) {
        mockLink()->setVehicleAttitudeOverrideDeg(0, 0, heading);
        vehicle()->heading()->setRawValue(heading);
        return reportPosition(vehicle(), mockLink(), controller, centre);
    };
    const auto finishTurn = [&](int firstStep) {
        for (int i = firstStep; i <= 12; ++i) {
            if (!turnTo(std::fmod(i * 30.0, 360))) {
                return false;
            }
        }
        return true;
    };
    QVERIFY(turnTo(330));
    QVERIFY(std::abs(controller.metrics().value(QStringLiteral("spinAngle")).toDouble() + 30) < 0.1);
    QVERIFY(std::abs(controller.metrics().value(QStringLiteral("evaluatedHeading")).toDouble() - 330) < 0.1);
    QVERIFY(turnTo(330.25));
    QCOMPARE(controller.attemptNumber(), 2);
    QCOMPARE(controller.lastAttemptFailure(), QStringLiteral("自旋中途回转"));
    QVERIFY(turnTo(0));
    QTRY_COMPARE_WITH_TIMEOUT(controller.phase(), QStringLiteral("原地自旋"), TestTimeout::longMs());
    for (const int heading : {60, 120, 180}) {
        QVERIFY(turnTo(heading));
        QVERIFY(std::abs(controller.metrics().value(QStringLiteral("headingError")).toDouble() - heading) < 0.1);
        QCOMPARE(controller.phase(), QStringLiteral("原地自旋"));
    }
    QVERIFY(turnTo(179.75));
    QVERIFY(controller.running());
    QCOMPARE(controller.attemptNumber(), 3);
    QCOMPARE(controller.attemptLimit(), 3);
    QCOMPARE(controller.lastAttemptFailure(), QStringLiteral("自旋中途回转"));
    QCOMPARE(speech.size(), 0);
    controller.stop();
    controller.setVoiceEnabled(true);
    controller.testVoice();
    QCOMPARE(speech.last()[0].toString(), QStringLiteral("ready"));
    controller.setExerciseMode(2);
    QVERIFY(turnTo(0));
    QVERIFY(controller.start());
    QCOMPARE(speech.last()[0].toString(), QStringLiteral("positioning"));
    QCOMPARE(controller.attemptNumber(), 1);
    QCOMPARE(controller.attemptLimit(), 3);
    QTRY_COMPARE_WITH_TIMEOUT(controller.phase(), QStringLiteral("原地自旋"), TestTimeout::longMs());
    QCOMPARE(speech.last()[0].toString(), QStringLiteral("spin"));
    QVERIFY(turnTo(30));
    QTRY_VERIFY_WITH_TIMEOUT(controller.metrics().value(QStringLiteral("phaseElapsed")).toDouble() >= 5.1,
                             TestTimeout::longMs());
    for (int step = 2; step < 12; ++step) {
        QVERIFY(turnTo(step * 30.0));
    }
    QVERIFY(turnTo(354.9));
    QVERIFY(!controller.spinPassed());
    QCOMPARE(controller.phase(), QStringLiteral("原地自旋"));
    QVERIFY(turnTo(355));
    QVERIFY(controller.running());
    QVERIFY(controller.spinPassed());
    QCOMPARE(controller.phase(), QStringLiteral("八字飞行"));
    QCOMPARE(speech[speech.size() - 2][0].toString(), QStringLiteral("spin_completed"));
    QCOMPARE(speech[speech.size() - 2][1].toString(), QStringLiteral("自旋考试通过。"));
    QCOMPARE(speech.last()[0].toString(), QStringLiteral("figure_eight"));
    QCOMPARE(controller.lastAnnouncement(), QStringLiteral("可以开始水平八字飞行。"));
    QVERIFY(controller.awaitingFigureEight());

    vehicle()->altitudeRelative()->setRawValue(3.0);
    QVERIFY(reportPosition(vehicle(), mockLink(), controller, centre));
    QCOMPARE(controller.attemptNumber(), 1);
    QVERIFY(controller.awaitingFigureEight());
    QVERIFY(reportReferencePosition(vehicle(), mockLink(), controller, 4));
    QCOMPARE(controller.attemptNumber(), 2);
    QCOMPARE(controller.phase(), QStringLiteral("中心筒就位"));
    QVERIFY(controller.spinPassed());
    QCOMPARE(speech.last()[0].toString(), QStringLiteral("retry"));
    QCOMPARE(speech[speech.size() - 2][0].toString(), QStringLiteral("altitude"));
    QVERIFY(controller.lastAttemptFailure().contains(QStringLiteral("高度偏差")));
    vehicle()->altitudeRelative()->setRawValue(1.75);
    QVERIFY(reportPosition(vehicle(), mockLink(), controller, centre));
    QTRY_COMPARE_WITH_TIMEOUT(controller.phase(), QStringLiteral("八字飞行"), TestTimeout::longMs());
    QCOMPARE(controller.attemptNumber(), 2);

    vehicle()->heading()->setRawValue(180);
    const auto reference = controller.referencePath();
    QVERIFY(reportPosition(vehicle(), mockLink(), controller, reference[4].value<QGeoCoordinate>()));
    QCOMPARE(controller.attemptNumber(), 3);
    QCOMPARE(controller.phase(), QStringLiteral("中心筒就位"));
    QVERIFY(controller.spinPassed());
    QVERIFY(controller.lastAttemptFailure().contains(QStringLiteral("航向偏差")));
    QVERIFY(turnTo(0));
    QTRY_COMPARE_WITH_TIMEOUT(controller.phase(), QStringLiteral("八字飞行"), TestTimeout::longMs());
    for (int i = 1; i < reference.size(); ++i) {
        QVERIFY(reportReferencePosition(vehicle(), mockLink(), controller, i));
    }
    QVERIFY(!controller.running());
    QCOMPARE(controller.metrics().value(QStringLiteral("completedLaps")).toInt(), 1);
    QCOMPARE(speech.last()[0].toString(), QStringLiteral("completed"));
    QCOMPARE(controller.lastAnnouncement(), QStringLiteral("考试通过。"));
    QCOMPARE(controller.attemptNumber(), 3);
    int spinPrompts = 0;
    for (const auto& cue : speech) {
        spinPrompts += cue[0].toString() == QStringLiteral("spin");
    }
    QCOMPARE(spinPrompts, 1);
    QVERIFY(!cleared.isEmpty());

    QVERIFY(controller.start());
    QVERIFY(!controller.spinPassed());
    for (int attempt = 1; attempt <= 3; ++attempt) {
        QCOMPARE(controller.attemptNumber(), attempt);
        QVERIFY(turnTo(0));
        QTRY_COMPARE_WITH_TIMEOUT(controller.phase(), QStringLiteral("原地自旋"), TestTimeout::longMs());
        QVERIFY(finishTurn(1));
    }
    QVERIFY(!controller.running());
    QCOMPARE(controller.attemptNumber(), 3);
    QCOMPARE(speech.last()[0].toString(), QStringLiteral("failed"));
    QCOMPARE(speech[speech.size() - 2][0].toString(), QStringLiteral("spin_too_fast"));
    QCOMPARE(controller.lastAnnouncement(), QStringLiteral("考试失败，三次机会已用完。"));
    QVERIFY(controller.status().contains(QStringLiteral("3 次机会已用完")));
    controller.setExerciseMode(1);
    QVERIFY(controller.setLimits({{QStringLiteral("speedEnabled"), true},
                                  {QStringLiteral("minSpeed"), 0.1},
                                  {QStringLiteral("maxSpeed"), 0.2}}));
    QVERIFY(controller.start());
    QVERIFY(turnTo(0));
    QTRY_COMPARE_WITH_TIMEOUT(controller.phase(), QStringLiteral("原地自旋"), TestTimeout::longMs());
    QVERIFY(turnTo(30));
    QTRY_VERIFY_WITH_TIMEOUT(controller.metrics().value(QStringLiteral("phaseElapsed")).toDouble() >= 5.1,
                             TestTimeout::longMs());
    QVERIFY(finishTurn(2));
    QVERIFY(!controller.running());
    QVERIFY(controller.spinPassed());
    QCOMPARE(speech.last()[0].toString(), QStringLiteral("spin_completed"));
    QCOMPARE(controller.lastAnnouncement(), QStringLiteral("自旋考试通过。"));
    controller.setExerciseMode(2);
    CaacFigureEightController restored;
    QCOMPARE(restored.exerciseMode(), 2);
    QVERIFY(restored.voiceEnabled());
    QCOMPARE(restored.limits().value(QStringLiteral("maxSpeed")).toDouble(), 0.2);
}


void CaacFigureEightControllerTest::_spinDirections_data()
{
    QTest::addColumn<int>("standard");
    QTest::addColumn<int>("mode");
    QTest::newRow("vlos-single-ccw") << 0 << 1;
    QTest::newRow("bvlos-single-ccw") << 1 << 1;
    QTest::newRow("vlos-combined-ccw") << 0 << 2;
    QTest::newRow("bvlos-combined-ccw") << 1 << 2;
    QTest::newRow("instructor-cw-then-ccw") << 2 << 1;
    QTest::newRow("instructor-ccw-then-cw-combined") << 2 << 2;
}

void CaacFigureEightControllerTest::_spinDirections()
{
    QFETCH(int, standard);
    QFETCH(int, mode);
    QSettings().remove(QStringLiteral("CAAC/FigureEight"));
    _connectMockLink();
    QVERIFY(vehicle());
    QVERIFY(waitForInitialConnect());
    _ignoreMissingPressureBackend();
    QVERIFY(takeoffAtExamHeight(vehicle()));
    QTRY_VERIFY_WITH_TIMEOUT(vehicle()->airborne(), TestTimeout::mediumMs());
    QVERIFY(reportTakeoffReference(vehicle(), mockLink()));
    CaacFigureEightController controller;
    controller.setStandard(standard);
    controller.setExerciseMode(mode);
    controller.setVoiceEnabled(true);
    const auto centre = vehicle()->coordinate();
    QVERIFY(controller.setPoints(QGCGeo::geodesicDestination(centre, 270, 12),
                                 QGCGeo::geodesicDestination(centre, 90, 12)));
    QTimer speedTelemetry;
    connect(&speedTelemetry, &QTimer::timeout, this, [this]() { reportSpeed(vehicle(), mockLink()); });
    speedTelemetry.start(100);
    QSignalSpy speech(&controller, &CaacFigureEightController::announcementRequested);
    const auto turnTo = [&](double heading) {
        mockLink()->setVehicleAttitudeOverrideDeg(0, 0, heading);
        vehicle()->heading()->setRawValue(heading);
        return reportPosition(vehicle(), mockLink(), controller, centre);
    };
    // Replay a single telemetry stream during the timed hold. Mixing queued mock
    // attitudes with the injected samples can replay the pre-turn heading.
    const auto holdHeading = [&](double heading) {
        const auto phase = controller.phase();
        const auto attempt = controller.attemptNumber();
        QElapsedTimer timer;
        timer.start();
        do {
            if (!turnTo(heading) || !controller.running() || controller.phase() != phase ||
                controller.attemptNumber() != attempt) {
                return false;
            }
            if (controller.metrics().value(QStringLiteral("phaseElapsed")).toDouble() >= 5.1) {
                return true;
            }
            QTest::qSleep(100);
        } while (timer.elapsed() < 9000);
        return false;
    };
    if (standard != 2) {
        QVERIFY(turnTo(0));
        QTRY_VERIFY_WITH_TIMEOUT(controller.running() || controller.start(), 6000);
        QTRY_COMPARE_WITH_TIMEOUT(controller.phase(), QStringLiteral("原地自旋"), 9000);
        QVERIFY2(holdHeading(300), qPrintable(controller.status() + controller.lastAttemptFailure()));
        for (const double heading : {300.0, 270.0, 210.0, 150.0, 90.0, 30.0, 5.1}) {
            QVERIFY(turnTo(heading));
            QVERIFY(controller.running());
            QVERIFY(!controller.spinPassed());
        }
        QVERIFY(std::abs(controller.metrics().value(QStringLiteral("spinAngle")).toDouble() + 354.9) < 0.01);
        const auto cuesBeforePass = speech.size();
        QVERIFY(turnTo(5));
        QVERIFY(controller.spinPassed());
        QCOMPARE(controller.running(), mode == 2);
        QCOMPARE(speech.size(), cuesBeforePass + (mode == 2 ? 2 : 1));
        QCOMPARE(speech[cuesBeforePass][0].toString(), QStringLiteral("spin_completed"));
        QCOMPARE(speech[cuesBeforePass][1].toString(), QStringLiteral("自旋考试通过。"));
        if (mode == 2) {
            QCOMPARE(controller.phase(), QStringLiteral("八字飞行"));
            QCOMPARE(speech.last()[0].toString(), QStringLiteral("figure_eight"));
            controller.stop();
        } else {
            QVERIFY(controller.metrics().value(QStringLiteral("progress")).toDouble() > 0.99999);
        }
        return;
    }
    const auto instructorHeading = [mode](double heading) {
        return mode == 1 ? heading : std::fmod(360.0 - heading, 360.0);
    };
    const auto turnInstructorTo = [&](double heading) { return turnTo(instructorHeading(heading)); };
    QVERIFY(turnInstructorTo(0));
    QTRY_VERIFY_WITH_TIMEOUT(controller.running() || controller.start(), 6000);
    QTRY_COMPARE_WITH_TIMEOUT(controller.phase(), QStringLiteral("原地自旋"), 9000);
    QVERIFY2(holdHeading(instructorHeading(30)), qPrintable(controller.status() + controller.lastAttemptFailure()));
    for (const int heading : {90, 150, 210, 270, 330, 355}) {
        QVERIFY(turnInstructorTo(heading));
    }
    QCOMPARE(controller.phase(), QStringLiteral("反向原地自旋"));
    QCOMPARE(speech.last()[0].toString(), mode == 1 ? QStringLiteral("spin_reverse") : QStringLiteral("spin_reverse_cw"));
    QVERIFY(turnInstructorTo(0));  // The aircraft continues in the first direction past the 355-degree pass threshold.
    QCOMPARE(controller.metrics().value(QStringLiteral("phaseElapsed")).toDouble(), 0.0);
    QVERIFY(turnInstructorTo(348));
    QVERIFY(turnInstructorTo(350));
    QVERIFY(turnInstructorTo(353));
    QVERIFY(turnInstructorTo(357.9));
    QCOMPARE(controller.attemptNumber(), 1);
    if (mode == 1) {
        QVERIFY(turnInstructorTo(358));  // Ten degrees of retreat from 348 across several samples.
        QCOMPARE(controller.attemptNumber(), 2);
        QCOMPARE(controller.lastAttemptFailure(), QStringLiteral("自旋中途回转"));
        QCOMPARE(speech[speech.size() - 2][0].toString(), QStringLiteral("spin_reversal"));
        QVERIFY(turnInstructorTo(0));
        QTRY_COMPARE_WITH_TIMEOUT(controller.phase(), QStringLiteral("原地自旋"), 9000);
        QVERIFY2(holdHeading(instructorHeading(30)), qPrintable(controller.status() + controller.lastAttemptFailure()));
        for (int step = 2; step <= 12; ++step) {
            QVERIFY(turnInstructorTo(std::fmod(step * 30.0, 360)));
        }
        QCOMPARE(controller.phase(), QStringLiteral("反向原地自旋"));
        QVERIFY(turnInstructorTo(348));
    }
    QVERIFY2(holdHeading(instructorHeading(mode == 1 ? 348.0 : 357.9)),
             qPrintable(controller.status() + controller.lastAttemptFailure()));
    for (const double heading : {300.0, 240.0, 180.0, 120.0, 60.0, 5.1}) {
        QVERIFY(turnInstructorTo(heading));
    }
    QVERIFY(!controller.spinPassed());
    const auto beforePass = speech.size();
    QVERIFY(turnInstructorTo(5));
    QVERIFY(controller.spinPassed());
    QVERIFY(speech.size() > beforePass);
    QCOMPARE(speech[beforePass][0].toString(), QStringLiteral("spin_completed"));
    QCOMPARE(controller.running(), mode == 2);
    if (mode == 2) {
        QCOMPARE(controller.phase(), QStringLiteral("八字飞行"));
        QCOMPARE(speech.last()[0].toString(), QStringLiteral("figure_eight"));
        controller.stop();
    }
}

void CaacFigureEightControllerTest::_centreQualification_data()
{
    QTest::addColumn<bool>("quaternionStream");
    QTest::addColumn<double>("qualifiedHeight");
    QTest::addColumn<double>("qualifiedSpeed");
    QTest::addColumn<double>("qualifiedClimb");
    QTest::addColumn<double>("siteBearing");
    QTest::addColumn<int>("exerciseMode");
    QTest::addColumn<double>("takeoffDistance");
    QTest::newRow("spin-normal-80-speed-and-climb-at-0.3mps") << true << 1.5 << 0.3 << 0.3 << 170.0 << 1 << 8.0;
    QTest::newRow("eight-normal-80-speed-and-climb-at-0.3mps") << true << 1.5 << 0.3 << 0.3 << 170.0 << 0 << 8.0;
    QTest::newRow("normal-350-speed-and-descent-above-2m") << true << 2.32 << 0.3 << -0.3 << 80.0 << 0 << 8.0;
    QTest::newRow("normal-0-euler-after-quaternion-at-1.5m") << false << 1.5 << 0.0 << 0.0 << 90.0 << 2 << 8.0;
    QTest::newRow("takeoff-too-close-use-field-heading") << true << 1.91 << 0.0 << 0.0 << 170.0 << 0 << 0.2;
    QTest::newRow("spin-arming-point-opposite-map-marker") << true << 2.28 << 0.08 << 0.0 << 270.0 << 1 << 8.0;
    QTest::newRow("combined-arming-point-opposite-map-marker") << true << 2.28 << 0.08 << 0.0 << 270.0 << 2 << 8.0;
}

void CaacFigureEightControllerTest::_centreQualification()
{
    QFETCH(bool, quaternionStream);
    QFETCH(double, qualifiedHeight);
    QFETCH(double, qualifiedSpeed);
    QFETCH(double, qualifiedClimb);
    QFETCH(double, siteBearing);
    QFETCH(int, exerciseMode);
    QFETCH(double, takeoffDistance);
    QSettings().remove(QStringLiteral("CAAC/FigureEight"));
    _connectMockLink();
    QVERIFY(vehicle());
    QVERIFY(waitForInitialConnect());
    QTRY_VERIFY_WITH_TIMEOUT(vehicle()->coordinate().isValid(), TestTimeout::mediumMs());
    CaacFigureEightController controller;
    controller.setExerciseMode(exerciseMode);
    const auto centre = vehicle()->coordinate();
    QVERIFY(controller.setPoints(QGCGeo::geodesicDestination(centre, siteBearing + 180, 12),
                                 QGCGeo::geodesicDestination(centre, siteBearing, 12)));
    const double idealHeading = std::fmod(siteBearing + 270, 360);
    QGeoCoordinate reportedCoordinate = centre;
    double reportedHeight = 0;
    double reportedHeading = idealHeading;
    double reportedSpeed = 0;
    double reportedClimb = 0;
    bool reportQuaternion = true;
    bool reportTelemetry = true;
    int eulerSamples = 0;
    const auto report = [&]() {
        if (!quaternionStream) {
            mavlink_message_t heartbeat{};
            mavlink_msg_heartbeat_pack(vehicle()->id(), vehicle()->compId(), &heartbeat, MAV_TYPE_QUADROTOR,
                                       vehicle()->firmwareType(), vehicle()->baseMode(), vehicle()->customMode(),
                                       MAV_STATE_ACTIVE);
            vehicle()->vehicleLinkManager()->mavlinkMessageReceived(mockLink(), heartbeat);
        }
        if (!reportTelemetry) {
            return true;
        }
        mockLink()->setVehicleAttitudeOverrideDeg(0, 0, reportedHeading);
        vehicle()->heading()->setRawValue(reportedHeading);
        vehicle()->altitudeRelative()->setRawValue(reportedHeight);
        if (!reportQuaternion) {
            ++eulerSamples;
        }
        return reportPosition(vehicle(), mockLink(), controller, reportedCoordinate, reportedSpeed, reportedClimb,
                              reportQuaternion);
    };
    QTimer telemetry;
    connect(&telemetry, &QTimer::timeout, this, [&]() { report(); });
    telemetry.start(100);
    QTRY_VERIFY_WITH_TIMEOUT(controller.running() || controller.start(), TestTimeout::mediumMs());
    QCOMPARE(controller.phase(), QStringLiteral("中心筒就位"));
    QCOMPARE(controller.attemptNumber(), 1);
    _ignoreMissingPressureBackend();
    QVERIFY(takeoffAtExamHeight(vehicle()));
    QTRY_VERIFY_WITH_TIMEOUT(vehicle()->airborne(), TestTimeout::mediumMs());
    QVERIFY(reportTakeoffReference(vehicle(), mockLink(), false, takeoffDistance));
    const auto rejected = [&]() {
        return report() && controller.phase() == QStringLiteral("中心筒就位") &&
               controller.metrics().value(QStringLiteral("hoverStableSeconds")).toDouble() == 0 &&
               !controller.metrics().value(QStringLiteral("positioningPending")).toStringList().isEmpty();
    };
    reportedHeight = 1.49;
    QVERIFY(rejected());
    reportedHeight = 2.32;
    reportedCoordinate = QGCGeo::geodesicDestination(centre, 90, 1.02);
    QVERIFY(rejected());
    reportedCoordinate = centre;
    reportedHeading = std::fmod(idealHeading + 16, 360);
    QVERIFY(rejected());
    reportedHeading = std::fmod(idealHeading + 344, 360);
    QVERIFY(rejected());
    reportedHeading = std::fmod(idealHeading + 180, 360);
    QVERIFY(rejected());
    QVERIFY(std::abs(controller.metrics().value(QStringLiteral("headingError")).toDouble() - 180) < 0.01);
    if (siteBearing == 270) {
        QTest::qWait(1200);  // Keep sending the reversed heading for longer than the qualification hold.
        QVERIFY(rejected());
        QCOMPARE(controller.attemptNumber(), 1);
    }
    reportedHeading = idealHeading;
    reportedSpeed = 0.31;
    QVERIFY(rejected());
    reportedSpeed = 0;
    reportedClimb = 0.31;
    QVERIFY(rejected());
    reportedClimb = -0.31;
    QVERIFY(rejected());
    reportedClimb = 0;
    reportedHeight = 1.5;
    QVERIFY(report());
    QVERIFY(std::abs(std::remainder(
                controller.metrics().value(QStringLiteral("idealHeading")).toDouble() - idealHeading, 360)) < 0.01);
    const double fieldHeading = controller.metrics().value(QStringLiteral("idealHeading")).toDouble();
    reportedCoordinate = QGCGeo::geodesicDestination(centre, idealHeading + 90, 0.5);
    QVERIFY(report());
    QVERIFY(std::abs(std::remainder(
                controller.metrics().value(QStringLiteral("idealHeading")).toDouble() - idealHeading, 360)) < 0.01);
    reportedCoordinate = centre;
    QTRY_VERIFY_WITH_TIMEOUT(controller.metrics().value(QStringLiteral("hoverStableSeconds")).toDouble() >= 0.4,
                             TestTimeout::mediumMs());
    reportedCoordinate = QGCGeo::geodesicDestination(centre, 90, 1.02);
    QVERIFY(report());
    QCOMPARE(controller.metrics().value(QStringLiteral("hoverStableSeconds")).toDouble(), 0.0);
    QCOMPARE(controller.phase(), QStringLiteral("中心筒就位"));
    if (!quaternionStream) {
        mockLink()->setCommLost(true);
        reportQuaternion = false;
        QTRY_VERIFY_WITH_TIMEOUT(eulerSamples >= 10, TestTimeout::mediumMs());
        QVERIFY2(controller.running(), qPrintable(controller.status()));
    }
    reportedCoordinate = centre;
    reportedHeight = qualifiedHeight;
    reportedHeading = std::fmod(fieldHeading + 345, 360);
    QElapsedTimer qualificationHold;
    qualificationHold.start();
    QVERIFY(report());
    QVERIFY(controller.metrics().value(QStringLiteral("positioningPending")).toStringList().isEmpty());
    reportedHeading = std::fmod(fieldHeading + 15, 360);
    reportedSpeed = qualifiedSpeed;
    reportedClimb = qualifiedClimb;
    QVERIFY(report());
    const auto pending = controller.metrics().value(QStringLiteral("positioningPending")).toStringList();
    const auto qualificationDetails = [&]() {
        return QJsonDocument::fromVariant(controller.metrics()).toJson(QJsonDocument::Compact);
    };
    QVERIFY2(pending.isEmpty(), qualificationDetails().constData());
    const auto qualifiedPhase = exerciseMode == 0 ? QStringLiteral("八字飞行") : QStringLiteral("原地自旋");
    QTRY_VERIFY2_WITH_TIMEOUT(!controller.running() || controller.phase() == qualifiedPhase,
                              qualificationDetails().constData(), 2000);
    QVERIFY2(controller.running(), qPrintable(controller.status()));
    QCOMPARE(controller.phase(), qualifiedPhase);
    QVERIFY(qualificationHold.elapsed() >= 1000);
    QCOMPARE(controller.metrics().value(QStringLiteral("hoverStableSeconds")).toDouble(), 1.0);
    QCOMPARE(controller.attemptNumber(), 1);
    if (!quaternionStream) {
        reportTelemetry = false;
        QTRY_VERIFY_WITH_TIMEOUT(!controller.running(), TestTimeout::mediumMs());
        QVERIFY(controller.status().contains(QStringLiteral("遥测")));
        QVERIFY(!controller.metrics().value(QStringLiteral("telemetryValid")).toBool());
    }
    controller.stop();
}

void CaacFigureEightControllerTest::_eightStartsEitherSide_data()
{
    QTest::addColumn<int>("standard");
    QTest::addColumn<bool>("rightFirst");
    QTest::addColumn<bool>("oppositeArmingSide");
    QTest::addColumn<QString>("directionFault");
    for (const int standard : {0, 1, 2}) {
        for (const bool right : {false, true}) {
            QTest::newRow(
                qPrintable(QStringLiteral("category-%1-%2-first").arg(standard).arg(right ? "right" : "left")))
                << standard << right << false << QString();
        }
    }
    QTest::newRow("right-first-opposite-takeoff-side") << 0 << true << true << QString();
    QTest::newRow("instructor-opposite-takeoff-side") << 2 << true << true << QString();
    QTest::newRow("instructor-left-nose-first-exit") << 2 << false << false << QStringLiteral("noseExit");
    QTest::newRow("instructor-right-nose-first-exit") << 2 << true << false << QStringLiteral("noseExit");
    QTest::newRow("instructor-tail-first-wrong-side-exit") << 2 << false << false << QStringLiteral("wrongSide");
    QTest::newRow("instructor-nose-first-mid-circle") << 2 << true << false << QStringLiteral("midCircle");
}

void CaacFigureEightControllerTest::_eightStartsEitherSide()
{
    QFETCH(int, standard);
    QFETCH(bool, rightFirst);
    QFETCH(bool, oppositeArmingSide);
    QFETCH(QString, directionFault);
    QSettings().remove(QStringLiteral("CAAC/FigureEight"));
    _connectMockLink();
    QVERIFY(vehicle());
    QVERIFY(waitForInitialConnect());
    _ignoreMissingPressureBackend();
    QVERIFY(takeoffAtExamHeight(vehicle()));
    QTRY_VERIFY_WITH_TIMEOUT(vehicle()->airborne(), TestTimeout::mediumMs());
    QVERIFY(reportTakeoffReference(vehicle(), mockLink(), oppositeArmingSide));
    CaacFigureEightController controller;
    controller.setStandard(standard);
    QCOMPARE(controller.attemptLimit(), 3);
    QVERIFY(controller.setLimits({{QStringLiteral("speedEnabled"), true}, {QStringLiteral("minSpeed"), 0.5}}));
    if (!directionFault.isEmpty()) {
        // A permissive tangent limit must never authorize nose-first flight.
        QVERIFY(controller.setLimits({{QStringLiteral("heading"), 180.0}}));
    }
    const auto centre = vehicle()->coordinate();
    QVERIFY(controller.setPoints(QGCGeo::geodesicDestination(centre, 270, 12),
                                 QGCGeo::geodesicDestination(centre, 90, 12)));
    QTimer speedTelemetry;
    connect(&speedTelemetry, &QTimer::timeout, this, [this]() { reportSpeed(vehicle(), mockLink()); });
    speedTelemetry.start(100);
    QTRY_VERIFY_WITH_TIMEOUT(controller.running() || controller.start(), TestTimeout::mediumMs());
    if (oppositeArmingSide) {
        QTest::qWait(1200);
        QCOMPARE(controller.phase(), QStringLiteral("中心筒就位"));
        QVERIFY(std::abs(controller.metrics().value(QStringLiteral("headingError")).toDouble() - 180) < 0.05);
        controller.stop();
        controller.setStandard(standard == 2 ? 0 : 2);
        controller.setStandard(standard);
        QVERIFY(controller.start());
        QCOMPARE(controller.phase(), QStringLiteral("中心筒就位"));
        QVERIFY(std::abs(controller.metrics().value(QStringLiteral("headingError")).toDouble() - 180) < 0.05);
        mockLink()->setVehicleAttitudeOverrideDeg(0, 0, 0);
        vehicle()->heading()->setRawValue(0);
        QVERIFY(reportPosition(vehicle(), mockLink(), controller, centre));
    }
    QTRY_COMPARE_WITH_TIMEOUT(controller.phase(), QStringLiteral("八字飞行"), TestTimeout::longMs());
    QVERIFY(controller.awaitingFigureEight());
    vehicle()->altitudeRelative()->setRawValue(4.0);
    vehicle()->heading()->setRawValue(180);
    QVERIFY(reportPosition(vehicle(), mockLink(), controller, QGCGeo::geodesicDestination(centre, 90, 0.17), 0));
    QVERIFY(controller.running());
    QVERIFY(controller.awaitingFigureEight());
    QCOMPARE(controller.metrics().value(QStringLiteral("violationSamples")).toInt(), 0);
    QVERIFY(std::abs(std::remainder(controller.metrics().value(QStringLiteral("idealHeading")).toDouble(), 360)) < 0.05);
    QVERIFY(std::abs(controller.metrics().value(QStringLiteral("headingError")).toDouble() - 180) < 0.05);
    vehicle()->altitudeRelative()->setRawValue(1.75);
    for (int step = 1; step <= 240; ++step) {
        const int index = ((rightFirst ? 120 : 0) + step - 1) % 240 + 1;
        if ((step == 4 && directionFault == QStringLiteral("noseExit")) ||
            (step == 18 && directionFault == QStringLiteral("midCircle"))) {
            const auto path = controller.referencePath();
            const auto from = path[index - 1].value<QGeoCoordinate>();
            const auto to = path[step == 4 ? index : index - 2].value<QGeoCoordinate>();
            vehicle()->heading()->setRawValue(QGCGeo::geodesicAzimuth(from, to));
            QVERIFY(reportPosition(vehicle(), mockLink(), controller, to, 1.0));
            QCOMPARE(controller.attemptNumber(), 2);
            QVERIFY2(controller.lastAttemptFailure().contains(QStringLiteral("机头朝前")), qPrintable(controller.status()));
            return;
        }
        if (directionFault == QStringLiteral("wrongSide")) {
            vehicle()->heading()->setRawValue(180);
            const auto to = QGCGeo::geodesicDestination(centre, 0, step * 0.31);
            QVERIFY(reportPosition(vehicle(), mockLink(), controller, to, 1.0));
            if (step == 4) {
                QCOMPARE(controller.attemptNumber(), 2);
                QVERIFY(controller.lastAttemptFailure().contains(QStringLiteral("机尾未朝起飞位置")));
                return;
            }
            continue;
        }
        QVERIFY(reportReferencePosition(vehicle(), mockLink(), controller, index));
        if (step <= 3) {
            QVERIFY(controller.awaitingFigureEight());
        } else if (step < 240) {
            QVERIFY(!controller.awaitingFigureEight());
            QVERIFY2(controller.running(), qPrintable(controller.status()));
        }
    }
    QVERIFY(!controller.running());
    QCOMPARE(controller.metrics().value(QStringLiteral("completedLaps")).toInt(), 1);
    QCOMPARE(controller.lastAnnouncement(), QStringLiteral("考试通过。"));
}

void CaacFigureEightControllerTest::_rightCircleDeparture_data()
{
    QTest::addColumn<bool>("secondTrace");
    QTest::addColumn<bool>("excessYaw");
    QTest::newRow("field-departure-1") << false << false;
    QTest::newRow("field-departure-2") << true << false;
    QTest::newRow("right-circle-heading-still-scored") << true << true;
}

void CaacFigureEightControllerTest::_rightCircleDeparture()
{
    QFETCH(bool, secondTrace);
    QFETCH(bool, excessYaw);
    QSettings().remove(QStringLiteral("CAAC/FigureEight"));
    _connectMockLink();
    QVERIFY(vehicle());
    QVERIFY(waitForInitialConnect());
    _ignoreMissingPressureBackend();
    QVERIFY(takeoffAtExamHeight(vehicle()));
    QTRY_VERIFY_WITH_TIMEOUT(vehicle()->airborne(), TestTimeout::mediumMs());
    QVERIFY(reportTakeoffReference(vehicle(), mockLink()));

    CaacFigureEightController controller;
    const auto centre = vehicle()->coordinate();
    QVERIFY(controller.setPoints(QGCGeo::geodesicDestination(centre, 270, 12.063),
                                 QGCGeo::geodesicDestination(centre, 90, 12.063)));
    QTimer speedTelemetry;
    connect(&speedTelemetry, &QTimer::timeout, this, [this]() { reportSpeed(vehicle(), mockLink()); });
    speedTelemetry.start(100);
    QTRY_VERIFY_WITH_TIMEOUT(controller.running() || controller.start(), TestTimeout::mediumMs());
    QTRY_VERIFY_WITH_TIMEOUT(controller.awaitingFigureEight(), TestTimeout::longMs());

    struct Sample {
        double range;
        double azimuth;
        double yaw;
    };
    // Field positions and yaw rotated into the test site's north-facing frame.
    const QList<Sample> firstTrace{{0.835, -2.06, 0}, {0.975, -2.84, 4.4}, {1.13, -3.11, 9.5},
                                   {1.289, -2.95, 14}, {1.458, -2.49, 17}, {1.636, -1.83, 21},
                                   {1.822, -1.04, 24.9}, {2.026, -0.07, 29.3}, {2.225, 0.98, 32.7}};
    const QList<Sample> nextTrace{{0.794, -40.74, 12.6}, {0.907, -33.78, 13.5}, {1.029, -28.25, 13.7},
                                  {1.166, -23.61, 14.8}, {1.314, -19.73, 16.3}, {1.481, -16.38, 19.1},
                                  {1.649, -13.86, 21.9}, {1.826, -11.91, 24.7}, {2.017, -10.15, 27.4},
                                  {2.201, -8.47, 29.2}, {2.385, -6.74, 30.1}, {2.576, -5.16, 31.1},
                                  {2.775, -3.74, 32.5}, {2.981, -2.32, 33.3}, {3.202, -0.68, 34.6},
                                  {3.41, 0.92, 36.2}};
    const auto& trace = secondTrace ? nextTrace : firstTrace;
    for (const auto& sample : trace) {
        vehicle()->heading()->setRawValue(sample.yaw + (excessYaw && sample.range > 1 ? 50 : 0));
        QVERIFY(reportPosition(vehicle(), mockLink(), controller,
                               QGCGeo::geodesicDestination(centre, sample.azimuth + 360, sample.range), 0.85));
        if (sample.range > 1 && excessYaw) {
            QCOMPARE(controller.attemptNumber(), 2);
            QCOMPARE(controller.lastAttemptFailure(), QStringLiteral("航向偏差超限"));
            return;
        }
        QCOMPARE(controller.attemptNumber(), 1);
        if (sample.range > 1) {
            QVERIFY(!controller.awaitingFigureEight());
            QVERIFY(controller.metrics().value(QStringLiteral("headingError")).toDouble() < 15);
        }
    }
    controller.stop();
}

void CaacFigureEightControllerTest::_headingEvaluation()
{
    QSettings().remove(QStringLiteral("CAAC/FigureEight"));
    _connectMockLink();
    QVERIFY(vehicle());
    QVERIFY(waitForInitialConnect());
    _ignoreMissingPressureBackend();
    QVERIFY(takeoffAtExamHeight(vehicle()));
    QTRY_VERIFY_WITH_TIMEOUT(vehicle()->airborne(), TestTimeout::mediumMs());
    QVERIFY(reportTakeoffReference(vehicle(), mockLink()));
    CaacFigureEightController controller;
    QTimer speedTelemetry;
    connect(&speedTelemetry, &QTimer::timeout, this, [this]() { reportSpeed(vehicle(), mockLink()); });
    speedTelemetry.start(100);
    const auto origin = vehicle()->coordinate();
    // Keep the forward traversal in this test. The opposite takeoff side is covered separately.
    const double siteBearings[] = {90, 35, 80};
    const double alignmentHeadings[] = {0, 305, 350};
    for (const int standard : {0, 1, 2}) {
        controller.setStandard(standard);
        const double bearing = siteBearings[standard];
        const double alignmentHeading = alignmentHeadings[standard];
        QVERIFY(controller.setPoints(QGCGeo::geodesicDestination(origin, bearing + 180, 12),
                                     QGCGeo::geodesicDestination(origin, bearing, 12)));
        const auto path = controller.referencePath();
        const double centreHeading = bearing + (standard == 2 ? 90 : -90);
        for (const int index : {1, 121}) {
            QVERIFY(std::abs(std::remainder(
                        QGCGeo::geodesicAzimuth(origin, path[index].value<QGeoCoordinate>()) - centreHeading, 360)) <
                    2);
        }
        QVERIFY(reportPosition(vehicle(), mockLink(), controller, origin));
        mockLink()->setVehicleAttitudeOverrideDeg(0, 0, alignmentHeading);
        vehicle()->heading()->setRawValue(alignmentHeading);
        QTRY_VERIFY_WITH_TIMEOUT(controller.running() || controller.start(), TestTimeout::mediumMs());
        QTRY_COMPARE_WITH_TIMEOUT(controller.phase(), QStringLiteral("八字飞行"), TestTimeout::longMs());
        for (int i = 1; i < path.size(); ++i) {
            const bool leftCircle = i <= 120;
            const auto circleCentre = QGCGeo::geodesicDestination(origin, bearing + (leftCircle ? 180 : 0), 6);
            const auto coordinate = path[i].value<QGeoCoordinate>();
            const QGeoCoordinate reportedCoordinate(qRound64(coordinate.latitude() * 1e7) * 1e-7,
                                                    qRound64(coordinate.longitude() * 1e7) * 1e-7);
            const bool clockwise = standard == 2 ? leftCircle : !leftCircle;
            const double ideal = std::fmod(
                QGCGeo::geodesicAzimuth(circleCentre, reportedCoordinate) + (clockwise ? 90 : -90) + 360, 360);
            const double bodyHeading = std::fmod(ideal + 15.25 + (standard == 2 ? 180 : 0), 360);
            vehicle()->heading()->setRawValue(bodyHeading);
            QVERIFY(reportPosition(vehicle(), mockLink(), controller, coordinate, 1));
            const auto metrics = controller.metrics();
            const double measuredIdeal = metrics.value(QStringLiteral("idealHeading")).toDouble();
            // At the shared centre either contiguous circle can supply the tangent after GPS quantization.
            const bool waiting = controller.awaitingFigureEight();
            if (waiting) {
                QVERIFY(std::abs(std::remainder(measuredIdeal - alignmentHeading, 360)) < 0.01);
            } else if (i % 120 != 0) {
                QVERIFY2(std::abs(std::remainder(measuredIdeal - ideal, 360)) < 0.01,
                         qPrintable(QStringLiteral("standard %1, point %2: ideal %3, received %4")
                                        .arg(standard)
                                        .arg(i)
                                        .arg(ideal)
                                        .arg(measuredIdeal)));
            }
            const double expectedError =
                std::abs(std::remainder(bodyHeading + (!waiting && standard == 2 ? 180 : 0) - measuredIdeal, 360));
            QVERIFY(std::abs(metrics.value(QStringLiteral("headingError")).toDouble() - expectedError) < 0.01);
            if (i < path.size() - 1) {
                QVERIFY(controller.running());
            }
        }
        QVERIFY(!controller.running());
        QCOMPARE(controller.lastAnnouncement(), QStringLiteral("考试通过。"));
    }
    for (const bool highSpeed : {true, false}) {
        QVERIFY(controller.setLimits({{QStringLiteral("speedEnabled"), true},
                                      {QStringLiteral("minSpeed"), highSpeed ? 0.0 : 1.0},
                                      {QStringLiteral("maxSpeed"), highSpeed ? 1.0 : 3.0}}));
        QVERIFY(reportPosition(vehicle(), mockLink(), controller, origin));
        const double alignmentHeading = alignmentHeadings[2];
        mockLink()->setVehicleAttitudeOverrideDeg(0, 0, alignmentHeading);
        vehicle()->heading()->setRawValue(alignmentHeading);
        QVERIFY(controller.start());
        QTRY_COMPARE_WITH_TIMEOUT(controller.phase(), QStringLiteral("八字飞行"), TestTimeout::longMs());
        if (highSpeed) {
            QVERIFY(reportReferencePosition(vehicle(), mockLink(), controller, 4));
            vehicle()->groundSpeed()->setRawValue(2.0);
            QVERIFY(QMetaObject::invokeMethod(&controller, "_sample", Qt::DirectConnection));
        } else {
            QVERIFY(reportReferencePosition(vehicle(), mockLink(), controller, 4, 0.5));
        }
        QVERIFY(controller.running());
        QCOMPARE(controller.attemptNumber(), 2);
        QCOMPARE(controller.lastAttemptFailure(), QStringLiteral("自定义速度超限"));
        controller.stop();
    }

    controller.setStandard(0);
    QVERIFY(controller.setLimits({{QStringLiteral("speedEnabled"), false}}));
    QVERIFY(reportPosition(vehicle(), mockLink(), controller, origin));
    vehicle()->heading()->setRawValue(0);
    QVERIFY(controller.start());
    QTRY_COMPARE_WITH_TIMEOUT(controller.phase(), QStringLiteral("八字飞行"), TestTimeout::longMs());
    QVERIFY(reportReferencePosition(vehicle(), mockLink(), controller, 4));
    const double ideal = controller.metrics().value(QStringLiteral("idealHeading")).toDouble();
    const double yaw = (ideal + 5.5 - 45) * std::numbers::pi / 180;
    mavlink_attitude_quaternion_t attitude{};
    attitude.q1 = std::cos(yaw / 2);
    attitude.q4 = std::sin(yaw / 2);
    attitude.repr_offset_q[0] = std::cos(std::numbers::pi / 8);
    attitude.repr_offset_q[3] = std::sin(std::numbers::pi / 8);
    mavlink_message_t message{};
    mavlink_msg_attitude_quaternion_encode(vehicle()->id(), vehicle()->compId(), &message, &attitude);
    QVERIFY(QMetaObject::invokeMethod(vehicle(), "_mavlinkMessageReceived", Qt::DirectConnection,
                                      Q_ARG(LinkInterface*, mockLink()), Q_ARG(mavlink_message_t, message)));
    mavlink_msg_attitude_pack(vehicle()->id(), vehicle()->compId(), &message, 0, 0, 0, std::numbers::pi_v<float>, 0, 0,
                              0);
    QVERIFY(QMetaObject::invokeMethod(vehicle(), "_mavlinkMessageReceived", Qt::DirectConnection,
                                      Q_ARG(LinkInterface*, mockLink()), Q_ARG(mavlink_message_t, message)));
    QVERIFY(QMetaObject::invokeMethod(&controller, "_sample", Qt::DirectConnection));
    QVERIFY(controller.running());
    QVERIFY(std::abs(controller.metrics().value(QStringLiteral("headingError")).toDouble() - 5.5) < 0.01);
    controller.stop();
}

void CaacFigureEightControllerTest::_minimumFigureEightSpeed()
{
    QSettings().remove(QStringLiteral("CAAC/FigureEight"));
    _connectMockLink();
    QVERIFY(vehicle());
    QVERIFY(waitForInitialConnect());
    _ignoreMissingPressureBackend();
    QVERIFY(takeoffAtExamHeight(vehicle()));
    QTRY_VERIFY_WITH_TIMEOUT(vehicle()->airborne(), TestTimeout::mediumMs());
    QVERIFY(reportTakeoffReference(vehicle(), mockLink()));
    CaacFigureEightController controller;
    const auto centre = vehicle()->coordinate();
    QVERIFY(controller.setPoints(QGCGeo::geodesicDestination(centre, 270, 12),
                                 QGCGeo::geodesicDestination(centre, 90, 12)));
    QTimer speedTelemetry;
    connect(&speedTelemetry, &QTimer::timeout, this, [this]() { reportSpeed(vehicle(), mockLink()); });
    speedTelemetry.start(100);
    QSignalSpy speech(&controller, &CaacFigureEightController::announcementRequested);
    QVERIFY(controller.setLimits({{QStringLiteral("speedEnabled"), true},
                                  {QStringLiteral("minSpeed"), 0.1},
                                  {QStringLiteral("maxSpeed"), 0.3}}));
    for (const bool customSpeed : {false, true}) {
        QVERIFY(controller.setLimits({{QStringLiteral("speedEnabled"), customSpeed},
                                      {QStringLiteral("minSpeed"), 0.1},
                                      {QStringLiteral("maxSpeed"), 0.3}}));
        vehicle()->heading()->setRawValue(0);
        mockLink()->setVehicleAttitudeOverrideDeg(0, 0, 0);
        QVERIFY(reportPosition(vehicle(), mockLink(), controller, centre));
        QVERIFY(controller.start());
        QTRY_COMPARE_WITH_TIMEOUT(controller.phase(), QStringLiteral("八字飞行"), TestTimeout::longMs());
        QVERIFY(reportPosition(vehicle(), mockLink(), controller, centre, 0));
        QVERIFY(controller.awaitingFigureEight());
        QCOMPARE(controller.attemptNumber(), 1);
        QVERIFY(reportReferencePosition(vehicle(), mockLink(), controller, 4, 0.3));
        QVERIFY(!controller.awaitingFigureEight());
        QCOMPARE(controller.attemptNumber(), 1);
        QVERIFY(reportReferencePosition(vehicle(), mockLink(), controller, 5, 0.299));
        QCOMPARE(controller.attemptNumber(), 2);
        QCOMPARE(controller.phase(), QStringLiteral("中心筒就位"));
        QCOMPARE(speech[speech.size() - 2][0].toString(), QStringLiteral("speed_low"));
        QVERIFY(controller.lastAttemptFailure().contains(QStringLiteral("0.3")));
        controller.stop();
    }
}

void CaacFigureEightControllerTest::_voiceResources()
{
    QFile manifest(QStringLiteral(":/CAAC/ExamAudio/manifest.json"));
    QVERIFY(manifest.open(QIODevice::ReadOnly));
    const auto clips = QJsonDocument::fromJson(manifest.readAll()).object().value(QStringLiteral("clips")).toArray();
    QStringList ids;
    for (const auto& entry : clips) {
        const auto clip = entry.toObject();
        const auto id = clip.value(QStringLiteral("id")).toString();
        QVERIFY(!ids.contains(id));
        ids.append(id);
        QFile audio(QStringLiteral(":/CAAC/ExamAudio/%1").arg(clip.value(QStringLiteral("file")).toString()));
        QVERIFY2(audio.open(QIODevice::ReadOnly), qPrintable(id));
        const auto data = audio.readAll();
        QVERIFY(data.startsWith("RIFF") && data.mid(8, 4) == "WAVE");
        QCOMPARE(data.size(), clip.value(QStringLiteral("byteSize")).toInteger());
        QCOMPARE(QString::fromLatin1(QCryptographicHash::hash(data, QCryptographicHash::Sha256).toHex()),
                 clip.value(QStringLiteral("sha256")).toString());
        if (id == QStringLiteral("spin_completed")) {
            QCOMPARE(clip.value(QStringLiteral("text")).toString(), QStringLiteral("自旋考试通过。"));
        } else if (id == QStringLiteral("completed")) {
            QCOMPARE(clip.value(QStringLiteral("text")).toString(), QStringLiteral("考试通过。"));
        } else if (id == QStringLiteral("failed")) {
            QCOMPARE(clip.value(QStringLiteral("text")).toString(), QStringLiteral("考试失败，三次机会已用完。"));
        }
    }
    for (const auto& id : {"ready", "positioning", "spin", "spin_reverse", "spin_reverse_cw", "spin_completed", "spin_timeout", "spin_reversal",
                           "figure_eight", "completed", "retry", "failed"}) {
        QVERIFY(ids.contains(QString::fromLatin1(id)));
    }
}

void CaacFigureEightControllerTest::_voiceQueue()
{
    CaacExamAudio audio;
    QElapsedTimer since;
    since.start();
    audio._pending = true;
    audio._currentId = QStringLiteral("positioning");
    QVERIFY(!audio._deferCue(QStringLiteral("spin_completed"), QStringLiteral("自旋考试通过。"), since));
    audio._currentId = QStringLiteral("spin_completed");
    QVERIFY(audio._deferCue(QStringLiteral("figure_eight"), QStringLiteral("可以开始水平八字飞行。"), since));
    QVERIFY(audio._deferCue(QStringLiteral("telemetry"), QStringLiteral("遥测中断"), since));
    auto next = audio._nextQueuedCue();
    QVERIFY(next.has_value());
    QCOMPARE(next->id, QStringLiteral("telemetry"));
    next = audio._nextQueuedCue();
    QVERIFY(next.has_value());
    QCOMPARE(next->id, QStringLiteral("figure_eight"));

    QVERIFY(audio._deferCue(QStringLiteral("area"), QStringLiteral("水平偏差超限"), since));
    QVERIFY(audio._deferCue(QStringLiteral("failed"), QStringLiteral("最终结果"), since));
    next = audio._nextQueuedCue();
    QVERIFY(next.has_value());
    QCOMPARE(next->id, QStringLiteral("area"));
    next = audio._nextQueuedCue();
    QVERIFY(next.has_value());
    QCOMPARE(next->id, QStringLiteral("failed"));

    audio._currentId = QStringLiteral("speed_low");
    QVERIFY(audio._deferCue(QStringLiteral("failed"), QStringLiteral("考试失败，三次机会已用完。"), since));
    QVERIFY(audio._deferCue(QStringLiteral("retry"), QStringLiteral("重新就位"), since));
    QVERIFY(audio._deferCue(QStringLiteral("ready"), QStringLiteral("就绪"), since));
    audio.clearPendingWarnings();
    next = audio._nextQueuedCue();
    QVERIFY(next.has_value());
    QCOMPARE(next->id, QStringLiteral("failed"));
    QCOMPARE(next->text, QStringLiteral("考试失败，三次机会已用完。"));
    QVERIFY(!audio._nextQueuedCue().has_value());

    audio._currentId = QStringLiteral("completed");
    QVERIFY(audio._deferCue(QStringLiteral("ready"), QStringLiteral("就绪"), since));
    audio.clearPendingWarnings();
    QVERIFY(audio._pending);
    QCOMPARE(audio._currentId, QStringLiteral("completed"));
    QVERIFY(!audio._nextQueuedCue().has_value());
    QVERIFY(!CaacExamAudio::_cueExpired(QStringLiteral("completed"), 60000));
    QVERIFY(!CaacExamAudio::_cueExpired(QStringLiteral("failed"), 60000));
    QVERIFY(!CaacExamAudio::_cueExpired(QStringLiteral("spin_completed"), 60000));
    QVERIFY(CaacExamAudio::_cueExpired(QStringLiteral("figure_eight"), 11000));
    QVERIFY(audio._deferCue(QStringLiteral("failed"), QStringLiteral("最终结果"), since));
    audio.stop();
    QVERIFY(!audio._pending);
    QVERIFY(audio._currentId.isEmpty());
    QVERIFY(!audio._nextQueuedCue().has_value());
}

UT_REGISTER_TEST(CaacFigureEightControllerTest, TestLabel::Unit, TestLabel::Vehicle, TestLabel::Slow)

#include "CaacFigureEightControllerTest.moc"
