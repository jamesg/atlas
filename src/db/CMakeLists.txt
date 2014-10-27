cmake_minimum_required(VERSION 2.6)

project("Atlas Components")
enable_testing()

file(GLOB atlas_db_sources *.cpp)
add_library(atlas-com-db ${atlas_db_sources})
target_link_libraries(
    atlas-com-db
    hades
    styx
    ${Boost_LIBRARIES}
    )

file(GLOB atlas_db_test_sources test/*.cpp)
add_executable(atlas-com-db-test ${atlas_db_test_sources})
add_test(atlas-com-db-test atlas-com-db-test)
target_link_libraries(
    atlas-com-db-test
    atlas-com-common
    atlas-com-db
    styx
    hades
    ${CMAKE_THREAD_LIBS_INIT}
    ${Boost_LIBRARIES}
    )

