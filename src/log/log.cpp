#include "atlas/log/log.hpp"

#include <iostream>

#include <boost/date_time/posix_time/posix_time.hpp>

namespace
{
    static atlas::log::detail::cerr_sink g_default_sink;
}

atlas::log::detail::basic_sink& atlas::log::detail::get_default_sink()
{
    return g_default_sink;
}

void atlas::log::detail::cerr_sink::log(const std::string& s)
{
    std::cerr << s << std::endl;
}

atlas::log::detail::file_sink::file_sink(const char *filename) :
    m_fstream(filename, std::ios::app)
{

}

void atlas::log::detail::file_sink::log(const std::string& s)
{
    m_fstream << s << std::endl;
}


atlas::log::detail::basic_log::basic_log(
        const char *source,
        const char *severity,
        basic_sink& sink
        ) :
    m_sink(sink)
{
    m_oss <<
        boost::posix_time::to_iso_string(
            boost::posix_time::second_clock::universal_time()
        ) << " " <<
        severity << " " << source << " ";
}

atlas::log::detail::basic_log::~basic_log()
{
    m_sink.log(m_oss.str());
}

atlas::log::detail::basic_sink& atlas::log::detail::basic_log::sink()
{
    return m_sink;
}

atlas::log::error::error(const char *source) :
    basic_log(source, "ERR")
{
}

atlas::log::information::information(const char *source) :
    basic_log(source, "INFO")
{
}

atlas::log::test::test(const char *source) :
    basic_log(source, "TEST")
{
}

atlas::log::warning::warning(const char *source) :
    basic_log(source, "WARN")
{
}
