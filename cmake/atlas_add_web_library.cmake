# Generate a library of plain files to be embedded into an executable.
#
# Parameters:
#  PREFIX - Prefix for names of objects in the static library (passed directly
#  to objcopy's --prefix-symbols option).
#  OUTPUT - Path to a static library file.
#  SOURCES - List of plain files to be added to the library.  Paths should be
#  relative to the current source directory.  Certain characters (., /, -) in
#  the path will be replaced by underscores (_) to produce variable names that
#  can be accessed from C code.
macro(atlas_add_web_library)
    cmake_parse_arguments("ADD_WEB_LIBRARY" "" "PREFIX;OUTPUT" "SOURCES" ${ARGN})

    foreach(SOURCE IN ITEMS ${ADD_WEB_LIBRARY_SOURCES})
        string(REGEX REPLACE "[-./]" "_" BASENAME ${SOURCE})
        list(APPEND OBJECTS "${BASENAME}.o")
        atlas_add_objcopy_target(
            PREFIX ${ADD_WEB_LIBRARY_PREFIX}
            FILENAME "${SOURCE}"
            OBJECT "${BASENAME}.o"
            )
    endforeach(SOURCE)

    add_custom_command(
        OUTPUT ${ADD_WEB_LIBRARY_OUTPUT}
        COMMAND ar -rc ${ADD_WEB_LIBRARY_OUTPUT} ${OBJECTS}
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        COMMENT "Archiving static HTML libraries to ${CMAKE_CURRENT_BINARY_DIR}/static.a"
        DEPENDS ${OBJECTS}
        )
endmacro(atlas_add_web_library)

