cmake_minimum_required(VERSION 2.6)

project("Atlas Components")

file(GLOB atlas_api_sources *.cpp)
add_library(atlas-com-api ${atlas_api_sources})
target_link_libraries(
    atlas-com-api
    atlas-com-api-client
    atlas-com-http-server
    hades
    styx
    ${CMAKE_THREAD_LIBS_INIT}
    ${Boost_LIBRARIES}
    ${OPENSSL_LIBRARIES}
    )

file(GLOB atlas_api_client_sources client/*.cpp)
add_library(atlas-com-api-client ${atlas_api_client_sources})
target_link_libraries(
    atlas-com-api-client
    atlas-com-http
    )

