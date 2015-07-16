#include "atlas/api/server.hpp"

#include <boost/bind.hpp>

#include "hades/mkstr.hpp"
#include "styx/serialise_json.hpp"

#include "atlas/jsonrpc/request.hpp"
#include "atlas/jsonrpc/result.hpp"

namespace
{
    void async_template(
            atlas::api::method_type method,
            atlas::jsonrpc::request& request,
            boost::function<void(atlas::jsonrpc::result&)> callback
            )
    {
        atlas::jsonrpc::result result;
        method(request, result);
        result.id() = request.id();
        callback(result);
    }
}

atlas::api::async_method_type atlas::api::detail::make_async(method_type method)
{
    return boost::bind(async_template, method, _1, _2);
    //return [method](
            //jsonrpc::request& request,
            //boost::function<void(jsonrpc::result&)> callback
            //)
    //{
        //jsonrpc::result result;
        //method(request, result);
        //result.id() = request.id();
        //callback(result);
    //};
}

atlas::api::detail::basic_method::basic_method(
        async_method_type method,
        auth_function_type check_auth
        ) :
    m_serve(method),
    m_check_auth(check_auth)
{
}

void atlas::api::detail::basic_method::serve(
        jsonrpc::request& request,
        boost::function<void(jsonrpc::result)> callback
        ) const
{
    bool authed = false;
    try
    {
        authed = m_check_auth(request);
    }
    catch(const std::exception& e)
    {
        jsonrpc::result result;
        result.error() = "Checking authentication";
        result.unauthorised() = true;
        log::error("api::detail::basic_method::serve") <<
            "error checking authentication: " << e.what();
        result.id() = request.id();
        callback(result);
        return;
    }

    if(!authed)
    {
        jsonrpc::result result;
        result.error() = "Not authorised";
        result.unauthorised() = true;
        result.id() = request.id();
        callback(result);
        return;
    }

    // m_serve may throw exceptions if it is synchronous or
    // is the first part of an asynchronous method.
    // The exception .what() text will be returned to the
    // client only for api::exception's.  The text from
    // other exceptions will not be returned in case it
    // contains sensitive information.
    try
    {
        m_serve(request, callback);
    }
    catch(const api::exception& e)
    {
        jsonrpc::result result;
        result.error() = e.what();
        log::error("atlas::api::detail::basic_method::serve") <<
            "api exception " << request.method() <<
            ": " << e.what();
        result.id() = request.id();
        callback(result);
    }
    catch(const std::exception& e)
    {
        jsonrpc::result result;
        result.error() = "Unknown error";
        log::error("atlas::api::detail::basic_method::serve") <<
            "in api function " << request.method() <<
            ": " << e.what();
        result.id() = request.id();
        callback(result);
    }
}

void atlas::api::server::serve(
        mg_connection *mg_conn,
        atlas::http::uri_parameters_type,
        http::uri_callback_type success,
        http::uri_callback_type failure
        ) const
{
    if(std::string(mg_conn->request_method) != "POST")
    {
        log::warning("atlas::api::server::serve") <<
            "mg_conn->request_method != \"POST\"";
        failure();
        return;
    }

    if(mg_conn->content == nullptr)
    {
        log::warning("atlas::api::server::serve") << "mg_conn->content is null";
        failure();
        return;
    }

    std::string json(mg_conn->content, (mg_conn->content + mg_conn->content_len));
    try
    {
        styx::element request_o = styx::parse_json(json);
        jsonrpc::request request(request_o);
        const char *token = mg_get_header(mg_conn, "Authorization");
        if(token != nullptr)
            request.token() = std::string(token);

        m_io->post(
            boost::bind(
                &api::server::serve,
                this,
                request,
                [mg_conn, success](jsonrpc::result jsonrpc_result)
                {
                    if(jsonrpc_result.has_key("error"))
                        mg_send_status(mg_conn, 500);
                    else
                        mg_send_status(mg_conn, 200);

                    log::information("atlas::api::server::serve") << "response: " <<
                        styx::serialise_json(jsonrpc_result);

                    // TODO: is Connection: close required?
                    mg_send_header(mg_conn, "Connection", "close");
                    mg_send_header(mg_conn, "Content-Type", "application/json");

                    std::string result_str(
                        styx::serialise_json(jsonrpc_result)
                        );
                    mg_send_data(mg_conn, result_str.c_str(), result_str.length());

                    success();
                }
                )
            );
    }
    catch(const std::exception&)
    {
        log::warning("atlas::api::server::serve") << "parsing json: " << json;
        failure();
    }
}

atlas::api::server::server(boost::shared_ptr<boost::asio::io_service> io) :
    m_io(io)
{
}

void atlas::api::server::install_async(
        std::string name,
        async_method_type method
        )
{
    if(m_methods.find(name) != m_methods.end())
        throw std::runtime_error(
                hades::mkstr() << "method already installed (" << name << ")"
                );
    m_methods.insert(
            name,
            new detail::basic_method(
                method,
                [](const jsonrpc::request&) { return true; }
                )
            );
}

void atlas::api::server::install(
        std::string name,
        method_type method,
        auth_function_type auth_function
        )
{
    if(m_methods.find(name) != m_methods.end())
        throw std::runtime_error(
                hades::mkstr() << "method already installed (" << name << ")"
                );
    m_methods.insert(
            name,
            new detail::basic_method(
                detail::make_async(method),
                auth_function
                )
            );
}

void atlas::api::server::serve(
        jsonrpc::request& request,
        boost::function<void(jsonrpc::result&)> callback
        ) const
{
    log::information("atlas::api::server::serve") << "request " <<
        "method: " << request.method() << " params: " <<
        std::string(styx::serialise_json(request.params())) <<
        "token: " << request.token();

    auto method_it = m_methods.find(request.method());
    if(method_it == m_methods.end())
    {
        styx::element result_e;
        jsonrpc::result result(result_e);
        log::warning("atlas::api::server::serve") <<
            "jsonrpc request for unknown method: " << std::string(request.method());
        result.error() = "Method unknown";
        result.id() = request.id();
        callback(result);
        return;
    }

    method_it->second->serve(request, callback);
}
