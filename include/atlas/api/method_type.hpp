#ifndef ATLAS_API_METHOD_TYPE_HPP
#define ATLAS_API_METHOD_TYPE_HPP

#include <boost/function.hpp>

namespace atlas
{
    namespace jsonrpc
    {
        struct request;
        struct result;
    }
    namespace api
    {
        typedef boost::function<void(jsonrpc::result&)> callback_type;
        typedef boost::function<
            void(jsonrpc::request&, jsonrpc::result&)
            > method_type;
        typedef boost::function<
            void(jsonrpc::request&, boost::function<void(jsonrpc::result&)>)
            > async_method_type;
    }
}

#endif

