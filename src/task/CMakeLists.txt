file(GLOB atlas_task_sources *.cpp)
add_library(atlas-task ${atlas_task_sources})
target_link_libraries(
    atlas-task
    atlas-com-common
    ${CMAKE_THREAD_LIBS_INIT}
    ${Boost_LIBRARIES}
    )

