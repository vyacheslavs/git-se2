#include "squashdiff.h"

using namespace gitse2;


const QString& SquashDiff::diff_title() const {
    return m_diff_title;
}
Result<> SquashDiff::initialize() {

    // sort the diff list
    std::sort(m_diff_list.begin(), m_diff_list.end(), [](const auto& lhs, const auto& rhs) {
        return std::count(lhs->path().begin(), lhs->path().end(), '/') > std::count(rhs->path().begin(), rhs->path().end(), '/');
    });


    m_tree_model = std::move(rval.value());
    return {};
}
