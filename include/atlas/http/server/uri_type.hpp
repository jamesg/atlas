#ifndef ATLAS_HTTP_URI_TYPE_HPP
#define ATLAS_HTTP_URI_TYPE_HPP

#include <boost/function.hpp>

#include "mongoose.h"

#include "atlas/auth/auth.hpp"
#include "atlas/http/server/response.hpp"

namespace atlas
{
    namespace http
    {
        /*!
         * \brief Type of parameters captured from the URI.
         */
        typedef std::vector<std::string> uri_parameters_type;
        /*!
         * \brief Collection of query string arguments.
         */
        typedef std::map<std::string, std::string> get_parameters_type;
        /*!
         * \brief Success or error callback for URI handlers.
         */
        typedef boost::function<void()> uri_callback_type;
        /*!
         * \brief Success callback with a result.
         */
        typedef boost::function<void(const response&)> uri_success_callback_type;
        /*!
         * \brief Synchronous URI function type.
         */
        typedef boost::function<response(uri_parameters_type)> uri_function_type;
        /*!
         * \brief Synchronous URI function type with access to the original
         * mg_connection structure.
         */
        typedef boost::function<response(mg_connection*, uri_parameters_type)>
            conn_uri_function_type;
        /*!
         * \brief Function to handle URI requests.
         *
         * The function will be called with the incoming connection and success
         * and error callbacks (exactly one should be called).
         */
        typedef boost::function<
            void(
                    mg_connection*,
                    uri_parameters_type,
                    uri_callback_type,
                    uri_callback_type
                    )
            >
            uri_type;
        /*!
         * \brief Function to allow/disallow access to a URL based on the
         * current user's credentials.
         */
        typedef boost::function<bool(const auth::token_type&, uri_parameters_type)>
            auth_function_type;
    }
}

#endif
