#ifndef ATLAS_HTTP_SERVER_STATIC_TEXT_HPP
#define ATLAS_HTTP_SERVER_STATIC_TEXT_HPP

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
         * \brief Respond to a HTTP request with static text.
         *
         * \param mimetype MIME type of the file.
         * \param content Text to send in the HTTP response.
         */
        void static_text(
                const std::string& mimetype,
                const std::string& content,
                mg_connection*,
                uri_parameters_type,
                http::uri_callback_type success,
                http::uri_callback_type error
                );
    }
}

#endif
