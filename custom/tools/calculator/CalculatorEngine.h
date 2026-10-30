#pragma once

#include <QtCore/QVariantMap>
#include <QtQml/QJSEngine>

class CalculatorEngine
{
public:
    CalculatorEngine();

    [[nodiscard]] bool isReady() const;
    double parsed(const QString& text);
    QVariantMap calculate(const QString& function, const QVariantList& arguments);

private:
    QJSEngine _engine;
    bool _ready = false;
};
