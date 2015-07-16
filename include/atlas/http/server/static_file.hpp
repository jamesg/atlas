#ifndef ATLAS_HTTP_STATIC_FILE_HPP
#define ATLAS_HTTP_STATIC_FILE_HPP

#include <string>

#include <boost/regex.hpp>

#include "mongoose.h"

#include "atlas/http/server/uri_type.hpp"

namespace atlas
{
    namespace http
    {
        class mimetypes;

        /*!
         * \brief Serve a static file, or an appropriate error if the file does
         * not exist or cannot be read.
         * \param filename Path to the file to be served.
         */
        void static_file(
                const mimetypes& mime_information,
                const std::string& filename,
                mg_connection*,
                uri_parameters_type,
                http::uri_callback_type success,
                http::uri_callback_type error
                );
    }
}

#endif
