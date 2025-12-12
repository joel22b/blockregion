#pragma once

#include <string>
#include <ostream>
#include <cstdint>

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

/*namespace fmt
{

template <>
struct formatter<errors::Code>
{
    /*template <typename ParseContext>
    constexpr auto parse(ParseContext &ctx)
    {
        return ctx.begin();
    }*//*

    template <typename FormatContext>
    auto format(const errors::Code &code, FormatContext &ctx) const
    {
        return format_to(ctx.out(), "{}", errors::codeToStr(code));
    }
};

} // namespace fmt*/

/*template <>
struct fmt::formatter<errors::Code> :
    fmt::formatter<std::string> {
  auto format(const errors::Code& code, format_context& ctx) const {
    return formatter<std::string>::format(errors::codeToStr(code), ctx);
  }
};*/

template <>
struct fmt::formatter<errors::Code> : fmt::formatter<std::string_view> {
    auto format(const errors::Code& code, fmt::format_context& ctx) const {
        std::string_view name;
        switch (code) {
            case errors::Code::Unknown: name = "Unknown"; break;
            case errors::Code::InvalidArgument: name = "Invalid Argument"; break;
            case errors::Code::InvalidOperation: name = "Invalid Operation"; break;
            case errors::Code::InvalidState: name = "Invalid State"; break;
            case errors::Code::Timeout: name = "Timeout"; break;
            case errors::Code::NotFound: name = "Not Found"; break;
            case errors::Code::AlreadyExists: name = "AlreadyExists"; break;
        }
        // Reuse the built-in string_view formatter
        return fmt::formatter<std::string_view>::format(name, ctx);
    }
};
