#include "VehicleConfigUITestBase.h"

#include <algorithm>
#include <array>
#include <cmath>

#include <QtCore/QCoreApplication>
#include <QtCore/QDir>
#include <QtCore/QElapsedTimer>
#include <QtCore/QLocale>
#include <QtCore/QMetaProperty>
#include <QtCore/QPointer>
#include <QtCore/QRegularExpression>
#include <QtCore/QScopeGuard>
#include <QtGui/QGuiApplication>
#include <QtGui/QScreen>
#include <QtGui/QWheelEvent>
#include <QtQml/QQmlListReference>
#include <QtQuick/QQuickItem>
#include <QtQuick/QQuickItemGrabResult>
#include <QtQuick/QQuickWindow>
#include <QtTest/QSignalSpy>
#include <QtTest/QTest>

#include "AppSettings.h"
#include "AutoPilotPlugin.h"
#include "Fact.h"
#include "MultiVehicleManager.h"
#include "ParameterEditorController.h"
#include "ParameterManager.h"
#include "QGCApplication.h"
#include "QGCCorePlugin.h"
#include "SettingsManager.h"
#include "Vehicle.h"
#include "VehicleComponent.h"

namespace {

/// Returns true if the item tree (including \a item itself) contains at least
/// one visible text, control or image item with a non-zero size. Layout
/// containers report a size even when all their children are hidden, so only
/// leaf content items count.
bool hasVisibleContent(QQuickItem* item)
{
    if (!item->isVisible()) {
        return false;
    }
    if ((item->width() > 0) && (item->height() > 0) &&
        (item->inherits("QQuickText") || item->inherits("QQuickControl") || item->inherits("QQuickImageBase"))) {
        return true;
    }
    const QList<QQuickItem*> children = item->childItems();
    for (QQuickItem* child : children) {
        if (hasVisibleContent(child)) {
            return true;
        }
    }
    return false;
}

}  // namespace

void VehicleConfigUITestBase::navigateToConfigureView()
{
    QVERIFY2(clickToolSelectDropdownButton(QStringLiteral("toolbar_viewConfigure")),
             "Failed to navigate to Configure view");
    QTRY_VERIFY_WITH_TIMEOUT(findVisibleItem(_rootItem, QStringLiteral("caacAccessPassword"), 0) ||
                                 findVisibleItem(_rootItem, QStringLiteral("vehicleConfig_root"), 0),
                             3000);
    if (findVisibleItem(_rootItem, QStringLiteral("caacAccessPassword"), 0)) {
        QVERIFY(clickButton(QStringLiteral("caacAccessPassword")));
        for (int digit = 0; digit < 3; ++digit) {
            QTest::keyClick(_window, Qt::Key_3);
        }
        QVERIFY(acceptDialog());
    }
    QTest::qWait(_viewDelay);

    QVERIFY2(findVisibleItem(_rootItem, QStringLiteral("vehicleConfig_root"), 3000),
             "vehicleConfig_root not found after navigating to Configure");
}

QQuickItem* VehicleConfigUITestBase::clickSidebarButton(const QString& objectName)
{
    if (findVisibleItem(_rootItem, QStringLiteral("vehicleConfig_backToMenu"), 0)) {
        if (!clickButton(QStringLiteral("vehicleConfig_backToMenu"))) {
            QTest::qFail("Failed to return to the inline vehicle settings menu", __FILE__, __LINE__);
            return nullptr;
        }
    }
    QQuickItem* btn = findVisibleItem(_rootItem, objectName, 3000);
    if (!btn) {
        QTest::qFail(qPrintable(QStringLiteral("Sidebar button not found: %1").arg(objectName)), __FILE__, __LINE__);
        return nullptr;
    }

    scrollIntoView(btn, QStringLiteral("vehicleConfig_sidebarFlickable"));
    const QPointF center = btn->mapToScene(QPointF(btn->width() / 2, btn->height() / 2));
    QTest::mouseClick(_window, Qt::LeftButton, Qt::NoModifier, center.toPoint());
    QTest::qWait(_pageDelay);

    return btn;
}

void VehicleConfigUITestBase::resetParamsToFirmwareDefaults(Vehicle* vehicle, const QString& sentinelParamName)
{
    ParameterManager* mgr = vehicle->parameterManager();

    // The sentinel parameter is non-default in the MockLink params file but
    // defaults to 0 in the parameter metadata, so it signals the refresh completing
    Fact* sentinelFact = mgr->getParameter(ParameterManager::defaultComponentId, sentinelParamName);
    QVERIFY2(sentinelFact, qPrintable(QStringLiteral("%1 fact not found").arg(sentinelParamName)));
    QVERIFY2(sentinelFact->rawValue().toInt() != 0,
             qPrintable(QStringLiteral("%1 already at default before reset").arg(sentinelParamName)));

    // Sends MAV_CMD_PREFLIGHT_STORAGE param1=2 which MockLink handles by resetting
    // its parameters to the metadata defaults
    mgr->resetAllParametersToDefaults();
    mgr->refreshAllParameters();

    QVERIFY2(QTest::qWaitFor([&] { return sentinelFact->rawValue().toInt() == 0; }, 30000),
             "Parameters never refreshed to firmware defaults");
}

void VehicleConfigUITestBase::resetAPMParamsToUncalibrated(Vehicle* vehicle)
{
    ParameterManager* mgr = vehicle->parameterManager();

    QVERIFY2(mgr->parameterExists(ParameterManager::defaultComponentId, QStringLiteral("COMPASS_OFS_X")),
             "COMPASS_OFS_X parameter not found");
    Fact* compassOfs = mgr->getParameter(ParameterManager::defaultComponentId, QStringLiteral("COMPASS_OFS_X"));
    QVERIFY2(compassOfs, "COMPASS_OFS_X fact not found");

    // Sends MAV_CMD_PREFLIGHT_STORAGE param1=2 which MockLink's APM branch zeroes
    // the compass and accel offset parameters.
    mgr->resetAllParametersToDefaults();
    mgr->refreshAllParameters();

    QVERIFY2(QTest::qWaitFor([&] { return qFuzzyIsNull(compassOfs->rawValue().toFloat()); }, 30000),
             "COMPASS_OFS_X never refreshed to 0 after APM param reset");
}

void VehicleConfigUITestBase::clickThroughAllComponents(Vehicle* vehicle, const QString& vehicleName)
{
    const QString prefix = vehicleName.isEmpty() ? QString() : (vehicleName + QStringLiteral(": "));

    const QVariantList components = vehicle->autopilotPlugin()->vehicleComponents();
    QVERIFY2(!components.isEmpty(), qPrintable(QStringLiteral("%1No vehicle components found").arg(prefix)));

    for (const QVariant& compVariant : components) {
        if (findVisibleItem(_rootItem, QStringLiteral("vehicleConfig_backToMenu"), 0)) {
            QVERIFY(clickButton(QStringLiteral("vehicleConfig_backToMenu")));
        }
        auto* comp = compVariant.value<VehicleComponent*>();
        if (!comp) {
            continue;
        }

        // Match the objectName set in VehicleConfigView.qml:
        // "vehicleConfig_comp_" + compName.replace(/ /g, "")
        const QString cleanName = QString(comp->name()).remove(QLatin1Char(' '));
        const QString buttonName = QStringLiteral("vehicleConfig_comp_") + cleanName;

        QQuickItem* btn = findVisibleItem(_rootItem, buttonName, 2000);
        if (!btn) {
            // Component may be hidden (e.g. optional peripheral not present) – skip silently
            continue;
        }

        clickSidebarButton(buttonName);
        if (QTest::currentTestFailed())
            return;

        QQuickItem* loader = findVisibleItem(_rootItem, QStringLiteral("vehicleConfig_panelLoader"), 2000);
        QVERIFY2(
            loader,
            qPrintable(
                QStringLiteral("%1vehicleConfig_panelLoader not found after clicking %2").arg(prefix, comp->name())));
        QVERIFY2(loader->property("item").value<QQuickItem*>() != nullptr,
                 qPrintable(QStringLiteral("%1Panel loader has no item after clicking %2").arg(prefix, comp->name())));

        verifyPanelContentVisible(prefix + comp->name());
        if (QTest::currentTestFailed())
            return;

        // Click each section button in the expanded tree and verify the page
        // shows content for that section
        const QStringList sectionIds = comp->sectionIds();
        for (const QString& sectionId : sectionIds) {
            // Matches objectName in VehicleConfigView.qml: "vehicleConfig_section_" + id without spaces
            const QString sectionButtonName =
                QStringLiteral("vehicleConfig_section_") + QString(sectionId).remove(QLatin1Char(' '));
            auto* section = findItem(_rootItem, sectionButtonName);
            if (!section || !section->property("sectionContentVisible").toBool()) {
                // Section hidden by sectionVisible() filtering – skip silently
                continue;
            }
            clickSidebarButton(sectionButtonName);
            if (QTest::currentTestFailed())
                return;
            verifyPanelContentVisible(prefix + comp->name() + QStringLiteral(" / ") + sectionId);
            if (QTest::currentTestFailed())
                return;
        }
    }
}

void VehicleConfigUITestBase::verifyPanelContentVisible(const QString& context)
{
    QQuickItem* loader = findVisibleItem(_rootItem, QStringLiteral("vehicleConfig_panelLoader"), 2000);
    QVERIFY2(loader, qPrintable(context + QStringLiteral(": vehicleConfig_panelLoader not found")));

    QQuickItem* panel = loader->property("item").value<QQuickItem*>();
    QVERIFY2(panel, qPrintable(context + QStringLiteral(": panel loader has no item")));

    // For SetupPage-based panels check the page content, not the SetupPage
    // chrome (title/description) which renders even when the content is blank
    QQuickItem* target = panel;
    if (QQuickItem* contentLoader = panel->findChild<QQuickItem*>(QStringLiteral("setupPage_contentLoader"))) {
        QQuickItem* contentItem = contentLoader->property("item").value<QQuickItem*>();
        QVERIFY2(contentItem, qPrintable(context + QStringLiteral(": setup page content loader has no item")));
        target = contentItem;
    }

    QVERIFY2(QTest::qWaitFor([&] { return hasVisibleContent(target); }, 3000),
             qPrintable(context + QStringLiteral(": page loaded but renders no visible content (blank page)")));

    if (const auto previewDirectory = qgetenv("CAAC_VEHICLE_SETUP_PREVIEWS"); !previewDirectory.isEmpty()) {
        QDir directory(QString::fromUtf8(previewDirectory));
        QVERIFY(directory.mkpath(QStringLiteral(".")));
        QString fileName = context;
        fileName.replace(QRegularExpression(QStringLiteral("[^\\p{L}\\p{N}_-]+")), QStringLiteral("_"));
        auto* config = findVisibleItem(_rootItem, QStringLiteral("vehicleConfig_root"));
        QVERIFY(config);
        const auto capture = config->grabToImage();
        QVERIFY(capture);
        QSignalSpy ready(capture.data(), &QQuickItemGrabResult::ready);
        QTRY_VERIFY_WITH_TIMEOUT(!ready.isEmpty(), 3000);
        QVERIFY(capture->saveToFile(directory.filePath(fileName + QStringLiteral(".png"))));
    }
}

void VehicleConfigUITestBase::verifyInteractiveTuningChart()
{
    QPointer<QQuickItem> chart = findVisibleItem(_rootItem, QStringLiteral("pidTuning_chart"));
    QVERIFY2(chart, "The real PID tuning chart did not load");
    QPointer<QQuickItem> graph = chart->property("graphView").value<QQuickItem*>();
    QVERIFY(graph);
    QCOMPARE(graph->objectName(), QStringLiteral("pidTuning_graph"));
    QCOMPARE(graph->window(), _window);
    QVERIFY(!chart->property("expanded").toBool());
    QPointer<QQuickWindow> floating;
    const auto closeFloating = qScopeGuard([&] {
        if (chart) {
            QMetaObject::invokeMethod(chart, "closeExpanded");
        }
        if (floating) {
            floating->close();
        }
    });

    // Static charts may reuse their frame; synchronize GUI layout without requiring buffer swaps.
    const auto waitForLayout = [](QQuickWindow* window) {
        if (!window || !window->isExposed()) {
            return false;
        }
        QSignalSpy passes(window, &QQuickWindow::afterAnimating);
        for (int index = 0; index < 3; ++index) {
            const qsizetype previousCount = passes.count();
            window->update();
            if (passes.count() == previousCount && !passes.wait(TestTimeout::shortMs())) {
                return false;
            }
        }
        return true;
    };
    QVERIFY(waitForLayout(_window));
    QQmlListReference seriesList(graph, "seriesList");
    QVERIFY(seriesList.isValid() && seriesList.canCount() && seriesList.canAt());
    QTRY_VERIFY_WITH_TIMEOUT(seriesList.count() > 0, TestTimeout::mediumMs());
    QList<QPointer<QObject>> series;
    for (qsizetype index = 0; index < seriesList.count(); ++index) {
        series.append(seriesList.at(index));
        QVERIFY(series.last());
    }
    const auto sameSeries = [&] {
        if (seriesList.count() != series.size()) {
            return false;
        }
        for (qsizetype index = 0; index < series.size(); ++index) {
            if (!series[index] || seriesList.at(index) != series[index].data()) {
                return false;
            }
        }
        return true;
    };
    const auto pointCounts = [&] {
        QList<int> counts;
        for (const auto& item : series) {
            counts.append(item ? item->property("count").toInt() : -1);
        }
        return counts;
    };
    const auto firstCount = [&] { return pointCounts().first(); };
    const auto allEmpty = [&] {
        for (const int count : pointCounts()) {
            if (count != 0) {
                return false;
            }
        }
        return true;
    };
    QPointer<QObject> axisX = graph->property("axisX").value<QObject*>();
    QPointer<QObject> axisY = graph->property("axisY").value<QObject*>();
    QVERIFY(axisX && axisY);
    const auto ranges = [&] {
        if (!axisX || !axisY) {
            return std::array<qreal, 4>{qQNaN(), qQNaN(), qQNaN(), qQNaN()};
        }
        return std::array<qreal, 4>{axisX->property("min").toReal(), axisX->property("max").toReal(),
                                    axisY->property("min").toReal(), axisY->property("max").toReal()};
    };
    const auto closeEnough = [](qreal actual, qreal expected) {
        return std::isfinite(actual) && std::isfinite(expected) &&
               qAbs(actual - expected) <= 1e-6 * qMax(1.0, qAbs(expected));
    };
    const auto validRanges = [&] {
        const auto values = ranges();
        return std::isfinite(values[0]) && std::isfinite(values[1]) && values[1] > values[0] &&
               std::isfinite(values[2]) && std::isfinite(values[3]) && values[3] > values[2];
    };
    const auto unchangedRanges = [&](const std::array<qreal, 4>& expected) {
        const auto actual = ranges();
        for (std::size_t index = 0; index < actual.size(); ++index) {
            if (!closeEnough(actual[index], expected[index])) {
                return false;
            }
        }
        return true;
    };
    const auto clickFloatingButton = [&](const QString& objectName) {
        QQuickItem* button = floating ? findVisibleItem(floating->contentItem(), objectName) : nullptr;
        if (!button || !button->isEnabled()) {
            return false;
        }
        button->ensurePolished();
        const QPoint point = button->mapToScene(QPointF(button->width() / 2, button->height() / 2)).toPoint();
        if (!QRect(QPoint(), floating->size()).contains(point)) {
            return false;
        }
        QTest::mouseClick(floating, Qt::LeftButton, Qt::NoModifier, point);
        return true;
    };
    QString chartClickFailure;
    const auto doubleClickInline = [&] {
        if (!scrollIntoView(chart, QStringLiteral("vehicleSetup_scroller"))) {
            return false;
        }
        graph->ensurePolished();
        QRectF visiblePlot = graph->mapRectToScene(graph->property("plotArea").toRectF());
        for (auto* ancestor = graph->parentItem(); ancestor; ancestor = ancestor->parentItem()) {
            if (ancestor->clip()) {
                visiblePlot = visiblePlot.intersected(ancestor->mapRectToScene(ancestor->boundingRect()));
            }
        }
        visiblePlot = visiblePlot.intersected(QRectF(QPointF(), _window->size())).adjusted(1, 1, -1, -1);
        if (visiblePlot.width() <= 1 || visiblePlot.height() <= 1) {
            chartClickFailure = QStringLiteral("Chart plot is outside the visible viewport");
            return false;
        }
        const QPoint point = visiblePlot.center().toPoint();
        auto* input = findVisibleItem(graph->parentItem(), QStringLiteral("pidTuning_chartInput"));
        if (!input) {
            chartClickFailure = QStringLiteral("Chart input area is not visible");
            return false;
        }
        QTest::mousePress(_window, Qt::LeftButton, Qt::NoModifier, point);
        const bool receivedPress = input->property("pressed").toBool();
        QTest::mouseRelease(_window, Qt::LeftButton, Qt::NoModifier, point);
        if (!receivedPress) {
            QStringList hitPath;
            QQuickItem* hit = _window->contentItem();
            while (hit) {
                hitPath.append(QStringLiteral("%1(%2)").arg(QString::fromLatin1(hit->metaObject()->className()),
                                                            hit->objectName()));
                hit = hit->childAt(hit->mapFromScene(point).x(), hit->mapFromScene(point).y());
            }
            chartClickFailure = QStringLiteral("Chart press at %1,%2 missed input: %3")
                                    .arg(point.x())
                                    .arg(point.y())
                                    .arg(hitPath.join(QStringLiteral(" / ")));
            return false;
        }
        QTest::mouseDClick(_window, Qt::LeftButton, Qt::NoModifier, point);
        return true;
    };
    const auto sendWheel = [&](const QPointF& localPoint) {
        const QPointF point = graph->mapToScene(localPoint);
        QTest::mouseMove(floating, point.toPoint());
        QWheelEvent wheel(point, floating->mapToGlobal(point.toPoint()), QPoint(), QPoint(0, 120), Qt::NoButton,
                          Qt::NoModifier, Qt::NoScrollPhase, false);
        QCoreApplication::sendEvent(floating, &wheel);
        return wheel.isAccepted();
    };

    QPointer<QQuickItem> inlineToggle = findItem(_rootItem, QStringLiteral("pidTuning_toggle"));
    QVERIFY(inlineToggle);
    QTRY_VERIFY_WITH_TIMEOUT(chart->property("running").toBool() && firstCount() >= 3 && validRanges(),
                             TestTimeout::mediumMs());
    QVERIFY(clickButtonScrolled(QStringLiteral("pidTuning_toggle"), QStringLiteral("vehicleSetup_scroller")));
    QTRY_VERIFY_WITH_TIMEOUT(!chart->property("running").toBool(), TestTimeout::shortMs());
    const QList<int> beforeExpansion = pointCounts();
    QVERIFY2(doubleClickInline(), qPrintable(chartClickFailure));
    QTRY_VERIFY2_WITH_TIMEOUT(
        chart->property("expanded").toBool() && graph->window() != _window,
        qPrintable(QStringLiteral("plotArea %1,%2 %3x%4; chart %5x%6; scene center %7,%8")
                       .arg(graph->property("plotArea").toRectF().x())
                       .arg(graph->property("plotArea").toRectF().y())
                       .arg(graph->property("plotArea").toRectF().width())
                       .arg(graph->property("plotArea").toRectF().height())
                       .arg(chart->width())
                       .arg(chart->height())
                       .arg(graph->mapToScene(graph->property("plotArea").toRectF().center()).x())
                       .arg(graph->mapToScene(graph->property("plotArea").toRectF().center()).y())),
        TestTimeout::mediumMs());
    floating = graph->window();
    QVERIFY(floating);
    QCOMPARE(floating->objectName(), QStringLiteral("pidTuning_chartWindow"));
    QCOMPARE(floating->modality(), Qt::NonModal);
    QVERIFY((floating->flags() & Qt::WindowType_Mask) == Qt::Window);
    QVERIFY(!(floating->flags() & Qt::FramelessWindowHint));
    QTRY_VERIFY_WITH_TIMEOUT(floating->isExposed(), TestTimeout::mediumMs());
    QVERIFY(waitForLayout(floating));
    QVERIFY(sameSeries());
    QCOMPARE(pointCounts(), beforeExpansion);
    QCOMPARE(graph->property("axisX").value<QObject*>(), axisX.data());
    QCOMPARE(graph->property("axisY").value<QObject*>(), axisY.data());

    QScreen* screen = floating->screen();
    QVERIFY(screen);
    QVERIFY(floating->width() <= screen->geometry().width());
    QVERIFY(floating->height() <= screen->geometry().height());
    const QRect workArea = screen->availableGeometry();
    QVERIFY(floating->minimumWidth() > 0 && floating->minimumHeight() > 0);
    QVERIFY(floating->maximumWidth() <= workArea.width() && floating->maximumHeight() <= workArea.height());
    const QMargins frame = floating->frameMargins();
    const QPoint position = workArea.topLeft() + QPoint(frame.left(), frame.top());
    floating->setPosition(position);
    QTRY_COMPARE_WITH_TIMEOUT(floating->position(), position, TestTimeout::shortMs());
    const QSize originalSize = floating->size();
    const QSizeF originalGraphSize(graph->width(), graph->height());
    const QSize resized(qMax(floating->minimumWidth(), originalSize.width() * 4 / 5),
                        qMax(floating->minimumHeight(), originalSize.height() * 4 / 5));
    QVERIFY(resized != originalSize);
    floating->resize(resized);
    QTRY_COMPARE_WITH_TIMEOUT(floating->size(), resized, TestTimeout::shortMs());
    QTRY_VERIFY_WITH_TIMEOUT(
        graph->width() > 0 && graph->height() > 0 &&
            (resized.width() == originalSize.width() || graph->width() < originalGraphSize.width() - 1) &&
            (resized.height() == originalSize.height() || graph->height() < originalGraphSize.height() - 1),
        TestTimeout::shortMs());
    QVERIFY(waitForLayout(floating));
    QVERIFY(workArea.contains(floating->frameGeometry()));
    QVERIFY(sameSeries());
    QCOMPARE(pointCounts(), beforeExpansion);

    QVERIFY(clickFloatingButton(QStringLiteral("pidTuning_expandedToggle")));
    QTRY_VERIFY_WITH_TIMEOUT(chart->property("running").toBool() && firstCount() > beforeExpansion.first() + 2,
                             TestTimeout::mediumMs());
    QPointer<QQuickItem> expandedToggle =
        findVisibleItem(floating->contentItem(), QStringLiteral("pidTuning_expandedToggle"));
    QVERIFY(expandedToggle);
    QCOMPARE(expandedToggle->property("text").toString(), inlineToggle->property("text").toString());
    QTRY_VERIFY_WITH_TIMEOUT(validRanges() && graph->property("plotArea").toRectF().width() > 0 &&
                                 graph->property("plotArea").toRectF().height() > 0,
                             TestTimeout::shortMs());
    const auto beforePan = ranges();
    const QRectF plot = graph->property("plotArea").toRectF();
    const QPoint press = graph->mapToScene(plot.center()).toPoint();
    const QPoint offset(qMax(20, qRound(plot.width() / 8)), qMax(20, qRound(plot.height() / 8)));
    QTest::mousePress(floating, Qt::LeftButton, Qt::NoModifier, press);
    for (int step = 1; step <= 4; ++step) {
        QTest::mouseMove(floating, press + offset * step / 4);
    }
    QTest::mouseRelease(floating, Qt::LeftButton, Qt::NoModifier, press + offset);
    QTRY_VERIFY_WITH_TIMEOUT(!chart->property("followLatest").toBool() && !closeEnough(ranges()[0], beforePan[0]) &&
                                 !closeEnough(ranges()[2], beforePan[2]),
                             TestTimeout::shortMs());
    QVERIFY(chart->property("running").toBool());
    const auto panned = ranges();
    const int countAfterPan = firstCount();
    QTRY_VERIFY_WITH_TIMEOUT(firstCount() > countAfterPan + 2, TestTimeout::mediumMs());
    QVERIFY2(unchangedRanges(panned), "Live samples overwrote the manually panned axes");

    // An off-centre cursor distinguishes anchored zoom from zooming around the axis centre or endpoint.
    const QRectF zoomPlot = graph->property("plotArea").toRectF();
    const QPointF cursor(zoomPlot.left() + zoomPlot.width() * 0.37, zoomPlot.top() + zoomPlot.height() * 0.63);
    const auto beforeZoom = ranges();
    const qreal cursorX = beforeZoom[0] + (beforeZoom[1] - beforeZoom[0]) * 0.37;
    const qreal cursorY = beforeZoom[2] + (beforeZoom[3] - beforeZoom[2]) * 0.37;
    QVERIFY(sendWheel(cursor));
    QTRY_VERIFY_WITH_TIMEOUT(ranges()[1] - ranges()[0] < beforeZoom[1] - beforeZoom[0] &&
                                 ranges()[3] - ranges()[2] < beforeZoom[3] - beforeZoom[2],
                             TestTimeout::shortMs());
    const auto zoomed = ranges();
    QVERIFY(closeEnough(zoomed[0] + (zoomed[1] - zoomed[0]) * 0.37, cursorX));
    QVERIFY(closeEnough(zoomed[2] + (zoomed[3] - zoomed[2]) * 0.37, cursorY));
    const int countAfterZoom = firstCount();
    QTRY_VERIFY_WITH_TIMEOUT(firstCount() > countAfterZoom + 2, TestTimeout::mediumMs());
    QVERIFY2(unchangedRanges(zoomed), "Live samples overwrote the cursor-centred zoom");
    QVERIFY(!chart->property("followLatest").toBool());
    QVERIFY(chart->property("running").toBool());
    QVERIFY(sameSeries());

    QVERIFY(clickFloatingButton(QStringLiteral("pidTuning_expandedToggle")));
    QTRY_VERIFY_WITH_TIMEOUT(!chart->property("running").toBool(), TestTimeout::shortMs());
    QCOMPARE(expandedToggle->property("text").toString(), inlineToggle->property("text").toString());
    const QList<int> stoppedCounts = pointCounts();
    const QMetaProperty countProperty =
        series.first()->metaObject()->property(series.first()->metaObject()->indexOfProperty("count"));
    QVERIFY(countProperty.hasNotifySignal());
    QSignalSpy countChanged(series.first(), countProperty.notifySignal());
    QVERIFY(countChanged.isValid());
    QVERIFY(floating->isExposed());
    QCOMPARE(graph->window(), floating.data());
    // A stopped chart need not render new frames. Observe its data beyond a sampling interval instead.
    QVERIFY(!countChanged.wait(TestTimeout::shortMs()));
    QCOMPARE(pointCounts(), stoppedCounts);
    QVERIFY(clickFloatingButton(QStringLiteral("pidTuning_expandedClose")));
    QTRY_VERIFY_WITH_TIMEOUT(!chart->property("expanded").toBool() && graph->window() == _window,
                             TestTimeout::shortMs());
    QVERIFY(!floating->isVisible());
    QVERIFY(!chart->property("followLatest").toBool());
    QVERIFY(unchangedRanges(zoomed));
    QVERIFY(sameSeries());
    QCOMPARE(pointCounts(), stoppedCounts);

    QVERIFY2(doubleClickInline(), qPrintable(chartClickFailure));
    QTRY_VERIFY_WITH_TIMEOUT(chart->property("expanded").toBool() && graph->window() == floating,
                             TestTimeout::shortMs());
    QVERIFY(clickFloatingButton(QStringLiteral("pidTuning_expandedRestore")));
    QTRY_VERIFY_WITH_TIMEOUT(chart->property("followLatest").toBool(), TestTimeout::shortMs());
    QVERIFY(chart->property("expanded").toBool());
    const qreal restoredMaximum = axisX->property("max").toReal();
    QVERIFY(clickFloatingButton(QStringLiteral("pidTuning_expandedToggle")));
    QTRY_VERIFY_WITH_TIMEOUT(chart->property("running").toBool() && firstCount() > stoppedCounts.first() + 2 &&
                                 validRanges() && axisX->property("max").toReal() > restoredMaximum,
                             TestTimeout::mediumMs());
    QVERIFY(sendWheel(graph->property("plotArea").toRectF().center()));
    QTRY_VERIFY_WITH_TIMEOUT(!chart->property("followLatest").toBool(), TestTimeout::shortMs());
    const QPoint doubleClick = graph->mapToScene(graph->property("plotArea").toRectF().center()).toPoint();
    QTest::mouseDClick(floating, Qt::LeftButton, Qt::NoModifier, doubleClick);
    QTRY_VERIFY_WITH_TIMEOUT(chart->property("followLatest").toBool(), TestTimeout::shortMs());
    QVERIFY(chart->property("expanded").toBool());
    QCOMPARE(graph->window(), floating.data());
    QVERIFY(clickFloatingButton(QStringLiteral("pidTuning_expandedToggle")));
    QTRY_VERIFY_WITH_TIMEOUT(!chart->property("running").toBool(), TestTimeout::shortMs());
    QVERIFY(clickFloatingButton(QStringLiteral("pidTuning_expandedClear")));
    QTRY_VERIFY_WITH_TIMEOUT(allEmpty(), TestTimeout::shortMs());
    QVERIFY(sameSeries());
    QVERIFY(clickFloatingButton(QStringLiteral("pidTuning_expandedClose")));
    QTRY_VERIFY_WITH_TIMEOUT(!chart->property("expanded").toBool() && graph->window() == _window,
                             TestTimeout::shortMs());
    QVERIFY(sameSeries());
    QVERIFY(allEmpty());

    QVERIFY(clickButtonScrolled(QStringLiteral("pidTuning_toggle"), QStringLiteral("vehicleSetup_scroller")));
    QTRY_VERIFY_WITH_TIMEOUT(chart->property("running").toBool() && firstCount() > 2, TestTimeout::mediumMs());
    QVERIFY(clickButtonScrolled(QStringLiteral("pidTuning_toggle"), QStringLiteral("vehicleSetup_scroller")));
    QTRY_VERIFY_WITH_TIMEOUT(!chart->property("running").toBool(), TestTimeout::shortMs());
    QVERIFY(clickButtonScrolled(QStringLiteral("pidTuning_clear"), QStringLiteral("vehicleSetup_scroller")));
    QTRY_VERIFY_WITH_TIMEOUT(allEmpty(), TestTimeout::shortMs());
    QVERIFY(sameSeries());

    QVERIFY2(doubleClickInline(), qPrintable(chartClickFailure));
    QTRY_VERIFY_WITH_TIMEOUT(chart->property("expanded").toBool(), TestTimeout::shortMs());
    auto* config = findVisibleItem(_rootItem, QStringLiteral("vehicleConfig_root"));
    QVERIFY(config);
    if (config->property("embedded").toBool()) {
        QVERIFY(clickButton(QStringLiteral("vehicleConfig_backToMenu")));
        QTRY_VERIFY_WITH_TIMEOUT(!floating || !floating->isVisible(), TestTimeout::shortMs());
    }
    QVERIFY(
        clickButtonScrolled(QStringLiteral("vehicleConfig_summary"), QStringLiteral("vehicleConfig_sidebarFlickable")));
    QTRY_VERIFY_WITH_TIMEOUT(!chart && !graph && !floating, TestTimeout::mediumMs());
    for (QWindow* window : QGuiApplication::topLevelWindows()) {
        QVERIFY(window->objectName() != QStringLiteral("pidTuning_chartWindow"));
    }
}

void VehicleConfigUITestBase::clickThroughAllComponentsAllLocales(Vehicle* vehicle, const QString& vehicleName)
{
    Fact* languageFact = SettingsManager::instance()->appSettings()->qLocaleLanguage();
    const QVariant savedLanguage = languageFact->rawValue();

    // A mid-test failure must not leak the language setting into later tests
    auto restoreLanguage = qScopeGuard([this, languageFact, savedLanguage] {
        if (languageFact->rawValue() == savedLanguage) {
            return;
        }
        qgcApp()->resetRebootMessageDebounce();
        languageFact->setRawValue(savedLanguage);
        (void) acceptDialog(5000);
    });

    // Translations may not exist for the machine's system locale (e.g. "C" on CI)
    ignoreLogMessage("API.QGCApplication", QtWarningMsg,
                     QRegularExpression(QStringLiteral("Qt lib localization for .* is not present")));
    ignoreLogMessage("API.QGCApplication", QtWarningMsg,
                     QRegularExpression(QStringLiteral("Error loading source localization for .*")));
    ignoreLogMessage("API.QGCApplication", QtWarningMsg,
                     QRegularExpression(QStringLiteral("Error loading json localization for .*")));

    // The stored locale is machine-dependent: force English so the first pass and
    // the anchor text capture are actually English.
    // Language is a reboot-required setting: each change announces a restart via
    // an app message dialog which must be dismissed before clicking the UI.
    if (languageFact->rawValue().toInt() != QLocale::English) {
        expectAppMessage(QRegularExpression(
            QRegularExpression::escape(Fact::tr("Restart application for changes to take effect."))));
        languageFact->setRawValue(QLocale::English);
        QVERIFY2(acceptDialog(5000), "Restart-required dialog never shown after switching language to English");
        verifyExpectedLogMessage();
        if (QTest::currentTestFailed())
            return;
    }

    clickThroughAllComponents(vehicle, vehicleName);
    if (QTest::currentTestFailed())
        return;

    // Language anchor: capture live English text so the switch can be verified
    // to actually retranslate the UI (not just flip the setting)
    QQuickItem* sensorsButton = clickSidebarButton(QStringLiteral("vehicleConfig_comp_Sensors"));
    if (QTest::currentTestFailed())
        return;
    QVERIFY(sensorsButton);
    if (sensorsButton->property("expandable").toBool() && !sensorsButton->property("expanded").toBool()) {
        QVERIFY(clickButtonScrolled(QStringLiteral("vehicleConfig_comp_Sensors_expandToggle"),
                                    QStringLiteral("vehicleConfig_sidebarFlickable")));
        QTRY_VERIFY_WITH_TIMEOUT(sensorsButton->property("expanded").toBool(), TestTimeout::shortMs());
    }
    if (findItem(_rootItem, QStringLiteral("vehicleConfig_section_Compass"))) {
        clickSidebarButton(QStringLiteral("vehicleConfig_section_Compass"));
        if (QTest::currentTestFailed())
            return;
    }
    QPointer<QQuickItem> anchor = findVisibleItem(_rootItem, QStringLiteral("sensorsSetup_calibrateCompass"), 3000);
    QVERIFY2(anchor, "Calibrate Compass language anchor not found");
    const QString englishAnchorText = anchor->property("text").toString();

    qgcApp()->resetRebootMessageDebounce();
    expectAppMessage(
        QRegularExpression(QRegularExpression::escape(Fact::tr("Restart application for changes to take effect."))));
    languageFact->setRawValue(QLocale::Chinese);
    QVERIFY2(acceptDialog(5000), "Restart-required dialog never shown after switching language to Chinese");
    verifyExpectedLogMessage();
    if (QTest::currentTestFailed())
        return;

    QVERIFY2(anchor, "Language anchor destroyed during language switch");
    QVERIFY2(
        QTest::qWaitFor([&] { return anchor && (anchor->property("text").toString() != englishAnchorText); }, 3000),
        qPrintable(
            QStringLiteral("UI text did not change after switching to Chinese (still '%1')").arg(englishAnchorText)));

    const QString zhPrefix = vehicleName.isEmpty() ? QStringLiteral("zh_CN") : vehicleName + QStringLiteral(" zh_CN");
    clickThroughAllComponents(vehicle, zhPrefix);
    if (QTest::currentTestFailed())
        return;

    // No restore needed when the saved language was already Chinese (no change, no dialog)
    if (languageFact->rawValue() != savedLanguage) {
        // showRebootAppMessage() debounces repeats: reset so the restore change shows its own dialog
        qgcApp()->resetRebootMessageDebounce();
        expectAppMessage(QRegularExpression(
            QRegularExpression::escape(Fact::tr("Restart application for changes to take effect."))));
        languageFact->setRawValue(savedLanguage);
        QVERIFY2(acceptDialog(5000), "Restart-required dialog never shown after restoring language");
        verifyExpectedLogMessage();
    }

    // The saved language may itself be non-English, so only verify retranslation
    // back to the English anchor when it renders English
    if (savedLanguage.toInt() == QLocale::English) {
        QPointer<QQuickItem> restoredAnchor =
            findVisibleItem(_rootItem, QStringLiteral("sensorsSetup_calibrateCompass"), 3000);
        if (restoredAnchor) {
            QVERIFY2(QTest::qWaitFor(
                         [&] {
                             return restoredAnchor &&
                                    (restoredAnchor->property("text").toString() == englishAnchorText);
                         },
                         3000),
                     "UI text did not return to English after restoring language");
        }
    }
}

void VehicleConfigUITestBase::waitForParamRefreshQuiet(Vehicle* vehicle)
{
    ParameterManager* mgr = vehicle->parameterManager();
    QElapsedTimer sinceLastResponse;
    sinceLastResponse.start();

    QObject context;
    QObject::connect(mgr, &ParameterManager::_paramRequestReadSuccess, &context, [&] { sinceLastResponse.restart(); });
    QObject::connect(mgr, &ParameterManager::_paramRequestReadFailure, &context, [&] { sinceLastResponse.restart(); });

    QVERIFY2(QTest::qWaitFor([&] { return sinceLastResponse.elapsed() > 500; }, 10000),
             "waitForParamRefreshQuiet: parameter refresh traffic still active after 10s");
}

void VehicleConfigUITestBase::navigateToAPMSensorsPage()
{
    navigateToConfigureView();
    if (QTest::currentTestFailed())
        return;

    QQuickItem* sensorsBtn = clickSidebarButton(QStringLiteral("vehicleConfig_comp_Sensors"));
    if (QTest::currentTestFailed())
        return;

    if (QGCCorePlugin::instance()->directMissionItemEditing()) {
        QVERIFY2(sensorsBtn->property("expanded").toBool(), "APM sensor secondary menu was not expanded");
        QVERIFY2(findVisibleItem(_rootItem, QStringLiteral("sensorsSetup_setOrientation"), 3000),
                 "Flight controller orientation was not shown in its secondary page");
        QVERIFY(!findVisibleItem(_rootItem, QStringLiteral("sensorsSetup_calibrateAccel"), 0));
        QVERIFY(!findVisibleItem(_rootItem, QStringLiteral("sensorsSetup_calibrateCompass"), 0));
        return;
    }
    QVERIFY2(findVisibleItem(_rootItem, QStringLiteral("sensorsSetup_calibrateAccel"), 5000),
             "sensorsSetup_calibrateAccel not found after opening Sensors page");
    QVERIFY2(findVisibleItem(_rootItem, QStringLiteral("sensorsSetup_calibrateCompass"), 3000),
             "sensorsSetup_calibrateCompass not found after opening Sensors page");
    Q_UNUSED(sensorsBtn);
}

void VehicleConfigUITestBase::selectAPMSensorSection(const QString& sectionId)
{
    if (!QGCCorePlugin::instance()->directMissionItemEditing()) {
        return;
    }
    QVERIFY(clickSidebarButton(QStringLiteral("vehicleConfig_section_") + QString(sectionId).remove(' ')));
    if (QTest::currentTestFailed()) {
        return;
    }
    QQuickItem* section =
        findItem(_rootItem, QStringLiteral("vehicleConfig_section_") + QString(sectionId).remove(' '));
    QVERIFY(section);
    QTRY_VERIFY_WITH_TIMEOUT(section->property("sectionChecked").toBool(), TestTimeout::shortMs());
}

void VehicleConfigUITestBase::verifyAPMCalIndicators(bool compassGreen, bool accelGreen, const char* context)
{
    struct Check
    {
        const char* name;
        bool expectedGreen;
    };

    const Check checks[] = {
        {"sensorsSetup_calibrateCompass", compassGreen},
        {"sensorsSetup_calibrateAccel", accelGreen},
    };

    for (const Check& c : checks) {
        selectAPMSensorSection(QString::fromLatin1(c.name).contains(QStringLiteral("Compass"))
                                   ? QStringLiteral("Compass")
                                   : QStringLiteral("Accelerometer"));
        if (QTest::currentTestFailed())
            return;
        QPointer<QQuickItem> btn = findVisibleItem(_rootItem, QLatin1String(c.name), 3000);
        QVERIFY2(
            btn,
            qPrintable(QStringLiteral("Button not found (%1): %2").arg(QLatin1String(context), QLatin1String(c.name))));
        (void) QTest::qWaitFor([&] { return btn && (btn->property("indicatorGreen").toBool() == c.expectedGreen); },
                               3000);
        QVERIFY2(btn, qPrintable(QStringLiteral("Button destroyed while waiting (%1): %2")
                                     .arg(QLatin1String(context), QLatin1String(c.name))));
        QVERIFY2(btn->property("indicatorGreen").toBool() == c.expectedGreen,
                 qPrintable(QStringLiteral("indicatorGreen is %1, expected %2 (%3): %4")
                                .arg(btn->property("indicatorGreen").toBool())
                                .arg(c.expectedGreen)
                                .arg(QLatin1String(context), QLatin1String(c.name))));
    }
}

void VehicleConfigUITestBase::runAPMFullAccelCal()
{
    selectAPMSensorSection(QStringLiteral("Accelerometer"));
    if (QTest::currentTestFailed())
        return;
    QVERIFY2(clickButtonScrolled(QStringLiteral("sensorsSetup_calibrateAccel"),
                                 QStringLiteral("sensorsSetup_buttonScroller")),
             "Failed to click Accelerometer button");
    if (QGCCorePlugin::instance()->directMissionItemEditing()) {
        QVERIFY(!findVisibleItem(_rootItem, QStringLiteral("sensorsSetup_inlinePreparation"), 0));
        QVERIFY(!findVisibleItem(_rootItem, QStringLiteral("sensorsSetup_boardOrientation"), 0));
        QVERIFY(!findVisibleItem(_rootItem, QStringLiteral("popupDialog_frame"), 0));
    } else {
        QVERIFY(clickButtonScrolled(QStringLiteral("sensorsSetup_confirmOrientationCalibration"),
                                    QStringLiteral("sensorsSetup_buttonScroller")));
    }

    // MockLink will now drive the 6-pose ACCELCAL_VEHICLE_POS handshake.
    // For each pose the orientation cal area becomes visible with the relevant
    // side InProgress; the user (test) clicks Next to advance.

    // Poses in the order MockLink sends them:
    // LEVEL→downSide, LEFT→leftSide, RIGHT→rightSide,
    // NOSEDOWN→noseDownSide, NOSEUP→tailDownSide, BACK→upsideDownSide
    struct PoseInfo
    {
        const char* sideObjectName;
    };

    static constexpr PoseInfo kPoses[] = {
        {"sensorsCal_downSide"},     {"sensorsCal_leftSide"},     {"sensorsCal_rightSide"},
        {"sensorsCal_noseDownSide"}, {"sensorsCal_tailDownSide"}, {"sensorsCal_upsideDownSide"},
    };

    for (const PoseInfo& pose : kPoses) {
        // Wait for the orientation cal area to show (controller.showOrientationCalArea = true)
        // and this side to be InProgress (calState == VehicleRotationCal.CalState.InProgress == 2)
        QPointer<QQuickItem> sideItem = findVisibleItem(_rootItem, QLatin1String(pose.sideObjectName), 10000);
        QVERIFY2(sideItem,
                 qPrintable(QStringLiteral("Side indicator not visible: %1").arg(QLatin1String(pose.sideObjectName))));
        if (QGCCorePlugin::instance()->directMissionItemEditing()) {
            QQuickItem* workflow = findVisibleItem(_rootItem, QStringLiteral("sensorsSetup_inlineWorkflow"));
            QVERIFY(workflow);
            QVERIFY(workflow->isAncestorOf(sideItem));
            QVERIFY(!findVisibleItem(_rootItem, QStringLiteral("popupDialog_frame"), 0));
        }

        // calState InProgress == 2 (VehicleRotationCal.CalState enum)
        QVERIFY2(QTest::qWaitFor([&] { return sideItem && (sideItem->property("calState").toInt() == 2); }, 10000),
                 qPrintable(QStringLiteral("Side never went InProgress: %1").arg(QLatin1String(pose.sideObjectName))));

        // Click Next — sends COMMAND_ACK back to MockLink to advance to next pose.
        // The controller enables the Next button when it receives the ACCELCAL_VEHICLE_POS message,
        // which happens on the same tick as the side going InProgress, so wait briefly for it.
        QPointer<QQuickItem> nextBtn = findVisibleItem(_rootItem, QStringLiteral("sensorsSetup_nextButton"), 3000);
        QVERIFY2(nextBtn, "Next button not found");
        QVERIFY2(
            QTest::qWaitFor([&] { return nextBtn && nextBtn->property("enabled").toBool(); }, 3000),
            qPrintable(QStringLiteral("Next button not enabled for side: %1").arg(QLatin1String(pose.sideObjectName))));
        if (&pose == &kPoses[0]) {
            const QString previewDirectory = qEnvironmentVariable("CAAC_SENSOR_PREVIEW_DIR");
            if (!previewDirectory.isEmpty()) {
                QVERIFY(QDir().mkpath(previewDirectory));
                QQuickItem* scroller = findVisibleItem(_rootItem, QStringLiteral("sensorsSetup_buttonScroller"));
                QVERIFY(scroller);
                scroller->setProperty("contentY", 0.0);
                QSignalSpy rendered(_window, &QQuickWindow::frameSwapped);
                _window->update();
                QTRY_VERIFY_WITH_TIMEOUT(!rendered.isEmpty(), TestTimeout::mediumMs());
                QVERIFY(_window->grabWindow().save(
                    QDir(previewDirectory).filePath(QStringLiteral("sensors-03-calibrating.png"))));
                QQuickItem* cancelBtn = findVisibleItem(_rootItem, QStringLiteral("sensorsSetup_cancelButton"));
                QVERIFY(cancelBtn);
                QVERIFY(scrollIntoView(cancelBtn, QStringLiteral("sensorsSetup_buttonScroller")));
                rendered.clear();
                _window->update();
                QTRY_VERIFY_WITH_TIMEOUT(!rendered.isEmpty(), TestTimeout::mediumMs());
                QVERIFY(_window->grabWindow().save(
                    QDir(previewDirectory).filePath(QStringLiteral("sensors-04-controls.png"))));
                scroller->setProperty("contentY",
                                      qMax(0.0, scroller->property("contentHeight").toDouble() - scroller->height()));
                rendered.clear();
                _window->update();
                QTRY_VERIFY_WITH_TIMEOUT(!rendered.isEmpty(), TestTimeout::mediumMs());
                QVERIFY(_window->grabWindow().save(
                    QDir(previewDirectory).filePath(QStringLiteral("sensors-05-poses.png"))));
            }
        }
        QVERIFY2(clickButtonScrolled(QStringLiteral("sensorsSetup_nextButton"),
                                     QStringLiteral("sensorsSetup_buttonScroller")),
                 "Failed to click Next button");
    }

    // After the last pose MockLink sends ACCELCAL_VEHICLE_POS_SUCCESS → _stopCalibration(Success)
    // Progress bar goes to 1.0 and the cal area closes
    QPointer<QQuickItem> progressBar = findVisibleItem(_rootItem, QStringLiteral("sensorsSetup_progressBar"), 2000);
    QVERIFY2(progressBar, "Progress bar not found during accel cal");
    QVERIFY2(QTest::qWaitFor(
                 [&] { return progressBar && qFuzzyCompare(progressBar->property("value").toDouble(), 1.0); }, 10000),
             "Progress bar never reached 1.0 after accel cal success");

    // Accel cal completion must show the generic post-calibration dialog
    // (postCalibrationDialog), NOT the compass results dialog with fitness bars
    // (postOnboardCompassCalibrationDialog).  Dismiss it so the sensors page is
    // unblocked before callers proceed.
    const bool inlineCalibration = QGCCorePlugin::instance()->directMissionItemEditing();
    const auto resultName =
        inlineCalibration ? QStringLiteral("sensorsSetup_calibrationResult") : QStringLiteral("postCalibrationDialog");
    QVERIFY2(findVisibleItem(_rootItem, resultName, 5000), "Post-accel-cal result not shown");
    QVERIFY2(!_rootItem->findChild<QQuickItem*>(QStringLiteral("postOnboardCompassCalibrationDialog")),
             "Compass results dialog incorrectly shown after accel cal");
    Vehicle* const vehicle = MultiVehicleManager::instance()->activeVehicle();
    QVERIFY(vehicle);
    QVERIFY2(vehicle->rebootRequired(), "Accel calibration did not latch rebootRequired");
    if (inlineCalibration) {
        QVERIFY(!findVisibleItem(_rootItem, QStringLiteral("popupDialog_acceptButton"), 0));
        QVERIFY(clickButtonScrolled(QStringLiteral("sensorsSetup_acknowledgeCalibration"),
                                    QStringLiteral("sensorsSetup_buttonScroller")));
    } else {
        QVERIFY2(clickButton(QStringLiteral("popupDialog_acceptButton")), "Failed to dismiss post-accel-cal dialog");
    }
}

void VehicleConfigUITestBase::runAPMCompassCal()
{
    selectAPMSensorSection(QStringLiteral("Compass"));
    if (QTest::currentTestFailed())
        return;
    QVERIFY2(clickButtonScrolled(QStringLiteral("sensorsSetup_calibrateCompass"),
                                 QStringLiteral("sensorsSetup_buttonScroller")),
             "Failed to click Compass button");
    if (QGCCorePlugin::instance()->directMissionItemEditing()) {
        QVERIFY(!findVisibleItem(_rootItem, QStringLiteral("sensorsSetup_inlinePreparation"), 0));
        QVERIFY(!findVisibleItem(_rootItem, QStringLiteral("popupDialog_frame"), 0));
    } else {
        QVERIFY(clickButtonScrolled(QStringLiteral("sensorsSetup_confirmOrientationCalibration"),
                                    QStringLiteral("sensorsSetup_buttonScroller")));
    }

    // MockLink sends MAG_CAL_PROGRESS 0→100 at ~10Hz
    QPointer<QQuickItem> progressBar = findVisibleItem(_rootItem, QStringLiteral("sensorsSetup_progressBar"), 2000);
    QVERIFY2(progressBar, "Progress bar not found during compass cal");

    // Wait for progress > 0 (calibration is running)
    QVERIFY2(QTest::qWaitFor([&] { return progressBar && (progressBar->property("value").toDouble() > 0.0); }, 5000),
             "Compass cal progress never advanced above 0");

    // Wait for completion: MAG_CAL_REPORT → StopCalibrationSuccessShowLog → dialog opens.
    // Note: StopCalibrationSuccessShowLog resets progress to 0 (not 1.0), so wait for
    // the post-cal dialog directly rather than waiting for progress == 1.0.
    const bool inlineCalibration = QGCCorePlugin::instance()->directMissionItemEditing();
    const auto resultName = inlineCalibration ? QStringLiteral("sensorsSetup_compassCalibrationResult")
                                              : QStringLiteral("postOnboardCompassCalibrationDialog");
    QVERIFY2(findVisibleItem(_rootItem, resultName, 25000), "Post-compass-cal result not shown");
    Vehicle* const vehicle = MultiVehicleManager::instance()->activeVehicle();
    QVERIFY(vehicle);
    QVERIFY2(vehicle->rebootRequired(), "Compass calibration did not latch rebootRequired");
    if (inlineCalibration) {
        QVERIFY(!findVisibleItem(_rootItem, QStringLiteral("popupDialog_acceptButton"), 0));
        QVERIFY(clickButtonScrolled(QStringLiteral("sensorsSetup_acknowledgeCalibration"),
                                    QStringLiteral("sensorsSetup_buttonScroller")));
    } else {
        QVERIFY2(clickButton(QStringLiteral("popupDialog_acceptButton")), "Failed to dismiss post-compass-cal dialog");
    }
}

void VehicleConfigUITestBase::verifyVehicleSetupMenuOrder(Vehicle* vehicle)
{
    QVERIFY(vehicle);
    if (findVisibleItem(_rootItem, QStringLiteral("vehicleConfig_backToMenu"), 0)) {
        QVERIFY(clickButton(QStringLiteral("vehicleConfig_backToMenu")));
    }
    QQuickItem* config = findVisibleItem(_rootItem, QStringLiteral("vehicleConfig_root"));
    QVERIFY(config);
    const QVariantList components = vehicle->autopilotPlugin()->vehicleComponents();
    QList<QPair<qreal, VehicleComponent*>> renderedComponents;
    QList<VehicleComponent*> expectedComponents;
    for (const QVariant& entry : components) {
        auto* component = entry.value<VehicleComponent*>();
        QVERIFY(component);
        const QString name = QStringLiteral("vehicleConfig_comp_") + QString(component->name()).remove(' ');
        if (QQuickItem* button = findVisibleItem(config, name, 0)) {
            for (QQuickItem* ancestor = button; ancestor; ancestor = ancestor->parentItem()) {
                ancestor->ensurePolished();
            }
            renderedComponents.append({button->mapToItem(config, QPointF()).y(), component});
        }
    }
    std::sort(renderedComponents.begin(), renderedComponents.end(),
              [](const auto& left, const auto& right) { return left.first < right.first; });
    const auto appendComponent = [&](const char* className) {
        for (const auto& entry : renderedComponents) {
            if (entry.second->inherits(className)) {
                expectedComponents.append(entry.second);
            }
        }
    };
    appendComponent("AirframeComponent");
    appendComponent("APMAirframeComponent");
    const auto appendKnownComponent = [&](AutoPilotPlugin::KnownVehicleComponent type) {
        if (auto* component = vehicle->autopilotPlugin()->findKnownVehicleComponent(type)) {
            expectedComponents.append(component);
        }
    };
    appendKnownComponent(AutoPilotPlugin::KnownSensorsVehicleComponent);
    appendKnownComponent(AutoPilotPlugin::KnownRadioVehicleComponent);
    appendKnownComponent(AutoPilotPlugin::KnownESCVehicleComponent);
    appendKnownComponent(AutoPilotPlugin::KnownFlightModesVehicleComponent);
    appendComponent("APMServoComponent");
    appendComponent("ActuatorComponent");
    appendComponent("MotorComponent");
    appendKnownComponent(AutoPilotPlugin::KnownPowerVehicleComponent);
    appendComponent("SafetyComponent");
    QVERIFY(expectedComponents.size() >= 5);
    QVERIFY(renderedComponents.size() >= expectedComponents.size());
    for (int index = 0; index < expectedComponents.size(); ++index) {
        QCOMPARE(renderedComponents[index].second->name(), expectedComponents[index]->name());
    }
}

void VehicleConfigUITestBase::verifyNarrowParameterEditor(Vehicle* vehicle, const QString& parameterName)
{
    QVERIFY(vehicle);
    const QString previewDirectory = qEnvironmentVariable("CAAC_PARAMETER_PREVIEW_DIR");
    Fact* language = SettingsManager::instance()->appSettings()->qLocaleLanguage();
    const QVariant savedLanguage = language->rawValue();
    const auto restoreLanguage = qScopeGuard([&] {
        if (!previewDirectory.isEmpty() && language->rawValue() != savedLanguage) {
            qgcApp()->resetRebootMessageDebounce();
            expectAppMessage(QRegularExpression(
                QRegularExpression::escape(Fact::tr("Restart application for changes to take effect."))));
            language->setRawValue(savedLanguage);
            (void) acceptDialog(TestTimeout::mediumMs());
            verifyExpectedLogMessage();
        }
    });
    if (!previewDirectory.isEmpty()) {
        QVERIFY(QDir().mkpath(previewDirectory));
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
            QVERIFY(acceptDialog(TestTimeout::mediumMs()));
            verifyExpectedLogMessage();
        }
    }
    const auto capture = [&](const QString& name) {
        if (previewDirectory.isEmpty()) {
            return true;
        }
        QSignalSpy frame(_window, &QQuickWindow::frameSwapped);
        _window->update();
        return waitForCondition([&] { return !frame.isEmpty(); }, TestTimeout::mediumMs(), name) &&
               _window->grabWindow().save(QDir(previewDirectory).filePath(parameterName + "-" + name + ".png"));
    };
    const QSize savedSize = _window->size();
    const auto restoreSize = qScopeGuard([this, savedSize] { _window->resize(savedSize); });
    const QMargins frame = _window->frameMargins();
    const int availableHeight =
        _window->screen() ? _window->screen()->availableGeometry().height() - frame.top() - frame.bottom() : 800;
    _window->resize(1000, qMin(previewDirectory.isEmpty() ? 800 : 1000, availableHeight));
    QTRY_COMPARE_WITH_TIMEOUT(_window->width(), 1000, TestTimeout::shortMs());
    QVERIFY(clickSidebarButton(QStringLiteral("vehicleConfig_parametersButton")));
    QQuickItem* editor = findVisibleItem(_rootItem, QStringLiteral("parameterEditor_root"));
    QVERIFY(editor);
    QTRY_VERIFY_WITH_TIMEOUT(editor->width() > 0 && editor->width() < _window->width() / 2, TestTimeout::shortMs());
    auto* controller = qobject_cast<ParameterEditorController*>(editor->property("_controller").value<QObject*>());
    QVERIFY(controller);
    QQuickItem* category = findVisibleItem(editor, QStringLiteral("parameterEditor_categorySelector"));
    QQuickItem* group = findVisibleItem(editor, QStringLiteral("parameterEditor_groupSelector"));
    QQuickItem* table = findVisibleItem(editor, QStringLiteral("parameterEditor_table"));
    QVERIFY(category);
    QVERIFY(group);
    QVERIFY(table);
    QVERIFY(!findVisibleItem(editor, QStringLiteral("parameterEditor_categoryTree"), 0));
    for (QQuickItem* control : {category, group, table}) {
        const QRectF bounds = control->mapRectToItem(editor, control->boundingRect());
        QVERIFY(bounds.left() >= -1 && bounds.right() <= editor->width() + 1);
    }
    QTRY_COMPARE_WITH_TIMEOUT(category->property("currentText").toString(),
                              controller->currentCategory()->property("name").toString(), TestTimeout::shortMs());
    if (!previewDirectory.isEmpty()) {
        auto* fullListTab = findVisibleItem(editor, QStringLiteral("parameterEditor_fullListTab"));
        QVERIFY(fullListTab);
        QVERIFY(QRegularExpression(QStringLiteral("[\\x{4e00}-\\x{9fff}]"))
                    .match(fullListTab->property("text").toString())
                    .hasMatch());
    }
    QQuickItem* tableFrame = findVisibleItem(editor, QStringLiteral("parameterEditor_tableFrame"));
    QVERIFY(tableFrame);
    QCOMPARE(tableFrame->width(), table->width());
    for (int column = 0; column < 3; ++column) {
        auto* cell = findVisibleItem(table, QStringLiteral("parameterEditor_cell_0_%1").arg(column));
        QVERIFY(cell);
        const QRectF bounds = cell->mapRectToItem(table, cell->boundingRect());
        QVERIFY(bounds.left() >= -1 && bounds.right() <= table->width() + 1);
    }
    QVERIFY(capture(QStringLiteral("01-overview")));
    QVERIFY(clickButton(QStringLiteral("parameterEditor_groupSelector")));
    auto* groupPopup = group->property("popup").value<QObject*>();
    QVERIFY(groupPopup);
    QTRY_VERIFY_WITH_TIMEOUT(groupPopup->property("opened").toBool(), TestTimeout::shortMs());
    QTest::keyClick(_window, Qt::Key_Home);
    if (group->property("count").toInt() > 1) {
        QTest::keyClick(_window, Qt::Key_Down);
    }
    QTest::keyClick(_window, Qt::Key_Return);
    QTRY_COMPARE_WITH_TIMEOUT(group->property("currentText").toString(),
                              controller->currentGroup()->property("name").toString(), TestTimeout::shortMs());
    QTRY_VERIFY_WITH_TIMEOUT(!groupPopup->property("visible").toBool(), TestTimeout::shortMs());
    QVERIFY(clickButton(QStringLiteral("parameterEditor_search")));
    auto* searchField = findVisibleItem(editor, QStringLiteral("parameterEditor_search"));
    QVERIFY(searchField);
    QTRY_VERIFY_WITH_TIMEOUT(searchField->hasActiveFocus(), TestTimeout::shortMs());
    const QString searchText = QStringLiteral("^") + QRegularExpression::escape(parameterName) + QStringLiteral("$");
    for (const QChar& character : searchText) {
        QTest::keyClick(_window, character.toLatin1());
    }
    const auto parameterModel = [&] {
        return qobject_cast<ParameterTableModel*>(controller->property("parameters").value<QObject*>());
    };
    QTRY_VERIFY_WITH_TIMEOUT(parameterModel() && parameterModel()->rowCount() == 1, TestTimeout::mediumMs());
    QTRY_VERIFY_WITH_TIMEOUT(parameterModel()->factAt(0) && parameterModel()->factAt(0)->name() == parameterName,
                             TestTimeout::mediumMs());
    Fact* fact = parameterModel()->factAt(0);
    QVERIFY(fact);
    QCOMPARE(fact->name(), parameterName);
    QVERIFY(!fact->readOnly());
    QVERIFY(fact->enumStrings().isEmpty());
    const QVariant savedValue = fact->rawValue();
    const bool wasFavorite = controller->isFavorite(parameterName);
    const auto restoreParameter = qScopeGuard([fact, savedValue, controller, parameterName, wasFavorite] {
        if (fact->rawValue() != savedValue) {
            QSignalSpy restored(fact, &Fact::vehicleUpdated);
            fact->setRawValue(savedValue);
            QTRY_VERIFY_WITH_TIMEOUT(restored.count() > 0, TestTimeout::mediumMs());
            QCOMPARE(fact->rawValue(), savedValue);
        }
        if (controller->isFavorite(parameterName) != wasFavorite) {
            controller->toggleFavorite(parameterName);
        }
    });
    if (wasFavorite) {
        controller->toggleFavorite(parameterName);
    }
    QVERIFY(clickButton(QStringLiteral("parameterEditor_favorite_") + parameterName));
    QVERIFY(controller->isFavorite(parameterName));
    QQuickItem* scrollBar = findVisibleItem(editor, QStringLiteral("parameterEditor_horizontalScrollBar"));
    QVERIFY(scrollBar);
    QTRY_VERIFY_WITH_TIMEOUT(table->property("contentWidth").toReal() > table->width(), TestTimeout::shortMs());
    const qreal thumbSize = scrollBar->property("size").toReal();
    const QPoint press =
        scrollBar->mapToScene(QPointF(scrollBar->width() * thumbSize / 2, scrollBar->height() / 2)).toPoint();
    const QPoint release = scrollBar->mapToScene(QPointF(scrollBar->width() / 2, scrollBar->height() / 2)).toPoint();
    QTest::mousePress(_window, Qt::LeftButton, Qt::NoModifier, press);
    QTest::mouseMove(_window, release);
    QTest::mouseRelease(_window, Qt::LeftButton, Qt::NoModifier, release);
    QTRY_VERIFY_WITH_TIMEOUT(table->property("contentX").toReal() > 0, TestTimeout::shortMs());
    const QString editName = QStringLiteral("parameterEditor_edit_") + parameterName + QStringLiteral("_2");
    QQuickItem* valueCell = nullptr;
    QTRY_VERIFY_WITH_TIMEOUT((valueCell = findVisibleItem(table, editName, 0)) != nullptr, TestTimeout::shortMs());
    const qreal cellX = valueCell->mapToItem(table, QPointF()).x() + table->property("contentX").toReal();
    QVERIFY(table->setProperty("contentX", cellX));
    QTRY_VERIFY_WITH_TIMEOUT(valueCell->mapToItem(table, QPointF()).x() >= -1 &&
                                 valueCell->mapToItem(table, QPointF(valueCell->width(), 0)).x() <= table->width() + 1,
                             TestTimeout::shortMs());
    QQuickItem* valueText =
        findVisibleItem(table, QStringLiteral("parameterEditor_valueText_") + parameterName + QStringLiteral("_2"));
    QVERIFY(valueText);
    QVERIFY(valueText->property("text").toString().contains(fact->cookedValueString()));
    QVERIFY(valueText->property("text").toString().contains(fact->cookedUnits()));
    QVERIFY(!valueText->property("truncated").toBool());
    QVERIFY(capture(QStringLiteral("02-value-and-units")));
    QVERIFY(clickButton(editName));
    QVERIFY(waitForDialog(parameterName));
    QVERIFY(capture(QStringLiteral("03-editor")));
    QQuickItem* valueField = findVisibleItem(_rootItem, QStringLiteral("parameterEditor_valueField"));
    QVERIFY(valueField);
    double newValue = fact->cookedValue().toDouble() + 1;
    if (!fact->validate(QString::number(newValue, 'g', 12), false).isEmpty()) {
        newValue = fact->cookedValue().toDouble() + 0.01;
    }
    const QString newText = QString::number(newValue, 'g', 12);
    QVERIFY(fact->validate(newText, false).isEmpty());
    QVERIFY(clickButton(QStringLiteral("parameterEditor_valueField")));
    QTest::keyClick(_window, Qt::Key_A, Qt::ControlModifier);
    for (const QChar& character : newText) {
        QTest::keyClick(_window, character.toLatin1());
    }
    QSignalSpy written(fact, &Fact::vehicleUpdated);
    QVERIFY(acceptDialog());
    QTRY_VERIFY_WITH_TIMEOUT(written.count() > 0, TestTimeout::mediumMs());
    QTRY_VERIFY_WITH_TIMEOUT(std::abs(fact->cookedValue().toDouble() - newValue) < 0.00001, TestTimeout::shortMs());
    QVERIFY(clickButton(QStringLiteral("parameterEditor_modifiedTab")));
    QTRY_VERIFY_WITH_TIMEOUT(parameterModel() && parameterModel()->rowCount() == 1, TestTimeout::mediumMs());
    QCOMPARE(parameterModel()->factAt(0), fact);
    QVERIFY(clickButton(QStringLiteral("parameterEditor_favoritesTab")));
    QTRY_VERIFY_WITH_TIMEOUT(parameterModel() && parameterModel()->rowCount() == 1, TestTimeout::mediumMs());
    QCOMPARE(parameterModel()->factAt(0), fact);
    QVERIFY(clickButton(QStringLiteral("parameterEditor_fullListTab")));
    QVERIFY(clickButton(QStringLiteral("parameterEditor_clearSearch")));
    QTRY_VERIFY_WITH_TIMEOUT(findVisibleItem(editor, QStringLiteral("parameterEditor_groupSelector"), 0),
                             TestTimeout::mediumMs());
}
