#include "atlas/http/server/static_text.hpp"

#include "atlas/http/server/error.hpp"
#include "atlas/log/log.hpp"
#include "atlas/http/server/mimetypes.hpp"

void atlas::http::static_text(
        const std::string& mimetype,
        const std::string& content,
        mg_connection *conn,
        boost::smatch,
        http::uri_callback_type success,
        http::uri_callback_type error
        )
{
    mg_send_status(conn, 200);

    mg_send_header(conn, "Connection", "close");
    mg_send_header(conn, "Content-Type", mimetype.c_str());

    mg_send_data(conn, content.c_str(), content.length());

    success();
}

