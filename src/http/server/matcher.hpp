#ifndef ATLAS_HTTP_SERVER_MATCHER_HPP
#define ATLAS_HTTP_SERVER_MATCHER_HPP

#include <boost/regex.hpp>

namespace atlas
{
    namespace http
    {
        class matcher
        {
        public:
            /*!
             * \brief Match GET requests to a URI matching a regular expression.
             *
             * \todo Consider making this constructor explicit.
             */
            matcher(const char *regex);
            /*!
             * \brief Match GET requests to a URI matching a regular expression.
             *
             * \todo Consider making this constructor explicit.
             */
            matcher(const std::string& regex);
            /*!
             * \brief Match requests of type 'method' to a URI matching a
             * regular expression.
             */
            matcher(const std::string& regex, const std::string& method);

            bool matches(
                    const std::string& uri,
                    const std::string& method,
                    boost::smatch& match
                    ) const;

            bool operator<(const matcher& o) const;
            bool operator==(const matcher& o) const;

        private:
            const std::string m_regex;
            std::string m_method;
        };
    }
}

#endif

