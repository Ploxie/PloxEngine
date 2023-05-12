cmake_minimum_required(VERSION 3.23)
set(CMAKE_CXX_STANDARD 20)

project(PloxEngine)

# Define folders
set(SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(VENDOR_DIR ${CMAKE_CURRENT_SOURCE_DIR}/vendors)
set(LIB_DIR ${VENDOR_DIR}/lib/x64/debug/)

# Source & Headers
file(GLOB_RECURSE SOURCES ${SRC_DIR}/*.cpp)
file(GLOB_RECURSE HEADERS ${SRC_DIR}/*.h)

# Includes
include_directories(${VENDOR_DIR}/include)

find_package(Vulkan REQUIRED)
message("Vulkan found: Version ${Vulkan_VERSION}")

# Define Libraries
set(LIBRARIES
        ${LIB_DIR}/EASTL.lib
        Vulkan::Vulkan)

# Binaries
add_library(${PROJECT_NAME} STATIC ${SOURCES})

# Linking
target_link_libraries(${PROJECT_NAME} LINK_PUBLIC ${LIBRARIES})
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)
target_include_directories(${PROJECT_NAME} PUBLIC ${VENDOR_DIR}/include)