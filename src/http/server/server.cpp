#include "atlas/http/server/server.hpp"

#include <iostream>
#include <string>

#include "mongoose.h"

#include "atlas/http/server/router.hpp"

namespace
{
    int accept(mg_connection *conn, mg_event ev)
    {
        return ((atlas::http::server*)conn->server_param)->router()(conn, ev);
    }
}

atlas::http::server::server(
        boost::shared_ptr<boost::asio::io_service> callback_io,
        const char *address,
        const char *port
        ) :
    m_router(new http::router)
{
    m_mg_server = mg_create_server((void*)this, &accept);
    if(m_mg_server == nullptr)
        throw std::runtime_error("creating mongoose server");
    mg_set_option(m_mg_server, "listening_port", port);
}

atlas::http::server::~server()
{
    mg_destroy_server(&m_mg_server);
}

atlas::http::router& atlas::http::server::router() const
{
    return *m_router;
}

void atlas::http::server::start()
{
    m_thread = boost::thread(boost::bind(&server::run, this));
}

void atlas::http::server::stop()
{
    m_thread.interrupt();
    m_thread.join();
}

void atlas::http::server::run()
{
    for(;;)
    {
        boost::this_thread::interruption_point();
        mg_poll_server(m_mg_server, 50);
    }
}

