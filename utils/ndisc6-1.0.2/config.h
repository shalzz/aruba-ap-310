/* config.h.  Generated from config.h.in by configure.  */
/* config.h.in.  Generated from configure.ac by autoheader.  */

/* Define to 1 if translation of program messages to the user's native
   language is requested. */
/* #undef ENABLE_NLS */

/* Define to 1 if you have the MacOS X function CFLocaleCopyCurrent in the
   CoreFoundation framework. */
/* #undef HAVE_CFLOCALECOPYCURRENT */

/* Define to 1 if you have the MacOS X function CFPreferencesCopyAppValue in
   the CoreFoundation framework. */
/* #undef HAVE_CFPREFERENCESCOPYAPPVALUE */

/* Define to 1 if you have the `clock_nanosleep' function. */
/* #undef HAVE_CLOCK_NANOSLEEP */

/* Define if the GNU dcgettext() function is already present or preinstalled.
   */
/* #undef HAVE_DCGETTEXT */

/* Define to 1 if you have the `fdatasync' function. */
#define HAVE_FDATASYNC 1

/* Define to 1 if you have the <getopt.h> header file. */
#define HAVE_GETOPT_H 1

/* Define to 1 if you have the `getopt_long' function. */
#define HAVE_GETOPT_LONG 1

/* Define if the GNU gettext() function is already present or preinstalled. */
/* #undef HAVE_GETTEXT */

/* Define if you have the iconv() function and it works. */
/* #undef HAVE_ICONV */

/* Define to 1 if you have the `inet6_rth_add' function. */
/* #undef HAVE_INET6_RTH_ADD */

/* Define to 1 if you have the <inttypes.h> header file. */
#define HAVE_INTTYPES_H 1

/* Define to 1 if you have the <memory.h> header file. */
#define HAVE_MEMORY_H 1

/* Define to 1 if you have the `ppoll' function. */
/* #undef HAVE_PPOLL */

/* Define to 1 if you have the <stdint.h> header file. */
#define HAVE_STDINT_H 1

/* Define to 1 if you have the <stdlib.h> header file. */
#define HAVE_STDLIB_H 1

/* Define to 1 if you have the <strings.h> header file. */
#define HAVE_STRINGS_H 1

/* Define to 1 if you have the <string.h> header file. */
#define HAVE_STRING_H 1

/* Define to 1 if you have the <sys/stat.h> header file. */
#define HAVE_SYS_STAT_H 1

/* Define to 1 if you have the <sys/types.h> header file. */
#define HAVE_SYS_TYPES_H 1

/* Define to 1 if you have the <unistd.h> header file. */
#define HAVE_UNISTD_H 1

/* Define to 1 if assertions should be disabled. */
#define NDEBUG 1

/* Define to 1 if your C compiler doesn't accept -c and -o together. */
/* #undef NO_MINUS_C_MINUS_O */

/* Name of package */
#define PACKAGE "ndisc6"

/* Define to the address where bug reports for this package should be sent. */
/* #define PACKAGE_BUGREPORT "remi_no_bulk_mail@remlab.net" */

/* Define to the hostname of the host who builds the package. */
/* #define PACKAGE_BUILD_HOSTNAME "pekdev-build.arubanetworks.com" */

/* Define to the full name of this package. */
#define PACKAGE_NAME "ndisc6"

/* Define to the full name and version of this package. */
#define PACKAGE_STRING "ndisc6 1.0.2"

/* Define to the one symbol short name of this package. */
#define PACKAGE_TARNAME "ndisc6"

/* Define to the home page for this package. */
/* #define PACKAGE_URL "" */

/* Define to the version of this package. */
#define PACKAGE_VERSION "1.0.2"

/* Define to 1 if you have the ANSI C header files. */
#define STDC_HEADERS 1

/* Enable extensions on AIX 3, Interix.  */
#ifndef _ALL_SOURCE
# define _ALL_SOURCE 1
#endif
/* Enable GNU extensions on systems that have them.  */
#ifndef _GNU_SOURCE
# define _GNU_SOURCE 1
#endif
/* Enable threading extensions on Solaris.  */
#ifndef _POSIX_PTHREAD_SEMANTICS
# define _POSIX_PTHREAD_SEMANTICS 1
#endif
/* Enable extensions on HP NonStop.  */
#ifndef _TANDEM_SOURCE
# define _TANDEM_SOURCE 1
#endif
/* Enable general extensions on Solaris.  */
#ifndef __EXTENSIONS__
# define __EXTENSIONS__ 1
#endif


/* Version number of package */
#define VERSION "1.0.2"

/* Define to 1 if on MINIX. */
/* #undef _MINIX */

/* Define to 2 if the system does not provide POSIX.1 features except with
   this defined. */
/* #undef _POSIX_1_SOURCE */

/* Define to 1 if you need to in order for `stat' and other things to work. */
/* #undef _POSIX_SOURCE */

/* Fallback replacement for GNU `getopt_long' */
#ifndef HAVE_GETOPT_LONG
# define getopt_long( argc, argv, optstring, longopts, longindex ) \
	getopt (argc, argv, optstring)
# if !GETOPT_STRUCT_OPTION && !HAVE_GETOPT_H
 struct option { const char *name; int has_arg; int *flag; int val; };
#  define GETOPT_STRUCT_OPTION 1
# endif
# ifndef required_argument
#  define no_argument 0
#  define required_argument 1
#  define optional_argument 2
# endif
#endif


#include "compat/fixups.h"

#ifndef HAVE_CLOCK_NANOSLEEP
# define clock_nanosleep( c, f, d, r ) nanosleep( d, r )
#endif

#define _( str )		gettext (str)
#define N_( str )		gettext_noop (str)

#define LOCALSTATEDIR "/var"
#define LOCALEDIR "/usr/share/local"
