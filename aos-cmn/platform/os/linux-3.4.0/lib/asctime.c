#if 0
#include <stdio.h>
#endif
#include <linux/types.h>
#include <linux/version.h>
#include <linux/module.h>
#include "linux/rtc.h"
#include "aruba_langinfo.h"

#if 0
typedef long off_t;
typedef u_int32_t uint32_t;
#endif

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,32))
#if 0
struct tm
{
  int tm_sec;                   /* Seconds.     [0-60] (1 leap second) */
  int tm_min;                   /* Minutes.     [0-59] */
  int tm_hour;                  /* Hours.       [0-23] */
  int tm_mday;                  /* Day.         [1-31] */
  int tm_mon;                   /* Month.       [0-11] */
  int tm_year;                  /* Year - 1900.  */
  int tm_wday;                  /* Day of week. [0-6] */
  int tm_yday;                  /* Days in year.[0-365] */
  int tm_isdst;                 /* DST.         [-1/0/1]*/
};
#endif

const unsigned short int __mon_yday[2][13] =
  {
    /* Normal years.  */
    { 0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334, 365 },
    /* Leap years.  */
    { 0, 31, 60, 91, 121, 152, 182, 213, 244, 274, 305, 335, 366 }
  };

#define	SECS_PER_HOUR	(60 * 60)
#define	SECS_PER_DAY	(SECS_PER_HOUR * 24)
# define __isleap(year)	\
  ((year) % 4 == 0 && ((year) % 100 != 0 || (year) % 400 == 0))

int
aruba_offtime (time_t *t, long int offset, struct rtc_time *tp)
{
  long int days, rem, y;
  const unsigned short int *ip;

  days = *t / SECS_PER_DAY;
  rem = *t % SECS_PER_DAY;
  rem += offset;
  while (rem < 0)
    {
      rem += SECS_PER_DAY;
      --days;
    }
  while (rem >= SECS_PER_DAY)
    {
      rem -= SECS_PER_DAY;
      ++days;
    }
  tp->tm_hour = rem / SECS_PER_HOUR;
  rem %= SECS_PER_HOUR;
  tp->tm_min = rem / 60;
  tp->tm_sec = rem % 60;
  /* January 1, 1970 was a Thursday.  */
  tp->tm_wday = (4 + days) % 7;
  if (tp->tm_wday < 0)
    tp->tm_wday += 7;
  y = 1970;

#define DIV(a, b) ((a) / (b) - ((a) % (b) < 0))
#define LEAPS_THRU_END_OF(y) (DIV (y, 4) - DIV (y, 100) + DIV (y, 400))

  while (days < 0 || days >= (__isleap (y) ? 366 : 365))
    {
      /* Guess a corrected year, assuming 365 days per year.  */
      long int yg = y + days / 365 - (days % 365 < 0);

      /* Adjust DAYS and Y to match the guessed year.  */
      days -= ((yg - y) * 365
	       + LEAPS_THRU_END_OF (yg - 1)
	       - LEAPS_THRU_END_OF (y - 1));
      y = yg;
    }
  tp->tm_year = y;
  if (tp->tm_year != y)
    {
      return 0;
    }
  tp->tm_yday = days;
  ip = __mon_yday[__isleap(y)];
  for (y = 11; days < (long int) ip[y]; --y)
    continue;
  days -= ip[y];
  tp->tm_mon = y;
  tp->tm_mday = days + 1;
  return 1;
}
#endif

#define _NL_ITEM_INDEX(item)		((int) (item) & 0xffff)

struct locale_data
{
  const char *name;
  const char *filedata;		/* Region mapping the file data.  */
  off_t filesize;		/* Size of the file (and the region).  */
  enum				/* Flavor of storage used for those.  */
  {
    ld_malloced,		/* Both are malloc'd.  */
    ld_mapped,			/* name is malloc'd, filedata mmap'd */
    ld_archive			/* Both point into mmap'd archive regions.  */
  } alloc;

  /* This provides a slot for category-specific code to cache data computed
     about this locale.  That code can set a cleanup function to deallocate
     the data.  */
  struct
  {
    void (*cleanup) (struct locale_data *);
    union
    {
      void *data;
    };
  } private;

  unsigned int usage_count;	/* Counter for users.  */

  int use_translit;		/* Nonzero if the mb*towv*() and wc*tomb()
				   functions should use transliteration.  */

  unsigned int nstrings;	/* Number of strings below.  */
        union locale_data_value
        {
            const uint32_t *wstr;
            const char *string;
            unsigned int word;		/* Note endian issues vs 64-bit pointers.  */
        }
        values[];	/* Items, usually pointers into `filedata'.  */
};

const struct locale_data _nl_C_LC_TIME =
{
  "C",
  NULL, 0, 0,			/* no file mapped */
  { NULL, },			/* no cached data */
  0xFFFFFFFF,
  0,
  111,
  {
    { .string = "Sun" },
    { .string = "Mon" },
    { .string = "Tue" },
    { .string = "Wed" },
    { .string = "Thu" },
    { .string = "Fri" },
    { .string = "Sat" },
    { .string = "Sunday" },
    { .string = "Monday" },
    { .string = "Tuesday" },
    { .string = "Wednesday" },
    { .string = "Thursday" },
    { .string = "Friday" },
    { .string = "Saturday" },
    { .string = "Jan" },
    { .string = "Feb" },
    { .string = "Mar" },
    { .string = "Apr" },
    { .string = "May" },
    { .string = "Jun" },
    { .string = "Jul" },
    { .string = "Aug" },
    { .string = "Sep" },
    { .string = "Oct" },
    { .string = "Nov" },
    { .string = "Dec" },
    { .string = "January" },
    { .string = "February" },
    { .string = "March" },
    { .string = "April" },
    { .string = "May" },
    { .string = "June" },
    { .string = "July" },
    { .string = "August" },
    { .string = "September" },
    { .string = "October" },
    { .string = "November" },
    { .string = "December" },
    { .string = "AM" },
    { .string = "PM" },
    { .string = "%a %b %e %H:%M:%S %Y" },
    { .string = "%m/%d/%y" },
    { .string = "%H:%M:%S" },
    { .string = "%I:%M:%S %p" },
    { .string = "" },
    { .string = "" },
    { .string = "" },
    { .string = "" },
    { .string = "" },
    { .string = "" },
    { .word = 0 },
    { .string = "" },
    { .wstr = (const uint32_t *) L"Sun" },
    { .wstr = (const uint32_t *) L"Mon" },
    { .wstr = (const uint32_t *) L"Tue" },
    { .wstr = (const uint32_t *) L"Wed" },
    { .wstr = (const uint32_t *) L"Thu" },
    { .wstr = (const uint32_t *) L"Fri" },
    { .wstr = (const uint32_t *) L"Sat" },
    { .wstr = (const uint32_t *) L"Sunday" },
    { .wstr = (const uint32_t *) L"Monday" },
    { .wstr = (const uint32_t *) L"Tuesday" },
    { .wstr = (const uint32_t *) L"Wednesday" },
    { .wstr = (const uint32_t *) L"Thursday" },
    { .wstr = (const uint32_t *) L"Friday" },
    { .wstr = (const uint32_t *) L"Saturday" },
    { .wstr = (const uint32_t *) L"Jan" },
    { .wstr = (const uint32_t *) L"Feb" },
    { .wstr = (const uint32_t *) L"Mar" },
    { .wstr = (const uint32_t *) L"Apr" },
    { .wstr = (const uint32_t *) L"May" },
    { .wstr = (const uint32_t *) L"Jun" },
    { .wstr = (const uint32_t *) L"Jul" },
    { .wstr = (const uint32_t *) L"Aug" },
    { .wstr = (const uint32_t *) L"Sep" },
    { .wstr = (const uint32_t *) L"Oct" },
    { .wstr = (const uint32_t *) L"Nov" },
    { .wstr = (const uint32_t *) L"Dec" },
    { .wstr = (const uint32_t *) L"January" },
    { .wstr = (const uint32_t *) L"February" },
    { .wstr = (const uint32_t *) L"March" },
    { .wstr = (const uint32_t *) L"April" },
    { .wstr = (const uint32_t *) L"May" },
    { .wstr = (const uint32_t *) L"June" },
    { .wstr = (const uint32_t *) L"July" },
    { .wstr = (const uint32_t *) L"August" },
    { .wstr = (const uint32_t *) L"September" },
    { .wstr = (const uint32_t *) L"October" },
    { .wstr = (const uint32_t *) L"November" },
    { .wstr = (const uint32_t *) L"December" },
    { .wstr = (const uint32_t *) L"AM" },
    { .wstr = (const uint32_t *) L"PM" },
    { .wstr = (const uint32_t *) L"%a %b %e %H:%M:%S %Y" },
    { .wstr = (const uint32_t *) L"%m/%d/%y" },
    { .wstr = (const uint32_t *) L"%H:%M:%S" },
    { .wstr = (const uint32_t *) L"%I:%M:%S %p" },
    { .wstr = (const uint32_t *) L"" },
    { .wstr = (const uint32_t *) L"" },
    { .wstr = (const uint32_t *) L"" },
    { .wstr = (const uint32_t *) L"" },
    { .wstr = (const uint32_t *) L"" },
    { .string = "\7" },
    { .word = 19971130 },
    { .string = "\4" },
    { .string = "\1" },
    { .string = "\2" },
    { .string = "\1" },
    { .string = "" },
    { .string = "%a %b %e %H:%M:%S %Z %Y" },
    { .wstr = (const uint32_t *) L"%a %b %e %H:%M:%S %Z %Y" },
    { .string = "ANSI_X3.4-1968" }
  }
};

#define ab_day_name(DAY) (_nl_C_LC_TIME.values[_NL_ITEM_INDEX (ABDAY_1)+(DAY)].string)
#define ab_month_name(MON) (_nl_C_LC_TIME.values[_NL_ITEM_INDEX (ABMON_1)+(MON)].string)
static const char format[] = "%.3s %.3s%3d %.2d:%.2d:%.2d %d";

int
aruba_asctime (const struct rtc_time *tp, char *buf, size_t buflen)
{
    return snprintf (buf, buflen, format,
                      (tp->tm_wday < 0 || tp->tm_wday >= 7 ?
                       "???" : ab_day_name (tp->tm_wday)),
                      (tp->tm_mon < 0 || tp->tm_mon >= 12 ?
                       "???" : ab_month_name (tp->tm_mon)),
                      tp->tm_mday, tp->tm_hour, tp->tm_min,
                      tp->tm_sec, tp->tm_year);
}
EXPORT_SYMBOL(aruba_asctime);

extern void to_tm(unsigned long tim, struct rtc_time *tm);

int
aruba_xtime2date (unsigned long xtime_secs, char *dest_str, int sz)
{
  struct rtc_time tp;

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,32))
  aruba_offtime (&xtime_secs, 0, &tp);
#else
  to_tm (xtime_secs, &tp);
#endif
  return aruba_asctime (&tp, dest_str, sz);
}
EXPORT_SYMBOL(aruba_xtime2date);

#if 0
char tmbuf[256];
int
main(int argc, char *argv[])
{
        time_t ttt=1329950715;
        struct rtc_time rgkt, *tp;
        char *ttstr = NULL;

        tp = aruba_secs2tm(ttt, &rgkt);
        if (tp) {
                ttstr = aruba_asctime(tp, tmbuf, sizeof(tmbuf));
                if (ttstr)
                        printf("%s", ttstr);
        }
}
#endif
