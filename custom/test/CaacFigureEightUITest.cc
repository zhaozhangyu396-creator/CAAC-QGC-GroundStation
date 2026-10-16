#include <cmath>

#include <QtCore/QJsonDocument>
#include <QtCore/QRectF>
#include <QtGui/QImage>
#include <QtQuick/QQuickItem>
#include <QtQuick/QQuickWindow>
#include <QtTest/QSignalSpy>

#include "CaacCorePlugin.h"
#include "CaacFigureEightController.h"
#include "CaacFigureEightFenceController.h"
#include "MissionController.h"
#include "MultiVehicleManager.h"
#include "PlanMasterController.h"
#include "QGCGeo.h"
#include "QmlUITestBase.h"

class CaacFigureEightUITest : public QmlUITestBase
{
    Q_OBJECT

private slots:
    void _independentTrainingDialog();

private:
    bool _clickOutsideDialog();
};

bool CaacFigureEightUITest::_clickOutsideDialog()
{
    auto* frame = findVisibleItem(_rootItem, QStringLiteral("popupDialog_frame"));
    if (!frame) {
        return false;
    }
    const auto margin = frame->property("spacing").toReal();
    const auto outerFrame = frame->mapRectToScene(frame->boundingRect().adjusted(-margin, -margin, margin, margin));
    const QRectF windowBounds(0, 0, _window->width() - 1, _window->height() - 1);
    const QPoint outsideFrame(qRound(outerFrame.left() / 2),
                              qRound(qBound(windowBounds.top(), outerFrame.center().y(), windowBounds.bottom())));
    if (outerFrame.left() <= 2 || !windowBounds.contains(outsideFrame) || outerFrame.contains(outsideFrame)) {
        return false;
    }
    QTest::mouseClick(_window, Qt::LeftButton, Qt::NoModifier, outsideFrame);
    return true;
}

void CaacFigureEightUITest::_independentTrainingDialog()
{
    startUI();
    if (QTest::currentTestFailed()) {
        return;
    }
    QVERIFY(!MultiVehicleManager::instance()->activeVehicle());
    auto* plugin = qobject_cast<CaacCorePlugin*>(QGCCorePlugin::instance());
    QVERIFY(plugin && plugin->figureEight());
    auto* controller = plugin->figureEight();
    auto* fence = controller->fence();
    QVERIFY(fence);
    const auto normalTools = {"caacMissionDetailsButton", "caacFlightDataButton",      "caacMapSettingsButton",
                              "caacCalculatorButton",     "caacClearTrajectoryButton", "caacPlanningPanelContents",
                              "caacPlanningPanelToggle"};
    const auto eightTools = {"caacEightSiteListButton", "caacEightPreferencesButton",     "caacEightStandardsButton",
                             "caacEightFenceButton",    "caacEightClearTrajectoryButton", "caacEightExamControl",
                             "caacEightExamTimer",      "caacEightStartExamButton"};
    const auto workspaceToolsMatch = [&](bool training) {
        for (const auto* objectName : normalTools) {
            if (!!findVisibleItem(_rootItem, QString::fromLatin1(objectName), 0) == training) {
                return false;
            }
        }
        for (const auto* objectName : eightTools) {
            if (!!findVisibleItem(_rootItem, QString::fromLatin1(objectName), 0) != training) {
                return false;
            }
        }
        return true;
    };

    QVERIFY(QMetaObject::invokeMethod(_window, "showPlanView"));
    auto* plan = findVisibleItem(_rootItem, QStringLiteral("mainView_plan"), TestTimeout::mediumMs());
    QVERIFY(plan);
    auto* master = plan->property("_planMasterController").value<PlanMasterController*>();
    QVERIFY(master);
    auto* mission = master->missionController();
    const QGeoCoordinate home(30, 120, 0);
    mission->setHomePosition(home);
    QVERIFY(mission->insertSimpleMissionItem(QGCGeo::geodesicDestination(home, 90, 150), 1, false));
    QTRY_COMPARE_WITH_TIMEOUT(mission->plannedHomePosition().altitude(), 0.0, TestTimeout::mediumMs());
    const auto routeBefore = master->saveToJson();

    QVERIFY(QMetaObject::invokeMethod(_window, "showFlyView"));
    auto* routeButton = findVisibleItem(_rootItem, QStringLiteral("caacFlightSettingsButton"));
    auto* eightButton = findVisibleItem(_rootItem, QStringLiteral("caacFigureEightButton"));
    QVERIFY(routeButton && eightButton);
    QVERIFY(eightButton->mapToScene(QPointF()).x() > routeButton->mapToScene(QPointF()).x());
    QVERIFY(clickButton(QStringLiteral("caacFigureEightButton")));
    QVERIFY(controller->viewActive());
    QTRY_VERIFY_WITH_TIMEOUT(workspaceToolsMatch(true), TestTimeout::mediumMs());
    QVERIFY(!findVisibleItem(_rootItem, QStringLiteral("caacFigureEightPage"), 0));
    auto* examTimer = findVisibleItem(_rootItem, QStringLiteral("caacEightExamTimer"));
    QVERIFY(examTimer);
    QCOMPARE(examTimer->property("text").toString(), QStringLiteral("00:00"));
    QVERIFY(clickButton(QStringLiteral("caacEightStartExamButton")));
    QVERIFY(!controller->running());
    QVERIFY(!controller->lastError().isEmpty());
    auto* monitorPage = findVisibleItem(_rootItem, QStringLiteral("caacFigureEightPage"));
    QVERIFY(monitorPage);
    QCOMPARE(monitorPage->property("currentIndex").toInt(), 3);
    auto* startError = findVisibleItem(_rootItem, QStringLiteral("caacFigureEightStartError"));
    QVERIFY(startError);
    QCOMPARE(startError->property("text").toString(), controller->lastError());
    QCOMPARE(master->saveToJson(), routeBefore);
    QVERIFY(!MultiVehicleManager::instance()->activeVehicle());
    QVERIFY(rejectDialog());
    QVERIFY(clickButton(QStringLiteral("caacEightExamTimer")));
    monitorPage = findVisibleItem(_rootItem, QStringLiteral("caacFigureEightPage"));
    QVERIFY(monitorPage);
    QCOMPARE(monitorPage->property("currentIndex").toInt(), 3);
    QVERIFY(rejectDialog());
    QVERIFY(clickButton(QStringLiteral("caacEightSiteListButton")));
    auto* pages = findVisibleItem(_rootItem, QStringLiteral("caacFigureEightPage"));
    QVERIFY(pages);
    QVERIFY(findVisibleItem(_rootItem, QStringLiteral("caacFigureEightStatus")));
    QVERIFY(!findVisibleItem(_rootItem, QStringLiteral("caacFlightSettingsPanel"), 0));
    QCOMPARE(master->saveToJson(), routeBefore);

    const auto scroller = QStringLiteral("popupDialog_contentFlickable");
    const auto toolsScroller = QStringLiteral("caacEightToolsScroller");
    for (const auto* objectName : {"caacFigureEightALatitude", "caacFigureEightALongitude", "caacFigureEightBLatitude",
                                   "caacFigureEightBLongitude", "caacFigureEightStandardSite"}) {
        QVERIFY(!findItem(_rootItem, QString::fromLatin1(objectName)));
    }
    auto* captureA = findVisibleItemScrolled(QStringLiteral("caacFigureEightCaptureA"), scroller);
    auto* captureB = findVisibleItemScrolled(QStringLiteral("caacFigureEightCaptureB"), scroller);
    QVERIFY(captureA && captureB);
    QVERIFY(!captureA->isEnabled());
    QVERIFY(!captureB->isEnabled());
    auto* calibrationStatus = findItem(_rootItem, QStringLiteral("caacFigureEightCalibrationStatus"));
    QVERIFY(calibrationStatus);
    QVERIFY(!calibrationStatus->isVisible());

    // A disconnected UI cannot capture RTK positions; use a known pair to exercise the generated display.
    QVERIFY(controller->setPoints(home, QGCGeo::geodesicDestination(home, 90, 24)));
    controller->setShowOnMap(true);
    controller->setFenceWidth(1.2);
    fence->setMargin(3);
    fence->setShowOnMap(true);
    QTRY_VERIFY_WITH_TIMEOUT(!controller->referencePath().isEmpty(), TestTimeout::mediumMs());
    QVERIFY(QGCGeo::geodesicDistance(controller->pointA(), home) < 0.01);
    QVERIFY(std::abs(QGCGeo::geodesicDistance(controller->pointA(), controller->pointB()) - 24) < 0.01);
    auto* reference =
        findVisibleItem(_rootItem, QStringLiteral("caacFigureEightReferencePath"), TestTimeout::mediumMs());
    QVERIFY(reference);
    QTRY_VERIFY_WITH_TIMEOUT(reference->property("visible").toBool(), TestTimeout::mediumMs());
    QTRY_COMPARE_WITH_TIMEOUT(reference->property("path").value<QList<QGeoCoordinate>>().size(),
                              controller->referencePath().size(), TestTimeout::mediumMs());
    auto* outerFence = findVisibleItem(_rootItem, QStringLiteral("caacFigureEightOuterFence"));
    QVERIFY(outerFence);
    QVERIFY(fence->path().size() > 3);
    const auto outerPath = outerFence->property("path").value<QList<QGeoCoordinate>>();
    QVERIFY(outerPath.size() >= fence->path().size());
    QCOMPARE(outerPath.first(), outerPath.last());
    controller->setShowOnMap(false);
    QTRY_VERIFY_WITH_TIMEOUT(!reference->property("visible").toBool(), TestTimeout::mediumMs());
    QVERIFY(outerFence->property("visible").toBool());
    fence->setShowOnMap(false);
    QTRY_VERIFY_WITH_TIMEOUT(!outerFence->property("visible").toBool(), TestTimeout::mediumMs());
    controller->setShowOnMap(true);
    QTRY_VERIFY_WITH_TIMEOUT(reference->property("visible").toBool(), TestTimeout::mediumMs());
    QVERIFY(!outerFence->property("visible").toBool());
    fence->setShowOnMap(true);
    QTRY_VERIFY_WITH_TIMEOUT(outerFence->property("visible").toBool(), TestTimeout::mediumMs());
    const auto pathBeforeClose = controller->referencePath();
    QList<QGeoCoordinate> markerCoordinates;
    int markerIndex = 0;
    for (const auto pathIndex : {0, 30, 60, 90, 150, 180, 210}) {
        auto* marker = findVisibleItem(_rootItem, QStringLiteral("caacFigureEightTrainingMarker%1").arg(markerIndex++));
        QVERIFY(marker);
        const auto coordinate = marker->property("coordinate").value<QGeoCoordinate>();
        QVERIFY(coordinate.isValid());
        QCOMPARE(coordinate, pathBeforeClose.at(pathIndex).value<QGeoCoordinate>());
        QVERIFY(!markerCoordinates.contains(coordinate));
        markerCoordinates.append(coordinate);
    }
    QVERIFY(!findItem(_rootItem, QStringLiteral("caacFigureEightTrainingMarker7")));
    QVERIFY(!findItem(_rootItem, QStringLiteral("caacFigureEightMarkerA")));
    QVERIFY(!findItem(_rootItem, QStringLiteral("caacFigureEightMarkerB")));

    for (int category = 0; category < 3; ++category) {
        QVERIFY(pages->setProperty("currentIndex", 1));
        auto* standard = findVisibleItemScrolled(QStringLiteral("caacFigureEightStandard"), scroller);
        QVERIFY(standard);
        QCOMPARE(standard->property("count").toInt(), 4);
        QVERIFY(standard->setProperty("currentIndex", category));
        QVERIFY(QMetaObject::invokeMethod(standard, "activated", Q_ARG(int, category)));
        QTRY_COMPARE_WITH_TIMEOUT(controller->standard(), category, TestTimeout::mediumMs());
        QVERIFY(pages->setProperty("currentIndex", 2));
        QVERIFY(clickButtonScrolled(QStringLiteral("caacFigureEightResetLimits"), scroller));
        auto* heading = findVisibleItemScrolled(QStringLiteral("caacFigureEightLimit_heading"), scroller);
        QVERIFY(heading);
        QTRY_COMPARE_WITH_TIMEOUT(heading->property("text").toDouble(), category == 2 ? 25.0 : 30.0,
                                  TestTimeout::mediumMs());
    }
    auto* horizontal = findVisibleItemScrolled(QStringLiteral("caacFigureEightLimit_horizontal"), scroller);
    QVERIFY(horizontal);
    QVERIFY(horizontal->setProperty("text", QStringLiteral("2.5")));
    QVERIFY(QMetaObject::invokeMethod(horizontal, "textEdited"));
    QVERIFY(clickButtonScrolled(QStringLiteral("caacFigureEightSaveLimits"), scroller));
    QTRY_COMPARE_WITH_TIMEOUT(controller->limits().value(QStringLiteral("horizontal")).toDouble(), 2.5,
                              TestTimeout::mediumMs());
    auto* corridorWidth = findVisibleItemScrolled(QStringLiteral("caacFigureEightCorridorWidth"), scroller);
    QVERIFY(corridorWidth);
    QVERIFY(!findVisibleItem(_rootItem, QStringLiteral("caacFigureEightFenceMargin"), 0));
    const auto fenceBeforeWidth = fence->path();
    QVERIFY(corridorWidth->setProperty("text", QStringLiteral("1.5")));
    QVERIFY(QMetaObject::invokeMethod(corridorWidth, "editingFinished"));
    QTRY_COMPARE_WITH_TIMEOUT(controller->fenceWidth(), 1.5, TestTimeout::mediumMs());
    QTRY_VERIFY_WITH_TIMEOUT(fence->path() != fenceBeforeWidth, TestTimeout::mediumMs());
    QCOMPARE(master->saveToJson(), routeBefore);

    auto* insideLabel = findVisibleItemScrolled(QStringLiteral("caacFigureEightStatus"), scroller);
    QVERIFY(insideLabel);
    QVERIFY(clickItemFraction(QStringLiteral("caacFigureEightStatus"), 0.5, 0.5));
    QVERIFY(findVisibleItem(_rootItem, QStringLiteral("caacFigureEightPage"), 0));

    QVERIFY2(_clickOutsideDialog(), "No map click point is inside the window and outside the popup background");
    QTRY_VERIFY_WITH_TIMEOUT(!findVisibleItem(_rootItem, QStringLiteral("caacFigureEightPage"), 0),
                             TestTimeout::mediumMs());
    QVERIFY(controller->viewActive());
    QTRY_VERIFY_WITH_TIMEOUT(workspaceToolsMatch(true), TestTimeout::mediumMs());
    QCOMPARE(master->saveToJson(), routeBefore);
    QVERIFY(clickButtonScrolled(QStringLiteral("caacEightClearTrajectoryButton"), toolsScroller));
    QCOMPARE(controller->referencePath(), pathBeforeClose);
    QVERIFY(controller->viewActive());
    QVERIFY(clickButtonScrolled(QStringLiteral("caacEightFenceButton"), toolsScroller));
    pages = findVisibleItem(_rootItem, QStringLiteral("caacFigureEightPage"));
    QVERIFY(pages);
    QCOMPARE(pages->property("currentIndex").toInt(), 4);
    QVERIFY(!findVisibleItem(_rootItem, QStringLiteral("caacFigureEightCorridorWidth"), 0));
    auto* fenceMargin = findVisibleItemScrolled(QStringLiteral("caacFigureEightFenceMargin"), scroller);
    QVERIFY(fenceMargin);
    const auto fenceBeforeMargin = fence->path();
    QVERIFY(fenceMargin->setProperty("text", QStringLiteral("4")));
    QVERIFY(QMetaObject::invokeMethod(fenceMargin, "editingFinished"));
    QTRY_COMPARE_WITH_TIMEOUT(fence->margin(), 4.0, TestTimeout::mediumMs());
    QTRY_VERIFY_WITH_TIMEOUT(fence->path() != fenceBeforeMargin, TestTimeout::mediumMs());
    QTRY_VERIFY_WITH_TIMEOUT(outerFence->property("path").value<QList<QGeoCoordinate>>() != outerPath,
                             TestTimeout::mediumMs());
    QVERIFY(findVisibleItemScrolled(QStringLiteral("caacFigureEightFenceShowOnMap"), scroller));
    auto* applyFence = findVisibleItemScrolled(QStringLiteral("caacFigureEightFenceApply"), scroller);
    QVERIFY(applyFence);
    QVERIFY(!applyFence->isEnabled());
    QVERIFY(!fence->applied());
    auto* readFence = findVisibleItemScrolled(QStringLiteral("caacFigureEightFenceRead"), scroller);
    QVERIFY(readFence);
    QVERIFY(!readFence->isEnabled());
    QVERIFY(findVisibleItemScrolled(QStringLiteral("caacFigureEightFenceStatus"), scroller));
    QVERIFY2(_clickOutsideDialog(), "No map click point is inside the window and outside the popup background");
    QTRY_VERIFY_WITH_TIMEOUT(!findVisibleItem(_rootItem, QStringLiteral("caacFigureEightPage"), 0),
                             TestTimeout::mediumMs());
    QVERIFY(controller->viewActive());
    QTRY_VERIFY_WITH_TIMEOUT(workspaceToolsMatch(true), TestTimeout::mediumMs());
    QVERIFY(clickButtonScrolled(QStringLiteral("caacEightSiteListButton"), toolsScroller));
    pages = findVisibleItem(_rootItem, QStringLiteral("caacFigureEightPage"));
    QVERIFY(pages);
    QCOMPARE(controller->referencePath(), pathBeforeClose);
    QVERIFY(pages->setProperty("currentIndex", 1));
    auto* standard = findVisibleItemScrolled(QStringLiteral("caacFigureEightStandard"), scroller);
    QVERIFY(standard);
    QCOMPARE(standard->property("currentIndex").toInt(), 2);
    QVERIFY(pages->setProperty("currentIndex", 2));
    horizontal = findVisibleItemScrolled(QStringLiteral("caacFigureEightLimit_horizontal"), scroller);
    QVERIFY(horizontal);
    QCOMPARE(horizontal->property("text").toDouble(), 2.5);

    if (const auto screenshot = qgetenv("CAAC_FIGURE_EIGHT_SCREENSHOT"); !screenshot.isEmpty()) {
        controller->resetLimits();
        auto* flickable = findVisibleItem(_rootItem, scroller);
        QVERIFY(flickable);
        QString filename = QString::fromUtf8(screenshot);
        if (filename.endsWith(QStringLiteral(".png"))) {
            filename.chop(4);
        }
        for (int page = 0; page < 5; ++page) {
            QVERIFY(pages->setProperty("currentIndex", page));
            QVERIFY(flickable->setProperty("contentY", 0.0));
            QSignalSpy rendered(_window, &QQuickWindow::frameSwapped);
            _window->requestUpdate();
            QTRY_VERIFY_WITH_TIMEOUT(rendered.count() > 0, TestTimeout::mediumMs());
            const auto wholeWindow = _window->grabWindow();
            QVERIFY(wholeWindow.save(filename + QStringLiteral("-page-%1.png").arg(page)));
            auto* popupFrame = findVisibleItem(_rootItem, QStringLiteral("popupDialog_frame"));
            QVERIFY(popupFrame);
            const auto margin = popupFrame->property("spacing").toReal();
            const QRectF frameRect(popupFrame->mapToScene(QPointF()),
                                   QSizeF(popupFrame->width(), popupFrame->height()));
            const auto panelRect = frameRect.adjusted(-margin, -margin, margin, margin);
            const auto scale = _window->devicePixelRatio();
            const QRectF imageRect(panelRect.x() * scale, panelRect.y() * scale, panelRect.width() * scale,
                                   panelRect.height() * scale);
            QVERIFY(wholeWindow.copy(imageRect.toAlignedRect().intersected(wholeWindow.rect()))
                        .save(filename + QStringLiteral("-panel-%1.png").arg(page)));
        }
    }

    QVERIFY(pages->setProperty("currentIndex", 3));
    QVERIFY(clickButtonScrolled(QStringLiteral("caacFigureEightStart"), scroller));
    QVERIFY(!controller->running());
    QVERIFY(!controller->lastError().isEmpty());
    auto* status = findVisibleItemScrolled(QStringLiteral("caacFigureEightStatus"), scroller);
    QVERIFY(status);
    QTRY_COMPARE_WITH_TIMEOUT(status->property("text").toString(), controller->lastError(), TestTimeout::mediumMs());
    QVERIFY(!MultiVehicleManager::instance()->activeVehicle());
    QCOMPARE(master->saveToJson(), routeBefore);
    QVERIFY(rejectDialog());
    QVERIFY(controller->viewActive());
    if (const auto screenshot = qgetenv("CAAC_FIGURE_EIGHT_SCREENSHOT"); !screenshot.isEmpty()) {
        QString filename = QString::fromUtf8(screenshot);
        if (filename.endsWith(QStringLiteral(".png"))) {
            filename.chop(4);
        }
        QSignalSpy rendered(_window, &QQuickWindow::frameSwapped);
        _window->requestUpdate();
        QTRY_VERIFY_WITH_TIMEOUT(rendered.count() > 0, TestTimeout::mediumMs());
        QVERIFY(_window->grabWindow().save(filename + QStringLiteral("-map.png")));
    }
    QVERIFY(clickButton(QStringLiteral("caacFlightSettingsButton")));
    QVERIFY(!controller->viewActive());
    QTRY_VERIFY_WITH_TIMEOUT(workspaceToolsMatch(false), TestTimeout::mediumMs());
    QTRY_VERIFY_WITH_TIMEOUT(!reference->property("visible").toBool(), TestTimeout::mediumMs());
    QVERIFY(!outerFence->property("visible").toBool());
    for (int index = 0; index < 7; ++index) {
        QVERIFY(!findVisibleItem(_rootItem, QStringLiteral("caacFigureEightTrainingMarker%1").arg(index), 0));
    }
    QCOMPARE(controller->referencePath(), pathBeforeClose);
    QVERIFY(findVisibleItem(_rootItem, QStringLiteral("caacFlightSettingsPanel")));
    QCOMPARE(master->saveToJson(), routeBefore);
    QVERIFY(clickButton(QStringLiteral("caacFlightSettingsButton")));
    QVERIFY(!findVisibleItem(_rootItem, QStringLiteral("caacFlightSettingsPanel"), 0));
    QVERIFY(QMetaObject::invokeMethod(_window, "showPlanView"));
    QVERIFY(findVisibleItem(_rootItem, QStringLiteral("mainView_plan")));
    QCOMPARE(master->saveToJson(), routeBefore);
}

UT_REGISTER_TEST(CaacFigureEightUITest, TestLabel::Integration)

#include "CaacFigureEightUITest.moc"
