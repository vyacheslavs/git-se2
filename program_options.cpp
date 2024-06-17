#include "program_options.h"
#include <QCommandLineParser>

static std::string explain_command_options_error(const gitse2::Error& e) {
    auto gen = gitse2::explain_generic(e) + "No first_commit argument provided.\n\n" + std::any_cast<const QString&>(e.aux).toStdString();

    return gen;
}

gitse2::Result<gitse2::ProgramOptions> gitse2::ProgramOptions::parse_program_options(const QCoreApplication& app)
{
    ProgramOptions opts;

    QCommandLineParser parser;
    parser.setApplicationDescription("Git split-explain utility");
    parser.addHelpOption();
    parser.addVersionOption();
    parser.addPositionalArgument("first_commit", QCoreApplication::translate("main", "Start git-se process from this first commit"));

    parser.process(app);

    auto posArgs = parser.positionalArguments();
    if (posArgs.empty())
        return unexpected_explained(ErrorCode::FirstCommitOmittedError, explain_command_options_error, parser.helpText());

    opts.m_first_commit = posArgs.at(0).toStdString();
    return opts;
}

const std::string &gitse2::ProgramOptions::first_commit() {
    return m_first_commit;
}
