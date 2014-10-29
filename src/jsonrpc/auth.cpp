#include "jsonrpc/auth.hpp"

#include "hades/crud.ipp"
#include "hades/filter.hpp"

#include "db/auth.hpp"

bool atlas::jsonrpc::auth::has_permission(
        hades::connection& conn,
        const std::string& permission,
        jsonrpc::request& request
        )
{
    // Superusers have all permissions.
    if(is_superuser(conn, request))
        return true;

    auto where = hades::where<std::string>(
            "permission = ? AND user.user_id = ?",
            hades::row<std::string>(permission)
            );
    styx::list user_permissions =
            atlas::user_permission::get_collection(conn, where);
    //atlas::user_permission::id_type user_permission{}
    return (user_permissions.size() > 0);
}

bool atlas::jsonrpc::auth::is_logged_in(
        hades::connection& conn,
        jsonrpc::request& request
        )
{
    return true;
}

bool atlas::jsonrpc::auth::is_superuser(
        hades::connection& conn,
        jsonrpc::request& request
        )
{
    return false;
}

