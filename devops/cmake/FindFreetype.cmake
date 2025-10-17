include(FetchContent)

FetchContent_Declare(
    Freetype
    GIT_REPOSITORY git@github.com:freetype/freetype.git
    GIT_TAG VER-2-12-1
    GIT_SHALLOW TRUE
)

FetchContent_MakeAvailable(Freetype)

set(freetype_INCLUDE_DIR "${freetype_SOURCE_DIR}/include")
