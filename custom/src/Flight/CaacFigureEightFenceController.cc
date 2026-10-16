#include "CaacFigureEightFenceController.h"

#include <cmath>
#include <utility>

#include <QtCore/QDateTime>
#include <QtCore/QDir>
#include <QtCore/QJsonArray>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>
#include <QtCore/QSaveFile>
#include <QtCore/QSettings>
#include <QtCore/QStandardPaths>
#include <QtCore/QUuid>

#include "CaacFigureEightController.h"
#include "CaacFigureEightFenceGeometry.h"
#include "Fact.h"
#include "GeoFenceManager.h"
#include "MultiVehicleManager.h"
#include "ParameterManager.h"
#include "QGCGeo.h"
#include "QmlObjectListModel.h"
#include "Vehicle.h"
#include "VehicleLinkManager.h"

namespace {
constexpr auto SETTINGS_GROUP = "CAAC/FigureEightFence";
constexpr int TRANSACTION_TIMEOUT_MS = 15000;
constexpr double POSITION_TOLERANCE_METERS = 0.03;

bool sameValue(const QVariant& left, const QVariant& right)
{
    bool leftValid = false;
    bool rightValid = false;
    const double leftNumber = left.toDouble(&leftValid);
    const double rightNumber = right.toDouble(&rightValid);
    return leftValid && rightValid && std::isfinite(leftNumber) && std::isfinite(rightNumber) &&
           std::abs(leftNumber - rightNumber) < 1e-6;
}
}  // namespace

CaacFigureEightFenceController::CaacFigureEightFenceController(CaacFigureEightController* provider)
    : QObject(provider)
    , _provider(provider)
    , _status(tr("围栏尚未应用到飞控。"))
{
    QSettings settings;
    settings.beginGroup(QLatin1String(SETTINGS_GROUP));
    const double savedMargin = settings.value(QStringLiteral("margin"), 3.0).toDouble();
    _margin = std::isfinite(savedMargin) && savedMargin >= 0 && savedMargin <= 100 ? savedMargin : 3;
    _showOnMap = settings.value(QStringLiteral("showOnMap"), true).toBool();
    _timeout.setSingleShot(true);
    _timeout.setInterval(TRANSACTION_TIMEOUT_MS);
    connect(&_timeout, &QTimer::timeout, this, [this]() { _fail(tr("等待飞控确认超时，请重新读取围栏状态。")); });
    if (_provider) {
        connect(_provider, &CaacFigureEightController::siteChanged, this,
                &CaacFigureEightFenceController::_updateGeometry);
        connect(_provider, &CaacFigureEightController::settingsChanged, this,
                &CaacFigureEightFenceController::_updateGeometry);
    }
    auto* manager = MultiVehicleManager::instance();
    connect(manager, &MultiVehicleManager::activeVehicleChanged, this, &CaacFigureEightFenceController::_setVehicle);
    _setVehicle(manager->activeVehicle());
    _updateGeometry();
}

void CaacFigureEightFenceController::setMargin(double margin)
{
    if (!std::isfinite(margin) || margin < 0 || margin > 100 || qFuzzyCompare(_margin, margin)) {
        return;
    }
    _margin = margin;
    _saveSettings();
    _updateGeometry();
    emit geometryChanged();
}

QVariantList CaacFigureEightFenceController::path() const
{
    QVariantList result;
    result.reserve(_path.size());
    for (const auto& coordinate : _path) {
        result.append(QVariant::fromValue(coordinate));
    }
    return result;
}

void CaacFigureEightFenceController::setShowOnMap(bool show)
{
    if (_showOnMap == show) {
        return;
    }
    _showOnMap = show;
    _saveSettings();
    emit geometryChanged();
}

bool CaacFigureEightFenceController::canApply() const
{
    return !busy() && _blockReason().isEmpty();
}

bool CaacFigureEightFenceController::canRead() const
{
    return !busy() && _blockReason(false).isEmpty();
}

QString CaacFigureEightFenceController::actionDescription() const
{
    const auto policy = _vehiclePolicy();
    return policy.description.isEmpty() ? policy.unsupportedReason : policy.description;
}

FirmwarePlugin::GeoFencePolicy CaacFigureEightFenceController::_vehiclePolicy() const
{
#ifdef QGC_UNITTEST_BUILD
    if (!_testPolicy.description.isEmpty()) {
        return _testPolicy;
    }
#endif
    auto* vehicle = _vehicle.data();
    return vehicle ? vehicle->firmwarePlugin()->trainingGeoFencePolicy(vehicle) : FirmwarePlugin::GeoFencePolicy();
}

QString CaacFigureEightFenceController::_blockReason(bool requirePolicy) const
{
    auto* vehicle = _vehicle.data();
    if (_path.size() < 3) {
        return tr("请先完成 A、B 点标定。");
    }
    if (!vehicle || !vehicle->isInitialConnectComplete() || vehicle->vehicleLinkManager()->primaryLink().expired() ||
        vehicle->vehicleLinkManager()->communicationLost()) {
        return tr("请连接无人机并等待连接完成。");
    }
    if (requirePolicy && (vehicle->armed() || vehicle->airborne())) {
        return tr("请在无人机落地并锁定后设置围栏。");
    }
    auto* parameters = vehicle->parameterManager();
    if (!parameters->parametersReady() || (requirePolicy && parameters->pendingWrites())) {
        return tr("请等待飞控参数读取或写入完成。");
    }
    auto* manager = vehicle->geoFenceManager();
    if (!manager || !manager->supported()) {
        return tr("当前飞控不支持多边形电子围栏。");
    }
    if (manager->inProgress()) {
        return tr("飞控正在传输围栏，请稍后重试。");
    }
    const auto policy = _vehiclePolicy();
    if (requirePolicy && (policy.description.isEmpty() || policy.activation.isEmpty())) {
        return policy.unsupportedReason.isEmpty() ? tr("当前飞控尚不支持本训练围栏的限制动作。")
                                                  : policy.unsupportedReason;
    }
    return {};
}

void CaacFigureEightFenceController::_setVehicle(Vehicle* vehicle)
{
    if (_vehicle == vehicle) {
        return;
    }
    _invalidate(tr("已切换无人机，需重新读取或应用围栏。"));
    for (const auto& connection : std::as_const(_vehicleConnections)) {
        disconnect(connection);
    }
    _vehicleConnections.clear();
    _vehicle = vehicle;
    if (vehicle) {
        const auto update = [this]() { _updateVehicleState(); };
        _vehicleConnections.append(connect(vehicle, &Vehicle::armedChanged, this, update));
        _vehicleConnections.append(connect(vehicle, &Vehicle::airborneChanged, this, update));
        _vehicleConnections.append(connect(vehicle, &Vehicle::initialConnectComplete, this, update));
        _vehicleConnections.append(connect(vehicle, &QObject::destroyed, this,
                                           [this]() { _invalidate(tr("无人机已断开，围栏状态需重新确认。")); }));
        auto* links = vehicle->vehicleLinkManager();
        _vehicleConnections.append(connect(links, &VehicleLinkManager::communicationLostChanged, this, update));
        _vehicleConnections.append(connect(links, &VehicleLinkManager::allLinksRemoved, this,
                                           [this]() { _invalidate(tr("无人机已断开，围栏状态需重新确认。")); }));
        auto* parameters = vehicle->parameterManager();
        _vehicleConnections.append(connect(parameters, &ParameterManager::parametersReadyChanged, this, update));
        _vehicleConnections.append(connect(parameters, &ParameterManager::pendingWritesChanged, this, update));
        _vehicleConnections.append(
            connect(parameters, &ParameterManager::_paramSetFailure, this, [this](int, const QString& name) {
                if ((_stage == Stage::WritePreparation || _stage == Stage::WriteParameters) &&
                    _parameterIndex < _parameterSteps.size() && _parameterSteps[_parameterIndex].first == name) {
                    _fail(tr("飞控未确认围栏参数 %1，请重新读取状态。").arg(name));
                }
            }));
        auto* manager = vehicle->geoFenceManager();
        _vehicleConnections.append(
            connect(manager, &GeoFenceManager::loadComplete, this, &CaacFigureEightFenceController::_loaded));
        _vehicleConnections.append(
            connect(manager, &GeoFenceManager::sendComplete, this, &CaacFigureEightFenceController::_sent));
        _vehicleConnections.append(connect(manager, &GeoFenceManager::inProgressChanged, this, update));
        _vehicleConnections.append(connect(manager, &GeoFenceManager::removeAllComplete, this,
                                           [this](bool) { _invalidate(tr("飞控围栏已被清除，请重新读取确认。")); }));
        _vehicleConnections.append(connect(manager, &GeoFenceManager::error, this, [this](int, const QString& error) {
            if (busy()) {
                _fail(tr("围栏传输失败：%1").arg(error));
            }
        }));
    }
    emit stateChanged();
}

void CaacFigureEightFenceController::_updateGeometry()
{
    const auto updated = _provider ? CaacFigureEightFenceGeometry::boundary(_provider->pointA(), _provider->pointB(),
                                                                            _provider->fenceWidth(), _margin)
                                   : QList<QGeoCoordinate>();
    if (_path == updated) {
        return;
    }
    _path = updated;
    _invalidate(tr("场地或围栏尺寸已变化，请重新应用到飞控。"));
    _replacementRequired = false;
    emit geometryChanged();
    emit stateChanged();
}

void CaacFigureEightFenceController::_updateVehicleState()
{
    auto* vehicle = _vehicle.data();
    if (!vehicle || vehicle->vehicleLinkManager()->primaryLink().expired() ||
        vehicle->vehicleLinkManager()->communicationLost()) {
        _invalidate(tr("无人机通信中断，围栏状态需重新读取确认。"));
    } else if (busy() && _applying && (vehicle->armed() || vehicle->airborne())) {
        _fail(tr("无人机已解锁或起飞，已停止后续配置；已发送的设置仍可能完成，请回读确认。"));
    }
    emit stateChanged();
}

bool CaacFigureEightFenceController::apply(bool replaceExisting)
{
    return _begin(true, replaceExisting);
}

void CaacFigureEightFenceController::readFromVehicle()
{
    _begin(false, false);
}

bool CaacFigureEightFenceController::_begin(bool applying, bool replaceExisting)
{
    if (busy()) {
        return false;
    }
    const auto reason = _blockReason(applying);
    if (!reason.isEmpty()) {
        _complete(reason, _applied);
        return false;
    }
    auto* vehicle = _vehicle.data();
    if (!vehicle) {
        return false;
    }
    _applying = applying;
    _geometryWritten = false;
    _writesStarted = false;
    _replaceExisting = replaceExisting;
    _replacementRequired = false;
    _applied = false;
    _expectedPath = _path;
    _policy = _vehiclePolicy();
    _parameterSteps = _policy.preparation + _policy.activation;
    _remoteParameters.clear();
    _stage = Stage::ReadExisting;
    _status = tr("正在读取飞控现有围栏…");
    _timeout.start();
    emit stateChanged();
    vehicle->geoFenceManager()->loadFromVehicle();
    return true;
}

void CaacFigureEightFenceController::_loaded()
{
    auto* vehicle = _vehicle.data();
    if (!vehicle || (_stage != Stage::ReadExisting && _stage != Stage::VerifyGeometry)) {
        if (_applied && !_matchesGeometry()) {
            _invalidate(tr("飞控围栏已变化，请重新读取或应用。"));
        }
        return;
    }
    if (_stage == Stage::VerifyGeometry) {
        if (!_matchesGeometry()) {
            _fail(tr("飞控回读围栏与训练边界不一致，未确认应用。"));
            return;
        }
        _startActivation();
        return;
    } else {
        _stage = Stage::ReadParameters;
        _parameterIndex = 0;
        _status = tr("正在读取飞控围栏参数…");
    }
    emit stateChanged();
    _nextParameter();
}

void CaacFigureEightFenceController::_nextParameter()
{
    disconnect(_parameterEcho);
    auto* vehicle = _vehicle.data();
    if (!vehicle ||
        (_stage != Stage::ReadParameters && _stage != Stage::WritePreparation && _stage != Stage::WriteParameters)) {
        return;
    }
    if (_applying && (vehicle->armed() || vehicle->airborne())) {
        _fail(tr("无人机已解锁或起飞，已停止后续配置；已发送的设置仍可能完成，请回读确认。"));
        return;
    }
    const qsizetype limit = _stage == Stage::WritePreparation ? _policy.preparation.size() : _parameterSteps.size();
    if (_parameterIndex == limit) {
        if (_stage == Stage::ReadParameters) {
            _parametersRead();
        } else if (_stage == Stage::WritePreparation) {
            if (_geometryWritten) {
                _startActivation();
            } else {
                _upload();
            }
        } else {
            bool matching = _matchesGeometry() && _matchesParameters();
            for (const auto& step : std::as_const(_parameterSteps)) {
                if (!sameValue(vehicle->parameterManager()
                                   ->getParameter(ParameterManager::defaultComponentId, step.first)
                                   ->rawValue(),
                               step.second)) {
                    matching = false;
                }
            }
            if (matching) {
                _complete(tr("围栏和越界限制动作已由飞控回读确认。"), true);
            } else {
                _fail(tr("飞控设置在确认过程中发生变化，请重新读取围栏状态。"));
            }
        }
        return;
    }
    const auto step = _parameterSteps[_parameterIndex];
    auto* parameters = vehicle->parameterManager();
    if (!parameters->parameterExists(ParameterManager::defaultComponentId, step.first)) {
        _fail(tr("飞控缺少围栏参数 %1。").arg(step.first));
        return;
    }
    auto* fact = parameters->getParameter(ParameterManager::defaultComponentId, step.first);
    const auto currentStage = _stage;
    const auto index = _parameterIndex;
    _parameterEcho =
        connect(fact, &Fact::vehicleUpdated, this, [this, step, currentStage, index](const QVariant& value) {
            if (_stage != currentStage || _parameterIndex != index) {
                return;
            }
            if (_stage != Stage::ReadParameters && !sameValue(value, step.second)) {
                return;
            }
            _remoteParameters.insert(step.first, value);
            disconnect(_parameterEcho);
            ++_parameterIndex;
            QTimer::singleShot(0, this, &CaacFigureEightFenceController::_nextParameter);
        });
    _timeout.start();
    if (_stage != Stage::ReadParameters && !sameValue(fact->rawValue(), step.second)) {
        _writesStarted = true;
        fact->setRawValue(step.second);
    } else {
        parameters->refreshParameter(ParameterManager::defaultComponentId, step.first);
    }
}

void CaacFigureEightFenceController::_parametersRead()
{
    auto* vehicle = _vehicle.data();
    if (!vehicle) {
        return;
    }
    const bool matching = _matchesGeometry();
    if (!_applying) {
        if (_policy.description.isEmpty() || _policy.activation.isEmpty()) {
            _complete(_policy.unsupportedReason.isEmpty()
                          ? tr("已读取飞控围栏；当前飞控尚不满足训练围栏限制要求，未确认启用。")
                          : tr("已读取飞控围栏。") + _policy.unsupportedReason);
            return;
        }
        _complete(matching && _matchesParameters() ? tr("围栏和越界限制动作已由飞控回读确认。")
                                                   : tr("飞控围栏或限制动作与当前训练设置不同。"),
                  matching && _matchesParameters());
        return;
    }
    auto* manager = vehicle->geoFenceManager();
    if (!matching && !manager->missionItems().isEmpty() && !_replaceExisting) {
        _replacementRequired = true;
        _complete(tr("飞控已有其他围栏；确认替换后将先备份，再应用训练围栏。"));
        return;
    }
    if (!_saveBackup()) {
        _fail(tr("无法保存现有围栏和参数备份，已取消应用。"));
        return;
    }
    _geometryWritten = matching;
    _stage = Stage::WritePreparation;
    _parameterIndex = 0;
    _status = tr("正在设置并确认飞控围栏安全准备参数…");
    emit stateChanged();
    _nextParameter();
}

void CaacFigureEightFenceController::_startActivation()
{
    _stage = Stage::WriteParameters;
    _parameterIndex = _policy.preparation.size();
    _status = tr("正在设置并确认飞控越界限制动作…");
    emit stateChanged();
    _nextParameter();
}

void CaacFigureEightFenceController::_upload()
{
    auto* vehicle = _vehicle.data();
    if (!vehicle) {
        return;
    }
    QmlObjectListModel polygons;
    QmlObjectListModel circles;
    QGCFencePolygon polygon(true);
    for (const auto& point : std::as_const(_expectedPath)) {
        polygon.appendVertex(point);
    }
    polygons.append(&polygon);
    _stage = Stage::Upload;
    _writesStarted = true;
    _status = tr("正在上传训练电子围栏…");
    _timeout.start();
    emit stateChanged();
    vehicle->geoFenceManager()->sendToVehicle(QGeoCoordinate(), polygons, circles);
    polygons.clear();
}

void CaacFigureEightFenceController::_sent(bool error)
{
    if (_stage != Stage::Upload) {
        if (_applied) {
            _invalidate(tr("飞控围栏已重新上传，请读取确认。"));
        }
        return;
    }
    auto* vehicle = _vehicle.data();
    if (error || !vehicle) {
        _fail(tr("训练围栏上传失败，请重新读取飞控状态。"));
        return;
    }
    _geometryWritten = true;
    _stage = Stage::VerifyGeometry;
    _status = tr("正在回读确认训练电子围栏…");
    _timeout.start();
    emit stateChanged();
    vehicle->geoFenceManager()->loadFromVehicle();
}

bool CaacFigureEightFenceController::_matchesGeometry() const
{
    auto* vehicle = _vehicle.data();
    if (!vehicle || _expectedPath.size() < 3) {
        return false;
    }
    auto* manager = vehicle->geoFenceManager();
    const auto& polygons = manager->polygons();
    const auto& items = manager->missionItems();
    if (polygons.size() != 1 || !polygons.first().inclusion() || !manager->circles().isEmpty() ||
        items.size() != _expectedPath.size() || polygons.first().count() != _expectedPath.size()) {
        return false;
    }
    for (const auto* item : items) {
        if (!item || item->command() != MAV_CMD_NAV_FENCE_POLYGON_VERTEX_INCLUSION ||
            item->frame() != MAV_FRAME_GLOBAL || item->param1() != _expectedPath.size()) {
            return false;
        }
    }
    const auto actual = polygons.first().path();
    for (int start = 0; start < actual.size(); ++start) {
        for (const int direction : {1, -1}) {
            bool matches = true;
            for (int index = 0; index < _expectedPath.size(); ++index) {
                const int actualIndex = (start + direction * index + actual.size()) % actual.size();
                const auto coordinate = actual[actualIndex].value<QGeoCoordinate>();
                if (!coordinate.isValid()) {
                    matches = false;
                    break;
                }
                const double distance = QGCGeo::geodesicDistance(_expectedPath[index], coordinate);
                if (!std::isfinite(distance) || distance > POSITION_TOLERANCE_METERS) {
                    matches = false;
                    break;
                }
            }
            if (matches) {
                return true;
            }
        }
    }
    return false;
}

bool CaacFigureEightFenceController::_matchesParameters() const
{
    for (const auto& step : _parameterSteps) {
        if (!_remoteParameters.contains(step.first) || !sameValue(_remoteParameters.value(step.first), step.second)) {
            return false;
        }
    }
    return true;
}

bool CaacFigureEightFenceController::_saveBackup()
{
    auto* vehicle = _vehicle.data();
    if (!vehicle) {
        return false;
    }
    auto* manager = vehicle->geoFenceManager();
    QJsonArray polygons;
    for (auto polygon : manager->polygons()) {
        QJsonObject object;
        polygon.saveToJson(object);
        polygons.append(object);
    }
    QJsonArray circles;
    for (auto circle : manager->circles()) {
        QJsonObject object;
        circle.saveToJson(object);
        circles.append(object);
    }
    QJsonArray returnPoint;
    for (const auto* item : manager->missionItems()) {
        if (item && item->command() == MAV_CMD_NAV_FENCE_RETURN_POINT) {
            returnPoint = {item->param5(), item->param6(), item->param7()};
        }
    }
    QJsonObject parameters;
    for (auto it = _remoteParameters.cbegin(); it != _remoteParameters.cend(); ++it) {
        parameters.insert(it.key(), QJsonValue::fromVariant(it.value()));
    }
    const auto created = QDateTime::currentDateTimeUtc();
    const QJsonObject object{{QStringLiteral("fileType"), QStringLiteral("CAAC Training Fence Backup")},
                             {QStringLiteral("version"), 1},
                             {QStringLiteral("createdUtc"), created.toString(Qt::ISODateWithMs)},
                             {QStringLiteral("vehicleId"), vehicle->id()},
                             {QStringLiteral("firmwareType"), static_cast<int>(vehicle->firmwareType())},
                             {QStringLiteral("polygons"), polygons},
                             {QStringLiteral("circles"), circles},
                             {QStringLiteral("breachReturn"), returnPoint},
                             {QStringLiteral("parameters"), parameters}};
    const auto directory =
        QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + QStringLiteral("/FigureEightFenceBackups");
    if (!QDir().mkpath(directory)) {
        return false;
    }
    const auto filename = directory + QStringLiteral("/vehicle-%1-%2-%3.json")
                                          .arg(vehicle->id())
                                          .arg(created.toString(QStringLiteral("yyyyMMdd-HHmmss-zzz")),
                                               QUuid::createUuid().toString(QUuid::Id128));
    QSaveFile file(filename);
    if (!file.open(QIODevice::WriteOnly)) {
        return false;
    }
    const auto bytes = QJsonDocument(object).toJson(QJsonDocument::Indented);
    if (file.write(bytes) != bytes.size() || !file.commit()) {
        return false;
    }
    _backupFile = filename;
    return true;
}

void CaacFigureEightFenceController::_complete(const QString& status, bool applied)
{
    disconnect(_parameterEcho);
    for (const auto& connection : std::as_const(_appliedParameterConnections)) {
        disconnect(connection);
    }
    _appliedParameterConnections.clear();
    _timeout.stop();
    _stage = Stage::Idle;
    _applied = applied;
    _status = status;
    auto* vehicle = _vehicle.data();
    if (_applied && vehicle) {
        auto* parameters = vehicle->parameterManager();
        for (const auto& step : std::as_const(_parameterSteps)) {
            if (parameters->parameterExists(ParameterManager::defaultComponentId, step.first)) {
                auto* fact = parameters->getParameter(ParameterManager::defaultComponentId, step.first);
                _appliedParameterConnections.append(
                    connect(fact, &Fact::rawValueChanged, this, [this, step](const QVariant& value) {
                        if (_applied && !sameValue(value, step.second)) {
                            _invalidate(tr("飞控围栏限制参数已变化，请重新读取确认。"));
                        }
                    }));
            }
        }
    }
    emit stateChanged();
}

void CaacFigureEightFenceController::_fail(const QString& reason)
{
    _complete((_geometryWritten ? tr("围栏已写入，启用未确认。") : QString()) + reason +
              (_writesStarted ? tr(" 飞控可能已接收部分设置，请回读确认。") : QString()));
}

void CaacFigureEightFenceController::_invalidate(const QString& reason)
{
    if (busy()) {
        _fail(reason);
    } else {
        _complete(reason);
    }
}

void CaacFigureEightFenceController::_saveSettings() const
{
    QSettings settings;
    settings.beginGroup(QLatin1String(SETTINGS_GROUP));
    settings.setValue(QStringLiteral("margin"), _margin);
    settings.setValue(QStringLiteral("showOnMap"), _showOnMap);
}
