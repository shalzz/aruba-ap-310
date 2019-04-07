#ifdef __BDI
#include "bdi.h"
#else
#ifdef __ECOS
#if defined(CYGNUM_USE_ENET_VERBOSE)
#   undef  VERBOSE
#   define VERBOSE CYGNUM_USE_ENET_VERBOSE
#else
#   define VERBOSE 0
#endif 
#define generic_printk             DEBUG_PRINTF
#define generic_udelay             A_UDELAY
#else
#include <config.h>
#include <linux/types.h>
#include <common.h>
#include <miiphy.h>
#include "phy.h"
#include "ar8021_phy.h"
#define generic_printk             printf
#define generic_udelay             udelay
#endif
#endif

#ifndef VERBOSE
#define  VERBOSE           1
#endif

#include "ar8021_phy.h"

typedef struct {
  uint16_t     is_enet_port;
  uint16_t     mac_unit;
  uint16_t     phy_addr;
  uint32_t     id;
  uint16_t     status;
} ar8021_phy_t;

static uint16_t nmbr_phys = 0;
static ar8021_phy_t phy_info[] = {
  {
    is_enet_port: 0,
    mac_unit    : 0,
    phy_addr    : 0
  },
  {
    is_enet_port: 0,
    mac_unit    : 0,
    phy_addr    : 0
  },
  {
    is_enet_port: 0,
    mac_unit    : 0,
    phy_addr    : 0
  },
  {
    is_enet_port: 0,
    mac_unit    : 0,
    phy_addr    : 0
  },
  {
    is_enet_port: 0,
    mac_unit    : 0,
    phy_addr    : 0
  },
  {
    is_enet_port: 0,
    mac_unit    : 0,
    phy_addr    : 0
  }
};

static uint16_t
ag7100_mii_read(uint32_t phybase, uint16_t phyaddr, uint16_t reg)
{
    uint16_t val;

    phy_reg_read(phybase, phyaddr, reg, &val);
    return val;
}

static void
ag7100_mii_write(uint32_t phybase, uint16_t phyaddr, uint16_t reg, uint32_t val)
{
    phy_reg_write(phybase, phyaddr, reg, val);
}

#if 0
static ar8021_phy_t *
ar8021_phy_find(int unit)
{
  int i;
  ar8021_phy_t *phy;
  
  for(i = 0; i < sizeof(phy_info)/sizeof(phy_info[0]); i++) {
    phy = &phy_info[i];
    if (phy->is_enet_port && (phy->mac_unit == unit))
      return phy;
  }
  generic_printk(" ERROR: Did not find unit %d\n", unit);
  return NULL;
}
#endif

/* Base read/write routines built on ag7100_mii_ macros */

static inline void
ar8021_phy_mii_write_main(int unit, uint32_t phy_addr, uint8_t reg, uint16_t data)
{
  ag7100_mii_write(unit, phy_addr, 31,  0);
  ag7100_mii_write(unit, phy_addr, reg, data);
}

static inline void 
ar8021_phy_mii_write_ex(int unit, uint32_t phy_addr, uint8_t reg, uint16_t data)
{
  ag7100_mii_write(unit, phy_addr, 31,  1);
  ag7100_mii_write(unit, phy_addr, reg, data);
}

static inline uint16_t  
ar8021_phy_mii_read_main(int unit, uint32_t phy_addr, uint8_t reg)
{
  ag7100_mii_write(unit, phy_addr, 31,  0);
  return ag7100_mii_read(unit, phy_addr, reg);
}

static inline uint16_t 
ar8021_phy_mii_read_ex(int unit, uint32_t phy_addr, uint8_t reg)
{
  uint16_t val;
  ag7100_mii_write(unit, phy_addr, 31,  1);
  val = ag7100_mii_read(unit, phy_addr, reg);
  ag7100_mii_write(unit, phy_addr, 31,  0);
  return val;
}

/* Super simple routine to make it easier to print bits */

static void
ar8021_phy_print_val_bit_desc (uint16_t val, char *fieldName, char *bit_descriptions[]) 
{
  int ii;
  char *p2;
  
  generic_printk(" %s=%04x ", fieldName, val);
  for (ii=0;ii<16; ii++) {
    p2 = val & 1<<(15-ii) ? bit_descriptions[ii*2] : bit_descriptions[ii*2+1];
    if (p2 && p2[0])      
        generic_printk("    %s\n", p2);
  }
}

/* **************************************************************************** 
 *
 * For each register of interest:
 * 1) register read
 * 2) register print if there are alot of fields or bits
 * 3) register write if there may be a need to write
 * 4) masked write if there is a write and there are bits.
 *
 * **************************************************************************** */

/* mode control */

CEXTERN uint16_t 
ar8021_phy_mii_read_mode_ctrl(int unit, uint32_t phy_addr)
{
  return ar8021_phy_mii_read_main(unit, phy_addr, 0);
}

static char *mode_ctrl_desc[16*2] = {
  "sreset","", /* 15 */
  "loopback", "", /* 14 */
  "f100","", /* 13 */
  "auto-eg","", /* 12 */
  "pwr-dwn","", /* 11 */
  "isolate","", /* 10 */
  "restar-auto-neg","", /*  9 */
  "full-duplex","half-duplex", /*  8 */
  "ctest","", /*  7 */
  "f1000","", /*  6 */
  "","", /*  5 */
  "","", /*  4 */
  "","", /*  3 */
  "","", /*  2 */
  "","", /*  1 */
  "","", /*  0 */
};

CEXTERN void 
ar8021_phy_mii_print_mode_ctrl(uint16_t val)
{
  ar8021_phy_print_val_bit_desc (val, "mode_ctr        (00 )", mode_ctrl_desc);
}

CEXTERN void 
ar8021_phy_mii_write_mode_ctrl(int unit, uint32_t phy_addr, uint16_t reg )
{
  ar8021_phy_mii_write_main(unit, phy_addr, 0, reg);
}

CEXTERN void 
ar8021_phy_mii_rmw_ctrl(int unit, uint32_t phy_addr, uint32_t mask, uint32_t val)
{
  uint16_t reg = ar8021_phy_mii_read_mode_ctrl(unit, phy_addr); 
  reg &= ~mask;
  reg |=  val;
  ar8021_phy_mii_write_mode_ctrl(unit, phy_addr, reg);
}

/* mode status */

CEXTERN uint16_t  
ar8021_phy_mii_read_mode_status(int unit, uint32_t phy_addr)
{
  return ar8021_phy_mii_read_main(unit, phy_addr, 1);
}

static char *mode_status_desc[16*2] = {
  "","", /* 15 */
  "","", /* 14 */
  "","", /* 13 */
  "","", /* 12 */
  "","", /* 11 */
  "","", /* 10 */
  "","", /*  9 */
  "","", /*  8 */
  "","", /*  7 */
  "","", /*  6 */
  "auto-neg-cmplt","", /*  5 */
  "rem-fault","", /*  4 */
  "auto-neg-cap","", /*  3 */
  "link-up","", /*  2 */
  "jabber","", /*  1 */
  "ex-cap","", /*  0 */
};

CEXTERN void 
ar8021_phy_mii_print_mode_status(uint16_t val)
{
  ar8021_phy_print_val_bit_desc (val, "mode_status     (01 )", mode_status_desc);
}

/* phy identification */

CEXTERN uint16_t 
ar8021_phy_mii_read_id1(int unit, uint32_t phy_addr)
{
  return ar8021_phy_mii_read_main(unit, phy_addr, 2);
}

CEXTERN uint16_t 
ar8021_phy_mii_read_id2(int unit, uint32_t phy_addr)
{
  return ar8021_phy_mii_read_main(unit, phy_addr, 3);
}

/* gige control R/W */

CEXTERN uint16_t 
ar8021_phy_mii_read_gige_ctrl(int unit, uint32_t phy_addr)
{
  return ar8021_phy_mii_read_main(unit, phy_addr, 9);
}

static char *gige_ctrl_desc[16*2] = {
  "distortion-test","", /* 15 */
  "jitter-test","", /* 14 */
  "waveform-test","", /* 13 */
  "man-cfg","", /* 12 */
  "master","", /* 11 */
  "","", /* 10 */
  "","", /*  9 */
  "","", /*  8 */
  "","", /*  7 */
  "","", /*  6 */
  "","", /*  5 */
  "","", /*  4 */
  "","", /*  3 */
  "","", /*  2 */
  "","", /*  1 */
  "","", /*  0 */
};

CEXTERN void 
ar8021_phy_mii_print_gige_ctrl(uint16_t val)
{
  ar8021_phy_print_val_bit_desc (val, "gige_ctrl       (09 )", gige_ctrl_desc);
}

CEXTERN void  
ar8021_phy_mii_write_gige_ctrl(int unit, uint32_t phy_addr, uint16_t reg)
{
  return ar8021_phy_mii_write_main(unit, phy_addr, 9, reg);
}

CEXTERN void 
ar8021_phy_rmw_gige_ctrl(int unit, uint32_t phy_addr, uint32_t mask, uint32_t val)
{
  uint16_t reg = ar8021_phy_mii_read_gige_ctrl(unit, phy_addr); 
  if (reg == 0xffff)
    return;
  reg &= ~mask;
  reg |=  val;
  ar8021_phy_mii_write_gige_ctrl(unit, phy_addr, reg);
}

/* gige status RO */

CEXTERN uint16_t  
ar8021_phy_read_gige_status(int unit, uint32_t phy_addr)
{
  return ar8021_phy_mii_read_main(unit, phy_addr, 10);
}

static char *gige_status_desc[16*2] = {
  "ms-fault","", /* 15 */
  "is-master","", /* 14 */
  "loc-rcv-ok","", /* 13 */
  "rem-rcv-ok","", /* 12 */
  "fdx-cap","", /* 11 */
  "hdx-cap","", /* 10 */
  "","", /*  9 */
  "","", /*  8 */
  "","", /*  7 */
  "","", /*  6 */
  "","", /*  5 */
  "","", /*  4 */
  "","", /*  3 */
  "","", /*  2 */
  "","", /*  1 */
  "","", /*  0 */
};

CEXTERN void 
ar8021_phy_mii_print_gige_status(uint16_t val)
{
  ar8021_phy_print_val_bit_desc (val, " gige_status     (10 )", gige_status_desc);
  generic_printk("    idle-err-cnt: %02x\n", val&0xff);
}

/* gige status extension 1 todo */

/* 100 base_t status extension RO */

CEXTERN uint16_t  
ar8021_phy_read_100_status_ex(int unit, uint32_t phy_addr)
{
  return ar8021_phy_mii_read_main(unit, phy_addr, 16);
}

static char *l00_status_ex_desc[16*2] = {
  "descram-lock","", /* 15 */
  "descram-err","", /* 14 */
  "disc","", /* 13 */
  "link-act","", /* 12 */
  "rcv-err","", /* 11 */
  "xmt-err","", /* 10 */
  "sos-err","", /*  9 */
  "","", /*  8 */
  "","", /*  7 */
  "","", /*  6 */
  "","", /*  5 */
  "","", /*  4 */
  "","", /*  3 */
  "","", /*  2 */
  "","", /*  1 */
  "","", /*  0 */
};

CEXTERN void 
ar8021_phy_mii_print_100_status_ex(uint16_t val)
{
  ar8021_phy_print_val_bit_desc (val, "100_status_ex   (16 )", l00_status_ex_desc);
}

/* gige status extention 2 RO */

CEXTERN uint16_t 
ar8021_phy_read_gige_status_ex_2(int unit, uint32_t phy_addr)
{
  return ar8021_phy_mii_read_main(unit, phy_addr, 17);
}

static char *gige_status_ex_2_desc[16*2] = {
  "descram-lock","", /* 15 */
  "descram-err","", /* 14 */
  "disc","", /* 13 */
  "link-up","", /* 12 */
  "rcv-err","", /* 11 */
  "xmt-err","", /* 10 */
  "ssd-err","", /*  9 */
  "esd-err","", /*  8 */
  "ce-err","", /*  7 */
  "bcm-det","", /*  6 */
  "mdi-x-err","", /*  5 */
  "","", /*  4 */
  "","", /*  3 */
  "","", /*  2 */
  "","", /*  1 */
  "","", /*  0 */
};

CEXTERN void 
ar8021_phy_mii_print_gige_status_ex_2(uint16_t val)
{
  ar8021_phy_print_val_bit_desc (val, "gige_status_ex_2((17 )", gige_status_ex_2_desc);
}

/* Error counts of various types saturate at 0xff */

CEXTERN uint16_t 
ar8021_phy_read_rx_error_count(int unit, uint32_t phy_addr)
{
  return ar8021_phy_mii_read_main(unit, phy_addr, 19);
}

CEXTERN uint16_t 
ar8021_phy_read_false_carrier_count(int unit, uint32_t phy_addr)
{
  return ar8021_phy_mii_read_main(unit, phy_addr, 20);
}

CEXTERN uint16_t 
ar8021_phy_read_disconnect_count(int unit, uint32_t phy_addr)
{
  return ar8021_phy_mii_read_main(unit, phy_addr, 21);
}

CEXTERN uint16_t 
ar8021_phy_read_ex_crtl_set_1(int unit, uint32_t phy_addr)
{
  return ar8021_phy_mii_read_main(unit, phy_addr, 23);
}

static char *ex_crtl_set_1_desc[16*2] = {
  "","", /* 15 */
  "","", /* 14 */
  "","", /* 13 */
  "","", /* 12 */
  "","", /* 11 */
  "","", /* 10 */
  "","", /*  9 */
  "skew-2ns-rx-tx","", /*  8 */
  "","", /*  7 */
  "","", /*  6 */
  "actiphy","", /*  5 */
  "","", /*  4 */
  "","", /*  3 */
  "","", /*  2 */
  "","", /*  1 */
  "","", /*  0 */
};

CEXTERN void 
ar8021_phy_mii_print_ex_ctrl_set_1(uint16_t val)
{
  ar8021_phy_print_val_bit_desc (val, "ex_crtl_set_1   (23 )", ex_crtl_set_1_desc);
}

CEXTERN void  
ar8021_phy_write_ex_crtl_set_1(int unit, uint32_t phy_addr, uint16_t reg)
{
  return ar8021_phy_mii_write_main(unit, phy_addr, 23, reg);
}

CEXTERN void 
ar8021_phy_rmw_ex_crtl_set_1(int unit, uint32_t phy_addr, uint32_t mask, uint32_t val)
{
  uint16_t reg = ar8021_phy_read_ex_crtl_set_1(unit, phy_addr); 
  if (reg == 0xffff)
    return;
  reg &= ~mask;
  reg |=  val;
  ar8021_phy_write_ex_crtl_set_1(unit, phy_addr, reg);
}

CEXTERN uint16_t 
ar8021_phy_read_ex_crtl_set_2(int unit, uint32_t phy_addr)
{
  return ar8021_phy_mii_read_main(unit, phy_addr, 24);
}

CEXTERN void  
ar8021_phy_write_ex_crtl_set_2(int unit, uint32_t phy_addr, uint16_t reg)
{
  return ar8021_phy_mii_write_main(unit, phy_addr, 24, reg);
}

CEXTERN void 
ar8021_phy_rmw_ex_crtl_set_2(int unit, uint32_t phy_addr, uint32_t mask, uint32_t val)
{
  uint16_t reg = ar8021_phy_read_ex_crtl_set_2(unit, phy_addr); 
  if (reg == 0xffff)
    return;
  reg &= ~mask;
  reg |=  val;
  ar8021_phy_write_ex_crtl_set_2(unit, phy_addr, reg);
}

CEXTERN uint16_t  
ar8021_phy_read_irq_msk(int unit, uint32_t phy_addr)
{
  return ar8021_phy_mii_read_main(unit, phy_addr, 25);
}

CEXTERN void 
ar8021_phy_write_irq_msk(int unit, uint32_t phy_addr, uint16_t reg)
{
  return ar8021_phy_mii_write_main(unit, phy_addr, 25, reg);
}

CEXTERN uint16_t  
ar8021_phy_read_irq_status(int unit, uint32_t phy_addr)
{
  return ar8021_phy_mii_read_main(unit, phy_addr, 26);
}
static char *irq_status_desc[16*2] = {
  "pending","", /* 15 */
  "speed","", /* 14 */
  "link","", /* 13 */
  "fdx","", /* 12 */
  "auto-err","", /* 11 */
  "auto-cmpl","", /* 10 */
  "inl-pwr","", /*  9 */
  "","", /*  8 */
  "","", /*  7 */
  "","", /*  6 */
  "","", /*  5 */
  "","", /*  4 */
  "","", /*  3 */
  "dwn-shft","", /*  2 */
  "ms-err","", /*  1 */
  "","", /*  0 */
};

CEXTERN void 
ar8021_phy_mii_print_irq_status(uint16_t val)
{
  ar8021_phy_print_val_bit_desc (val, "irq_status      (26 )", irq_status_desc);
}

#if 0
/* RO */

CEXTERN uint16_t 
ar8021_phy_read_aux_ctrl_status(int unit, uint32_t phy_addr)
{
  return ar8021_phy_mii_read_main(unit, phy_addr, 28);
}

static char *aux_ctrl_status_desc[16*2] = {
  "auto-cmplt","", /* 15 */
  "auto-dis","", /* 14 */
  "cross-over","", /* 13 */
  "cd swap","", /* 12 */
  "a-pol","", /* 11 */
  "b-pol","", /* 10 */
  "c-pol","", /*  9 */
  "d-pol","", /*  8 */
  "","", /*  7 */
  "","", /*  6 */
  "fdx","", /*  5 */
  "gige","", /*  4 */
  "100-base-T","", /*  3 */
  "","", /*  2 */
  "sticky","", /*  1 */
  "","", /*  0 */
};

CEXTERN void 
ar8021_phy_mii_print_aux_ctrl_status(uint16_t val)
{
  ar8021_phy_print_val_bit_desc (val, "aux_ctrl_status (28 )", aux_ctrl_status_desc);
}

CEXTERN uint16_t 
ar8021_phy_read_delay_skew_status(int unit, uint32_t phy_addr)
{
  return ar8021_phy_mii_read_main(unit, phy_addr, 29);
}

CEXTERN void 
ar8021_phy_mii_print_delay_skew_status(uint16_t val)
{
  generic_printk(" delay_skew_     (29 )=%04x\n", val);
  generic_printk("    pair a delay: %02x\n", (val>>12)&7);
  generic_printk("    pair b delay: %02x\n", (val>> 8)&7);
  generic_printk("    pair c delay: %02x\n", (val>> 4)&7);
  generic_printk("    pair d delay: %02x\n", (val>> 0)&7);
}

/* Extended registers - specific to chips? */

CEXTERN uint16_t 
ar8021_phy_read_crc_good_counter(int unit, uint32_t phy_addr)
{
  return ar8021_phy_mii_read_ex(unit, phy_addr, 18);
}

/* Resistor calibration */

CEXTERN uint16_t 
ar8021_phy_read_mac_resistor_calibration(int unit, uint32_t phy_addr)
{
  return ar8021_phy_mii_read_ex(unit, phy_addr, 19);
}

CEXTERN void 
ar8021_phy_write_mac_resistor_calibration(int unit, uint32_t phy_addr, uint16_t val)
{
  ar8021_phy_mii_write_ex(unit, phy_addr, 19, val);
}

CEXTERN void 
ar8021_phy_rmw_mac_resistor_calibration(int unit, uint32_t phy_addr, uint32_t mask, uint32_t val)
{
  uint16_t reg = ar8021_phy_read_mac_resistor_calibration(unit, phy_addr); 
  reg &= ~mask;
  reg |=  val;
  ar8021_phy_write_mac_resistor_calibration(unit, phy_addr, reg);
}

CEXTERN uint16_t 
ar8021_phy_read_ex_crtl_set_5(int unit, uint32_t phy_addr)
{
  return ar8021_phy_mii_read_ex(unit, phy_addr, 27);
}

CEXTERN void 
ar8021_phy_mii_print_ex_crtl_set_5(uint16_t val)
{
  generic_printk(" ex_crtl_set_5   (27E)=%04x\n", val);
  generic_printk("    crs:         %x\n", (val>>12)&7);
  generic_printk("    100  tx amp: %x\n", (val>> 6)&7);
  generic_printk("    gige tx amp: %x\n", (val>> 3)&7);
  generic_printk("    gige edge:   %x\n", (val>> 0)&7);

  if (val & 1<<10)
    generic_printk("   FAR-END-LOOPBACK\n");
}

CEXTERN void  
ar8021_phy_write_ex_crtl_set_5(int unit, uint32_t phy_addr, uint16_t reg)
{
  return ar8021_phy_mii_write_ex(unit, phy_addr, 27, reg);
}

CEXTERN void 
ar8021_phy_rmw_ex_crtl_set_5(int unit, uint32_t phy_addr, uint32_t mask, uint32_t val)
{
  uint16_t reg = ar8021_phy_read_ex_crtl_set_5(unit, phy_addr); 
  if (reg == 0xffff)
    return;
  reg &= ~mask;
  reg |=  val;
  ar8021_phy_write_ex_crtl_set_5(unit, phy_addr, reg);
}

CEXTERN uint16_t 
ar8021_phy_read_skew_ctrl(int unit, uint32_t phy_addr)
{
  return ar8021_phy_mii_read_ex(unit, phy_addr, 28);
}

CEXTERN void 
ar8021_phy_mii_print_skew_ctrl(uint16_t val)
{
  generic_printk(" skew_ctrl       (28E)=%04x\n", val);
  generic_printk("    tx cmp: %x\n", (val>>14)&3);
  generic_printk("    rx cmp: %x\n", (val>>12)&7);
  generic_printk("    jumbo:  %x\n", (val>>10)&7); 
}

CEXTERN void  
ar8021_phy_write_skew_ctrl(int unit, uint32_t phy_addr, uint16_t reg)
{
  return ar8021_phy_mii_write_ex(unit, phy_addr, 28, reg);
}

CEXTERN void 
ar8021_phy_rmw_skew_ctrl(int unit, uint32_t phy_addr, uint32_t mask, uint32_t val)
{
  uint16_t reg = ar8021_phy_read_skew_ctrl(unit, phy_addr); 
  if (reg == 0xffff)
    return;
  reg &= ~mask;
  reg |=  val;
  ar8021_phy_write_skew_ctrl(unit, phy_addr, reg);
}

CEXTERN uint16_t 
ar8021_phy_read_epg_1(int unit, uint32_t phy_addr)
{
  return ar8021_phy_mii_read_ex(unit, phy_addr, 29);
}

CEXTERN void  
ar8021_phy_write_epg_1(int unit, uint32_t phy_addr, uint16_t reg)
{
  return ar8021_phy_mii_write_ex(unit, phy_addr, 29, reg);
}

CEXTERN void 
ar8021_phy_rmw_epg_1(int unit, uint32_t phy_addr, uint32_t mask, uint32_t val)
{
  uint16_t reg = ar8021_phy_read_epg_1(unit, phy_addr); 
  if (reg == 0xffff)
    return;
  reg &= ~mask;
  reg |=  val;
  ar8021_phy_write_epg_1(unit, phy_addr, reg);
}

CEXTERN uint16_t 
ar8021_phy_read_epg_2(int unit, uint32_t phy_addr)
{
  return ar8021_phy_mii_read_ex(unit, phy_addr, 20);
}

CEXTERN void  
ar8021_phy_write_epg_2(int unit, uint32_t phy_addr, uint16_t reg)
{
  return ar8021_phy_mii_write_ex(unit, phy_addr, 30, reg);
}

CEXTERN void 
ar8021_phy_rmw_epg_2(int unit, uint32_t phy_addr, uint32_t mask, uint32_t val)
{
  uint16_t reg = ar8021_phy_read_epg_2(unit, phy_addr); 
  if (reg == 0xffff)
    return;
  reg &= ~mask;
  reg |=  val;
  ar8021_phy_write_epg_2(unit, phy_addr, reg);
}
#endif

/* Functions */

CEXTERN void 
ar8021_phy_mii_soft_reset(int unit, uint32_t phy_addr) 
{
  ar8021_phy_mii_rmw_ctrl(unit, phy_addr, 1<<15, 1<<15);
  generic_udelay(10);
}

CEXTERN void  
ar8021_phy_mii_set_loopback(int unit, uint32_t phy_addr, int onOff) 
{
  uint32_t   val = onOff ? 1<<14 : 0;	      
  ar8021_phy_mii_rmw_ctrl(unit, phy_addr, 1<<14, val);
}

CEXTERN void 
ar8021_phy_mii_set_auto(int unit, uint32_t phy_addr, int onOff) 
{
  uint32_t   val = onOff ? 1<<12 : 0;	  
  ar8021_phy_mii_rmw_ctrl(unit, phy_addr, 1<<12, val);
}

CEXTERN void 
ar8021_phy_mii_force_speed(int unit, uint32_t phy_addr, int speed) 
{
  uint32_t val=0;
  if (speed & 1<<0) val |= 1<<6;
  if (speed & 2<<0) val |= 1<<13;
  ar8021_phy_mii_rmw_ctrl(unit, phy_addr, 1<<6 | 1<<13 , val);
}

CEXTERN void 
ar8021_phy_mii_restart_auto(int unit, uint32_t phy_addr, int onOff) 
{
  uint32_t   val = onOff ? 1<<9 : 0;	  
  ar8021_phy_mii_rmw_ctrl(unit, phy_addr, 1<<9, val);
}

CEXTERN void 
ar8021_phy_mii_set_duplex(int unit, uint32_t phy_addr, int onOff) 
{
  uint32_t   val = onOff ? 1<<8 : 0;	  
  ar8021_phy_mii_rmw_ctrl(unit, phy_addr, 1<<8, val);
}

/* IEEE GIGE extended control */

CEXTERN void 
ar8021_phy_set_gige_test_normal(int unit, uint32_t phy_addr) 
{
  ar8021_phy_rmw_gige_ctrl(unit, phy_addr, 7<<13, 0<<13);
}

CEXTERN void 
ar8021_phy_set_gige_test_waveform(int unit, uint32_t phy_addr) 
{
  ar8021_phy_rmw_gige_ctrl(unit, phy_addr, 7<<13, 1<<13);
}

CEXTERN void 
ar8021_phy_set_gige_test_jitter_master(int unit, uint32_t phy_addr) 
{
  ar8021_phy_rmw_gige_ctrl(unit, phy_addr, 7<<13, 2<<13);
}

CEXTERN void 
ar8021_phy_set_gige_test_jitter_slave(int unit, uint32_t phy_addr) 
{
  ar8021_phy_rmw_gige_ctrl(unit, phy_addr, 7<<13, 3<<13);
}

CEXTERN void 
ar8021_phy_set_gige_test_distortion(int unit, uint32_t phy_addr) 
{
  ar8021_phy_rmw_gige_ctrl(unit, phy_addr, 7<<13, 4<<13);
}

CEXTERN void 
ar8021_phy_set_gige_test_master(int unit, uint32_t phy_addr) 
{
  ar8021_phy_rmw_gige_ctrl(unit, phy_addr, 3<<11, 3<<11);
}

CEXTERN void 
ar8021_phy_set_gige_test_slave(int unit, uint32_t phy_addr) 
{
  ar8021_phy_rmw_gige_ctrl(unit, phy_addr, 3<<11, 2<<11);
}

CEXTERN void 
ar8021_phy_set_gige_multiport(int unit, uint32_t phy_addr, int onOff) 
{
  uint32_t   val = onOff ? 1<<10 : 0;	  
  ar8021_phy_rmw_gige_ctrl(unit, phy_addr, 1<<10, val);
}

#if 0
CEXTERN uint16_t
ar8021_phy_get_speed(uint16_t unit, uint16_t phy_addr)
{
  uint16_t speed = (ar8021_phy_read_aux_ctrl_status(unit, phy_addr) >> 3) & 3;
  
  switch(speed) {
  case 0:
    return AG7100_PHY_SPEED_10T;
  case 1:
    return AG7100_PHY_SPEED_100TX;
  case 2:
    return AG7100_PHY_SPEED_1000T;
  default:
    generic_printk("ERROR: Unkown speed read!\n");
    return 0;
  }
}
#endif

static void 
ar8021_phy_setup_generic(uint16_t unit, uint16_t phy_addr)
{
  ar8021_phy_mii_set_duplex(unit, phy_addr, 1);

#ifdef CONFIG_AR9100
  ag7100_mii_write(unit, phy_addr, 0x1f,  0x1);
  ag7100_mii_write(unit, phy_addr, 0x1c,  0x3000);
  ag7100_mii_write(unit, phy_addr, 0x1f,  0x0);
#endif
}

static void 
ar8021_phy_setup_ar8021(uint16_t unit, uint16_t phy_addr)
{
  int i;
  // magic from Atheros F1E code
  ag7100_mii_write(unit, phy_addr, 0x1d, 0x0);	// select debug reg 0
  ag7100_mii_write(unit, phy_addr, 0x1e, 0x34e);	// delay RX clock
  ag7100_mii_write(unit, phy_addr, 0x1d, 0x5);	// select debug reg 5
  ag7100_mii_write(unit, phy_addr, 0x1e, 0x100);	// delay TX clock
  ar8021_phy_mii_set_duplex(unit, phy_addr, 1);

  for (i = 0; i < 500; i++) {
    udelay(1000);
  }
}

CEXTERN int
ar8021_phy_discover_and_setup_phy(int unit)
{
  uint16_t phy_addr;
  uint16_t unit_cnt;  

  unit_cnt=0;
  nmbr_phys=0;

  for (phy_addr=0; phy_addr<31; phy_addr++) {

    uint16_t id1 = ar8021_phy_mii_read_id1(unit, phy_addr);
    uint32_t id2 = ar8021_phy_mii_read_id2(unit, phy_addr);
    uint32_t id  = id1<<16 | id2;

    uint32_t id_sav = 0;
    uint16_t phy_status = 0;
    
    if (id1 > 0 && id1 < 0x7ff) {
      ar8021_phy_mii_soft_reset( unit, phy_addr );
      phy_status=ar8021_phy_mii_read_mode_status(unit, phy_addr);
   
//      generic_printk(" VSCXXX Found %d  unit %d:%d  phy_addr: %d  id: %08x\n",
//		     nmbr_phys, unit, unit_cnt, phy_addr, id);
      
      phy_info[nmbr_phys].id=id;
      phy_info[nmbr_phys].phy_addr=phy_addr;
      phy_info[nmbr_phys].mac_unit=unit_cnt;
      phy_info[nmbr_phys].is_enet_port=0;

      if (id != id_sav) {
	switch(id) {
	case 0x004dd04e:
//	  generic_printk(" PHY is AR8021\n");
	  ar8021_phy_setup_ar8021(unit, phy_addr);
//	  ar8021_phy_setup_generic(unit, phy_addr);
	  phy_info[nmbr_phys].is_enet_port=1;
	  unit_cnt++;
	  break;
	default:
	  generic_printk(" PHY is unknown, using generic IEEE interface\n");
	  ar8021_phy_setup_generic(unit, phy_addr);
	  phy_info[nmbr_phys].is_enet_port=1;
	  unit_cnt++;
	  break;
	}
	id_sav=id;
      }
      nmbr_phys++;
    }
  }
  if (nmbr_phys == 0) {
    generic_printk(" ERROR: No PHY IDs found \n");
    return 1;
  }
  if (unit_cnt == 0) {
    generic_printk(" ERROR: No PHY IDs assigned to unit\n");
    return 1;
  }
  return 0;
}

/* ***********************************************************
 *
 * These are exported for use by ag7100_phy.h
 *
 * *********************************************************** */

int 
ar8021_phy_setup(int unit)
{

  if (ar8021_phy_discover_and_setup_phy(unit))
    return -1;

  return 0;
}
