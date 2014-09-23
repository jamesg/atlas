#ifndef ATLAS_HTTP_CLIENT_HPP
#define ATLAS_HTTP_CLIENT_HPP

#include <boost/asio.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/function.hpp>
#include <boost/noncopyable.hpp>
#include <boost/range.hpp>
#include <boost/thread.hpp>
#include <curl/curl.h>

namespace atlas
{
    namespace http
    {
        class client :
            public boost::noncopyable,
            public boost::enable_shared_from_this<client>
        {
            public:
                typedef boost::shared_ptr<boost::asio::io_service> io_type;
                typedef boost::shared_ptr<http::client> ptr_type;
                typedef boost::function<void(const std::string&)> callback_type;

                /*!
                 * \brief Prepare a HTTP request.
                 *
                 * \param io io_service which should be used to call the
                 * success and failure callbacks and destroy the request
                 * object.  This must not be the io_service that the cpp-netlib
                 * request is operating in.  The io_service will not complete
                 * until the HTTP request has completed.
                 * \param uri URI to query, including protocol, domain, path
                 * and port (if not the default).
                 * \param success Function to call with the body of the
                 * response upon success.
                 * \param failure Function to call with an error message upon
                 * failure.
                 */
                static ptr_type create(
                        io_type io,
                        const std::string& uri,
                        const callback_type& success,
                        const callback_type& failure
                        );
                ~client();

                /*!
                 * \brief Start making a GET request.  The success or failure
                 * callback will be called when the request is complete.
                 */
                void get();
                /*!
                 * \brief Start making a POST request.  The success or failure
                 * callback will be called when the request is complete.
                 */
                void post(const std::string& body);

            private:
                client(
                        io_type callback_io,
                        const std::string& uri,
                        const callback_type& success,
                        const callback_type& failure
                        );

                void process();
                void go();
                //void data_received(
                        //boost::iterator_range<const char*> const& response,
                        //const boost::system::error_code& errc
                        //);

                /*!
                 * \brief To be called from the callback io_service when the
                 * request has succeeded.
                 */
                void success(const std::string&);
                /*!
                 * \brief To be called from the callback io_service when the
                 * request has failed.
                 */
                void failure(const std::string&);

                io_type m_callback_io;
                std::string m_uri;
                //std::unique_ptr<detail::client_impl> m_impl;
                std::string m_body;
                boost::scoped_ptr<boost::asio::io_service::work> m_work;
                callback_type m_success, m_failure;
                CURL *m_curl;
                char m_curl_error_buf[CURL_ERROR_SIZE];
                boost::thread m_thread;
        };
    }
}

#endif

