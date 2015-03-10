#ifndef ATLAS_DB_AUTH_HPP
#define ATLAS_DB_AUTH_HPP

#include "hades/crud.hpp"
#include "hades/flag.hpp"
#include "hades/has_candidate_key.hpp"
#include "hades/has_flags.hpp"
#include "hades/tuple.hpp"
#include "hades/relation.hpp"

#include "atlas/db/temporal.hpp"

namespace atlas
{
    namespace db
    {
        namespace attr
        {
            namespace user
            {
                extern const char user_id[];
                extern const char username[];
            }
            namespace user_password
            {
                extern const char password[];
            }
            namespace user_permission
            {
                extern const char permission[];
            }
            namespace user_session
            {
                extern const char token[];
            }
        }
        namespace flag
        {
            namespace user
            {
                extern const char enabled[];
                extern const char super[];
            }
        }
        namespace relvar
        {
            extern const char user[];
            extern const char user_created[];
            extern const char user_password[];
            extern const char user_permission[];
            extern const char user_session[];
            extern const char user_session_updated[];
        }
    }
    struct user :
        public hades::has_candidate_key<db::attr::user::user_id>,
        public hades::tuple<
            db::attr::user::user_id,
            db::attr::user::username>,
        public hades::relation<db::relvar::user>,
        public hades::has_flags<
            db::flag::user::enabled,
            db::flag::user::super>,
        public hades::crud<user>
    {
        user()
        {
        }
        user(const styx::element& e) :
            styx::object(e)
        {
        }
    };
    typedef atlas::db::semi_temporal<user::id_type, db::relvar::user_created>
        user_created;
    typedef hades::flag<user::id_type, db::flag::user::enabled> user_enabled;
    typedef hades::flag<user::id_type, db::flag::user::super> user_super;
    struct user_password :
        public hades::has_candidate_key<db::attr::user::user_id>,
        public hades::tuple<
            db::attr::user::user_id,
            db::attr::user_password::password>,
        public hades::relation<db::relvar::user_password>,
        public hades::crud<user_password>
    {
        user_password()
        {
        }
        user_password(const styx::element& e) :
            styx::object(e)
        {
        }
        bool check(const std::string& password)
        {
            return get_string<db::attr::user_password::password>() == password;
        }
    };
    struct user_permission :
        public hades::has_candidate_key<
            db::attr::user::user_id,
            db::attr::user_permission::permission>,
        public hades::tuple<
            db::attr::user::user_id,
            db::attr::user_permission::permission>,
        public hades::relation<db::relvar::user_permission>,
        public hades::crud<user_permission>
    {
        user_permission()
        {
        }
        user_permission(const styx::element& e) :
            styx::object(e)
        {
        }
    };
    struct user_session :
        public hades::has_candidate_key<
            db::attr::user::user_id,
            db::attr::user_session::token>,
        public hades::tuple<
            db::attr::user::user_id,
            db::attr::user_session::token>,
        public hades::relation<db::relvar::user_session>,
        public hades::crud<user_session>
    {
        user_session()
        {
        }
        user_session(const styx::element& e) :
            styx::object(e)
        {
        }
    };
    /*!
     * \brief Last time the session was updated
     */
    typedef atlas::db::semi_temporal<
        user_session::id_type,
        db::relvar::user_session_updated>
        user_session_updated;
    namespace db
    {
        namespace user_permission
        {
            std::vector<atlas::user_permission> user_permissions(
                    hades::connection&,
                    atlas::user::id_type
                    );
        }
        namespace user_session
        {
            /*!
             * \brief Create a new session with a randomly generated session
             * token.
             */
            atlas::user_session start(hades::connection&, atlas::user::id_type);
            /*!
             * \brief Check that a session token is valid.  If the token is
             * valid, its last updated time is set to the current time.
             */
            bool validate(hades::connection&, const std::string& token);
            /*!
             * \brief Get a session object from a session token.
             */
            atlas::user_session token_session(
                    hades::connection&,
                    const std::string& token
                    );
            void delete_old(hades::connection&);
        }
        namespace auth
        {
            constexpr const int token_length = 64;

            void create(hades::connection&);
        }
    }
}

#endif

