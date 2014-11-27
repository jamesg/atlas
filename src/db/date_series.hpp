#ifndef ATLAS_DB_DATE_SERIES_HPP
#define ATLAS_DB_DATE_SERIES_HPP

#include <boost/date_time/posix_time/posix_time.hpp>

#include "hades/crud.hpp"
#include "hades/filter.hpp"
#include "hades/get_one.hpp"
#include "hades/mkstr.hpp"
#include "hades/relation.hpp"
#include "hades/tuple.hpp"

#include "db/date.hpp"
#include "db/detail.hpp"
#include "log/log.hpp"

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
         * \brief Record a custom id type, a date and a data value.
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
                static constexpr const char * const date_attribute = DateAttribute;
                date_series()
                {
                }
                date_series(const styx::element& e) :
                    styx::object_accessor(e)
                {
                }
                date_series(const Id& id)
                {
                    set_base_id(id);
                }

                // Accessor functions.
                std::string& date()
                {
                    tuple_type& t = static_cast<tuple_type&>(*this);
                    return t.template get_string<DateAttribute>();
                }

                double& attribute()
                {
                    tuple_type& t = static_cast<tuple_type&>(*this);
                    return t.template get_double<Attribute>();
                }

                void set_base_id(Id id)
                {
                    typedef typename Id::attribute_list_type attrl;
                    attrl::copy_attributes(id, *this);
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
                    hades::order_by order_by(
                            hades::mkstr() << DateAttribute << " ASC",
                            limit
                            );
                    styx::list series = date_series<Id, Relation, Attribute, DateAttribute>
                        ::get_collection(conn, order_by);
                    return series;
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
                    hades::crud<
                        date_series<Id, Relation, Attribute, DateAttribute>
                        >::save(conn);
                }
                /*!
                 * \brief Record a value for the date series at the current
                 * time.
                 */
                void record(double attr, hades::connection& conn)
                {
                    return record(
                        db::date::to_string(
                            boost::posix_time::second_clock::universal_time()
                            ),
                        attr,
                        conn
                        );
                }
        };

        /*!
         * \brief Get the nearest date_series object with a date after that
         * specified.
         * \throws std::runtime_error if there is no date_series after the date
         * specified.
         */
        template<typename DateSeries>
        DateSeries nearest_after(
                hades::connection& conn,
                boost::posix_time::ptime date
                )
        {
            hades::filter filter(
                hades::where(
                    hades::mkstr() << DateSeries::date_attribute << " > ?",
                    hades::row<std::string>(db::date::to_string(date))
                    ),
                hades::order_by(
                    hades::mkstr() << DateSeries::date_attribute << " ASC",
                    1
                    )
                );
            return hades::get_one<DateSeries>(conn, filter);
        }

        /*!
         * \brief Get the nearest date_series object with a date after that
         * specified.
         * \throws std::runtime_error if there is no date_series after the date
         * specified.
         */
        template<typename DateSeries>
        DateSeries nearest_before(
                hades::connection& conn,
                boost::posix_time::ptime date
                )
        {
            hades::filter filter(
                hades::where(
                    hades::mkstr() << DateSeries::date_attribute << " < ?",
                    hades::row<std::string>(db::date::to_string(date))
                    ),
                hades::order_by(
                    hades::mkstr() << DateSeries::date_attribute << " DESC",
                    1
                    )
                );
            return hades::get_one<DateSeries>(conn, filter);
        }

        template<typename DateSeries>
        DateSeries nearest(
                hades::connection& conn,
                boost::posix_time::ptime date
                )
        {
            DateSeries before, after;
            try
            {
                before = nearest_before<DateSeries>(conn, date);
            }
            catch(const hades::exception&) { }
            try
            {
                after = nearest_after<DateSeries>(conn, date);
            }
            catch(const hades::exception&) { }

            if(!before.id_set() && !after.id_set())
                throw std::runtime_error(
                        "no date series data, cannot find nearest value"
                        );
            if(before.id_set() && !after.id_set())
                return before;
            if(!before.id_set() && after.id_set())
                return after;
            boost::posix_time::time_duration duration_before(
                date - db::date::to_ptime(before.date())
                );
            boost::posix_time::time_duration duration_after(
                db::date::to_ptime(after.date()) - date
                );
            return (duration_before < duration_after)?before:after;
        }

        /*!
         * \brief Find the distance of date from the nearest point in this
         * series.
         */
        template<typename DateSeries>
        boost::posix_time::time_duration distance(
                hades::connection& conn,
                boost::posix_time::ptime date
                )
        {
            DateSeries near = nearest<DateSeries>(conn, date);
            return db::date::to_ptime(near.date()) - date;
        }

        /*!
         * \brief Estimate the value of the data attribute at precisely
         * the given date by using the nearest data points.
         * \note The value may be inaccurate and should be checked with
         * the accuracy function.
         */
        template<typename DateSeries>
        double estimate(
                hades::connection& conn,
                boost::posix_time::ptime date
                )
        {
            if(
                abs(
                    (
                        db::date::to_ptime(
                            nearest<DateSeries>(conn, date).date()
                            ) -
                        date
                        ).total_seconds()
                    ) > 3600
                )
                throw std::runtime_error("no data point within one hour");

            DateSeries before, after;
            try
            {
                before = nearest_before<DateSeries>(conn, date);
            }
            catch(const hades::exception&) { }
            try
            {
                after = nearest_after<DateSeries>(conn, date);
            }
            catch(const hades::exception&) { }

            if(!before.id_set() && !after.id_set())
                throw std::runtime_error(
                        "no date series data, cannot estimate value"
                        );
            if(before.id_set() && !after.id_set())
            {
                return before.attribute();
            }
            if(!before.id_set() && after.id_set())
            {
                return after.attribute();
            }

            boost::posix_time::ptime before_time(
                    db::date::to_ptime(before.date())
                    );
            boost::posix_time::ptime after_time(
                    db::date::to_ptime(after.date())
                    );
            // before and after are both set, choose an average
            boost::posix_time::time_duration duration =
                after_time - before_time;
            return (
                    before.attribute() * (after_time - date).total_seconds() +
                    after.attribute() * (date - before_time).total_seconds()
                   ) /
                duration.total_seconds();
        }
    }
}

#endif

