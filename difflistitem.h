#ifndef DIFFLISTITEM_H
#define DIFFLISTITEM_H

#include <git2.h>
#include <string>
#include <vector>
#include <memory>

class DiffListItem
{
    public:
        DiffListItem() = default;

        void add_delta(const git_diff_delta* d);
        void add_binary_delta(const git_diff_binary* d);

        [[nodiscard]] const std::string& path() const;
        [[nodiscard]] int status() const;

    private:
        git_diff_delta m_original_delta;
        std::string m_path_new;
        std::string m_path_old;

        git_diff_binary m_original_binary_delta;
        std::vector<char> m_binary_data_new;
        std::vector<char> m_binary_data_old;
};

using DiffListItemPtr = std::shared_ptr<DiffListItem>;
using DiffList = std::vector<DiffListItemPtr>;

#endif // DIFFLISTITEM_H
