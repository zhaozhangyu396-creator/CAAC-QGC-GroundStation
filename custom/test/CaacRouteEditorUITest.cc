#include <QtCore/QPointer>
#include <QtQuick/QQuickItem>
#include <QtQuick/QQuickWindow>
#include <QtTest/QTest>

#include "MissionController.h"
#include "PlanMasterController.h"
#include "QmlUITestBase.h"
#include "SimpleMissionItem.h"

class CaacRouteEditorUITest : public QmlUITestBase
{
    Q_OBJECT

private slots:
    void _testMouseEditing();
};

void CaacRouteEditorUITest::_testMouseEditing()
{
    startUI();
    if (QTest::currentTestFailed()) {
        return;
    }
    QVERIFY(clickToolSelectDropdownButton(QStringLiteral("toolbar_viewPlan")));
    auto* plan = findVisibleItem(_rootItem, QStringLiteral("mainView_plan"), TestTimeout::mediumMs());
    QVERIFY(plan);
    auto* master = plan->property("_planMasterController").value<PlanMasterController*>();
    QVERIFY(master);
    auto* mission = master->missionController();
    auto* map = findVisibleItem(_rootItem, QStringLiteral("planView_map"));
    QVERIFY(map);
    QVERIFY(findVisibleItem(_rootItem, QStringLiteral("caacRouteEditor")));
    QVERIFY(map->property("directMissionItemEditing").toBool());
    const QGeoCoordinate center(30, 120);
    map->setProperty("center", QVariant::fromValue(center));
    map->setProperty("zoomLevel", 16.0);
    QTRY_VERIFY_WITH_TIMEOUT(map->property("mapReady").toBool(), TestTimeout::mediumMs());
    const QPoint a = map->mapToScene(QPointF(map->width() * 0.4, map->height() * 0.5)).toPoint();
    const QPoint b = a + QPoint(100, 50);
    QTest::mouseClick(_window, Qt::LeftButton, Qt::NoModifier, a);
    QCOMPARE(mission->visualItems()->count(), 1);
    // The second click within the platform double-click interval creates exactly one waypoint.
    QTest::mouseClick(_window, Qt::LeftButton, Qt::NoModifier, a);
    QTRY_COMPARE_WITH_TIMEOUT(mission->visualItems()->count(), 2, TestTimeout::mediumMs());
    QTest::mouseClick(_window, Qt::LeftButton, Qt::NoModifier, b);
    QTest::mouseClick(_window, Qt::LeftButton, Qt::NoModifier, b);
    QTRY_COMPARE_WITH_TIMEOUT(mission->visualItems()->count(), 3, TestTimeout::mediumMs());
    QPointer<SimpleMissionItem> first = mission->visualItems()->value<SimpleMissionItem*>(1);
    QVERIFY(first);
    auto* dragger = findVisibleItem(_rootItem, QStringLiteral("missionItemDrag_1"), TestTimeout::mediumMs());
    QVERIFY(dragger);
    const QPoint marker = dragger->mapToScene(QPointF(dragger->width() / 2, dragger->height() / 2)).toPoint();
    QTest::mouseClick(_window, Qt::RightButton, Qt::NoModifier, marker);
    QVERIFY(waitForDialog(QStringLiteral("Waypoint properties"), TestTimeout::mediumMs()));
    auto* latitude = findVisibleItem(_rootItem, QStringLiteral("caacWaypointLatitude"));
    auto* altitude = findVisibleItem(_rootItem, QStringLiteral("caacWaypointAltitude"));
    QVERIFY(latitude);
    QVERIFY(altitude);
    const auto oldCoordinate = first->coordinate();
    latitude->setProperty("text", QStringLiteral("30.12345678"));
    altitude->setProperty("text", QStringLiteral("NaN"));
    QVERIFY(acceptDialog());
    QVERIFY(dialogVisible(QStringLiteral("Waypoint properties")));
    QCOMPARE(first->coordinate(), oldCoordinate);
    altitude->setProperty("text", QStringLiteral("123.45"));
    QVERIFY(acceptDialog());
    QTRY_VERIFY_WITH_TIMEOUT(!dialogVisible(QStringLiteral("Waypoint properties")), TestTimeout::mediumMs());
    QCOMPARE(first->coordinate().latitude(), 30.12345678);
    QCOMPARE(first->altitude()->rawValue().toDouble(), 123.45);

    // Bring the edited waypoint back into view and select the other waypoint before dragging it.
    first->setCoordinate(oldCoordinate);
    mission->setCurrentPlanViewSeqNum(mission->visualItems()->value<VisualMissionItem*>(2)->sequenceNumber(), true);
    dragger = findVisibleItem(_rootItem, QStringLiteral("missionItemDrag_1"), TestTimeout::mediumMs());
    QVERIFY(dragger);
    const QPoint from = dragger->mapToScene(QPointF(dragger->width() / 2, dragger->height() / 2)).toPoint();
    QTest::mousePress(_window, Qt::LeftButton, Qt::NoModifier, from);
    for (int i = 1; i <= 5; ++i) {
        QTest::mouseMove(_window, from + QPoint(i * 8, -i * 5));
    }
    QTest::mouseRelease(_window, Qt::LeftButton, Qt::NoModifier, from + QPoint(40, -25));
    QTRY_VERIFY_WITH_TIMEOUT(first->coordinate().distanceTo(oldCoordinate) > 1, TestTimeout::mediumMs());
    QCOMPARE(first->altitude()->rawValue().toDouble(), 123.45);
    QCOMPARE(mission->visualItems()->count(), 3);

    const auto beforePan = map->property("center").value<QGeoCoordinate>();
    const QPoint blank = map->mapToScene(QPointF(map->width() * 0.35, map->height() * 0.75)).toPoint();
    QTest::mousePress(_window, Qt::LeftButton, Qt::NoModifier, blank);
    for (int i = 1; i <= 5; ++i) {
        QTest::mouseMove(_window, blank + QPoint(i * 12, 0));
    }
    QTest::mouseRelease(_window, Qt::LeftButton, Qt::NoModifier, blank + QPoint(60, 0));
    QTRY_VERIFY_WITH_TIMEOUT(map->property("center").value<QGeoCoordinate>().distanceTo(beforePan) > 1,
                             TestTimeout::mediumMs());
    QCOMPARE(mission->visualItems()->count(), 3);
    const QString screenshot = qEnvironmentVariable("CAAC_UI_SCREENSHOT");
    if (!screenshot.isEmpty()) {
        QVERIFY(_window->grabWindow().save(screenshot));
    }
    master->removeAll();
}

UT_REGISTER_TEST(CaacRouteEditorUITest, TestLabel::Integration, TestLabel::MissionManager)

#include "CaacRouteEditorUITest.moc"
