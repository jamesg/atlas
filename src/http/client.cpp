#include "client.hpp"

#include "hades/mkstr.hpp"

#include "log/log.hpp"

namespace
{
    int write_function(char *data, size_t size, size_t n_memb, std::string *str)
    {
        if(data == nullptr)
            return 0;
        str->append(data, size*n_memb);
        return size*n_memb;
    }
}

atlas::http::client::client(
        io_type callback_io,
        const std::string& uri,
        const callback_type& success,
        const callback_type& failure
        ) :
    m_callback_io(callback_io),
    m_uri(uri),
    m_success(success),
    m_failure(failure)
{
    m_work.reset(new boost::asio::io_service::work(*m_callback_io));
    m_curl = curl_easy_init();
    if(m_curl == nullptr)
        throw std::runtime_error("initialising curl handle");
    // Null-terminated string.
    m_curl_error_buf[0] = '\0';
    // CURL installs a signal handler to be used when DNS lookups fail.  This
    // does not work on all platforms, so disable signals in CURL.
    curl_easy_setopt(m_curl, CURLOPT_NOSIGNAL, 1);
    curl_easy_setopt(m_curl, CURLOPT_URL, uri.c_str());
    curl_easy_setopt(m_curl, CURLOPT_ERRORBUFFER, &m_curl_error_buf);
}

atlas::http::client::~client()
{
    curl_easy_cleanup(m_curl);
}

atlas::http::ptr_type atlas::http::client::create(
        io_type callback_io,
        const std::string& uri,
        const callback_type& success,
        const callback_type& failure
        )
{
    ptr_type out(new client(callback_io, uri, success, failure));

    return out;
}

void atlas::http::client::get()
{
    curl_easy_setopt(m_curl, CURLOPT_WRITEDATA, (void*)(&m_body));
    curl_easy_setopt(m_curl, CURLOPT_WRITEFUNCTION, &write_function);
    go();
}

void atlas::http::client::post(const std::string& body)
{
    m_send = body;
    curl_easy_setopt(m_curl, CURLOPT_POSTFIELDS, m_send.c_str());
    curl_easy_setopt(m_curl, CURLOPT_WRITEDATA, (void*)(&m_body));
    curl_easy_setopt(m_curl, CURLOPT_WRITEFUNCTION, &write_function);
    curl_easy_setopt(m_curl, CURLOPT_TIMEOUT, 2);
    go();
}

void atlas::http::client::go()
{
    auto result = curl_easy_perform(m_curl);
    if(result == CURLE_OK)
        m_callback_io->post(
                boost::bind(
                    &client::success,
                    shared_from_this(),
                    m_body
                    )
                );
    else
        m_callback_io->post(
                boost::bind(
                    &client::failure,
                    shared_from_this(),
                    (hades::mkstr() << "CURL error: " <<
                        std::string(m_curl_error_buf)).str()
                    )
                );
    m_work.reset();
}

void atlas::http::client::success(const std::string& str)
{
    m_success(str);
    m_work.reset();
}

void atlas::http::client::failure(const std::string& str)
{
    m_failure(str);
    m_work.reset();
}

void atlas::http::get(
        io_type callback_io,
        const std::string& uri,
        const callback_type& success,
        const callback_type& failure
        )
{
    // Create the client outside of the thread so that the client can start its
    // work before this function returns.
    ptr_type c = client::create(callback_io, uri, success, failure);
    boost::thread(
        [c, callback_io, uri, success, failure]() {
            c->get();
        }
        );
}

void atlas::http::post(
        io_type callback_io,
        const std::string& uri,
        const std::string& body,
        const callback_type& success,
        const callback_type& failure
        )
{
    boost::thread(
        [callback_io, uri, body, success, failure]() {
            ptr_type c = client::create(callback_io, uri, success, failure);
            c->post(body);
        }
        );
}

