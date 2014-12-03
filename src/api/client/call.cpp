#include "call.hpp"

#include "hades/mkstr.hpp"
#include "styx/serialise_json.hpp"

#include "http/client.hpp"
#include "jsonrpc/request.hpp"
#include "jsonrpc/result.hpp"
#include "log/log.hpp"

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
            styx::serialise_json(request.get_element());
    http::post(
            io,
            endpoint,
            styx::serialise_json(request.get_element()),
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

