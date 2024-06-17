#ifndef REPOSITORY_H
#define REPOSITORY_H

#include <memory>
#include "error-handling.h"
#include <git2.h>
#include <QtCore/qglobal.h>
#include "git-etcetera.h"

namespace gitse2 {

    using RepositoryRef = std::shared_ptr<class Repository>;

    class Repository
    {
        public:
            Q_DISABLE_COPY_MOVE(Repository)

            static Result<RepositoryRef> open();
            ~Repository();
        private:
            Repository() = default;

            std::string m_repo_path {"."};
            git_repository_ptr m_repo;
    };
}

#endif // REPOSITORY_H
