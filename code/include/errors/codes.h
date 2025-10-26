#pragma once

#include <string>
#include <ostream>
#include <cstdint>

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
