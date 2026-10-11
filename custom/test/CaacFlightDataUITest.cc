#include <QtCore/QRegularExpression>
#include <QtCore/QScopeGuard>
#include <QtGui/QImage>
#include <QtQuick/QQuickItem>
#include <QtQuick/QQuickWindow>
#include <QtTest/QSignalSpy>

#include "CaacCorePlugin.h"
#include "CaacFlightDistance.h"
#include "MockLink.h"
#include "QGCGeo.h"
#include "QmlUITestBase.h"
#include "Vehicle.h"

class CaacFlightDataUITest : public QmlUITestBase
{
    Q_OBJECT

private slots:
    void _liveFlightData();
};

void CaacFlightDataUITest::_liveFlightData()
{
    startUI();
    if (QTest::currentTestFailed()) {
        return;
    }
    QVERIFY(verifyEnabled(QStringLiteral("caacCenterVehicleButton"), false, QStringLiteral("no aircraft")));
    QVERIFY(clickButton(QStringLiteral("caacFlightDataButton")));
    auto* panel = findVisibleItem(_rootItem, QStringLiteral("caacFlyDataPanel"));
    QVERIFY(panel);
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

    auto* layer = findItem(_rootItem, QStringLiteral("caacFlyLayer"));
    QVERIFY(layer);
    auto* map = layer->property("mapControl").value<QObject*>();
    QVERIFY(map);
    QVERIFY(map->setProperty("center", QVariant::fromValue(QGeoCoordinate(-30, -120))));
    QVERIFY(clickButton(QStringLiteral("caacCenterVehicleButton")));
    const auto centered = map->property("center").value<QGeoCoordinate>();
    QVERIFY(QGCGeo::geodesicDistance(centered, vehicle->coordinate()) < 0.01);
    disconnectMockLink(mock);
    mock.clear();
    QVERIFY(verifyText(QStringLiteral("caacNorthDistanceValue"), unavailable, QStringLiteral("aircraft disconnected")));
    QVERIFY(verifyEnabled(QStringLiteral("caacCenterVehicleButton"), false, QStringLiteral("aircraft disconnected")));
}

UT_REGISTER_TEST(CaacFlightDataUITest, TestLabel::Integration, TestLabel::Vehicle)

#include "CaacFlightDataUITest.moc"
