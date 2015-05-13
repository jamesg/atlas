#ifndef ATLAS_HTTP_SERVER_STATIC_STRING_HPP
#define ATLAS_HTTP_SERVER_STATIC_STRING_HPP

#include <string>

#define ATLAS_DECLARE_STATIC_STRING(PREFIX, NAME) \
    extern "C" { \
        extern char PREFIX##_binary_##NAME##_start; \
        extern char PREFIX##_binary_##NAME##_end; \
        extern size_t PREFIX##_binary_##NAME##_size; \
    }

#define ATLAS_STATIC_STD_STRING(PREFIX, NAME) \
    std::string(&PREFIX##_binary_##NAME##_start, &PREFIX##_binary_##NAME##_end)

#endif

