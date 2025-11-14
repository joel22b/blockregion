include(FetchContent)

FetchContent_Declare(
    spdlog
    GIT_REPOSITORY git@github.com:gabime/spdlog.git
    GIT_TAG v1.16.0
    GIT_SHALLOW TRUE
)

FetchContent_MakeAvailable(spdlog)