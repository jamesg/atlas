cmake_minimum_required(VERSION 2.6)

project("Atlas Components")

file(GLOB atlas_http_sources *.cpp)
add_library(atlas-com-http ${atlas_http_sources})
target_link_libraries(
    atlas-com-http
    atlas-com-common
    ${CMAKE_THREAD_LIBS_INIT}
    ${Boost_LIBRARIES}
    ${CURL_LIBRARIES}
    )

# The HTTP server depends on GPLv2 code.
file(GLOB atlas_http_server_sources server/*.cpp)
add_library(atlas-com-http-server ${atlas_http_server_sources})
target_link_libraries(
    atlas-com-http-server
    atlas-com-common
    atlas-com-http
    mongoose
    styx
    ${CMAKE_THREAD_LIBS_INIT}
    ${Boost_LIBRARIES}
    )
file(GLOB atlas_http_server_test_sources server/test/*.cpp)
add_executable(
    atlas-http-server-test
    ${atlas_http_server_test_sources}
    )
target_link_libraries(
    atlas-http-server-test
    atlas-com-http-server
    )
add_test(atlas-http-server-test atlas-http-server-test)

