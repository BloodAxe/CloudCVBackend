cmake_minimum_required(VERSION 2.8)
project(CloudCVBackend)

# Search for required software packages:
find_package(OpenCV REQUIRED)

# Tell CMake to put all executables to bin/ folder
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Add include directories to search path:
include_directories(${OpenCV_INCLUDE_DIRS})
include_directories(${OpenCV_INCLUDE_DIR})

# Add link directories:
link_directories(${OpenCV_LIBRARY_DIRS})

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
add_subdirectory(modules)