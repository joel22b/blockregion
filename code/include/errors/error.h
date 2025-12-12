#pragma once

#include <errors/codes.h>

#include <spdlog/fmt/fmt.h>

namespace errors
{

class Error
{
public:
    Error(const Error& err) : m_code(err.getCode()), m_message(err.getMessage()) {}
    Error(const std::string message, const Code code = Code::Unknown) : m_code(code), m_message(message) {}

    Code getCode() const { return m_code; }
    std::string getMessage() const { return m_message; }

    bool operator==(const Error& other)
    {
        return m_code == other.m_code; 
    }

    friend std::ostream&
    operator<<(std::ostream& os, const Error& err)
    {
        os << "[" << err.getCode() << "]: " << err.getMessage();
        return os;
    }

    Error prefix(const std::string message)
    {
        m_message = message + m_message;
        return *this;
    }

private:
    Code m_code {Code::Unknown};
    std::string m_message {""};
};

} // namespace errors

/*namespace fmt
{

template <>
struct formatter<errors::Error>
{
    /*template <typename ParseContext>
    constexpr auto parse(ParseContext &ctx)
    {
        return ctx.begin();
    }*//*

    template <typename FormatContext>
    auto format(const errors::Error &error, FormatContext &ctx) const
    {
        return format_to(ctx.out(), "[{}]: {}", error.getCode(), error.getMessage());
    }
};

} // namespace fmt*/

/*template <>
struct fmt::formatter<errors::Error> :
    fmt::formatter<std::string> {
  auto format(const errors::Error& error, format_context& ctx) const {
    //return formatter<std::string>::format(errors::codeToStr(code), ctx);
    return format_to(ctx.out(), "[{}]: {}"_cf, error.getCode(), error.getMessage());
  }
};*/

template <> struct fmt::formatter<errors::Error> {
  constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }

  template <typename FormatContext>
  auto format(const errors::Error& error, FormatContext& ctx) const {
    return format_to(ctx.out(), "({}, {})", error.getCode(), error.getMessage());
  }
};
