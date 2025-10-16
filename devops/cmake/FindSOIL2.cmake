include(FetchContent)

FetchContent_Declare(
    SOIL2-pkg
    GIT_REPOSITORY git@github.com:SpartanJ/SOIL2.git
    GIT_TAG 1.31
    GIT_SHALLOW TRUE
)

FetchContent_MakeAvailable(SOIL2-pkg)

set(SOIL2_INCLUDE_DIR "${soil2_SOURCE_DIR}/src")

#add_subdirectory(${soil2_SOURCE_DIR} soil2-dir)

#FetchContent_Populate(SOIL2)
#if (NOT soil2_NOT_POPULATED)
#    execute_process(
#        COMMAND cmake ${soil2_SOURCE_DIR}
#        WORKING_DIRECTORY ${soil2_SOURCE_DIR}
#    )

#    execute_process(
#        COMMAND make -j
#        WORKING_DIRECTORY ${soil2_SOURCE_DIR}
#    )

#    set(SOIL2_INCLUDE_DIR "${soil2_SOURCE_DIR}/src")
#    set(SOIL2_LIBRARY "${soil2_SOURCE_DIR}/libsoil2.a")

#    message(STATUS "Installed SOIL2 1.31")
#endif()
