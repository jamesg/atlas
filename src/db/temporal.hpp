#ifndef ATLAS_DB_TEMPORAL_HPP
#define ATLAS_DB_TEMPORAL_HPP

#include <boost/date_time/posix_time/posix_time.hpp>

#include "hades/crud.hpp"
#include "hades/has_candidate_key.hpp"
#include "hades/relation.hpp"
#include "hades/tuple.hpp"

#include "db/detail.hpp"

namespace atlas
{
    namespace db
    {
        namespace attr
        {
            namespace temporal
            {
                /*!
                 * \brief A UTC date representing the date of a change to data
                 * in another table.  The default date attribute for any tuple
                 * type inheriting from temporal.
                 */
                extern const char date[];
            }
        }
        /*!
         * \brief Make a semi-temporal type from an existing key type.  The
         * type contains the key given as a template argument and an additional
         * date attribute.  The given key becomes a key in the new type.
         *
         * \note This type is suitable for storing zero or one dates for each
         * tuple in the associated relation.  If any number of tuples must be
         * stored, atlas::db::date_log is more appropriate.
         */
        template<
            typename Id,
            const char *Relation,
            const char *Attribute=attr::temporal::date>
        class semi_temporal :
            public hades::crud<semi_temporal<Id, Relation, Attribute>>,
            public virtual Id::template extend<Attribute>,
            public virtual Id::candidate_key_type,
            public virtual hades::relation<Relation>
        {
            public:
                typedef typename Id::template extend<Attribute> tuple_type;
                typedef hades::crud<semi_temporal<Id, Relation, Attribute>>
                    crud_type;

                semi_temporal()
                {
                }

                explicit semi_temporal(const styx::element& e) :
                    styx::object_accessor(e)
                {
                }

                semi_temporal(const Id& id, const std::string& date)
                {
                    this->get_element() = id.get_element();
                    tuple_type::template get_string<Attribute>() = date;
                }

                std::string& date()
                {
                    return tuple_type::template get_string<Attribute>();
                }

                /*!
                 * \brief Update the record to the given date.
                 *
                 * \post The date attribute of this tuple has been updated.
                 */
                void update(const std::string& date_, hades::connection& conn)
                {
                    date() = date_;
                    crud_type::save(conn);
                }

                /*!
                 * \brief Update the record to the given date.
                 *
                 * \post The date attribute of this tuple has been updated.
                 */
                void update(
                        const boost::posix_time::ptime& date_,
                        hades::connection& conn
                        )
                {
                    date() = boost::posix_time::to_iso_extended_string(
                            date_
                            );
                    crud_type::save(conn);
                }

                /*!
                 * \brief Update the record to the current date.
                 *
                 * \post The date attribute of this tuple has been updated.
                 */
                void update(hades::connection& conn)
                {
                    update(detail::utc_date_iso_extended(), conn);
                }
        };

        /*!
         * \brief Make a date log type from an existing key type.  The
         * type contains the key given as a template argument and an additional
         * date attribute.  The given key and the date attribute become a key
         * in the new type.
         *
         * \note This type is suitable for storing any number of tuples with a
         * date attribute.  If only one date should be stored for each tuple of
         * the associated relation, db::temporal is more appropriate.
         */
        template<
            typename Id,
            const char *Relation,
            const char *Attribute=attr::temporal::date>
        class date_log :
            public hades::crud<date_log<Id, Relation, Attribute>>,
            public Id::attribute_list_type::template extend<Attribute>::tuple_type,
            public Id::attribute_list_type::template extend<Attribute>::has_candidate_key_type,
            public hades::relation<Relation>
        {
            public:
                typedef typename Id::attribute_list_type::
                    template extend<Attribute>::tuple_type
                    tuple_type;
                typedef typename Id::attribute_list_type::
                    template extend<Attribute>::has_candidate_key_type
                    has_candidate_key_type;
                typedef hades::crud<date_log<Id, Relation, Attribute>> crud_type;

                date_log(styx::element& e) :
                    styx::object_accessor(e)
                {
                }

                std::string& date()
                {
                    return tuple_type::template get_string<Attribute>();
                }

                /*!
                 * \brief Update the record to the given date.
                 *
                 * \post The date attribute of this tuple has been updated.
                 */
                void update_date(const std::string& date_, hades::connection& conn)
                {
                    date() = date_;
                    crud_type::insert(conn);
                }

                /*!
                 * \brief Update the record to the current date.
                 *
                 * \post The date attribute of this tuple has been updated.
                 */
                void update_now(hades::connection& conn)
                {
                    update_date(detail::utc_date_iso_extended(), conn);
                }
        };
    }
}

#endif

