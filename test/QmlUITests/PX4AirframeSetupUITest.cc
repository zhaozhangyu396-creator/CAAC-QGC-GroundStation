#include "PX4AirframeSetupUITest.h"

#include <algorithm>

#include <QtQml/QJSValue>
#include <QtQuick/QQuickItem>
#include <QtQuick/QQuickWindow>
#include <QtTest/QSignalSpy>
#include <QtTest/QTest>

#include "Fact.h"
#include "MockLink.h"
#include "MultiVehicleManager.h"
#include "ParameterManager.h"
#include "QGCCorePlugin.h"
#include "Vehicle.h"

UT_REGISTER_TEST(PX4AirframeSetupUITest, TestLabel::Integration)

void PX4AirframeSetupUITest::_navigateToAirframePanel()
{
    navigateToConfigureView();
    if (QTest::currentTestFailed()) return;

    QQuickItem *airframeBtn = findVisibleItem(_rootItem, QStringLiteral("vehicleConfig_comp_Airframe"), 2000);
    QVERIFY2(airframeBtn, "vehicleConfig_comp_Airframe button not found");

    // SYS_AUTOSTART was reset to 0 so AirframeComponent::setupComplete is false:
    // the button must show the config-required indicator
    (void) QTest::qWaitFor([&] { return !airframeBtn->property("setupComplete").toBool(); }, 3000);
    QVERIFY2(!airframeBtn->property("setupComplete").toBool(),
             "Airframe button does not show config required after SYS_AUTOSTART reset");

    clickSidebarButton(QStringLiteral("vehicleConfig_comp_Airframe"));
    if (QTest::currentTestFailed()) return;

    QVERIFY2(QTest::qWaitFor([&] { return airframeBtn->property("checked").toBool(); }, 3000),
             "Airframe button not selected after clicking it");

    QVERIFY2(findVisibleItem(_rootItem, QStringLiteral("airframeCategory_multirotor"), 3000),
             "Airframe category menu not shown after opening Airframe page");
    QVERIFY2(!findVisibleItem(_rootItem, QStringLiteral("airframeSetup_applyButton"), 0),
             "Apply and Restart shown before choosing an airframe category");
    QVERIFY2(
        clickButtonScrolled(QStringLiteral("airframeCategory_multirotor"), QStringLiteral("vehicleSetup_scroller")),
        "Failed to open multirotor airframes");

    QQuickItem* applyButton = findVisibleItem(_rootItem, QStringLiteral("airframeSetup_applyButton"), 3000);
    QVERIFY2(applyButton, "Apply and Restart button not found after opening Airframe page");

    // No airframe has been selected yet so Apply and Restart must not show as
    // the primary action
    QVERIFY2(!applyButton->property("primary").toBool(),
             "Apply and Restart button shows as primary before an airframe is selected");

    // The standard airframes show first so users don't have to scroll past the
    // exotic frames to find a regular quad
    QQuickItem *firstTypeBox = findVisibleItem(_rootItem, QStringLiteral("airframeTypeBox_0"), 3000);
    QVERIFY2(firstTypeBox, "First airframe type box not found");
    QCOMPARE(firstTypeBox->property("airframeTypeName").toString(), QStringLiteral("Quadrotor x"));

    // No airframe is configured (SYS_AUTOSTART is 0) so no airframe type box
    // may show as selected
    for (int i = 0; ; i++) {
        QQuickItem *typeBox = findVisibleItem(_rootItem, QStringLiteral("airframeTypeBox_%1").arg(i), 0);
        if (!typeBox) {
            QVERIFY2(i > 0, "No airframe type boxes found");
            break;
        }
        QVERIFY2(!typeBox->property("airframeTypeSelected").toBool(),
                 qPrintable(QStringLiteral("Airframe type box selected with no airframe configured: %1")
                                .arg(typeBox->property("airframeTypeName").toString())));
    }

    // Airframe is the first setup step and has no prerequisite, so the
    // prerequisite-setup message panel must not be shown
    QVERIFY2(!findVisibleItem(_rootItem, QStringLiteral("vehicleConfig_messagePanel"), 0),
             "Prerequisite setup message panel shown instead of Airframe page");
}

void PX4AirframeSetupUITest::_verifyAirframePrereq(const QString &compObjectName, bool expectPrereqShown, const QString &checkedSectionObjectName)
{
    QQuickItem *compBtn = clickSidebarButton(compObjectName);
    if (QTest::currentTestFailed()) return;

    QQuickItem *messagePanel = findVisibleItem(_rootItem, QStringLiteral("vehicleConfig_messagePanel"), expectPrereqShown ? 3000 : 0);
    if (expectPrereqShown) {
        QVERIFY2(messagePanel,
                 qPrintable(QStringLiteral("Airframe prerequisite message panel not shown for: %1").arg(compObjectName)));
    } else {
        QVERIFY2(!messagePanel,
                 qPrintable(QStringLiteral("Prerequisite message panel unexpectedly shown for: %1").arg(compObjectName)));
    }

    // Components with children must expand on click even when the prerequisite
    // is not met
    if (compBtn->property("expandable").toBool()) {
        QVERIFY2(QTest::qWaitFor([&] { return compBtn->property("expanded").toBool(); }, 3000),
                 qPrintable(QStringLiteral("Component tree did not expand after click: %1").arg(compObjectName)));
    }

    // Components which auto-select their first section on root click must do so
    // even when the prerequisite is not met
    if (!checkedSectionObjectName.isEmpty()) {
        const auto expectedSection = QGCCorePlugin::instance()->directMissionItemEditing() &&
                                             checkedSectionObjectName == QStringLiteral("vehicleConfig_section_Compass")
                                         ? QStringLiteral("vehicleConfig_section_Orientations")
                                         : checkedSectionObjectName;
        QQuickItem* sectionBtn = findItem(_rootItem, expectedSection);
        QVERIFY2(sectionBtn,
                 qPrintable(QStringLiteral("Section button not found: %1").arg(checkedSectionObjectName)));
        QVERIFY2(QTest::qWaitFor([&] { return sectionBtn->property("sectionChecked").toBool(); }, 3000),
                 qPrintable(QStringLiteral("First child section not selected after clicking %1: %2").arg(compObjectName, checkedSectionObjectName)));
    }
}

void PX4AirframeSetupUITest::_testNavigateToAirframe()
{
    runWithMockLink(
        [] { return MockLink::startPX4MockLink(); },
        [&](QPointer<MockLink> mockLink, Vehicle* vehicle) {
            // Reset all params including SYS_AUTOSTART so the test starts with no
            // airframe configured
            mockLink->setResetSysAutostartOnParamReset(true);
            resetParamsToFirmwareDefaults(vehicle, QStringLiteral("SYS_AUTOSTART"));
            if (QTest::currentTestFailed())
                return;

            _navigateToAirframePanel();
            if (QTest::currentTestFailed())
                return;

            QQuickItem* page = findVisibleItem(_rootItem, QStringLiteral("airframeSetup_root"), 3000);
            QVERIFY(page);
            Fact* autostart = vehicle->parameterManager()->getParameter(-1, QStringLiteral("SYS_AUTOSTART"));
            Fact* autoconfig = vehicle->parameterManager()->getParameter(-1, QStringLiteral("SYS_AUTOCONFIG"));
            QVERIFY(autostart);
            QVERIFY(autoconfig);
            QSignalSpy autostartWrites(autostart, &Fact::containerRawValueChanged);
            QSignalSpy autoconfigWrites(autoconfig, &Fact::containerRawValueChanged);
            QVERIFY(autostartWrites.isValid());
            QVERIFY(autoconfigWrites.isValid());
            const QVariant originalAutostart = autostart->rawValue();
            const QVariant originalAutoconfig = autoconfig->rawValue();
            const QStringList categories = {
                QStringLiteral("fixedwing"), QStringLiteral("vtol"),  QStringLiteral("rover"),
                QStringLiteral("airship"),   QStringLiteral("other"),
            };
            for (const auto& category : categories) {
                QVERIFY(clickButtonScrolled(QStringLiteral("airframeCategory_back"),
                                            QStringLiteral("vehicleSetup_scroller")));
                QVERIFY(clickButtonScrolled(QStringLiteral("airframeCategory_%1").arg(category),
                                            QStringLiteral("vehicleSetup_scroller")));
                QTRY_COMPARE_WITH_TIMEOUT(page->property("categoryFilter").toString(), category, 3000);
                QCOMPARE(page->property("pendingAutostartId").toInt(), originalAutostart.toInt());
            }
            QCOMPARE(autostart->rawValue(), originalAutostart);
            QCOMPARE(autoconfig->rawValue(), originalAutoconfig);
            QCOMPARE(autostartWrites.count(), 0);
            QCOMPARE(autoconfigWrites.count(), 0);

            // Simulation contains different vehicle classes; its picture must follow the filtered model and selection.
            struct SimulationCategory
            {
                QString category;
                QList<int> ids;
                QStringList images;
            };
            const SimulationCategory simulationCategories[] = {
                {QStringLiteral("multirotor"),
                 {1001, 1100},
                 {QStringLiteral("quadrotor-x"), QStringLiteral("quadrotor-x")}},
                {QStringLiteral("fixedwing"), {1101}, {QStringLiteral("standard-plane")}},
                {QStringLiteral("vtol"),
                 {1102, 1103},
                 {QStringLiteral("vtol-tailsitter"), QStringLiteral("standard-vtol")}},
            };
            for (const auto& simulation : simulationCategories) {
                QVERIFY(clickButtonScrolled(QStringLiteral("airframeCategory_back"),
                                            QStringLiteral("vehicleSetup_scroller")));
                QVERIFY(clickButtonScrolled(QStringLiteral("airframeCategory_%1").arg(simulation.category),
                                            QStringLiteral("vehicleSetup_scroller")));
                QQuickItem* combo = findVisibleItemScrolled(QStringLiteral("SimulationComboBox"),
                                                            QStringLiteral("vehicleSetup_scroller"));
                QVERIFY(combo);
                const QVariant modelValue = combo->property("model");
                const QVariantList model = modelValue.metaType() == QMetaType::fromType<QJSValue>()
                                               ? modelValue.value<QJSValue>().toVariant().toList()
                                               : modelValue.toList();
                QCOMPARE(model.count(), simulation.ids.count());
                QQuickItem* image = findItem(_rootItem, QStringLiteral("airframeImage_Simulation"));
                QVERIFY(image);
                for (int index = 0; index < model.count(); ++index) {
                    const QObject* airframe = model.at(index).value<QObject*>();
                    QVERIFY(airframe);
                    QCOMPARE(airframe->property("category").toString(), simulation.category);
                    QCOMPARE(airframe->property("autostartId").toInt(), simulation.ids.at(index));
                    combo->forceActiveFocus();
                    QTest::keyClick(_window, index == 0 ? Qt::Key_Home : Qt::Key_End);
                    QTRY_COMPARE_WITH_TIMEOUT(combo->property("currentIndex").toInt(), index, TestTimeout::shortMs());
                    const QUrl expectedImage(
                        QStringLiteral("qrc:/qmlimages/Airframe/Uploaded/%1.svg").arg(simulation.images.at(index)));
                    QTRY_COMPARE_WITH_TIMEOUT(image->property("source").toUrl(), expectedImage, TestTimeout::shortMs());
                }
            }
            QCOMPARE(autostartWrites.count(), 0);
            QCOMPARE(autoconfigWrites.count(), 0);
        });
}

void PX4AirframeSetupUITest::_testAirframePrereqPages()
{
    runWithMockLink(
        [] { return MockLink::startPX4MockLink(); },
        [&](QPointer<MockLink> mockLink, Vehicle *vehicle) {
    // Reset all params including SYS_AUTOSTART so Airframe setup is incomplete
    mockLink->setResetSysAutostartOnParamReset(true);
    resetParamsToFirmwareDefaults(vehicle, QStringLiteral("SYS_AUTOSTART"));
    if (QTest::currentTestFailed()) return;

    navigateToConfigureView();
    if (QTest::currentTestFailed()) return;

    // With Airframe setup incomplete, PX4AutoPilotPlugin::prerequisiteSetup()
    // requires Airframe first for Sensors, Power, Safety, PID Tuning and
    // Flight Modes (COM_RC_IN_MODE defaults to 0), so clicking those pages
    // shows the prerequisite message panel instead of the page. Expandable
    // components must still expand, and Sensors must still auto-select its
    // first section (Compass).
    struct PrereqCheck {
        const char *objectName;
        bool expectPrereqShown;
        const char *checkedSectionObjectName;
    };
    const PrereqCheck checks[] = {
        { .objectName = "vehicleConfig_comp_Sensors",     .expectPrereqShown = true,  .checkedSectionObjectName = "vehicleConfig_section_Compass" },
        { .objectName = "vehicleConfig_comp_Power",       .expectPrereqShown = true,  .checkedSectionObjectName = "" },
        { .objectName = "vehicleConfig_comp_PIDTuning",   .expectPrereqShown = true,  .checkedSectionObjectName = "" },
        { .objectName = "vehicleConfig_comp_FlightModes", .expectPrereqShown = true,  .checkedSectionObjectName = "" },
        { .objectName = "vehicleConfig_comp_Radio",       .expectPrereqShown = false, .checkedSectionObjectName = "" },
        { .objectName = "vehicleConfig_comp_Joystick",    .expectPrereqShown = false, .checkedSectionObjectName = "" },
        { .objectName = "vehicleConfig_comp_Airframe",    .expectPrereqShown = false, .checkedSectionObjectName = "" },
    };
    for (const PrereqCheck &check : checks) {
        _verifyAirframePrereq(QLatin1String(check.objectName), check.expectPrereqShown, QLatin1String(check.checkedSectionObjectName));
        if (QTest::currentTestFailed()) return;
    }
    if (QGCCorePlugin::instance()->directMissionItemEditing()) {
        QVERIFY(!findVisibleItem(_rootItem, QStringLiteral("vehicleConfig_comp_Safety"), 0));
        QVERIFY(
            clickButtonScrolled(QStringLiteral("toolbar_safety"), QStringLiteral("caacSettingsNavigationScroller")));
        QVERIFY(findVisibleItem(_rootItem, QStringLiteral("caacSafetySettings")));
        QVERIFY(!findVisibleItem(_rootItem, QStringLiteral("vehicleConfig_messagePanel"), 0));
    } else {
        _verifyAirframePrereq(QStringLiteral("vehicleConfig_comp_Safety"), true, QString());
    }
    });
}

void PX4AirframeSetupUITest::_testApplyAirframe()
{
    runWithMockLink(
        [] { return MockLink::startPX4MockLink(); },
        [&](QPointer<MockLink> mockLink, Vehicle *vehicle) {
    // Reset all params including SYS_AUTOSTART so the test starts with no
    // airframe configured
    mockLink->setResetSysAutostartOnParamReset(true);
    resetParamsToFirmwareDefaults(vehicle, QStringLiteral("SYS_AUTOSTART"));
    if (QTest::currentTestFailed()) return;

    _navigateToAirframePanel();
    if (QTest::currentTestFailed()) return;

    Fact* autostart = vehicle->parameterManager()->getParameter(-1, QStringLiteral("SYS_AUTOSTART"));
    Fact* autoconfig = vehicle->parameterManager()->getParameter(-1, QStringLiteral("SYS_AUTOCONFIG"));
    QVERIFY(autostart);
    QVERIFY(autoconfig);
    QSignalSpy autostartWrites(autostart, &Fact::containerRawValueChanged);
    QSignalSpy autoconfigWrites(autoconfig, &Fact::containerRawValueChanged);
    QVERIFY(autostartWrites.isValid());
    QVERIFY(autoconfigWrites.isValid());

    // The expected autostart id comes from the first entry of the Quadrotor x
    // combo model so the test doesn't hardcode airframe metadata values
    QQuickItem *combo = findVisibleItem(_rootItem, QStringLiteral("Quadrotor xComboBox"), 3000);
    QVERIFY2(combo, "Quadrotor x combo box not found");
    const QVariant modelValue = combo->property("model");
    const QVariantList comboModel = modelValue.metaType() == QMetaType::fromType<QJSValue>()
                                        ? modelValue.value<QJSValue>().toVariant().toList()
                                        : modelValue.toList();
    QVERIFY2(!comboModel.isEmpty(), "Quadrotor x combo model is empty");
    QObject *firstAirframe = comboModel.first().value<QObject*>();
    QVERIFY2(firstAirframe, "First Quadrotor x airframe entry is not a QObject");
    const int expectedAutostartId = firstAirframe->property("autostartId").toInt();
    QVERIFY2(expectedAutostartId != 0, "First Quadrotor x airframe has no autostart id");

    // Select the Quadrotor x airframe type box
    QQuickItem* typeBox =
        findVisibleItemScrolled(QStringLiteral("airframeTypeBox_0"), QStringLiteral("vehicleSetup_scroller"));
    QVERIFY2(typeBox, "airframeTypeBox_0 not found");
    const QPointF boxCenter = typeBox->mapToScene(QPointF(typeBox->width() / 2, typeBox->height() / 2));
    QTest::mouseClick(_window, Qt::LeftButton, Qt::NoModifier, boxCenter.toPoint());
    QTest::qWait(_pageDelay);

    // The clicked airframe type box must now show as selected
    QVERIFY2(QTest::qWaitFor([&] { return typeBox->property("airframeTypeSelected").toBool(); }, 3000),
             "Quadrotor x type box not selected after clicking it");

    QQuickItem* page = findVisibleItem(_rootItem, QStringLiteral("airframeSetup_root"), 3000);
    QVERIFY(page);
    QCOMPARE(page->property("pendingAutostartId").toInt(), expectedAutostartId);
    QVERIFY(clickButtonScrolled(QStringLiteral("airframeCategory_back"), QStringLiteral("vehicleSetup_scroller")));
    QVERIFY(clickButtonScrolled(QStringLiteral("airframeCategory_fixedwing"), QStringLiteral("vehicleSetup_scroller")));
    QCOMPARE(page->property("pendingAutostartId").toInt(), expectedAutostartId);
    QVERIFY(clickButtonScrolled(QStringLiteral("airframeCategory_back"), QStringLiteral("vehicleSetup_scroller")));
    QVERIFY(
        clickButtonScrolled(QStringLiteral("airframeCategory_multirotor"), QStringLiteral("vehicleSetup_scroller")));
    typeBox = findVisibleItem(_rootItem, QStringLiteral("airframeTypeBox_0"), 3000);
    QVERIFY(typeBox);
    QTRY_VERIFY_WITH_TIMEOUT(typeBox->property("airframeTypeSelected").toBool(), 3000);
    QCOMPARE(autostartWrites.count(), 0);
    QCOMPARE(autoconfigWrites.count(), 0);

    // Selecting an airframe makes Apply and Restart the primary action
    QQuickItem *applyButton = findVisibleItem(_rootItem, QStringLiteral("airframeSetup_applyButton"), 3000);
    QVERIFY2(applyButton, "Apply and Restart button not found");
    QVERIFY2(QTest::qWaitFor([&] { return applyButton->property("primary").toBool(); }, 3000),
             "Apply and Restart button not primary after selecting an airframe");

    QVERIFY(clickButtonScrolled(QStringLiteral("airframeSetup_applyButton"), QStringLiteral("vehicleSetup_scroller")));
    QVERIFY(rejectDialog());
    QCOMPARE(autostartWrites.count(), 0);
    QCOMPARE(autoconfigWrites.count(), 0);

    // Apply and confirm the restart dialog. The reboot command, its ack and the
    // link disconnect all happen within a single timer callback, so a signal spy
    // is required to observe the command result: polling MockLink would only see
    // the state after the link is already gone.
    QSignalSpy spyCmdResult(vehicle, &Vehicle::mavCommandResult);
    QVERIFY2(spyCmdResult.isValid(), "Failed to create mavCommandResult spy");

    QVERIFY2(clickButtonScrolled(QStringLiteral("airframeSetup_applyButton"), QStringLiteral("vehicleSetup_scroller")),
             "Failed to click Apply and Restart");
    QVERIFY2(findVisibleItem(_rootItem, QStringLiteral("popupDialog_acceptButton"), 3000),
             "Apply confirmation dialog not shown");
    QVERIFY2(clickButton(QStringLiteral("popupDialog_acceptButton")), "Failed to confirm Apply dialog");

    // Both params must arrive at the simulated firmware via PARAM_SET
    QVERIFY2(QTest::qWaitFor([&] {
                 return mockLink && mockLink->paramValue(MAV_COMP_ID_AUTOPILOT1, QStringLiteral("SYS_AUTOSTART")).toInt() == expectedAutostartId;
             }, 10000),
             "SYS_AUTOSTART never reached the expected autostart id on MockLink");
    QVERIFY2(QTest::qWaitFor([&] {
                 return mockLink && mockLink->paramValue(MAV_COMP_ID_AUTOPILOT1, QStringLiteral("SYS_AUTOCONFIG")).toInt() == 1;
             }, 10000),
             "SYS_AUTOCONFIG never set to 1 on MockLink");

    // The controller then reboots the vehicle: MockLink must ack
    // MAV_CMD_PREFLIGHT_REBOOT_SHUTDOWN with MAV_RESULT_ACCEPTED
    const auto rebootAccepted = [&] {
        for (const QList<QVariant> &args : spyCmdResult) {
            if ((args.at(2).toInt() == MAV_CMD_PREFLIGHT_REBOOT_SHUTDOWN) && (args.at(3).toInt() == MAV_RESULT_ACCEPTED)) {
                return true;
            }
        }
        return false;
    };
    QVERIFY2(QTest::qWaitFor(rebootAccepted, 10000),
             "MAV_CMD_PREFLIGHT_REBOOT_SHUTDOWN never accepted by MockLink");

    // The restart flow disconnects the link, dropping the active vehicle
    QVERIFY2(QTest::qWaitFor([&] { return MultiVehicleManager::instance()->activeVehicle() == nullptr; }, 10000),
             "Vehicle never disconnected after Apply and Restart");

    QVERIFY(!findVisibleItem(_rootItem, QStringLiteral("popupDialog_acceptButton"), 0));
    QCOMPARE(std::count_if(
                 spyCmdResult.cbegin(), spyCmdResult.cend(),
                 [](const QList<QVariant>& args) { return args.at(2).toInt() == MAV_CMD_PREFLIGHT_REBOOT_SHUTDOWN; }),
             1);
    });
}
