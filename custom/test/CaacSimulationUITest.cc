#include <cmath>
#include <cstdio>
#include <memory>
#include <utility>

#include <QtCore/QLineF>
#include <QtCore/QRegularExpression>
#include <QtCore/QScopeGuard>
#include <QtGui/QGuiApplication>
#include <QtGui/QScreen>
#include <QtQml/QQmlEngine>
#include <QtQml/QQmlExpression>
#include <QtQuick/QQuickItem>
#include <QtQuick/QQuickWindow>
#include <QtTest/QSignalSpy>

#include "AppSettings.h"
#include "BatteryFactGroupListModel.h"
#include "CaacCorePlugin.h"
#include "CaacFlightDistance.h"
#include "CaacRouteEditorController.h"
#include "CaacSimulationController.h"
#include "CaacSimulationEngine.h"
#include "Joystick.h"
#include "JoystickManager.h"
#include "JoystickManagerSettings.h"
#include "JoystickSettings.h"
#include "MissionController.h"
#include "MissionManager.h"
#include "MockJoystick.h"
#include "MockLink.h"
#include "MultiVehicleManager.h"
#include "PlanMasterController.h"
#include "QGCGeo.h"
#include "QGCMAVLink.h"
#include "QmlUITestBase.h"
#include "SettingsManager.h"
#include "SimpleMissionItem.h"
#include "UnitsSettings.h"
#include "Vehicle.h"
#include "VehicleLinkManager.h"

class CaacSimulationUITest : public QmlUITestBase
{
    Q_OBJECT
private slots:
    void _defaultStartPosition();
    void _levelFlightTelemetry();
    void _uploadAndFlightControls();
    void _closedRouteAndSimulationToggle();
    void _fourLoopsFromUnsavedSettings();
    void _usbJoystickModeSwitch();
};

void CaacSimulationUITest::_usbJoystickModeSwitch()
{
    startUI();
    if (QTest::currentTestFailed()) {
        return;
    }
    const QString joystickName = QStringLiteral("CAAC USB training switch");
    auto* manager = JoystickManager::instance();
    manager->init();
    std::unique_ptr<MockJoystick> input(MockJoystick::create(joystickName, 5, 4, 0));
    QVERIFY(input && input->isValid());
    QTRY_VERIFY_WITH_TIMEOUT(manager->availableJoystickNames().contains(joystickName), TestTimeout::mediumMs());
    SettingsManager::instance()->joystickManagerSettings()->activeJoystickName()->setRawValue(joystickName);
    QTRY_VERIFY_WITH_TIMEOUT(manager->activeJoystick() && manager->activeJoystick()->name() == joystickName,
                             TestTimeout::mediumMs());
    auto* joystick = manager->activeJoystick();
    for (int axis = 0; axis < 5; ++axis) {
        joystick->setAxisCalibration(axis, {});
    }
    using Calibration = RemoteControlCalibrationController;
    joystick->setFunctionForChannel(Calibration::stickFunctionRoll, 0);
    joystick->setFunctionForChannel(Calibration::stickFunctionPitch, 1);
    joystick->setFunctionForChannel(Calibration::stickFunctionYaw, 2);
    joystick->setFunctionForChannel(Calibration::stickFunctionThrottle, 3);
    auto* settings = joystick->settings();
    settings->calibrated()->setRawValue(true);
    settings->throttleModeCenterZero()->setRawValue(false);
    settings->simulationModeSwitchAxis()->setRawValue(4);
    settings->simulationModeSwitchPositions()->setRawValue(3);
    settings->simulationModeSwitchReversed()->setRawValue(false);
    settings->simulationModeSwitchEnabled()->setRawValue(true);
    QCOMPARE(settings->simulationModeLow()->rawValue().toInt(), 3);
    QCOMPARE(settings->simulationModeMiddle()->rawValue().toInt(), 1);
    QCOMPARE(settings->simulationModeHigh()->rawValue().toInt(), 2);
    QVERIFY(input->setAxis(4, Joystick::AxisMax));
    auto* plan = findVisibleItem(_rootItem, QStringLiteral("mainView_plan"));
    QVERIFY(plan);
    auto* master = plan->property("_planMasterController").value<PlanMasterController*>();
    QVERIFY(master);
    const QGeoCoordinate home(30, 120, 0);
    auto* mission = master->missionController();
    mission->setHomePosition(home);
    auto* waypoint = qobject_cast<SimpleMissionItem*>(
        mission->insertSimpleMissionItem(QGCGeo::geodesicDestination(home, 0, 500), 1, false));
    QVERIFY(waypoint);
    waypoint->altitude()->setRawValue(5);
    CaacRouteEditorController editor;
    editor.setPlanMasterController(master);
    auto flightSettings = CaacSimulationController::defaults();
    flightSettings["takeoffHeight"] = 5.0;
    flightSettings["verticalSpeed"] = 10.0;
    QVERIFY(editor.setSimulationSettings(flightSettings));
    auto* plugin = qobject_cast<CaacCorePlugin*>(QGCCorePlugin::instance());
    QVERIFY(plugin);
    auto* simulation = plugin->simulation();
    const auto stop = qScopeGuard([simulation, manager]() {
        manager->setActiveJoystickEnabledForActiveVehicle(false);
        simulation->stop();
    });
    ignoreLogMessage("Utilities.QGCSensors", QtWarningMsg,
                     QRegularExpression(QStringLiteral("Failed to connect to pressure backend")));
    ignoreLogMessage("Utilities.QGCSensors", QtWarningMsg,
                     QRegularExpression(QStringLiteral("Error Initializing Pressure Sensor")));
    QVERIFY(simulation->start(master));
    QTRY_VERIFY_WITH_TIMEOUT(simulation->vehicle() && simulation->vehicle()->isInitialConnectComplete(),
                             TestTimeout::longMs());
    auto* vehicle = simulation->vehicle();
    manager->setActiveJoystickEnabledForActiveVehicle(true);
    QTRY_COMPARE_WITH_TIMEOUT(simulation->modeSwitchPosition(), 2, TestTimeout::mediumMs());
    QVERIFY(simulation->modeSwitchAxisAllowed(4));
    for (int axis = 0; axis < 4; ++axis) {
        QVERIFY(!simulation->modeSwitchAxisAllowed(axis));
    }
    QSignalSpy uploaded(simulation, &CaacSimulationController::missionUploaded);
    QVERIFY(simulation->upload(master));
    QTRY_VERIFY_WITH_TIMEOUT(!uploaded.isEmpty(), TestTimeout::longMs());
    vehicle->startMission();
    QTRY_VERIFY_WITH_TIMEOUT(vehicle->airborne() && vehicle->groundSpeed()->rawValue().toDouble() > 1,
                             TestTimeout::longMs());
    simulation->setEmergencyMode(true);
    QVERIFY(simulation->emergencyMode());
    QVERIFY(findVisibleItem(_rootItem, QStringLiteral("caacEmergencyBlackout")));
    QVERIFY(clickButton(QStringLiteral("caacSimulationManagement")));
    QVERIFY(clickButton(QStringLiteral("caacSimulationJoystickMappingToggle")));
    QVERIFY(findVisibleItemScrolled(QStringLiteral("caacSimulationModeSwitchAxis"),
                                    QStringLiteral("popupDialog_contentFlickable")));
    QVERIFY(findVisibleItemScrolled(QStringLiteral("caacSimulationModePosition2"),
                                    QStringLiteral("popupDialog_contentFlickable")));
    QVERIFY(rejectDialog());
    QVERIFY(input->setAxis(4, 0));
    QTRY_VERIFY_WITH_TIMEOUT(simulation->manualControlActive(), TestTimeout::mediumMs());
    QTRY_VERIFY_WITH_TIMEOUT(vehicle->flightMode() != vehicle->missionFlightMode(), TestTimeout::mediumMs());
    const int sequence = vehicle->missionManager()->currentIndex();
    const auto beforeManual = vehicle->coordinate();
    QVERIFY(input->setAxis(1, Joystick::AxisMax));
    QTRY_VERIFY_WITH_TIMEOUT(QGCGeo::geodesicDistance(beforeManual, vehicle->coordinate()) > 3,
                             TestTimeout::mediumMs());
    QTRY_VERIFY_WITH_TIMEOUT(vehicle->pitch()->rawValue().toDouble() < -10, TestTimeout::mediumMs());
    QCOMPARE(vehicle->missionManager()->currentIndex(), sequence);
    manager->setActiveJoystickEnabledForActiveVehicle(false);
    QTRY_COMPARE_WITH_TIMEOUT(vehicle->groundSpeed()->rawValue().toDouble(), 0.0, TestTimeout::mediumMs());
    QVERIFY(simulation->manualControlActive());
    QVERIFY(simulation->status().contains(
        QCoreApplication::translate("CaacSimulationController", "Joystick input lost; simulation hovering.")));
    QVERIFY(input->setAxis(1, 0));
    manager->setActiveJoystickEnabledForActiveVehicle(true);
    QVERIFY(input->setAxis(4, Joystick::AxisMin));
    QTRY_COMPARE_WITH_TIMEOUT(vehicle->flightMode(), vehicle->rtlFlightMode(), TestTimeout::mediumMs());
    QVERIFY(input->setAxis(4, Joystick::AxisMax));
    QTRY_COMPARE_WITH_TIMEOUT(vehicle->flightMode(), vehicle->missionFlightMode(), TestTimeout::mediumMs());
    settings->simulationModeSwitchReversed()->setRawValue(true);
    QTRY_COMPARE_WITH_TIMEOUT(simulation->modeSwitchPosition(), 0, TestTimeout::mediumMs());
    QCOMPARE(vehicle->flightMode(), vehicle->missionFlightMode());
    QVERIFY(input->setAxis(4, 0));
    QTRY_VERIFY_WITH_TIMEOUT(simulation->manualControlActive(), TestTimeout::mediumMs());
    QVERIFY(input->setAxis(4, Joystick::AxisMax));
    QTRY_COMPARE_WITH_TIMEOUT(vehicle->flightMode(), vehicle->rtlFlightMode(), TestTimeout::mediumMs());
    QSignalSpy positionUpdates(simulation, &CaacSimulationController::joystickChannelsChanged);
    settings->simulationModeSwitchPositions()->setRawValue(2);
    QTRY_VERIFY_WITH_TIMEOUT(positionUpdates.count() >= 10, TestTimeout::mediumMs());
    QCOMPARE(simulation->modeSwitchPosition(), 0);
    QVERIFY(input->setAxis(4, Joystick::AxisMin));
    QTRY_COMPARE_WITH_TIMEOUT(vehicle->flightMode(), vehicle->missionFlightMode(), TestTimeout::mediumMs());
    QVERIFY(input->setAxis(4, 0));
    QSignalSpy channels(simulation, &CaacSimulationController::joystickChannelsChanged);
    QTRY_VERIFY_WITH_TIMEOUT(channels.count() >= 5, TestTimeout::mediumMs());
    QCOMPARE(vehicle->flightMode(), vehicle->missionFlightMode());
    QCOMPARE(simulation->modeSwitchPosition(), 2);
    settings->simulationModeSwitchPositions()->setRawValue(3);
    QTRY_COMPARE_WITH_TIMEOUT(simulation->modeSwitchPosition(), 1, TestTimeout::mediumMs());
    QVERIFY(input->setAxis(4, Joystick::AxisMin));
    QTRY_COMPARE_WITH_TIMEOUT(simulation->modeSwitchPosition(), 2, TestTimeout::mediumMs());
    QTRY_COMPARE_WITH_TIMEOUT(vehicle->flightMode(), vehicle->missionFlightMode(), TestTimeout::mediumMs());
    QVERIFY(input->setAxis(4, 0));
    QTRY_VERIFY_WITH_TIMEOUT(simulation->manualControlActive(), TestTimeout::mediumMs());
    ignoreLogMessage(
        "Joystick.JoystickSDL", QtWarningMsg,
        QRegularExpression(QStringLiteral("Joystick disconnected during update:.*CAAC USB training switch")));
    ignoreLogMessage(
        "Joystick.Joystick", QtWarningMsg,
        QRegularExpression(QStringLiteral("Joystick disconnected or update failed:.*CAAC USB training switch")));
    ignoreLogMessage("Joystick.Joystick", QtWarningMsg,
                     QRegularExpression(QStringLiteral(
                         "^Joystick polling thread not running even though _pollingVehicle was set\\.$")));
    input.reset();
    QTRY_VERIFY_WITH_TIMEOUT(!manager->availableJoystickNames().contains(joystickName), TestTimeout::mediumMs());
    QTRY_COMPARE_WITH_TIMEOUT(vehicle->groundSpeed()->rawValue().toDouble(), 0.0, TestTimeout::mediumMs());
    QVERIFY(simulation->active());
    QVERIFY(simulation->manualControlActive());
}

void CaacSimulationUITest::_fourLoopsFromUnsavedSettings()
{
    std::fprintf(stderr, "CAAC four-loop step: start UI\n");
    startUI();
    if (QTest::currentTestFailed()) {
        return;
    }
    auto* plan = findVisibleItem(_rootItem, QStringLiteral("mainView_plan"));
    auto* panel = findVisibleItem(_rootItem, QStringLiteral("caacRouteEditor"));
    QVERIFY(plan && panel);
    auto* master = plan->property("_planMasterController").value<PlanMasterController*>();
    auto* editor = panel->property("routeController").value<CaacRouteEditorController*>();
    QVERIFY(master && editor);
    auto* mission = master->missionController();
    const QGeoCoordinate home(30, 120, 20);
    mission->setHomePosition(home);
    const auto first = QGCGeo::geodesicDestination(home, 90, 10);
    const QList<QGeoCoordinate> points{first, QGCGeo::geodesicDestination(first, 0, 10),
                                       QGCGeo::geodesicDestination(home, 0, 10),
                                       QGCGeo::geodesicDestination(home, 0, 3)};
    for (const auto& point : points) {
        QVERIFY(editor->addWaypoint(point));
        editor->currentItem()->altitude()->setRawValue(6);
    }
    auto settings = CaacSimulationController::defaults();
    settings["takeoffHeight"] = 2.0;
    settings["verticalSpeed"] = 10.0;
    QVERIFY(editor->setSimulationSettings(settings));
    auto* plugin = qobject_cast<CaacCorePlugin*>(QGCCorePlugin::instance());
    QVERIFY(plugin);
    auto* simulation = plugin->simulation();
    QVERIFY(simulation);
    const auto stop = qScopeGuard([simulation]() { simulation->stop(); });
    ignoreLogMessage("Utilities.QGCSensors", QtWarningMsg,
                     QRegularExpression(QStringLiteral("Failed to connect to pressure backend")));
    ignoreLogMessage("Utilities.QGCSensors", QtWarningMsg,
                     QRegularExpression(QStringLiteral("Error Initializing Pressure Sensor")));
    QVERIFY(simulation->start(master));
    QTRY_VERIFY_WITH_TIMEOUT(simulation->vehicle() && simulation->vehicle()->isInitialConnectComplete(),
                             TestTimeout::longMs());
    std::fprintf(stderr, "CAAC four-loop step: edit settings\n");
    QVERIFY(clickButton(QStringLiteral("caacGlobalSettingsButton")));
    const auto scroller = QStringLiteral("caacGlobalSettingsScroller");
    QVERIFY(clickButtonScrolled(QStringLiteral("caacSimClosedRoute"), scroller));
    for (const auto& choice :
         {qMakePair("caacSimLoopMode", 1), qMakePair("caacSimStartPoint", 1), qMakePair("caacSimEndBehavior", 1),
          qMakePair("caacSimReturnPoint", 4), qMakePair("caacSimClimbMode", 0)}) {
        auto* combo = findVisibleItemScrolled(QLatin1String(choice.first), scroller);
        QVERIFY(combo && combo->setProperty("currentIndex", choice.second));
    }
    QVERIFY(clickButtonScrolled(QStringLiteral("caacSimLoopCount"), scroller));
    QTest::keyClick(_window, Qt::Key_A, Qt::ControlModifier);
    QTest::keyClick(_window, Qt::Key_4);
    QTest::keyClick(_window, Qt::Key_Tab);
    QVERIFY(clickButtonScrolled(QStringLiteral("caacSimVerticalSpeed"), scroller));
    QTest::keyClick(_window, Qt::Key_A, Qt::ControlModifier);
    QTest::keyClick(_window, Qt::Key_2);
    QTest::keyClick(_window, Qt::Key_Tab);
    QCOMPARE(editor->simulationSettings().value("loopCount").toInt(), 1);
    QVERIFY(clickButton(QStringLiteral("caacWaypointListButton")));
    QSignalSpy upload(simulation, &CaacSimulationController::missionUploaded);
    QVERIFY(clickButton(QStringLiteral("caacUploadWaypoints")));
    QTRY_VERIFY_WITH_TIMEOUT(!upload.isEmpty(), TestTimeout::longMs());
    std::fprintf(stderr, "CAAC four-loop step: settings uploaded\n");
    QCOMPARE(simulation->remainingLoops(), 4);
    QCOMPARE(editor->simulationSettings().value("loopCount").toInt(), 4);
    QVERIFY(editor->simulationSettings().value("closed").toBool());
    QVERIFY(findVisibleItem(_rootItem, QStringLiteral("caacSimulationPreview_closure")));
    QCOMPARE(editor->simulationSettings().value("startSequence").toInt(), 1);
    QCOMPARE(editor->simulationSettings().value("returnSequence").toInt(), 4);
    QCOMPARE(editor->simulationSettings().value("climbMode").toInt(), 0);
    QCOMPARE(editor->simulationSettings().value("verticalSpeed").toDouble(), 2.0);
    auto* vehicle = simulation->vehicle();
    int jumps = 0;
    for (const auto* item : vehicle->missionManager()->missionItems()) {
        if (item->command() == MAV_CMD_DO_JUMP) {
            ++jumps;
            QCOMPARE(item->param2(), 3.0);
        }
    }
    QCOMPARE(jumps, 1);
    QList<int> arrivals{0, 0, 0, 0};
    int near = -1;
    const auto position = connect(vehicle, &Vehicle::coordinateChanged, this, [&]() {
        int closest = -1;
        for (int i = 0; i < points.size(); ++i) {
            if (QGCGeo::geodesicDistance(vehicle->coordinate(), points[i]) < 0.2) {
                closest = i;
                break;
            }
        }
        if (closest >= 0 && closest != near) {
            ++arrivals[closest];
        }
        near = closest;
    });
    const auto disconnectPosition = qScopeGuard([&]() { disconnect(position); });
    vehicle->startMission();
    std::fprintf(stderr, "CAAC four-loop step: mission started\n");
    QTRY_VERIFY_WITH_TIMEOUT(vehicle->altitudeRelative()->rawValue().toDouble() > 3, TestTimeout::longMs());
    QVERIFY(vehicle->altitudeRelative()->rawValue().toDouble() < 5.5);
    QVERIFY(QGCGeo::geodesicDistance(vehicle->coordinate(), home) < 0.2);
    QVERIFY(std::abs(vehicle->groundSpeed()->rawValue().toDouble()) < 0.01);
    QTRY_COMPARE_WITH_TIMEOUT(simulation->status(),
                              QCoreApplication::translate("CaacSimulationController", "Simulation mission finished"),
                              60000);
    QTRY_COMPARE_WITH_TIMEOUT(arrivals, QList<int>({5, 4, 4, 5}), TestTimeout::mediumMs());
    std::fprintf(stderr, "CAAC four-loop step: all four loops completed\n");
    QCOMPARE(simulation->remainingLoops(), 0);
    QVERIFY(QGCGeo::geodesicDistance(vehicle->coordinate(), points[3]) < 0.2);
    QVERIFY(vehicle->airborne());
}

void CaacSimulationUITest::_levelFlightTelemetry()
{
    startUI();
    if (QTest::currentTestFailed()) {
        return;
    }
    auto* plan = findVisibleItem(_rootItem, QStringLiteral("mainView_plan"));
    QVERIFY(plan);
    auto* master = plan->property("_planMasterController").value<PlanMasterController*>();
    QVERIFY(master);
    const QGeoCoordinate home(30, 120, 20);
    auto* mission = master->missionController();
    mission->setHomePosition(home);
    auto* waypoint = qobject_cast<SimpleMissionItem*>(
        mission->insertSimpleMissionItem(QGCGeo::geodesicDestination(home, 0, 100), 1, false));
    QVERIFY(waypoint);
    waypoint->altitude()->setRawValue(2);
    CaacRouteEditorController editor;
    editor.setPlanMasterController(master);
    auto settings = CaacSimulationController::defaults();
    settings["takeoffHeight"] = 2.0;
    settings["verticalSpeed"] = 10.0;
    QVERIFY(editor.setSimulationSettings(settings));
    auto* plugin = qobject_cast<CaacCorePlugin*>(QGCCorePlugin::instance());
    QVERIFY(plugin);
    auto* simulation = plugin->simulation();
    QVERIFY(simulation);
    const auto stop = qScopeGuard([simulation]() { simulation->stop(); });
    ignoreLogMessage("Utilities.QGCSensors", QtWarningMsg,
                     QRegularExpression(QStringLiteral("Failed to connect to pressure backend")));
    ignoreLogMessage("Utilities.QGCSensors", QtWarningMsg,
                     QRegularExpression(QStringLiteral("Error Initializing Pressure Sensor")));
    QVERIFY(simulation->start(master));
    QTRY_VERIFY_WITH_TIMEOUT(simulation->vehicle() && simulation->vehicle()->isInitialConnectComplete(),
                             TestTimeout::longMs());
    auto* vehicle = simulation->vehicle();
    const auto link = vehicle->vehicleLinkManager()->primaryLink().lock();
    auto* mock = qobject_cast<MockLink*>(link.get());
    QVERIFY(mock);
    mavlink_message_t attitude{};
    const float offset[4] = {};
    mavlink_msg_attitude_quaternion_pack_chan(vehicle->id(), MAV_COMP_ID_AUTOPILOT1, mock->mavlinkChannel(), &attitude,
                                              0, 1, 0, 0, 0, 0, 0, 0, offset);
    mock->respondWithMavlinkMessage(attitude, true);
    QTRY_COMPARE_WITH_TIMEOUT(vehicle->pitch()->rawValue().toDouble(), 0.0, TestTimeout::mediumMs());
    QSignalSpy uploaded(simulation, &CaacSimulationController::missionUploaded);
    QVERIFY(simulation->upload(master));
    QTRY_VERIFY_WITH_TIMEOUT(!uploaded.isEmpty(), TestTimeout::longMs());
    vehicle->startMission();
    QTRY_VERIFY_WITH_TIMEOUT(vehicle->airborne() && vehicle->groundSpeed()->rawValue().toDouble() > 4,
                             TestTimeout::longMs());
    QTRY_VERIFY_WITH_TIMEOUT(vehicle->pitch()->rawValue().toDouble() < -1, TestTimeout::mediumMs());
    auto* instrument = findVisibleItem(_rootItem, QStringLiteral("caacAttitudeInstrument"));
    QVERIFY(instrument);
    QTRY_COMPARE_WITH_TIMEOUT(instrument->property("pitchAngle").toDouble(), vehicle->pitch()->rawValue().toDouble(),
                              TestTimeout::mediumMs());
    vehicle->pauseVehicle();
    QTRY_COMPARE_WITH_TIMEOUT(vehicle->pitch()->rawValue().toDouble(), 0.0, TestTimeout::mediumMs());
}

void CaacSimulationUITest::_defaultStartPosition()
{
    startUI();
    if (QTest::currentTestFailed()) {
        return;
    }
    QVERIFY(QMetaObject::invokeMethod(_window, "showPlanView"));
    auto* plan = findVisibleItem(_rootItem, QStringLiteral("mainView_plan"));
    QVERIFY(plan);
    auto* master = plan->property("_planMasterController").value<PlanMasterController*>();
    QVERIFY(master);
    auto* mission = master->missionController();
    QVERIFY(!mission->plannedHomePosition().isValid());
    auto* plugin = qobject_cast<CaacCorePlugin*>(QGCCorePlugin::instance());
    QVERIFY(plugin);
    auto* simulation = plugin->simulation();
    QVERIFY(simulation);
    const auto stop = qScopeGuard([simulation]() { simulation->stop(); });
    const QGeoCoordinate expected(25.137898058569604, 102.7212769885262, 0);
    for (int attempt = 0; attempt < 2; ++attempt) {
        QVERIFY(clickToolSelectDropdownButton(QStringLiteral("toolbar_simulation")));
        QTRY_VERIFY_WITH_TIMEOUT(simulation->vehicle() && simulation->vehicle()->isInitialConnectComplete(),
                                 TestTimeout::longMs());
        auto* vehicle = simulation->vehicle();
        QTRY_VERIFY_WITH_TIMEOUT(QGCGeo::geodesicDistance(vehicle->coordinate(), expected) < 0.02,
                                 TestTimeout::mediumMs());
        QTRY_VERIFY_WITH_TIMEOUT(QGCGeo::geodesicDistance(vehicle->homePosition(), expected) < 0.02,
                                 TestTimeout::mediumMs());
        QTRY_COMPARE_WITH_TIMEOUT(mission->plannedHomePosition(), expected, TestTimeout::mediumMs());
        auto* map = findVisibleItem(_rootItem, QStringLiteral("planView_map"));
        QVERIFY(map);
        QVERIFY(QGCGeo::geodesicDistance(map->property("center").value<QGeoCoordinate>(), expected) < 0.02);
        auto* marker = findVisibleItem(_rootItem, QStringLiteral("planView_vehicle_%1").arg(vehicle->id()));
        QVERIFY(marker);
        auto* homeDragger = findVisibleItem(_rootItem, QStringLiteral("missionItemDrag_0"));
        QVERIFY(homeDragger);
        auto* homeIndicator = homeDragger->property("itemIndicator").value<QQuickItem*>();
        QVERIFY(homeIndicator);
        QVERIFY(marker->z() > homeIndicator->z());
        QVERIFY(marker->z() < homeDragger->z());
        QTRY_COMPARE_WITH_TIMEOUT(marker->property("coordinate").value<QGeoCoordinate>(), expected,
                                  TestTimeout::mediumMs());
        auto movedHome = QGCGeo::geodesicDestination(expected, 90, 30);
        movedHome.setAltitude(expected.altitude());
        mission->setHomePosition(movedHome);
        QTRY_COMPARE_WITH_TIMEOUT(marker->property("coordinate").value<QGeoCoordinate>(), movedHome,
                                  TestTimeout::mediumMs());
        QVERIFY(QGCGeo::geodesicDistance(vehicle->coordinate(), expected) < 0.02);
        mission->setHomePosition(expected);
        QTRY_COMPARE_WITH_TIMEOUT(marker->property("coordinate").value<QGeoCoordinate>(), expected,
                                  TestTimeout::mediumMs());
        QCOMPARE(vehicle->altitudeRelative()->rawValue().toDouble(), 0.0);
        QVERIFY(!vehicle->armed());
        QVERIFY(!vehicle->airborne());
        QVERIFY(!simulation->missionReady());
        simulation->stop();
        QTRY_COMPARE_WITH_TIMEOUT(MultiVehicleManager::instance()->vehicles()->count(), 0, TestTimeout::longMs());
    }
}

void CaacSimulationUITest::_closedRouteAndSimulationToggle()
{
    const auto step = [](const char* label) {
        std::fprintf(stderr, "CAAC closed-route step: %s\n", label);
        std::fflush(stderr);
    };
    step("start UI");
    startUI();
    if (QTest::currentTestFailed()) {
        return;
    }
    QVERIFY(QMetaObject::invokeMethod(_window, "showPlanView"));
    step("plan opened");
    auto* plan = findVisibleItem(_rootItem, QStringLiteral("mainView_plan"));
    QVERIFY(plan);
    auto* master = plan->property("_planMasterController").value<PlanMasterController*>();
    QVERIFY(master);
    auto* mission = master->missionController();
    const QGeoCoordinate home(30, 120, 20);
    mission->setHomePosition(home);
    QList<QGeoCoordinate> coordinates;
    for (int bearing : {0, 90, 180}) {
        coordinates.append(QGCGeo::geodesicDestination(home, bearing, 10));
        auto* waypoint = qobject_cast<SimpleMissionItem*>(
            mission->insertSimpleMissionItem(coordinates.last(), mission->visualItems()->count(), false));
        QVERIFY(waypoint);
        waypoint->altitude()->setRawValue(2);
    }
    CaacRouteEditorController editor;
    editor.setPlanMasterController(master);
    auto settings = CaacSimulationController::defaults();
    settings["startSequence"] = 2;
    settings["closed"] = true;
    settings["loopMode"] = 1;
    settings["loopCount"] = 10;
    settings["endBehavior"] = 1;
    settings["returnSequence"] = 2;
    settings["takeoffHeight"] = 2.0;
    settings["horizontalSpeed"] = 20.0;
    settings["verticalSpeed"] = 10.0;
    settings["turnMode"] = 3;
    settings["holdSeconds"] = 0.1;
    QVERIFY(editor.setSimulationSettings(settings));
    const auto original = master->saveToJson();
    auto* plugin = qobject_cast<CaacCorePlugin*>(CaacCorePlugin::instance());
    QVERIFY(plugin);
    auto* simulation = plugin->simulation();
    QVERIFY(simulation);
    const auto stop = qScopeGuard([simulation]() { simulation->stop(); });
    ignoreLogMessage("Utilities.QGCSensors", QtWarningMsg,
                     QRegularExpression(QStringLiteral("Failed to connect to pressure backend")));
    ignoreLogMessage("Utilities.QGCSensors", QtWarningMsg,
                     QRegularExpression(QStringLiteral("Error Initializing Pressure Sensor")));
    const auto clickSimulation = [&]() { return clickToolSelectDropdownButton(QStringLiteral("toolbar_simulation")); };
    QVERIFY(clickSimulation());
    step("connection requested");
    QTRY_VERIFY_WITH_TIMEOUT(simulation->vehicle() && simulation->vehicle()->isInitialConnectComplete(),
                             TestTimeout::longMs());
    QVERIFY(clickButton(QStringLiteral("toolbar_qgcLogo")));
    QVERIFY(verifyText(QStringLiteral("toolbar_simulation"),
                       QCoreApplication::translate("SelectViewDropdown", "Cancel simulation"),
                       QStringLiteral("active simulation")));
    QVERIFY(QMetaObject::invokeMethod(_window, "showPlanView"));
    QVERIFY(clickSimulation());
    step("cancel requested");
    QTRY_VERIFY_WITH_TIMEOUT(!simulation->active() && !MultiVehicleManager::instance()->activeVehicle(),
                             TestTimeout::mediumMs());
    QVERIFY(clickButton(QStringLiteral("toolbar_qgcLogo")));
    QVERIFY(verifyText(QStringLiteral("toolbar_simulation"),
                       QCoreApplication::translate("SelectViewDropdown", "Simulation"),
                       QStringLiteral("simulation disconnected")));
    QVERIFY(QMetaObject::invokeMethod(_window, "showPlanView"));
    QVERIFY(clickSimulation());
    step("reconnection requested");
    QVERIFY2(simulation->active(), qPrintable(simulation->status()));
    QTRY_VERIFY_WITH_TIMEOUT(simulation->vehicle() && simulation->vehicle()->isInitialConnectComplete(),
                             TestTimeout::longMs());

    struct FlowCase
    {
        bool closed;
        int start;
        int loopMode;
        int cycles;
        int returnPoint;
    };

    const QList<FlowCase> flowCases{{false, 1, 0, 1, -1}, {false, 2, 1, 2, 3}, {false, 2, 2, 1, -1}, {true, 3, 0, 1, 0},
                                    {true, 1, 1, 1, -1},  {true, 3, 1, 2, 1},  {true, 2, 2, 1, -1}};
    for (const auto& flow : flowCases) {
        auto variant = settings;
        variant["closed"] = flow.closed;
        variant["startSequence"] = flow.start;
        variant["loopMode"] = flow.loopMode;
        variant["loopCount"] = flow.cycles;
        variant["endBehavior"] = flow.returnPoint < 0 ? 0 : 1;
        variant["returnSequence"] = qMax(flow.returnPoint, 0);
        QVERIFY(editor.setSimulationSettings(variant));
        const auto beforePreview = master->saveToJson();
        const auto preview = editor.simulationPreviewSegments(variant);
        QVERIFY(!preview.isEmpty());
        QCOMPARE(master->saveToJson(), beforePreview);
        const auto hasSegment = [&preview](const QString& kind, const QGeoCoordinate& from, const QGeoCoordinate& to) {
            for (const auto& value : preview) {
                const auto segment = value.toMap();
                if (segment.value("kind").toString() == kind &&
                    QGCGeo::geodesicDistance(segment.value("coordinate1").value<QGeoCoordinate>(), from) < 0.02 &&
                    QGCGeo::geodesicDistance(segment.value("coordinate2").value<QGeoCoordinate>(), to) < 0.02) {
                    return true;
                }
            }
            return false;
        };
        const auto selectedStart = coordinates.at(flow.start - 1);
        QVERIFY(hasSegment(QStringLiteral("start"), home, selectedStart));
        if (flow.closed) {
            const auto previous = coordinates.at((flow.start - 2 + coordinates.count()) % coordinates.count());
            QVERIFY(hasSegment(QStringLiteral("closure"), previous, selectedStart));
        } else if (flow.loopMode == 2 || flow.cycles > 1) {
            QVERIFY(hasSegment(QStringLiteral("loop"), coordinates.last(), selectedStart));
        }
        const auto finish = flow.closed ? selectedStart : coordinates.last();
        const auto returned = flow.returnPoint == 0 ? home : coordinates.at(qMax(flow.returnPoint - 1, 0));
        if (flow.loopMode != 2 && flow.returnPoint >= 0 && QGCGeo::geodesicDistance(finish, returned) >= 0.01) {
            QVERIFY(hasSegment(QStringLiteral("return"), finish, returned));
        }
        QSignalSpy variantUpload(simulation, &CaacSimulationController::missionUploaded);
        QVERIFY(simulation->upload(master));
        QTRY_VERIFY_WITH_TIMEOUT(!variantUpload.isEmpty(), TestTimeout::longMs());
        QMap<uint16_t, mavlink_mission_item_int_t> compiled;
        for (const auto* item : simulation->vehicle()->missionManager()->missionItems()) {
            mavlink_mission_item_int_t wire{};
            wire.seq = static_cast<uint16_t>(item->sequenceNumber());
            wire.command = static_cast<uint16_t>(item->command());
            wire.frame = static_cast<uint8_t>(item->frame());
            wire.param1 = item->param1();
            wire.param2 = item->param2();
            wire.param3 = item->param3();
            wire.param4 = item->param4();
            wire.x = static_cast<int32_t>(std::llround(item->param5() * 1e7));
            wire.y = static_cast<int32_t>(std::llround(item->param6() * 1e7));
            wire.z = item->param7();
            compiled.insert(wire.seq, wire);
        }
        CaacSimulationEngine engine;
        QString error;
        QVERIFY2(engine.load(compiled, home, {.horizontalSpeed = 20, .verticalSpeed = 10, .turnMode = 3}, error),
                 qPrintable(error));
        QList<int> visits{0, 0, 0};
        int nearWaypoint = -1;
        for (int tick = 0; tick < 8000 && !engine.state().finished; ++tick) {
            engine.advance(0.05);
            int closest = -1;
            for (int i = 0; i < coordinates.size(); ++i) {
                if (QGCGeo::geodesicDistance(engine.state().position, coordinates.at(i)) < 0.2) {
                    closest = i;
                    break;
                }
            }
            if (closest >= 0 && closest != nearWaypoint) {
                ++visits[closest];
            }
            nearWaypoint = closest;
        }
        if (flow.loopMode == 2) {
            QVERIFY(!engine.state().finished);
            QVERIFY(visits.at(flow.start - 1) > 3);
        } else {
            QVERIFY(engine.state().finished);
            for (int i = 0; i < coordinates.size(); ++i) {
                int expected = flow.closed || i >= flow.start - 1 ? flow.cycles : 0;
                if (flow.closed && i == flow.start - 1) {
                    ++expected;
                }
                const int finalBodyPoint = flow.closed ? flow.start : coordinates.size();
                if (flow.returnPoint > 0 && flow.returnPoint != finalBodyPoint && i == flow.returnPoint - 1) {
                    ++expected;
                }
                QCOMPARE(visits.at(i), expected);
            }
            const int finalPoint =
                flow.returnPoint < 0 ? (flow.closed ? flow.start : coordinates.size()) : flow.returnPoint;
            QVERIFY(QGCGeo::geodesicDistance(engine.state().position,
                                             finalPoint == 0 ? home : coordinates.at(finalPoint - 1)) < 0.02);
            QCOMPARE(engine.state().airborne, finalPoint != 0);
        }
    }
    auto readSettings = settings;
    readSettings["startSequence"] = 1;
    readSettings["returnSequence"] = 1;
    readSettings["loopCount"] = 2;
    QVERIFY(editor.setSimulationSettings(readSettings));
    QSignalSpy beforeReadUpload(simulation, &CaacSimulationController::missionUploaded);
    QVERIFY(simulation->upload(master));
    QTRY_VERIFY_WITH_TIMEOUT(!beforeReadUpload.isEmpty(), TestTimeout::longMs());
    QSignalSpy readCompleted(mission, &MissionController::newItemsFromVehicle);
    mission->loadFromVehicle();
    QTRY_VERIFY_WITH_TIMEOUT(!readCompleted.isEmpty(), TestTimeout::longMs());
    QSignalSpy readUpload(simulation, &CaacSimulationController::missionUploaded);
    QVERIFY2(simulation->upload(master), qPrintable(simulation->status()));
    QTRY_VERIFY_WITH_TIMEOUT(!readUpload.isEmpty(), TestTimeout::longMs());
    int readWaypoints = 0;
    for (const auto* item : simulation->vehicle()->missionManager()->missionItems()) {
        if (item->command() == MAV_CMD_NAV_WAYPOINT) {
            const int expected = readWaypoints < 3 ? readWaypoints + 1 : 1;
            QCOMPARE(simulation->waypointNumber(item->sequenceNumber() + 1, item->coordinate()), expected);
            ++readWaypoints;
        }
    }
    QCOMPARE(readWaypoints, 4);
    QString restoreError;
    QVERIFY2(mission->load(original.object().value("mission").toObject(), restoreError), qPrintable(restoreError));
    QVERIFY(editor.setSimulationSettings(settings));
    QCOMPARE(master->saveToJson(), original);
    step("compiled flow variants and read/reupload verified");
    QSignalSpy uploaded(simulation, &CaacSimulationController::missionUploaded);
    QVERIFY(simulation->upload(master));
    step("upload requested");
    QTRY_VERIFY_WITH_TIMEOUT(!uploaded.isEmpty(), TestTimeout::longMs());
    auto* vehicle = simulation->vehicle();
    QList<const MissionItem*> uploadedWaypoints;
    int takeoffs = 0;
    int jumps = 0;
    for (const auto* item : vehicle->missionManager()->missionItems()) {
        if (item->command() == MAV_CMD_NAV_WAYPOINT) {
            uploadedWaypoints.append(item);
        } else if (item->command() == MAV_CMD_NAV_TAKEOFF) {
            ++takeoffs;
        } else if (item->command() == MAV_CMD_DO_JUMP) {
            ++jumps;
            QCOMPARE(item->param2(), 9.0);
            QCOMPARE(item->param1(), uploadedWaypoints.first()->sequenceNumber() + 1.0);
        }
    }
    QCOMPARE(takeoffs, 1);
    QCOMPARE(jumps, 1);
    QCOMPARE(uploadedWaypoints.size(), 4);
    const QList<int> expectedOrder{1, 2, 0, 1};
    for (int i = 0; i < expectedOrder.size(); ++i) {
        const auto* item = uploadedWaypoints.at(i);
        QVERIFY(QGCGeo::geodesicDistance(item->coordinate(), coordinates.at(expectedOrder.at(i))) < 0.02);
        QCOMPARE(simulation->waypointNumber(item->sequenceNumber() + 1, item->coordinate()), expectedOrder.at(i) + 1);
    }
    QList<int> arrivals{0, 0, 0};
    int near = -1;
    const auto arrivalConnection = connect(vehicle, &Vehicle::coordinateChanged, this, [&]() {
        int closest = -1;
        for (int i = 0; i < coordinates.size(); ++i) {
            if (QGCGeo::geodesicDistance(vehicle->coordinate(), coordinates.at(i)) < 0.2) {
                closest = i;
                break;
            }
        }
        if (closest >= 0 && closest != near) {
            ++arrivals[closest];
        }
        near = closest;
    });
    const auto disconnectArrivals = qScopeGuard([&]() { disconnect(arrivalConnection); });
    vehicle->startMission();
    step("ten-cycle mission started");
    QTRY_VERIFY_WITH_TIMEOUT(vehicle->airborne(), TestTimeout::longMs());
    QTRY_COMPARE_WITH_TIMEOUT(simulation->status(),
                              QCoreApplication::translate("CaacSimulationController", "Simulation mission finished"),
                              60000);
    QCOMPARE(arrivals, QList<int>({10, 11, 10}));
    QVERIFY(QGCGeo::geodesicDistance(vehicle->coordinate(), coordinates.at(1)) < 0.2);
    QVERIFY(vehicle->airborne());
    QVERIFY(vehicle->armed());
    QVERIFY(std::abs(vehicle->altitudeRelative()->rawValue().toDouble() - 2) < 0.1);
    QCOMPARE(master->saveToJson(), original);
}

void CaacSimulationUITest::_uploadAndFlightControls()
{
    const auto step = [](const char* label) {
        std::fprintf(stderr, "CAAC simulation step: %s\n", label);
        std::fflush(stderr);
    };
    step("start UI");
    startUI();
    if (QTest::currentTestFailed()) {
        return;
    }
    step("UI started");
    auto* startupPlugin = CaacCorePlugin::instance();
    QVERIFY(startupPlugin);
    QVERIFY(startupPlugin->firstRunPromptsToShow().isEmpty());
    auto* appSettings = SettingsManager::instance()->appSettings();
    QVERIFY(appSettings->audioMuted()->rawDefaultValue().toBool());
    QVERIFY(appSettings->audioMuted()->rawValue().toBool());
    QVERIFY(appSettings->audioMuted()->property("userVisible").toBool());
    QCOMPARE(appSettings->preferredFirmwareClass()->rawDefaultValue().toUInt(),
             static_cast<uint>(QGCMAVLink::FirmwareClassPX4));
    QCOMPARE(appSettings->preferredFirmwareClass()->rawValue().toUInt(),
             static_cast<uint>(QGCMAVLink::FirmwareClassPX4));
    QCOMPARE(appSettings->preferredVehicleClass()->rawDefaultValue().toUInt(),
             static_cast<uint>(QGCMAVLink::VehicleClassMultiRotor));
    QCOMPARE(appSettings->preferredVehicleClass()->rawValue().toUInt(),
             static_cast<uint>(QGCMAVLink::VehicleClassMultiRotor));
    auto* startupUnits = SettingsManager::instance()->unitsSettings();
    for (const auto& expected :
         {std::pair<Fact*, uint>{startupUnits->horizontalDistanceUnits(), UnitsSettings::HorizontalDistanceUnitsMeters},
          {startupUnits->verticalDistanceUnits(), UnitsSettings::VerticalDistanceUnitsMeters},
          {startupUnits->areaUnits(), UnitsSettings::AreaUnitsSquareMeters},
          {startupUnits->speedUnits(), UnitsSettings::SpeedUnitsMetersPerSecond},
          {startupUnits->temperatureUnits(), UnitsSettings::TemperatureUnitsCelsius}}) {
        QCOMPARE(expected.first->rawDefaultValue().toUInt(), expected.second);
        QCOMPARE(expected.first->rawValue().toUInt(), expected.second);
        QVERIFY(expected.first->property("userVisible").toBool());
    }
    QCOMPARE(SettingsManager::instance()->appSettings()->uiScalePercent()->rawDefaultValue().toUInt(), 120U);
    QCOMPARE(SettingsManager::instance()->appSettings()->uiScalePercent()->rawValue().toUInt(), 120U);
    QCOMPARE(QGuiApplication::applicationDisplayName(), QStringLiteral("飞虎无人机地面站系统"));
    QCOMPARE(_window->title(), QStringLiteral("飞虎无人机地面站系统"));
    QVERIFY(_window->width() >= _window->minimumWidth());
    QVERIFY(_window->height() >= _window->minimumHeight());
    QVERIFY(verifyText(QStringLiteral("caacDashboardHeadingValue"), QStringLiteral("—"), QStringLiteral("no heading")));
    auto* initialAttitude = findVisibleItem(_rootItem, QStringLiteral("caacAttitudeInstrument"));
    QVERIFY(initialAttitude);
    QVERIFY(!initialAttitude->property("attitudeAvailable").toBool());
    QVERIFY(QMetaObject::invokeMethod(_window, "showPlanView"));
    step("plan opened");
    auto* plan = findVisibleItem(_rootItem, QStringLiteral("mainView_plan"));
    QVERIFY(plan);
    auto* master = plan->property("_planMasterController").value<PlanMasterController*>();
    QVERIFY(master);
    auto* mission = master->missionController();
    const QGeoCoordinate home(30, 120, 20);
    mission->setHomePosition(home);
    auto* plugin = qobject_cast<CaacCorePlugin*>(QGCCorePlugin::instance());
    QVERIFY(plugin);
    auto* simulation = plugin->simulation();
    QVERIFY(simulation);
    const auto stop = qScopeGuard([simulation]() { simulation->stop(); });
    ignoreLogMessage("Utilities.QGCSensors", QtWarningMsg,
                     QRegularExpression(QStringLiteral("Failed to connect to pressure backend")));
    ignoreLogMessage("Utilities.QGCSensors", QtWarningMsg,
                     QRegularExpression(QStringLiteral("Error Initializing Pressure Sensor")));
    step("connect virtual aircraft");
    QVERIFY(clickButton(QStringLiteral("toolbar_qgcLogo")));
    auto* simulationButton = findVisibleItem(_rootItem, QStringLiteral("toolbar_simulation"));
    QVERIFY(simulationButton && simulationButton->isEnabled());
    QVERIFY(
        clickButtonScrolled(QStringLiteral("toolbar_simulation"), QStringLiteral("caacSettingsNavigationScroller")));
    step("connection requested");
    QTRY_VERIFY_WITH_TIMEOUT(simulation->vehicle() && simulation->vehicle()->isInitialConnectComplete(),
                             TestTimeout::longMs());
    QVERIFY(!simulation->vehicle()->armed());
    QVERIFY(!simulation->vehicle()->airborne());
    QVERIFY(!simulation->missionReady());
    QTRY_VERIFY_WITH_TIMEOUT(QGCGeo::geodesicDistance(simulation->vehicle()->coordinate(), home) < 0.02,
                             TestTimeout::mediumMs());
    QVERIFY(QGCGeo::geodesicDistance(mission->plannedHomePosition(), home) < 0.02);
    QCOMPARE(mission->plannedHomePosition().altitude(), home.altitude());
    QVERIFY(findVisibleItem(_rootItem, QStringLiteral("mainView_plan")));
    for (int bearing : {0, 90}) {
        auto* item = qobject_cast<SimpleMissionItem*>(mission->insertSimpleMissionItem(
            QGCGeo::geodesicDestination(home, bearing, 1000), mission->visualItems()->count(), false));
        QVERIFY(item);
        item->altitude()->setRawValue(2);
    }
    CaacRouteEditorController editor;
    editor.setPlanMasterController(master);
    auto settings = CaacSimulationController::defaults();
    settings["takeoffHeight"] = 2.0;
    settings["verticalSpeed"] = 10.0;
    settings["loopMode"] = 1;
    settings["loopCount"] = 3;
    settings["closed"] = true;
    QVERIFY(editor.setSimulationSettings(settings));
    const auto original = master->saveToJson();
    step("upload simulation");
    QVERIFY(clickButton(QStringLiteral("caacWaypointListButton")));
    QSignalSpy initialUpload(simulation, &CaacSimulationController::missionUploaded);
    QVERIFY(initialUpload.isValid());
    QVERIFY(clickButton(QStringLiteral("caacUploadWaypoints")));
    QTRY_VERIFY_WITH_TIMEOUT(!initialUpload.isEmpty(), TestTimeout::longMs());
    QVERIFY(simulation->missionReady());
    QVERIFY(!findItem(_rootItem, QStringLiteral("caacSimulationBanner")));
    QVERIFY(!simulation->vehicle()->armed());
    QVERIFY(!simulation->vehicle()->airborne());
    QVERIFY(!findVisibleItem(_rootItem, QStringLiteral("caacArmMenuButton"), 0));
    QVERIFY(!findVisibleItem(_rootItem, QStringLiteral("caacVehicleMessagesButton"), 0));
    const QStringList statusButtons{QStringLiteral("caacSensorStatusButton")};
    const QStringList statusSections{QStringLiteral("mainStatusSensorSection")};
    for (int section = 0; section < statusButtons.size(); ++section) {
        QVERIFY(clickButton(statusButtons[section]));
        QVERIFY(findVisibleItem(_rootItem, statusSections[section]));
        for (int other = 0; other < statusSections.size(); ++other) {
            if (other != section) {
                QVERIFY(!findVisibleItem(_rootItem, statusSections[other], 0));
            }
        }
        QQmlExpression closeDrawer(qmlContext(plan), plan, QStringLiteral("mainWindow.closeIndicatorDrawer()"));
        closeDrawer.evaluate();
        QVERIFY(!closeDrawer.hasError());
        QTRY_VERIFY_WITH_TIMEOUT(!findVisibleItem(_rootItem, statusSections[section], 0), TestTimeout::mediumMs());
    }
    QVERIFY(!findVisibleItem(_rootItem, QStringLiteral("caacFlightSettingsPanel"), 0));
    QVERIFY(clickButton(QStringLiteral("caacFlightSettingsButton")));
    auto* flightActions = findVisibleItem(_rootItem, QStringLiteral("flyViewToolStrip_flickable"));
    auto* toolbar = findVisibleItem(_rootItem, QStringLiteral("flyViewToolStrip"));
    auto* menu = findVisibleItem(_rootItem, QStringLiteral("caacFlightSettingsPanel"));
    auto* host = findVisibleItem(_rootItem, QStringLiteral("caacTopFlightActions"));
    QVERIFY(flightActions && toolbar && menu && host);
    QVERIFY(!flightActions->property("interactive").toBool());
    QTRY_VERIFY_WITH_TIMEOUT(flightActions->property("contentWidth").toDouble() <= flightActions->width() + 1,
                             TestTimeout::mediumMs());
    QTRY_COMPARE_WITH_TIMEOUT(toolbar->property("_roundColumns").toInt(), 4, TestTimeout::mediumMs());
    const QStringList actionOrder{QStringLiteral("caacTakeoff"),           QStringLiteral("caacReturnHome"),
                                  QStringLiteral("caacStartMission"),      QStringLiteral("caacPauseMission"),
                                  QStringLiteral("caacJumpWaypoint"),      QStringLiteral("caacModifyWaypointAltitude"),
                                  QStringLiteral("caacChangeFlightSpeed"), QStringLiteral("caacSimulationEmergency")};
    auto* firstAction = findVisibleItem(_rootItem, actionOrder.first());
    QVERIFY(firstAction);
    const auto firstPosition = firstAction->mapToScene(QPointF());
    for (int i = 0; i < actionOrder.size(); ++i) {
        auto* action = findVisibleItem(_rootItem, actionOrder[i]);
        QVERIFY(action);
        const auto position = action->mapToScene(QPointF());
        auto* rowStart = findVisibleItem(_rootItem, actionOrder[(i / 4) * 4]);
        QVERIFY(rowStart);
        QVERIFY(std::abs(position.y() - rowStart->mapToScene(QPointF()).y()) < 1);
        if (i % 4 > 0) {
            auto* previous = findVisibleItem(_rootItem, actionOrder[i - 1]);
            QVERIFY(position.x() > previous->mapToScene(QPointF()).x());
        }
        if (i >= 4) {
            QVERIFY(position.y() > firstPosition.y() + firstAction->height());
        }
        QVERIFY(std::abs(action->property("roundDiameter").toDouble() -
                         firstAction->property("roundDiameter").toDouble()) < 1);
        const auto bounds = action->mapRectToItem(menu, action->boundingRect());
        QVERIFY(bounds.left() >= 0 && bounds.right() <= menu->width() + 1);
        QVERIFY(bounds.top() >= 0 && bounds.bottom() <= menu->height() + 1);
        auto* label = findVisibleItem(_rootItem, actionOrder[i] + QStringLiteral("_label"));
        QVERIFY(label);
        const auto labelBounds = label->mapRectToItem(flightActions, label->boundingRect());
        QVERIFY(labelBounds.top() >= 0 && labelBounds.bottom() <= flightActions->height() + 1);
    }
    auto* settingsButton = findVisibleItem(_rootItem, QStringLiteral("caacFlightSettingsButton"));
    QVERIFY(settingsButton);
    const auto buttonBounds = settingsButton->mapRectToItem(menu->parentItem(), settingsButton->boundingRect());
    QVERIFY(menu->y() >= buttonBounds.bottom());
    QVERIFY(menu->y() - buttonBounds.bottom() < settingsButton->height());
    auto* header = host->parentItem();
    QVERIFY(header);
    QCOMPARE(header->height(), header->property("_statusHeight").toDouble());
    QVERIFY(!findVisibleItem(_rootItem, QStringLiteral("toolbar_mainStatusIndicator"), 0));
    for (const auto* name : {"caacFlightSettingsButton", "caacSensorStatusButton"}) {
        auto* button = findVisibleItem(_rootItem, QLatin1String(name));
        QVERIFY(button);
        const auto center = button->mapToItem(header, QPointF(button->width() / 2, button->height() / 2));
        QVERIFY(std::abs(center.y() - header->height() / 2) < 1);
    }
    QVERIFY(clickButton(QStringLiteral("caacFlightSettingsButton")));
    QVERIFY(!findVisibleItem(_rootItem, QStringLiteral("caacFlightSettingsPanel"), 0));
    QVERIFY(clickButton(QStringLiteral("caacFlightSettingsButton")));
    const auto originalWindowSize = _window->size();
    auto* indicators = findVisibleItem(_rootItem, QStringLiteral("flyViewToolBarIndicators"));
    QVERIFY(indicators);
    QVERIFY(_window->screen());
    const auto frameMargins = _window->frameMargins();
    const int availableWidth =
        _window->screen()->availableGeometry().width() - frameMargins.left() - frameMargins.right();
    QVERIFY(availableWidth >= _window->minimumWidth());
    for (const int requestedWidth : {800, 1600}) {
        _window->resize(std::clamp(requestedWidth, _window->minimumWidth(), availableWidth),
                        originalWindowSize.height());
        QTRY_VERIFY_WITH_TIMEOUT(flightActions->property("contentWidth").toDouble() <= flightActions->width(),
                                 TestTimeout::mediumMs());
        QCOMPARE(header->height(), header->property("_statusHeight").toDouble());
        QVERIFY(host->mapToScene(QPointF(host->width(), 0)).x() <= indicators->mapToScene(QPointF(0, 0)).x() + 1);
        auto* navigation = findVisibleItem(_rootItem, QStringLiteral("caacRouteEditor"));
        auto* instrument = findVisibleItem(_rootItem, QStringLiteral("caacAttitudeInstrument"));
        QVERIFY(navigation);
        QVERIFY(instrument);
        QTRY_VERIFY_WITH_TIMEOUT(
            !instrument->mapRectToScene(instrument->boundingRect())
                 .intersects(navigation->mapRectToScene(
                     QRectF(0, 0, navigation->width(), navigation->property("navigationHeight").toDouble()))),
            TestTimeout::mediumMs());
        if (qEnvironmentVariableIsSet("CAAC_SIMULATION_UI_SCREENSHOT")) {
            QSignalSpy frame(_window, &QQuickWindow::frameSwapped);
            _window->update();
            QTRY_VERIFY_WITH_TIMEOUT(!frame.isEmpty(), TestTimeout::mediumMs());
            QVERIFY(
                _window->grabWindow().save(QStringLiteral("/workspace/caac-inline-topbar-%1.png").arg(requestedWidth)));
        }
    }
    _window->resize(originalWindowSize);
    const auto doubleClickAction = [&](const QString& name) {
        if (!findVisibleItem(_rootItem, QStringLiteral("caacFlightSettingsPanel"), 0) &&
            !clickButton(QStringLiteral("caacFlightSettingsButton"))) {
            return false;
        }
        auto* button = findVisibleItemScrolled(name, QStringLiteral("flyViewToolStrip_flickable"));
        if (!button || !button->isEnabled()) {
            return false;
        }
        QTest::mouseDClick(_window, Qt::LeftButton, Qt::NoModifier,
                           button->mapToScene(QPointF(button->width() / 2, button->height() / 2)).toPoint());
        return true;
    };
    step("manual takeoff");
    QVERIFY(clickButtonScrolled(QStringLiteral("caacTakeoff"), QStringLiteral("flyViewToolStrip_flickable")));
    QVERIFY(!simulation->vehicle()->airborne());
    QVERIFY(!findVisibleItem(_rootItem, QStringLiteral("guidedActionConfirmButton"), 0));
    QVERIFY(doubleClickAction(QStringLiteral("caacTakeoff")));
    QTRY_VERIFY2_WITH_TIMEOUT(simulation->vehicle() && simulation->vehicle()->airborne(),
                              qPrintable(simulation->status()), TestTimeout::longMs());
    step("airborne");
    auto* vehicle = simulation->vehicle();
    QVERIFY(vehicle);
    QVERIFY(verifyText(QStringLiteral("caacArmStatusValue"), QStringLiteral("解锁"),
                       QStringLiteral("read-only armed status")));
    QCOMPARE(MultiVehicleManager::instance()->vehicles()->count(), 1);
    auto* attitude = findVisibleItem(_rootItem, QStringLiteral("caacAttitudeInstrument"));
    QVERIFY(attitude);
    QTRY_VERIFY_WITH_TIMEOUT(attitude->property("attitudeAvailable").toBool(), TestTimeout::mediumMs());
    QCOMPARE(attitude->property("rollAngle").toDouble(), vehicle->roll()->rawValue().toDouble());
    QCOMPARE(attitude->property("pitchAngle").toDouble(), vehicle->pitch()->rawValue().toDouble());
    QCOMPARE(attitude->property("headingAngle").toDouble(), vehicle->heading()->rawValue().toDouble());
    QVERIFY(findVisibleItem(_rootItem, QStringLiteral("caacFlightSettingsPanel")));
    QVERIFY(clickButton(QStringLiteral("caacFlightSettingsButton")));
    QVERIFY(clickButton(QStringLiteral("caacFlightSettingsButton")));
    QVERIFY(verifyText(QStringLiteral("caacModifyWaypointAltitude"),
                       QCoreApplication::translate("CaacFlightActionList", "Change altitude"),
                       QStringLiteral("independent altitude button")));
    QTRY_COMPARE_WITH_TIMEOUT(vehicle->flightMode(), vehicle->pauseFlightMode(), TestTimeout::longMs());
    step("start route from flight controls");
    auto* startAction = findVisibleItem(_rootItem, QStringLiteral("caacStartMission"));
    QVERIFY(startAction);
    QTRY_VERIFY_WITH_TIMEOUT(startAction->isEnabled(), TestTimeout::mediumMs());
    QVERIFY(clickButtonScrolled(QStringLiteral("caacStartMission"), QStringLiteral("flyViewToolStrip_flickable")));
    QCOMPARE(vehicle->flightMode(), vehicle->pauseFlightMode());
    QVERIFY(!findVisibleItem(_rootItem, QStringLiteral("guidedActionConfirmButton"), 0));
    QVERIFY(doubleClickAction(QStringLiteral("caacStartMission")));
    QVERIFY(!findVisibleItem(_rootItem, QStringLiteral("guidedActionConfirmButton"), 0));
    QTRY_COMPARE_WITH_TIMEOUT(vehicle->flightMode(), vehicle->missionFlightMode(), TestTimeout::mediumMs());
    auto* manager = vehicle->missionManager();
    QVERIFY(!manager->inProgress());
    step("reject upload while mission is running");
    QSignalSpy blockedTransfer(manager, &MissionManager::sendComplete);
    const auto originalAircraftItems = manager->missionItems();
    expectAppMessage(QRegularExpression(QStringLiteral("当前航点任务正在执行，请先暂停当前任务，再上传新航线。")));
    QVERIFY(!simulation->upload(master));
    verifyExpectedLogMessage();
    QVERIFY(waitForDialog(QStringLiteral("上传航线")));
    QVERIFY(acceptDialog());
    QVERIFY(QMetaObject::invokeMethod(master, "upload"));
    QVERIFY(waitForDialog(QStringLiteral("上传航线")));
    QVERIFY(acceptDialog());
    QCOMPARE(manager->missionItems(), originalAircraftItems);
    QCOMPARE(blockedTransfer.count(), 0);
    QVERIFY(!manager->inProgress());
    QCOMPARE(vehicle->flightMode(), vehicle->missionFlightMode());
    int takeoffs = 0;
    int jumps = 0;
    int firstWaypointSequence = -1;
    int loopTarget = -1;
    for (const auto* item : manager->missionItems()) {
        if (item->command() == MAV_CMD_NAV_TAKEOFF) {
            ++takeoffs;
            QCOMPARE(item->param7(), 2.0);
            QVERIFY(QGCGeo::geodesicDistance(item->coordinate(), home) < 0.02);
        }
        if (item->command() == MAV_CMD_NAV_WAYPOINT && firstWaypointSequence < 0) {
            firstWaypointSequence = item->sequenceNumber();
        }
        if (item->command() == MAV_CMD_DO_JUMP) {
            loopTarget = static_cast<int>(item->param1());
            ++jumps;
            QCOMPARE(item->param2(), 2.0);
        }
    }
    QCOMPARE(takeoffs, 1);
    QCOMPARE(jumps, 1);
    QVERIFY(firstWaypointSequence > 0);
    QCOMPARE(loopTarget, firstWaypointSequence + 1);
    QCOMPARE(master->saveToJson(), original);

    QVERIFY(!findItem(_rootItem, QStringLiteral("caacSimulationBanner")));
    QTRY_VERIFY_WITH_TIMEOUT(vehicle->groundSpeed()->rawValue().toDouble() > 0, TestTimeout::mediumMs());
    QTRY_VERIFY_WITH_TIMEOUT(vehicle->pitch()->rawValue().toDouble() < -1, TestTimeout::mediumMs());
    auto* heading = findVisibleItem(_rootItem, QStringLiteral("caacDashboardHeadingValue"));
    QVERIFY(heading);
    QTRY_COMPARE_WITH_TIMEOUT(heading->property("text").toString(),
                              vehicle->heading()->cookedValueString() + " " + vehicle->heading()->cookedUnits(),
                              TestTimeout::mediumMs());
    step("emergency blackout");
    const auto positionBeforeEmergency = vehicle->coordinate();
    const auto modeBeforeEmergency = vehicle->flightMode();
    QVERIFY(doubleClickAction(QStringLiteral("caacSimulationEmergency")));
    QVERIFY(simulation->emergencyMode());
    QVERIFY(findVisibleItem(_rootItem, QStringLiteral("caacEmergencyBlackout")));
    auto* emergencyAttitude = findVisibleItem(_rootItem, QStringLiteral("caacEmergencyAttitude"));
    QVERIFY(emergencyAttitude);
    QSignalSpy emergencyFrame(_window, &QQuickWindow::frameSwapped);
    _window->update();
    QVERIFY(emergencyFrame.wait(TestTimeout::mediumMs()));
    const QRectF emergencyRect(emergencyAttitude->mapToScene(QPointF(0, 0)),
                               QSizeF(emergencyAttitude->width(), emergencyAttitude->height()));
    const QRectF toolbarRect(header->mapToScene(QPointF(0, 0)), QSizeF(header->width(), header->height()));
    QVERIFY(findVisibleItem(_rootItem, QStringLiteral("caacFlightSettingsPanel")));
    QVERIFY(clickButton(QStringLiteral("caacFlightSettingsButton")));
    QVERIFY(!findVisibleItem(_rootItem, QStringLiteral("caacFlightSettingsPanel"), 0));
    QVERIFY(!emergencyRect.intersects(toolbarRect));
    auto* emergencyControls = findVisibleItem(_rootItem, QStringLiteral("caacSimulationManagement"));
    QVERIFY(emergencyControls);
    const QRectF emergencyControlsRect(emergencyControls->mapToScene(QPointF(0, 0)),
                                       QSizeF(emergencyControls->width(), emergencyControls->height()));
    for (const auto& entry : {std::pair{QStringLiteral("Altitude"), vehicle->altitudeRelative()},
                              std::pair{QStringLiteral("Speed"), vehicle->groundSpeed()},
                              std::pair{QStringLiteral("Heading"), vehicle->heading()}}) {
        auto* value =
            findVisibleItem(_rootItem, QStringLiteral("caacEmergency") + entry.first + QStringLiteral("Value"));
        QVERIFY(value);
        QTRY_VERIFY_WITH_TIMEOUT(value->property("text").toString().endsWith(entry.second->cookedValueString() + " " +
                                                                             entry.second->cookedUnits()),
                                 TestTimeout::mediumMs());
        QVERIFY(value->mapToScene(QPointF(0, 0)).y() >= emergencyRect.bottom());
        QVERIFY(value->property("contentWidth").toDouble() <= value->width() + 1);
        const QRectF valueRect(value->mapToScene(QPointF(0, 0)), QSizeF(value->width(), value->height()));
        QVERIFY(!valueRect.intersects(emergencyControlsRect));
    }
    QVERIFY(!findVisibleItem(_rootItem, QStringLiteral("planView_map"), 0));
    QVERIFY(!findVisibleItem(_rootItem, QStringLiteral("caacDashboardHeadingValue"), 0));
    QVERIFY(!findVisibleItem(_rootItem, QStringLiteral("caacFlightDataButton"), 0));
    QTRY_VERIFY_WITH_TIMEOUT(QGCGeo::geodesicDistance(positionBeforeEmergency, vehicle->coordinate()) > 1,
                             TestTimeout::mediumMs());
    QCOMPARE(vehicle->flightMode(), modeBeforeEmergency);
    if (qEnvironmentVariableIsSet("CAAC_SIMULATION_UI_SCREENSHOT")) {
        QVERIFY(_window->grabWindow().save(QStringLiteral("/workspace/caac-emergency-attitude.png")));
    }
    QVERIFY(doubleClickAction(QStringLiteral("caacSimulationEmergency")));
    QVERIFY(!simulation->emergencyMode());
    QVERIFY(findVisibleItem(_rootItem, QStringLiteral("planView_map")));
    QCOMPARE(vehicle->flightMode(), modeBeforeEmergency);
    const auto link = vehicle->vehicleLinkManager()->primaryLink().lock();
    auto* mock = qobject_cast<MockLink*>(link.get());
    QVERIFY(mock);
    step("pause");
    mock->clearReceivedMavCommandCounts();
    QVERIFY(doubleClickAction(QStringLiteral("caacPauseMission")));
    QVERIFY(!findVisibleItem(_rootItem, QStringLiteral("guidedActionConfirmButton"), 0));
    QTRY_COMPARE_WITH_TIMEOUT(vehicle->flightMode(), vehicle->pauseFlightMode(), TestTimeout::mediumMs());
    QTRY_COMPARE_WITH_TIMEOUT(vehicle->groundSpeed()->rawValue().toDouble(), 0.0, TestTimeout::mediumMs());
    QVERIFY(mock->receivedMavCommandCount(MAV_CMD_DO_REPOSITION) > 0);
    const auto pausedPosition = vehicle->coordinate();
    QSignalSpy heartbeat(vehicle, &Vehicle::mavlinkMessageReceived);
    QVERIFY(heartbeat.isValid());
    QVERIFY(heartbeat.wait(TestTimeout::mediumMs()));
    QVERIFY(QGCGeo::geodesicDistance(pausedPosition, vehicle->coordinate()) < 0.02);
    step("change flight speed");
    mock->clearReceivedMavCommandCounts();
    QVERIFY(doubleClickAction(QStringLiteral("caacChangeFlightSpeed")));
    auto* speedValue = findVisibleItem(_rootItem, QStringLiteral("caacFlightSpeedValue"));
    QVERIFY(speedValue);
    QVERIFY(speedValue->setProperty("text", QStringLiteral("0")));
    QVERIFY(acceptDialog());
    QVERIFY(dialogVisible(QCoreApplication::translate("CaacFlightSpeedDialog", "Change flight speed")));
    QCOMPARE(mock->receivedMavCommandCount(MAV_CMD_DO_CHANGE_SPEED), 0);
    QVERIFY(speedValue->setProperty("text", QStringLiteral("8")));
    QSignalSpy speedAck(vehicle, &Vehicle::mavCommandResult);
    QVERIFY(speedAck.isValid());
    QVERIFY(acceptDialog());
    QTRY_COMPARE_WITH_TIMEOUT(mock->receivedMavCommandCount(MAV_CMD_DO_CHANGE_SPEED), 1, TestTimeout::mediumMs());
    const auto speedAccepted = [&speedAck]() {
        for (const auto& result : speedAck) {
            if (result[2].toInt() == MAV_CMD_DO_CHANGE_SPEED && result[3].toInt() == MAV_RESULT_ACCEPTED) {
                return true;
            }
        }
        return false;
    };
    QTRY_VERIFY_WITH_TIMEOUT(speedAccepted(), TestTimeout::mediumMs());
    QCOMPARE(master->saveToJson(), original);
    QVERIFY(findVisibleItem(_rootItem, QStringLiteral("caacFlightSettingsPanel")));
    QVERIFY(clickButton(QStringLiteral("caacFlightSettingsButton")));
    QVERIFY(!findVisibleItem(_rootItem, QStringLiteral("caacFlightSettingsPanel"), 0));
    auto* layer = findVisibleItem(_rootItem, QStringLiteral("caacFlyLayer"));
    QVERIFY(layer);
    auto* map = layer->property("mapControl").value<QObject*>();
    QVERIFY(map);
    QQmlExpression liveMaster(qmlContext(layer), layer, QStringLiteral("globals.planMasterControllerFlyView"));
    auto* flyMaster = liveMaster.evaluate().value<PlanMasterController*>();
    QVERIFY(!liveMaster.hasError());
    QVERIFY(flyMaster);
    auto* flyMission = flyMaster->missionController();
    for (int i = 1; i < flyMission->visualItems()->count(); ++i) {
        auto* item = flyMission->visualItems()->value<SimpleMissionItem*>(i);
        if (item && item->command() == MAV_CMD_NAV_WAYPOINT) {
            const int expected =
                QGCGeo::geodesicDistance(item->coordinate(),
                                         mission->visualItems()->value<SimpleMissionItem*>(1)->coordinate()) < 0.02
                    ? 1
                    : 2;
            QCOMPARE(simulation->waypointNumber(item->sequenceNumber(), item->coordinate()), expected);
            auto* mapLabel =
                findItem(qobject_cast<QQuickItem*>(map), QStringLiteral("missionItemMapLabel_%1").arg(expected));
            QVERIFY(mapLabel);
            QCOMPARE(mapLabel->property("index").toInt(), expected);
        }
    }
    const auto chooseLastWaypoint = [&]() -> SimpleMissionItem* {
        SimpleMissionItem* last = nullptr;
        int lastNumber = -1;
        for (int i = 1; i < flyMission->visualItems()->count(); ++i) {
            auto* item = flyMission->visualItems()->value<SimpleMissionItem*>(i);
            if (item && item->command() == MAV_CMD_NAV_WAYPOINT) {
                const int number = simulation->waypointNumber(item->sequenceNumber(), item->coordinate());
                if (number > lastNumber) {
                    last = item;
                    lastNumber = number;
                }
            }
        }
        return last;
    };
    step("live altitude change");
    // Exercise scrolling at the compact desktop size used by Windows runners.
    const QSize previousWindowSize = _window->size();
    const QSize previousMinimumSize = _window->minimumSize();
    _window->setMinimumSize(QSize(0, 0));
    _window->resize(1000, 500);
    {
        auto* units = SettingsManager::instance()->unitsSettings()->verticalDistanceUnits();
        const auto previousUnits = units->rawValue();
        const auto restoreUnits = qScopeGuard([units, previousUnits]() { units->setRawValue(previousUnits); });
        for (const auto unit : {UnitsSettings::VerticalDistanceUnitsMeters, UnitsSettings::VerticalDistanceUnitsFeet}) {
            units->setRawValue(unit);
            const double targetMeters = unit == UnitsSettings::VerticalDistanceUnitsMeters ? 4.0 : 4.572;
            QVERIFY(doubleClickAction(QStringLiteral("caacModifyWaypointAltitude")));
            step("height dialog opened");
            QVERIFY(clickButtonScrolled(QStringLiteral("caacChangeFlightAltitude"),
                                        QStringLiteral("popupDialog_contentFlickable")));
            auto* liveAltitude = findVisibleItem(_rootItem, QStringLiteral("caacFlightWaypointAltitude"));
            QVERIFY(liveAltitude);
            QVERIFY(!findVisibleItem(_rootItem, QStringLiteral("caacFlightWaypointChoice"), 0));
            QVERIFY(liveAltitude->setProperty("text", QString::number(targetMeters)));
            mock->clearReceivedMavCommandCounts();
            QVERIFY(acceptDialog());
            QVERIFY(!findVisibleItem(_rootItem, QStringLiteral("guidedActionConfirmButton"), 0));
            step("live height command sent");
            QTRY_VERIFY_WITH_TIMEOUT(mock->receivedMavCommandCount(MAV_CMD_DO_REPOSITION) > 0, TestTimeout::mediumMs());
            QTRY_VERIFY_WITH_TIMEOUT(std::abs(vehicle->altitudeRelative()->rawValue().toDouble() - targetMeters) < 0.1,
                                     TestTimeout::longMs());
        }
    }
    _window->setMinimumSize(previousMinimumSize);
    _window->resize(previousWindowSize);
    step("height edit");
    auto* target = chooseLastWaypoint();
    QVERIFY(target);
    const auto targetSequence = target->sequenceNumber();
    const auto previousAltitude = target->altitude()->rawValue();
    const auto beforeInvalidBulk = flyMaster->saveToJson();
    QVERIFY(!plugin
                 ->updateFlightWaypointAltitudes(flyMaster,
                                                 {QVariant::fromValue(static_cast<QObject*>(target)),
                                                  QVariant::fromValue(static_cast<QObject*>(nullptr))},
                                                 10)
                 .isEmpty());
    QCOMPARE(flyMaster->saveToJson(), beforeInvalidBulk);
    QVERIFY(!plugin->updateFlightWaypointAltitude(flyMaster, target, qQNaN()).isEmpty());
    QCOMPARE(target->altitude()->rawValue(), previousAltitude);
    QVERIFY(doubleClickAction(QStringLiteral("caacModifyWaypointAltitude")));
    auto* choices = findVisibleItem(_rootItem, QStringLiteral("caacFlightWaypointChoice"));
    QVERIFY(choices);
    QCOMPARE(choices->property("count").toInt(), 3);
    QVERIFY(choices->setProperty("currentIndex", choices->property("count").toInt() - 1));
    QCOMPARE(choices->property("currentText").toString(),
             QCoreApplication::translate("CaacMissionActionDialog", "Waypoint %1").arg(2));
    auto* altitude = findVisibleItem(_rootItem, QStringLiteral("caacFlightWaypointAltitude"));
    QVERIFY(altitude);
    QVERIFY(altitude->setProperty("text", "3"));
    QSignalSpy upload(manager, &MissionManager::sendComplete);
    QVERIFY(upload.isValid());
    QVERIFY(acceptDialog());
    QTRY_VERIFY_WITH_TIMEOUT(!upload.isEmpty(), TestTimeout::mediumMs());
    QCOMPARE(upload.last().first().toBool(), false);
    QTRY_VERIFY_WITH_TIMEOUT(!manager->inProgress(), TestTimeout::mediumMs());
    bool uploadedAltitude = false;
    for (const auto* item : manager->missionItems()) {
        if (item->sequenceNumber() == targetSequence - 1 && item->command() == MAV_CMD_NAV_WAYPOINT) {
            uploadedAltitude = item->param7() == 3;
        }
    }
    QVERIFY(uploadedAltitude);
    step("jump");
    QVERIFY(doubleClickAction(QStringLiteral("caacJumpWaypoint")));
    QVERIFY(!findVisibleItem(_rootItem, QStringLiteral("caacFlightWaypointAltitude"), 0));
    QVERIFY(!findVisibleItem(_rootItem, QStringLiteral("caacChangeFlightAltitude"), 0));
    choices = findVisibleItem(_rootItem, QStringLiteral("caacFlightWaypointChoice"));
    QVERIFY(choices);
    QCOMPARE(choices->property("count").toInt(), 2);
    QVERIFY(choices->setProperty("currentIndex", choices->property("count").toInt() - 1));
    QVERIFY(acceptDialog());
    QVERIFY(!findVisibleItem(_rootItem, QStringLiteral("guidedActionConfirmButton"), 0));
    QTRY_COMPARE_WITH_TIMEOUT(manager->currentIndex(), targetSequence - 1, TestTimeout::mediumMs());
    step("continue");
    QVERIFY(doubleClickAction(QStringLiteral("caacStartMission")));
    QVERIFY(!findVisibleItem(_rootItem, QStringLiteral("guidedActionConfirmButton"), 0));
    QTRY_COMPARE_WITH_TIMEOUT(vehicle->flightMode(), vehicle->missionFlightMode(), TestTimeout::mediumMs());
    QTRY_VERIFY_WITH_TIMEOUT(vehicle->groundSpeed()->rawValue().toDouble() > 0, TestTimeout::mediumMs());
    const auto screenshot = qEnvironmentVariable("CAAC_SIMULATION_UI_SCREENSHOT");
    if (!screenshot.isEmpty()) {
        QSignalSpy frame(_window, &QQuickWindow::frameSwapped);
        _window->update();
        QVERIFY(frame.wait(TestTimeout::mediumMs()));
        QVERIFY(_window->grabWindow().save(screenshot));
    }
    QVERIFY(plugin->flightDistance());
    QTRY_VERIFY_WITH_TIMEOUT(plugin->flightDistance()->north() + plugin->flightDistance()->east() > 0,
                             TestTimeout::mediumMs());
    step("all waypoint heights while flying");
    QVERIFY(vehicle->flightMode() == vehicle->missionFlightMode());
    const auto heightBeforeAll = vehicle->altitudeRelative()->rawValue().toDouble();
    QVERIFY(doubleClickAction(QStringLiteral("caacModifyWaypointAltitude")));
    choices = findVisibleItem(_rootItem, QStringLiteral("caacFlightWaypointChoice"));
    altitude = findVisibleItem(_rootItem, QStringLiteral("caacFlightWaypointAltitude"));
    QVERIFY(choices && altitude);
    QVERIFY(choices->setProperty("currentIndex", 0));
    QCOMPARE(choices->property("currentText").toString(),
             QCoreApplication::translate("CaacMissionActionDialog", "All waypoints"));
    QVERIFY(altitude->setProperty("text", "100"));
    QSignalSpy bulkUpload(manager, &MissionManager::sendComplete);
    QVERIFY(bulkUpload.isValid());
    QVERIFY(acceptDialog());
    QTRY_VERIFY_WITH_TIMEOUT(!bulkUpload.isEmpty(), TestTimeout::longMs());
    QCOMPARE(bulkUpload.last().first().toBool(), false);
    QTRY_COMPARE_WITH_TIMEOUT(vehicle->flightMode(), vehicle->missionFlightMode(), TestTimeout::longMs());
    int updatedWaypoints = 0;
    for (const auto* item : manager->missionItems()) {
        if (item->command() == MAV_CMD_NAV_LOITER_UNLIM) {
            QCOMPARE(item->param7(), 100.0);
        }
        if (item->command() == MAV_CMD_NAV_WAYPOINT) {
            QCOMPARE(item->param7(), 100.0);
            ++updatedWaypoints;
        }
    }
    QVERIFY(updatedWaypoints >= 2);
    QTRY_VERIFY_WITH_TIMEOUT(vehicle->altitudeRelative()->rawValue().toDouble() > heightBeforeAll + 0.1,
                             TestTimeout::longMs());
    QTRY_VERIFY_WITH_TIMEOUT(std::abs(vehicle->pitch()->rawValue().toDouble()) > 0.1, TestTimeout::mediumMs());
    QTRY_COMPARE_WITH_TIMEOUT(attitude->property("pitchAngle").toDouble(), vehicle->pitch()->rawValue().toDouble(),
                              TestTimeout::mediumMs());

    QVERIFY(vehicle->batteries()->count() > 0);
    for (int i = 0; i < vehicle->batteries()->count(); ++i) {
        auto* battery = vehicle->batteries()->value<BatteryFactGroup*>(i);
        QVERIFY(battery);
        QCOMPARE(battery->percentRemaining()->rawValue().toDouble(), 100.0);
        QCOMPARE(battery->chargeState()->rawValue().toInt(), static_cast<int>(MAV_BATTERY_CHARGE_STATE_OK));
    }
    step("right-click guided point");
    QVERIFY(QMetaObject::invokeMethod(_window, "showPlanView"));
    auto* planMap = findVisibleItem(_rootItem, QStringLiteral("planView_map"));
    QVERIFY(planMap);
    auto* localWaypoint = mission->visualItems()->value<SimpleMissionItem*>(1);
    QVERIFY(localWaypoint);
    const auto gotoPoint = QGCGeo::geodesicDestination(vehicle->coordinate(), 90, 10);
    localWaypoint->setCoordinate(gotoPoint);
    planMap->setProperty("zoomLevel", 18);
    planMap->setProperty("center", QVariant::fromValue(gotoPoint));
    auto* dragger = findVisibleItem(_rootItem, QStringLiteral("missionItemDrag_1"), TestTimeout::mediumMs());
    QVERIFY(dragger);
    auto* indicator = dragger->property("itemIndicator").value<QQuickItem*>();
    QVERIFY(indicator);
    const auto markerPosition = [&]() {
        return dragger->mapToScene(indicator->property("anchorPoint").toPointF() +
                                   QPointF(dragger->property("_touchMarginHorizontal").toDouble(),
                                           dragger->property("_touchMarginVertical").toDouble()));
    };
    const auto middle = planMap->mapToScene(QPointF(planMap->width() / 2, planMap->height() / 2));
    QTRY_VERIFY_WITH_TIMEOUT(QLineF(markerPosition(), middle).length() < 1, TestTimeout::mediumMs());
    mock->clearReceivedMavCommandCounts();
    QTest::mouseClick(_window, Qt::RightButton, Qt::NoModifier, markerPosition().toPoint());
    auto* gotoButton = findVisibleItem(_rootItem, QStringLiteral("caacContextGoto"));
    QVERIFY(gotoButton && gotoButton->isEnabled());
    QVERIFY(clickButton(QStringLiteral("caacContextGoto")));
    auto* confirmation = findVisibleItem(_rootItem, QStringLiteral("guidedActionConfirmButton"));
    QVERIFY(confirmation);
    QVERIFY(QMetaObject::invokeMethod(confirmation, "activated"));
    QTRY_VERIFY_WITH_TIMEOUT(std::abs(vehicle->roll()->rawValue().toDouble()) > 0.1, TestTimeout::mediumMs());
    QTRY_COMPARE_WITH_TIMEOUT(attitude->property("rollIndicatorAngle").toDouble(),
                              vehicle->roll()->rawValue().toDouble(), TestTimeout::mediumMs());
    if (qEnvironmentVariableIsSet("CAAC_SIMULATION_UI_SCREENSHOT")) {
        QSignalSpy frame(_window, &QQuickWindow::frameSwapped);
        _window->update();
        QVERIFY(frame.wait(TestTimeout::mediumMs()));
        QVERIFY(_window->grabWindow().save(QStringLiteral("/workspace/caac-turning-attitude.png")));
    }
    QTRY_VERIFY_WITH_TIMEOUT(QGCGeo::geodesicDistance(vehicle->coordinate(), gotoPoint) < 0.1, TestTimeout::mediumMs());
    QVERIFY(mock->receivedMavCommandCount(MAV_CMD_DO_REPOSITION) > 0);
    QTRY_COMPARE_WITH_TIMEOUT(vehicle->flightMode(), vehicle->pauseFlightMode(), TestTimeout::mediumMs());

    step("waypoint list read and upload");
    QVERIFY(QMetaObject::invokeMethod(_window, "showPlanView"));
    QVERIFY(clickButton(QStringLiteral("caacWaypointListButton")));
    auto* readButton = findVisibleItem(_rootItem, QStringLiteral("caacReadWaypoints"));
    auto* uploadButton = findVisibleItem(_rootItem, QStringLiteral("caacUploadWaypoints"));
    QVERIFY(readButton && uploadButton);
    QTRY_VERIFY_WITH_TIMEOUT(readButton->isEnabled(), TestTimeout::mediumMs());
    const auto beforeRead = master->saveToJson();
    QVERIFY(clickButton(QStringLiteral("caacReadWaypoints")));
    QVERIFY(waitForDialog(QCoreApplication::translate("CaacRouteEditor", "Read waypoints")));
    QVERIFY(rejectDialog());
    QCOMPARE(master->saveToJson(), beforeRead);
    QVERIFY(clickButton(QStringLiteral("caacReadWaypoints")));
    QVERIFY(waitForDialog(QCoreApplication::translate("CaacRouteEditor", "Read waypoints")));
    QVERIFY(acceptDialog());
    QTRY_VERIFY_WITH_TIMEOUT(!master->syncInProgress() && mission->visualItems()->count() > 3, TestTimeout::mediumMs());
    QTRY_VERIFY_WITH_TIMEOUT(uploadButton->isEnabled(), TestTimeout::mediumMs());
    QVERIFY(QMetaObject::invokeMethod(_window, "showPlanView"));
    if (vehicle->flightMode() != vehicle->pauseFlightMode()) {
        QVERIFY(doubleClickAction(QStringLiteral("caacPauseMission")));
        QVERIFY(!findVisibleItem(_rootItem, QStringLiteral("guidedActionConfirmButton"), 0));
        QTRY_COMPARE_WITH_TIMEOUT(vehicle->flightMode(), vehicle->pauseFlightMode(), TestTimeout::mediumMs());
    }
    QVERIFY(QMetaObject::invokeMethod(_window, "showPlanView"));
    QSignalSpy listUpload(manager, &MissionManager::sendComplete);
    QVERIFY(listUpload.isValid());
    QVERIFY(clickButton(QStringLiteral("caacUploadWaypoints")));
    QTRY_VERIFY_WITH_TIMEOUT(!listUpload.isEmpty(), TestTimeout::mediumMs());
    QCOMPARE(listUpload.last().first().toBool(), false);
    QTRY_VERIFY_WITH_TIMEOUT(!master->syncInProgress(), TestTimeout::mediumMs());
    QVERIFY(QMetaObject::invokeMethod(_window, "showPlanView"));
    step("manual return home");
    QVERIFY(doubleClickAction(QStringLiteral("caacReturnHome")));
    QTRY_VERIFY_WITH_TIMEOUT(!vehicle->airborne() && !vehicle->armed(), TestTimeout::longMs());
    QVERIFY(QGCGeo::geodesicDistance(vehicle->coordinate(), home) < 0.1);
    step("restart");
    if (!simulation->emergencyMode()) {
        QVERIFY(doubleClickAction(QStringLiteral("caacSimulationEmergency")));
    }
    QVERIFY(clickButton(QStringLiteral("caacSimulationManagement")));
    QVERIFY(clickButton(QStringLiteral("caacEmergencyRestart")));
    QCOMPARE(plugin->flightDistance()->north(), 0.0);
    QCOMPARE(plugin->flightDistance()->east(), 0.0);
    step("manual landing");
    if (!findVisibleItem(_rootItem, QStringLiteral("caacFlightSettingsPanel"), 0)) {
        QVERIFY(clickButton(QStringLiteral("caacFlightSettingsButton")));
    }
    // Wait for the telemetry and the corresponding guided control state to agree after restarting.
    QTRY_VERIFY_WITH_TIMEOUT(findVisibleItem(_rootItem, QStringLiteral("caacTakeoff"), 0) &&
                                 findVisibleItem(_rootItem, QStringLiteral("caacTakeoff"), 0)->isEnabled(),
                             TestTimeout::mediumMs());
    QVERIFY(doubleClickAction(QStringLiteral("caacTakeoff")));
    QTRY_VERIFY_WITH_TIMEOUT(vehicle->airborne(), TestTimeout::longMs());
    if (!simulation->emergencyMode()) {
        QVERIFY(doubleClickAction(QStringLiteral("caacSimulationEmergency")));
    }
    QVERIFY(clickButton(QStringLiteral("caacSimulationManagement")));
    QTRY_VERIFY_WITH_TIMEOUT(findVisibleItem(_rootItem, QStringLiteral("caacLand"), 0) &&
                                 findVisibleItem(_rootItem, QStringLiteral("caacLand"), 0)->isEnabled(),
                             TestTimeout::mediumMs());
    QVERIFY(clickButton(QStringLiteral("caacLand")));
    QTRY_VERIFY_WITH_TIMEOUT(!vehicle->airborne() && !vehicle->armed(), TestTimeout::longMs());
    if (!simulation->emergencyMode()) {
        QVERIFY(doubleClickAction(QStringLiteral("caacSimulationEmergency")));
    }
    QVERIFY(clickButton(QStringLiteral("caacSimulationManagement")));
    QVERIFY(clickButton(QStringLiteral("caacDisconnectSimulation")));
    QVERIFY(!simulation->emergencyMode());
    QTRY_VERIFY_WITH_TIMEOUT(!MultiVehicleManager::instance()->activeVehicle(), TestTimeout::mediumMs());
    QVERIFY(!findItem(_rootItem, QStringLiteral("caacSimulationBanner")));
    if (!findVisibleItem(_rootItem, QStringLiteral("caacFlightSettingsPanel"), 0)) {
        QVERIFY(clickButton(QStringLiteral("caacFlightSettingsButton")));
    }
    QVERIFY(verifyEnabled(QStringLiteral("caacStartMission"), false, QStringLiteral("no aircraft")));
    step("other aircraft rejection");
    Vehicle* otherVehicle = nullptr;
    auto otherLink = connectMockLinkAndWaitReady([]() { return MockLink::startPX4MockLink(); }, otherVehicle);
    if (!otherLink || !otherVehicle) {
        return;
    }
    const auto disconnectOther = qScopeGuard([&]() { disconnectMockLink(otherLink); });
    otherLink->clearReceivedMavCommandCounts();
    QVERIFY(!simulation->start(master));
    QVERIFY(!simulation->active());
    QCOMPARE(otherLink->receivedMavCommandCount(MAV_CMD_MISSION_START), 0);
    QCOMPARE(otherLink->receivedMavCommandCount(MAV_CMD_COMPONENT_ARM_DISARM), 0);
    QVERIFY(!otherVehicle->armed());
}

UT_REGISTER_TEST(CaacSimulationUITest, TestLabel::Integration, TestLabel::Vehicle)
#include "CaacSimulationUITest.moc"
