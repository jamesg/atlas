#include "atlas/db/auth.hpp"

#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/nondet_random.hpp>
#include <boost/random/variate_generator.hpp>
#include <boost/random/uniform_int.hpp>

#include "atlas/log/log.hpp"
#include "hades/crud.ipp"
#include "hades/devoid.hpp"
#include "hades/exists.hpp"
#include "hades/filter.hpp"
#include "hades/get_collection.hpp"
#include "hades/get_one.hpp"

const char atlas::db::attr::user::user_id[] = "user_id";
const char atlas::db::attr::user::username[] = "username";
const char atlas::db::attr::user_password::password[] = "password";
const char atlas::db::attr::user_permission::permission[] = "permission";
const char atlas::db::attr::user_session::token[] = "token";
const char atlas::db::flag::user::enabled[] = "atlas_user_enabled";
const char atlas::db::flag::user::super[] = "atlas_user_super";
const char atlas::db::relvar::user[] = "atlas_user";
const char atlas::db::relvar::user_created[] = "atlas_user_created";
const char atlas::db::relvar::user_password[] = "atlas_user_password";
const char atlas::db::relvar::user_permission[] = "atlas_user_permission";
const char atlas::db::relvar::user_session[] = "atlas_user_session";
const char atlas::db::relvar::user_session_updated[] = "atlas_user_session_updated";

namespace
{
    constexpr const int session_expiry_hours = 24;
    /*!
     * \brief Generate a random token of length token_length.
     */
    std::string generate_token()
    {
        std::ostringstream oss;
        boost::random_device rng;
        // Only characters that don't require URI encoding are used.
        static const std::string chars =
             "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
             "abcdefghijklmnopqrstuvwxyz"
             "0123456789";//+/";
        boost::variate_generator<boost::random_device&, boost::uniform_int<>>
            gen(rng, boost::uniform_int<>(0, chars.size()-1));
        for(int i = 0; i < atlas::db::auth::token_length; ++i)
            oss << chars[gen()];
        return oss.str();
    }
}

std::vector<atlas::user_permission> atlas::db::user_permission::user_permissions(
        hades::connection& conn,
        atlas::user::id_type id
        )
{
    auto where =
        hades::where(
            "user_id = ?",
            hades::row<styx::int_type>(id.get_int<db::attr::user::user_id>())
            );
    return hades::get_collection_vector<atlas::user_permission>(conn, where);
}

atlas::user_session atlas::db::user_session::start(
        hades::connection& conn,
        atlas::user::id_type id
        )
{
    atlas::user_session session;
    session.get_int<db::attr::user::user_id>() =
        id.get_int<db::attr::user::user_id>();
    session.get_string<db::attr::user_session::token>() = generate_token();
    session.save(conn);
    return session;
}

bool atlas::db::user_session::stop(
        const std::string& token,
        hades::connection& conn
        )
{
    return hades::devoid(
            "DELETE FROM atlas_user_session WHERE token = ?",
            hades::row<std::string>(token),
            conn
            ) > 0;
}

atlas::user_session atlas::db::user_session::token_session(
        hades::connection& conn,
        const std::string& token
        )
{
    auto where = hades::where(
            "token = ?",
            hades::row<std::string>(token)
            );
    return hades::get_one<atlas::user_session>(conn, where);
}

void atlas::db::user_session::delete_old(hades::connection& conn)
{
    boost::posix_time::ptime earliest_session =
            boost::posix_time::second_clock::universal_time() -
            boost::posix_time::hours(session_expiry_hours);
    hades::devoid(
            "DELETE FROM atlas_user_session WHERE user_id IN ( "
            " SELECT user_id FROM atlas_user_session_updated "
            " WHERE date < ? "
            " )",
            hades::row<std::string>(
                boost::posix_time::to_iso_extended_string(earliest_session)
                ),
            conn
            );
}

void atlas::db::auth::create(hades::connection& conn)
{
    hades::devoid(
            "CREATE TABLE IF NOT EXISTS atlas_user ( "
            " user_id INTEGER PRIMARY KEY AUTOINCREMENT, "
            " username VARCHAR, "
            " UNIQUE(username) "
            " ) ",
            conn
            );
    hades::devoid(
            "CREATE TABLE IF NOT EXISTS atlas_user_enabled ( "
            " user_id INTEGER PRIMARY KEY "
            "  REFERENCES atlas_user(user_id) ON DELETE CASCADE "
            " ) ",
            conn
            );
    hades::devoid(
            "CREATE TABLE IF NOT EXISTS atlas_user_super ( "
            " user_id INTEGER PRIMARY KEY "
            "  REFERENCES atlas_user(user_id) ON DELETE CASCADE "
            " ) ",
            conn
            );
    hades::devoid(
            "CREATE TABLE IF NOT EXISTS atlas_user_created ( "
            " user_id INTEGER, "
            " date VARCHAR, "
            " FOREIGN KEY(user_id) "
            "  REFERENCES atlas_user(user_id) ON DELETE CASCADE "
            " ) ",
            conn
            );
    hades::devoid(
            "CREATE TABLE IF NOT EXISTS atlas_user_password ( "
            " user_id INTEGER, "
            " password VARCHAR, "
            " PRIMARY KEY(user_id), "
            " FOREIGN KEY(user_id) "
            "  REFERENCES atlas_user(user_id) ON DELETE CASCADE "
            " ) ",
            conn
            );
    hades::devoid(
            "CREATE TABLE IF NOT EXISTS atlas_user_password ( "
            " user_id INTEGER, "
            " permission VARCHAR, "
            " PRIMARY KEY(user_id, permission), "
            " FOREIGN KEY(user_id) "
            "  REFERENCES atlas_user(user_id) ON DELETE CASCADE "
            " ) ",
            conn
            );
    hades::devoid(
            "CREATE TABLE IF NOT EXISTS atlas_user_session ( "
            " user_id INTEGER, "
            " token VARCHAR, "
            " PRIMARY KEY(token), "
            " FOREIGN KEY(user_id) "
            "  REFERENCES atlas_user(user_id) ON DELETE CASCADE "
            " ) ",
            conn
            );
    hades::devoid(
            "CREATE TABLE IF NOT EXISTS atlas_user_session_updated ( "
            " token VARCHAR, "
            " date VARCHAR, "
            " PRIMARY KEY(token), "
            " FOREIGN KEY(token) "
            "  REFERENCES atlas_user_session(token) ON DELETE CASCADE "
            " ) ",
            conn
            );
    if(!hades::exists<atlas::user>(conn, hades::where("username = 'root'")))
    {
        atlas::user root;
        root.get_string<db::attr::user::username>() = "root";
        root.get_bool<db::flag::user::enabled>() = true;
        root.get_bool<db::flag::user::super>() = true;
        root.save(conn);
    }
}

bool atlas::db::user_session::validate(hades::connection& conn, const std::string& token)
{
    // TODO check that the session time is valid.
    // TODO update the session valid time.
    log::test("atlas::db::user_session::validate") << "validating " << token;
    bool valid = hades::exists<atlas::user_session>(
            conn,
            hades::where(
                "token = ?",
                hades::row<std::string>(token)
                )
            );
    log::test("atlas::db::user_session::validate") << (valid ? "valid" : "invalid");
    return valid;
}

