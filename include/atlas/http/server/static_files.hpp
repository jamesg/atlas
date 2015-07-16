#ifndef ATLAS_HTTP_SERVER_STATIC_FILES_HPP
#define ATLAS_HTTP_SERVER_STATIC_FILES_HPP

#include <boost/shared_ptr.hpp>

#include "atlas/http/server/router.hpp"

namespace atlas
{
    namespace http
    {
        /*!
         * \brief Construct a router to serve useful static files such as
         * common Javascript and CSS libraries.
         */
        boost::shared_ptr<router> static_files(
            boost::shared_ptr<boost::asio::io_service>
        );
    }
}

#endif
