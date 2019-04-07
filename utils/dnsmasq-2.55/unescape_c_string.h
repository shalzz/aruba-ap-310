#ifndef UNESCAPE_C_STRING_H
#define UNESCAPE_C_STRING_H

typedef enum C_STRING_RESULT {
    C_STRING_RESULT_END,
    C_STRING_RESULT_NOCHAR,
    C_STRING_RESULT_PENDING,
    C_STRING_RESULT_SYNBAD,        
    C_STRING_RESULT_VALID,
    C_STRING_RESULT_VALIDPUSH
} C_STRING_RESULT;

typedef enum C_STRING_FLAG {
    C_STRING_FLAG_NONE,
    C_STRING_FLAG_END
} C_STRING_FLAG;

int unescape_c_string(char *cp, char c, int *astate, int flag);
int str_unescape_c_string(char *dst, const char *src);
ssize_t strn_unescape_c_string(char *dst, const char *src, size_t sz);

#endif

