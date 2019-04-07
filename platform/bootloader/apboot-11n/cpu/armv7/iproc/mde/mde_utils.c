/*  *********************************************************************
    *  Copyright 2012
    *  Broadcom Corporation. All rights reserved.
    *
    *  This software is furnished under license and may be used and
    *  copied only in accordance with the following terms and
    *  conditions.  Subject to these conditions, you may download,
    *  copy, install, use, modify and distribute modified or unmodified
    *  copies of this software in source and/or binary form.  No title
    *  or ownership is transferred hereby.
    *
    *  1) Any source code used, modified or distributed must reproduce
    *     and retain this copyright notice and list of conditions as
    *     they appear in the source file.
    *
    *  2) No right is granted to use any trade name, trademark, or
    *     logo of Broadcom Corporation. Neither the "Broadcom
    *     Corporation" name nor any trademark or logo of Broadcom
    *     Corporation may be used to endorse or promote products
    *     derived from this software without the prior written
    *     permission of Broadcom Corporation.
    *
    *  3) THIS SOFTWARE IS PROVIDED "AS-IS" AND ANY EXPRESS OR
    *     IMPLIED WARRANTIES, INCLUDING BUT NOT LIMITED TO, ANY IMPLIED
    *     WARRANTIES OF MRCHANTABILITY, FITNESS FOR A PARTICULAR
    *     PURPOSE, OR NON-INFRINGEMENT ARE DISCLAIMED. IN NO EVENT
    *     SHALL BROADCOM BE LIABLE FOR ANY DAMAGES WHATSOEVER, AND IN
    *     PARTICULAR, BROADCOM SHALL NOT BE LIABLE FOR DIRECT, INDIRECT,
    *     INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
    *     (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
    *     GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
    *     BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
    *     OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR
    *     TORT (INCLUDING NEGLIGENCE OR OTHERWISE), EVEN IF ADVISED OF
    *     THE POSSIBILITY OF SUCH DAMAGE.
    *********************************************************************
    *  Broadcom Common Diagnostics Environment (CDE)
    *********************************************************************
    *  Filename: cde_main.c
    *
    *  Function:
    *
    *  Author:  Chung Kwong (C.K.) Der (ckder@broadcom.com)
    *
 */

#include <mde_common_headers.h>

#include <stdarg.h>
#ifdef PLATFORM
#if PLATFORM != HOST_LINUX
#include "lib_types.h"
#else
#include <linux/string.h>
#endif
#endif

extern unsigned host_get_elapsed_msec(void);

unsigned mdeShowInfo   ( sMde_t *mde )
{
  int inx;

  // This is mainly for debug - but it can also be informative
  host_printf ("\n%s\nVer %08x Date %08x\n", 
				 mde->name, mde->version, mde->date);
  host_printf ("MDE common command count %d\n\n", mde -> common.cmd_count );

  host_printf ("Target chip memory region count %d\n", mde -> mem_blks_count);

  for (inx = 0; inx < mde -> mem_blks_count; inx++ ) {
	host_printf ("\tRegion %02d: Offset address %08x, size %08x\n", 
				   mde -> mem_blks [inx].id,
				   mde -> mem_blks [inx].start,
				   mde -> mem_blks [inx].bsize );
  }
  host_printf ("\nTotal memory visible to MDE %08x (%d bytes)\n",
				 mde -> mem_total_bytes, mde -> mem_total_bytes );

  //  host_printf ("\nDDR speed = %dMHz\n\n",  mde->ddr_frequency );
  return DIAG_RET_OK;
}

/// 
#if PLATFORM != HOST_LINUX
size_t strlen(const char *str)
{
    size_t cnt = 0;

    while (*str) {
	str++;
	cnt++;
	}

    return cnt;
}


int strcmp(const char *dest,const char *src)
{
    while (*src && *dest) {
	if (*dest < *src) return -1;
	if (*dest > *src) return 1;
	dest++;	
	src++;
	}

    if (*dest && !*src) return 1;
    if (!*dest && *src) return -1;
    return 0;
}

int strncmp(const char *dest,const char *src, size_t cnt)
{
	int i = 0;
	while ( (*src && *dest) && (i < (cnt - 1) )) {
		if (*dest < *src) return -1;
		if (*dest > *src) return 1;
		dest++;	
		src++;
		i++;
	}

	if (*dest == *src) 
		return 0;
	else if (*dest > *src) 
		return 1;
	else 
		return -1;
}

char *strchr(const char *dest,int c)
{
    while (*dest) {
	if (*dest == c) return (char *) dest;
	dest++;
	}
    return NULL;
}

char *strstr(const char *dest, char * find)
{
	char c, sc;
	size_t len;
	char * s = (char *) dest;

	if ((c = *find++) != 0) {
		len = strlen(find);
		do {
			do {
				if ((sc = *s++) == 0)
					return NULL;
			} while (sc != c);
		} while (strncmp(s, find, len) != 0);
		s--;
	}
	return s;
}

char *strncpy(char *dest,const char *src,size_t cnt)
{
    char *ptr = dest;

    while (*src && (cnt > 0)) {
	*ptr++ = *src++;
	cnt--;
	}
    if (cnt > 0) *ptr = '\0';

    return dest;
}

char *strcpy(char *dest,const char *src)
{
    char *ptr = dest;

    while (*src) *ptr++ = *src++;
    *ptr = '\0';

    return dest;
}

char *strnchr(const char *dest,int c,size_t cnt)
{
    while (*dest && (cnt > 0)) {
	if (*dest == c) return (char *) dest;
	dest++;
	cnt--;
	}
    return NULL;
}
#endif

////
#define BASE_10 10
#define BASE_16 16

static unsigned cdeAtoD (char *str)
{
  int inx;
  int lng = strlen (str);
  unsigned val = 0;

  inx = 0;
  while (lng-- > 0) {
    val = val * BASE_10 + (unsigned) (str [inx++] - '0');
  }
  return val;
}

static unsigned cdeAtoX (char *str)
{
  int inx, lng = (int) strlen (str);
  unsigned val = 0, digit;

  inx = 0;
  while (lng-- > 0) {
    digit = (unsigned)str [inx++] - '0';
    if (digit >= 10) {
      digit -= 7;
      if (digit >= 0x21) digit -= 0x20;  
    }
    val = val * BASE_16 + digit;
  }
  return val;
}

static char *cdeCpy (char *from, char *to, int limit)
{
  char this;
  int  inx = 0;

  while (inx < limit) {
    this = from [inx];
    if ((this != EOS) && (this != SPACE)) {
      to [inx++] = this; continue;
    }
    to [inx] = EOS;
    return &from [inx];
  }
  return NULL;
}

void mdeScanf ( sMde_t *mde, char *ctl, ...)
{
  va_list marker;
  char *line = mde -> user_line;

  int inx = 0, cnt = 0, go_ahead = 1, tknx = 0;

  char key;
  char *psrc, *pdst;
  unsigned *pi, val;
  char local_token_list [MAX_TOKEN_SUPPORTED][DIAG_SIZE_SMALL];

  va_start (marker, ctl);

  // break down all tokens
  psrc = line;
  while (cnt < MAX_TOKEN_SUPPORTED) {
    psrc = cdeCpy (psrc, &local_token_list [cnt][0], DIAG_SIZE_SMALL);
    cnt ++;
    key = *psrc;
    if (key == EOS) break;
    psrc ++;
  }
  va_start (marker, ctl);
  inx = 0;

  while (go_ahead) {
    key = ctl [inx];
    if (key == EOS) break; // direct exit
    if (key != '%') {
      host_printf ("mdeScanf: bad ctl [%s] - SW error\n", ctl);
      host_printf ("inx = %d key = [%c: %x]\n", inx, key, key);
      break; 
    }
    // handles s, d, x
    key = ctl [++inx];
    switch (key) {
    case 's':
      pdst = va_arg (marker, char *);
      strcpy (pdst,  &local_token_list [tknx][0]);
      tknx ++;
      break;
    case 'd':     
      pi = va_arg (marker, unsigned *);
      *pi = cdeAtoD (&local_token_list [tknx][0]);
      tknx ++;
      break;

    case 'x':
      pi = va_arg (marker, unsigned *);
      val = cdeAtoX (&local_token_list [tknx][0]);
      *pi = val;
      
      tknx ++;
      break;
    default:
      host_printf ("mdeScanf: cannot handle ctl character [%c -> %x]\n", key, key);
      go_ahead = 0; // do not continue
      break;
    }
    inx ++;
  }
  va_end (marker);
}

void mdeLineScanf ( char *line, char *ctl, ...)
{
  va_list marker;

  int inx = 0, cnt = 0, go_ahead = 1, tknx = 0;

  char key;
  char *psrc, *pdst;
  unsigned *pi, val;
  char local_token_list [MAX_TOKEN_SUPPORTED][DIAG_SIZE_SMALL];

  va_start (marker, ctl);

  // break down all tokens
  psrc = line;
  while (cnt < MAX_TOKEN_SUPPORTED) {
    psrc = cdeCpy (psrc, &local_token_list [cnt][0], DIAG_SIZE_SMALL);
    cnt ++;
    key = *psrc;
    if (key == EOS) break;
    psrc ++;
  }
  va_start (marker, ctl);
  inx = 0;

  while (go_ahead) {
    key = ctl [inx];
    if (key == EOS) break; // direct exit
    if (key != '%') {
      host_printf ("mdeScanf: bad ctl [%s] - SW error\n", ctl);
      host_printf ("inx = %d key = [%c: %x]\n", inx, key, key);
      break; 
    }
    // handles s, d, x
    key = ctl [++inx];
    switch (key) {
    case 's':
      pdst = va_arg (marker, char *);
      strcpy (pdst,  &local_token_list [tknx][0]);
      tknx ++;
      break;
    case 'd':     
      pi = va_arg (marker, unsigned *);
      *pi = cdeAtoD (&local_token_list [tknx][0]);
      tknx ++;
      break;

    case 'x':
      pi = va_arg (marker, unsigned *);
      val = cdeAtoX (&local_token_list [tknx][0]);
      *pi = val;
      
      tknx ++;
      break;
    default:
      host_printf ("mdeScanf: cannot handle ctl character [%c -> %x]\n", key, key);
      go_ahead = 0; // do not continue
      break;
    }
    inx ++;
  }
  va_end (marker);
}

#define ERROR_ALLOWED 128
unsigned mdeVerifyBlk (unsigned address, unsigned count, unsigned data, unsigned incrementer,
					   sMde_t *mde)
{
  int inx;
  int limit = 0;
  unsigned *p, val, read;
  
  for (inx = 0, val = data, p = (unsigned*)address; 
	   inx < count; inx ++, p ++, val += incrementer ) {
    read = *p;
    if (read == val) continue;
    host_printf ("\ncdeVerifyBlk failed @%08x expected %08x but read %08x (XOR %08x)\n", p, val, read, val ^ read);

    if (limit ++ < ERROR_ALLOWED) continue;
	host_printf ("Too many errors, quit now...\n");
    return DIAG_RET_BAD;
  }
  return DIAG_RET_OK;
}

unsigned *mdeCmpInts (unsigned *from, unsigned *to, unsigned count, unsigned *result,
					  sMde_t *mde)
{
#define LIMIT 8

  // returns the next address (to)
  int inx, limit = 0;
  unsigned fd, td, diff;

  *result = DIAG_RET_OK;
  
  for (inx = 0; inx < count; inx ++, to ++, from ++) {
    fd = *from;
    td = *to;
    if (fd == td) continue;
    diff = fd ^ td;
    host_printf ("\nFailed (@%08x = %08x) != (@%08x = %08x) diff (%08x)\n", from, fd, to, td, diff);
    if (limit++ < LIMIT) continue;
    break; // do not continue, too many errors
  }
  if ( limit != 0) {
	*result = DIAG_RET_BAD;
  }
  return to;
}

#define INTS_PER_LINE  4
#define BYTES_PER_LINE (INTS_PER_LINE*sizeof(int))


static void showAscii (unsigned ascii_dinx, int count, sMde_t *mde)
{
  unsigned *p;
  ITOB data;
  char this;

  int  iinx;
  int  cinx;

  /// BUG: need to fix the endian issue
  /// ASCII come out in reverse order
  p = (unsigned*) &mde->bread.data [ascii_dinx];

  host_printf ("\t");

  for (iinx = 0; iinx < count; iinx ++ ) {
    data.ival = *p++;
	for (cinx = 3; cinx >= 0 ; cinx -- ) {
	  //	for (cinx = 0; cinx < 4; cinx ++ ) {
	  this = data.bval [cinx];
	  if ((this >= SPACE) && (this < DEL)) host_printf ("%c", this);
	  else host_printf (".");
	}
  }
  host_printf ("\n");
}

void    mdeShowBlk ( sMde_t *mde, int count )
{
  int inx, linx, dinx = 0, ascii_dinx ;
  int lines = count / INTS_PER_LINE;




  unsigned addx;

  addx = mde->bread.address;  /// This is the read address

  for (linx = 0; linx < lines; linx ++, addx += BYTES_PER_LINE ) {
	host_printf ("%08x -> ", addx );
	ascii_dinx = dinx;
	for (inx = 0; inx < INTS_PER_LINE; inx ++) {
	  host_printf ("%08x ", mde->bread.data [ dinx++]);
	}
	showAscii ( ascii_dinx, INTS_PER_LINE, mde);
  }
  host_printf ("\n");
}

unsigned mdeQuery ( sMde_t *mde, char *question, unsigned original)
{
  unsigned new;
  host_printf ("%s = %08x changed to ", question, original);
  get_user_input ( mde );
  if ( mde -> user_line [0] == 0 ) return original; // no change
  mdeScanf ( mde, "%x", &new );
  return new;
}


//// New: print out the result
/// avoid using floating point

void mde_fdiv (unsigned val, unsigned divr, int place, sMde_t *mde )
{
  unsigned qu, val1, rd, inx, inj = 0, q1;
  char buf [32];
  val1 = val;
  //dbg_vsprintf
  qu = val1 / divr;
  rd = val1 % divr;
  q1 = qu;

  for (inx = 0; inx < place; inx ++) {
    val1 = rd * 10;
    qu = val1 / divr;
    rd = val1 % divr;
	buf [inj ++ ] = qu + '0';
  }
  buf [inj] = 0;
  host_printf (" %d.%s ", q1, buf);
}

unsigned mdeMemCmpBufs (sMde_t *mde )
{
  char discard [DIAG_SIZE_SMALL];
  unsigned from, to, size, result;

  mdeScanf (mde, "%s%x%x%x", discard, &from, &to, &size);

  host_printf ("Compare data @%08x against data @%08x (size %x)\n", from, to, size);

  mdeCmpInts ((unsigned *)from, (unsigned *)to, size, &result, mde);
  if (result != DIAG_RET_OK) return DIAG_RET_BAD;
  host_printf ("No error was found\n");
  return DIAG_RET_OK;
}

int mdeCountTokens(char * str)
{
  int isToken = 0;
  int count   = 0;

  while (*str != '\0')
  {
     if (*str == ' ') 
	 {
        isToken=0;
	 }
	 else
	 {
	    if (isToken==0) count++;
        isToken=1;
	 }
	 str++;
  }

  return count;
}


// JIRA SWMEMSYS-329
//  generate a random seed using TMT PRND algorithm
//  input:   unsigned int *g_current_seed
//            a global variable which is updated with
//            new random seed.
//            If it’s 0, it will use cpu’s timer as 
//            RND value.
//  output:  unsigned int *g_current_seed
//  return:  none
void generate_random_seed( unsigned int *g_current_seed ) {
  tinymt32_t tinymt;
  // if seed passed to me is 0.
  if ( *g_current_seed == 0 ) {
	*g_current_seed = host_get_elapsed_msec ();
  }
  // else pick a new random seed using TMT
  else {
	// initialize tiny
	tinymt.mat1 = 0;
	tinymt.mat2 = 0;
	tinymt.tmat = 0;
	
	tinymt32_init(&tinymt, *g_current_seed);
	*g_current_seed = tinymt32_generate_uint32( &tinymt );
  }
}

// PW change starts here

int is_big_endian(void) 
{   
  union {        
	   uint32_t i;         
     char c[4];     
	}
	bint = {0x01000000};  
  return bint.c[0] ;  
}

void endian_swap(char * p)
{
   unsigned char temp;
   temp = p[3];
   p[3] = p[0];
   p[0] = temp;
   temp = p[2];
   p[2] = p[1];
   p[1] = temp;
}


extern unsigned get_system_ms_time (void);


unsigned current_time_in_seconds (void)
{
  return (host_get_elapsed_msec () / 1000);
}

unsigned time_elapsed_in_seconds (unsigned start)
{
  return 0;
}
