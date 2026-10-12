#include <cstdio>

#include <QtCore/QLineF>
#include <QtCore/QRegularExpression>
#include <QtCore/QScopeGuard>
#include <QtGui/QGuiApplication>
#include <QtQuick/QQuickItem>
#include <QtQuick/QQuickWindow>
#include <QtTest/QSignalSpy>

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
#include "QmlUITestBase.h"
#include "SimpleMissionItem.h"
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
    QCOMPARE(QGuiApplication::applicationDisplayName(), QStringLiteral("飞虎无人机地面站系统"));
    QCOMPARE(_window->title(), QStringLiteral("飞虎无人机地面站系统"));
    QVERIFY(verifyText(QStringLiteral("caacDashboardHeadingValue"),
                       QCoreApplication::translate("TelemetryValuesBar", "Unavailable"), QStringLiteral("no heading")));
    QVERIFY(clickToolSelectDropdownButton(QStringLiteral("toolbar_viewPlan")));
    auto* plan = findVisibleItem(_rootItem, QStringLiteral("mainView_plan"));
    QVERIFY(plan);
    auto* master = plan->property("_planMasterController").value<PlanMasterController*>();
    QVERIFY(master);
    auto* mission = master->missionController();
    const QGeoCoordinate home(30, 120, 20);
    mission->setHomePosition(home);
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
    auto* plugin = qobject_cast<CaacCorePlugin*>(QGCCorePlugin::instance());
    QVERIFY(plugin);
    auto* simulation = plugin->simulation();
    QVERIFY(simulation);
    const auto stop = qScopeGuard([simulation]() { simulation->stop(); });
    ignoreLogMessage("Utilities.QGCSensors", QtWarningMsg,
                     QRegularExpression(QStringLiteral("Failed to connect to pressure backend")));
    ignoreLogMessage("Utilities.QGCSensors", QtWarningMsg,
                     QRegularExpression(QStringLiteral("Error Initializing Pressure Sensor")));
    QVERIFY(
        clickButtonScrolled(QStringLiteral("planToolStrip_simulationButton"), QStringLiteral("toolStrip_flickable")));
    step("upload simulation");
    QVERIFY(clickButton(QStringLiteral("caacUploadSimulation")));
    QTRY_VERIFY2_WITH_TIMEOUT(simulation->vehicle() && simulation->vehicle()->airborne(),
                              qPrintable(simulation->status()), TestTimeout::longMs());
    step("airborne");
    auto* vehicle = simulation->vehicle();
    QVERIFY(vehicle);
    QCOMPARE(MultiVehicleManager::instance()->vehicles()->count(), 1);
    QCOMPARE(vehicle->flightMode(), vehicle->missionFlightMode());
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
    QVERIFY(clickButton(QStringLiteral("caacViewSimulation")));
    QVERIFY(verifyVisibility(QStringLiteral("caacSimulationBanner"), true, QStringLiteral("simulation identity")));
    QTRY_VERIFY_WITH_TIMEOUT(vehicle->groundSpeed()->rawValue().toDouble() > 0, TestTimeout::mediumMs());
    auto* heading = findVisibleItem(_rootItem, QStringLiteral("caacDashboardHeadingValue"));
    QVERIFY(heading);
    QTRY_COMPARE_WITH_TIMEOUT(heading->property("text").toString(),
                              vehicle->heading()->cookedValueString() + " " + vehicle->heading()->cookedUnits(),
                              TestTimeout::mediumMs());
    const auto link = vehicle->vehicleLinkManager()->primaryLink().lock();
    auto* mock = qobject_cast<MockLink*>(link.get());
    QVERIFY(mock);
    step("pause");
    mock->clearReceivedMavCommandCounts();
    QVERIFY(clickButtonScrolled(QStringLiteral("caacPauseMission"), QStringLiteral("flyViewToolStrip_flickable")));
    auto* confirmation =
        findVisibleItem(_rootItem, QStringLiteral("guidedActionConfirmButton"), TestTimeout::mediumMs());
    QVERIFY(confirmation);
    QVERIFY(QMetaObject::invokeMethod(confirmation, "activated"));
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
    step("height edit");
    auto* target = chooseLastWaypoint();
    QVERIFY(target);
    const auto targetSequence = target->sequenceNumber();
    const auto previousAltitude = target->altitude()->rawValue();
    QVERIFY(!plugin->updateFlightWaypointAltitude(flyMaster, target, qQNaN()).isEmpty());
    QCOMPARE(target->altitude()->rawValue(), previousAltitude);
    QVERIFY(clickButtonScrolled(QStringLiteral("caacModifyWaypointAltitude"),
                                QStringLiteral("flyViewToolStrip_flickable")));
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
    QVERIFY(clickButtonScrolled(QStringLiteral("caacJumpWaypoint"), QStringLiteral("flyViewToolStrip_flickable")));
    choices = findVisibleItem(_rootItem, QStringLiteral("caacFlightWaypointChoice"));
    QVERIFY(choices);
    QVERIFY(choices->setProperty("currentIndex", choices->property("count").toInt() - 1));
    QVERIFY(acceptDialog());
    confirmation = findVisibleItem(_rootItem, QStringLiteral("guidedActionConfirmButton"), TestTimeout::mediumMs());
    QVERIFY(confirmation);
    QVERIFY(QMetaObject::invokeMethod(confirmation, "activated"));
    QTRY_COMPARE_WITH_TIMEOUT(manager->currentIndex(), targetSequence - 1, TestTimeout::mediumMs());
    step("continue");
    QVERIFY(clickButtonScrolled(QStringLiteral("caacStartMission"), QStringLiteral("flyViewToolStrip_flickable")));
    confirmation = findVisibleItem(_rootItem, QStringLiteral("guidedActionConfirmButton"), TestTimeout::mediumMs());
    QVERIFY(confirmation);
    QVERIFY(QMetaObject::invokeMethod(confirmation, "activated"));
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
    confirmation = findVisibleItem(_rootItem, QStringLiteral("guidedActionConfirmButton"), TestTimeout::mediumMs());
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
        QVERIFY(clickButtonScrolled(QStringLiteral("caacPauseMission"), QStringLiteral("flyViewToolStrip_flickable")));
        confirmation = findVisibleItem(_rootItem, QStringLiteral("guidedActionConfirmButton"));
        QVERIFY(confirmation);
        QVERIFY(QMetaObject::invokeMethod(confirmation, "activated"));
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
    step("restart");
    QVERIFY(simulation->restart());
    QCOMPARE(plugin->flightDistance()->north(), 0.0);
    QCOMPARE(plugin->flightDistance()->east(), 0.0);
    simulation->stop();
    QTRY_VERIFY_WITH_TIMEOUT(!MultiVehicleManager::instance()->activeVehicle(), TestTimeout::mediumMs());
    QVERIFY(verifyVisibility(QStringLiteral("caacSimulationBanner"), false, QStringLiteral("disconnected")));
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
