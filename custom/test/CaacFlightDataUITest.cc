#include <QtCore/QRegularExpression>
#include <QtCore/QScopeGuard>
#include <QtGui/QClipboard>
#include <QtGui/QFont>
#include <QtGui/QGuiApplication>
#include <QtGui/QImage>
#include <QtGui/QScreen>
#include <QtGui/QWheelEvent>
#include <QtQml/QJSValue>
#include <QtQuick/QQuickItem>
#include <QtQuick/QQuickItemGrabResult>
#include <QtQuick/QQuickWindow>
#include <QtTest/QSignalSpy>

#include "AppSettings.h"
#include "CaacCorePlugin.h"
#include "CaacFlightDistance.h"
#include "Fact.h"
#include "FlightMapSettings.h"
#include "MAVLinkProtocol.h"
#include "MissionController.h"
#include "MockLink.h"
#include "MultiVehicleManager.h"
#include "ParameterManager.h"
#include "PlanMasterController.h"
#include "QGCApplication.h"
#include "QGCGeo.h"
#include "QGCMapEngineManager.h"
#include "QmlUITestBase.h"
#include "RadioStatusFactGroup.h"
#include "RemoteIDManager.h"
#include "SettingsManager.h"
#include "TrajectoryPoints.h"
#include "Vehicle.h"

class CaacFlightDataUITest : public QmlUITestBase
{
    Q_OBJECT

private slots:
    void _vehicleRebootConfirmation();
    void _toolbarStatusSpacing_data();
    void _toolbarStatusSpacing();
    void _mapZoomControls_data();
    void _mapZoomControls();
    void _liveFlightData();
    void _dashboardInteractions();
    void _safetyAndUomTelemetry();
    void _safetySpeedUnits_data();
    void _safetySpeedUnits();
    void _safetyFailsafeEnums_data();
    void _safetyFailsafeEnums();

private:
    void _resizeWindow(QSize size);
};

void CaacFlightDataUITest::_resizeWindow(QSize size)
{
#ifdef Q_OS_WIN
    QVERIFY(_window->screen());
    const auto available = _window->screen()->availableGeometry();
    const auto margins = _window->frameMargins();
    const QSize maximum(available.width() - margins.left() - margins.right(),
                        available.height() - margins.top() - margins.bottom());
    size = size.boundedTo(maximum).expandedTo(_window->minimumSize());
    _window->setPosition(available.topLeft() + QPoint(margins.left(), margins.top()));
#endif
    _window->resize(size);
}

void CaacFlightDataUITest::_toolbarStatusSpacing_data()
{
    QTest::addColumn<int>("windowWidth");
    QTest::addColumn<qreal>("fontScale");
    QTest::newRow("normal") << 1280 << qreal(1);
    QTest::newRow("large-font") << 1280 << qreal(1.4);
    QTest::newRow("narrow-large-font") << 800 << qreal(1.4);
    QTest::newRow("wide") << 1920 << qreal(1);
}

void CaacFlightDataUITest::_toolbarStatusSpacing()
{
    QFETCH(int, windowWidth);
    QFETCH(qreal, fontScale);
    startUI();
    if (QTest::currentTestFailed()) {
        return;
    }
    _resizeWindow(QSize(windowWidth, 800));
    auto* label = findVisibleItem(_rootItem, QStringLiteral("caacConnectionStatusLabel"));
    auto* button = findVisibleItem(_rootItem, QStringLiteral("caacConnectionStatusButton"));
    auto* rc = findVisibleItem(_rootItem, QStringLiteral("caacRCRSSIIndicator"));
    QVERIFY(label && button && rc);
    const auto buttonLabel = [&](const QString& name) -> QQuickItem* {
        auto* item = findVisibleItem(_rootItem, name);
        if (!item) {
            return nullptr;
        }
        auto* content = item->property("contentItem").value<QQuickItem*>();
        if (content) {
            for (auto* child : content->childItems()) {
                if (child->property("paintedWidth").isValid() && child->property("text") == item->property("text")) {
                    return child;
                }
            }
        }
        return nullptr;
    };
    auto* mode = buttonLabel(QStringLiteral("caacFlightModeButton"));
    auto* route = buttonLabel(QStringLiteral("caacFlightSettingsButton"));
    auto* sensors = buttonLabel(QStringLiteral("caacSensorStatusButton"));
    auto* armed = findVisibleItem(_rootItem, QStringLiteral("caacArmStatusValue"));
    QVERIFY(mode && route && sensors && armed);
    const auto aligned = [&]() {
        const qreal pointSize = mode->property("font").value<QFont>().pointSizeF();
        const auto baseline = [](QQuickItem* item) { return item->mapToScene(QPointF(0, item->baselineOffset())).y(); };
        const auto scale = [](QQuickItem* item) {
            return item->mapToScene(QPointF(0, 1)).y() - item->mapToScene(QPointF(0, 0)).y();
        };
        for (auto* item : {route, sensors, armed}) {
            if (!qFuzzyCompare(item->property("font").value<QFont>().pointSizeF(), pointSize) ||
                qAbs(scale(item) - scale(mode)) > 0.001 || qAbs(baseline(item) - baseline(mode)) > 1.5) {
                return false;
            }
        }
        return true;
    };
    QTRY_VERIFY_WITH_TIMEOUT(aligned(), TestTimeout::shortMs());
    QFont font = label->property("font").value<QFont>();
    font.setPointSizeF(font.pointSizeF() * fontScale);
    QVERIFY(label->setProperty("font", font));
    const auto labelBounds = [&]() {
        return label->mapRectToScene(QRectF(0, 0, label->property("paintedWidth").toReal(), label->height()));
    };
    const auto spacingIsClear = [&]() {
        const auto text = labelBounds();
        const auto buttonBounds = button->mapRectToScene(button->boundingRect());
        const auto rcBounds = rc->mapRectToScene(rc->boundingRect());
        return text.width() > 0 && text.right() <= buttonBounds.right() &&
               rcBounds.left() - text.right() >= text.height() * 0.4;
    };
    QTRY_VERIFY2_WITH_TIMEOUT(
        spacingIsClear(),
        qPrintable(QStringLiteral("Connection text right=%1, button right=%2, RC left=%3, text height=%4")
                       .arg(labelBounds().right())
                       .arg(button->mapRectToScene(button->boundingRect()).right())
                       .arg(rc->mapRectToScene(rc->boundingRect()).left())
                       .arg(labelBounds().height())),
        TestTimeout::shortMs());
}

void CaacFlightDataUITest::_mapZoomControls_data()
{
    QTest::addColumn<QSize>("windowSize");
    QTest::newRow("short") << QSize(800, 600);
    QTest::newRow("normal") << QSize(1280, 800);
    QTest::newRow("large") << QSize(1600, 1200);
}

void CaacFlightDataUITest::_mapZoomControls()
{
    QFETCH(QSize, windowSize);
    startUI();
    if (QTest::currentTestFailed()) {
        return;
    }
    _resizeWindow(windowSize);
    auto* map = findVisibleItem(_rootItem, QStringLiteral("planView_map"));
    auto* zoomIn = findVisibleItem(_rootItem, QStringLiteral("caacMapZoomIn"));
    auto* zoomOut = findVisibleItem(_rootItem, QStringLiteral("caacMapZoomOut"));
    QVERIFY(map && zoomIn && zoomOut);
    const auto controlsFit = [&]() {
        for (auto* button : {zoomIn, zoomOut}) {
            const auto bounds = button->mapRectToScene(button->boundingRect());
            if (bounds.width() <= 0 || bounds.height() <= 0 || bounds.top() < 0 ||
                bounds.bottom() > _window->height()) {
                return false;
            }
            for (auto* parent = button->parentItem(); parent; parent = parent->parentItem()) {
                if (parent->clip() && !parent->mapRectToScene(parent->boundingRect()).contains(bounds)) {
                    return false;
                }
            }
        }
        return true;
    };
    QTRY_VERIFY_WITH_TIMEOUT(controlsFit(), TestTimeout::shortMs());
    const qreal zoom = (map->property("minimumZoomLevel").toReal() + map->property("maximumZoomLevel").toReal()) / 2;
    QVERIFY(map->setProperty("zoomLevel", zoom));
    QVERIFY(clickButton(QStringLiteral("caacMapZoomIn")));
    QTRY_COMPARE_WITH_TIMEOUT(map->property("zoomLevel").toReal(), zoom + 1, TestTimeout::shortMs());
    QVERIFY(clickButton(QStringLiteral("caacMapZoomOut")));
    QTRY_COMPARE_WITH_TIMEOUT(map->property("zoomLevel").toReal(), zoom, TestTimeout::shortMs());
}

void CaacFlightDataUITest::_safetySpeedUnits_data()
{
    QTest::addColumn<int>("firmwareType");
    QTest::addColumn<int>("vehicleType");
    QTest::addColumn<QString>("parameterName");
    QTest::addColumn<QString>("logicalName");
    QTest::addColumn<double>("rawUnitsPerMeterPerSecond");
    QTest::addColumn<double>("targetMetersPerSecond");
    QTest::newRow("px4-meters-per-second")
        << int(MAV_AUTOPILOT_PX4) << int(MAV_TYPE_QUADROTOR) << QStringLiteral("MPC_XY_CRUISE")
        << QStringLiteral("MPC_XY_CRUISE") << 1.0 << 7.0;
    QTest::newRow("apm-current-meters-per-second")
        << int(MAV_AUTOPILOT_ARDUPILOTMEGA) << int(MAV_TYPE_QUADROTOR) << QStringLiteral("WP_SPD")
        << QStringLiteral("MPC_XY_CRUISE") << 1.0 << 7.0;
    QTest::newRow("apm-legacy-centimeters-per-second")
        << int(MAV_AUTOPILOT_ARDUPILOTMEGA) << int(MAV_TYPE_SUBMARINE) << QStringLiteral("PILOT_SPEED_UP")
        << QStringLiteral("MPC_Z_VEL_MAX_UP") << 100.0 << 3.25;
}

void CaacFlightDataUITest::_safetySpeedUnits()
{
    QFETCH(int, firmwareType);
    QFETCH(int, vehicleType);
    QFETCH(QString, parameterName);
    QFETCH(QString, logicalName);
    QFETCH(double, rawUnitsPerMeterPerSecond);
    QFETCH(double, targetMetersPerSecond);
    startUI();
    if (QTest::currentTestFailed()) {
        return;
    }
    Vehicle* vehicle = nullptr;
    auto mock = connectMockLinkAndWaitReady(
        [firmwareType, vehicleType]() {
            auto* config = new MockConfiguration(QStringLiteral("Safety speed units"));
            config->setFirmwareType(static_cast<MAV_AUTOPILOT>(firmwareType));
            config->setVehicleType(static_cast<MAV_TYPE>(vehicleType));
            config->setSendRadioStatus(false);
            config->setSendStatusText(false);
            return MockLink::startMockLink(config);
        },
        vehicle);
    QVERIFY(mock && vehicle);
    const auto disconnect = qScopeGuard([&]() { disconnectMockLink(mock); });
    QVERIFY(vehicle->parameterManager()->parameterExists(vehicle->defaultComponentId(), parameterName));
    auto* speed = vehicle->parameterManager()->getParameter(vehicle->defaultComponentId(), parameterName);
    QVERIFY(speed);
    QCOMPARE(speed->rawUnits(), rawUnitsPerMeterPerSecond == 100.0 ? QStringLiteral("cm/s") : QStringLiteral("m/s"));
    QVERIFY(speed->enumStrings().isEmpty());
    const double initialRaw = 4.0 * rawUnitsPerMeterPerSecond;
    if (speed->rawValue().toDouble() != initialRaw) {
        QSignalSpy updated(speed, &Fact::vehicleUpdated);
        speed->setRawValue(initialRaw);
        QTRY_VERIFY_WITH_TIMEOUT(!updated.isEmpty(), TestTimeout::mediumMs());
    }
    QTRY_COMPARE_WITH_TIMEOUT(speed->rawValue().toDouble(), initialRaw, TestTimeout::mediumMs());
    QVERIFY(clickButton(QStringLiteral("toolbar_qgcLogo")));
    QVERIFY(clickButtonScrolled(QStringLiteral("toolbar_safety"), QStringLiteral("caacSettingsNavigationScroller")));
    const QString fieldName = QStringLiteral("caacSafety_") + logicalName + QStringLiteral("_value");
    const QString scroller = QStringLiteral("caacSafetyScroller");
    auto* field = findVisibleItemScrolled(fieldName, scroller);
    QVERIFY(field && field->isEnabled());
    QTRY_COMPARE_WITH_TIMEOUT(field->property("text").toString().toDouble(), 4.0, TestTimeout::mediumMs());
    auto* units = findVisibleItem(_rootItem, QStringLiteral("caacSafety_") + logicalName + QStringLiteral("_units"));
    QVERIFY(units);
    QCOMPARE(units->property("text").toString(), QStringLiteral("m/s"));
    const int writes = mock->receivedMavlinkMessageCount(MAVLINK_MSG_ID_PARAM_SET);
    QVERIFY(clickButtonScrolled(fieldName, scroller));
    QTest::keyClick(_window, Qt::Key_A, Qt::ControlModifier);
    const QString editedText = QString::number(targetMetersPerSecond);
    for (const auto character : editedText) {
        QTest::keyClick(_window, character.toLatin1());
    }
    QCOMPARE(field->property("text").toString(), editedText);
    QTest::keyClick(_window, Qt::Key_Tab);
    QCOMPARE(speed->rawValue().toDouble(), initialRaw);
    QCOMPARE(mock->receivedMavlinkMessageCount(MAVLINK_MSG_ID_PARAM_SET), writes);
    QSignalSpy written(speed, &Fact::vehicleUpdated);
    QVERIFY(clickButtonScrolled(QStringLiteral("caacSafetyConfirm_MPC_XY_CRUISE"), scroller));
    QTRY_COMPARE_WITH_TIMEOUT(speed->rawValue().toDouble(), targetMetersPerSecond * rawUnitsPerMeterPerSecond,
                              TestTimeout::mediumMs());
    QTRY_VERIFY_WITH_TIMEOUT(mock->receivedMavlinkMessageCount(MAVLINK_MSG_ID_PARAM_SET) > writes,
                             TestTimeout::mediumMs());
    QTRY_VERIFY_WITH_TIMEOUT(!written.isEmpty(), TestTimeout::mediumMs());
    QTRY_COMPARE_WITH_TIMEOUT(field->property("text").toString().toDouble(), targetMetersPerSecond,
                              TestTimeout::mediumMs());
}

void CaacFlightDataUITest::_safetyFailsafeEnums_data()
{
    QTest::addColumn<int>("firmwareType");
    QTest::newRow("px4") << int(MAV_AUTOPILOT_PX4);
    QTest::newRow("apm") << int(MAV_AUTOPILOT_ARDUPILOTMEGA);
}

void CaacFlightDataUITest::_safetyFailsafeEnums()
{
    QFETCH(int, firmwareType);
    startUI();
    if (QTest::currentTestFailed()) {
        return;
    }
    auto* language = SettingsManager::instance()->appSettings()->qLocaleLanguage();
    const QVariant savedLanguage = language->rawValue();
    const auto restoreLanguage = qScopeGuard([&]() {
        if (language->rawValue() != savedLanguage) {
            qgcApp()->resetRebootMessageDebounce();
            expectAppMessage(QRegularExpression(
                QRegularExpression::escape(Fact::tr("Restart application for changes to take effect."))));
            language->setRawValue(savedLanguage);
            (void) acceptDialog(5000);
            verifyExpectedLogMessage();
        }
    });
    ignoreLogMessage("API.QGCApplication", QtWarningMsg,
                     QRegularExpression(QStringLiteral("Qt lib localization for .* is not present")));
    ignoreLogMessage("API.QGCApplication", QtWarningMsg,
                     QRegularExpression(QStringLiteral("Error loading source localization for .*")));
    ignoreLogMessage("API.QGCApplication", QtWarningMsg,
                     QRegularExpression(QStringLiteral("Error loading json localization for .*")));
    if (language->rawValue().toInt() != QLocale::Chinese) {
        qgcApp()->resetRebootMessageDebounce();
        expectAppMessage(QRegularExpression(
            QRegularExpression::escape(Fact::tr("Restart application for changes to take effect."))));
        language->setRawValue(QLocale::Chinese);
        QVERIFY(acceptDialog(5000));
        verifyExpectedLogMessage();
    }
    Vehicle* vehicle = nullptr;
    auto mock = connectMockLinkAndWaitReady(
        [firmwareType]() {
            auto* config = new MockConfiguration(QStringLiteral("Chinese failsafe choices"));
            config->setFirmwareType(static_cast<MAV_AUTOPILOT>(firmwareType));
            config->setVehicleType(MAV_TYPE_QUADROTOR);
            config->setSendStatusText(false);
            return MockLink::startMockLink(config);
        },
        vehicle);
    QVERIFY(mock && vehicle);
    const auto disconnect = qScopeGuard([&]() { disconnectMockLink(mock); });
    _resizeWindow(QSize(1000, 800));
    QVERIFY(clickButton(QStringLiteral("toolbar_qgcLogo")));
    QVERIFY(clickButtonScrolled(QStringLiteral("toolbar_safety"), QStringLiteral("caacSettingsNavigationScroller")));
    const int writes = mock->receivedMavlinkMessageCount(MAVLINK_MSG_ID_PARAM_SET);
    const QString scroller = QStringLiteral("caacSafetyScroller");
    const QRegularExpression chinese(QStringLiteral("[\\x{3400}-\\x{9fff}]"));
    for (const QString& logicalName : {QStringLiteral("NAV_RCL_ACT"), QStringLiteral("NAV_DLL_ACT")}) {
        const QString prefix = QStringLiteral("caacSafety_") + logicalName;
        auto* combo = findVisibleItemScrolled(prefix + QStringLiteral("_choice"), scroller);
        QVERIFY(combo && combo->isEnabled());
        auto* row = combo->parentItem();
        while (row && row->objectName() != prefix) {
            row = row->parentItem();
        }
        QVERIFY(row);
        auto* fact = row->property("fact").value<Fact*>();
        QVERIFY(fact);
        const auto originalStrings = fact->enumStrings();
        const auto originalValues = fact->enumValues();
        const QVariant originalValue = fact->rawValue();
        QCOMPARE(combo->property("count").toInt(), originalStrings.size());
        QStringList translated;
        for (int index = 0; index < originalStrings.size(); ++index) {
            QVariant display;
            QVERIFY(QMetaObject::invokeMethod(combo, "displayString", Q_RETURN_ARG(QVariant, display),
                                              Q_ARG(QVariant, originalStrings.at(index))));
            const QString text = display.toString();
            QVERIFY2(chinese.match(text).hasMatch(), qPrintable(originalStrings.at(index) + " -> " + text));
            QVERIFY(!text.contains(QRegularExpression(QStringLiteral("[()（）]"))));
            QVERIFY(!text.contains(QStringLiteral("FS_OPTIONS")));
            translated.append(text);
        }
        auto* selectedText = findItem(combo, prefix + QStringLiteral("_choiceText"));
        QVERIFY(selectedText);
        QTRY_COMPARE_WITH_TIMEOUT(selectedText->property("text").toString(), translated.at(fact->enumIndex()),
                                  TestTimeout::shortMs());
        QTRY_VERIFY_WITH_TIMEOUT(selectedText->height() + 1 >= selectedText->implicitHeight(), TestTimeout::shortMs());
        QVERIFY(clickButtonScrolled(prefix + QStringLiteral("_choice"), scroller));
        auto* popup = combo->property("popup").value<QObject*>();
        QVERIFY(popup);
        QTRY_VERIFY_WITH_TIMEOUT(popup->property("opened").toBool(), TestTimeout::shortMs());
        auto* list = popup->property("contentItem").value<QQuickItem*>();
        QVERIFY(list);
        for (int index = 0; index < translated.size(); ++index) {
            QVERIFY(QMetaObject::invokeMethod(list, "positionViewAtIndex", Q_ARG(int, index), Q_ARG(int, 0)));
            const QString optionName = prefix + QStringLiteral("_option_%1_text").arg(index);
            QTRY_VERIFY_WITH_TIMEOUT(findItem(list, optionName), TestTimeout::shortMs());
            auto* text = findItem(list, optionName);
            QCOMPARE(text->property("text").toString(), translated.at(index));
            auto* separator = findItem(list, prefix + QStringLiteral("_option_%1_separator").arg(index));
            QVERIFY(separator);
            QCOMPARE(separator->isVisible(), index < translated.size() - 1);
            QTRY_VERIFY_WITH_TIMEOUT(text->height() + 1 >= text->implicitHeight(), TestTimeout::shortMs());
            QTRY_VERIFY_WITH_TIMEOUT(text->property("paintedWidth").toReal() <= text->width() + 1,
                                     TestTimeout::shortMs());
        }
        QTest::keyClick(_window, Qt::Key_Escape);
        QTRY_VERIFY_WITH_TIMEOUT(!popup->property("opened").toBool(), TestTimeout::shortMs());
        QCOMPARE(fact->enumStrings(), originalStrings);
        QCOMPARE(fact->enumValues(), originalValues);
        QCOMPARE(fact->rawValue(), originalValue);
    }
    QCOMPARE(mock->receivedMavlinkMessageCount(MAVLINK_MSG_ID_PARAM_SET), writes);
}

void CaacFlightDataUITest::_safetyAndUomTelemetry()
{
    startUI();
    if (QTest::currentTestFailed()) {
        return;
    }
    QVERIFY(verifyText(QStringLiteral("caacRCRSSIIndicatorValue"), QStringLiteral("--"),
                       QStringLiteral("offline RC signal")));
    QVERIFY(verifyText(QStringLiteral("caacTelemetryRSSIIndicatorValue"), QStringLiteral("--"),
                       QStringLiteral("offline data link signal")));
    Vehicle* vehicle = nullptr;
    auto mock = connectMockLinkAndWaitReady(
        []() {
            auto* config = new MockConfiguration(QStringLiteral("Safety and UOM telemetry test"));
            config->setFirmwareType(MAV_AUTOPILOT_PX4);
            config->setVehicleType(MAV_TYPE_QUADROTOR);
            config->setSendRadioStatus(false);
            config->setSendStatusText(false);
            return MockLink::startMockLink(config);
        },
        vehicle);
    QVERIFY(mock && vehicle);
    const auto disconnect = qScopeGuard([&]() { disconnectMockLink(mock); });
    vehicle->rcRSSI()->setRawValue(0);
    QTRY_VERIFY_WITH_TIMEOUT(
        verifyText(QStringLiteral("caacRCRSSIIndicatorValue"), QStringLiteral("0%"), QStringLiteral("RC signal")),
        TestTimeout::mediumMs());
    vehicle->rcRSSI()->setRawValue(80);
    QTRY_VERIFY_WITH_TIMEOUT(
        verifyText(QStringLiteral("caacRCRSSIIndicatorValue"), QStringLiteral("80%"), QStringLiteral("RC signal")),
        TestTimeout::mediumMs());
    vehicle->rcRSSI()->setRawValue(255);
    QTRY_VERIFY_WITH_TIMEOUT(
        verifyText(QStringLiteral("caacRCRSSIIndicatorValue"), QStringLiteral("--"), QStringLiteral("RC signal")),
        TestTimeout::mediumMs());
    mavlink_message_t radio{};
    mavlink_msg_radio_status_pack(vehicle->id(), MAV_COMP_ID_AUTOPILOT1, &radio, uint8_t(-60), uint8_t(-65), 100, 0, 0,
                                  0, 0);
    vehicle->radioStatusFactGroup()->handleMessage(vehicle, radio);
    QTRY_VERIFY_WITH_TIMEOUT(verifyText(QStringLiteral("caacTelemetryRSSIIndicatorValue"), QStringLiteral("-60 dBm"),
                                        QStringLiteral("data link signal")),
                             TestTimeout::mediumMs());
    for (const auto* name : {"caacTopFlightTime", "caacConnectionStatusButton", "caacRCRSSIIndicatorValue",
                             "caacTelemetryRSSIIndicatorValue"}) {
        auto* indicator = findVisibleItem(_rootItem, QLatin1String(name));
        QVERIFY(indicator);
        QVERIFY(indicator->mapToScene(QPointF()).x() > _rootItem->width() / 2);
    }
    QVERIFY(clickButton(QStringLiteral("toolbar_qgcLogo")));
    const auto navigation = QStringLiteral("caacSettingsNavigationScroller");
    QVERIFY(clickButtonScrolled(QStringLiteral("toolbar_safety"), navigation));
    auto* safety = findVisibleItem(_rootItem, QStringLiteral("caacSafetySettings"));
    QVERIFY(safety);
    auto* cruise = vehicle->parameterManager()->getParameter(vehicle->defaultComponentId(), "MPC_XY_CRUISE");
    QVERIFY(cruise);
    const auto original = cruise->rawValue();
    auto* field =
        findVisibleItemScrolled(QStringLiteral("caacSafety_MPC_XY_CRUISE_value"), QStringLiteral("caacSafetyScroller"));
    QVERIFY(field && field->isEnabled());
    QVERIFY(
        clickButtonScrolled(QStringLiteral("caacSafety_MPC_XY_CRUISE_value"), QStringLiteral("caacSafetyScroller")));
    QTest::keyClick(_window, Qt::Key_A, Qt::ControlModifier);
    QTest::keyClick(_window, Qt::Key_7);
    QTest::keyClick(_window, Qt::Key_Tab);
    QCOMPARE(cruise->rawValue(), original);
    const int writes = mock->receivedMavlinkMessageCount(MAVLINK_MSG_ID_PARAM_SET);
    QVERIFY(
        clickButtonScrolled(QStringLiteral("caacSafetyConfirm_MPC_XY_CRUISE"), QStringLiteral("caacSafetyScroller")));
    QTRY_COMPARE_WITH_TIMEOUT(cruise->rawValue().toDouble(), 7.0, TestTimeout::mediumMs());
    QTRY_VERIFY_WITH_TIMEOUT(mock->receivedMavlinkMessageCount(MAVLINK_MSG_ID_PARAM_SET) > writes,
                             TestTimeout::mediumMs());
    uint8_t mac[20]{};
    uint8_t serial[20] = "CAAC-TEST-SERIAL";
    mavlink_message_t identity{};
    mavlink_msg_open_drone_id_basic_id_pack(vehicle->id(), MAV_COMP_ID_ODID_TXRX_1, &identity, vehicle->id(), 0, mac,
                                            MAV_ODID_ID_TYPE_SERIAL_NUMBER, MAV_ODID_UA_TYPE_HELICOPTER_OR_MULTIROTOR,
                                            serial);
    mock->respondWithMavlinkMessage(identity, true);
    QTRY_COMPARE_WITH_TIMEOUT(vehicle->remoteIDManager()->aircraftSerial(), QStringLiteral("CAAC-TEST-SERIAL"),
                              TestTimeout::mediumMs());
    QVERIFY(clickButtonScrolled(QStringLiteral("toolbar_uom"), navigation));
    auto* uom = findVisibleItem(_rootItem, QStringLiteral("caacUOMSettings"));
    QVERIFY(uom);
    QCOMPARE(uom->property("vehicle").value<Vehicle*>(), vehicle);
    QCOMPARE(uom->property("serial").toString(), vehicle->remoteIDManager()->aircraftSerial());
    QVERIFY(clickButton(QStringLiteral("caacUOMSerialTab")));
    QVERIFY(clickButtonScrolled(QStringLiteral("caacUOMCopyAll"), QStringLiteral("caacUOMScroller")));
    QVERIFY(QGuiApplication::clipboard()->text().contains(QStringLiteral("CAAC-TEST-SERIAL")));
    QVERIFY(clickButton(QStringLiteral("caacUOMTelemetryTab")));
    auto* heading =
        findVisibleItemScrolled(QStringLiteral("caacUOMTelemetry_heading"), QStringLiteral("caacUOMScroller"));
    QVERIFY(heading);
    const auto expectedHeading = [&]() {
        return vehicle->heading()->cookedValueString() + " " + vehicle->heading()->cookedUnits();
    };
    vehicle->heading()->setRawValue(123.4);
    QTRY_COMPARE_WITH_TIMEOUT(heading->property("text").toString(), expectedHeading(), TestTimeout::mediumMs());
    vehicle->heading()->setRawValue(234.5);
    QTRY_COMPARE_WITH_TIMEOUT(heading->property("text").toString(), expectedHeading(), TestTimeout::mediumMs());
    QSignalSpy restorePending(vehicle->parameterManager(), &ParameterManager::pendingWritesChanged);
    cruise->setRawValue(original);
    QTRY_VERIFY_WITH_TIMEOUT(restorePending.count() >= 2 && !vehicle->parameterManager()->pendingWrites(),
                             TestTimeout::mediumMs());
}

void CaacFlightDataUITest::_dashboardInteractions()
{
    startUI();
    if (QTest::currentTestFailed()) {
        return;
    }
    Vehicle* vehicle = nullptr;
    auto mock = connectMockLinkAndWaitReady([]() { return MockLink::startPX4MockLink(); }, vehicle);
    QVERIFY(mock && vehicle);
    const auto disconnect = qScopeGuard([&]() { disconnectMockLink(mock); });
    auto* dashboard = findVisibleItem(_rootItem, QStringLiteral("caacFlightDashboard"));
    auto* footer = findVisibleItem(_rootItem, QStringLiteral("caacFlightTelemetryBar"));
    auto* arrow = findVisibleItem(_rootItem, QStringLiteral("caacTelemetryHeadingArrow"));
    QVERIFY(dashboard && footer && arrow);
    const auto previousHeading = vehicle->heading()->rawValue();
    vehicle->heading()->setRawValue(123.4);
    QTRY_VERIFY_WITH_TIMEOUT(std::abs(arrow->property("headingAngle").toDouble() - 123.4) < 1e-6,
                             TestTimeout::mediumMs());
    auto* compassValue = findVisibleItem(_rootItem, QStringLiteral("caacTelemetryHeadingValue"));
    QVERIFY(compassValue);
    QCOMPARE(compassValue->property("text").toString(), QStringLiteral("123.4°"));
    const auto valuePosition = compassValue->mapToItem(arrow, QPointF());
    QVERIFY(valuePosition.y() > arrow->height() / 2);
    QVERIFY(QRectF(0, 0, arrow->width(), arrow->height())
                .contains(QRectF(valuePosition, QSizeF(compassValue->width(), compassValue->height()))));
    if (const auto path = qgetenv("CAAC_HEADING_UI_PREVIEW"); !path.isEmpty()) {
        const auto capture = arrow->grabToImage();
        QVERIFY(capture);
        QSignalSpy ready(capture.data(), &QQuickItemGrabResult::ready);
        QTRY_VERIFY_WITH_TIMEOUT(!ready.isEmpty(), TestTimeout::mediumMs());
        QVERIFY(capture->saveToFile(QString::fromUtf8(path)));
    }
    auto* heading = findVisibleItem(_rootItem, QStringLiteral("caacDashboardHeadingValue"));
    QVERIFY(heading);
    QVERIFY(heading->property("text").toString().startsWith(vehicle->heading()->cookedValueString()));
    vehicle->heading()->setRawValue(previousHeading);
    for (const auto* key : {"Altitude", "Speed", "GPS", "Pitch", "Voltage", "HomeDistance", "Roll"}) {
        QVERIFY(
            findVisibleItem(_rootItem, QStringLiteral("caacDashboard") + QLatin1String(key) + QStringLiteral("Value")));
    }
    const auto beforeWidth = dashboard->width();
    const auto beforeHeight = dashboard->height();
    QVERIFY(clickButton(QStringLiteral("caacDashboardZoomButton")));
    QTRY_VERIFY_WITH_TIMEOUT(dashboard->width() > beforeWidth, TestTimeout::mediumMs());
    QVERIFY(clickButton(QStringLiteral("caacDashboardZoomButton")));
    QTRY_VERIFY_WITH_TIMEOUT(std::abs(dashboard->width() - beforeWidth) < 1, TestTimeout::mediumMs());
    QTRY_VERIFY_WITH_TIMEOUT(std::abs(dashboard->height() - beforeHeight) < 1, TestTimeout::mediumMs());
    QSignalSpy rendered(_window, &QQuickWindow::frameSwapped);
    _window->update();
    QTRY_VERIFY_WITH_TIMEOUT(!rendered.isEmpty(), TestTimeout::mediumMs());
    auto* drag = findVisibleItem(_rootItem, QStringLiteral("caacDashboardDragArea"));
    QVERIFY(drag);
    const auto press = drag->mapToScene(QPointF(drag->width() / 2, drag->height() / 3)).toPoint();
    const auto before = dashboard->mapToScene(QPointF());
    QTest::mousePress(_window, Qt::LeftButton, Qt::NoModifier, press);
    QCOMPARE(_window->mouseGrabberItem(), drag);
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
    QVERIFY(clickButtonScrolled(QStringLiteral("caacMissionDetailsButton"), QStringLiteral("caacMainToolsScroller")));
    QVERIFY(verifyVisibility(QStringLiteral("caacMissionDetailsPanel"), true, QStringLiteral("mission panel")));
    QVERIFY(verifyText(QStringLiteral("caacMissionTarget"),
                       QCoreApplication::translate("CaacMissionDetails", "Unavailable"),
                       QStringLiteral("no target without aircraft")));
    QVERIFY(clickButtonScrolled(QStringLiteral("caacFlightDataButton"), QStringLiteral("caacMainToolsScroller")));
    QVERIFY(verifyVisibility(QStringLiteral("caacMissionDetailsPanel"), false, QStringLiteral("mission panel closed")));
    QVERIFY(verifyVisibility(QStringLiteral("caacFlyDataPanel"), true, QStringLiteral("data panel opened")));
    QVERIFY(clickButtonScrolled(QStringLiteral("caacMissionDetailsButton"), QStringLiteral("caacMainToolsScroller")));
    QVERIFY(verifyVisibility(QStringLiteral("caacFlyDataPanel"), false, QStringLiteral("data panel closed")));
    QVERIFY(clickButtonScrolled(QStringLiteral("caacMissionDetailsButton"), QStringLiteral("caacMainToolsScroller")));
    QVERIFY(verifyVisibility(QStringLiteral("caacMissionDetailsPanel"), false, QStringLiteral("toggle current panel")));
    QVERIFY(clickButtonScrolled(QStringLiteral("caacMapSettingsButton"), QStringLiteral("caacMainToolsScroller")));
    QVERIFY(verifyVisibility(QStringLiteral("caacMissionDetailsPanel"), false, QStringLiteral("only map panel")));
    QVERIFY(verifyVisibility(QStringLiteral("caacFlyDataPanel"), false, QStringLiteral("only map panel")));
    auto* mapSettings = findVisibleItem(_rootItem, QStringLiteral("caacMapSettingsPanel"));
    QVERIFY(mapSettings);
    auto* mapFacts = SettingsManager::instance()->flightMapSettings();
    const auto originalProvider = mapFacts->mapProvider()->rawValue();
    const auto originalMapType = mapFacts->mapType()->rawValue();
    const auto restoreMap = qScopeGuard([&]() {
        mapFacts->mapProvider()->setRawValue(originalProvider);
        mapFacts->mapType()->setRawValue(originalMapType);
    });
    auto* provider = findVisibleItem(_rootItem, QStringLiteral("caacMapProvider"));
    QVERIFY(provider && provider->property("count").toInt() > 1);
    QVERIFY(scrollIntoView(provider, QStringLiteral("caacFlyPanelsScroller")));
    QVERIFY(clickButton(QStringLiteral("caacMapProvider")));
    QTest::keyClick(_window, Qt::Key_Home);
    QTest::keyClick(_window, Qt::Key_Down);
    QTest::keyClick(_window, Qt::Key_Return);
    QTRY_COMPARE_WITH_TIMEOUT(mapFacts->mapProvider()->rawValue().toString(),
                              provider->property("currentText").toString(), TestTimeout::mediumMs());
    QVERIFY(QGCMapEngineManager::mapTypeList(mapFacts->mapProvider()->rawValue().toString())
                .contains(mapFacts->mapType()->rawValue().toString()));
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
    QVERIFY(clickButtonScrolled(QStringLiteral("caacMapSettingsButton"), QStringLiteral("caacMainToolsScroller")));
    QVERIFY(clickButtonScrolled(QStringLiteral("caacFlightDataButton"), QStringLiteral("caacMainToolsScroller")));
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
        QVERIFY(!findVisibleItem(_rootItem, QStringLiteral("indicatorDrawerExpandButton"), 0));
        QVERIFY(!findVisibleItem(_rootItem, QStringLiteral("caacGeofenceAction"), 0));
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

void CaacFlightDataUITest::_vehicleRebootConfirmation()
{
    runWithMockLink(
        [] { return MockLink::startPX4MockLink(); },
        [&](QPointer<MockLink> mock, Vehicle* vehicle) {
            auto* manager = vehicle->parameterManager();
            auto* fact = manager->getParameter(ParameterManager::defaultComponentId, QStringLiteral("BAT1_V_CHARGED"));
            QVERIFY(fact);
            QVERIFY(fact->vehicleRebootRequired());
            const int systemId = vehicle->id();
            QSignalSpy messages(MAVLinkProtocol::instance(), &MAVLinkProtocol::messageReceived);
            QSignalSpy updated(fact, &Fact::vehicleUpdated);
            const double original = fact->rawValue().toDouble();
            const QRegularExpression notice(QRegularExpression::escape(QGCApplication::tr(
                "Vehicle reboot required for changes to take effect. Select OK to reboot the flight controller now.")));
            qgcApp()->resetRebootMessageDebounce();
            expectAppMessage(notice);
            fact->setRawValue(original + 0.1);
            verifyExpectedLogMessage();
            auto* accept = findVisibleItem(_rootItem, QStringLiteral("popupDialog_acceptButton"));
            QVERIFY(accept);
            QTRY_VERIFY_WITH_TIMEOUT(!updated.isEmpty() && !manager->pendingWrites(), TestTimeout::mediumMs());
            manager->setPendingWritesForTest(true);
            QTRY_VERIFY(!accept->isEnabled());
            manager->setPendingWritesForTest(false);
            QTRY_VERIFY(accept->isEnabled());
            vehicle->setArmed(true, false);
            QTRY_VERIFY(vehicle->armed());
            QTRY_VERIFY(!accept->isEnabled());
            vehicle->setArmed(false, false);
            QTRY_VERIFY(!vehicle->armed());
            QTRY_VERIFY(accept->isEnabled());
            QVERIFY(rejectDialog());
            QTRY_VERIFY_WITH_TIMEOUT(!dialogVisible(QGCApplication::tr("Notice")), TestTimeout::shortMs());
            const auto rebootAccepted = [&] {
                for (const auto& result : messages) {
                    const auto message = result.at(1).value<mavlink_message_t>();
                    if (message.sysid != systemId || message.msgid != MAVLINK_MSG_ID_COMMAND_ACK) {
                        continue;
                    }
                    mavlink_command_ack_t ack{};
                    mavlink_msg_command_ack_decode(&message, &ack);
                    if (ack.command == MAV_CMD_PREFLIGHT_REBOOT_SHUTDOWN && ack.result == MAV_RESULT_ACCEPTED) {
                        return true;
                    }
                }
                return false;
            };
            QVERIFY(!rebootAccepted());
            QCOMPARE(mock->receivedMavCommandCount(MAV_CMD_PREFLIGHT_REBOOT_SHUTDOWN), 0);
            qgcApp()->resetRebootMessageDebounce();
            expectAppMessage(notice);
            fact->setRawValue(original + 0.2);
            verifyExpectedLogMessage();
            QTRY_VERIFY_WITH_TIMEOUT(!manager->pendingWrites(), TestTimeout::mediumMs());
            QTRY_COMPARE_WITH_TIMEOUT(
                mock->paramValue(MAV_COMP_ID_AUTOPILOT1, QStringLiteral("BAT1_V_CHARGED")).toFloat(),
                static_cast<float>(original + 0.2), TestTimeout::mediumMs());
            QVERIFY(!rebootAccepted());
            QCOMPARE(mock->receivedMavCommandCount(MAV_CMD_PREFLIGHT_REBOOT_SHUTDOWN), 0);
            auto* confirm = findVisibleItem(_rootItem, QStringLiteral("popupDialog_acceptButton"));
            QVERIFY(confirm);
            QTRY_VERIFY_WITH_TIMEOUT(confirm->isEnabled(), TestTimeout::mediumMs());
            QVERIFY(acceptDialog());
            QTRY_VERIFY_WITH_TIMEOUT(rebootAccepted(), TestTimeout::mediumMs());
            QTRY_VERIFY_WITH_TIMEOUT(!MultiVehicleManager::instance()->activeVehicle(), TestTimeout::mediumMs());
        });
}

UT_REGISTER_TEST(CaacFlightDataUITest, TestLabel::Integration, TestLabel::Vehicle)

#include "CaacFlightDataUITest.moc"
