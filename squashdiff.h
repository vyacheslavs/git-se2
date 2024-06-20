#ifndef SQUASHDIFF_H
#define SQUASHDIFF_H

#include <memory>
#include <QtCore/qglobal.h>
#include <git2.h>
#include <QString>
#include <QObject>
#include "difflistitem.h"
#include "error-handling.h"
#include "difftreemodel.h"

namespace gitse2 {

    class SquashDiff : public QObject {
        Q_OBJECT

        public:

            Q_PROPERTY(QString title READ diff_title NOTIFY diff_title_changed)
            Q_PROPERTY(const DiffTreeModel* tree_model READ tree_model NOTIFY tree_model_changed)

            const QString& diff_title() const;
            const DiffTreeModel* tree_model() const;

        signals:
            void diff_title_changed(const QString& new_title);
            void tree_model_changed(const DiffTreeModel* new_model);

        private:
            SquashDiff() = default;

            DiffList m_diff_list;
            QString m_diff_title {"Untitled"};
            DiffTreeModelPtr m_tree_model;

            [[nodiscard]] Result<> initialize();

            friend class Repository;
    };

    using SquashDiffPtr = std::unique_ptr<SquashDiff>;
}

#endif // SQUASHDIFF_H
