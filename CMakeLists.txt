cmake_minimum_required(VERSION 3.10)
project(minigit)

set(CMAKE_CXX_STANDARD 17)

include_directories(include)

file(GLOB_RECURSE SOURCES "src/*.cpp")
file(GLOB_RECURSE HEADERS "include/*.h")

add_executable(minigit ${SOURCES} ${HEADERS})

source_group(TREE ${CMAKE_SOURCE_DIR}/include PREFIX "Header Files" FILES ${HEADERS})
source_group(TREE ${CMAKE_SOURCE_DIR}/src PREFIX "Source Files" FILES ${SOURCES})