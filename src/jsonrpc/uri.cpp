#include "uri.hpp"

#include <boost/bind.hpp>

#include "api/server.hpp"
#include "log/log.hpp"
#include "request.hpp"
#include "result.hpp"

void atlas::jsonrpc::uri(
        boost::shared_ptr<boost::asio::io_service> callback_io,
        api::server& server,
        mg_connection *conn,
        http::uri_callback_type success,
        http::uri_callback_type error
        )
{
    if(std::string(conn->request_method) != "POST")
    {
        log::warning("atlas::jsonrpc::uri") <<
            "conn->request_method != \"POST\"";
        error();
        return;
    }

    if(conn->content == nullptr)
    {
        log::warning("atlas::jsonrpc::uri") << "conn->content is null";
        error();
        return;
    }

    styx::element request_o;
    std::string json(conn->content, (conn->content + conn->content_len));
    if(!styx::parse_json(json, request_o))
    {
        log::warning("atlas::jsonrpc::uri") << "parsing json: " << json;
        error();
        return;
    }
    jsonrpc::request request(request_o);

    const char *token = mg_get_header(conn, "Authorization");
    if(token != nullptr)
        request.token() = std::string(token);

    callback_io->post(
        boost::bind(
            &api::server::serve,
            boost::ref(server),
            request,
            [conn, success](jsonrpc::result jsonrpc_result)
            {
                if(jsonrpc_result.has_key("error"))
                    mg_send_status(conn, 500);
                else
                    mg_send_status(conn, 200);

                log::information("jsonrpc::uri") << "response: " <<
                    styx::serialise_json(jsonrpc_result.get_element());

                // TODO: is Connection: close required?
                mg_send_header(conn, "Connection", "close");
                mg_send_header(conn, "Content-Type", "text/json");

                std::string result_str(
                    styx::serialise_json(jsonrpc_result.get_element())
                    );
                mg_send_data(conn, result_str.c_str(), result_str.length());

                success();
            }
            )
        );
}

