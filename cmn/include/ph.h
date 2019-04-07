#ifndef _PWHASH_H_
#define _PWHASH_H_

#ifndef __FAT_AP__
#ifdef UC_CROSS
#include <openssl/sha.h>
#endif
#include <include/aruba_defs.h>
#include <crypto-util/sha1_nativeimpl.h>
#endif

/*
 * The hash is in a special format:
 *     [ 4 byte salt ] [ 1 byte hash algorithm ] [ hash digest ]
 *
 * The hash is then converted into an Aruba string, ie. the byte 0xde
 * becomes the string "de".
 */

// Version in which hashed passwords was introduced.
#define PW_HASH_MAJOR_VER 3
#define PW_HASH_MINOR_VER 2
#ifndef SHA1_MAC_LEN
#define SHA1_MAC_LEN 20
#endif
#define SALT_SIZE 4
#define ALG_SIZE 1
#define PW_BIN_HASH_SIZE (SALT_SIZE + ALG_SIZE + SHA1_MAC_LEN)
#define PW_STRING_HASH_SIZE (PW_BIN_HASH_SIZE * 2)

#define PW_HASH_ALG_SHA1 1
#define PW_HASH_ALG_CURRENT PW_HASH_ALG_SHA1

#ifdef __FAT_AP__
#define SHA256_MAC_LEN 32
#define PW_HASH_ALG_PBKDF2_SHA256 2
#undef PW_BIN_HASH_SIZE
/*PW_BIN_HASH_SIZE should be calculated based on lenght of largest
 *hash size, as of now it is sha256*/
#define PW_BIN_HASH_SIZE (SALT_SIZE + ALG_SIZE + SHA256_MAC_LEN)
#undef PW_STRING_HASH_SIZE
#define PW_STRING_HASH_SIZE (PW_BIN_HASH_SIZE * 2)
char *arubaHash2(char *input);
#endif

// Caller must free returned buffer when finished.
char *arubaDecryptBuffer(char *buffer);

// Caller must free returned buffer when finished.
char *arubaHash(char *input);

int arubaHashCmp(const char *plaintext, const char *hash_string);

#ifdef AP_PLATFORM
extern int RAND_bytes(unsigned char *buf, int num);
#endif

#ifdef __FIPS_BUILD__

# ifdef __FAT_AP__
#include <fcntl.h>
#include <unistd.h>
#   define FIPS_CFG_FILE                      "/tmp/fips_mode.cfg"
#   define FIPS_MODE_ENABLE                   'Y'
#   define FIPS_MODE_DISABLE                  'N'

#define FIPS_SUPPORTED_CIPHERS		"ECDHE-RSA-AES256-GCM-SHA384:ECDHE-RSA-AES128-GCM-SHA256:ECDHE-RSA-AES256-SHA384:ECDHE-RSA-AES128-SHA256:DHE-RSA-AES256-GCM-SHA384:DHE-RSA-AES128-GCM-SHA256:DHE-RSA-AES256-SHA384:DHE-RSA-AES128-SHA256"

static inline int 
FIPSModeEnabled(void)
{
    int ret = 0;
    char fips_mode;

    int fd = open(FIPS_CFG_FILE, O_RDONLY);
    if (fd == -1) {
        return ret;
    }
    if (read(fd, &fips_mode, 1) == -1) {
        goto err;
    }

    if (fips_mode == FIPS_MODE_ENABLE) {
        ret = 1;
    }

err:
    close(fd);
    return ret;
}
# endif

void checkAndSetFIPSMode( const char *processname ) ;

//TODO These sha functions should go into a separate wrapper file for all crypto stuff
void calculate_sha1(unsigned char *inputbuf, unsigned long input_len, unsigned char *out_digest) ;
#endif

#endif // _PWHASH_H_
