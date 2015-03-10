#include "catch.hpp"

#include "atlas/http/client.hpp"
#include "atlas/http/server/response.hpp"
#include "atlas/http/server/router.hpp"
#include "atlas/http/server/server.hpp"

SCENARIO("atlas::http::server") {
    GIVEN("a server with some routes") {
        boost::shared_ptr<boost::asio::io_service> io(
                new boost::asio::io_service
                );
        atlas::http::server server(io, "localhost", "13337");

        server.router().install<>(
                "/test",
                []() {
                    return atlas::http::text_response("test");
                }
                );
        server.router().install<std::string>(
                "/message/(.+)",
                [](std::string message) {
                    return atlas::http::text_response(message);
                }
                );

        WHEN("a request is made for /test") {
            bool successful = false;

            server.start();
            atlas::http::get(
                    io,
                    "http://localhost:13337/test",
                    [&server, &successful](const std::string& result) {
                        THEN("the result is as expected") {
                            REQUIRE(result == "test");
                        }
                        successful = true;
                        server.stop();
                    },
                    [&server](const std::string& result) {
                        server.stop();
                    }
                    );
            io->run();
            io->reset();

            THEN("the request succeeded") {
                REQUIRE(successful);
            }
        }

        WHEN("a request is made for /message") {
            bool successful = false;

            server.start();
            atlas::http::get(
                    io,
                    "http://localhost:13337/message/hello",
                    [&server, &successful](const std::string& result) {
                        THEN("the result is as expected") {
                            REQUIRE(result == "hello");
                        }
                        successful = true;
                        server.stop();
                    },
                    [&server](const std::string& result) {
                        server.stop();
                    }
                    );
            io->run();
            io->reset();

            THEN("the request succeeded") {
                REQUIRE(successful);
            }
        }
    }
}

