#include "atlas/http/server/response.hpp"

#include "styx/element.hpp"
#include "styx/list.hpp"
#include "styx/object.hpp"
#include "styx/serialise_json.hpp"

atlas::http::response::response() :
    status_code(200)
{
}

atlas::http::response::response(const std::string& str) :
    data(str),
    status_code(200)
{
    headers["Content-type"] = "text/plain";
}

atlas::http::response atlas::http::text_response(const std::string& str)
{
    atlas::http::response out(str);
    return out;
}

atlas::http::response atlas::http::text_response(
        const int code,
        const std::string& str
        )
{
    atlas::http::response out(str);
    out.status_code = code;
    return out;
}

atlas::http::response atlas::http::raw_json_response(
        const styx::element& element
        )
{
    atlas::http::response out;
    out.data = styx::serialise_json(element);
    out.headers["Content-type"] = "text/json";
    return out;
}

atlas::http::response atlas::http::json_response(
        const styx::element& element
        )
{
    styx::object o;
    o["data"] = element;
    return raw_json_response(o);
}

atlas::http::response atlas::http::json_error_response(
        const std::string& message
        )
{
    styx::object o;
    o["error"] = message;
    atlas::http::response out;
    out.data = styx::serialise_json(o);
    out.headers["Content-type"] = "text/json";
    out.status_code = 500;
    return out;
}

