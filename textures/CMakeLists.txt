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
