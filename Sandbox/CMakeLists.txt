cmake_minimum_required(VERSION 3.23)
set(CMAKE_CXX_STANDARD 20)

project(Sandbox)

# Define folders
set(SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)

file(GLOB_RECURSE SOURCES ${SRC_DIR}/*.cpp)
file(GLOB_RECURSE HEADERS ${SRC_DIR}/*.h)

# Binaries
add_executable(${PROJECT_NAME} ${SOURCES})

target_link_libraries (${PROJECT_NAME} LINK_PUBLIC PloxEngine)