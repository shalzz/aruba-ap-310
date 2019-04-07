#ifndef _SHA1_WRAP_XYSSL_H_
#define _SHA1_WRAP_XYSSL_H_

#include <ctaocrypt/sha.h>

#define sha1_context  Sha
#define sha1_starts   InitSha
#define sha1_update   ShaUpdate
#define sha1_finish   ShaFinal

#endif
