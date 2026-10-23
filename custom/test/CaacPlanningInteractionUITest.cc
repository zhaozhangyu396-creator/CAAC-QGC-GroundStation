#include <algorithm>
#include <cmath>
#include <memory>
#include <utility>

#include <QtCore/QDir>
#include <QtCore/QJsonArray>
#include <QtCore/QLineF>
#include <QtCore/QRectF>
#include <QtCore/QRegularExpression>
#include <QtCore/QScopeGuard>
#include <QtGui/QClipboard>
#include <QtGui/QGuiApplication>
#include <QtGui/QImage>
#include <QtGui/QScreen>
#include <QtQml/QQmlExpression>
#include <QtQuick/QQuickItem>
#include <QtQuick/QQuickItemGrabResult>
#include <QtQuick/QQuickWindow>
#include <QtTest/QSignalSpy>
#include <QtTest/QTest>

#include "AppSettings.h"
#include "CaacCorePlugin.h"
#include "CaacRouteEditorController.h"
#include "CaacSimulationController.h"
#include "Fact.h"
#include "FlightPathSegment.h"
#include "GeoFenceController.h"
#include "GeoFenceManager.h"
#include "Joystick.h"
#include "JoystickManager.h"
#include "JoystickManagerSettings.h"
#include "JoystickSettings.h"
#include "MissionController.h"
#include "MissionSettingsItem.h"
#include "MockJoystick.h"
#include "MockLink.h"
#include "MultiVehicleManager.h"
#include "ParameterManager.h"
#include "PlanMasterController.h"
#include "QGCFencePolygon.h"
#include "QGCGeo.h"
#include "QGroundControlQmlGlobal.h"
#include "QmlUITestBase.h"
#include "RemoteIDSettings.h"
#include "SettingsManager.h"
#include "SimpleMissionItem.h"
#include "SpeedSection.h"
#include "Vehicle.h"

class CaacPlanningInteractionUITest : public QmlUITestBase
{
    Q_OBJECT
private slots:
    void _pointMenusAndOrientation();
    void _liveRoutePreview();
    void _homeRelativeEditing();
    void _homePreviewInsertion();
    void _dragSnappingAndRelations();
    void _synchronizeBasicsUI();
    void _calculatorUI();
    void _preferencesUI();
    void _newSettingsPages();
    void _applicationSettingsCollapse();
    void _calibrationAndSwitchSettings();
    void _waypointDragRemainsResponsive_data();
    void _waypointDragRemainsResponsive();
    void _statisticsPanelInteraction();
    void _mapContextClear();
    void _homeAltitudeAutoSave();
    void _batteryAndFenceSettings();
};

void CaacPlanningInteractionUITest::_applicationSettingsCollapse()
{
    startUI();
    if (QTest::currentTestFailed()) {
        return;
    }
    QVERIFY(clickButton(QStringLiteral("toolbar_qgcLogo")));
    QVERIFY(
        clickButtonScrolled(QStringLiteral("toolbar_viewSettings"), QStringLiteral("caacSettingsNavigationScroller")));
    auto* settings = findVisibleItem(_rootItem, QStringLiteral("appSettings_root"));
    auto* general = findVisibleItem(settings, QStringLiteral("settingsButton_General"));
    auto* loader = findItem(settings, QStringLiteral("settings_rightPanel"));
    QVERIFY(settings && general && loader);
    const QString scroller = QStringLiteral("settings_buttonList");
    const QString arrow = QStringLiteral("settingsButton_General_expandToggle");
    if (general->property("expanded").toBool()) {
        QVERIFY(clickButtonScrolled(arrow, scroller));
    }
    QTRY_VERIFY_WITH_TIMEOUT(!general->property("expanded").toBool(), TestTimeout::shortMs());
    QVERIFY(clickButtonScrolled(arrow, scroller));
    QTRY_VERIFY_WITH_TIMEOUT(general->property("expanded").toBool(), TestTimeout::shortMs());
    auto* section = findVisibleItem(settings, QStringLiteral("settingsSection_General_0"));
    QVERIFY(section);
    auto* sectionText = section->property("contentItem").value<QQuickItem*>();
    QVERIFY(sectionText);
    const QString searchText = sectionText->property("text").toString();
    QVERIFY(clickButtonScrolled(QStringLiteral("settingsSection_General_0"), scroller));
    const QPointer<QQuickItem> page = loader->property("item").value<QQuickItem*>();
    QVERIFY(page);
    const QVariant selectedSection = settings->property("_selectedSectionIndex");
    QCOMPARE(selectedSection.toInt(), 0);
    QVERIFY(clickButton(QStringLiteral("settings_backToMenu")));
    QVERIFY(clickButtonScrolled(arrow, scroller));
    QTRY_VERIFY_WITH_TIMEOUT(!general->property("expanded").toBool(), TestTimeout::shortMs());
    QVERIFY(!findVisibleItem(settings, QStringLiteral("settingsSection_General_0"), 0));
    QCOMPARE(loader->property("item").value<QQuickItem*>(), page.data());
    QCOMPARE(settings->property("_selectedSectionIndex"), selectedSection);
    QVERIFY(clickButtonScrolled(arrow, scroller));
    QTRY_VERIFY_WITH_TIMEOUT(general->property("expanded").toBool(), TestTimeout::shortMs());
    QVERIFY(clickButtonScrolled(arrow, scroller));
    QTRY_VERIFY_WITH_TIMEOUT(!general->property("expanded").toBool(), TestTimeout::shortMs());
    QCOMPARE(loader->property("item").value<QQuickItem*>(), page.data());
    QCOMPARE(settings->property("_selectedSectionIndex"), selectedSection);
    auto* search = findVisibleItem(settings, QStringLiteral("settings_searchField"));
    QVERIFY(search && !searchText.isEmpty());
    QVERIFY(search->setProperty("text", searchText));
    QTRY_VERIFY_WITH_TIMEOUT(general->property("expanded").toBool(), TestTimeout::shortMs());
    QVERIFY(clickButtonScrolled(arrow, scroller));
    QTRY_VERIFY_WITH_TIMEOUT(!general->property("expanded").toBool(), TestTimeout::shortMs());
    QVERIFY(!findVisibleItem(settings, QStringLiteral("settingsSection_General_0"), 0));
    QVERIFY(search->setProperty("text", QString()));
    QTRY_VERIFY_WITH_TIMEOUT(!general->property("expanded").toBool(), TestTimeout::shortMs());
    QCOMPARE(loader->property("item").value<QQuickItem*>(), page.data());
    QCOMPARE(settings->property("_selectedSectionIndex"), selectedSection);
}

void CaacPlanningInteractionUITest::_calibrationAndSwitchSettings()
{
    std::unique_ptr<MockJoystick> input;
    runWithMockLink(
        [] { return MockLink::startPX4MockLink(); },
        [&](QPointer<MockLink>, Vehicle* vehicle) {
            const QString joystickName = QStringLiteral("USB calibration preview");
            auto* manager = JoystickManager::instance();
            manager->init();
            input.reset(MockJoystick::create(joystickName, 5, 4, 0));
            QVERIFY(input && input->isValid());
            QTRY_VERIFY_WITH_TIMEOUT(manager->availableJoystickNames().contains(joystickName), TestTimeout::mediumMs());
            SettingsManager::instance()->joystickManagerSettings()->activeJoystickName()->setRawValue(joystickName);
            QTRY_VERIFY_WITH_TIMEOUT(manager->activeJoystick() && manager->activeJoystick()->name() == joystickName,
                                     TestTimeout::mediumMs());
            QCOMPARE(manager->activeJoystick()->settings()->exponentialPct()->shortDescription(),
                     QStringLiteral("EXP"));
            QCOMPARE(manager->activeJoystick()->settings()->axisFrequencyHz()->shortDescription(),
                     QStringLiteral("摇杆输入频率"));
            const QString previewDir = qEnvironmentVariable("CAAC_CONTROL_PREVIEW_DIR");
            if (!previewDir.isEmpty()) {
                _window->resize(1000, 760);
            }
            const auto capture = [&](const QString& name) {
                if (previewDir.isEmpty()) {
                    return true;
                }
                if (!QDir().mkpath(previewDir)) {
                    return false;
                }
                QSignalSpy frame(_window, &QQuickWindow::frameSwapped);
                _window->update();
                return waitForCondition([&]() { return !frame.isEmpty(); }, TestTimeout::mediumMs(), name) &&
                       _window->grabWindow().save(previewDir + "/" + name + ".png");
            };
            auto* plan = findVisibleItem(_rootItem, QStringLiteral("mainView_plan"));
            QVERIFY(plan);
            QQmlExpression open(qmlContext(plan), plan, QStringLiteral("mainWindow.showVehicleConfig()"));
            open.evaluate();
            QVERIFY(!open.hasError());
            const auto componentName = [](const char* context, const char* text) {
                return QStringLiteral("vehicleConfig_comp_") + QCoreApplication::translate(context, text).remove(' ');
            };
            const auto sidebar = QStringLiteral("vehicleConfig_sidebarFlickable");
            QVERIFY(clickButtonScrolled(componentName("JoystickComponent", "Joystick"), sidebar));
            const auto scroller = QStringLiteral("vehicleSetup_scroller");
            auto* next = findVisibleItemScrolled(QStringLiteral("remoteControlCalibrationNext"), scroller);
            QVERIFY(next);
            auto* calibration = qobject_cast<RemoteControlCalibrationController*>(
                next->parentItem()->parentItem()->property("controller").value<QObject*>());
            QVERIFY(calibration);
            QTRY_VERIFY_WITH_TIMEOUT(calibration->channelCount() >= 4, TestTimeout::mediumMs());
            QVERIFY(clickButtonScrolled(QStringLiteral("remoteControlCalibrationNext"), scroller));
            QTRY_COMPARE_WITH_TIMEOUT(next->property("text").toString(),
                                      QCoreApplication::translate("RemoteControlCalibrationController", "Next"),
                                      TestTimeout::mediumMs());
            QVERIFY(next->isEnabled());
            QVERIFY(next->property("settingsMenuStyle").toBool());
            QCOMPARE(next->property("textColor").value<QColor>(), QColor(Qt::white));
            QVERIFY(capture(QStringLiteral("calibration-next-white")));
            QVERIFY(clickButtonScrolled(QStringLiteral("remoteControlCalibrationCancel"), scroller));
            QVERIFY(clickButton(QStringLiteral("vehicleConfig_backToMenu")));
            QVERIFY(clickButtonScrolled(componentName("FlightModesComponent", "Flight Modes"), sidebar));
            if (findVisibleItem(_rootItem, QStringLiteral("vehicleConfig_backToMenu"), 0)) {
                QVERIFY(clickButton(QStringLiteral("vehicleConfig_backToMenu")));
            }
            QVERIFY(clickButtonScrolled(QStringLiteral("vehicleConfig_section_SwitchSettings"), sidebar));
            const QString channelName = QStringLiteral("px4SwitchChannel_RC_MAP_ARM_SW");
            auto* channel = findVisibleItem(_rootItem, channelName);
            QVERIFY(channel);
            QTRY_COMPARE_WITH_TIMEOUT(channel->property("currentText").toString(), QStringLiteral("未分配"),
                                      TestTimeout::mediumMs());
            QVERIFY(capture(QStringLiteral("switch-settings-chinese")));
            QVERIFY(clickButton(channelName));
            QTest::keyClick(_window, Qt::Key_Home);
            QTest::keyClick(_window, Qt::Key_Down);
            QTest::keyClick(_window, Qt::Key_Return);
            QTRY_COMPARE_WITH_TIMEOUT(channel->property("currentText").toString(), QStringLiteral("通道 1"),
                                      TestTimeout::mediumMs());
            QCOMPARE(vehicle->parameterManager()->getParameter(-1, QStringLiteral("RC_MAP_ARM_SW"))->rawValue().toInt(),
                     1);
            auto* joystick = manager->activeJoystick();
            for (int axis = 0; axis < 5; ++axis) {
                joystick->setAxisCalibration(axis, {});
            }
            using Calibration = RemoteControlCalibrationController;
            joystick->setFunctionForChannel(Calibration::stickFunctionRoll, 0);
            joystick->setFunctionForChannel(Calibration::stickFunctionPitch, 1);
            joystick->setFunctionForChannel(Calibration::stickFunctionYaw, 2);
            joystick->setFunctionForChannel(Calibration::stickFunctionThrottle, 3);
            joystick->settings()->calibrated()->setRawValue(true);
            const auto parameter = [vehicle](const char* name) {
                return vehicle->parameterManager()->getParameter(-1, QLatin1String(name));
            };
            parameter("RC_MAP_FLTMODE")->setRawValue(5);
            parameter("COM_FLTMODE1")->setRawValue(5);
            parameter("COM_FLTMODE4")->setRawValue(8);
            parameter("COM_FLTMODE6")->setRawValue(3);
            manager->setActiveJoystickEnabledForActiveVehicle(true);
            auto* source = findVisibleItem(_rootItem, QStringLiteral("px4ModeInputSource"));
            QVERIFY(source);
            QTRY_VERIFY_WITH_TIMEOUT(source->property("text").toString().contains(QStringLiteral("USB")),
                                     TestTimeout::mediumMs());
            auto* monitor = findVisibleItemScrolled(QStringLiteral("rcChannelMonitorValue_4"),
                                                    QStringLiteral("px4FlightModesScroller"));
            QVERIFY(monitor);
            QVERIFY(input->setAxis(4, Joystick::AxisMin));
            QTRY_COMPARE_WITH_TIMEOUT(monitor->property("rcValue").toInt(), 1000, TestTimeout::mediumMs());
            QVERIFY(capture(QStringLiteral("usb-channel-monitor-low")));
            QVERIFY(input->setAxis(4, Joystick::AxisMax));
            QTRY_COMPARE_WITH_TIMEOUT(monitor->property("rcValue").toInt(), 2000, TestTimeout::mediumMs());
            QVERIFY(capture(QStringLiteral("usb-channel-monitor-high")));
            QVERIFY(clickButton(QStringLiteral("vehicleConfig_backToMenu")));
            QVERIFY(clickButtonScrolled(QStringLiteral("vehicleConfig_section_FlightModes"), sidebar));
            for (const auto& position :
                 {qMakePair(Joystick::AxisMin, 1), qMakePair(0, 4), qMakePair(Joystick::AxisMax, 6)}) {
                QVERIFY(input->setAxis(4, position.first));
                auto* slot = findVisibleItemScrolled(QStringLiteral("px4FlightModeSlot_%1").arg(position.second),
                                                     QStringLiteral("px4FlightModesScroller"));
                QVERIFY(slot);
                QTRY_COMPARE_WITH_TIMEOUT(slot->property("color").value<QColor>(), QColor(Qt::yellow),
                                          TestTimeout::mediumMs());
                QVERIFY(capture(QStringLiteral("usb-mode-slot-%1").arg(position.second)));
            }
            QSignalSpy modeWrites(vehicle->parameterManager(), &ParameterManager::_paramSetSuccess);
            parameter("RC_MAP_FLTMODE")->setRawValue(6);
            auto* highSlot = findVisibleItem(_rootItem, QStringLiteral("px4FlightModeSlot_6"));
            QVERIFY(highSlot);
            QTRY_VERIFY_WITH_TIMEOUT(highSlot->property("color").value<QColor>() != QColor(Qt::yellow),
                                     TestTimeout::mediumMs());
            QTRY_VERIFY_WITH_TIMEOUT(!modeWrites.isEmpty(), TestTimeout::mediumMs());
            QCOMPARE(modeWrites.last()[1].toString(), QStringLiteral("RC_MAP_FLTMODE"));
            modeWrites.clear();
            parameter("RC_MAP_FLTMODE")->setRawValue(5);
            QTRY_VERIFY_WITH_TIMEOUT(!modeWrites.isEmpty(), TestTimeout::mediumMs());
            QCOMPARE(modeWrites.last()[1].toString(), QStringLiteral("RC_MAP_FLTMODE"));
            manager->setActiveJoystickEnabledForActiveVehicle(false);
            source = findVisibleItem(_rootItem, QStringLiteral("px4ModeInputSource"));
            QVERIFY(source);
            QTRY_VERIFY_WITH_TIMEOUT(source->property("text").toString().contains(QStringLiteral("飞控接收机")),
                                     TestTimeout::mediumMs());
            QTRY_VERIFY_WITH_TIMEOUT(!vehicle->parameterManager()->pendingWrites(), TestTimeout::mediumMs());
        });
}

void CaacPlanningInteractionUITest::_batteryAndFenceSettings()
{
    runWithMockLink([] { return MockLink::startPX4MockLink(); },
                    [&](QPointer<MockLink>, Vehicle* vehicle) {
                        auto* parameters = vehicle->parameterManager();
                        const auto fact = [parameters](const char* name) {
                            return parameters->getParameter(-1, QLatin1String(name));
                        };
                        fact("BAT1_N_CELLS")->setRawValue(12);
                        fact("BAT1_V_EMPTY")->setRawValue(3.6);
                        fact("BAT1_V_CHARGED")->setRawValue(4.05);
                        fact("BAT_LOW_THR")->setRawValue(0.15);
                        fact("BAT_CRIT_THR")->setRawValue(0.07);
                        QVERIFY(clickToolSelectDropdownButton(QStringLiteral("toolbar_safety")));
                        const auto scroller = QStringLiteral("caacSafetyScroller");
                        auto* low = findVisibleItemScrolled(QStringLiteral("caacLowBatteryVoltage"), scroller);
                        auto* critical =
                            findVisibleItemScrolled(QStringLiteral("caacCriticalBatteryVoltage"), scroller);
                        QVERIFY(low && critical);
                        QTRY_VERIFY_WITH_TIMEOUT(std::abs(low->property("text").toDouble() - 44.01) < 0.01,
                                                 TestTimeout::mediumMs());
                        QVERIFY(std::abs(critical->property("text").toDouble() - 43.58) < 0.01);
                        QVERIFY(!findItem(_rootItem, QStringLiteral("caacCriticalBatteryAction_0"))->isEnabled());
                        QVERIFY(low->setProperty("text", QStringLiteral("44.28")));
                        QVERIFY(QMetaObject::invokeMethod(low, "textEdited"));
                        QVERIFY(critical->setProperty("text", QStringLiteral("43.47")));
                        QVERIFY(QMetaObject::invokeMethod(critical, "textEdited"));
                        QVERIFY(clickButtonScrolled(QStringLiteral("caacConfirmBatteryProtection"), scroller));
                        QTRY_VERIFY_WITH_TIMEOUT(std::abs(fact("BAT_LOW_THR")->rawValue().toDouble() - 0.2) < 0.0001,
                                                 TestTimeout::mediumMs());
                        QVERIFY(std::abs(fact("BAT_CRIT_THR")->rawValue().toDouble() - 0.05) < 0.0001);
                        auto* fenceWidget = findItem(_rootItem, QStringLiteral("caacGeoFenceSettings"));
                        QVERIFY(fenceWidget);
                        auto* fence = fenceWidget->property("controller").value<GeoFenceController*>();
                        QVERIFY(fence && fence->supported());
                        auto* draw = findVisibleItemScrolled(QStringLiteral("caacFenceDraw"), scroller);
                        QVERIFY(draw);
                        QTRY_VERIFY_WITH_TIMEOUT(draw->isEnabled(), TestTimeout::mediumMs());
                        QVERIFY(clickButtonScrolled(QStringLiteral("caacFenceDraw"), scroller));
                        QTRY_COMPARE_WITH_TIMEOUT(fence->polygons()->count(), 1, TestTimeout::mediumMs());
                        QVERIFY(findVisibleItemScrolled(QStringLiteral("caacFencePreviewMap"), scroller));
                        const auto savedFence = fence->polygons()->value<QGCFencePolygon*>(0)->coordinateList();
                        QSignalSpy sent(vehicle->geoFenceManager(), &GeoFenceManager::sendComplete);
                        QVERIFY(clickButtonScrolled(QStringLiteral("caacFenceWrite"), scroller));
                        QTRY_VERIFY_WITH_TIMEOUT(!sent.isEmpty(), TestTimeout::longMs());
                        QCOMPARE(sent.last().first().toBool(), false);
                        fence->removeAll();
                        QSignalSpy loaded(fence, &GeoFenceController::loadComplete);
                        QVERIFY(clickButtonScrolled(QStringLiteral("caacFenceRead"), scroller));
                        QTRY_VERIFY_WITH_TIMEOUT(!loaded.isEmpty(), TestTimeout::longMs());
                        QCOMPARE(fence->polygons()->count(), 1);
                        auto* restoredPolygon = fence->polygons()->value<QGCFencePolygon*>(0);
                        QVERIFY(restoredPolygon->inclusion());
                        const auto restoredFence = restoredPolygon->coordinateList();
                        QCOMPARE(restoredFence.size(), savedFence.size());
                        for (int i = 0; i < savedFence.size(); ++i) {
                            QVERIFY(QGCGeo::geodesicDistance(restoredFence[i], savedFence[i]) < 0.02);
                        }
                    });
}

void CaacPlanningInteractionUITest::_homeAltitudeAutoSave()
{
    startUI();
    if (QTest::currentTestFailed()) {
        return;
    }
    auto* plan = findVisibleItem(_rootItem, QStringLiteral("mainView_plan"));
    QVERIFY(plan);
    auto* master = plan->property("_planMasterController").value<PlanMasterController*>();
    QVERIFY(master);
    auto* mission = master->missionController();
    mission->setHomePosition(QGeoCoordinate(30, 120, 2062));
    auto* home = mission->visualItems()->value<MissionSettingsItem*>(0);
    QVERIFY(home);
    home->setDirty(false);
    const QList<double> firstAltitude{2061};
    QVERIFY(QMetaObject::invokeMethod(home, "_terrainDataReceived", Qt::DirectConnection, Q_ARG(bool, true),
                                      Q_ARG(QList<double>, firstAltitude)));
    QCOMPARE(mission->plannedHomePosition().altitude(), 2062.0);
    QCOMPARE(home->plannedHomePositionAltitude()->rawValue().toDouble(), 2062.0);
    QVERIFY(!home->dirty());

    mission->setHomePosition(QGeoCoordinate(30.0001, 120, 2070));
    home->setDirty(false);
    const QList<double> secondAltitude{2080};
    QVERIFY(QMetaObject::invokeMethod(home, "_terrainDataReceived", Qt::DirectConnection, Q_ARG(bool, true),
                                      Q_ARG(QList<double>, secondAltitude)));
    QCOMPARE(mission->plannedHomePosition().altitude(), 2070.0);
    QVERIFY(!home->dirty());

    const QList<double> missingAltitude;
    QVERIFY(QMetaObject::invokeMethod(home, "_terrainDataReceived", Qt::DirectConnection, Q_ARG(bool, false),
                                      Q_ARG(QList<double>, missingAltitude)));
    home->plannedHomePositionAltitude()->setRawValue(qQNaN());
    const QList<double> resolvedAltitude{2090};
    QVERIFY(QMetaObject::invokeMethod(home, "_terrainDataReceived", Qt::DirectConnection, Q_ARG(bool, true),
                                      Q_ARG(QList<double>, resolvedAltitude)));
    QTRY_COMPARE_WITH_TIMEOUT(mission->plannedHomePosition().altitude(), 2090.0, TestTimeout::mediumMs());
    QCOMPARE(home->plannedHomePositionAltitude()->rawValue().toDouble(), 2090.0);
    QCOMPARE(mission->plannedHomePosition().latitude(), 30.0001);
    QVERIFY(clickButton(QStringLiteral("caacGlobalSettingsButton")));
    auto* altitude =
        findVisibleItemScrolled(QStringLiteral("caacHomeAltitude"), QStringLiteral("caacGlobalSettingsScroller"));
    QVERIFY(altitude);
    QCOMPARE(altitude->property("text").toString().toDouble(), 2090.0);
}

void CaacPlanningInteractionUITest::_mapContextClear()
{
    startUI();
    if (QTest::currentTestFailed()) {
        return;
    }
    auto* panel = findVisibleItem(_rootItem, QStringLiteral("caacRouteEditor"));
    QVERIFY(panel);
    auto* editor = panel->property("routeController").value<CaacRouteEditorController*>();
    auto* master = panel->property("planMasterController").value<PlanMasterController*>();
    auto* map = qobject_cast<QQuickItem*>(panel->property("editorMap").value<QObject*>());
    QVERIFY(editor && master && map);
    master->missionController()->setHomePosition(QGeoCoordinate(30, 120, 20));
    QVERIFY(editor->addWaypoint(QGeoCoordinate(30.001, 120)));
    const auto before = master->saveToJson();
    const auto home = master->missionController()->plannedHomePosition();
    QTest::mouseClick(_window, Qt::RightButton, Qt::NoModifier,
                      map->mapToScene(QPointF(map->width() * 0.5, map->height() * 0.4)).toPoint());
    QVERIFY(clickButton(QStringLiteral("caacMapClearWaypoints")));
    QVERIFY(waitForDialog(QCoreApplication::translate("CaacRouteEditor", "Clear waypoints")));
    QVERIFY(acceptDialog());
    QCOMPARE(master->missionController()->visualItems()->count(), 1);
    QCOMPARE(master->missionController()->plannedHomePosition(), home);
    QVERIFY(editor->undo());
    QCOMPARE(master->saveToJson(), before);
}

void CaacPlanningInteractionUITest::_statisticsPanelInteraction()
{
    startUI();
    if (QTest::currentTestFailed()) {
        return;
    }
    auto* clear =
        findVisibleItemScrolled(QStringLiteral("caacClearTrajectoryButton"), QStringLiteral("caacMainToolsScroller"));
    auto* center = findVisibleItem(_rootItem, QStringLiteral("caacHomeCenterButton"));
    auto* rail = findVisibleItem(_rootItem, QStringLiteral("caacMapEditTools"));
    QVERIFY(clear && center && rail);
    QTRY_VERIFY_WITH_TIMEOUT(
        rail->mapRectToScene(rail->boundingRect()).top() >= clear->mapRectToScene(clear->boundingRect()).bottom(),
        TestTimeout::shortMs());
    QTRY_VERIFY2_WITH_TIMEOUT(std::abs(rail->mapRectToScene(rail->boundingRect()).center().x() -
                                       clear->mapRectToScene(clear->boundingRect()).center().x()) < 1,
                              qPrintable(QStringLiteral("Rail center %1; clear button center %2; parent widths %3, %4")
                                             .arg(rail->mapRectToScene(rail->boundingRect()).center().x())
                                             .arg(clear->mapRectToScene(clear->boundingRect()).center().x())
                                             .arg(rail->parentItem()->width())
                                             .arg(clear->parentItem()->width())),
                              TestTimeout::shortMs());
    QVERIFY(clickToolSelectDropdownButton(QStringLiteral("toolbar_planningStats")));
    auto* statistics = findVisibleItem(_rootItem, QStringLiteral("planView_statistics"));
    QVERIFY(statistics);
    const auto original = statistics->mapToScene(QPointF());
    auto point = statistics->mapToScene(QPointF(statistics->width() * 0.5, statistics->height() * 0.3)).toPoint();
    QTest::mousePress(_window, Qt::LeftButton, Qt::NoModifier, point);
    QTest::mouseMove(_window, point + QPoint(15, -15));
    QTest::mouseMove(_window, point + QPoint(40, -80));
    QTest::mouseRelease(_window, Qt::LeftButton, Qt::NoModifier, point + QPoint(40, -80));
    QTRY_VERIFY_WITH_TIMEOUT(statistics->mapToScene(QPointF()).x() > original.x() + 20 &&
                                 statistics->mapToScene(QPointF()).y() < original.y() - 30,
                             TestTimeout::mediumMs());
    point = statistics->mapToScene(QPointF(statistics->width() * 0.5, statistics->height() * 0.3)).toPoint();
    QTest::mouseClick(_window, Qt::RightButton, Qt::NoModifier, point);
    QVERIFY(clickButton(QStringLiteral("caacStatisticsClose")));
    QTRY_VERIFY_WITH_TIMEOUT(!statistics->isVisible(), TestTimeout::mediumMs());
    QVERIFY(clickToolSelectDropdownButton(QStringLiteral("toolbar_planningStats")));
    QVERIFY(findVisibleItem(_rootItem, QStringLiteral("planView_statistics")));
}

void CaacPlanningInteractionUITest::_waypointDragRemainsResponsive_data()
{
    QTest::addColumn<int>("waypointCount");
    QTest::addColumn<bool>("simulationConnected");
    QTest::addColumn<bool>("closed");
    QTest::newRow("single") << 1 << false << false;
    QTest::newRow("multiple") << 16 << false << false;
    QTest::newRow("dense") << 96 << false << false;
    QTest::newRow("connected-five-waypoints") << 5 << true << false;
    QTest::newRow("connected-closed-five-waypoints") << 5 << true << true;
}

void CaacPlanningInteractionUITest::_waypointDragRemainsResponsive()
{
    QFETCH(int, waypointCount);
    QFETCH(bool, simulationConnected);
    QFETCH(bool, closed);
    startUI();
    if (QTest::currentTestFailed()) {
        return;
    }
    auto* plan = findVisibleItem(_rootItem, QStringLiteral("mainView_plan"));
    auto* map = findVisibleItem(_rootItem, QStringLiteral("planView_map"));
    auto* panel = findVisibleItem(_rootItem, QStringLiteral("caacRouteEditor"));
    QVERIFY(plan && map && panel);
    auto* master = plan->property("_planMasterController").value<PlanMasterController*>();
    auto* editor = panel->property("routeController").value<CaacRouteEditorController*>();
    QVERIFY(master && editor);
    QVERIFY(map->setProperty("allowVehicleLocationCenter", false));
    QVERIFY(map->setProperty("allowGCSLocationCenter", false));
    const QGeoCoordinate home(30, 120, 0);
    master->missionController()->setHomePosition(home);
    auto* plugin = qobject_cast<CaacCorePlugin*>(QGCCorePlugin::instance());
    QVERIFY(plugin);
    const auto preferences = plugin->planningPreferences();
    const auto restore = qScopeGuard([plugin, preferences]() { plugin->updatePlanningPreferences(preferences); });
    QVERIFY(plugin->updatePlanningPreferences({{"horizontalSnap", true},
                                               {"verticalSnap", true},
                                               {"angleSnap", true},
                                               {"symmetrySnap", true},
                                               {"midpointSnap", true},
                                               {"intersectionSnap", true}}));
    auto* simulation = plugin->simulation();
    const auto stop = qScopeGuard([simulation, simulationConnected]() {
        if (simulationConnected) {
            simulation->stop();
        }
    });
    if (simulationConnected) {
        auto point = home;
        for (const auto& leg : {qMakePair(341.8, 263.0), qMakePair(75.0, 177.2), qMakePair(165.0, 137.8),
                                qMakePair(75.0, 189.7), qMakePair(180.0, 142.85)}) {
            point = QGCGeo::geodesicDestination(point, leg.first, leg.second);
            QVERIFY(editor->addWaypoint(point));
        }
        auto options = editor->simulationSettings();
        options["closed"] = closed;
        QVERIFY(editor->setSimulationSettings(options));
        ignoreLogMessage("Utilities.QGCSensors", QtWarningMsg,
                         QRegularExpression(QStringLiteral("Failed to connect to pressure backend")));
        ignoreLogMessage("Utilities.QGCSensors", QtWarningMsg,
                         QRegularExpression(QStringLiteral("Error Initializing Pressure Sensor")));
        QVERIFY(simulation->start(master));
        QTRY_VERIFY_WITH_TIMEOUT(simulation->vehicle() && simulation->vehicle()->isInitialConnectComplete(),
                                 TestTimeout::longMs());
        QVERIFY(_window->screen());
        const auto available = _window->screen()->availableGeometry();
        const auto margins = _window->frameMargins();
        _window->resize(std::min(1280, available.width() - margins.left() - margins.right()),
                        std::min(900, available.height() - margins.top() - margins.bottom()));
        QVERIFY(clickButton(QStringLiteral("caacFlightSettingsButton")));
    } else {
        for (int i = 0; i < waypointCount; ++i) {
            QVERIFY(editor->addWaypoint(QGCGeo::geodesicDestination(home, 360.0 * i / waypointCount, 200)));
        }
    }
    const int draggedIndex = simulationConnected ? waypointCount : 1;
    auto* waypoint = master->missionController()->visualItems()->value<SimpleMissionItem*>(draggedIndex);
    QVERIFY(waypoint);
    const auto original = master->saveToJson();
    const double altitude = waypoint->coordinate().altitude();
    editor->clearHistory();
    map->setProperty("center", QVariant::fromValue(waypoint->coordinate()));
    map->setProperty("zoomLevel", 17);
    auto* dragger = findVisibleItem(_rootItem, QStringLiteral("missionItemDrag_%1").arg(draggedIndex));
    QVERIFY(dragger);
    const auto center = [map]() { return map->mapToScene(QPointF(map->width() / 2, map->height() / 2)); };
    QTRY_VERIFY_WITH_TIMEOUT(
        QLineF(dragger->mapToScene(QPointF(dragger->width() / 2, dragger->height() / 2)), center()).length() < 1,
        TestTimeout::mediumMs());
    QSignalSpy frames(_window, &QQuickWindow::frameSwapped);
    for (int repetition = 0; repetition < 4; ++repetition) {
        const auto start = dragger->mapToScene(QPointF(dragger->width() / 2, dragger->height() / 2)).toPoint();
        QTest::mousePress(_window, Qt::LeftButton, Qt::NoModifier, start);
        for (int step = 1; step <= 16; ++step) {
            QTest::mouseMove(_window, start + QPoint(step % 5 - 2, -step * 2));
            const auto rendered = frames.count();
            _window->update();
            QTRY_VERIFY_WITH_TIMEOUT(frames.count() > rendered, TestTimeout::mediumMs());
        }
        QPoint releaseOffset(0, -32);
        if (simulationConnected) {
            for (int step = 17; step <= 32; ++step) {
                releaseOffset = QPoint(step % 7 - 3, -step * 2);
                QTest::mouseMove(_window, start + releaseOffset, 0);
            }
            const auto rendered = frames.count();
            _window->update();
            QTRY_VERIFY_WITH_TIMEOUT(frames.count() > rendered, TestTimeout::mediumMs());
        }
        const auto stationaryCoordinate = waypoint->coordinate();
        for (int frame = 0; frame < 8; ++frame) {
            const auto rendered = frames.count();
            _window->update();
            QTRY_VERIFY_WITH_TIMEOUT(frames.count() > rendered, TestTimeout::mediumMs());
        }
        QCOMPARE(waypoint->coordinate(), stationaryCoordinate);
        QTest::mouseRelease(_window, Qt::LeftButton, Qt::NoModifier, start + releaseOffset);
        QVERIFY(master->saveToJson() != original);
        QCOMPARE(master->missionController()->plannedHomePosition(), home);
        QCOMPARE(waypoint->coordinate().altitude(), altitude);
        QCOMPARE(master->missionController()->visualItems()->count(), waypointCount + 1);
        QVERIFY(editor->canUndo());
        const QPointer<QQuickItem> previousDragger(dragger);
        QVERIFY(editor->undo());
        QCOMPARE(master->saveToJson(), original);
        QVERIFY(!editor->canUndo());
        QTRY_VERIFY_WITH_TIMEOUT(previousDragger.isNull(), TestTimeout::mediumMs());
        waypoint = master->missionController()->visualItems()->value<SimpleMissionItem*>(draggedIndex);
        QVERIFY(waypoint);
        QTRY_VERIFY_WITH_TIMEOUT(
            (dragger = findVisibleItem(_rootItem, QStringLiteral("missionItemDrag_%1").arg(draggedIndex), 0)),
            TestTimeout::mediumMs());
        QTRY_VERIFY_WITH_TIMEOUT(
            QLineF(dragger->mapToScene(QPointF(dragger->width() / 2, dragger->height() / 2)), center()).length() < 1,
            TestTimeout::mediumMs());
    }
}

void CaacPlanningInteractionUITest::_newSettingsPages()
{
    startUI();
    if (QTest::currentTestFailed()) {
        return;
    }
    auto* plan = findVisibleItem(_rootItem, QStringLiteral("mainView_plan"));
    QVERIFY(plan);
    auto* plugin = qobject_cast<CaacCorePlugin*>(QGCCorePlugin::instance());
    QVERIFY(plugin);
    auto* settings = SettingsManager::instance()->remoteIDSettings();
    const auto region = settings->region()->rawValue();
    const auto basic = settings->sendBasicID()->rawValue();
    const auto operatorID = settings->sendOperatorID()->rawValue();
    const auto location = settings->locationType()->rawValue();
    const auto restore = qScopeGuard([&]() {
        settings->region()->setRawValue(region);
        settings->sendBasicID()->setRawValue(basic);
        settings->sendOperatorID()->setRawValue(operatorID);
        settings->locationType()->setRawValue(location);
    });
    settings->region()->setRawValue(2);
    QVERIFY(clickButton(QStringLiteral("toolbar_qgcLogo")));
    const QString previewDir = qEnvironmentVariable("CAAC_SETTINGS_PREVIEW_DIR");
    const auto capture = [&](const QString& name) {
        if (previewDir.isEmpty()) {
            return true;
        }
        if (!QDir().mkpath(previewDir)) {
            return false;
        }
        QSignalSpy frame(_window, &QQuickWindow::frameSwapped);
        _window->update();
        if (!waitForCondition([&]() { return !frame.isEmpty(); }, TestTimeout::mediumMs(), name)) {
            return false;
        }
        auto* target = findVisibleItem(_rootItem, QStringLiteral("caacSettingsMenu"), 0);
        if (!target) {
            target = findVisibleItem(_rootItem, QStringLiteral("caacPlanningPanelContents"), 0);
        }
        if (!target) {
            return false;
        }
        const auto grab = target->grabToImage();
        if (!grab) {
            return false;
        }
        QSignalSpy ready(grab.data(), &QQuickItemGrabResult::ready);
        if (!waitForCondition([&]() { return !ready.isEmpty(); }, TestTimeout::mediumMs(), name)) {
            return false;
        }
        if (!grab->saveToFile(previewDir + "/" + name + ".png")) {
            return false;
        }
        auto* host = findVisibleItem(_rootItem, QStringLiteral("caacTopFlightActions"), 0);
        if (host) {
            const auto toolbar = host->parentItem()->grabToImage();
            if (!toolbar) {
                return false;
            }
            QSignalSpy toolbarReady(toolbar.data(), &QQuickItemGrabResult::ready);
            if (!waitForCondition([&]() { return !toolbarReady.isEmpty(); }, TestTimeout::mediumMs(), name)) {
                return false;
            }
            return toolbar->saveToFile(previewDir + "/caacTopToolbar.png");
        }
        return true;
    };
    for (const auto& page :
         {qMakePair("toolbar_rid", "caacRIDSettings"), qMakePair("toolbar_safety", "caacSafetySettings"),
          qMakePair("toolbar_uom", "caacUOMSettings")}) {
        QVERIFY(clickButtonScrolled(QLatin1String(page.first), QStringLiteral("caacSettingsNavigationScroller")));
        QVERIFY(findVisibleItem(_rootItem, QLatin1String(page.second)));
        QVERIFY(!findVisibleItem(_rootItem, QStringLiteral("settings_rightPanel"), 0));
        QVERIFY(plan->isVisible());
        QVERIFY(capture(QLatin1String(page.second)));
        if (QLatin1String(page.first) == QLatin1String("toolbar_rid")) {
            auto* classification =
                findVisibleItemScrolled(QStringLiteral("caacRIDClass_value"), QStringLiteral("caacRIDScroller"));
            QVERIFY(classification);
            QCOMPARE(classification->property("fact").value<Fact*>(), settings->classChina());
            QCOMPARE(classification->property("currentText").toString(), settings->classChina()->enumStringValue());
            QVERIFY(capture(QStringLiteral("caacRIDClassification")));
            QVERIFY(clickButtonScrolled(QStringLiteral("caacRIDEnterUASCode"), QStringLiteral("caacRIDScroller")));
            QVERIFY(findVisibleItem(_rootItem, QStringLiteral("caacUOMSettings")));
            QVERIFY(verifyChecked(QStringLiteral("caacUOMRegistrationTab"), true,
                                  QStringLiteral("RID to UOM registration")));
            auto* registrationCode =
                findVisibleItemScrolled(QStringLiteral("caacUOMUASCode"), QStringLiteral("caacUOMScroller"));
            QVERIFY(registrationCode);
            QCOMPARE(registrationCode->property("fact").value<Fact*>(), settings->basicID());
        }
    }
    auto* uas = findVisibleItemScrolled(QStringLiteral("caacUOMUASCode"), QStringLiteral("caacUOMScroller"));
    QVERIFY(uas);
    QCOMPARE(uas->property("fact").value<Fact*>(), settings->basicID());
    auto* uomPage = findVisibleItem(_rootItem, QStringLiteral("caacUOMSettings"));
    QVERIFY(uomPage && !uomPage->property("registrationNoticeExpanded").toBool());
    QVERIFY(clickButtonScrolled(QStringLiteral("caacUOMRegistrationNoticeToggle"), QStringLiteral("caacUOMScroller")));
    QVERIFY(uomPage->property("registrationNoticeExpanded").toBool());
    QVERIFY(capture(QStringLiteral("caacUOMRegistrationNoticeExpanded")));
    QVERIFY(clickButtonScrolled(QStringLiteral("caacUOMRegistrationNoticeToggle"), QStringLiteral("caacUOMScroller")));
    QVERIFY(!uomPage->property("registrationNoticeExpanded").toBool());
    QVERIFY(capture(QStringLiteral("caacUOMRegistrationNoticePreview")));
    QVERIFY(clickButton(QStringLiteral("caacUOMSerialTab")));
    QVERIFY(capture(QStringLiteral("caacUOMSerial")));
    QVERIFY(clickButton(QStringLiteral("caacUOMTelemetryTab")));
    QVERIFY(findVisibleItemScrolled(QStringLiteral("caacUOMTelemetry_mode"), QStringLiteral("caacUOMScroller")));
    QVERIFY(capture(QStringLiteral("caacUOMTelemetry")));
    auto* drawer = findItem(_rootItem, QStringLiteral("mainView_toolDrawer"));
    QVERIFY(drawer && !drawer->isVisible());
    QVERIFY(
        clickButtonScrolled(QStringLiteral("toolbar_viewSettings"), QStringLiteral("caacSettingsNavigationScroller")));
    auto* applicationSettings = findVisibleItem(_rootItem, QStringLiteral("appSettings_root"));
    QVERIFY(applicationSettings && applicationSettings->property("embedded").toBool());
    QVERIFY(!findVisibleItem(_rootItem, QStringLiteral("settings_rightPanel"), 0));
    QVERIFY(plan->isVisible() && !drawer->isVisible());
    QVERIFY(capture(QStringLiteral("caacApplicationSettingsMenu")));
    QVERIFY(clickButtonScrolled(QStringLiteral("settingsButton_General"), QStringLiteral("settings_buttonList")));
    auto* generalSettings = findVisibleItem(_rootItem, QStringLiteral("settingsPage_General"));
    QVERIFY(generalSettings && generalSettings->property("fillAvailableWidth").toBool());
    QQuickItem* languageCombo = nullptr;
    const auto* languageFact = SettingsManager::instance()->appSettings()->qLocaleLanguage();
    for (auto* item : generalSettings->findChildren<QQuickItem*>()) {
        if (item->property("fact").value<Fact*>() == languageFact && item->property("comboBox").isValid()) {
            languageCombo = qobject_cast<QQuickItem*>(item->property("comboBox").value<QObject*>());
            break;
        }
    }
    QVERIFY(languageCombo);
    const auto comboFitsPage = [&]() {
        const auto bounds = languageCombo->mapRectToItem(generalSettings, languageCombo->boundingRect());
        return bounds.width() > 0 && bounds.left() >= 0 && bounds.right() <= generalSettings->width() + 1;
    };
    QVERIFY(capture(QStringLiteral("caacApplicationSettingsInline")));
    QTRY_VERIFY2_WITH_TIMEOUT(comboFitsPage(),
                              qPrintable(QStringLiteral("General page width %1; language bounds %2, width %3")
                                             .arg(generalSettings->width())
                                             .arg(languageCombo->mapToItem(generalSettings, QPointF()).x())
                                             .arg(languageCombo->width())),
                              TestTimeout::mediumMs());
    QVERIFY(plan->isVisible() && !drawer->isVisible());
    QVERIFY(clickButton(QStringLiteral("settings_backToMenu")));
    QVERIFY(!findVisibleItem(_rootItem, QStringLiteral("settings_rightPanel"), 0));
    QVERIFY(findVisibleItem(_rootItem, QStringLiteral("settingsButton_General")));
    QVERIFY(
        clickButtonScrolled(QStringLiteral("toolbar_preferences"), QStringLiteral("caacSettingsNavigationScroller")));
    QVERIFY(findVisibleItem(_rootItem, QStringLiteral("caacPreferences")));
    QVERIFY(capture(QStringLiteral("caacPreferencesNeutral")));
    QQmlExpression close(qmlContext(plan), plan, QStringLiteral("mainWindow.closeIndicatorDrawer()"));
    close.evaluate();
    QVERIFY(!close.hasError());
    QVERIFY(drawer && !drawer->isVisible());
    for (const auto* function : {"showVehicleConfig", "showAnalyzeTool"}) {
        QQmlExpression open(qmlContext(plan), plan, QStringLiteral("mainWindow.%1()").arg(QLatin1String(function)));
        open.evaluate();
        QVERIFY(!open.hasError());
        QVERIFY(!findVisibleItem(_rootItem, QStringLiteral("caacAccessDialog"), 0));
        if (function == QLatin1String("showVehicleConfig")) {
            auto* config = findVisibleItem(_rootItem, QStringLiteral("vehicleConfig_root"));
            QVERIFY(config);
            QVERIFY(config->property("embedded").toBool());
            QVERIFY(findVisibleItem(_rootItem, QStringLiteral("caacSettingsMenu")));
            QVERIFY(plan->isVisible());
            QVERIFY(!drawer->isVisible());
            QVERIFY(capture(QStringLiteral("caacVehicleSetupMenu")));
            QVERIFY(clickButtonScrolled(QStringLiteral("vehicleConfig_offline_Airframe"),
                                        QStringLiteral("vehicleConfig_sidebarFlickable")));
            auto* catalog = findVisibleItem(_rootItem, QStringLiteral("airframeCatalogPage"));
            QVERIFY(catalog);
            QVERIFY(!findVisibleItem(_rootItem, QStringLiteral("airframeCategory_boat"), 0));
            QVERIFY(capture(QStringLiteral("caacAirframeCategories")));
            const auto catalogScroller = QStringLiteral("airframeCatalogScroller");
            for (const auto& category :
                 {qMakePair("multirotor", "quadrotor-x"), qMakePair("fixedwing", "standard-plane"),
                  qMakePair("vtol", "standard-vtol"), qMakePair("rover", "rover"), qMakePair("airship", "airship"),
                  qMakePair("other", "helicopter")}) {
                QVERIFY(clickButtonScrolled(QStringLiteral("airframeCategory_") + QLatin1String(category.first),
                                            catalogScroller));
                QCOMPARE(catalog->property("category").toString(), QLatin1String(category.first));
                auto* image = findVisibleItemScrolled(
                    QStringLiteral("airframeCatalogImage_") + QLatin1String(category.second), catalogScroller);
                QVERIFY(image);
                QTRY_COMPARE_WITH_TIMEOUT(image->property("status").toInt(), 1, TestTimeout::mediumMs());
                QVERIFY(!image->property("source").toUrl().isEmpty());
                QVERIFY(!MultiVehicleManager::instance()->activeVehicle());
                QVERIFY(capture(QStringLiteral("caacAirframe_") + QLatin1String(category.first)));
                QVERIFY(clickButtonScrolled(QStringLiteral("airframeCategory_back"), catalogScroller));
                QCOMPARE(catalog->property("category").toString(), QString());
            }
            QVERIFY(clickButton(QStringLiteral("vehicleConfig_backToMenu")));
            QVERIFY(clickButtonScrolled(QStringLiteral("vehicleConfig_offline_Sensors"),
                                        QStringLiteral("vehicleConfig_sidebarFlickable")));
            auto* sensors = findVisibleItem(_rootItem, QStringLiteral("sensorsSetup_inlinePanel"));
            QVERIFY(sensors);
            QCOMPARE(sensors->property("sectionId").toString(), QStringLiteral("Orientations"));
            auto* orientation = findVisibleItemScrolled(QStringLiteral("sensorsSetup_setOrientation"),
                                                        QStringLiteral("sensorsSetup_buttonScroller"));
            QVERIFY(orientation && !orientation->isEnabled());
            QVERIFY(!findVisibleItem(sensors, QStringLiteral("sensorsSetup_inlineWorkflow"), 0));
            QVERIFY(plan->isVisible() && !drawer->isVisible());
            QVERIFY(capture(QStringLiteral("caacVehicleCalibrationInline")));
            QVERIFY(clickButton(QStringLiteral("vehicleConfig_backToMenu")));
            QVERIFY(clickButtonScrolled(QStringLiteral("vehicleConfig_summary"),
                                        QStringLiteral("vehicleConfig_sidebarFlickable")));
            QVERIFY(findVisibleItem(_rootItem, QStringLiteral("vehicleConfig_backToMenu")));
            QVERIFY(!drawer->isVisible());
            QVERIFY(clickButton(QStringLiteral("vehicleConfig_backToMenu")));
            QVERIFY(findVisibleItem(_rootItem, QStringLiteral("vehicleConfig_summary")));
        } else {
            auto* analysis = findVisibleItem(_rootItem, QStringLiteral("analyzeView_root"));
            QVERIFY(analysis && analysis->property("embedded").toBool());
            QVERIFY(findVisibleItem(_rootItem, QStringLiteral("caacSettingsMenu")));
            QVERIFY(plan->isVisible() && !drawer->isVisible());
            QVERIFY(capture(QStringLiteral("caacAnalyzeMenu")));
            const auto onboardLogs =
                QStringLiteral("analyzeButton_") + QCoreApplication::translate("QGCCorePlugin", "Onboard Logs");
            QVERIFY(clickButtonScrolled(onboardLogs, QStringLiteral("analyze_buttonList")));
            QVERIFY(findVisibleItem(_rootItem, QStringLiteral("analyze_panel")));
            QVERIFY(plan->isVisible() && !drawer->isVisible());
            QVERIFY(capture(QStringLiteral("caacAnalyzeInline")));
            QVERIFY(clickButton(QStringLiteral("analyze_backToMenu")));
            QVERIFY(!findVisibleItem(_rootItem, QStringLiteral("analyze_panel"), 0));
            for (const auto& tool : {qMakePair("Log Viewer", "logViewer_"), qMakePair("GeoTag Images", "geoTag_")}) {
                const QString prefix = QLatin1String(tool.second);
                const auto buttonName =
                    QStringLiteral("analyzeButton_") + QCoreApplication::translate("QGCCorePlugin", tool.first);
                QVERIFY(clickButtonScrolled(buttonName, QStringLiteral("analyze_buttonList")));
                auto* page = findVisibleItem(_rootItem, prefix + QStringLiteral("page"));
                auto* scroller = findItem(page, prefix + QStringLiteral("scroller"));
                QVERIFY(page && scroller);
                const auto fitsWidth = [&](QQuickItem* item) {
                    for (auto* ancestor = item; ancestor; ancestor = ancestor->parentItem()) {
                        ancestor->ensurePolished();
                    }
                    const auto bounds = item->mapRectToItem(scroller, item->boundingRect());
                    return item->width() > 0 && item->height() > 0 && bounds.left() >= -1 &&
                           bounds.right() <= scroller->width() + 1;
                };
                const auto textFits = [&](QQuickItem* item) {
                    return fitsWidth(item) && item->property("paintedWidth").toReal() <= item->width() + 1 &&
                           item->property("paintedHeight").toReal() <= item->height() + 1;
                };
                for (const qreal width : {320.0, 640.0}) {
                    QVERIFY(page->setProperty("availableWidth", width));
                    QVERIFY(page->setProperty("availableHeight", 420.0));
                    QTRY_COMPARE_WITH_TIMEOUT(scroller->width(), width, TestTimeout::shortMs());
                    QVERIFY(scroller->setProperty("contentY", 0.0));
                    const bool logViewer = prefix == QStringLiteral("logViewer_");
                    const QStringList controls =
                        logViewer ? QStringList{QStringLiteral("openULog"),   QStringLiteral("openTLog"),
                                                QStringLiteral("clear"),      QStringLiteral("tabChart"),
                                                QStringLiteral("tabMap"),     QStringLiteral("tabParameters"),
                                                QStringLiteral("tabMessages")}
                                  : QStringList{QStringLiteral("timeOffset"), QStringLiteral("preview"),
                                                QStringLiteral("start")};
                    for (const auto& controlName : controls) {
                        auto* control = findItem(page, prefix + controlName);
                        QVERIFY2(control, qPrintable(prefix + controlName));
                        QTRY_VERIFY2_WITH_TIMEOUT(fitsWidth(control), qPrintable(prefix + controlName),
                                                  TestTimeout::shortMs());
                    }
                    if (logViewer) {
                        auto* binButton = findItem(page, QStringLiteral("logViewer_openBin"));
                        if (binButton && binButton->isVisible()) {
                            QTRY_VERIFY_WITH_TIMEOUT(fitsWidth(binButton), TestTimeout::shortMs());
                        }
                        auto* tabs = findItem(page, QStringLiteral("logViewer_tabs"));
                        QVERIFY(tabs);
                        for (const auto& tab : {qMakePair("tabMap", 1), qMakePair("tabParameters", 2),
                                                qMakePair("tabMessages", 3), qMakePair("tabChart", 0)}) {
                            QVERIFY(clickButtonScrolled(prefix + QLatin1String(tab.first),
                                                        QStringLiteral("logViewer_scroller")));
                            QTRY_COMPARE_WITH_TIMEOUT(tabs->property("currentIndex").toInt(), tab.second,
                                                      TestTimeout::shortMs());
                        }
                    } else {
                        for (const auto& name : {QStringLiteral("offsetHelp"), QStringLiteral("previewHelp")}) {
                            auto* label = findItem(page, prefix + name);
                            QVERIFY(label);
                            QTRY_VERIFY2_WITH_TIMEOUT(textFits(label), qPrintable(prefix + name),
                                                      TestTimeout::shortMs());
                        }
                        auto* preview = findItem(page, QStringLiteral("geoTag_preview"));
                        QVERIFY(preview);
                        auto* previewText = preview->property("contentItem").value<QQuickItem*>();
                        QVERIFY(previewText);
                        QTRY_VERIFY_WITH_TIMEOUT(textFits(previewText), TestTimeout::shortMs());
                    }
                    auto* bottom =
                        findItem(page, prefix + (logViewer ? QStringLiteral("endContent") : QStringLiteral("start")));
                    QVERIFY(bottom);
                    QTRY_VERIFY_WITH_TIMEOUT(logViewer
                                                 ? scroller->property("contentHeight").toReal() >= scroller->height()
                                                 : scroller->property("contentHeight").toReal() > scroller->height(),
                                             TestTimeout::shortMs());
                    const qreal maximumY =
                        std::max(0.0, scroller->property("contentHeight").toReal() - scroller->height());
                    QVERIFY(scroller->setProperty("contentY", maximumY));
                    QTRY_VERIFY_WITH_TIMEOUT(
                        [&] {
                            const auto bounds = bottom->mapRectToItem(scroller, bottom->boundingRect());
                            return bounds.bottom() <= scroller->height() + 1 && bounds.bottom() > 0;
                        }(),
                        TestTimeout::shortMs());
                    QCOMPARE(scroller->property("contentX").toReal(), 0.0);
                }
                QVERIFY(clickButton(QStringLiteral("analyze_backToMenu")));
                QVERIFY(!findVisibleItem(_rootItem, QStringLiteral("analyze_panel"), 0));
            }
        }
        QQmlExpression returnToPlan(qmlContext(plan), plan, QStringLiteral("mainWindow.showPlanView()"));
        returnToPlan.evaluate();
        QVERIFY(!returnToPlan.hasError());
        open.evaluate();
        QVERIFY(!open.hasError());
        QVERIFY(!findVisibleItem(_rootItem, QStringLiteral("caacAccessDialog"), 0));
        QVERIFY(!drawer->isVisible());
        returnToPlan.evaluate();
        QVERIFY(!returnToPlan.hasError());
    }
    auto* master = plan->property("_planMasterController").value<PlanMasterController*>();
    QVERIFY(master);
    master->missionController()->setHomePosition(QGeoCoordinate(30, 120, 20));
    QVERIFY(clickButton(QStringLiteral("caacTemplateEditorButton")));
    auto* templates = findVisibleItem(_rootItem, QStringLiteral("caacTemplateTabs"));
    QVERIFY(templates && templates->setProperty("currentIndex", 1));
    auto* diagram =
        findVisibleItemScrolled(QStringLiteral("caacTemplatePreview"), QStringLiteral("caacTemplateScroller"));
    QVERIFY(diagram);
    QVERIFY(capture(QStringLiteral("caacHexagonTemplate")));
}

void CaacPlanningInteractionUITest::_preferencesUI()
{
    startUI();
    if (QTest::currentTestFailed()) {
        return;
    }
    auto* plugin = qobject_cast<CaacCorePlugin*>(QGCCorePlugin::instance());
    QVERIFY(plugin);
    const auto original = plugin->planningPreferences();
    const auto restore = qScopeGuard([&]() { plugin->updatePlanningPreferences(original); });
    QVERIFY(clickButton(QStringLiteral("toolbar_qgcLogo")));
    auto* horizontal = findVisibleItem(_rootItem, QStringLiteral("caacPreference_horizontalSnap"));
    QVERIFY(horizontal);
    const bool before = original.value("horizontalSnap").toBool();
    QVERIFY(clickButton(QStringLiteral("caacPreference_horizontalSnap")));
    QTRY_COMPARE_WITH_TIMEOUT(plugin->planningPreferences().value("horizontalSnap").toBool(), !before,
                              TestTimeout::mediumMs());
    QVERIFY(clickButtonScrolled(QStringLiteral("caacPreferenceSnapAngle"), QStringLiteral("caacPreferencesScroller")));
    QTest::keyClick(_window, Qt::Key_A, Qt::ControlModifier);
    QTest::keyClick(_window, Qt::Key_3);
    QTest::keyClick(_window, Qt::Key_0);
    QTest::keyClick(_window, Qt::Key_Tab);
    QTRY_COMPARE_WITH_TIMEOUT(plugin->planningPreferences().value("snapAngle").toDouble(), 30.0,
                              TestTimeout::mediumMs());
    QVERIFY(clickButton(QStringLiteral("caacPreferencesUITab")));
    auto* theme = SettingsManager::instance()->appSettings()->indoorPalette();
    const auto previousTheme = theme->rawValue();
    const auto restoreTheme = qScopeGuard([&]() { theme->setRawValue(previousTheme); });
    QVERIFY(clickButton(QStringLiteral("caacPreferenceThemeLight")));
    QTRY_COMPARE_WITH_TIMEOUT(theme->rawValue().toInt(), 0, TestTimeout::mediumMs());
    QVERIFY(clickButton(QStringLiteral("caacPreferenceThemeDark")));
    QTRY_COMPARE_WITH_TIMEOUT(theme->rawValue().toInt(), 1, TestTimeout::mediumMs());
    QVERIFY(!findItem(_rootItem, QStringLiteral("caacPreferencesFlightTab")));
    QVERIFY(!findItem(_rootItem, QStringLiteral("caacPreferenceFlightParameters")));
    QVERIFY(clickButton(QStringLiteral("caacPreferencesPlanningTab")));
    auto* drawing = findVisibleItem(_rootItem, QStringLiteral("caacDrawingSettingsFrame"));
    QVERIFY(drawing);
    horizontal = findVisibleItemScrolled(QStringLiteral("caacPreference_horizontalSnap"),
                                         QStringLiteral("caacPreferencesScroller"));
    QVERIFY(horizontal);
    QCOMPARE(horizontal->property("checked").toBool(), !before);
    QVERIFY(!findVisibleItem(_rootItem, QStringLiteral("caacPreferenceThemeDark"), 0));
    QVERIFY(clickButton(QStringLiteral("caacPreferencesUITab")));
    QVERIFY(findVisibleItem(_rootItem, QStringLiteral("caacPreferenceThemeDark")));
    QVERIFY(!drawing->isVisible());
}

void CaacPlanningInteractionUITest::_calculatorUI()
{
    startUI();
    if (QTest::currentTestFailed()) {
        return;
    }
    auto* calculator =
        findVisibleItemScrolled(QStringLiteral("caacCalculatorButton"), QStringLiteral("caacMainToolsScroller"));
    QVERIFY(calculator);
    if (const auto image = qgetenv("CAAC_MENU_PREVIEW"); !image.isEmpty()) {
        QVERIFY(_window->screen());
        const auto available = _window->screen()->availableGeometry().size();
        _window->resize(std::min(1440, available.width() - 30), std::min(930, available.height() - 30));
        auto* toolScroller = findVisibleItem(_rootItem, QStringLiteral("caacMainToolsScroller"));
        QVERIFY(toolScroller && toolScroller->setProperty("contentY", 0));
        auto* map = findVisibleItem(_rootItem, QStringLiteral("planView_map"));
        auto* plan = findVisibleItem(_rootItem, QStringLiteral("mainView_plan"));
        QVERIFY(map && plan);
        auto* master = plan->property("_planMasterController").value<PlanMasterController*>();
        QVERIFY(master);
        const QGeoCoordinate home(25.137898058569604, 102.7212769885262, 1890);
        master->missionController()->setHomePosition(home);
        for (int bearing : {0, 90, 180}) {
            master->missionController()->insertSimpleMissionItem(QGCGeo::geodesicDestination(home, bearing, 130),
                                                                 master->missionController()->visualItems()->count(),
                                                                 false);
        }
        map->setProperty("center", QVariant::fromValue(home));
        map->setProperty("zoomLevel", 17);
        QSignalSpy frame(_window, &QQuickWindow::frameSwapped);
        _window->update();
        QTRY_VERIFY_WITH_TIMEOUT(!frame.isEmpty(), TestTimeout::mediumMs());
        QVERIFY(_window->grabWindow().save(QString::fromUtf8(image)));
    }
    if (const auto image = qgetenv("CAAC_PREFERENCES_PREVIEW"); !image.isEmpty()) {
        QVERIFY(clickButton(QStringLiteral("toolbar_qgcLogo")));
        QVERIFY(findVisibleItem(_rootItem, QStringLiteral("caacPreferences")));
        auto* tab = findVisibleItem(_rootItem, QStringLiteral("caacPreferencesPlanningTab"));
        QVERIFY(tab && tab->width() > 0 && tab->height() > 0);
        QVERIFY(findVisibleItem(_rootItem, QStringLiteral("caacPreference_horizontalSnap")));
        QSignalSpy frame(_window, &QQuickWindow::frameSwapped);
        _window->update();
        QTRY_VERIFY_WITH_TIMEOUT(!frame.isEmpty(), TestTimeout::mediumMs());
        QVERIFY(_window->grabWindow().save(QString::fromUtf8(image)));
        auto* plan = findVisibleItem(_rootItem, QStringLiteral("mainView_plan"));
        QVERIFY(plan);
        QQmlExpression closeDrawer(qmlContext(plan), plan, QStringLiteral("mainWindow.closeIndicatorDrawer()"));
        closeDrawer.evaluate();
        QVERIFY(!closeDrawer.hasError());
    }
    QVERIFY(clickButtonScrolled(QStringLiteral("caacCalculatorButton"), QStringLiteral("caacMainToolsScroller")));
    QVERIFY(waitForDialog(QCoreApplication::translate("CaacCalculator", "Angle calculator")));
    const auto field = [&](const QString& name) {
        return findVisibleItemScrolled(name, QStringLiteral("popupDialog_contentFlickable"));
    };
    auto* wind = field(QStringLiteral("caacCalculatorWindValue"));
    auto* preset = field(QStringLiteral("caacCalculatorWindPreset"));
    QVERIFY(preset);
    QVERIFY(preset->setProperty("currentIndex", 1));
    QTRY_COMPARE_WITH_TIMEOUT(preset->property("currentText").toString(), QString::fromUtf8("NNE 北东北 · 22.5°"),
                              TestTimeout::mediumMs());
    QVERIFY(preset->setProperty("currentIndex", 11));
    QTRY_COMPARE_WITH_TIMEOUT(preset->property("currentText").toString(), QString::fromUtf8("WSW 西西南 · 247.5°"),
                              TestTimeout::mediumMs());
    QVERIFY(wind && wind->setProperty("text", QStringLiteral("112.5")));
    QVERIFY(clickButtonScrolled(QStringLiteral("caacCalculatorWindCalculate"),
                                QStringLiteral("popupDialog_contentFlickable")));
    auto* windResult = field(QStringLiteral("caacCalculatorWindResult"));
    QVERIFY(windResult);
    QVERIFY(windResult->property("resultText").toString().contains(QStringLiteral("67.5")));
    QVERIFY(clickButtonScrolled(QStringLiteral("caacCalculatorWindResultCopy"),
                                QStringLiteral("popupDialog_contentFlickable")));
    QCOMPARE(QGuiApplication::clipboard()->text(), windResult->property("resultText").toString());
    auto* hours = field(QStringLiteral("caacCalculatorHours"));
    auto* minutes = field(QStringLiteral("caacCalculatorMinutes"));
    QVERIFY(hours && minutes);
    QVERIFY(hours->setProperty("text", QStringLiteral("16")));
    QVERIFY(minutes->setProperty("text", QStringLiteral("30")));
    QVERIFY(clickButtonScrolled(QStringLiteral("caacCalculatorTimeCalculate"),
                                QStringLiteral("popupDialog_contentFlickable")));
    auto* timeResult = field(QStringLiteral("caacCalculatorTimeResult"));
    QVERIFY(timeResult && timeResult->property("resultText").toString().contains(QStringLiteral("247.5")));
    QVERIFY(clickButtonScrolled(QStringLiteral("caacCalculatorTrigCalculate"),
                                QStringLiteral("popupDialog_contentFlickable")));
    auto* trigResult = field(QStringLiteral("caacCalculatorTrigResult"));
    QVERIFY(trigResult && trigResult->property("resultText").toString().contains(QStringLiteral("0.5")));
    auto* c = field(QStringLiteral("caacCalculatorSideC"));
    auto* a = field(QStringLiteral("caacCalculatorSideA"));
    QVERIFY(c && a);
    QVERIFY(c->setProperty("text", QStringLiteral("30")));
    QVERIFY(a->setProperty("text", QStringLiteral("20")));
    QVERIFY(clickButtonScrolled(QStringLiteral("caacCalculatorTriangleCalculate"),
                                QStringLiteral("popupDialog_contentFlickable")));
    auto* triangle = field(QStringLiteral("caacCalculatorTriangleResult"));
    QVERIFY(triangle && triangle->property("valid").toBool());
    auto* b = field(QStringLiteral("caacCalculatorSideB"));
    QVERIFY(b);
    QVERIFY(std::abs(b->property("text").toDouble() - std::sqrt(500)) < 1e-8);
    QVERIFY(c->setProperty("text", QStringLiteral("68")));
    QVERIFY(clickButtonScrolled(QStringLiteral("caacCalculatorTriangleCalculate"),
                                QStringLiteral("popupDialog_contentFlickable")));
    QVERIFY(triangle->property("valid").toBool());
    QVERIFY(std::abs(b->property("text").toDouble() - std::sqrt(4224)) < 1e-8);
    if (const auto image = qgetenv("CAAC_CALCULATOR_PREVIEW"); !image.isEmpty()) {
        auto* scroller = findVisibleItem(_rootItem, QStringLiteral("popupDialog_contentFlickable"));
        QVERIFY(scroller && scroller->setProperty("contentY", 0));
        QVERIFY(_window->grabWindow().save(QString::fromUtf8(image)));
    }
    QVERIFY(rejectDialog());
    QVERIFY(verifyText(QStringLiteral("caacConnectionStatusButton"), QStringLiteral("未连接"),
                       QStringLiteral("offline connection status")));
    QVERIFY(clickButton(QStringLiteral("caacConnectionStatusButton")));
    auto* connectionPage = findVisibleItem(_rootItem, QStringLiteral("toolbar_connections"));
    QVERIFY(connectionPage);
    QTRY_VERIFY_WITH_TIMEOUT(connectionPage->property("checked").toBool(), TestTimeout::mediumMs());
    auto* connectionContents = findVisibleItem(_rootItem, QStringLiteral("caacSettingsSubmenu"));
    QVERIFY(connectionContents);
    QTRY_VERIFY_WITH_TIMEOUT(connectionContents->property("source").toUrl().toString().contains(
                                 QStringLiteral("CaacConnectionSettings.qml")),
                             TestTimeout::mediumMs());
    QVERIFY(findVisibleItem(_rootItem, QStringLiteral("caacConnectionSettings")));
    QVERIFY(findVisibleItem(_rootItem, QStringLiteral("caacConnectionSerialPort")));
    QVERIFY(findVisibleItem(_rootItem, QStringLiteral("caacConnectionBaudRate")));
    QVERIFY(!findItem(_rootItem, QStringLiteral("toolbar_viewFlightPlan")));
    const auto expandedWidth = findVisibleItem(_rootItem, QStringLiteral("caacSettingsMenu"))->width();
    QVERIFY(clickButton(QStringLiteral("toolbar_connections")));
    QVERIFY(!findVisibleItem(_rootItem, QStringLiteral("caacConnectionSerialPort"), 0));
    QVERIFY(findVisibleItem(_rootItem, QStringLiteral("caacSettingsMenu"))->width() < expandedWidth);
    QVERIFY(clickButton(QStringLiteral("toolbar_connections")));
    QVERIFY(findVisibleItem(_rootItem, QStringLiteral("caacConnectionSerialPort")));
    QVERIFY(
        clickButtonScrolled(QStringLiteral("caacMoreConnectionSettings"), QStringLiteral("caacConnectionScroller")));
    QVERIFY(findVisibleItem(_rootItem, QStringLiteral("settingsPage_CommLinks")));
    QVERIFY(clickButton(QStringLiteral("caacConnectionBack")));
    QVERIFY(findVisibleItem(_rootItem, QStringLiteral("mainView_plan")));
    QVERIFY(!findVisibleItem(_rootItem, QStringLiteral("settings_rightPanel"), 0));
    QVERIFY(clickButtonScrolled(QStringLiteral("toolbar_rtk"), QStringLiteral("caacSettingsNavigationScroller")));
    QVERIFY(findVisibleItem(_rootItem, QStringLiteral("settingsPage_RTKCorrections")));
    QVERIFY(findVisibleItem(_rootItem, QStringLiteral("settingsGroup_NTRIPServer")));
    QVERIFY(!findVisibleItem(_rootItem, QStringLiteral("settingsPage_CommLinks"), 0));
    QVERIFY(
        clickButtonScrolled(QStringLiteral("toolbar_preferences"), QStringLiteral("caacSettingsNavigationScroller")));
    QVERIFY(findVisibleItem(_rootItem, QStringLiteral("caacPreferences")));
}

void CaacPlanningInteractionUITest::_homePreviewInsertion()
{
    startUI();
    if (QTest::currentTestFailed()) {
        return;
    }
    auto* plan = findVisibleItem(_rootItem, QStringLiteral("mainView_plan"));
    auto* map = findVisibleItem(_rootItem, QStringLiteral("planView_map"));
    auto* panel = findVisibleItem(_rootItem, QStringLiteral("caacRouteEditor"));
    QVERIFY(plan && map && panel);
    auto* master = plan->property("_planMasterController").value<PlanMasterController*>();
    auto* editor = panel->property("routeController").value<CaacRouteEditorController*>();
    QVERIFY(master && editor);
    auto* mission = master->missionController();
    const QGeoCoordinate home(30, 120, 20);
    mission->setHomePosition(home);
    for (const int bearing : {0, 90, 180}) {
        QVERIFY(editor->addWaypoint(QGCGeo::geodesicDestination(home, bearing, 150)));
    }
    auto settings = CaacSimulationController::defaults();
    settings["closed"] = false;
    settings["startSequence"] = 2;
    settings["returnSequence"] = 0;
    QVERIFY(editor->setSimulationSettings(settings));
    editor->clearHistory();
    const auto original = master->saveToJson();
    map->setProperty("zoomLevel", 18);
    for (const int endpoint : {2, 3}) {
        const auto point = mission->visualItems()->value<SimpleMissionItem*>(endpoint)->coordinate();
        const double bearing = QGCGeo::geodesicAzimuth(home, point);
        const double distance = QGCGeo::geodesicDistance(home, point);
        const auto midpoint = QGCGeo::geodesicDestination(home, bearing, distance / 2);
        map->setProperty("center", QVariant::fromValue(midpoint));
        const auto mapMiddle = map->mapToScene(QPointF(map->width() / 2, map->height() / 2));
        QTest::mouseMove(_window, (mapMiddle + QPointF(-70, -70)).toPoint());
        QPointF hoverPoint;
        QVERIFY(QMetaObject::invokeMethod(
            map, "fromCoordinate", Q_RETURN_ARG(QPointF, hoverPoint),
            Q_ARG(QGeoCoordinate, QGCGeo::geodesicDestination(home, bearing, distance / 3)), Q_ARG(bool, false)));
        QTest::mouseMove(_window, map->mapToScene(hoverPoint).toPoint());
        auto* split = findVisibleItem(_rootItem, QStringLiteral("caacRouteSplitButton"));
        QVERIFY(split);
        QTRY_VERIFY_WITH_TIMEOUT(
            QLineF(split->mapToScene(QPointF(split->width() / 2, split->height() / 2)), mapMiddle).length() < 1,
            TestTimeout::mediumMs());
        QVERIFY(clickButton(QStringLiteral("caacRouteSplitButton")));
        QTRY_COMPARE_WITH_TIMEOUT(mission->visualItems()->count(), 5, TestTimeout::mediumMs());
        const int insertedIndex = endpoint == 2 ? 2 : 4;
        QVERIFY(QGCGeo::geodesicDistance(mission->visualItems()->value<SimpleMissionItem*>(insertedIndex)->coordinate(),
                                         midpoint) < 0.01);
        QCOMPARE(mission->plannedHomePosition(), home);
        QTest::keyClick(_window, Qt::Key_Z, Qt::ControlModifier);
        QTRY_COMPARE_WITH_TIMEOUT(master->saveToJson(), original, TestTimeout::mediumMs());
    }
}

void CaacPlanningInteractionUITest::_synchronizeBasicsUI()
{
    startUI();
    if (QTest::currentTestFailed()) {
        return;
    }
    auto* plan = findVisibleItem(_rootItem, QStringLiteral("mainView_plan"));
    auto* panel = findVisibleItem(_rootItem, QStringLiteral("caacRouteEditor"));
    QVERIFY(plan && panel);
    QVERIFY(!findVisibleItem(_rootItem, QStringLiteral("mainView_fly"), 0));
    QVERIFY(!findVisibleItem(_rootItem, QStringLiteral("planView_statistics"), 0));
    auto* flightTools = findVisibleItem(_rootItem, QStringLiteral("caacMissionDetailsButton"));
    QVERIFY(flightTools);
    QVERIFY(flightTools->mapToScene(QPointF()).x() < _rootItem->width() / 4);
    QVERIFY(clickToolSelectDropdownButton(QStringLiteral("toolbar_planningStats")));
    QVERIFY(findVisibleItem(_rootItem, QStringLiteral("planView_statistics")));
    QVERIFY(clickToolSelectDropdownButton(QStringLiteral("toolbar_planningStats")));
    QVERIFY(!findVisibleItem(_rootItem, QStringLiteral("planView_statistics"), 0));
    auto* master = plan->property("_planMasterController").value<PlanMasterController*>();
    auto* editor = panel->property("routeController").value<CaacRouteEditorController*>();
    QVERIFY(master && editor);
    auto* mission = master->missionController();
    const QGeoCoordinate home(30, 120, 20);
    mission->setHomePosition(home);
    QVERIFY(editor->addWaypoint(QGCGeo::geodesicDestination(home, 90, 100)));
    QVERIFY(editor->addWaypoint(QGCGeo::geodesicDestination(home, 0, 100)));
    mission->visualItems()->value<SimpleMissionItem*>(2)->setAltitudeFrame(
        QGroundControlQmlGlobal::AltitudeFrameAbsolute);
    editor->clearHistory();
    const auto original = master->saveToJson();
    QVERIFY(clickButton(QStringLiteral("caacGlobalSettingsButton")));
    auto* homeAltitude =
        findVisibleItemScrolled(QStringLiteral("caacHomeAltitude"), QStringLiteral("caacGlobalSettingsScroller"));
    QVERIFY(homeAltitude);
    QTRY_COMPARE_WITH_TIMEOUT(homeAltitude->property("text").toString().toDouble(), 20.0, TestTimeout::mediumMs());
    auto* homeItem = mission->visualItems()->value<MissionSettingsItem*>(0);
    QVERIFY(homeItem);
    homeItem->plannedHomePositionAltitude()->setRawValue(1900.0);
    QTRY_COMPARE_WITH_TIMEOUT(homeAltitude->property("text").toString().toDouble(), 1900.0, TestTimeout::mediumMs());
    QVERIFY(clickButtonScrolled(QStringLiteral("caacHomeAltitude"), QStringLiteral("caacGlobalSettingsScroller")));
    QTest::keyClick(_window, Qt::Key_A, Qt::ControlModifier);
    QTest::keyClick(_window, Qt::Key_1);
    QTest::keyClick(_window, Qt::Key_9);
    QTest::keyClick(_window, Qt::Key_3);
    QTest::keyClick(_window, Qt::Key_0);
    homeItem->plannedHomePositionAltitude()->setRawValue(1940.0);
    QCOMPARE(homeAltitude->property("text").toString().toDouble(), 1930.0);
    mission->setHomePosition(QGeoCoordinate(30.0001, 120, 1950));
    QTRY_COMPARE_WITH_TIMEOUT(homeAltitude->property("text").toString().toDouble(), 1950.0, TestTimeout::mediumMs());
    mission->setHomePosition(home);
    QTRY_COMPARE_WITH_TIMEOUT(homeAltitude->property("text").toString().toDouble(), 20.0, TestTimeout::mediumMs());
    editor->clearHistory();
    QVERIFY(!findItem(_rootItem, QStringLiteral("caacSpecifyMissionSpeed")));
    auto* speedField =
        findVisibleItemScrolled(QStringLiteral("caacMissionSpeed"), QStringLiteral("caacGlobalSettingsScroller"));
    QVERIFY(speedField && speedField->setProperty("text", QStringLiteral("8")));
    QVERIFY(clickButtonScrolled(QStringLiteral("caacSyncWaypointSpeed"), QStringLiteral("caacGlobalSettingsScroller")));
    QVERIFY(homeItem->speedSection()->specifyFlightSpeed());
    QCOMPARE(homeItem->specifiedFlightSpeed(), 8.0);
    for (int i = 1; i <= 2; ++i) {
        auto* speed = qobject_cast<SpeedSection*>(mission->visualItems()->value<SimpleMissionItem*>(i)->speedSection());
        QVERIFY(speed && speed->specifyFlightSpeed());
        QCOMPARE(speed->flightSpeed()->rawValue().toDouble(), 8.0);
        auto* point = mission->visualItems()->value<SimpleMissionItem*>(i);
        QCOMPARE(point->waypointNumber(), i);
        auto* marker = findItem(_rootItem, QStringLiteral("missionItemMapLabel_%1").arg(point->sequenceNumber()));
        QVERIFY(marker);
        QTRY_COMPARE_WITH_TIMEOUT(marker->property("index").toInt(), i, TestTimeout::mediumMs());
    }
    QTest::keyClick(_window, Qt::Key_Z, Qt::ControlModifier);
    QTRY_COMPARE_WITH_TIMEOUT(master->saveToJson(), original, TestTimeout::mediumMs());
    auto* altitudeFact = SettingsManager::instance()->appSettings()->defaultMissionItemAltitude();
    const auto defaultAltitude = altitudeFact->rawValue();
    const auto restoreAltitude =
        qScopeGuard([altitudeFact, defaultAltitude]() { altitudeFact->setRawValue(defaultAltitude); });
    QVERIFY(clickButtonScrolled(QStringLiteral("caacDefaultWaypointAltitude"),
                                QStringLiteral("caacGlobalSettingsScroller")));
    QTest::keyClick(_window, Qt::Key_A, Qt::ControlModifier);
    QTest::keyClick(_window, Qt::Key_5);
    QTest::keyClick(_window, Qt::Key_0);
    QTest::keyClick(_window, Qt::Key_0);
    QTest::keyClick(_window, Qt::Key_Tab);
    QTRY_COMPARE_WITH_TIMEOUT(altitudeFact->rawValue().toDouble(), 500.0, TestTimeout::mediumMs());
    QVERIFY(
        clickButtonScrolled(QStringLiteral("caacSyncWaypointAltitude"), QStringLiteral("caacGlobalSettingsScroller")));
    for (int i = 1; i <= 2; ++i) {
        auto* item = mission->visualItems()->value<SimpleMissionItem*>(i);
        QCOMPARE(item->altitude()->rawValue().toDouble(), 500.0);
        QCOMPARE(item->altitudeFrame(), i == 1 ? QGroundControlQmlGlobal::AltitudeFrameRelative
                                               : QGroundControlQmlGlobal::AltitudeFrameAbsolute);
    }
    QCOMPARE(mission->plannedHomePosition(), home);
    QTest::keyClick(_window, Qt::Key_Z, Qt::ControlModifier);
    QTRY_COMPARE_WITH_TIMEOUT(master->saveToJson(), original, TestTimeout::mediumMs());
}

void CaacPlanningInteractionUITest::_dragSnappingAndRelations()
{
    startUI();
    if (QTest::currentTestFailed()) {
        return;
    }
    QVERIFY(QMetaObject::invokeMethod(_window, "showPlanView"));
    auto* plan = findVisibleItem(_rootItem, QStringLiteral("mainView_plan"));
    auto* map = findVisibleItem(_rootItem, QStringLiteral("planView_map"));
    auto* panel = findVisibleItem(_rootItem, QStringLiteral("caacRouteEditor"));
    QVERIFY(plan && map && panel);
    auto* master = plan->property("_planMasterController").value<PlanMasterController*>();
    auto* editor = panel->property("routeController").value<CaacRouteEditorController*>();
    QVERIFY(master && editor);
    QVERIFY(map->setProperty("allowVehicleLocationCenter", false));
    QVERIFY(map->setProperty("allowGCSLocationCenter", false));
    QVERIFY(panel->property("panelsCollapsed").toBool());
    QVERIFY(!findVisibleItem(_rootItem, QStringLiteral("caacPanelTitle"), 0));
    const auto collapsedHeight = panel->height();
    const auto beforeMapClick = master->saveToJson();
    for (const auto* name : {"caacGlobalSettingsButton", "caacRelativeEditorButton", "caacTemplateEditorButton",
                             "caacWaypointListButton"}) {
        QVERIFY(clickButton(QLatin1String(name)));
        QVERIFY(!panel->property("panelsCollapsed").toBool());
        QVERIFY(findVisibleItem(_rootItem, QStringLiteral("caacPanelTitle")));
        QVERIFY(clickButton(QLatin1String(name)));
        QVERIFY(panel->property("panelsCollapsed").toBool());
        QTRY_VERIFY_WITH_TIMEOUT(panel->height() <= collapsedHeight + 1, TestTimeout::mediumMs());
        QVERIFY(clickButton(QLatin1String(name)));
        auto* title = findVisibleItem(_rootItem, QStringLiteral("caacPanelTitle"));
        QVERIFY(title);
        QTest::mouseClick(_window, Qt::LeftButton, Qt::NoModifier,
                          title->mapToScene(QPointF(title->width() / 2, title->height() / 2)).toPoint());
        QVERIFY(!panel->property("panelsCollapsed").toBool());
        QTest::mouseClick(_window, Qt::LeftButton, Qt::NoModifier,
                          map->mapToScene(QPointF(map->width() * 0.4, map->height() * 0.7)).toPoint());
        QTRY_VERIFY_WITH_TIMEOUT(panel->property("panelsCollapsed").toBool(), TestTimeout::mediumMs());
        QCOMPARE(master->saveToJson(), beforeMapClick);
    }
    auto* mission = master->missionController();
    const QGeoCoordinate origin(30, 120, 20);
    const auto home = QGCGeo::geodesicDestination(origin, 0, 200);
    mission->setHomePosition(home);
    QVERIFY(editor->addWaypoint(origin));
    QVERIFY(editor->addWaypoint(QGCGeo::geodesicDestination(origin, 75, 60)));
    QVERIFY(editor->addWaypoint(QGCGeo::geodesicDestination(origin, 45, 180)));
    editor->clearHistory();
    const auto original = master->saveToJson();
    const auto desired = QGCGeo::geodesicDestination(origin, 90.5, 90);
    map->setProperty("center", QVariant::fromValue(desired));
    map->setProperty("zoomLevel", 19.0);
    map->setProperty("bearing", 35.0);
    auto* dragger = findVisibleItem(_rootItem, QStringLiteral("missionItemDrag_2"));
    QVERIFY(dragger);
    auto* indicator = dragger->property("itemIndicator").value<QQuickItem*>();
    QVERIFY(indicator);
    QSignalSpy ready(_window, &QQuickWindow::frameSwapped);
    _window->update();
    QVERIFY(ready.wait(TestTimeout::mediumMs()));
    auto* moving = mission->visualItems()->value<SimpleMissionItem*>(2);
    const auto altitude = moving->coordinate().altitude();
    const auto start = dragger->mapToScene(indicator->property("anchorPoint").toPointF()).toPoint();
    const auto target = map->mapToScene(QPointF(map->width() / 2, map->height() / 2)).toPoint();
    const auto camera = map->property("center").value<QGeoCoordinate>();
    QTest::mousePress(_window, Qt::LeftButton, Qt::NoModifier, start);
    auto* grabber = _window->mouseGrabberItem();
    QVERIFY(grabber);
    QCOMPARE(grabber->parentItem()->property("gestureTarget").value<QObject*>(), dragger);
    QTest::mouseMove(_window, start + QPoint(15, 15));
    QTest::mouseMove(_window, target);
    QVERIFY(panel->property("_draggedWaypoint").value<QObject*>());
    auto* guides = findVisibleItem(_rootItem, QStringLiteral("caacWaypointSnapGuides"));
    QVERIFY2(guides, qPrintable(QStringLiteral("Drag bearing %1, distance %2, guides %3, editor bound %4")
                                    .arg(QGCGeo::geodesicAzimuth(origin, moving->coordinate()))
                                    .arg(QGCGeo::geodesicDistance(origin, moving->coordinate()))
                                    .arg(panel->property("_dragGuides").toList().size())
                                    .arg(map->property("missionItemDragEditor").value<QObject*>() == panel)));
    QVERIFY(guides->property("guideCount").toInt() > 0);
    QTRY_VERIFY_WITH_TIMEOUT(
        std::abs(std::remainder(QGCGeo::geodesicAzimuth(origin, moving->coordinate()) - 90, 360)) < 0.001,
        TestTimeout::mediumMs());
    QCOMPARE(map->property("center").value<QGeoCoordinate>(), camera);
    QCOMPARE(moving->coordinate().altitude(), altitude);
    auto* label = findVisibleItem(_rootItem, QStringLiteral("caacWaypointAngle_2"));
    QVERIFY(label);
    const auto angle = editor->waypointAngles(false).last().toMap().value("angle").toDouble();
    QVERIFY(std::abs(label->property("angle").toDouble() - angle) < 0.001);
    if (const auto screenshot = qgetenv("CAAC_SNAP_UI_SCREENSHOT"); !screenshot.isEmpty()) {
        QVERIFY(_window->grabWindow().save(QString::fromUtf8(screenshot)));
    }
    QTest::mouseRelease(_window, Qt::LeftButton, Qt::NoModifier, target);
    QTRY_VERIFY_WITH_TIMEOUT(!findVisibleItem(_rootItem, QStringLiteral("caacWaypointSnapGuides"), 0),
                             TestTimeout::mediumMs());
    QVERIFY(clickButton(QStringLiteral("caacUndo")));
    QCOMPARE(master->saveToJson(), original);
    auto* plugin = qobject_cast<CaacCorePlugin*>(QGCCorePlugin::instance());
    QVERIFY(plugin);
    const auto preferences = plugin->planningPreferences();
    const auto restore = qScopeGuard([&]() { plugin->updatePlanningPreferences(preferences); });
    QVERIFY(plugin->updatePlanningPreferences({{"horizontalSnap", false},
                                               {"verticalSnap", false},
                                               {"angleSnap", false},
                                               {"symmetrySnap", false},
                                               {"midpointSnap", false},
                                               {"intersectionSnap", true}}));
    const auto intersection = QGCGeo::geodesicDestination(origin, 0, 50);
    moving = mission->visualItems()->value<SimpleMissionItem*>(2);
    QVERIFY(moving);
    QVERIFY(map->setProperty("center", QVariant::fromValue(moving->coordinate())));
    QVERIFY(map->setProperty("zoomLevel", 17.0));
    map->setProperty("bearing", 180.0);
    QTRY_VERIFY_WITH_TIMEOUT(map->property("mapReady").toBool(), TestTimeout::mediumMs());
    ready.clear();
    _window->update();
    QVERIFY(ready.wait(TestTimeout::mediumMs()));
    dragger = findVisibleItem(_rootItem, QStringLiteral("missionItemDrag_2"));
    QVERIFY(dragger);
    indicator = dragger->property("itemIndicator").value<QQuickItem*>();
    moving = mission->visualItems()->value<SimpleMissionItem*>(2);
    QVERIFY(indicator && moving);
    const auto dragAnchor = [&] {
        return dragger->mapToScene(indicator->property("anchorPoint").toPointF() +
                                   QPointF(dragger->property("_touchMarginHorizontal").toReal(),
                                           dragger->property("_touchMarginVertical").toReal()));
    };
    QTRY_VERIFY_WITH_TIMEOUT(
        QLineF(dragAnchor(), map->mapToScene(QPointF(map->width() / 2, map->height() / 2))).length() < 1,
        TestTimeout::mediumMs());
    QPointF intersectionPoint;
    QVERIFY(QMetaObject::invokeMethod(map, "fromCoordinate", Q_RETURN_ARG(QPointF, intersectionPoint),
                                      Q_ARG(QGeoCoordinate, QGCGeo::geodesicDestination(intersection, 90, 0.5)),
                                      Q_ARG(bool, false)));
    const auto intersectionStart = dragAnchor().toPoint();
    const auto intersectionTarget = map->mapToScene(intersectionPoint).toPoint();
    for (const auto& point : {intersectionStart, intersectionTarget}) {
        QVERIFY(map->mapRectToScene(map->boundingRect()).contains(point));
        for (const auto& name : {QStringLiteral("caacMainToolsScroller"), QStringLiteral("caacMapEditTools"),
                                 QStringLiteral("caacMissionDetailsButton"), QStringLiteral("caacRouteEditor")}) {
            auto* overlay = findVisibleItem(_rootItem, name, 0);
            QVERIFY2(!overlay || !overlay->mapRectToScene(overlay->boundingRect()).contains(point), qPrintable(name));
        }
    }
    QTest::mousePress(_window, Qt::LeftButton, Qt::NoModifier, intersectionStart);
    grabber = _window->mouseGrabberItem();
    QVERIFY(grabber);
    QVERIFY2(grabber->parentItem()->property("gestureTarget").value<QObject*>() == dragger,
             qPrintable(QStringLiteral("Start %1,%2; grabbed %3 / %4; panel %5")
                            .arg(intersectionStart.x())
                            .arg(intersectionStart.y())
                            .arg(grabber->objectName())
                            .arg(grabber->parentItem()->objectName())
                            .arg(panel->property("activePanel").toInt())));
    QTest::mouseMove(_window, intersectionStart + QPoint(15, 15));
    QTest::mouseMove(_window, intersectionTarget);
    QCOMPARE(panel->property("_draggedWaypoint").value<QObject*>(), moving);
    QTRY_VERIFY_WITH_TIMEOUT(panel->property("_dragGuides").toList().size() > 0, TestTimeout::mediumMs());
    QCOMPARE(panel->property("_dragGuides").toList().first().toMap().value("kind").toInt(), 4);
    QVERIFY2(QGCGeo::geodesicDistance(intersection, moving->coordinate()) < 10,
             qPrintable(QStringLiteral("Intersection drag offset: %1 m")
                            .arg(QGCGeo::geodesicDistance(intersection, moving->coordinate()))));
    QVERIFY(QGCGeo::geodesicDistance(
                panel->property("_dragGuides").toList().first().toMap().value("intersection").value<QGeoCoordinate>(),
                moving->coordinate()) < 0.01);
    QVERIFY(std::abs(std::remainder(QGCGeo::geodesicAzimuth(origin, moving->coordinate()), 360)) < 0.001);
    QCOMPARE(moving->coordinate().altitude(), altitude);
    if (const auto screenshot = qgetenv("CAAC_INTERSECTION_UI_SCREENSHOT"); !screenshot.isEmpty()) {
        QVERIFY(_window->grabWindow().save(QString::fromUtf8(screenshot)));
    }
    QTest::mouseRelease(_window, Qt::LeftButton, Qt::NoModifier, intersectionTarget);
    QTRY_VERIFY_WITH_TIMEOUT(!findVisibleItem(_rootItem, QStringLiteral("caacWaypointSnapGuides"), 0),
                             TestTimeout::mediumMs());
    QVERIFY(clickButton(QStringLiteral("caacUndo")));
    QCOMPARE(master->saveToJson(), original);
    QVERIFY(clickButton(QStringLiteral("caacWaypointListButton")));
    auto* files = findVisibleItem(_rootItem, QStringLiteral("caacWaypointFileActions"));
    QVERIFY(files);
    for (const auto* name : {"planToolbar_openButton", "planToolbar_saveButton", "planToolbar_clearButton",
                             "planToolbar_hamburgerButton"}) {
        auto* button = findVisibleItem(_rootItem, QLatin1String(name));
        QVERIFY(button);
        QVERIFY(files->isAncestorOf(button));
        QVERIFY(button->mapRectToItem(panel, button->boundingRect()).right() <= panel->width() + 1);
    }
    QVERIFY(!findVisibleItem(_rootItem, QStringLiteral("planToolbar_uploadButton"), 0));
    if (const auto screenshot = qgetenv("CAAC_WAYPOINT_SETTINGS_SCREENSHOT"); !screenshot.isEmpty()) {
        QVERIFY(_window->grabWindow().save(QString::fromUtf8(screenshot)));
    }
    map->setProperty("center", QVariant::fromValue(home));
    map->setProperty("bearing", 0.0);
    auto* homeDragger = findVisibleItem(_rootItem, QStringLiteral("missionItemDrag_0"));
    QVERIFY(homeDragger);
    const auto homePosition = [&]() {
        return homeDragger->mapToScene(QPointF(homeDragger->width() / 2, homeDragger->height() / 2));
    };
    QTRY_VERIFY_WITH_TIMEOUT(QLineF(homePosition(), target).length() < 1, TestTimeout::mediumMs());
    QTest::mouseClick(_window, Qt::RightButton, Qt::NoModifier, homePosition().toPoint());
    QVERIFY(clickButton(QStringLiteral("caacTakeoffPointButton")));
    QVERIFY(clickButton(QStringLiteral("caacContextHomeRelations")));
    auto* mode = findVisibleItem(_rootItem, QStringLiteral("caacRelationMode"));
    QVERIFY(mode);
    mode->setProperty("currentIndex", 1);
    auto* distance = findVisibleItem(_rootItem, QStringLiteral("caacRelationDistance"));
    QVERIFY(distance);
    distance->setProperty("text", QStringLiteral("10"));
    if (const auto screenshot = qgetenv("CAAC_HOME_RELATIONS_SCREENSHOT"); !screenshot.isEmpty()) {
        QVERIFY(_window->grabWindow().save(QString::fromUtf8(screenshot)));
    }
    QVERIFY(acceptDialog());
    const auto a = mission->visualItems()->value<SimpleMissionItem*>(1)->coordinate();
    const auto b = mission->visualItems()->value<SimpleMissionItem*>(2)->coordinate();
    const auto expected = QGCGeo::geodesicDestination(b, QGCGeo::geodesicAzimuth(b, a) + 180, 10);
    QVERIFY(QGCGeo::geodesicDistance(expected, mission->plannedHomePosition()) < 0.01);
    QCOMPARE(mission->plannedHomePosition().altitude(), home.altitude());
    auto* undoButton = findVisibleItem(_rootItem, QStringLiteral("caacUndo"));
    QVERIFY(undoButton && undoButton->isEnabled());
    QSignalSpy undoClick(undoButton, SIGNAL(clicked()));
    QVERIFY(undoClick.isValid());
    QVERIFY(clickButton(QStringLiteral("caacUndo")));
    QCOMPARE(undoClick.count(), 1);
    QCOMPARE(master->saveToJson(), original);
}

void CaacPlanningInteractionUITest::_homeRelativeEditing()
{
    startUI();
    if (QTest::currentTestFailed()) {
        return;
    }
    QVERIFY(QMetaObject::invokeMethod(_window, "showPlanView"));
    auto* plan = findVisibleItem(_rootItem, QStringLiteral("mainView_plan"));
    auto* map = findVisibleItem(_rootItem, QStringLiteral("planView_map"));
    auto* panel = findVisibleItem(_rootItem, QStringLiteral("caacRouteEditor"));
    QVERIFY(plan && map && panel);
    auto* master = plan->property("_planMasterController").value<PlanMasterController*>();
    auto* editor = panel->property("routeController").value<CaacRouteEditorController*>();
    QVERIFY(master && editor);
    auto* mission = master->missionController();
    const QGeoCoordinate home(30, 120, 20);
    mission->setHomePosition(home);
    const auto reference = QGCGeo::geodesicDestination(home, 90, 100);
    QVERIFY(editor->addWaypoint(reference));
    QVERIFY(editor->addWaypoint(QGCGeo::geodesicDestination(home, 180, 130)));
    const auto original = master->saveToJson();
    map->setProperty("zoomLevel", 18);
    const auto midpoint = QGCGeo::geodesicDestination(home, 90, 50);
    map->setProperty("center", QVariant::fromValue(midpoint));
    const auto mapMiddle = map->mapToScene(QPointF(map->width() / 2, map->height() / 2));
    QTest::mouseMove(_window, (mapMiddle + QPointF(-80, -80)).toPoint());
    QVERIFY(!findVisibleItem(_rootItem, QStringLiteral("caacRouteSplitButton"), 0));
    QPointF hoverPoint;
    QVERIFY(QMetaObject::invokeMethod(map, "fromCoordinate", Q_RETURN_ARG(QPointF, hoverPoint),
                                      Q_ARG(QGeoCoordinate, QGCGeo::geodesicDestination(home, 90, 25)),
                                      Q_ARG(bool, false)));
    QTest::mouseMove(_window, map->mapToScene(hoverPoint).toPoint());
    auto* split = findVisibleItem(_rootItem, QStringLiteral("caacRouteSplitButton"), TestTimeout::mediumMs());
    QVERIFY(split);
    QTRY_VERIFY_WITH_TIMEOUT(QLineF(split->mapToScene(QPointF(split->width() / 2, split->height() / 2)),
                                    map->mapToScene(QPointF(map->width() / 2, map->height() / 2)))
                                     .length() < 1,
                             TestTimeout::mediumMs());
    panel->setProperty("measurePick", 1);
    QTRY_VERIFY_WITH_TIMEOUT(!findVisibleItem(_rootItem, QStringLiteral("caacRouteSplitButton"), 0),
                             TestTimeout::mediumMs());
    QCOMPARE(mission->visualItems()->count(), 3);
    panel->setProperty("measurePick", 0);
    QVERIFY(clickButton(QStringLiteral("caacRouteSplitButton")));
    QTRY_COMPARE_WITH_TIMEOUT(mission->visualItems()->count(), 4, TestTimeout::mediumMs());
    auto* inserted = mission->visualItems()->value<SimpleMissionItem*>(1);
    QVERIFY(inserted && inserted->command() == MAV_CMD_NAV_WAYPOINT);
    QVERIFY(QGCGeo::geodesicDistance(inserted->coordinate(), midpoint) < 0.01);
    QVERIFY(QGCGeo::geodesicDistance(mission->visualItems()->value<SimpleMissionItem*>(2)->coordinate(), reference) <
            0.01);
    QCOMPARE(mission->plannedHomePosition().altitude(), 20.0);
    _window->requestActivate();
    QTRY_VERIFY_WITH_TIMEOUT(_window->isActive(), TestTimeout::mediumMs());
    QTest::keyClick(_window, Qt::Key_Z, Qt::ControlModifier);
    QTRY_COMPARE_WITH_TIMEOUT(master->saveToJson(), original, TestTimeout::mediumMs());
    map->setProperty("center", QVariant::fromValue(home));
    auto* dragger = findVisibleItem(_rootItem, QStringLiteral("missionItemDrag_0"), TestTimeout::mediumMs());
    QVERIFY(dragger);
    const auto position = [&]() { return dragger->mapToScene(QPointF(dragger->width() / 2, dragger->height() / 2)); };
    QTRY_VERIFY_WITH_TIMEOUT(
        QLineF(position(), map->mapToScene(QPointF(map->width() / 2, map->height() / 2))).length() < 1,
        TestTimeout::mediumMs());
    QVERIFY(!_window->grabWindow().isNull());
    QTest::mouseClick(_window, Qt::RightButton, Qt::NoModifier, position().toPoint());
    QVERIFY(findVisibleItem(_rootItem, QStringLiteral("caacDrawingAidButton")));
    QVERIFY(findVisibleItem(_rootItem, QStringLiteral("caacPlaceCoordinateAxes")));
    QVERIFY(findVisibleItem(_rootItem, QStringLiteral("caacContextClearWaypoints")));
    QVERIFY(findVisibleItem(_rootItem, QStringLiteral("caacContextReverse")));
    QVERIFY(!findVisibleItem(_rootItem, QStringLiteral("caacContextDeleteWaypoint"), 0));
    QTest::keyClick(_window, Qt::Key_Escape);
    QTest::mouseDClick(_window, Qt::LeftButton, Qt::NoModifier, position().toPoint());
    auto* combo = findVisibleItem(_rootItem, QStringLiteral("caacHomeRelativeReference"));
    auto* bearing = findVisibleItem(_rootItem, QStringLiteral("caacWaypointQuickBearing"));
    auto* distance = findVisibleItem(_rootItem, QStringLiteral("caacWaypointQuickDistance"));
    QVERIFY(combo && bearing && distance);
    auto* homeContent = findVisibleItem(_rootItem, QStringLiteral("caacWaypointPopupContent"));
    QVERIFY(homeContent);
    QTRY_VERIFY_WITH_TIMEOUT(homeContent->property("popupGeometry").toRectF().height() > 0 &&
                                 homeContent->property("popupGeometry").toRectF().height() <= map->height(),
                             TestTimeout::mediumMs());
    QVERIFY(homeContent->property("popupGeometry").toRectF().width() <= map->width());
    QCOMPARE(combo->property("count").toInt(), 2);
    QCOMPARE(combo->property("currentIndex").toInt(), 0);
    QVERIFY(std::abs(distance->property("text").toString().toDouble() - 100) < 0.01);
    QVERIFY(!findVisibleItem(_rootItem, QStringLiteral("caacWaypointQuickDelete"), 0));
    combo->setProperty("currentIndex", 1);
    QVERIFY(QMetaObject::invokeMethod(combo, "activated", Q_ARG(int, 1)));
    QVERIFY(std::abs(distance->property("text").toString().toDouble() - 130) < 0.01);
    QCOMPARE(master->saveToJson(), original);
    combo->setProperty("currentIndex", 0);
    QVERIFY(QMetaObject::invokeMethod(combo, "activated", Q_ARG(int, 0)));
    QVERIFY(clickButton(QStringLiteral("caacWaypointFineTuning")));
    _window->requestActivate();
    QTRY_VERIFY_WITH_TIMEOUT(_window->isActive(), TestTimeout::mediumMs());
    QTest::keyClick(_window, Qt::Key_Right);
    QTRY_VERIFY_WITH_TIMEOUT(
        std::abs(QGCGeo::geodesicDistance(reference, mission->plannedHomePosition()) - 100.01) < 0.001,
        TestTimeout::mediumMs());
    QCOMPARE(mission->plannedHomePosition().altitude(), 20.0);
    auto* undoShortcut = panel->findChild<QObject*>(QStringLiteral("caacRouteUndoShortcut"));
    QVERIFY(undoShortcut);
    QVERIFY(undoShortcut->property("enabled").toBool());
    auto* homePopup = panel->findChild<QObject*>(QStringLiteral("caacWaypointPopup"));
    QVERIFY(homePopup);
    QSignalSpy popupClosed(homePopup, SIGNAL(closed()));
    QSignalSpy undoActivated(undoShortcut, SIGNAL(activated()));
    QTest::keyClick(_window, Qt::Key_Z, Qt::ControlModifier);
    QTRY_COMPARE_WITH_TIMEOUT(undoActivated.count(), 1, TestTimeout::mediumMs());
    QTRY_COMPARE_WITH_TIMEOUT(master->saveToJson(), original, TestTimeout::mediumMs());
    QTRY_COMPARE_WITH_TIMEOUT(popupClosed.count(), 1, TestTimeout::mediumMs());
    QVERIFY(!findVisibleItem(_rootItem, QStringLiteral("caacWaypointQuickBearing"), 0));
    auto* restoredHome = findVisibleItem(_rootItem, QStringLiteral("missionItemDrag_0"));
    QVERIFY(restoredHome);
    QTRY_VERIFY_WITH_TIMEOUT(
        QLineF(restoredHome->mapToScene(QPointF(restoredHome->width() / 2, restoredHome->height() / 2)),
               map->mapToScene(QPointF(map->width() / 2, map->height() / 2)))
                .length() < 1,
        TestTimeout::mediumMs());
    QVERIFY(!_window->grabWindow().isNull());
    QTest::mouseDClick(
        _window, Qt::LeftButton, Qt::NoModifier,
        restoredHome->mapToScene(QPointF(restoredHome->width() / 2, restoredHome->height() / 2)).toPoint());
    bearing = findVisibleItem(_rootItem, QStringLiteral("caacWaypointQuickBearing"));
    distance = findVisibleItem(_rootItem, QStringLiteral("caacWaypointQuickDistance"));
    QVERIFY(bearing && distance);
    if (const auto screenshot = qgetenv("CAAC_HOME_UI_SCREENSHOT"); !screenshot.isEmpty()) {
        QVERIFY(_window->grabWindow().save(QString::fromUtf8(screenshot)));
    }
    bearing->setProperty("text", QStringLiteral("238"));
    distance->setProperty("text", QStringLiteral("28.1"));
    QVERIFY(clickButton(QStringLiteral("caacWaypointQuickConfirm")));
    QVERIFY(QGCGeo::geodesicDistance(mission->plannedHomePosition(),
                                     QGCGeo::geodesicDestination(reference, 238, 28.1)) < 0.01);
    QCOMPARE(mission->plannedHomePosition().altitude(), 20.0);
    QCOMPARE(mission->visualItems()->value<SimpleMissionItem*>(1)->coordinate().latitude(), reference.latitude());
    QVERIFY(clickButton(QStringLiteral("caacUndo")));
    QCOMPARE(master->saveToJson(), original);
    map->setProperty("center", QVariant::fromValue(reference));
    auto* firstDragger = findVisibleItem(_rootItem, QStringLiteral("missionItemDrag_1"));
    QVERIFY(firstDragger);
    const auto firstPosition = [&]() {
        return firstDragger->mapToScene(QPointF(firstDragger->width() / 2, firstDragger->height() / 2));
    };
    QTRY_VERIFY_WITH_TIMEOUT(QLineF(firstPosition(), mapMiddle).length() < 1, TestTimeout::mediumMs());
    QTest::mouseClick(_window, Qt::RightButton, Qt::NoModifier, firstPosition().toPoint());
    QVERIFY(clickButton(QStringLiteral("caacTakeoffPointButton")));
    const auto cameraBeforeCenter = map->property("center").value<QGeoCoordinate>();
    QVERIFY(clickButton(QStringLiteral("caacContextCenterHome")));
    const QGeoCoordinate expectedCenter(
        (reference.latitude() + QGCGeo::geodesicDestination(home, 180, 130).latitude()) / 2,
        (reference.longitude() + home.longitude()) / 2, 20);
    QTRY_VERIFY_WITH_TIMEOUT(QGCGeo::geodesicDistance(mission->plannedHomePosition(), expectedCenter) < 0.01,
                             TestTimeout::mediumMs());
    QCOMPARE(map->property("center").value<QGeoCoordinate>(), cameraBeforeCenter);
    QCOMPARE(mission->plannedHomePosition().altitude(), 20.0);
    QTest::keyClick(_window, Qt::Key_Z, Qt::ControlModifier);
    QTRY_COMPARE_WITH_TIMEOUT(master->saveToJson(), original, TestTimeout::mediumMs());
}

void CaacPlanningInteractionUITest::_liveRoutePreview()
{
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
    QVERIFY(master && editor);
    const QGeoCoordinate home(30, 120, 20);
    master->missionController()->setHomePosition(home);
    QList<QGeoCoordinate> points;
    for (int bearing : {0, 90, 180}) {
        points.append(QGCGeo::geodesicDestination(home, bearing, 40));
        QVERIFY(editor->addWaypoint(points.last()));
    }
    const auto original = master->saveToJson();
    QVERIFY(!editor->hasSimulationSettings());
    QVERIFY(clickButton(QStringLiteral("caacGlobalSettingsButton")));
    QVERIFY(clickButtonScrolled(QStringLiteral("caacSimClosedRoute"), QStringLiteral("caacGlobalSettingsScroller")));
    const auto matches = [](QQuickItem* line, const QGeoCoordinate& from, const QGeoCoordinate& to) {
        if (!line) {
            return false;
        }
        QQmlExpression expression(qmlContext(line), line,
                                  QStringLiteral("path.length === 2 && Math.abs(path[0].latitude - %1) < 1e-8 && "
                                                 "Math.abs(path[0].longitude - %2) < 1e-8 && "
                                                 "Math.abs(path[1].latitude - %3) < 1e-8 && "
                                                 "Math.abs(path[1].longitude - %4) < 1e-8")
                                      .arg(from.latitude(), 0, 'g', 17)
                                      .arg(from.longitude(), 0, 'g', 17)
                                      .arg(to.latitude(), 0, 'g', 17)
                                      .arg(to.longitude(), 0, 'g', 17));
        const bool value = expression.evaluate().toBool();
        return !expression.hasError() && value;
    };
    QTRY_VERIFY_WITH_TIMEOUT(matches(findVisibleItem(_rootItem, QStringLiteral("caacSimulationPreview_closure"), 0),
                                     points.last(), points.first()),
                             TestTimeout::mediumMs());
    auto* start = findVisibleItem(_rootItem, QStringLiteral("caacSimStartPoint"));
    auto* returning = findVisibleItem(_rootItem, QStringLiteral("caacSimReturnPoint"));
    auto* loop = findVisibleItem(_rootItem, QStringLiteral("caacSimLoopMode"));
    QVERIFY(start && returning && loop);
    QVERIFY(loop->setProperty("currentIndex", 1));
    auto* count = findVisibleItem(_rootItem, QStringLiteral("caacSimLoopCount"));
    QVERIFY(count);
    QVERIFY(start->setProperty("currentIndex", 2));
    QVERIFY(returning->setProperty("currentIndex", 3));
    QTRY_VERIFY_WITH_TIMEOUT(
        matches(findVisibleItem(_rootItem, QStringLiteral("caacSimulationPreview_start"), 0), home, points.at(1)),
        TestTimeout::mediumMs());
    QTRY_VERIFY_WITH_TIMEOUT(matches(findVisibleItem(_rootItem, QStringLiteral("caacSimulationPreview_closure"), 0),
                                     points.first(), points.at(1)),
                             TestTimeout::mediumMs());
    QTRY_VERIFY_WITH_TIMEOUT(matches(findVisibleItem(_rootItem, QStringLiteral("caacSimulationPreview_return"), 0),
                                     points.at(1), points.last()),
                             TestTimeout::mediumMs());
    QVERIFY(clickButtonScrolled(QStringLiteral("caacSimClosedRoute"), QStringLiteral("caacGlobalSettingsScroller")));
    QVERIFY(loop->setProperty("currentIndex", 1));
    QVERIFY(count->setProperty("text", QStringLiteral("2")));
    QTRY_VERIFY_WITH_TIMEOUT(!findVisibleItem(_rootItem, QStringLiteral("caacSimulationPreview_closure"), 0),
                             TestTimeout::mediumMs());
    QTRY_VERIFY_WITH_TIMEOUT(matches(findVisibleItem(_rootItem, QStringLiteral("caacSimulationPreview_loop"), 0),
                                     points.last(), points.at(1)),
                             TestTimeout::mediumMs());
    QCOMPARE(master->saveToJson(), original);
    QVERIFY(!editor->hasSimulationSettings());
    auto* item = master->missionController()->visualItems()->value<SimpleMissionItem*>(2);
    QVERIFY(item);
    const auto moved = QGCGeo::geodesicDestination(home, 100, 60);
    item->setCoordinate(moved);
    QTRY_VERIFY_WITH_TIMEOUT(
        matches(findVisibleItem(_rootItem, QStringLiteral("caacSimulationPreview_start"), 0), home, moved),
        TestTimeout::mediumMs());
    QTRY_VERIFY_WITH_TIMEOUT(
        matches(findVisibleItem(_rootItem, QStringLiteral("caacSimulationPreview_loop"), 0), points.last(), moved),
        TestTimeout::mediumMs());
    QVERIFY(clickButtonScrolled(QStringLiteral("caacSaveSimulationSettings"),
                                QStringLiteral("caacGlobalSettingsScroller")));
    QVERIFY(editor->hasSimulationSettings());
    QCOMPARE(editor->simulationSettings().value("startSequence").toInt(), 2);
    QVERIFY(clickButton(QStringLiteral("caacWaypointListButton")));
    QTRY_VERIFY_WITH_TIMEOUT(
        matches(findVisibleItem(_rootItem, QStringLiteral("caacSimulationPreview_loop"), 0), points.last(), moved),
        TestTimeout::mediumMs());
}

void CaacPlanningInteractionUITest::_pointMenusAndOrientation()
{
    startUI();
    if (QTest::currentTestFailed()) {
        return;
    }
    QVERIFY(QMetaObject::invokeMethod(_window, "showPlanView"));
    auto* plan = findVisibleItem(_rootItem, QStringLiteral("mainView_plan"));
    auto* map = findVisibleItem(_rootItem, QStringLiteral("planView_map"));
    auto* panel = findVisibleItem(_rootItem, QStringLiteral("caacRouteEditor"));
    QVERIFY(plan && map && panel);
    auto* master = plan->property("_planMasterController").value<PlanMasterController*>();
    auto* editor = panel->property("routeController").value<CaacRouteEditorController*>();
    QVERIFY(master && editor);
    QVERIFY(clickButton(QStringLiteral("caacWaypointListButton")));
    for (const auto* name : {"caacReadWaypoints", "caacUploadWaypoints"}) {
        auto* button = findVisibleItem(_rootItem, QLatin1String(name));
        QVERIFY(button && !button->isEnabled());
    }
    auto* mission = master->missionController();
    const QGeoCoordinate home(30, 120, 20);
    mission->setHomePosition(home);
    const auto a = QGCGeo::geodesicDestination(home, 90, 7.1);
    QVERIFY(editor->addWaypoint(a));
    QVERIFY(editor->addWaypoint(a));
    QTRY_COMPARE_WITH_TIMEOUT(mission->simpleFlightPathSegments()->count(), 2, TestTimeout::mediumMs());
    auto* firstLeg = mission->simpleFlightPathSegments()->value<FlightPathSegment*>(0);
    QVERIFY(firstLeg);
    QVERIFY(QGCGeo::geodesicDistance(firstLeg->coordinate1(), home) < 0.01);
    QVERIFY(QGCGeo::geodesicDistance(firstLeg->coordinate2(), a) < 0.01);
    QVERIFY(firstLeg->setProperty("terrainCollision", true));
    auto* clearanceWarning = findVisibleItem(_rootItem, QStringLiteral("caacTerrainClearanceWarning"));
    QVERIFY(clearanceWarning);
    const auto originalWarningPosition = clearanceWarning->mapToScene(QPointF());
    const auto warningPoint =
        clearanceWarning->mapToScene(QPointF(clearanceWarning->width() / 2, clearanceWarning->height() / 2)).toPoint();
    QTest::mousePress(_window, Qt::LeftButton, Qt::NoModifier, warningPoint);
    QTest::mouseMove(_window, warningPoint + QPoint(15, 15));
    QTest::mouseMove(_window, warningPoint + QPoint(70, 60));
    QTest::mouseRelease(_window, Qt::LeftButton, Qt::NoModifier, warningPoint + QPoint(70, 60));
    QTRY_VERIFY_WITH_TIMEOUT(clearanceWarning->mapToScene(QPointF()).x() > originalWarningPosition.x() + 30 &&
                                 clearanceWarning->mapToScene(QPointF()).y() > originalWarningPosition.y() + 30,
                             TestTimeout::mediumMs());
    QVERIFY(firstLeg->setProperty("terrainCollision", false));
    QTRY_VERIFY_WITH_TIMEOUT(!findVisibleItem(_rootItem, QStringLiteral("caacTerrainClearanceWarning"), 0),
                             TestTimeout::mediumMs());
    map->setProperty("zoomLevel", 18);
    map->setProperty("center", QVariant::fromValue(home));
    auto* homeDragger = findVisibleItem(_rootItem, QStringLiteral("missionItemDrag_0"), TestTimeout::mediumMs());
    QVERIFY(homeDragger);
    const auto beforeHomeDrag = master->saveToJson();
    mission->setDirty(false);
    const auto neighborBeforeHomeDrag = mission->visualItems()->value<SimpleMissionItem*>(2)->coordinate();
    const auto homeMapCenter = map->property("center").value<QGeoCoordinate>();
    QTRY_VERIFY_WITH_TIMEOUT(
        QLineF(homeDragger->mapToScene(QPointF(homeDragger->width() / 2, homeDragger->height() / 2)),
               map->mapToScene(QPointF(map->width() / 2, map->height() / 2)))
                .length() < 1,
        TestTimeout::mediumMs());
    QVERIFY(!_window->grabWindow().isNull());
    const auto homeScenePosition = [homeDragger]() {
        for (auto* ancestor = homeDragger; ancestor; ancestor = ancestor->parentItem()) {
            ancestor->ensurePolished();
        }
        return homeDragger->mapToScene(QPointF(homeDragger->width() / 2, homeDragger->height() / 2));
    };
    QPointF lastHomePosition(qQNaN(), qQNaN());
    QVERIFY(waitForCondition(
        [&]() {
            const auto position = homeScenePosition();
            const bool stable = position == lastHomePosition;
            lastHomePosition = position;
            return stable;
        },
        TestTimeout::shortMs(), QStringLiteral("Home drag position settled")));
    const auto homePress = homeScenePosition().toPoint();
    QTest::mousePress(_window, Qt::LeftButton, Qt::NoModifier, homePress);
    auto* homeGrabber = _window->mouseGrabberItem();
    QVERIFY(homeGrabber);
    QTRY_VERIFY_WITH_TIMEOUT(homeGrabber->property("pressed").toBool(), TestTimeout::mediumMs());
    QCOMPARE(homeGrabber->parentItem()->property("gestureTarget").value<QObject*>(), homeDragger);
    for (int step = 1; step <= 5; ++step) {
        QTest::mouseMove(_window, homePress + QPoint(step * 8, step * 4));
    }
    QTRY_VERIFY_WITH_TIMEOUT(QGCGeo::geodesicDistance(mission->plannedHomePosition(), home) > 1,
                             TestTimeout::mediumMs());
    QTest::mouseRelease(_window, Qt::LeftButton, Qt::NoModifier, homePress + QPoint(40, 20));
    QTRY_VERIFY_WITH_TIMEOUT(QGCGeo::geodesicDistance(mission->plannedHomePosition(), home) > 1,
                             TestTimeout::mediumMs());
    QCOMPARE(mission->plannedHomePosition().altitude(), home.altitude());
    QVERIFY(mission->dirty());
    QCOMPARE(mission->currentPlanViewSeqNum(), 0);
    const auto draggedHome = mission->plannedHomePosition();
    const auto savedHome =
        master->saveToJson().object().value("mission").toObject().value("plannedHomePosition").toArray();
    QVERIFY(savedHome.size() >= 3);
    QCOMPARE(savedHome.at(0).toDouble(), draggedHome.latitude());
    QCOMPARE(savedHome.at(1).toDouble(), draggedHome.longitude());
    QCOMPARE(mission->visualItems()->value<SimpleMissionItem*>(2)->coordinate(), neighborBeforeHomeDrag);
    QVERIFY(QGCGeo::geodesicDistance(homeMapCenter, map->property("center").value<QGeoCoordinate>()) < 0.02);
    auto* undoButton = findVisibleItem(_rootItem, QStringLiteral("caacUndo"));
    QVERIFY(undoButton && undoButton->isEnabled());
    QSignalSpy undoClick(undoButton, SIGNAL(clicked()));
    QVERIFY(undoClick.isValid());
    QVERIFY(clickButton(QStringLiteral("caacUndo")));
    QCOMPARE(undoClick.count(), 1);
    QCOMPARE(master->saveToJson(), beforeHomeDrag);
    mission->setCurrentPlanViewSeqNum(1, true);
    map->setProperty("center", QVariant::fromValue(a));
    auto* dragger = findVisibleItem(_rootItem, QStringLiteral("missionItemDrag_2"), TestTimeout::mediumMs());
    QVERIFY(dragger);
    auto* indicator = dragger->property("itemIndicator").value<QQuickItem*>();
    QVERIFY(indicator);
    QTRY_VERIFY_WITH_TIMEOUT(findVisibleItem(_rootItem, QStringLiteral("missionItemMapLabel_2"), 0),
                             TestTimeout::mediumMs());
    QTRY_VERIFY_WITH_TIMEOUT(!findVisibleItem(_rootItem, QStringLiteral("missionItemMapLabel_1"), 0),
                             TestTimeout::mediumMs());
    const auto markerPosition = [&]() {
        return dragger->mapToScene(indicator->property("anchorPoint").toPointF() +
                                   QPointF(dragger->property("_touchMarginHorizontal").toDouble(),
                                           dragger->property("_touchMarginVertical").toDouble()));
    };
    const auto middle = map->mapToScene(QPointF(map->width() / 2, map->height() / 2));
    QTRY_VERIFY_WITH_TIMEOUT(QLineF(markerPosition(), middle).length() < 1, TestTimeout::mediumMs());
    QTest::mouseMove(_window, markerPosition().toPoint());
    auto* overlap = findVisibleItem(_rootItem, QStringLiteral("caacWaypointOverlapText"), TestTimeout::mediumMs());
    QVERIFY(overlap);
    const auto labels = overlap->property("text").toString();
    QVERIFY(labels.contains(QStringLiteral("1")) && labels.contains(QStringLiteral("2")));
    QVERIFY(!labels.contains(QCoreApplication::translate("CaacWaypointOverlap", "Takeoff point")));
    QTest::mousePress(_window, Qt::LeftButton, Qt::NoModifier, markerPosition().toPoint());
    auto* waypointGrabber = _window->mouseGrabberItem();
    QVERIFY(waypointGrabber);
    QCOMPARE(waypointGrabber->parentItem()->property("gestureTarget").value<QObject*>(), dragger);
    QTest::mouseRelease(_window, Qt::LeftButton, Qt::NoModifier, markerPosition().toPoint());
    QVERIFY(!findVisibleItem(_rootItem, QStringLiteral("caacWaypointQuickBearing"), 0));
    QTest::mouseDClick(_window, Qt::LeftButton, Qt::NoModifier, markerPosition().toPoint());
    QVERIFY(findVisibleItem(_rootItem, QStringLiteral("caacWaypointQuickBearing")));
    QVERIFY(findVisibleItem(_rootItem, QStringLiteral("caacWaypointQuickDistance")));
    auto* popup = findVisibleItem(_rootItem, QStringLiteral("caacWaypointPopupContent"));
    QVERIFY(popup);
    const auto popupGeometry = [&]() { return popup->property("popupGeometry").toRectF(); };
    QVERIFY(!_window->grabWindow().isNull());
    const auto initialPopupGeometry = popupGeometry();
    for (int toggle = 0; toggle < 2; ++toggle) {
        QVERIFY(clickButton(QStringLiteral("caacWaypointFineTuning")));
        QVERIFY(!_window->grabWindow().isNull());
        QCOMPARE(popupGeometry(), initialPopupGeometry);
    }
    const auto selected = mission->currentPlanViewItem()->sequenceNumber();
    auto* waypoint = editor->currentItem();
    QVERIFY(waypoint);
    const double initialDistance = editor->waypointLeg(waypoint).value("distance").toDouble();
    QVERIFY(clickButton(QStringLiteral("caacWaypointFineTuning")));
    _window->requestActivate();
    QTRY_VERIFY_WITH_TIMEOUT(_window->isActive(), TestTimeout::mediumMs());
    QTest::keyClick(_window, Qt::Key_Right);
    QTRY_VERIFY_WITH_TIMEOUT(
        std::abs(editor->waypointLeg(waypoint).value("distance").toDouble() - initialDistance - 0.01) < 0.001,
        TestTimeout::mediumMs());
    const auto initialBearing = editor->waypointLeg(waypoint).value("bearing").toDouble();
    QTest::keyClick(_window, Qt::Key_Up);
    QTRY_VERIFY_WITH_TIMEOUT(
        std::abs(std::remainder(editor->waypointLeg(waypoint).value("bearing").toDouble() - initialBearing - 0.01,
                                360)) < 0.001,
        TestTimeout::mediumMs());
    QTest::keyClick(_window, Qt::Key_Down);
    QVERIFY(std::abs(std::remainder(editor->waypointLeg(waypoint).value("bearing").toDouble() - initialBearing, 360)) <
            0.001);
    QTest::keyClick(_window, Qt::Key_Left);
    QVERIFY(std::abs(editor->waypointLeg(waypoint).value("distance").toDouble() - initialDistance) < 0.001);
    if (const auto screenshot = qgetenv("CAAC_POINT_UI_SCREENSHOT"); !screenshot.isEmpty()) {
        QVERIFY(_window->grabWindow().save(QString::fromUtf8(screenshot)));
    }
    auto* quickBearing = findVisibleItem(_rootItem, QStringLiteral("caacWaypointQuickBearing"));
    auto* quickDistance = findVisibleItem(_rootItem, QStringLiteral("caacWaypointQuickDistance"));
    QVERIFY(quickBearing && quickDistance);
    quickBearing->setProperty("text", QStringLiteral("90.44"));
    quickDistance->setProperty("text", QStringLiteral("360.65"));
    QVERIFY(clickButton(QStringLiteral("caacWaypointQuickConfirm")));
    QVERIFY(std::abs(editor->waypointLeg(waypoint).value("distance").toDouble() - 360.65) < 0.001);
    QVERIFY(std::abs(editor->waypointLeg(waypoint).value("bearing").toDouble() - 90.44) < 0.001);
    QVERIFY(!findVisibleItem(_rootItem, QStringLiteral("caacWaypointQuickBearing"), 0));
    map->setProperty("center", QVariant::fromValue(a));
    QSignalSpy hoverFrame(_window, &QQuickWindow::frameSwapped);
    _window->update();
    QVERIFY(hoverFrame.wait(TestTimeout::mediumMs()));
    QTest::mouseMove(_window, middle.toPoint());
    QTRY_VERIFY_WITH_TIMEOUT(!findVisibleItem(_rootItem, QStringLiteral("caacWaypointOverlapText"), 0),
                             TestTimeout::mediumMs());
    map->setProperty("center", QVariant::fromValue(waypoint->coordinate()));
    QTRY_VERIFY_WITH_TIMEOUT(QLineF(markerPosition(), middle).length() < 1, TestTimeout::mediumMs());
    QTest::mouseClick(_window, Qt::RightButton, Qt::NoModifier, markerPosition().toPoint());
    QVERIFY(findVisibleItem(_rootItem, QStringLiteral("caacContextDeleteWaypoint")));
    auto* gotoButton = findVisibleItem(_rootItem, QStringLiteral("caacContextGoto"));
    QVERIFY(gotoButton && !gotoButton->isEnabled());
    if (const auto screenshot = qgetenv("CAAC_POINT_MENU_SCREENSHOT"); !screenshot.isEmpty()) {
        QVERIFY(_window->grabWindow().save(QString::fromUtf8(screenshot)));
    }
    QVERIFY(clickButton(QStringLiteral("caacDrawingAidButton")));
    for (const auto* name : {"caacAuxiliaryCircleRadius", "caacAuxiliaryLineBearing"}) {
        auto* field = findVisibleItem(_rootItem, QLatin1String(name));
        QVERIFY(field);
        QCOMPARE(field->property("placeholderTextColor").value<QColor>(), field->property("color").value<QColor>());
    }
    QVERIFY(clickButton(QStringLiteral("caacAuxiliaryCircleRadius")));
    QTest::keyClick(_window, '5');
    QTest::keyClick(_window, '0');
    QTest::keyClick(_window, Qt::Key_Return);
    auto* circle = findVisibleItem(_rootItem, QStringLiteral("caacAuxiliaryCircle"));
    QVERIFY(circle);
    QCOMPARE(circle->property("radius").toDouble(), 50.0);
    QTest::mouseClick(_window, Qt::RightButton, Qt::NoModifier, markerPosition().toPoint());
    QVERIFY(clickButton(QStringLiteral("caacDrawingAidButton")));
    QVERIFY(clickButton(QStringLiteral("caacAuxiliaryLineBearing")));
    QTest::keyClick(_window, '9');
    QTest::keyClick(_window, '0');
    QTest::keyClick(_window, Qt::Key_Return);
    QVERIFY(findVisibleItem(_rootItem, QStringLiteral("caacAuxiliaryLine")));
    QTest::keyClick(_window, Qt::Key_Escape);
    QTest::keyClick(_window, Qt::Key_Escape);
    QTest::mouseClick(_window, Qt::RightButton, Qt::NoModifier, markerPosition().toPoint());
    QVERIFY(clickButton(QStringLiteral("caacPlaceCoordinateAxes")));
    auto* axes = findVisibleItem(_rootItem, QStringLiteral("caacCoordinateAxes"));
    auto* graphic = findVisibleItem(_rootItem, QStringLiteral("caacCoordinateAxesGraphic"));
    QVERIFY(axes && graphic);
    QVERIFY(graphic->width() < map->width() / 2);
    for (const auto& direction : {QStringLiteral("N"), QStringLiteral("S"), QStringLiteral("W"), QStringLiteral("E")}) {
        auto* label = findVisibleItem(_rootItem, QStringLiteral("caacAxesDirection_") + direction);
        QVERIFY(label);
        QCOMPARE(label->property("text").toString(), direction);
    }
    const auto axesOrigin = axes->property("coordinate").value<QGeoCoordinate>();
    const auto mapCenter = map->property("center").value<QGeoCoordinate>();
    const auto beforeAxesDrag = master->saveToJson();
    const auto armPosition = [&]() {
        return graphic->mapToScene(QPointF(graphic->width() * 0.75, graphic->height() / 2)).toPoint();
    };
    const auto arm = armPosition();
    QTest::mousePress(_window, Qt::LeftButton, Qt::NoModifier, arm);
    QTest::mouseMove(_window, arm + QPoint(60, 40));
    QTest::mouseRelease(_window, Qt::LeftButton, Qt::NoModifier, arm + QPoint(60, 40));
    QTRY_VERIFY_WITH_TIMEOUT(
        QGCGeo::geodesicDistance(axesOrigin, axes->property("coordinate").value<QGeoCoordinate>()) > 1,
        TestTimeout::mediumMs());
    QCOMPARE(master->saveToJson(), beforeAxesDrag);
    QVERIFY(QGCGeo::geodesicDistance(mapCenter, map->property("center").value<QGeoCoordinate>()) < 0.02);
    if (const auto screenshot = qgetenv("CAAC_AXES_UI_SCREENSHOT"); !screenshot.isEmpty()) {
        QVERIFY(_window->grabWindow().save(QString::fromUtf8(screenshot)));
    }
    QTest::mouseClick(_window, Qt::RightButton, Qt::NoModifier, armPosition());
    QVERIFY(clickButton(QStringLiteral("caacDeleteCoordinateAxes")));
    QTRY_VERIFY_WITH_TIMEOUT(!panel->property("hasCoordinateAxes").toBool(), TestTimeout::mediumMs());

    QVERIFY(clickButton(QStringLiteral("caacEditWaypoint_%1").arg(selected)));
    QVERIFY(waitForDialog(QCoreApplication::translate("CaacWaypointDialog", "Waypoint properties")));
    QVERIFY(rejectDialog());
    QVERIFY(findVisibleItem(_rootItem, QStringLiteral("caacRemoveWaypoint_%1").arg(selected)));

    const auto beforeBatchDelete = master->saveToJson();
    const auto homeBeforeBatchDelete = mission->plannedHomePosition();
    QVERIFY(!findVisibleItem(_rootItem, QStringLiteral("caacSelectWaypoint_0"), 0));
    QVERIFY(clickButton(QStringLiteral("caacSelectAllWaypoints")));
    QCOMPARE(panel->property("selectedWaypointCount").toInt(), 2);
    QVERIFY(clickButtonScrolled(QStringLiteral("caacSelectWaypoint_1"), QStringLiteral("caacWaypointList")));
    QCOMPARE(panel->property("selectedWaypointCount").toInt(), 1);
    QVERIFY(clickButton(QStringLiteral("caacDeleteSelectedWaypoints")));
    QCOMPARE(mission->visualItems()->count(), 2);
    QCOMPARE(mission->plannedHomePosition(), homeBeforeBatchDelete);
    QVERIFY(clickButton(QStringLiteral("caacUndo")));
    QCOMPARE(master->saveToJson(), beforeBatchDelete);
    QCOMPARE(panel->property("selectedWaypointCount").toInt(), 0);
    QVERIFY(clickButton(QStringLiteral("caacSelectAllWaypoints")));
    QVERIFY(clickButton(QStringLiteral("caacDeleteSelectedWaypoints")));
    QCOMPARE(mission->visualItems()->count(), 1);
    QVERIFY(clickButton(QStringLiteral("caacUndo")));
    QCOMPARE(master->saveToJson(), beforeBatchDelete);
    QVERIFY(clickButton(QStringLiteral("caacRelativeEditorButton")));
    auto* relativeTarget = findVisibleItem(_rootItem, QStringLiteral("caacRelativeTarget"));
    auto* relativeReference = findVisibleItem(_rootItem, QStringLiteral("caacRelativeReference"));
    auto* relativeDistance = findVisibleItem(_rootItem, QStringLiteral("caacRelativeDistance"));
    QVERIFY(relativeTarget && relativeReference && relativeDistance);
    auto* relativeAngleType = findVisibleItem(_rootItem, QStringLiteral("caacRelativeAngleType"));
    auto* relativeAngle = findVisibleItem(_rootItem, QStringLiteral("caacRelativeAngle"));
    QVERIFY(relativeAngleType && relativeAngle);
    auto* relativeInsertAfter = findVisibleItem(_rootItem, QStringLiteral("caacRelativeInsertAfter"));
    QVERIFY(relativeInsertAfter);
    const auto beforeRelativeChain = master->saveToJson();
    const int beforeRelativeCount = mission->visualItems()->count();
    QVERIFY(relativeReference->setProperty("currentIndex", 0));
    QVERIFY(relativeInsertAfter->setProperty("currentIndex", 0));
    QVERIFY(relativeAngle->setProperty("text", QStringLiteral("270")));
    QVERIFY(relativeDistance->setProperty("text", QStringLiteral("90")));
    QVERIFY(clickButton(QStringLiteral("caacRelativeAdd")));
    QTRY_COMPARE_WITH_TIMEOUT(relativeReference->property("currentIndex").toInt(), 1, TestTimeout::mediumMs());
    QTRY_COMPARE_WITH_TIMEOUT(relativeInsertAfter->property("currentIndex").toInt(), 1, TestTimeout::mediumMs());
    QCOMPARE(mission->visualItems()->count(), beforeRelativeCount + 1);
    const auto firstRelative = mission->visualItems()->value<SimpleMissionItem*>(1)->coordinate();
    QVERIFY(std::abs(QGCGeo::geodesicDistance(home, firstRelative) - 90) < 0.01);
    const auto beforeInvalidRelative = master->saveToJson();
    QVERIFY(relativeDistance->setProperty("text", QStringLiteral("0")));
    QVERIFY(clickButton(QStringLiteral("caacRelativeAdd")));
    QCOMPARE(master->saveToJson(), beforeInvalidRelative);
    QCOMPARE(relativeReference->property("currentIndex").toInt(), 1);
    QCOMPARE(relativeInsertAfter->property("currentIndex").toInt(), 1);
    QVERIFY(relativeAngle->setProperty("text", QStringLiteral("0")));
    QVERIFY(relativeDistance->setProperty("text", QStringLiteral("30")));
    QVERIFY(clickButton(QStringLiteral("caacRelativeAdd")));
    QTRY_COMPARE_WITH_TIMEOUT(relativeReference->property("currentIndex").toInt(), 2, TestTimeout::mediumMs());
    QTRY_COMPARE_WITH_TIMEOUT(relativeInsertAfter->property("currentIndex").toInt(), 2, TestTimeout::mediumMs());
    QCOMPARE(mission->visualItems()->count(), beforeRelativeCount + 2);
    const auto secondRelative = mission->visualItems()->value<SimpleMissionItem*>(2)->coordinate();
    QVERIFY(std::abs(QGCGeo::geodesicDistance(firstRelative, secondRelative) - 30) < 0.01);
    QVERIFY(std::abs(std::remainder(QGCGeo::geodesicAzimuth(firstRelative, secondRelative), 360)) < 0.02);
    QVERIFY(clickButton(QStringLiteral("caacUndo")));
    QVERIFY(clickButton(QStringLiteral("caacUndo")));
    QCOMPARE(master->saveToJson(), beforeRelativeChain);
    const auto beforeEndpoint = master->saveToJson();
    for (int targetIndex : {1, 2}) {
        QVERIFY(relativeAngleType->setProperty("currentIndex", 1));
        QVERIFY(relativeTarget->setProperty("currentIndex", targetIndex));
        QVERIFY(QMetaObject::invokeMethod(relativeTarget, "activated", Q_ARG(int, targetIndex)));
        QVERIFY(relativeAngleType->isEnabled());
        QCOMPARE(relativeAngleType->property("currentIndex").toInt(), 1);
        QVERIFY(relativeReference->setProperty("currentIndex", 1));
        QVERIFY(QMetaObject::invokeMethod(relativeReference, "activated", Q_ARG(int, 1)));
        QVERIFY(relativeAngle->setProperty("text", QStringLiteral("90")));
        QVERIFY(relativeDistance->setProperty("text", QStringLiteral("25")));
        QVERIFY(clickButton(QStringLiteral("caacRelativeAdd")));
        QVERIFY(master->saveToJson() != beforeEndpoint);
        const auto endpoint =
            targetIndex == 1
                ? mission->plannedHomePosition()
                : mission->visualItems()->value<SimpleMissionItem*>(mission->visualItems()->count() - 1)->coordinate();
        const auto reference = mission->visualItems()->value<SimpleMissionItem*>(1)->coordinate();
        QVERIFY(std::abs(QGCGeo::geodesicDistance(reference, endpoint) - 25) < 0.01);
        QVERIFY(std::abs(std::remainder(QGCGeo::geodesicAzimuth(reference, endpoint), 360)) < 0.02);
        QVERIFY(clickButton(QStringLiteral("caacUndo")));
        QCOMPARE(master->saveToJson(), beforeEndpoint);
    }
    QVERIFY(clickButton(QStringLiteral("caacTemplateEditorButton")));
    auto* bearing =
        findVisibleItemScrolled(QStringLiteral("caacTemplateBearing"), QStringLiteral("caacTemplateScroller"));
    QVERIFY(bearing);
    const auto original = master->saveToJson();
    const auto enterHeading = [&](const QString& value) {
        bearing->forceActiveFocus();
        QTest::keyClick(_window, Qt::Key_A, Qt::ControlModifier);
        for (const auto character : value) {
            QTest::keyClick(_window, character.toLatin1());
        }
    };
    enterHeading(QStringLiteral("56"));
    QTRY_VERIFY_WITH_TIMEOUT(editor->rotationPreviewActive(), TestTimeout::mediumMs());
    const auto preview = master->saveToJson();
    QVERIFY(preview != original);
    enterHeading(QStringLiteral("90"));
    enterHeading(QStringLiteral("56"));
    QCOMPARE(master->saveToJson(), preview);
    QVERIFY(clickButtonScrolled(QStringLiteral("caacCancelOrientation"), QStringLiteral("caacTemplateScroller")));
    QCOMPARE(master->saveToJson(), original);
    bearing = findVisibleItemScrolled(QStringLiteral("caacTemplateBearing"), QStringLiteral("caacTemplateScroller"));
    QVERIFY(bearing);
    enterHeading(QStringLiteral("90"));
    QVERIFY(clickButtonScrolled(QStringLiteral("caacConfirmOrientation"), QStringLiteral("caacTemplateScroller")));
    QCOMPARE(editor->routeOrientation(), 90.0);
    QVERIFY(!editor->rotationPreviewActive());
    QVERIFY(clickButton(QStringLiteral("caacUndo")));
    QCOMPARE(master->saveToJson(), original);
    QVERIFY(clickButton(QStringLiteral("caacGlobalSettingsButton")));
    for (const auto* name : {"caacHomeSection", "caacBasicSection", "caacSimTurnSection", "caacSimClimbSection",
                             "caacSimLoopsSection", "caacMapEditingSection"}) {
        QVERIFY(findItem(_rootItem, QLatin1String(name)));
    }
    QVERIFY(clickButtonScrolled(QStringLiteral("caacHomeAltitude"), QStringLiteral("caacGlobalSettingsScroller")));
    QTest::keyClick(_window, Qt::Key_A, Qt::ControlModifier);
    QTest::keyClick(_window, Qt::Key_2);
    QTest::keyClick(_window, Qt::Key_0);
    QTest::keyClick(_window, Qt::Key_Tab);
    const auto beforeHomeCoordinates = master->saveToJson();
    for (const auto& entry :
         {std::pair<QString, QString>{QStringLiteral("caacHomeLongitude"), QStringLiteral("8.54525860115325")},
          {QStringLiteral("caacHomeLatitude"), QStringLiteral("47.3982382440215")}}) {
        auto* decimal = findVisibleItemScrolled(entry.first, QStringLiteral("caacGlobalSettingsScroller"));
        QVERIFY(decimal);
        QVERIFY(decimal->setProperty("text", entry.second));
        QVERIFY(clickButtonScrolled(entry.first + QStringLiteral("FormatToggle"),
                                    QStringLiteral("caacGlobalSettingsScroller")));
        auto* direction = findVisibleItem(_rootItem, entry.first + QStringLiteral("Hemisphere"));
        QVERIFY(direction);
        QVERIFY(direction->setProperty("currentIndex", 1));
        QVERIFY(clickButtonScrolled(entry.first + QStringLiteral("FormatToggle"),
                                    QStringLiteral("caacGlobalSettingsScroller")));
        decimal = findVisibleItem(_rootItem, entry.first);
        QVERIFY(decimal);
        QVERIFY(std::abs(decimal->property("text").toString().toDouble() + entry.second.toDouble()) < 1e-10);
        QVERIFY(clickButtonScrolled(entry.first + QStringLiteral("FormatToggle"),
                                    QStringLiteral("caacGlobalSettingsScroller")));
    }
    QVERIFY(clickButtonScrolled(QStringLiteral("caacApplyHome"), QStringLiteral("caacGlobalSettingsScroller")));
    QVERIFY(std::abs(mission->plannedHomePosition().longitude() + 8.54525860115325) < 1e-10);
    QVERIFY(std::abs(mission->plannedHomePosition().latitude() + 47.3982382440215) < 1e-10);
    QCOMPARE(mission->plannedHomePosition().altitude(), 20.0);
    const auto validDmsHome = master->saveToJson();
    auto* minutes = findVisibleItem(_rootItem, QStringLiteral("caacHomeLatitudeMinutes"));
    QVERIFY(minutes);
    QVERIFY(minutes->setProperty("text", QStringLiteral("60")));
    QVERIFY(clickButtonScrolled(QStringLiteral("caacApplyHome"), QStringLiteral("caacGlobalSettingsScroller")));
    QCOMPARE(master->saveToJson(), validDmsHome);
    QVERIFY(clickButton(QStringLiteral("caacUndo")));
    QCOMPARE(master->saveToJson(), beforeHomeCoordinates);
    QVERIFY(!findItem(_rootItem, QStringLiteral("caacSimFlightSection")));
    QVERIFY(!findItem(_rootItem, QStringLiteral("caacSimActionsSection")));
    QVERIFY(!findItem(_rootItem, QStringLiteral("caacOpenSimulation")));
    auto* horizontal =
        findVisibleItemScrolled(QStringLiteral("caacSimHorizontalSpeed"), QStringLiteral("caacGlobalSettingsScroller"));
    QVERIFY(horizontal);
    auto* basicSection = findItem(_rootItem, QStringLiteral("caacBasicSection"));
    auto* basicAncestor = horizontal->parentItem();
    while (basicAncestor && basicAncestor != basicSection) {
        basicAncestor = basicAncestor->parentItem();
    }
    QCOMPARE(basicAncestor, basicSection);
    auto* takeoffHeight =
        findVisibleItemScrolled(QStringLiteral("caacSimTakeoffHeight"), QStringLiteral("caacGlobalSettingsScroller"));
    QVERIFY(takeoffHeight);
    QVERIFY(takeoffHeight->setProperty("text", QStringLiteral("15")));
    horizontal->setProperty("text", QStringLiteral("7.5"));
    QVERIFY(clickButtonScrolled(QStringLiteral("caacSaveSimulationSettings"),
                                QStringLiteral("caacGlobalSettingsScroller")));
    QCOMPARE(editor->simulationSettings().value("horizontalSpeed").toDouble(), 7.5);
    QCOMPARE(editor->simulationSettings().value("takeoffHeight").toDouble(), 15.0);
    QVERIFY(!findItem(_rootItem, QStringLiteral("caacSimWindSection")));
    QVERIFY(!findItem(_rootItem, QStringLiteral("caacSimWindSpeed")));
    auto* saveSettings = findVisibleItem(_rootItem, QStringLiteral("caacSaveSimulationSettings"));
    QVERIFY(saveSettings);
    auto* ancestor = saveSettings->parentItem();
    while (ancestor && ancestor != basicSection) {
        ancestor = ancestor->parentItem();
    }
    QCOMPARE(ancestor, basicSection);

    QVERIFY(
        clickButtonScrolled(QStringLiteral("caacGlobalShowDrawingAids"), QStringLiteral("caacGlobalSettingsScroller")));
    QVERIFY(!panel->property("showDrawingAids").toBool());
    QVERIFY(
        clickButtonScrolled(QStringLiteral("caacGlobalShowDrawingAids"), QStringLiteral("caacGlobalSettingsScroller")));
    QVERIFY(panel->property("showDrawingAids").toBool());
    if (const auto screenshot = qgetenv("CAAC_GLOBAL_UI_SCREENSHOT"); !screenshot.isEmpty()) {
        QVERIFY(scrollIntoView(horizontal, QStringLiteral("caacGlobalSettingsScroller")));
        QVERIFY(_window->grabWindow().save(QString::fromUtf8(screenshot)));
    }
    QVERIFY(clickToolSelectDropdownButton(QStringLiteral("toolbar_viewSettings")));
    QVERIFY(findVisibleItem(_rootItem, QStringLiteral("settingsButton_General")));
    for (const auto* name : {"settingsButton_Help", "settingsButton_PX4 Log Transfer", "settingsButton_Remote ID"}) {
        QVERIFY(!findItem(_rootItem, QLatin1String(name)));
    }
    QCOMPARE(SettingsManager::instance()->appSettings()->preferredVehicleClass()->label(), QStringLiteral("首选机型"));
    QCOMPARE(QCoreApplication::translate("Maps.SettingsUI.json", "Tokens"), QStringLiteral("访问令牌"));
}

UT_REGISTER_TEST(CaacPlanningInteractionUITest, TestLabel::Integration, TestLabel::MissionManager)

#include "CaacPlanningInteractionUITest.moc"
