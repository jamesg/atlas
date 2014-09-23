#ifndef ATLAS_API_CALL_HPP
#define ATLAS_API_CALL_HPP

#include <boost/asio.hpp>
#include <boost/function.hpp>
#include <boost/shared_ptr.hpp>

#include "styx/styx.hpp"

namespace atlas
{
    namespace jsonrpc
    {
        struct request;
        struct result;
    }
    namespace api
    {
       void call(
               boost::shared_ptr<boost::asio::io_service> io,
               const std::string& endpoint,
               jsonrpc::request& request,
               boost::function<void(styx::element&)> success,
               boost::function<void(const std::string&)> failure
               );
    }
}

#endif

