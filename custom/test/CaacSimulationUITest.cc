#include <cmath>
#include <cstdio>
#include <utility>

#include <QtCore/QLineF>
#include <QtCore/QRegularExpression>
#include <QtCore/QScopeGuard>
#include <QtGui/QGuiApplication>
#include <QtQuick/QQuickItem>
#include <QtQuick/QQuickWindow>
#include <QtTest/QSignalSpy>

#include "AppSettings.h"
#include "BatteryFactGroupListModel.h"
#include "CaacCorePlugin.h"
#include "CaacFlightDistance.h"
#include "CaacRouteEditorController.h"
#include "CaacSimulationController.h"
#include "MissionController.h"
#include "MissionManager.h"
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
    void _uploadAndFlightControls();
};

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
    QVERIFY(verifyText(QStringLiteral("caacDashboardHeadingValue"),
                       QCoreApplication::translate("TelemetryValuesBar", "Unavailable"), QStringLiteral("no heading")));
    QVERIFY(verifyText(QStringLiteral("caacAttitudeHeading"),
                       QCoreApplication::translate("CaacAttitudeInstrument", "Not available"),
                       QStringLiteral("no attitude heading")));
    QVERIFY(clickToolSelectDropdownButton(QStringLiteral("toolbar_viewPlan")));
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
    auto* simulationButton = findVisibleItem(_rootItem, QStringLiteral("planToolStrip_simulationButton"));
    QVERIFY(simulationButton && simulationButton->isEnabled());
    QVERIFY(
        clickButtonScrolled(QStringLiteral("planToolStrip_simulationButton"), QStringLiteral("toolStrip_flickable")));
    step("connection requested");
    QTRY_VERIFY_WITH_TIMEOUT(simulation->vehicle() && simulation->vehicle()->isInitialConnectComplete(),
                             TestTimeout::longMs());
    QVERIFY(!simulation->vehicle()->armed());
    QVERIFY(!simulation->vehicle()->airborne());
    QVERIFY(!simulation->missionReady());
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
    QVERIFY(initialUpload.wait(TestTimeout::longMs()));
    QVERIFY(simulation->missionReady());
    QVERIFY(!findItem(_rootItem, QStringLiteral("caacSimulationBanner")));
    QVERIFY(!simulation->vehicle()->armed());
    QVERIFY(!simulation->vehicle()->airborne());
    auto* flightActions = findVisibleItem(_rootItem, QStringLiteral("flyViewToolStrip_flickable"));
    QVERIFY(flightActions);
    QVERIFY(!flightActions->property("interactive").toBool());
    QTRY_VERIFY_WITH_TIMEOUT(flightActions->property("contentWidth").toDouble() <= flightActions->width() + 1,
                             TestTimeout::mediumMs());
    auto* toolbar = findVisibleItem(_rootItem, QStringLiteral("flyViewToolStrip"));
    QVERIFY(toolbar);
    const auto originalWidth = toolbar->property("maxWidth");
    QTRY_COMPARE_WITH_TIMEOUT(toolbar->property("_roundColumns").toInt(), 4, TestTimeout::mediumMs());
    auto* rightTool = findVisibleItem(_rootItem, QStringLiteral("caacFlightDataButton"));
    QVERIFY(rightTool);
    QTRY_VERIFY_WITH_TIMEOUT(std::abs(rightTool->width() - toolbar->property("roundButtonDiameter").toDouble()) < 1,
                             TestTimeout::mediumMs());
    auto* firstAction = findVisibleItem(_rootItem, QStringLiteral("caacStartMission"));
    auto* secondRowAction = findVisibleItem(_rootItem, QStringLiteral("caacJumpWaypoint"));
    QVERIFY(firstAction && secondRowAction);
    auto* lastRightTool = findVisibleItem(_rootItem, QStringLiteral("caacCenterVehicleButton"));
    QVERIFY(lastRightTool);
    QTRY_VERIFY_WITH_TIMEOUT(
        std::abs(firstAction->mapToScene(QPointF(firstAction->width() / 2, 0)).x() +
                 lastRightTool->mapToScene(QPointF(lastRightTool->width() / 2, 0)).x() - _window->width()) < 2,
        TestTimeout::mediumMs());
    QTRY_VERIFY_WITH_TIMEOUT(
        std::abs(rightTool->mapToScene(QPointF(0, 0)).y() - firstAction->mapToScene(QPointF(0, 0)).y() -
                 firstAction->property("contentMargins").toDouble()) < 1,
        TestTimeout::mediumMs());
    QVERIFY(secondRowAction->mapToScene(QPointF(0, 0)).y() >
            firstAction->mapToScene(QPointF(0, 0)).y() + firstAction->height() * 0.8);
    QVERIFY(toolbar->setProperty("maxWidth", 400.0));
    QTRY_COMPARE_WITH_TIMEOUT(toolbar->property("_roundColumns").toInt(), 4, TestTimeout::mediumMs());
    QTRY_VERIFY_WITH_TIMEOUT(flightActions->property("contentWidth").toDouble() <= flightActions->width() + 1,
                             TestTimeout::mediumMs());
    QVERIFY(toolbar->setProperty("maxWidth", originalWidth));
    const auto doubleClickAction = [&](const QString& name) {
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
    QCOMPARE(MultiVehicleManager::instance()->vehicles()->count(), 1);
    auto* attitude = findVisibleItem(_rootItem, QStringLiteral("caacAttitudeInstrument"));
    QVERIFY(attitude);
    QTRY_VERIFY_WITH_TIMEOUT(attitude->property("attitudeAvailable").toBool(), TestTimeout::mediumMs());
    QCOMPARE(attitude->property("rollAngle").toDouble(), vehicle->roll()->rawValue().toDouble());
    QCOMPARE(attitude->property("pitchAngle").toDouble(), vehicle->pitch()->rawValue().toDouble());
    QCOMPARE(attitude->property("headingAngle").toDouble(), vehicle->heading()->rawValue().toDouble());
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
    QCOMPARE(loopTarget, firstWaypointSequence);
    QCOMPARE(master->saveToJson(), original);

    QVERIFY(!findItem(_rootItem, QStringLiteral("caacSimulationBanner")));
    QTRY_VERIFY_WITH_TIMEOUT(vehicle->groundSpeed()->rawValue().toDouble() > 0, TestTimeout::mediumMs());
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
    const QRectF emergencyRect(emergencyAttitude->mapToScene(QPointF(0, 0)),
                               QSizeF(emergencyAttitude->width(), emergencyAttitude->height()));
    const QRectF toolbarRect(toolbar->mapToScene(QPointF(0, 0)), QSizeF(toolbar->width(), toolbar->height()));
    QVERIFY(!emergencyRect.intersects(toolbarRect));
    QVERIFY(!findVisibleItem(_rootItem, QStringLiteral("flyViewMap"), 0));
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
    QVERIFY(findVisibleItem(_rootItem, QStringLiteral("flyViewMap")));
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
    auto* layer = findItem(_rootItem, QStringLiteral("caacFlyLayer"));
    QVERIFY(layer);
    auto* map = layer->property("mapControl").value<QObject*>();
    QVERIFY(map);
    auto* flyMaster = map->property("planMasterController").value<PlanMasterController*>();
    QVERIFY(flyMaster);
    auto* flyMission = flyMaster->missionController();
    const auto chooseLastWaypoint = [&]() -> SimpleMissionItem* {
        for (int i = flyMission->visualItems()->count() - 1; i > 0; --i) {
            auto* item = flyMission->visualItems()->value<SimpleMissionItem*>(i);
            if (item && item->command() == MAV_CMD_NAV_WAYPOINT) {
                return item;
            }
        }
        return nullptr;
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
    QVERIFY(choices->setProperty("currentIndex", choices->property("count").toInt() - 1));
    auto* altitude = findVisibleItem(_rootItem, QStringLiteral("caacFlightWaypointAltitude"));
    QVERIFY(altitude);
    QVERIFY(altitude->setProperty("text", "3"));
    QSignalSpy upload(manager, &MissionManager::sendComplete);
    QVERIFY(upload.isValid());
    QVERIFY(acceptDialog());
    QVERIFY(upload.wait(TestTimeout::mediumMs()));
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
        if (item->command() == MAV_CMD_NAV_WAYPOINT) {
            QCOMPARE(item->param7(), 100.0);
            ++updatedWaypoints;
        }
    }
    QVERIFY(updatedWaypoints >= 2);
    QTRY_VERIFY_WITH_TIMEOUT(vehicle->altitudeRelative()->rawValue().toDouble() > heightBeforeAll + 0.1,
                             TestTimeout::longMs());
    QVERIFY(vehicle->batteries()->count() > 0);
    for (int i = 0; i < vehicle->batteries()->count(); ++i) {
        auto* battery = vehicle->batteries()->value<BatteryFactGroup*>(i);
        QVERIFY(battery);
        QCOMPARE(battery->percentRemaining()->rawValue().toDouble(), 100.0);
        QCOMPARE(battery->chargeState()->rawValue().toInt(), static_cast<int>(MAV_BATTERY_CHARGE_STATE_OK));
    }
    step("right-click guided point");
    QVERIFY(clickToolSelectDropdownButton(QStringLiteral("toolbar_viewPlan")));
    auto* planMap = findVisibleItem(_rootItem, QStringLiteral("planView_map"));
    QVERIFY(planMap);
    auto* localWaypoint = mission->visualItems()->value<SimpleMissionItem*>(1);
    QVERIFY(localWaypoint);
    const auto gotoPoint = QGCGeo::geodesicDestination(vehicle->coordinate(), 90, 2);
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
    QTRY_VERIFY_WITH_TIMEOUT(QGCGeo::geodesicDistance(vehicle->coordinate(), gotoPoint) < 0.1, TestTimeout::mediumMs());
    QVERIFY(mock->receivedMavCommandCount(MAV_CMD_DO_REPOSITION) > 0);
    QTRY_COMPARE_WITH_TIMEOUT(vehicle->flightMode(), vehicle->pauseFlightMode(), TestTimeout::mediumMs());

    step("waypoint list read and upload");
    QVERIFY(clickToolSelectDropdownButton(QStringLiteral("toolbar_viewPlan")));
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
    QVERIFY(clickToolSelectDropdownButton(QStringLiteral("toolbar_viewFly")));
    if (vehicle->flightMode() != vehicle->pauseFlightMode()) {
        QVERIFY(doubleClickAction(QStringLiteral("caacPauseMission")));
        QVERIFY(!findVisibleItem(_rootItem, QStringLiteral("guidedActionConfirmButton"), 0));
        QTRY_COMPARE_WITH_TIMEOUT(vehicle->flightMode(), vehicle->pauseFlightMode(), TestTimeout::mediumMs());
    }
    QVERIFY(clickToolSelectDropdownButton(QStringLiteral("toolbar_viewPlan")));
    QSignalSpy listUpload(manager, &MissionManager::sendComplete);
    QVERIFY(listUpload.isValid());
    QVERIFY(clickButton(QStringLiteral("caacUploadWaypoints")));
    QVERIFY(listUpload.wait(TestTimeout::mediumMs()));
    QCOMPARE(listUpload.last().first().toBool(), false);
    QTRY_VERIFY_WITH_TIMEOUT(!master->syncInProgress(), TestTimeout::mediumMs());
    QVERIFY(clickToolSelectDropdownButton(QStringLiteral("toolbar_viewFly")));
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
