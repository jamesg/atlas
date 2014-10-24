#include "auth.hpp"

#include <boost/bind.hpp>

#include "hades/get_by_id.hpp"
#include "hades/get_collection.hpp"
#include "hades/join.hpp"

#include "hades/crud.ipp"

#include "api/server.hpp"
#include "db/auth.hpp"

void atlas::api::auth::install(
        hades::connection& conn,
        atlas::api::server& server
        )
{
    server.install<styx::list>(
            "user_list",
            boost::bind(
                &hades::equi_join<
                    atlas::user,
                    atlas::user_enabled,
                    atlas::user_super
                    >,
                boost::ref(conn)
                )
            );
    server.install<styx::element, int>(
            "user_get",
            [&conn](int user_id) {
                atlas::user out = hades::get_by_id<atlas::user>(
                    conn,
                    atlas::user::id_type{user_id}
                    );
                return out.get_element();
            }
            );
    server.install<styx::element, styx::element>(
            "user_save",
            [&conn](styx::element e) {
                atlas::user user(e);
                user.save(conn);
                return user.get_element();
            }
            );
    server.install<bool, styx::element>(
            "user_destroy",
            [&conn](styx::element& e) {
                atlas::user user(e);
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

