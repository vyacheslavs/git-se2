#ifndef SQUASHDIFF_H
#define SQUASHDIFF_H

#include <memory>
#include <QtCore/qglobal.h>
#include <git2.h>
#include <string>
#include <vector>

namespace gitse2 {

    class SquashDiff {
        public:

        struct diff_list_item {
            git_diff_delta original_delta;
            std::string m_path_new;
            std::string m_path_old;

            git_diff_binary original_binary_delta;
            std::vector<char> binary_data_new;
            std::vector<char> binary_data_old;
        };

        using diff_list = std::vector<diff_list_item>;

        private:
            SquashDiff() = default;

            diff_list m_diff_list;

            friend class Repository;
    };

    using SquashDiffPtr = std::unique_ptr<SquashDiff>;
}

#endif // SQUASHDIFF_H
