#ifndef SQUASHDIFFLIST_H
#define SQUASHDIFFLIST_H

#include <QAbstractListModel>
#include <squashdiff.h>

class SquashDiffList : public QAbstractListModel {
        Q_OBJECT
    public:
        SquashDiffList();

        void append(gitse2::SquashDiffPtr&& diff);

        Q_INVOKABLE virtual int rowCount(const QModelIndex &parent = QModelIndex()) const override;
        Q_INVOKABLE virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
        Q_INVOKABLE virtual QHash<int, QByteArray> roleNames() const override;

    private:
        std::vector<gitse2::SquashDiffPtr> m_list_of_diffs;

        enum Roles{
            ModelDataRole = Qt::UserRole+1
        };
};

#endif // SQUASHDIFFLIST_H
