#include <cmath>

#include <QtCore/QLineF>
#include <QtCore/QRectF>
#include <QtGui/QImage>
#include <QtQuick/QQuickItem>
#include <QtQuick/QQuickWindow>
#include <QtTest/QTest>

#include "AppSettings.h"
#include "CaacRouteEditorController.h"
#include "Fact.h"
#include "FlightPathSegment.h"
#include "MissionController.h"
#include "PlanMasterController.h"
#include "QGCGeo.h"
#include "QmlUITestBase.h"
#include "SettingsManager.h"
#include "SimpleMissionItem.h"

class CaacPlanningInteractionUITest : public QmlUITestBase
{
    Q_OBJECT
private slots:
    void _pointMenusAndOrientation();
};

void CaacPlanningInteractionUITest::_pointMenusAndOrientation()
{
    startUI();
    if (QTest::currentTestFailed()) {
        return;
    }
    QVERIFY(clickToolSelectDropdownButton(QStringLiteral("toolbar_viewPlan")));
    auto* plan = findVisibleItem(_rootItem, QStringLiteral("mainView_plan"));
    auto* map = findVisibleItem(_rootItem, QStringLiteral("planView_map"));
    auto* panel = findVisibleItem(_rootItem, QStringLiteral("caacRouteEditor"));
    QVERIFY(plan && map && panel);
    auto* master = plan->property("_planMasterController").value<PlanMasterController*>();
    auto* editor = panel->property("routeController").value<CaacRouteEditorController*>();
    QVERIFY(master && editor);
    for (const auto* name : {"caacReadWaypoints", "caacUploadWaypoints"}) {
        auto* button = findVisibleItem(_rootItem, QLatin1String(name));
        QVERIFY(button && !button->isEnabled());
    }
    auto* mission = master->missionController();
    const QGeoCoordinate home(30, 120, 20);
    mission->setHomePosition(home);
    const auto a = QGCGeo::geodesicDestination(home, 90, 100);
    QVERIFY(editor->addWaypoint(a));
    QVERIFY(editor->addWaypoint(a));
    QTRY_COMPARE_WITH_TIMEOUT(mission->simpleFlightPathSegments()->count(), 2, TestTimeout::mediumMs());
    auto* firstLeg = mission->simpleFlightPathSegments()->value<FlightPathSegment*>(0);
    QVERIFY(firstLeg);
    QVERIFY(QGCGeo::geodesicDistance(firstLeg->coordinate1(), home) < 0.01);
    QVERIFY(QGCGeo::geodesicDistance(firstLeg->coordinate2(), a) < 0.01);
    QVERIFY(firstLeg->setProperty("terrainCollision", true));
    QVERIFY(findVisibleItem(_rootItem, QStringLiteral("caacTerrainClearanceWarning")));
    QVERIFY(firstLeg->setProperty("terrainCollision", false));
    QTRY_VERIFY_WITH_TIMEOUT(!findVisibleItem(_rootItem, QStringLiteral("caacTerrainClearanceWarning"), 0),
                             TestTimeout::mediumMs());
    map->setProperty("zoomLevel", 18);
    map->setProperty("center", QVariant::fromValue(a));
    auto* dragger = findVisibleItem(_rootItem, QStringLiteral("missionItemDrag_2"), TestTimeout::mediumMs());
    QVERIFY(dragger);
    auto* indicator = dragger->property("itemIndicator").value<QQuickItem*>();
    QVERIFY(indicator);
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
    QTest::mouseClick(_window, Qt::LeftButton, Qt::NoModifier, markerPosition().toPoint());
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
    QTest::keyClick(_window, Qt::Key_Escape);
    QVERIFY(!findVisibleItem(_rootItem, QStringLiteral("caacWaypointQuickBearing"), 0));
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

    QVERIFY(clickButton(QStringLiteral("caacRelativeEditorButton")));
    auto* relativeTarget = findVisibleItem(_rootItem, QStringLiteral("caacRelativeTarget"));
    auto* relativeReference = findVisibleItem(_rootItem, QStringLiteral("caacRelativeReference"));
    auto* relativeDistance = findVisibleItem(_rootItem, QStringLiteral("caacRelativeDistance"));
    QVERIFY(relativeTarget && relativeReference && relativeDistance);
    auto* relativeAngleType = findVisibleItem(_rootItem, QStringLiteral("caacRelativeAngleType"));
    auto* relativeAngle = findVisibleItem(_rootItem, QStringLiteral("caacRelativeAngle"));
    QVERIFY(relativeAngleType && relativeAngle);
    const auto beforeEndpoint = master->saveToJson();
    for (int targetIndex : {1, 2}) {
        QVERIFY(relativeAngleType->setProperty("currentIndex", 1));
        QVERIFY(relativeTarget->setProperty("currentIndex", targetIndex));
        QVERIFY(QMetaObject::invokeMethod(relativeTarget, "activated", Q_ARG(int, targetIndex)));
        QVERIFY(relativeAngleType->isEnabled());
        QCOMPARE(relativeAngleType->property("currentIndex").toInt(), 1);
        QVERIFY(relativeReference->setProperty("currentIndex", 1));
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
    for (const auto* name :
         {"caacHomeSection", "caacBasicSection", "caacSimFlightSection", "caacSimTurnSection", "caacSimClimbSection",
          "caacSimLoopsSection", "caacSimWindSection", "caacMapEditingSection"}) {
        QVERIFY(findItem(_rootItem, QLatin1String(name)));
    }
    auto* horizontal =
        findVisibleItemScrolled(QStringLiteral("caacSimHorizontalSpeed"), QStringLiteral("caacGlobalSettingsScroller"));
    QVERIFY(horizontal);
    horizontal->setProperty("text", QStringLiteral("7.5"));
    QVERIFY(clickButtonScrolled(QStringLiteral("caacSaveSimulationSettings"),
                                QStringLiteral("caacGlobalSettingsScroller")));
    QCOMPARE(editor->simulationSettings().value("horizontalSpeed").toDouble(), 7.5);
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
