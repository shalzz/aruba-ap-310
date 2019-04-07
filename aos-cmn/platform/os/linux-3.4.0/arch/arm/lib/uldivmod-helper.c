/* Miscellaneous BPABI functions.

   Copyright (C) 2003, 2004  Free Software Foundation, Inc.
   Contributed by CodeSourcery, LLC.

   This file is free software; you can redistribute it and/or modify it
   under the terms of the GNU General Public License as published by the
   Free Software Foundation; either version 2, or (at your option) any
   later version.

   In addition to the permissions in the GNU General Public License, the
   Free Software Foundation gives you unlimited permission to link the
   compiled version of this file into combinations with other programs,
   and to distribute those combinations without any restriction coming
   from the use of this file.  (The General Public License restrictions
   do apply in other respects; for example, they cover modification of
   the file, and distribution when not linked into a combine
   executable.)

   This file is distributed in the hope that it will be useful, but
   WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; see the file COPYING.  If not, write to
   the Free Software Foundation, 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.  */

#include <linux/module.h>
#include <asm/bitops.h>
#include <asm/byteorder.h>
#include <asm/div64.h>

uint64_t 
__udivdi3(uint64_t dividend, uint64_t divisor)
{
  uint32_t d = divisor;

  /* Scale divisor to 32 bits */
  if (divisor > 0xffffffffULL) {
    unsigned int shift = fls(divisor >> 32);

    d = divisor >> shift;
    dividend >>= shift;
  }

  /* avoid 64 bit division if possible */
  if (dividend >> 32)
    do_div(dividend, d);
  else
    dividend = (uint32_t) dividend / d;

  return dividend;
}
EXPORT_SYMBOL(__udivdi3);

typedef struct a {
#ifdef __BIG_ENDIAN
  int high, low;
#elif defined(__LITTLE_ENDIAN)
  int low, high;
#else
#error "unknown byte order"
#endif
} a;
typedef union b {
  struct a s;
  long long ll;
} b;

int64_t 
__divdi3(int64_t dividend, int64_t divisor)
{
  int32_t d = divisor;
  int c = 0;
  union b end = { .ll = dividend };
  union b sor = { .ll = divisor };
 
  if (end.s.high < 0) {
    c = ~c;
    end.ll = -end.ll;
  }
  if (sor.s.high < 0) {
    c = ~c;
    sor.ll = -sor.ll;
  }

  dividend = end.ll;
  divisor = sor.ll;

  /* Scale divisor to 32 bits */
  if (divisor > 0xffffffffULL) {
    unsigned int shift = fls(divisor >> 32);

    d = divisor >> shift;
    dividend >>= shift;
  }

  /* avoid 64 bit division if possible */
  if (dividend >> 32)
    do_div(dividend, d);
  else
    dividend = (int32_t) dividend / d;

  if (c) {
    dividend = -dividend;
  }
  return dividend;
}
EXPORT_SYMBOL(__divdi3);

long long
__gnu_ldivmod_helper (long long a, 
		      long long b, 
		      long long *remainder)
{
  long long quotient;

  quotient = __divdi3 (a, b);
  *remainder = a - b * quotient;
  return quotient;
}

unsigned long long
__gnu_uldivmod_helper (unsigned long long a, 
		       unsigned long long b,
		       unsigned long long *remainder)
{
  unsigned long long quotient;

  quotient = __udivdi3 (a, b);
  *remainder = a - b * quotient;
  return quotient;
}
#if 0
extern unsigned long long __aeabi_uldivmod(unsigned long long, unsigned long long);
EXPORT_SYMBOL(__aeabi_uldivmod);
#endif
