#pragma once

#include <QtCore/QMap>
#include <QtCore/QObject>
#include <QtCore/QPointer>
#include <QtCore/QTimer>
#include <QtCore/QVariantList>
#include <QtPositioning/QGeoCoordinate>
#include <QtQmlIntegration/QtQmlIntegration>

#include "FirmwarePlugin.h"

class CaacFigureEightController;
class Vehicle;

/// Transfers a training boundary through the vehicle's fence manager and confirms remote settings.
class CaacFigureEightFenceController : public QObject
{
    Q_OBJECT
    QML_ELEMENT
    QML_UNCREATABLE("Owned by the figure-eight controller")
    Q_PROPERTY(double margin READ margin WRITE setMargin NOTIFY geometryChanged)
    Q_PROPERTY(QVariantList path READ path NOTIFY geometryChanged)
    Q_PROPERTY(bool showOnMap READ showOnMap WRITE setShowOnMap NOTIFY geometryChanged)
    Q_PROPERTY(bool busy READ busy NOTIFY stateChanged)
    Q_PROPERTY(bool applied READ applied NOTIFY stateChanged)
    Q_PROPERTY(bool canApply READ canApply NOTIFY stateChanged)
    Q_PROPERTY(bool canRead READ canRead NOTIFY stateChanged)
    Q_PROPERTY(bool replacementRequired READ replacementRequired NOTIFY stateChanged)
    Q_PROPERTY(QString status READ status NOTIFY stateChanged)
    Q_PROPERTY(QString actionDescription READ actionDescription NOTIFY stateChanged)
    Q_PROPERTY(QString backupFile READ backupFile NOTIFY stateChanged)

public:
    explicit CaacFigureEightFenceController(CaacFigureEightController* provider);

    double margin() const { return _margin; }

    void setMargin(double margin);
    QVariantList path() const;

    bool showOnMap() const { return _showOnMap; }

    void setShowOnMap(bool show);

    bool busy() const { return _stage != Stage::Idle; }

    bool applied() const { return _applied; }

    bool canApply() const;
    bool canRead() const;

    bool replacementRequired() const { return _replacementRequired; }

    QString status() const { return _status; }

    QString actionDescription() const;

    QString backupFile() const { return _backupFile; }

    Q_INVOKABLE bool apply(bool replaceExisting = false);
    Q_INVOKABLE void readFromVehicle();

signals:
    void geometryChanged();
    void stateChanged();

private:
#ifdef QGC_UNITTEST_BUILD
    friend class CaacFigureEightFenceControllerTest;
    FirmwarePlugin::GeoFencePolicy _testPolicy;
#endif
    enum class Stage
    {
        Idle,
        ReadExisting,
        ReadParameters,
        WritePreparation,
        Upload,
        VerifyGeometry,
        WriteParameters
    };

    void _setVehicle(Vehicle* vehicle);
    void _updateGeometry();
    void _updateVehicleState();
    bool _begin(bool applying, bool replaceExisting);
    QString _blockReason(bool requirePolicy = true) const;
    FirmwarePlugin::GeoFencePolicy _vehiclePolicy() const;
    void _loaded();
    void _sent(bool error);
    void _nextParameter();
    void _parametersRead();
    void _startActivation();
    void _upload();
    bool _matchesGeometry() const;
    bool _matchesParameters() const;
    bool _saveBackup();
    void _complete(const QString& status, bool applied = false);
    void _fail(const QString& reason);
    void _invalidate(const QString& reason);
    void _saveSettings() const;

    CaacFigureEightController* _provider = nullptr;
    QPointer<Vehicle> _vehicle;
    QList<QMetaObject::Connection> _vehicleConnections;
    QList<QMetaObject::Connection> _appliedParameterConnections;
    QMetaObject::Connection _parameterEcho;
    QList<QGeoCoordinate> _path;
    QList<QGeoCoordinate> _expectedPath;
    FirmwarePlugin::GeoFencePolicy _policy;
    QList<QPair<QString, QVariant>> _parameterSteps;
    QMap<QString, QVariant> _remoteParameters;
    QTimer _timeout;
    Stage _stage = Stage::Idle;
    int _parameterIndex = 0;
    double _margin = 3;
    bool _showOnMap = true;
    bool _applied = false;
    bool _applying = false;
    bool _replaceExisting = false;
    bool _replacementRequired = false;
    bool _geometryWritten = false;
    bool _writesStarted = false;
    QString _status;
    QString _backupFile;
};
