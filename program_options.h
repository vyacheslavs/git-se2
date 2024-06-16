#ifndef PROGRAM_OPTIONS_H
#define PROGRAM_OPTIONS_H

#include "error-handling.h"
#include <QCoreApplication>

namespace gitse2 {
    class ProgramOptions
    {
        public:
            static Result<ProgramOptions> parse_program_options(const QCoreApplication& app);
        private:
            ProgramOptions() = default;

            QString m_first_commit;
    };
}


#endif // PROGRAM_OPTIONS_H
