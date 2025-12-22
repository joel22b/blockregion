include(FetchContent)

FetchContent_Declare(
    nlohmann_json
    GIT_REPOSITORY git@github.com:nlohmann/json.git
    GIT_TAG v3.12.0
    GIT_SHALLOW TRUE

)
set(JSON_BuildTests OFF CACHE BOOL "" FORCE)
set(JSON_Install OFF CACHE BOOL "" FORCE)

#FetchContent_MakeAvailable(nlohmann_json)

FetchContent_GetProperties(nlohmann_json)
if(NOT nlohmann_json_POPULATED)
    FetchContent_Populate(nlohmann_json)
    # Manually add the directory with EXCLUDE_FROM_ALL
    add_subdirectory(${nlohmann_json_SOURCE_DIR} ${nlohmann_json_BINARY_DIR} EXCLUDE_FROM_ALL)
endif()