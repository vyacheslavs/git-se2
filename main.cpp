#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include "program_options.h"

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    auto options = gitse2::ProgramOptions::parse_program_options(app);
    if (!options) {
        qCritical().noquote() << gitse2::explain_nested_error(options.error());
        return 1;
    }

    QQmlApplicationEngine engine;
    const QUrl url(QStringLiteral("qrc:/git-se2/Main.qml"));
    QObject::connect(
        &engine,
        &QQmlApplicationEngine::objectCreationFailed,
        &app,
        []() { QCoreApplication::exit(-1); },
        Qt::QueuedConnection);
    engine.load(url);

    return app.exec();
}
