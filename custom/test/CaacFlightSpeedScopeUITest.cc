#include <QtCore/QPointer>
#include <QtCore/QRegularExpression>
#include <QtCore/QScopeGuard>
#include <QtQml/QQmlEngine>
#include <QtQml/QQmlExpression>
#include <QtQuick/QQuickItem>
#include <QtQuick/QQuickWindow>
#include <QtTest/QSignalSpy>

#include "CaacCorePlugin.h"
#include "CaacMissionUpdateController.h"
#include "CaacRouteEditorController.h"
#include "CaacSimulationController.h"
#include "Fact.h"
#include "MissionController.h"
#include "MissionItem.h"
#include "MissionManager.h"
#include "PlanMasterController.h"
#include "QGCGeo.h"
#include "QmlUITestBase.h"
#include "SimpleMissionItem.h"
#include "SpeedSection.h"
#include "Vehicle.h"

class CaacFlightSpeedScopeUITest : public QmlUITestBase
{
    Q_OBJECT

private slots:
    void _scopedUpdatesPreserveFlightProgress();
    void _selectedDepartureIncludesCurrentLeg();

private:
    bool _openSpeedDialog();
};

bool CaacFlightSpeedScopeUITest::_openSpeedDialog()
{
    if (!findVisibleItem(_rootItem, QStringLiteral("caacFlightSettingsPanel"), 0) &&
        !clickButton(QStringLiteral("caacFlightSettingsButton"))) {
        return false;
    }
    auto* button =
        findVisibleItemScrolled(QStringLiteral("caacChangeFlightSpeed"), QStringLiteral("flyViewToolStrip_flickable"));
    if (!button || !button->isEnabled()) {
        return false;
    }
    QTest::mouseDClick(_window, Qt::LeftButton, Qt::NoModifier,
                       button->mapToScene(QPointF(button->width() / 2, button->height() / 2)).toPoint());
    return true;
}

void CaacFlightSpeedScopeUITest::_scopedUpdatesPreserveFlightProgress()
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
    const auto firstCoordinate = QGCGeo::geodesicDestination(home, 90, 600);
    mission->setHomePosition(home);
    for (const auto& coordinate : {firstCoordinate, QGCGeo::geodesicDestination(firstCoordinate, 0, 100),
                                   QGCGeo::geodesicDestination(firstCoordinate, 0, 200)}) {
        QVERIFY(editor->addWaypoint(coordinate));
        auto* item = editor->currentItem();
        QVERIFY(item && item->speedSection());
        item->altitude()->setRawValue(2);
        item->speedSection()->flightSpeed()->setRawValue(5);
        item->speedSection()->setSpecifyFlightSpeed(true);
    }
    QVERIFY(editor->setMissionFlightSpeed(true, 5));
    auto* first = mission->visualItems()->value<SimpleMissionItem*>(1);
    auto* second = mission->visualItems()->value<SimpleMissionItem*>(2);
    auto* third = mission->visualItems()->value<SimpleMissionItem*>(3);
    QVERIFY(first && second && third);
    auto settings = CaacSimulationController::defaults();
    settings["takeoffHeight"] = 2.0;
    settings["verticalSpeed"] = 10.0;
    settings["yawRate"] = 360.0;
    settings["endBehavior"] = 2;
    QVERIFY(editor->setSimulationSettings(settings));
    panel->setProperty("activePanel", -1);
    map->setProperty("center", QVariant::fromValue(QGCGeo::geodesicDestination(home, 90, 300)));
    map->setProperty("zoomLevel", 16.0);

    auto* simulation = plugin->simulation();
    QVERIFY(simulation);
    const auto stop = qScopeGuard([simulation]() { simulation->stop(); });
    // Optional host pressure backends are unavailable on some desktop runners.
    ignoreLogMessage("Utilities.QGCSensors", QtWarningMsg,
                     QRegularExpression(QStringLiteral("Failed to connect to pressure backend$")));
    ignoreLogMessage("Utilities.QGCSensors", QtWarningMsg,
                     QRegularExpression(QStringLiteral("Error Initializing Pressure Sensor$")));
    QVERIFY(simulation->start(master));
    QTRY_VERIFY_WITH_TIMEOUT(simulation->vehicle() && simulation->vehicle()->isInitialConnectComplete(),
                             TestTimeout::longMs());
    QSignalSpy uploads(simulation, &CaacSimulationController::missionUploaded);
    QVERIFY(simulation->upload(master));
    QTRY_COMPARE_WITH_TIMEOUT(uploads.count(), 1, TestTimeout::longMs());
    auto* vehicle = simulation->vehicle();
    auto* manager = vehicle->missionManager();
    vehicle->startMission();
    QTRY_VERIFY_WITH_TIMEOUT(vehicle->airborne() && home.distanceTo(vehicle->coordinate()) > 35, TestTimeout::longMs());
    const auto targetCoordinate = [&]() {
        const int index = manager->currentIndex();
        const auto& items = manager->missionItems();
        return index >= 0 && index < items.size() ? items.at(index)->coordinate() : QGeoCoordinate();
    };
    const auto targetingFirst = [&]() {
        const auto target = targetCoordinate();
        return target.isValid() && target.distanceTo(firstCoordinate) < 0.1;
    };
    QTRY_VERIFY_WITH_TIMEOUT(targetingFirst(), TestTimeout::mediumMs());
    const auto original = master->saveToJson();

    QVERIFY(_openSpeedDialog());
    auto* scope = findVisibleItem(_rootItem, QStringLiteral("caacFlightSpeedScope"));
    auto* speed = findVisibleItem(_rootItem, QStringLiteral("caacFlightSpeedValue"));
    QVERIFY(scope && speed);
    QVERIFY(scope->setProperty("currentIndex", 2));
    QVERIFY(speed->setProperty("text", QStringLiteral("0")));
    QVERIFY(acceptDialog());
    QVERIFY(findVisibleItem(_rootItem, QStringLiteral("caacFlightSpeedValue"), 0));
    QCOMPARE(master->saveToJson(), original);
    QCOMPARE(uploads.count(), 1);
    QVERIFY(speed->setProperty("text", QStringLiteral("8")));
    QVERIFY(rejectDialog());
    QTRY_VERIFY_WITH_TIMEOUT(!findVisibleItem(_rootItem, QStringLiteral("caacFlightSpeedValue"), 0),
                             TestTimeout::mediumMs());
    QCOMPARE(master->saveToJson(), original);
    QCOMPARE(uploads.count(), 1);

    const auto labelText = [&](int index) {
        auto* label = findVisibleItem(map, QStringLiteral("caacRouteLegLabel_%1").arg(index), 0);
        return label ? label->property("legText").toString() : QString();
    };
    for (int round = 0; round < 3; ++round) {
        const int scopeIndex = round == 0 ? 1 : 2;
        const bool sameValueOverride = round == 2;
        if (sameValueOverride) {
            QSignalSpy acknowledged(vehicle, &Vehicle::mavCommandResult);
            vehicle->guidedModeChangeGroundSpeedMetersSecond(8);
            const auto accepted = [&acknowledged]() {
                for (const auto& result : acknowledged) {
                    if (result[2].toInt() == MAV_CMD_DO_CHANGE_SPEED && result[3].toInt() == MAV_RESULT_ACCEPTED) {
                        return true;
                    }
                }
                return false;
            };
            QTRY_VERIFY_WITH_TIMEOUT(accepted(), TestTimeout::mediumMs());
            QTRY_COMPARE_WITH_TIMEOUT(simulation->currentMissionDetails().value("speed").toDouble(), 8.0,
                                      TestTimeout::mediumMs());
            QTRY_VERIFY_WITH_TIMEOUT(labelText(0).endsWith(QStringLiteral("8.0 m/s")), TestTimeout::mediumMs());
        }
        QVERIFY(_openSpeedDialog());
        scope = findVisibleItem(_rootItem, QStringLiteral("caacFlightSpeedScope"));
        speed = findVisibleItem(_rootItem, QStringLiteral("caacFlightSpeedValue"));
        QVERIFY(scope && speed);
        QVERIFY(scope->setProperty("currentIndex", scopeIndex));
        if (scopeIndex == 1) {
            auto* waypoint = findVisibleItem(_rootItem, QStringLiteral("caacFlightSpeedWaypoint"));
            QVERIFY(waypoint);
            QCOMPARE(waypoint->property("count").toInt(), 3);
            QVERIFY(waypoint->setProperty("currentIndex", 1));
        }
        const double requestedSpeed = scopeIndex == 1 ? 8 : 6;
        QVERIFY(speed->setProperty("text", QString::number(requestedSpeed)));
        QQmlExpression transaction(qmlContext(speed), speed, QStringLiteral("update"));
        QPointer<CaacMissionUpdateController> update =
            qobject_cast<CaacMissionUpdateController*>(transaction.evaluate().value<QObject*>());
        QVERIFY(!transaction.hasError() && update);
        QSignalSpy finished(update, &CaacMissionUpdateController::finished);
        const auto beforePosition = vehicle->coordinate();
        const auto beforeJson = master->saveToJson();
        const int beforeUploads = uploads.count();
        QVERIFY(acceptDialog());
        QVERIFY(update && update->busy());
        QTRY_VERIFY_WITH_TIMEOUT(!scope->isEnabled() && !speed->isEnabled(), TestTimeout::shortMs());
        QTRY_COMPARE_WITH_TIMEOUT(finished.count(), 1, TestTimeout::longMs());
        QVERIFY2(finished.first().first().toBool(), qPrintable(finished.first().at(1).toString()));
        QTRY_VERIFY_WITH_TIMEOUT(!findVisibleItem(_rootItem, QStringLiteral("caacFlightSpeedValue"), 0),
                                 TestTimeout::mediumMs());
        QCOMPARE(uploads.count(), beforeUploads + 1);
        if (sameValueOverride) {
            QCOMPARE(master->saveToJson(), beforeJson);
        } else {
            QVERIFY(master->saveToJson() != beforeJson);
        }
        QTRY_VERIFY_WITH_TIMEOUT(mission->uploadConfirmed(), TestTimeout::mediumMs());
        QTRY_COMPARE_WITH_TIMEOUT(vehicle->flightMode(), vehicle->missionFlightMode(), TestTimeout::mediumMs());
        QTRY_VERIFY_WITH_TIMEOUT(targetingFirst(), TestTimeout::mediumMs());
        QVERIFY(home.distanceTo(vehicle->coordinate()) >= home.distanceTo(beforePosition) - 0.5);
        QVERIFY(vehicle->airborne() && vehicle->armed());

        if (scopeIndex == 1) {
            QCOMPARE(first->specifiedFlightSpeed(), 5.0);
            QCOMPARE(second->specifiedFlightSpeed(), 8.0);
            QCOMPARE(third->specifiedFlightSpeed(), 5.0);
            QTRY_COMPARE_WITH_TIMEOUT(simulation->currentMissionDetails().value("speed").toDouble(), 5.0,
                                      TestTimeout::mediumMs());
            QTRY_VERIFY_WITH_TIMEOUT(labelText(0).endsWith(QStringLiteral("5.0 m/s")), TestTimeout::mediumMs());
            QTRY_VERIFY_WITH_TIMEOUT(labelText(1).endsWith(QStringLiteral("5.0 m/s")), TestTimeout::mediumMs());
            QTRY_VERIFY_WITH_TIMEOUT(labelText(2).endsWith(QStringLiteral("8.0 m/s")), TestTimeout::mediumMs());
        } else {
            for (auto* item : {first, second, third}) {
                QCOMPARE(item->specifiedFlightSpeed(), 6.0);
            }
            QTRY_COMPARE_WITH_TIMEOUT(simulation->currentMissionDetails().value("speed").toDouble(), 6.0,
                                      TestTimeout::mediumMs());
            for (int index = 0; index < 3; ++index) {
                QTRY_VERIFY_WITH_TIMEOUT(labelText(index).endsWith(QStringLiteral("6.0 m/s")), TestTimeout::mediumMs());
            }
            int speedCommands = 0;
            for (const auto* item : manager->missionItems()) {
                if (item->command() == MAV_CMD_DO_CHANGE_SPEED && item->param1() < 2) {
                    QCOMPARE(item->param2(), 6.0);
                    ++speedCommands;
                }
            }
            QVERIFY(speedCommands >= 3);
        }
    }
}

void CaacFlightSpeedScopeUITest::_selectedDepartureIncludesCurrentLeg()
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
    const auto firstCoordinate = QGCGeo::geodesicDestination(home, 90, 20);
    const auto secondCoordinate = QGCGeo::geodesicDestination(firstCoordinate, 0, 600);
    mission->setHomePosition(home);
    for (const auto& coordinate : {firstCoordinate, secondCoordinate}) {
        QVERIFY(editor->addWaypoint(coordinate));
        auto* item = editor->currentItem();
        QVERIFY(item && item->speedSection());
        item->altitude()->setRawValue(2);
        item->speedSection()->flightSpeed()->setRawValue(5);
        item->speedSection()->setSpecifyFlightSpeed(true);
    }
    QVERIFY(editor->setMissionFlightSpeed(true, 5));
    auto settings = CaacSimulationController::defaults();
    settings["takeoffHeight"] = 2.0;
    settings["verticalSpeed"] = 10.0;
    settings["yawRate"] = 360.0;
    settings["endBehavior"] = 2;
    QVERIFY(editor->setSimulationSettings(settings));
    panel->setProperty("activePanel", -1);
    const auto mapCenter = QGCGeo::geodesicDestination(firstCoordinate, 0, 300);
    map->setProperty("center", QVariant::fromValue(mapCenter));
    map->setProperty("zoomLevel", 16.0);
    auto* simulation = plugin->simulation();
    QVERIFY(simulation);
    const auto stop = qScopeGuard([simulation]() { simulation->stop(); });
    // Optional host pressure backends are unavailable on some desktop runners.
    ignoreLogMessage("Utilities.QGCSensors", QtWarningMsg,
                     QRegularExpression(QStringLiteral("Failed to connect to pressure backend$")));
    ignoreLogMessage("Utilities.QGCSensors", QtWarningMsg,
                     QRegularExpression(QStringLiteral("Error Initializing Pressure Sensor$")));
    QVERIFY(simulation->start(master));
    QTRY_VERIFY_WITH_TIMEOUT(simulation->vehicle() && simulation->vehicle()->isInitialConnectComplete(),
                             TestTimeout::longMs());
    QSignalSpy uploads(simulation, &CaacSimulationController::missionUploaded);
    QVERIFY(simulation->upload(master));
    QTRY_COMPARE_WITH_TIMEOUT(uploads.count(), 1, TestTimeout::longMs());
    auto* vehicle = simulation->vehicle();
    auto* manager = vehicle->missionManager();
    const auto targetingSecond = [&]() {
        const int index = manager->currentIndex();
        const auto& items = manager->missionItems();
        return index >= 0 && index < items.size() && items.at(index)->coordinate().isValid() &&
               items.at(index)->coordinate().distanceTo(secondCoordinate) < 0.1;
    };
    vehicle->startMission();
    QTRY_VERIFY_WITH_TIMEOUT(vehicle->airborne() && targetingSecond(), TestTimeout::longMs());
    QSignalSpy acknowledged(vehicle, &Vehicle::mavCommandResult);
    vehicle->guidedModeChangeGroundSpeedMetersSecond(8);
    const auto accepted = [&acknowledged]() {
        for (const auto& result : acknowledged) {
            if (result[2].toInt() == MAV_CMD_DO_CHANGE_SPEED && result[3].toInt() == MAV_RESULT_ACCEPTED) {
                return true;
            }
        }
        return false;
    };
    QTRY_VERIFY_WITH_TIMEOUT(accepted(), TestTimeout::mediumMs());
    QTRY_COMPARE_WITH_TIMEOUT(simulation->currentMissionDetails().value("speed").toDouble(), 8.0,
                              TestTimeout::mediumMs());
    map->setProperty("center", QVariant::fromValue(mapCenter));
    const auto currentLegText = [&]() {
        auto* label = findVisibleItem(map, QStringLiteral("caacRouteLegLabel_1"), 0);
        return label ? label->property("legText").toString() : QString();
    };
    QTRY_VERIFY_WITH_TIMEOUT(currentLegText().endsWith(QStringLiteral("8.0 m/s")), TestTimeout::mediumMs());
    const auto original = master->saveToJson();
    QVERIFY(_openSpeedDialog());
    auto* scope = findVisibleItem(_rootItem, QStringLiteral("caacFlightSpeedScope"));
    auto* speed = findVisibleItem(_rootItem, QStringLiteral("caacFlightSpeedValue"));
    QVERIFY(scope && speed);
    QVERIFY(scope->setProperty("currentIndex", 1));
    auto* waypoint = findVisibleItem(_rootItem, QStringLiteral("caacFlightSpeedWaypoint"));
    QVERIFY(waypoint);
    QCOMPARE(waypoint->property("count").toInt(), 2);
    QVERIFY(waypoint->setProperty("currentIndex", 0));
    QVERIFY(speed->setProperty("text", QStringLiteral("5")));
    QQmlExpression transaction(qmlContext(speed), speed, QStringLiteral("update"));
    QPointer<CaacMissionUpdateController> update =
        qobject_cast<CaacMissionUpdateController*>(transaction.evaluate().value<QObject*>());
    QVERIFY(!transaction.hasError() && update);
    QSignalSpy finished(update, &CaacMissionUpdateController::finished);
    const auto beforePosition = vehicle->coordinate();
    QVERIFY(acceptDialog());
    QVERIFY(update && update->busy());
    QTRY_COMPARE_WITH_TIMEOUT(finished.count(), 1, TestTimeout::longMs());
    QVERIFY2(finished.first().first().toBool(), qPrintable(finished.first().at(1).toString()));
    QTRY_VERIFY_WITH_TIMEOUT(!findVisibleItem(_rootItem, QStringLiteral("caacFlightSpeedValue"), 0),
                             TestTimeout::mediumMs());
    QCOMPARE(uploads.count(), 2);
    QCOMPARE(master->saveToJson(), original);
    QTRY_COMPARE_WITH_TIMEOUT(simulation->currentMissionDetails().value("speed").toDouble(), 5.0,
                              TestTimeout::mediumMs());
    QTRY_VERIFY_WITH_TIMEOUT(currentLegText().endsWith(QStringLiteral("5.0 m/s")), TestTimeout::mediumMs());
    QTRY_VERIFY_WITH_TIMEOUT(targetingSecond(), TestTimeout::mediumMs());
    QTRY_COMPARE_WITH_TIMEOUT(vehicle->flightMode(), vehicle->missionFlightMode(), TestTimeout::mediumMs());
    QVERIFY(home.distanceTo(vehicle->coordinate()) >= home.distanceTo(beforePosition) - 0.5);
    QVERIFY(vehicle->airborne() && vehicle->armed());
    for (int i = 1; i <= 2; ++i) {
        QCOMPARE(mission->visualItems()->value<SimpleMissionItem*>(i)->specifiedFlightSpeed(), 5.0);
    }
}

UT_REGISTER_TEST(CaacFlightSpeedScopeUITest, TestLabel::Integration, TestLabel::MissionManager)

#include "CaacFlightSpeedScopeUITest.moc"
