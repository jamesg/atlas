#include "jsonrpc/auth.hpp"

#include "hades/crud.ipp"
#include "hades/filter.hpp"

#include "styx/serialise_json.hpp"

#include "db/auth.hpp"
#include "jsonrpc/request.hpp"
#include "log/log.hpp"

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
    atlas::log::information("jsonrpc::auth::is_logged_in") << "checking " << styx::serialise_json(request.get_element());
    atlas::log::information("jsonrpc::auth::is_logged_in") << "checking " << request.token();
    auto where = hades::where<std::string>(
            "token = ?",
            hades::row<std::string>(request.token())
            );
    styx::list sessions = atlas::user_session::get_collection(conn, where);
    atlas::log::information("jsonrpc::auth::is_logged_in") << "sessions " << styx::serialise_json(sessions);
    return (sessions.size() == 1);
}

bool atlas::jsonrpc::auth::is_superuser(
        hades::connection& conn,
        jsonrpc::request& request
        )
{
    return false;
}

