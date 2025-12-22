include(FetchContent)

FetchContent_Declare(
    GLEW
    GIT_REPOSITORY https://github.com/Perlmint/glew-cmake.git
    GIT_TAG glew-cmake-2.2.0
    GIT_SHALLOW TRUE
)

#FetchContent_MakeAvailable(GLEW)

FetchContent_GetProperties(GLEW)
if(NOT glew_POPULATED)
    FetchContent_Populate(GLEW)
    # Manually add the directory with EXCLUDE_FROM_ALL
    add_subdirectory(${glew_SOURCE_DIR} ${glew_BINARY_DIR} EXCLUDE_FROM_ALL)
endif()
