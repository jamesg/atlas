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
        return status;
    }

    if(ev == MG_AUTH)
        return MG_TRUE;
    if(ev != MG_REQUEST)
        return MG_FALSE;
    log::information("http::handler") << "request " << conn->uri;
    std::map<std::string, uri_type>::const_iterator i =
        m_functions.find(conn->uri);
    if(i == m_functions.cend())
    {
        http_connection *http_conn = new http_connection;
        conn->connection_param = http_conn;
        http::error(
                404,
                "function not found",
                conn,
                boost::bind(&http_connection::report_success, http_conn),
                boost::bind(&http_connection::report_failure, http_conn)
                );
    }
    else
    {
        auto f = i->second;
        http_connection *http_conn = new http_connection;
        conn->connection_param = http_conn;
        f(
                conn,
                boost::bind(&http_connection::report_success, http_conn),
                boost::bind(&http_connection::report_failure, http_conn)
                );
    }
    return MG_MORE;
}

void atlas::http::handler::log(const std::string&)
{
}

void atlas::http::handler::install(
    const std::string& uri,
    const uri_type& uri_function
    )
{
    if(m_functions.count(uri))
        throw std::runtime_error(
            hades::mkstr() << "uri handler already registered (" << uri << ")"
            );
    m_functions[uri] = uri_function;
}

