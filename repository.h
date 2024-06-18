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

            [[nodiscard]] static Result<RepositoryRef> open();
            [[nodiscard]] Result<> squash(const std::string& first_commit);

        private:
            Repository() = default;

            std::string m_repo_path {"."};
            git_repository_ptr m_repo;
            git_commit_ptr m_target_head;
            git_commit_ptr m_first_commit;

            [[nodiscard]] Result<git_annotated_commit_ptr> resolve_commit(const std::string& commit);
            [[nodiscard]] Result<> resolve_commit(const std::string& commit, git_annotated_commit_ptr& gac, git_commit_ptr& gc);
            [[nodiscard]] Result<git_commit_ptr> checkout_commit(const std::string& commit);
            [[nodiscard]] Result<> create_branch(const std::string& branch, const git_commit_ptr& commit);
            [[nodiscard]] Result<> apply_diff(const git_commit_ptr& commit1, const git_commit_ptr& commit2);
    };
}

#endif // REPOSITORY_H
