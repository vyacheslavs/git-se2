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

static std::string explain_repository_fail(const Error& e) {
    const git_error *last_error = giterr_last();

    std::stringstream ss;

    ss << explain_generic(e);
    ss << "libgit2 error = " << std::any_cast<int>(e.aux);
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

Result<> Repository::squash(const std::string& first_commit) {
    git_annotated_commit_ptr gac;
    git_commit_ptr head_commit;
    if (auto resolve_rval = resolve_commit("HEAD", gac, head_commit); !resolve_rval)
        return unexpected_nested(ErrorCode::GitGenericError, resolve_rval.error());

    auto rval = checkout_commit(first_commit);
    if (!rval)
        return unexpected_nested(ErrorCode::GitGenericError, rval.error());

    git_commit_ptr new_head(std::move(rval.value()));

    const auto new_branch_name = fmt::format("git-se/{}", first_commit);
    auto rval_branch = create_branch(new_branch_name, new_head);
    if (!rval_branch)
        return unexpected_nested(ErrorCode::GitGenericError, rval_branch.error());

    if (auto err = git_repository_set_head(m_repo.get(), fmt::format("refs/heads/git-se/{}", first_commit).c_str()); err != GIT_OK)
        return unexpected_explained(ErrorCode::GitRepositoryOpenError, explain_repository_fail, err);

    if (auto apply_rval = apply_diff(new_head, head_commit); !apply_rval)
        return unexpected_nested(ErrorCode::GitGenericError, rval_branch.error());

    git_oid tree_id, commit_id;
    git_index *index = nullptr;

    if (auto err = git_repository_index(&index, m_repo.get()); err != GIT_OK)
        return unexpected_explained(ErrorCode::GitRepositoryOpenError, explain_repository_fail, err);

    git_index_ptr index_ptr(index);

    if (auto err = git_index_add_all(index, NULL, GIT_INDEX_ADD_DEFAULT, NULL, NULL); err != GIT_OK)
        return unexpected_explained(ErrorCode::GitRepositoryOpenError, explain_repository_fail, err);

    if (auto err = git_index_write(index); err != GIT_OK)
        return unexpected_explained(ErrorCode::GitRepositoryOpenError, explain_repository_fail, err);

    if (auto err = git_index_write_tree(&tree_id, index); err != GIT_OK)
        return unexpected_explained(ErrorCode::GitRepositoryOpenError, explain_repository_fail, err);

    git_tree* tree = nullptr;
    if (auto err = git_tree_lookup(&tree, m_repo.get(), &tree_id); err != GIT_OK)
        return unexpected_explained(ErrorCode::GitRepositoryOpenError, explain_repository_fail, err);

    git_tree_ptr tree_ptr(tree);

    git_signature *sig = NULL;
    if (auto err = git_signature_now(&sig, "Git SE", "git-se@git-se.se"); err != GIT_OK)
        return unexpected_explained(ErrorCode::GitRepositoryOpenError, explain_repository_fail, err);

    git_signature_ptr sig_ptr(sig);

    const git_commit* commit2 = new_head.get();

    if (auto err = git_commit_create_v(
            &commit_id, m_repo.get(), "HEAD", sig, sig, NULL, "Git SE auto generated message", tree,
            1, commit2); err != GIT_OK)
        return unexpected_explained(ErrorCode::GitRepositoryOpenError, explain_repository_fail, err);

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

Result<> Repository::resolve_commit(const std::string &commit, git_annotated_commit_ptr &gac, git_commit_ptr &gc)
{
    git_repository_free(m_repo);
    auto rval = resolve_commit(commit);
    if (!rval)
        return unexpected_nested(ErrorCode::GitGenericError, rval.error());

    gac = std::move(rval.value());

    git_commit *target_commit = nullptr;
    int err = git_commit_lookup(&target_commit, m_repo.get(), git_annotated_commit_id(gac.get()));
    if (err != GIT_OK)
        return unexpected_explained(ErrorCode::GitGenericError, explain_repository_fail, err);

    gc.reset(target_commit);

    return {};
}

Result<git_commit_ptr> Repository::checkout_commit(const std::string &commit) {

    git_annotated_commit_ptr ac_target_commit;
    git_commit_ptr target_commit;
    git_checkout_options checkout_opts = GIT_CHECKOUT_OPTIONS_INIT;
    checkout_opts.checkout_strategy = GIT_CHECKOUT_SAFE;

    auto rval = resolve_commit(commit, ac_target_commit, target_commit);
    if (!rval)
        return unexpected_nested(ErrorCode::GitGenericError, rval.error());

    int err = git_checkout_tree(m_repo.get(), (const git_object *)target_commit.get(), &checkout_opts);
    if (err != GIT_OK)
        return unexpected_explained(ErrorCode::GitGenericError, explain_repository_fail, err);

    err = git_repository_set_head_detached_from_annotated(m_repo.get(), ac_target_commit.get());
    if (err != GIT_OK)
        return unexpected_explained(ErrorCode::GitGenericError, explain_repository_fail, err);

    return target_commit;
}

Result<> Repository::create_branch(const std::string &branch, const git_commit_ptr& commit) {
    // will be creating branch on HEAD

    git_reference* ref = nullptr; // free reference???
    int err = git_branch_create(&ref, m_repo.get(), branch.c_str(), commit.get(), 1);
    if (err != GIT_OK)
        return unexpected_explained(ErrorCode::GitGenericError, explain_repository_fail, err);

    git_reference_ptr ref_ptr(ref);
    return {};
}

Result<> Repository::apply_diff(const git_commit_ptr &commit1, const git_commit_ptr &commit2) {
    git_tree* tree1 = nullptr;
    git_tree* tree2 = nullptr;

    if (auto err = git_commit_tree(&tree1, commit1.get()); err != GIT_OK)
        return unexpected_explained(ErrorCode::GitGenericError, explain_repository_fail, err);

    git_tree_ptr tree1_ptr(tree1);

    if (auto err = git_commit_tree(&tree2, commit2.get()); err != GIT_OK)
        return unexpected_explained(ErrorCode::GitGenericError, explain_repository_fail, err);

    git_tree_ptr tree2_ptr(tree2);

    git_diff *diff = nullptr;
    if (auto err = git_diff_tree_to_tree(&diff, m_repo.get(), tree1, tree2, NULL); err != GIT_OK)
        return unexpected_explained(ErrorCode::GitGenericError, explain_repository_fail, err);

    git_diff_ptr diff_ptr(diff);

    if (auto err = git_apply(m_repo.get(), diff, GIT_APPLY_LOCATION_BOTH, nullptr); err != GIT_OK)
        return unexpected_explained(ErrorCode::GitGenericError, explain_repository_fail, err);

    // now commit changes

    return {};
}

