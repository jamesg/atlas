#ifndef ATLAS_JSONRPC_URI_HPP
#define ATLAS_JSONRPC_URI_HPP

#include <boost/asio.hpp>
#include <boost/shared_ptr.hpp>

#include "mongoose.h"

#include "atlas/api/server.hpp"
#include "atlas/http/server/uri_type.hpp"

namespace atlas
{
    namespace api
    {
        class server;
    }
    namespace jsonrpc
    {
        /*!
         * \brief HTTP URI handler for a JSONRPC API request.
         *
         * The 'glue' between atlas::http and atlas::api.  Converts a HTTP
         * request to a HTTP/JSONRPC request.
         */
        void uri(
                boost::shared_ptr<boost::asio::io_service> callback_io,
                api::server&,
                mg_connection *conn,
                boost::smatch,
                http::uri_callback_type success,
                http::uri_callback_type error
                );
    }
}

#endif

