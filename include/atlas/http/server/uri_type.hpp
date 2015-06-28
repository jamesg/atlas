#ifndef ATLAS_HTTP_URI_TYPE_HPP
#define ATLAS_HTTP_URI_TYPE_HPP

#include <boost/function.hpp>
#include <boost/regex.hpp>

#include "mongoose.h"

#include "atlas/auth/auth.hpp"
#include "atlas/http/server/response.hpp"

namespace atlas
{
    namespace http
    {
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
        typedef boost::function<response(boost::smatch)> uri_function_type;
        /*!
         * \brief Synchronous URI function type with access to the original
         * mg_connection structure.
         */
        typedef boost::function<response(mg_connection*, boost::smatch)>
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
                    boost::smatch,
                    uri_callback_type,
                    uri_callback_type
                    )
            >
            uri_type;
        /*!
         * \brief Function to allow/disallow access to a URL based on the
         * current user's credentials.
         */
        typedef boost::function<bool(const auth::token_type&, boost::smatch)>
            auth_function_type;
    }
}

#endif

