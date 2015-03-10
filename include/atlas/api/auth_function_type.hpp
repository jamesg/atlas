#ifndef ATLAS_API_AUTH_FUNCTION_TYPE_HPP
#define ATLAS_API_AUTH_FUNCTION_TYPE_HPP

#include <functional>

#include "hades/connection.hpp"

#include "atlas/jsonrpc/request.hpp"

namespace atlas
{
    namespace api
    {
        typedef std::function<bool(jsonrpc::request&)> auth_function_type;
    }
}

#endif

