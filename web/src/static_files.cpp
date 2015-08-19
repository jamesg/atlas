#include "atlas/http/server/static_files.hpp"

#include <boost/bind.hpp>

#include "atlas/http/server/mimetypes.hpp"
#include "atlas/http/server/static_text.hpp"

#define ATLAS_DECLARE_STATIC_STRING(PREFIX) \
    extern "C" { \
        extern char atlas_binary_##PREFIX##_start; \
        extern char atlas_binary_##PREFIX##_end; \
        extern size_t atlas_binary_##PREFIX##_size; \
    }

#define ATLAS_STATIC_STD_STRING(PREFIX) \
    std::string(&atlas_binary_##PREFIX##_start, &atlas_binary_##PREFIX##_end)

ATLAS_DECLARE_STATIC_STRING(ShareTechMono_Regular_ttf)
ATLAS_DECLARE_STATIC_STRING(ShareTech_Regular_ttf)
ATLAS_DECLARE_STATIC_STRING(backbone_js)
ATLAS_DECLARE_STATIC_STRING(backbone_min_js)
ATLAS_DECLARE_STATIC_STRING(css_defaults_css)
ATLAS_DECLARE_STATIC_STRING(css_form_css)
ATLAS_DECLARE_STATIC_STRING(css_grid_css)
ATLAS_DECLARE_STATIC_STRING(css_list_css)
ATLAS_DECLARE_STATIC_STRING(css_messagebox_css)
ATLAS_DECLARE_STATIC_STRING(css_modal_css)
ATLAS_DECLARE_STATIC_STRING(css_navigation_css)
ATLAS_DECLARE_STATIC_STRING(css_plant_theme_css)
ATLAS_DECLARE_STATIC_STRING(css_table_css)
ATLAS_DECLARE_STATIC_STRING(css_teletype_theme_css)
ATLAS_DECLARE_STATIC_STRING(d3_d3_js)
ATLAS_DECLARE_STATIC_STRING(d3_d3_min_js)
ATLAS_DECLARE_STATIC_STRING(jquery_js)
ATLAS_DECLARE_STATIC_STRING(jquery_min_js)
ATLAS_DECLARE_STATIC_STRING(moment_min_js)
ATLAS_DECLARE_STATIC_STRING(open_iconic_font_css_open_iconic_css)
ATLAS_DECLARE_STATIC_STRING(open_iconic_font_fonts_open_iconic_ttf)
ATLAS_DECLARE_STATIC_STRING(open_iconic_font_fonts_open_iconic_woff)
ATLAS_DECLARE_STATIC_STRING(open_iconic_sprite_open_iconic_svg)
ATLAS_DECLARE_STATIC_STRING(pure_min_css)
ATLAS_DECLARE_STATIC_STRING(stacked_application_js)
ATLAS_DECLARE_STATIC_STRING(svg_injector_js)
ATLAS_DECLARE_STATIC_STRING(svg_injector_min_js)
ATLAS_DECLARE_STATIC_STRING(underscore_js)
ATLAS_DECLARE_STATIC_STRING(underscore_js)
ATLAS_DECLARE_STATIC_STRING(underscore_min_js)
ATLAS_DECLARE_STATIC_STRING(xcharts_xcharts_css)
ATLAS_DECLARE_STATIC_STRING(xcharts_xcharts_js)
ATLAS_DECLARE_STATIC_STRING(xcharts_xcharts_min_css)
ATLAS_DECLARE_STATIC_STRING(xcharts_xcharts_min_js)

boost::shared_ptr<atlas::http::router> atlas::http::static_files(
    boost::shared_ptr<boost::asio::io_service> io
)
{
    boost::shared_ptr<router> out(new router(io));

    mimetypes mime_information;

    auto install_static_text = [&mime_information, out](
            const std::string& url,
            const std::string& text
            )
    {
        std::string extension;
        {
            std::string::size_type dot_pos = url.find_last_of('.');
            if(dot_pos != std::string::npos)
                extension = url.substr(dot_pos+1);
        }
        out->install(
                url,
                boost::bind(
                    &atlas::http::static_text,
                    mime_information.content_type(extension),
                    text,
                    _1,
                    _2,
                    _3,
                    _4
                    )
                );
    };

    install_static_text("/ShareTech-Regular.ttf", ATLAS_STATIC_STD_STRING(ShareTech_Regular_ttf));
    install_static_text("/ShareTechMono-Regular.ttf", ATLAS_STATIC_STD_STRING(ShareTechMono_Regular_ttf));
    install_static_text("/backbone-min.js", ATLAS_STATIC_STD_STRING(backbone_min_js));
    install_static_text("/backbone.js", ATLAS_STATIC_STD_STRING(backbone_js));
    install_static_text("/d3/d3.js", ATLAS_STATIC_STD_STRING(d3_d3_js));
    install_static_text("/d3/d3.min.js", ATLAS_STATIC_STD_STRING(d3_d3_min_js));
    install_static_text("/jquery.js", ATLAS_STATIC_STD_STRING(jquery_js));
    install_static_text("/jquery.min.js", ATLAS_STATIC_STD_STRING(jquery_min_js));
    install_static_text("/moment.min.js", ATLAS_STATIC_STD_STRING(moment_min_js));
    install_static_text("/open-iconic/font/css/open-iconic.css", ATLAS_STATIC_STD_STRING(open_iconic_font_css_open_iconic_css));
    install_static_text("/open-iconic/font/fonts/open-iconic.ttf", ATLAS_STATIC_STD_STRING(open_iconic_font_fonts_open_iconic_ttf));
    install_static_text("/open-iconic/font/fonts/open-iconic.woff", ATLAS_STATIC_STD_STRING(open_iconic_font_fonts_open_iconic_woff));
    install_static_text("/open-iconic/sprite/open-iconic.svg", ATLAS_STATIC_STD_STRING(open_iconic_sprite_open_iconic_svg));
    install_static_text("/pure-min.css", ATLAS_STATIC_STD_STRING(pure_min_css));
    install_static_text("/stacked_application.js", ATLAS_STATIC_STD_STRING(stacked_application_js));
    install_static_text("/svg-injector.js", ATLAS_STATIC_STD_STRING(svg_injector_js));
    install_static_text("/svg-injector.min.js", ATLAS_STATIC_STD_STRING(svg_injector_min_js));
    install_static_text("/underscore-min.js", ATLAS_STATIC_STD_STRING(underscore_min_js));
    install_static_text("/underscore.js", ATLAS_STATIC_STD_STRING(underscore_js));
    install_static_text("/css/defaults.css", ATLAS_STATIC_STD_STRING(css_defaults_css));
    install_static_text("/css/form.css", ATLAS_STATIC_STD_STRING(css_form_css));
    install_static_text("/css/grid.css", ATLAS_STATIC_STD_STRING(css_grid_css));
    install_static_text("/css/list.css", ATLAS_STATIC_STD_STRING(css_list_css));
    install_static_text("/css/messagebox.css", ATLAS_STATIC_STD_STRING(css_messagebox_css));
    install_static_text("/css/modal.css", ATLAS_STATIC_STD_STRING(css_modal_css));
    install_static_text("/css/navigation.css", ATLAS_STATIC_STD_STRING(css_navigation_css));
    install_static_text("/css/plant-theme.css", ATLAS_STATIC_STD_STRING(css_plant_theme_css));
    install_static_text("/css/table.css", ATLAS_STATIC_STD_STRING(css_table_css));
    install_static_text("/css/teletype-theme.css", ATLAS_STATIC_STD_STRING(css_teletype_theme_css));
    install_static_text("/xcharts/xcharts.css", ATLAS_STATIC_STD_STRING(xcharts_xcharts_css));
    install_static_text("/xcharts/xcharts.js", ATLAS_STATIC_STD_STRING(xcharts_xcharts_js));
    install_static_text("/xcharts/xcharts.min.css", ATLAS_STATIC_STD_STRING(xcharts_xcharts_min_css));
    install_static_text("/xcharts/xcharts.min.js", ATLAS_STATIC_STD_STRING(xcharts_xcharts_min_js));

    return out;
}
