#ifndef ATLAS_JSONRPC_REQUEST_HPP
#define ATLAS_JSONRPC_REQUEST_HPP

#include "styx/styx.hpp"
#include "styx/object_accessor.hpp"

namespace atlas
{
    namespace jsonrpc
    {
        /*!
         * Accessor for a JSONRPC request.
         */
        struct request : public styx::object_accessor
        {
            request()
            {
            }

            explicit request(const styx::element& e) :
                styx::object_accessor(e)
            {
            }

            std::string& method() { return get_string("method"); }
            styx::list& params() { return get_list("params"); }
            std::string& token() { return get_string("token"); }
        };
    }
}

#endif

