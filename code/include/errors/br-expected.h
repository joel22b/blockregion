#pragma once

#include <expected>

#include "errors/error.h"

/**********************************************************
 * This is a wrapper of std::expected and std::unexpected
 * to always use the custom Error type defined in
 * "error.h".
 * 
 * The purpose of this is to make handling errors simplier
 * and more standardized.
 *********************************************************/

namespace errors
{

template<typename SuccessType = void>
using expected = std::expected<SuccessType, Error>;

// Proxy object for implicit conversion
struct unexpected_maker
{
    const Error err;

    template<typename SuccessType>
    operator expected<SuccessType>() const {
        return std::unexpected(err);
    }
};

inline
unexpected_maker
unexpected(const std::string& message, const Code code = Code::Unknown)
{
    return unexpected_maker{Error{message, code}};
}

inline
unexpected_maker
unexpected(const Error& err)
{
    return unexpected_maker{Error{err}};
}

template<typename SuccessType>
inline
bool
has_error(const expected<SuccessType>& result)
{
    return !result.has_value();
}

} // namespace errors
