#ifndef ERRORHANDLING_H
#define ERRORHANDLING_H

#include <any>
#include <string>
#include <tl/expected.hpp>

namespace gitse2 {
    enum class ErrorCode {
        None,
        FirstCommitOmittedError,
        GitRepositoryOpenError,
        GitGenericError,
        GitSquashDiffCreateError,
    };

    using explain_aux_callback = std::string(*)(const struct Error&);

    struct Error {
        ErrorCode code {ErrorCode::None};
        explain_aux_callback cb {nullptr};
        std::string fun_name;
        std::string source;
        int source_line {0};
        std::any aux;
    };

    template<typename T = void>
    using Result = tl::expected<T, Error>;

    using combined_error = std::pair<Error,Error>;

    std::string explain_nested_error(const Error& e);
    std::string explain_generic(const Error& e);
    std::string code_to_string(ErrorCode ec);
    std::string explain_invalid_parameter(const Error& e);
    std::string explain_combined_error(const Error& e);

    template <typename ME>
    [[maybe_unused]] Error make_combined_error_(const Error& e, const ME& might_be_e, ErrorCode c, const char* func, const char* source, int line) {
        if (!might_be_e)
            return gitse2::Error{c,explain_combined_error,func,source,line,combined_error(e, might_be_e.error())};
        return e;
    }

    template<typename F>
    void unwind_nested(const Error& e, F && callback) {
        const Error* e_it = &e;
        while (e_it->cb == explain_nested_error) {
            callback(*e_it);
            e_it = &std::any_cast<const Error&>(e_it->aux);
        }
        callback(*e_it);
    }

    bool is_nested_error(const Error& e);

    template <int N>
    const Error& get_nested(const Error& e) {
        return std::any_cast<const Error&>(get_nested<N-1>(e).aux);
    }
    template <>
    const Error& get_nested<0>(const Error& e);
}

#define unexpected_error(code) tl::unexpected<gitse2::Error>({code, gitse2::explain_generic, __PRETTY_FUNCTION__, __FILE__, __LINE__})
#define unexpected_explained(code, foo, bar) tl::unexpected<gitse2::Error>({code, foo, __PRETTY_FUNCTION__, __FILE__, __LINE__, bar})
#define unexpected_sqlite_exception(code, bar) tl::unexpected<gitse2::Error>({code, gitse2::explain_sqlite_error, __PRETTY_FUNCTION__, __FILE__, __LINE__, bar})
#define unexpected_nested(code, nested) tl::unexpected<gitse2::Error>({code, gitse2::explain_nested_error, __PRETTY_FUNCTION__, __FILE__, __LINE__, nested});
#define unexpected_invalid_input_parameter(code, param_name) tl::unexpected<gitse2::Error>({code, gitse2::explain_invalid_parameter, __PRETTY_FUNCTION__, __FILE__, __LINE__, std::string(param_name)})
#define unexpected_combined_error(code, nested1, nested2) tl::unexpected<gitse2::Error>({code, gitse2::explain_combined_error, __PRETTY_FUNCTION__, __FILE__, __LINE__, make_combined_error(nested1, nested2, code)});
#define make_combined_error(e, might_e, c) gitse2::make_combined_error_(e, might_e, c, __PRETTY_FUNCTION__, __FILE__, __LINE__)
#define make_nested_error(c, nested) gitse2::Error{c, gitse2::explain_nested_error, __PRETTY_FUNCTION__, __FILE__, __LINE__, nested}
#define make_sqlite_error(c, code) gitse2::Error{c, gitse2::explain_sqlite_error, __PRETTY_FUNCTION__, __FILE__, __LINE__, code}


class error_handling
{
public:
    error_handling();
};

#endif // ERRORHANDLING_H
