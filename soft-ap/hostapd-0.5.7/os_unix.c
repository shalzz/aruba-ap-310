/*
 * wpa_supplicant/hostapd / OS specific functions for UNIX/POSIX systems
 * Copyright (c) 2005-2006, Jouni Malinen <jkmaline@cc.hut.fi>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * Alternatively, this software may be distributed under the terms of BSD
 * license.
 *
 * See README and COPYING for more details.
 */

#include "includes.h"
#include <sys/stat.h>

#include "os.h"
#ifdef __FIPS_BUILD__
#include <openssl/rand.h>
#ifndef __FAT_AP__
#include <openssl/fips_rand.h>
#endif
#include <openssl/err.h>
#include <fcntl.h>
#endif
void os_sleep(os_time_t sec, os_time_t usec)
{
	if (sec)
		sleep(sec);
	if (usec)
		usleep(usec);
}


int os_get_time(struct os_time *t)
{
	int res;
	struct timeval tv;
	res = gettimeofday(&tv, NULL);
	t->sec = tv.tv_sec;
	t->usec = tv.tv_usec;
	return res;
}


int os_mktime(int year, int month, int day, int hour, int min, int sec,
	      os_time_t *t)
{
	struct tm tm;

	if (year < 1970 || month < 1 || month > 12 || day < 1 || day > 31 ||
	    hour < 0 || hour > 23 || min < 0 || min > 59 || sec < 0 ||
	    sec > 60)
		return -1;

	memset(&tm, 0, sizeof(tm));
	tm.tm_year = year - 1900;
	tm.tm_mon = month - 1;
	tm.tm_mday = day;
	tm.tm_hour = hour;
	tm.tm_min = min;
	tm.tm_sec = sec;

	*t = (os_time_t) mktime(&tm);
	return 0;
}

int write_pid_file(const char *pid_file)
{
	if (pid_file) {
		FILE *f = fopen(pid_file, "w");
		if (f) {
			fprintf(f, "%u\n", getpid());
			fclose(f);
		}
	}
	return 0;
}

int is_pid_file_present(const char *pid_file)
{
	struct stat s;

	if (stat(pid_file, &s) == 0)
		return 1;
	else
		return 0;
}

int os_daemonize(const char *pid_file)
{
	if (daemon(0, 0)) {
		perror("daemon");
		return -1;
	}

	if (pid_file) {
		FILE *f = fopen(pid_file, "w");
		if (f) {
			fprintf(f, "%u\n", getpid());
			fclose(f);
		}
	}

	return -0;
}


void os_daemonize_terminate(const char *pid_file)
{
	if (pid_file)
		unlink(pid_file);
}


#ifdef __FIPS_BUILD__
static int g_randfd = 0;

static void
crypto_init(void)
{
    g_randfd = open("/dev/urandom", O_RDONLY);
    if (!g_randfd) {
        /* fatal, but should never happen */
        system("echo \"hostapd : Fatal Error : cannot open random dev\" > /dev/console");
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

int os_get_random(unsigned char *buf, size_t len)
{
    if (!g_randfd) {
        crypto_init();
        /*
         * rand() coupled with SHA1 to compute each byte
         * seed is md5 of /proc/sys/kernel/random concatentated with "ifconfig -a" output
         * eth interrupts are used to generate entropy to /dev/urandom
         * enbaled RANDOM_SAMPLE to natsemi in case it is only eth interface used
         *
         */
        crypto_read(buf, len);
#ifdef CONFIG_PALOMINO
        {
            static int seeded = 0;
            if(0==seeded){
                /*little bizzare, need to do this on palomino so that
                 * openssl eay is happy*/
                RAND_seed(buf, len);
                seeded = 1;
            }
        }
        /*One more read just so to differ*/
        crypto_read(buf, len);
#endif /* end of CONFIG_PALOMINO*/
        /*Self test for NON-Approved RNG*/
        if(!memcmp(buf, buf+8, 8)){
            /* fatal, but should never happen */
            system("echo \"hostapd : Fatal Error : Key1 & Key 2 Stuck\" > /dev/console");
            /* TODO: handle error */
            return -1;
        }
#ifdef __FAT_AP__
	RAND_seed( buf, len) ;
#else
	FIPS_x931_method()->cleanup();
        FIPS_x931_set_key(buf,32);
        FIPS_x931_seed(buf+32,16);
        if(0 != compare_FIPS_prng_key_with_seed(buf, buf+32)){
            /* fatal, but should never happen */
            system("echo \"hostapd : Fatal Error : FIPS Rand Seed Failed\" > /dev/console");
            /* TODO: handle error */
            return -1;
        }
#endif
    }
    /*Since we use the random number for provide secrity functions, the Random number
     * implementation should be of FIPS approved random number generator
     * OpenSSL uses : ANS X9.31 (FIPS approved) */
    if (RAND_bytes(buf,len) <= 0) {
#ifndef __NO_OPENSSL__
        ERR_load_crypto_strings();
        ERR_print_errors(BIO_new_fp(stderr,BIO_NOCLOSE));
#endif
        system("echo \"hostapd : Fatal Error : Random number generation failed \" > /dev/console");
        return -1;
    }
    return 0;
}
#else /*else of #ifdef __FIPS_BUILD__*/
int os_get_random(unsigned char *buf, size_t len)
{
	FILE *f;
	size_t rc;

	f = fopen("/dev/urandom", "rb");
	if (f == NULL) {
		printf("Could not open /dev/urandom.\n");
		return -1;
	}

	rc = fread(buf, 1, len, f);
	fclose(f);

	return rc != len ? -1 : 0;
}
#endif /*end of #ifdef __FIPS_BUILD__ */


unsigned long os_random(void)
{
	return random();
}


char * os_rel2abs_path(const char *rel_path)
{
	char *buf = NULL, *cwd, *ret;
	size_t len = 128, cwd_len, rel_len, ret_len;
	int last_errno;

	if (rel_path[0] == '/')
		return strdup(rel_path);

	for (;;) {
		buf = malloc(len);
		if (buf == NULL)
			return NULL;
		cwd = getcwd(buf, len);
		if (cwd == NULL) {
			last_errno = errno;
			free(buf);
			if (last_errno != ERANGE)
				return NULL;
			len *= 2;
			if (len > 2000)
				return NULL;
		} else {
			buf[len - 1] = '\0';
			break;
		}
	}

	cwd_len = strlen(cwd);
	rel_len = strlen(rel_path);
	ret_len = cwd_len + 1 + rel_len + 1;
	ret = malloc(ret_len);
	if (ret) {
		memcpy(ret, cwd, cwd_len);
		ret[cwd_len] = '/';
		memcpy(ret + cwd_len + 1, rel_path, rel_len);
		ret[ret_len - 1] = '\0';
	}
	free(buf);
	return ret;
}


int os_program_init(void)
{
	return 0;
}


void os_program_deinit(void)
{
}


int os_setenv(const char *name, const char *value, int overwrite)
{
	return setenv(name, value, overwrite);
}


int os_unsetenv(const char *name)
{
#if defined(__FreeBSD__) || defined(__NetBSD__)
	unsetenv(name);
	return 0;
#else
	return unsetenv(name);
#endif
}


char * os_readfile(const char *name, size_t *len)
{
	FILE *f;
	char *buf;
        int ret;

	f = fopen(name, "rb");
	if (f == NULL)
		return NULL;

	fseek(f, 0, SEEK_END);
	ret = ftell(f);
        if (ret >= 0) *len = ret;
        else {
                fclose(f);
                return NULL;
        }
	fseek(f, 0, SEEK_SET);

	buf = malloc(*len);
	if (buf == NULL) {
		fclose(f);
		return NULL;
	}

	fread(buf, 1, *len, f);
	fclose(f);

	return buf;
}


void * os_zalloc(size_t size)
{
	return calloc(1, size);
}
