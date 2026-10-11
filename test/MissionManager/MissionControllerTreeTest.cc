#include "MissionControllerTreeTest.h"

#include "AppSettings.h"
#include "MissionController.h"
#include "MissionSettingsItem.h"
#include "MultiSignalSpy.h"
#include "PlanMasterController.h"
#include "QGCCorePlugin.h"
#include "QmlObjectTreeModel.h"
#include "RallyPointController.h"
#include "SettingsManager.h"
#include "SimpleMissionItem.h"
#include "TestFixtures.h"

using namespace TestFixtures;

MissionControllerTreeTest::~MissionControllerTreeTest() = default;

void MissionControllerTreeTest::cleanup()
{
    _masterController.reset();
    _missionController = nullptr;
    MissionControllerManagerTest::cleanup();
}

void MissionControllerTreeTest::_initForTest()
{
    const bool showPlanInformation = QGCCorePlugin::instance()->showPlanInformation();
    const int omittedGroups = showPlanInformation ? 0 : 1;
    _planFileGroupRow = showPlanInformation ? MissionController::kPlanFileGroupRow : -1;
    _defaultsGroupRow = MissionController::kDefaultsGroupRow - omittedGroups;
    _missionGroupRow = MissionController::kMissionGroupRow - omittedGroups;
    _fenceGroupRow = MissionController::kFenceGroupRow - omittedGroups;
    _rallyGroupRow = MissionController::kRallyGroupRow - omittedGroups;
    _groupCount = MissionController::kGroupCount - omittedGroups;

    _initForFirmwareType(MAV_AUTOPILOT_PX4);
    SettingsManager::instance()->appSettings()->offlineEditingFirmwareClass()->setRawValue(
        QGCMAVLink::firmwareClass(MAV_AUTOPILOT_PX4));

    _masterController = std::make_unique<PlanMasterController>();
    _masterController->setFlyView(false);
    _missionController = _masterController->missionController();

    MultiSignalSpy spy;
    QVERIFY(spy.init(_missionController));
    _masterController->start();
    QVERIFY(spy.waitForSignal("visualItemsReset", TestTimeout::mediumMs()));

    // Sanity — visualItems has MissionSettingsItem at index 0
    QVERIFY(_missionController->visualItems());
    QCOMPARE(_missionController->visualItems()->count(), 1);
}

// ===========================================================================
// Tree structure after init
// ===========================================================================

void MissionControllerTreeTest::_testTreeStructureAfterInit()
{
    _initForTest();

    QmlObjectTreeModel* tree = _missionController->visualItemsTree();
    QVERIFY(tree);

    // Root should have all expected top-level groups
    QCOMPARE(tree->rowCount(), _groupCount);

    // Verify group node types
    const QModelIndex planFileGroup = tree->index(_planFileGroupRow, 0);
    const QModelIndex defaultsGroup = tree->index(_defaultsGroupRow, 0);
    const QModelIndex missionGroup = tree->index(_missionGroupRow, 0);
    const QModelIndex fenceGroup = tree->index(_fenceGroupRow, 0);
    const QModelIndex rallyGroup = tree->index(_rallyGroupRow, 0);
    QCOMPARE(planFileGroup.isValid(), _planFileGroupRow >= 0);
    QVERIFY(defaultsGroup.isValid());
    QVERIFY(missionGroup.isValid());
    QVERIFY(fenceGroup.isValid());
    QVERIFY(rallyGroup.isValid());

    if (_planFileGroupRow >= 0) {
        QCOMPARE(tree->data(planFileGroup, QmlObjectTreeModel::NodeTypeRole).toString(),
                 QStringLiteral("planFileGroup"));
    }
    QCOMPARE(tree->data(defaultsGroup, QmlObjectTreeModel::NodeTypeRole).toString(), QStringLiteral("defaultsGroup"));
    QCOMPARE(tree->data(missionGroup, QmlObjectTreeModel::NodeTypeRole).toString(), QStringLiteral("missionGroup"));
    QCOMPARE(tree->data(fenceGroup, QmlObjectTreeModel::NodeTypeRole).toString(), QStringLiteral("fenceGroup"));
    QCOMPARE(tree->data(rallyGroup, QmlObjectTreeModel::NodeTypeRole).toString(), QStringLiteral("rallyGroup"));

    // Plan File group should have 1 child (planFileInfo marker)
    if (_planFileGroupRow >= 0) {
        QCOMPARE(tree->rowCount(planFileGroup), 1);
        const QModelIndex planFileChild = tree->index(0, 0, planFileGroup);
        QCOMPARE(tree->data(planFileChild, QmlObjectTreeModel::NodeTypeRole).toString(),
                 QStringLiteral("planFileInfo"));
    }
    const QModelIndex transformGroup = tree->index(_groupCount - 1, 0);
    QCOMPARE(tree->data(transformGroup, QmlObjectTreeModel::NodeTypeRole).toString(), QStringLiteral("transformGroup"));
    QCOMPARE(QModelIndex(_missionController->transformGroupIndex()), transformGroup);

    // Defaults group should have 1 child (defaultsInfo marker)
    QCOMPARE(tree->rowCount(defaultsGroup), 1);
    const QModelIndex defaultsChild = tree->index(0, 0, defaultsGroup);
    QCOMPARE(tree->data(defaultsChild, QmlObjectTreeModel::NodeTypeRole).toString(), QStringLiteral("defaultsInfo"));

    // Mission group should contain 1 child (the MissionSettingsItem)
    QCOMPARE(tree->rowCount(missionGroup), 1);

    // Fence group should have 1 child (fenceEditor marker)
    QCOMPARE(tree->rowCount(fenceGroup), 1);
    const QModelIndex fenceChild = tree->index(0, 0, fenceGroup);
    QCOMPARE(tree->data(fenceChild, QmlObjectTreeModel::NodeTypeRole).toString(), QStringLiteral("fenceEditor"));

    // Rally group should have 1 child (rallyHeader marker)
    QCOMPARE(tree->rowCount(rallyGroup), 1);
    const QModelIndex rallyChild = tree->index(0, 0, rallyGroup);
    QCOMPARE(tree->data(rallyChild, QmlObjectTreeModel::NodeTypeRole).toString(), QStringLiteral("rallyHeader"));
}

// ===========================================================================
// Insert waypoints → tree sync
// ===========================================================================

void MissionControllerTreeTest::_testInsertWaypointUpdatesTree()
{
    _initForTest();

    QmlObjectTreeModel* tree = _missionController->visualItemsTree();
    const QModelIndex missionGroup = tree->index(_missionGroupRow, 0);

    // Before insert: 1 mission item (MissionSettingsItem)
    const int initialMissionChildren = tree->rowCount(missionGroup);
    QCOMPARE(initialMissionChildren, _missionController->visualItems()->count());

    // Insert 3 waypoints
    const QList<QGeoCoordinate> waypoints = Coord::waypointPath(Coord::zurich(), 3);
    for (int i = 0; i < waypoints.count(); ++i) {
        _missionController->insertSimpleMissionItem(waypoints[i], i + 1);
    }

    // _visualItems should have 4 items (settings + 3 waypoints)
    QCOMPARE(_missionController->visualItems()->count(), 4);

    // Tree's mission group should mirror that count
    QCOMPARE(tree->rowCount(missionGroup), 4);

    // Verify tree children correspond to visualItems
    for (int i = 0; i < _missionController->visualItems()->count(); i++) {
        QObject* listObj = _missionController->visualItems()->get(i);
        const QModelIndex treeChild = tree->index(i, 0, missionGroup);
        QObject* treeObj = tree->data(treeChild, Qt::UserRole).value<QObject*>();
        QCOMPARE(treeObj, listObj);
    }
}

// ===========================================================================
// Remove waypoints → tree sync
// ===========================================================================

void MissionControllerTreeTest::_testRemoveWaypointUpdatesTree()
{
    _initForTest();

    QmlObjectTreeModel* tree = _missionController->visualItemsTree();
    const QModelIndex missionGroup = tree->index(_missionGroupRow, 0);

    // Insert 3 waypoints
    const QList<QGeoCoordinate> waypoints = Coord::waypointPath(Coord::zurich(), 3);
    for (int i = 0; i < waypoints.count(); ++i) {
        _missionController->insertSimpleMissionItem(waypoints[i], i + 1);
    }
    QCOMPARE(tree->rowCount(missionGroup), 4); // settings + 3 waypoints

    // Remove middle waypoint (visualItem index 2)
    _missionController->removeVisualItem(2);

    QCOMPARE(_missionController->visualItems()->count(), 3);
    QCOMPARE(tree->rowCount(missionGroup), 3);

    // Verify correspondence
    for (int i = 0; i < _missionController->visualItems()->count(); i++) {
        QObject* listObj = _missionController->visualItems()->get(i);
        const QModelIndex treeChild = tree->index(i, 0, missionGroup);
        QObject* treeObj = tree->data(treeChild, Qt::UserRole).value<QObject*>();
        QCOMPARE(treeObj, listObj);
    }
}

// ===========================================================================
// removeAll → tree rebuilt
// ===========================================================================

void MissionControllerTreeTest::_testRemoveAllRebuildsTree()
{
    _initForTest();

    QmlObjectTreeModel* tree = _missionController->visualItemsTree();

    // Insert waypoints
    const QList<QGeoCoordinate> waypoints = Coord::waypointPath(Coord::zurich(), 3);
    for (int i = 0; i < waypoints.count(); ++i) {
        _missionController->insertSimpleMissionItem(waypoints[i], i + 1);
    }

    const QModelIndex missionGroupBefore = tree->index(_missionGroupRow, 0);
    QCOMPARE(tree->rowCount(missionGroupBefore), 4);

    // Remove all
    _missionController->removeAll();

    // After removeAll, tree should still have all groups
    QCOMPARE(tree->rowCount(), _groupCount);

    // Mission group should have 1 child (the new MissionSettingsItem)
    const QModelIndex missionGroupAfter = tree->index(_missionGroupRow, 0);
    QCOMPARE(tree->rowCount(missionGroupAfter), 1);

    // That child should be the new settings item
    const QModelIndex settingsChild = tree->index(0, 0, missionGroupAfter);
    QObject* settingsObj = tree->data(settingsChild, Qt::UserRole).value<QObject*>();
    QCOMPARE(settingsObj, static_cast<QObject*>(_missionController->visualItems()->get(0)));
}

// ===========================================================================
// Persistent group indexes remain valid across operations
// ===========================================================================

void MissionControllerTreeTest::_testPersistentGroupIndexes()
{
    _initForTest();

    QmlObjectTreeModel* tree = _missionController->visualItemsTree();

    // Capture persistent indexes for each group
    QPersistentModelIndex planFilePersist(tree->index(_planFileGroupRow, 0));
    QPersistentModelIndex defaultsPersist(tree->index(_defaultsGroupRow, 0));
    QPersistentModelIndex missionPersist(tree->index(_missionGroupRow, 0));
    QPersistentModelIndex fencePersist(tree->index(_fenceGroupRow, 0));
    QPersistentModelIndex rallyPersist(tree->index(_rallyGroupRow, 0));

    // Insert waypoints
    const QList<QGeoCoordinate> waypoints = Coord::waypointPath(Coord::zurich(), 5);
    for (int i = 0; i < waypoints.count(); ++i) {
        _missionController->insertSimpleMissionItem(waypoints[i], i + 1);
    }

    // All persistent indexes should still be valid
    QCOMPARE(planFilePersist.isValid(), _planFileGroupRow >= 0);
    QVERIFY(defaultsPersist.isValid());
    QVERIFY(missionPersist.isValid());
    QVERIFY(fencePersist.isValid());
    QVERIFY(rallyPersist.isValid());

    // They should still be at the same rows (group structure didn't change)
    QCOMPARE(planFilePersist.row(), _planFileGroupRow);
    QCOMPARE(defaultsPersist.row(), _defaultsGroupRow);
    QCOMPARE(missionPersist.row(), _missionGroupRow);
    QCOMPARE(fencePersist.row(), _fenceGroupRow);
    QCOMPARE(rallyPersist.row(), _rallyGroupRow);

    // Remove some waypoints
    _missionController->removeVisualItem(3);
    _missionController->removeVisualItem(2);

    // Persistent indexes should still be valid
    QCOMPARE(planFilePersist.isValid(), _planFileGroupRow >= 0);
    QVERIFY(defaultsPersist.isValid());
    QVERIFY(missionPersist.isValid());
    QVERIFY(fencePersist.isValid());
    QVERIFY(rallyPersist.isValid());
}

// ===========================================================================
// recalcChildItems is a no-op for the tree (no crash)
// ===========================================================================

void MissionControllerTreeTest::_testRecalcChildItemsNoCrash()
{
    _initForTest();

    // Insert enough waypoints to exercise child item recalc
    const QList<QGeoCoordinate> waypoints = Coord::waypointPath(Coord::zurich(), 4);
    for (int i = 0; i < waypoints.count(); ++i) {
        _missionController->insertSimpleMissionItem(waypoints[i], i + 1);
    }

    QmlObjectTreeModel* tree = _missionController->visualItemsTree();
    const QModelIndex missionGroup = tree->index(_missionGroupRow, 0);

    // Tree should be consistent — recalcChildItems happens automatically during insert
    QCOMPARE(tree->rowCount(missionGroup), _missionController->visualItems()->count());

    // Verify no crash and tree is still functional
    QVERIFY(tree->index(0, 0, missionGroup).isValid());
    QCOMPARE(tree->rowCount(), _groupCount);
}

// ===========================================================================
// Exposed Q_PROPERTY persistent index getters match tree model indices
// ===========================================================================

void MissionControllerTreeTest::_testExposedPersistentIndexGettersMatchTree()
{
    _initForTest();

    QmlObjectTreeModel* tree = _missionController->visualItemsTree();

    QCOMPARE(QModelIndex(_missionController->planFileGroupIndex()), tree->index(_planFileGroupRow, 0));
    QCOMPARE(QModelIndex(_missionController->defaultsGroupIndex()), tree->index(_defaultsGroupRow, 0));
    QCOMPARE(QModelIndex(_missionController->missionGroupIndex()), tree->index(_missionGroupRow, 0));
    QCOMPARE(QModelIndex(_missionController->fenceGroupIndex()), tree->index(_fenceGroupRow, 0));
    QCOMPARE(QModelIndex(_missionController->rallyGroupIndex()), tree->index(_rallyGroupRow, 0));
}

// ===========================================================================
// Exposed persistent indexes survive removeAll rebuild
// ===========================================================================

void MissionControllerTreeTest::_testExposedIndexesSurviveRemoveAll()
{
    _initForTest();

    // Insert some waypoints then removeAll to trigger tree rebuild
    const QList<QGeoCoordinate> waypoints = Coord::waypointPath(Coord::zurich(), 3);
    for (int i = 0; i < waypoints.count(); ++i) {
        _missionController->insertSimpleMissionItem(waypoints[i], i + 1);
    }

    _missionController->removeAll();

    // The CONSTANT Q_PROPERTY persistent indexes must still be valid
    QCOMPARE(_missionController->planFileGroupIndex().isValid(), _planFileGroupRow >= 0);
    QVERIFY(_missionController->defaultsGroupIndex().isValid());
    QVERIFY(_missionController->missionGroupIndex().isValid());
    QVERIFY(_missionController->fenceGroupIndex().isValid());
    QVERIFY(_missionController->rallyGroupIndex().isValid());

    // They should still point to the correct rows
    QCOMPARE(_missionController->planFileGroupIndex().row(), _planFileGroupRow);
    QCOMPARE(_missionController->defaultsGroupIndex().row(), _defaultsGroupRow);
    QCOMPARE(_missionController->missionGroupIndex().row(), _missionGroupRow);
    QCOMPARE(_missionController->fenceGroupIndex().row(), _fenceGroupRow);
    QCOMPARE(_missionController->rallyGroupIndex().row(), _rallyGroupRow);
}

// ===========================================================================
// Rally header appears when no points, disappears when points added, reappears when removed
// ===========================================================================

void MissionControllerTreeTest::_testRallyHeaderDynamicVisibility()
{
    _initForTest();

    QmlObjectTreeModel* tree = _missionController->visualItemsTree();
    const QPersistentModelIndex rallyGroup(_missionController->rallyGroupIndex());
    auto* rallyController = _masterController->rallyPointController();
    QVERIFY(rallyController);

    // Initially: rally group has 1 child — the header marker
    QCOMPARE(tree->rowCount(rallyGroup), 1);
    QCOMPARE(tree->data(tree->index(0, 0, rallyGroup), QmlObjectTreeModel::NodeTypeRole).toString(),
             QStringLiteral("rallyHeader"));

    // Add a rally point → header removed, replaced by rallyItem
    rallyController->addPoint(QGeoCoordinate(47.3977, 8.5456));
    QCOMPARE(tree->rowCount(rallyGroup), 1);
    QCOMPARE(tree->data(tree->index(0, 0, rallyGroup), QmlObjectTreeModel::NodeTypeRole).toString(),
             QStringLiteral("rallyItem"));

    // Add a second rally point
    rallyController->addPoint(QGeoCoordinate(47.3980, 8.5460));
    QCOMPARE(tree->rowCount(rallyGroup), 2);
    QCOMPARE(tree->data(tree->index(0, 0, rallyGroup), QmlObjectTreeModel::NodeTypeRole).toString(),
             QStringLiteral("rallyItem"));
    QCOMPARE(tree->data(tree->index(1, 0, rallyGroup), QmlObjectTreeModel::NodeTypeRole).toString(),
             QStringLiteral("rallyItem"));

    // Remove the first rally point — still 1 rallyItem, no header
    rallyController->removePoint(rallyController->points()->get(0));
    QCOMPARE(tree->rowCount(rallyGroup), 1);
    QCOMPARE(tree->data(tree->index(0, 0, rallyGroup), QmlObjectTreeModel::NodeTypeRole).toString(),
             QStringLiteral("rallyItem"));

    // Remove the last rally point → header marker reappears
    rallyController->removePoint(rallyController->points()->get(0));
    QCOMPARE(tree->rowCount(rallyGroup), 1);
    QCOMPARE(tree->data(tree->index(0, 0, rallyGroup), QmlObjectTreeModel::NodeTypeRole).toString(),
             QStringLiteral("rallyHeader"));
}

#include "UnitTest.h"

UT_REGISTER_TEST(MissionControllerTreeTest, TestLabel::Integration, TestLabel::MissionManager)
