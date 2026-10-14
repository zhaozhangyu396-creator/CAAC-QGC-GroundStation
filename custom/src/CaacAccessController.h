#pragma once

#include <QtCore/QObject>
#include <QtQmlIntegration/QtQmlIntegration>

class CaacAccessController : public QObject
{
    Q_OBJECT
    QML_ELEMENT
    QML_UNCREATABLE("Owned by the application plugin")
    Q_PROPERTY(bool configured READ configured NOTIFY configuredChanged)

public:
    explicit CaacAccessController(QObject* parent = nullptr,
                                  const QString& storageGroup = QStringLiteral("CAAC/Access"));
    bool configured() const;
    Q_INVOKABLE void ensureDefaultPassword();
    Q_INVOKABLE bool verify(const QString& password) const;
    Q_INVOKABLE bool setPassword(const QString& currentPassword, const QString& password);

signals:
    void configuredChanged();

private:
    const QString _storageGroup;
};
