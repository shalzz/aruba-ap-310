/*
 * icPhy.h - definitions for the ethernet PHY.
 * This code supports a simple 1-port ethernet phy, ICPLUS,
 * All definitions in this file are operating system independent!
 */

#ifndef MARVELLPHY_H
#define MARVELLPHY_H

#define AZALEA
/*****************/
/* PHY Registers */
/*****************/
#define MV_PHY_CONTROL                    0
#define MV_PHY_STATUS                     1
#define MV_PHY_ID1                        2
#define MV_PHY_ID2                        3
#define MV_AUTONEG_ADVERT                 4
#define MV_LINK_PARTNER_ABILITY           5
#define MV_AUTONEG_EXPANSION              6
#define MV_AUTONEG_NEXTPAGE               7
#define MV_AUTONEG_LINK_PARTNER_NEXTPAGE  8
#define MV_PHY_GB_CONTROL                 9
#define MV_PHY_GB_STATUS                  10
#define MV_PHY_EXTEND_STATUS              15
#define MV_PHY_SPECIFIC_CONTROL1          16
#define MV_PHY_LINK_STATUS                17
#define MV_PHY_INT_ENABLE                 18
#define MV_PHY_INT_STATUS                 19
#define MV_PHY_SPECIFIC_CONTROL2          20
#define MV_PHY_RX_ERROR_COUNTER           21
#define MV_PHY_PAGE                       22
#define MV_PHY_GLB_STATUS                 23
#define MV_PHY_LED_CONTROL                24
#define MV_PHY_MAN_LED_OVERRIDE           25
#define MV_PHY_EXT_SPECIFIC_CR2           26
#define MV_PHY_EXT_SPECIFIC_SR2           27
#define MV_PHY_MDI_VIRTUAL_CABLE_TESTER   28
#define MV_PHY_PAGE2                      29

/* MV_PHY_LINK_STATUS */
#define MV_PHY_LINK_FULL_DUPLEX        0x2000
#define MV_PHY_LINK_1000BASETX_M       0xc000
#define MV_PHY_LINK_1000BASETX         0x8000
#define MV_PHY_LINK_100BASETX          0x4000
#define MV_PHY_LINK_10BASETX           0x0000
#define MV_PHY_LINK_UP		       0x0400

/* MV_PHY_CONTROL fields */
#define MV_CTRL_SOFTWARE_RESET                    0x8000
#define MV_CTRL_LOOPBAK                           0x4000
#define MV_CTRL_SPEED_100                         0x2000
#define MV_CTRL_AUTONEGOTIATION_ENABLE            0x1000
#define MV_CTRL_START_AUTONEGOTIATION             0x0200
#define MV_CTRL_SPEED_FULL_DUPLEX                 0x0100
#define MV_CTRL_SPEED_1000                        0x0040

/* Phy status fields */
#define MV_STATUS_AUTO_NEG_DONE                   0x0020
#define MV_STATUS_LINK_PASS                       0x0004

#define MV_AUTONEG_DONE(mv_phy_status)                   \
    (((mv_phy_status) &                                  \
        (MV_STATUS_AUTO_NEG_DONE)) ==                    \
        (MV_STATUS_AUTO_NEG_DONE))

/* Mask used for ID comparisons */
#define MARVELL_PHY_ID_MASK             0xfffffff0

/* Known PHY IDs */
#define MARVELL_PHY_ID_88E1111          0x01410cc0

/* MARVELL_PHY_ID1 fields */
#define MV_PHY_ID1_EXPECTATION                    0x0141 /* OUI >> 6 */

/* MARVELL fields */
#define MV_OUI_LSB_MASK                           0xfc00
#define MV_OUI_LSB_EXPECTATION                    0x0c00
#define MV_OUI_LSB_SHIFT                              10
#define MV_MODEL_NUM_MASK                         0x03f0
#define MV_MODEL_NUM_EXPECTATION                  0x00c0
#define MV_MODEL_NUM_SHIFT                             4
#define MV_REV_NUM_MASK                           0x000f
#define MV_REV_NUM_SHIFT                               0

/* Link Partner ability */
#define MV_LINK_100BASETX_FULL_DUPLEX       0x0100
#define MV_LINK_100BASETX                   0x0080
#define MV_LINK_10BASETX_FULL_DUPLEX        0x0040
#define MV_LINK_10BASETX                    0x0020

/* Advertisement register. */
#define MV_ADVERTISE_100FULL                0x0100
#define MV_ADVERTISE_100HALF                0x0080  
#define MV_ADVERTISE_10FULL                 0x0040  
#define MV_ADVERTISE_10HALF                 0x0020  
#define MV_ADVERTISE_802_3                  0x0001
#define MV_ADVERTISE_PAUSE_CAP              0x0400  /* Try for pause               */
#define MV_ADVERTISE_PAUSE_ASYM             0x0800  /* Try for asymetric pause     */



#define MV_ADVERTISE_ALL (MV_ADVERTISE_10HALF | MV_ADVERTISE_10FULL | \
                       MV_ADVERTISE_100HALF | MV_ADVERTISE_100FULL | \
                       MV_ADVERTISE_PAUSE_CAP | MV_ADVERTISE_PAUSE_ASYM | \
                       MV_ADVERTISE_802_3)

#define MV_ADVERTISE_1000FULL               0x0200
#define MV_ADVERTISE_1000HALF               0x0100
#define MV_ADVERTISE_GE_ALL (MV_ADVERTISE_1000FULL | MV_ADVERTISE_1000HALF)
               
/*specific*/
#define MV_PHY_LED_DIRECT        0x4100
#define MV_PHY_LED_COMBINE       0x411c
#define MV_PHY_EXT_CR            0x14
#define MV_RX_DELAY              0x80
#define MV_TX_DELAY              0x2

#define MV_HWCFG_MODE_MASK               0xf
#define MV_HWCFG_MODE_COPPER_RGMII       0xb
#define MV_HWCFG_MODE_FIBER_RGMII        0x3
#define MV_HWCFG_MODE_SGMII_NO_CLK       0x4
#define MV_HWCFG_MODE_COPPER_RTBI        0x9
#define MV_HWCFG_FIBER_COPPER_AUTO       0x8000
#define MV_HWCFG_FIBER_COPPER_RES        0x2000
        
#define MV_COPPER                0
#define MV_FIBER                 1

/*interrupt*/
#define MV_INTERRUPT_ACT_LOW         0x0400
#define MV_INTERRUPT_ENERGY          0x0010
#define MV_INTERRUPT_FIFO_OUF        0x0080
#define MV_INTERRUPT_FALSE_CARRIER   0x0100
#define MV_INTERRUPT_SYM_ERROR       0x0200
#define MV_INTERRUPT_LINK_STATUS_CH  0x0400
#define MV_INTERRUPT_AUTO_NEG_COMP   0x0800
#define MV_INTERRUPT_PAGE_REC        0x1000
#define MV_INTERRUPT_DUPLEX_CH       0x2000
#define MV_INTERRUPT_SPEED_CH        0x4000

#define MV_INTERRUPT_ALL (MV_INTERRUPT_ENERGY | MV_INTERRUPT_FIFO_OUF | MV_INTERRUPT_FALSE_CARRIER | \
                          MV_INTERRUPT_SYM_ERROR | MV_INTERRUPT_LINK_STATUS_CH | MV_INTERRUPT_AUTO_NEG_COMP | \
                          MV_INTERRUPT_PAGE_REC | MV_INTERRUPT_DUPLEX_CH | MV_INTERRUPT_SPEED_CH)

/* PHY Addresses */
#define MV_PHY0_ADDR    0
#define MV_PHY1_ADDR    1

/* Azalea */
#define BOOL    uint32_t
int mv_phyIsFullDuplex(int ethUnit);
BOOL mv_phySpeed(int ethUnit);
#ifdef AZALEA
int mv_phyIsUp(int ethUnit);
#else
void mv_phyIsUp(int ethUnit);
#endif
BOOL mv_phySetup(int ethUnit);
BOOL mv_phyIntInit(int ethUnit);

void mv_phy_on(int phyUnit); 
void mv_phy_off(int phyUnit); 
uint32_t mv_phyGet(int phyUnit, uint32_t regnum);
void mv_phySet(int phyUnit, uint32_t regnum, uint32_t value);

#endif
