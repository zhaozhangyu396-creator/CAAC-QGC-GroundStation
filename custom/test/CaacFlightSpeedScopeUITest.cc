#include <QtCore/QPointer>
#include <QtCore/QRegularExpression>
#include <QtCore/QScopeGuard>
#include <QtQml/QQmlEngine>
#include <QtQml/QQmlExpression>
#include <QtQuick/QQuickItem>
#include <QtQuick/QQuickWindow>
#include <QtTest/QSignalSpy>

#include "AppSettings.h"
#include "CaacCorePlugin.h"
#include "CaacMissionUpdateController.h"
#include "CaacRouteEditorController.h"
#include "CaacSimulationController.h"
#include "Fact.h"
#include "FirmwarePlugin.h"
#include "MissionController.h"
#include "MissionItem.h"
#include "MissionManager.h"
#include "PlanMasterController.h"
#include "QGCGeo.h"
#include "QmlUITestBase.h"
#include "SettingsManager.h"
#include "SimpleMissionItem.h"
#include "SpeedSection.h"
#include "Vehicle.h"

class CaacFlightSpeedScopeUITest : public QmlUITestBase
{
    Q_OBJECT

private slots:
    void _scopedUpdatesPreserveFlightProgress_data();
    void _scopedUpdatesPreserveFlightProgress();
    void _selectedDepartureIncludesCurrentLeg();
    void _insertsSpeedsWithSelectedStart_data();
    void _insertsSpeedsWithSelectedStart();

private:
    bool _openSpeedDialog();
    bool _triggerFlightAction(const QString& name);
};

bool CaacFlightSpeedScopeUITest::_openSpeedDialog()
{
    return _triggerFlightAction(QStringLiteral("caacChangeFlightSpeed"));
}

bool CaacFlightSpeedScopeUITest::_triggerFlightAction(const QString& name)
{
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
}

void CaacFlightSpeedScopeUITest::_scopedUpdatesPreserveFlightProgress_data()
{
    QTest::addColumn<bool>("specifiedWaypointSpeeds");
    QTest::addColumn<bool>("allWaypointsFirst");
    QTest::newRow("existing-waypoint-speed-sections") << true << false;
    QTest::newRow("raw-upload-selected-waypoint") << false << false;
    QTest::newRow("raw-upload-all-waypoints") << false << true;
}

void CaacFlightSpeedScopeUITest::_scopedUpdatesPreserveFlightProgress()
{
    QFETCH(bool, specifiedWaypointSpeeds);
    QFETCH(bool, allWaypointsFirst);
    auto* defaultSpeed = SettingsManager::instance()->appSettings()->offlineEditingHoverSpeed();
    const auto previousDefaultSpeed = defaultSpeed->rawValue();
    const auto restoreDefaultSpeed =
        qScopeGuard([defaultSpeed, previousDefaultSpeed]() { defaultSpeed->setRawValue(previousDefaultSpeed); });
    const double initialSpeed = specifiedWaypointSpeeds ? 5.0 : 8.0;
    defaultSpeed->setRawValue(initialSpeed);
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
    const QGeoCoordinate home = specifiedWaypointSpeeds
                                    ? QGeoCoordinate(30, 120, 20)
                                    : QGeoCoordinate(25.137904838950554, 102.72128132348416, 2062.357);
    const auto firstCoordinate = QGCGeo::geodesicDestination(home, 90, specifiedWaypointSpeeds ? 600 : 60);
    const double spacing = specifiedWaypointSpeeds ? 100 : 30;
    const auto fifthCoordinate = QGCGeo::geodesicDestination(firstCoordinate, 0, spacing * 4);
    mission->setHomePosition(home);
    for (const auto& coordinate : {firstCoordinate, QGCGeo::geodesicDestination(firstCoordinate, 0, spacing),
                                   QGCGeo::geodesicDestination(firstCoordinate, 0, spacing * 2),
                                   QGCGeo::geodesicDestination(firstCoordinate, 0, spacing * 3), fifthCoordinate}) {
        QVERIFY(editor->addWaypoint(coordinate));
        auto* item = editor->currentItem();
        QVERIFY(item && item->speedSection());
        item->altitude()->setRawValue(2);
        item->speedSection()->flightSpeed()->setRawValue(5);
        item->speedSection()->setSpecifyFlightSpeed(specifiedWaypointSpeeds);
    }
    QVERIFY(editor->setMissionFlightSpeed(specifiedWaypointSpeeds, initialSpeed));
    auto* first = mission->visualItems()->value<SimpleMissionItem*>(1);
    auto* second = mission->visualItems()->value<SimpleMissionItem*>(2);
    auto* third = mission->visualItems()->value<SimpleMissionItem*>(3);
    QVERIFY(first && second && third);
    auto settings = CaacSimulationController::defaults();
    settings["takeoffHeight"] = 2.0;
    settings["verticalSpeed"] = 10.0;
    settings["yawRate"] = 360.0;
    settings["endBehavior"] = specifiedWaypointSpeeds ? 2 : 0;
    settings["closed"] = specifiedWaypointSpeeds;
    settings["loopMode"] = specifiedWaypointSpeeds ? 1 : 0;
    settings["loopCount"] = 2;
    if (specifiedWaypointSpeeds) {
        QVERIFY(editor->setSimulationSettings(settings));
    }
    panel->setProperty("activePanel", -1);
    const auto mapCenter = specifiedWaypointSpeeds ? QGCGeo::geodesicDestination(home, 90, 300)
                                                   : QGCGeo::geodesicDestination(firstCoordinate, 0, spacing * 2);
    map->setProperty("center", QVariant::fromValue(mapCenter));
    map->setProperty("zoomLevel", specifiedWaypointSpeeds ? 16.0 : 19.0);

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
    auto* vehicle = simulation->vehicle();
    auto* manager = vehicle->missionManager();
    if (specifiedWaypointSpeeds) {
        QVERIFY(simulation->upload(master));
        QTRY_COMPARE_WITH_TIMEOUT(uploads.count(), 1, TestTimeout::longMs());
    } else {
        QSignalSpy sent(manager, &MissionManager::sendComplete);
        mission->sendToVehicle();
        QTRY_COMPARE_WITH_TIMEOUT(sent.count(), 1, TestTimeout::longMs());
        QVERIFY(!sent.first().first().toBool());
        QTRY_VERIFY_WITH_TIMEOUT(simulation->missionReady(), TestTimeout::mediumMs());
        vehicle->guidedModeTakeoff(2);
        QTRY_VERIFY_WITH_TIMEOUT(vehicle->airborne(), TestTimeout::mediumMs());
        QTRY_COMPARE_WITH_TIMEOUT(vehicle->flightMode(), vehicle->firmwarePlugin()->pauseFlightMode(),
                                  TestTimeout::mediumMs());
    }
    const int initialUploads = uploads.count();
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
    if (!specifiedWaypointSpeeds) {
        vehicle->pauseVehicle();
        QTRY_COMPARE_WITH_TIMEOUT(vehicle->flightMode(), vehicle->firmwarePlugin()->pauseFlightMode(),
                                  TestTimeout::mediumMs());
    }
    const auto navigationCommandCount = [manager]() {
        int count = 0;
        for (const auto* item : manager->missionItems()) {
            count += item->command() != MAV_CMD_DO_CHANGE_SPEED;
        }
        return count;
    };
    const int originalNavigationCommands = navigationCommandCount();
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
    QCOMPARE(uploads.count(), initialUploads);
    QVERIFY(speed->setProperty("text", QStringLiteral("8")));
    QVERIFY(rejectDialog());
    QTRY_VERIFY_WITH_TIMEOUT(!findVisibleItem(_rootItem, QStringLiteral("caacFlightSpeedValue"), 0),
                             TestTimeout::mediumMs());
    QCOMPARE(master->saveToJson(), original);
    QCOMPARE(uploads.count(), initialUploads);

    const auto labelText = [&](int index) {
        auto* label = findVisibleItem(map, QStringLiteral("caacRouteLegLabel_%1").arg(index), 0);
        return label ? label->property("legText").toString() : QString();
    };
    const int rounds = specifiedWaypointSpeeds ? 3 : 1;
    for (int round = 0; round < rounds; ++round) {
        const int scopeIndex = specifiedWaypointSpeeds ? (round == 0 ? 1 : 2) : (allWaypointsFirst ? 2 : 1);
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
            QCOMPARE(waypoint->property("count").toInt(), 5);
            QVERIFY(waypoint->setProperty("currentIndex", specifiedWaypointSpeeds ? 1 : 0));
        }
        const double requestedSpeed = !specifiedWaypointSpeeds ? 9 : scopeIndex == 1 ? 8 : 6;
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
        QCOMPARE(navigationCommandCount(), originalNavigationCommands);
        if (!specifiedWaypointSpeeds) {
            QCOMPARE(vehicle->flightMode(), vehicle->firmwarePlugin()->pauseFlightMode());
            QVERIFY(beforePosition.distanceTo(vehicle->coordinate()) < 0.1);
            QVERIFY(_triggerFlightAction(QStringLiteral("caacStartMission")));
        }
        QTRY_COMPARE_WITH_TIMEOUT(vehicle->flightMode(), vehicle->missionFlightMode(), TestTimeout::mediumMs());
        QTRY_VERIFY_WITH_TIMEOUT(targetingFirst(), TestTimeout::mediumMs());
        QVERIFY(home.distanceTo(vehicle->coordinate()) >= home.distanceTo(beforePosition) - 0.5);
        QVERIFY(vehicle->airborne() && vehicle->armed());

        if (scopeIndex == 1) {
            const double firstSpeed = specifiedWaypointSpeeds ? initialSpeed : requestedSpeed;
            const double secondSpeed = specifiedWaypointSpeeds ? requestedSpeed : initialSpeed;
            QCOMPARE(first->specifiedFlightSpeed(), firstSpeed);
            QCOMPARE(second->specifiedFlightSpeed(), secondSpeed);
            QCOMPARE(third->specifiedFlightSpeed(), initialSpeed);
            QTRY_COMPARE_WITH_TIMEOUT(simulation->currentMissionDetails().value("speed").toDouble(), initialSpeed,
                                      TestTimeout::mediumMs());
            const auto initialLabel = QStringLiteral("%1 m/s").arg(initialSpeed, 0, 'f', 1);
            QTRY_VERIFY_WITH_TIMEOUT(labelText(0).endsWith(initialLabel), TestTimeout::mediumMs());
            const auto firstLabel = QStringLiteral("%1 m/s").arg(firstSpeed, 0, 'f', 1);
            const auto secondLabel = QStringLiteral("%1 m/s").arg(secondSpeed, 0, 'f', 1);
            QTRY_VERIFY_WITH_TIMEOUT(labelText(1).endsWith(firstLabel), TestTimeout::mediumMs());
            QTRY_VERIFY_WITH_TIMEOUT(labelText(2).endsWith(secondLabel), TestTimeout::mediumMs());
        } else {
            for (auto* item : {first, second, third}) {
                QCOMPARE(item->specifiedFlightSpeed(), requestedSpeed);
            }
            QTRY_COMPARE_WITH_TIMEOUT(simulation->currentMissionDetails().value("speed").toDouble(), requestedSpeed,
                                      TestTimeout::mediumMs());
            for (int index = 0; index < 3; ++index) {
                const auto requestedLabel = QStringLiteral("%1 m/s").arg(requestedSpeed, 0, 'f', 1);
                QTRY_VERIFY_WITH_TIMEOUT(labelText(index).endsWith(requestedLabel), TestTimeout::mediumMs());
            }
            int speedCommands = 0;
            for (const auto* item : manager->missionItems()) {
                if (item->command() == MAV_CMD_DO_CHANGE_SPEED && item->param1() < 2) {
                    QCOMPARE(item->param2(), requestedSpeed);
                    ++speedCommands;
                }
            }
            QVERIFY(speedCommands >= 3);
        }
        const double expectedSpeed = scopeIndex == 1 ? initialSpeed : requestedSpeed;
        QTRY_VERIFY_WITH_TIMEOUT(qAbs(vehicle->groundSpeed()->rawValue().toDouble() - expectedSpeed) < 0.2,
                                 TestTimeout::mediumMs());
    }

    if (!specifiedWaypointSpeeds) {
        if (!allWaypointsFirst) {
            QTRY_VERIFY_WITH_TIMEOUT(targetCoordinate().distanceTo(second->coordinate()) < 0.1 &&
                                         qAbs(vehicle->groundSpeed()->rawValue().toDouble() - 9.0) < 0.2,
                                     TestTimeout::longMs());
            QCOMPARE(simulation->currentMissionDetails().value("speed").toDouble(), 9.0);
        } else {
            vehicle->pauseVehicle();
            QTRY_COMPARE_WITH_TIMEOUT(vehicle->flightMode(), vehicle->firmwarePlugin()->pauseFlightMode(),
                                      TestTimeout::mediumMs());
            auto changedSettings = CaacSimulationController::defaults();
            changedSettings["horizontalSpeed"] = initialSpeed;
            changedSettings["closed"] = true;
            QVERIFY(master->setProperty("caacSimulationSettings", changedSettings) ||
                    master->property("caacSimulationSettings").toMap() == changedSettings);
            QVERIFY(plugin->setFlightWaypointSpeeds(master, {}, 10, true).isEmpty());
            const int beforeRejectedUploads = uploads.count();
            for (int attempt = 0; attempt < 2; ++attempt) {
                CaacMissionUpdateController rejected;
                QSignalSpy rejectedResult(&rejected, &CaacMissionUpdateController::finished);
                QVERIFY(rejected.updateSpeeds(master, simulation, 10));
                QTRY_COMPARE_WITH_TIMEOUT(rejectedResult.count(), 1, TestTimeout::longMs());
                QVERIFY(!rejectedResult.first().first().toBool());
                QCOMPARE(uploads.count(), beforeRejectedUploads);
                QCOMPARE(vehicle->flightMode(), vehicle->firmwarePlugin()->pauseFlightMode());
                QCOMPARE(simulation->currentMissionDetails().value("speed").toDouble(), 9.0);
                QVERIFY(!mission->uploadConfirmed());
            }
        }
        return;
    }
    int fifthSequence = -1;
    for (int index = 0; index < manager->missionItems().size(); ++index) {
        const auto* item = manager->missionItems().at(index);
        if (item->command() == MAV_CMD_NAV_WAYPOINT && item->coordinate().distanceTo(fifthCoordinate) < 0.1) {
            fifthSequence = index;
            break;
        }
    }
    QVERIFY(fifthSequence >= 0);
    const int offset = vehicle->firmwarePlugin()->sendHomePositionToVehicle() ? 0 : 1;
    vehicle->setCurrentMissionSequence(fifthSequence + offset);
    const auto targetingFifth = [&]() {
        const auto target = targetCoordinate();
        return target.isValid() && target.distanceTo(fifthCoordinate) < 0.1;
    };
    QTRY_VERIFY_WITH_TIMEOUT(targetingFifth(), TestTimeout::mediumMs());
    QQmlExpression liveMaster(qmlContext(panel), panel, QStringLiteral("globals.planMasterControllerFlyView"));
    auto* flyMaster = liveMaster.evaluate().value<PlanMasterController*>();
    QVERIFY(!liveMaster.hasError() && flyMaster);
    auto* flyMission = flyMaster->missionController();
    QTRY_VERIFY_WITH_TIMEOUT(flyMission->currentMissionIndex() >= flyMission->visualItems()->count() - 1,
                             TestTimeout::mediumMs());
    const int remainingLoops = simulation->remainingLoops();
    QVERIFY(remainingLoops >= 1);
    const auto pauseMode = vehicle->firmwarePlugin()->pauseFlightMode();
    for (int repetition = 0; repetition < 2; ++repetition) {
        QVERIFY(_triggerFlightAction(QStringLiteral("caacPauseMission")));
        QTRY_COMPARE_WITH_TIMEOUT(vehicle->flightMode(), pauseMode, TestTimeout::mediumMs());
        QVERIFY(targetingFifth());
        QCOMPARE(simulation->remainingLoops(), remainingLoops);
        auto* start =
            findVisibleItemScrolled(QStringLiteral("caacStartMission"), QStringLiteral("flyViewToolStrip_flickable"));
        QVERIFY(start);
        QTRY_VERIFY_WITH_TIMEOUT(start->isEnabled(), TestTimeout::mediumMs());
        QVERIFY(_triggerFlightAction(QStringLiteral("caacStartMission")));
        QTRY_COMPARE_WITH_TIMEOUT(vehicle->flightMode(), vehicle->missionFlightMode(), TestTimeout::mediumMs());
        QVERIFY(targetingFifth());
        QCOMPARE(simulation->remainingLoops(), remainingLoops);
    }

    QVERIFY(_triggerFlightAction(QStringLiteral("caacPauseMission")));
    QTRY_COMPARE_WITH_TIMEOUT(vehicle->flightMode(), pauseMode, TestTimeout::mediumMs());
    const auto pausedPosition = vehicle->coordinate();
    const auto pausedJson = master->saveToJson();
    QVERIFY(_openSpeedDialog());
    scope = findVisibleItem(_rootItem, QStringLiteral("caacFlightSpeedScope"));
    speed = findVisibleItem(_rootItem, QStringLiteral("caacFlightSpeedValue"));
    QVERIFY(scope && speed);
    QVERIFY(scope->setProperty("currentIndex", 2));
    QVERIFY(speed->setProperty("text", QStringLiteral("6")));
    QQmlExpression transaction(qmlContext(speed), speed, QStringLiteral("update"));
    QPointer<CaacMissionUpdateController> update =
        qobject_cast<CaacMissionUpdateController*>(transaction.evaluate().value<QObject*>());
    QVERIFY(!transaction.hasError() && update);
    QSignalSpy finished(update, &CaacMissionUpdateController::finished);
    const int uploadsBeforePausedChange = uploads.count();
    auto* busyStart = findItem(_rootItem, QStringLiteral("caacStartMission"));
    QVERIFY(busyStart);
    QVERIFY(acceptDialog());
    QVERIFY(update && update->busy());
    QVERIFY(vehicle->flightPlanUpdateInProgress());
    QTRY_VERIFY_WITH_TIMEOUT(!busyStart->isEnabled(), TestTimeout::shortMs());
    QTRY_COMPARE_WITH_TIMEOUT(finished.count(), 1, TestTimeout::longMs());
    QVERIFY(!vehicle->flightPlanUpdateInProgress());
    QVERIFY2(finished.first().first().toBool(), qPrintable(finished.first().at(1).toString()));
    QTRY_VERIFY_WITH_TIMEOUT(!findVisibleItem(_rootItem, QStringLiteral("caacFlightSpeedValue"), 0),
                             TestTimeout::mediumMs());
    QCOMPARE(uploads.count(), uploadsBeforePausedChange + 1);
    QCOMPARE(master->saveToJson(), pausedJson);
    QCOMPARE(vehicle->flightMode(), pauseMode);
    QVERIFY(vehicle->coordinate().distanceTo(pausedPosition) < 0.1);
    QVERIFY(targetingFifth());
    QCOMPARE(simulation->remainingLoops(), remainingLoops);
    QTRY_COMPARE_WITH_TIMEOUT(simulation->currentMissionDetails().value("speed").toDouble(), 6.0,
                              TestTimeout::mediumMs());
    auto* start =
        findVisibleItemScrolled(QStringLiteral("caacStartMission"), QStringLiteral("flyViewToolStrip_flickable"));
    QVERIFY(start);
    QTRY_VERIFY_WITH_TIMEOUT(start->isEnabled(), TestTimeout::mediumMs());
    QVERIFY(_triggerFlightAction(QStringLiteral("caacStartMission")));
    QTRY_COMPARE_WITH_TIMEOUT(vehicle->flightMode(), vehicle->missionFlightMode(), TestTimeout::mediumMs());
    QVERIFY(targetingFifth());
    QCOMPARE(simulation->remainingLoops(), remainingLoops);
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

void CaacFlightSpeedScopeUITest::_insertsSpeedsWithSelectedStart_data()
{
    QTest::addColumn<bool>("allWaypoints");
    QTest::newRow("selected-waypoint-open-loop") << false;
    QTest::newRow("all-waypoints-open-loop") << true;
}

void CaacFlightSpeedScopeUITest::_insertsSpeedsWithSelectedStart()
{
    QFETCH(bool, allWaypoints);
    startUI();
    if (QTest::currentTestFailed()) {
        return;
    }
    QVERIFY(QMetaObject::invokeMethod(_window, "showPlanView"));
    auto* plan = findVisibleItem(_rootItem, QStringLiteral("mainView_plan"));
    auto* panel = findVisibleItem(_rootItem, QStringLiteral("caacRouteEditor"));
    QVERIFY(plan && panel);
    auto* master = plan->property("_planMasterController").value<PlanMasterController*>();
    auto* editor = panel->property("routeController").value<CaacRouteEditorController*>();
    auto* plugin = qobject_cast<CaacCorePlugin*>(QGCCorePlugin::instance());
    QVERIFY(master && editor && plugin);
    master->removeAll();
    auto* mission = master->missionController();
    const QGeoCoordinate home(30, 120, 20);
    mission->setHomePosition(home);
    for (const double bearing : {0.0, 90.0, 180.0}) {
        QVERIFY(editor->addWaypoint(QGCGeo::geodesicDestination(home, bearing, 600)));
        editor->currentItem()->altitude()->setRawValue(2);
        QVERIFY(!editor->currentItem()->speedSection()->specifyFlightSpeed());
    }
    auto* selected = mission->visualItems()->value<SimpleMissionItem*>(2);
    QVERIFY(selected);
    const auto selectedCoordinate = selected->coordinate();
    QVERIFY(editor->setMissionFlightSpeed(true, 5));
    auto settings = CaacSimulationController::defaults();
    settings["takeoffHeight"] = 2.0;
    settings["verticalSpeed"] = 10.0;
    settings["startSequence"] = selected->sequenceNumber();
    settings["loopMode"] = 1;
    settings["loopCount"] = 2;
    settings["endBehavior"] = 2;
    QVERIFY(editor->setSimulationSettings(settings));
    panel->setProperty("activePanel", -1);
    auto* simulation = plugin->simulation();
    QVERIFY(simulation);
    const auto stop = qScopeGuard([simulation]() { simulation->stop(); });
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
    const auto targetingSelected = [&]() {
        const int index = manager->currentIndex();
        const auto& items = manager->missionItems();
        return index >= 0 && index < items.size() && items.at(index)->command() == MAV_CMD_NAV_WAYPOINT &&
               items.at(index)->coordinate().distanceTo(selectedCoordinate) < 0.1;
    };
    vehicle->startMission();
    QTRY_VERIFY_WITH_TIMEOUT(vehicle->airborne() && targetingSelected(), TestTimeout::longMs());
    const int loops = simulation->remainingLoops();
    QVERIFY(_openSpeedDialog());
    auto* scope = findVisibleItem(_rootItem, QStringLiteral("caacFlightSpeedScope"));
    auto* speed = findVisibleItem(_rootItem, QStringLiteral("caacFlightSpeedValue"));
    auto* waypoint = findItem(_rootItem, QStringLiteral("caacFlightSpeedWaypoint"));
    QVERIFY(scope && speed && waypoint);
    QVERIFY(scope->setProperty("currentIndex", allWaypoints ? 2 : 1));
    QVERIFY(waypoint->setProperty("currentIndex", 1));
    QVERIFY(speed->setProperty("text", QStringLiteral("6")));
    QQmlExpression transaction(qmlContext(speed), speed, QStringLiteral("update"));
    QPointer<CaacMissionUpdateController> update =
        qobject_cast<CaacMissionUpdateController*>(transaction.evaluate().value<QObject*>());
    QVERIFY(!transaction.hasError() && update);
    QSignalSpy finished(update, &CaacMissionUpdateController::finished);
    QVERIFY(acceptDialog());
    QTRY_COMPARE_WITH_TIMEOUT(finished.count(), 1, TestTimeout::longMs());
    QVERIFY2(finished.first().first().toBool(), qPrintable(finished.first().at(1).toString()));
    QCOMPARE(uploads.count(), 2);
    QCOMPARE(vehicle->flightMode(), vehicle->missionFlightMode());
    QVERIFY(targetingSelected());
    QCOMPARE(simulation->remainingLoops(), loops);
    QCOMPARE(selected->specifiedFlightSpeed(), 6.0);
    QCOMPARE(simulation->currentMissionDetails().value("speed").toDouble(), allWaypoints ? 6.0 : 5.0);
}

UT_REGISTER_TEST(CaacFlightSpeedScopeUITest, TestLabel::Integration, TestLabel::MissionManager)

#include "CaacFlightSpeedScopeUITest.moc"
