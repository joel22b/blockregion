include(FetchContent)

FetchContent_Declare(
    nlohmann_json
    GIT_REPOSITORY git@github.com:nlohmann/json.git
    GIT_TAG v3.12.0
    GIT_SHALLOW TRUE

)
set(JSON_BuildTests OFF CACHE BOOL "" FORCE)
set(JSON_Install OFF CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(nlohmann_json)