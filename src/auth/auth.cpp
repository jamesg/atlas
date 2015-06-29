#include "atlas/auth/auth.hpp"

#include "hades/crud.ipp"
#include "hades/exists.hpp"
#include "hades/filter.hpp"
#include "hades/join.hpp"

#include "atlas/db/auth.hpp"

bool atlas::auth::has_permission(
        hades::connection& conn,
        const std::string& permission,
        const std::string& token
        )
{
    // Superusers have all permissions.
    if(is_superuser(conn, token))
        return true;

    user_session u(db::user_session::token_session(conn, token));

    return hades::exists<user_permission>(
        conn,
        hades::where(
            "permission = ? AND atlas_user.user_id = ?",
            hades::row<std::string, int>(
                permission,
                u.get_int<db::attr::user::user_id>()
                )
            )
        );
}

bool atlas::auth::is_signed_in(
        hades::connection& conn,
        const std::string& token
        )
{
    return hades::exists<user_session>(
        conn,
        hades::where(
            "token = ?",
            hades::row<std::string>(token)
            )
        );
}

bool atlas::auth::is_superuser(
        hades::connection& conn,
        const std::string& token
        )
{
    //atlas::log::information("atlas::auth::is_superuser") << "is_superuser";
    atlas::user_session session = atlas::db::user_session::token_session(
            conn,
            token
            );
    auto where = hades::where(
            "atlas_user.user_id = ? AND "
            "atlas_user_enabled.user_id IS NOT NULL AND "
            "atlas_user_super.user_id IS NOT NULL ",
            hades::row<int>(session.get_int<atlas::db::attr::user::user_id>())
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
