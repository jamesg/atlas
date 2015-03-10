#include "atlas/http/server/mimetypes.hpp"

#include <fstream>
#include <ios>

#include <boost/tokenizer.hpp>

#include "atlas/log/log.hpp"

atlas::http::mimetypes::mimetypes()
{
    std::ifstream etc_mime_types("/etc/mime.types", std::ios::in);
    if(!etc_mime_types.is_open())
        throw std::runtime_error("unable to open /etc/mime.types for reading");

    std::string line;
    while(std::getline(etc_mime_types, line))
    {
        typedef boost::tokenizer<boost::char_separator<char> > tokenizer;
        tokenizer t(line, boost::char_separator<char>("\t "));

        tokenizer::iterator iter = t.begin();
        if(iter != t.end())
        {
            std::string mimetype = *iter;
            ++iter;
            for(; iter != t.end(); ++iter)
                m_mimetypes[*iter] = mimetype;
        }
    }

    log::information("http::mimetypes") << "installed " <<
        m_mimetypes.size() << " mimetypes";
}

std::string atlas::http::mimetypes::content_type(
        const std::string& extension
        ) const
{
    std::map<std::string, std::string>::const_iterator i = m_mimetypes.find(extension);
    return (i == m_mimetypes.cend())?
        "text/plain":
        i->second;
}

