#ifndef __EXPORTS_H__
#define __EXPORTS_H__

#ifndef __ASSEMBLY__

#include <common.h>

/* These are declarations of exported functions available in C code */
unsigned long get_version(void);
int  getc(void);
int  tstc(void);
void putc(const char);
void puts(const char*);
int  printf(const char* fmt, ...);
void install_hdlr(int, interrupt_handler_t*, void*);
void free_hdlr(int);
void *malloc(size_t);
void free(void*);
void udelay(unsigned long);
#ifdef CONFIG_OCTEON
uint64_t get_timer(uint64_t);
#else
unsigned long get_timer(unsigned long);
#endif
void vprintf(const char *, va_list);
//void do_reset (void);
#ifdef CONFIG_MARVELL
void *realloc(void*, size_t);
void *calloc(size_t, size_t);
void *memalign(size_t, size_t);
u32 mvGetRtcSec(void);
#endif
#if (CONFIG_COMMANDS & CFG_CMD_I2C) || defined(CONFIG_GRENACHE)
int i2c_write (uchar, uint, int , uchar* , int);
int i2c_read (uchar, uint, int , uchar* , int);
int i2c_reg_write(uchar devid, uint addr,  uchar val);
uchar i2c_reg_read(uchar devid, uint addr);
int i2c_set_bus_num(unsigned int bus);
unsigned int i2c_get_bus_num(void);
int i2c_probe(uchar chip);
#endif	/* CFG_CMD_I2C */

void app_startup(char **);

#endif    /* ifndef __ASSEMBLY__ */

enum {
#define EXPORT_FUNC(x) XF_ ## x ,
#include <_exports.h>
#undef EXPORT_FUNC

	XF_MAX
};

#define XF_VERSION	2

#if defined(CONFIG_I386)
extern gd_t *global_data;
#endif

#endif	/* __EXPORTS_H__ */
