macro(atlas_add_objcopy_target prefix filename object)
    add_custom_command(
        OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/${object}
        COMMAND objcopy --prefix-symbols=${prefix} --input binary --output elf64-x86-64 --binary-architecture i386 ${filename} ${CMAKE_CURRENT_BINARY_DIR}/${object}
        DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/${filename}
        COMMENT "Generating ${object} from ${filename}" VERBATIM
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        )
endmacro(atlas_add_objcopy_target)

