#pragma once

#include <git2.h>
#include <memory>

struct git_annotated_commit_deleter {
    void operator()(git_annotated_commit* p) {
        git_annotated_commit_free(p);
    }
};

struct git_commit_deleter {
    void operator()(git_commit* p) {
        git_commit_free(p);
    }
};

struct git_tree_deleter {
    void operator()(git_tree* p) {
        git_tree_free(p);
    }
};

struct git_diff_deleter {
    void operator()(git_diff* p) {
        git_diff_free(p);
    }
};

struct git_index_deleter {
    void operator()(git_index* p) {
        git_index_free(p);
    }
};

struct git_signature_deleter {
    void operator()(git_signature* p) {
        git_signature_free(p);
    }
};

struct git_reference_deleter {
    void operator()(git_reference* p) {
        git_reference_free(p);
    }
};

struct git_repository_deleter {
    void operator()(git_repository* p) {
        git_repository_free(p);
    }
};

using git_annotated_commit_ptr = std::unique_ptr<git_annotated_commit, git_annotated_commit_deleter>;
using git_commit_ptr = std::unique_ptr<git_commit, git_commit_deleter>;
using git_tree_ptr = std::unique_ptr<git_tree, git_tree_deleter>;
using git_diff_ptr = std::unique_ptr<git_diff, git_diff_deleter>;
using git_index_ptr = std::unique_ptr<git_index, git_index_deleter>;
using git_signature_ptr = std::unique_ptr<git_signature, git_signature_deleter>;
using git_reference_ptr = std::unique_ptr<git_reference, git_reference_deleter>;
using git_repository_ptr = std::unique_ptr<git_repository, git_repository_deleter>;
