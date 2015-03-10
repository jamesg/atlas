#include "atlas/api/client/call.hpp"

#include "hades/mkstr.hpp"
#include "styx/serialise_json.hpp"

#include "atlas/http/client.hpp"
#include "atlas/jsonrpc/request.hpp"
#include "atlas/jsonrpc/result.hpp"
#include "atlas/log/log.hpp"

void atlas::api::call(
        boost::shared_ptr<boost::asio::io_service> io,
       const std::string& endpoint,
       jsonrpc::request& request,
       boost::function<void(styx::element&)> success,
       boost::function<void(const std::string&)> failure
       )
{
    log::information("atlas::api::call") << "making request to " << endpoint;
    log::information("atlas::api::call") << "making request " <<
            styx::serialise_json(request);
    http::post(
            io,
            endpoint,
            styx::serialise_json(request),
            [success, failure](const std::string& str) {
                try
                {
                    styx::element e = styx::parse_json(str);
                    jsonrpc::result result(e);
                    if(result.error().empty())
                        success(result.data());
                    else
                        failure(hades::mkstr() << "API call: " << result.error());
                }
                catch(const std::exception&)
                {
                    failure("parsing json");
                }
            },
            [failure](const std::string& str) {
                log::warning("atlas::api::call") << "request unsuccessful: " <<
                        str;
                failure(str);
            }
            );
}

