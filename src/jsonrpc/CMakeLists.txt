cmake_minimum_required(VERSION 2.6)

project("Atlas Components")

file(GLOB atlas_jsonrpc_sources *.cpp)
add_library(atlas-com-jsonrpc ${atlas_jsonrpc_sources})
target_link_libraries(
    atlas-com-jsonrpc
    atlas-com-http-server
    )

