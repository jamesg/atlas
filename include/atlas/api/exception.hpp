#ifndef ATLAS_API_EXCEPTION_HPP
#define ATLAS_API_EXCEPTION_HPP

#include <stdexcept>

namespace atlas
{
    namespace api
    {
        class exception : public std::runtime_error
        {
            public:
                exception(const std::string& what_) :
                    std::runtime_error(what_)
                {
                }
        };
    }
}

#endif

