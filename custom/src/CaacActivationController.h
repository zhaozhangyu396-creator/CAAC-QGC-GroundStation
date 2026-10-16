#pragma once

#include <functional>

#include <QtCore/QByteArray>
#include <QtCore/QObject>
#include <QtCore/QString>
#include <QtQmlIntegration/QtQmlIntegration>

class QGCCorePlugin;

class CaacActivationController : public QObject
{
    Q_OBJECT
    QML_ELEMENT
    QML_UNCREATABLE("Owned by the application plugin")
    Q_PROPERTY(bool activated READ activated NOTIFY activatedChanged)
    Q_PROPERTY(QString machineCode READ machineCode CONSTANT)
    Q_PROPERTY(QString errorMessage READ errorMessage NOTIFY errorMessageChanged)

public:
    explicit CaacActivationController(QObject* parent = nullptr);
    CaacActivationController(const QByteArray& publicKey, const QByteArray& machineIdentity, const QString& licensePath,
                             QObject* parent = nullptr);

    bool activated() const { return _activated; }

    QString machineCode() const;

    QString errorMessage() const { return _errorMessage; }

    Q_INVOKABLE bool activate(const QString& code);
    Q_INVOKABLE void copyMachineCode() const;
    bool showWindow(QGCCorePlugin* plugin, QObject* parent, std::function<void()> onAuthorized);

signals:
    void activatedChanged();
    void errorMessageChanged();

private:
    void _loadSavedLicense();
    void _setError(const QString& message);
    const QByteArray _publicKey;
    const QByteArray _machineHash;
    const QString _licensePath;
    QString _errorMessage;
    bool _activated = false;
};
