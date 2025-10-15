cmake_minimum_required(VERSION 3.22)

project(Blockregion LANGUAGES CXX)

#######################################################
# General
#######################################################
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/../devops/cmake" ${CMAKE_MODULE_PATH})

include(FetchContent)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED YES)

#######################################################
# Load dependencies
#######################################################
find_package(GLEW REQUIRED)
find_package(glfw3 REQUIRED)
find_package(OpenGL REQUIRED)
find_package(SOIL2 REQUIRED)
find_package(glm REQUIRED)
find_package(Freetype REQUIRED)
find_package(nlohmann_json REQUIRED)

#######################################################
# Blockregion
#######################################################
set(EXE blockregion)

add_executable(${EXE}
    src/main.cpp
    src/game.cpp
    src/world.cpp
    src/entity.cpp
    src/player.cpp
    src/block.cpp
    src/chunk.cpp
    src/collision-detection.cpp
)

target_include_directories(${EXE} PUBLIC 
    ${OPENGL_INCLUDE_DIR}
    ${SOIL2_INCLUDE_DIR}
    ${GLM_INCLUDE_DIRS}
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}
)

target_link_libraries(${EXE} PRIVATE 
    GLEW::GLEW glfw ${OPENGL_gl_LIBRARY} OpenGL::GL Freetype::Freetype ${SOIL2_LIBRARY} glm::glm nlohmann_json::nlohmann_json
)

target_compile_definitions(${EXE} PUBLIC
    -DSHADERS_PATH="${CMAKE_SOURCE_DIR}/shaders/"
    -DTEXTURES_PATH="${CMAKE_SOURCE_DIR}/../textures/"
)

target_compile_features(${EXE} PRIVATE cxx_std_20)
