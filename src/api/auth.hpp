#ifndef ATLAS_API_AUTH_HPP
#define ATLAS_API_AUTH_HPP

namespace hades
{
    class connection;
}
namespace atlas
{
    namespace api
    {
        class server;
    }
}
namespace atlas
{
    namespace api
    {
        namespace auth
        {
            void install(
                hades::connection& conn,
                atlas::api::server& server
                );
        }
    }
}

#endif

