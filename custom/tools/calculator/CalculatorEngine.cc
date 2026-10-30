#include "CalculatorEngine.h"

#include <limits>

#include <QtCore/QFile>

CalculatorEngine::CalculatorEngine()
{
    Q_INIT_RESOURCE(calculator_math);
    QFile source(QStringLiteral(":/calculator/CaacCalculatorMath.js"));
    if (!source.open(QIODevice::ReadOnly)) {
        return;
    }
    QString code = QString::fromUtf8(source.readAll());
    code.remove(QStringLiteral(".pragma library"));
    _ready = !_engine.evaluate(code, source.fileName()).isError();
}

bool CalculatorEngine::isReady() const
{
    return _ready;
}

double CalculatorEngine::parsed(const QString& text)
{
    if (!_ready) {
        return std::numeric_limits<double>::quiet_NaN();
    }
    return _engine.globalObject().property(QStringLiteral("parsed")).call({QJSValue(text)}).toNumber();
}

QVariantMap CalculatorEngine::calculate(const QString& function, const QVariantList& arguments)
{
    if (!_ready) {
        return {{QStringLiteral("ok"), false}, {QStringLiteral("error"), QStringLiteral("engine")}};
    }
    const QJSValue callable = _engine.globalObject().property(function);
    QJSValueList values;
    for (const QVariant& argument : arguments) {
        values.append(_engine.toScriptValue(argument));
    }
    const QJSValue result = callable.call(values);
    if (!callable.isCallable() || result.isError() || !result.isObject()) {
        return {{QStringLiteral("ok"), false}, {QStringLiteral("error"), QStringLiteral("engine")}};
    }
    return result.toVariant().toMap();
}
