
#include "dense_enum_defn.h"

#undef ARUBA_DENSE_ENUM_DEF_START
#undef ARUBA_DENSE_ENUM_DEF_END
#undef ARUBA_DENSE_ENUM_ENTRY
#undef ARUBA_DENSE_ENUM_ENTRY_W_VAL
#undef ARUBA_DENSE_ENUM_ENTRY_USRSTR
#undef ARUBA_DENSE_ENUM_ENTRY_W_VAL_N_USRSTR

#ifdef BUILD_ENUM_STR_DEFN
#define ARUBA_DENSE_ENUM_DEF_START(__type, __max_entry)             \
    ar_dense_enum_str_entry_t __type##_str_defn[__max_entry + 1] = {
#else
#define ARUBA_DENSE_ENUM_DEF_START(__type, __max_entry) \
    typedef enum __type {
#endif

#ifdef BUILD_ENUM_STR_DEFN
#define ARUBA_DENSE_ENUM_ENTRY(__type, __entry)    \
    [__entry] = { .str = #__entry },
#define ARUBA_DENSE_ENUM_ENTRY_W_VAL(__type, __entry, __val)    \
    [__entry] = { .str = #__entry },
#define ARUBA_DENSE_ENUM_ENTRY_USRSTR(__type, __entry, __str)  \
    [__entry] = { .str = __str },
#define ARUBA_DENSE_ENUM_ENTRY_W_VAL_N_USRSTR(__type, __entry, __val, __str) \
    [__entry] = { .str = __str },
#else
#define ARUBA_DENSE_ENUM_ENTRY(__type, __entry) \
    __entry,                                                            
#define ARUBA_DENSE_ENUM_ENTRY_W_VAL(__type, __entry, __val)    \
    __entry = __val,                                     
#define ARUBA_DENSE_ENUM_ENTRY_USRSTR(__type, __entry, __str)  \
    __entry,
#define ARUBA_DENSE_ENUM_ENTRY_W_VAL_N_USRSTR(__type, __entry, __val, __str) \
    __entry = __val,
#endif

#ifdef BUILD_ENUM_STR_DEFN
#define ARUBA_DENSE_ENUM_DEF_END(__type, __max_entry,                   \
                                 __default_str, __prefixstr)            \
    };                                                                  \
    ar_dense_enum_str_array_t __type##_array =                          \
    {                                                                   \
        .prefixlen = (sizeof(__prefixstr)/sizeof(char) - 1),            \
        .num_entries = __max_entry,                                     \
        .str_entries = __type##_str_defn                                \
    };                                                                  \
    const char *__type##_get_str(int enum_type) {                       \
        if (enum_type < __type##_array.num_entries)                     \
            return (__type##_str_defn[enum_type].str+                   \
                    __type##_array.prefixlen);                          \
        else                                                            \
            return (__default_str);                                     \
    }

#else
#define ARUBA_DENSE_ENUM_DEF_END(__type, __max_entry,                   \
                                 __default_str, __prefixstr)            \
    } __type##_t;                                                       \
    extern const char *__type##_get_str(int enum_type);  
#endif
