#pragma once

#include <string>
#include <ostream>

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
};

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
            os << "InvalidArgument";
            break;
        case Code::InvalidOperation:
            os << "InvalidOperation";
            break;
        case Code::InvalidState:
            os << "InvalidState";
            break;
        case Code::Timeout:
            os << "Timeout";
            break;
        default:
            os << "undefined";
            break;
    }
    return os;
}

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
