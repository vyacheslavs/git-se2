#include "error-handling.h"

#include <sstream>

std::string gitse2::explain_nested_error(const Error& e) {
    std::stringstream ss;
    unwind_nested(e, [&](const Error& ex) {
        if (is_nested_error(ex))
            ss << ex.fun_name << " (file://"<<ex.source<<":"<<ex.source_line<<")\n";
        else if (ex.cb)
            ss << ex.cb(ex);
        else
            ss << explain_generic(ex) << "\n";
    });
    return ss.str();
}

std::string gitse2::explain_generic(const Error& ex) {
    std::stringstream ss;
    ss << ex.fun_name << " (file://"<<ex.source<<":"<<ex.source_line<<") "<<code_to_string(ex.code);
    return ss.str();
}

std::string gitse2::explain_invalid_parameter(const Error& e) {
    std::stringstream ss;
    ss << explain_generic(e) << "invalid input parameter `"<<std::any_cast<std::string>(e)<<"`\n";
    return ss.str();
}

bool gitse2::is_nested_error(const Error& e) {
    return e.cb == explain_nested_error;
}

namespace gitse2 {
    template<>
    const Error& get_nested<0>(const Error& e) {
        return e;
    }
}

std::string gitse2::code_to_string(ErrorCode ec) {
    static const std::unordered_map<ErrorCode, std::string> error_map {
        {ErrorCode::None, "No error"}
    };

    auto it = error_map.find(ec);
    if (it != error_map.end())
        return it->second;

    return "";
}
std::string gitse2::explain_combined_error(const gitse2::Error& e) {
    std::stringstream ss;

    auto combined = std::any_cast<combined_error>(e.aux);
    ss << explain_nested_error(combined.first);
    ss << "\n at the same time:\n";
    ss << explain_nested_error(combined.second);

    return ss.str();
}
