cmake_minimum_required(VERSION 3.23)
set(CMAKE_CXX_STANDARD 20)

project(PloxEngine)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)


add_subdirectory(PloxEngine)
add_subdirectory(Sandbox)

