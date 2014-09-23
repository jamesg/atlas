#include "log/log.hpp"

namespace
{
    static atlas::log::detail::cerr_sink g_default_sink;
}

atlas::log::detail::basic_sink& atlas::log::detail::get_default_sink()
{
    return g_default_sink;
}

