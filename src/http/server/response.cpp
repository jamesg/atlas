#include "response.hpp"

#include "styx/element.hpp"
#include "styx/list.hpp"
#include "styx/object.hpp"
#include "styx/serialise_json.hpp"

atlas::http::response atlas::http::text_response(const std::string& str)
{
    atlas::http::response out(str);
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

