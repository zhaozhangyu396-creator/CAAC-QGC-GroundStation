#include "CaacActivationController.h"

#include <utility>

#include <QtCore/QDir>
#include <QtCore/QFile>
#include <QtCore/QFileInfo>
#include <QtCore/QPointer>
#include <QtCore/QSaveFile>
#include <QtCore/QStandardPaths>
#include <QtCore/QSysInfo>
#include <QtCore/QTimer>
#include <QtGui/QClipboard>
#include <QtGui/QGuiApplication>
#include <QtGui/QWindow>
#include <QtQml/QQmlApplicationEngine>
#include <QtQuickControls2/QQuickStyle>

#include "CaacActivationCodec.h"
#include "CaacActivationPublicKey.h"
#include "QGCCorePlugin.h"

CaacActivationController::CaacActivationController(QObject* parent)
    : CaacActivationController(QByteArray::fromHex(QByteArrayLiteral(CAAC_ACTIVATION_PUBLIC_KEY_HEX)),
                               QSysInfo::machineUniqueId(),
                               QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation) +
                                   QStringLiteral("/CAAC/GroundStation/activation.license"),
                               parent)
{}

CaacActivationController::CaacActivationController(const QByteArray& publicKey, const QByteArray& machineIdentity,
                                                   const QString& licensePath, QObject* parent)
    : QObject(parent)
    , _publicKey(publicKey)
    , _machineHash(CaacActivationCodec::machineHash(machineIdentity))
    , _licensePath(licensePath)
{
    _loadSavedLicense();
}

void CaacActivationController::_loadSavedLicense()
{
    QFile file(_licensePath);
    const auto code = file.open(QIODevice::ReadOnly) ? QString::fromUtf8(file.read(4097)) : QString();
    const auto error = CaacActivationCodec::verify(code, _publicKey, _machineHash);
    const bool activated = error.isEmpty();
    if (_activated != activated) {
        _activated = activated;
        emit activatedChanged();
    }
    _setError(!code.isEmpty() || _publicKey.size() != 32 || _machineHash.isEmpty() ? error : QString());
}

QString CaacActivationController::machineCode() const
{
    return CaacActivationCodec::machineCode(_machineHash);
}

void CaacActivationController::_setError(const QString& message)
{
    if (_errorMessage != message) {
        _errorMessage = message;
        emit errorMessageChanged();
    }
}

bool CaacActivationController::activate(const QString& code)
{
    const auto error = code.size() > 4096 ? tr("Invalid activation code format. Paste the complete code.")
                                          : CaacActivationCodec::verify(code, _publicKey, _machineHash);
    if (!error.isEmpty()) {
        _setError(error);
        return false;
    }
    const auto normalized = CaacActivationCodec::normalize(code).toUtf8();
    QSaveFile file(_licensePath);
    if (!QDir().mkpath(QFileInfo(_licensePath).absolutePath()) || !file.open(QIODevice::WriteOnly) ||
        file.write(normalized) != normalized.size() || !file.commit()) {
        _setError(tr("Unable to save the activation. Check write permissions and try again."));
        return false;
    }
    _setError({});
    if (!_activated) {
        _activated = true;
        emit activatedChanged();
    }
    return true;
}

void CaacActivationController::copyMachineCode() const
{
    QGuiApplication::clipboard()->setText(machineCode());
}

bool CaacActivationController::showWindow(QGCCorePlugin* plugin, QObject* parent, std::function<void()> onAuthorized)
{
    _loadSavedLicense();
    if (_activated) {
        return false;
    }
    if (QQuickStyle::name() != QStringLiteral("Basic")) {
        QQuickStyle::setStyle(QStringLiteral("Basic"));
    }
    auto* engine = plugin->createQmlApplicationEngine(parent);
    engine->setInitialProperties({{QStringLiteral("activation"), QVariant::fromValue(this)}});
    connect(
        this, &CaacActivationController::activatedChanged, engine,
        [this, plugin, engine, onAuthorized = std::move(onAuthorized)]() {
            if (!_activated) {
                return;
            }
            QTimer::singleShot(0, engine, [plugin, engine, onAuthorized]() {
                onAuthorized();
                for (auto* root : engine->rootObjects()) {
                    if (auto* window = qobject_cast<QWindow*>(root)) {
                        window->hide();
                    }
                }
                plugin->destroyQmlApplicationEngine(engine);
            });
        },
        Qt::SingleShotConnection);
    engine->load(QUrl(QStringLiteral("qrc:/qml/CAAC/Plan/CaacActivationWindow.qml")));
    if (engine->rootObjects().isEmpty()) {
        plugin->destroyQmlApplicationEngine(engine);
        QTimer::singleShot(0, QGuiApplication::instance(), &QCoreApplication::quit);
    }
    return true;
}
