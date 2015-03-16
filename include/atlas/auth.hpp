#ifndef ATLAS_AUTH_HPP
#define ATLAS_AUTH_HPP

#include <string>

namespace hades
{
    class connection;
}

namespace atlas
{
    namespace auth
    {
        typedef std::string token_type;

        bool has_permission(
                hades::connection&,
                const std::string& permission,
                const token_type&
                );
        bool is_signed_in(hades::connection&, const token_type&);
        bool is_superuser(hades::connection&, const token_type&);
    }
}

#endif

