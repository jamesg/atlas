#ifndef ATLAS_JSONRPC_RESULT_HPP
#define ATLAS_JSONRPC_RESULT_HPP

#include "styx/styx.hpp"
#include "styx/object.hpp"

namespace atlas
{
    namespace jsonrpc
    {
        /*!
         * Accessor struct for the result of a JSONRPC result.
         */
        struct result : public styx::object
        {
            /*!
             * \brief Create a result with the JSONRPC version preset.
             */
            result()
            {
                jsonrpc() = "2.0";
            }

            /*!
             * \brief Create a result from an existing JSON element.  The
             * JSONRPC version attribute will not be set.
             */
            explicit result(styx::element& e) :
                styx::object(e)
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
            /*!
             * \brief JSONRPC allows the client to attach an id to each
             * procedure call.  The id can be of any type and should be copied
             * to the response.
             */
            styx::element& id() { return get_element("id"); }

            /*!
             * \brief The version of JSONRPC understood by the server should be
             * included in the response.
             */
            std::string& jsonrpc() { return get_string("jsonrpc"); }
        };
    }
}

#endif

