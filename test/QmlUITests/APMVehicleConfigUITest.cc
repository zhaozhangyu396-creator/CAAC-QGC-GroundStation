#include "APMVehicleConfigUITest.h"

#include <QtCore/QPointer>
#include <QtCore/QScopeGuard>
#include <QtGui/QFontMetricsF>
#include <QtGui/QScreen>
#include <QtQuick/QQuickItem>
#include <QtQuick/QQuickWindow>
#include <QtTest/QTest>

#include "Fact.h"
#include "MockLink.h"
#include "ParameterManager.h"
#include "Vehicle.h"

UT_REGISTER_TEST(APMVehicleConfigUITest, TestLabel::Integration)

#ifdef QGC_CUSTOM_BUILD
namespace {

QQuickItem* findTuningFactControl(QQuickItem* root, const Fact* fact)
{
    if (!root->isVisible()) {
        return nullptr;
    }
    if ((root->inherits("QQuickControl") || root->inherits("QQuickTextInput") || root->inherits("QQuickFlow")) &&
        root->property("fact").value<Fact*>() == fact) {
        return root;
    }
    for (QQuickItem* child : root->childItems()) {
        if (QQuickItem* control = findTuningFactControl(child, fact)) {
            return control;
        }
    }
    return nullptr;
}

}  // namespace

void APMVehicleConfigUITest::_testSidebarComponentCollapse()
{
    runWithMockLink([] { return MockLink::startAPMArduCopterMockLink(); },
                    [&](QPointer<MockLink>, Vehicle*) {
                        navigateToConfigureView();
                        if (QTest::currentTestFailed()) {
                            return;
                        }
                        const QString buttonName =
                            QStringLiteral("vehicleConfig_comp_") +
                            QCoreApplication::translate("APMFlightSafetyComponent", "Flight Safety").remove(' ');
                        QQuickItem* button = clickSidebarButton(buttonName);
                        QVERIFY(button);
                        QQuickItem* config = findVisibleItem(_rootItem, QStringLiteral("vehicleConfig_root"));
                        QVERIFY(config);
                        QQuickItem* loader = findItem(config, QStringLiteral("vehicleConfig_panelLoader"));
                        QVERIFY(loader);
                        const QPointer<QQuickItem> panel = loader->property("item").value<QQuickItem*>();
                        QVERIFY(panel);
                        const QVariant selectedSection = config->property("_selectedSectionIndex");
                        QTRY_VERIFY_WITH_TIMEOUT(button->property("expanded").toBool(), TestTimeout::shortMs());
                        if (findVisibleItem(config, QStringLiteral("vehicleConfig_backToMenu"), 0)) {
                            QVERIFY(clickButton(QStringLiteral("vehicleConfig_backToMenu")));
                        }
                        QVERIFY(scrollIntoView(button, QStringLiteral("vehicleConfig_sidebarFlickable")));
                        const QString sectionName = QStringLiteral("vehicleConfig_section_ReturntoLaunch");
                        QTRY_VERIFY_WITH_TIMEOUT(findVisibleItem(config, sectionName, 0), TestTimeout::shortMs());
                        const auto verifyPagePreserved = [&] {
                            QCOMPARE(loader->property("item").value<QQuickItem*>(), panel.data());
                            QCOMPARE(config->property("_selectedSectionIndex"), selectedSection);
                        };

                        QVERIFY(clickButton(buttonName + QStringLiteral("_expandToggle")));
                        QTRY_VERIFY_WITH_TIMEOUT(!button->property("expanded").toBool(), TestTimeout::shortMs());
                        QTRY_VERIFY_WITH_TIMEOUT(!findItem(config, sectionName), TestTimeout::shortMs());
                        verifyPagePreserved();
                        QVERIFY(clickButton(buttonName + QStringLiteral("_expandToggle")));
                        QTRY_VERIFY_WITH_TIMEOUT(button->property("expanded").toBool(), TestTimeout::shortMs());
                        QTRY_VERIFY_WITH_TIMEOUT(findVisibleItem(config, sectionName, 0), TestTimeout::shortMs());
                        verifyPagePreserved();
                        QVERIFY(clickButton(buttonName));
                        QTRY_VERIFY_WITH_TIMEOUT(!button->property("expanded").toBool(), TestTimeout::shortMs());
                        QTRY_VERIFY_WITH_TIMEOUT(!findItem(config, sectionName), TestTimeout::shortMs());
                        verifyPagePreserved();
                        QVERIFY(clickButton(buttonName));
                        QTRY_VERIFY_WITH_TIMEOUT(button->property("expanded").toBool(), TestTimeout::shortMs());
                        QTRY_VERIFY_WITH_TIMEOUT(findVisibleItem(config, sectionName, 0), TestTimeout::shortMs());
                        verifyPagePreserved();
                        QVERIFY(clickButton(buttonName));
                        QTRY_VERIFY_WITH_TIMEOUT(!button->property("expanded").toBool(), TestTimeout::shortMs());
                        verifyPagePreserved();

                        QQuickItem* search = findVisibleItem(config, QStringLiteral("vehicleConfig_searchField"));
                        QVERIFY(search);
                        QVERIFY(search->setProperty("text", QStringLiteral("arming")));
                        QTRY_VERIFY_WITH_TIMEOUT(button->property("expanded").toBool(), TestTimeout::shortMs());
                        QVERIFY(scrollIntoView(button, QStringLiteral("vehicleConfig_sidebarFlickable")));
                        QVERIFY(clickButton(buttonName + QStringLiteral("_expandToggle")));
                        QTRY_VERIFY_WITH_TIMEOUT(!button->property("expanded").toBool(), TestTimeout::shortMs());
                        verifyPagePreserved();
                        QVERIFY(search->setProperty("text", QString()));
                        QTRY_VERIFY_WITH_TIMEOUT(!button->property("expanded").toBool(), TestTimeout::shortMs());
                        verifyPagePreserved();
                    });
}

void APMVehicleConfigUITest::_testNarrowEscCalibration()
{
    runWithMockLink(
        [] { return MockLink::startAPMArduCopterMockLink(); },
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
            QVERIFY(clickSidebarButton(QStringLiteral("vehicleConfig_comp_") +
                                       QCoreApplication::translate("APMESCComponent", "ESC").remove(' ')));
            QQuickItem* loader = findVisibleItem(_rootItem, QStringLiteral("vehicleConfig_panelLoader"));
            QVERIFY(loader);
            QQuickItem* panel = loader->property("item").value<QQuickItem*>();
            QVERIFY(panel);
            QQuickItem* viewport = findVisibleItem(panel, QStringLiteral("vehicleSetup_scroller"));
            QVERIFY(viewport);
            QQuickItem* instructions = findItem(panel, QStringLiteral("esc_calibrationInstructions"));
            QQuickItem* warning = findItem(panel, QStringLiteral("esc_calibrationWarning"));
            QQuickItem* calibrate = findItem(panel, QStringLiteral("esc_calibrateButton"));
            QVERIFY(instructions);
            QVERIFY(warning);
            QVERIFY(calibrate);
            ParameterManager* parameters = vehicle->parameterManager();
            QVERIFY(parameters);
            QVERIFY(
                parameters->parameterExists(ParameterManager::defaultComponentId, QStringLiteral("ESC_CALIBRATION")));
            Fact* calibration =
                parameters->getParameter(ParameterManager::defaultComponentId, QStringLiteral("ESC_CALIBRATION"));
            QVERIFY(calibration);
            const QVariant savedCalibration = calibration->rawValue();

            QList<QQuickItem*> labels{warning};
            for (QQuickItem* child : instructions->childItems()) {
                if (child->inherits("QQuickText")) {
                    labels.append(child);
                }
            }
            QCOMPARE(labels.size(), 10);
            int wrappedLabels = 0;
            for (QQuickItem* label : labels) {
                const auto fitsViewport = [&] {
                    for (QQuickItem* ancestor = label; ancestor; ancestor = ancestor->parentItem()) {
                        ancestor->ensurePolished();
                    }
                    const QRectF bounds = label->mapRectToItem(viewport, label->boundingRect());
                    return label->width() > 0 && label->height() > 0 && bounds.left() >= -1 &&
                           bounds.right() <= viewport->width() + 1 &&
                           label->property("contentWidth").toReal() <= label->width() + 1;
                };
                QTRY_VERIFY2_WITH_TIMEOUT(fitsViewport(), qPrintable(label->property("text").toString()),
                                          TestTimeout::shortMs());
                QVERIFY(label->property("wrapMode").toInt() != 0);
                QVERIFY(!label->property("truncated").toBool());
                wrappedLabels += label->property("lineCount").toInt() > 1;
                const qreal absoluteY =
                    label->mapToItem(viewport, QPointF()).y() + viewport->property("contentY").toReal();
                const qreal maximumY = qMax(0.0, viewport->property("contentHeight").toReal() - viewport->height());
                viewport->setProperty("contentY", qBound(0.0, absoluteY, maximumY));
                QTRY_VERIFY_WITH_TIMEOUT(
                    label->mapRectToItem(viewport, label->boundingRect()).top() >= -1 &&
                        label->mapRectToItem(viewport, label->boundingRect()).bottom() <= viewport->height() + 1,
                    TestTimeout::shortMs());
            }
            QVERIFY(wrappedLabels > 0);
            const QRectF buttonBounds = calibrate->mapRectToItem(viewport, calibrate->boundingRect());
            QVERIFY(buttonBounds.left() >= -1 && buttonBounds.right() <= viewport->width() + 1);
            QCOMPARE(viewport->property("contentX").toReal(), 0.0);
            QCOMPARE(calibration->rawValue(), savedCalibration);
        });
}

void APMVehicleConfigUITest::_testNarrowParameterEditor()
{
    runWithMockLink([] { return MockLink::startAPMArduCopterMockLink(); },
                    [&](QPointer<MockLink>, Vehicle* vehicle) {
                        navigateToConfigureView();
                        if (QTest::currentTestFailed()) {
                            return;
                        }
                        verifyVehicleSetupMenuOrder(vehicle);
                        if (QTest::currentTestFailed()) {
                            return;
                        }
                        verifyNarrowParameterEditor(vehicle, QStringLiteral("ATC_RAT_RLL_P"));
                    });
}

void APMVehicleConfigUITest::_testNarrowTuningControls_data()
{
    QTest::addColumn<QString>("sectionName");
    QTest::addColumn<QStringList>("parameterNames");

    QTest::newRow("flight-response") << QStringLiteral("Flight Response")
                                     << QStringList{QStringLiteral("ATC_RAT_RLL_P"), QStringLiteral("PSC_D_ACC_P"),
                                                    QStringLiteral("ATC_INPUT_TC"), QStringLiteral("MOT_SPIN_ARM"),
                                                    QStringLiteral("MOT_SPIN_MIN")};
    QTest::newRow("autotune") << QStringLiteral("AutoTune")
                              << QStringList{QStringLiteral("AUTOTUNE_AXES"), QStringLiteral("AUTOTUNE_AGGR"),
                                             QStringLiteral("AUTOTUNE_MIN_D")};
    QTest::newRow("in-flight-tuning") << QStringLiteral("In Flight Tuning")
                                      << QStringList{QStringLiteral("TUNE"), QStringLiteral("TUNE_MIN"),
                                                     QStringLiteral("TUNE_MAX")};
}

void APMVehicleConfigUITest::_testNarrowTuningControls()
{
    QFETCH(QString, sectionName);
    QFETCH(QStringList, parameterNames);

    runWithMockLink(
        [] { return MockLink::startAPMArduCopterMockLink(); },
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
            QVERIFY(clickSidebarButton(QStringLiteral("vehicleConfig_comp_") +
                                       QCoreApplication::translate("APMTuningComponent", "Tuning").remove(' ')));
            if (QTest::currentTestFailed()) {
                return;
            }
            QQuickItem* config = findVisibleItem(_rootItem, QStringLiteral("vehicleConfig_root"));
            QVERIFY(config);
            QVERIFY(config->property("embedded").toBool());
            QQuickItem* loader = findVisibleItem(config, QStringLiteral("vehicleConfig_panelLoader"));
            QVERIFY(loader);
            QQuickItem* panel = loader->property("item").value<QQuickItem*>();
            QVERIFY(panel);
            QVERIFY(panel->setProperty("sectionIdFilter", sectionName));
            QQuickItem* viewport = findVisibleItem(panel, QStringLiteral("vehicleSetup_scroller"));
            QVERIFY(viewport);
            QTRY_VERIFY_WITH_TIMEOUT(viewport->width() > 0 && viewport->width() < _window->width() / 2,
                                     TestTimeout::shortMs());

            QList<QQuickItem*> controls;
            ParameterManager* parameters = vehicle->parameterManager();
            QVERIFY(parameters);
            for (const QString& name : parameterNames) {
                QVERIFY2(parameters->parameterExists(ParameterManager::defaultComponentId, name), qPrintable(name));
                Fact* fact = parameters->getParameter(ParameterManager::defaultComponentId, name);
                QVERIFY(fact);
                QQuickItem* control = nullptr;
                QTRY_VERIFY2_WITH_TIMEOUT((control = findTuningFactControl(panel, fact)) != nullptr, qPrintable(name),
                                          TestTimeout::shortMs());
                controls.append(control);
                if (control->inherits("QQuickFlow")) {
                    for (QQuickItem* child : control->childItems()) {
                        if (child->isVisible() && child->inherits("QQuickControl")) {
                            controls.append(child);
                        }
                    }
                }
            }
            if (sectionName == QStringLiteral("AutoTune")) {
                QQuickItem* channel = findVisibleItem(panel, QStringLiteral("apmAutoTuneChannelComboBox"));
                QVERIFY(channel);
                controls.append(channel);
            }

            for (QQuickItem* control : controls) {
                const QFontMetricsF metrics(control->property("font").value<QFont>());
                const qreal minimumWidth = metrics.horizontalAdvance(QStringLiteral("0000"));
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
                    return QStringLiteral("%1 tuning control %2 does not fit: viewport=%3, bounds=(%4,%5)-(%6,%7)")
                        .arg(sectionName, control->objectName())
                        .arg(viewport->width())
                        .arg(bounds.left())
                        .arg(bounds.top())
                        .arg(bounds.right())
                        .arg(bounds.bottom());
                };
                QTRY_VERIFY2_WITH_TIMEOUT(fitsViewport(), qPrintable(failureDetails()), TestTimeout::shortMs());
                const qreal absoluteY =
                    control->mapToItem(viewport, QPointF()).y() + viewport->property("contentY").toReal();
                const qreal maximumY = qMax(0.0, viewport->property("contentHeight").toReal() - viewport->height());
                viewport->setProperty("contentY", qBound(0.0, absoluteY, maximumY));
                QTRY_VERIFY_WITH_TIMEOUT(
                    control->mapRectToItem(viewport, control->boundingRect()).top() >= -1 &&
                        control->mapRectToItem(viewport, control->boundingRect()).bottom() <= viewport->height() + 1,
                    TestTimeout::shortMs());
                QCOMPARE(viewport->property("contentX").toReal(), 0.0);
            }
            verifyPanelContentVisible(QStringLiteral("Narrow_APM_Tuning_") + sectionName);
        });
}
#endif

void APMVehicleConfigUITest::init()
{
    if (!apmFirmwareSupported()) {
        QSKIP("ArduPilot support not registered in this build");
    }
    VehicleConfigUITestBase::init();
}

// ---------------------------------------------------------------------------
// Shared implementation
// ---------------------------------------------------------------------------

void APMVehicleConfigUITest::_runNavigateVehicleConfig(
    const std::function<MockLink *()> &factory, const QString &vehicleName)
{
    runWithMockLink(factory, [&](QPointer<MockLink> /*mockLink*/, Vehicle *vehicle) {
    // -------------------------------------------------------------------------
    // Navigate to the Configure view
    // -------------------------------------------------------------------------
    navigateToConfigureView();
    if (QTest::currentTestFailed()) return;

    // -------------------------------------------------------------------------
    // Click Summary
    // -------------------------------------------------------------------------
    QVERIFY2(clickButton(QStringLiteral("vehicleConfig_summary")),
             qPrintable(QStringLiteral("%1: Failed to click Summary button").arg(vehicleName)));
    QTest::qWait(_viewDelay);

    // -------------------------------------------------------------------------
    // Click through each vehicle component, in English and Chinese
    // -------------------------------------------------------------------------
    clickThroughAllComponentsAllLocales(vehicle, vehicleName);

    });
}

// ---------------------------------------------------------------------------
// Per-vehicle-type test slots
// ---------------------------------------------------------------------------

void APMVehicleConfigUITest::_testInteractiveTuningChart()
{
    runWithMockLink(
        [] { return MockLink::startAPMArduCopterMockLink(); },
        [&](QPointer<MockLink>, Vehicle*) {
            navigateToConfigureView();
            if (QTest::currentTestFailed()) {
                return;
            }
            const QString componentName =
                QCoreApplication::translate("APMAdvancedTuningCopterComponent", "Tuning - Advanced").remove(' ');
            QVERIFY(clickSidebarButton(QStringLiteral("vehicleConfig_comp_") + componentName));
            if (QTest::currentTestFailed()) {
                return;
            }
            verifyInteractiveTuningChart();
        });
}

void APMVehicleConfigUITest::_testArduCopter()
{
    _runNavigateVehicleConfig(
        [] { return MockLink::startAPMArduCopterMockLink(); },
        QStringLiteral("ArduCopter"));
}

void APMVehicleConfigUITest::_testArduPlane()
{
    _runNavigateVehicleConfig(
        [] { return MockLink::startAPMArduPlaneMockLink(); },
        QStringLiteral("ArduPlane"));
}

void APMVehicleConfigUITest::_testArduSub()
{
    // TODO: APMTuningComponentSub.qml references parameters that don't exist in
    // the Sub MockLink, causing null-fact TypeErrors. Needs investigation.
    QSKIP("ArduSub Tuning page has parameter mismatches with MockLink – skipping pending fix");
}

void APMVehicleConfigUITest::_testArduRover()
{
    _runNavigateVehicleConfig(
        [] { return MockLink::startAPMArduRoverMockLink(); },
        QStringLiteral("ArduRover"));
}
