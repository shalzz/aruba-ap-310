#ifndef _MXC_H_
#define _MXC_H_

/*
 * Platform dependent definitions
 */
#ifdef CONFIG_MSWITCH
#define MXC_DEV                 "/dev/mxc"
#define MXC_DATAPATH
#define MXC_CPBOOT
#define MXC_MAX_FANS            3
#define MXC_MAX_LCS             3
#define MXC_MAX_PSS             3
#define MXC_MAX_SCS             2
#define MXC_MAX_APSPERLC        24
#define MXC_MAX_FESPERLC        24
#define MXC_MAX_PORTSPERLC      26
#define MXC_MAX_MACS            128
/* Map between internal (scci) and external slot numbering */
#define MXC_INT_SLOT(Slot) \
    (((Slot) < 2)? ((Slot)+1): ((Slot)-2))
#define MXC_EXT_SLOT(Slot) \
    (((Slot) < 2)? ((Slot)+2): ((Slot)-1))
#define MXC_EXT_SLOT_MIN        1
#define MXC_EXT_SLOT_MAX        3
#define MXC_FLASH_SECTOR_SIZE   256
#define MXC_FLASH_BASE_ADDR     0xff800000
#define MXC_SOE_DEV(s,p)       ((s)*MXC_MAX_APSPERLC+(p))
#define MXC_SOE_MULTI_BAUDS
#define MXC_MGMT_INTF
#define MXC_CPU_IDLES           18500000
#define MXC_PORT_CHANNEL
#define MXC_PORT_CHANNELS       8
#define MXC_LC_SWITCH_NAME     "SwitchCore CXE-1000"
#define MXC_LC_SWITCH_CXE
#define MXC_I2C
#endif /* CONFIG_MSWITCH */

#ifdef CONFIG_NEBBIOLO
#define MXC_DEV                 "/dev/mxc"
#define MXC_DATAPATH
#define MXC_MAX_FANS            3
#define MXC_MAX_LCS             4
#define MXC_MAX_PSS             3
#define MXC_MAX_SCS             2
#define MXC_MAX_APSPERLC        24
#define MXC_MAX_FESPERLC        24
#define MXC_MAX_PORTSPERLC      26
#define MXC_MAX_MACS            128
#define MXC_MAX_MACS_DOLCETTO   4
/* Map between internal (scci) and external slot numbering */
#define MXC_INT_SLOT(Slot) \
    (((Slot) < 2)? (3-(Slot)): ((Slot)-2))
#define MXC_EXT_SLOT(Slot) \
    (((Slot) < 2)? ((Slot)+2): (3-(Slot)))
#define MXC_EXT_SLOT_MIN        0
#define MXC_EXT_SLOT_MAX        3
#define MXC_FLASH_SECTOR_SIZE   256
#define MXC_FLASH_BASE_ADDR     0xBC000000
#define MXC_SOE_DEV(s,p)       ((s)*MXC_MAX_APSPERLC+(p))
#define MXC_SOE_MULTI_BAUDS
#define MXC_MGMT_INTF
#define MXC_CPU_IDLES           18500000
#define MXC_PORT_CHANNEL
#define MXC_PORT_CHANNELS       8
#define MXC_LC_SWITCH_NAME     "Broadcom 56308"
#define MXC_I2C
#endif /* CONFIG_NEBBIOLO */

#ifdef CONFIG_CABERNET
#define MXC_DEV                 "/dev/mxc"
#define MXC_DATAPATH
#define MXC_CPBOOT
#define MXC_MAX_FANS            4
#define MXC_MAX_LCS             1
#define MXC_MAX_PSS             1
#define MXC_MAX_SCS             1
#define MXC_MAX_APSPERLC        8
#define MXC_MAX_FESPERLC        8
#define MXC_MAX_PORTSPERLC      9
#define MXC_MAX_MACS            16
#define MXC_INT_SLOT(Slot)      1
#define MXC_EXT_SLOT(Slot)      1
#define MXC_EXT_SLOT_MIN        1
#define MXC_EXT_SLOT_MAX        1
#define MXC_FLASH_SECTOR_SIZE   256
#define MXC_FLASH_BASE_ADDR     0xff800000
#define MXC_SOE_DEV(s,p)       (p)
#define MXC_CPU_IDLES           12300000
#define MXC_PORT_CHANNELS       0
#define MXC_LC_SWITCH_NAME     "RoboSwitch BCM5382"
#define MXC_LC_SWITCH_ROBO
#define MXC_I2C
#endif /* CONFIG_CABERNET */           

#ifdef CONFIG_SYRAH
#define MXC_DEV                 "/dev/mxc"
#define MXC_DATAPATH
#define MXC_CPBOOT
#define MXC_MAX_FANS            4
#define MXC_MAX_LCS             1
#define MXC_MAX_PSS             1
#define MXC_MAX_SCS             1
#define MXC_MAX_APSPERLC        24
#define MXC_MAX_FESPERLC        24
#define MXC_MAX_PORTSPERLC      26
#define MXC_MAX_MACS            32
#define MXC_INT_SLOT(Slot)      1
#define MXC_EXT_SLOT(Slot)      1
#define MXC_EXT_SLOT_MIN        1
#define MXC_EXT_SLOT_MAX        1
#define MXC_FLASH_BASE_ADDR     0xff800000
#define MXC_FLASH_SECTOR_SIZE   256
#define MXC_SOE_DEV(s,p)       (p)
#define MXC_CPU_IDLES           18500000
#define MXC_PORT_CHANNEL
#define MXC_PORT_CHANNELS       8
#define MXC_LC_SWITCH_NAME     "SwitchCore CXE-1000"
#define MXC_LC_SWITCH_CXE
#define MXC_I2C
#endif /* CONFIG_SYRAH */

#ifdef CONFIG_MALBEC
#define MXC_DEV                 "/dev/mxc"
#define MXC_DATAPATH
#define MXC_CPBOOT
#define MXC_MAX_FANS            0
#define MXC_MAX_LCS             1
#define MXC_MAX_PSS             1
#define MXC_MAX_SCS             1
#define MXC_MAX_APSPERLC        0
#define MXC_MAX_FESPERLC        1
#define MXC_MAX_PORTSPERLC      2
#define MXC_MAX_MACS            2
#define MXC_INT_SLOT(Slot)      1
#define MXC_EXT_SLOT(Slot)      1
#define MXC_EXT_SLOT_MIN        1
#define MXC_EXT_SLOT_MAX        1
#define MXC_FLASH_SECTOR_SIZE   256
#define MXC_FLASH_BASE_ADDR     0xff800000
#define MXC_SOE_DEV(s,p)       (p)
#define MXC_CPU_IDLES           12300000
#define MXC_PORT_CHANNEL
#define MXC_PORT_CHANNELS       0
#define MXC_LC_SWITCH_NAME     "No Switch Chip"
#define MXC_I2C
#endif /* CONFIG_MALBEC */

#ifdef CONFIG_GRENACHE
#define MXC_DEV                 "/dev/mxc"
#define MXC_DATAPATH
#define MXC_MAX_FANS            3
#define MXC_MAX_LCS             1
#define MXC_MAX_PSS             1
#define MXC_MAX_SCS             1
#define MXC_MAX_APSPERLC        0
#define MXC_MAX_FESPERLC        0
#define MXC_MAX_PORTSPERLC      8
#define MXC_MAX_PORTSPERLC_620  9
#define MXC_MAX_MACS            16
/* Map between internal (scci) and external slot numbering */
#define MXC_INT_SLOT(Slot)      1
#define MXC_EXT_SLOT(Slot)      1
#define MXC_EXT_SLOT_MIN        1
#define MXC_EXT_SLOT_MAX        1
#define MXC_FLASH_SECTOR_SIZE   256
#define MXC_FLASH_BASE_ADDR     0xBC000000
#define MXC_SOE_DEV(s,p)       (p)
#define MXC_SOE_MULTI_BAUDS
#define MXC_CPU_IDLES           18500000
#define MXC_PORT_CHANNEL
#define MXC_PORT_CHANNELS       8
#define MXC_LC_SWITCH_NAME     "Marvell Link Street 88E6131"
#define MXC_LC_SWITCH_NAME_620 "Marvell Link Street 88E6096"
#define MXC_I2C
#endif /* CONFIG_GRENACHE */

#ifdef CONFIG_ZEN_MASTER
#define MXC_MAX_FANS            0
#define MXC_MAX_LCS             1
#define MXC_MAX_PSS             0
#define MXC_MAX_SCS             1
#define MXC_MAX_APSPERLC        0
#define MXC_MAX_FESPERLC        1
#define MXC_MAX_PORTSPERLC      1
#define MXC_MAX_MACS            1
#define MXC_INT_SLOT(Slot)      1
#define MXC_EXT_SLOT(Slot)      1
#define MXC_EXT_SLOT_MIN        1
#define MXC_EXT_SLOT_MAX        1
#define MXC_PROD_NAME          "Aruba Zen Master"
#define MXC_FLASH_BASE_ADDR     0xff800000
#define MXC_SOE_DEV(s,p)       (p)
#define MXC_CPU_IDLES           18500000
#define MXC_LC_SWITCH_NAME     "No Switch Chip"
#endif /* CONFIG_ZEN_MASTER */

#if defined(CONFIG_PORFIDIO)
#define MXC_DEV                 "/dev/mxc"
#define MXC_DATAPATH
#define MXC_MAX_FANS            3
#define MXC_MAX_LCS             1
#define MXC_MAX_PSS             2
#define MXC_MAX_SCS             1
#define MXC_MAX_APSPERLC        4
#define MXC_MAX_FESPERLC        4
#define MXC_MAX_MACS            16
#define MXC_INT_SLOT(Slot)      Slot
#define MXC_EXT_SLOT(Slot)      Slot
#define MXC_EXT_SLOT_MIN        0
#define MXC_EXT_SLOT_MAX        0
#define MXC_FLASH_SECTOR_SIZE   256
#define MXC_FLASH_BASE_ADDR     0xff800000
#define MXC_SOE_DEV(s,p)       (p)
#define MXC_CPU_IDLES           18500000
#define MXC_PORT_CHANNEL
#define MXC_PORT_CHANNELS       8
#define MXC_LC_SWITCH_NAME     "No Switch Chip"
#define MXC_MAX_PORTSPERLC_7008 8 
#define MXC_MAX_PORTSPERLC_7005 4 
#define MXC_MAX_PORTSPERLC_7010 18
#define MXC_MAX_PORTSPERLC_7030 8
#define MXC_MAX_PORTSPERLC_7205 6
#define MXC_MAX_PORTSPERLC_7024 26
#define MXC_MAX_PORTSPERLC_7200 6
/* define max ports as max of all the platforms */
#define MXC_MAX_PORTSPERLC      MXC_MAX_PORTSPERLC_7024 
#define MXC_I2C
#endif /* CONFIG_PORFIDIO */

#if defined(CONFIG_MILAGRO)
#define MXC_DEV                 "/dev/mxc"
#define MXC_DATAPATH
#define MXC_MAX_FANS            3
#define MXC_MAX_LCS             1
#define MXC_MAX_PSS             2
#define MXC_MAX_SCS             1
#define MXC_MAX_APSPERLC        4
#define MXC_MAX_FESPERLC        4
#define MXC_MAX_MACS            16
#define MXC_INT_SLOT(Slot)      Slot
#define MXC_EXT_SLOT(Slot)      Slot
#define MXC_EXT_SLOT_MIN        0
#define MXC_EXT_SLOT_MAX        0
#define MXC_FLASH_SECTOR_SIZE   256
#define MXC_FLASH_BASE_ADDR     0xff800000
#define MXC_SOE_DEV(s,p)       (p)
#define MXC_CPU_IDLES           18500000
#define MXC_PORT_CHANNEL
#define MXC_PORT_CHANNELS       8
#define MXC_LC_SWITCH_NAME     "No Switch Chip"
#define MXC_MAX_PORTSPERLC_7280 16
/* define max ports as max of all the platforms */
#define MXC_MAX_PORTSPERLC      MXC_MAX_PORTSPERLC_7280 
#define MXC_I2C
#endif /* CONFIG_MILAGRO */

#ifdef X86_TARGET_PLATFORM
#define MXC_DEV                 "/dev/mxc"
#define MXC_DATAPATH
#define MXC_MAX_FANS            3
#define MXC_MAX_LCS             1
#define MXC_MAX_PSS             2
#define MXC_MAX_SCS             1
#define MXC_MAX_APSPERLC        4
#define MXC_MAX_FESPERLC        4
#define MXC_MAX_PORTSPERLC      16
#define MXC_MAX_MACS            16
#define MXC_INT_SLOT(Slot)      1
#define MXC_EXT_SLOT(Slot)      1
#define MXC_EXT_SLOT_MIN        1
#define MXC_EXT_SLOT_MAX        1
#define MXC_FLASH_SECTOR_SIZE   256
#define MXC_FLASH_BASE_ADDR     0xff800000
#define MXC_SOE_DEV(s,p)       (p)
#define MXC_CPU_IDLES           18500000
#define MXC_PORT_CHANNEL
#define MXC_PORT_CHANNELS       8
#define MXC_LC_SWITCH_NAME     "No Switch Chip"
#define MXC_I2C
#endif /* CONFIG_PORFIDIO */

#if !defined(CONFIG_GRENACHE) && (defined (AP_PLATFORM))
/* ATMEL/AMD flash has 63 64KB sectors + 8 8KB sectors */
#define MXC_FLASH_BASE_ADDR     0xbfc00000
#define MXC_FLASH_SECTOR1_NUM   8
#define MXC_FLASH_SECTOR2_NUM   63
#define MXC_FLASH_SECTOR1_SIZE  (8*1024)
#define MXC_FLASH_SECTOR2_SIZE  (64*1024)
#endif /* AP_PLATFORM */


#endif /* _MXC_H_ */
