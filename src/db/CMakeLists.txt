cmake_minimum_required(VERSION 2.6)

project("Atlas Components")

file(GLOB atlas_db_sources *.cpp)
add_library(atlas-com-db ${atlas_db_sources})
target_link_libraries(
    atlas-com-db
    hades
    styx
    ${Boost_LIBRARIES}
    )

