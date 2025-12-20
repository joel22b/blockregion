cmake_minimum_required(VERSION 3.22)

project(Blockregion LANGUAGES CXX)

#######################################################
# Version number
#######################################################
set(MAJOR "1")
set(MINOR "0")
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
set(CPACK_PACKAGE_DIRECTORY ${CMAKE_BINARY_DIR}/packages)

set(CMAKE_MODULE_PATH "${GIT_REPO_BASE}/devops/cmake" ${CMAKE_MODULE_PATH})

# Set C++ 23 version
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED YES)

set(CPACK_DEBIAN_PACKAGE_DEPENDS)

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
find_package(Asio REQUIRED)

#######################################################
# Add subdirectories
#######################################################
add_subdirectory(code)

#######################################################
# Installation Instructions
#######################################################
set(CPACK_PACKAGE_NAME "blockregion")
set(CPACK_PACKAGE_VERSION_MAJOR ${MAJOR})
set(CPACK_PACKAGE_VERSION_MINOR ${MINOR})
set(CPACK_PACKAGE_VERSION_PATCH ${REV})
set(CPACK_PACKAGE_VENDOR "joel22b")
set(CPACK_PACKAGE_CONTACT "https://github.com/joel22b")
set(CPACK_PACKAGE_DESCRIPTION "A minecraft like game made from scratch. Repo: https://github.com/joel22b/blockregion")
set(CPACK_PACKAGE_FILE_NAME "${CPACK_PACKAGE_NAME}-${VERSION_STRING}")

#set(CPACK_COMPONENTS_ALL BlockregionApp)
set(CPACK_TGZ_BLOCKREGIONAPP_FILE_NAME "${CPACK_PACKAGE_FILE_NAME}")
#set(CPACK_ARCHIVE_COMPONENT_INSTALL ON)
#set(CPACK_DEBIAN_PACKAGE_COMPONENT ON) 

if(WIN32)
    set(CPACK_GENERATOR "WIX;ZIP")
elseif(APPLE)
    set(CPACK_GENERATOR "DragNDrop;TGZ")
elseif(UNIX AND NOT APPLE)
    # RPM not supported yet
    set(CPACK_GENERATOR "DEB;TGZ")
endif()

include(CPack)
