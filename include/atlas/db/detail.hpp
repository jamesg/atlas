#ifndef ATLAS_DB_DETAIL_HPP
#define ATLAS_DB_DETAIL_HPP

#include <string>

namespace atlas
{
    namespace db
    {
        namespace detail
        {
            /*!
             * \brief Get the current date/time formatted as an ISO extended
             * string (ready for insertion into a SQLite database).
             */
            std::string utc_date_iso_extended();
        }
    }
}

#endif

