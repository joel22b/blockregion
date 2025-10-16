include(FetchContent)

FetchContent_Declare(
    glm
    GIT_REPOSITORY git@github.com:g-truc/glm.git
    GIT_TAG 1.0.1
)

FetchContent_MakeAvailable(glm)
