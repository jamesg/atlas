#ifndef ATLAS_DB_DATE_HPP
#define ATLAS_DB_DATE_HPP

#include <ctime>

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
             * \brief Length of a date string returned by
             * to_string(boost::gregorian::date).
             */
            static const int date_len = 10;
            /*!
             * \brief Length of a date time string returned by
             * to_string(boost::posix_time::ptime).
             */
            static const int datetime_len = 19;

            /*!
             * \brief Convert a boost::gregorian::date to a standard string
             * format.
             */
            std::string to_string(const boost::gregorian::date&);
            /*!
             * \brief Convert a boost::posix_time::ptime to a standard string
             * format.
             */
            std::string to_string(const boost::posix_time::ptime&);
            /*!
             * \brief Convert a string in the standard date format to a
             * boost::gregorian::date.
             */
            boost::gregorian::date to_date(const std::string&);
            /*!
             * \brief Convert a string in the standard format to a
             * boost::posix_time::ptime.
             */
            boost::posix_time::ptime to_ptime(const std::string&);

            /*!
             * \brief Convert a UTC boost::posix_time::ptime to a Unix
             * timestamp.
             *
             * \note Boost has a function boost::posix_time::from_time_t to do
             * the opposite conversion.
             */
            time_t to_unix_time(boost::posix_time::ptime t);
        }
    }
}

#endif
