#include "CaacCorePlugin.h"

#include <QtCore/QApplicationStatic>

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
