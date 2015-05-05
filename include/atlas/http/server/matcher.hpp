#ifndef ATLAS_HTTP_SERVER_MATCHER_HPP
#define ATLAS_HTTP_SERVER_MATCHER_HPP

#include <boost/regex.hpp>

namespace atlas
{
    namespace http
    {
        /*!
         * \brief Matches incoming request data (URI and method) and extracts
         * URI parameters.
         */
        class matcher
        {
        public:
            /*!
             * \brief Match all requests to a URI matching a regular expression.
             *
             * \todo Consider making this constructor explicit.
             */
            matcher(const char *regex);
            /*!
             * \brief Match all requests to a URI matching a regular
             * expression, regardless of method.
             *
             * \param priority Matcher priority.  Matchers with lower numbers
             * are tried first.  The default priority is 0.
             */
            matcher(const std::string& regex, int priority=0);
            /*!
             * \brief Match requests of type 'method' to a URI matching a
             * regular expression.
             *
             * \param priority Matcher priority.  Matchers with lower numbers
             * are tried first.  The default priority is 0.
             */
            matcher(const std::string& regex, const std::string& method, int priority=0);

            /*!
             * \brief Determine whether or not the URI matches this object.
             */
            bool matches(
                    const std::string& uri,
                    const std::string& method,
                    boost::smatch& match
                    ) const;

            const std::string& regex() const;
            const std::string& method() const;

            bool operator<(const matcher& o) const;
            bool operator==(const matcher& o) const;

        private:
            const std::string m_regex;
            /*!
             * \brief HTTP method to match.  The empty string matches any
             * method.
             */
            std::string m_method;
            /*!
             * \brief Priority of this matcher, used in case more than one
             * matcher installed on a router can match a URI.  Lower numbers
             * mean this matcher will be tried first.  The default priority is
             * 0.
             */
            const int m_priority;
        };
    }
}

#endif

