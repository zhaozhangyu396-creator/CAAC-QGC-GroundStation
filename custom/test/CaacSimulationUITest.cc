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
#include "CaacSimulationEngine.h"
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
    void _closedRouteAndSimulationToggle();
};

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
    QVERIFY(clickToolSelectDropdownButton(QStringLiteral("toolbar_viewPlan")));
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
    const auto clickSimulation = [&]() {
        return clickButtonScrolled(QStringLiteral("planToolStrip_simulationButton"),
                                   QStringLiteral("toolStrip_flickable"));
    };
    QVERIFY(clickSimulation());
    step("connection requested");
    QTRY_VERIFY_WITH_TIMEOUT(simulation->vehicle() && simulation->vehicle()->isInitialConnectComplete(),
                             TestTimeout::longMs());
    QVERIFY(verifyText(QStringLiteral("planToolStrip_simulationButton"),
                       QCoreApplication::translate("PlanView", "Cancel simulation"),
                       QStringLiteral("active simulation")));
    QVERIFY(clickSimulation());
    step("cancel requested");
    QTRY_VERIFY_WITH_TIMEOUT(!simulation->active() && !MultiVehicleManager::instance()->activeVehicle(),
                             TestTimeout::mediumMs());
    QVERIFY(verifyText(QStringLiteral("planToolStrip_simulationButton"),
                       QCoreApplication::translate("PlanView", "Simulation"),
                       QStringLiteral("simulation disconnected")));
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
    QTRY_VERIFY_WITH_TIMEOUT(!initialUpload.isEmpty(), TestTimeout::longMs());
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
    QTRY_COMPARE_WITH_TIMEOUT(toolbar->property("_roundColumns").toInt(), 7, TestTimeout::mediumMs());
    auto* rightTool = findVisibleItem(_rootItem, QStringLiteral("caacFlightDataButton"));
    QVERIFY(rightTool);
    auto* host = findVisibleItem(_rootItem, QStringLiteral("caacTopFlightActions"));
    QVERIFY(host);
    const QStringList actionOrder{QStringLiteral("caacTakeoff"),
                                  QStringLiteral("caacReturnHome"),
                                  QStringLiteral("caacStartMission"),
                                  QStringLiteral("caacPauseMission"),
                                  QStringLiteral("caacJumpWaypoint"),
                                  QStringLiteral("caacModifyWaypointAltitude"),
                                  QStringLiteral("caacSimulationEmergency")};
    auto* firstAction = findVisibleItem(_rootItem, actionOrder.first());
    QVERIFY(firstAction);
    const auto firstPosition = firstAction->mapToScene(QPointF(0, 0));
    double previousX = -1;
    for (const auto& name : actionOrder) {
        auto* action = findVisibleItem(_rootItem, name);
        QVERIFY(action);
        const auto position = action->mapToScene(QPointF(0, 0));
        QVERIFY(position.x() > previousX);
        QVERIFY(std::abs(position.y() - firstPosition.y()) < 1);
        QVERIFY(std::abs(action->property("roundDiameter").toDouble() -
                         firstAction->property("roundDiameter").toDouble()) < 1);
        QVERIFY(position.y() + action->height() <= host->mapToScene(QPointF(0, host->height())).y() + 1);
        previousX = position.x();
    }
    QVERIFY(firstPosition.y() < rightTool->mapToScene(QPointF(0, 0)).y());
    auto* modeIndicator = findVisibleItem(_rootItem, QStringLiteral("toolbar_flightModeIndicator"));
    auto* indicators = findVisibleItem(_rootItem, QStringLiteral("flyViewToolBarIndicators"));
    QVERIFY(modeIndicator && indicators);
    QVERIFY(firstPosition.x() >= modeIndicator->mapToScene(QPointF(modeIndicator->width(), 0)).x());
    auto* header = host->parentItem();
    QVERIFY(header);
    QCOMPARE(header->height(), header->property("_statusHeight").toDouble());
    QVERIFY(host->mapToScene(QPointF(0, host->height())).y() <=
            header->mapToScene(QPointF(0, header->height())).y() + 1);
    QVERIFY(host->mapToScene(QPointF(host->width(), 0)).x() <= indicators->mapToScene(QPointF(0, 0)).x() + 1);
    const auto originalWindowSize = _window->size();
    for (const int requestedWidth : {800, 1600}) {
        _window->resize(std::max(_window->minimumWidth(), requestedWidth), originalWindowSize.height());
        QTRY_VERIFY_WITH_TIMEOUT(flightActions->property("contentWidth").toDouble() <= flightActions->width(),
                                 TestTimeout::mediumMs());
        QCOMPARE(header->height(), header->property("_statusHeight").toDouble());
        QVERIFY(host->mapToScene(QPointF(host->width(), 0)).x() <= indicators->mapToScene(QPointF(0, 0)).x() + 1);
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
    QCOMPARE(loopTarget, firstWaypointSequence + 1);
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
    for (int i = 1; i < flyMission->visualItems()->count(); ++i) {
        auto* item = flyMission->visualItems()->value<SimpleMissionItem*>(i);
        if (item && item->command() == MAV_CMD_NAV_WAYPOINT) {
            const int expected =
                QGCGeo::geodesicDistance(item->coordinate(),
                                         mission->visualItems()->value<SimpleMissionItem*>(1)->coordinate()) < 0.02
                    ? 1
                    : 2;
            QCOMPARE(simulation->waypointNumber(item->sequenceNumber(), item->coordinate()), expected);
            auto* mapLabel = findItem(qobject_cast<QQuickItem*>(map),
                                      QStringLiteral("missionItemMapLabel_%1").arg(item->sequenceNumber()));
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
    QVERIFY(clickToolSelectDropdownButton(QStringLiteral("toolbar_viewPlan")));
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
    QTRY_VERIFY_WITH_TIMEOUT(!listUpload.isEmpty(), TestTimeout::mediumMs());
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
