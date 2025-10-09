cmake_minimum_required(VERSION 3.22)

project(Blockregion)

#######################################################
# General
#######################################################
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

#######################################################
# Blockregion
#######################################################
set(EXE blockregion)

add_executable(${EXE} src/main.cpp)
