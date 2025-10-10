cmake_minimum_required(VERSION 3.22)

project(Blockregion)

#######################################################
# General
#######################################################
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/../devops/cmake" ${CMAKE_MODULE_PATH})

include(FetchContent)

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
    src/texture-loader.cpp
)

target_include_directories(${EXE} PUBLIC 
    ${OPENGL_INCLUDE_DIR}
    ${SOIL2_INCLUDE_DIR}
    ${GLM_INCLUDE_DIRS}
    "${CMAKE_SOURCE_DIR}/shaders"
)

target_link_libraries(${EXE} PRIVATE 
    GLEW::GLEW glfw ${OPENGL_gl_LIBRARY} OpenGL::GL Freetype::Freetype ${SOIL2_LIBRARY} glm::glm nlohmann_json::nlohmann_json #${FREETYPE_LIBRARIES}
)
