
#ifndef __DENSE_ENUM_DEFN_H__
#define __DENSE_ENUM_DEFN_H__

typedef struct ar_dense_enum_str_entry_s {
    const char *str;
}ar_dense_enum_str_entry_t;

typedef struct ar_dense_enum_str_array_s {
    unsigned int prefixlen;
    unsigned int num_entries;
    ar_dense_enum_str_entry_t *str_entries;
} ar_dense_enum_str_array_t;

#endif /*__DENSE_ENUM_DEFN_H__ */

