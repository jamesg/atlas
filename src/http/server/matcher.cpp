#include "atlas/http/server/matcher.hpp"

#include <boost/algorithm/string.hpp>

atlas::http::matcher::matcher(const char *regex) :
    m_regex(regex),
    m_method("")
{
}

atlas::http::matcher::matcher(const std::string& regex) :
    m_regex(regex),
    m_method("")
{
}

atlas::http::matcher::matcher(
        const std::string& regex,
        const std::string& method
        ) :
    m_regex(regex),
    m_method(method)
{
    boost::algorithm::to_lower(m_method);
}

bool atlas::http::matcher::matches(
        const std::string& uri,
        const std::string& method,
        boost::smatch& match
        ) const
{
    std::string m(method);
    boost::algorithm::to_lower(m);
    if(m_method != "" && m_method != m)
        return false;
    return boost::regex_match(uri, match, boost::regex(m_regex));
}

const std::string& atlas::http::matcher::regex() const
{
    return m_regex;
}

const std::string& atlas::http::matcher::method() const
{
    return m_method;
}

bool atlas::http::matcher::operator<(const matcher& o) const
{
    return m_regex < o.m_regex ||
        (m_regex == o.m_regex && m_method < o.m_method);
}

bool atlas::http::matcher::operator==(const matcher& o) const
{
    return m_regex == o.m_regex && m_method == o.m_method;
}

