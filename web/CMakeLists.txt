atlas_add_objcopy_target(atlas backbone.js backbone_js.o)
atlas_add_objcopy_target(atlas backbone-min.js backbone-min_js.o)
atlas_add_objcopy_target(atlas underscore.js underscore_js.o)
atlas_add_objcopy_target(atlas underscore-min.js underscore-min_js.o)
atlas_add_objcopy_target(atlas jquery.js jquery_js.o)
atlas_add_objcopy_target(atlas stacked_application.js stacked_application_js.o)
atlas_add_objcopy_target(atlas open-iconic/font/css/open-iconic.css open_iconic_font_css_open_iconic_css.o)
atlas_add_objcopy_target(atlas open-iconic/font/fonts/open-iconic.ttf open_iconic_font_fonts_open_iconic_ttf.o)
atlas_add_objcopy_target(atlas open-iconic/font/fonts/open-iconic.woff open_iconic_font_fonts_open_iconic_woff.o)
atlas_add_objcopy_target(atlas ShareTechMono-Regular.ttf ShareTechMono_Regular_ttf.o)
atlas_add_objcopy_target(atlas ShareTech-Regular.ttf ShareTech_Regular_ttf.o)

add_custom_command(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/static.a
    COMMAND ar -rc ${CMAKE_CURRENT_BINARY_DIR}/static.a
        ${CMAKE_CURRENT_BINARY_DIR}/backbone_js.o
        ${CMAKE_CURRENT_BINARY_DIR}/backbone-min_js.o
        ${CMAKE_CURRENT_BINARY_DIR}/underscore_js.o
        ${CMAKE_CURRENT_BINARY_DIR}/underscore-min_js.o
        ${CMAKE_CURRENT_BINARY_DIR}/jquery_js.o
        ${CMAKE_CURRENT_BINARY_DIR}/stacked_application_js.o
        ${CMAKE_CURRENT_BINARY_DIR}/open_iconic_font_css_open_iconic_css.o
        ${CMAKE_CURRENT_BINARY_DIR}/open_iconic_font_fonts_open_iconic_ttf.o
        ${CMAKE_CURRENT_BINARY_DIR}/open_iconic_font_fonts_open_iconic_woff.o
        ${CMAKE_CURRENT_BINARY_DIR}/ShareTechMono_Regular_ttf.o
        ${CMAKE_CURRENT_BINARY_DIR}/ShareTech_Regular_ttf.o
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    COMMENT "Archiving static HTML libraries to ${CMAKE_CURRENT_BINARY_DIR}/static.a"
    DEPENDS
        ${CMAKE_CURRENT_BINARY_DIR}/backbone_js.o
        ${CMAKE_CURRENT_BINARY_DIR}/backbone-min_js.o
        ${CMAKE_CURRENT_BINARY_DIR}/underscore_js.o
        ${CMAKE_CURRENT_BINARY_DIR}/underscore-min_js.o
        ${CMAKE_CURRENT_BINARY_DIR}/jquery_js.o
        ${CMAKE_CURRENT_BINARY_DIR}/stacked_application_js.o
        ${CMAKE_CURRENT_BINARY_DIR}/open_iconic_font_css_open_iconic_css.o
        ${CMAKE_CURRENT_BINARY_DIR}/open_iconic_font_fonts_open_iconic_ttf.o
        ${CMAKE_CURRENT_BINARY_DIR}/open_iconic_font_fonts_open_iconic_woff.o
        ${CMAKE_CURRENT_BINARY_DIR}/ShareTechMono_Regular_ttf.o
        ${CMAKE_CURRENT_BINARY_DIR}/ShareTech_Regular_ttf.o
    )

add_custom_target(atlas-web-text ALL DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/static.a)

file(GLOB atlas_web_sources src/*.cpp)
add_library(atlas-web ${atlas_web_sources})
add_dependencies(atlas-web atlas-web-text)
target_link_libraries(
    atlas-web
    atlas-com-http-server
    ${CMAKE_CURRENT_BINARY_DIR}/static.a
    )

