#ifndef ATLAS_HTTP_CLIENT_IPP
#define ATLAS_HTTP_CLIENT_IPP

#include <boost/network/protocol/http/client.hpp>

namespace atlas
{
    namespace http
    {
        namespace detail
        {
            struct client_impl
            {
                boost::network::http::client client;
            };
        }
    }
}

#endif

