#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include "program_options.h"
#include "repository.h"
#include "squashdifflist.h"
#include <QQmlContext>

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    auto options = gitse2::ProgramOptions::parse_program_options(app);
    if (!options) {
        qCritical().noquote() << gitse2::explain_nested_error(options.error());
        return 1;
    }

    auto repo = gitse2::Repository::open();
    if (!repo) {
        qCritical().noquote() << gitse2::explain_nested_error(repo.error());
        return 2;
    }

    auto rval = repo.value()->squash(options.value().first_commit());
    if (!rval) {
        qCritical().noquote() << gitse2::explain_nested_error(rval.error());
        return 3;
    }

    SquashDiffList sdl;

    auto squash_diff = repo.value()->create_squash_diff();
    if (!squash_diff) {
        qCritical().noquote() << gitse2::explain_nested_error(squash_diff.error());
        return 4;
    }

    sdl.append(std::move(squash_diff.value()));

    QQmlApplicationEngine engine;

    auto *context = engine.rootContext();
    context->setContextProperty("repeater_model", &sdl);

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
