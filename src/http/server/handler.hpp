#ifndef ATLAS_HTTP_HANDLER_HPP
#define ATLAS_HTTP_HANDLER_HPP

#include <map>

//#include <boost/network/protocol/http/server.hpp>

#include "uri_type.hpp"

namespace atlas
{
    namespace http
    {
        /*!
         * Route incoming HTTP requests to individual handler functions.
         */
        class handler
        {
            public:
                /*!
                 * Route an HTTP request to a handler function.
                 */
                int operator()(mg_connection*, mg_event);
                void log(const std::string&);
                /*!
                 * \brief Install a function to respond to a specific URI.
                 *
                 * \param uri_function Function to execute when the URI is
                 * requested.  The function will be provided with cpp-netlib
                 * request and connection_ptr objects.
                 *
                 * \note The URI function will be called in the current thread
                 * (from the web server's thread pool).
                 *
                 * \throws std::runtime_error if a function has already been
                 * installed for this URI.
                 */
                void install(
                    const std::string& uri,
                    const uri_type& uri_function
                    );
            private:
                std::map<std::string, uri_type> m_functions;
        };
    }
}

#endif

