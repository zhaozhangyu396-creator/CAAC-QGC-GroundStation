#pragma once

#include "QGCCorePlugin.h"

class CaacCorePlugin : public QGCCorePlugin
{
    Q_OBJECT

public:
    explicit CaacCorePlugin(QObject* parent = nullptr);
    static QGCCorePlugin* instance();

    QUrl planViewEditor() const override;

    bool directMissionItemEditing() const override { return true; }
};
