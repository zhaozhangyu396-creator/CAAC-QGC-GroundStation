#include "CaacFigureEightController.h"

#include <algorithm>
#include <cmath>
#include <limits>
#include <numbers>

#include <QtCore/QJsonArray>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>
#include <QtCore/QSettings>

#include "CaacFigureEightFenceController.h"
#include "Fact.h"
#include "MultiVehicleManager.h"
#include "QGCFileHelper.h"
#include "QGCGeo.h"
#include "TrajectoryPoints.h"
#include "Vehicle.h"
#include "VehicleGPSFactGroup.h"
#include "VehicleLinkManager.h"

namespace {
constexpr int POINTS_PER_CIRCLE = 120;
constexpr int MAX_SITES = 200;
constexpr int MAX_RECORDED_POINTS = 18000;
constexpr qint64 TELEMETRY_TIMEOUT_MS = 3000;
// Product freshness window for a future ground-sensor adapter; the exam standard specifies no transport timeout.
constexpr qint64 WIND_TIMEOUT_MS = 10000;
constexpr auto SETTINGS_GROUP = "CAAC/FigureEight";

QVariantMap coordinateMap(const QGeoCoordinate& coordinate)
{
    return {{QStringLiteral("latitude"), coordinate.latitude()}, {QStringLiteral("longitude"), coordinate.longitude()}};
}

QGeoCoordinate siteCoordinate(const QVariantMap& site, const QString& key)
{
    const auto values = site.value(key).toMap();
    bool validLatitude = false;
    bool validLongitude = false;
    const double latitude = values.value(QStringLiteral("latitude")).toDouble(&validLatitude);
    const double longitude = values.value(QStringLiteral("longitude")).toDouble(&validLongitude);
    return validLatitude && validLongitude ? QGeoCoordinate(latitude, longitude) : QGeoCoordinate();
}

double wrappedDifference(double value)
{
    return std::remainder(value, 360.0);
}
}  // namespace

CaacFigureEightController::CaacFigureEightController(QObject* parent)
    : QObject(parent)
    , _status(tr("设置场地后可开始手动八字训练。"))
{
    _clock.start();
    connect(this, &CaacFigureEightController::settingsChanged, this, [this]() { _readyAnnounced = false; });
    connect(this, &CaacFigureEightController::siteChanged, this, [this]() { _readyAnnounced = false; });
    connect(this, &CaacFigureEightController::sessionChanged, this, &CaacFigureEightController::_updateCalibration);
    _timer.setInterval(100);
    connect(&_timer, &QTimer::timeout, this, &CaacFigureEightController::_sample);
    _windTimer.setInterval(1000);
    connect(&_windTimer, &QTimer::timeout, this, &CaacFigureEightController::_checkWindFreshness);
    QSettings settings;
    settings.beginGroup(QLatin1String(SETTINGS_GROUP));
    _exerciseMode = std::clamp(settings.value(QStringLiteral("exerciseMode"), 0).toInt(), 0, 2);
    _voiceEnabled = settings.value(QStringLiteral("voiceEnabled"), true).toBool();
    _windSource = settings.value(QStringLiteral("windSource"), 0).toInt() == 1 ? 1 : 0;
    const double manualWind = settings.value(QStringLiteral("manualWindSpeed"), 0.0).toDouble();
    _manualWindSpeed = std::isfinite(manualWind) && manualWind >= 0 && manualWind <= 100 ? manualWind : 0;
    _standard = std::clamp(settings.value(QStringLiteral("standard"), 0).toInt(), 0, 3);
    _limits = _defaults(_standard);
    QMap<int, QVariantMap> savedCategoryLimits;
    for (int category = 0; category < 4; ++category) {
        _categoryLimits.insert(category, _defaults(category));
        auto saved = settings.value(QStringLiteral("limits%1").arg(category)).toMap();
        if (saved.isEmpty() && category == _standard) {
            saved = settings.value(QStringLiteral("limits")).toMap();
        }
        savedCategoryLimits.insert(category, saved);
    }
    _showOnMap = settings.value(QStringLiteral("showOnMap"), true).toBool();
    const double width = settings.value(QStringLiteral("fenceWidth"), 1.2).toDouble();
    _fenceWidth = std::isfinite(width) && width >= 0.1 && width <= 20 ? width : 1.2;
    const auto savedSites = settings.value(QStringLiteral("sites")).toList();
    for (const auto& site : savedSites) {
        if (_sites.size() < MAX_SITES && _validSite(site.toMap())) {
            _sites.append(site.toMap());
        }
    }
    const int selected = settings.value(QStringLiteral("selectedSite"), -1).toInt();
    if (selected >= 0 && selected < _sites.size()) {
        selectSite(selected);
    }
    const int selectedStandard = _standard;
    for (int category = 0; category < 4; ++category) {
        _standard = category;
        _limits = _defaults(category);
        if (!savedCategoryLimits.value(category).isEmpty()) {
            setLimits(savedCategoryLimits.value(category));
        }
    }
    _standard = selectedStandard;
    _limits = _categoryLimits.value(_standard);
    _clearError();
    _saveSettings();
    auto* manager = MultiVehicleManager::instance();
    connect(manager, &MultiVehicleManager::activeVehicleChanged, this, &CaacFigureEightController::_setVehicle);
    _setVehicle(manager->activeVehicle());
    _updateCalibration();
    clearRecording();
    _lastWindValid = windValid();
    _windTimer.start();
    _fence = new CaacFigureEightFenceController(this);
}

QVariantMap CaacFigureEightController::_defaults(int standard)
{
    // MD-92-FS-02 (2024-05-11), attachment 3: small/medium multirotors, level-one wind, 0–4 m/s.
    // https://pilot.caac.gov.cn/servlet/FileUploadManagerServlet?method=noDownload&UUID=ecc0832a-2652-4318-be4d-defdd82ceffe
    return {{QStringLiteral("horizontal"), 2.0},
            {QStringLiteral("vertical"), 1.0},
            {QStringLiteral("heading"), standard == 2 ? 25.0 : 30.0},
            {QStringLiteral("durationEnabled"), false},
            {QStringLiteral("maxDuration"), 180.0},
            {QStringLiteral("speedEnabled"), false},
            {QStringLiteral("minSpeed"), 0.3},
            {QStringLiteral("maxSpeed"), 3.0}};
}

void CaacFigureEightController::setStandard(int standard)
{
    if (_running || standard < 0 || standard > 3 || standard == _standard) {
        return;
    }
    _standard = standard;
    _limits = _categoryLimits.value(standard, _defaults(standard));
    _saveSettings();
    emit settingsChanged();
    _windUpdated();
}

bool CaacFigureEightController::setLimits(const QVariantMap& limits)
{
    if (_running) {
        return _fail(tr("请先结束训练，再修改标准。"));
    }
    auto updated = _limits;
    for (auto it = limits.cbegin(); it != limits.cend(); ++it) {
        if (!updated.contains(it.key())) {
            return _fail(tr("训练限制包含未知设置。"));
        }
        if (it.key() == QStringLiteral("speedEnabled") || it.key() == QStringLiteral("durationEnabled")) {
            if (it.value().metaType().id() != QMetaType::Bool) {
                return _fail(tr("训练限制开关无效。"));
            }
            updated[it.key()] = it.value().toBool();
        } else {
            bool valid = false;
            const double number = it.value().toDouble(&valid);
            if (!valid || !std::isfinite(number) || number < 0 || number > 86400) {
                return _fail(tr("训练限制必须为有效的非负数。"));
            }
            updated[it.key()] = number;
        }
    }
    if (updated.value(QStringLiteral("horizontal")).toDouble() <= 0 ||
        updated.value(QStringLiteral("horizontal")).toDouble() > 100 ||
        updated.value(QStringLiteral("vertical")).toDouble() <= 0 ||
        updated.value(QStringLiteral("vertical")).toDouble() > 100 ||
        updated.value(QStringLiteral("heading")).toDouble() <= 0 ||
        updated.value(QStringLiteral("heading")).toDouble() > 180 ||
        updated.value(QStringLiteral("maxDuration")).toDouble() < 1 ||
        updated.value(QStringLiteral("maxSpeed")).toDouble() <= 0 ||
        updated.value(QStringLiteral("maxSpeed")).toDouble() > 100 ||
        updated.value(QStringLiteral("minSpeed")).toDouble() > updated.value(QStringLiteral("maxSpeed")).toDouble()) {
        return _fail(tr("请检查偏差、时长和速度限制的范围。"));
    }
    _limits = updated;
    _categoryLimits.insert(_standard, updated);
    _clearError();
    _saveSettings();
    emit settingsChanged();
    _windUpdated();
    return true;
}

void CaacFigureEightController::resetLimits()
{
    if (!_running) {
        _limits = _defaults(_standard);
        _categoryLimits.insert(_standard, _limits);
        _clearError();
        _saveSettings();
        emit settingsChanged();
        _windUpdated();
    }
}

bool CaacFigureEightController::_validSite(const QVariantMap& site)
{
    const QString name = site.value(QStringLiteral("name")).toString().trimmed();
    const auto pointA = siteCoordinate(site, QStringLiteral("a"));
    const auto pointB = siteCoordinate(site, QStringLiteral("b"));
    if (name.isEmpty() || name.size() > 100 || !pointA.isValid() || !pointB.isValid()) {
        return false;
    }
    const double distance = QGCGeo::geodesicDistance(pointA, pointB);
    return std::isfinite(distance) && distance >= 4 && distance <= 2000;
}

bool CaacFigureEightController::setPoints(const QGeoCoordinate& pointA, const QGeoCoordinate& pointB)
{
    if (_running) {
        return _fail(tr("请先结束训练，再修改场地。"));
    }
    const QVariantMap site{{QStringLiteral("name"), QStringLiteral("preview")},
                           {QStringLiteral("a"), coordinateMap(pointA)},
                           {QStringLiteral("b"), coordinateMap(pointB)}};
    if (!_validSite(site)) {
        return _fail(tr("A、B 点需为有效坐标，间距为 4 至 2000 米。"));
    }
    _pointA = QGeoCoordinate(pointA.latitude(), pointA.longitude());
    _pointB = QGeoCoordinate(pointB.latitude(), pointB.longitude());
    _calibrationVehicle.clear();
    _selectedSite = -1;
    _rebuildPath();
    _saveSettings();
    _clearError();
    emit siteChanged();
    return true;
}

bool CaacFigureEightController::capturePointA()
{
    return _capturePoint(true);
}

bool CaacFigureEightController::capturePointB()
{
    return _capturePoint(false);
}

bool CaacFigureEightController::_capturePoint(bool pointA)
{
    _updateCalibration();
    if (!_calibrationReady) {
        return _fail(_calibrationStatus);
    }
    // Starting a new calibration must not combine a live capture with a saved or another vehicle's point.
    const bool continuing = _calibrationVehicle == _vehicle;
    auto nextA = continuing ? _pointA : QGeoCoordinate();
    auto nextB = continuing ? _pointB : QGeoCoordinate();
    if (pointA) {
        nextA = _rtkCoordinate;
    } else {
        nextB = _rtkCoordinate;
    }
    if (nextA.isValid() && nextB.isValid()) {
        const double distance = QGCGeo::geodesicDistance(nextA, nextB);
        if (!std::isfinite(distance) || distance < 4 || distance > 2000) {
            return _fail(tr("A、B 点间距需为 4 至 2000 米，请将无人机移动到另一标定点后重试。"));
        }
    }
    _pointA = nextA;
    _pointB = nextB;
    _calibrationVehicle = _vehicle;
    _selectedSite = -1;
    _rebuildPath();
    if (!_reference.isEmpty()) {
        setShowOnMap(true);
    }
    _saveSettings();
    _clearError();
    emit siteChanged();
    return true;
}

QString CaacFigureEightController::_calibrationBlockReason() const
{
    if (_running) {
        return tr("请先结束训练，再标定场地。");
    }
    if (!_vehicle || !_vehicle->isInitialConnectComplete()) {
        return tr("请连接具备 RTK 定位的无人机，完成连接后标定。");
    }
    if (_vehicle->vehicleLinkManager()->communicationLost()) {
        return tr("遥测连接中断，请恢复连接后标定。");
    }
    if (_vehicle->airborne()) {
        return tr("请将无人机放在 A 点或 B 点地面后标定。");
    }
    if (_lastGpsFix < 0 || _clock.elapsed() - _lastGpsFix > TELEMETRY_TIMEOUT_MS) {
        return tr("等待实时 RTK 定位数据，请保持无人机静置。");
    }
    if (_gpsFixType != GPS_FIX_TYPE_RTK_FIXED) {
        return tr("等待 RTK 固定解后标定当前点。");
    }
    if (!_rtkCoordinate.isValid()) {
        return tr("无人机当前位置无效，请等待 RTK 定位。");
    }
    return {};
}

void CaacFigureEightController::_updateCalibration()
{
    const QString reason = _calibrationBlockReason();
    const bool ready = reason.isEmpty();
    const QString status = ready ? tr("RTK 固定解，可标定无人机当前位置。") : reason;
    if (_calibrationReady != ready || _calibrationStatus != status) {
        _calibrationReady = ready;
        _calibrationStatus = status;
        emit calibrationChanged();
    }
}

void CaacFigureEightController::selectSite(int index)
{
    if (_running || index < -1 || index >= _sites.size()) {
        return;
    }
    if (index == -1) {
        _selectedSite = -1;
        _saveSettings();
        emit siteChanged();
        return;
    }
    const auto site = _sites.at(index).toMap();
    _calibrationVehicle.clear();
    _pointA = siteCoordinate(site, QStringLiteral("a"));
    _pointB = siteCoordinate(site, QStringLiteral("b"));
    _selectedSite = index;
    _rebuildPath();
    _saveSettings();
    _clearError();
    emit siteChanged();
}

bool CaacFigureEightController::saveSite(const QString& name, const QGeoCoordinate& pointA,
                                         const QGeoCoordinate& pointB)
{
    if (_running) {
        return _fail(tr("请先结束训练，再保存场地。"));
    }
    const QVariantMap site{{QStringLiteral("name"), name.trimmed()},
                           {QStringLiteral("a"), coordinateMap(pointA)},
                           {QStringLiteral("b"), coordinateMap(pointB)}};
    if (!_validSite(site)) {
        return _fail(tr("请输入场地名称及有效的 A、B 点，间距为 4 至 2000 米。"));
    }
    int index = -1;
    for (int i = 0; i < _sites.size(); ++i) {
        if (_sites.at(i).toMap().value(QStringLiteral("name")) == site.value(QStringLiteral("name"))) {
            index = i;
            break;
        }
    }
    if (index < 0) {
        if (_sites.size() >= MAX_SITES) {
            return _fail(tr("场地数量已达到上限。"));
        }
        index = _sites.size();
        _sites.append(site);
    } else {
        _sites[index] = site;
    }
    selectSite(index);
    emit sitesChanged();
    return true;
}

void CaacFigureEightController::removeSite(int index)
{
    if (_running || index < 0 || index >= _sites.size()) {
        return;
    }
    _sites.removeAt(index);
    if (_selectedSite == index) {
        _selectedSite = -1;
        _pointA = {};
        _pointB = {};
        _calibrationVehicle.clear();
        _rebuildPath();
    } else if (_selectedSite > index) {
        --_selectedSite;
    }
    _saveSettings();
    emit sitesChanged();
    emit siteChanged();
}

bool CaacFigureEightController::importSites(const QString& filename)
{
    if (_running) {
        return _fail(tr("请先结束训练，再导入场地。"));
    }
    QString error;
    const auto data = QGCFileHelper::readFile(filename, &error, 1024 * 1024);
    QJsonParseError parseError;
    const auto document = QJsonDocument::fromJson(data, &parseError);
    if (data.isEmpty() || parseError.error != QJsonParseError::NoError || !document.isObject()) {
        return _fail(tr("无法读取有效的八字场地文件。%1").arg(error));
    }
    const auto object = document.object();
    const auto array = object.value(QStringLiteral("sites")).toArray();
    if (object.value(QStringLiteral("fileType")).toString() != QStringLiteral("CAACFigureEightSites") ||
        object.value(QStringLiteral("version")).toInt() != 1 || !object.value(QStringLiteral("sites")).isArray() ||
        array.size() > MAX_SITES) {
        return _fail(tr("八字场地文件格式或版本无效。"));
    }
    auto merged = _sites;
    for (const auto& value : array) {
        if (!value.isObject() || !_validSite(value.toObject().toVariantMap())) {
            return _fail(tr("导入文件包含无效场地，未更改已有场地。"));
        }
        const auto site = value.toObject().toVariantMap();
        const auto name = site.value(QStringLiteral("name")).toString().trimmed();
        const auto existing = std::find_if(merged.begin(), merged.end(), [&name](const QVariant& saved) {
            return saved.toMap().value(QStringLiteral("name")).toString() == name;
        });
        if (existing == merged.end()) {
            merged.append(site);
        } else {
            *existing = site;
        }
    }
    if (merged.size() > MAX_SITES) {
        return _fail(tr("导入后场地数量超出上限，未更改已有场地。"));
    }
    _sites = merged;
    if (_selectedSite >= 0 && _selectedSite < _sites.size()) {
        selectSite(_selectedSite);
    }
    _clearError();
    _saveSettings();
    emit sitesChanged();
    return true;
}

bool CaacFigureEightController::exportSites(const QString& filename)
{
    const QJsonObject object{{QStringLiteral("fileType"), QStringLiteral("CAACFigureEightSites")},
                             {QStringLiteral("version"), 1},
                             {QStringLiteral("sites"), QJsonArray::fromVariantList(_sites)}};
    if (filename.isEmpty() || !QGCFileHelper::atomicWrite(filename, QJsonDocument(object).toJson())) {
        return _fail(tr("无法保存八字场地文件。"));
    }
    _clearError();
    return true;
}

QList<QGeoCoordinate> CaacFigureEightController::referenceCoordinates(const QGeoCoordinate& pointA,
                                                                      const QGeoCoordinate& pointB)
{
    if (!pointA.isValid() || !pointB.isValid()) {
        return {};
    }
    const double diameter = QGCGeo::geodesicDistance(pointA, pointB);
    if (!std::isfinite(diameter) || diameter < 4 || diameter > 2000) {
        return {};
    }
    const double bearing = QGCGeo::geodesicAzimuth(pointA, pointB);
    const auto origin = QGCGeo::geodesicDestination(pointA, bearing, diameter / 2);
    const double radius = diameter / 4;
    QList<QGeoCoordinate> path;
    for (int circle = 0; circle < 2; ++circle) {
        for (int i = circle == 0 ? 0 : 1; i <= POINTS_PER_CIRCLE; ++i) {
            const double angle = 2 * std::numbers::pi * i / POINTS_PER_CIRCLE;
            const double along = (circle == 0 ? -1 : 1) * radius * (1 - std::cos(angle));
            const double lateral = radius * std::sin(angle);
            path.append(QGCGeo::geodesicDestination(
                origin, bearing + std::atan2(lateral, along) * 180 / std::numbers::pi, std::hypot(along, lateral)));
        }
    }
    return path;
}

void CaacFigureEightController::_rebuildPath()
{
    _reference = referenceCoordinates(_pointA, _pointB);
    _cumulativeLength = {0};
    for (int i = 1; i < _reference.size(); ++i) {
        _cumulativeLength.append(_cumulativeLength.last() + QGCGeo::geodesicDistance(_reference[i - 1], _reference[i]));
    }
}

QVariantList CaacFigureEightController::referencePath() const
{
    QVariantList path;
    path.reserve(_reference.size());
    for (const auto& coordinate : _reference) {
        path.append(QVariant::fromValue(coordinate));
    }
    return path;
}

void CaacFigureEightController::setViewActive(bool active)
{
    if (_viewActive != active) {
        _viewActive = active;
        emit viewActiveChanged();
    }
}

void CaacFigureEightController::setShowOnMap(bool visible)
{
    if (_showOnMap != visible) {
        _showOnMap = visible;
        _saveSettings();
        emit settingsChanged();
    }
}

void CaacFigureEightController::setFenceWidth(double width)
{
    if (!_running && std::isfinite(width) && width >= 0.1 && width <= 20 && width != _fenceWidth) {
        _fenceWidth = width;
        _saveSettings();
        emit settingsChanged();
    }
}

void CaacFigureEightController::_saveSettings() const
{
    QSettings settings;
    settings.beginGroup(QLatin1String(SETTINGS_GROUP));
    settings.setValue(QStringLiteral("standard"), _standard);
    settings.setValue(QStringLiteral("limits"), _limits);
    for (auto it = _categoryLimits.cbegin(); it != _categoryLimits.cend(); ++it) {
        settings.setValue(QStringLiteral("limits%1").arg(it.key()), it.value());
    }
    settings.setValue(QStringLiteral("sites"), _sites);
    settings.setValue(QStringLiteral("selectedSite"), _selectedSite);
    settings.setValue(QStringLiteral("showOnMap"), _showOnMap);
    settings.setValue(QStringLiteral("fenceWidth"), _fenceWidth);
    settings.setValue(QStringLiteral("windSource"), _windSource);
    settings.setValue(QStringLiteral("manualWindSpeed"), _manualWindSpeed);
    settings.setValue(QStringLiteral("exerciseMode"), _exerciseMode);
    settings.setValue(QStringLiteral("voiceEnabled"), _voiceEnabled);
}

void CaacFigureEightController::setExerciseMode(int mode)
{
    if (!_running && mode >= 0 && mode <= 2 && mode != _exerciseMode) {
        _exerciseMode = mode;
        _phase = Phase::Idle;
        _saveSettings();
        emit settingsChanged();
        emit sessionChanged();
    }
}

QString CaacFigureEightController::phase() const
{
    switch (_phase) {
        case Phase::Spin:
            return tr("原地自旋");
        case Phase::ReverseSpin:
            return tr("反向原地自旋");
        case Phase::FigureEight:
            return tr("八字飞行");
        case Phase::Finished:
            return tr("已结束");
        default:
            return tr("待开始");
    }
}

void CaacFigureEightController::setVoiceEnabled(bool enabled)
{
    if (_voiceEnabled != enabled) {
        _voiceEnabled = enabled;
        if (!enabled) {
            emit alarmsCleared();
        }
        _saveSettings();
        emit settingsChanged();
    }
}

void CaacFigureEightController::testVoice()
{
    _announce(QStringLiteral("ready"), tr("可以开始考试"));
}

void CaacFigureEightController::_announce(const QString& id, const QString& text)
{
    if (_lastAnnouncement != text) {
        _lastAnnouncement = text;
        emit announcementChanged();
    }
    if (_voiceEnabled) {
        emit announcementRequested(id, text);
    }
}

void CaacFigureEightController::_updateReady()
{
    if (_running) {
        return;
    }
    bool ready = _vehicle && _vehicle->armed() && _vehicle->airborne() && _telemetryReady() && windValid();
    if (ready && _exerciseMode != 1) {
        if (_reference.isEmpty()) {
            ready = false;
        } else {
            double distance = 0;
            double heading = 0;
            _nearestProgress(_vehicle->coordinate(), distance, heading);
            ready = distance <= effectiveLimits().value(QStringLiteral("horizontal")).toDouble();
        }
    }
    if (ready && !_readyAnnounced) {
        _readyAnnounced = true;
        _announce(QStringLiteral("ready"), tr("可以开始考试"));
    } else if (!ready) {
        _readyAnnounced = false;
    }
}

void CaacFigureEightController::_beginPhase(bool reverseSpin)
{
    if (!_vehicle) {
        return;
    }
    if (reverseSpin) {
        _phase = Phase::ReverseSpin;
    } else if (_phase == Phase::Spin || _phase == Phase::ReverseSpin) {
        _phase = Phase::FigureEight;
    } else {
        _phase = _exerciseMode == 0 ? Phase::FigureEight : Phase::Spin;
    }
    _phaseClock.start();
    _hadActiveAlarms = false;
    emit alarmsCleared();
    _previousPosition = _vehicle->coordinate();
    _lastSample = _sessionClock.elapsed();
    _alarmCandidate.clear();
    if (_phase == Phase::FigureEight) {
        _previousProgress = 0;
        double distance = 0;
        double heading = 0;
        _previousProgress = _nearestProgress(_vehicle->coordinate(), distance, heading);
        _status = tr("正在记录手动八字飞行。");
        _announce(QStringLiteral("figure_eight"), tr("开始八字飞行"));
    } else {
        _spinAnchor = _vehicle->coordinate();
        _spinLastHeading = _vehicle->heading()->rawValue().toDouble();
        _spinAngle = 0;
        _status = _phase == Phase::ReverseSpin ? tr("请手动逆时针完成 360 度原地自旋，用时 5 至 30 秒。")
                                               : tr("请手动顺时针完成 360 度原地自旋，用时 5 至 30 秒。");
        _announce(_phase == Phase::ReverseSpin ? QStringLiteral("spin_reverse") : QStringLiteral("spin"),
                  _phase == Phase::ReverseSpin ? tr("开始反向原地自旋") : tr("开始原地自旋"));
    }
    emit sessionChanged();
}

QVariantMap CaacFigureEightController::effectiveLimits() const
{
    auto effective = _limits;
    const double factor = windValid() ? windFactor() : 1.0;
    for (const auto* key : {"horizontal", "vertical", "heading"}) {
        effective[QLatin1String(key)] = _limits.value(QLatin1String(key)).toDouble() * factor;
    }
    return effective;
}

void CaacFigureEightController::setWindSource(int source)
{
    if (source < 0 || source > 1 || source == _windSource) {
        return;
    }
    _windSource = source;
    _clearError();
    _saveSettings();
    _windUpdated();
}

void CaacFigureEightController::setManualWindSpeed(double speed)
{
    if (!std::isfinite(speed) || speed < 0 || speed > 100) {
        _fail(tr("风速必须为 0 至 100 米/秒的有效数值。"));
        return;
    }
    _clearError();
    if (_manualWindSpeed != speed) {
        _manualWindSpeed = speed;
        _saveSettings();
        _windUpdated();
    }
}

double CaacFigureEightController::windSpeed() const
{
    return _windSource == 1 ? _groundWindSpeed : _manualWindSpeed;
}

bool CaacFigureEightController::windValid() const
{
    if (_windSource == 0) {
        return true;
    }
    return _groundWindReceived >= 0 && _clock.elapsed() - _groundWindReceived <= WIND_TIMEOUT_MS &&
           std::isfinite(_groundWindSpeed) && _groundWindSpeed >= 0 && _groundWindSpeed <= 100;
}

int CaacFigureEightController::windLevel() const
{
    if (!windValid()) {
        return 0;
    }
    // The source intervals share endpoints; use the stricter lower level at exactly 4 and 8 m/s.
    return windSpeed() <= 4 ? 1 : (windSpeed() <= 8 ? 2 : 3);
}

double CaacFigureEightController::windFactor() const
{
    // Attachment 3 increases each wind level's position and heading tolerances by 15% from the previous level.
    switch (windLevel()) {
        case 1:
            return 1.0;
        case 2:
            return 1.15;
        case 3:
            return 1.15 * 1.15;
        default:
            return 0;
    }
}

bool CaacFigureEightController::updateGroundWind(double speed)
{
    if (!std::isfinite(speed) || speed < 0 || speed > 100) {
        _groundWindReceived = -1;
        _groundWindSpeed = qQNaN();
        _windUpdated();
        return _fail(tr("地面风速数据无效，请检查传感器。"));
    }
    _groundWindSpeed = speed;
    _groundWindReceived = _clock.elapsed();
    _clearError();
    _windUpdated();
    return true;
}

void CaacFigureEightController::_windUpdated()
{
    _lastWindValid = windValid();
    emit windChanged();
    if (_running) {
        _metrics[QStringLiteral("windSpeed")] = windSpeed();
        _metrics[QStringLiteral("windSource")] = _windSource;
        _metrics[QStringLiteral("windLevel")] = windLevel();
        _metrics[QStringLiteral("windFactor")] = windFactor();
        _metrics[QStringLiteral("windValid")] = _lastWindValid;
        emit sampleChanged();
    }
    if (_running && !_lastWindValid) {
        _finish(tr("地面风速数据缺失或失效，训练记录已结束，请检查传感器后重新开始。"), QStringLiteral("wind"));
    }
}

void CaacFigureEightController::_checkWindFreshness()
{
    _updateCalibration();
    if (_lastWindValid != windValid()) {
        _windUpdated();
    }
    _updateReady();
}

void CaacFigureEightController::_setVehicle(Vehicle* vehicle)
{
    if (_vehicle == vehicle && (vehicle || _vehicleConnections.isEmpty())) {
        return;
    }
    if (_running) {
        _finish(tr("飞行器已切换或断开，训练记录已结束。"), QStringLiteral("telemetry"));
    }
    for (const auto& connection : _vehicleConnections) {
        disconnect(connection);
    }
    _vehicleConnections.clear();
    if (_pointA.isValid() != _pointB.isValid()) {
        _pointA = {};
        _pointB = {};
        _selectedSite = -1;
        _rebuildPath();
        emit siteChanged();
    }
    _calibrationVehicle.clear();
    _vehicle = vehicle;
    _lastPosition = _lastAttitude = _lastSpeed = -1;
    _lastGpsFix = -1;
    _gpsFixType = GPS_FIX_TYPE_NO_GPS;
    _rtkCoordinate = {};
    _updateCalibration();
    if (!vehicle) {
        return;
    }
    _vehicleConnections.append(
        connect(vehicle, &Vehicle::mavlinkMessageReceived, this, [this](const mavlink_message_t& message) {
            if (!_vehicle || message.sysid != _vehicle->id() || message.compid != _vehicle->compId()) {
                return;
            }
            const auto now = _clock.elapsed();
            if (message.msgid == MAVLINK_MSG_ID_GLOBAL_POSITION_INT) {
                mavlink_global_position_int_t position{};
                mavlink_msg_global_position_int_decode(&message, &position);
                if (position.lat != 0 || position.lon != 0) {
                    _lastPosition = now;
                }
            } else if (message.msgid == MAVLINK_MSG_ID_ATTITUDE ||
                       message.msgid == MAVLINK_MSG_ID_ATTITUDE_QUATERNION) {
                _lastAttitude = now;
            } else if (message.msgid == MAVLINK_MSG_ID_VFR_HUD) {
                _lastSpeed = now;
            } else if (message.msgid == MAVLINK_MSG_ID_GPS_RAW_INT) {
                mavlink_gps_raw_int_t gps{};
                mavlink_msg_gps_raw_int_decode(&message, &gps);
                _lastGpsFix = now;
                _gpsFixType = gps.fix_type;
                _rtkCoordinate = QGeoCoordinate(gps.lat * 1e-7, gps.lon * 1e-7);
                _updateCalibration();
            }
        }));
    _vehicleConnections.append(
        connect(vehicle, &Vehicle::initialConnectComplete, this, &CaacFigureEightController::_updateCalibration));
    _vehicleConnections.append(
        connect(vehicle, &Vehicle::airborneChanged, this, &CaacFigureEightController::_updateCalibration));
    _vehicleConnections.append(connect(vehicle, &Vehicle::armedChanged, this, [this](bool armed) {
        if (_running && !armed) {
            _finish(tr("飞行器已上锁，训练记录已结束。"));
        }
    }));
    _vehicleConnections.append(
        connect(vehicle->vehicleLinkManager(), &VehicleLinkManager::communicationLostChanged, this, [this](bool lost) {
            _updateCalibration();
            if (_running && lost) {
                _finish(tr("遥测连接中断，训练记录已结束。"), QStringLiteral("telemetry"));
            }
        }));
    _vehicleConnections.append(connect(vehicle, &QObject::destroyed, this, [this]() { _setVehicle(nullptr); }));
}

bool CaacFigureEightController::_telemetryReady() const
{
    if (!_vehicle || !_vehicle->isInitialConnectComplete() || _vehicle->vehicleLinkManager()->communicationLost() ||
        !_vehicle->coordinate().isValid() || !std::isfinite(_vehicle->altitudeRelative()->rawValue().toDouble()) ||
        !_vehicle->gpsFactGroup() || _vehicle->gpsFactGroup()->lock()->rawValue().toInt() < 3 ||
        !std::isfinite(_vehicle->heading()->rawValue().toDouble()) ||
        !std::isfinite(_vehicle->groundSpeed()->rawValue().toDouble()) ||
        _vehicle->groundSpeed()->rawValue().toDouble() < 0) {
        return false;
    }
    const auto now = _clock.elapsed();
    return _lastPosition >= 0 && now - _lastPosition <= TELEMETRY_TIMEOUT_MS && _lastAttitude >= 0 &&
           now - _lastAttitude <= TELEMETRY_TIMEOUT_MS && _lastSpeed >= 0 && now - _lastSpeed <= TELEMETRY_TIMEOUT_MS;
}

bool CaacFigureEightController::start()
{
    if (_running) {
        return false;
    }
    if (_exerciseMode != 1 && _reference.isEmpty()) {
        return _fail(tr("请先设置有效的 A、B 点场地。"));
    }
    if (!windValid()) {
        return _fail(tr("地面风速数据缺失或已超过 10 秒，请检查传感器后开始训练。"));
    }
    if (!_vehicle || !_vehicle->armed() || !_vehicle->airborne() || !_telemetryReady()) {
        return _fail(tr("请连接已起飞的飞行器，并等待有效的位置、高度、航向和速度遥测。"));
    }
    double distance = 0;
    double heading = 0;
    _previousProgress = 0;
    if (_exerciseMode != 1) {
        _nearestProgress(_vehicle->coordinate(), distance, heading);
    }
    if (_exerciseMode != 1 && distance > effectiveLimits().value(QStringLiteral("horizontal")).toDouble()) {
        return _fail(tr("飞行器尚未进入八字参考轨迹附近，请手动飞入后开始记录。"));
    }
    clearRecording();
    _baselineAltitude = _vehicle->altitudeRelative()->rawValue().toDouble();
    _previousPosition = _vehicle->coordinate();
    _sessionClock.start();
    _lastSample = 0;
    _lastRecorded = 0;
    _running = true;
    _readyAnnounced = true;
    _phase = Phase::Idle;
    _clearError();
    _timer.start();
    _beginPhase();
    _sample();
    return true;
}

void CaacFigureEightController::stop()
{
    if (_running) {
        _sample();
        if (_running) {
            _finish(tr("训练记录已结束。偏差结果仅供训练参考。"));
        }
    }
}

void CaacFigureEightController::clearTrajectory()
{
    _recordedPath.clear();
    if (_vehicle) {
        auto* trajectory = _vehicle->property("trajectoryPoints").value<TrajectoryPoints*>();
        if (trajectory) {
            trajectory->clear();
        }
    }
    emit sampleChanged();
}

void CaacFigureEightController::clearRecording()
{
    if (_running) {
        return;
    }
    _recordedPath.clear();
    _violations.clear();
    _signedProgress = 0;
    _travelDistance = 0;
    _violationSamples = 0;
    _completedLaps = 0;
    _alarmCandidate.clear();
    _alarmLastAnnounced.clear();
    _lastAlarm = -6000;
    _hadActiveAlarms = false;
    emit alarmsCleared();
    _previousPosition = {};
    _metrics = {{QStringLiteral("elapsed"), 0.0},
                {QStringLiteral("altitudeError"), 0.0},
                {QStringLiteral("horizontalError"), 0.0},
                {QStringLiteral("headingError"), 0.0},
                {QStringLiteral("speed"), 0.0},
                {QStringLiteral("completedLaps"), 0},
                {QStringLiteral("violations"), QStringList()},
                {QStringLiteral("violationSamples"), 0},
                {QStringLiteral("progress"), 0.0},
                {QStringLiteral("distance"), 0.0},
                {QStringLiteral("telemetryValid"), false},
                {QStringLiteral("windSpeed"), windSpeed()},
                {QStringLiteral("windSource"), _windSource},
                {QStringLiteral("windLevel"), windLevel()},
                {QStringLiteral("windFactor"), windFactor()},
                {QStringLiteral("windValid"), windValid()}};
    emit sampleChanged();
}

double CaacFigureEightController::_nearestProgress(const QGeoCoordinate& coordinate, double& distance,
                                                   double& heading) const
{
    distance = std::numeric_limits<double>::infinity();
    heading = 0;
    double result = 0;
    for (int i = 1; i < _reference.size(); ++i) {
        double north = 0;
        double east = 0;
        double down = 0;
        double legNorth = 0;
        double legEast = 0;
        QGCGeo::convertGeoToNed(coordinate, _reference[i - 1], north, east, down);
        QGCGeo::convertGeoToNed(_reference[i], _reference[i - 1], legNorth, legEast, down);
        const double squaredLength = legNorth * legNorth + legEast * legEast;
        const double fraction =
            squaredLength > 0 ? std::clamp((north * legNorth + east * legEast) / squaredLength, 0.0, 1.0) : 0;
        const double error = std::hypot(north - fraction * legNorth, east - fraction * legEast);
        const double progress = _cumulativeLength[i - 1] + fraction * (_cumulativeLength[i] - _cumulativeLength[i - 1]);
        // At the shared centre, prefer the segment contiguous with the previous sample.
        const double length = _cumulativeLength.last();
        const double change = std::abs(std::remainder(progress - _previousProgress, length));
        const double oldChange = std::abs(std::remainder(result - _previousProgress, length));
        if (error < distance - 0.001 || (std::abs(error - distance) <= 0.001 && change < oldChange)) {
            distance = error;
            result = progress;
            heading = QGCGeo::geodesicAzimuth(_reference[i - 1], _reference[i]);
        }
    }
    return result;
}

void CaacFigureEightController::_sample()
{
    if (!_running) {
        return;
    }
    if (!windValid()) {
        _metrics[QStringLiteral("windValid")] = false;
        emit sampleChanged();
        _windUpdated();
        return;
    }
    if (!_vehicle || _vehicle != MultiVehicleManager::instance()->activeVehicle() || !_vehicle->armed() ||
        !_vehicle->airborne() || !_telemetryReady()) {
        _metrics[QStringLiteral("telemetryValid")] = false;
        emit sampleChanged();
        _finish(tr("飞行状态或遥测已失效，训练记录已结束，未判定完成。"), QStringLiteral("telemetry"));
        return;
    }
    if (_phase == Phase::Spin || _phase == Phase::ReverseSpin) {
        _sampleSpin();
        return;
    }
    const auto now = _sessionClock.elapsed();
    const double elapsed = now / 1000.0;
    const auto coordinate = _vehicle->coordinate();
    const double speed = _vehicle->groundSpeed()->rawValue().toDouble();
    const auto appliedLimits = effectiveLimits();
    const double altitudeError = std::abs(_vehicle->altitudeRelative()->rawValue().toDouble() - _baselineAltitude);
    double horizontalError = 0;
    double tangentHeading = 0;
    const double progress = _nearestProgress(coordinate, horizontalError, tangentHeading);
    const double targetHeading = tangentHeading + (_standard == 2 ? 180.0 : 0.0);
    const double headingError = std::abs(wrappedDifference(_vehicle->heading()->rawValue().toDouble() - targetHeading));
    const double length = _cumulativeLength.last();
    const double movement = QGCGeo::geodesicDistance(_previousPosition, coordinate);
    const double delta = std::remainder(progress - _previousProgress, length);
    const double seconds = std::max(0.0, (now - _lastSample) / 1000.0);
    const double plausibleMovement = std::max(2.0, (std::max(speed, 0.0) + 5.0) * seconds);
    if (movement > plausibleMovement) {
        _finish(tr("位置遥测发生跳变，训练记录已结束，未判定完成。"), QStringLiteral("telemetry"));
        return;
    }
    if (horizontalError <= appliedLimits.value(QStringLiteral("horizontal")).toDouble() &&
        std::abs(delta) <= std::max(2.0, movement * 2 + 0.5)) {
        _signedProgress += delta;
    } else {
        // Skipping across the shared centre must not combine repeated visits to one lobe into a full lap.
        _signedProgress = 0;
    }
    if (_signedProgress >= length - 0.05) {
        ++_completedLaps;
        _signedProgress = std::max(0.0, _signedProgress - length);
    }
    _travelDistance += movement;
    _previousProgress = progress;
    _previousPosition = coordinate;
    _lastSample = now;
    bool sampleViolated = false;
    const auto recordViolation = [this, &sampleViolated](bool violated, const QString& message) {
        sampleViolated = sampleViolated || violated;
        if (violated && !_violations.contains(message)) {
            _violations.append(message);
        }
    };
    recordViolation(horizontalError > appliedLimits.value(QStringLiteral("horizontal")).toDouble(), tr("水平偏差超限"));
    recordViolation(altitudeError > appliedLimits.value(QStringLiteral("vertical")).toDouble(), tr("高度偏差超限"));
    recordViolation(headingError > appliedLimits.value(QStringLiteral("heading")).toDouble(), tr("航向偏差超限"));
    if (_limits.value(QStringLiteral("speedEnabled")).toBool()) {
        recordViolation(speed < _limits.value(QStringLiteral("minSpeed")).toDouble() ||
                            speed > _limits.value(QStringLiteral("maxSpeed")).toDouble(),
                        tr("自定义速度超限"));
    }
    const bool timedOut = _limits.value(QStringLiteral("durationEnabled")).toBool() &&
                          _phaseClock.elapsed() / 1000.0 > _limits.value(QStringLiteral("maxDuration")).toDouble();
    recordViolation(timedOut, tr("自定义训练时长超限"));
    if (sampleViolated) {
        ++_violationSamples;
    }
    _metrics = {{QStringLiteral("elapsed"), elapsed},
                {QStringLiteral("altitudeError"), altitudeError},
                {QStringLiteral("horizontalError"), horizontalError},
                {QStringLiteral("headingError"), headingError},
                {QStringLiteral("speed"), speed},
                {QStringLiteral("completedLaps"), _completedLaps},
                {QStringLiteral("violations"), _violations},
                {QStringLiteral("violationSamples"), _violationSamples},
                {QStringLiteral("progress"), _completedLaps + std::max(0.0, _signedProgress) / length},
                {QStringLiteral("distance"), _travelDistance},
                {QStringLiteral("telemetryValid"), true},
                {QStringLiteral("windSpeed"), windSpeed()},
                {QStringLiteral("windSource"), _windSource},
                {QStringLiteral("windLevel"), windLevel()},
                {QStringLiteral("windFactor"), windFactor()},
                {QStringLiteral("windValid"), windValid()}};
    if (_recordedPath.isEmpty() || (now - _lastRecorded >= 200 && movement > 0.01)) {
        if (_recordedPath.size() >= MAX_RECORDED_POINTS) {
            _finish(tr("训练记录已达到一小时容量，请结束本次记录。"));
            return;
        }
        _recordedPath.append(QVariant::fromValue(coordinate));
        _lastRecorded = now;
    }
    emit sampleChanged();
    if (timedOut) {
        _finish(tr("已达到自定义时长限制，训练记录已结束。"), QStringLiteral("time_up"));
    } else if (_exerciseMode == 2 && _completedLaps > 0) {
        _finish(tr("已完成自旋和八字飞行，请查看本次偏差记录。"), QStringLiteral("completed"));
    } else {
        _updateAlarms(horizontalError, altitudeError, headingError, speed);
    }
}

void CaacFigureEightController::_sampleSpin()
{
    if (!_vehicle) {
        return;
    }
    const auto now = _sessionClock.elapsed();
    const double phaseElapsed = _phaseClock.elapsed() / 1000.0;
    const auto coordinate = _vehicle->coordinate();
    const double speed = _vehicle->groundSpeed()->rawValue().toDouble();
    const double heading = _vehicle->heading()->rawValue().toDouble();
    const double change = wrappedDifference(heading - _spinLastHeading);
    if (std::abs(change) > 90) {
        _finish(tr("自旋航向遥测发生跳变，训练记录已结束。"), QStringLiteral("telemetry"));
        return;
    }
    _spinAngle += change * (_phase == Phase::ReverseSpin ? -1 : 1);
    _spinLastHeading = heading;
    const double horizontalError = QGCGeo::geodesicDistance(_spinAnchor, coordinate);
    const double altitudeError = std::abs(_vehicle->altitudeRelative()->rawValue().toDouble() - _baselineAltitude);
    const auto appliedLimits = effectiveLimits();
    bool sampleViolated = false;
    const auto violation = [this, &sampleViolated](bool active, const QString& text) {
        if (active) {
            sampleViolated = true;
            if (!_violations.contains(text)) {
                _violations.append(text);
            }
        }
    };
    violation(horizontalError > appliedLimits.value(QStringLiteral("horizontal")).toDouble(), tr("水平偏差超限"));
    violation(altitudeError > appliedLimits.value(QStringLiteral("vertical")).toDouble(), tr("高度偏差超限"));
    if (sampleViolated) {
        ++_violationSamples;
    }
    _travelDistance += QGCGeo::geodesicDistance(_previousPosition, coordinate);
    _previousPosition = coordinate;
    _lastSample = now;
    _metrics = {{QStringLiteral("elapsed"), now / 1000.0},
                {QStringLiteral("phaseElapsed"), phaseElapsed},
                {QStringLiteral("spinAngle"), std::max(0.0, _spinAngle)},
                {QStringLiteral("altitudeError"), altitudeError},
                {QStringLiteral("horizontalError"), horizontalError},
                {QStringLiteral("headingError"), 0.0},
                {QStringLiteral("speed"), speed},
                {QStringLiteral("completedLaps"), 0},
                {QStringLiteral("violations"), _violations},
                {QStringLiteral("violationSamples"), _violationSamples},
                {QStringLiteral("progress"), std::clamp(_spinAngle / 360.0, 0.0, 1.0)},
                {QStringLiteral("distance"), _travelDistance},
                {QStringLiteral("telemetryValid"), true},
                {QStringLiteral("windSpeed"), windSpeed()},
                {QStringLiteral("windSource"), _windSource},
                {QStringLiteral("windLevel"), windLevel()},
                {QStringLiteral("windFactor"), windFactor()},
                {QStringLiteral("windValid"), windValid()}};
    if (_recordedPath.isEmpty() || now - _lastRecorded >= 200) {
        if (_recordedPath.size() >= MAX_RECORDED_POINTS) {
            _finish(tr("训练记录已达到容量上限。"));
            return;
        }
        _recordedPath.append(QVariant::fromValue(coordinate));
        _lastRecorded = now;
    }
    emit sampleChanged();
    if (phaseElapsed > 30) {
        _finish(tr("原地自旋已超过 30 秒，训练记录已结束。"), QStringLiteral("time_up"));
    } else if (_spinAngle >= 359.5) {
        if (phaseElapsed < 5) {
            _finish(tr("原地自旋少于 5 秒，请降低转速后重新开始。"), QStringLiteral("spin_too_fast"));
        } else if (_standard == 2 && _phase == Phase::Spin) {
            _beginPhase(true);
        } else if (_exerciseMode == 2) {
            _beginPhase();
        } else {
            _finish(tr("已完成 360 度原地自旋，请查看本次偏差记录。"), QStringLiteral("completed"));
        }
    } else {
        _updateAlarms(horizontalError, altitudeError, qQNaN(), speed);
    }
}

void CaacFigureEightController::_updateAlarms(double horizontalError, double altitudeError, double headingError,
                                              double speed)
{
    const auto applied = effectiveLimits();
    const bool speedEnabled = _limits.value(QStringLiteral("speedEnabled")).toBool();
    const QList<QPair<QString, QString>> messages{{QStringLiteral("area"), tr("超出考试区域，请返回")},
                                                  {QStringLiteral("altitude"), tr("高度偏差过大，请调整高度")},
                                                  {QStringLiteral("heading"), tr("航向偏差过大，请调整航向")},
                                                  {QStringLiteral("speed_high"), tr("速度过快，请减速")},
                                                  {QStringLiteral("speed_low"), tr("速度过慢，请适当加速")}};
    QSet<QString> active;
    if (horizontalError > applied.value(QStringLiteral("horizontal")).toDouble()) {
        active.insert(QStringLiteral("area"));
    }
    if (altitudeError > applied.value(QStringLiteral("vertical")).toDouble()) {
        active.insert(QStringLiteral("altitude"));
    }
    if (std::isfinite(headingError) && headingError > applied.value(QStringLiteral("heading")).toDouble()) {
        active.insert(QStringLiteral("heading"));
    }
    if (speedEnabled && speed > _limits.value(QStringLiteral("maxSpeed")).toDouble()) {
        active.insert(QStringLiteral("speed_high"));
    }
    if (speedEnabled && _phase == Phase::FigureEight && speed < _limits.value(QStringLiteral("minSpeed")).toDouble()) {
        active.insert(QStringLiteral("speed_low"));
    }
    if (active.isEmpty() && _hadActiveAlarms) {
        emit alarmsCleared();
    }
    _hadActiveAlarms = !active.isEmpty();
    const auto now = _clock.elapsed();
    QString candidate;
    QString text;
    for (const auto& message : messages) {
        if (active.contains(message.first) && now - _alarmLastAnnounced.value(message.first, -6000) >= 6000) {
            candidate = message.first;
            text = message.second;
            break;
        }
    }
    if (candidate != _alarmCandidate) {
        _alarmCandidate = candidate;
        _alarmCandidateClock.restart();
        return;
    }
    if (candidate.isEmpty() || !_alarmCandidateClock.isValid() || _alarmCandidateClock.elapsed() < 300) {
        return;
    }
    if (now - _lastAlarm < 2000 || now - _alarmLastAnnounced.value(candidate, -6000) < 6000) {
        return;
    }
    _lastAlarm = now;
    _alarmLastAnnounced.insert(candidate, now);
    _announce(candidate, text);
}

void CaacFigureEightController::_finish(const QString& message, const QString& cue)
{
    _timer.stop();
    _running = false;
    _phase = Phase::Finished;
    _hadActiveAlarms = false;
    emit alarmsCleared();
    _status = message;
    _announce(cue, message);
    emit sessionChanged();
}

bool CaacFigureEightController::_fail(const QString& message)
{
    _lastError = message;
    emit lastErrorChanged();
    return false;
}

void CaacFigureEightController::_clearError()
{
    if (!_lastError.isEmpty()) {
        _lastError.clear();
        emit lastErrorChanged();
    }
}
