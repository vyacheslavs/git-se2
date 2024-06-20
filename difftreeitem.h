#ifndef DIFFTREEITEM_H
#define DIFFTREEITEM_H

#include <vector>
#include <memory>
#include "difflistitem.h"
#include "error-handling.h"
#include <QVariant>

using DiffTreeItemPtr = std::unique_ptr<class DiffTreeItem>;
using DiffTreeItemList = std::vector<DiffTreeItemPtr>;

class DiffTreeItem
{
    public:
        [[nodiscard]] static gitse2::Result<DiffTreeItemPtr> createTree(const DiffList& list);
        [[nodiscard]] DiffTreeItem* child(int row) const;
        [[nodiscard]] int childCount() const;
        [[nodiscard]] DiffTreeItem* parentItem() const;
        [[nodiscard]] int row() const;
        [[nodiscard]] int columnCount() const;
        [[nodiscard]] QVariant data(int column) const;
        [[nodiscard]] DiffListItem* diff() const;

        [[nodiscard]] DiffTreeItem* add_child(const std::string& child_name);
    private:
        DiffTreeItem() = default;

    private:
        DiffTreeItemList m_children;
        DiffTreeItem* m_parentItem {nullptr};
        DiffListItem* m_diff_ptr {nullptr};
        int m_index {0};
        QVariant m_data {"root"};
};

#endif // DIFFTREEITEM_H
