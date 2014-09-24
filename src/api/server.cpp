#include "server.hpp"

#include "styx/serialise_json.hpp"

#include "hades/mkstr.hpp"
#include "jsonrpc/request.hpp"
#include "jsonrpc/result.hpp"

atlas::api::async_method_type atlas::api::detail::make_async(method_type method)
{
    return [method](
            jsonrpc::request& request,
            boost::function<void(jsonrpc::result&)> callback
            )
    {
        jsonrpc::result result;
        method(request, result);
        callback(result);
    };
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
        styx::element result_e;
        jsonrpc::result result(result_e);
        result.error() = "Checking authentication";
        log::error("api::detail::basic_method::serve") <<
            "error checking authentication: " << e.what();
        callback(result);
        return;
    }

    if(!authed)
    {
        jsonrpc::result result;
        result.error() = "Not authorised";
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
        styx::element result_e;
        jsonrpc::result result(result_e);
        result.error() = e.what();
        log::error("api::detail::basic_method::serve") <<
            "api exception " << request.method() <<
            ": " << e.what();
        callback(result);
    }
    catch(const std::exception& e)
    {
        styx::element result_e;
        jsonrpc::result result(result_e);
        result.error() = "Unknown error";
        log::error("api::detail::basic_method::serve") <<
            "in api function " << request.method() <<
            ": " << e.what();
        callback(result);
    }
}

atlas::api::server::server()
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
    log::information("api::server::serve") << "request " <<
        "method: " << request.method() << " params: " <<
        std::string(styx::serialise_json(request.params()));

    auto method_it = m_methods.find(request.method());
    if(method_it == m_methods.end())
    {
        styx::element result_e;
        jsonrpc::result result(result_e);
        log::warning("api::server::serve") <<
            "jsonrpc request for unknown method: " << std::string(request.method());
        result.error() = "Method unknown";
        callback(result);
        return;
    }

    method_it->second->serve(request, callback);
}

