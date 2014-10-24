#include "jsonrpc/auth.hpp"

bool atlas::jsonrpc::auth::is_logged_in(jsonrpc::request& request)
{
    return true;
}

bool atlas::jsonrpc::auth::is_superuser(jsonrpc::request& request)
{
    return true;
}

