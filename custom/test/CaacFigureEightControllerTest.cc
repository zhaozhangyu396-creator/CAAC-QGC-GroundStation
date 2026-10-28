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

bool reportSpeed(Vehicle* vehicle, MockLink* link)
{
    if (!vehicle || !link) {
        return false;
    }
    mavlink_message_t speed{};
    mavlink_msg_vfr_hud_pack(vehicle->id(), vehicle->compId(), &speed, 0, 0, 0, 0, 0, 0);
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
                    const QGeoCoordinate& coordinate)
{
    if (!vehicle || !link) {
        return false;
    }
    const double yaw = vehicle->heading()->rawValue().toDouble() * std::numbers::pi / 180;
    mavlink_message_t attitude{};
    mavlink_msg_attitude_pack(vehicle->id(), vehicle->compId(), &attitude, 0, 0, 0, yaw, 0, 0, 0);
    if (!QMetaObject::invokeMethod(vehicle, "_mavlinkMessageReceived", Qt::DirectConnection,
                                   Q_ARG(LinkInterface*, link), Q_ARG(mavlink_message_t, attitude)) ||
        !reportSpeed(vehicle, link)) {
        return false;
    }
    const float offset[] = {1, 0, 0, 0};
    mavlink_msg_attitude_quaternion_pack(vehicle->id(), vehicle->compId(), &attitude, 0, std::cos(yaw / 2), 0, 0,
                                         std::sin(yaw / 2), 0, 0, 0, offset);
    if (!QMetaObject::invokeMethod(vehicle, "_mavlinkMessageReceived", Qt::DirectConnection,
                                   Q_ARG(LinkInterface*, link), Q_ARG(mavlink_message_t, attitude))) {
        return false;
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

bool reportReferencePosition(Vehicle* vehicle, MockLink* link, CaacFigureEightController& controller, int index)
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
    return reportPosition(vehicle, link, controller, path[index].value<QGeoCoordinate>());
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
    void _centreQualification();
    void _headingEvaluation();
    void _voiceResources();

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
    QCOMPARE(controller.lastAnnouncement(), QStringLiteral("请将无人机飞至中心筒上方，保持一点五米高度"));
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
    QVERIFY(finishTurn(1));
    QVERIFY(!controller.running());
    QVERIFY(controller.lastAnnouncement().contains(QStringLiteral("少于 5 秒")));
    QCOMPARE(speech.size(), 0);
    controller.setVoiceEnabled(true);
    controller.testVoice();
    QCOMPARE(speech.last()[0].toString(), QStringLiteral("ready"));
    controller.setExerciseMode(2);
    QVERIFY(controller.start());
    QCOMPARE(speech.last()[0].toString(), QStringLiteral("positioning"));
    QCOMPARE(controller.attemptNumber(), 1);
    QCOMPARE(controller.attemptLimit(), 3);
    QTRY_COMPARE_WITH_TIMEOUT(controller.phase(), QStringLiteral("原地自旋"), TestTimeout::longMs());
    QCOMPARE(speech.last()[0].toString(), QStringLiteral("spin"));
    QVERIFY(turnTo(30));
    QTRY_VERIFY_WITH_TIMEOUT(controller.metrics().value(QStringLiteral("phaseElapsed")).toDouble() >= 5.1,
                             TestTimeout::longMs());
    QVERIFY(finishTurn(2));
    QVERIFY(controller.running());
    QVERIFY(controller.spinPassed());
    QCOMPARE(controller.phase(), QStringLiteral("八字飞行"));
    QCOMPARE(speech.last()[0].toString(), QStringLiteral("figure_eight"));

    vehicle()->altitudeRelative()->setRawValue(3.0);
    QVERIFY(reportPosition(vehicle(), mockLink(), controller, centre));
    QCOMPARE(controller.attemptNumber(), 2);
    QCOMPARE(controller.phase(), QStringLiteral("中心筒就位"));
    QVERIFY(controller.spinPassed());
    QCOMPARE(speech.last()[0].toString(), QStringLiteral("retry"));
    QVERIFY(controller.lastAttemptFailure().contains(QStringLiteral("高度偏差")));
    vehicle()->altitudeRelative()->setRawValue(1.75);
    QVERIFY(reportPosition(vehicle(), mockLink(), controller, centre));
    QTRY_COMPARE_WITH_TIMEOUT(controller.phase(), QStringLiteral("八字飞行"), TestTimeout::longMs());
    QCOMPARE(controller.attemptNumber(), 2);

    vehicle()->heading()->setRawValue(180);
    const auto reference = controller.referencePath();
    QVERIFY(reportPosition(vehicle(), mockLink(), controller, reference[1].value<QGeoCoordinate>()));
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
    QCOMPARE(controller.lastAnnouncement(), QStringLiteral("考试成功。"));
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
    QVERIFY(controller.status().contains(QStringLiteral("3 次机会已用完")));
    CaacFigureEightController restored;
    QCOMPARE(restored.exerciseMode(), 2);
    QVERIFY(restored.voiceEnabled());
}

void CaacFigureEightControllerTest::_centreQualification()
{
    QSettings().remove(QStringLiteral("CAAC/FigureEight"));
    _connectMockLink();
    QVERIFY(vehicle());
    QVERIFY(waitForInitialConnect());
    QTRY_VERIFY_WITH_TIMEOUT(vehicle()->coordinate().isValid(), TestTimeout::mediumMs());
    CaacFigureEightController controller;
    const auto centre = vehicle()->coordinate();
    QVERIFY(controller.setPoints(QGCGeo::geodesicDestination(centre, 270, 12),
                                 QGCGeo::geodesicDestination(centre, 90, 12)));
    QGeoCoordinate reportedCoordinate = centre;
    double reportedHeight = 0;
    QTimer telemetry;
    connect(&telemetry, &QTimer::timeout, this, [&]() {
        vehicle()->altitudeRelative()->setRawValue(reportedHeight);
        reportPosition(vehicle(), mockLink(), controller, reportedCoordinate);
    });
    telemetry.start(100);
    QTRY_VERIFY_WITH_TIMEOUT(controller.running() || controller.start(), TestTimeout::mediumMs());
    QCOMPARE(controller.phase(), QStringLiteral("中心筒就位"));
    QCOMPARE(controller.attemptNumber(), 1);
    _ignoreMissingPressureBackend();
    QVERIFY(takeoffAtExamHeight(vehicle()));
    QTRY_VERIFY_WITH_TIMEOUT(vehicle()->airborne(), TestTimeout::mediumMs());
    const auto report = [&]() {
        vehicle()->altitudeRelative()->setRawValue(reportedHeight);
        return reportPosition(vehicle(), mockLink(), controller, reportedCoordinate);
    };
    for (const double height : {1.49, 2.01}) {
        reportedHeight = height;
        QVERIFY(report());
        QCOMPARE(controller.metrics().value(QStringLiteral("hoverStableSeconds")).toDouble(), 0.0);
        QCOMPARE(controller.phase(), QStringLiteral("中心筒就位"));
    }
    reportedHeight = 1.5;
    QVERIFY(report());
    QTRY_VERIFY_WITH_TIMEOUT(controller.metrics().value(QStringLiteral("hoverStableSeconds")).toDouble() >= 1,
                             TestTimeout::mediumMs());
    reportedCoordinate = QGCGeo::geodesicDestination(centre, 90, 3);
    QVERIFY(report());
    QCOMPARE(controller.metrics().value(QStringLiteral("hoverStableSeconds")).toDouble(), 0.0);
    QCOMPARE(controller.phase(), QStringLiteral("中心筒就位"));
    reportedCoordinate = centre;
    reportedHeight = 2.0;
    QVERIFY(report());
    QTRY_COMPARE_WITH_TIMEOUT(controller.phase(), QStringLiteral("八字飞行"), TestTimeout::longMs());
    QVERIFY(controller.metrics().value(QStringLiteral("elapsed")).toDouble() >= 4);
    QCOMPARE(controller.attemptNumber(), 1);
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
    CaacFigureEightController controller;
    QTimer speedTelemetry;
    connect(&speedTelemetry, &QTimer::timeout, this, [this]() { reportSpeed(vehicle(), mockLink()); });
    speedTelemetry.start(100);
    const auto origin = vehicle()->coordinate();
    const double siteBearings[] = {90, 35, 270};
    for (const int standard : {0, 1, 2}) {
        controller.setStandard(standard);
        const double bearing = siteBearings[standard];
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
            QVERIFY(reportPosition(vehicle(), mockLink(), controller, coordinate));
            const auto metrics = controller.metrics();
            const double measuredIdeal = metrics.value(QStringLiteral("idealHeading")).toDouble();
            // At the shared centre either contiguous circle can supply the tangent after GPS quantization.
            if (i % 120 != 0) {
                QVERIFY2(std::abs(std::remainder(measuredIdeal - ideal, 360)) < 0.01,
                         qPrintable(QStringLiteral("standard %1, point %2: ideal %3, received %4")
                                        .arg(standard)
                                        .arg(i)
                                        .arg(ideal)
                                        .arg(measuredIdeal)));
            }
            const double expectedError =
                std::abs(std::remainder(bodyHeading + (standard == 2 ? 180 : 0) - measuredIdeal, 360));
            QVERIFY(std::abs(metrics.value(QStringLiteral("headingError")).toDouble() - expectedError) < 0.01);
            if (i < path.size() - 1) {
                QVERIFY(controller.running());
            }
        }
        QVERIFY(!controller.running());
        QCOMPARE(controller.lastAnnouncement(), QStringLiteral("考试成功。"));
    }
    for (const bool highSpeed : {true, false}) {
        QVERIFY(controller.setLimits({{QStringLiteral("speedEnabled"), true},
                                      {QStringLiteral("minSpeed"), highSpeed ? 0.0 : 1.0},
                                      {QStringLiteral("maxSpeed"), highSpeed ? 1.0 : 3.0}}));
        QVERIFY(reportPosition(vehicle(), mockLink(), controller, origin));
        QVERIFY(controller.start());
        QTRY_COMPARE_WITH_TIMEOUT(controller.phase(), QStringLiteral("八字飞行"), TestTimeout::longMs());
        if (highSpeed) {
            QVERIFY(reportReferencePosition(vehicle(), mockLink(), controller, 0));
            vehicle()->groundSpeed()->setRawValue(2.0);
            QVERIFY(QMetaObject::invokeMethod(&controller, "_sample", Qt::DirectConnection));
        } else {
            QVERIFY(reportReferencePosition(vehicle(), mockLink(), controller, 1));
        }
        QVERIFY(!controller.running());
        QCOMPARE(controller.lastAttemptFailure(), QStringLiteral("自定义速度超限"));
    }

    controller.setStandard(0);
    QVERIFY(controller.setLimits({{QStringLiteral("speedEnabled"), false}}));
    QVERIFY(reportPosition(vehicle(), mockLink(), controller, origin));
    QVERIFY(controller.start());
    QTRY_COMPARE_WITH_TIMEOUT(controller.phase(), QStringLiteral("八字飞行"), TestTimeout::longMs());
    QVERIFY(reportReferencePosition(vehicle(), mockLink(), controller, 1));
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
    }
    for (const auto& id :
         {"ready", "positioning", "spin", "spin_reverse", "figure_eight", "completed", "retry", "failed"}) {
        QVERIFY(ids.contains(QString::fromLatin1(id)));
    }
}

UT_REGISTER_TEST(CaacFigureEightControllerTest, TestLabel::Unit, TestLabel::Vehicle, TestLabel::Slow)

#include "CaacFigureEightControllerTest.moc"
