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

ATLAS_DECLARE_STATIC_STRING(backbone_js)
ATLAS_DECLARE_STATIC_STRING(underscore_js)
ATLAS_DECLARE_STATIC_STRING(backbone_min_js)
ATLAS_DECLARE_STATIC_STRING(underscore_js)
ATLAS_DECLARE_STATIC_STRING(underscore_min_js)
ATLAS_DECLARE_STATIC_STRING(jquery_js)
ATLAS_DECLARE_STATIC_STRING(stacked_application_js)
ATLAS_DECLARE_STATIC_STRING(open_iconic_font_css_open_iconic_css)
ATLAS_DECLARE_STATIC_STRING(open_iconic_font_fonts_open_iconic_ttf)
ATLAS_DECLARE_STATIC_STRING(open_iconic_font_fonts_open_iconic_woff)
ATLAS_DECLARE_STATIC_STRING(ShareTechMono_Regular_ttf)
ATLAS_DECLARE_STATIC_STRING(ShareTech_Regular_ttf)

boost::shared_ptr<atlas::http::router> atlas::http::static_files()
{
    boost::shared_ptr<router> out(new router);

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

    install_static_text("/backbone.js", ATLAS_STATIC_STD_STRING(backbone_js));
    install_static_text("/backbone-min.js", ATLAS_STATIC_STD_STRING(backbone_min_js));
    install_static_text("/underscore.js", ATLAS_STATIC_STD_STRING(underscore_js));
    install_static_text("/underscore-min.js", ATLAS_STATIC_STD_STRING(underscore_min_js));
    install_static_text("/jquery.js", ATLAS_STATIC_STD_STRING(jquery_js));
    install_static_text("/stacked_application.js", ATLAS_STATIC_STD_STRING(stacked_application_js));
    install_static_text("/open-iconic/font/css/open-iconic.css", ATLAS_STATIC_STD_STRING(open_iconic_font_css_open_iconic_css));
    install_static_text("/open-iconic/font/fonts/open-iconic.ttf", ATLAS_STATIC_STD_STRING(open_iconic_font_fonts_open_iconic_ttf));
    install_static_text("/open-iconic/font/fonts/open-iconic.woff", ATLAS_STATIC_STD_STRING(open_iconic_font_fonts_open_iconic_woff));
    install_static_text("/ShareTechMono-Regular.ttf", ATLAS_STATIC_STD_STRING(ShareTechMono_Regular_ttf));
    install_static_text("/ShareTech-Regular.ttf", ATLAS_STATIC_STD_STRING(ShareTech_Regular_ttf));

    return out;
}

