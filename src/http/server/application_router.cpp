#include "atlas/http/server/application_router.hpp"

#include <boost/bind.hpp>

#include "atlas/http/server/static_text.hpp"

const atlas::http::mimetypes&
atlas::http::application_router::mime_information() const
{
    return m_mime_information;
}

void atlas::http::application_router::install_static_text(
        const std::string& uri,
        const std::string& text
        )
{
    std::string extension;
    {
        std::string::size_type dot_pos = uri.find_last_of('.');
        if(dot_pos != std::string::npos)
            extension = uri.substr(dot_pos+1);
    }
    install_static_text(uri, extension, text);
}

void atlas::http::application_router::install_static_text(
        const std::string& uri,
        const std::string& extension,
        const std::string& text
        )
{
    auto mimetype = m_mime_information.content_type(extension);
    install(
        atlas::http::matcher(uri, "GET"),
        boost::bind(&atlas::http::static_text, mimetype, text, _1, _2, _3, _4)
        );
}

