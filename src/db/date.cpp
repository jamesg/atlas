#include "atlas/db/date.hpp"

std::string atlas::db::date::to_string(const boost::gregorian::date& date)
{
    return boost::gregorian::to_iso_extended_string(date);
}

std::string atlas::db::date::to_string(const boost::posix_time::ptime& date)
{
    return boost::posix_time::to_iso_extended_string(date);
}

boost::gregorian::date atlas::db::date::to_date(const std::string& str)
{
    return boost::gregorian::from_string(str);
}

boost::posix_time::ptime atlas::db::date::to_ptime(const std::string& str)
{
    return boost::date_time::parse_delimited_time<boost::posix_time::ptime>(
            str,
            'T'
            );
}

time_t atlas::db::date::to_unix_time(boost::posix_time::ptime t)
{
    return (
        t -
        boost::posix_time::ptime(boost::gregorian::date(1970, 1, 1))
    ).total_seconds();
}
