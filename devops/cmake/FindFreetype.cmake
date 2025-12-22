include(FetchContent)

FetchContent_Declare(
    Freetype
    GIT_REPOSITORY https://github.com/freetype/freetype.git
    GIT_TAG VER-2-12-1
    GIT_SHALLOW TRUE
)

#FetchContent_MakeAvailable(Freetype)

FetchContent_GetProperties(Freetype)
if(NOT freetype_POPULATED)
    FetchContent_Populate(Freetype)
    # Manually add the directory with EXCLUDE_FROM_ALL
    add_subdirectory(${freetype_SOURCE_DIR} ${freetype_BINARY_DIR} EXCLUDE_FROM_ALL)
endif()

set(freetype_INCLUDE_DIR "${freetype_SOURCE_DIR}/include")
