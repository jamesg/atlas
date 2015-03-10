#ifndef ATLAS_HTTP_SERVER_HPP
#define ATLAS_HTTP_SERVER_HPP

#include <boost/asio.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/thread.hpp>

extern "C"
{
    struct mg_server;
}

namespace atlas
{
    namespace http
    {
        class router;

        class server
        {
        public:
            server(
                    boost::shared_ptr<boost::asio::io_service> callback_io,
                    const char *address,
                    const char *port
                    );
            ~server();

            void start();
            void stop();

            http::router& router() const;

        private:
            void run();

            boost::scoped_ptr<http::router> m_router;
            boost::thread m_thread;
            mg_server *m_mg_server;
        };
    }
}

#endif

