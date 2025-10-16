include(FetchContent)

FetchContent_Declare(
    GLEW
    GIT_REPOSITORY git@github.com:Perlmint/glew-cmake.git
    GIT_TAG glew-cmake-2.2.0
    GIT_SHALLOW TRUE
)

FetchContent_MakeAvailable(GLEW)
