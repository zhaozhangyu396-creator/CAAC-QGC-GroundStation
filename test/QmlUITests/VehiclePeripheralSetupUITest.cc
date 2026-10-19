#include "VehiclePeripheralSetupUITest.h"

#include <cmath>
#include <numbers>

#include <QtCore/QDir>
#include <QtCore/QRegularExpression>
#include <QtCore/QTimer>
#include <QtGui/QScreen>
#include <QtQuick/QQuickItem>
#include <QtQuick/QQuickItemGrabResult>
#include <QtQuick/QQuickWindow>
#include <QtTest/QSignalSpy>

#include "AutoPilotPlugin.h"
#include "Fact.h"
#include "MockLink.h"
#include "ParameterManager.h"
#include "Vehicle.h"
#include "VehicleComponent.h"
#include "VehicleGPSFactGroup.h"

void VehiclePeripheralSetupUITest::_capture(const QString& name)
{
    const QString directory = qEnvironmentVariable("CAAC_PERIPHERAL_PREVIEW_DIR");
    if (directory.isEmpty()) {
        return;
    }
    QVERIFY(QDir().mkpath(directory));
    const QString targetName =
        name.endsWith(QStringLiteral("trim")) || name.endsWith(QStringLiteral("radar"))
            ? QStringLiteral("vehiclePeripheralContent")
        : name.contains(QStringLiteral("gps-heading-"))         ? QStringLiteral("gpsHeadingCard")
        : name.endsWith(QStringLiteral("gps-position"))         ? QStringLiteral("gpsAntennaPositionPreview")
        : name.contains(QStringLiteral("heading-compensation")) ? QStringLiteral("gpsInstallationCompensation")
        : name.endsWith(QStringLiteral("summary"))              ? QStringLiteral("dualAntennaSummaryCard")
                                                                : QStringLiteral("caacSettingsMenu");
    auto* target = findVisibleItem(_rootItem, targetName);
    if (name.endsWith(QStringLiteral("failsafe-options"))) {
        const QString logicalName = name.endsWith(QStringLiteral("gcs-failsafe-options"))
                                        ? QStringLiteral("NAV_DLL_ACT")
                                        : QStringLiteral("NAV_RCL_ACT");
        auto* choice = findItem(_rootItem, QStringLiteral("caacSafety_") + logicalName + QStringLiteral("_choice"));
        QVERIFY(choice);
        auto* popup = choice->property("popup").value<QObject*>();
        QVERIFY(popup);
        target = qobject_cast<QQuickItem*>(popup->property("contentItem").value<QObject*>());
    }
    QVERIFY(target);
    const auto grab = target->grabToImage();
    QVERIFY(grab);
    QSignalSpy ready(grab.data(), &QQuickItemGrabResult::ready);
    QVERIFY_SIGNAL_WAIT(ready, TestTimeout::mediumMs());
    QVERIFY(grab->saveToFile(directory + QLatin1Char('/') + name + QStringLiteral(".png")));
}

void VehiclePeripheralSetupUITest::_settings_data()
{
    QTest::addColumn<bool>("apm");
    QTest::newRow("APM") << true;
    QTest::newRow("PX4") << false;
}

void VehiclePeripheralSetupUITest::_settings()
{
    QFETCH(bool, apm);
    runWithMockLink(
        [apm]() { return apm ? MockLink::startAPMArduCopterMockLink() : MockLink::startPX4MockLink(); },
        [&](QPointer<MockLink> mockLink, Vehicle* vehicle) {
            navigateToConfigureView();
            if (QTest::currentTestFailed()) {
                return;
            }
            const QString prefix = apm ? QStringLiteral("apm-") : QStringLiteral("px4-");
            const QString scroller = QStringLiteral("vehiclePeripheralScroller");
            auto* gpsButton = findItem(_rootItem, QStringLiteral("vehicleConfig_gps"));
            auto* radarButton = findItem(_rootItem, QStringLiteral("vehicleConfig_radar"));
            auto* trimButton = findItem(_rootItem, QStringLiteral("vehicleConfig_trim"));
            auto* modesComponent = vehicle->autopilotPlugin()->findKnownVehicleComponent(
                AutoPilotPlugin::KnownFlightModesVehicleComponent);
            QVERIFY(modesComponent);
            auto* modes = findItem(_rootItem,
                                   QStringLiteral("vehicleConfig_comp_") + QString(modesComponent->name()).remove(' '));
            QVERIFY(gpsButton && radarButton && trimButton && modes);
            if (apm) {
                for (const auto& entry : vehicle->autopilotPlugin()->vehicleComponents()) {
                    auto* component = entry.value<VehicleComponent*>();
                    QVERIFY(component);
                    if (component->inherits("APMFailsafesComponent") ||
                        component->inherits("APMFlightSafetyComponent")) {
                        QVERIFY(!findVisibleItem(
                            _rootItem, QStringLiteral("vehicleConfig_comp_") + QString(component->name()).remove(' '),
                            0));
                    }
                }
            }
            auto* sidebar = findItem(_rootItem, QStringLiteral("vehicleConfig_sidebarFlickable"));
            QVERIFY(sidebar);
            for (auto* button : {modes, gpsButton, radarButton, trimButton}) {
                button->ensurePolished();
            }
            QVERIFY(modes->mapToItem(sidebar, QPointF()).y() < gpsButton->mapToItem(sidebar, QPointF()).y());
            QVERIFY(gpsButton->mapToItem(sidebar, QPointF()).y() < radarButton->mapToItem(sidebar, QPointF()).y());
            QVERIFY(radarButton->mapToItem(sidebar, QPointF()).y() < trimButton->mapToItem(sidebar, QPointF()).y());
            scrollIntoView(gpsButton, QStringLiteral("vehicleConfig_sidebarFlickable"));
            _capture(prefix + QStringLiteral("menu"));

            QVERIFY(clickSidebarButton(QStringLiteral("vehicleConfig_gps")));
            auto* page = findVisibleItem(_rootItem, QStringLiteral("vehiclePeripheralSetup"));
            QVERIFY(page);
            QCOMPARE(page->property("page").toString(), QStringLiteral("gps"));
            _capture(prefix + QStringLiteral("gps"));
            auto* manager = vehicle->parameterManager();
            Fact* fusion =
                manager->getParameter(-1, apm ? QStringLiteral("EK3_SRC1_YAW") : QStringLiteral("EKF2_GPS_CTRL"));
            auto* headingSettings = findVisibleItem(page, QStringLiteral("gpsHeadingSettings"));
            const QString enableName = apm ? QStringLiteral("peripheralParameter_yawSource_enable")
                                           : QStringLiteral("peripheralParameter_yawFusion_bit");
            auto* headingEnable = findVisibleItem(page, enableName);
            QVERIFY(fusion && headingSettings && headingEnable);
            QCOMPARE(headingEnable->property("text").toString(), QStringLiteral("启用双天线定向"));
            QVERIFY(!headingEnable->property("checked").toBool());
            const int initialFusion = fusion->rawValue().toInt();
            QSignalSpy fusionWrites(fusion, &Fact::containerRawValueChanged);
            QSignalSpy fusionAck(fusion, &Fact::vehicleUpdated);
            _capture(prefix + QStringLiteral("gps-heading-disabled"));
            QVERIFY(clickButtonScrolled(enableName, scroller));
            QVERIFY(headingEnable->property("checked").toBool());
            QCOMPARE(fusionWrites.count(), 0);
            QCOMPARE(fusion->rawValue().toInt(), initialFusion);
            mockLink->setArmed(true);
            QTRY_VERIFY_WITH_TIMEOUT(vehicle->armed(), TestTimeout::mediumMs());
            QVERIFY(!headingEnable->isEnabled());
            QVERIFY(QMetaObject::invokeMethod(headingSettings, "save"));
            QCOMPARE(fusionWrites.count(), 0);
            mockLink->setArmed(false);
            QTRY_VERIFY_WITH_TIMEOUT(!vehicle->armed(), TestTimeout::mediumMs());
            QVERIFY(clickButtonScrolled(QStringLiteral("gpsHeadingSettings_save"), scroller));
            QTRY_VERIFY_WITH_TIMEOUT(!fusionAck.isEmpty() && !manager->pendingWrites(), TestTimeout::mediumMs());
            QCOMPARE(fusion->rawValue().toInt(), apm ? 2 : initialFusion | 8);
            QCOMPARE(headingSettings->property("resultText").toString(), QStringLiteral("已收到飞控保存确认。"));
            if (apm) {
                auto* sourceChoice = findVisibleItem(page, QStringLiteral("peripheralParameter_yawSource_choice"));
                QVERIFY(sourceChoice);
                const int fallbackIndex = fusion->enumValues().indexOf(QVariant(3));
                QVERIFY(fallbackIndex >= 0);
                QVERIFY(sourceChoice->setProperty("currentIndex", fallbackIndex));
                QVERIFY(QMetaObject::invokeMethod(sourceChoice, "activated", Q_ARG(int, fallbackIndex)));
                QVERIFY(headingEnable->property("checked").toBool());
                QCOMPARE(fusion->rawValue().toInt(), 2);
                fusionAck.clear();
                QVERIFY(clickButtonScrolled(QStringLiteral("gpsHeadingSettings_save"), scroller));
                QTRY_VERIFY_WITH_TIMEOUT(!fusionAck.isEmpty() && !manager->pendingWrites(), TestTimeout::mediumMs());
                QCOMPARE(fusion->rawValue().toInt(), 3);
                _capture(prefix + QStringLiteral("gps-heading-enabled"));
                const int writes = fusionWrites.count();
                QVERIFY(clickButtonScrolled(QStringLiteral("gpsHeadingSettings_save"), scroller));
                QCOMPARE(fusionWrites.count(), writes);
                QCOMPARE(fusion->rawValue().toInt(), 3);
            }
            if (!apm) {
                _capture(prefix + QStringLiteral("gps-heading-enabled"));
            }
            fusionAck.clear();
            QVERIFY(clickButtonScrolled(enableName, scroller));
            QVERIFY(!headingEnable->property("checked").toBool());
            QCOMPARE(fusion->rawValue().toInt(), apm ? 3 : initialFusion | 8);
            QVERIFY(clickButtonScrolled(QStringLiteral("gpsHeadingSettings_save"), scroller));
            QTRY_VERIFY_WITH_TIMEOUT(!fusionAck.isEmpty() && !manager->pendingWrites(), TestTimeout::mediumMs());
            QCOMPARE(fusion->rawValue().toInt(), apm ? 1 : initialFusion & ~8);
            QVERIFY(clickButtonScrolled(QStringLiteral("gpsInstallationPosition"), scroller));
            auto* positionScroller = findItem(page, scroller);
            QVERIFY(positionScroller);
            QVERIFY(positionScroller->property("contentY").toDouble() > 0);
            _capture(prefix + QStringLiteral("gps-installation"));
            const QString offsetName = apm ? QStringLiteral("GPS1_POS_X") : QStringLiteral("EKF2_GPS_POS_X");
            Fact* offset = manager->getParameter(-1, offsetName);
            QVERIFY(offset);
            const double initialOffset = offset->rawValue().toDouble();
            auto* preview = findItem(page, QStringLiteral("gpsAntennaPositionPreview"));
            auto* topMarker = findItem(page, QStringLiteral("gpsTopMarker"));
            auto* sideMarker = findItem(page, QStringLiteral("gpsSideMarker"));
            auto* xInput = findItem(page, QStringLiteral("peripheralParameter_gpsX_value"));
            QVERIFY(preview && topMarker && sideMarker && xInput);
            QVERIFY(std::abs(xInput->property("text").toDouble() - initialOffset * 100.0) < 1e-3);
            const double initialTopY = topMarker->y();
            const double initialSideX = sideMarker->x();
            QSignalSpy offsetWrites(offset, &Fact::containerRawValueChanged);
            QSignalSpy offsetAck(offset, &Fact::vehicleUpdated);
            QVERIFY(clickButtonScrolled(QStringLiteral("peripheralParameter_gpsX_plus"), scroller));
            QTRY_VERIFY_WITH_TIMEOUT(
                std::abs(preview->property("positionX").toDouble() - initialOffset * 100.0 - 1.0) < 1e-3,
                TestTimeout::shortMs());
            QVERIFY(topMarker->y() < initialTopY);
            QVERIFY(sideMarker->x() > initialSideX);
            for (const auto& coordinate : {std::pair{QStringLiteral("gpsX"), QStringLiteral("25")},
                                           std::pair{QStringLiteral("gpsY"), QStringLiteral("-15")},
                                           std::pair{QStringLiteral("gpsZ"), QStringLiteral("-10")}}) {
                auto* input = findItem(
                    page, QStringLiteral("peripheralParameter_") + coordinate.first + QStringLiteral("_value"));
                QVERIFY(input);
                QVERIFY(input->setProperty("text", coordinate.second));
                QVERIFY(QMetaObject::invokeMethod(input, "textEdited"));
            }
            QTRY_COMPARE_WITH_TIMEOUT(preview->property("positionX").toDouble(), 25.0, TestTimeout::shortMs());
            QCOMPARE(preview->property("positionY").toDouble(), -15.0);
            QCOMPARE(preview->property("positionZ").toDouble(), -10.0);
            _capture(prefix + QStringLiteral("gps-position"));
            QCOMPARE(offsetWrites.count(), 0);
            QCOMPARE(offset->rawValue().toDouble(), initialOffset);
            QVERIFY(clickButtonScrolled(QStringLiteral("gpsAntennaOffsets_save"), scroller));
            QTRY_VERIFY_WITH_TIMEOUT(!offsetAck.isEmpty() && !manager->pendingWrites(), TestTimeout::mediumMs());
            QCOMPARE(offsetWrites.count(), 1);
            QVERIFY(std::abs(offset->rawValue().toDouble() - 0.25) < 1e-6);
            const QString yName = apm ? QStringLiteral("GPS1_POS_Y") : QStringLiteral("EKF2_GPS_POS_Y");
            const QString zName = apm ? QStringLiteral("GPS1_POS_Z") : QStringLiteral("EKF2_GPS_POS_Z");
            QVERIFY(std::abs(manager->getParameter(-1, yName)->rawValue().toDouble() + 0.15) < 1e-6);
            QVERIFY(std::abs(manager->getParameter(-1, zName)->rawValue().toDouble() + 0.10) < 1e-6);
            auto* offsets = findItem(page, QStringLiteral("gpsAntennaOffsets"));
            QVERIFY(offsets);
            QCOMPARE(offsets->property("resultText").toString(), QStringLiteral("已收到飞控保存确认。"));
            _capture(prefix + QStringLiteral("gps-offsets"));

            QVERIFY(clickButtonScrolled(QStringLiteral("gpsHeadingInstallation"), scroller));
            auto* compensation = findVisibleItem(page, QStringLiteral("gpsInstallationCompensation"));
            QVERIFY(compensation);
            if (apm) {
                QVERIFY(findVisibleItem(compensation, QStringLiteral("gpsMovingBaseSettings")));
                QVERIFY(!findVisibleItem(compensation, QStringLiteral("gpsHeadingOffsetSettings")));
            } else {
                Fact* yawOffset = manager->getParameter(-1, QStringLiteral("EKF2_GPS_YAW_OFF"));
                auto* input = findVisibleItem(compensation, QStringLiteral("peripheralParameter_yawOffset_value"));
                auto* settings = findVisibleItem(compensation, QStringLiteral("gpsHeadingOffsetSettings"));
                auto* diagram = findItem(compensation, QStringLiteral("gpsHeadingCompensationPreview"));
                auto* antennaA = findItem(compensation, QStringLiteral("gpsHeadingAntennaA"));
                auto* antennaB = findItem(compensation, QStringLiteral("gpsHeadingAntennaB"));
                QVERIFY(yawOffset && input && settings && diagram && antennaA && antennaB);
                const double initialYawOffset = yawOffset->rawValue().toDouble();
                QSignalSpy writes(yawOffset, &Fact::containerRawValueChanged);
                QSignalSpy acknowledgement(yawOffset, &Fact::vehicleUpdated);
                QVERIFY(input->setProperty("text", QStringLiteral("450")));
                QVERIFY(QMetaObject::invokeMethod(input, "textEdited"));
                QVERIFY(!diagram->property("valid").toBool());
                QVERIFY(clickButtonScrolled(QStringLiteral("gpsHeadingOffsetSettings_save"), scroller));
                QVERIFY(input->property("validationError").toBool());
                QCOMPARE(writes.count(), 0);
                QVERIFY(input->setProperty("text", QStringLiteral("90")));
                QVERIFY(QMetaObject::invokeMethod(input, "textEdited"));
                QVERIFY(diagram->property("valid").toBool());
                QTRY_VERIFY_WITH_TIMEOUT(antennaA->x() > antennaB->x(), TestTimeout::shortMs());
                QVERIFY(std::abs(antennaA->y() - antennaB->y()) < 0.01);
                QCOMPARE(yawOffset->rawValue().toDouble(), initialYawOffset);
                QCOMPARE(writes.count(), 0);
                QVERIFY(clickButtonScrolled(QStringLiteral("gpsHeadingOffsetSettings_save"), scroller));
                QTRY_VERIFY_WITH_TIMEOUT(!acknowledgement.isEmpty() && !manager->pendingWrites(),
                                         TestTimeout::mediumMs());
                QCOMPARE(yawOffset->rawValue().toDouble(), 90.0);
                QCOMPARE(settings->property("resultText").toString(), QStringLiteral("已收到飞控保存确认。"));
                _capture(prefix + QStringLiteral("heading-compensation"));
                acknowledgement.clear();
                QVERIFY(input->setProperty("text", QString::number(initialYawOffset)));
                QVERIFY(QMetaObject::invokeMethod(input, "textEdited"));
                QVERIFY(clickButtonScrolled(QStringLiteral("gpsHeadingOffsetSettings_save"), scroller));
                QTRY_VERIFY_WITH_TIMEOUT(!acknowledgement.isEmpty() && !manager->pendingWrites(),
                                         TestTimeout::mediumMs());
                QCOMPARE(yawOffset->rawValue().toDouble(), initialYawOffset);
                QCOMPARE(writes.count(), 2);
                _capture(prefix + QStringLiteral("heading-compensation-zero"));
            }
            if (apm) {
                _capture(prefix + QStringLiteral("heading-compensation"));
            }

            QVERIFY(clickSidebarButton(QStringLiteral("vehicleConfig_trim")));
            Fact* trim =
                manager->getParameter(-1, apm ? QStringLiteral("AHRS_TRIM_X") : QStringLiteral("SENS_BOARD_X_OFF"));
            QVERIFY(trim);
            const double initialTrim = trim->rawValue().toDouble();
            QSignalSpy trimWrites(trim, &Fact::containerRawValueChanged);
            QSignalSpy trimAck(trim, &Fact::vehicleUpdated);
            QVERIFY(clickButtonScrolled(QStringLiteral("trimDriftRight"), scroller));
            QVERIFY(clickButtonScrolled(QStringLiteral("trimDriftLeft"), scroller));
            QVERIFY(clickButtonScrolled(QStringLiteral("trimDriftForward"), scroller));
            QVERIFY(clickButtonScrolled(QStringLiteral("trimDriftBackward"), scroller));
            QVERIFY(clickButtonScrolled(QStringLiteral("trimDriftLeft"), scroller));
            QCOMPARE(trimWrites.count(), 0);
            QVERIFY(clickButtonScrolled(QStringLiteral("flightControllerTrim_save"), scroller));
            QTRY_VERIFY_WITH_TIMEOUT(!trimAck.isEmpty() && !manager->pendingWrites(), TestTimeout::mediumMs());
            const double increment = apm ? 0.1 * std::numbers::pi / 180.0 : 0.1;
            QVERIFY(std::abs(trim->rawValue().toDouble() - initialTrim - increment) < 1e-5);
            QVERIFY(clickButtonScrolled(QStringLiteral("trimDriftLeft"), scroller));
            _capture(prefix + QStringLiteral("trim"));

            auto* save = findItem(_rootItem, QStringLiteral("flightControllerTrim_save"));
            QVERIFY(save);
            mockLink->setArmed(true);
            QTRY_VERIFY_WITH_TIMEOUT(vehicle->armed(), TestTimeout::mediumMs());
            QVERIFY(!save->isEnabled());
            const int writesWhileArmed = trimWrites.count();
            auto* trimSection = findItem(_rootItem, QStringLiteral("flightControllerTrim"));
            QVERIFY(trimSection);
            QVERIFY(QMetaObject::invokeMethod(trimSection, "save"));
            QCOMPARE(trimWrites.count(), writesWhileArmed);
            mockLink->setArmed(false);
            QTRY_VERIFY_WITH_TIMEOUT(!vehicle->armed(), TestTimeout::mediumMs());

            auto* invalidTrim = findVisibleItemScrolled(QStringLiteral("peripheralParameter_trimRoll_value"), scroller);
            QVERIFY(invalidTrim);
            QVERIFY(invalidTrim->setProperty("text", QStringLiteral("99")));
            QVERIFY(QMetaObject::invokeMethod(invalidTrim, "textEdited"));
            QVERIFY(clickButtonScrolled(QStringLiteral("flightControllerTrim_save"), scroller));
            QVERIFY(invalidTrim->property("validationError").toBool());
            QCOMPARE(trimWrites.count(), writesWhileArmed);
            QVERIFY(clickButtonScrolled(QStringLiteral("flightControllerTrim_read"), scroller));
            QVERIFY(!invalidTrim->property("validationError").toBool());
            waitForParamRefreshQuiet(vehicle);
            if (QTest::currentTestFailed()) {
                return;
            }

            QVERIFY(clickSidebarButton(QStringLiteral("vehicleConfig_radar")));
            auto* distance = findVisibleItem(_rootItem, QStringLiteral("radarLiveDistance"));
            QVERIFY(distance);
            QCOMPARE(distance->property("text").toString(), QStringLiteral("未提供"));
            mavlink_distance_sensor_t range{};
            range.min_distance = 10;
            range.max_distance = 3000;
            range.current_distance = 28;
            range.orientation = MAV_SENSOR_ROTATION_PITCH_270;
            range.type = MAV_DISTANCE_SENSOR_LASER;
            range.signal_quality = 100;
            mavlink_message_t message{};
            mavlink_msg_distance_sensor_encode(vehicle->id(), vehicle->defaultComponentId(), &message, &range);
            mockLink->respondWithMavlinkMessage(message);
            QTRY_COMPARE_WITH_TIMEOUT(distance->property("text").toString(), QStringLiteral("0.28 m"),
                                      TestTimeout::mediumMs());
            _capture(prefix + QStringLiteral("radar"));
            QTRY_COMPARE_WITH_TIMEOUT(distance->property("text").toString(), QStringLiteral("未提供"),
                                      TestTimeout::mediumMs());
            range.signal_quality = 1;
            mavlink_msg_distance_sensor_encode(vehicle->id(), vehicle->defaultComponentId(), &message, &range);
            mockLink->respondWithMavlinkMessage(message);
            QTRY_COMPARE_WITH_TIMEOUT(vehicle->distanceSensorFactGroup()->property("downwardUpdateTime").toLongLong(),
                                      0, TestTimeout::mediumMs());

            mockLink->setVehicleAttitudeOverrideDeg(0, 0, 12);
            QTRY_COMPARE_WITH_TIMEOUT(vehicle->roll()->rawValue().toDouble(), 0.0, TestTimeout::shortMs());
            QTRY_COMPARE_WITH_TIMEOUT(vehicle->pitch()->rawValue().toDouble(), 0.0, TestTimeout::shortMs());
            QVERIFY(clickButton(QStringLiteral("vehicleConfig_backToMenu")));
            QVERIFY(clickButtonScrolled(QStringLiteral("vehicleConfig_summary"),
                                        QStringLiteral("vehicleConfig_sidebarFlickable")));
            auto* yaw = findVisibleItem(_rootItem, QStringLiteral("dualAntenna_measuredYaw"));
            auto* compass = findVisibleItem(_rootItem, QStringLiteral("dualAntenna_compassHeading"));
            QVERIFY(yaw && compass);
            QCOMPARE(compass->property("valueText").toString(), QStringLiteral("未提供"));
            mavlink_raw_imu_t rawImu{};
            rawImu.ymag = -100;
            mavlink_msg_raw_imu_encode(vehicle->id(), vehicle->defaultComponentId(), &message, &rawImu);
            mockLink->respondWithMavlinkMessage(message);
            QTRY_COMPARE_WITH_TIMEOUT(compass->property("valueText").toString(), QStringLiteral("90.0 °"),
                                      TestTimeout::shortMs());
            mockLink->setVehicleAttitudeOverrideDeg(0, 0, 270);
            QTRY_VERIFY_WITH_TIMEOUT(std::abs(vehicle->heading()->rawValue().toDouble() - 270.0) <= 1.0,
                                     TestTimeout::shortMs());
            QCOMPARE(compass->property("valueText").toString(), QStringLiteral("90.0 °"));
            mavlink_gps_raw_int_t gps{};
            mavlink_gps2_raw_t gps2{};
            gps.fix_type = GPS_FIX_TYPE_RTK_FIXED;
            gps.satellites_visible = 21;
            gps.yaw = 12345;
            QTimer gpsTelemetry;
            connect(&gpsTelemetry, &QTimer::timeout, vehicle, [&]() {
                mavlink_message_t sample{};
                mavlink_msg_gps_raw_int_encode(vehicle->id(), vehicle->defaultComponentId(), &sample, &gps);
                mockLink->respondWithMavlinkMessage(sample);
                mavlink_msg_gps2_raw_encode(vehicle->id(), vehicle->defaultComponentId(), &sample, &gps2);
                mockLink->respondWithMavlinkMessage(sample);
            });
            // Keep the injected receiver state current alongside MockLink's default GPS stream.
            gpsTelemetry.start(25);
            mavlink_msg_gps_raw_int_encode(vehicle->id(), vehicle->defaultComponentId(), &message, &gps);
            mockLink->respondWithMavlinkMessage(message);
            QTRY_COMPARE_WITH_TIMEOUT(yaw->property("valueText").toString(), QStringLiteral("123.5 °"),
                                      TestTimeout::shortMs());
            gps2.fix_type = GPS_FIX_TYPE_RTK_FIXED;
            gps2.satellites_visible = 19;
            gps2.yaw = 23456;
            mavlink_msg_gps2_raw_encode(vehicle->id(), vehicle->defaultComponentId(), &message, &gps2);
            mockLink->respondWithMavlinkMessage(message);
            QTRY_COMPARE_WITH_TIMEOUT(vehicle->gps2FactGroup()->yaw()->rawValue().toDouble(), 234.56,
                                      TestTimeout::shortMs());
            QTRY_COMPARE_WITH_TIMEOUT(yaw->property("valueText").toString(), QStringLiteral("123.5 °"),
                                      TestTimeout::shortMs());
            _capture(prefix + QStringLiteral("summary"));
            gps.yaw = 0;
            mavlink_msg_gps_raw_int_encode(vehicle->id(), vehicle->defaultComponentId(), &message, &gps);
            mockLink->respondWithMavlinkMessage(message);
            QTRY_COMPARE_WITH_TIMEOUT(yaw->property("valueText").toString(), QStringLiteral("234.6 °"),
                                      TestTimeout::shortMs());
            gps.yaw = 36000;
            mavlink_msg_gps_raw_int_encode(vehicle->id(), vehicle->defaultComponentId(), &message, &gps);
            mockLink->respondWithMavlinkMessage(message);
            QTRY_COMPARE_WITH_TIMEOUT(yaw->property("valueText").toString(), QStringLiteral("0.0 °"),
                                      TestTimeout::shortMs());
            gps.yaw = 0;
            gps2.yaw = 0;
            mavlink_msg_gps_raw_int_encode(vehicle->id(), vehicle->defaultComponentId(), &message, &gps);
            mockLink->respondWithMavlinkMessage(message);
            mavlink_msg_gps2_raw_encode(vehicle->id(), vehicle->defaultComponentId(), &message, &gps2);
            mockLink->respondWithMavlinkMessage(message);
            QTRY_COMPARE_WITH_TIMEOUT(yaw->property("valueText").toString(), QStringLiteral("未提供"),
                                      TestTimeout::shortMs());
            gpsTelemetry.stop();
            mavlink_highres_imu_t imu{};
            imu.fields_updated = 0x01c0;
            imu.xmag = -1;
            mavlink_msg_highres_imu_encode(vehicle->id(), vehicle->defaultComponentId(), &message, &imu);
            mockLink->respondWithMavlinkMessage(message);
            QTRY_COMPARE_WITH_TIMEOUT(compass->property("valueText").toString(), QStringLiteral("180.0 °"),
                                      TestTimeout::shortMs());
            mavlink_msg_raw_imu_encode(vehicle->id(), vehicle->defaultComponentId(), &message, &rawImu);
            mockLink->respondWithMavlinkMessage(message);
            QCOMPARE(compass->property("valueText").toString(), QStringLiteral("180.0 °"));
            mockLink->setVehicleAttitudeOverrideDeg(0, 30, 12);
            QTRY_VERIFY_WITH_TIMEOUT(std::abs(vehicle->pitch()->rawValue().toDouble() - 30.0) < 0.01,
                                     TestTimeout::shortMs());
            imu.xmag = std::sqrt(3.0) / 2.0;
            imu.zmag = 0.5;
            mavlink_msg_highres_imu_encode(vehicle->id(), vehicle->defaultComponentId(), &message, &imu);
            mockLink->respondWithMavlinkMessage(message);
            QTRY_COMPARE_WITH_TIMEOUT(compass->property("valueText").toString(), QStringLiteral("0.0 °"),
                                      TestTimeout::shortMs());
            QTRY_COMPARE_WITH_TIMEOUT(compass->property("valueText").toString(), QStringLiteral("未提供"),
                                      TestTimeout::mediumMs());
            mockLink->clearVehicleAttitudeOverride();

            QVERIFY(clickButtonScrolled(QStringLiteral("toolbar_safety"),
                                        QStringLiteral("caacSettingsNavigationScroller")));
            auto* safety = findVisibleItem(_rootItem, QStringLiteral("caacSafetySettings"));
            QVERIFY(safety);
            const QString safetyScroller = QStringLiteral("caacSafetyScroller");
            auto* returnSettings = findVisibleItemScrolled(QStringLiteral("caacReturnSettings"), safetyScroller);
            QVERIFY(returnSettings);
            QCOMPARE(returnSettings->property("title").toString(), QStringLiteral("返航设置"));
            _capture(prefix + QStringLiteral("safety-return"));
            const QSize originalSize = _window->size();
            const QMargins frame = _window->frameMargins();
            const int availableHeight =
                _window->screen() ? _window->screen()->availableGeometry().height() - frame.top() - frame.bottom()
                                  : 1000;
            const QSize expandedSize(1000, qMin(1000, availableHeight));
            _window->resize(expandedSize);
            QTRY_COMPARE_WITH_TIMEOUT(_window->size(), expandedSize, TestTimeout::shortMs());
            QTRY_COMPARE_WITH_TIMEOUT(_window->contentItem()->height(), qreal(expandedSize.height()),
                                      TestTimeout::shortMs());
            for (const QString& logicalName : {QStringLiteral("NAV_RCL_ACT"), QStringLiteral("NAV_DLL_ACT")}) {
                const QString rowName = QStringLiteral("caacSafety_") + logicalName;
                auto* choice = findVisibleItemScrolled(rowName + QStringLiteral("_choice"), safetyScroller);
                auto* row = findItem(_rootItem, rowName);
                QVERIFY(choice && row);
                auto* fact = row->property("fact").value<Fact*>();
                QVERIFY(fact);
                const auto originalStrings = fact->enumStrings();
                const auto originalValues = fact->enumValues();
                const auto originalValue = fact->rawValue();
                QSignalSpy writes(fact, &Fact::containerRawValueChanged);
                QStringList translated;
                for (const auto& original : originalStrings) {
                    QVariant display;
                    QVERIFY(QMetaObject::invokeMethod(choice, "displayString", Q_RETURN_ARG(QVariant, display),
                                                      Q_ARG(QVariant, original)));
                    const QString text = display.toString();
                    QVERIFY2(QRegularExpression(QStringLiteral("[\\x{3400}-\\x{9fff}]")).match(text).hasMatch(),
                             qPrintable(text));
                    QVERIFY(!text.contains(QRegularExpression(QStringLiteral("[()（）]"))));
                    QVERIFY(!text.contains(QStringLiteral("FS_OPTIONS")));
                    translated.append(text);
                }
                QVERIFY(clickButtonScrolled(rowName + QStringLiteral("_choice"), safetyScroller));
                auto* popup = choice->property("popup").value<QObject*>();
                QVERIFY(popup);
                QTRY_VERIFY_WITH_TIMEOUT(popup->property("opened").toBool(), TestTimeout::shortMs());
                auto* list = popup->property("contentItem").value<QQuickItem*>();
                QVERIFY(list);
                for (int index = 0; index < translated.size(); ++index) {
                    QVERIFY(QMetaObject::invokeMethod(list, "positionViewAtIndex", Q_ARG(int, index), Q_ARG(int, 0)));
                    const QString optionName = rowName + QStringLiteral("_option_%1").arg(index);
                    QTRY_VERIFY_WITH_TIMEOUT(findItem(list, optionName + QStringLiteral("_text")),
                                             TestTimeout::shortMs());
                    auto* text = findItem(list, optionName + QStringLiteral("_text"));
                    QCOMPARE(text->property("text").toString(), translated[index]);
                    QVERIFY(text->height() + 1 >= text->implicitHeight());
                    auto* separator = findItem(list, optionName + QStringLiteral("_separator"));
                    QVERIFY(separator);
                    QCOMPARE(separator->isVisible(), index < translated.size() - 1);
                }
                QVERIFY(QMetaObject::invokeMethod(list, "positionViewAtIndex", Q_ARG(int, 0), Q_ARG(int, 0)));
                _capture(prefix + (logicalName == QStringLiteral("NAV_DLL_ACT") ? QStringLiteral("gcs-failsafe-options")
                                                                                : QStringLiteral("failsafe-options")));
                QTest::keyClick(_window, Qt::Key_Escape);
                QTRY_VERIFY_WITH_TIMEOUT(!popup->property("visible").toBool(), TestTimeout::shortMs());
                QCOMPARE(fact->enumStrings(), originalStrings);
                QCOMPARE(fact->enumValues(), originalValues);
                QCOMPARE(fact->rawValue(), originalValue);
                QCOMPARE(writes.count(), 0);
            }
            _window->resize(originalSize);
            QTRY_COMPARE_WITH_TIMEOUT(_window->contentItem()->height(), double(originalSize.height()),
                                      TestTimeout::shortMs());
            if (apm) {
                auto* fenceEnable = manager->getParameter(-1, QStringLiteral("FENCE_ENABLE"));
                auto* fenceAction = manager->getParameter(-1, QStringLiteral("FENCE_ACTION"));
                QVERIFY(fenceEnable && fenceAction);
                const int initialEnable = fenceEnable->rawValue().toInt();
                const QVariant initialAction = fenceAction->rawValue();
                QSignalSpy fenceEnableAck(fenceEnable, &Fact::vehicleUpdated);
                QVERIFY(clickButtonScrolled(QStringLiteral("caacFenceEnabled"), safetyScroller));
                QTRY_VERIFY_WITH_TIMEOUT(!fenceEnableAck.isEmpty() && !manager->pendingWrites(),
                                         TestTimeout::mediumMs());
                QCOMPARE(fenceEnable->rawValue().toInt(), initialEnable ? 0 : 1);
                QCOMPARE(fenceAction->rawValue(), initialAction);

                QVERIFY(clickButtonScrolled(QStringLiteral("caacFenceAdditionalSettings_toggle"), safetyScroller));
                Fact* fenceType = manager->getParameter(-1, QStringLiteral("FENCE_TYPE"));
                QVERIFY(fenceType);
                const int initialType = fenceType->rawValue().toInt();
                QSignalSpy typeAck(fenceType, &Fact::vehicleUpdated);
                QVERIFY(clickButtonScrolled(QStringLiteral("peripheralParameter_fenceType_flag_8"), safetyScroller));
                QCOMPARE(fenceType->rawValue().toInt(), initialType);
                QVERIFY(clickButtonScrolled(QStringLiteral("caacFenceAdditionalSettings_save"), safetyScroller));
                QTRY_VERIFY_WITH_TIMEOUT(!typeAck.isEmpty() && !manager->pendingWrites(), TestTimeout::mediumMs());
                QCOMPARE(fenceType->rawValue().toInt(), initialType ^ 8);

                QVERIFY(clickButtonScrolled(QStringLiteral("caacNavigationFailsafeSettings_toggle"), safetyScroller));
                Fact* ekfThreshold = manager->getParameter(-1, QStringLiteral("FS_EKF_THRESH"));
                QVERIFY(ekfThreshold);
                const double initialThreshold = ekfThreshold->rawValue().toDouble();
                double expectedThreshold = initialThreshold + 0.1;
                QSignalSpy ekfAck(ekfThreshold, &Fact::vehicleUpdated);
                if (ekfThreshold->enumValues().isEmpty()) {
                    QVERIFY(
                        clickButtonScrolled(QStringLiteral("peripheralParameter_ekfThreshold_plus"), safetyScroller));
                } else {
                    auto* choice = findVisibleItemScrolled(QStringLiteral("peripheralParameter_ekfThreshold_choice"),
                                                           safetyScroller);
                    QVERIFY(choice);
                    const int selection =
                        (choice->property("currentIndex").toInt() + 1) % choice->property("count").toInt();
                    expectedThreshold = ekfThreshold->enumValues()[selection].toDouble();
                    QVERIFY(clickButton(QStringLiteral("peripheralParameter_ekfThreshold_choice")));
                    auto* popup = choice->property("popup").value<QObject*>();
                    QVERIFY(popup);
                    QTRY_VERIFY_WITH_TIMEOUT(popup->property("opened").toBool(), TestTimeout::shortMs());
                    QTest::keyClick(_window, Qt::Key_Home);
                    QTRY_COMPARE_WITH_TIMEOUT(choice->property("highlightedIndex").toInt(), 0, TestTimeout::shortMs());
                    for (int index = 0; index < selection; ++index) {
                        QTest::keyClick(_window, Qt::Key_Down);
                        QTRY_COMPARE_WITH_TIMEOUT(choice->property("highlightedIndex").toInt(), index + 1,
                                                  TestTimeout::shortMs());
                    }
                    QTest::keyClick(_window, Qt::Key_Return);
                    QTRY_COMPARE_WITH_TIMEOUT(choice->property("currentIndex").toInt(), selection,
                                              TestTimeout::shortMs());
                    QTRY_VERIFY_WITH_TIMEOUT(!popup->property("visible").toBool(), TestTimeout::shortMs());
                }
                QCOMPARE(ekfThreshold->rawValue().toDouble(), initialThreshold);
                QVERIFY(clickButtonScrolled(QStringLiteral("caacNavigationFailsafeSettings_save"), safetyScroller));
                QTRY_VERIFY_WITH_TIMEOUT(!ekfAck.isEmpty() && !manager->pendingWrites(), TestTimeout::mediumMs());
                QVERIFY(std::abs(ekfThreshold->rawValue().toDouble() - expectedThreshold) < 1e-6);
                QVERIFY(
                    findVisibleItemScrolled(QStringLiteral("caacNavigationFailsafeSettings_toggle"), safetyScroller));
                _capture(prefix + QStringLiteral("safety-navigation"));
            }
        });
}

UT_REGISTER_TEST(VehiclePeripheralSetupUITest, TestLabel::Integration)
