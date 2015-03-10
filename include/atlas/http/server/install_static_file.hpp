#ifndef ATLAS_HTTP_INSTALL_STATIC_FILE_HPP
#define ATLAS_HTTP_INSTALL_STATIC_FILE_HPP

#include <string>

#include "atlas/http/fwd.hpp"

namespace atlas
{
    namespace http
    {
        void install_static_file(
                atlas::http::server& server,
                const atlas::http::mimetypes& mime_information,
                const std::string& filename,
                const std::string& uri
                );
    }
}

#endif
