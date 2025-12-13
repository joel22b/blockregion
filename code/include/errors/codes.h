#pragma once

#include <string>
#include <ostream>
#include <cstdint>
#include <format>

#include <spdlog/fmt/fmt.h>

#include <string_view>

namespace errors
{

enum Code
{
    Unknown = 0,
    InvalidArgument,
    InvalidOperation,
    InvalidState,
    Timeout,
    InitializationError,
    NotFound,
    AlreadyExists,
};

inline
std::string
codeToStr(const Code code)
{
    switch (code)
    {
        case Code::Unknown:
            return "Unknown";
        case Code::InvalidArgument:
            return "Invalid Argument";
        case Code::InvalidOperation:
            return "Invalid Operation";
        case Code::InvalidState:
            return "Invalid State";
        case Code::Timeout:
            return "Timeout";
        case Code::NotFound:
            return "Not Found";
        case Code::AlreadyExists:
            return "Already Exists";
        default:
            return "undefined=" + std::to_string(static_cast<uint32_t>(code));
    }
}

inline
std::ostream&
operator<<(std::ostream& os, const Code& code)
{
    switch (code)
    {
        case Code::Unknown:
            os << "Unknown";
            break;
        case Code::InvalidArgument:
            os << "Invalid Argument";
            break;
        case Code::InvalidOperation:
            os << "Invalid Operation";
            break;
        case Code::InvalidState:
            os << "Invalid State";
            break;
        case Code::Timeout:
            os << "Timeout";
            break;
        case Code::NotFound:
            os << "Not Found";
            break;
        case Code::AlreadyExists:
            os << "Already Exists";
            break;
        default:
            os << "undefined=" << static_cast<uint32_t>(code);
            break;
    }
    return os;
}

} // namespace errors

namespace fmt
{

template <>
struct formatter<errors::Code>
{
    constexpr auto parse(format_parse_context& ctx)
    { return ctx.begin(); }

    template <typename FormatContext>
    auto format(const errors::Code& code, FormatContext& ctx) const
    {
        return format_to(ctx.out(), "{}", errors::codeToStr(code));
    }
};

} // namespace fmt

// Define the std::formatter specialization within the std namespace
template <>
struct std::formatter<errors::Code> : std::formatter<std::string_view> {
    // We inherit from std::formatter<std::string_view> 
    // and use its parse/format members.

    // The format function simply calls our helper function
    auto format(errors::Code code, std::format_context& ctx) const {
        return std::formatter<std::string_view>::format(
            errors::codeToStr(code), ctx
        );
    }
};
