#include "db/date.hpp"

std::string atlas::db::date::to_string(const boost::posix_time::ptime& date)
{
    return boost::posix_time::to_iso_extended_string(date);
}

boost::posix_time::ptime atlas::db::date::to_ptime(const std::string& str)
{
    return boost::date_time::parse_delimited_time<boost::posix_time::ptime>(
            str,
            'T'
            );
}

