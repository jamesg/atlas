#include "temporal.hpp"

const char atlas::db::attr::temporal::date[] = "date";

std::string atlas::db::detail::utc_date_iso_extended()
{
    return boost::posix_time::to_iso_extended_string(
            boost::posix_time::second_clock::universal_time()
            );
}

