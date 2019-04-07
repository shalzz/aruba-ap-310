#ifndef _NVRAM_H_
#define _NVRAM_H_

#if defined(CONFIG_ARUBA_CTRL_CPBOOT) || defined(CONFIG_PORFIDIO) || defined(CONFIG_MILAGRO)
#ifndef __KERNEL__
#include "../include/aruba_defs.h"
#endif
#include "xlp_dbg.h"

#define NVRAM_DEVICE_NAME				"/dev/nvram"
#define NVRAM_BASE_ADDRESS               0x0000
#define NVRAM_MANUF_OFFSET               0x0200
#define NVRAM_MANUF_SIZE                 0x0100
/* the next 2 are hard coded in rcS */
#define NVRAM_CFTEST_OFFSET              0x0300
#define NVRAM_CFTEST_SIZE                0x0004
#define NVRAM_VERBOSE_BOOT_OFFSET        0x0304
#define NVRAM_VERBOSE_BOOT_SIZE          0x0004

// NVRAM constants for diagnostic uptime counter
#define NVRAM_UPTIME_BASE                0x0308 // nvram offset of diag uptime counter
#define NVRAM_UPTIME_INIT                0      // 32bit offsets; init flag
#define NVRAM_UPTIME_DATE_SET            1      // date uptime set
#define NVRAM_UPTIME                     2      // uptime counter
#define NVRAM_UPTIME_SIZE                4      // size of uptime values
#define NVRAM_UPTIME_MAGIC               0xbeefd00d // "cowboy"

#if defined(CONFIG_MILAGRO)
#define NVRAM_PANIC_SIZE                 sizeof(xlp_dbg_info_milagro)
#else
#define NVRAM_PANIC_SIZE                 sizeof(xlp_dbg_info)
#endif
 

#define NVRAM_PANIC_OFFSET(x)            (0)
#define NVRAM_PANICS                     1
#define NVRAM_FT_OFFSET                  0x2000
#define NVRAM_FT_SIZE                    0x100
#define NVRAM_REBOOT_STRING_OFFSET       0x7fd0
#define NVRAM_REBOOT_STRING_SIZE         34
#define NVRAM_NOBOOT_SIBYTE_OFFSET       0x7ff2
#define NVRAM_NOBOOT_NANNY_OFFSET        0x7ff3
#define NVRAM_REBOOT_INTENT_OFFSET       0x7ff4

/* donot reorder */
#define NVRAM_REBOOT_NANNY_OFFSET        0x7ff5
#define NVRAM_REBOOT_CAUSE_OFFSET        0x7ff6
/* donot reorder */

#define NVRAM_BOOT_PART_OFFSET           0x7ff7
#define NVRAM_TPM_DATA_OFFSET            0
#define NVRAM_TPM_DATA_SIZE              0x4000
#define NVRAM_SIZE                       0x20000

#endif /* CONFIG_ARUBA_CPBOOT)*/

#if defined(X86_TARGET_PLATFORM)
#ifndef __KERNEL__
#include "../include/aruba_defs.h"
#endif

#define NVRAM_DEVICE_NAME				"/flash/nvramsim.cfg"
#define NVRAM_BASE_ADDRESS               0x0000
#define NVRAM_MANUF_OFFSET               0x0200
#define NVRAM_MANUF_SIZE                 0x0100
/* the next 2 are hard coded in rcS */
#define NVRAM_CFTEST_OFFSET              0x0300
#define NVRAM_CFTEST_SIZE                0x0004
#define NVRAM_VERBOSE_BOOT_OFFSET        0x0304
#define NVRAM_VERBOSE_BOOT_SIZE          0x0004

// NVRAM constants for diagnostic uptime counter
#define NVRAM_UPTIME_BASE                0x0308 // nvram offset of diag uptime counter
#define NVRAM_UPTIME_INIT                0      // 32bit offsets; init flag
#define NVRAM_UPTIME_DATE_SET            1      // date uptime set
#define NVRAM_UPTIME                     2      // uptime counter
#define NVRAM_UPTIME_SIZE                4      // size of uptime values
#define NVRAM_UPTIME_MAGIC               0xbeefd00d // "cowboy"

#define NVRAM_PANIC_SIZE                 sizeof(xlp_dbg_info)
#define NVRAM_PANIC_OFFSET(x)            (0)
#define NVRAM_PANICS                     1
#define NVRAM_FT_OFFSET                  0x2000
#define NVRAM_FT_SIZE                    0x100
#define NVRAM_REBOOT_STRING_OFFSET       0x7fd0
#define NVRAM_REBOOT_STRING_SIZE         34
#define NVRAM_NOBOOT_SIBYTE_OFFSET       0x7ff2
#define NVRAM_NOBOOT_NANNY_OFFSET        0x7ff3
#define NVRAM_REBOOT_INTENT_OFFSET       0x7ff4

/* donot reorder */
#define NVRAM_REBOOT_NANNY_OFFSET        0x7ff5
#define NVRAM_REBOOT_CAUSE_OFFSET        0x7ff6
/* donot reorder */

#define NVRAM_BOOT_PART_OFFSET           0x7ff7
#define NVRAM_TPM_DATA_OFFSET            0
#define NVRAM_TPM_DATA_SIZE              0x4000

#define NVRAM_SERIAL_CONSOLE_OFFSET      0x20001
#define NVRAM_SERIAL_CONSOLE_SIZE        0x00001
#define NVRAM_SIZE                       0x20100

#endif /* X86_TARGET_PLATFORM */

#if defined(CONFIG_GRENACHE) || defined(CONFIG_CORVINA)
#ifndef __KERNEL__
#include "../include/aruba_defs.h"
#endif
#include "xlr_dbg.h"

#define NVRAM_DEVICE_NAME				"/dev/mtd0"
#define NVRAM_BASE_ADDRESS               0x0000
#define NVRAM_MANUF_OFFSET               0x0200
#define NVRAM_MANUF_SIZE                 0x0100
/* the next 2 are hard coded in rcS */
#define NVRAM_CFTEST_OFFSET              0x0300
#define NVRAM_CFTEST_SIZE                0x0004
#define NVRAM_VERBOSE_BOOT_OFFSET        0x0304
#define NVRAM_VERBOSE_BOOT_SIZE          0x0004

// NVRAM constants for forced Pinot FPGA reconfiguration
#define NVRAM_PINOT_FORCE_DNLD_OFFSET    0x0308
#define NVRAM_PINOT_FORCE_DNLD_SIZE      0x0004

// NVRAM constants for diagnostic uptime counter
#define NVRAM_UPTIME_BASE                0x030c // nvram offset of diag uptime counter
#define NVRAM_UPTIME_INIT                0      // 32bit offsets; init flag
#define NVRAM_UPTIME_DATE_SET            1      // date uptime set
#define NVRAM_UPTIME                     2      // uptime counter
#define NVRAM_UPTIME_SIZE                4      // size of uptime values
#define NVRAM_UPTIME_MAGIC               0xbeefd00d // "cowboy"


#define NVRAM_PANIC_SIZE                 sizeof(xlr_dbg_info)
#define NVRAM_PANIC_OFFSET(x)            (0)
#define NVRAM_PANICS                     1
#define NVRAM_FT_OFFSET                  0x2000
#define NVRAM_FT_SIZE                    0x100
#define NVRAM_REBOOT_STRING_OFFSET       0x7fd0
#define NVRAM_REBOOT_STRING_SIZE         34
#define NVRAM_NOBOOT_SIBYTE_OFFSET       0x7ff2
#define NVRAM_NOBOOT_NANNY_OFFSET        0x7ff3
#define NVRAM_REBOOT_INTENT_OFFSET       0x7ff4

/* donot reorder */
#define NVRAM_REBOOT_NANNY_OFFSET        0x7ff5
#define NVRAM_REBOOT_CAUSE_OFFSET        0x7ff6
/* donot reorder */

#define NVRAM_BOOT_PART_OFFSET           0x7ff7
#define NVRAM_TPM_DATA_OFFSET            0xB000    // Grenache with 0x10000 sector size
//#define NVRAM_TPM_DATA_OFFSET            0x18000  // Neb-Dolcetto with a 0x20000 sector size
#define NVRAM_TPM_DATA_SIZE              0x4000
#ifdef CONFIG_CORVINA
#define NVRAM_SIZE                       0x20000
#else
#define NVRAM_SIZE                       0x10000
#endif
#endif  /* CONFIG_GRENACHE */

#if defined(CONFIG_NEBBIOLO)
#ifndef __KERNEL__
#include "../include/aruba_defs.h"
#endif
#include "xlr_dbg.h"

#define NVRAM_DEVICE_NAME				"/dev/nvram-eeprom"
#define NVRAM_BASE_ADDRESS               0x0000
#define NVRAM_MANUF_OFFSET               0x0200
#define NVRAM_MANUF_SIZE                 0x0100
/* the next 2 are hard coded in rcS */
#define NVRAM_CFTEST_OFFSET              0x0300
#define NVRAM_CFTEST_SIZE                0x0004
#define NVRAM_VERBOSE_BOOT_OFFSET        0x0304
#define NVRAM_VERBOSE_BOOT_SIZE          0x0004

// NVRAM constants for forced Pinot FPGA reconfiguration
#define NVRAM_PINOT_FORCE_DNLD_OFFSET    0x0308
#define NVRAM_PINOT_FORCE_DNLD_SIZE      0x0004

// NVRAM constants for diagnostic uptime counter
#define NVRAM_UPTIME_BASE                0x030c // nvram offset of diag uptime counter
#define NVRAM_UPTIME_INIT                0      // 32bit offsets; init flag
#define NVRAM_UPTIME_DATE_SET            1      // date uptime set
#define NVRAM_UPTIME                     2      // uptime counter
#define NVRAM_UPTIME_SIZE                4      // size of uptime values
#define NVRAM_UPTIME_MAGIC               0xbeefd00d // "cowboy"


#define NVRAM_PANIC_SIZE                 sizeof(xlr_dbg_info)
#define NVRAM_PANIC_OFFSET(x)            (0)
#define NVRAM_PANICS                     1
#define NVRAM_FT_OFFSET                  0x2000
#define NVRAM_FT_SIZE                    0x100
#define NVRAM_REBOOT_STRING_OFFSET       0x7fd0
#define NVRAM_REBOOT_STRING_SIZE         34
#define NVRAM_NOBOOT_SIBYTE_OFFSET       0x7ff2
#define NVRAM_NOBOOT_NANNY_OFFSET        0x7ff3
#define NVRAM_REBOOT_INTENT_OFFSET       0x7ff4

/* donot reorder */
#define NVRAM_REBOOT_NANNY_OFFSET        0x7ff5
#define NVRAM_REBOOT_CAUSE_OFFSET        0x7ff6
/* donot reorder */

#define NVRAM_BOOT_PART_OFFSET           0x7ff7
#define NVRAM_TPM_DATA_OFFSET            0x18000
#define NVRAM_TPM_DATA_SIZE              0x4000
#define NVRAM_SIZE                       0x20000
#endif /* CONFIG_NEBBIOLO */

#if defined(CONFIG_MSWITCH)
#define NVRAM_BASE_ADDRESS               0xff000000
#define NVRAM_CPBOOT_OFFSET              0x0000
#define NVRAM_CPBOOT_SIZE                0x0200
#define NVRAM_MANUF_OFFSET               0x0200
#define NVRAM_MANUF_SIZE                 0x0100
/* the next 2 are hard coded in rcS */
#define NVRAM_CFTEST_OFFSET              0x0300
#define NVRAM_CFTEST_SIZE                0x0004
#define NVRAM_VERBOSE_BOOT_OFFSET        0x0304
#define NVRAM_VERBOSE_BOOT_SIZE          0x0004

#define NVRAM_PANIC_SIZE                 0x0400
#define NVRAM_PANIC_OFFSET(x)            (0x1000+((x)*NVRAM_PANIC_SIZE))
#define NVRAM_PANICS                     4
#define NVRAM_FT_OFFSET                  0x2000
#define NVRAM_FT_SIZE                    0x100
#define NVRAM_REBOOT_STRING_OFFSET       0x7fd0
#define NVRAM_REBOOT_STRING_SIZE         34
#define NVRAM_NOBOOT_SIBYTE_OFFSET       0x7ff2
#define NVRAM_NOBOOT_NANNY_OFFSET        0x7ff3
#define NVRAM_REBOOT_INTENT_OFFSET       0x7ff4

/* donot reorder */
#define NVRAM_REBOOT_NANNY_OFFSET        0x7ff5
#define NVRAM_REBOOT_CAUSE_OFFSET        0x7ff6
/* donot reorder */

#define NVRAM_BOOT_PART_OFFSET           0x7ff7
#define NVRAM_SIZE                       0x8000
#endif /* defined(CONFIG_MSWITCH) */

#if defined(CONFIG_CABERNET) || defined(CONFIG_SYRAH) || defined(CONFIG_MALBEC)
/* NVRAM is really in boot FLASH after cpboot */
#define NVRAM_BASE_ADDRESS               0xff87f000
#define NVRAM_CPBOOT_OFFSET              0x000
#define NVRAM_CPBOOT_SIZE                0x200
#define NVRAM_BOOT_PART_OFFSET           0x200
#define NVRAM_NOBOOT_SIBYTE_OFFSET       0x201
#define NVRAM_NOBOOT_NANNY_OFFSET        0x202
#define NVRAM_REBOOT_INTENT_OFFSET       0x203

/* donot reorder */
#define NVRAM_REBOOT_NANNY_OFFSET        0x204
#define NVRAM_REBOOT_CAUSE_OFFSET        0x205
/* donot reorder */

#define NVRAM_REBOOT_STRING_OFFSET       0x206
#define NVRAM_REBOOT_STRING_SIZE         34
/* the next 2 are hard coded in rcS */
#define NVRAM_CFTEST_OFFSET              0x300
#define NVRAM_CFTEST_SIZE                0x004
#define NVRAM_VERBOSE_BOOT_OFFSET        0x304
#define NVRAM_VERBOSE_BOOT_SIZE          0x004
#define NVRAM_COUNTRY_OFFSET		 0x308
#define NVRAM_COUNTRY_SIZE		 0x010

#define NVRAM_PANIC_SIZE                 0x400
#define NVRAM_PANIC_OFFSET(x)            (0x400+((x)*NVRAM_PANIC_SIZE))
#define NVRAM_PANICS                     2
#define NVRAM_EEPROM_OFFSET              0xE00
#define NVRAM_EEPROM_SIZE                0x100
#define NVRAM_MANUF_OFFSET               0xf00
#define NVRAM_MANUF_SIZE                 0x0100
#define NVRAM_SIZE                       0x1000
#endif /* defined(CONFIG_CABERNET) || defined(CONFIG_SYRAH) || defined(CONFIG_MALBEC) */

#ifdef CONFIG_ZEN_MASTER
/* location of NVRAM is unknown XXX */
#define NVRAM_BASE_ADDRESS               0xff87f000
#define NVRAM_CPBOOT_OFFSET              0x000
#define NVRAM_CPBOOT_SIZE                0x200
#define NVRAM_BOOT_PART_OFFSET           0x200
#define NVRAM_NOBOOT_SIBYTE_OFFSET       0x201
#define NVRAM_NOBOOT_NANNY_OFFSET        0x202
#define NVRAM_REBOOT_INTENT_OFFSET       0x203

/* donot reorder */
#define NVRAM_REBOOT_NANNY_OFFSET        0x204
#define NVRAM_REBOOT_CAUSE_OFFSET        0x205
/* donot reorder */

#define NVRAM_REBOOT_STRING_OFFSET       0x206
#define NVRAM_REBOOT_STRING_SIZE         34
#define NVRAM_PANIC_SIZE                 0x400
#define NVRAM_PANIC_OFFSET(x)            (0x400+((x)*NVRAM_PANIC_SIZE))
#define NVRAM_PANICS                     2
#define NVRAM_EEPROM_OFFSET              0xE00
#define NVRAM_EEPROM_SIZE                0x100
#define NVRAM_MANUF_OFFSET               0xf00
#define NVRAM_MANUF_SIZE                 0x0100
#define NVRAM_SIZE                       0x1000
#endif /* CONFIG_ZEN_MASTER */

#endif /* _NVRAM_H_ */
