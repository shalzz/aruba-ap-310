/* utelnetd.c
 *
 * Simple telnet server
 *
 * Bjorn Wesen, Axis Communications AB 
 * <bjornw@axis.com>
 * 
 * Joerg Schmitz-Linneweber, Aston GmbH
 * <schmitz-linneweber@aston-technologie.de>
 *
 * Vladimir Oleynik
 * <dzo@simtreas.ru>
 *
 * Robert Schwebel, Pengutronix
 * <r.schwebel@pengutronix.de>
 * 
 *
 * This file is distributed under the GNU General Public License (GPL),
 * please see the file LICENSE for further information.
 * 
 * ---------------------------------------------------------------------------
 * (C) 2000, 2001, 2002 by the authors mentioned above
 * ---------------------------------------------------------------------------
 *
 * The telnetd manpage says it all:
 *
 *   Telnetd operates by allocating a pseudo-terminal device (see pty(4))  for
 *   a client, then creating a login process which has the slave side of the
 *   pseudo-terminal as stdin, stdout, and stderr. Telnetd manipulates the
 *   master side of the pseudo-terminal, implementing the telnet protocol and
 *   passing characters between the remote client and the login process.
 */

#include <sys/time.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <string.h>
#define __USE_XOPEN
#define __USE_GNU
#include <stdlib.h>
#undef __USE_XOPEN
#undef __USE_GNU
#include <unistd.h>
#include <errno.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdint.h>
#include <signal.h>
#include <termios.h>
#if !defined(AP_PLATFORM)
#include "cmn/dispatcher/dispatcher.h"
#include "cmn/papi/MHApi.h"
#include <cmn/mgmt/mgmtModule.h>
#include <cmn/mgmt/mgmtModuleMsg.h>
#include "../../cmn/util/md5.h"
#include "../../cmn/util/passwd.h"
#include <cmn/util/ipv6_cmn.h>
#include <cli/cliauth/cliauth.h>
#include <cli/cli_args.h>
#include <amapi/amapi.h>
#include <include/encryptpass.h>
#include <include/ph.h>
#include <cli/cli.h>
#include "cmn/util/cli_utils.h"
#include <unistd.h>
#include <string.h>
#include <openssl/ec.h>
#include <openssl/bn.h>
#include <openssl/sha.h>
#include <openssl/hmac.h>
#include <openssl/rand.h>
#include <openssl/buffer.h>
#include <openssl/err.h>
#include <cmn/util/file_utils.h>
#endif

#ifdef DEBUG
#define TELCMDS
#define TELOPTS
#endif
#include <arpa/telnet.h>
#include <ctype.h>
#include <net/if.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <arpa/inet.h>

extern int errno;

#define BUFSIZE 4000

#ifndef MIN 
#define MIN(a,b) ((a) > (b) ? (b) : (a))
#endif 

#define CURVE_BYTES     8

#define ASCII_0         48      // ASCII value for 0
#define ASCII_9         57      // ASCII value for 9 
#define ASCII_A         65      // ASCII value for A
#define ASCII_Z         90      // ASCII value for Z
#define ASCII_a         97      // ASCII value for a
#define ASCII_z         122     // ASCII value for z

unsigned char allzero[20] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00
};

static const char Hex[]="0123456789ABCDEF";

#define SUPPORT_MODE_MAX_USERNAME_LEN 64   // SUPPORT_MODE_USERNAME_BUF_LEN has
                                           // to be greater than SUPPORT_MODE_MAX_USERNAME_LEN
#define SUPPORT_MODE_USERNAME_BUF_LEN 96

static char *loginpath = NULL;
#if !defined(AP_PLATFORM)
static ip_addr_t peer_ip;
static uint16_t peer_port;
#endif

/* shell name and arguments */

static char *cli_argv_init[] = {"cli", "-l", NULL, NULL};
static char *argv_init[] = {NULL, NULL, NULL, NULL, NULL, NULL};

/* structure that describes a session */

struct tsession {
	struct tsession *next;
	int sockfd, ptyfd;
	int shell_pid;
	/* two circular buffers */
	char *buf1, *buf2;
	int rdidx1, wridx1, size1;
	int rdidx2, wridx2, size2;
};

static void abort_session(struct tsession *);

static int is_fat_ap = 0;

#ifdef DEBUG
#define DEBUG_OUT(fmt, args...) syslog(LOG_INFO, fmt, ##args)
#ifdef USE_SYSLOG
#include <syslog.h>
#else
#define syslog(pri, fmt, args...) fprintf(stderr, fmt, ##args)
#endif
#else
#define DEBUG_OUT(...)
//static inline void DEBUG_OUT(const char *format, ...) {};
#endif

/*

   This is how the buffers are used. The arrows indicate the movement
   of data.

   +-------+     wridx1++     +------+     rdidx1++     +----------+
   |       | <--------------  | buf1 | <--------------  |          |
   |       |     size1--      +------+     size1++      |          |
   |  pty  |                                            |  socket  |
   |       |     rdidx2++     +------+     wridx2++     |          |
   |       |  --------------> | buf2 |  --------------> |          |
   +-------+     size2++      +------+     size2--      +----------+

   Each session has got two buffers.

*/

static int maxfd;
static int nologin = 0;
static int cliproc = 0;
static int cli_init = 0;

static struct tsession *sessions;

static int
check_fat_ap(void)
{
#ifndef AP_PLATFORM
	return 0;
#else
	struct stat sb;
	return (stat("/tmp/fatap", &sb) == 0) ? 1 : 0;
#endif
}

/* 
 * This code was ported from a version which was made for busybox. 
 * So we have to define some helper functions which are originally 
 * available in busybox...
 */  

void show_usage(void)
{
	printf("Usage: telnetd [-p port] [-i interface] [-l loginprogram] [-d] [-c]\n");
	printf("\n");
	printf("   -p port          specify the tcp port to connect to\n");
	printf("   -i interface     specify the network interface to listen on\n");
	printf("                    (default: all interfaces)\n");
	printf("   -l loginprogram  program started by the server\n");
	printf("   -d               daemonize\n");
	printf("   -c               Controller CLI access\n");
	printf("\n");         
	exit(1);
}

static int cli_fgets(char *buf, int len, FILE *f)
{
    char *p;
    if(!fgets(buf, len, f)) {
        return 0;
    }
    p = buf;
    while (*p && *p != '\n') {
        p++;
    }
    *p = 0;
    return 1;
}

#if !defined(AP_PLATFORM)
static int cli_passwd(char *user, char *pass)
{
    FILE *f;
    char fuser[PASSWD_SIZE];
    char fpass[NEW_PASSWD_SIZE];
    char ppass[PASSWD_SIZE];
    int rc = 0; /* assume failure */
    f = fopen(PASSWD_FILE, "r");
    if (f) {
        /* file exists, search for user:passwd */
        while (cli_fgets(fuser, sizeof(fuser), f)) {
	    if (!cli_fgets(fpass, sizeof(fpass), f)) {
	        /* something wrong */
	        break;
	    }
	    /* verify user */
	    if (strcmp(fuser, user)) {
	        /* failed */
	        continue;
	    }
	    /* verify password */
		if(arubaHashCmp(pass, fpass) == 0){
	        /* success */
	        rc = 1;
			break;
		}
	}
    } else {
		char digest[16];
        if(!strcmp(user, OVERRIDE_USER)){
			md5_calc(digest, pass, strlen(pass));
			if(memcmp(digest, OVERRIDE_PASSWD, sizeof(digest)) == 0)
				return 1;
			else
				return 0;
		}
    }
    return rc;
}
#endif

static char *ugets(char *s, int size, FILE *stream,
		   int t, struct termios* termbuf)
{
    int i = 0;
    while (i < size-1) {
        s[i] = fgetc(stream);
	if (s[i] == '\n') {
	    s[i+1] = '\0';
	    return s;
	}
	if (s[i] == '\r') {
	    /* windows: ignore '\r' and reconfigure term */
	    termbuf->c_iflag |= IGNCR;
	    termbuf->c_iflag &= ~INLCR;
	    tcsetattr(t, TCSANOW, termbuf);
	    continue;
	}
	i++;
    }
    return NULL;
}


#if !defined(AP_PLATFORM)
/*This code has been adapted from Open SSL function - BN_bn2hex*/
/* This function converts a BIGNUM to hex alike BN_bn2hex except
   that, this function preserves the leading zeros if there are in 
   the BIGNUM */
/* Must 'OPENSSL_free' the returned data */
char *BN_bn2hex_custom(const BIGNUM *a)
{
    int i,j,v;
//  int z=0;
    char *buf;
    char *p;

    buf=(char *)OPENSSL_malloc(a->top*BN_BYTES*2+2);
    if (buf == NULL)
    {
        BNerr(BN_F_BN_BN2HEX,ERR_R_MALLOC_FAILURE);
        goto err;
    }
    p=buf;
    if (a->neg) *(p++)='-';
    if (BN_is_zero(a)) *(p++)='0';
    for (i=a->top-1; i >=0; i--)
    {
        for (j=BN_BITS2-8; j >= 0; j-=8)
        {
            v=((int)(a->d[i]>>(long)j))&0xff;
            // preserving the leading zeros
//          if (z || (v != 0))
//                {
                *(p++)=Hex[v>>4];
                *(p++)=Hex[v&0x0f];
//                z=1;
//                }
        }
    }
    *p='\0';
err:
    return(buf);
}

/* This function has been adapted from OpenSSL library function 
EC_GROUP_new_curve_GFp. This function skips the NIST validation of the 
parameters and generates the curve which is used in generating the token and 
one-time-password. Refer to the bug 142275 */
static EC_GROUP *EC_GROUP_new_curve_GFp_custom(const BIGNUM *p, const BIGNUM *a, const BIGNUM *b, BN_CTX *ctx)
{
    const EC_METHOD *meth;
    EC_GROUP *ret;

    meth = EC_GFp_mont_method();

    ret = EC_GROUP_new(meth);
    if (ret == NULL)
        return NULL;

    if (!EC_GROUP_set_curve_GFp(ret, p, a, b, ctx))	{
        EC_GROUP_clear_free(ret);
        return NULL;
    }

	return ret;
}

static int generateOneTimePasswd(char *user, char *tokenstr, char *passwd)
{
	BIGNUM *p = NULL, *inter = NULL, *one = NULL;
	BIGNUM *controllerpriv = NULL, *token = NULL, *ss = NULL;
	BIGNUM *a = NULL, *b = NULL;
	BIGNUM *serverx = NULL, *servery = NULL;
	unsigned char sschar[CURVE_BYTES], abits[CURVE_BYTES*2], pwd[SHA_DIGEST_LENGTH];
	BN_CTX *bnctx = NULL;
	HMAC_CTX ctx;
	HMAC_CTX ctx1;
	EC_GROUP *grp = NULL;
	EC_POINT *serverpub = NULL, *controllerpub = NULL, *generator = NULL, *Z = NULL;
	BIGNUM *genx = NULL, *geny = NULL, *y = NULL;
	unsigned char username[64] = {0};
	unsigned int mdlen;
	unsigned char ctr, prk[SHA_DIGEST_LENGTH];
	int i;
	int status = -1;
	BIGNUM *password = NULL;
    char *token_hex = NULL, *passwd_hex = NULL;

	if (user == NULL)
		return status;

	memset(username, 0, sizeof(username));
	snprintf(username, sizeof(username), "%s", user);

	if (((a = BN_new()) == NULL) ||
		((b = BN_new()) == NULL) ||
		((p = BN_new()) == NULL) ||
		((inter = BN_new()) == NULL) ||
		((one = BN_new()) == NULL) ||
		((controllerpriv = BN_new()) == NULL) ||
		((genx = BN_new()) == NULL) ||
		((geny = BN_new()) == NULL) ||
		((y = BN_new()) == NULL) ||
		((serverx = BN_new()) == NULL) ||
		((servery = BN_new()) == NULL) ||
		((token = BN_new()) == NULL) ||
		((ss = BN_new()) == NULL) ||
		((password = BN_new()) == NULL) ||
		((bnctx = BN_CTX_new()) == NULL)) {
		printf("\n Cannot create points/bignums/context");
		goto fin;
	}


	/*
	 * the random 64-bit elliptic curve
	 */
	BN_hex2bn(&p, "FE4382C5413A02FF");
	BN_hex2bn(&a, "5BA3091245C856AB");
	BN_hex2bn(&b, "C2AB76EF7FE1D7F2");
	BN_hex2bn(&genx, "BB3C71C351AAE96B");
	BN_hex2bn(&geny, "830B4345D75E9275");

/* Using the custom adapted function EC_GROUP_new_curve_GFp_custom for FIPS mode 
to skip the NIST validation of the parameters used in the EC curve generation,
which is used to generate the token and one-time-password. This is done to keep
the token size as 16 bytes, which would get longer in case NIST compliant 
parameters are used */
#if defined (FIPS)
	if ((grp = EC_GROUP_new_curve_GFp_custom(p, a, b, bnctx)) == NULL) {
#else
	if ((grp = EC_GROUP_new_curve_GFp(p, a, b, bnctx)) == NULL) {
#endif
		printf("\n Cannot create a curve from q, a, and b");
		goto fin;
	}

	if (((serverpub = EC_POINT_new(grp)) == NULL) ||
		((controllerpub = EC_POINT_new(grp)) == NULL) ||
		((Z = EC_POINT_new(grp)) == NULL)) {
		printf("\n Cannot create points on curve");
	    goto fin;
	}
	if ((generator = EC_POINT_new(grp)) == NULL) {
		printf("\n unable to allocate generator");
		goto fin;
	}
	if (!EC_POINT_set_affine_coordinates_GFp(grp, generator, genx, geny, bnctx)) {
		printf("\n Can't get generator's coordinates!");
		goto fin;
	}

	/*
	 * server public key (note: THIS IS NOT THE REAL ONE!!!!!)
	 */
	BN_hex2bn(&serverx, "3A5C2C676D253232");
	BN_hex2bn(&servery, "5CEBB232132200DA");
	if (!EC_POINT_set_affine_coordinates_GFp(grp, serverpub, serverx, servery, bnctx)) {
		printf("\n unable to set server's public key");
		goto fin;
	}

	/*
	 * do a diffie-hellman with server's public key...
	 *
	 * Use the technique from B.1.1 in FIPS 186-3 to produce
	 * a private key. This reduces the bias imposed by the
	 * modulo operation.
	 *
	 * we don't know q so use p as a rough stand-in
	 */
	BN_one(one);
	BN_sub(inter, p, one);
	RAND_pseudo_bytes(abits, CURVE_BYTES*2);    /* 64 additional bits */
	BN_bin2bn(abits, CURVE_BYTES*2, controllerpriv);
	BN_mod(controllerpriv, controllerpriv, inter, bnctx);
	BN_add(controllerpriv, controllerpriv, one); /* x = (c mod (q-1)) + 1 */

	if (!EC_POINT_mul(grp, controllerpub, NULL, generator, controllerpriv, bnctx)) {
		printf("\n unable to generate point on the curve");
		goto fin;
	}
	/*
	 * Z = serverpub^controllerpriv
	 * ss is the x-coordinate of the secret
	 */
	if (!EC_POINT_mul(grp, Z, NULL, serverpub, controllerpriv, bnctx)) {
		printf("\n unable to complete D-H");
		goto fin;
	}
	if (!EC_POINT_get_affine_coordinates_GFp(grp, Z, ss, NULL, bnctx)) {
		printf("\n Unable to get shared secret");
		goto fin;
	}

	/*
	 * the token is the x-coordinate of the controller's public key
	 */
	if (!EC_POINT_get_affine_coordinates_GFp(grp, controllerpub, token, NULL, bnctx)) {
		printf("\n unable to obtain token!");
		goto fin;
	}

    token_hex = BN_bn2hex_custom(token);
	snprintf(tokenstr, 17, "%s", token_hex);

	mdlen = SHA_DIGEST_LENGTH;

	/*
	 * password = HKDF(ss, username)
	 */
	BN_bn2bin(ss, sschar);
	HMAC_Init(&ctx, allzero, sizeof(allzero), EVP_sha1());
	HMAC_Update(&ctx, sschar, CURVE_BYTES);
	HMAC_Final(&ctx, prk, &mdlen);

	HMAC_Init(&ctx1, prk, SHA_DIGEST_LENGTH, EVP_sha1());
	HMAC_Update(&ctx1, (unsigned char *)username, strlen(username));
	ctr = 0x01;
	HMAC_Update(&ctx1, (unsigned char *)&ctr, 1);
	mdlen = CURVE_BYTES;
	HMAC_Final(&ctx1, pwd, &mdlen);

	BN_bin2bn(pwd, CURVE_BYTES, password);
    passwd_hex = BN_bn2hex_custom(password);

	snprintf(passwd, 17, "%s", passwd_hex);

	status = 0;

fin:
	if (a != NULL) {
		BN_free(a);
	}
	if (b != NULL) {
		BN_free(b);
	}
	if (p != NULL) {
		BN_free(p);
	}
	if (inter != NULL) {
		BN_free(inter);
	}
	if (genx != NULL) {
		BN_free(genx);
	}
	if (geny != NULL) {
		BN_free(geny);
	}
	if (y != NULL) {
		BN_free(y);
	}
	if (serverx != NULL) {
		BN_free(serverx);
	}
	if (servery != NULL) {
		BN_free(servery);
	}

	if (generator != NULL) {
		EC_POINT_free(generator);
	}
	if (serverpub != NULL) {
		EC_POINT_free(serverpub);
	}
	if (controllerpub != NULL) {
		EC_POINT_free(controllerpub);
	}
	if (Z != NULL) {
		EC_POINT_free(Z);
	}

	if (controllerpriv != NULL) {
		BN_free(controllerpriv);
	}
	if (ss != NULL) {
		BN_free(ss);
	}

	if (password != NULL) {
		BN_free(password);
	}

	if (bnctx != NULL) {
		BN_CTX_free(bnctx);
	}
    
    if (token_hex != NULL) {
        OPENSSL_free(token_hex);
    }
    if (passwd_hex != NULL) {
        OPENSSL_free(passwd_hex);
    }

	return status;
}
#endif


#if !defined(AP_PLATFORM)
int verifySecret(char *secret)
{
	int i;
	char *p, *s;
	unsigned char mdhash[16];

	md5_calc(mdhash, secret, strlen(secret));

	/* compare calulated digest to our digest which is saved in ARUBA_DIGEST */
	p = mdhash;
	s = (char*)ARUBA_SECRET;
	for(i=0;i<16;i++, s++, p++){
		if(*s != *p) {
			return -1;
		}
	}
	return 0;
}
#endif

static int
validateSupportModeInput (char *str) {
    int i = 0;
    if (strlen(str) > SUPPORT_MODE_MAX_USERNAME_LEN) {
        return 0;
    }
    for (i = 0; i < strlen(str); i++) {
        if (str[i] == ' ')
            return 0;
    }
    return 1;
}

// This functions checks whether the password has any heading or trailing
// non-alphanumeric characters that may input while copying the token
static int
validateAlphanumeric (char *pwd)
{
    int pwdlen = strlen(pwd);

    if (((pwd[0] >= ASCII_0 && pwd[0] <= ASCII_9 ) ||
         (pwd[0] >= ASCII_A && pwd[0] <= ASCII_Z) ||
         (pwd[0] >= ASCII_a && pwd[0] <= ASCII_z)) && 
        ((pwd[pwdlen - 1] >= ASCII_0 && pwd[pwdlen - 1] <= ASCII_9) ||
         (pwd[pwdlen - 1] >= ASCII_A && pwd[pwdlen - 1] <= ASCII_Z) ||
         (pwd[pwdlen - 1] >= ASCII_a && pwd[pwdlen - 1] <= ASCII_z))) {
        return 1;
    } else {
        return 0;
    }
}

static void verifyUser(int t, struct termios* termbuf)
{
    char secret[256];
    int len = 0;
	int count = 0;
    int productionImage = 0, manufacturingImage = 0;

#if defined (PRODUCTION_IMAGE)
    productionImage = 1;
#endif
#if defined (MANUFACTURING_IMAGE)
    manufacturingImage = 1;
#endif

#if !defined(AP_PLATFORM)
    char buf[MAX_BANNER_SIZE + 1];

    while (1) {
        char user[256], passwd[256];
        if(count > 2){
            printf("\nExceeded Maximum retries\n");
            exit(0);
        }
        count++;
		printf("User: ");
		fflush(stdout);

        ugets(user, sizeof(user), stdin, t, termbuf);

        /* remove \n */
        len = strlen(user);
        if (len < 2) {
            continue;
        }
        user[len-1]=0;

        /* turn echo off */
        termbuf->c_lflag &= ~ECHO;
        tcsetattr(t, TCSANOW, termbuf);
	
	FILE *fp = fopen(BANNER_FILE, "r");
        if (fp != NULL) {
        while (fgets(buf, sizeof(buf), fp)) {
            printf("%s", buf);
        }
        fflush(stdout);
        fclose(fp);
    }


        printf("Password: "); fflush(stdout);

        ugets(passwd, sizeof(passwd), stdin, t, termbuf);

        /* turn echo back on */
        termbuf->c_lflag |= ECHO;
        tcsetattr(t, TCSANOW, termbuf);

        /* remove \n */
        len = strlen(passwd);
        if (len < 2) {
            continue;
        }
        passwd[len-1]=0;

        if (cliproc) {
            char role[255];
            int auth_status = 0;
            /* 
             * As soon as auth info is entered by user, Open,do auth and close 
             * the PAPI channel for each forked child. This avoids seperate PAPI
             * port for each forked child just for AAA auth
             */
            if (cli_auth_init(UTELNETD_CLI) == 0) {
                printf("\nAnother session initialization in progress. Try again...\n");
                continue;
            }
            auth_status = cli_auth_user(user, passwd, AAA_USER_CONNECT_TYPE_TELNET,
                    2525, &peer_ip, peer_port, role, sizeof(role));
            cli_auth_close();
            if (auth_status) {
                snprintf(cliuserarg, sizeof(cliuserarg), CLI_USER_FORMAT ,user);
                snprintf(clirolearg, sizeof(clirolearg), CLI_ROLE_FORMAT ,role);
                snprintf(cliuseriparg, sizeof(cliuseriparg), CLI_USERIP_FORMAT,
                        aruba_ip_to_str(peer_ip));
				snprintf(cliparentid, sizeof(cliparentid), CLI_PARENT_FORMAT ,CLI_PARENT_TELNET);
                argv_init[1] = cliuserarg;
                argv_init[2] = clirolearg; 
                argv_init[3] = cliuseriparg;
				argv_init[4] = cliparentid;				
                printf("\n");
                break;
            } else {
                printf("\nInvalid password\n");
            }
        } else {
            if(cli_passwd(user,passwd)==0) {
                printf("\nInvalid password\n");
            } else {
                /* 
                 * Verify support password 
                 */
                printf("\n");
                count = 0;
                while(1) {
                    if ( manufacturingImage || (!productionImage && 
                        is_file_present("//flash//.supportmode"))) {
                        if(count > 2){
                            printf("\nExceeded Maximum retries\n");
                            exit(0);
                        }
                        count++;
                        /* turn echo off */
                        termbuf->c_lflag &= ~ECHO;
                        tcsetattr(t, TCSANOW, termbuf);

                        printf("Support Password: "); fflush(stdout);

                        ugets(secret, sizeof(secret), stdin, t, termbuf);

                        /* turn echo back on */
                        termbuf->c_lflag |= ECHO;
                        tcsetattr(t, TCSANOW, termbuf);

                        /* remove \n */
                        len = strlen(secret);
                        if (len < 2) {
                            continue;
                        }
                        secret[len-1]=0;

                        if(verifySecret(secret)!=0){
                            printf("\nInvalid Support Password.\n");
                            continue;
                        }
                        printf("\n");
                        break;
                    } else {
                        char support_username[SUPPORT_MODE_USERNAME_BUF_LEN];
                        char user_passwd[64];
                        char token_passwd[64];
                        char token[64];
                        int status = 0, i = 0;

                        memset(support_username, 0, sizeof(support_username));
                        memset(user_passwd, 0, sizeof(user_passwd));
                        memset(token_passwd, 0, sizeof(token_passwd));

                        if (count > 2) {
                            printf("Exceeded maximum retries.\n");
                            exit(0);
                        }
                        count++;

                        printf("\nUsername (Please enter with @domain.com):");
                        fflush(stdout);
                        ugets(support_username, sizeof(support_username), stdin, t, termbuf);
                        len = strlen(support_username);

                        support_username[len - 1] = 0;
                        if (len < 2 || !validateSupportModeInput(support_username)) {
                            if (count < 3) {
                                printf("\nInvalid username! Please retry.");
                            } else {
                                printf("\nInvalid username! ");
                            }
                            continue;
                        }

                        status = generateOneTimePasswd(support_username, token, token_passwd);
                        if ((status != 0) || (strlen(token) == 0) ||
                            (strlen(token_passwd) == 0)) {
                            printf("\nUnable to generate one time password");
                            fflush(stdout);
                            continue;
                        }
   
                        printf("\nToken: ");
                        for (i = 0; i < (2*CURVE_BYTES); i++) {
                            printf("%c", token[i]);
                            fflush(stdout);

                            if(((i+1) % 4 == 0) && (i < (2*CURVE_BYTES)-1)) {
                                printf("-");
                                fflush(stdout);
                            }
                        }
                        fflush(stdout);
 
                        printf("\nPlease generate one time password at"
                               " https://ase.arubanetworks.com/decode_aos_key");
                        fflush(stdout);

                        printf("\nSupport Password:");
                        fflush(stdout);

                        ugets(user_passwd, sizeof(user_passwd), stdin, t, termbuf);
                        len = strlen(user_passwd);

                        if (len < 2) {
                            if (count < 3) {
                                printf("\nInvalid password! Please retry.");
                            } else {
                                printf("\nInvalid password! ");
                            }
                            continue;
                        } 
                        user_passwd[len - 1] = 0;

                        if (!validateAlphanumeric(user_passwd)) {
                            if (count < 3) {
                                printf("\r\n\r\nInvalid password! Password contains "
                                       "non-alphanumeric characters. Please retry.");
                            } else {
                                printf("\r\n\r\nInvalid password! Password contains "
                                       "non-alphanumeric characters. ");
                            }
                            continue;
                        }

                        if (strcasecmp(user_passwd, token_passwd) != 0) {
                            if (count < 3) {
                                printf("\nInvalid password! Please retry.");
                            } else {
                                printf("\nInvalid password! ");
                            }
                            continue;
                        }
                        printf("\n");
                        break;
                    }
                }
                break;
            }
        }
    }
#else
    {
      struct stat sb ;
      if (stat("/tmp/telnet_enable", &sb) < 0) {
	printf("Telnet access disabled. Enable in switch CLI\n") ;
	exit(0) ;
      }
    }
#endif
}

void perror_msg_and_die(char *text)
{
    fprintf(stderr,text);
    exit(1);
}

void error_msg_and_die(char *text)
{
    perror_msg_and_die(text);
}


/* 
   Remove all IAC's from the buffer pointed to by bf (recieved IACs are ignored
   and must be removed so as to not be interpreted by the terminal).  Make an
   uninterrupted string of characters fit for the terminal.  Do this by packing
   all characters meant for the terminal sequentially towards the end of bf. 

   Return a pointer to the beginning of the characters meant for the terminal.
   and make *processed equal to the number of characters that were actually
   processed and *num_totty the number of characters that should be sent to
   the terminal.  
   
   Note - If an IAC (3 byte quantity) starts before (bf + len) but extends
   past (bf + len) then that IAC will be left unprocessed and *processed will be
   less than len.
  
   FIXME - if we mean to send 0xFF to the terminal then it will be escaped,
   what is the escape character?  We aren't handling that situation here.

  */
static char *
remove_iacs(unsigned char *bf, int len, int *processed, int *num_totty) {
    unsigned char *ptr = bf;
    unsigned char *totty = bf;
    unsigned char *end = bf + len;
   
    while (ptr < end) {
	if (*ptr != IAC) {
	    *totty++ = *ptr++;
	}
	else {
	    if ((ptr+2) < end) {
		/* the entire IAC is contained in the buffer 
		   we were asked to process. */
		DEBUG_OUT("Ignoring IAC 0x%02x, %s, %s\n", *ptr, TELCMD(*(ptr+1)), TELOPT(*(ptr+2)));
		ptr += 3;
	    } else {
		/* only the beginning of the IAC is in the 
		   buffer we were asked to process, we can't
		   process this char. */
		break;
	    }
	}
    }

    *processed = ptr - bf;
    *num_totty = totty - bf;
    /* move the chars meant for the terminal towards the end of the 
       buffer. */
    return memmove(ptr - *num_totty, bf, *num_totty);
}


static int getpty(char *line)
{
        int p;
        int i;

#ifdef CONFIG_DEVPTS_FS
	p = open("/dev/ptmx", 2);
	if (p > 0){
		grantpt(p);
		unlockpt(p);
		strcpy(line, ptsname(p));
		return p;
	}
#else

        p = getpt();
        if (p < 0) {
                DEBUG_OUT("getpty(): couldn't get pty\n");
                close(p);
                return -1;
        }
        if (grantpt(p)<0 || unlockpt(p)<0) {
                DEBUG_OUT("getpty(): couldn't grant and unlock pty\n");
                close(p);
                return -1;
        }
	if (isatty(p) && (ioctl(p, TIOCGPTN, &i) == 0)){
		sprintf(line, "/dev/pts/%d", i);
		return p;
	}
        DEBUG_OUT("getpty(): got pty %s\n",ptsname(p));
	if (ptsname(p) == NULL){
		//perror("ptsname");
		//printf("errno = %d\n", errno);
	}
        strcpy(line, (const char*)ptsname(p));

        return(p);
#endif
}


static void
send_iac(struct tsession *ts, unsigned char command, int option)
{
	/* We rely on that there is space in the buffer for now.  */
	char *b = ts->buf2 + ts->rdidx2;
	*b++ = IAC;
	*b++ = command;
	*b++ = option;
	ts->rdidx2 += 3;
	ts->size2 += 3;
}


static struct tsession *
make_new_session(int sockfd, struct sockaddr_in6 *remote)
{
	struct termios termbuf;
	int pty, pid;
	static char tty_name[32];
	struct tsession *ts;
	int t1, t2;

#if !defined(AP_PLATFORM)
    if (num_active_sessions() >= MAX_NON_CONSOLE_SESSIONS) {
		fprintf(stderr, "Max sessions active (%d) !\n", 
                MAX_NON_CONSOLE_SESSIONS);
        return NULL;
    }
#endif

    ts = (struct tsession *)malloc(sizeof(struct tsession));
	if (ts == NULL) {
	    return ts;
	}

	ts->buf1 = (char *)malloc(BUFSIZE);
	ts->buf2 = (char *)malloc(BUFSIZE);

	ts->sockfd = sockfd;

	ts->rdidx1 = ts->wridx1 = ts->size1 = 0;
	ts->rdidx2 = ts->wridx2 = ts->size2 = 0;

	/* Got a new connection, set up a tty and spawn a shell.  */

	pty = getpty(tty_name);

	if (pty < 0) {
		fprintf(stderr, "All network ports in use!\n");
		return 0;
	}

	if (pty > maxfd)
		maxfd = pty;

	ts->ptyfd = pty;

	/* Make the telnet client understand we will echo characters so it 
	 * should not do it locally. We don't tell the client to run linemode,
	 * because we want to handle line editing and tab completion and other
	 * stuff that requires char-by-char support.
	 */

	send_iac(ts, WILL, TELOPT_ECHO);
	send_iac(ts, DO, TELOPT_SGA);
	send_iac(ts, WILL, TELOPT_SGA);


	if ((pid = fork()) < 0) {
		perror("fork");
		abort_session(ts) ;
		return 0 ;
	}
	if (pid == 0) {
		/* In child, open the child's side of the tty.  */
		int i, t;

		for(i = 0; i <= maxfd; i++)
			close(i);
		/* make new process group */
		if (setsid() < 0)
			perror_msg_and_die("setsid");

		//t = open(tty_name, O_RDWR | O_NOCTTY);
		t = open(tty_name, O_RDWR);
		if (t < 0)
			perror_msg_and_die("Could not open tty");

		t1 = dup(0);
		t2 = dup(1);

		tcsetpgrp(0, getpid());
  
		/* The pseudo-terminal allocated to the client is configured to operate in
		 * cooked mode, and with XTABS CRMOD enabled (see tty(4)).
		 */

		tcgetattr(t, &termbuf);
		termbuf.c_lflag |= ECHO; /* if we use readline we dont want this */
		termbuf.c_oflag |= ONLCR|XTABS;
		termbuf.c_iflag |= ICRNL|INLCR;
		termbuf.c_iflag &= ~IXOFF;
		/* termbuf.c_lflag &= ~ICANON; */
		tcsetattr(t, TCSANOW, &termbuf);

		DEBUG_OUT("stdin, stdout, stderr: %d %d %d\n", t, t1, t2);


		/* 
		 * we don't run /bin/login (no binary)
		 * verify the user ourselves 
		 */
		if (!nologin) {
		verifyUser(t, &termbuf);
		}

		/* exec shell, with correct argv and env */
		if (is_fat_ap) {
#ifdef __FAT_AP__
            char str[INET6_ADDRSTRLEN];
            if (IN6_IS_ADDR_V4MAPPED(&remote->sin6_addr)) {
                struct in_addr sin_addr;
                memcpy(&sin_addr,
                       remote->sin6_addr.s6_addr + 12, sizeof(sin_addr));
                setenv("REMOTE_ADDR", inet_ntoa(sin_addr), 1);
            } else {
                setenv("REMOTE_ADDR",
                       inet_ntop(AF_INET6, &remote->sin6_addr,
                                 str, INET6_ADDRSTRLEN), 1);
            }
            if (access("/etc/motd", R_OK) == 0) {
                FILE *fp = fopen("/etc/motd", "r");
                int ch;
                if (fp) {
                    while((ch = fgetc(fp))!= EOF) {
                        printf("%c", ch);
                    }
                    fflush(stdout);
                    fclose(fp);
                }
            }
#endif
			execv(loginpath, cli_argv_init);
		} else {
            if(!cliproc) {
                        argv_init[1] = "--login";
                        argv_init[2] = "--telnet";
            }
			execv(loginpath, argv_init);
		}
	    fprintf(stderr, "Execv failed : %s(%d)\n", strerror(errno), errno);
	}

	ts->shell_pid = pid;

	return ts;
}

static void
abort_session(struct tsession *ts)
{

	free(ts->buf1);
	free(ts->buf2);
	close(ts->ptyfd);
	close(ts->sockfd);
	if(ts->ptyfd == maxfd || ts->sockfd == maxfd)
		maxfd--;
	if(ts->ptyfd == maxfd || ts->sockfd == maxfd)
		maxfd--;

	free(ts);
}

static void
free_session(struct tsession *ts)
{
	struct tsession *t = sessions;

	/* Unlink this telnet session from the session list.  */
	if(t == ts)
		sessions = ts->next;
	else {
		while(t->next != ts)
			t = t->next;
		t->next = ts->next;
	}

	free(ts->buf1);
	free(ts->buf2);

	kill(ts->shell_pid, SIGKILL);

	wait4(ts->shell_pid, NULL, 0, NULL);

	close(ts->ptyfd);
	close(ts->sockfd);

	if(ts->ptyfd == maxfd || ts->sockfd == maxfd)
		maxfd--;
	if(ts->ptyfd == maxfd || ts->sockfd == maxfd)
		maxfd--;

	free(ts);
}

int main(int argc, char **argv)
{
	struct sockaddr_in6 sa;
	int master_fd = -1;
	fd_set rdfdset, wrfdset;
	int selret;
	int on = 1;
#ifdef REMOTE_AP
    int portnbr = 2323;
#else
	int portnbr = 23;
#endif
	int c, ii;
	int daemonize = 0;
	char *interface_name = NULL;
	struct ifreq interface;

	is_fat_ap = check_fat_ap();

	signal(SIGPIPE, SIG_IGN);
	/* check if user supplied a port number */

	for (;;) {
		c = getopt( argc, argv, "i:p:l:hdnc");
		if (c == EOF) break;
		switch (c) {
			case 'p':
				portnbr = atoi(optarg);
				break;
			case 'i':
				interface_name = strdup(optarg);
				break;
			case 'l':
				loginpath = strdup(optarg);
				break;
			case 'd':
				daemonize = 1;
				break;
			case 'n':
				nologin = 1;
				break;
			case 'c':
				cliproc = 1;
				break;
			case 'h': 
			default:
				show_usage();
				exit(1);
		}
	}

#ifdef DEBUG
#ifdef USE_SYSLOG
	openlog("utelned", 0, LOG_USER);
	syslog(LOG_INFO, "utelnetd starting\n");
#endif
#endif
	if (!loginpath) {
	  loginpath = "/bin/login";
	  if (access(loginpath, X_OK) < 0)
	    loginpath = "/bin/sh";
	}
	  
	if (access(loginpath, X_OK) < 0) {
		/* workaround: error_msg_and_die has doesn't understand
 		   variable argument lists yet */
		fprintf(stderr,"\"%s\"",loginpath);
		perror_msg_and_die(" is no valid executable!\n");
	}

#if 0
    /* don't print this...too obvious for ssh */
	printf("telnetd: starting\n");
	printf("  port: %i; interface: %s; login program: %s\n",
		portnbr, (interface_name)?interface_name:"any", loginpath);
#endif

	argv_init[0] = loginpath;
	cli_argv_init[0] = loginpath;
	sessions = 0;

	/* Grab a TCP socket.  */
	master_fd = socket(AF_INET6, SOCK_STREAM, 0);
	if (master_fd < 0) {
		perror("socket");
		return 1;
	}
	(void)setsockopt(master_fd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));

	/* Set it to listen to specified port.  */

	memset((void *)&sa, 0, sizeof(sa));
	sa.sin6_family = AF_INET6;
	sa.sin6_port = htons(portnbr);

	/* Set it to listen on the specified interface */
	if (interface_name) {
		strncpy(interface.ifr_ifrn.ifrn_name, interface_name, IFNAMSIZ);
		(void)setsockopt(master_fd, SOL_SOCKET,
				SO_BINDTODEVICE, &interface, sizeof(interface));
	}

	if (bind(master_fd, (struct sockaddr *) &sa, sizeof(sa)) < 0) {
		perror("bind");
		return 1;
	}

	if (listen(master_fd, 1) < 0) {
		perror("listen");
		return 1;
	}

	if (daemonize) 
	{
		DEBUG_OUT("  daemonizing\n");
		if (daemon(0, 1) < 0) perror_msg_and_die("daemon");
	}

	maxfd = master_fd;

	do {
		struct tsession *ts;

		FD_ZERO(&rdfdset);
		FD_ZERO(&wrfdset);

		/* select on the master socket, all telnet sockets and their
		 * ptys if there is room in their respective session buffers.
		 */

		ts = sessions;
		while (ts) {
			/* buf1 is used from socket to pty
			 * buf2 is used from pty to socket
			 */
			if (ts->size1 > 0) {
				FD_SET(ts->ptyfd, &wrfdset);  /* can write to pty */
			}
			if (ts->size1 < BUFSIZE) {
				FD_SET(ts->sockfd, &rdfdset); /* can read from socket */
			}
			if (ts->size2 > 0) {
				FD_SET(ts->sockfd, &wrfdset); /* can write to socket */
			}
			if (ts->size2 < BUFSIZE) {
				FD_SET(ts->ptyfd, &rdfdset);  /* can read from pty */
			}
			ts = ts->next;
		}

		FD_SET(master_fd, &rdfdset);

		selret = select(maxfd + 1, &rdfdset, &wrfdset, 0, 0);

		if (!selret)
			break;

		/* First check for and accept new sessions.  */
		if (FD_ISSET(master_fd, &rdfdset)) {
			int fd, salen;

			salen = sizeof(sa);	
			if ((fd = accept(master_fd, (struct sockaddr *)&sa,
					 &salen)) < 0) {
				continue;
			} else {
				/* Create a new session and link it into
				   our active list.  */
                struct tsession *new_ts = make_new_session(fd, &sa);
#if !defined(AP_PLATFORM)
                if (IN6_IS_ADDR_V4MAPPED(&sa.sin6_addr)) {
                    struct in_addr sin_addr;
                    memcpy(&sin_addr,
                            sa.sin6_addr.s6_addr + 12, sizeof(sin_addr));
                    SETV4(peer_ip, sin_addr.s_addr);
                } else {
                    SETV6(peer_ip, sa.sin6_addr.s6_addr);
                }
                peer_port = sa.sin6_port;
#endif
				if (new_ts) {
					new_ts->next = sessions;
					sessions = new_ts;
					if (fd > maxfd)
						maxfd = fd;
				} else {
					close(fd);
				}
			}
		}
accept_over:
		/* Then check for data tunneling.  */

		ts = sessions;
		while (ts) { /* For all sessions...  */
			int maxlen, w, r;
			struct tsession *next = ts->next; /* in case we free ts. */

			if (ts->size1 && FD_ISSET(ts->ptyfd, &wrfdset)) {
			    int processed, num_totty;
			    char *ptr;
				/* Write to pty from buffer 1.  */
				
				maxlen = MIN(BUFSIZE - ts->wridx1,
					     ts->size1);
				ptr = remove_iacs(ts->buf1 + ts->wridx1, maxlen, 
					&processed, &num_totty);
		
				/* the difference between processed and num_totty
				   is all the iacs we removed from the stream.
				   Adjust buf1 accordingly. */
				ts->wridx1 += processed - num_totty;
				ts->size1 -= processed - num_totty;

				w = write(ts->ptyfd, ptr, num_totty);
				if (w < 0) {
					perror("write");
					free_session(ts);
					ts = next;
					continue;
				}
				ts->wridx1 += w;
				ts->size1 -= w;
				if (ts->wridx1 == BUFSIZE)
					ts->wridx1 = 0;
			}

			if (ts->size2 && FD_ISSET(ts->sockfd, &wrfdset)) {
				/* Write to socket from buffer 2.  */
				maxlen = MIN(BUFSIZE - ts->wridx2,
					     ts->size2);
				w = write(ts->sockfd, ts->buf2 + ts->wridx2, maxlen);
				if (w < 0) {
					perror("write");
					free_session(ts);
					ts = next;
					continue;
				}
				ts->wridx2 += w;
				ts->size2 -= w;
				if (ts->wridx2 == BUFSIZE)
					ts->wridx2 = 0;
			}

			if (ts->size1 < BUFSIZE && FD_ISSET(ts->sockfd, &rdfdset)) {
				/* Read from socket to buffer 1. */
				maxlen = MIN(BUFSIZE - ts->rdidx1,
					     BUFSIZE - ts->size1);
				r = read(ts->sockfd, ts->buf1 + ts->rdidx1, maxlen);
				if (!r || (r < 0 && errno != EINTR)) {
					free_session(ts);
					ts = next;
					continue;
				}
				if(!*(ts->buf1 + ts->rdidx1 + r - 1)) {
					r--;
					if(!r)
						continue;
				}
				ts->rdidx1 += r;
				ts->size1 += r;
				if (ts->rdidx1 == BUFSIZE)
					ts->rdidx1 = 0;
			}

			if (ts->size2 < BUFSIZE && FD_ISSET(ts->ptyfd, &rdfdset)) {
				/* Read from pty to buffer 2.  */
				maxlen = MIN(BUFSIZE - ts->rdidx2,
					     BUFSIZE - ts->size2);
				r = read(ts->ptyfd, ts->buf2 + ts->rdidx2, maxlen);
				if (!r || (r < 0 && errno != EINTR)) {
					free_session(ts);
					ts = next;
					continue;
				}
				for (ii=0; ii < r; ii++)
				  if (*(ts->buf2 + ts->rdidx2 + ii) == 3)
				    fprintf(stderr, "found <CTRL>-<C> in data!\n");
				ts->rdidx2 += r;
				ts->size2 += r;
				if (ts->rdidx2 == BUFSIZE)
					ts->rdidx2 = 0;
			}

			if (ts->size1 == 0) {
				ts->rdidx1 = 0;
				ts->wridx1 = 0;
			}
			if (ts->size2 == 0) {
				ts->rdidx2 = 0;
				ts->wridx2 = 0;
			}
			ts = next;
		}

	} while (1);

	return 0;
}
