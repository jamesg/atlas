#ifndef ATLAS_DB_DATE_HPP
#define ATLAS_DB_DATE_HPP

#include <boost/date_time/posix_time/posix_time.hpp>

namespace atlas
{
    namespace db
    {
        /*!
         * \brief Functions for storing dates in a standard string format.
         *
         * SQLite does not provide a safe 'date' type, so it is up the database
         * user to store dates in a sensible string format.
         */
        namespace date
        {
            /*!
             * \brief Convert a boost::posix_time::ptime to a standard string
             * format.
             */
            std::string to_string(const boost::posix_time::ptime&);
            /*!
             * \brief Convert a string in the standard format to a
             * boost::posix_time::ptime.
             */
            boost::posix_time::ptime to_ptime(const std::string&);
        }
    }
}

#endif

