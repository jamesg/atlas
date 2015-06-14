#ifndef ATLAS_HTTP_SERVER_EXCEPTION_HPP
#define ATLAS_HTTP_SERVER_EXCEPTION_HPP

#include <stdexcept>

namespace atlas
{
    namespace http
    {
        class exception : public std::runtime_error
        {
            public:
                exception(const std::string& what_, const int code=500) :
                    std::runtime_error(what_),
                    m_code(code)
                {
                }

                int code() const
                {
                    return m_code;
                }
            private:
                const int m_code;
        };
    }
}

#endif

