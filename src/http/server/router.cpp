#include "atlas/http/server/router.hpp"

#include <atomic>

#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>

#include "hades/mkstr.hpp"

#include "atlas/http/server/error.hpp"
#include "atlas/log/log.hpp"

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
        response r = f(match);
        mg_send_status(conn, r.status_code);
        for(std::pair<std::string, std::string> header : r.headers)
            mg_send_header(conn, header.first.c_str(), header.second.c_str());
        mg_send_data(conn, r.data.c_str(), r.data.length());
        success();
    };
}

atlas::http::uri_type atlas::http::detail::make_async_with_data(data_uri_function_type f)
{
    return [f](
            mg_connection *conn,
            boost::smatch match,
            uri_callback_type success,
            uri_callback_type failure
            )
    {
        response r = f(std::string(conn->content, conn->content_len), match);
        for(std::pair<std::string, std::string> header : r.headers)
            mg_send_header(conn, header.first.c_str(), header.second.c_str());
        mg_send_data(conn, r.data.c_str(), r.data.length());
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
    m_serve(conn, match, success, failure);
}

int atlas::http::router::operator()(
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
        if(status != MG_TRUE)
            http::error(500, "fail", conn);
        return MG_TRUE;
    }

    if(ev == MG_AUTH)
        return MG_TRUE;
    if(ev != MG_REQUEST)
        return MG_FALSE;
    log::information("atlas::http::router") << "request " << conn->uri;

    http_connection *http_conn = new http_connection;
    conn->connection_param = http_conn;

    for(
            boost::ptr_map<matcher, detail::basic_function>::iterator i =
                m_functions.begin(), e = m_functions.end();
            i != e; ++i
            )
    {
        boost::smatch match;
        bool matched = i->first.matches(
                std::string(conn->uri),
                std::string(conn->request_method),
                match
                );
        if(matched)
        {
            auto f = *i->second;
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
            conn,
            boost::bind(&http_connection::report_success, http_conn),
            boost::bind(&http_connection::report_failure, http_conn)
            );
    return MG_MORE;
}

void atlas::http::router::install(
    matcher m,
    uri_type uri_function
    )
{
    // This won't catch all conflicting handlers because they are based on
    // regular expressions.
    if(m_functions.count(m))
        throw std::runtime_error(
            hades::mkstr() << "uri handler already registered"
            );
    m_functions.insert(m, new detail::basic_function(uri_function));
}

