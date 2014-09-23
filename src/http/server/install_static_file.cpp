#include "install_static_file.hpp"

#include <boost/bind.hpp>

#include "hades/mkstr.hpp"

#include "http/server/handler.hpp"
#include "http/server/static_file.hpp"
#include "http/server/server.hpp"

void atlas::http::install_static_file(
        atlas::http::server& server,
        const atlas::http::mimetypes& mime_information,
        const std::string& filename,
        const std::string& uri
        )
{
    server.router().install(
        uri,
        boost::bind(
            &atlas::http::static_file,
            boost::ref(mime_information),
            std::string(hades::mkstr() << "web/static/" << filename),
            _1,
            _2,
            _3
            )
        );
}

void atlas::http::install_static_file(
        atlas::http::server& server,
        const atlas::http::mimetypes& mime_information,
        const std::string& filename
        )
{
    install_static_file(
            server,
            mime_information,
            filename,
            hades::mkstr() << "/" << filename
            );
}

