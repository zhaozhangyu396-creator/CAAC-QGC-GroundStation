#include <QtCore/QCommandLineParser>
#include <QtCore/QTimer>
#include <QtGui/QIcon>
#include <QtWidgets/QApplication>
#include <QtWidgets/QMessageBox>

#include "CalculatorWindow.h"

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);
    QApplication::setStyle(QStringLiteral("Fusion"));
    Q_INIT_RESOURCE(calculator_icon);
    QCoreApplication::setApplicationName(QStringLiteral("CaacCalculator"));
    QCoreApplication::setApplicationVersion(QStringLiteral(CAAC_CALCULATOR_VERSION));
    QApplication::setApplicationDisplayName(QStringLiteral("飞虎角度计算器"));
    QApplication::setWindowIcon(QIcon(QStringLiteral(":/calculator/calculator.ico")));
    QCommandLineParser parser;
    parser.setApplicationDescription(QStringLiteral("独立离线角度计算器"));
    parser.addHelpOption();
    parser.addVersionOption();
    const QCommandLineOption smokeTest(QStringLiteral("smoke-test"), QStringLiteral("验证启动后自动退出"));
    parser.addOption(smokeTest);
    parser.process(app);

    CalculatorWindow window;
    if (!window.isReady()) {
        if (!parser.isSet(smokeTest)) {
            QMessageBox::critical(nullptr, QStringLiteral("启动失败"),
                                  QStringLiteral("无法加载程序资源，请重新安装。"));
        }
        return 1;
    }
    window.show();
    if (parser.isSet(smokeTest)) {
        QTimer::singleShot(0, &window, [&app, &window]() { app.exit(window.isVisible() ? 0 : 1); });
    }
    return app.exec();
}
