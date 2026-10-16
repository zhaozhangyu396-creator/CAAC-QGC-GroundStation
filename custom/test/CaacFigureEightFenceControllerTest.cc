#include <limits>

#include <QtCore/QFile>
#include <QtCore/QJsonArray>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>
#include <QtCore/QSettings>
#include <QtTest/QSignalSpy>

#include "CaacFigureEightController.h"
#include "CaacFigureEightFenceController.h"
#include "Fact.h"
#include "FactMetaData.h"
#include "GeoFenceManager.h"
#include "LinkInterface.h"
#include "MissionManager.h"
#include "ParameterManager.h"
#include "QGCGeo.h"
#include "QmlObjectListModel.h"
#include "Vehicle.h"
#include "VehicleTestManualConnect.h"

class CaacFigureEightFenceControllerTest : public VehicleTestManualConnect
{
    Q_OBJECT

private slots:
    void _settingsAndOffline();
    void _nativeFirmwareCannotEnable_data();
    void _nativeFirmwareCannotEnable();
    void _customFirmwarePolicyIsReadOnly_data();
    void _customFirmwarePolicyIsReadOnly();
    void _transferReadbackAndInvalidation();
    void _preparationBeforeGeometry();
    void _armingStopsFollowingSteps();
    void _existingFenceRequiresBackupAndConfirmation();
    void _activationFailureDoesNotReportApplied();
    void _armedAndVehicleSwitch();

private:
    void _resetSettings();
    void _setSite(CaacFigureEightController& controller);
    void _useTestPolicy(CaacFigureEightFenceController* fence);
    void _writeFixtureParameter(Fact* fact, const QVariant& value);
    Fact* _parameterFixture(const QString& name, int value, MAV_PARAM_TYPE wireType = MAV_PARAM_TYPE_INT32);
};

void CaacFigureEightFenceControllerTest::_resetSettings()
{
    QSettings settings;
    settings.remove(QStringLiteral("CAAC/FigureEight"));
    settings.remove(QStringLiteral("CAAC/FigureEightFence"));
}

void CaacFigureEightFenceControllerTest::_setSite(CaacFigureEightController& controller)
{
    const QGeoCoordinate a(30, 120);
    QVERIFY(controller.setPoints(a, QGCGeo::geodesicDestination(a, 90, 24)));
}

void CaacFigureEightFenceControllerTest::_useTestPolicy(CaacFigureEightFenceController* fence)
{
    QVERIFY(fence);
    // This verifies transport with an opaque future firmware policy, not native PX4/AP compliance.
    fence->_testPolicy.description = QStringLiteral("Test firmware fence policy");
    fence->_testPolicy.activation = {{QStringLiteral("GF_ACTION"), 2}};
}

void CaacFigureEightFenceControllerTest::_writeFixtureParameter(Fact* fact, const QVariant& value)
{
    QVERIFY(vehicle());
    QVERIFY(fact);
    auto* parameters = vehicle()->parameterManager();
    QSignalSpy echoed(fact, &Fact::vehicleUpdated);
    QSignalSpy written(parameters, &ParameterManager::_paramSetSuccess);
    QVERIFY(echoed.isValid());
    QVERIFY(written.isValid());
    fact->forceSetRawValue(value);
    QVERIFY_SIGNAL_COUNT_WAIT(written, 1, TestTimeout::longMs());
    QCOMPARE(written.first()[1].toString(), fact->name());
    QVERIFY(!echoed.isEmpty());
    QCOMPARE(echoed.last().first().toDouble(), value.toDouble());
    QCOMPARE(mockLink()->paramValue(fact->componentId(), fact->name()).toDouble(), value.toDouble());
    QTRY_VERIFY_WITH_TIMEOUT(!parameters->pendingWrites(), TestTimeout::longMs());
}

Fact* CaacFigureEightFenceControllerTest::_parameterFixture(const QString& name, int value, MAV_PARAM_TYPE wireType)
{
    auto* currentVehicle = vehicle();
    if (!currentVehicle) {
        return nullptr;
    }
    auto* parameters = currentVehicle->parameterManager();
    if (!parameters->parameterExists(-1, name)) {
        mavlink_param_union_t encoded{};
        if (wireType == MAV_PARAM_TYPE_INT8) {
            encoded.param_int8 = value;
        } else if (wireType == MAV_PARAM_TYPE_REAL32) {
            encoded.param_float = value;
        } else {
            encoded.param_int32 = value;
        }
        mavlink_message_t message{};
        const auto bytes = name.toLatin1();
        mavlink_msg_param_value_pack(currentVehicle->id(), currentVehicle->compId(), &message, bytes.constData(),
                                     encoded.param_float, wireType, parameters->parameterNames(-1).size(), 65535);
        parameters->mavlinkMessageReceived(message);
    }
    if (!parameters->parameterExists(-1, name)) {
        return nullptr;
    }
    auto* fact = parameters->getParameter(-1, name);
    fact->containerSetRawValue(value);
    return fact;
}

void CaacFigureEightFenceControllerTest::_settingsAndOffline()
{
    _resetSettings();
    CaacFigureEightController controller;
    auto* fence = controller.fence();
    QVERIFY(fence);
    QCOMPARE(fence->margin(), 3);
    QVERIFY(fence->path().isEmpty());
    QVERIFY(!fence->canApply());
    QVERIFY(!fence->apply());
    QVERIFY(!fence->applied());
    _setSite(controller);
    QVERIFY(fence->path().size() >= 3);
    const auto previous = fence->path();
    fence->setMargin(5);
    QVERIFY(fence->path() != previous);
    fence->setMargin(std::numeric_limits<double>::quiet_NaN());
    fence->setMargin(-1);
    fence->setMargin(101);
    QCOMPARE(fence->margin(), 5);
    fence->setShowOnMap(false);
    QVERIFY(!fence->showOnMap());
    CaacFigureEightController restored;
    QCOMPARE(restored.fence()->margin(), 5);
    QVERIFY(!restored.fence()->showOnMap());
    QVERIFY(!restored.fence()->applied());
}

void CaacFigureEightFenceControllerTest::_nativeFirmwareCannotEnable_data()
{
    TestData::addAutopilotRows();
}

void CaacFigureEightFenceControllerTest::_nativeFirmwareCannotEnable()
{
    UT_FETCH_AUTOPILOT();
    Q_UNUSED(autopilotName);
    if (autopilot == MAV_AUTOPILOT_ARDUPILOTMEGA && !apmFirmwareSupported()) {
        QSKIP("ArduPilot not included in this build");
    }
    _resetSettings();
    _connectMockLink(autopilot);
    QVERIFY(vehicle());
    QVERIFY(waitForParametersReady());
    CaacFigureEightController controller;
    _setSite(controller);
    auto* fence = controller.fence();
    QVERIFY(!fence->actionDescription().isEmpty());
    QVERIFY(!fence->canApply());
    mockLink()->clearReceivedMavlinkMessageCounts();
    QVERIFY(!fence->apply());
    QCOMPARE(mockLink()->receivedMavlinkMessageCount(MAVLINK_MSG_ID_MISSION_COUNT), 0);
    QCOMPARE(mockLink()->receivedMavlinkMessageCount(MAVLINK_MSG_ID_PARAM_SET), 0);
    QVERIFY(!fence->applied());
    QVERIFY(fence->canRead());
    fence->readFromVehicle();
    QTRY_VERIFY_WITH_TIMEOUT(!fence->busy(), TestTimeout::longMs());
    QVERIFY(!fence->applied());
    QCOMPARE(mockLink()->receivedMavlinkMessageCount(MAVLINK_MSG_ID_PARAM_SET), 0);
}

void CaacFigureEightFenceControllerTest::_customFirmwarePolicyIsReadOnly_data()
{
    QTest::addColumn<MAV_AUTOPILOT>("autopilot");
    QTest::addColumn<QString>("autopilotName");
    QTest::addColumn<bool>("wrongType");
    QTest::newRow("PX4") << MAV_AUTOPILOT_PX4 << QStringLiteral("PX4") << false;
    QTest::newRow("ArduPilot") << MAV_AUTOPILOT_ARDUPILOTMEGA << QStringLiteral("ArduPilot") << false;
    QTest::newRow("PX4 wrong marker type") << MAV_AUTOPILOT_PX4 << QStringLiteral("PX4") << true;
    QTest::newRow("ArduPilot wrong marker type") << MAV_AUTOPILOT_ARDUPILOTMEGA << QStringLiteral("ArduPilot") << true;
}

void CaacFigureEightFenceControllerTest::_customFirmwarePolicyIsReadOnly()
{
    UT_FETCH_AUTOPILOT();
    QFETCH(bool, wrongType);
    Q_UNUSED(autopilotName);
    if (autopilot == MAV_AUTOPILOT_ARDUPILOTMEGA && !apmFirmwareSupported()) {
        QSKIP("ArduPilot not included in this build");
    }
    _resetSettings();
    _connectMockLink(autopilot);
    QVERIFY(vehicle());
    QVERIFY(waitForParametersReady());
    const bool px4 = autopilot == MAV_AUTOPILOT_PX4;
    const auto wireType = wrongType ? MAV_PARAM_TYPE_REAL32 : px4 ? MAV_PARAM_TYPE_INT32 : MAV_PARAM_TYPE_INT8;
    auto* noLand = _parameterFixture(px4 ? QStringLiteral("GF_NOLAND") : QStringLiteral("FENCE_NOLD"), 0, wireType);
    auto* action = _parameterFixture(px4 ? QStringLiteral("GF_ACTION") : QStringLiteral("FENCE_ACTION"), 0);
    QVERIFY(noLand);
    QVERIFY(action);
    noLand->setEnumInfo({QStringLiteral("Disabled"), QStringLiteral("Invalid")}, {0, 2});
    action->setEnumInfo({QStringLiteral("Disabled"), QStringLiteral("Brake")}, {0, px4 ? 2 : 4});
    Fact* type = nullptr;
    Fact* enable = nullptr;
    if (!px4) {
        type = _parameterFixture(QStringLiteral("FENCE_TYPE"), 3);
        enable = _parameterFixture(QStringLiteral("FENCE_ENABLE"), 0);
        QVERIFY(type);
        QVERIFY(enable);
        type->metaData()->setBitmaskInfo(
            {QStringLiteral("Height"), QStringLiteral("Circle"), QStringLiteral("Polygon")}, {1, 2, 4});
        enable->setEnumInfo({QStringLiteral("Disabled"), QStringLiteral("Enabled")}, {0, 1});
    }
    auto* plugin = vehicle()->firmwarePlugin();
    mockLink()->clearReceivedMavlinkMessageCounts();
    QVERIFY(plugin->trainingGeoFencePolicy(vehicle()).description.isEmpty());
    noLand->setEnumInfo({QStringLiteral("Disabled"), QStringLiteral("Enabled")}, {0, 1});
    if (wrongType) {
        QVERIFY(plugin->trainingGeoFencePolicy(vehicle()).description.isEmpty());
        QCOMPARE(mockLink()->receivedMavlinkMessageCount(MAVLINK_MSG_ID_PARAM_SET), 0);
        return;
    }
    const auto policy = plugin->trainingGeoFencePolicy(vehicle());
    QVERIFY2(!policy.description.isEmpty(), qPrintable(policy.unsupportedReason));
    QVERIFY(policy.unsupportedReason.isEmpty());
    QCOMPARE(policy.preparation.first().first, noLand->name());
    QCOMPARE(policy.preparation.first().second.toInt(), 1);
    QCOMPARE(policy.preparation[1].first, action->name());
    QCOMPARE(policy.preparation[1].second.toInt(), px4 ? 2 : 4);
    QCOMPARE(policy.activation.size(), 1);
    if (px4) {
        QCOMPARE(policy.preparation.size(), 2);
        QCOMPARE(policy.activation.first().first, action->name());
        QCOMPARE(policy.activation.first().second.toInt(), 2);
    } else {
        QCOMPARE(policy.preparation.size(), 3);
        QCOMPARE(policy.preparation[2].first, type->name());
        QCOMPARE(policy.preparation[2].second.toInt(), 7);
        QCOMPARE(policy.activation.first().first, enable->name());
        QCOMPARE(policy.activation.first().second.toInt(), 1);
        QCOMPARE(type->rawValue().toInt(), 3);
        QCOMPARE(enable->rawValue().toInt(), 0);
    }
    QCOMPARE(noLand->rawValue().toInt(), 0);
    QCOMPARE(action->rawValue().toInt(), 0);
    noLand->setEnumInfo({}, {});
    QVERIFY(!plugin->trainingGeoFencePolicy(vehicle()).description.isEmpty());
    noLand->containerSetRawValue(2);
    QVERIFY(plugin->trainingGeoFencePolicy(vehicle()).description.isEmpty());
    noLand->containerSetRawValue(0);
    noLand->setEnumInfo({QStringLiteral("Disabled")}, {0});
    QVERIFY(plugin->trainingGeoFencePolicy(vehicle()).description.isEmpty());
    noLand->setEnumInfo({QStringLiteral("Disabled"), QStringLiteral("Enabled")}, {0, 1});
    noLand->metaData()->setReadOnly(true);
    QVERIFY(plugin->trainingGeoFencePolicy(vehicle()).description.isEmpty());
    noLand->metaData()->setReadOnly(false);
    QCOMPARE(mockLink()->receivedMavlinkMessageCount(MAVLINK_MSG_ID_PARAM_SET), 0);
    QCOMPARE(mockLink()->receivedMavlinkMessageCount(MAVLINK_MSG_ID_MISSION_COUNT), 0);
}

void CaacFigureEightFenceControllerTest::_transferReadbackAndInvalidation()
{
    _resetSettings();
    _connectMockLink();
    QVERIFY(vehicle());
    QVERIFY(waitForParametersReady());
    CaacFigureEightController controller;
    _setSite(controller);
    auto* fence = controller.fence();
    _useTestPolicy(fence);
    auto* action = vehicle()->parameterManager()->getParameter(-1, QStringLiteral("GF_ACTION"));
    _writeFixtureParameter(action, 0);
    QCOMPARE(action->rawValue().toInt(), 0);
    const auto mission = vehicle()->missionManager()->missionItems();
    const auto mode = vehicle()->flightMode();
    bool geometryAcknowledged = false;
    bool parametersWrittenBeforeGeometry = false;
    connect(vehicle()->geoFenceManager(), &GeoFenceManager::sendComplete, &controller,
            [&geometryAcknowledged](bool error) { geometryAcknowledged = !error; });
    connect(action, &Fact::containerRawValueChanged, &controller,
            [&](const QVariant&) { parametersWrittenBeforeGeometry |= !geometryAcknowledged; });
    QVERIFY(fence->canApply());
    QVERIFY(fence->apply());
    QVERIFY(fence->busy());
    QVERIFY(!fence->applied());
    QTRY_VERIFY_WITH_TIMEOUT(!fence->busy(), TestTimeout::longMs());
    QVERIFY2(fence->applied(), qPrintable(fence->status()));
    QVERIFY(geometryAcknowledged);
    QVERIFY(!parametersWrittenBeforeGeometry);
    QCOMPARE(action->rawValue().toInt(), 2);
    QCOMPARE(mockLink()->paramValue(vehicle()->compId(), QStringLiteral("GF_ACTION")).toInt(), 2);
    QCOMPARE(vehicle()->geoFenceManager()->polygons().size(), 1);
    QCOMPARE(vehicle()->geoFenceManager()->polygons().first().count(), fence->path().size());
    QCOMPARE(vehicle()->missionManager()->missionItems(), mission);
    QCOMPARE(vehicle()->flightMode(), mode);
    QVERIFY(QFile::exists(fence->backupFile()));
    fence->readFromVehicle();
    QTRY_VERIFY_WITH_TIMEOUT(!fence->busy(), TestTimeout::longMs());
    QVERIFY2(fence->applied(), qPrintable(fence->status()));
    QVERIFY(fence->apply());
    QTRY_VERIFY_WITH_TIMEOUT(!fence->busy(), TestTimeout::longMs());
    QVERIFY(fence->applied());
    QVERIFY(!fence->replacementRequired());
    controller.setViewActive(false);
    QVERIFY(fence->applied());
    controller.clearTrajectory();
    QVERIFY(fence->applied());
    QSignalSpy actionWritten(vehicle()->parameterManager(), &ParameterManager::_paramSetSuccess);
    QVERIFY(actionWritten.isValid());
    action->setRawValue(0);
    QVERIFY(!fence->applied());
    QVERIFY_SIGNAL_COUNT_WAIT(actionWritten, 1, TestTimeout::longMs());
    QCOMPARE(actionWritten.first()[1].toString(), action->name());
    QCOMPARE(mockLink()->paramValue(action->componentId(), action->name()).toInt(), 0);
    QTRY_VERIFY_WITH_TIMEOUT(!vehicle()->parameterManager()->pendingWrites(), TestTimeout::longMs());
    fence->setMargin(5);
    QVERIFY(!fence->applied());
}

void CaacFigureEightFenceControllerTest::_preparationBeforeGeometry()
{
    _resetSettings();
    _connectMockLink();
    QVERIFY(vehicle());
    QVERIFY(waitForParametersReady());
    CaacFigureEightController controller;
    _setSite(controller);
    auto* fence = controller.fence();
    _useTestPolicy(fence);
    fence->_testPolicy.preparation = {{QStringLiteral("GF_MAX_HOR_DIST"), 1.0}, {QStringLiteral("GF_ACTION"), 2}};
    auto* preparation = vehicle()->parameterManager()->getParameter(-1, QStringLiteral("GF_MAX_HOR_DIST"));
    _writeFixtureParameter(preparation, 0.0);
    bool preparationEchoed = false;
    bool geometryBeforePreparation = false;
    connect(preparation, &Fact::vehicleUpdated, &controller,
            [&](const QVariant& value) { preparationEchoed |= value.toDouble() == 1.0; });
    connect(fence, &CaacFigureEightFenceController::stateChanged, &controller, [&]() {
        if (fence->_stage == CaacFigureEightFenceController::Stage::Upload) {
            geometryBeforePreparation |= !preparationEchoed;
        }
    });
    QVERIFY(fence->apply());
    QTRY_VERIFY_WITH_TIMEOUT(!fence->busy(), TestTimeout::longMs());
    QVERIFY2(fence->applied(), qPrintable(fence->status()));
    QVERIFY(preparationEchoed);
    QVERIFY(!geometryBeforePreparation);
}

void CaacFigureEightFenceControllerTest::_armingStopsFollowingSteps()
{
    _resetSettings();
    _connectMockLink();
    QVERIFY(vehicle());
    QVERIFY(waitForParametersReady());
    CaacFigureEightController controller;
    _setSite(controller);
    auto* fence = controller.fence();
    _useTestPolicy(fence);
    fence->_testPolicy.preparation = {{QStringLiteral("GF_MAX_HOR_DIST"), 1.0},
                                      {QStringLiteral("GF_MAX_VER_DIST"), 2.0}};
    auto* preparation = vehicle()->parameterManager()->getParameter(-1, QStringLiteral("GF_MAX_HOR_DIST"));
    auto* following = vehicle()->parameterManager()->getParameter(-1, QStringLiteral("GF_MAX_VER_DIST"));
    _writeFixtureParameter(preparation, 0.0);
    _writeFixtureParameter(following, 0.0);
    bool armInjected = false;
    connect(preparation, &Fact::vehicleUpdated, &controller, [&](const QVariant& value) {
        if (armInjected || value.toDouble() != 1.0) {
            return;
        }
        armInjected = true;
        mavlink_message_t heartbeat{};
        mavlink_msg_heartbeat_pack(vehicle()->id(), vehicle()->compId(), &heartbeat, MAV_TYPE_QUADROTOR,
                                   MAV_AUTOPILOT_PX4, MAV_MODE_FLAG_SAFETY_ARMED, 0, MAV_STATE_ACTIVE);
        QVERIFY(QMetaObject::invokeMethod(vehicle(), "_mavlinkMessageReceived", Qt::DirectConnection,
                                          Q_ARG(LinkInterface*, mockLink()), Q_ARG(mavlink_message_t, heartbeat)));
    });
    mockLink()->clearReceivedMavlinkMessageCounts();
    QVERIFY(fence->apply());
    QTRY_VERIFY_WITH_TIMEOUT(!fence->busy(), TestTimeout::longMs());
    QVERIFY(armInjected);
    QVERIFY(!fence->applied());
    QVERIFY(fence->status().contains(QStringLiteral("停止后续配置")));
    QCOMPARE(following->rawValue().toDouble(), 0.0);
    QCOMPARE(mockLink()->receivedMavlinkMessageCount(MAVLINK_MSG_ID_MISSION_COUNT), 0);
}

void CaacFigureEightFenceControllerTest::_existingFenceRequiresBackupAndConfirmation()
{
    _resetSettings();
    _connectMockLink();
    QVERIFY(vehicle());
    QVERIFY(waitForParametersReady());
    auto* manager = vehicle()->geoFenceManager();
    QGCFencePolygon existing(true);
    existing.appendVertex(QGeoCoordinate(30, 120));
    existing.appendVertex(QGeoCoordinate(30.001, 120));
    existing.appendVertex(QGeoCoordinate(30, 120.001));
    QmlObjectListModel polygons;
    QmlObjectListModel circles;
    polygons.append(&existing);
    QSignalSpy sent(manager, &GeoFenceManager::sendComplete);
    QVERIFY(sent.isValid());
    manager->sendToVehicle(QGeoCoordinate(), polygons, circles);
    QVERIFY_SIGNAL_COUNT_WAIT(sent, 1, TestTimeout::longMs());
    QVERIFY(!sent.first().first().toBool());
    polygons.clear();
    CaacFigureEightController controller;
    _setSite(controller);
    auto* fence = controller.fence();
    _useTestPolicy(fence);
    const int previousSends = sent.size();
    mockLink()->clearReceivedMavlinkMessageCounts();
    QVERIFY(fence->apply());
    QTRY_VERIFY_WITH_TIMEOUT(!fence->busy(), TestTimeout::longMs());
    QVERIFY(fence->replacementRequired());
    QVERIFY(!fence->applied());
    QCOMPARE(sent.size(), previousSends);
    QCOMPARE(mockLink()->receivedMavlinkMessageCount(MAVLINK_MSG_ID_PARAM_SET), 0);
    QCOMPARE(manager->polygons().first().count(), 3);
    QVERIFY(fence->apply(true));
    QTRY_VERIFY_WITH_TIMEOUT(!fence->busy(), TestTimeout::longMs());
    QVERIFY2(fence->applied(), qPrintable(fence->status()));
    QFile backup(fence->backupFile());
    QVERIFY(backup.open(QIODevice::ReadOnly));
    const auto object = QJsonDocument::fromJson(backup.readAll()).object();
    QCOMPARE(object.value(QStringLiteral("polygons")).toArray().size(), 1);
    QVERIFY(object.value(QStringLiteral("parameters")).toObject().contains(QStringLiteral("GF_ACTION")));
    QCOMPARE(manager->polygons().first().count(), fence->path().size());
}

void CaacFigureEightFenceControllerTest::_activationFailureDoesNotReportApplied()
{
    _resetSettings();
    _connectMockLink();
    QVERIFY(vehicle());
    QVERIFY(waitForParametersReady());
    auto* action = vehicle()->parameterManager()->getParameter(-1, QStringLiteral("GF_ACTION"));
    _writeFixtureParameter(action, 0);
    CaacFigureEightController controller;
    _setSite(controller);
    auto* fence = controller.fence();
    _useTestPolicy(fence);
    QSignalSpy reads(vehicle()->parameterManager(), &ParameterManager::_paramRequestReadSuccess);
    QVERIFY(reads.isValid());
    mockLink()->setParamSetFailureMode(MockLink::FailParamSetParamError);
    expectAppMessage(QRegularExpression(QStringLiteral("Parameter write failed")));
    QVERIFY(fence->apply());
    QTRY_VERIFY_WITH_TIMEOUT(!fence->busy(), TestTimeout::longMs());
    QVERIFY(!fence->applied());
    QVERIFY(fence->status().contains(QStringLiteral("围栏已写入，启用未确认")));
    QCOMPARE(vehicle()->geoFenceManager()->polygons().size(), 1);
    mockLink()->setParamSetFailureMode(MockLink::FailParamSetNone);
    QVERIFY_SIGNAL_COUNT_WAIT(reads, 2, TestTimeout::longMs());
    QCOMPARE(action->rawValue().toInt(), 0);
    QTRY_VERIFY_WITH_TIMEOUT(!vehicle()->parameterManager()->pendingWrites(), TestTimeout::longMs());
    verifyExpectedLogMessage();
}

void CaacFigureEightFenceControllerTest::_armedAndVehicleSwitch()
{
    _resetSettings();
    _connectMockLink();
    QVERIFY(vehicle());
    QVERIFY(waitForParametersReady());
    CaacFigureEightController controller;
    _setSite(controller);
    auto* fence = controller.fence();
    _useTestPolicy(fence);
    QVERIFY(fence->canApply());
    mavlink_message_t heartbeat{};
    mavlink_msg_heartbeat_pack(vehicle()->id(), vehicle()->compId(), &heartbeat, MAV_TYPE_QUADROTOR, MAV_AUTOPILOT_PX4,
                               MAV_MODE_FLAG_SAFETY_ARMED, 0, MAV_STATE_ACTIVE);
    QVERIFY(QMetaObject::invokeMethod(vehicle(), "_mavlinkMessageReceived", Qt::DirectConnection,
                                      Q_ARG(LinkInterface*, mockLink()), Q_ARG(mavlink_message_t, heartbeat)));
    QVERIFY(vehicle()->armed());
    QVERIFY(!fence->canApply());
    QVERIFY(!fence->apply());
    QVERIFY(fence->canRead());
    MultiVehicleManager::instance()->setActiveVehicle(nullptr);
    QTRY_COMPARE_WITH_TIMEOUT(MultiVehicleManager::instance()->activeVehicle(), nullptr, TestTimeout::mediumMs());
    QVERIFY(!fence->canApply());
    QVERIFY(!fence->applied());
    QVERIFY(!fence->apply());
    MultiVehicleManager::instance()->setActiveVehicle(vehicle());
    QTRY_COMPARE_WITH_TIMEOUT(MultiVehicleManager::instance()->activeVehicle(), vehicle(), TestTimeout::mediumMs());
}

UT_REGISTER_TEST(CaacFigureEightFenceControllerTest, TestLabel::Unit, TestLabel::Vehicle)

#include "CaacFigureEightFenceControllerTest.moc"
