#ifndef ATLAS_JSONRPC_AUTH_HPP
#define ATLAS_JSONRPC_AUTH_HPP

#include <string>

namespace hades
{
    class connection;
}

namespace atlas
{
    namespace jsonrpc
    {
        struct request;

        namespace auth
        {
            bool has_permission(
                    hades::connection&,
                    const std::string& permission,
                    jsonrpc::request&
                    );
            bool is_logged_in(hades::connection&, jsonrpc::request&);
            bool is_superuser(hades::connection&, jsonrpc::request&);
        }
    }
}

#endif

