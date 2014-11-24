#include "call.hpp"

#include "hades/mkstr.hpp"

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
                jsonrpc::result result;
                if(styx::parse_json(str, result.get_element()))
                {
                    if(result.error().empty())
                        success(result.data());
                    else
                        failure(hades::mkstr() << "API call: " << result.error());
                }
                else
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

