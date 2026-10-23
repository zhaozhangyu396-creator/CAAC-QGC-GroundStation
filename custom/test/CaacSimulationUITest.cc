#include <algorithm>
#include <cmath>
#include <cstdio>
#include <functional>
#include <memory>
#include <utility>

#include <QtCore/QJsonArray>
#include <QtCore/QLineF>
#include <QtCore/QRegularExpression>
#include <QtCore/QScopeGuard>
#include <QtCore/QSettings>
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
#include "CaacMissionUpdateController.h"
#include "CaacRouteEditorController.h"
#include "CaacSimulationController.h"
#include "CaacSimulationEngine.h"
#include "FlightPathSegment.h"
#include "Joystick.h"
#include "JoystickManager.h"
#include "JoystickManagerSettings.h"
#include "JoystickSettings.h"
#include "MissionController.h"
#include "MissionManager.h"
#include "MissionSettingsItem.h"
#include "MockJoystick.h"
#include "MockLink.h"
#include "MockLinkMissionItemHandler.h"
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
    void _levelFlightTelemetry();
    void _uploadAndFlightControls();
    void _repeatFromLanding();
    void _usbJoystickModeSwitch();
};

class CaacSimulationFlowUITest : public QmlUITestBase
{
    Q_OBJECT
private slots:
    void _closedRouteAndSimulationToggle();
    void _fourLoopsFromUnsavedSettings();
};

class CaacDefaultHomeUITest : public QmlUITestBase
{
    Q_OBJECT
private slots:
    void _defaultStartPosition();
    void _missingAltitudeSimulation_data();
    void _missingAltitudeSimulation();
};

class CaacMissionUpdateUITest : public QmlUITestBase
{
    Q_OBJECT
private slots:
    void _updateWaypointsDuringFlight();
};

void CaacDefaultHomeUITest::_missingAltitudeSimulation_data()
{
    QTest::addColumn<bool>("terrainAvailable");
    QTest::newRow("configured-training-field") << false;
    QTest::newRow("terrain-at-another-location") << true;
}

void CaacDefaultHomeUITest::_missingAltitudeSimulation()
{
    QFETCH(bool, terrainAvailable);
    startUI();
    if (QTest::currentTestFailed()) {
        return;
    }
    auto* panel = findVisibleItem(_rootItem, QStringLiteral("caacRouteEditor"));
    QVERIFY(panel);
    auto* master = panel->property("planMasterController").value<PlanMasterController*>();
    auto* editor = panel->property("routeController").value<CaacRouteEditorController*>();
    auto* plugin = qobject_cast<CaacCorePlugin*>(QGCCorePlugin::instance());
    QVERIFY(master && editor && plugin);
    auto* mission = master->missionController();
    auto expected = CaacCorePlugin::defaultHomePosition();
    if (terrainAvailable) {
        expected = expected.atDistanceAndAzimuth(1000, 90);
        expected.setAltitude(1800);
    }
    auto missing = expected;
    missing.setAltitude(0);
    mission->setHomePosition(missing);
    QVERIFY(editor->addWaypoint(expected.atDistanceAndAzimuth(30, 0)));
    QVERIFY(editor->addWaypoint(expected.atDistanceAndAzimuth(60, 0)));
    for (int i = 1; i <= 2; ++i) {
        mission->visualItems()->value<SimpleMissionItem*>(i)->altitude()->setRawValue(20);
    }
    auto* home = mission->visualItems()->value<MissionSettingsItem*>(0);
    QVERIFY(home);
    if (terrainAvailable) {
        const QList<double> heights{expected.altitude()};
        QVERIFY(QMetaObject::invokeMethod(home, "_terrainDataReceived", Qt::DirectConnection, Q_ARG(bool, true),
                                          Q_ARG(QList<double>, heights)));
    }
    auto* simulation = plugin->simulation();
    QVERIFY(simulation);
    const auto stop = qScopeGuard([simulation]() { simulation->stop(); });
    QVERIFY(clickToolSelectDropdownButton(QStringLiteral("toolbar_simulation")));
    QTRY_VERIFY_WITH_TIMEOUT(simulation->vehicle() && simulation->vehicle()->isInitialConnectComplete(),
                             TestTimeout::longMs());
    QTRY_COMPARE_WITH_TIMEOUT(mission->plannedHomePosition(), expected, TestTimeout::mediumMs());
    QVERIFY(clickButton(QStringLiteral("caacGlobalSettingsButton")));
    auto* field =
        findVisibleItemScrolled(QStringLiteral("caacHomeAltitude"), QStringLiteral("caacGlobalSettingsScroller"));
    QVERIFY(field);
    QTRY_COMPARE_WITH_TIMEOUT(field->property("text").toString().toDouble(), expected.altitude(),
                              TestTimeout::mediumMs());
    QVERIFY(clickButtonScrolled(QStringLiteral("caacApplyHome"), QStringLiteral("caacGlobalSettingsScroller")));
    QVERIFY(!home->property("caacHomeAltitudeManual").toBool());
    const auto saved = master->saveToJson()
                           .object()
                           .value(QStringLiteral("mission"))
                           .toObject()
                           .value(QStringLiteral("plannedHomePosition"))
                           .toArray();
    QCOMPARE(saved.at(2).toDouble(), expected.altitude());
    QSignalSpy uploaded(simulation, &CaacSimulationController::missionUploaded);
    QVERIFY(simulation->upload(master));
    QTRY_COMPARE_WITH_TIMEOUT(uploaded.count(), 1, TestTimeout::longMs());
    QVERIFY(simulation->missionReady());
    QTRY_COMPARE_WITH_TIMEOUT(simulation->vehicle()->homePosition().altitude(), expected.altitude(),
                              TestTimeout::mediumMs());
    QTRY_COMPARE_WITH_TIMEOUT(simulation->vehicle()->altitudeAMSL()->rawValue().toDouble(), expected.altitude(),
                              TestTimeout::mediumMs());
    const auto aircraftPosition = simulation->vehicle()->coordinate();
    auto moved = expected.atDistanceAndAzimuth(20, 90);
    moved.setAltitude(expected.altitude());
    auto* map = qobject_cast<QQuickItem*>(panel->property("editorMap").value<QObject*>());
    QVERIFY(map);
    const QVariant homeObject = QVariant::fromValue(static_cast<QObject*>(home));
    QVERIFY(QMetaObject::invokeMethod(map, "missionItemDragStarted", Q_ARG(QVariant, homeObject)));
    mission->setHomePosition(moved);
    QVERIFY(QMetaObject::invokeMethod(map, "missionItemDragFinished", Q_ARG(QVariant, homeObject)));
    const QList<double> movedTerrain{expected.altitude() + 5};
    QVERIFY(QMetaObject::invokeMethod(home, "_terrainDataReceived", Qt::DirectConnection, Q_ARG(bool, true),
                                      Q_ARG(QList<double>, movedTerrain)));
    moved.setAltitude(expected.altitude() + 5);
    expected = moved;
    QTRY_COMPARE_WITH_TIMEOUT(mission->plannedHomePosition(), expected, TestTimeout::mediumMs());
    QTRY_VERIFY_WITH_TIMEOUT(std::abs(simulation->vehicle()->homePosition().altitude() - expected.altitude()) < 0.01,
                             TestTimeout::mediumMs());
    QTRY_VERIFY_WITH_TIMEOUT(simulation->vehicle()->homePosition().distanceTo(expected) < 0.03,
                             TestTimeout::mediumMs());
    QTRY_COMPARE_WITH_TIMEOUT(field->property("text").toString().toDouble(), expected.altitude(),
                              TestTimeout::mediumMs());
    QVERIFY(simulation->vehicle()->coordinate().distanceTo(aircraftPosition) < 0.03);
    QVERIFY(simulation->missionReady());
    QCOMPARE(uploaded.count(), 1);
    const auto findLeg = [&]() -> FlightPathSegment* {
        auto* legs = mission->simpleFlightPathSegments();
        for (int i = 0; i < legs->count(); ++i) {
            auto* leg = legs->value<FlightPathSegment*>(i);
            if (leg && std::abs(leg->coord1AMSLAlt() - expected.altitude() - 20) < 0.01 &&
                std::abs(leg->coord2AMSLAlt() - expected.altitude() - 20) < 0.01) {
                return leg;
            }
        }
        return nullptr;
    };
    QTRY_VERIFY_WITH_TIMEOUT(findLeg(), TestTimeout::mediumMs());
    auto* leg = findLeg();
    QVERIFY(leg->setProperty("amslTerrainHeights", QVariantList{expected.altitude()}));
    QVERIFY(QMetaObject::invokeMethod(leg, "_updateTerrainCollision", Qt::DirectConnection));
    QVERIFY(!leg->terrainCollision());
    QVERIFY(leg->setProperty("amslTerrainHeights", QVariantList{expected.altitude() + 30}));
    QVERIFY(QMetaObject::invokeMethod(leg, "_updateTerrainCollision", Qt::DirectConnection));
    QVERIFY(leg->terrainCollision());
}

void CaacSimulationUITest::_repeatFromLanding()
{
    startUI();
    if (QTest::currentTestFailed()) {
        return;
    }
    auto* plan = findVisibleItem(_rootItem, QStringLiteral("mainView_plan"));
    QVERIFY(plan);
    auto* master = plan->property("_planMasterController").value<PlanMasterController*>();
    QVERIFY(master);
    const QGeoCoordinate home(30, 120, 0);
    const auto landing = QGCGeo::geodesicDestination(home, 90, 15);
    auto* mission = master->missionController();
    mission->setHomePosition(home);
    auto* waypoint = qobject_cast<SimpleMissionItem*>(
        mission->insertSimpleMissionItem(QGCGeo::geodesicDestination(home, 0, 15), 1, false));
    auto* land = qobject_cast<SimpleMissionItem*>(mission->insertSimpleMissionItem(landing, 2, false));
    QVERIFY(waypoint && land);
    waypoint->altitude()->setRawValue(3);
    land->missionItem().setCommand(MAV_CMD_NAV_LAND);
    land->altitude()->setRawValue(0);
    CaacRouteEditorController editor;
    editor.setPlanMasterController(master);
    auto settings = CaacSimulationController::defaults();
    settings["takeoffHeight"] = 3.0;
    settings["horizontalSpeed"] = 25.0;
    settings["verticalSpeed"] = 10.0;
    settings["yawRate"] = 360.0;
    QVERIFY(editor.setSimulationSettings(settings));
    auto* plugin = qobject_cast<CaacCorePlugin*>(QGCCorePlugin::instance());
    QVERIFY(plugin);
    auto* simulation = plugin->simulation();
    const auto stop = qScopeGuard([simulation]() { simulation->stop(); });
    QVERIFY(simulation->start(master));
    QTRY_VERIFY_WITH_TIMEOUT(simulation->vehicle() && simulation->vehicle()->isInitialConnectComplete(),
                             TestTimeout::longMs());
    QSignalSpy uploaded(simulation, &CaacSimulationController::missionUploaded);
    QVERIFY(!mission->uploadConfirmed());
    QVERIFY(simulation->upload(master));
    QVERIFY(!mission->uploadConfirmed());
    QTRY_VERIFY_WITH_TIMEOUT(!uploaded.isEmpty(), TestTimeout::longMs());
    QVERIFY(mission->uploadConfirmed());
    auto* vehicle = simulation->vehicle();
    const auto action = [&](const QString& name) {
        if (!findVisibleItem(_rootItem, QStringLiteral("caacFlightSettingsPanel"), 0) &&
            !clickButton(QStringLiteral("caacFlightSettingsButton"))) {
            return false;
        }
        auto* button = findVisibleItemScrolled(name, QStringLiteral("flyViewToolStrip_flickable"));
        if (!button || !button->isEnabled()) {
            return false;
        }
        const auto point = button->mapToScene(QPointF(button->width() / 2, button->height() / 2)).toPoint();
        QTest::mouseDClick(_window, Qt::LeftButton, Qt::NoModifier, point);
        return true;
    };
    for (int round = 0; round < 2; ++round) {
        QVERIFY(action(QStringLiteral("caacTakeoff")));
        QTRY_VERIFY_WITH_TIMEOUT(vehicle->airborne() && vehicle->altitudeRelative()->rawValue().toDouble() > 2.9,
                                 TestTimeout::longMs());
        QVERIFY(QGCGeo::geodesicDistance(vehicle->coordinate(), round == 0 ? home : landing) < 0.2);
        QVERIFY(action(QStringLiteral("caacStartMission")));
        QTRY_VERIFY_WITH_TIMEOUT(QGCGeo::geodesicDistance(vehicle->coordinate(), round == 0 ? home : landing) > 3,
                                 TestTimeout::longMs());
        QTRY_VERIFY_WITH_TIMEOUT(!vehicle->airborne() && !vehicle->armed() &&
                                     simulation->status() == QCoreApplication::translate("CaacSimulationController",
                                                                                         "Simulation mission finished"),
                                 TestTimeout::longMs());
        QVERIFY(QGCGeo::geodesicDistance(vehicle->coordinate(), landing) < 0.2);
        auto* marker = findVisibleItem(_rootItem, QStringLiteral("planView_vehicle_%1").arg(vehicle->id()));
        QVERIFY(marker);
        QVERIFY(!marker->property("previewHome").toBool());
        QVERIFY(QGCGeo::geodesicDistance(marker->property("coordinate").value<QGeoCoordinate>(), landing) < 0.2);
    }
}

void CaacMissionUpdateUITest::_updateWaypointsDuringFlight()
{
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
    const QGeoCoordinate home(30, 120, 20);
    master->missionController()->setHomePosition(home);
    QVERIFY(editor->addWaypoint(QGCGeo::geodesicDestination(home, 90, 200)));
    auto* waypoint = editor->currentItem();
    waypoint->altitude()->setRawValue(2);
    QVERIFY(editor->addWaypoint(QGCGeo::geodesicDestination(home, 0, 200)));
    editor->currentItem()->altitude()->setRawValue(2);
    auto* landing = qobject_cast<SimpleMissionItem*>(
        master->missionController()->insertSimpleMissionItem(QGCGeo::geodesicDestination(home, 180, 200), 3, false));
    QVERIFY(landing);
    landing->missionItem().setCommand(MAV_CMD_NAV_LAND);
    landing->altitude()->setRawValue(0);
    auto settings = CaacSimulationController::defaults();
    settings["takeoffHeight"] = 2.0;
    settings["loopMode"] = 1;
    settings["loopCount"] = 3;
    settings["closed"] = true;
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
    QSignalSpy uploaded(simulation, &CaacSimulationController::missionUploaded);
    QVERIFY(simulation->upload(master));
    QTRY_VERIFY_WITH_TIMEOUT(!uploaded.isEmpty(), TestTimeout::longMs());
    auto* vehicle = simulation->vehicle();
    auto* manager = vehicle->missionManager();
    vehicle->startMission();
    QTRY_VERIFY_WITH_TIMEOUT(vehicle->airborne() && vehicle->groundSpeed()->rawValue().toDouble() > 1,
                             TestTimeout::longMs());
    QVERIFY(clickButtonScrolled(QStringLiteral("caacMissionDetailsButton"), QStringLiteral("caacMainToolsScroller")));
    QTRY_VERIFY_WITH_TIMEOUT(
        findVisibleItem(_rootItem, QStringLiteral("caacMissionSpeed")) &&
            findVisibleItem(_rootItem, QStringLiteral("caacMissionSpeed"))->property("text").toString() ==
                QStringLiteral("5.0 m/s"),
        TestTimeout::mediumMs());
    QVERIFY(verifyText(QStringLiteral("caacMissionClimb"),
                       QCoreApplication::translate("CaacMissionDetails", "Change altitude along the leg"),
                       QStringLiteral("uploaded simulation climb mode")));
    QVERIFY(clickButtonScrolled(QStringLiteral("caacMissionDetailsButton"), QStringLiteral("caacMainToolsScroller")));
    const int previousTarget = manager->currentIndex();
    const int previousLoops = simulation->remainingLoops();
    auto* map = panel->property("editorMap").value<QObject*>();
    QVERIFY(map);
    auto* mapItem = qobject_cast<QQuickItem*>(map);
    QVERIFY(mapItem);
    QString dragError;
    const auto drag = [&](SimpleMissionItem* item) -> bool {
        dragError.clear();
        map->setProperty("center", QVariant::fromValue(item->coordinate()));
        map->setProperty("zoomLevel", 17);
        auto* dragger = findVisibleItem(_rootItem, QStringLiteral("missionItemDrag_%1").arg(item->sequenceNumber()));
        auto* indicator = dragger ? dragger->property("itemIndicator").value<QQuickItem*>() : nullptr;
        if (!dragger || !indicator) {
            dragError = QStringLiteral("Missing dragger or marker");
            return false;
        }
        const auto markerPosition = [&]() {
            return dragger->mapToScene(indicator->property("anchorPoint").toPointF() +
                                       QPointF(dragger->property("_touchMarginHorizontal").toDouble(),
                                               dragger->property("_touchMarginVertical").toDouble()));
        };
        const auto middle = mapItem->mapToScene(QPointF(mapItem->width() / 2, mapItem->height() / 2));
        if (!waitForCondition([&]() { return QLineF(markerPosition(), middle).length() < 1; }, TestTimeout::mediumMs(),
                              QStringLiteral("Waypoint drag marker centered"))) {
            return false;
        }
        const auto before = item->coordinate();
        const auto start = markerPosition().toPoint();
        QTest::mousePress(_window, Qt::LeftButton, Qt::NoModifier, start);
        auto* grabber = _window->mouseGrabberItem();
        const bool grabbedWaypoint = grabber && grabber->parentItem() &&
                                     grabber->parentItem()->property("gestureTarget").value<QObject*>() == dragger;
        if (!grabbedWaypoint) {
            dragError = QStringLiteral("Mouse press reached %1 (%2), not the waypoint at %3,%4")
                            .arg(grabber ? grabber->objectName() : QStringLiteral("null"),
                                 grabber ? QString::fromLatin1(grabber->metaObject()->className()) : QString())
                            .arg(start.x())
                            .arg(start.y());
        }
        for (int step = 1; step <= 6; ++step) {
            QTest::mouseMove(_window, start + QPoint(step * 5, -step * 5));
        }
        QTest::mouseRelease(_window, Qt::LeftButton, Qt::NoModifier, start + QPoint(30, -30));
        if (QGCGeo::geodesicDistance(before, item->coordinate()) <= 0.01) {
            if (dragError.isEmpty()) {
                dragError = QStringLiteral("Waypoint coordinate did not move");
            }
            return false;
        }
        if (!waitForDialog(QCoreApplication::translate("CaacRouteEditor", "Update waypoints"))) {
            dragError = QStringLiteral("Waypoint moved, but no upload dialog appeared");
            return false;
        }
        return true;
    };
    QSignalSpy transfers(manager, &MissionManager::sendComplete);
    auto* globalSettings = findItem(_rootItem, QStringLiteral("caacGlobalSettingsPanel"));
    auto* takeoffDraft = findItem(_rootItem, QStringLiteral("caacSimTakeoffHeight"));
    QVERIFY(globalSettings && takeoffDraft);
    QVERIFY(takeoffDraft->setProperty("text", QStringLiteral("3")));
    QVERIFY(globalSettings->setProperty("simulationInputsEdited", true));
    QVERIFY2(drag(waypoint), qPrintable(dragError));
    QVERIFY(rejectDialog());
    QCOMPARE(transfers.count(), 0);
    QCOMPARE(vehicle->flightMode(), vehicle->missionFlightMode());
    QVERIFY2(drag(waypoint), qPrintable(dragError));
    auto* update = panel->findChild<CaacMissionUpdateController*>(QStringLiteral("caacMissionUpdateController"));
    QVERIFY(update);
    QSignalSpy finished(update, &CaacMissionUpdateController::finished);
    bool pausedDuringTransfer = false;
    const auto transfer = connect(manager, &MissionManager::inProgressChanged, this, [&](bool progress) {
        if (progress) {
            pausedDuringTransfer = vehicle->flightMode() != vehicle->missionFlightMode();
        }
    });
    const auto disconnectTransfer = qScopeGuard([&]() { disconnect(transfer); });
    QVERIFY(acceptDialog());
    QTRY_VERIFY_WITH_TIMEOUT(!finished.isEmpty(), TestTimeout::longMs());
    QVERIFY2(finished.last()[0].toBool(), qPrintable(finished.last()[1].toString()));
    QVERIFY(pausedDuringTransfer);
    QCOMPARE(editor->simulationSettings().value("takeoffHeight").toDouble(), 2.0);
    QVERIFY(globalSettings->property("simulationInputsEdited").toBool());
    QCOMPARE(transfers.count(), 1);
    QCOMPARE(manager->currentIndex(), previousTarget);
    QCOMPARE(simulation->remainingLoops(), previousLoops);
    QCOMPARE(vehicle->flightMode(), vehicle->missionFlightMode());
    const auto positionUploaded = [&](const SimpleMissionItem* point) {
        const auto& items = manager->missionItems();
        return std::any_of(items.cbegin(), items.cend(), [&](const MissionItem* item) {
            return item->command() == point->command() &&
                   QGCGeo::geodesicDistance(item->coordinate(), point->coordinate()) < 0.02;
        });
    };
    QVERIFY(positionUploaded(waypoint));
    QVERIFY2(drag(waypoint), qPrintable(dragError));
    finished.clear();
    QVERIFY(acceptDialog());
    QTRY_VERIFY_WITH_TIMEOUT(!finished.isEmpty(), TestTimeout::longMs());
    QVERIFY2(finished.last()[0].toBool(), qPrintable(finished.last()[1].toString()));
    QCOMPARE(transfers.count(), 2);
    QCOMPARE(vehicle->flightMode(), vehicle->missionFlightMode());
    QVERIFY(positionUploaded(waypoint));
    vehicle->pauseVehicle();
    QTRY_COMPARE_WITH_TIMEOUT(vehicle->flightMode(), vehicle->pauseFlightMode(), TestTimeout::mediumMs());
    QVERIFY2(drag(landing), qPrintable(dragError));
    QVERIFY(rejectDialog());
    QCOMPARE(transfers.count(), 2);
    QCOMPARE(vehicle->flightMode(), vehicle->pauseFlightMode());
    QVERIFY2(drag(landing), qPrintable(dragError));
    finished.clear();
    QVERIFY(acceptDialog());
    QTRY_VERIFY_WITH_TIMEOUT(!finished.isEmpty(), TestTimeout::longMs());
    QVERIFY2(finished.last()[0].toBool(), qPrintable(finished.last()[1].toString()));
    QCOMPARE(transfers.count(), 3);
    QCOMPARE(vehicle->flightMode(), vehicle->missionFlightMode());
    QCOMPARE(manager->currentIndex(), previousTarget);
    QCOMPARE(simulation->remainingLoops(), previousLoops);
    QVERIFY(positionUploaded(landing));
    vehicle->pauseVehicle();
    QTRY_COMPARE_WITH_TIMEOUT(vehicle->flightMode(), vehicle->pauseFlightMode(), TestTimeout::mediumMs());
    QVERIFY2(drag(landing), qPrintable(dragError));
    finished.clear();
    QVERIFY(acceptDialog());
    QTRY_VERIFY_WITH_TIMEOUT(!finished.isEmpty(), TestTimeout::longMs());
    QVERIFY2(finished.last()[0].toBool(), qPrintable(finished.last()[1].toString()));
    QCOMPARE(vehicle->flightMode(), vehicle->missionFlightMode());
    QVERIFY(positionUploaded(landing));
    QVERIFY(takeoffDraft->setProperty("text", QStringLiteral("2")));
    QVERIFY(globalSettings->setProperty("simulationInputsEdited", false));
    waypoint->setCoordinate(QGCGeo::geodesicDestination(waypoint->coordinate(), 90, 5));
    finished.clear();
    pausedDuringTransfer = false;
    QVERIFY(QMetaObject::invokeMethod(master, "upload"));
    QVERIFY(waitForDialog(QCoreApplication::translate("CaacRouteEditor", "Update waypoints")));
    QVERIFY(acceptDialog());
    QTRY_VERIFY_WITH_TIMEOUT(!finished.isEmpty(), TestTimeout::longMs());
    QVERIFY2(finished.last()[0].toBool(), qPrintable(finished.last()[1].toString()));
    QVERIFY(pausedDuringTransfer);
    QCOMPARE(vehicle->flightMode(), vehicle->missionFlightMode());
    QCOMPARE(manager->currentIndex(), previousTarget);
    QCOMPARE(simulation->remainingLoops(), previousLoops);
    QVERIFY(positionUploaded(waypoint));
    QVERIFY(master->missionController()->uploadConfirmed());
    auto* link = qobject_cast<MockLink*>(vehicle->vehicleLinkManager()->primaryLink().lock().get());
    QVERIFY(link);
    expectLogMessage("API.QGCApplication.AppMessage", QtDebugMsg,
                     QRegularExpression(QStringLiteral("任务传输失败|Mission transfer failed")));
    link->setMissionItemFailureMode(MockLinkMissionItemHandler::FailWriteFinalAckErrorAck, MAV_MISSION_ERROR);
    finished.clear();
    QVERIFY2(drag(waypoint), qPrintable(dragError));
    QVERIFY(acceptDialog());
    QTRY_VERIFY_WITH_TIMEOUT(!finished.isEmpty(), TestTimeout::longMs());
    QVERIFY(!finished.last()[0].toBool());
    verifyExpectedLogMessage();
    QVERIFY(vehicle->flightMode() != vehicle->missionFlightMode());
    const auto dismissDialog = [&](const QString& title) {
        std::function<QQuickItem*(QQuickItem*)> findAccept = [&](QQuickItem* item) -> QQuickItem* {
            if (!item || !item->isVisible()) {
                return nullptr;
            }
            if (item->objectName() == QStringLiteral("popupDialog_title") &&
                item->property("text").toString() == title) {
                return findVisibleItem(item->parentItem(), QStringLiteral("popupDialog_acceptButton"), 0);
            }
            for (auto* child : item->childItems()) {
                if (auto* button = findAccept(child)) {
                    return button;
                }
            }
            return nullptr;
        };
        if (!waitForDialog(title)) {
            return false;
        }
        auto* button = findAccept(_rootItem);
        if (!button) {
            return false;
        }
        QTest::mouseClick(_window, Qt::LeftButton, Qt::NoModifier,
                          button->mapToScene(QPointF(button->width() / 2, button->height() / 2)).toPoint());
        return waitForCondition([&]() { return !dialogVisible(title); }, TestTimeout::mediumMs(),
                                QStringLiteral("Upload error acknowledged"));
    };
    QVERIFY(dismissDialog(QCoreApplication::translate("CaacRouteEditor", "Update waypoints")));
    QVERIFY(dismissDialog(QCoreApplication::translate("QGCApplication", "Notice")));
    link->setMissionItemFailureMode(MockLinkMissionItemHandler::FailNone, MAV_MISSION_ACCEPTED);
    finished.clear();
    QVERIFY2(drag(waypoint), qPrintable(dragError));
    QVERIFY(acceptDialog());
    QTRY_VERIFY_WITH_TIMEOUT(!finished.isEmpty(), TestTimeout::longMs());
    QVERIFY2(finished.last()[0].toBool(), qPrintable(finished.last()[1].toString()));
    QCOMPARE(vehicle->flightMode(), vehicle->missionFlightMode());
    QVERIFY(positionUploaded(waypoint));
    QCOMPARE(manager->currentIndex(), previousTarget);
    QCOMPARE(simulation->remainingLoops(), previousLoops);
}

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
    QTRY_VERIFY_WITH_TIMEOUT(MultiVehicleManager::instance()->activeVehicle() == vehicle, TestTimeout::mediumMs());
    QSignalSpy rawChannels(joystick, &Joystick::rawChannelValuesChanged);
    QVERIFY(rawChannels.isValid());
    manager->setActiveJoystickEnabledForActiveVehicle(true);
    QTRY_VERIFY_WITH_TIMEOUT(manager->activeJoystickEnabledForActiveVehicle(), TestTimeout::mediumMs());
    QTRY_VERIFY_WITH_TIMEOUT(joystick->isRunning(), TestTimeout::mediumMs());
    QTRY_VERIFY_WITH_TIMEOUT(!rawChannels.isEmpty(), TestTimeout::mediumMs());
    QTRY_COMPARE_WITH_TIMEOUT(simulation->joystickChannels().value(4).toInt(), Joystick::AxisMax,
                              TestTimeout::mediumMs());
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
    auto* modeEnabled = findVisibleItemScrolled(QStringLiteral("caacSimulationModeSwitchEnabled"),
                                                QStringLiteral("popupDialog_contentFlickable"));
    QVERIFY(modeEnabled);
    QTRY_VERIFY_WITH_TIMEOUT(modeEnabled->isEnabled(), TestTimeout::shortMs());
    settings->simulationModeSwitchAxis()->setRawValue(0);
    QTRY_VERIFY_WITH_TIMEOUT(!modeEnabled->isEnabled(), TestTimeout::shortMs());
    QVERIFY(simulation->modeSwitchUnavailableReason().contains(QStringLiteral("CH1")));
    joystick->setFunctionForChannel(Calibration::stickFunctionRoll, 4);
    QTRY_VERIFY_WITH_TIMEOUT(modeEnabled->isEnabled(), TestTimeout::shortMs());
    settings->calibrated()->setRawValue(false);
    QTRY_VERIFY_WITH_TIMEOUT(!modeEnabled->isEnabled(), TestTimeout::shortMs());
    settings->calibrated()->setRawValue(true);
    QTRY_VERIFY_WITH_TIMEOUT(modeEnabled->isEnabled(), TestTimeout::shortMs());
    settings->simulationModeSwitchAxis()->setRawValue(4);
    joystick->setFunctionForChannel(Calibration::stickFunctionRoll, 0);
    manager->setActiveJoystickEnabledForActiveVehicle(false);
    QVERIFY(clickButton(QStringLiteral("caacSimulationModeSwitchEnabled")));
    QVERIFY(!settings->simulationModeSwitchEnabled()->rawValue().toBool());
    QVERIFY(clickButton(QStringLiteral("caacSimulationModeSwitchEnabled")));
    QVERIFY(settings->simulationModeSwitchEnabled()->rawValue().toBool());
    QVERIFY(manager->activeJoystickEnabledForActiveVehicle());
    QVERIFY(rejectDialog());
    QTRY_COMPARE_WITH_TIMEOUT(simulation->modeSwitchPosition(), 2, TestTimeout::mediumMs());
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
    QTRY_COMPARE_WITH_TIMEOUT(
        simulation->status(),
        QCoreApplication::translate("CaacSimulationController", "Simulation controlled by USB joystick"),
        TestTimeout::mediumMs());
    QVERIFY(clickButton(QStringLiteral("caacSimulationManagement")));
    auto* takeover = findVisibleItem(_rootItem, QStringLiteral("caacEmergencyManualControl"));
    QVERIFY(takeover);
    const auto takeoverText =
        QCoreApplication::translate("CaacSimulationEmergencyDialog", "Attitude mode / joystick takeover");
    const auto releaseText =
        QCoreApplication::translate("CaacSimulationEmergencyDialog", "End manual control and hover");
    QTRY_COMPARE_WITH_TIMEOUT(takeover->property("text").toString(), releaseText, TestTimeout::mediumMs());
    QVERIFY(input->setAxis(4, Joystick::AxisMin));
    QTRY_COMPARE_WITH_TIMEOUT(vehicle->flightMode(), vehicle->rtlFlightMode(), TestTimeout::mediumMs());
    QVERIFY(!simulation->manualControlActive());
    QTRY_COMPARE_WITH_TIMEOUT(takeover->property("text").toString(), takeoverText, TestTimeout::mediumMs());
    QVERIFY(input->setAxis(4, 0));
    QTRY_VERIFY_WITH_TIMEOUT(simulation->manualControlActive(), TestTimeout::mediumMs());
    QTRY_COMPARE_WITH_TIMEOUT(takeover->property("text").toString(), releaseText, TestTimeout::mediumMs());
    QVERIFY(rejectDialog());
    manager->setActiveJoystickEnabledForActiveVehicle(false);
    QTRY_COMPARE_WITH_TIMEOUT(vehicle->groundSpeed()->rawValue().toDouble(), 0.0, TestTimeout::mediumMs());
    QVERIFY(simulation->manualControlActive());
    QTRY_VERIFY_WITH_TIMEOUT(simulation->status().contains(QCoreApplication::translate(
                                 "CaacSimulationController", "Joystick input lost; simulation hovering.")),
                             TestTimeout::mediumMs());
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

void CaacSimulationFlowUITest::_fourLoopsFromUnsavedSettings()
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
    const auto selectedStartCoordinate = mission->visualItems()->value<SimpleMissionItem*>(1)->coordinate();
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
    for (const auto& choice : {qMakePair("caacSimLoopMode", 1), qMakePair("caacSimStartPoint", 1),
                               qMakePair("caacSimEndBehavior", 2), qMakePair("caacSimClimbMode", 0)}) {
        auto* combo = findVisibleItemScrolled(QLatin1String(choice.first), scroller);
        QVERIFY(combo && combo->setProperty("currentIndex", choice.second));
    }
    auto* ending = findVisibleItemScrolled(QStringLiteral("caacSimEndBehavior"), scroller);
    QVERIFY(ending);
    QCOMPARE(ending->property("count").toInt(), 3);
    QCOMPARE(ending->property("currentText").toString(),
             QCoreApplication::translate("CaacSimulationSettings", "Return to takeoff point and hover"));
    QVERIFY(!findVisibleItem(_rootItem, QStringLiteral("caacSimReturnPoint")));
    QVERIFY(clickButtonScrolled(QStringLiteral("caacSimLoopCount"), scroller));
    QTest::keyClick(_window, Qt::Key_A, Qt::ControlModifier);
    QTest::keyClick(_window, Qt::Key_4);
    QTest::keyClick(_window, Qt::Key_Tab);
    QVERIFY(clickButtonScrolled(QStringLiteral("caacSimVerticalSpeed"), scroller));
    QTest::keyClick(_window, Qt::Key_A, Qt::ControlModifier);
    QTest::keyClick(_window, Qt::Key_2);
    QTest::keyClick(_window, Qt::Key_Tab);
    QCOMPARE(editor->simulationSettings().value("loopCount").toInt(), 1);
    auto* startChoice = findVisibleItemScrolled(QStringLiteral("caacSimStartPoint"), scroller);
    QVERIFY(startChoice);
    QCOMPARE(startChoice->property("currentIndex").toInt(), 1);
    QVERIFY(clickButton(QStringLiteral("caacWaypointListButton")));
    QSignalSpy upload(simulation, &CaacSimulationController::missionUploaded);
    QVERIFY(clickButton(QStringLiteral("caacUploadWaypoints")));
    QTRY_VERIFY_WITH_TIMEOUT(!upload.isEmpty(), TestTimeout::longMs());
    std::fprintf(stderr, "CAAC four-loop step: settings uploaded\n");
    QCOMPARE(simulation->remainingLoops(), 4);
    QCOMPARE(editor->simulationSettings().value("loopCount").toInt(), 4);
    QVERIFY(editor->simulationSettings().value("closed").toBool());
    QVERIFY(findVisibleItem(_rootItem, QStringLiteral("caacSimulationPreview_closure")));
    const int selectedStartSequence = editor->simulationSettings().value("startSequence").toInt();
    QVERIFY(selectedStartSequence > 0);
    SimpleMissionItem* selectedStart = nullptr;
    for (int i = 1; i < mission->visualItems()->count(); ++i) {
        auto* item = mission->visualItems()->value<SimpleMissionItem*>(i);
        if (item && item->sequenceNumber() == selectedStartSequence) {
            selectedStart = item;
            break;
        }
    }
    QVERIFY2(selectedStart && selectedStart->command() == MAV_CMD_NAV_WAYPOINT,
             "Saving the mission-speed header must retain the selected start waypoint");
    QVERIFY(QGCGeo::geodesicDistance(selectedStart->coordinate(), selectedStartCoordinate) < 0.02);
    QCOMPARE(editor->simulationSettings().value("endBehavior").toInt(), 2);
    QVERIFY(findVisibleItem(_rootItem, QStringLiteral("caacSimulationPreview_return")));
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
    QTRY_COMPARE_WITH_TIMEOUT(arrivals, QList<int>({5, 4, 4, 4}), TestTimeout::mediumMs());
    std::fprintf(stderr, "CAAC four-loop step: all four loops completed\n");
    QCOMPARE(simulation->remainingLoops(), 0);
    QTRY_VERIFY_WITH_TIMEOUT(QGCGeo::geodesicDistance(vehicle->coordinate(), home) < 0.2, TestTimeout::mediumMs());
    QVERIFY(vehicle->airborne());
    QTRY_COMPARE_WITH_TIMEOUT(vehicle->altitudeRelative()->rawValue().toDouble(), 6.0, TestTimeout::mediumMs());
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

void CaacDefaultHomeUITest::_defaultStartPosition()
{
    QSettings settings;
    settings.setValue(QStringLiteral("FlightMapPosition/Latitude"), 30.0);
    settings.setValue(QStringLiteral("FlightMapPosition/Longitude"), 120.0);
    settings.setValue(QStringLiteral("FlightMapPosition/FlightMapZoom"), 7.0);
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
    const QGeoCoordinate expected(25.137904838950554, 102.72128132348416, 2062.0);
    QCOMPARE(mission->plannedHomePosition(), expected);
    QVERIFY(!master->dirtyForSave());
    auto* startupMap = findVisibleItem(_rootItem, QStringLiteral("planView_map"));
    QVERIFY(startupMap);
    QTRY_VERIFY_WITH_TIMEOUT(startupMap->property("mapReady").toBool(), TestTimeout::mediumMs());
    QVERIFY(QGCGeo::geodesicDistance(startupMap->property("center").value<QGeoCoordinate>(), expected) < 0.02);
    QCOMPARE(startupMap->property("zoomLevel").toDouble(), 19.0);
    auto* plugin = qobject_cast<CaacCorePlugin*>(QGCCorePlugin::instance());
    QVERIFY(plugin);
    auto* simulation = plugin->simulation();
    QVERIFY(simulation);
    const auto stop = qScopeGuard([simulation]() { simulation->stop(); });
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
        QCOMPARE(map->property("zoomLevel").toDouble(), 19.0);
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

void CaacSimulationFlowUITest::_closedRouteAndSimulationToggle()
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
        bool homeHover = false;
        bool plannedLanding = false;
        bool absoluteAltitude = false;
    };

    const QList<FlowCase> flowCases{{false, 1, 0, 1, -1},
                                    {false, 2, 1, 2, 3},
                                    {false, 2, 2, 1, -1},
                                    {true, 3, 0, 1, 0},
                                    {true, 1, 1, 1, -1},
                                    {true, 3, 1, 2, 1},
                                    {true, 2, 2, 1, -1},
                                    {false, 2, 0, 1, 1, true, true},
                                    {true, 3, 1, 2, 1, true, false, true},
                                    {false, 1, 2, 1, 2, true}};
    for (const auto& flow : flowCases) {
        auto* finalWaypoint = mission->visualItems()->value<SimpleMissionItem*>(flow.closed ? flow.start : 3);
        QVERIFY(finalWaypoint);
        if (flow.absoluteAltitude) {
            finalWaypoint->setAltitudeFrame(QGroundControlQmlGlobal::AltitudeFrameAbsolute);
            finalWaypoint->altitude()->setRawValue(27);
        }
        if (flow.plannedLanding) {
            auto* landing = qobject_cast<SimpleMissionItem*>(
                mission->insertSimpleMissionItem(home, mission->visualItems()->count(), false));
            QVERIFY(landing);
            landing->setCommand(MAV_CMD_NAV_LAND);
            landing->setAltitudeFrame(QGroundControlQmlGlobal::AltitudeFrameRelative);
            landing->altitude()->setRawValue(0);
        }
        const auto restoreRoute = qScopeGuard([&]() {
            if (flow.plannedLanding) {
                mission->removeVisualItem(mission->visualItems()->count() - 1);
            }
            if (flow.absoluteAltitude) {
                finalWaypoint->setAltitudeFrame(QGroundControlQmlGlobal::AltitudeFrameRelative);
                finalWaypoint->altitude()->setRawValue(2);
            }
        });
        auto variant = settings;
        variant["closed"] = flow.closed;
        variant["startSequence"] = flow.start;
        variant["loopMode"] = flow.loopMode;
        variant["loopCount"] = flow.cycles;
        variant["endBehavior"] = flow.homeHover ? 2 : flow.returnPoint < 0 ? 0 : 1;
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
        const auto returned =
            flow.homeHover || flow.returnPoint == 0 ? home : coordinates.at(qMax(flow.returnPoint - 1, 0));
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
        QCOMPARE(master->saveToJson(), beforePreview);
        if (flow.plannedLanding) {
            QCOMPARE(compiled.last().command, MAV_CMD_NAV_LAND);
            QCOMPARE(compiled.last().frame, MAV_FRAME_GLOBAL_RELATIVE_ALT);
            QCOMPARE(compiled.last().z, 0.0f);
            QVERIFY(QGCGeo::geodesicDistance(QGeoCoordinate(compiled.last().x / 1e7, compiled.last().y / 1e7), home) <
                    0.02);
        } else if (flow.homeHover) {
            QCOMPARE(compiled.last().command, MAV_CMD_NAV_LOITER_UNLIM);
            QCOMPARE(compiled.last().frame, flow.absoluteAltitude ? MAV_FRAME_GLOBAL : MAV_FRAME_GLOBAL_RELATIVE_ALT);
            for (const auto& item : compiled) {
                QVERIFY(item.command != MAV_CMD_NAV_LAND && item.command != MAV_CMD_NAV_RETURN_TO_LAUNCH);
            }
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
                if (!flow.homeHover && flow.returnPoint > 0 && flow.returnPoint != finalBodyPoint &&
                    i == flow.returnPoint - 1) {
                    ++expected;
                }
                QCOMPARE(visits.at(i), expected);
            }
            const int finalPoint = flow.plannedLanding || flow.homeHover ? 0
                                   : flow.returnPoint < 0 ? (flow.closed ? flow.start : coordinates.size())
                                                          : flow.returnPoint;
            QVERIFY(QGCGeo::geodesicDistance(engine.state().position,
                                             finalPoint == 0 ? home : coordinates.at(finalPoint - 1)) < 0.02);
            QCOMPARE(engine.state().airborne, !flow.plannedLanding && (flow.homeHover || finalPoint != 0));
            if (flow.plannedLanding) {
                QCOMPARE(engine.state().position.altitude(), home.altitude());
            } else if (flow.homeHover) {
                QCOMPARE(engine.state().position.altitude(), flow.absoluteAltitude ? 27.0 : 22.0);
                const auto position = engine.state().position;
                engine.advance(20);
                QCOMPARE(engine.state().position, position);
                QCOMPARE(engine.state().northSpeed, 0.0);
                QCOMPARE(engine.state().eastSpeed, 0.0);
                QCOMPARE(engine.state().climbSpeed, 0.0);
                QVERIFY(engine.state().airborne);
            }
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
    step("upload simulation");
    QVERIFY(clickButton(QStringLiteral("caacWaypointListButton")));
    QSignalSpy initialUpload(simulation, &CaacSimulationController::missionUploaded);
    QVERIFY(initialUpload.isValid());
    QVERIFY(clickButton(QStringLiteral("caacUploadWaypoints")));
    QTRY_VERIFY_WITH_TIMEOUT(!initialUpload.isEmpty(), TestTimeout::longMs());
    QVERIFY(simulation->missionReady());
    // The first upload commits the always-visible initial-speed setting to the plan.
    const auto original = master->saveToJson();
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
    QVERIFY(waitForDialog(QCoreApplication::translate("CaacRouteEditor", "Update waypoints")));
    QVERIFY(rejectDialog());
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
            SimpleMissionItem* sourceWaypoint = nullptr;
            int expected = -1;
            int waypointNumber = 0;
            for (int sourceIndex = 1; sourceIndex < mission->visualItems()->count(); ++sourceIndex) {
                auto* source = mission->visualItems()->value<SimpleMissionItem*>(sourceIndex);
                if (source && source->command() == MAV_CMD_NAV_WAYPOINT) {
                    ++waypointNumber;
                    if (QGCGeo::geodesicDistance(source->coordinate(), item->coordinate()) < 0.02) {
                        QVERIFY(!sourceWaypoint);
                        sourceWaypoint = source;
                        expected = waypointNumber;
                    }
                }
            }
            QVERIFY(sourceWaypoint);
            QCOMPARE(sourceWaypoint->waypointNumber(), expected);
            QCOMPARE(simulation->waypointNumber(item->sequenceNumber(), item->coordinate()), expected);
            const auto labelName = QStringLiteral("missionItemMapLabel_%1").arg(sourceWaypoint->sequenceNumber());
            QQuickItem* mapLabel = nullptr;
            QTRY_VERIFY2_WITH_TIMEOUT((mapLabel = findItem(qobject_cast<QQuickItem*>(map), labelName)),
                                      qPrintable(labelName), TestTimeout::mediumMs());
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
    vehicle->setFlightMode(vehicle->missionFlightMode());
    QTRY_COMPARE_WITH_TIMEOUT(vehicle->flightMode(), vehicle->missionFlightMode(), TestTimeout::mediumMs());
    QTRY_VERIFY_WITH_TIMEOUT(vehicle->groundSpeed()->rawValue().toDouble() > 1, TestTimeout::mediumMs());
    QVERIFY(!plugin->changeCurrentFlightAltitude(nullptr, 4).isEmpty());
    const auto missionBeforeLiveHeight = master->saveToJson();
    QSignalSpy currentHeightUploads(manager, &MissionManager::sendComplete);
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
            auto* currentHeightAction = findVisibleItemScrolled(QStringLiteral("caacChangeFlightAltitude"),
                                                                QStringLiteral("popupDialog_contentFlickable"));
            QVERIFY(currentHeightAction && currentHeightAction->isEnabled());
            if (unit == UnitsSettings::VerticalDistanceUnitsMeters) {
                QCOMPARE(vehicle->flightMode(), vehicle->missionFlightMode());
                QQmlExpression legacyHeightAllowed(qmlContext(currentHeightAction), currentHeightAction,
                                                   QStringLiteral("globals.guidedControllerFlyView.showChangeAlt"));
                QVERIFY(!legacyHeightAllowed.evaluate().toBool());
                QVERIFY(!legacyHeightAllowed.hasError());
            }
            QVERIFY(clickButtonScrolled(QStringLiteral("caacChangeFlightAltitude"),
                                        QStringLiteral("popupDialog_contentFlickable")));
            auto* liveAltitude = findVisibleItem(_rootItem, QStringLiteral("caacFlightWaypointAltitude"));
            QVERIFY(liveAltitude);
            QVERIFY(!findVisibleItem(_rootItem, QStringLiteral("caacFlightWaypointChoice"), 0));
            mock->clearReceivedMavCommandCounts();
            QVERIFY(liveAltitude->setProperty("text", QStringLiteral("0")));
            QVERIFY(acceptDialog());
            QVERIFY(dialogVisible(QCoreApplication::translate("CaacMissionActionDialog", "Change altitude")));
            QCOMPARE(mock->receivedMavCommandCount(MAV_CMD_DO_REPOSITION), 0);
            QVERIFY(liveAltitude->setProperty("text", QString::number(targetMeters)));
            const int targetBeforeHeight = manager->currentIndex();
            const int loopsBeforeHeight = simulation->remainingLoops();
            QSignalSpy altitudeAcknowledged(vehicle, &Vehicle::mavCommandResult);
            QVERIFY(acceptDialog());
            QVERIFY(!findVisibleItem(_rootItem, QStringLiteral("guidedActionConfirmButton"), 0));
            step("live height command sent");
            QTRY_VERIFY_WITH_TIMEOUT(mock->receivedMavCommandCount(MAV_CMD_DO_REPOSITION) > 0, TestTimeout::mediumMs());
            const auto acceptedHeight = [&altitudeAcknowledged]() {
                for (const auto& result : altitudeAcknowledged) {
                    if (result[2].toInt() == MAV_CMD_DO_REPOSITION && result[3].toInt() == MAV_RESULT_ACCEPTED) {
                        return true;
                    }
                }
                return false;
            };
            QTRY_VERIFY_WITH_TIMEOUT(acceptedHeight(), TestTimeout::mediumMs());
            QTRY_VERIFY_WITH_TIMEOUT(std::abs(vehicle->altitudeRelative()->rawValue().toDouble() - targetMeters) < 0.1,
                                     TestTimeout::longMs());
            QTRY_VERIFY_WITH_TIMEOUT(!findVisibleItem(_rootItem, QStringLiteral("caacFlightWaypointAltitude"), 0),
                                     TestTimeout::mediumMs());
            QCOMPARE(vehicle->flightMode(), vehicle->pauseFlightMode());
            QCOMPARE(manager->currentIndex(), targetBeforeHeight);
            QCOMPARE(simulation->remainingLoops(), loopsBeforeHeight);
            QCOMPARE(currentHeightUploads.count(), 0);
            QCOMPARE(master->saveToJson(), missionBeforeLiveHeight);
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
UT_REGISTER_TEST(CaacSimulationFlowUITest, TestLabel::Integration, TestLabel::Vehicle)
UT_REGISTER_TEST(CaacDefaultHomeUITest, TestLabel::Integration, TestLabel::Vehicle)
UT_REGISTER_TEST(CaacMissionUpdateUITest, TestLabel::Integration, TestLabel::Vehicle)
#include "CaacSimulationUITest.moc"
