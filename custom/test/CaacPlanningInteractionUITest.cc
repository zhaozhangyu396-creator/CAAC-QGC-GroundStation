#include <cmath>

#include <QtCore/QLineF>
#include <QtGui/QImage>
#include <QtQuick/QQuickItem>
#include <QtQuick/QQuickWindow>
#include <QtTest/QTest>

#include "AppSettings.h"
#include "CaacRouteEditorController.h"
#include "Fact.h"
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
    const auto selected = mission->currentPlanViewItem()->sequenceNumber();
    auto* waypoint = editor->currentItem();
    QVERIFY(waypoint);
    const double initialDistance = editor->waypointLeg(waypoint).value("distance").toDouble();
    QVERIFY(clickButton(QStringLiteral("caacWaypointFineTuning")));
    _window->requestActivate();
    QTRY_VERIFY_WITH_TIMEOUT(_window->isActive(), TestTimeout::mediumMs());
    QTest::keyClick(_window, Qt::Key_Right);
    QTRY_VERIFY_WITH_TIMEOUT(
        std::abs(editor->waypointLeg(waypoint).value("distance").toDouble() - initialDistance - 0.1) < 0.001,
        TestTimeout::mediumMs());
    const auto initialBearing = editor->waypointLeg(waypoint).value("bearing").toDouble();
    QTest::keyClick(_window, Qt::Key_Up);
    QTRY_VERIFY_WITH_TIMEOUT(
        std::abs(std::remainder(editor->waypointLeg(waypoint).value("bearing").toDouble() - initialBearing - 0.1,
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
    QVERIFY(findItem(_rootItem, QStringLiteral("caacAuxiliaryLine")));
    QVERIFY(clickButton(QStringLiteral("caacEditWaypoint_%1").arg(selected)));
    QVERIFY(waitForDialog(QCoreApplication::translate("CaacWaypointDialog", "Waypoint properties")));
    QVERIFY(rejectDialog());
    QVERIFY(findVisibleItem(_rootItem, QStringLiteral("caacRemoveWaypoint_%1").arg(selected)));

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
