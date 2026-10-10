#include "CaacCorePlugin.h"

#include <QtCore/QApplicationStatic>
#include <QtCore/QJsonObject>
#include <QtPositioning/QGeoCoordinate>

#include "PlanMasterController.h"

Q_APPLICATION_STATIC(CaacCorePlugin, caacCorePluginInstance);

CaacCorePlugin::CaacCorePlugin(QObject* parent)
    : QGCCorePlugin(parent)
{}

QGCCorePlugin* CaacCorePlugin::instance()
{
    return caacCorePluginInstance();
}

QUrl CaacCorePlugin::planViewEditor() const
{
    return QUrl(QStringLiteral("qrc:/qml/CAAC/Plan/CaacRouteEditor.qml"));
}

void CaacCorePlugin::postSaveToJson(PlanMasterController* controller, QJsonObject& json)
{
    const auto center = controller->property("caacRotationCenter").value<QGeoCoordinate>();
    if (center.isValid()) {
        json[QStringLiteral("caac")] = QJsonObject{{QStringLiteral("version"), 1},
                                                   {QStringLiteral("centerLatitude"), center.latitude()},
                                                   {QStringLiteral("centerLongitude"), center.longitude()}};
    }
}

void CaacCorePlugin::postLoadFromJson(PlanMasterController* controller, QJsonObject& json)
{
    const auto metadata = json.value(QStringLiteral("caac")).toObject();
    QGeoCoordinate center;
    if (metadata.value(QStringLiteral("version")).toInt() == 1 &&
        metadata.value(QStringLiteral("centerLatitude")).isDouble() &&
        metadata.value(QStringLiteral("centerLongitude")).isDouble()) {
        center = QGeoCoordinate(metadata.value(QStringLiteral("centerLatitude")).toDouble(),
                                metadata.value(QStringLiteral("centerLongitude")).toDouble());
    }
    controller->setProperty("caacRotationCenter", QVariant::fromValue(center));
}
