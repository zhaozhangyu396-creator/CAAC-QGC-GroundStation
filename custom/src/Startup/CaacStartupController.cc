#include "CaacStartupController.h"

#include <QtQml/QQmlApplicationEngine>
#include <QtQuick/QQuickWindow>

CaacStartupController::CaacStartupController(QObject* parent)
    : QObject(parent)
{
    setStage(Stage::Preparing);
}

CaacStartupController::~CaacStartupController()
{
    close();
}

bool CaacStartupController::show()
{
    if (_engine) {
        return _window != nullptr;
    }
    _engine = new QQmlApplicationEngine(this);
    _engine->setInitialProperties({{QStringLiteral("startup"), QVariant::fromValue(this)}});
    _engine->load(QUrl(QStringLiteral("qrc:/CAAC/Startup/CaacStartupWindow.qml")));
    if (_engine->rootObjects().isEmpty()) {
        close();
        return false;
    }
    _window = qobject_cast<QQuickWindow*>(_engine->rootObjects().first());
    if (!_window) {
        close();
        return false;
    }
    return true;
}

void CaacStartupController::_setProgress(int progress, const QString& statusText)
{
    if (_closing || _failed || _finishing || progress < _progress) {
        return;
    }
    if (_progress != progress) {
        _progress = progress;
        emit progressChanged();
    }
    if (_statusText != statusText) {
        _statusText = statusText;
        emit statusTextChanged();
    }
}

void CaacStartupController::setStage(Stage stage)
{
    switch (stage) {
        case Stage::Preparing:
            _setProgress(0, tr("正在初始化系统…"));
            break;
        case Stage::Interface:
            _setProgress(15, tr("正在加载主界面…"));
            break;
        case Stage::VideoBackend:
            _setProgress(45, tr("正在初始化视频系统…"));
            break;
        case Stage::Services:
            _setProgress(70, tr("正在初始化设备与连接…"));
            break;
        case Stage::FirstFrame:
            _setProgress(95, tr("正在准备主界面…"));
            break;
    }
}

void CaacStartupController::completeVideoInitialization(bool available)
{
    _videoAvailable = available;
    _videoInitializationFinished = true;
    _tryComplete();
}

void CaacStartupController::markServicesReady()
{
    _servicesReady = true;
    _tryComplete();
}

void CaacStartupController::markMainFrameReady()
{
    _mainFrameReady = true;
    _tryComplete();
}

void CaacStartupController::_tryComplete()
{
    if (_closing || _failed || _finishing || !_videoInitializationFinished || !_servicesReady || !_mainFrameReady) {
        return;
    }
    _setProgress(100, _videoAvailable ? tr("系统已就绪") : tr("系统已就绪，图传暂不可用"));
    _finishing = true;
    if (_window) {
        // Arm after synchronization so an older queued swap cannot reveal the main window before the 100% frame.
        connect(
            _window, &QQuickWindow::afterSynchronizing, this,
            [this, window = _window]() {
                connect(window, &QQuickWindow::frameSwapped, this, &CaacStartupController::_completionFramePresented,
                        Qt::ConnectionType(Qt::QueuedConnection | Qt::SingleShotConnection));
            },
            Qt::ConnectionType(Qt::DirectConnection | Qt::SingleShotConnection));
        _window->update();
    }
}

void CaacStartupController::_completionFramePresented()
{
    if (_closing || !_finishing || _failed || _finished) {
        return;
    }
    _finished = true;
    if (_window) {
        _window->hide();
    }
    emit finishedChanged();
}

void CaacStartupController::fail(const QString& message)
{
    if (_failed || _finished) {
        return;
    }
    _failed = true;
    _statusText = message;
    emit failedChanged();
    emit statusTextChanged();
}

void CaacStartupController::close()
{
    _closing = true;
    _window = nullptr;
    delete _engine;
    _engine = nullptr;
}

void CaacStartupController::requestClose()
{
    if (_closing) {
        return;
    }
    _closing = true;
    emit closeRequested();
}
