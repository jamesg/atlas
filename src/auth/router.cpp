#include "atlas/auth/router.hpp"

#include "atlas/auth.hpp"
#include "atlas/db/auth.hpp"
#include "atlas/http/server/response.hpp"
#include "hades/crud.ipp"
#include "hades/get_one.hpp"
#include "hades/join.hpp"

atlas::auth::router::router(hades::connection& conn)
{
    install_get<>(
            http::matcher("/session", "GET"),
            [&conn](http::get_parameters_type params) {
                if(params.find("username") == params.end())
                    return atlas::http::text_response(
                        400,
                        "username parameter required"
                        );
                const std::string username(params.find("username")->second);
                if(params.find("password") == params.end())
                    return atlas::http::text_response(
                        400,
                        "password parameter required"
                        );
                const std::string password(params.find("password")->second);

                atlas::user user;
                try
                {
                    user = hades::get_one<atlas::user>(
                        conn,
                        hades::where(
                            "username = ? ",
                            hades::row<std::string>(username)
                            )
                        );
                }
                catch(const std::exception&)
                {
                    return http::text_response(
                            403,
                            "User not found"
                            );
                }
                atlas::user_password user_password;
                user_password.from_id(conn, user.id());
                if(
                    user_password.get_string<db::attr::user_password::password>() !=
                    password
                    )
                    return http::text_response(
                            403,
                            "Incorrect password"
                            );

                atlas::user_session session =
                    db::user_session::start(conn, user.id());

                return http::json_response(session);
            }
            );
    install<std::string>(
            http::matcher("/session/([^/]+)", "DELETE"),
            [&conn](std::string token) {
                return http::text_response(
                    db::user_session::stop(token, conn) ? 200 : 500,
                    "sign out"
                    );
            },
            [](const std::string& token, std::string url_token) {
                // Check that the session token is the one the client is trying
                // to delete.
                return (token == url_token);
            }
            );
    install<>(
            http::matcher("/session", "DELETE"),
            [&conn]() {
                return http::text_response(500, "not yet implemented");
            }
            );
    install<>(
            http::matcher("/user", "GET"),
            [&conn]() {
                return http::json_response(
                        hades::equi_outer_join<user, user_enabled, user_super>(conn)
                        );
            }
            );
    install<int>(
            http::matcher("/user/([0-9]+)", "GET"),
            [&conn](const int user_id) {
                styx::list users(
                        hades::equi_outer_join<user, user_enabled, user_super>(conn)
                        );
                if(users.size() == 0)
                    return atlas::http::text_response(404, "user not found");
                return atlas::http::json_response(users.at(0));
            }
            );
    install_json<user, int>(
            http::matcher("/user/([0-9]+)", "PUT"),
            [&conn](user u, const int) {
                u.update(conn);
                return atlas::http::json_response(u);
            }
            );
    install_json<user>(
            http::matcher("/user", "POST"),
            [&conn](user u) {
                u.insert(conn);
                return atlas::http::json_response(u);
            }
            );
}

