cmake_minimum_required(VERSION 3.2)

project(VkVoxels)

# Set options
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Choose the build type ( Debug / Release )" FORCE)
endif()

set(CMAKE_INSTALL_PREFIX ${CMAKE_SOURCE_DIR})

# Add a place to look for CMake modules in
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/Modules;${CMAKE_MODULE_PATH}")

# Add source files
set(SRC_DIR "src")
file(GLOB_RECURSE SOURCES LIST_DIRECTORIES false ${SRC_DIR}/*)

include_directories(${SRC_DIR})

find_package(GLFW REQUIRED)
include_directories(${GLFW_INCLUDE_DIRS})
link_libraries(${GLFW_LIBRARIES})

link_libraries(vulkan)

# Enable C++14
if(CMAKE_COMPILER_IS_GNUCXX)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -Wall -Wextra -pedantic -mfpmath=sse -msse3 -pthread")
endif()

add_executable(${CMAKE_PROJECT_NAME} ${SOURCES})
