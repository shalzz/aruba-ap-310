#include <sys/types.h>
#include <ctype.h>
#include <stdlib.h>
#include "unescape_c_string.h"

#ifndef isoctal
# define isoctal(c)    ((c) >= '0' && (c) <= '7')
#endif
#ifndef digittoint
# define digittoint(c) (((c) >= '0' && (c) <= '9') ? (c) - '0' : tolower(c) - 'a' + 10)
#endif

typedef enum C_STRING_STATE {
    C_STRING_STATE_GROUND,
    C_STRING_STATE_START,
    C_STRING_STATE_OCTAL2,
    C_STRING_STATE_OCTAL3,
    C_STRING_STATE_HEX1,
    C_STRING_STATE_HEX2
} C_STRING_STATE;

int
unescape_c_string(char *cp, char c, int *astate, int flag)
{
   if (flag & C_STRING_FLAG_END) {
        switch (*astate) {
        case C_STRING_STATE_OCTAL2:
        case C_STRING_STATE_OCTAL3:
        case C_STRING_STATE_HEX1:
        case C_STRING_STATE_HEX2:           
           *astate = C_STRING_STATE_GROUND;
            return C_STRING_RESULT_VALID;
        case C_STRING_STATE_GROUND:
            return C_STRING_RESULT_NOCHAR;
        default:
            return C_STRING_RESULT_SYNBAD;
       } 
   }
   switch (*astate) {
   case C_STRING_STATE_GROUND:
       *cp = 0;
       if (c == '\\') {
           *astate = C_STRING_STATE_START;
           return C_STRING_RESULT_PENDING;
       }
       *cp = c;
        return C_STRING_RESULT_VALID;
        
    case C_STRING_STATE_START:
        *cp = 0;
        switch (c) {
        case '\\':
            *cp = c;
            *astate = C_STRING_STATE_GROUND;
            return C_STRING_RESULT_VALID;
            
       case '0': case '1': case '2': case '3':
       case '4': case '5': case '6': case '7':
           *cp = (c - '0');
           *astate = C_STRING_STATE_OCTAL2;
           return C_STRING_RESULT_PENDING;
            
        case 'x':
           *astate = C_STRING_STATE_HEX1;
           return C_STRING_RESULT_PENDING;
            
       case 'n':
           *cp = '\n';
           *astate = C_STRING_STATE_GROUND;
           return C_STRING_RESULT_VALID;
       case 'r':
           *cp = '\r';
           *astate = C_STRING_STATE_GROUND;
           return C_STRING_RESULT_VALID;
       case 'b':
           *cp = '\b';
           *astate = C_STRING_STATE_GROUND;
           return C_STRING_RESULT_VALID;
       case 'a':
           *cp = '\a';
           *astate = C_STRING_STATE_GROUND;
           return C_STRING_RESULT_VALID;

       case 'v':
           *cp = '\v';
           *astate = C_STRING_STATE_GROUND;
           return C_STRING_RESULT_VALID;
       case 't':
           *cp = '\t';
           *astate = C_STRING_STATE_GROUND;
           return C_STRING_RESULT_VALID;
       case 'f':
           *cp = '\f';
           *astate = C_STRING_STATE_GROUND;
           return C_STRING_RESULT_VALID;
       case 's':
           *cp = ' ';
           *astate = C_STRING_STATE_GROUND;
           return C_STRING_RESULT_VALID;
       case 'E':
           *cp = '\033';
           *astate = C_STRING_STATE_GROUND;
            return C_STRING_RESULT_VALID;
            
        case '\n':
        case '$':
            *astate = C_STRING_STATE_GROUND;
            return C_STRING_RESULT_NOCHAR;
        }
        *astate = C_STRING_STATE_GROUND;        
        return C_STRING_RESULT_SYNBAD;
        
    case C_STRING_STATE_OCTAL2:
        if (isoctal((int) (unsigned char) c)) {
           *cp = (*cp << 3) + (c - '0');
           *astate = C_STRING_STATE_OCTAL3;    
           return C_STRING_RESULT_PENDING;
       }
        *astate = C_STRING_STATE_GROUND;
        return C_STRING_RESULT_VALIDPUSH;

    case C_STRING_STATE_OCTAL3:
       *astate = C_STRING_STATE_GROUND;
       if (isoctal((int) (unsigned char) c)) {
           *cp = (*cp << 3) + (c - '0');
           return C_STRING_RESULT_VALID;
       }
       return C_STRING_RESULT_VALIDPUSH;
        
    case C_STRING_STATE_HEX1:
        if (isxdigit((int) (unsigned char) c)) {
            *cp = digittoint((int) (unsigned char) c);
            *astate = C_STRING_STATE_HEX2;
           return C_STRING_RESULT_PENDING;            
        }
        *astate = C_STRING_STATE_GROUND;
        return C_STRING_RESULT_VALIDPUSH;
        
    case C_STRING_STATE_HEX2:
        *astate = C_STRING_STATE_GROUND;
        if (isxdigit((int) (unsigned char) c)) {
            *cp = (*cp << 4) + digittoint((int) (unsigned char) c);
           return C_STRING_RESULT_VALID;            
        }
        return C_STRING_RESULT_VALIDPUSH;
        
    default:
        *astate = C_STRING_STATE_GROUND;
        return C_STRING_RESULT_SYNBAD;
    }
}
int
str_unescape_c_string(char *dst, const char *src)
{
   char c;
   char *start = dst;
   int state = 0;

  while ((c = *src++)) {
  again:
      switch (unescape_c_string(dst, c, &state, 0)) {
       case C_STRING_RESULT_VALID:
           dst++;
           break;
       case C_STRING_RESULT_VALIDPUSH:
           dst++;
           goto again;
       case C_STRING_RESULT_PENDING:
       case C_STRING_RESULT_NOCHAR:
           break;
       default:
           *dst = 0;
           return -1;
       }
   }
   if (unescape_c_string(dst, c, &state, C_STRING_FLAG_END) == C_STRING_RESULT_VALID) {
       dst++;
    }
   *dst = 0;
   return dst - start;
}

ssize_t
strn_unescape_c_string(char *dst, const char *src, size_t sz)
{
   char c, p;
   char *start = dst, *end = dst + sz - 1;
   int state = 0;

   if (sz > 0) {
       *end = 0;
    }
   while ((c = *src++)) {
   again:
       switch (unescape_c_string(&p, c, &state, 0)) {
       case C_STRING_RESULT_VALID:
           if (dst < end) {
               *dst = p;
            }
           dst++;
           break;
       case C_STRING_RESULT_VALIDPUSH:
           if (dst < end) {
               *dst = p;
            }
           dst++;
           goto again;
       case 0:
       case C_STRING_RESULT_NOCHAR:
           break;
       default:
           if (dst <= end) {
               *dst = 0;
            }
           return -1;
       }
   }
   if (unescape_c_string(&p, c, &state, C_STRING_FLAG_END) == C_STRING_RESULT_VALID) {
       if (dst < end) {
           *dst = p;
        }
       dst++;
   }
   if (dst <= end) {
       *dst = 0;
    }
   return dst - start;
}

