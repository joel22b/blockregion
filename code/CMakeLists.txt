#######################################################
# Blockregion
#######################################################
set(EXE blockregion)

add_executable(${EXE}
    src/main.cpp
    src/game.cpp
    include/world/world.cpp
    src/entity.cpp
    src/player.cpp
    src/collision-detection.cpp
)

target_include_directories(${EXE} PUBLIC 
    ${OPENGL_INCLUDE_DIR}
    ${SOIL2_INCLUDE_DIR}
    ${GLM_INCLUDE_DIRS}
    ${GIT_REPO_BASE}/code/include
    ${CMAKE_SOURCE_DIR}
    ${freetype_INCLUDE_DIR}
)

target_link_libraries(${EXE} PRIVATE 
    ${OPENGL_gl_LIBRARY}
    libglew_static
    glfw
    OpenGL::GL
    freetype
    soil2
    glm::glm
    nlohmann_json::nlohmann_json
    spdlog::spdlog
)

target_compile_definitions(${EXE} PUBLIC
    -DVERSION_STRING="${VERSION_STRING}"
    -DSHADERS_PATH="${GIT_REPO_BASE}/code/shaders/"
    -DTEXTURES_PATH="${GIT_REPO_BASE}/textures/"
)
