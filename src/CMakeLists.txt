cmake_minimum_required(VERSION 2.6)

project("Atlas Components")

add_subdirectory(api)
add_subdirectory(db)
add_subdirectory(http)
add_subdirectory(jsonrpc)

file(GLOB atlas_common_sources log/*.cpp *.cpp)
add_library(atlas-com-common ${atlas_common_sources})

