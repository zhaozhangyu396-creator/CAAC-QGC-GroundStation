#include "PX4VehicleConfigUITest.h"

#include <QtCore/QPointer>
#include <QtCore/QScopeGuard>
#include <QtGui/QFont>
#include <QtGui/QFontMetricsF>
#include <QtGui/QScreen>
#include <QtQuick/QQuickItem>
#include <QtQuick/QQuickWindow>
#include <QtTest/QTest>

#include "Fact.h"
#include "MockLink.h"
#include "ParameterManager.h"
#include "QGCCorePlugin.h"
#include "Vehicle.h"

UT_REGISTER_TEST(PX4VehicleConfigUITest, TestLabel::Integration)

#ifdef QGC_CUSTOM_BUILD
namespace {

QQuickItem* findFactControl(QQuickItem* root, const Fact* fact)
{
    if (!root->isVisible()) {
        return nullptr;
    }
    if ((root->inherits("QQuickControl") || root->inherits("QQuickTextInput")) &&
        root->property("fact").value<Fact*>() == fact) {
        return root;
    }
    for (QQuickItem* child : root->childItems()) {
        if (QQuickItem* control = findFactControl(child, fact)) {
            return control;
        }
    }
    return nullptr;
}

}  // namespace

void PX4VehicleConfigUITest::_testNarrowParameterEditor()
{
    runWithMockLink([] { return MockLink::startPX4MockLink(); },
                    [&](QPointer<MockLink>, Vehicle* vehicle) {
                        navigateToConfigureView();
                        if (QTest::currentTestFailed()) {
                            return;
                        }
                        verifyVehicleSetupMenuOrder(vehicle);
                        if (QTest::currentTestFailed()) {
                            return;
                        }
                        verifyNarrowParameterEditor(vehicle, QStringLiteral("COM_DL_LOSS_T"));
                    });
}

void PX4VehicleConfigUITest::_testNarrowParameterControls_data()
{
    QTest::addColumn<QString>("componentName");
    QTest::addColumn<QStringList>("parameterNames");

    QTest::newRow("safety") << QStringLiteral("Safety")
                            << QStringList{QStringLiteral("COM_LOW_BAT_ACT"), QStringLiteral("BAT_LOW_THR"),
                                           QStringLiteral("BAT_CRIT_THR"), QStringLiteral("BAT_EMERGEN_THR"),
                                           QStringLiteral("CP_DIST")};
    QTest::newRow("battery") << QStringLiteral("Power")
                             << QStringList{QStringLiteral("BAT1_SOURCE"), QStringLiteral("BAT1_N_CELLS"),
                                            QStringLiteral("BAT1_CAPACITY"), QStringLiteral("BAT1_V_EMPTY"),
                                            QStringLiteral("BAT1_V_CHARGED")};
    QTest::newRow("additional-channels") << QStringLiteral("Radio")
                                         << QStringList{
                                                QStringLiteral("RC_MAP_AUX1"),   QStringLiteral("RC_MAP_AUX2"),
                                                QStringLiteral("RC_MAP_PARAM1"), QStringLiteral("RC_MAP_PARAM2"),
                                                QStringLiteral("RC_MAP_PARAM3"), QStringLiteral("RC_MAP_PAY_SW")};
}

void PX4VehicleConfigUITest::_testNarrowParameterControls()
{
    QFETCH(QString, componentName);
    QFETCH(QStringList, parameterNames);
    QVERIFY(!parameterNames.isEmpty());

    runWithMockLink(
        [] { return MockLink::startPX4MockLink(); },
        [&](QPointer<MockLink>, Vehicle* vehicle) {
            const QSize savedSize = _window->size();
            const auto restoreSize = qScopeGuard([this, savedSize] { _window->resize(savedSize); });
            const QMargins frame = _window->frameMargins();
            const int availableHeight =
                _window->screen() ? _window->screen()->availableGeometry().height() - frame.top() - frame.bottom()
                                  : 800;
            _window->resize(1000, qMin(800, availableHeight));
            QTRY_COMPARE_WITH_TIMEOUT(_window->width(), 1000, TestTimeout::shortMs());

            navigateToConfigureView();
            if (QTest::currentTestFailed()) {
                return;
            }
            QQuickItem* config = findVisibleItem(_rootItem, QStringLiteral("vehicleConfig_root"));
            QVERIFY(config);
            QVERIFY(config->property("embedded").toBool());
            QVERIFY(clickSidebarButton(QStringLiteral("vehicleConfig_comp_") + componentName));
            if (QTest::currentTestFailed()) {
                return;
            }
            QQuickItem* loader = findVisibleItem(config, QStringLiteral("vehicleConfig_panelLoader"));
            QVERIFY(loader);
            QQuickItem* panel = loader->property("item").value<QQuickItem*>();
            QVERIFY(panel);
            QQuickItem* viewport = findVisibleItem(panel, QStringLiteral("vehicleSetup_scroller"));
            QVERIFY(viewport);
            QTRY_VERIFY_WITH_TIMEOUT(viewport->width() > 0 && viewport->width() < _window->width() / 2,
                                     TestTimeout::shortMs());

            QQuickItem* firstControl = nullptr;
            ParameterManager* parameters = vehicle->parameterManager();
            QVERIFY(parameters);
            for (const QString& name : parameterNames) {
                QVERIFY2(parameters->parameterExists(ParameterManager::defaultComponentId, name), qPrintable(name));
                Fact* fact = parameters->getParameter(ParameterManager::defaultComponentId, name);
                QVERIFY2(fact, qPrintable(name));
                QQuickItem* control = nullptr;
                QTRY_VERIFY2_WITH_TIMEOUT((control = findFactControl(panel, fact)) != nullptr, qPrintable(name),
                                          TestTimeout::shortMs());
                if (!firstControl) {
                    firstControl = control;
                }
                const QFontMetricsF metrics(control->property("font").value<QFont>());
                const qreal minimumWidth = metrics.horizontalAdvance(QStringLiteral("0000")) +
                                           control->property("leftPadding").toReal() +
                                           control->property("rightPadding").toReal();
                const auto fitsViewport = [&] {
                    for (QQuickItem* ancestor = control; ancestor; ancestor = ancestor->parentItem()) {
                        ancestor->ensurePolished();
                    }
                    const QRectF bounds = control->mapRectToItem(viewport, control->boundingRect());
                    return control->width() >= minimumWidth && control->height() >= metrics.height() &&
                           bounds.left() >= -1 && bounds.right() <= viewport->width() + 1;
                };
                const auto failureDetails = [&] {
                    const QRectF bounds = control->mapRectToItem(viewport, control->boundingRect());
                    return QStringLiteral(
                               "%1 control is clipped or too narrow: viewport=%2, control=%3x%4, "
                               "minimum=%5x%6, bounds=(%7,%8)-(%9,%10)")
                        .arg(name)
                        .arg(viewport->width())
                        .arg(control->width())
                        .arg(control->height())
                        .arg(minimumWidth)
                        .arg(metrics.height())
                        .arg(bounds.left())
                        .arg(bounds.top())
                        .arg(bounds.right())
                        .arg(bounds.bottom());
                };
                QTRY_VERIFY2_WITH_TIMEOUT(fitsViewport(), qPrintable(failureDetails()), TestTimeout::shortMs());
                QCOMPARE(viewport->property("contentX").toReal(), 0.0);
            }

            // Capture the affected controls using only vertical scrolling; horizontal scrolling must not hide clipping.
            QVERIFY(firstControl);
            const qreal currentY = viewport->property("contentY").toReal();
            const qreal targetY = firstControl->mapToItem(viewport, QPointF()).y() + currentY;
            const qreal maximumY = qMax(0.0, viewport->property("contentHeight").toReal() - viewport->height());
            viewport->setProperty("contentY", qBound(0.0, targetY, maximumY));
            QCOMPARE(viewport->property("contentX").toReal(), 0.0);
            verifyPanelContentVisible(QStringLiteral("Narrow_") + componentName);
        });
}
#endif

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
        auto* popup = menu->property("popup").value<QObject*>();
        QVERIFY(popup);
        QTRY_VERIFY_WITH_TIMEOUT(popup->property("opened").toBool(), TestTimeout::shortMs());
        QTest::keyClick(_window, Qt::Key_Home);
        QTRY_COMPARE_WITH_TIMEOUT(menu->property("highlightedIndex").toInt(), 0, TestTimeout::shortMs());
        for (int index = 0; index < pageIndex; ++index) {
            QTest::keyClick(_window, Qt::Key_Down);
            QTRY_COMPARE_WITH_TIMEOUT(menu->property("highlightedIndex").toInt(), index + 1, TestTimeout::shortMs());
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

void PX4VehicleConfigUITest::_testInteractiveTuningChart()
{
    runWithMockLink([] { return MockLink::startPX4MockLink(); },
                    [&](QPointer<MockLink>, Vehicle*) {
                        navigateToConfigureView();
                        if (QTest::currentTestFailed()) {
                            return;
                        }
                        QVERIFY(clickSidebarButton(QStringLiteral("vehicleConfig_comp_PIDTuning")));
                        if (QTest::currentTestFailed()) {
                            return;
                        }
                        _selectTuningPage(QStringLiteral("pidTuning_tab_RateController"), 0);
                        if (QTest::currentTestFailed()) {
                            return;
                        }
                        verifyInteractiveTuningChart();
                    });
}
