#include "difftreemodel.h"
#include <git2.h>

gitse2::Result<DiffTreeModelPtr> DiffTreeModel::create_model(const DiffList &list) {

    DiffTreeModelPtr model (new DiffTreeModel());

    auto rval = DiffTreeItem::createTree(list);
    if (!rval)
        return unexpected_error(gitse2::ErrorCode::GitSquashDiffCreateError);
    model->m_root = std::move(rval.value());
    return model;
}

QModelIndex DiffTreeModel::index(int row, int column, const QModelIndex &parent) const {
    if (!hasIndex(row, column, parent))
        return {};

    DiffTreeItem *parentItem = parent.isValid()
                               ? static_cast<DiffTreeItem*>(parent.internalPointer())
                               : m_root.get();

    if (auto *childItem = parentItem->child(row))
        return createIndex(row, column, childItem);

    return {};
}

QModelIndex DiffTreeModel::parent(const QModelIndex &index) const {
    if (!index.isValid())
        return {};

    DiffTreeItem *childItem = static_cast<DiffTreeItem*>(index.internalPointer());
    DiffTreeItem *parentItem = childItem->parentItem();

    return parentItem != m_root.get()
               ? createIndex(parentItem->row(), 0, parentItem) : QModelIndex{};
}

int DiffTreeModel::rowCount(const QModelIndex &parent) const {
    if (parent.column() > 0)
        return 0;

    const DiffTreeItem *parentItem = parent.isValid()
                                     ? static_cast<const DiffTreeItem*>(parent.internalPointer())
                                     : m_root.get();
    return parentItem->childCount();
}

int DiffTreeModel::columnCount(const QModelIndex &parent) const {
    if (parent.isValid())
        return static_cast<DiffTreeItem*>(parent.internalPointer())->columnCount();
    return m_root->columnCount();
}

QVariant DiffTreeModel::data(const QModelIndex &index, int role) const {
    if (!index.isValid() || !(role == Qt::DisplayRole || role == DiffTypeRole))
        return {};

    const auto *item = static_cast<const DiffTreeItem*>(index.internalPointer());

    if (role == DiffTypeRole) {
        auto diff = item->diff();
        if (diff) {
            switch (diff->status()) {
                case GIT_DELTA_ADDED:
                    return "green";
                case GIT_DELTA_DELETED:
                    return "red";
                case GIT_DELTA_MODIFIED:
                    return "brown";
                default:
                    break;
            }
        }
    }

    return item->data(index .column());
}

QHash<int, QByteArray> DiffTreeModel::roleNames() const {
    QHash<int, QByteArray> roles;
    roles[Qt::DisplayRole] = "display";
    roles[DiffTypeRole] = "diff_type";
    return roles;
}
