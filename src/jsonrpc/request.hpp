#ifndef ATLAS_JSONRPC_REQUEST_HPP
#define ATLAS_JSONRPC_REQUEST_HPP

#include "styx/styx.hpp"
#include "styx/object.hpp"

namespace atlas
{
    namespace jsonrpc
    {
        /*!
         * Accessor for a JSONRPC request.
         */
        struct request : public styx::object
        {
            request()
            {
            }

            explicit request(const styx::element& e) :
                styx::object(e)
            {
            }

            std::string& method() { return get_string("method"); }
            styx::list& params() { return get_list("params"); }
            std::string& token() { return get_string("token"); }
            /*!
             * \brief JSONRPC allows the client to attach an id to each
             * procedure call.  The id can be of any type and should be copied
             * to the response.
             */
            styx::element& id() { return get_element("id"); }
        };
    }
}

#endif

