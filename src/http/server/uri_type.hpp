#ifndef ATLAS_HTTP_URI_TYPE_HPP
#define ATLAS_HTTP_URI_TYPE_HPP

#include <boost/function.hpp>
#include <boost/regex.hpp>

#include "mongoose.h"

namespace atlas
{
    namespace http
    {
        /*!
         * \brief Success or error callback for URI handlers.
         */
        typedef boost::function<void()> uri_callback_type;
        /*!
         * \brief Synchronous URI function type.
         */
        typedef boost::function<std::string(boost::smatch)> uri_function_type;
        /*!
         * \brief Function to handle URI requests.
         *
         * The function will be called with the incoming connection and success
         * and error callbacks (exactly one should be called).
         */
        typedef boost::function<
            void(
                    mg_connection*,
                    boost::smatch,
                    uri_callback_type,
                    uri_callback_type
                    )
            >
            uri_type;
    }
}

#endif

