#ifndef DIFFTREEMODEL_H
#define DIFFTREEMODEL_H

#include <QAbstractItemModel>
#include "difftreeitem.h"
#include "error-handling.h"

using DiffTreeModelPtr = std::unique_ptr<class DiffTreeModel>;

class DiffTreeModel : public QAbstractItemModel {
    Q_OBJECT
    public:
        Q_DISABLE_COPY_MOVE(DiffTreeModel)

        [[nodiscard]] static gitse2::Result<DiffTreeModelPtr> create_model(const DiffList& list);
        Q_INVOKABLE virtual QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const override;
        [[nodiscard]] QModelIndex parent(const QModelIndex &index) const override;
        [[nodiscard]] int rowCount(const QModelIndex &parent = {}) const override;
        [[nodiscard]] int columnCount(const QModelIndex &parent = {}) const override;
        [[nodiscard]] QVariant data(const QModelIndex &index, int role) const override;
        [[nodiscard]] virtual QHash<int,QByteArray> roleNames() const override;
    private:
        DiffTreeModel() = default;
        DiffTreeItemPtr m_root;

        enum Roles{
            DiffTypeRole = Qt::UserRole+1
        };

};

#endif // DIFFTREEMODEL_H
