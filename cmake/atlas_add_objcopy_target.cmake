# Add a plain file to be placed in an object file, ready for embedding into an
# executable.
#
macro(atlas_add_objcopy_target)
    cmake_parse_arguments(ADD_OBJCOPY_TARGET "" "PREFIX;FILENAME;OBJECT" "" ${ARGN})
    add_custom_command(
        OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/${ADD_OBJCOPY_TARGET_OBJECT}
        COMMAND ld -r -o ${CMAKE_CURRENT_BINARY_DIR}/${ADD_OBJCOPY_TARGET_OBJECT}
            -z noexecstack --format=binary ${ADD_OBJCOPY_TARGET_FILENAME}
        COMMAND objcopy --prefix-symbols=${ADD_OBJCOPY_TARGET_PREFIX}
            --rename-section .data=.rodata,alloc,load,readonly,data,contents
            ${CMAKE_CURRENT_BINARY_DIR}/${ADD_OBJCOPY_TARGET_OBJECT}
        DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/${ADD_OBJCOPY_TARGET_FILENAME}
        COMMENT "Generating ${ADD_OBJCOPY_TARGET_OBJECT} from ${ADD_OBJCOPY_TARGET_FILENAME}" VERBATIM
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        )
endmacro(atlas_add_objcopy_target)

