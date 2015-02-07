#include "handler.hpp"

#include <atomic>

#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>

#include "error.hpp"
#include "hades/mkstr.hpp"
#include "log/log.hpp"

namespace
{
    struct http_connection
    {
        std::atomic<int> status;

        http_connection() :
            status(MG_MORE)
        {
        }

        void report_success()
        {
            status = MG_TRUE;
        }

        void report_failure()
        {
            status = MG_FALSE;
        }
    };
}

atlas::http::uri_type atlas::http::detail::make_async(uri_function_type f)
{
    return [f](
            mg_connection *conn,
            boost::smatch match,
            uri_callback_type success,
            uri_callback_type failure
            )
    {
        std::string result = f(match);
        mg_send_data(conn, result.c_str(), result.length());
        success();
    };
}

atlas::http::detail::basic_function::basic_function(uri_type function) :
    m_serve(function)
{
}

void atlas::http::detail::basic_function::serve(
        boost::smatch match,
        mg_connection *conn,
        uri_callback_type success,
        uri_callback_type failure
        ) const
{
    log::test("atlas::http::detail::basic_function::serve") << "serving request";
    m_serve(conn, match, success, failure);
}

int atlas::http::handler::operator()(
        mg_connection *conn,
        mg_event ev
        )
{
    if(
            ev == MG_POLL &&
            conn->connection_param &&
            ((http_connection*)(conn->connection_param))->status != MG_MORE
      )
    {
        int status = ((http_connection*)(conn->connection_param))->status;
        delete (http_connection*)(conn->connection_param);
        conn->connection_param = nullptr;
        atlas::log::information("http::handler") << "finish " << conn->uri;
        if(status != MG_TRUE)
            http::error(500, "fail", conn);
        return MG_TRUE;
    }

    if(ev == MG_AUTH)
        return MG_TRUE;
    if(ev != MG_REQUEST)
        return MG_FALSE;
    log::information("http::handler") << "request " << conn->uri;

    for(
            boost::ptr_map<std::string, detail::basic_function>::iterator i =
                m_functions.begin(), e = m_functions.end();
            i != e; ++i
            )
    {
        boost::smatch match;
        bool matched = boost::regex_match(
                std::string(conn->uri),
                match,
                boost::regex(i->first)
                );
        if(matched)
        {
            log::test("atlas::http::handler") << "handler found for " << conn->uri << " (" << i->first << ")";
            auto f = *i->second;
            http_connection *http_conn = new http_connection;
            conn->connection_param = http_conn;
            f.serve(
                    match,
                    conn,
                    boost::bind(&http_connection::report_success, http_conn),
                    boost::bind(&http_connection::report_failure, http_conn)
                    );
            return MG_MORE;
        }
    }

    http::error(
            404,
            hades::mkstr() << "uri handler not found for " << conn->uri,
            conn
            );
    return MG_MORE;
}

void atlas::http::handler::install(
    std::string uri,
    uri_type uri_function
    )
{
    log::information("atlas::http::handler::install") << "installing uri handler for " << uri;
    if(m_functions.count(uri))
        throw std::runtime_error(
            hades::mkstr() << "uri handler already registered (" << uri << ")"
            );
    m_functions.insert(uri, new detail::basic_function(uri_function));
}

