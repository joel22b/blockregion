include(FetchContent)

FetchContent_Declare(
    asio
    GIT_REPOSITORY https://github.com/chriskohlhoff/asio.git
    GIT_TAG asio-1-36-0
    GIT_SHALLOW TRUE
)

# Since Asio's main repo doesn't have a root CMakeLists.txt for FetchContent_MakeAvailable,
# we populate it and manually create an interface target.
FetchContent_GetProperties(asio)
if(NOT asio_POPULATED)
  FetchContent_Populate(asio)
  
  # Create an INTERFACE library for the header-only Asio
  add_library(asio INTERFACE)
  
  # Define ASIO_STANDALONE to use it without Boost
  target_compile_definitions(asio INTERFACE ASIO_STANDALONE)
  
  # Set the include path (the headers are in the 'asio/include' subdirectory)
  target_include_directories(asio INTERFACE ${asio_SOURCE_DIR}/asio/include)
  
  # Asio requires system threading libraries (e.g., pthreads on Linux)
  find_package(Threads REQUIRED)
  target_link_libraries(asio INTERFACE Threads::Threads)
endif()