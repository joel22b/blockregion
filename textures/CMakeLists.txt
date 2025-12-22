#######################################################
# Installation Instructions
#######################################################
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/textures.json
        DESTINATION textures
)

#######################################################
# Textures
#######################################################
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/blocks_diffuse.png
        DESTINATION textures
)

install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/blocks_specular.png
        DESTINATION textures
)

#######################################################
# Fonts
#######################################################
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/arial.ttf
        DESTINATION textures
)

#######################################################
# Copy textures to build directory
#######################################################
add_custom_target(copy_textures ALL
        COMMAND ${CMAKE_COMMAND} -E copy_directory
                "${CMAKE_CURRENT_SOURCE_DIR}"
                "${CMAKE_CURRENT_BINARY_DIR}"
)
