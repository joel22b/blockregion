#pragma once

/********************************
 * Linux
********************************/

#ifdef __linux__

#include <unistd.h>
#include <limits.h>
#include <filesystem>

namespace utils
{

inline
std::filesystem::path get_executable_dir()
{
    char result[PATH_MAX];
    ssize_t count = readlink("/proc/self/exe", result, PATH_MAX);
    if (count != -1) {
        return std::filesystem::path(std::string(result, count)).parent_path();
    }
    return "";
}

} // namespace utils

#endif

/********************************
 * Windows
********************************/

#ifdef _WIN32

#include <windows.h>
#include <iostream>
#include <filesystem>

namespace utils
{

inline
std::filesystem::path get_executable_dir()
{
    wchar_t path[MAX_PATH];
    GetModuleFileNameW(NULL, path, MAX_PATH);
    return std::filesystem::path(path).parent_path();
}

} // namespace utils

#endif

/********************************
 * MacOS
********************************/

#ifdef __APPLE__

#include <mach-o/dyld.h>
#include <limits.h>
#include <filesystem>

namespace utils
{

std::filesystem::path get_executable_dir()
{
    char path[PATH_MAX];
    uint32_t size = sizeof(path);
    if (_NSGetExecutablePath(path, &size) == 0) {
        return std::filesystem::path(path).parent_path();
    }
    return "";
}

} // namespace utils

#endif
