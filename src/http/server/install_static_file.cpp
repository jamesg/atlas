#include "atlas/http/server/install_static_file.hpp"

#include <boost/bind.hpp>

#include "hades/mkstr.hpp"

#include "atlas/http/server/router.hpp"
#include "atlas/http/server/static_file.hpp"
#include "atlas/http/server/server.hpp"

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
            filename,
            _1,
            _2,
            _3,
            _4
            )
        );
}

