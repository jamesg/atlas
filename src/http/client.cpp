#include "client.hpp"

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

atlas::http::client::ptr_type atlas::http::client::create(
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
    curl_easy_setopt(m_curl, CURLOPT_POSTFIELDS, body.c_str());
    curl_easy_setopt(m_curl, CURLOPT_WRITEDATA, (void*)(&m_body));
    curl_easy_setopt(m_curl, CURLOPT_WRITEFUNCTION, &write_function);
    curl_easy_setopt(m_curl, CURLOPT_TIMEOUT, 2);
    go();
}

void atlas::http::client::process()
{
    if(curl_easy_perform(m_curl) == CURLE_OK)
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
                    std::string(m_curl_error_buf)
                    )
                );
}

void atlas::http::client::go()
{
    m_work.reset(new boost::asio::io_service::work(*m_callback_io));
    m_thread = boost::thread(boost::bind(&client::process, shared_from_this()));
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
    m_curl = curl_easy_init();
    if(m_curl == nullptr)
        throw std::runtime_error("initialising curl handle");
    curl_easy_setopt(m_curl, CURLOPT_URL, uri.c_str());
    curl_easy_setopt(m_curl, CURLOPT_ERRORBUFFER, &m_curl_error_buf);
}

atlas::http::client::~client()
{
    curl_easy_cleanup(m_curl);
    log::information("http client") << "destroy";
}

void atlas::http::client::success(const std::string& str)
{
    log::information("http client") << "success";
    m_success(str);
    m_work.reset();
}

void atlas::http::client::failure(const std::string& str)
{
    m_failure(str);
    m_work.reset();
}

