/* vi: set sw=4 ts=4: */
/*
 * Utility routines.
 *
 * Copyright 1998 by Albert Cahalan; all rights reserved.
 * Copyright (C) 2002 by Vladimir Oleynik <dzo@simtreas.ru>
 * SELinux support: (c) 2007 by Yuichi Nakamura <ynakam@hitachisoft.jp>
 *
 * Licensed under GPLv2 or later, see file LICENSE in this tarball for details.
 */

#include "libbb.h"

#include <sys/ioctl.h>
#include <unistd.h>
#include <string.h>
#include <memory.h>
#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>
#include <fcntl.h>
#include <setjmp.h>
#include <ctype.h>

# if ULONG_MAX > 0xffffffff
#define OFF_FMT "ll"
#else
#define OFF_FMT	"l"
#endif

extern void xfunc_die(void );
extern void bb_simple_perror_msg(const char *s);


#define ENABLE_DEBUG	0
#define ENABLE_FEATURE_CLEAN_UP	0
#define ENABLE_FEATURE_PREFER_APPLETS 1
#define ENABLE_HUSH 0

#define xrealloc_vector(vector, shift, idx) \
	xrealloc_vector_helper((vector), (sizeof((vector)[0]) << 8) + (shift), (idx))
#ifndef DMALLOC
/* dmalloc provides variants of these that do abort() on failure.
 * Since dmalloc's prototypes overwrite the impls here as they are
 * included after these prototypes in libbb.h, all is well.
 */
// Warn if we can't allocate size bytes of memory.
void* FAST_FUNC malloc_or_warn(size_t size)
{
	void *ptr = malloc(size);
	if (ptr == NULL && size != 0)
		bb_error_msg(bb_msg_memory_exhausted);
	return ptr;
}

// Die if we can't allocate size bytes of memory.
void* FAST_FUNC xmalloc(size_t size)
{
	void *ptr = malloc(size);
	if (ptr == NULL && size != 0)
		bb_error_msg_and_die(bb_msg_memory_exhausted);
	return ptr;
}

// Die if we can't resize previously allocated memory.  (This returns a pointer
// to the new memory, which may or may not be the same as the old memory.
// It'll copy the contents to a new chunk and free the old one if necessary.)
void* FAST_FUNC xrealloc(void *ptr, size_t size)
{
	ptr = realloc(ptr, size);
	if (ptr == NULL && size != 0)
		bb_error_msg_and_die(bb_msg_memory_exhausted);
	return ptr;
}
#endif /* DMALLOC */

// Die if we can't allocate and zero size bytes of memory.
void* FAST_FUNC xzalloc(size_t size)
{
	void *ptr = xmalloc(size);
	memset(ptr, 0, size);
	return ptr;
}

// Die if we can't copy a string to freshly allocated memory.
char* FAST_FUNC xstrdup(const char *s)
{
	char *t;

	if (s == NULL)
		return NULL;

	t = strdup(s);

	if (t == NULL)
		bb_error_msg_and_die(bb_msg_memory_exhausted);

	return t;
}

// Die if we can't allocate n+1 bytes (space for the null terminator) and copy
// the (possibly truncated to length n) string into it.
char* FAST_FUNC xstrndup(const char *s, int n)
{
	int m;
	char *t;

	if (ENABLE_DEBUG && s == NULL)
		bb_error_msg_and_die("xstrndup bug");

	/* We can just xmalloc(n+1) and strncpy into it, */
	/* but think about xstrndup("abc", 10000) wastage! */
	m = n;
	t = (char*) s;
	while (m) {
		if (!*t) break;
		m--;
		t++;
	}
	n -= m;
	t = xmalloc(n + 1);
	t[n] = '\0';

	return memcpy(t, s, n);
}

// Die if we can't open a file and return a FILE* to it.
// Notice we haven't got xfread(), This is for use with fscanf() and friends.
FILE* FAST_FUNC xfopen(const char *path, const char *mode)
{
	FILE *fp = fopen(path, mode);
	if (fp == NULL)
		bb_perror_msg_and_die("can't open '%s'", path);
	return fp;
}

// Die if we can't open a file and return a fd.
int FAST_FUNC xopen3(const char *pathname, int flags, int mode)
{
	int ret;

	ret = open(pathname, flags, mode);
	if (ret < 0) {
		bb_perror_msg_and_die("can't open '%s'", pathname);
	}
	return ret;
}

// Die if we can't open an existing file and return a fd.
int FAST_FUNC xopen(const char *pathname, int flags)
{
	return xopen3(pathname, flags, 0666);
}

// Warn if we can't open a file and return a fd.
int FAST_FUNC open3_or_warn(const char *pathname, int flags, int mode)
{
	int ret;

	ret = open(pathname, flags, mode);
	if (ret < 0) {
		bb_perror_msg("can't open '%s'", pathname);
	}
	return ret;
}

// Warn if we can't open a file and return a fd.
int FAST_FUNC open_or_warn(const char *pathname, int flags)
{
	return open3_or_warn(pathname, flags, 0666);
}

void FAST_FUNC xunlink(const char *pathname)
{
	if (unlink(pathname))
		bb_perror_msg_and_die("can't remove file '%s'", pathname);
}

void FAST_FUNC xrename(const char *oldpath, const char *newpath)
{
	if (rename(oldpath, newpath))
		bb_perror_msg_and_die("can't move '%s' to '%s'", oldpath, newpath);
}

int FAST_FUNC rename_or_warn(const char *oldpath, const char *newpath)
{
	int n = rename(oldpath, newpath);
	if (n)
		bb_perror_msg("can't move '%s' to '%s'", oldpath, newpath);
	return n;
}

void FAST_FUNC xpipe(int filedes[2])
{
	if (pipe(filedes))
		bb_perror_msg_and_die("can't create pipe");
}

void FAST_FUNC xdup2(int from, int to)
{
	if (dup2(from, to) != to)
		bb_perror_msg_and_die("can't duplicate file descriptor");
}

// "Renumber" opened fd
void FAST_FUNC xmove_fd(int from, int to)
{
	if (from == to)
		return;
	xdup2(from, to);
	close(from);
}

/*
 * Write all of the supplied buffer out to a file.
 * This does multiple writes as necessary.
 * Returns the amount written, or -1 on an error.
 */
ssize_t FAST_FUNC full_write(int fd, const void *buf, size_t len)
{
	ssize_t cc;
	ssize_t total;

	total = 0;

	while (len) {
		cc = safe_write(fd, buf, len);

		if (cc < 0) {
			if (total) {
				/* we already wrote some! */
				/* user can do another write to know the error code */
				return total;
			}
			return cc;	/* write() returns -1 on failure. */
		}

		total += cc;
		buf = ((const char *)buf) + cc;
		len -= cc;
	}

	return total;
}
// Die with an error message if we can't write the entire buffer.
void FAST_FUNC xwrite(int fd, const void *buf, size_t count)
{
	if (count) {
		ssize_t size = full_write(fd, buf, count);
		if ((size_t)size != count)
			bb_error_msg_and_die("short write");
	}
}
void FAST_FUNC xwrite_str(int fd, const char *str)
{
	xwrite(fd, str, strlen(str));
}

// Die with an error message if we can't lseek to the right spot.
off_t FAST_FUNC xlseek(int fd, off_t offset, int whence)
{
	off_t off = lseek(fd, offset, whence);
	if (off == (off_t)-1) {
		if (whence == SEEK_SET)
			bb_perror_msg_and_die("lseek(%"OFF_FMT"u)", offset);
		bb_perror_msg_and_die("lseek");
	}
	return off;
}

// Die with supplied filename if this FILE* has ferror set.
void FAST_FUNC die_if_ferror(FILE *fp, const char *fn)
{
	if (ferror(fp)) {
		/* ferror doesn't set useful errno */
		bb_error_msg_and_die("%s: I/O error", fn);
	}
}

// Die with an error message if stdout has ferror set.
void FAST_FUNC die_if_ferror_stdout(void)
{
	die_if_ferror(stdout, bb_msg_standard_output);
}

// Die with an error message if we have trouble flushing stdout.
void FAST_FUNC xfflush_stdout(void)
{
	if (fflush(stdout)) {
		bb_perror_msg_and_die(bb_msg_standard_output);
	}
}


int FAST_FUNC bb_putchar(int ch)
{
	/* time.c needs putc(ch, stdout), not putchar(ch).
	 * it does "stdout = stderr;", but then glibc's putchar()
	 * doesn't work as expected. bad glibc, bad */
	return putc(ch, stdout);
}

/* Die with an error message if we can't copy an entire FILE* to stdout,
 * then close that file. */
void FAST_FUNC xprint_and_close_file(FILE *file)
{
	fflush(stdout);
	// copyfd outputs error messages for us.
	if (bb_copyfd_eof(fileno(file), 1) == -1)
		xfunc_die();

	fclose(file);
}

// Die with an error message if we can't malloc() enough space and do an
// sprintf() into that space.
char* FAST_FUNC xasprintf(const char *format, ...)
{
	va_list p;
	int r;
	char *string_ptr;

#if 1
	// GNU extension
	va_start(p, format);
	r = vasprintf(&string_ptr, format, p);
	va_end(p);
#else
	// Bloat for systems that haven't got the GNU extension.
	va_start(p, format);
	r = vsnprintf(NULL, 0, format, p);
	va_end(p);
	string_ptr = xmalloc(r+1);
	va_start(p, format);
	r = vsnprintf(string_ptr, r+1, format, p);
	va_end(p);
#endif

	if (r < 0)
		bb_error_msg_and_die(bb_msg_memory_exhausted);
	return string_ptr;
}

#if 0 /* If we will ever meet a libc which hasn't [f]dprintf... */
int FAST_FUNC fdprintf(int fd, const char *format, ...)
{
	va_list p;
	int r;
	char *string_ptr;

#if 1
	// GNU extension
	va_start(p, format);
	r = vasprintf(&string_ptr, format, p);
	va_end(p);
#else
	// Bloat for systems that haven't got the GNU extension.
	va_start(p, format);
	r = vsnprintf(NULL, 0, format, p) + 1;
	va_end(p);
	string_ptr = malloc(r);
	if (string_ptr) {
		va_start(p, format);
		r = vsnprintf(string_ptr, r, format, p);
		va_end(p);
	}
#endif

	if (r >= 0) {
		full_write(fd, string_ptr, r);
		free(string_ptr);
	}
	return r;
}
#endif

void FAST_FUNC xsetenv(const char *key, const char *value)
{
	if (setenv(key, value, 1))
		bb_error_msg_and_die(bb_msg_memory_exhausted);
}

/* Handles "VAR=VAL" strings, even those which are part of environ
 * _right now_
 */
void FAST_FUNC bb_unsetenv(const char *var)
{
	char *tp = strchr(var, '=');

	if (!tp) {
		unsetenv(var);
		return;
	}

	/* In case var was putenv'ed, we can't replace '='
	 * with NUL and unsetenv(var) - it won't work,
	 * env is modified by the replacement, unsetenv
	 * sees "VAR" instead of "VAR=VAL" and does not remove it!
	 * horror :( */
	tp = xstrndup(var, tp - var);
	unsetenv(tp);
	free(tp);
}


// Die with an error message if we can't set gid.  (Because resource limits may
// limit this user to a given number of processes, and if that fills up the
// setgid() will fail and we'll _still_be_root_, which is bad.)
void FAST_FUNC xsetgid(gid_t gid)
{
	if (setgid(gid)) bb_perror_msg_and_die("setgid");
}

// Die with an error message if we can't set uid.  (See xsetgid() for why.)
void FAST_FUNC xsetuid(uid_t uid)
{
	if (setuid(uid)) bb_perror_msg_and_die("setuid");
}

// Die if we can't chdir to a new path.
void FAST_FUNC xchdir(const char *path)
{
	if (chdir(path))
		bb_perror_msg_and_die("chdir(%s)", path);
}

void FAST_FUNC xchroot(const char *path)
{
	if (chroot(path))
		bb_perror_msg_and_die("can't change root directory to %s", path);
}

// Print a warning message if opendir() fails, but don't die.
DIR* FAST_FUNC warn_opendir(const char *path)
{
	DIR *dp;

	dp = opendir(path);
	if (!dp)
		bb_perror_msg("can't open '%s'", path);
	return dp;
}

// Die with an error message if opendir() fails.
DIR* FAST_FUNC xopendir(const char *path)
{
	DIR *dp;

	dp = opendir(path);
	if (!dp)
		bb_perror_msg_and_die("can't open '%s'", path);
	return dp;
}

// Die with an error message if we can't open a new socket.
int FAST_FUNC xsocket(int domain, int type, int protocol)
{
	int r = socket(domain, type, protocol);

	if (r < 0) {
		/* Hijack vaguely related config option */
#if ENABLE_VERBOSE_RESOLUTION_ERRORS
		const char *s = "INET";
		if (domain == AF_PACKET) s = "PACKET";
		if (domain == AF_NETLINK) s = "NETLINK";
USE_FEATURE_IPV6(if (domain == AF_INET6) s = "INET6";)
		bb_perror_msg_and_die("socket(AF_%s)", s);
#else
		bb_perror_msg_and_die("socket");
#endif
	}

	return r;
}

// Die with an error message if we can't bind a socket to an address.
void FAST_FUNC xbind(int sockfd, struct sockaddr *my_addr, socklen_t addrlen)
{
	if (bind(sockfd, my_addr, addrlen)) bb_perror_msg_and_die("bind");
}

// Die with an error message if we can't listen for connections on a socket.
void FAST_FUNC xlisten(int s, int backlog)
{
	if (listen(s, backlog)) bb_perror_msg_and_die("listen");
}

/* Die with an error message if sendto failed.
 * Return bytes sent otherwise  */
ssize_t FAST_FUNC xsendto(int s, const void *buf, size_t len, const struct sockaddr *to,
				socklen_t tolen)
{
	ssize_t ret = sendto(s, buf, len, 0, to, tolen);
	if (ret < 0) {
		if (ENABLE_FEATURE_CLEAN_UP)
			close(s);
		bb_perror_msg_and_die("sendto");
	}
	return ret;
}

// xstat() - a stat() which dies on failure with meaningful error message
void FAST_FUNC xstat(const char *name, struct stat *stat_buf)
{
	if (stat(name, stat_buf))
		bb_perror_msg_and_die("can't stat '%s'", name);
}

// selinux_or_die() - die if SELinux is disabled.
void FAST_FUNC selinux_or_die(void)
{
#if ENABLE_SELINUX
	int rc = is_selinux_enabled();
	if (rc == 0) {
		bb_error_msg_and_die("SELinux is disabled");
	} else if (rc < 0) {
		bb_error_msg_and_die("is_selinux_enabled() failed");
	}
#else
	bb_error_msg_and_die("SELinux support is disabled");
#endif
}

int FAST_FUNC ioctl_or_perror_and_die(int fd, unsigned request, void *argp, const char *fmt,...)
{
	int ret;
	va_list p;

	ret = ioctl(fd, request, argp);
	if (ret < 0) {
		va_start(p, fmt);
		bb_verror_msg(fmt, p/*, strerror(errno)*/);
		/* xfunc_die can actually longjmp, so be nice */
		va_end(p);
		xfunc_die();
	}
	return ret;
}

int FAST_FUNC ioctl_or_perror(int fd, unsigned request, void *argp, const char *fmt,...)
{
	va_list p;
	int ret = ioctl(fd, request, argp);

	if (ret < 0) {
		va_start(p, fmt);
		bb_verror_msg(fmt, p/*, strerror(errno)*/);
		va_end(p);
	}
	return ret;
}

#if ENABLE_IOCTL_HEX2STR_ERROR
int FAST_FUNC bb_ioctl_or_warn(int fd, unsigned request, void *argp, const char *ioctl_name)
{
	int ret;

	ret = ioctl(fd, request, argp);
	if (ret < 0)
		bb_simple_perror_msg(ioctl_name);
	return ret;
}
int FAST_FUNC bb_xioctl(int fd, unsigned request, void *argp, const char *ioctl_name)
{
	int ret;

	ret = ioctl(fd, request, argp);
	if (ret < 0)
		bb_simple_perror_msg_and_die(ioctl_name);
	return ret;
}
#else
int FAST_FUNC bb_ioctl_or_warn(int fd, unsigned request, void *argp)
{
	int ret;

	ret = ioctl(fd, request, argp);
	if (ret < 0)
		bb_perror_msg("ioctl %#x failed", request);
	return ret;
}
int FAST_FUNC bb_xioctl(int fd, unsigned request, void *argp)
{
	int ret;

	ret = ioctl(fd, request, argp);
	if (ret < 0)
		bb_perror_msg_and_die("ioctl %#x failed", request);
	return ret;
}
#endif

char* FAST_FUNC skip_whitespace(const char *s)
{
	/* NB: isspace('\0') returns 0 */
	while (isspace(*s)) ++s;

	return (char *) s;
}

char* FAST_FUNC skip_non_whitespace(const char *s)
{
	while (*s && !isspace(*s)) ++s;

	return (char *) s;
}
int die_sleep;
jmp_buf die_jmp;
int xfunc_error_retval = EXIT_FAILURE;

void FAST_FUNC xfunc_die(void)
{
	if (die_sleep) {
		if ((ENABLE_FEATURE_PREFER_APPLETS || ENABLE_HUSH)
		 && die_sleep < 0
		) {
			/* Special case. We arrive here if NOFORK applet
			 * calls xfunc, which then decides to die.
			 * We don't die, but jump instead back to caller.
			 * NOFORK applets still cannot carelessly call xfuncs:
			 * p = xmalloc(10);
			 * q = xmalloc(10); // BUG! if this dies, we leak p!
			 */
			/* -2222 means "zero" (longjmp can't pass 0)
			 * run_nofork_applet() catches -2222. */
			longjmp(die_jmp, xfunc_error_retval ? xfunc_error_retval : -2222);
		}
		sleep(die_sleep);
	}
	exit(xfunc_error_retval);
}

/* Resize (grow) malloced vector.
 *
 *  #define magic packed two parameters into one:
 *  sizeof = sizeof_and_shift >> 8
 *  shift  = (sizeof_and_shift) & 0xff
 *
 * Lets say shift = 4. 1 << 4 == 0x10.
 * If idx == 0, 0x10, 0x20 etc, vector[] is resized to next higher
 * idx step, plus one: if idx == 0x20, vector[] is resized to 0x31,
 * thus last usable element is vector[0x30].
 *
 * In other words: after xrealloc_vector(v, 4, idx) it's ok to use
 * at least v[idx] and v[idx+1], for all idx values.
 *
 * New elements are zeroed out, but only if realloc was done
 * (not on every call). You can depend on v[idx] and v[idx+1] being
 * zeroed out if you use it like this:
 *  v = xrealloc_vector(v, 4, idx);
 *  v[idx].some_fields = ...; - the rest stays 0/NULL
 *  idx++;
 * If you do not advance idx like above, you should be more careful.
 * Next call to xrealloc_vector(v, 4, idx) may or may not zero out v[idx].
 */
void* FAST_FUNC xrealloc_vector_helper(void *vector, unsigned sizeof_and_shift, int idx)
{
	int mask = 1 << (uint8_t)sizeof_and_shift;

	if (!(idx & (mask - 1))) {
		sizeof_and_shift >>= 8; /* sizeof(vector[0]) */
		vector = xrealloc(vector, sizeof_and_shift * (idx + mask + 1));
		memset((char*)vector + (sizeof_and_shift * idx), 0, sizeof_and_shift * (mask + 1));
	}
	return vector;
}

void FAST_FUNC bb_simple_perror_msg(const char *s)
{
	bb_perror_msg("%s", s);
}
FILE* FAST_FUNC fopen_or_warn(const char *path, const char *mode)
{
	FILE *fp = fopen(path, mode);
	if (!fp) {
		bb_simple_perror_msg(path);
		//errno = 0; /* why? */
	}
	return fp;
}

FILE* FAST_FUNC fopen_for_read(const char *path)
{
	return fopen(path, "r");
}

FILE* FAST_FUNC xfopen_for_read(const char *path)
{
	return xfopen(path, "r");
}

FILE* FAST_FUNC fopen_for_write(const char *path)
{
	return fopen(path, "w");
}

FILE* FAST_FUNC xfopen_for_write(const char *path)
{
	return xfopen(path, "w");
}

// Convert unsigned integer to ascii, writing into supplied buffer.
// A truncated result contains the first few digits of the result ala strncpy.
// Returns a pointer past last generated digit, does _not_ store NUL.
void BUG_sizeof_unsigned_not_4(void);
char* FAST_FUNC utoa_to_buf(unsigned n, char *buf, unsigned buflen)
{
	unsigned i, out, res;
	if (sizeof(unsigned) != 4)
		BUG_sizeof_unsigned_not_4();
	if (buflen) {
		out = 0;
		for (i = 1000000000; i; i /= 10) {
			res = n / i;
			if (res || out || i == 1) {
				if (!--buflen) break;
				out++;
				n -= res*i;
				*buf++ = '0' + res;
			}
		}
	}
	return buf;
}

static char local_buf[sizeof(int) * 3];
// Convert unsigned integer to ascii using a static buffer (returned).
char* FAST_FUNC utoa(unsigned n)
{
	*(utoa_to_buf(n, local_buf, sizeof(local_buf))) = '\0';

	return local_buf;
}

char* FAST_FUNC uid2uname(uid_t uid)
{
	struct passwd *pw = getpwuid(uid);
	return (pw) ? pw->pw_name : NULL;
}
char* FAST_FUNC uid2uname_utoa(long uid)
{
	char *name = uid2uname(uid);
	return (name) ? name : utoa(uid);
}

char* FAST_FUNC gid2group(gid_t gid)
{
	struct group *gr = getgrgid(gid);
	return (gr) ? gr->gr_name : NULL;
}

char* FAST_FUNC gid2group_utoa(long gid)
{
	char *name = gid2group(gid);
	return (name) ? name : utoa(gid);
}
typedef struct unsigned_to_name_map_t {
	long id;
	char name[USERNAME_MAX_SIZE];
} unsigned_to_name_map_t;

typedef struct cache_t {
	unsigned_to_name_map_t *cache;
	int size;
} cache_t;

static cache_t username, groupname;

static void clear_cache(cache_t *cp)
{
	free(cp->cache);
	cp->cache = NULL;
	cp->size = 0;
}
void FAST_FUNC clear_username_cache(void)
{
	clear_cache(&username);
	clear_cache(&groupname);
}

#if 0 /* more generic, but we don't need that yet */
/* Returns -N-1 if not found. */
/* cp->cache[N] is allocated and must be filled in this case */
static int get_cached(cache_t *cp, unsigned id)
{
	int i;
	for (i = 0; i < cp->size; i++)
		if (cp->cache[i].id == id)
			return i;
	i = cp->size++;
	cp->cache = xrealloc_vector(cp->cache, 2, i);
	cp->cache[i++].id = id;
	return -i;
}
#endif

static char* get_cached(cache_t *cp, long id,
			char* FAST_FUNC x2x_utoa(long id))
{
	int i;
	for (i = 0; i < cp->size; i++)
		if (cp->cache[i].id == id)
			return cp->cache[i].name;
	i = cp->size++;
	cp->cache = xrealloc_vector(cp->cache, 2, i);
	cp->cache[i].id = id;
	/* Never fails. Generates numeric string if name isn't found */
	safe_strncpy(cp->cache[i].name, x2x_utoa(id), sizeof(cp->cache[i].name));
	return cp->cache[i].name;
}
const char* FAST_FUNC get_cached_username(uid_t uid)
{
	return get_cached(&username, uid, uid2uname_utoa);
}
const char* FAST_FUNC get_cached_groupname(gid_t gid)
{
	return get_cached(&groupname, gid, gid2group_utoa);
}


#define PROCPS_BUFSIZE 1024

static int read_to_buf(const char *filename, void *buf)
{
	int fd;
	/* open_read_close() would do two reads, checking for EOF.
	 * When you have 10000 /proc/$NUM/stat to read, it isn't desirable */
	ssize_t ret = -1;
	fd = open(filename, O_RDONLY);
	if (fd >= 0) {
		ret = read(fd, buf, PROCPS_BUFSIZE-1);
		close(fd);
	}
	((char *)buf)[ret > 0 ? ret : 0] = '\0';
	return ret;
}

static procps_status_t* FAST_FUNC alloc_procps_scan(void)
{
	unsigned n = getpagesize();
	procps_status_t* sp = xzalloc(sizeof(procps_status_t));
	sp->dir = xopendir("/proc");
	while (1) {
		n >>= 1;
		if (!n) break;
		sp->shift_pages_to_bytes++;
	}
	sp->shift_pages_to_kb = sp->shift_pages_to_bytes - 10;
	return sp;
}

void FAST_FUNC free_procps_scan(procps_status_t* sp)
{
	closedir(sp->dir);
	free(sp->argv0);
	USE_SELINUX(free(sp->context);)
	free(sp);
}

#if ENABLE_FEATURE_TOPMEM
static unsigned long fast_strtoul_16(char **endptr)
{
	unsigned char c;
	char *str = *endptr;
	unsigned long n = 0;

	while ((c = *str++) != ' ') {
		c = ((c|0x20) - '0');
		if (c > 9)
			// c = c + '0' - 'a' + 10:
			c = c - ('a' - '0' - 10);
		n = n*16 + c;
	}
	*endptr = str; /* We skip trailing space! */
	return n;
}
/* TOPMEM uses fast_strtoul_10, so... */
#undef ENABLE_FEATURE_FAST_TOP
#define ENABLE_FEATURE_FAST_TOP 1
#endif

#if ENABLE_FEATURE_FAST_TOP
/* We cut a lot of corners here for speed */
static unsigned long fast_strtoul_10(char **endptr)
{
	char c;
	char *str = *endptr;
	unsigned long n = *str - '0';

	while ((c = *++str) != ' ')
		n = n*10 + (c - '0');

	*endptr = str + 1; /* We skip trailing space! */
	return n;
}
static char *skip_fields(char *str, int count)
{
	do {
		while (*str++ != ' ')
			continue;
		/* we found a space char, str points after it */
	} while (--count);
	return str;
}
#endif

static unsigned long long ret_ERANGE(void)
{
	errno = ERANGE; /* this ain't as small as it looks (on glibc) */
	return ULLONG_MAX;
}
static unsigned long long handle_errors(unsigned long long v, char **endp, char *endptr)
{
	if (endp) *endp = endptr;

	/* errno is already set to ERANGE by strtoXXX if value overflowed */
	if (endptr[0]) {
		/* "1234abcg" or out-of-range? */
		if (isalnum(endptr[0]) || errno)
			return ret_ERANGE();
		/* good number, just suspicious terminator */
		errno = EINVAL;
	}
	return v;
}
#if ULONG_MAX == ULLONG_MAX

unsigned long bb_strtoul(const char *arg, char **endp, int base)
{ return bb_strtoull(arg, endp, base); }
static 
long bb_strtol(const char *arg, char **endp, int base)
{ return bb_strtoll(arg, endp, base); }
#else
long bb_strtol(const char *arg, char **endp, int base) FAST_FUNC;

unsigned long FAST_FUNC bb_strtoul(const char *arg, char **endp, int base)
{
	unsigned long v;
	char *endptr;

	if (!isalnum(arg[0])) return ret_ERANGE();
	errno = 0;
	v = strtoul(arg, &endptr, base);
	return handle_errors(v, endp, endptr);
}
#endif
#if UINT_MAX != ULONG_MAX
unsigned FAST_FUNC bb_strtou(const char *arg, char **endp, int base)
{
	unsigned long v;
	char *endptr;

	if (!isalnum(arg[0])) return ret_ERANGE();
	errno = 0;
	v = strtoul(arg, &endptr, base);
	if (v > UINT_MAX) return ret_ERANGE();
	return handle_errors(v, endp, endptr);
}

#if 0
int FAST_FUNC bb_strtoi(const char *arg, char **endp, int base)
{
	long v;
	char *endptr;

	char first = (arg[0] != '-' ? arg[0] : arg[1]);
	if (!isalnum(first)) return ret_ERANGE();

	errno = 0;
	v = strtol(arg, &endptr, base);
	if (v > INT_MAX) return ret_ERANGE();
	if (v < INT_MIN) return ret_ERANGE();
	return handle_errors(v, endp, endptr);
}
#endif
#else
static 
unsigned bb_strtou(const char *arg, char **endp, int base)
{ return bb_strtoul(arg, endp, base); }
static 
int bb_strtoi(const char *arg, char **endp, int base)
{ return bb_strtol(arg, endp, base); }
#endif
void BUG_comm_size(void);
procps_status_t* FAST_FUNC procps_scan(procps_status_t* sp, int flags)
{
	struct dirent *entry;
	char buf[PROCPS_BUFSIZE];
	char filename[sizeof("/proc//cmdline") + sizeof(int)*3];
	char *filename_tail;
	long tasknice;
	unsigned pid;
	int n;
	struct stat sb;

	if (!sp)
		sp = alloc_procps_scan();

	for (;;) {
		entry = readdir(sp->dir);
		if (entry == NULL) {
			free_procps_scan(sp);
			return NULL;
		}
		pid = bb_strtou(entry->d_name, NULL, 10);
		if (errno)
			continue;

		/* After this point we have to break, not continue
		 * ("continue" would mean that current /proc/NNN
		 * is not a valid process info) */

		memset(&sp->vsz, 0, sizeof(*sp) - offsetof(procps_status_t, vsz));

		sp->pid = pid;
		if (!(flags & ~PSSCAN_PID)) break;

#if ENABLE_SELINUX
		if (flags & PSSCAN_CONTEXT) {
			if (getpidcon(sp->pid, &sp->context) < 0)
				sp->context = NULL;
		}
#endif

		filename_tail = filename + sprintf(filename, "/proc/%d", pid);

		if (flags & PSSCAN_UIDGID) {
			if (stat(filename, &sb))
				break;
			/* Need comment - is this effective or real UID/GID? */
			sp->uid = sb.st_uid;
			sp->gid = sb.st_gid;
		}

		if (flags & PSSCAN_STAT) {
			char *cp, *comm1;
			int tty;
#if !ENABLE_FEATURE_FAST_TOP
			unsigned long vsz, rss;
#endif
			/* see proc(5) for some details on this */
			strcpy(filename_tail, "/stat");
			n = read_to_buf(filename, buf);
			if (n < 0)
				break;
			cp = strrchr(buf, ')'); /* split into "PID (cmd" and "<rest>" */
			/*if (!cp || cp[1] != ' ')
				break;*/
			cp[0] = '\0';
			if (sizeof(sp->comm) < 16)
				BUG_comm_size();
			comm1 = strchr(buf, '(');
			/*if (comm1)*/
				safe_strncpy(sp->comm, comm1 + 1, sizeof(sp->comm));

#if !ENABLE_FEATURE_FAST_TOP
			n = sscanf(cp+2,
				"%c %u "               /* state, ppid */
				"%u %u %d %*s "        /* pgid, sid, tty, tpgid */
				"%*s %*s %*s %*s %*s " /* flags, min_flt, cmin_flt, maj_flt, cmaj_flt */
				"%lu %lu "             /* utime, stime */
				"%*s %*s %*s "         /* cutime, cstime, priority */
				"%ld "                 /* nice */
				"%*s %*s "             /* timeout, it_real_value */
				"%lu "                 /* start_time */
				"%lu "                 /* vsize */
				"%lu "                 /* rss */
#if ENABLE_FEATURE_TOP_SMP_PROCESS
				"%*s %*s %*s %*s %*s %*s " /*rss_rlim, start_code, end_code, start_stack, kstk_esp, kstk_eip */
				"%*s %*s %*s %*s "         /*signal, blocked, sigignore, sigcatch */
				"%*s %*s %*s %*s "         /*wchan, nswap, cnswap, exit_signal */
				"%d"                       /*cpu last seen on*/
#endif
				,
				sp->state, &sp->ppid,
				&sp->pgid, &sp->sid, &tty,
				&sp->utime, &sp->stime,
				&tasknice,
				&sp->start_time,
				&vsz,
				&rss
#if ENABLE_FEATURE_TOP_SMP_PROCESS
				, &sp->last_seen_on_cpu
#endif
				);

			if (n < 11)
				break;
#if ENABLE_FEATURE_TOP_SMP_PROCESS
			if (n < 11+15)
				sp->last_seen_on_cpu = 0;
#endif

			/* vsz is in bytes and we want kb */
			sp->vsz = vsz >> 10;
			/* vsz is in bytes but rss is in *PAGES*! Can you believe that? */
			sp->rss = rss << sp->shift_pages_to_kb;
			sp->tty_major = (tty >> 8) & 0xfff;
			sp->tty_minor = (tty & 0xff) | ((tty >> 12) & 0xfff00);
#else
/* This costs ~100 bytes more but makes top faster by 20%
 * If you run 10000 processes, this may be important for you */
			sp->state[0] = cp[2];
			cp += 4;
			sp->ppid = fast_strtoul_10(&cp);
			sp->pgid = fast_strtoul_10(&cp);
			sp->sid = fast_strtoul_10(&cp);
			tty = fast_strtoul_10(&cp);
			sp->tty_major = (tty >> 8) & 0xfff;
			sp->tty_minor = (tty & 0xff) | ((tty >> 12) & 0xfff00);
			cp = skip_fields(cp, 6); /* tpgid, flags, min_flt, cmin_flt, maj_flt, cmaj_flt */
			sp->utime = fast_strtoul_10(&cp);
			sp->stime = fast_strtoul_10(&cp);
			cp = skip_fields(cp, 3); /* cutime, cstime, priority */
			tasknice = fast_strtoul_10(&cp);
			cp = skip_fields(cp, 2); /* timeout, it_real_value */
			sp->start_time = fast_strtoul_10(&cp);
			/* vsz is in bytes and we want kb */
			sp->vsz = fast_strtoul_10(&cp) >> 10;
			/* vsz is in bytes but rss is in *PAGES*! Can you believe that? */
			sp->rss = fast_strtoul_10(&cp) << sp->shift_pages_to_kb;
#if ENABLE_FEATURE_TOP_SMP_PROCESS
			/* (6): rss_rlim, start_code, end_code, start_stack, kstk_esp, kstk_eip */
			/* (4): signal, blocked, sigignore, sigcatch */
			/* (4): wchan, nswap, cnswap, exit_signal */
			cp = skip_fields(cp, 14);
//FIXME: is it safe to assume this field exists?
			sp->last_seen_on_cpu = fast_strtoul_10(&cp);
#endif
#endif /* end of !ENABLE_FEATURE_TOP_SMP_PROCESS */

			if (sp->vsz == 0 && sp->state[0] != 'Z')
				sp->state[1] = 'W';
			else
				sp->state[1] = ' ';
			if (tasknice < 0)
				sp->state[2] = '<';
			else if (tasknice) /* > 0 */
				sp->state[2] = 'N';
			else
				sp->state[2] = ' ';
		}

#if ENABLE_FEATURE_TOPMEM
		if (flags & (PSSCAN_SMAPS)) {
			FILE *file;

			strcpy(filename_tail, "/smaps");
			file = fopen_for_read(filename);
			if (!file)
				break;
			while (fgets(buf, sizeof(buf), file)) {
				unsigned long sz;
				char *tp;
				char w;
#define SCAN(str, name) \
	if (strncmp(buf, str, sizeof(str)-1) == 0) { \
		tp = skip_whitespace(buf + sizeof(str)-1); \
		sp->name += fast_strtoul_10(&tp); \
		continue; \
	}
				SCAN("Shared_Clean:" , shared_clean );
				SCAN("Shared_Dirty:" , shared_dirty );
				SCAN("Private_Clean:", private_clean);
				SCAN("Private_Dirty:", private_dirty);
#undef SCAN
				// f7d29000-f7d39000 rw-s ADR M:m OFS FILE
				tp = strchr(buf, '-');
				if (tp) {
					*tp = ' ';
					tp = buf;
					sz = fast_strtoul_16(&tp); /* start */
					sz = (fast_strtoul_16(&tp) - sz) >> 10; /* end - start */
					// tp -> "rw-s" string
					w = tp[1];
					// skipping "rw-s ADR M:m OFS "
					tp = skip_whitespace(skip_fields(tp, 4));
					// filter out /dev/something (something != zero)
					if (strncmp(tp, "/dev/", 5) != 0 || strcmp(tp, "/dev/zero\n") == 0) {
						if (w == 'w') {
							sp->mapped_rw += sz;
						} else if (w == '-') {
							sp->mapped_ro += sz;
						}
					}
//else printf("DROPPING %s (%s)\n", buf, tp);
					if (strcmp(tp, "[stack]\n") == 0)
						sp->stack += sz;
				}
			}
			fclose(file);
		}
#endif /* TOPMEM */

#if 0 /* PSSCAN_CMD is not used */
		if (flags & (PSSCAN_CMD|PSSCAN_ARGV0)) {
			free(sp->argv0);
			sp->argv0 = NULL;
			free(sp->cmd);
			sp->cmd = NULL;
			strcpy(filename_tail, "/cmdline");
			/* TODO: to get rid of size limits, read into malloc buf,
			 * then realloc it down to real size. */
			n = read_to_buf(filename, buf);
			if (n <= 0)
				break;
			if (flags & PSSCAN_ARGV0)
				sp->argv0 = xstrdup(buf);
			if (flags & PSSCAN_CMD) {
				do {
					n--;
					if ((unsigned char)(buf[n]) < ' ')
						buf[n] = ' ';
				} while (n);
				sp->cmd = xstrdup(buf);
			}
		}
#else
		if (flags & (PSSCAN_ARGV0|PSSCAN_ARGVN)) {
			free(sp->argv0);
			sp->argv0 = NULL;
			strcpy(filename_tail, "/cmdline");
			n = read_to_buf(filename, buf);
			if (n <= 0)
				break;
			if (flags & PSSCAN_ARGVN) {
				sp->argv_len = n;
				sp->argv0 = xmalloc(n + 1);
				memcpy(sp->argv0, buf, n + 1);
				/* sp->argv0[n] = '\0'; - buf has it */
			} else {
				sp->argv_len = 0;
				sp->argv0 = xstrdup(buf);
			}
		}
#endif
		break;
	}
	return sp;
}

ssize_t FAST_FUNC safe_read(int fd, void *buf, size_t count)
{
	ssize_t n;

	do {
		n = read(fd, buf, count);
	} while (n < 0 && errno == EINTR);

	return n;
}

/*
 * Read all of the supplied buffer from a file.
 * This does multiple reads as necessary.
 * Returns the amount read, or -1 on an error.
 * A short read is returned on an end of file.
 */
ssize_t FAST_FUNC full_read(int fd, void *buf, size_t len)
{
	ssize_t cc;
	ssize_t total;

	total = 0;

	while (len) {
		cc = safe_read(fd, buf, len);

		if (cc < 0) {
			if (total) {
				/* we already have some! */
				/* user can do another read to know the error code */
				return total;
			}
			return cc; /* read() returns -1 on failure. */
		}
		if (cc == 0)
			break;
		buf = ((char *)buf) + cc;
		total += cc;
		len -= cc;
	}

	return total;
}
ssize_t FAST_FUNC read_close(int fd, void *buf, size_t size)
{
	/*int e;*/
	size = full_read(fd, buf, size);
	/*e = errno;*/
	close(fd);
	/*errno = e;*/
	return size;
}

ssize_t FAST_FUNC open_read_close(const char *filename, void *buf, size_t size)
{
	int fd = open(filename, O_RDONLY);
	if (fd < 0)
		return fd;
	return read_close(fd, buf, size);
}
void FAST_FUNC read_cmdline(char *buf, int col, unsigned pid, const char *comm)
{
	ssize_t sz;
	char filename[sizeof("/proc//cmdline") + sizeof(int)*3];

	sprintf(filename, "/proc/%u/cmdline", pid);
	sz = open_read_close(filename, buf, col);
	if (sz > 0) {
		buf[sz] = '\0';
		while (--sz >= 0)
			if ((unsigned char)(buf[sz]) < ' ')
				buf[sz] = ' ';
	} else {
		snprintf(buf, col, "[%s]", comm);
	}
}

/* from kernel:
	//             pid comm S ppid pgid sid tty_nr tty_pgrp flg
	sprintf(buffer,"%d (%s) %c %d  %d   %d  %d     %d       %lu %lu \
%lu %lu %lu %lu %lu %ld %ld %ld %ld %d 0 %llu %lu %ld %lu %lu %lu %lu %lu \
%lu %lu %lu %lu %lu %lu %lu %lu %d %d %lu %lu %llu\n",
		task->pid,
		tcomm,
		state,
		ppid,
		pgid,
		sid,
		tty_nr,
		tty_pgrp,
		task->flags,
		min_flt,
		cmin_flt,
		maj_flt,
		cmaj_flt,
		cputime_to_clock_t(utime),
		cputime_to_clock_t(stime),
		cputime_to_clock_t(cutime),
		cputime_to_clock_t(cstime),
		priority,
		nice,
		num_threads,
		// 0,
		start_time,
		vsize,
		mm ? get_mm_rss(mm) : 0,
		rsslim,
		mm ? mm->start_code : 0,
		mm ? mm->end_code : 0,
		mm ? mm->start_stack : 0,
		esp,
		eip,
the rest is some obsolete cruft
*/
