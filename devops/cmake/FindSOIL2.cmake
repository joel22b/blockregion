FetchContent_Declare(
    SOIL2
    GIT_REPOSITORY git@github.com:SpartanJ/SOIL2.git
    GIT_TAG 1.31
)

FetchContent_MakeAvailable(SOIL2)

set(SOIL2_INCLUDE_DIR "${soil2_SOURCE_DIR}/src")
set(SOIL2_LIBRARY "/home/jbraun/dependencies/SOIL2/build/libsoil2.a")