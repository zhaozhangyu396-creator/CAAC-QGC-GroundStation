#include "CaacFigureEightController.h"

#include <algorithm>
#include <cmath>
#include <limits>
#include <numbers>

#include <QtCore/QJsonArray>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>
#include <QtCore/QSettings>
#include <QtGui/QQuaternion>

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
constexpr qint64 CENTRE_HOLD_MS = 3000;
constexpr double CENTRE_MIN_HEIGHT = 1.5;
constexpr double CENTRE_RADIUS = 1.0;
constexpr double CENTRE_HEADING_TOLERANCE = 30.0;
constexpr double HOVER_MAX_SPEED = 0.3;
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
    // MD-92-FS-02 (2024-05-11), attachment 2, pp. 14–16: helicopters, level-one wind, 0–4 m/s.
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
    _rebuildPath();
    _saveSettings();
    emit settingsChanged();
    emit siteChanged();
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
                                                                      const QGeoCoordinate& pointB, bool instructor)
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
            const double lateral = radius * std::sin(angle) * (instructor ? 1 : -1);
            path.append(QGCGeo::geodesicDestination(
                origin, bearing + std::atan2(lateral, along) * 180 / std::numbers::pi, std::hypot(along, lateral)));
        }
    }
    return path;
}

void CaacFigureEightController::_rebuildPath()
{
    _reference = referenceCoordinates(_pointA, _pointB, _standard == 2);
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
        _attemptNumber = 0;
        _spinPassed = false;
        _saveSettings();
        emit settingsChanged();
        emit sessionChanged();
    }
}

QString CaacFigureEightController::phase() const
{
    switch (_phase) {
        case Phase::Positioning:
            return tr("中心筒就位");
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
    if (ready) {
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
    const bool spinCompleted = _phase == Phase::Spin || _phase == Phase::ReverseSpin;
    if (reverseSpin) {
        _phase = Phase::ReverseSpin;
    } else if (_phase == Phase::Spin || _phase == Phase::ReverseSpin) {
        _phase = Phase::FigureEight;
    } else {
        _phase = _exerciseMode == 0 || (_exerciseMode == 2 && _spinPassed) ? Phase::FigureEight : Phase::Spin;
    }
    _phaseClock.invalidate();
    _subjectStarted = false;
    emit alarmsCleared();
    _previousPosition = _vehicle->coordinate();
    _phaseAnchor = _previousPosition;
    _lastSample = _sessionClock.elapsed();
    if (_phase == Phase::FigureEight) {
        _previousProgress = 0;
        double distance = 0;
        double heading = 0;
        _previousProgress = _nearestProgress(_vehicle->coordinate(), distance, heading);
        _status = tr("可以从左圆或右圆开始水平八字飞行，运动离开中心筒半径 1 米范围后开始评分。");
        _announce(spinCompleted ? QStringLiteral("spin_completed") : QStringLiteral("figure_eight"),
                  spinCompleted ? tr("自旋完成，可以开始水平八字飞行。") : tr("可以开始水平八字飞行。"));
    } else {
        _phaseAnchor = _reference.first();
        _spinLastHeading = _headingDegrees;
        _spinAngle = 0;
        _status = _phase == Phase::ReverseSpin ? tr("请手动逆时针完成 360 度原地自旋，用时 5 至 30 秒。")
                                               : tr("请手动顺时针完成 360 度原地自旋，用时 5 至 30 秒。");
        _announce(_phase == Phase::ReverseSpin ? QStringLiteral("spin_reverse") : QStringLiteral("spin"),
                  _phase == Phase::ReverseSpin ? tr("可以开始反向原地自旋") : tr("可以开始原地自旋"));
    }
    _metrics.remove(QStringLiteral("positioningPending"));
    emit sampleChanged();
    emit sessionChanged();
}

void CaacFigureEightController::_prepareAttempt()
{
    _phase = Phase::Positioning;
    _hoverSince.invalidate();
    _phaseClock.invalidate();
    _subjectStarted = false;
    _signedProgress = 0;
    _completedLaps = 0;
    _spinAngle = 0;
    _violations.clear();
    _violationSamples = 0;
    emit alarmsCleared();
    _status =
        tr("请在中心筒半径 1 米范围内、高度不低于 1.5 米处对尾悬停 3 秒。机尾朝向起飞位置，允许 "
           "±30°；水平和垂直速度均不超过 0.3 米/秒。");
    _announce(_attemptNumber > 1 ? QStringLiteral("retry") : QStringLiteral("positioning"),
              _attemptNumber > 1 ? tr("请返回中心筒上方，在不低于一点五米的高度对尾悬停三秒后重试。")
                                 : tr("请将无人机飞至中心筒上方一点五米处，并保持对位悬停。"));
    emit sessionChanged();
}

void CaacFigureEightController::_samplePositioning()
{
    if (!_vehicle || _reference.isEmpty()) {
        return;
    }
    const auto coordinate = _vehicle->coordinate();
    const double altitude = _vehicle->altitudeRelative()->rawValue().toDouble();
    const double distance = QGCGeo::geodesicDistance(_reference.first(), coordinate);
    const double speed = _vehicle->groundSpeed()->rawValue().toDouble();
    const double verticalSpeed = _vehicle->climbRate()->rawValue().toDouble();
    const bool takeoffValid =
        _takeoffPosition.isValid() && QGCGeo::geodesicDistance(_takeoffPosition, coordinate) >= 0.5;
    const double idealHeading = takeoffValid ? QGCGeo::geodesicAzimuth(_takeoffPosition, coordinate) : qQNaN();
    const double headingError = std::abs(wrappedDifference(_headingDegrees - idealHeading));
    QStringList pending;
    if (!_vehicle->armed() || !_vehicle->airborne()) {
        pending.append(tr("等待无人机解锁起飞"));
    }
    if (distance > CENTRE_RADIUS) {
        pending.append(tr("请进入中心筒半径 1 米范围"));
    }
    if (altitude < CENTRE_MIN_HEIGHT) {
        pending.append(tr("请保持高度不低于 1.5 米"));
    }
    if (!takeoffValid) {
        pending.append(tr("当前位置距起飞位置过近，无法确定对尾方向"));
    } else if (headingError > CENTRE_HEADING_TOLERANCE + 1e-4) {  // MAVLink yaw is a float in radians.
        pending.append(tr("请将机尾朝向起飞位置，角度误差不超过 30°"));
    }
    if (speed > HOVER_MAX_SPEED || !std::isfinite(verticalSpeed) || std::abs(verticalSpeed) > HOVER_MAX_SPEED) {
        pending.append(tr("请保持悬停，水平和垂直速度均不超过 0.3 米/秒"));
    }
    const bool inPosition = pending.isEmpty();
    if (!inPosition) {
        _hoverSince.invalidate();
    } else if (!_hoverSince.isValid()) {
        _hoverSince.start();
    }
    const auto now = _sessionClock.elapsed();
    const double stableSeconds = _hoverSince.isValid() ? _hoverSince.elapsed() / 1000.0 : 0;
    _metrics = {{QStringLiteral("elapsed"), now / 1000.0},
                {QStringLiteral("centreDistance"), distance},
                {QStringLiteral("hoverStableSeconds"), std::min(3.0, stableSeconds)},
                {QStringLiteral("relativeAltitude"), altitude},
                {QStringLiteral("positioningPending"), pending},
                {QStringLiteral("idealHeading"), idealHeading},
                {QStringLiteral("evaluatedHeading"), _headingDegrees},
                {QStringLiteral("tailHeadingError"), headingError},
                {QStringLiteral("verticalSpeed"), verticalSpeed},
                {QStringLiteral("subjectStarted"), false},
                {QStringLiteral("altitudeError"), 0.0},
                {QStringLiteral("horizontalError"), distance},
                {QStringLiteral("headingError"), headingError},
                {QStringLiteral("speed"), speed},
                {QStringLiteral("completedLaps"), 0},
                {QStringLiteral("violationSamples"), 0},
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
    if (inPosition && _hoverSince.elapsed() >= CENTRE_HOLD_MS) {
        _baselineAltitude = altitude;
        _beginPhase();
    }
}

void CaacFigureEightController::_failAttempt(const QString& reason, const QString& cue)
{
    _lastAttemptFailure = reason;
    if (_attemptNumber < attemptLimit()) {
        emit alarmsCleared();
        _announce(cue, tr("%1，本次考试未通过。").arg(reason));
        ++_attemptNumber;
        _prepareAttempt();
    } else {
        _finish(tr("%1 考试未通过，%2 次机会已用完。").arg(reason).arg(attemptLimit()), cue);
    }
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
    _headingDegrees = qQNaN();
    _usingAttitudeQuaternion = false;
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
            } else if (message.msgid == MAVLINK_MSG_ID_ATTITUDE && !_usingAttitudeQuaternion) {
                mavlink_attitude_t attitude{};
                mavlink_msg_attitude_decode(&message, &attitude);
                if (std::isfinite(attitude.yaw)) {
                    _headingDegrees = std::fmod(wrappedDifference(attitude.yaw * 180 / std::numbers::pi) + 360, 360);
                    _lastAttitude = now;
                }
            } else if (message.msgid == MAVLINK_MSG_ID_ATTITUDE_QUATERNION) {
                mavlink_attitude_quaternion_t attitude{};
                mavlink_msg_attitude_quaternion_decode(&message, &attitude);
                QQuaternion quaternion(attitude.q1, attitude.q2, attitude.q3, attitude.q4);
                if (quaternion.lengthSquared() < 0.5f) {
                    return;
                }
                const QQuaternion offset(attitude.repr_offset_q[0], attitude.repr_offset_q[1],
                                         attitude.repr_offset_q[2], attitude.repr_offset_q[3]);
                if (offset.length() >= 0.5f) {
                    quaternion *= offset;
                }
                const float q[] = {quaternion.scalar(), quaternion.x(), quaternion.y(), quaternion.z()};
                float roll = 0;
                float pitch = 0;
                float yaw = 0;
                mavlink_quaternion_to_euler(q, &roll, &pitch, &yaw);
                if (std::isfinite(yaw)) {
                    _headingDegrees = std::fmod(wrappedDifference(yaw * 180 / std::numbers::pi) + 360, 360);
                    _usingAttitudeQuaternion = true;
                    _lastAttitude = now;
                }
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
        if (_running && _phase == Phase::Positioning && armed) {
            _takeoffPosition = _vehicle ? _vehicle->armedPosition() : QGeoCoordinate();
        }
        if (_running && _phase != Phase::Positioning && !armed) {
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
        !std::isfinite(_vehicle->heading()->rawValue().toDouble()) || !std::isfinite(_headingDegrees) ||
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
    if (_reference.isEmpty()) {
        return _fail(tr("请先设置有效的 A、B 点场地。"));
    }
    if (!windValid()) {
        return _fail(tr("地面风速数据缺失或已超过 10 秒，请检查传感器后开始训练。"));
    }
    if (!_vehicle || !_telemetryReady()) {
        return _fail(tr("请连接飞行器，并等待有效的位置、高度、航向和速度遥测。"));
    }
    clearRecording();
    _takeoffPosition = _vehicle->armed() ? _vehicle->armedPosition() : _vehicle->coordinate();
    if (!_takeoffPosition.isValid()) {
        _takeoffPosition = _vehicle->homePosition();
    }
    _previousPosition = _vehicle->coordinate();
    _sessionClock.start();
    _lastSample = 0;
    _lastRecorded = 0;
    _running = true;
    _readyAnnounced = true;
    _phase = Phase::Idle;
    _attemptNumber = 1;
    _clearError();
    _timer.start();
    _prepareAttempt();
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
    _hoverSince.invalidate();
    _phaseClock.invalidate();
    _attemptNumber = 0;
    _spinPassed = false;
    _subjectStarted = false;
    _lastAttemptFailure.clear();
    _phase = Phase::Idle;
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
    emit sessionChanged();
}

double CaacFigureEightController::_nearestProgress(const QGeoCoordinate& coordinate, double& distance,
                                                   double& heading) const
{
    distance = std::numeric_limits<double>::infinity();
    heading = 0;
    double result = 0;
    int nearestCircle = 0;
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
            nearestCircle = i <= POINTS_PER_CIRCLE ? 0 : 1;
        }
    }
    if (!_reference.isEmpty()) {
        const double radius = QGCGeo::geodesicDistance(_pointA, _pointB) / 4;
        const double bearing = QGCGeo::geodesicAzimuth(_pointA, _pointB);
        const auto centre =
            QGCGeo::geodesicDestination(_reference.first(), bearing + (nearestCircle == 0 ? 180 : 0), radius);
        if (QGCGeo::geodesicDistance(centre, coordinate) > 0.001) {
            const bool clockwise = _standard == 2 ? nearestCircle == 0 : nearestCircle == 1;
            heading = std::fmod(QGCGeo::geodesicAzimuth(centre, coordinate) + (clockwise ? 90 : -90) + 360, 360);
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
    if (!_vehicle || _vehicle != MultiVehicleManager::instance()->activeVehicle() || !_telemetryReady()) {
        _metrics[QStringLiteral("telemetryValid")] = false;
        emit sampleChanged();
        _finish(tr("飞行状态或遥测已失效，训练记录已结束，未判定完成。"), QStringLiteral("telemetry"));
        return;
    }
    if (_phase == Phase::Positioning) {
        _samplePositioning();
        return;
    }
    if (!_vehicle->armed() || !_vehicle->airborne()) {
        _finish(tr("飞行器已结束飞行，考试未完成。"));
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
    const double evaluatedHeading = std::fmod(_headingDegrees + (_standard == 2 ? 180 : 0), 360);
    const double headingError = std::abs(wrappedDifference(evaluatedHeading - tangentHeading));
    const double length = _cumulativeLength.last();
    const double movement = QGCGeo::geodesicDistance(_previousPosition, coordinate);
    const double centreDistance = QGCGeo::geodesicDistance(_reference.first(), coordinate);
    const double seconds = std::max(0.0, (now - _lastSample) / 1000.0);
    const double plausibleMovement = std::max(2.0, (std::max(speed, 0.0) + 5.0) * seconds);
    if (movement > plausibleMovement) {
        _finish(tr("位置遥测发生跳变，训练记录已结束，未判定完成。"), QStringLiteral("telemetry"));
        return;
    }
    if (!_subjectStarted && centreDistance > CENTRE_RADIUS &&
        (QGCGeo::geodesicDistance(_phaseAnchor, coordinate) >= 0.25 || speed > HOVER_MAX_SPEED)) {
        _subjectStarted = true;
        _phaseClock.start();
        // Both lobes start at the shared centre. Anchor progress to the lobe actually entered.
        _previousProgress = progress < length / 2 ? 0 : length / 2;
    }
    const double delta = std::remainder(progress - _previousProgress, length);
    if (_subjectStarted && horizontalError <= appliedLimits.value(QStringLiteral("horizontal")).toDouble() &&
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
    QString failureCue;
    const auto recordViolation = [this, &sampleViolated, &failureCue](bool violated, const QString& message,
                                                                      const QString& cue) {
        sampleViolated = sampleViolated || violated;
        if (violated && !_violations.contains(message)) {
            _violations.append(message);
        }
        if (violated && failureCue.isEmpty()) {
            failureCue = cue;
        }
    };
    recordViolation(_subjectStarted && horizontalError > appliedLimits.value(QStringLiteral("horizontal")).toDouble(),
                    tr("水平偏差超限"), QStringLiteral("area"));
    recordViolation(_subjectStarted && altitudeError > appliedLimits.value(QStringLiteral("vertical")).toDouble(),
                    tr("高度偏差超限"), QStringLiteral("altitude"));
    recordViolation(_subjectStarted && headingError > appliedLimits.value(QStringLiteral("heading")).toDouble(),
                    tr("航向偏差超限"), QStringLiteral("heading"));
    if (_subjectStarted && _limits.value(QStringLiteral("speedEnabled")).toBool()) {
        recordViolation(speed < _limits.value(QStringLiteral("minSpeed")).toDouble() ||
                            speed > _limits.value(QStringLiteral("maxSpeed")).toDouble(),
                        tr("自定义速度超限"),
                        speed < _limits.value(QStringLiteral("minSpeed")).toDouble() ? QStringLiteral("speed_low")
                                                                                     : QStringLiteral("speed_high"));
    }
    const bool timedOut = _subjectStarted && _limits.value(QStringLiteral("durationEnabled")).toBool() &&
                          _phaseClock.elapsed() / 1000.0 > _limits.value(QStringLiteral("maxDuration")).toDouble();
    recordViolation(timedOut, tr("自定义训练时长超限"), QStringLiteral("time_up"));
    if (sampleViolated) {
        ++_violationSamples;
    }
    _metrics = {{QStringLiteral("elapsed"), elapsed},
                {QStringLiteral("relativeAltitude"), _vehicle->altitudeRelative()->rawValue().toDouble()},
                {QStringLiteral("centreDistance"), centreDistance},
                {QStringLiteral("subjectStarted"), _subjectStarted},
                {QStringLiteral("hoverStableSeconds"), 3.0},
                {QStringLiteral("altitudeError"), altitudeError},
                {QStringLiteral("horizontalError"), horizontalError},
                {QStringLiteral("headingError"), headingError},
                {QStringLiteral("idealHeading"), tangentHeading},
                {QStringLiteral("evaluatedHeading"), evaluatedHeading},
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
    if (sampleViolated) {
        _failAttempt(_violations.join(tr("、")), failureCue);
    } else if (_completedLaps > 0) {
        _finish(tr("考试成功。"), QStringLiteral("completed"));
    }
}

void CaacFigureEightController::_sampleSpin()
{
    if (!_vehicle) {
        return;
    }
    const auto now = _sessionClock.elapsed();
    const auto coordinate = _vehicle->coordinate();
    const double speed = _vehicle->groundSpeed()->rawValue().toDouble();
    const double heading = _headingDegrees;
    const double change = wrappedDifference(heading - _spinLastHeading);
    if (std::abs(change) > 90) {
        _finish(tr("自旋航向遥测发生跳变，训练记录已结束。"), QStringLiteral("telemetry"));
        return;
    }
    _spinAngle += change * (_phase == Phase::ReverseSpin ? -1 : 1);
    _spinLastHeading = heading;
    if (!_subjectStarted && std::abs(_spinAngle) >= 3) {
        _subjectStarted = true;
        _phaseClock.start();
    }
    const double phaseElapsed = _subjectStarted ? _phaseClock.elapsed() / 1000.0 : 0;
    const double horizontalError = QGCGeo::geodesicDistance(_phaseAnchor, coordinate);
    const double altitudeError = std::abs(_vehicle->altitudeRelative()->rawValue().toDouble() - _baselineAltitude);
    const auto appliedLimits = effectiveLimits();
    bool sampleViolated = false;
    QString failureCue;
    const auto violation = [this, &sampleViolated, &failureCue](bool active, const QString& text, const QString& cue) {
        if (active) {
            sampleViolated = true;
            if (!_violations.contains(text)) {
                _violations.append(text);
            }
            if (failureCue.isEmpty()) {
                failureCue = cue;
            }
        }
    };
    violation(horizontalError > appliedLimits.value(QStringLiteral("horizontal")).toDouble(), tr("水平偏差超限"),
              QStringLiteral("area"));
    violation(altitudeError > appliedLimits.value(QStringLiteral("vertical")).toDouble(), tr("高度偏差超限"),
              QStringLiteral("altitude"));
    violation(_subjectStarted && _limits.value(QStringLiteral("speedEnabled")).toBool() &&
                  speed > _limits.value(QStringLiteral("maxSpeed")).toDouble(),
              tr("自定义速度超限"), QStringLiteral("speed_high"));
    if (sampleViolated) {
        ++_violationSamples;
    }
    _travelDistance += QGCGeo::geodesicDistance(_previousPosition, coordinate);
    _previousPosition = coordinate;
    _lastSample = now;
    _metrics = {{QStringLiteral("elapsed"), now / 1000.0},
                {QStringLiteral("phaseElapsed"), phaseElapsed},
                {QStringLiteral("relativeAltitude"), _vehicle->altitudeRelative()->rawValue().toDouble()},
                {QStringLiteral("centreDistance"), QGCGeo::geodesicDistance(_reference.first(), coordinate)},
                {QStringLiteral("hoverStableSeconds"), 3.0},
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
    if (sampleViolated) {
        _failAttempt(_violations.join(tr("、")), failureCue);
    } else if (phaseElapsed > 30) {
        _failAttempt(tr("原地自旋已超过 30 秒。"), QStringLiteral("spin_timeout"));
    } else if (_spinAngle >= 359.5) {
        if (phaseElapsed < 5) {
            _failAttempt(tr("原地自旋少于 5 秒，请降低转速后重新开始。"), QStringLiteral("spin_too_fast"));
        } else if (_standard == 2 && _phase == Phase::Spin) {
            _beginPhase(true);
        } else if (_exerciseMode == 2) {
            _spinPassed = true;
            _beginPhase();
        } else {
            _spinPassed = true;
            _finish(tr("考试成功。"), QStringLiteral("completed"));
        }
    }
}

void CaacFigureEightController::_finish(const QString& message, const QString& cue)
{
    _timer.stop();
    _running = false;
    _phase = Phase::Finished;
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
