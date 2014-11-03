#ifndef ATLAS_DB_DATE_SERIES_HPP
#define ATLAS_DB_DATE_SERIES_HPP

#include "hades/crud.hpp"
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
                date_series()
                {
                }
                date_series(const styx::element& e) :
                    styx::object_accessor(e)
                {
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

