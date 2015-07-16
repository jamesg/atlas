#ifndef ATLAS_AUTH_ROUTER_HPP
#define ATLAS_AUTH_ROUTER_HPP

#include "atlas/http/server/router.hpp"

namespace atlas
{
    namespace auth
    {
        /*!
         * \brief REST API for managing tokens and users.
         */
        class router : public http::router
        {
        public:
            router(
                boost::shared_ptr<boost::asio::io_service>,
                hades::connection&
            );
        };
    }
}

#endif
