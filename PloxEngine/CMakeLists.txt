cmake_minimum_required(VERSION 3.23)
set(CMAKE_CXX_STANDARD 20)

project(PloxEngine)

# Define folders
set(SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)

# Source & Headers
file(GLOB_RECURSE SOURCES ${SRC_DIR}/*.cpp)
file(GLOB_RECURSE HEADERS ${SRC_DIR}/*.h)

# Binaries
add_library(${PROJECT_NAME} STATIC ${SOURCES})

target_include_directories (${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)