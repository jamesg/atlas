file(GLOB atlas_auth_sources *.cpp)
add_library(atlas-auth ${atlas_auth_sources})
target_link_libraries(
    atlas-auth
    atlas-com-db
    atlas-com-http
    hades
    styx
    ${Boost_LIBRARIES}
    )

