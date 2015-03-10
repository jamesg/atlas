#ifndef ATLAS_HTTP_RESPONSE_HPP
#define ATLAS_HTTP_RESPONSE_HPP

#include <map>

#include "styx/element.hpp"

namespace atlas
{
    namespace http
    {
        struct response
        {
            std::string data;
            std::map<std::string, std::string> headers;

            response()
            {
            }

            response(const std::string& str) :
                data(str)
            {
                headers["Content-type"] = "text/plain";
            }
        };

        /*!
         * \brief Generate a plain text response.
         */
        response text_response(const std::string&);
        /*!
         * \brief Generate a raw JSON response.
         *
         * Returning raw JSON data to the client presents a potential security
         * risk.  If the data is returned to a web browser, malicious
         * Javascript running on a third party website could intercept the
         * data.  Always prefer atlas::http::json_response to
         * atlas::http::raw_json_response.
         */
        response raw_json_response(const styx::element&);
        /*!
         * \brief Generate a JSON response containing the element at a key
         * named "data".
         */
        response json_response(const styx::element&);
    }
}

#endif

