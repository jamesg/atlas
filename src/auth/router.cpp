#include "atlas/auth/router.hpp"

#include "atlas/auth.hpp"
#include "atlas/db/auth.hpp"
#include "atlas/http/server/response.hpp"
#include "hades/crud.ipp"
#include "hades/get_one.hpp"

atlas::auth::router::router(hades::connection& conn)
{
    install_get<>(
            http::matcher("/token", "GET"),
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

                return http::json_response(
                        session.get_string<db::attr::user_session::token>()
                        );
            }
            );
    install<std::string>(
            http::matcher("/token/([^/]+)", "DELETE"),
            [&conn](std::string token) {
                return http::text_response(
                    db::user_session::stop(token, conn) ? 200 : 500,
                    "sign out"
                    );
            }
            );
    install<>(
            http::matcher("/token", "DELETE"),
            [&conn]() {
                return http::text_response(500, "not yet implemented");
            }
            );
    install<>(
            http::matcher("/user", "GET"),
            [&conn]() {
                return http::json_response(user::get_collection(conn));
            }
            );
}

