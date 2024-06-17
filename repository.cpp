#include "repository.h"
#include "qdebug.h"
#include <qglobal.h>
#include <sstream>
#include <fmt/format.h>

using namespace gitse2;

class LibGit2Initializer {
public:

    void init() {
        if (!m_initialized) { // TODO: make atomic
            git_libgit2_init();
            m_initialized = true;
        }
    }

    ~LibGit2Initializer() {
        if (m_initialized) {
            git_libgit2_shutdown();
        }
    }

private:
    bool m_initialized {false};
};

Q_GLOBAL_STATIC(LibGit2Initializer, _initializer)

static std::string explain_repository_open_fail(const Error& e) {
    const git_error *last_error = giterr_last();

    std::stringstream ss;

    ss << explain_generic(e);
    ss << "opening repository failed, error = " << std::any_cast<int>(e.aux);
    if (last_error) {
        ss << ", last_error: "<<last_error->message<<"\n";
    }

    return ss.str();
}

Result<RepositoryRef> Repository::open()
{
    _initializer->init();
    RepositoryRef out_ref(new Repository());

    git_repository* repo = nullptr;
    int error = git_repository_open_ext(&repo, out_ref->m_repo_path.c_str(), 0, NULL);
    if (error != 0 || !repo)
        return unexpected_explained(ErrorCode::GitRepositoryOpenError, explain_repository_open_fail, error);

    out_ref->m_repo.reset(repo);

    return out_ref;
}

template <> class fmt::formatter<git_commit_ptr> {
public:
    constexpr auto parse (format_parse_context& ctx) { return ctx.begin(); }
    template <typename Context>
    constexpr auto format (const git_commit_ptr& commit, Context& ctx) const {
        char oid_str[GIT_OID_HEXSZ + 1] = {};
        auto oid = git_commit_id(commit.get());
        git_oid_tostr(oid_str, sizeof(oid_str), oid);
        return format_to(ctx.out(), "{}", oid_str);
    }
};

    return {};
}

Repository::~Repository()
Result<git_annotated_commit_ptr> Repository::resolve_commit(const std::string &commit) {
    int err = 0;
    git_reference *ref = NULL;
    git_annotated_commit* target = nullptr;

    err = git_reference_dwim(&ref, m_repo.get(), commit.c_str());
    if (err == GIT_OK) {
        git_annotated_commit_from_ref(&target, m_repo.get(), ref);
        git_reference_free(ref);
        return git_annotated_commit_ptr(target);
    }

    git_object *obj;
    err = git_revparse_single(&obj, m_repo.get(), commit.c_str());
    if (err == GIT_OK) {
        err = git_annotated_commit_lookup(&target, m_repo.get(), git_object_id(obj));
        git_object_free(obj);
    }

    if (err != GIT_OK)
        return unexpected_explained(ErrorCode::GitGenericError, explain_repository_fail, err);

    return git_annotated_commit_ptr(target);
}
{
    git_repository_free(m_repo);
}
