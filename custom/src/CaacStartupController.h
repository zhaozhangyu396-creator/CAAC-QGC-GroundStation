#pragma once

#include <QtCore/QObject>
#include <QtCore/QString>
#include <QtQmlIntegration/QtQmlIntegration>

class QQmlApplicationEngine;
class QQuickWindow;

class CaacStartupController : public QObject
{
    Q_OBJECT
    QML_ELEMENT
    QML_UNCREATABLE("Owned by the application")
    Q_PROPERTY(int progress READ progress NOTIFY progressChanged)
    Q_PROPERTY(QString statusText READ statusText NOTIFY statusTextChanged)
    Q_PROPERTY(bool failed READ failed NOTIFY failedChanged)
    Q_PROPERTY(bool finished READ finished NOTIFY finishedChanged)

public:
    enum class Stage
    {
        Preparing,
        Interface,
        VideoBackend,
        Services,
        FirstFrame
    };

    explicit CaacStartupController(QObject* parent = nullptr);
    ~CaacStartupController() override;

    bool show();
    void setStage(Stage stage);
    void markBackendReady();
    void markServicesReady();
    void markMainFrameReady();
    void fail(const QString& message);
    void close();

    int progress() const { return _progress; }

    QString statusText() const { return _statusText; }

    bool failed() const { return _failed; }

    bool finished() const { return _finished; }

    Q_INVOKABLE void requestClose();

signals:
    void progressChanged();
    void statusTextChanged();
    void failedChanged();
    void finishedChanged();
    void closeRequested();

private slots:
    void _completionFramePresented();

private:
    void _setProgress(int progress, const QString& statusText);
    void _tryComplete();

    QQmlApplicationEngine* _engine = nullptr;
    QQuickWindow* _window = nullptr;
    QString _statusText;
    int _progress = 0;
    bool _backendReady = false;
    bool _servicesReady = false;
    bool _mainFrameReady = false;
    bool _finishing = false;
    bool _finished = false;
    bool _failed = false;
    bool _closing = false;
};
