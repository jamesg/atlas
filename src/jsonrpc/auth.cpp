#include "atlas/jsonrpc/auth.hpp"

#include "hades/crud.ipp"
#include "hades/filter.hpp"
#include "hades/join.hpp"
#include "styx/serialise_json.hpp"

#include "atlas/db/auth.hpp"
#include "atlas/jsonrpc/request.hpp"
#include "atlas/log/log.hpp"

bool atlas::jsonrpc::auth::has_permission(
        hades::connection& conn,
        const std::string& permission,
        jsonrpc::request& request
        )
{
    // Superusers have all permissions.
    if(is_superuser(conn, request))
        return true;

    auto where = hades::where(
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
    auto where = hades::where(
            "token = ?",
            hades::row<std::string>(request.token())
            );
    styx::list sessions = atlas::user_session::get_collection(conn, where);
    return (sessions.size() == 1);
}

bool atlas::jsonrpc::auth::is_superuser(
        hades::connection& conn,
        jsonrpc::request& request
        )
{
    atlas::log::information("jsonrpc::auth::is_superuser") << "is_superuser";
    atlas::user_session session = atlas::db::user_session::token_session(
            conn,
            request.token()
            );
    auto where = hades::where(
            "user.user_id = ? AND "
            "user_enabled.user_id IS NOT NULL AND "
            "user_super.user_id IS NOT NULL ",
            hades::row<styx::int_type>(session.get_int<db::attr::user::user_id>())
            );
    styx::list users = hades::equi_outer_join<
        atlas::user,
        atlas::user_enabled,
        atlas::user_super
        >(
                conn,
                where
                );
    return (users.size() == 1);
}

