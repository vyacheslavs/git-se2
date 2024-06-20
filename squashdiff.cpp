#include "squashdiff.h"

using namespace gitse2;


const QString& SquashDiff::diff_title() const {
    return m_diff_title;
}

const DiffTreeModel* SquashDiff::tree_model() const {
    return m_tree_model.get();
}

Result<> SquashDiff::initialize() {

    // sort the diff list
    std::sort(m_diff_list.begin(), m_diff_list.end(), [](const auto& lhs, const auto& rhs) {
        return std::count(lhs->path().begin(), lhs->path().end(), '/') > std::count(rhs->path().begin(), rhs->path().end(), '/');
    });

    auto rval = m_tree_model->create_model(m_diff_list);
    if (!rval)
        return unexpected_error(ErrorCode::GitSquashDiffCreateError);

    m_tree_model = std::move(rval.value());
    return {};
}
