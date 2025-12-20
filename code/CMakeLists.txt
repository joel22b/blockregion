#######################################################
# Get all source files
#######################################################
file(GLOB_RECURSE BLOCKREGION_SOURCE_FILES "*.cpp")

#######################################################
# Blockregion
#######################################################
set(EXE blockregion)

add_executable(${EXE}
    ${BLOCKREGION_SOURCE_FILES}
)

target_include_directories(${EXE} PRIVATE 
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
    asio
)

target_compile_definitions(${EXE} PRIVATE
    -DVERSION_STRING="${VERSION_STRING}"
    -DSHADERS_PATH="../code/shaders/"
    -DTEXTURES_PATH="../textures/"
)

#######################################################
# Installation Instructions
#######################################################
install(TARGETS ${EXE}
        DESTINATION bin
        #COMPONENT BlockregionApp
)

#install(FILES shaders/block.frag
#        DESTINATION shaders
#)
