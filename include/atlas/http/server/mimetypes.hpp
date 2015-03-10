#ifndef ATLAS_HTTP_MIMETYPES_HPP
#define ATLAS_HTTP_MIMETYPES_HPP

#include <map>
#include <string>

namespace atlas
{
    namespace http
    {
        /*!
         * \brief Cache information on mime types acquired from the operating system.
         *
         * On Linux, MIME information is acquired from /etc/mime.types.
         */
        class mimetypes
        {
            public:
                mimetypes();

                /*!
                 * \brief Get the value of the HTTP Content-Type header
                 * appropriate for the file extension.
                 */
                std::string content_type(const std::string& extension) const;

            private:
                std::map<std::string, std::string> m_mimetypes;
        };
    }
}

#endif

