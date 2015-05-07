macro(atlas_add_objcopy_target prefix filename object)
    add_custom_command(
        OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/${object}
        COMMAND ld -r -o ${CMAKE_CURRENT_BINARY_DIR}/${object} -z noexecstack --format=binary ${filename}
        COMMAND objcopy --prefix-symbols=${prefix} --rename-section .data=.rodata,alloc,load,readonly,data,contents ${CMAKE_CURRENT_BINARY_DIR}/${object}
        #COMMAND objcopy --prefix-symbols=${prefix} --input binary ${filename} ${CMAKE_CURRENT_BINARY_DIR}/${object}
        DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/${filename}
        COMMENT "Generating ${object} from ${filename}" VERBATIM
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        )
endmacro(atlas_add_objcopy_target)

