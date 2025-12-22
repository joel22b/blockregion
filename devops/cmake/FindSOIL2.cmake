include(FetchContent)

FetchContent_Declare(
    SOIL2
    GIT_REPOSITORY git@github.com:SpartanJ/SOIL2.git
    GIT_TAG 1.31
    GIT_SHALLOW TRUE
)

#FetchContent_MakeAvailable(SOIL2)

FetchContent_GetProperties(SOIL2)
if(NOT soil2_POPULATED)
    FetchContent_Populate(SOIL2)
    # Manually add the directory with EXCLUDE_FROM_ALL
    add_subdirectory(${soil2_SOURCE_DIR} ${soil2_BINARY_DIR} EXCLUDE_FROM_ALL)
endif()

set(SOIL2_INCLUDE_DIR "${soil2_SOURCE_DIR}/src")
