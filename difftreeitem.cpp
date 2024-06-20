#include "difftreeitem.h"
#include <filesystem>


gitse2::Result<DiffTreeItemPtr> DiffTreeItem::createTree(const DiffList &list) {
    DiffTreeItemPtr root(new DiffTreeItem);

    for (const auto& item : list) {
        qDebug().noquote()<<"========== SPLIT ========="<<item->path().c_str() << "  status: "<<item->status();
        auto it = root.get();
        for (const auto& p : std::filesystem::path(item->path())) {
            qDebug().noquote() << p.c_str();
            it = it->add_child(p.c_str());
        }
        it->m_diff_ptr = item.get();
    }
    return root;
}

DiffTreeItem *DiffTreeItem::child(int row) const {
    return row >= 0 && row < childCount() ? m_children.at(row).get() : nullptr;
}

int DiffTreeItem::childCount() const {
    return m_children.size();
}

DiffTreeItem *DiffTreeItem::parentItem() const {
    return m_parentItem;
}

int DiffTreeItem::row() const {
    return m_index;
}

int DiffTreeItem::columnCount() const {
    return 1;
}

QVariant DiffTreeItem::data(int column) const {
    return m_data;
}

DiffListItem *DiffTreeItem::diff() const {
    return m_diff_ptr;
}

DiffTreeItem* DiffTreeItem::add_child(const std::string &child_name) {
    // first, look for the child in the list
    for (const auto& child : m_children) {
        if (child->data(0).toString().toStdString() == child_name) {
            // this child is already existed
            return child.get();
        }
    }

    // there's no such child, create a new one
    DiffTreeItemPtr new_child(new DiffTreeItem);
    new_child->m_data = QString::fromStdString(child_name);
    new_child->m_parentItem = this;
    new_child->m_index = m_children.size();
    m_children.push_back(std::move(new_child));

    return m_children.back().get();
}
