atlas_add_web_library(
    PREFIX atlas
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/static.a
    SOURCES
        "ShareTech-Regular.ttf"
        "ShareTechMono-Regular.ttf"
        "backbone-min.js"
        "backbone.js"
        "css/defaults.css"
        "css/form.css"
        "css/grid.css"
        "css/list.css"
        "css/messagebox.css"
        "css/modal.css"
        "css/navigation.css"
        "css/teletype-theme.css"
        "jquery.js"
        "modal.css"
        "modal.js"
        "open-iconic/font/css/open-iconic.css"
        "open-iconic/font/fonts/open-iconic.ttf"
        "open-iconic/font/fonts/open-iconic.woff"
        "pure-min.css"
        "stacked_application.js"
        "underscore-min.js"
        "underscore.js"
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

