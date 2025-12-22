include(FetchContent)

FetchContent_Declare(
    glm
    GIT_REPOSITORY git@github.com:g-truc/glm.git
    GIT_TAG 1.0.1
    GIT_SHALLOW TRUE
)

#FetchContent_MakeAvailable(glm)

FetchContent_GetProperties(glm)
if(NOT glm_POPULATED)
    FetchContent_Populate(glm)
    # Manually add the directory with EXCLUDE_FROM_ALL
    add_subdirectory(${glm_SOURCE_DIR} ${glm_BINARY_DIR} EXCLUDE_FROM_ALL)
endif()
