atlas_add_web_library(
    PREFIX atlas
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/static.a
    TARGET atlas-web-text
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
        "css/plant-theme.css"
        "css/teletype-theme.css"
        "d3/d3.js"
        "d3/d3.min.js"
        "jquery.js"
        "jquery.min.js"
        "modal.css"
        "modal.js"
        "moment.min.js"
        "open-iconic/font/css/open-iconic.css"
        "open-iconic/font/fonts/open-iconic.ttf"
        "open-iconic/font/fonts/open-iconic.woff"
        "pure-min.css"
        "stacked_application.js"
        "underscore-min.js"
        "underscore.js"
        "xcharts/xcharts.css"
        "xcharts/xcharts.js"
        "xcharts/xcharts.min.css"
        "xcharts/xcharts.min.js"
    )

file(GLOB atlas_web_sources src/*.cpp)
add_library(atlas-web ${atlas_web_sources})
add_dependencies(atlas-web atlas-web-text)
target_link_libraries(
    atlas-web
    atlas-com-http-server
    ${CMAKE_CURRENT_BINARY_DIR}/static.a
    )

