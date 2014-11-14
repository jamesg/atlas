#include "auth.hpp"

#include <boost/bind.hpp>

#include "hades/get_by_id.hpp"
#include "hades/get_one.hpp"
#include "hades/join.hpp"

#include "hades/crud.ipp"

#include "api/server.hpp"
#include "db/auth.hpp"
#include "jsonrpc/auth.hpp"
#include "log/log.hpp"

void atlas::api::auth::install(
        hades::connection& conn,
        atlas::api::server& server
        )
{
    server.install<styx::object, std::string, std::string>(
            "user_signin",
            [&conn](std::string username, std::string password) {
                atlas::user user;
                auto where = hades::where<std::string>(
                    "user.username = ?",
                    hades::row<std::string>(username)
                    );
                try
                {
                    user = hades::get_one<atlas::user>(conn, where);
                }
                catch(const std::exception&)
                {
                    throw api::exception("User not found");
                }

                atlas::user_password user_password;
                user_password.from_id(conn, user.id());
                if(user_password.get_string<db::attr::user_password::password>() !=
                    password)
                    throw api::exception("The password is incorrect");

                atlas::user_session session =
                    db::user_session::start(conn, user.id());

                styx::object user_o = user.get_object();
                user_o.get_string("token") =
                    session.get_string<db::attr::user_session::token>();
                return user_o;
            }
            );
    server.install<styx::list>(
            "user_list",
            boost::bind(
                &hades::equi_outer_join<
                    atlas::user,
                    atlas::user_enabled,
                    atlas::user_super
                    >,
                boost::ref(conn)
                ),
            boost::bind(jsonrpc::auth::is_superuser, boost::ref(conn), _1)
            );
    server.install<styx::element, styx::element>(
            "user_get",
            [&conn](styx::element user_id_e) {
                atlas::user::id_type user_id(user_id_e);
                atlas::user out = hades::get_by_id<atlas::user>(conn, user_id);
                return out.get_element();
            }
            );
    server.install<styx::element, styx::element>(
            "user_save",
            [&conn](styx::element e) {
                atlas::user user(e);
                if(user.exists(conn))
                {
                    atlas::user existing;
                    existing.from_id(conn, user.id());

                    if(existing.get_string<db::attr::user::username>() !=
                        user.get_string<db::attr::user::username>())
                        throw api::exception("Changing usernames is not allowed");
                }
                if(user.get_string<db::attr::user::username>() == "root" &&
                    !user.get_bool<db::flag::user::enabled>())
                    throw api::exception("The root account cannot be disabled");
                if(user.get_string<db::attr::user::username>() == "root" &&
                    !user.get_bool<db::flag::user::super>())
                    throw api::exception("The root account must be a superuser");

                user.save(conn);
                return user.get_element();
            }
            );
    server.install<bool, styx::element>(
            "user_destroy",
            [&conn](styx::element& e) {
                atlas::user user(e);
                if(user.get_string<db::attr::user::username>() == "root")
                    throw api::exception("The root account cannot be removed");
                return user.destroy(conn);
            }
            );
    server.install<styx::list, int>(
            "user_permissions",
            [&conn](int user_id) {
                std::vector<atlas::user_permission> permissions =
                    atlas::db::user_permission::user_permissions(
                        conn,
                        atlas::user::id_type{user_id}
                        );
                styx::list out;
                for(const atlas::user_permission& permission : permissions)
                    out.append(permission.get_element());
                return out;
            }
            );
}

