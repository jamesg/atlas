#ifndef ATLAS_DB_DATE_SERIES_HPP
#define ATLAS_DB_DATE_SERIES_HPP

#include "hades/crud.hpp"
#include "hades/filter.hpp"
#include "hades/relation.hpp"
#include "hades/tuple.hpp"

#include "db/detail.hpp"

namespace atlas
{
    namespace db
    {
        namespace attr
        {
            namespace date_series
            {
                extern const char date[];
            }
        }
        /*!
         * \brief
         */
        template<
            typename Id,
            const char *Relation,
            const char *Attribute,
            const char *DateAttribute=attr::date_series::date>
        class date_series :
            public hades::crud<
                date_series<Id, Relation, Attribute, DateAttribute>
                >,
            public Id::attribute_list_type::
                template extend<DateAttribute>::template extend<Attribute>::
                tuple_type,
            public Id::attribute_list_type::
                template extend<DateAttribute>::has_candidate_key_type,
            public hades::relation<Relation>
        {
            public:
                typedef typename Id::attribute_list_type::
                    template extend<DateAttribute>::template extend<Attribute>::
                    tuple_type tuple_type;
                typedef Id base_id_type;
                date_series()
                {
                }
                date_series(const styx::element& e) :
                    styx::object_accessor(e)
                {
                }

                // Accessor functions.
                std::string& date()
                {
                    tuple_type& t = static_cast<tuple_type&>(*this);
                    return t.template get_string<DateAttribute>();
                }

                // Database functions.

                /*!
                 * \brief Get a list of all series data newer than the given
                 * date, ordered by date ascending.
                 * \param limit Maximum number of series to return.
                 */
                static styx::list newer_than(
                        hades::connection& conn,
                        const std::string& date,
                        const int limit=-1
                        )
                {
                    auto where = hades::where(
                            hades::mkstr() << DateAttribute << " > ?",
                            hades::row<std::string>(date)
                            );
                    auto order_by = hades::order_by(
                            hades::mkstr() << DateAttribute << " ASC",
                            limit
                            );
                    auto filter = hades::filter(
                            where,
                            order_by
                            );
                    return date_series<Id, Relation, Attribute, DateAttribute>
                        ::get_collection(conn, filter);
                }

                /*!
                 * \brief Get the earliest date series recorded in ascending date order.
                 * \param limit Number of date series to return.  Leave as
                 * default to return all date series.
                 */
                static styx::list first(
                        hades::connection& conn,
                        const int limit=-1
                        )
                {
                    auto order_by = hades::order_by(
                            hades::mkstr() << DateAttribute << " ASC",
                            limit
                            );
                    return date_series<Id, Relation, Attribute, DateAttribute>
                        ::get_collection(conn, order_by);
                }

                void set_base_id(Id id)
                {
                    typedef typename Id::attribute_list_type attrl;
                    attrl::copy_attributes(id, *this);
                }
                /*!
                 * \brief Record a value for the date series.
                 */
                void record(
                        const std::string& date,
                        double attr,
                        hades::connection& conn
                        )
                {
                    tuple_type& t = static_cast<tuple_type&>(*this);
                    t.template get_string<DateAttribute>() = date;
                    t.template get_double<Attribute>() = attr;
                    //crud_type::insert(conn);
                    hades::crud<
                        date_series<Id, Relation, Attribute, DateAttribute>
                        >::insert(conn);
                }
                /*!
                 * \brief Record a value for the date series at the current
                 * time.
                 */
                void record(double attr, hades::connection& conn)
                {
                    return record(
                            detail::utc_date_iso_extended(),
                            attr,
                            conn
                            );
                }
        };
    }
}

#endif

