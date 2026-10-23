#include <QtCore/QList>
#include <QtCore/QRegularExpression>
#include <QtCore/QScopeGuard>
#include <QtCore/QScopedPointer>
#include <QtCore/QTemporaryDir>
#include <QtGui/QColor>
#include <QtPositioning/QGeoCoordinate>
#include <QtQml/QJSValue>
#include <QtQml/QQmlComponent>
#include <QtQml/QQmlEngine>
#include <QtQml/QQmlExpression>
#include <QtQml/QQmlProperty>
#include <QtQuick/QQuickItem>
#include <QtQuick/QQuickWindow>
#include <QtTest/QSignalSpy>

#include "CaacCorePlugin.h"
#include "CaacRouteEditorController.h"
#include "CaacSimulationController.h"
#include "Fact.h"
#include "FlightPathSegment.h"
#include "MissionController.h"
#include "MissionManager.h"
#include "MockLink.h"
#include "PlanMasterController.h"
#include "QmlUITestBase.h"
#include "SimpleMissionItem.h"
#include "Vehicle.h"

class CaacRouteUploadUITest : public QmlUITestBase
{
    Q_OBJECT

private slots:
    void _normalRouteKeepsAcknowledgedColors();
    void _compiledPreviewKeepsAcknowledgedColors();

private:
    struct ExpectedLeg
    {
        QString kind;
        QGeoCoordinate from;
        QGeoCoordinate to;
        QColor color;
    };

    void _allowUnavailablePressureBackend();
    void _prepareRoute(PlanMasterController* master, CaacRouteEditorController* editor, double altitude);
    static QColor _evaluateColor(QObject* scope, const QString& source, QString* error = nullptr);
    static QVariantList _variantList(const QVariant& value);
    static bool _normalColorsMatch(QQuickItem* map, const QList<QColor>& expected, QString& diagnostic);
    static bool _previewColorsMatch(QQuickItem* map, QQuickItem* panel, const QList<ExpectedLeg>& expected,
                                    QString& diagnostic);
};

void CaacRouteUploadUITest::_allowUnavailablePressureBackend()
{
    // Optional host pressure backends are unavailable on some desktop runners.
    ignoreLogMessage("Utilities.QGCSensors", QtWarningMsg,
                     QRegularExpression(QStringLiteral("Failed to connect to pressure backend$")));
    ignoreLogMessage("Utilities.QGCSensors", QtWarningMsg,
                     QRegularExpression(QStringLiteral("Error Initializing Pressure Sensor$")));
}

void CaacRouteUploadUITest::_prepareRoute(PlanMasterController* master, CaacRouteEditorController* editor,
                                          double altitude)
{
    QVERIFY(master && editor);
    master->removeAll();
    auto* mission = master->missionController();
    mission->setHomePosition(QGeoCoordinate(30, 120, 20));
    for (const auto& coordinate : {QGeoCoordinate(30, 120.001), QGeoCoordinate(30.001, 120.001),
                                   QGeoCoordinate(30.001, 120.002), QGeoCoordinate(30.002, 120.002)}) {
        QVERIFY(editor->addWaypoint(coordinate));
        auto* item = mission->visualItems()->value<SimpleMissionItem*>(mission->visualItems()->count() - 1);
        QVERIFY(item);
        item->setAltitudeFrame(QGroundControlQmlGlobal::AltitudeFrameRelative);
        item->altitude()->setRawValue(altitude);
    }
    QVERIFY(editor->setMissionFlightSpeed(true, 5));
    QTRY_COMPARE_WITH_TIMEOUT(mission->simpleFlightPathSegments()->count(), 4, TestTimeout::mediumMs());
}

QColor CaacRouteUploadUITest::_evaluateColor(QObject* scope, const QString& source, QString* error)
{
    if (!scope || !qmlContext(scope)) {
        if (error) {
            *error = QStringLiteral("Missing QML scope for %1").arg(source);
        }
        return {};
    }
    QQmlExpression expression(qmlContext(scope), scope, source);
    const auto value = expression.evaluate();
    if (expression.hasError()) {
        if (error) {
            *error = expression.error().toString();
        }
        return {};
    }
    return value.value<QColor>();
}

QVariantList CaacRouteUploadUITest::_variantList(const QVariant& value)
{
    return value.metaType() == QMetaType::fromType<QJSValue>() ? value.value<QJSValue>().toVariant().toList()
                                                               : value.toList();
}

bool CaacRouteUploadUITest::_normalColorsMatch(QQuickItem* map, const QList<QColor>& expected, QString& diagnostic)
{
    for (int i = 0; i < expected.size(); ++i) {
        const auto name = QStringLiteral("caacMissionRouteLine_%1").arg(i);
        auto* line = findItem(map, name);
        if (!line) {
            diagnostic = name + QStringLiteral(" missing");
            return false;
        }
        const auto colorValue = QQmlProperty::read(line, QStringLiteral("line.color"));
        const auto actual = colorValue.value<QColor>();
        if (!actual.isValid() || actual != expected.at(i)) {
            diagnostic = QStringLiteral("%1 actual %2 expected %3 (property valid %4, color valid %5)")
                             .arg(name, actual.name(), expected.at(i).name())
                             .arg(colorValue.isValid())
                             .arg(actual.isValid());
            return false;
        }
    }
    if (findItem(map, QStringLiteral("caacMissionRouteLine_%1").arg(expected.size()))) {
        diagnostic = QStringLiteral("Unexpected extra normal route segment");
        return false;
    }
    return true;
}

bool CaacRouteUploadUITest::_previewColorsMatch(QQuickItem* map, QQuickItem* panel, const QList<ExpectedLeg>& expected,
                                                QString& diagnostic)
{
    const auto segments = _variantList(panel->property("simulationPreviewSegments"));
    if (segments.size() != expected.size()) {
        diagnostic = QStringLiteral("Preview has %1 segments; expected %2").arg(segments.size()).arg(expected.size());
        return false;
    }
    for (int i = 0; i < expected.size(); ++i) {
        const auto segment = segments.at(i).toMap();
        const auto& leg = expected.at(i);
        const auto from = segment.value(QStringLiteral("coordinate1")).value<QGeoCoordinate>();
        const auto to = segment.value(QStringLiteral("coordinate2")).value<QGeoCoordinate>();
        if (!from.isValid() || !to.isValid() || from.distanceTo(leg.from) >= 0.02 || to.distanceTo(leg.to) >= 0.02 ||
            segment.value(QStringLiteral("kind")).toString() != leg.kind) {
            diagnostic = QStringLiteral("Preview segment %1 has unexpected directed geometry or kind").arg(i);
            return false;
        }
        const auto name = QStringLiteral("caacSimulationPreview_") + leg.kind +
                          (leg.kind == QStringLiteral("body") ? QStringLiteral("_%1").arg(i) : QString());
        auto* line = findItem(map, name);
        if (!line) {
            diagnostic = name + QStringLiteral(" missing");
            return false;
        }
        const auto colorValue = QQmlProperty::read(line, QStringLiteral("line.color"));
        const auto actual = colorValue.value<QColor>();
        if (!actual.isValid() || actual != leg.color) {
            diagnostic = QStringLiteral("%1 actual %2 expected %3 (property valid %4, color valid %5)")
                             .arg(name, actual.name(), leg.color.name())
                             .arg(colorValue.isValid())
                             .arg(actual.isValid());
            return false;
        }
    }
    return true;
}

void CaacRouteUploadUITest::_normalRouteKeepsAcknowledgedColors()
{
    _allowUnavailablePressureBackend();
    runWithMockLink(
        [] { return MockLink::startPX4MockLink(); },
        [&](QPointer<MockLink>, Vehicle* vehicle) {
            QVERIFY(vehicle);
            QTRY_VERIFY_WITH_TIMEOUT(vehicle->initialPlanRequestComplete(), TestTimeout::mediumMs());
            QVERIFY(QMetaObject::invokeMethod(_window, "showPlanView"));
            auto* plan = findVisibleItem(_rootItem, QStringLiteral("mainView_plan"));
            auto* panel = findVisibleItem(_rootItem, QStringLiteral("caacRouteEditor"));
            auto* map = findVisibleItem(_rootItem, QStringLiteral("planView_map"));
            QVERIFY(plan && panel && map);
            auto* master = plan->property("_planMasterController").value<PlanMasterController*>();
            auto* editor = panel->property("routeController").value<CaacRouteEditorController*>();
            QVERIFY(master && editor);
            QTRY_VERIFY_WITH_TIMEOUT(!master->syncInProgress(), TestTimeout::mediumMs());
            _prepareRoute(master, editor, 30);
            if (QTest::currentTestFailed()) {
                return;
            }
            QVERIFY(panel->setProperty("activePanel", -1));
            QVERIFY(map->setProperty("center", QVariant::fromValue(QGeoCoordinate(30.001, 120.001))));
            QVERIFY(map->setProperty("zoomLevel", 16.0));
            QVERIFY(!panel->property("simulationPreviewActive").toBool());
            auto* mission = master->missionController();
            const auto green = _evaluateColor(map, QStringLiteral("QGroundControl.globalPalette.colorGreen"));
            const auto orange =
                _evaluateColor(map, QStringLiteral("QGroundControl.globalPalette.mapMissionTrajectory"));
            QVERIFY(green.isValid() && orange.isValid() && green != orange);
            QString diagnostic;
            QTRY_VERIFY2_WITH_TIMEOUT(_normalColorsMatch(map, {orange, orange, orange, orange}, diagnostic),
                                      qPrintable(diagnostic), TestTimeout::mediumMs());
            QSignalSpy sent(vehicle->missionManager(), &MissionManager::sendComplete);
            QVERIFY(sent.isValid());
            mission->sendToVehicle();
            QTRY_COMPARE_WITH_TIMEOUT(sent.count(), 1, TestTimeout::longMs());
            QVERIFY(!sent.first().first().toBool());
            QTRY_VERIFY2_WITH_TIMEOUT(_normalColorsMatch(map, {green, green, green, green}, diagnostic),
                                      qPrintable(diagnostic), TestTimeout::mediumMs());

            auto* appended = qobject_cast<SimpleMissionItem*>(
                mission->insertSimpleMissionItem(QGeoCoordinate(30.003, 120.002), 5, false));
            QVERIFY(appended);
            appended->altitude()->setRawValue(30);
            QTRY_VERIFY2_WITH_TIMEOUT(_normalColorsMatch(map, {green, green, green, green, orange}, diagnostic),
                                      qPrintable(diagnostic), TestTimeout::mediumMs());
            auto* inserted = qobject_cast<SimpleMissionItem*>(
                mission->insertSimpleMissionItem(QGeoCoordinate(30.0005, 120.0013), 2, false));
            QVERIFY(inserted);
            inserted->altitude()->setRawValue(30);
            // Only the two new adjacencies and the appended tail need uploading.
            const QList<QColor> modified{green, orange, orange, green, green, orange};
            QTRY_VERIFY2_WITH_TIMEOUT(_normalColorsMatch(map, modified, diagnostic), qPrintable(diagnostic),
                                      TestTimeout::mediumMs());
            mission->setDirty(false);
            QTemporaryDir directory;
            QVERIFY(directory.isValid());
            QVERIFY(master->saveToFile(directory.filePath(QStringLiteral("normal-edits.plan"))));
            QTRY_VERIFY2_WITH_TIMEOUT(_normalColorsMatch(map, modified, diagnostic), qPrintable(diagnostic),
                                      TestTimeout::mediumMs());
            QCOMPARE(sent.count(), 1);

            auto* oldLeg = mission->simpleFlightPathSegments()->value<FlightPathSegment*>(3);
            QVERIFY(oldLeg && oldLeg->uploaded());
            QVERIFY(oldLeg->setProperty("amslTerrainHeights", QVariantList{1000.0}));
            QVERIFY(QMetaObject::invokeMethod(oldLeg, "_updateTerrainCollision", Qt::DirectConnection));
            QVERIFY(oldLeg->terrainCollision());
            QTRY_VERIFY2_WITH_TIMEOUT(
                _normalColorsMatch(map, {green, orange, orange, QColor(Qt::red), green, orange}, diagnostic),
                qPrintable(diagnostic), TestTimeout::mediumMs());
            QVERIFY(oldLeg->setProperty("amslTerrainHeights", QVariantList{0.0}));
            QVERIFY(QMetaObject::invokeMethod(oldLeg, "_updateTerrainCollision", Qt::DirectConnection));
            QVERIFY(!oldLeg->terrainCollision());
            QTRY_VERIFY2_WITH_TIMEOUT(_normalColorsMatch(map, modified, diagnostic), qPrintable(diagnostic),
                                      TestTimeout::mediumMs());

            const auto repeatedFrom = mission->visualItems()->value<SimpleMissionItem*>(3)->coordinate();
            const auto repeatedTo = mission->visualItems()->value<SimpleMissionItem*>(4)->coordinate();
            for (const auto& coordinate : {repeatedFrom, repeatedTo}) {
                auto* repeated = qobject_cast<SimpleMissionItem*>(
                    mission->insertSimpleMissionItem(coordinate, mission->visualItems()->count(), false));
                QVERIFY(repeated);
                repeated->altitude()->setRawValue(30);
            }
            mission->sendToVehicle();
            QTRY_COMPARE_WITH_TIMEOUT(sent.count(), 2, TestTimeout::longMs());
            QVERIFY(!sent.last().first().toBool());
            QQmlComponent pathComponent(qmlEngine(map));
            pathComponent.setData("import QGroundControl.GeoMap\nGeoMapMissionPath {}", QUrl());
            QVERIFY2(pathComponent.isReady(), qPrintable(pathComponent.errorString()));
            const QScopedPointer<QObject> path(pathComponent.createWithInitialProperties(
                {{QStringLiteral("missionController"), QVariant::fromValue(mission)},
                 {QStringLiteral("vehicle"), QVariant::fromValue(vehicle)}}));
            QVERIFY(path);
            const auto states = [&]() {
                QQmlExpression expression(
                    qmlContext(path.data()), path.data(),
                    QStringLiteral("_routePoints.map((point, index) => _legColorState(index)).slice(1)"));
                return _variantList(expression.evaluate());
            };
            QTRY_COMPARE_WITH_TIMEOUT(states(), QVariantList({2, 2, 2, 2, 2, 2, 2, 2}), TestTimeout::mediumMs());
            mission->visualItems()->value<SimpleMissionItem*>(8)->altitude()->setRawValue(40);
            QTRY_VERIFY2_WITH_TIMEOUT(
                _normalColorsMatch(map, {green, green, green, green, green, green, green, orange}, diagnostic),
                qPrintable(diagnostic), TestTimeout::mediumMs());
            QTRY_COMPARE_WITH_TIMEOUT(states(), QVariantList({2, 2, 2, 2, 2, 2, 2, 0}), TestTimeout::mediumMs());
        });
}

void CaacRouteUploadUITest::_compiledPreviewKeepsAcknowledgedColors()
{
    startUI();
    if (QTest::currentTestFailed()) {
        return;
    }
    QVERIFY(QMetaObject::invokeMethod(_window, "showPlanView"));
    auto* plan = findVisibleItem(_rootItem, QStringLiteral("mainView_plan"));
    auto* panel = findVisibleItem(_rootItem, QStringLiteral("caacRouteEditor"));
    auto* map = findVisibleItem(_rootItem, QStringLiteral("planView_map"));
    QVERIFY(plan && panel && map);
    auto* master = plan->property("_planMasterController").value<PlanMasterController*>();
    auto* editor = panel->property("routeController").value<CaacRouteEditorController*>();
    auto* plugin = qobject_cast<CaacCorePlugin*>(QGCCorePlugin::instance());
    QVERIFY(master && editor && plugin);
    _prepareRoute(master, editor, 2);
    if (QTest::currentTestFailed()) {
        return;
    }
    auto* mission = master->missionController();
    const auto home = mission->plannedHomePosition();
    const auto first = mission->visualItems()->value<SimpleMissionItem*>(1)->coordinate();
    const auto second = mission->visualItems()->value<SimpleMissionItem*>(2)->coordinate();
    auto* originalThird = mission->visualItems()->value<SimpleMissionItem*>(3);
    const auto third = originalThird->coordinate();
    const auto fourth = mission->visualItems()->value<SimpleMissionItem*>(4)->coordinate();
    auto settings = CaacSimulationController::defaults();
    settings["closed"] = true;
    settings["loopMode"] = 1;
    settings["loopCount"] = 2;
    settings["endBehavior"] = 2;
    settings["takeoffHeight"] = 2.0;
    QVERIFY(editor->setSimulationSettings(settings));
    QVERIFY(panel->setProperty("activePanel", -1));
    QVERIFY(map->setProperty("center", QVariant::fromValue(QGeoCoordinate(30.001, 120.001))));
    QVERIFY(map->setProperty("zoomLevel", 16.0));
    const auto green = _evaluateColor(map, QStringLiteral("QGroundControl.globalPalette.colorGreen"));
    const auto orange = _evaluateColor(map, QStringLiteral("QGroundControl.globalPalette.mapMissionTrajectory"));
    QVERIFY(green.isValid() && orange.isValid() && green != orange);
    QString diagnostic;
    const auto legs = [&](const QColor& color) -> QList<ExpectedLeg> {
        return {{QStringLiteral("start"), home, first, color},     {QStringLiteral("body"), first, second, color},
                {QStringLiteral("body"), second, third, color},    {QStringLiteral("body"), third, fourth, color},
                {QStringLiteral("closure"), fourth, first, color}, {QStringLiteral("return"), first, home, color}};
    };
    QTRY_VERIFY_WITH_TIMEOUT(panel->property("simulationPreviewActive").toBool(), TestTimeout::mediumMs());
    QTRY_VERIFY2_WITH_TIMEOUT(_previewColorsMatch(map, panel, legs(orange), diagnostic), qPrintable(diagnostic),
                              TestTimeout::mediumMs());
    auto* simulation = plugin->simulation();
    QVERIFY(simulation);
    const auto stop = qScopeGuard([simulation]() { simulation->stop(); });
    _allowUnavailablePressureBackend();
    QVERIFY(simulation->start(master));
    QTRY_VERIFY_WITH_TIMEOUT(simulation->vehicle() && simulation->vehicle()->isInitialConnectComplete(),
                             TestTimeout::longMs());
    QTRY_VERIFY_WITH_TIMEOUT(!master->syncInProgress(), TestTimeout::mediumMs());
    QSignalSpy uploaded(simulation, &CaacSimulationController::missionUploaded);
    QVERIFY(uploaded.isValid());
    QVERIFY(simulation->upload(master));
    QTRY_COMPARE_WITH_TIMEOUT(uploaded.count(), 1, TestTimeout::longMs());
    QVERIFY(simulation->missionReady());
    QTRY_VERIFY2_WITH_TIMEOUT(_previewColorsMatch(map, panel, legs(green), diagnostic), qPrintable(diagnostic),
                              TestTimeout::mediumMs());

    const QGeoCoordinate middle(30.0005, 120.0013);
    auto* inserted = qobject_cast<SimpleMissionItem*>(mission->insertSimpleMissionItem(middle, 2, false));
    QVERIFY(inserted);
    inserted->altitude()->setRawValue(2);
    const QList<ExpectedLeg> insertedLegs{
        {QStringLiteral("start"), home, first, green},    {QStringLiteral("body"), first, middle, orange},
        {QStringLiteral("body"), middle, second, orange}, {QStringLiteral("body"), second, third, green},
        {QStringLiteral("body"), third, fourth, green},   {QStringLiteral("closure"), fourth, first, green},
        {QStringLiteral("return"), first, home, green}};
    QTRY_VERIFY2_WITH_TIMEOUT(_previewColorsMatch(map, panel, insertedLegs, diagnostic), qPrintable(diagnostic),
                              TestTimeout::mediumMs());
    originalThird->altitude()->setRawValue(3);
    auto changedHeight = insertedLegs;
    changedHeight[3].color = orange;
    changedHeight[4].color = orange;
    QTRY_VERIFY2_WITH_TIMEOUT(_previewColorsMatch(map, panel, changedHeight, diagnostic), qPrintable(diagnostic),
                              TestTimeout::mediumMs());
    mission->setDirty(false);
    QTemporaryDir directory;
    QVERIFY(directory.isValid());
    QVERIFY(master->saveToFile(directory.filePath(QStringLiteral("preview-edits.plan"))));
    QTRY_VERIFY2_WITH_TIMEOUT(_previewColorsMatch(map, panel, changedHeight, diagnostic), qPrintable(diagnostic),
                              TestTimeout::mediumMs());
    QCOMPARE(uploaded.count(), 1);
}

UT_REGISTER_TEST(CaacRouteUploadUITest, TestLabel::Integration, TestLabel::MissionManager)

#include "CaacRouteUploadUITest.moc"
