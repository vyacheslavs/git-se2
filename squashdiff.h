#ifndef SQUASHDIFF_H
#define SQUASHDIFF_H

#include <memory>
#include <QtCore/qglobal.h>
#include <git2.h>
#include <string>
#include <vector>
#include <QString>
#include <QObject>

namespace gitse2 {

    class SquashDiff : public QObject {
        Q_OBJECT

        public:

        struct diff_list_item {
            git_diff_delta original_delta;
            std::string m_path_new;
            std::string m_path_old;
            Q_PROPERTY(QString title READ diff_title NOTIFY diff_title_changed)

            git_diff_binary original_binary_delta;
            std::vector<char> binary_data_new;
            std::vector<char> binary_data_old;
        };
            const QString& diff_title() const;

        using diff_list = std::vector<diff_list_item>;
        signals:
            void diff_title_changed(const QString& new_title);

        private:
            SquashDiff() = default;

            diff_list m_diff_list;
            QString m_diff_title {"Untitled"};

            friend class Repository;
    };

    using SquashDiffPtr = std::unique_ptr<SquashDiff>;
}

#endif // SQUASHDIFF_H
