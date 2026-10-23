#include <QtCore/QPointer>
#include <QtCore/QScopeGuard>
#include <QtQuick/QQuickItem>
#include <QtQuick/QQuickWindow>
#include <QtTest/QSignalSpy>

#include "CaacCorePlugin.h"
#include "CaacRouteEditorController.h"
#include "CaacSimulationController.h"
#include "Fact.h"
#include "MissionController.h"
#include "MissionSettingsItem.h"
#include "PlanMasterController.h"
#include "QGCGeo.h"
#include "QmlUITestBase.h"
#include "SimpleMissionItem.h"
#include "SpeedSection.h"
#include "Vehicle.h"

class CaacLegSpeedUITest : public QmlUITestBase
{
    Q_OBJECT

private slots:
    void _homeLegSpeed();
    void _runtimeSpeedRefresh();
};

void CaacLegSpeedUITest::_homeLegSpeed()
{
    startUI();
    if (QTest::currentTestFailed()) {
        return;
    }
    QVERIFY(QMetaObject::invokeMethod(_window, "showPlanView"));
    auto* plan = findVisibleItem(_rootItem, QStringLiteral("mainView_plan"));
    auto* panel = findVisibleItem(_rootItem, QStringLiteral("caacRouteEditor"));
    auto* map = findVisibleItem(_rootItem, QStringLiteral("planView_map"));
    QVERIFY(plan && panel && map);
    auto* master = plan->property("_planMasterController").value<PlanMasterController*>();
    auto* editor = panel->property("routeController").value<CaacRouteEditorController*>();
    QVERIFY(master && editor);
    master->removeAll();
    auto* mission = master->missionController();
    const QGeoCoordinate home(30, 120, 20);
    mission->setHomePosition(home);
    QVERIFY(editor->addWaypoint(QGCGeo::geodesicDestination(home, 90, 100)));
    QVERIFY(editor->addWaypoint(QGCGeo::geodesicDestination(home, 90, 200)));
    QVERIFY(editor->setMissionFlightSpeed(true, 6));
    panel->setProperty("activePanel", -1);
    map->setProperty("center", QVariant::fromValue(QGCGeo::geodesicDestination(home, 90, 100)));
    map->setProperty("zoomLevel", 17.0);
    QPointer<QQuickItem> homeLabel;
    QTRY_VERIFY_WITH_TIMEOUT((homeLabel = findVisibleItem(map, QStringLiteral("caacRouteLegLabel_0"), 0)),
                             TestTimeout::mediumMs());
    QTRY_VERIFY_WITH_TIMEOUT(homeLabel->property("legText").toString().endsWith(QStringLiteral("6.0 m/s")),
                             TestTimeout::mediumMs());
    QVERIFY(editor->setMissionFlightSpeed(true, 7));
    QTRY_VERIFY_WITH_TIMEOUT(homeLabel->property("legText").toString().endsWith(QStringLiteral("7.0 m/s")),
                             TestTimeout::mediumMs());
    auto* first = mission->visualItems()->value<SimpleMissionItem*>(1);
    QVERIFY(first && first->speedSection());
    first->speedSection()->flightSpeed()->setRawValue(9);
    first->speedSection()->setSpecifyFlightSpeed(true);
    QPointer<QQuickItem> nextLabel;
    QTRY_VERIFY_WITH_TIMEOUT((nextLabel = findVisibleItem(map, QStringLiteral("caacRouteLegLabel_1"), 0)),
                             TestTimeout::mediumMs());
    QTRY_VERIFY_WITH_TIMEOUT(nextLabel->property("legText").toString().endsWith(QStringLiteral("9.0 m/s")),
                             TestTimeout::mediumMs());
    QTRY_VERIFY_WITH_TIMEOUT((homeLabel = findVisibleItem(map, QStringLiteral("caacRouteLegLabel_0"), 0)) &&
                                 homeLabel->property("legText").toString().endsWith(QStringLiteral("7.0 m/s")),
                             TestTimeout::mediumMs());
}

void CaacLegSpeedUITest::_runtimeSpeedRefresh()
{
    startUI();
    if (QTest::currentTestFailed()) {
        return;
    }
    QVERIFY(QMetaObject::invokeMethod(_window, "showPlanView"));
    auto* plan = findVisibleItem(_rootItem, QStringLiteral("mainView_plan"));
    auto* panel = findVisibleItem(_rootItem, QStringLiteral("caacRouteEditor"));
    auto* map = findVisibleItem(_rootItem, QStringLiteral("planView_map"));
    QVERIFY(plan && panel && map);
    auto* master = plan->property("_planMasterController").value<PlanMasterController*>();
    auto* editor = panel->property("routeController").value<CaacRouteEditorController*>();
    auto* plugin = qobject_cast<CaacCorePlugin*>(QGCCorePlugin::instance());
    QVERIFY(master && editor && plugin);
    master->removeAll();
    auto* mission = master->missionController();
    const QGeoCoordinate home(30, 120, 20);
    const auto firstCoordinate = QGCGeo::geodesicDestination(home, 90, 200);
    const auto secondCoordinate = QGCGeo::geodesicDestination(firstCoordinate, 0, 100);
    mission->setHomePosition(home);
    QVERIFY(editor->addWaypoint(firstCoordinate));
    QVERIFY(editor->addWaypoint(secondCoordinate));
    QVERIFY(editor->setMissionFlightSpeed(true, 5));
    auto* first = mission->visualItems()->value<SimpleMissionItem*>(1);
    auto* second = mission->visualItems()->value<SimpleMissionItem*>(2);
    QVERIFY(first && second && first->speedSection());
    first->altitude()->setRawValue(2);
    second->altitude()->setRawValue(2);
    first->speedSection()->flightSpeed()->setRawValue(7);
    first->speedSection()->setSpecifyFlightSpeed(true);
    auto settings = CaacSimulationController::defaults();
    settings["takeoffHeight"] = 2.0;
    settings["verticalSpeed"] = 10.0;
    settings["yawRate"] = 360.0;
    settings["endBehavior"] = 2;
    QVERIFY(editor->setSimulationSettings(settings));
    panel->setProperty("activePanel", -1);
    map->setProperty("center", QVariant::fromValue(QGCGeo::geodesicDestination(home, 90, 100)));
    map->setProperty("zoomLevel", 17.0);
    auto* simulation = plugin->simulation();
    QVERIFY(simulation);
    const auto stop = qScopeGuard([simulation]() { simulation->stop(); });
    QVERIFY(simulation->start(master));
    QTRY_VERIFY_WITH_TIMEOUT(simulation->vehicle() && simulation->vehicle()->isInitialConnectComplete(),
                             TestTimeout::longMs());
    QSignalSpy uploaded(simulation, &CaacSimulationController::missionUploaded);
    QVERIFY(simulation->upload(master));
    QTRY_VERIFY_WITH_TIMEOUT(!uploaded.isEmpty(), TestTimeout::longMs());
    const auto original = master->saveToJson();
    QPointer<QQuickItem> homeLabel;
    QPointer<QQuickItem> nextLabel;
    QTRY_VERIFY_WITH_TIMEOUT((homeLabel = findVisibleItem(map, QStringLiteral("caacRouteLegLabel_0"), 0)),
                             TestTimeout::mediumMs());
    QTRY_VERIFY_WITH_TIMEOUT((nextLabel = findVisibleItem(map, QStringLiteral("caacRouteLegLabel_1"), 0)),
                             TestTimeout::mediumMs());
    QTRY_VERIFY_WITH_TIMEOUT(homeLabel->property("legText").toString().endsWith(QStringLiteral("5.0 m/s")),
                             TestTimeout::mediumMs());
    QTRY_VERIFY_WITH_TIMEOUT(nextLabel->property("legText").toString().endsWith(QStringLiteral("7.0 m/s")),
                             TestTimeout::mediumMs());
    auto* vehicle = simulation->vehicle();
    vehicle->startMission();
    QTRY_VERIFY_WITH_TIMEOUT(vehicle->airborne() && vehicle->groundSpeed()->rawValue().toDouble() > 1,
                             TestTimeout::longMs());
    for (double speed : {6.0, 8.0}) {
        QSignalSpy acknowledged(vehicle, &Vehicle::mavCommandResult);
        vehicle->guidedModeChangeGroundSpeedMetersSecond(speed);
        const auto accepted = [&acknowledged]() {
            for (const auto& result : acknowledged) {
                if (result[2].toInt() == MAV_CMD_DO_CHANGE_SPEED && result[3].toInt() == MAV_RESULT_ACCEPTED) {
                    return true;
                }
            }
            return false;
        };
        QTRY_VERIFY_WITH_TIMEOUT(accepted(), TestTimeout::mediumMs());
        QTRY_COMPARE_WITH_TIMEOUT(vehicle->commandedFlightSpeed(), speed, TestTimeout::mediumMs());
        QTRY_COMPARE_WITH_TIMEOUT(simulation->currentMissionDetails().value("speed").toDouble(), speed,
                                  TestTimeout::mediumMs());
        const auto suffix = QStringLiteral("%1 m/s").arg(speed, 0, 'f', 1);
        QTRY_VERIFY_WITH_TIMEOUT(homeLabel && homeLabel->property("legText").toString().endsWith(suffix),
                                 TestTimeout::mediumMs());
        QVERIFY(nextLabel && nextLabel->property("legText").toString().endsWith(QStringLiteral("7.0 m/s")));
        QCOMPARE(master->saveToJson(), original);
    }
    QTRY_COMPARE_WITH_TIMEOUT(simulation->currentMissionDetails().value("speed").toDouble(), 7.0,
                              TestTimeout::longMs());
    QTRY_VERIFY_WITH_TIMEOUT(homeLabel && homeLabel->property("legText").toString().endsWith(QStringLiteral("5.0 m/s")),
                             TestTimeout::mediumMs());
    QTRY_VERIFY_WITH_TIMEOUT(nextLabel && nextLabel->property("legText").toString().endsWith(QStringLiteral("7.0 m/s")),
                             TestTimeout::mediumMs());
    QCOMPARE(master->saveToJson(), original);
}

UT_REGISTER_TEST(CaacLegSpeedUITest, TestLabel::Integration, TestLabel::MissionManager)

#include "CaacLegSpeedUITest.moc"
