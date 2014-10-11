#include "error.hpp"

void atlas::http::error(
        int code,
        const std::string& message,
        mg_connection *conn,
        http::uri_callback_type success,
        http::uri_callback_type error
        )
{
    mg_send_status(conn, code);
    // TODO: is Connection: close required?
    mg_send_header(conn, "Connection", "close");
    mg_send_header(conn, "Content-Type", "text/plain");
    mg_send_data(conn, message.c_str(), message.length());
}

void atlas::http::error(
        int code,
        const std::string& message,
        mg_connection *conn
        )
{
    mg_send_status(conn, code);
    // TODO: is Connection: close required?
    mg_send_header(conn, "Connection", "close");
    mg_send_header(conn, "Content-Type", "text/plain");
    mg_send_data(conn, message.c_str(), message.length());
}

