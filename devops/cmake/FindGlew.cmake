include(FetchContent)

FetchContent_Declare(
    GLEW
    GIT_REPOSITORY git@github.com:Perlmint/glew-cmake.git
    GIT_TAG glew-cmake-2.2.0
    GIT_SHALLOW TRUE
)

FetchContent_GetProperties(GLEW)
if(NOT glew_POPULATED)
    FetchContent_Populate(GLEW)
    
    # Patch the CMakeLists.txt to update cmake_minimum_required version
    file(READ "${glew_SOURCE_DIR}/CMakeLists.txt" GLEW_CMAKELISTS_CONTENT)
    string(REPLACE 
        "cmake_minimum_required(VERSION 2.8.12)" 
        "cmake_minimum_required(VERSION 3.5)" 
        GLEW_CMAKELISTS_CONTENT 
        "${GLEW_CMAKELISTS_CONTENT}"
    )
    file(WRITE "${glew_SOURCE_DIR}/CMakeLists.txt" "${GLEW_CMAKELISTS_CONTENT}")
    
    add_subdirectory(${glew_SOURCE_DIR} ${glew_BINARY_DIR})
endif()
