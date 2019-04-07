/*
 * magic.c - PPP Magic Number routines.
 *
 * Copyright (c) 1989 Carnegie Mellon University.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms are permitted
 * provided that the above copyright notice and this paragraph are
 * duplicated in all such forms and that any documentation,
 * advertising materials, and other materials related to such
 * distribution and use acknowledge that the software was developed
 * by Carnegie Mellon University.  The name of the
 * University may not be used to endorse or promote products derived
 * from this software without specific prior written permission.
 * THIS SOFTWARE IS PROVIDED ``AS IS'' AND WITHOUT ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
 * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 */

#define RCSID	"$Id: magic.c,v 1.9 1999/08/13 06:46:15 paulus Exp $"

#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/time.h>
#include <stdlib.h>
#include <unistd.h>

#include "pppd.h"
#include "magic.h"

#ifdef __FIPS_BUILD__
#include <openssl/rand.h>
#ifndef __FAT_AP__
#include <openssl/fips_rand.h>
#endif
#include <openssl/err.h>
#include <string.h>
#include <fcntl.h>
#endif

static const char rcsid[] = RCSID;

#ifndef __FIPS_BUILD__
extern long mrand48 __P((void));
extern void srand48 __P((long));
#endif

#ifdef __FIPS_BUILD__
static int g_randfd = 0;

static void
crypto_init(void)
{
    g_randfd = open("/dev/urandom", O_RDONLY);
    if (!g_randfd) {
        /* fatal, but should never happen */
        system("echo \"pppd : Fatal Error : cannot open random dev\" > /dev/console");
        /* TODO: handle error */
        exit(1);
    }
}

static void
crypto_read(void *buffer, size_t length)
{
   char *b = (char *)buffer;

    do {
        int l = read(g_randfd, b, length);
        if (l < 0) {
            /* fatal, but should not be possible */
            fprintf(stderr, "Fatal error in reading the random dev\n");
            sleep(1);
            continue;
        }
        b      += l;
        length -= l;

    } while(length > 0);
}

void
magic_init()
{
    if (!g_randfd) {
        crypto_init();
        char tmpBuf[128];
        /*
         * rand() coupled with SHA1 to compute each byte
         * seed is md5 of /proc/sys/kernel/random concatentated with "ifconfig -a" output
         * eth interrupts are used to generate entropy to /dev/urandom
         * enbaled RANDOM_SAMPLE to natsemi in case it is only eth interface used
         *
         */
        crypto_read(tmpBuf, sizeof(tmpBuf));
#ifdef CONFIG_PALOMINO
        {
            static int seeded = 0;
            if(0==seeded){
                /*little bizzare, need to do this on palomino so that
                 * openssl eay is happy*/
                RAND_seed(tmpBuf, sizeof(tmpBuf));
                seeded = 1;
            }
        }
        /*One more read just so to differ*/
        crypto_read(tmpBuf, sizeof(tmpBuf));
#endif /* end of CONFIG_PALOMINO*/
        /*Self test for NON-Approved RNG*/
        if(!memcmp(tmpBuf, tmpBuf+8, 8)){
            /* fatal, but should never happen */
            system("echo \"pppd : Fatal Error : Key1 & Key 2 Stuck\" > /dev/console");
            /* TODO: handle error */
            return ;
        }
#ifdef __FAT_AP__
	RAND_seed(tmpBuf, sizeof(tmpBuf));
#else
        FIPS_x931_method()->cleanup();
        FIPS_x931_set_key(tmpBuf,32);
        FIPS_x931_seed(tmpBuf+32,16);
        if(0 != compare_FIPS_prng_key_with_seed(tmpBuf, tmpBuf+32)){
            /* fatal, but should never happen */
            system("echo \"pppd : Fatal Error : FIPS Rand Seed Failed\" > /dev/console");
            /* TODO: handle error */
            return ;
        }
#endif
    }
}
#else /*else of #ifdef __FIPS_BUILD__*/
/*
 * magic_init - Initialize the magic number generator.
 *
 * Attempts to compute a random number seed which will not repeat.
 * The current method uses the current hostid, current process ID
 * and current time, currently.
 */
void
magic_init()
{
    long seed;
    struct timeval t;

    gettimeofday(&t, NULL);
    seed = get_host_seed() ^ t.tv_sec ^ t.tv_usec ^ getpid();
    srand48(seed);
}
#endif /*endof of #ifdef __FIPS_BUILD__*/

/*
 * magic - Returns the next magic number.
 */
u_int32_t
magic()
{
#ifdef __FIPS_BUILD__
    u_int32_t t_var;
    if (RAND_bytes((unsigned char *)&t_var,sizeof(t_var)) <= 0) {
#ifndef __FAT_AP__
        ERR_load_crypto_strings();
        ERR_print_errors(BIO_new_fp(stderr,BIO_NOCLOSE));
#endif
        system("echo \"pppd : Fatal Error : Random number generation failed \" > /dev/console");
        return -1;
    }
    return t_var;
#else /*else of #ifdef __FIPS_BUILD__*/
    return (u_int32_t) mrand48();
#endif /*endof of #ifdef __FIPS_BUILD__*/
}

#ifdef NO_DRAND48
/*
 * Substitute procedures for those systems which don't have
 * drand48 et al.
 */

double
drand48()
{
#ifdef __FIPS_BUILD__
    double t_var;
    if (RAND_bytes((unsigned char *)&t_var,sizeof(t_var)) <= 0) {
        ERR_load_crypto_strings();
        ERR_print_errors(BIO_new_fp(stderr,BIO_NOCLOSE));
        system("echo \"pppd : Fatal Error : Random number generation failed \" > /dev/console");
        return -1;
    }
    return t_var;
#else /*else of #ifdef __FIPS_BUILD__*/
    return (double)random() / (double)0x7fffffffL; /* 2**31-1 */
#endif /*endof of #ifdef __FIPS_BUILD__*/
}

#ifndef __FIPS_BUILD__
long
mrand48()
{
    return random();
}

void
srand48(seedval)
long seedval;
{
    srandom((int)seedval);
}
#endif /*endof of #ifndef __FIPS_BUILD__*/

#endif
