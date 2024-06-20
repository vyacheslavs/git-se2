#include "difflistitem.h"
#include <memory.h>

void DiffListItem::add_delta(const git_diff_delta* delta) {
    m_original_delta = *delta;
    if (delta->new_file.path) m_path_new = delta->new_file.path;
    if (delta->old_file.path) m_path_old = delta->old_file.path;

    m_original_delta.new_file.path = m_path_new.c_str();
    m_original_delta.old_file.path = m_path_old.c_str();
}

void DiffListItem::add_binary_delta(const git_diff_binary *binary) {
    m_original_binary_delta = *binary;
    if (binary->new_file.datalen > 0) {
        m_binary_data_new.resize(binary->new_file.datalen);
        memcpy(m_binary_data_new.data(), binary->new_file.data, binary->new_file.datalen);
    }
    if (binary->old_file.datalen > 0) {
        m_binary_data_old.resize(binary->old_file.datalen);
        memcpy(m_binary_data_old.data(), binary->old_file.data, binary->old_file.datalen);
    }
    m_original_binary_delta.new_file.data = m_binary_data_new.data();
    m_original_binary_delta.old_file.data = m_binary_data_old.data();

}

const std::string &DiffListItem::path() const {
    return m_path_new;
}

int DiffListItem::status() const {
    return m_original_delta.status;
}
