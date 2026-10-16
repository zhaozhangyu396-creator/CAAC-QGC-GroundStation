#include "PX4VehicleConfigUITest.h"

#include <QtCore/QPointer>
#include <QtCore/QScopeGuard>
#include <QtGui/QScreen>
#include <QtQuick/QQuickItem>
#include <QtQuick/QQuickWindow>
#include <QtTest/QTest>

#include "MockLink.h"
#include "QGCCorePlugin.h"
#include "Vehicle.h"

UT_REGISTER_TEST(PX4VehicleConfigUITest, TestLabel::Integration)

void PX4VehicleConfigUITest::_testNavigateVehicleConfig()
{
    runWithMockLink(
        [] { return MockLink::startPX4MockLink(); },
        [&](QPointer<MockLink> /*mockLink*/, Vehicle *vehicle) {
    navigateToConfigureView();
    if (QTest::currentTestFailed()) return;

    // -------------------------------------------------------------------------
    // Click Summary
    // -------------------------------------------------------------------------
    QVERIFY2(clickButton(QStringLiteral("vehicleConfig_summary")), "Failed to click Summary button");
    QTest::qWait(_viewDelay);

    // -------------------------------------------------------------------------
    // Click through each vehicle component, in English and Chinese
    // -------------------------------------------------------------------------
    clickThroughAllComponentsAllLocales(vehicle);

    });
}

// Embedded directories use bordered rows; standalone dividers are hidden while searching.
void PX4VehicleConfigUITest::_testSidebarDividers()
{
    runWithMockLink(
        [] { return MockLink::startPX4MockLink(); },
        [&](QPointer<MockLink> /*mockLink*/, Vehicle * /*vehicle*/) {
    navigateToConfigureView();
    if (QTest::currentTestFailed()) return;

    QQuickItem *const summaryDivider = findItem(_rootItem, QStringLiteral("vehicleConfig_summaryDivider"));
    QQuickItem *const componentsDivider = findItem(_rootItem, QStringLiteral("vehicleConfig_componentsDivider"));
    QQuickItem* const config = findVisibleItem(_rootItem, QStringLiteral("vehicleConfig_root"));
    QVERIFY(summaryDivider);
    QVERIFY(componentsDivider);
    QVERIFY(config);
    const bool showDividers = !config->property("embedded").toBool();
    QTRY_COMPARE_WITH_TIMEOUT(summaryDivider->isVisible(), showDividers, TestTimeout::shortMs());
    QTRY_COMPARE_WITH_TIMEOUT(componentsDivider->isVisible(), showDividers, TestTimeout::shortMs());

    QQuickItem *const searchField = findVisibleItem(_rootItem, QStringLiteral("vehicleConfig_searchField"), 3000);
    QVERIFY(searchField);
    searchField->setProperty("text", QStringLiteral("sensors"));
    QTRY_VERIFY(!summaryDivider->isVisible());
    QTRY_VERIFY(!componentsDivider->isVisible());

    searchField->setProperty("text", QString());
    QTRY_COMPARE_WITH_TIMEOUT(summaryDivider->isVisible(), showDividers, TestTimeout::shortMs());
    QTRY_COMPARE_WITH_TIMEOUT(componentsDivider->isVisible(), showDividers, TestTimeout::shortMs());

    });
}

void PX4VehicleConfigUITest::_selectTuningPage(const QString& tabObjectName, int pageIndex)
{
    const QPointer<QQuickItem> menu = findVisibleItem(_rootItem, QStringLiteral("pidTuning_sectionMenu"), 0);
    if (menu) {
        QVERIFY(scrollIntoView(menu, QStringLiteral("vehicleSetup_scroller")));
        QVERIFY(clickButton(QStringLiteral("pidTuning_sectionMenu")));
        QTest::keyClick(_window, Qt::Key_Home);
        for (int index = 0; index < pageIndex; ++index) {
            QTest::keyClick(_window, Qt::Key_Down);
        }
        QTest::keyClick(_window, Qt::Key_Return);
        const auto selectedPageIndex = [this]() {
            const auto* currentMenu = findVisibleItem(_rootItem, QStringLiteral("pidTuning_sectionMenu"), 0);
            return currentMenu ? currentMenu->property("currentIndex").toInt() : -1;
        };
        QTRY_COMPARE_WITH_TIMEOUT(selectedPageIndex(), pageIndex, TestTimeout::shortMs());
    } else {
        QVERIFY(clickButton(tabObjectName));
    }
}

// Cycle through the axis buttons on a PID tuning tab. Each click exercises
// the LineSeries remove/re-add path on GraphsView; qWait lets the polish pass run.
void PX4VehicleConfigUITest::_cycleAxisButtons(const QStringList &axisNames)
{
    for (const QString &name : axisNames) {
        const QString objectName = QStringLiteral("pidTuning_axisButton_") + name;
        QVERIFY2(findVisibleItem(_rootItem, objectName, 2000),
                 qPrintable(QStringLiteral("Axis button not found: %1").arg(objectName)));
        QVERIFY2(clickButtonScrolled(objectName, QStringLiteral("vehicleSetup_scroller")),
                 qPrintable(QStringLiteral("Failed to click axis button: %1").arg(objectName)));
        QTest::qWait(_pageDelay > 0 ? _pageDelay : 200); // let the polish pass run
    }
}

void PX4VehicleConfigUITest::_testDisconnectWithPIDTuningOpen()
{
    runWithMockLink(
        [] { return MockLink::startPX4MockLink(); },
        [&](QPointer<MockLink> /*mockLink*/, Vehicle * /*vehicle*/) {
    // -------------------------------------------------------------------------
    // Navigate to Configure view and open the PID Tuning sidebar page
    // -------------------------------------------------------------------------
    navigateToConfigureView();
    if (QTest::currentTestFailed()) return;

    // PX4TuningComponent::name() returns "PID Tuning" -> objectName "vehicleConfig_comp_PIDTuning"
    clickSidebarButton(QStringLiteral("vehicleConfig_comp_PIDTuning"));
    if (QTest::currentTestFailed()) return;

    QVERIFY2(findVisibleItem(_rootItem, QStringLiteral("vehicleConfig_panelLoader"), 2000),
             "vehicleConfig_panelLoader not found after opening PID Tuning");

    // -------------------------------------------------------------------------
    // Rate Controller: Roll -> Pitch -> Yaw -> Roll
    // -------------------------------------------------------------------------
    _selectTuningPage(QStringLiteral("pidTuning_tab_RateController"), 0);
    if (QTest::currentTestFailed())
        return;
    QTest::qWait(_pageDelay);
    _cycleAxisButtons({QStringLiteral("Roll"), QStringLiteral("Pitch"), QStringLiteral("Yaw"), QStringLiteral("Roll")});
    if (QTest::currentTestFailed()) return;

    // -------------------------------------------------------------------------
    // Attitude Controller: Roll -> Pitch -> Yaw -> Roll
    // -------------------------------------------------------------------------
    _selectTuningPage(QStringLiteral("pidTuning_tab_AttitudeController"), 1);
    if (QTest::currentTestFailed())
        return;
    QTest::qWait(_pageDelay);
    _cycleAxisButtons({QStringLiteral("Roll"), QStringLiteral("Pitch"), QStringLiteral("Yaw"), QStringLiteral("Roll")});
    if (QTest::currentTestFailed()) return;

    // -------------------------------------------------------------------------
    // Velocity Controller: Horizontal -> Vertical -> Horizontal
    // -------------------------------------------------------------------------
    _selectTuningPage(QStringLiteral("pidTuning_tab_VelocityController"), 2);
    if (QTest::currentTestFailed())
        return;
    QTest::qWait(_pageDelay);
    _cycleAxisButtons({QStringLiteral("Horizontal"), QStringLiteral("Vertical"), QStringLiteral("Horizontal")});
    if (QTest::currentTestFailed()) return;

    // -------------------------------------------------------------------------
    // Position Controller: Horizontal -> Vertical -> Horizontal
    // -------------------------------------------------------------------------
    _selectTuningPage(QStringLiteral("pidTuning_tab_PositionController"), 3);
    if (QTest::currentTestFailed())
        return;
    QTest::qWait(_pageDelay);
    _cycleAxisButtons({QStringLiteral("Horizontal"), QStringLiteral("Vertical"), QStringLiteral("Horizontal")});
    if (QTest::currentTestFailed()) return;

    if (QGCCorePlugin::instance()->directMissionItemEditing()) {
        const int savedWidth = _window->width();
        const auto restoreWidth = qScopeGuard([this, savedWidth] { _window->setWidth(savedWidth); });
        const int availableWidth = _window->screen() ? _window->screen()->availableGeometry().width() : savedWidth;
        _window->setWidth(qMin(1500, qMax(savedWidth, availableWidth - 40)));
        const auto* config = findVisibleItem(_rootItem, QStringLiteral("vehicleConfig_root"));
        QVERIFY(config);
        if (config->property("embedded").toBool()) {
            auto* wideMenu = findVisibleItem(_rootItem, QStringLiteral("pidTuning_sectionMenu"), 3000);
            QVERIFY(wideMenu);
            QCOMPARE(wideMenu->property("currentIndex").toInt(), 3);
        } else {
            auto* positionTab = findVisibleItem(_rootItem, QStringLiteral("pidTuning_tab_PositionController"), 3000);
            QVERIFY(positionTab);
            QTRY_VERIFY(positionTab->property("checked").toBool());
        }
        _window->setWidth(savedWidth);
        auto* menu = findVisibleItem(_rootItem, QStringLiteral("pidTuning_sectionMenu"), 3000);
        QVERIFY(menu);
        QCOMPARE(menu->property("currentIndex").toInt(), 3);
    }

    });
}
