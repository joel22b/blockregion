include(FetchContent)

FetchContent_Declare(
    Freetype
    GIT_REPOSITORY https://github.com/freetype/freetype.git
    GIT_TAG VER-2-12-1
    GIT_SHALLOW TRUE
)

FetchContent_GetProperties(Freetype)
if(NOT freetype_POPULATED)
    FetchContent_Populate(Freetype)
    
    # Patch the CMakeLists.txt to update cmake_minimum_required version
    file(READ "${freetype_SOURCE_DIR}/CMakeLists.txt" FREETYPE_CMAKELISTS_CONTENT)
    string(REPLACE 
        "cmake_minimum_required(VERSION 3.0)" 
        "cmake_minimum_required(VERSION 3.5)" 
        FREETYPE_CMAKELISTS_CONTENT 
        "${FREETYPE_CMAKELISTS_CONTENT}"
    )
    file(WRITE "${freetype_SOURCE_DIR}/CMakeLists.txt" "${FREETYPE_CMAKELISTS_CONTENT}")
    
    # Manually add the directory with EXCLUDE_FROM_ALL
    add_subdirectory(${freetype_SOURCE_DIR} ${freetype_BINARY_DIR} EXCLUDE_FROM_ALL)
endif()

set(freetype_INCLUDE_DIR "${freetype_SOURCE_DIR}/include")
