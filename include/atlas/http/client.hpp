#ifndef ATLAS_HTTP_CLIENT_HPP
#define ATLAS_HTTP_CLIENT_HPP

#include <boost/asio.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/function.hpp>
#include <boost/noncopyable.hpp>
#include <boost/range.hpp>
#include <boost/thread.hpp>
#include <boost/scoped_ptr.hpp>
#include <curl/curl.h>

#include "styx/element.hpp"

namespace atlas
{
    namespace http
    {
        class client;

        typedef boost::shared_ptr<boost::asio::io_service> io_type;
        typedef boost::shared_ptr<http::client> ptr_type;
        typedef boost::function<void(const std::string&)> callback_type;

        /*!
         * \brief HTTP client for managing a single HTTP request.
         *
         * \internal
         * \note Constructing a client instantiates a Boost ASIO work object
         * which will prevent the connected IO service finishing until the
         * request is complete.
         */
        class client :
            public boost::noncopyable,
            public boost::enable_shared_from_this<client>
        {
            public:

                /*!
                 * \brief Prepare a HTTP request.
                 *
                 * \note A Boost ASIO work object will be instantiated which
                 * will prevent the IO service stopping until the request is
                 * complete.
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

                void go();

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
                // Keep a local copy of data to send (required by CURL).
                std::string m_send;
                // Data received over HTTP.
                std::string m_body;
                // Work to stop the callback io_service finishing until the
                // requrest has finidhed.
                boost::scoped_ptr<boost::asio::io_service::work> m_work;
                // Callbacks for success, failure.
                callback_type m_success, m_failure;
                CURL *m_curl;
                char m_curl_error_buf[CURL_ERROR_SIZE];
        };

        /*!
         * \brief Start making a GET request.  The success or failure callback
         * will be called when the request is complete.
         *
         * \param callback_io io_service which should be used to call the
         * success and failure callbacks and destroy the request object.  This
         * must not be the io_service that the cpp-netlib request is operating
         * in.  The io_service will not complete until the HTTP request has
         * completed.
         * \param uri URI to query, including protocol, domain, path and port
         * (if not the default).
         * \param success Function to call with the body of the response upon
         * success.
         * \param failure Function to call with an error message upon failure.
         */
        void get(
                io_type callback_io,
                const std::string& uri,
                const callback_type& success,
                const callback_type& failure
                );
        /*!
         * \brief Make an HTTP GET request and parse the resulting data as JSON.
         *
         * \param callback_io io_service which should be used to call the
         * success and failure callbacks and destroy the request object.  This
         * must not be the io_service that the cpp-netlib request is operating
         * in.  The io_service will not complete until the HTTP request has
         * completed.
         * \param uri URI to query, including protocol, domain, path and port
         * (if not the default).
         * \param success Function to call with the decoded JSON data upon
         * success.
         * \param failure Function to call with an error message upon failure.
         */
        void get_json(
                io_type callback_io,
                const std::string& url,
                const boost::function<void(styx::element)>& success,
                const callback_type& failure
                );
        /*!
         * \brief Start making a POST request.  The success or failure callback
         * will be called when the request is complete.
         *
         * \param callback_io io_service which should be used to call the
         * success and failure callbacks and destroy the request object.  This
         * must not be the io_service that the cpp-netlib request is operating
         * in.  The io_service will not complete until the HTTP request has
         * completed.
         * \param uri URI to query, including protocol, domain, path and port
         * (if not the default).
         * \param body HTTP POST data to be sent in the request.
         * \param success Function to call with the body of the response upon
         * success.
         * \param failure Function to call with an error message upon failure.
         */
        void post(
                io_type callback_io,
                const std::string& uri,
                const std::string& body,
                const callback_type& success,
                const callback_type& failure
                );
    }
}

#endif
