cmake_minimum_required(VERSION 2.6)

project("Atlas Components")

add_subdirectory(api)
add_subdirectory(auth)
add_subdirectory(db)
add_subdirectory(http)
add_subdirectory(jsonrpc)
add_subdirectory(task)

file(GLOB atlas_common_sources log/*.cpp *.cpp)
add_library(atlas-com-common ${atlas_common_sources})
target_link_libraries(atlas-com-common ${Boost_LIBRARIES})
