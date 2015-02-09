#ifndef ATLAS_HTTP_ROUTER_HPP
#define ATLAS_HTTP_ROUTER_HPP

#include <map>

#include <boost/fusion/include/at_c.hpp>
#include <boost/fusion/include/invoke.hpp>
#include <boost/fusion/include/vector.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/ptr_container/ptr_map.hpp>

#include "hades/mkstr.hpp"

#include "log/log.hpp"
#include "uri_type.hpp"

namespace atlas
{
    namespace http
    {
        namespace detail
        {
            /*!
             * \brief Turn a synchronous URI router function into an
             * asynchronous one in order to make it compatible with the lower
             * level basic_function interface.
             */
            uri_type make_async(uri_function_type function);
            /*!
             * \brief Wrapper for a router function as stored by the router.
             */
            class basic_function
            {
            public:
                basic_function(uri_type);

                /*!
                 * \brief Handle a request from the client for an API
                 * function.
                 *
                 * \throws None.
                 */
                void serve(
                        boost::smatch,
                        mg_connection*,
                        uri_callback_type sucess,
                        uri_callback_type failure
                        ) const;

            private:
                uri_type m_serve;
            };

            /*!
             * Copy a string from a regex match result to a Fusion vector.
             */
            template<int Index, typename Container>
            void copy_string_to_vector(
                    const boost::smatch& match,
                    Container& container
                    )
            {
                // Type of the element at index 'Index' in the Container.
                // TODO: enable parsing of ints, etc.
                // For now, only strings are supported.
                typedef
                    typename std::remove_reference<
                        typename boost::fusion::result_of::at<
                            Container, boost::mpl::int_<Index>
                            >::type
                        >::type element_type;
                try
                {
                    if(match.size() > Index+1)
                    {
                        element_type e = boost::lexical_cast<element_type>(
                                match[Index+1]
                                );
                        boost::fusion::at_c<Index>(container) = e;
                    }
                    else
                        boost::fusion::at_c<Index>(container) = element_type();
                }
                catch(const boost::bad_lexical_cast& e)
                {
                    throw std::runtime_error(
                        hades::mkstr() << "casting uri argument: " << e.what()
                        );
                }
            }
            /*!
             * \brief Copy all elements in a boost::smatch to a
             * boost::fusion::vector of std::string.  If the list is too long
             * for the vector, only size(vector) elements will be copied.  If
             * the vector is too long for the boost::smatch, additional
             * elements are set to std::string().
             */
            template<int from, int to>
            struct copy_to_vector
            {
                template<typename Container>
                void copy(const boost::smatch& list, Container& container)
                {
                    copy_string_to_vector<from, Container>(list, container);
                    copy_to_vector<from+1, to>()
                        .template copy<Container>(list, container);
                };
            };
            template<int to>
            struct copy_to_vector<to, to>
            {
                template<typename Container>
                void copy(const boost::smatch&, Container&)
                {
                };
            };

            /*!
             */
            template<typename ...Arguments>
            class unwrapped_function : public basic_function
            {
            public:
                typedef boost::function<http::response(Arguments...)>
                    unwrapped_function_type;

                unwrapped_function(unwrapped_function_type function) :
                    basic_function(
                        detail::make_async(
                            [function](boost::smatch match)
                            {
                                typedef boost::fusion::vector<Arguments...>
                                    arg_values_type;
                                arg_values_type arg_values;
                                copy_to_vector<0, sizeof...(Arguments)>()
                                    .copy(match, arg_values);

                                // Invoke the API function with the argument
                                // list.
                                http::response out =
                                    boost::fusion::invoke(function, arg_values);
                                return out;
                            }
                            )
                        )
                {
                }
            };
        }
        /*!
         * Route incoming HTTP requests to individual router functions.
         */
        class router
        {
        public:
            /*!
             * Route an HTTP request to a router function.
             */
            int operator()(mg_connection*, mg_event);
            /*!
             * \brief Install a function to respond to a specific URI.
             *
             * \param uri_function Function to execute when the URI is
             * requested.  The function will be provided with cpp-netlib
             * request and connection_ptr objects.
             *
             * \note The URI function will be called in the current thread
             * (from the web server's thread pool).
             *
             * \throws std::runtime_error if a function has already been
             * installed for this URI.
             */
            void install(
                std::string uri,
                uri_type uri_function
                );

            /*!
             * \brief Install a function to respond to a URI matched by a regular expression.
             *
             * \param uri Regular expression to match URIs that should be
             * served by this function.
             * \param function Method accepting the URI parameters and
             * returning a string.
             */
            template<typename ...Arguments>
            void install(
                std::string uri,
                typename detail::unwrapped_function<Arguments...>::unwrapped_function_type function
                )
            {
                log::information("atlas::http::router::install") <<
                    "installing " << uri;
                if(m_functions.count(uri))
                    throw std::runtime_error(
                        hades::mkstr() <<
                        "uri handler already registered (" << uri << ")"
                        );
                m_functions.insert(
                    uri,
                    static_cast<detail::basic_function*>(
                        new detail::unwrapped_function<Arguments...>(function)
                        )
                    );
            }
        private:
            //std::map<std::string, uri_type> m_functions;
            boost::ptr_map<std::string, detail::basic_function> m_functions;
        };
    }
}

#endif

