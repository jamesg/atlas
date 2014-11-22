#ifndef ATLAS_DB_COMMS_HPP
#define ATLAS_DB_COMMS_HPP

namespace atlas
{
    namespace db
    {
        template<const char *GetMethod>
        struct has_get_method
        {
            static constexpr const char * const get_method = GetMethod;
        };
        template<const char *SaveMethod>
        struct has_save_method
        {
            static constexpr const char * const save_method = SaveMethod;
        };
        template<const char *SaveListMethod>
        struct has_save_list_method
        {
            static constexpr const char * const save_list_method =
                SaveListMethod;
        };
        template<const char *ListMethod>
        struct has_list_method
        {
            static constexpr const char * const list_method = ListMethod;
        };
    }
}

#endif

