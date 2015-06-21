#ifndef ATLAS_HTTP_ROUTER_HPP
#define ATLAS_HTTP_ROUTER_HPP

#include <map>

#include <boost/fusion/include/at_c.hpp>
#include <boost/fusion/include/invoke.hpp>
#include <boost/fusion/include/vector.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/ptr_container/ptr_map.hpp>

#include "hades/mkstr.hpp"
#include "styx/cast.hpp"
#include "styx/is_json_type.hpp"
#include "styx/styx.hpp"

#include "atlas/log/log.hpp"
#include "atlas/http/server/matcher.hpp"
#include "atlas/http/server/uri_type.hpp"

namespace atlas
{
    namespace http
    {
        namespace detail
        {
            /*!
             * \brief Remove the first const, volatile and reference from
             * 'Type'.
             */
            template<typename Type>
            struct base_type
            {
                typedef typename std::remove_cv<typename std::remove_reference<Type>::type>::type type;
            };
            /*!
             * \brief Convert incoming JSON data to another JSON data type
             * using a styx::cast, or to any other type that has a
             * styx::element constructor.
             */
            template<typename Json>
            typename std::enable_if<
                styx::is_json_type<typename base_type<Json>::type>::value,
                Json>::type
                    convert_json(const styx::element& e)
            {
                return styx::cast<Json>(e);
            }
            /*!
             * \brief Convert incoming JSON data to another JSON data type
             * using a styx::cast, or to any other type that has a
             * styx::element constructor.
             */
            template<typename Json>
            typename std::enable_if<
                !styx::is_json_type<typename base_type<Json>::type>::value,
                Json>::type
                    convert_json(const styx::element& e)
            {
                return (typename base_type<Json>::type)(e);
            }
            /*!
             * \brief Turn a synchronous URI router function into an
             * asynchronous one in order to make it compatible with the lower
             * level basic_function interface.
             */
            uri_type make_async(uri_function_type function);
            /*!
             * \brief Turn a synchronous URI router function into an
             * asynchronous one in order to make it compatible with the lower
             * level basic_function interface.
             */
            uri_type make_async_with_conn(conn_uri_function_type function);
            /*!
             * \brief Wrapper for a router function as stored by the router.
             */
            class basic_function
            {
            public:
                basic_function(uri_type, auth_function_type);

                /*!
                 * \brief Handle a request from the client for an API
                 * function.
                 *
                 * \note Is a function of type atlas::http::uri_type.
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
                auth_function_type m_auth_function;
            };

            /*!
             * Copy a string from a regex match result to a Fusion vector.
             */
            template<int Src, int Dest, typename Container>
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
                            Container, boost::mpl::int_<Dest>
                            >::type
                        >::type element_type;
                try
                {
                    if(match.size() > Src+1)
                    {
                        element_type e = boost::lexical_cast<element_type>(
                                match[Src+1]
                                );
                        boost::fusion::at_c<Dest>(container) = e;
                    }
                    else
                        boost::fusion::at_c<Dest>(container) = element_type();
                }
                catch(const boost::bad_lexical_cast& e)
                {
                    throw std::runtime_error(
                        hades::mkstr() << "casting uri argument: " << e.what()
                        );
                }
            }
            /*!
             */
            template<int SrcFrom, int SrcTo, int DestFrom, int DestTo>
            struct copy_to_vector
            {
                template<typename Container>
                void copy(const boost::smatch& list, Container& container)
                {
                    copy_string_to_vector<SrcFrom, DestFrom, Container>(list, container);
                    copy_to_vector<SrcFrom+1, SrcTo, DestFrom+1, DestTo>()
                        .template copy<Container>(list, container);
                };
            };
            template<int SrcFrom, int SrcTo, int Dest>
            struct copy_to_vector<SrcFrom, SrcTo, Dest, Dest>
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

                unwrapped_function(
                        unwrapped_function_type function,
                        auth_function_type auth_function
                        ) :
                    basic_function(
                        detail::make_async(
                            [function](boost::smatch match)
                            {
                                typedef boost::fusion::vector<Arguments...>
                                    arg_values_type;
                                arg_values_type arg_values;
                                copy_to_vector<0, sizeof...(Arguments), 0, sizeof...(Arguments)>()
                                    .copy(match, arg_values);

                                // Invoke the API function with the argument
                                // list.
                                http::response out =
                                    boost::fusion::invoke(function, arg_values);
                                return out;
                            }
                            ),
                            auth_function
                        )
                {
                }
            };

            get_parameters_type parse_get_parameters(mg_connection *);

            /*!
             * \brief A function accepting GET parameters.
             */
            template<typename ...Arguments>
            class get_function : public basic_function
            {
            public:
                typedef boost::function<http::response(get_parameters_type, Arguments...)>
                    unwrapped_function_type;
                typedef boost::function<
                    void(
                            get_parameters_type,
                            uri_success_callback_type,
                            uri_callback_type,
                            Arguments...
                            )
                    > unwrapped_async_function_type;

                get_function(
                        unwrapped_async_function_type function,
                        auth_function_type auth_function
                        ) :
                    basic_function(
                            [function](
                                mg_connection *mg_conn,
                                boost::smatch match,
                                uri_callback_type success,
                                uri_callback_type error
                                )
                            {
                                get_parameters_type params(
                                    detail::parse_get_parameters(mg_conn)
                                    );

                                typedef boost::fusion::vector<
                                    get_parameters_type,
                                    uri_success_callback_type,
                                    uri_callback_type,
                                    Arguments...
                                    > arg_values_type;
                                arg_values_type arg_values;
                                // Copy the first value (JSON data).
                                boost::fusion::at_c<0>(arg_values) = params;
                                // Copy the second and third values
                                // (success and error callbacks).
                                boost::fusion::at_c<1>(arg_values) =
                                    [mg_conn, success](atlas::http::response r) {
                                        // TODO send response
                                        // move this - possibly to http/server/response.hpp
                                        mg_send_status(mg_conn, r.status_code);
                                        for(std::pair<std::string, std::string> header : r.headers)
                                            mg_send_header(mg_conn, header.first.c_str(), header.second.c_str());
                                        mg_send_data(mg_conn, r.data.c_str(), r.data.length());
                                        success();
                                    };
                                boost::fusion::at_c<2>(arg_values) = error;
                                // Copy remaining arguments (URI placeholders).
                                copy_to_vector<
                                    0, sizeof...(Arguments),
                                    3, sizeof...(Arguments) + 3>()
                                    .copy(match, arg_values);

                                // Invoke the API function with the argument
                                // list.
                                boost::fusion::invoke(function, arg_values);
                            },
                            auth_function
                            )
                {
                }

                get_function(
                        unwrapped_function_type function,
                        auth_function_type auth_function
                        ) :
                    basic_function(
                        detail::make_async_with_conn(
                            [function](mg_connection *mg_conn, boost::smatch match)
                            {
                                get_parameters_type params(
                                    detail::parse_get_parameters(mg_conn)
                                    );

                                typedef boost::fusion::vector<get_parameters_type, Arguments...>
                                    arg_values_type;
                                arg_values_type arg_values;
                                boost::fusion::at_c<0>(arg_values) = params;
                                // Copy remaining arguments (URI placeholders).
                                copy_to_vector<
                                    0, sizeof...(Arguments),
                                    1, sizeof...(Arguments) + 1>()
                                    .copy(match, arg_values);

                                // Invoke the API function with the argument
                                // list.
                                http::response out =
                                    boost::fusion::invoke(function, arg_values);
                                return out;
                            }
                            ),
                            auth_function
                        )
                {
                }
            };

            /*!
             * \brief A function accepting POST or PUT data in addition to URI
             * parameters.
             */
            template<typename Json, typename ...Arguments>
            class json_function : public basic_function
            {
            public:
                typedef boost::function<http::response(Json, Arguments...)>
                    unwrapped_function_type;
                typedef boost::function<
                    void(Json, uri_success_callback_type, uri_callback_type, Arguments...)
                    > unwrapped_async_function_type;

                json_function(
                        unwrapped_async_function_type function,
                        auth_function_type auth_function
                        ) :
                    basic_function(
                            [function](
                                mg_connection *mg_conn,
                                boost::smatch match,
                                uri_callback_type success,
                                uri_callback_type error
                                )
                            {
                                std::string data(mg_conn->content, mg_conn->content_len);
                                styx::element element(styx::parse_json(data));

                                try
                                {
                                    // Convert the incoming data to the type
                                    // required for the handler function.
                                    Json json_data(detail::convert_json<Json>(element));

                                    typedef boost::fusion::vector<
                                        Json,
                                        uri_success_callback_type,
                                        uri_callback_type,
                                        Arguments...
                                        > arg_values_type;
                                    arg_values_type arg_values;
                                    // Copy the first value (JSON data).
                                    boost::fusion::at_c<0>(arg_values) = json_data;
                                    // Copy the second and third values
                                    // (success and error callbacks).
                                    boost::fusion::at_c<1>(arg_values) =
                                        [mg_conn, success](atlas::http::response r) {
                                            // TODO send response
                                            // move this - possibly to http/server/response.hpp
                                            mg_send_status(mg_conn, r.status_code);
                                            for(std::pair<std::string, std::string> header : r.headers)
                                                mg_send_header(mg_conn, header.first.c_str(), header.second.c_str());
                                            mg_send_data(mg_conn, r.data.c_str(), r.data.length());
                                            success();
                                        };
                                    boost::fusion::at_c<2>(arg_values) = error;
                                    // Copy remaining arguments (URI placeholders).
                                    copy_to_vector<
                                        0, sizeof...(Arguments),
                                        3, sizeof...(Arguments) + 3>()
                                        .copy(match, arg_values);

                                    // Invoke the API function with the argument
                                    // list.
                                    boost::fusion::invoke(function, arg_values);
                                }
                                catch(const boost::bad_get&)
                                {
                                    // The JSON input was the wrong type of
                                    // element (e.g. a list was expected but an
                                    // object received).
                                    error();
                                }
                            },
                            auth_function
                            )
                {
                }

                json_function(
                        unwrapped_function_type function,
                        auth_function_type auth_function
                        ) :
                    basic_function(
                        detail::make_async_with_conn(
                            [function](mg_connection *conn, boost::smatch match)
                            {
                                const std::string content(
                                    conn->content,
                                    conn->content_len
                                    );
                                styx::element element(
                                    styx::parse_json(content)
                                    );

                                try
                                {
                                    // Convert the incoming data to the type
                                    // required for the handler function.
                                    Json json_data(detail::convert_json<Json>(element));

                                    typedef boost::fusion::vector<Json, Arguments...>
                                        arg_values_type;
                                    arg_values_type arg_values;
                                    boost::fusion::at_c<0>(arg_values) = json_data;
                                    // Copy remaining arguments (URI placeholders).
                                    copy_to_vector<
                                        0, sizeof...(Arguments),
                                        1, sizeof...(Arguments) + 1>()
                                        .copy(match, arg_values);

                                    // Invoke the API function with the argument
                                    // list.
                                    http::response out =
                                        boost::fusion::invoke(function, arg_values);
                                    return out;
                                }
                                catch(const boost::bad_get&)
                                {
                                    // The JSON input was the wrong type of
                                    // element (e.g. a list was expected but an
                                    // object received).
                                    http::response out =
                                        json_error_response("bad get");
                                    return out;
                                }
                            }
                            ),
                            auth_function
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
             * \brief Route an HTTP request to a router function.
             */
            int operator()(mg_connection*, mg_event);
            /*!
             * \brief Handle an incoming connection, taking the first part of
             * the match as a URL.
             */
            void serve(
                    mg_connection*,
                    boost::smatch,
                    uri_callback_type success,
                    uri_callback_type failure
                    );
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
                matcher m,
                uri_type uri_function
                );
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
                matcher m,
                uri_type uri_function,
                auth_function_type auth_function
                );

            /*!
             * \brief Install a function to respond to a URI matched by a regular expression.
             *
             * \param function Method accepting the URI parameters and
             * returning a string.
             */
            template<typename ...Arguments>
            void install(
                matcher m,
                typename detail::unwrapped_function<Arguments...>::unwrapped_function_type function
                )
            {
                if(m_functions.count(m))
                    throw std::runtime_error(
                        hades::mkstr() <<
                        "uri handler already registered (" << m.regex() << ")"
                        );
                m_functions.insert(
                    m,
                    static_cast<detail::basic_function*>(
                        new detail::unwrapped_function<Arguments...>(
                            function,
                            [](const auth::token_type&) { return true; }
                            )
                        )
                    );
            }
            /*!
             * \brief Install a function to respond to a URI matched by a regular expression.
             *
             * \param function Method accepting the URI parameters and
             * returning a string.
             */
            template<typename ...Arguments>
            void install(
                matcher m,
                typename detail::unwrapped_function<Arguments...>::unwrapped_function_type function,
                auth_function_type auth_function
                )
            {
                if(m_functions.count(m))
                    throw std::runtime_error(
                        hades::mkstr() <<
                        "uri handler already registered (" << m.regex() << ")"
                        );
                m_functions.insert(
                    m,
                    static_cast<detail::basic_function*>(
                        new detail::unwrapped_function<Arguments...>(function, auth_function)
                        )
                    );
            }

            /*!
             * \brief Install a function to respond to a URI matched by a
             * regular expression.
             *
             * \param function Method accepting the URI parameters and
             * returning a string.
             */
            template<typename ...Arguments>
            void install_get(
                matcher m,
                typename detail::get_function<Arguments...>
                    ::unwrapped_function_type function
                )
            {
                if(m_functions.count(m))
                    throw std::runtime_error(
                        hades::mkstr() <<
                        "uri handler already registered (" << m.regex() << ")"
                        );
                m_functions.insert(
                    m,
                    static_cast<detail::basic_function*>(
                        new detail::get_function<Arguments...>(
                            function,
                            [](const auth::token_type&) { return true; }
                            )
                        )
                    );
            }
            /*!
             * \brief Install a function to respond to a URI matched by a
             * regular expression.
             *
             * \param function Method accepting the URI parameters and
             * returning a string.
             */
            template<typename ...Arguments>
            void install_get(
                matcher m,
                typename detail::get_function<Arguments...>
                    ::unwrapped_function_type function,
                auth_function_type auth_function
                )
            {
                if(m_functions.count(m))
                    throw std::runtime_error(
                        hades::mkstr() <<
                        "uri handler already registered (" << m.regex() << ")"
                        );
                m_functions.insert(
                    m,
                    new detail::get_function<Arguments...>(function, auth_function)
                    );
            }
            /*!
             * \brief Install a function to respond to a URI matched by a regular expression.
             *
             * \param function Method accepting the URI parameters and
             * returning a string.
             */
            template<typename ...Arguments>
            void install_json(
                matcher m,
                typename detail::json_function<Arguments...>::unwrapped_function_type function
                )
            {
                if(m_functions.count(m))
                    throw std::runtime_error(
                        hades::mkstr() <<
                        "uri handler already registered (" << m.regex() << ")"
                        );
                m_functions.insert(
                    m,
                    static_cast<detail::basic_function*>(
                        new detail::json_function<Arguments...>(
                            function,
                            [](const auth::token_type&) { return true; }
                            )
                        )
                    );
            }
            /*!
             * \brief Install a function to respond to a URI matched by a regular expression.
             *
             * \param function Method accepting the URI parameters and
             * returning a string.
             */
            template<typename Json, typename ...Arguments>
            void install_json(
                matcher m,
                typename detail::json_function<Json, Arguments...>
                    ::unwrapped_function_type function,
                auth_function_type auth_function
                )
            {
                if(m_functions.count(m))
                    throw std::runtime_error(
                        hades::mkstr() <<
                        "uri handler already registered (" << m.regex() << ")"
                        );
                m_functions.insert(
                    m,
                    new detail::json_function<Json, Arguments...>(function, auth_function)
                    );
            }
            /*!
             * \brief Install an asynchronous function accepting JSON-formatted
             * POST data.
             *
             * \param function Method accepting JSON data, success and failure
             * callbacks and URI parameters.
             */
            template<typename Json, typename ...Arguments>
            void install_json_async(
                matcher m,
                typename detail::json_function<Json, Arguments...>
                    ::unwrapped_async_function_type function
                )
            {
                if(m_functions.count(m))
                    throw std::runtime_error(
                        hades::mkstr() <<
                        "uri handler already registered (" << m.regex() << ")"
                        );
                m_functions.insert(
                    m,
                    new detail::json_function<Json, Arguments...>(
                        function,
                        [](const auth::token_type&) { return true; }
                        )
                    );
            }
            /*!
             * \brief Install an asynchronous function accepting JSON-formatted
             * POST data.
             *
             * \param function Method accepting JSON data, success and failure
             * callbacks and URI parameters.
             */
            template<typename Json, typename ...Arguments>
            void install_json_async(
                matcher m,
                typename detail::json_function<Json, Arguments...>
                    ::unwrapped_async_function_type function,
                auth_function_type auth_function
                )
            {
                if(m_functions.count(m))
                    throw std::runtime_error(
                        hades::mkstr() <<
                        "uri handler already registered (" << m.regex() << ")"
                        );
                m_functions.insert(
                    m,
                    new detail::json_function<Json, Arguments...>(function, auth_function)
                    );
            }
        private:
            boost::ptr_map<matcher, detail::basic_function> m_functions;
        };
    }
}

#endif

