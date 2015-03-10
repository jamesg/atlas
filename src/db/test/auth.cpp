#include "catch.hpp"

#include "hades/connection.hpp"
#include "hades/crud.ipp"
#include "hades/filter.hpp"

#include "atlas/db/auth.hpp"

// Database tables are created every time an executable runs, so it must be
// safe to call atlas::db::create on a database that has been initialised.
SCENARIO("the atlas-com database tables can be created twice") {
    GIVEN("a new database") {
        hades::connection conn = hades::connection::in_memory_database();
        WHEN("the tables are created twice") {
            atlas::db::auth::create(conn);
            atlas::db::auth::create(conn);
            THEN("there were no errors") {
                REQUIRE(true);
            }
        }
    }
}

SCENARIO("the root user exists in a new database") {
    GIVEN("a new database") {
        hades::connection conn = hades::connection::in_memory_database();
        WHEN("the database is created") {
            atlas::db::auth::create(conn);
            THEN("the root user is present") {
                auto where = hades::where("username = 'root'");
                styx::list root_users =
                    atlas::user::get_collection(conn, where);
                REQUIRE(root_users.size() == 1);
            }
        }
    }
}

