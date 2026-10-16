#pragma once

#include <QtCore/QList>
#include <QtCore/QMap>

/// \brief MVC Controller for AirframeComponent.qml.
///
class AirframeComponentAirframes
{
public:
    typedef struct {
        QString name;
        int autostartId;
        QString vehicleClass;
        QString category;
    } AirframeInfo_t;

    typedef struct {
        QString name;
        QString imageResource;
        QList<AirframeInfo_t*> rgAirframeInfo;
    } AirframeType_t;

    static QMap<QString, AirframeComponentAirframes::AirframeType_t*>& get();

    /// Airframe types in display order: standard frames first, then the rest
    /// alphabetically by group name.
    static QList<AirframeType_t*> sortedTypes();

    static void clear();
    static void insert(const QString& group, const QString& image, const QString& name, int id,
                       const QString& vehicleClass);

protected:
    static QMap<QString, AirframeType_t*> rgAirframeTypes;

private:
};
