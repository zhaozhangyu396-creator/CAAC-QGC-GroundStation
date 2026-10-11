#include <QtCore/QTimer>
#include <QtGui/QGuiApplication>
#include <QtGui/QWindow>
#include <QtNetwork/QLocalSocket>

#ifdef Q_OS_WIN
#include <QtCore/qt_windows.h>
#elif defined(Q_OS_LINUX)
#include <xcb/xcb.h>

#include <QtGui/qguiapplication_platform.h>
#endif

class FocusTestWindow : public QWindow
{
protected:
    void exposeEvent(QExposeEvent* event) override
    {
        QWindow::exposeEvent(event);
        if (isExposed()) {
            QTimer::singleShot(0, this, &QWindow::requestActivate);
        }
    }
};

int main(int argc, char* argv[])
{
    QGuiApplication application(argc, argv);
    const auto arguments = application.arguments();
    if (arguments.size() != 3) {
        return 1;
    }
    bool validWindow = false;
    const auto parentWindow = arguments[2].toULongLong(&validWindow);
    if (!validWindow || parentWindow == 0) {
        return 1;
    }
    QLocalSocket control;
    control.connectToServer(arguments[1]);
    if (!control.waitForConnected(5000)) {
        return 1;
    }
    FocusTestWindow window;
    window.setTitle(QStringLiteral("CAAC external focus test"));
    window.resize(320, 240);
    window.show();
    QObject::connect(&control, &QLocalSocket::readyRead, &window, [&] {
        if (control.canReadLine() && control.readLine() == "return\n") {
            // Activate the original native window from the foreground process.
            // Qt foreign windows do not support activation on Windows or retain X11 mapping state.
#ifdef Q_OS_WIN
            SetForegroundWindow(reinterpret_cast<HWND>(static_cast<quintptr>(parentWindow)));
#elif defined(Q_OS_LINUX)
            if (auto* native = application.nativeInterface<QNativeInterface::QX11Application>()) {
                xcb_set_input_focus(native->connection(), XCB_INPUT_FOCUS_PARENT,
                                    static_cast<xcb_window_t>(parentWindow), XCB_CURRENT_TIME);
                xcb_flush(native->connection());
            }
#endif
        }
    });
    return application.exec();
}
