
/* generic_i2c.c */

#ifdef __BDI
#include "bdi.h"
#define protect()
#define unprotect()
#define generic_check_interruption() (1)
#else
#ifdef __ECOS
#include <cyg/hal/ar7100_soc.h>
#include "ag7100_ecos.h" 
#define printk             DEBUG_PRINTF
#define udelay             A_UDELAY
#define protect()
#define unprotect()
#define generic_check_interruption() (1)
#else
#include <asm/addrspace.h>
#include <asm/types.h>
#include <common.h>
#include "ar7100_soc.h"
#define printk printf

#define protect()
#define unprotect()

#define generic_check_interruption() (1)

#endif
#endif

#include "generic_i2c.h"

#define GENERIC_I2C_IO_EXP            0x40

#undef USE_TEST_CODE
#define USE_TEST_CODE

/* ************************************************************** */

static int generic_gpio_errcnt = 0;

static void inline
generic_gpio_errclr(void) 
{
  generic_gpio_errcnt = 0;
}

static void inline
generic_gpio_check_rc(unsigned int rc) 
{
  if (rc) generic_gpio_errcnt++;
}

static int inline
generic_gpio_errget(void) 
{
  return generic_gpio_errcnt;
}

/* drive bits in mask low */
void ar7100_gpio_drive_low(unsigned int mask)
{
  ar7100_reg_rmw_set (AR7100_GPIO_OE,    mask);
}

/* drive bits in mask high */
void ar7100_gpio_drive_high(unsigned int mask)
{
  ar7100_reg_rmw_clear (AR7100_GPIO_OE,    mask);
}

/* Allow bits in mask to float to their quiescent state and test results */
unsigned int ar7100_gpio_float_high_test(unsigned int mask)
{
  volatile unsigned int d;
  ar7100_reg_rmw_clear(AR7100_GPIO_OE,  mask);
#ifdef CONFIG_MSR2K
  udelay(1);  /* seems pull-ups aren't quite fast enough */
#endif
  d = ar7100_reg_rd(AR7100_GPIO_IN);
  d = ar7100_reg_rd(AR7100_GPIO_IN) & mask;
  return d!=mask;
}

/* ************************************************************** */

#if defined(CONFIG_MSR2K)
#define GENERIC_I2C_SCL  (1<<2)
#define GENERIC_I2C_SDA  (1<<1)
#define GENERIC_I2C_PAUSE 10 //2
#else
#define GENERIC_I2C_SCL  (1<<0)
#define GENERIC_I2C_SDA  (1<<1)
#define GENERIC_I2C_PAUSE 10 //2
#endif

static void inline
generic_i2c_chigh_dhigh(void)
{
  generic_gpio_check_rc(ar7100_gpio_float_high_test ( GENERIC_I2C_SCL | GENERIC_I2C_SDA ));
  udelay( GENERIC_I2C_PAUSE);
}

static void inline
generic_i2c_chigh_dlow(void)
{
  generic_gpio_check_rc(ar7100_gpio_float_high_test ( GENERIC_I2C_SCL ));
  ar7100_gpio_drive_low( GENERIC_I2C_SDA );
  udelay( GENERIC_I2C_PAUSE );
}

static void inline
generic_i2c_clow_dlow(void)
{
  ar7100_gpio_drive_low( GENERIC_I2C_SCL |  GENERIC_I2C_SDA );
  udelay( GENERIC_I2C_PAUSE );
}

static void inline
generic_i2c_clow_dhigh(void)
{
  ar7100_gpio_drive_low( GENERIC_I2C_SCL );
  generic_gpio_check_rc(ar7100_gpio_float_high_test( GENERIC_I2C_SDA ));
  udelay( GENERIC_I2C_PAUSE );
}

static void inline
generic_i2c_clow_dfloat(void)
{
  ar7100_gpio_drive_low( GENERIC_I2C_SCL );
  ar7100_reg_rmw_clear(AR7100_GPIO_OE, GENERIC_I2C_SDA ); 
  udelay( GENERIC_I2C_PAUSE );
}

static void inline
generic_i2c_chigh_dfloat(void)
{
  ar7100_gpio_drive_high( GENERIC_I2C_SCL );
  ar7100_reg_rmw_clear( AR7100_GPIO_OE, GENERIC_I2C_SDA ); 
  udelay( GENERIC_I2C_PAUSE );
}

static int
generic_i2c_chigh_dread(void)
{
  int d;

  d = (ar7100_reg_rd( AR7100_GPIO_IN ) & GENERIC_I2C_SDA) ? 1 : 0;
  udelay ( GENERIC_I2C_PAUSE/2);

  return d;
}

static void inline
generic_i2c_start(void)
{
  generic_i2c_chigh_dhigh();
  generic_i2c_chigh_dlow();
  generic_i2c_clow_dlow();
}

static void inline
generic_i2c_repstart(void)
{
  ar7100_reg_rmw_clear(AR7100_GPIO_OE, GENERIC_I2C_SDA);
  ar7100_reg_rmw_clear(AR7100_GPIO_OE, GENERIC_I2C_SCL);
  udelay ( GENERIC_I2C_PAUSE);
  ar7100_reg_rmw_set(AR7100_GPIO_OE, GENERIC_I2C_SDA);
  udelay ( GENERIC_I2C_PAUSE);
  ar7100_reg_rmw_set(AR7100_GPIO_OE, GENERIC_I2C_SCL);
  udelay ( GENERIC_I2C_PAUSE);
}

static void inline
generic_i2c_stop(void)
{
  generic_i2c_clow_dlow();
  generic_i2c_chigh_dlow();
  generic_i2c_chigh_dhigh();
}

static int
generic_i2c_raw_write_8(unsigned char v)
{
  int ack;
  int ii=7;
  do {
    if ((1<<ii) & v) {
      ar7100_gpio_drive_high( GENERIC_I2C_SDA );
      udelay ( GENERIC_I2C_PAUSE);
      ar7100_gpio_drive_high( GENERIC_I2C_SCL );
      udelay ( GENERIC_I2C_PAUSE);
    } else {
      ar7100_gpio_drive_low( GENERIC_I2C_SDA );
      udelay ( GENERIC_I2C_PAUSE);
      ar7100_gpio_drive_high( GENERIC_I2C_SCL );
      udelay ( GENERIC_I2C_PAUSE);
    }
    ar7100_gpio_drive_low( GENERIC_I2C_SCL );
      udelay ( GENERIC_I2C_PAUSE);
  } while (ii--);

  ar7100_gpio_drive_high( GENERIC_I2C_SDA );
  udelay ( GENERIC_I2C_PAUSE);
  ar7100_gpio_drive_high( GENERIC_I2C_SCL );
  udelay ( GENERIC_I2C_PAUSE);
  ack=generic_i2c_chigh_dread();
  ar7100_gpio_drive_low( GENERIC_I2C_SCL );
  udelay ( GENERIC_I2C_PAUSE);

  return ack;
}

static void
generic_i2c_raw_read_8(char lastByte, unsigned char *v)
{
  int d;
  int ii=7;
  int jj=0;

  ar7100_gpio_drive_high( GENERIC_I2C_SDA );
  udelay ( GENERIC_I2C_PAUSE);
  do {
    ar7100_gpio_drive_high( GENERIC_I2C_SCL );
    udelay ( GENERIC_I2C_PAUSE);
    d=generic_i2c_chigh_dread();
    ar7100_gpio_drive_low( GENERIC_I2C_SCL );
    udelay ( GENERIC_I2C_PAUSE);
    if (d) jj |= 1<<ii;
  } while (ii--);

  if (lastByte) {
    ar7100_gpio_drive_high( GENERIC_I2C_SDA );
    udelay ( GENERIC_I2C_PAUSE);
  } else {
    ar7100_gpio_drive_low( GENERIC_I2C_SDA );
    udelay ( GENERIC_I2C_PAUSE);
  }
  ar7100_gpio_drive_high( GENERIC_I2C_SCL );
  udelay ( GENERIC_I2C_PAUSE);
  ar7100_gpio_drive_low( GENERIC_I2C_SCL );
  udelay ( GENERIC_I2C_PAUSE);
  ar7100_gpio_drive_high( GENERIC_I2C_SDA );
  udelay ( GENERIC_I2C_PAUSE);

  *v = jj & 0xff;
}

static int 
generic_i2c_raw_write_bytes_to_addr(int addr, unsigned char *buffer, int count)
{
  volatile int ack;
  int ii;
  generic_gpio_errclr();
  generic_i2c_start();
  ack = generic_i2c_raw_write_8(addr&0xfe);
  if (ack)
    return 1;

  for (ii=0; ii<count; ii++) { 
    ack = generic_i2c_raw_write_8(buffer[ii]);
  }
  generic_i2c_stop();
  return generic_gpio_errget();
}

static int
generic_i2c_raw_read_bytes_from_addr(int addr, unsigned char *buffer, int count)
{
  int ack;
  int ii;
  generic_gpio_errclr();
  generic_i2c_start();
  ack = generic_i2c_raw_write_8((addr&0xff)|0x01);
  if (ack) {
      return 1;
  }
  for (ii=0; ii<count; ii++) generic_i2c_raw_read_8( ii==(count-1), &buffer[ii]);
  generic_i2c_stop();  
  return generic_gpio_errget();
}

static int 
generic_i2c_raw_write_bytes_to_addr_with_reg(
  int addr, int reg, unsigned char *buffer, int count
)
{
  volatile int ack;
  int ii;

  generic_gpio_errclr();
  generic_i2c_start();
  ack = generic_i2c_raw_write_8(addr&0xfe);
#if 0
  if (ack) {
    return 1;
  }
#endif
  ack = generic_i2c_raw_write_8(reg);
  if (ack) {
    return 1;
  }

  for (ii=0; ii<count; ii++) { 
    ack = generic_i2c_raw_write_8(buffer[ii]);
  }
  generic_i2c_stop();
  return generic_gpio_errget();
}

static int
generic_i2c_raw_read_bytes_from_addr_with_reg(
  int addr, int reg, unsigned char *buffer, int count
)
{
  int ack;
  int ii;
  generic_gpio_errclr();
  generic_i2c_start();
  ack = generic_i2c_raw_write_8(addr&0xfe);
  if (ack) {
    return 1;
  }
  ack = generic_i2c_raw_write_8(reg&0xff);
  if (ack) {
    return 1;
  }
  generic_i2c_repstart();  
  ack = generic_i2c_raw_write_8((addr&0xff)|0x01);
  if (ack) {
    return 1;
  }
  for (ii=0; ii<count; ii++) generic_i2c_raw_read_8( ii==(count-1), &buffer[ii]);
  generic_i2c_stop();  
  return generic_gpio_errget();
}

#ifdef USE_TEST_CODE

void generic_i2c_test_write_bits(void)
{
  printk("Writing bit stream of AA00\n");
  generic_gpio_errclr();
  do {
    generic_i2c_start();
    generic_i2c_raw_write_8(0xAA);
    generic_i2c_raw_write_8(0x00);
    generic_i2c_stop();  
    udelay(1000);
  } while generic_check_interruption();
}

void
generic_i2c_test_addr_strapping(void)  
{
  int jj;

  int end   = 0xfe;
  int addr  = 0x20;
  
  jj=0;
  printk("Looping through addresses %02x .. %02x\n", addr, end);
  while (addr<end) {
    volatile int ack;
    generic_i2c_start();
    ack = generic_i2c_raw_write_8(addr&0xfe);
    generic_i2c_stop();
    if (0==ack) {
      jj++;
      printk(" Found addr:  %02x\n", addr);
    }
    addr+=2;
  };

  if (0==jj)
    printk(" Failed test, no i2c found\n");
}

#endif

/* ************************************************************** */

static unsigned short generic_i2c_shadow_of_gpio = 0;

int
generic_i2c_write_gpio(unsigned short d)
{
  int errcnt;

  unsigned char b[2];
  b[0] = d;
  b[1] = d>>8;

  protect();

  errcnt = generic_i2c_raw_write_bytes_to_addr(GENERIC_I2C_IO_EXP, b, sizeof(b));
  
  if (errcnt == 0)
    generic_i2c_shadow_of_gpio = d;
  else
    printk("%s failed: %d %04x\n",  __FUNCTION__, errcnt, d );
  
  unprotect();

  return errcnt;
}

int
generic_i2c_read_gpio(unsigned short *d)
{
  int errcnt;
  unsigned char b[2];

  protect();

  errcnt = generic_i2c_raw_read_bytes_from_addr(GENERIC_I2C_IO_EXP, b, sizeof(b));
  if (errcnt == 0) {
    *d = b[1]<<8 | b[0];
    generic_i2c_shadow_of_gpio = *d;
  }
  else
    printk("%s failed: %d %04x\n",  __FUNCTION__, errcnt, *d );

  unprotect();

  return errcnt;
}

int
generic_i2c_init_gpio(void)
{
#if defined(CONFIG_TALISKER)
  ar7100_gpio_enable_i2c_on_gpio_0_1();  /* 0=SCL, 1=SDA */
  // drive the pins on both bit-bang busses low
  ar7100_reg_wr(AR7100_GPIO_CLEAR, 0x33);
#elif defined(CONFIG_MSR2K)
  ar7100_gpio_enable_i2c_on_gpio_2_1();  /* 2=SCL, 1=SDA */
  // drive pins on bit-bang bus low
  ar7100_reg_wr(AR7100_GPIO_CLEAR, 0x6);
#else
  ar7100_gpio_enable_i2c_on_gpio_0_1();
  generic_i2c_write_gpio(GENERIC_I2C_INIT_OUT_HIGH);
  ar7100_reg_wr(AR7100_GPIO_CLEAR, 0x3);
#endif
  return 0;
}

#ifdef USE_TEST_CODE

static void
generic_i2c_val_bit_desc (unsigned int chg, unsigned short val, char *bit_descriptions[]) 
{
  int ii;
  int jj;
  char *p2;
  
  for (ii=0;ii<16; ii++) {
    jj = 1<<(15-ii);
    if ( chg & jj ) {
      p2 = bit_descriptions[ii];
      if (p2 && p2[0]) {
	if (val & jj ) {
	  printk(" %s-H\n", p2);
	} else {
	  printk(" %s-L\n", p2);
	}
      }
    }
  }
}

static char *gpio_desc[16] = {
  "tp28",      /* 15 */
  "tp27",      /* 14 */
  "tp26",      /* 13 */
  "tp25",      /* 12 */
  "tp24",      /* 11 */
  "led2",      /* 10 */
  "led1",      /*  9 */
  "jump_sw",   /*  8 */
  "7?",        /*  7 */
  "sw_reset",  /*  6 */
  "slic_int1", /*  5 */
  "slic_int0", /*  4 */
  "slic_cs1",  /*  3 */
  "slic_cs0",  /*  2 */
  "stereo",    /*  1 */
  "vsc",       /*  0 */
};

void
generic_i2c_test_read_write_gpio(void)
{
  unsigned short d;
  unsigned short e;
  unsigned short f;

  printk("Looping on gpio alternately turning on LED1,TP24 and LED2,TP25 \n");
  printk("Pressing either push button will generate an output\n");

  generic_i2c_init_gpio();
  do {
    d= GENERIC_I2C_JUMP_LED1 | GENERIC_I2C_TP28 | GENERIC_I2C_INIT_INPUT;
    generic_i2c_write_gpio(d);
    generic_i2c_read_gpio (&e);
    f = d^e;
    if ( f & (GENERIC_I2C_INIT_OUT_HIGH|GENERIC_I2C_INIT_OUT_LOW ))
      printk("err: wrote: %04x  expected: %04x  dif: %04x \n", d, e, f);

    generic_i2c_val_bit_desc (f, e, gpio_desc); 
    udelay(100000);

    d= GENERIC_I2C_JUMP_LED2 | GENERIC_I2C_TP27;
    generic_i2c_write_gpio(d);
    generic_i2c_read_gpio (&e);
    f = d^e;
    if (f)
      printk("err: wrote: %04x  expected: %04x  chg: %04x \n", d, e, f);

    generic_i2c_val_bit_desc (f, e, gpio_desc); 
    udelay(200000);

  } while generic_check_interruption();
}

void
generic_i2c_test_toggle(unsigned short t)
{
  unsigned short d=0;

  printk("Toggling i2c GPIO pins %08x\n", t);
  generic_i2c_read_gpio(&d);
  do {
    generic_i2c_write_gpio(d | t);
    udelay(1000);
    generic_i2c_write_gpio(d & ~t);
    udelay(1000);
  } while generic_check_interruption();
}

#endif

int 
generic_i2c_assert_cs(int cs)
{
  unsigned short d = generic_i2c_shadow_of_gpio;

  /* We are active low, so turn on all of the bits */
  d  |= GENERIC_I2C_VSC73XX_CS|GENERIC_I2C_STEREO_CS|GENERIC_I2C_SLIC_CS0;
  
  /* Depending on CS, turn a single bit off */
  switch (cs) {
  case GENERIC_I2C_VSC73XX_CS:
  case GENERIC_I2C_STEREO_CS:
  case GENERIC_I2C_SLIC_CS0:
    d &= ~cs;
    return generic_i2c_write_gpio(d);
    
  default:
    printk("%s fail: %d\n", __FUNCTION__, cs);
    return -1;
  } 
}

void 
generic_i2c_deassert_cs(void) {
  unsigned short d = generic_i2c_shadow_of_gpio;

  /* We are active low, so turn on all of the bits */
  d  |= GENERIC_I2C_VSC73XX_CS|GENERIC_I2C_STEREO_CS|GENERIC_I2C_SLIC_CS0;
  generic_i2c_write_gpio(d);
}

#ifdef USE_TEST_CODE

void
generic_i2c_test_cs_toggle(int cs)
{
  printk("Looping on CS %d \n", cs);

  do {
    generic_i2c_assert_cs(cs);
    generic_i2c_deassert_cs();
    udelay(1000);
  } while generic_check_interruption();
}

#endif

/* allow GPIO0/GPIO1 to be used as SCL/SDA for software based i2c */
void
ar7100_gpio_enable_i2c_on_gpio_0_1(void)
{
  // leave the UART enabled and clear everything else
  ar7100_reg_wr(AR7100_GPIO_FUNCTIONS, 0x100);
  // disable interrupts on pins 1 and 2, and also stop driving them
  ar7100_reg_rmw_clear (AR7100_GPIO_INT_ENABLE,  3);
  ar7100_reg_rmw_clear (AR7100_GPIO_OE,          3);
  // pins should always be driven low
  ar7100_reg_wr (AR7100_GPIO_CLEAR,              3);
#ifdef CONFIG_TALISKER
  // clear TPM bus (pins 4/5) as well)
  ar7100_reg_rmw_clear (AR7100_GPIO_INT_ENABLE,  0x30);
  ar7100_reg_rmw_clear (AR7100_GPIO_OE,          0x30);
  // pins should always be driven low
  ar7100_reg_wr (AR7100_GPIO_CLEAR,              0x30);
#endif
}

/* allow GPIO2/GPIO1 to be used as SCL/SDA for software based i2c */
void
ar7100_gpio_enable_i2c_on_gpio_2_1(void)
{
  // leave the UART enabled and clear everything else
  ar7100_reg_wr(AR7100_GPIO_FUNCTIONS, 0x100);
  // disable interrupts on gpio 2 and 1, and also stop driving them
  ar7100_reg_rmw_clear (AR7100_GPIO_INT_ENABLE,  6);
  ar7100_reg_rmw_clear (AR7100_GPIO_OE,          6);
  // pins should always be driven low
  ar7100_reg_wr (AR7100_GPIO_CLEAR,              6);
}

static int __i2c_inited = 0;

//
// in the two routines below, shift the chip address left 1 bit to leave room
// for the R/W bit
//
int
i2c_read(uchar chip, uint addr, int alen, uchar *buffer, int len)
{
    if (!__i2c_inited) {
        generic_i2c_init_gpio();
	__i2c_inited = 1;
    }
    if (alen > 1 || !buffer || !len) {
        return 1;
    }
    if (alen) {
        return generic_i2c_raw_read_bytes_from_addr_with_reg(chip<<1, addr, buffer, len);
    } else {
        return generic_i2c_raw_read_bytes_from_addr(chip<<1, buffer, len);
    }
}

/* NOTE!!!  Aruba currently is NOT using a device which requires 16bit
 *          offsets, so the code here has been written to only support 
 *          8bit (1 byte) offsets.  If needed, the time can be spent to
 *          support the alen argument (2 byte offsets) properly.
 *          If the length is 0, then we skip the register pointer phase
 */
int
i2c_write(uchar chip, uint addr, int alen, uchar *buffer, int len)
{
    if (!__i2c_inited) {
        generic_i2c_init_gpio();
	__i2c_inited = 1;
    }

    if (alen > 1 || !buffer || !len) {
        return 1;
    }

    if (alen) {
        return generic_i2c_raw_write_bytes_to_addr_with_reg(chip<<1, addr, buffer, len);
    } else {
        return generic_i2c_raw_write_bytes_to_addr(chip<<1, buffer, len);
    }

}

/*-----------------------------------------------------------------------
 * Probe to see if a chip is present using single byte read.  Also good for checking 
 * for the completion of EEPROM writes since the chip stops responding until
 * the write completes (typically 10mSec).
 */
int
i2c_probe(uchar addr)
{
    volatile int ack;

    if (!__i2c_inited) {
        generic_i2c_init_gpio();
	__i2c_inited = 1;
    }
    generic_i2c_start();
    ack = generic_i2c_raw_write_8((addr << 1) & 0xfe);
    generic_i2c_stop();
    if (0 == ack) {
        return 0;
    }
    return 1;
}

int
i2c_reg_write(uchar devid, uint addr,  uchar val)
{
    int ret;
    
    ret = i2c_write(devid, addr, 1, &val, 1);
    return ret;
}

uchar
i2c_reg_read(uchar devid, uint addr)
{
    uchar   val;
    
    i2c_read(devid, addr, 1, &val, 1);
    return val;
}
