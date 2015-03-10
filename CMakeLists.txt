cmake_minimum_required(VERSION 2.6)

project("Atlas Components")

include_directories("lib/catch/include")
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

