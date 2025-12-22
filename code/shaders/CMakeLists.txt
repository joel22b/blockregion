#######################################################
# Installation Instructions
#######################################################
install(DIRECTORY ${CMAKE_BINARY_DIR}/shaders/
        DESTINATION shaders
)

#######################################################
# Copy shaders to build directory
#######################################################
add_custom_target(copy_shaders_block ALL
        COMMAND ${CMAKE_COMMAND} -E copy_directory
                "${CMAKE_CURRENT_SOURCE_DIR}/block"
                "${CMAKE_BINARY_DIR}/shaders/block"
)

add_custom_target(copy_shaders_text ALL
        COMMAND ${CMAKE_COMMAND} -E copy_directory
                "${CMAKE_CURRENT_SOURCE_DIR}/text"
                "${CMAKE_BINARY_DIR}/shaders/text"
)

add_custom_target(copy_shaders ALL
        DEPENDS copy_shaders_block
                copy_shaders_text
)
