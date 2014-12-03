#ifndef ATLAS_API_SERVER_HPP
#define ATLAS_API_SERVER_HPP

#include <map>
#include <type_traits>

#include <boost/function.hpp>
#include <boost/fusion/include/at_c.hpp>
#include <boost/fusion/include/invoke.hpp>
#include <boost/fusion/include/vector.hpp>
#include <boost/mpl/for_each.hpp>
#include <boost/mpl/range_c.hpp>
#include <boost/ptr_container/ptr_map.hpp>

#include "hades/mkstr.hpp"
#include "styx/cast.hpp"
#include "styx/get_default.hpp"
#include "styx/styx.hpp"

#include "api/auth_function_type.hpp"
#include "api/exception.hpp"
#include "api/method_type.hpp"
#include "jsonrpc/request.hpp"
#include "jsonrpc/result.hpp"
#include "log/log.hpp"

namespace atlas
{
    namespace jsonrpc
    {
        struct request;
        struct result;
    }

    namespace api
    {
        namespace detail
        {
            /*!
             * \brief Wrap a synchronous API function to appear as a
             * synchronous one.
             */
            async_method_type make_async(method_type method);
            /*!
             * \brief Wrapper for a JSONRPC method as stored by the API server.
             */
            class basic_method
            {
                public:
                    basic_method(async_method_type, auth_function_type);

                    /*!
                     * \brief Handle a request from the client for an API
                     * function.
                     *
                     * \param request The JSONRPC request object.
                     * \param callback Function that will return the result to
                     * the client.
                     *
                     * \note First checks that the client is authorised using
                     * the check_auth function.
                     *
                     * \throws None.
                     */
                    void serve(
                            jsonrpc::request& request,
                            boost::function<void(jsonrpc::result)> callback
                            ) const;

                private:
                    async_method_type m_serve;
                    auth_function_type m_check_auth;
            };

            /*!
             * Copy a single element from a JSON list to a vector.
             */
            template<int Index, typename Container>
            void copy_element_to_vector(const styx::list& list, Container& container)
            {
                // Type of the element at index 'Index' in the Container.
                typedef
                    typename std::remove_reference<
                        typename boost::fusion::result_of::at<
                            Container, boost::mpl::int_<Index>
                            >::type
                        >::type json_type;
                try
                {
                    if(list.size() > Index)
                    {
                        styx::element o = list.at(Index);
                        boost::fusion::at_c<Index>(container) =
                            styx::cast<json_type>(o);
                    }
                    else
                        boost::fusion::at_c<Index>(container) =
                            styx::get_default<json_type>();
                }
                catch(const std::exception& e)
                {
                    throw std::runtime_error(hades::mkstr() << "check_auth exception: " << e.what());
                }
            }
            /*!
             * \brief Copy all elements in a styx::list to a
             * boost::fusion::vector of styx::element.  If the list is too long
             * for the vector, only size(vector) elements will be copied.  If
             * the vector is too long for the list, additional elements are set
             * to styx::null_t().
             */
            template<int from, int to>
            struct copy_list_to_vector
            {
                template<typename Container>
                void copy(const styx::list& list, Container& container)
                {
                    copy_element_to_vector<from, Container>(list, container);
                    copy_list_to_vector<from+1, to>().template copy<Container>(list, container);
                };
            };
            template<int to>
            struct copy_list_to_vector<to, to>
            {
                template<typename Container>
                void copy(const styx::list& list, Container& container)
                {
                };
            };

            /*!
             * \brief Unwrap a JSONRPC parameter list and supply individual
             * parameters as arguments to a function.
             */
            template<typename Return, typename ...Arguments>
            class unwrapped_method : public basic_method
            {
            public:
                typedef boost::function<Return(Arguments...)>
                    unwrapped_method_type;
                /*!
                 * \brief Check that the user making the request is
                 * authorised to make the request.  The entire request is
                 * supplied because some relevant information is outside of
                 * the parameters structure (e.g. the HTTP Authorisation
                 * header).
                 */
                typedef boost::function<
                    bool(jsonrpc::request, Arguments...)
                    > unwrapped_auth_type;

                unwrapped_method(unwrapped_method_type method, unwrapped_auth_type check_auth) :
                    basic_method(
                        detail::make_async(
                            [method](
                                jsonrpc::request& request,
                                jsonrpc::result& result
                                )
                            {
                                typedef boost::fusion::vector<Arguments...> arg_values_type;
                                arg_values_type arg_values;
                                copy_list_to_vector<0, sizeof...(Arguments)>()
                                    .copy(request.params(), arg_values);

                                // Invoke the API function with the argument list.
                                Return out = boost::fusion::invoke(method, arg_values);
                                result.data() = out;
                            }
                            ),
                            [check_auth](
                                jsonrpc::request& request
                                ) -> bool
                            {
                                styx::list& l = request.params();
                                typedef boost::fusion::vector<Arguments...> arg_values_type;
                                arg_values_type arg_values;
                                copy_list_to_vector<0, sizeof...(Arguments)>()
                                    .copy(l, arg_values);

                                // Append the JSONRPC request to the argument list.
                                auto args = boost::fusion::push_front(arg_values, request);
                                // Invoke the check_auth function with the argument list.
                                return boost::fusion::invoke(
                                    check_auth,
                                    args
                                    );
                            }
                        )
                {
                }
            };
        }

        class server
        {
            public:
                server();

                /*!
                 * \brief Install an asynchronous JSONRPC method on the server.
                 */
                void install_async(std::string name, async_method_type method);
                /*!
                 * \brief Install a JSONRPC method with a client authentication
                 * service on the server.
                 */
                void install(
                    std::string,
                    method_type,
                    auth_function_type
                    );
                /*!
                 * \brief Install a JSONRPC method on the server.  The params
                 * element in the request should be an array.  Values in the
                 * array are supplied to the API function.  If too few
                 * parameters are provided in the request, remaining parameters
                 * are filled in with 'null'.
                 *
                 * \note The API function can be used by any client, even if
                 * they have not authenticated.
                 */
                template<typename Return, typename ...Arguments>
                void install(
                    std::string name,
                    typename detail::unwrapped_method<Return, Arguments...>::unwrapped_method_type method
                    )
                {
                    log::information("api::server::install") << "installing " << name;
                    m_methods.insert(
                        name,
                        static_cast<detail::basic_method*>(
                            new detail::unwrapped_method<Return, Arguments...>(
                                method,
                                [](jsonrpc::request&, Arguments...) { return true; }
                                )
                            )
                        );
                }
                /*!
                 * \brief Install a JSONRPC method on the server.  The params
                 * element in the request should be an array.  Values in the
                 * array are supplied to the API function.  If too few
                 * parameters are provided in the request, remaining parameters
                 * are filled in with 'null'.
                 */
                template<typename Return, typename ...Arguments>
                void install(
                    std::string name,
                    typename detail::unwrapped_method<Return, Arguments...>::unwrapped_method_type method,
                    typename detail::unwrapped_method<Return, Arguments...>::unwrapped_auth_type auth_function
                    )
                {
                    log::information("api::server::install") << "installing " << name;
                    m_methods.insert(
                        name,
                        static_cast<detail::basic_method*>(
                            new detail::unwrapped_method<Return, Arguments...>(
                                method,
                                auth_function
                                )
                            )
                        );
                }
                /*!
                 * \brief Handle a JSONRPC request (calls one of the installed
                 * methods or returns an error).
                 */
                void serve(
                        jsonrpc::request&,
                        boost::function<void(jsonrpc::result&)>
                        ) const;
            private:
                boost::ptr_map<std::string, detail::basic_method> m_methods;
        };
    }
}

#endif

