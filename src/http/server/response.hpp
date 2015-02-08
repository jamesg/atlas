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

        response text_response(const std::string&);
        response json_response(const styx::element&);
    }
}

#endif

