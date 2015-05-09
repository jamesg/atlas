cmake_minimum_required(VERSION 2.6)

project("Atlas Components")

include("cmake/atlas_add_objcopy_target.cmake")
include("cmake/atlas_add_web_library.cmake")

include_directories("lib/catch/include")

add_subdirectory(web)

add_subdirectory(src)

# Local copy of Mongoose
add_library(
    mongoose
    "lib/mongoose/src/mongoose.c"
    )
set_source_files_properties(
    lib/mongoose/mongoose.c
    PROPERTIES
    COMPILE_DEFINITIONS NS_ENABLE_SSL
    )

target_link_libraries(
    mongoose
    ${OPENSSL_LIBRARIES}
    "-lssl"
    )

