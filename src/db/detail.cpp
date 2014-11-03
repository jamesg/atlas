#include "db/detail.hpp"

#include <boost/date_time/posix_time/posix_time.hpp>

std::string atlas::db::detail::utc_date_iso_extended()
{
    return boost::posix_time::to_iso_extended_string(
            boost::posix_time::second_clock::universal_time()
            );
}

