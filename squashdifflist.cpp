#include "squashdifflist.h"

SquashDiffList::SquashDiffList() {}

void SquashDiffList::append(gitse2::SquashDiffPtr&& diff) {
    beginInsertRows(QModelIndex(), rowCount(), rowCount());
    m_list_of_diffs.push_back(std::move(diff));
    endInsertRows();
}

int SquashDiffList::rowCount(const QModelIndex &parent) const {
    return m_list_of_diffs.size();
}

QVariant SquashDiffList::data(const QModelIndex &index, int role) const {
    if (!index.isValid()) {
        return QVariant();
    }

    if (index.row() >= m_list_of_diffs.size()) {
        return QVariant();
    }

    if (role == Qt::DisplayRole || role == ModelDataRole) {
        return QVariant::fromValue(m_list_of_diffs[index.row()].get());
    }

    return QVariant();
}

QHash<int, QByteArray> SquashDiffList::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[ModelDataRole] = "modelData";
    return roles;
}
