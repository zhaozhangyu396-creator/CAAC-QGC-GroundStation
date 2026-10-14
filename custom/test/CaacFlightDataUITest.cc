#include <QtCore/QRegularExpression>
#include <QtCore/QScopeGuard>
#include <QtGui/QGuiApplication>
#include <QtGui/QImage>
#include <QtGui/QWheelEvent>
#include <QtQml/QJSValue>
#include <QtQuick/QQuickItem>
#include <QtQuick/QQuickWindow>
#include <QtTest/QSignalSpy>

#include "CaacCorePlugin.h"
#include "CaacFlightDistance.h"
#include "Fact.h"
#include "MissionController.h"
#include "MockLink.h"
#include "ParameterManager.h"
#include "PlanMasterController.h"
#include "QGCGeo.h"
#include "QmlUITestBase.h"
#include "TrajectoryPoints.h"
#include "Vehicle.h"

class CaacFlightDataUITest : public QmlUITestBase
{
    Q_OBJECT

private slots:
    void _liveFlightData();
    void _dashboardInteractions();
};

void CaacFlightDataUITest::_dashboardInteractions()
{
    startUI();
    if (QTest::currentTestFailed()) {
        return;
    }
    Vehicle* vehicle = nullptr;
    auto mock = connectMockLinkAndWaitReady([]() { return MockLink::startPX4MockLink(); }, vehicle);
    QVERIFY(mock && vehicle);
    auto* dashboard = findVisibleItem(_rootItem, QStringLiteral("caacFlightDashboard"));
    auto* footer = findVisibleItem(_rootItem, QStringLiteral("caacFlightTelemetryBar"));
    auto* arrow = findVisibleItem(_rootItem, QStringLiteral("caacTelemetryHeadingArrow"));
    QVERIFY(dashboard && footer && arrow);
    const auto previousHeading = vehicle->heading()->rawValue();
    vehicle->heading()->setRawValue(123.4);
    QTRY_VERIFY_WITH_TIMEOUT(std::abs(arrow->property("headingAngle").toDouble() - 123.4) < 1e-6,
                             TestTimeout::mediumMs());
    auto* heading = findVisibleItem(_rootItem, QStringLiteral("caacDashboardHeadingValue"));
    QVERIFY(heading);
    QVERIFY(heading->property("text").toString().startsWith(vehicle->heading()->cookedValueString()));
    vehicle->heading()->setRawValue(previousHeading);
    for (const auto* key : {"Altitude", "Speed", "GPS", "Pitch", "Voltage", "HomeDistance", "Roll"}) {
        QVERIFY(
            findVisibleItem(_rootItem, QStringLiteral("caacDashboard") + QLatin1String(key) + QStringLiteral("Value")));
    }
    const auto beforeWidth = dashboard->width();
    QVERIFY(clickButton(QStringLiteral("caacDashboardZoomButton")));
    QTRY_VERIFY_WITH_TIMEOUT(dashboard->width() > beforeWidth, TestTimeout::mediumMs());
    QVERIFY(clickButton(QStringLiteral("caacDashboardZoomButton")));
    QTRY_VERIFY_WITH_TIMEOUT(std::abs(dashboard->width() - beforeWidth) < 1, TestTimeout::mediumMs());
    auto* drag = findVisibleItem(_rootItem, QStringLiteral("caacDashboardDragArea"));
    QVERIFY(drag);
    const auto press = drag->mapToScene(QPointF(drag->width() / 2, drag->height() / 3)).toPoint();
    const auto before = dashboard->mapToScene(QPointF());
    QTest::mousePress(_window, Qt::LeftButton, Qt::NoModifier, press);
    for (int i = 1; i <= 4; ++i) {
        QTest::mouseMove(_window, press - QPoint(i * 15, i * 8));
    }
    QTest::mouseRelease(_window, Qt::LeftButton, Qt::NoModifier, press - QPoint(60, 32));
    QTRY_VERIFY_WITH_TIMEOUT(dashboard->mapToScene(QPointF()).x() < before.x() - 20, TestTimeout::mediumMs());
    QTRY_VERIFY_WITH_TIMEOUT(dashboard->mapToScene(QPointF()).y() < before.y() - 10, TestTimeout::mediumMs());
    const auto wheelPosition = drag->mapToScene(QPointF(drag->width() / 2, drag->height() / 3));
    QTest::mouseMove(_window, wheelPosition.toPoint());
    QWheelEvent wheel(wheelPosition, _window->mapToGlobal(wheelPosition.toPoint()), QPoint(), QPoint(0, 120),
                      Qt::NoButton, Qt::NoModifier, Qt::NoScrollPhase, false);
    QCoreApplication::sendEvent(_window, &wheel);
    QTRY_VERIFY_WITH_TIMEOUT(dashboard->property("sizeFactor").toDouble() > 1, TestTimeout::mediumMs());
    if (const auto screenshot = qgetenv("CAAC_DASHBOARD_UI_SCREENSHOT"); !screenshot.isEmpty()) {
        QVERIFY(_window->grabWindow().save(QString::fromUtf8(screenshot)));
    }
    disconnectMockLink(mock);
}

void CaacFlightDataUITest::_liveFlightData()
{
    startUI();
    if (QTest::currentTestFailed()) {
        return;
    }
    QVERIFY(!findVisibleItem(_rootItem, QStringLiteral("caacCenterVehicleButton"), 0));
    QVERIFY(verifyText(QStringLiteral("caacTopFlightTime"), QStringLiteral("00:00:00"), QStringLiteral("no aircraft")));
    QVERIFY(verifyText(QStringLiteral("caacConnectionStatusButton"), QStringLiteral("未连接"),
                       QStringLiteral("no aircraft")));
    QVERIFY(clickButton(QStringLiteral("caacMissionDetailsButton")));
    QVERIFY(verifyVisibility(QStringLiteral("caacMissionDetailsPanel"), true, QStringLiteral("mission panel")));
    QVERIFY(verifyText(QStringLiteral("caacMissionTarget"),
                       QCoreApplication::translate("CaacMissionDetails", "Unavailable"),
                       QStringLiteral("no target without aircraft")));
    QVERIFY(clickButton(QStringLiteral("caacFlightDataButton")));
    QVERIFY(verifyVisibility(QStringLiteral("caacMissionDetailsPanel"), false, QStringLiteral("mission panel closed")));
    QVERIFY(verifyVisibility(QStringLiteral("caacFlyDataPanel"), true, QStringLiteral("data panel opened")));
    QVERIFY(clickButton(QStringLiteral("caacMissionDetailsButton")));
    QVERIFY(verifyVisibility(QStringLiteral("caacFlyDataPanel"), false, QStringLiteral("data panel closed")));
    QVERIFY(clickButton(QStringLiteral("caacMissionDetailsButton")));
    QVERIFY(verifyVisibility(QStringLiteral("caacMissionDetailsPanel"), false, QStringLiteral("toggle current panel")));
    QVERIFY(clickButton(QStringLiteral("caacMapSettingsButton")));
    QVERIFY(verifyVisibility(QStringLiteral("caacMissionDetailsPanel"), false, QStringLiteral("only map panel")));
    QVERIFY(verifyVisibility(QStringLiteral("caacFlyDataPanel"), false, QStringLiteral("only map panel")));
    auto* mapSettings = findVisibleItem(_rootItem, QStringLiteral("caacMapSettingsPanel"));
    QVERIFY(mapSettings);
    auto* addCombo = findVisibleItem(_rootItem, QStringLiteral("caacMapDoubleClickAdd"));
    QVERIFY(addCombo);
    QVERIFY(scrollIntoView(addCombo, QStringLiteral("caacFlyPanelsScroller")));
    if (QGuiApplication::platformName() != QStringLiteral("offscreen")) {
        _window->requestActivate();
        QVERIFY(QTest::qWaitForWindowActive(_window));
    }
    QVERIFY(clickButton(QStringLiteral("caacMapDoubleClickAdd")));
    if (const auto screenshot = qgetenv("CAAC_COMBO_UI_SCREENSHOT"); !screenshot.isEmpty()) {
        QVERIFY(_window->grabWindow().save(QString::fromUtf8(screenshot)));
    }
    QTest::keyClick(_window, Qt::Key_Home);
    QTest::keyClick(_window, Qt::Key_Return);
    auto* preferences = qobject_cast<CaacCorePlugin*>(QGCCorePlugin::instance());
    QVERIFY(preferences);
    const auto restorePreferences = qScopeGuard([preferences]() { preferences->setDoubleClickAddWaypoint(true); });
    QTRY_VERIFY_WITH_TIMEOUT(!preferences->doubleClickAddWaypoint(), TestTimeout::mediumMs());
    preferences->setDoubleClickAddWaypoint(true);
    QVERIFY(clickButton(QStringLiteral("caacMapSettingsButton")));
    QVERIFY(clickButton(QStringLiteral("caacFlightDataButton")));
    auto* panel = findVisibleItem(_rootItem, QStringLiteral("caacFlyDataPanel"));
    QVERIFY(panel);
    auto* handle = findVisibleItem(_rootItem, QStringLiteral("caacFlightDataDragHandle"));
    QVERIFY(handle);
    QVERIFY(scrollIntoView(handle, QStringLiteral("caacFlyPanelsScroller")));
    const auto beforeDrag = handle->mapToScene(QPointF(0, 0));
    const int dragDirection = beforeDrag.x() < _rootItem->width() / 2 ? 1 : -1;
    const auto press = handle->mapToScene(QPointF(handle->width() / 2, handle->height() / 2)).toPoint();
    QTest::mousePress(_window, Qt::LeftButton, Qt::NoModifier, press);
    for (int i = 1; i <= 4; ++i) {
        QTest::mouseMove(_window, press + QPoint(dragDirection * i * 10, 0));
    }
    QTest::mouseRelease(_window, Qt::LeftButton, Qt::NoModifier, press + QPoint(dragDirection * 40, 0));
    QTRY_VERIFY_WITH_TIMEOUT(dragDirection * (handle->mapToScene(QPointF(0, 0)).x() - beforeDrag.x()) > 10,
                             TestTimeout::mediumMs());
    auto* pagination = findVisibleItem(_rootItem, QStringLiteral("caacFlightDataPagination"));
    QVERIFY(pagination);
    QVERIFY(scrollIntoView(pagination, QStringLiteral("caacFlyPanelsScroller")));
    QVERIFY(clickItemFraction(QStringLiteral("caacFlightDataPagination"), 0.85, 0.5));
    QVERIFY(verifyProperty(QStringLiteral("caacFlyDataPanel"), "currentPage", 2, QStringLiteral("third page dot")));
    const auto unavailable = QCoreApplication::translate("CaacFlyDataPanel", "Unavailable");
    QVERIFY(verifyText(QStringLiteral("caacNorthDistanceValue"), unavailable, QStringLiteral("no telemetry")));

    ignoreLogMessage("Utilities.QGCSensors", QtWarningMsg,
                     QRegularExpression(QStringLiteral("Failed to connect to pressure backend")));
    ignoreLogMessage("Utilities.QGCSensors", QtWarningMsg,
                     QRegularExpression(QStringLiteral("Error Initializing Pressure Sensor")));
    Vehicle* vehicle = nullptr;
    auto mock = connectMockLinkAndWaitReady([]() { return MockLink::startPX4MockLink(); }, vehicle);
    if (!mock || !vehicle) {
        return;
    }
    const auto disconnect = qScopeGuard([&]() { disconnectMockLink(mock); });
    QVERIFY(verifyText(QStringLiteral("caacConnectionStatusButton"), QStringLiteral("已连接"),
                       QStringLiteral("aircraft connected")));
    {
        auto* flightTime = vehicle->getFact(QStringLiteral("flightTime"));
        QVERIFY(flightTime);
        const auto initialTime = flightTime->rawValue();
        const auto restoreTime = qScopeGuard([&]() { flightTime->setRawValue(initialTime); });
        flightTime->setRawValue(3661);
        auto* timeLabel = findVisibleItem(_rootItem, QStringLiteral("caacTopFlightTime"));
        QVERIFY(timeLabel);
        QTRY_COMPARE_WITH_TIMEOUT(timeLabel->property("text").toString(), QStringLiteral("01:01:01"),
                                  TestTimeout::mediumMs());
    }
    {
        const QStringList expectedModes{QStringLiteral("手动模式"), QStringLiteral("姿态模式"),
                                        QStringLiteral("手动特技"), QStringLiteral("定高模式"),
                                        QStringLiteral("定点模式"), QStringLiteral("保持模式"),
                                        QStringLiteral("任务模式"), QStringLiteral("返航模式")};
        QTRY_COMPARE_WITH_TIMEOUT(vehicle->flightModes(), expectedModes, TestTimeout::longMs());
        QVERIFY(clickButton(QStringLiteral("toolbar_flightModeIndicator")));
        QVERIFY(clickButton(QStringLiteral("indicatorDrawerExpandButton")));
        auto* geofence = findVisibleItem(_rootItem, QStringLiteral("caacGeofenceAction"));
        QVERIFY(geofence);
        QVariant actionLabels = geofence->property("model");
        if (actionLabels.canConvert<QJSValue>()) {
            actionLabels = actionLabels.value<QJSValue>().toVariant();
        }
        QCOMPARE(
            actionLabels.toStringList(),
            QStringList({QStringLiteral("无行为"), QStringLiteral("警告"), QStringLiteral("保持悬停"),
                         QStringLiteral("返航"), QStringLiteral("飞行中止（停止动力输出）"), QStringLiteral("降落")}));
        auto* geofenceFact = geofence->property("fact").value<Fact*>();
        QVERIFY(geofenceFact);
        const auto savedGeofenceAction = geofenceFact->rawValue();
        const auto restoreGeofenceAction =
            qScopeGuard([geofenceFact, savedGeofenceAction] { geofenceFact->setRawValue(savedGeofenceAction); });
        for (int i = 0; i < 6; ++i) {
            QCOMPARE(geofenceFact->enumValues().at(i).toInt(), i);
            QVERIFY(QMetaObject::invokeMethod(geofence, "activated", Q_ARG(int, i)));
            QTRY_COMPARE_WITH_TIMEOUT(geofenceFact->rawValue().toInt(), i, TestTimeout::mediumMs());
            QTRY_VERIFY_WITH_TIMEOUT(!vehicle->parameterManager()->pendingWrites(), TestTimeout::mediumMs());
        }
        geofenceFact->setRawValue(savedGeofenceAction);
        QTRY_VERIFY_WITH_TIMEOUT(!vehicle->parameterManager()->pendingWrites(), TestTimeout::mediumMs());
        auto* drawerLoader = findItem(_rootItem, QStringLiteral("indicatorDrawerLoader"));
        QVERIFY(drawerLoader);
        QTest::keyClick(_window, Qt::Key_Escape);
        QTRY_VERIFY_WITH_TIMEOUT(!drawerLoader->property("item").value<QObject*>(), TestTimeout::mediumMs());
    }
    auto* plugin = qobject_cast<CaacCorePlugin*>(QGCCorePlugin::instance());
    QVERIFY(plugin);
    auto* distance = plugin->flightDistance();
    QVERIFY(distance);
    QVERIFY(
        verifyText(QStringLiteral("caacNorthDistanceValue"), QStringLiteral("0.0"), QStringLiteral("before takeoff")));
    vehicle->sendMavCommand(vehicle->defaultComponentId(), MAV_CMD_NAV_TAKEOFF, false, 0, 0, 0, 0, 0, 0, 10);
    QTRY_VERIFY_WITH_TIMEOUT(vehicle->airborne(), TestTimeout::longMs());
    const auto start = vehicle->coordinate();
    QVERIFY(start.isValid());
    vehicle->coordinateChanged(start);
    vehicle->coordinateChanged(QGCGeo::geodesicDestination(start, 45, 100));
    QVERIFY(verifyText(QStringLiteral("caacNorthDistanceValue"), QStringLiteral("70.7"),
                       QStringLiteral("north component")));
    QVERIFY(
        verifyText(QStringLiteral("caacEastDistanceValue"), QStringLiteral("70.7"), QStringLiteral("east component")));
    const auto screenshot = qEnvironmentVariable("CAAC_FLIGHT_UI_SCREENSHOT");
    if (!screenshot.isEmpty()) {
        QSignalSpy frame(_window, &QQuickWindow::frameSwapped);
        QVERIFY(frame.isValid());
        _window->update();
        QVERIFY(frame.wait(TestTimeout::mediumMs()));
        QVERIFY(_window->grabWindow().save(screenshot));
    }
    QVERIFY(scrollIntoView(pagination, QStringLiteral("caacFlyPanelsScroller")));
    QVERIFY(clickItemFraction(QStringLiteral("caacFlightDataPagination"), 0.5, 0.5));
    QVERIFY(verifyProperty(QStringLiteral("caacFlyDataPanel"), "currentPage", 1, QStringLiteral("second page dot")));
    auto* log = findVisibleItem(_rootItem, QStringLiteral("caacFlightLog"));
    QVERIFY(log);
    QVERIFY(QMetaObject::invokeMethod(
        mock.data(),
        [mock]() {
            if (mock) {
                mock->sendStatusTextMessage(MAV_SEVERITY_INFO, QStringLiteral("Mission: 209 WP"));
            }
        },
        Qt::QueuedConnection));
    QTRY_VERIFY_WITH_TIMEOUT(log->property("text").toString().contains(QStringLiteral("Mission: 209 WP")),
                             TestTimeout::mediumMs());
    QVERIFY(
        QRegularExpression(QStringLiteral("\\d{2}:\\d{2}:\\d{2}")).match(log->property("text").toString()).hasMatch());
    QVERIFY(panel->setProperty("currentPage", 2));
    plugin->resetFlightDistanceForSimulationRestart(vehicle);
    QVERIFY(verifyText(QStringLiteral("caacNorthDistanceValue"), QStringLiteral("0.0"),
                       QStringLiteral("simulation reset API")));

    auto* layer = findVisibleItem(_rootItem, QStringLiteral("caacFlyLayer"));
    QVERIFY(layer);
    auto* map = layer->property("mapControl").value<QObject*>();
    QVERIFY(map);
    // Model a user pan so automatic following does not animate over the button's one-shot centering.
    QVERIFY(QMetaObject::invokeMethod(map, "mapPanStart"));
    QVERIFY(map->setProperty("center", QVariant::fromValue(QGeoCoordinate(-30, -120))));
    QVERIFY(QMetaObject::invokeMethod(map, "mapPanStop"));
    auto* center = findVisibleItem(_rootItem, QStringLiteral("caacHomeCenterButton"));
    QVERIFY(center && center->isEnabled());
    auto* master = map->property("planMasterController").value<PlanMasterController*>();
    QVERIFY(master);
    master->missionController()->setHomePosition(QGCGeo::geodesicDestination(vehicle->coordinate(), 90, 200));
    const auto homeBefore = master->missionController()->plannedHomePosition();
    const auto missionBefore = master->saveToJson();
    QVERIFY(homeBefore.isValid());
    QVERIFY(clickButton(QStringLiteral("caacHomeCenterButton")));
    QTRY_VERIFY_WITH_TIMEOUT(
        QGCGeo::geodesicDistance(map->property("center").value<QGeoCoordinate>(), homeBefore) < 0.01,
        TestTimeout::mediumMs());
    QCOMPARE(master->saveToJson(), missionBefore);
    QVERIFY(map->setProperty("center", QVariant::fromValue(QGeoCoordinate(-30, -120))));
    const auto centerPosition = center->mapToScene(QPointF(center->width() / 2, center->height() / 2)).toPoint();
    QTest::mouseClick(_window, Qt::LeftButton, Qt::NoModifier, centerPosition);
    QTest::mouseDClick(_window, Qt::LeftButton, Qt::NoModifier, centerPosition);
    QTRY_VERIFY_WITH_TIMEOUT(
        QGCGeo::geodesicDistance(map->property("center").value<QGeoCoordinate>(), vehicle->coordinate()) < 0.01,
        TestTimeout::mediumMs());
    QCOMPARE(master->missionController()->plannedHomePosition(), homeBefore);
    QCOMPARE(master->saveToJson(), missionBefore);
    const auto timers = center->findChildren<QObject*>(QStringLiteral("caacMapCenterSingleClickTimer"));
    QCOMPARE(timers.size(), 1);
    QVERIFY(!timers.first()->property("running").toBool());
    auto* trajectory = vehicle->property("trajectoryPoints").value<TrajectoryPoints*>();
    QVERIFY(trajectory);
    trajectory->start();
    QVERIFY(QMetaObject::invokeMethod(trajectory, "_vehicleCoordinateChanged", Qt::DirectConnection,
                                      Q_ARG(QGeoCoordinate, vehicle->coordinate())));
    QVERIFY(
        QMetaObject::invokeMethod(trajectory, "_vehicleCoordinateChanged", Qt::DirectConnection,
                                  Q_ARG(QGeoCoordinate, QGCGeo::geodesicDestination(vehicle->coordinate(), 45, 20))));
    trajectory->stop();
    QVERIFY(!trajectory->list().isEmpty());
    auto* mapTrajectory = findVisibleItem(_rootItem, QStringLiteral("caacFlightTrajectory"));
    QVERIFY(mapTrajectory);
    QSignalSpy cleared(trajectory, &TrajectoryPoints::pointsCleared);
    QVERIFY(clickButtonScrolled(QStringLiteral("caacClearTrajectoryButton"), QStringLiteral("caacMainToolsScroller")));
    QCOMPARE(cleared.count(), 1);
    QVERIFY(trajectory->list().isEmpty());
    QCOMPARE(master->saveToJson(), missionBefore);
    disconnectMockLink(mock);
    mock.clear();
    QVERIFY(verifyText(QStringLiteral("caacNorthDistanceValue"), unavailable, QStringLiteral("aircraft disconnected")));
    QVERIFY(!findVisibleItem(_rootItem, QStringLiteral("caacCenterVehicleButton"), 0));
    QVERIFY(verifyText(QStringLiteral("caacConnectionStatusButton"), QStringLiteral("未连接"),
                       QStringLiteral("aircraft disconnected")));
}

UT_REGISTER_TEST(CaacFlightDataUITest, TestLabel::Integration, TestLabel::Vehicle)

#include "CaacFlightDataUITest.moc"
