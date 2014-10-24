#ifndef ATLAS_JSONRPC_AUTH_HPP
#define ATLAS_JSONRPC_AUTH_HPP

namespace atlas
{
    namespace jsonrpc
    {
        struct request;

        namespace auth
        {
            bool is_logged_in(jsonrpc::request&);
            bool is_superuser(jsonrpc::request&);
        }
    }
}

#endif

