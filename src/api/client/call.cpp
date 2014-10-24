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
    log::information("api::call") << "making request to " << endpoint;
    //log::information("api::call") << "making request " << styx::serialise_json(request.get_element());
    http::client::create(
            io,
            endpoint,
            [success, failure](const std::string& str) {
                jsonrpc::result result;
                if(styx::parse_json(str, result.get_element()))
                {
                    if(result.error().empty())
                        success(result.data());
                    else
                        failure(result.error());
                }
                else
                {
                    failure("parsing json");
                }
            },
            [failure](const std::string& str) {
                log::warning("api::call") << "request unsuccessful";
                failure(str);
            }
            )->post(styx::serialise_json(request.get_element()));
}

