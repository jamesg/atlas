#ifndef ATLAS_HTTP_ERROR_HPP
#define ATLAS_HTTP_ERROR_HPP

#include <string>

#include "mongoose.h"

#include "http/server/uri_type.hpp"

namespace atlas
{
    namespace http
    {
        void error(
                int code,
                const std::string& message,
                mg_connection*,
                http::uri_callback_type success,
                http::uri_callback_type error
                );
        void error(
                int code,
                const std::string& message,
                mg_connection*
                );
    }
}

#endif

