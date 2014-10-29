#ifndef ATLAS_JSONRPC_RESULT_HPP
#define ATLAS_JSONRPC_RESULT_HPP

#include "styx/styx.hpp"
#include "styx/object_accessor.hpp"

namespace atlas
{
    namespace jsonrpc
    {
        /*!
         * Accessor struct for the result of a JSONRPC result.
         */
        struct result : public styx::object_accessor
        {
            result()
            {
            }

            explicit result(styx::element& e) :
                styx::object_accessor(e)
            {
            }

            /*!
             * \brief Standard JSONRPC error field.
             */
            std::string& error() { return get_string("error"); }
            /*!
             * \brief Standard JSONRPC result data field.
             */
            styx::element& data() { return get_element("result"); }
            /*!
             * \brief Non-standard error field.  Set to true if there was an
             * authentiation error.
             */
            bool& unauthorised() { return get_bool("unauthorised"); }
        };
    }
}

#endif

