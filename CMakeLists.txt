cmake_minimum_required(VERSION 3.22)

project(Blockregion LANGUAGES CXX)

#######################################################
# Version number
#######################################################
set(MAJOR "0")
set(MINOR "2")
set(REV   "0")
set(VERSION_STRING "${MAJOR}.${MINOR}.${REV}")
message(STATUS "Version: ${VERSION_STRING}")

#######################################################
# Find top level directory
#######################################################
find_package(Git REQUIRED)

execute_process(
    COMMAND ${GIT_EXECUTABLE} rev-parse --show-toplevel
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    OUTPUT_VARIABLE GIT_REPO_BASE
    OUTPUT_STRIP_TRAILING_WHITESPACE
)

message(STATUS "GIT_REPO_BASE: ${GIT_REPO_BASE}")

#######################################################
# General
#######################################################
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

set(CMAKE_MODULE_PATH "${GIT_REPO_BASE}/devops/cmake" ${CMAKE_MODULE_PATH})

# Set C++ 23 version
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED YES)

#######################################################
# Load dependencies
#######################################################
find_package(OpenGL REQUIRED)
find_package(Glew REQUIRED)
find_package(glfw3 REQUIRED)
find_package(SOIL2 REQUIRED)
find_package(glm REQUIRED)
find_package(PkgConfig REQUIRED)
find_package(Freetype REQUIRED)
find_package(nlohmann_json REQUIRED)
find_package(Spdlog REQUIRED)

#######################################################
# Add subdirectories
#######################################################
add_subdirectory(code)
