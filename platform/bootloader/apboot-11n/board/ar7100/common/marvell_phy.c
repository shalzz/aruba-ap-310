/*
 * Manage the ICPLUS ethernet PHY.
 *
 * All definitions in this file are operating system independent!
 */

#include <config.h>
#include <linux/types.h>
#include <common.h>
#include "phy.h"
#include <miiphy.h>
#include "ar7100_soc.h"
#include "marvell_phy.h"
#include "../cpu/mips/ar7100/ag7100_phy.h"

#define AR7100_GPIO_PIN_7  7

extern void ar7100_gpio_intr_enable_by_io(int gpio);

extern void ar7100_gpio_config_int(int gpio,
                                   ar7100_gpio_int_type_t type,
                                   ar7100_gpio_int_pol_t polarity);

/* PHY selections and access functions */

typedef enum {
    PHY_SRCPORT_INFO, 
    PHY_PORTINFO_SIZE,
} PHY_CAP_TYPE;

typedef enum {
    PHY_SRCPORT_NONE,
    PHY_SRCPORT_VLANTAG, 
    PHY_SRCPORT_TRAILER,
} PHY_SRCPORT_TYPE;

#define DEF_PHY_UNIT 0

#ifdef DEBUG
#undef DEBUG
#define DRV_DEBUG 1
#else
#define DRV_DEBUG 0
#endif

#if DRV_DEBUG
#define DRV_DEBUG_PHYERROR  0x00000001
#define DRV_DEBUG_PHYCHANGE 0x00000002
#define DRV_DEBUG_PHYSETUP  0x00000004
#define DRV_DEBUG_PHYINT    0x00000008

int mvPhyDebug = DRV_DEBUG_PHYERROR;

#define DRV_LOG(FLG, X0, X1, X2, X3, X4, X5, X6)    \
{                                                   \
    if (mvPhyDebug & (FLG)) {                       \
        logMsg(X0, X1, X2, X3, X4, X5, X6);         \
    }                                               \
}

#define DRV_MSG(x,a,b,c,d,e,f)                      \
    logMsg(x,a,b,c,d,e,f)

#define DRV_PRINT(FLG, X)                           \
{                                                   \
    if (mvPhyDebug & (FLG)) {                       \
        printf X;                                   \
    }                                               \
}

#else /* !DRV_DEBUG */
#define DRV_LOG(DBG_SW, X0, X1, X2, X3, X4, X5, X6)
#define DRV_MSG(x,a,b,c,d,e,f)
#define DRV_PRINT(DBG_SW,X)
#endif

#define MV_LAN_PORT_VLAN          1
#define MV_WAN_PORT_VLAN          2

#define ENET_UNIT_DEFAULT 0

#define BOOL    uint32_t
#define TRUE    1
#define FALSE   0

/*
 * Track per-PHY port information.
 */
typedef struct {
    BOOL   isEnetPort;       /* normal enet port */
    BOOL   isPhyAlive;       /* last known state of link */
    int    ethUnit;          /* MAC associated with this phy port */
    uint32_t phyBase;
    uint32_t phyAddr;          /* PHY registers associated with this phy port */
    uint32_t VLANTableSetting; /* Value to be written to VLAN table */
} mvPhyInfo_t;

/*
 * Per-PHY information, indexed by PHY unit number.
 */
mvPhyInfo_t mvPhyInfo[] = {

    {TRUE,   /* phy port 0 -- LAN port 0 */
     FALSE,
     ENET_UNIT_DEFAULT,
     0,
     MV_PHY1_ADDR,
     MV_LAN_PORT_VLAN
    },
    {TRUE,   /* phy port 1 -- LAN port 1 */
     FALSE,
     ENET_UNIT_DEFAULT,
     0,
     MV_PHY1_ADDR,
     MV_LAN_PORT_VLAN
    },
};

#define MV_PHY_MAX (sizeof(mvPhyInfo) / sizeof(mvPhyInfo[0]))

/* Convenience macros to access myPhyInfo */
#define MV_IS_ENET_PORT(phyUnit) (mvPhyInfo[phyUnit].isEnetPort)
#define MV_IS_PHY_ALIVE(phyUnit) (mvPhyInfo[phyUnit].isPhyAlive)
#define MV_ETHUNIT(phyUnit) (mvPhyInfo[phyUnit].ethUnit)
#define MV_PHYBASE(phyUnit) (mvPhyInfo[phyUnit].phyBase)
#define MV_PHYADDR(phyUnit) (mvPhyInfo[phyUnit].phyAddr)
#define MV_VLAN_TABLE_SETTING(phyUnit) (mvPhyInfo[phyUnit].VLANTableSetting)


#define MV_IS_ETHUNIT(ethUnit) \
            (MV_IS_ENET_PORT(ethUnit) &&        \
            MV_ETHUNIT(ethUnit) == (ethUnit))

/* Forward references */
BOOL       mv_phyIsLinkAlive(int phyUnit);
static void mv_VLANInit(int ethUnit);
static void mv_verifyReady(int ethUnit);
#ifdef DEBUG
void       mv_phyShow(int phyUnit);
#endif

/******************************************************************************
*
* mv_phyIsLinkAlive - test to see if the specified link is alive
*
* RETURNS:
*    TRUE  --> link is alive
*    FALSE --> link is down
*/
BOOL
mv_phyIsLinkAlive(int phyUnit)
{
    uint16_t phyHwStatus;
    uint32_t phyBase;
    uint32_t phyAddr;

    phyBase = MV_PHYBASE(phyUnit);
    phyAddr = MV_PHYADDR(phyUnit);

    phyHwStatus = phy_reg_read(phyBase, phyAddr, MV_PHY_STATUS);

    if (phyHwStatus & MV_STATUS_LINK_PASS) {
        return TRUE;
    } else {
        return FALSE;
    }
}

/******************************************************************************
*
* mv_VLANInit - initialize "port-based VLANs" for the specified enet unit.
*/
static void
mv_VLANInit(int ethUnit)
{
}


static void
mv_verifyReady(int phyUnit)
{
    uint32_t  phyBase = 0;
    uint32_t  phyAddr;
    uint16_t  phyID1;
    uint16_t  phyID2;

    /*
     * The first read to the Phy port registers always fails and
     * returns 0.   So get things started with a bogus read.
     */
    if (!MV_IS_ETHUNIT(phyUnit)) {
        DRV_PRINT(DRV_DEBUG_PHYERROR, ("Wrong phyUnit %d\n", phyUnit));
    }

    phyBase = MV_PHYBASE(phyUnit);
    phyAddr = MV_PHYADDR(phyUnit);
    
    phyID1 = phy_reg_read(phyBase, phyAddr, MV_PHY_ID1); /* returns 0 */


    /*******************/
    /* Verify phy port */
    /*******************/
    phyBase = MV_PHYBASE(phyUnit);
    phyAddr = MV_PHYADDR(phyUnit);

    phyID1 = phy_reg_read(phyBase, phyAddr, MV_PHY_ID1);
    if (phyID1 != MV_PHY_ID1_EXPECTATION) {
        DRV_PRINT(DRV_DEBUG_PHYERROR,
                  ("Invalid PHY ID1 for enet%d port%d.  Expected 0x%04x, read 0x%04x\n",
                   phyUnit,
                   phyUnit,
                   MV_PHY_ID1_EXPECTATION,
                   phyID1));
 	return;
    }
    
    phyID2 = phy_reg_read(phyBase, phyAddr, MV_PHY_ID2);
    if ((phyID2 & MV_OUI_LSB_MASK) != MV_OUI_LSB_EXPECTATION) {
        DRV_PRINT(DRV_DEBUG_PHYERROR,
                  ("Invalid PHY ID2 for enet%d port %d.  Expected 0x%04x, read 0x%04x\n",
                   phyUnit,
                   phyUnit,
                   MV_OUI_LSB_EXPECTATION,
                   phyID2));
        return;
    }
    
    DRV_PRINT(DRV_DEBUG_PHYERROR,
              ("Found PHY enet%d port%d: model 0x%x revision 0x%x\n",
               phyUnit,
               phyUnit,
               (phyID2 & MV_MODEL_NUM_MASK) >> MV_MODEL_NUM_SHIFT,
               (phyID2 & MV_REV_NUM_MASK) >> MV_REV_NUM_SHIFT));
}

/******************************************************************************
*
* mv_phy_irp - Interrupt handler.
*/
#if 0
static irqreturn_t phy_irq(int cpl, void *dev_id, struct pt_regs *regs)
{
    uint32_t  phyBase = 0;
    uint32_t  phyAddr;
    phyBase = MV_PHYBASE(0);
    phyAddr = MV_PHYADDR(0);
    printk("******Got Phy Interrupt!,status = 0x%04x******\n", phy_reg_read(phyBase, phyAddr, MV_PHY_INT_STATUS));

    return IRQ_HANDLED;
}
BOOL
mv_phyIntInit(int phyUnit)
{
    uint32_t  phyBase = 0;
    uint32_t  phyAddr;

    int req = 0;
    u32 mask = 0;
    uint16_t  rdvalue;

    if (!MV_IS_ETHUNIT(phyUnit)) {
        DRV_PRINT(DRV_DEBUG_PHYERROR, ("Wrong phyUnit %d\n", phyUnit));
	return FALSE;
    }

    phyBase = MV_PHYBASE(phyUnit);
    phyAddr = MV_PHYADDR(phyUnit);

    /*interrupt enable*/
    mask = ar7100_reg_rd(AR7100_MISC_INT_MASK);
    ar7100_reg_wr(AR7100_MISC_INT_MASK, mask | (1 << 2)); /* Enable GPIO interrupt mask */

    /*PHY interrupt*/
    ar7100_gpio_config_int (AR7100_GPIO_PIN_7, INT_TYPE_LEVEL, INT_POL_ACTIVE_LOW);

    ar7100_gpio_intr_enable_by_io(AR7100_GPIO_PIN_7);
    ar7100_gpio_config_input(AR7100_GPIO_PIN_7);

    req = request_irq (AR7100_GPIO_IRQn(AR7100_GPIO_PIN_7), phy_irq, SA_INTERRUPT, "Eth-PHY", NULL);
    if (req != 0) {
        printk (KERN_ERR "unable to request IRQ for Marvell eth PHY(error %d)\n", req);
    }

    /*Enable all interrupt*/
    rdvalue = phy_reg_read(phyBase, phyAddr, MV_PHY_INT_ENABLE);
    rdvalue |= MV_INTERRUPT_ALL;
    phy_reg_write(phyBase, phyAddr, MV_PHY_INT_ENABLE, rdvalue);
   
    DRV_PRINT(DRV_DEBUG_PHY_INT, ("phy interrupt initialized!\n"));
    return TRUE;
}
#endif

/******************************************************************************
*
* mv_phySetup - reset and setup the PHY associated with
* the specified MAC unit number.
*
* Resets the associated PHY port.
*
* RETURNS:
*    TRUE  --> associated PHY is alive
*    FALSE --> no LINKs on this ethernet unit
*/

BOOL
mv_phySetup(int phyUnit)
{
    uint16_t  phyHwStatus;
    uint16_t  timeout;
    int       liveLinks = 0;
    uint32_t  phyBase = 0;
    uint32_t  phyAddr;
    uint16_t  rdvalue;

    if (!MV_IS_ETHUNIT(phyUnit)) {
        DRV_PRINT(DRV_DEBUG_PHYERROR, ("Wrong phyUnit %d\n", phyUnit));
	return FALSE;
    }

    phyBase = MV_PHYBASE(phyUnit);
    phyAddr = MV_PHYADDR(phyUnit);

    /* Enable Fiber/Copper auto selection */
    rdvalue = phy_reg_read(phyBase, phyAddr, MV_PHY_EXT_SPECIFIC_SR2);
    rdvalue &= ~MV_HWCFG_FIBER_COPPER_AUTO;
    phy_reg_write(phyBase, phyAddr, MV_PHY_EXT_SPECIFIC_SR2, rdvalue);

    rdvalue = phy_reg_read(phyBase, phyAddr, MV_PHY_CONTROL);
    rdvalue |= MV_CTRL_SOFTWARE_RESET;
    phy_reg_write(phyBase, phyAddr, MV_PHY_CONTROL, rdvalue);
    mdelay(300);

    rdvalue = phy_reg_read(phyBase, phyAddr, MV_PHY_SPECIFIC_CONTROL2);
#if 1
    rdvalue |= (MV_RX_DELAY | MV_TX_DELAY);
#elif 0
    rdvalue &= ~MV_TX_DELAY;
    rdvalue |= MV_RX_DELAY;
#elif 0
    rdvalue &= ~MV_RX_DELAY;
    rdvalue |= MV_TX_DELAY;
#endif
    phy_reg_write(phyBase, phyAddr, MV_PHY_SPECIFIC_CONTROL2, rdvalue);

    rdvalue = phy_reg_read(phyBase, phyAddr, MV_PHY_EXT_SPECIFIC_SR2);

    rdvalue &= ~(MV_HWCFG_MODE_MASK);

    if (rdvalue & MV_HWCFG_FIBER_COPPER_RES)
       rdvalue |= MV_HWCFG_MODE_FIBER_RGMII;
    else
       rdvalue |= MV_HWCFG_MODE_COPPER_RGMII;

    /*interrupt active low*/
    rdvalue |= MV_INTERRUPT_ACT_LOW;

    phy_reg_write(phyBase, phyAddr, MV_PHY_EXT_SPECIFIC_SR2, rdvalue);

#if 0
    phy_reg_write(phyBase, phyAddr, 0, 0x8000);
                
    phy_reg_write(phyBase, phyAddr, 0x1d, 0x1f);
          
    phy_reg_write(phyBase, phyAddr, 0x1e, 0x200c);
        
    phy_reg_write(phyBase, phyAddr, 0x1d, 0x5);
          
    phy_reg_write(phyBase, phyAddr, 0x1e, 0);
          
    phy_reg_write(phyBase, phyAddr, 0x1e, 0x100);
        
    phy_reg_write(phyBase, phyAddr, 0x10, 0x0060);
        
    phy_reg_write(phyBase, phyAddr, 0x18, 0x4100);
        
    phy_reg_write(phyBase, phyAddr, 0x1d, 0x00);
#endif

    /* start auto negogiation on each phy */
    phy_reg_write(phyBase, phyAddr, MV_AUTONEG_ADVERT, MV_ADVERTISE_ALL);

    rdvalue = phy_reg_read(phyBase, phyAddr, MV_PHY_GB_CONTROL);
    rdvalue |= MV_ADVERTISE_GE_ALL;
    phy_reg_write(phyBase, phyAddr, MV_PHY_GB_CONTROL, rdvalue);

    rdvalue = phy_reg_read(phyBase, phyAddr, MV_PHY_CONTROL);
    rdvalue |= (MV_CTRL_AUTONEGOTIATION_ENABLE | MV_CTRL_SPEED_FULL_DUPLEX | MV_CTRL_SPEED_1000);
    phy_reg_write(phyBase, phyAddr, MV_PHY_CONTROL, rdvalue);

    /* Reset PHYs*/
    rdvalue |= MV_CTRL_SOFTWARE_RESET;
    phy_reg_write(phyBase, phyAddr, MV_PHY_CONTROL, rdvalue);

    /*
     * After the phy is reset, it takes a little while before
     * it can respond properly.
     */
    mdelay(300);

    /* Verify that the switch is what we think it is, and that it's ready */
    mv_verifyReady(phyUnit);

    /* See if there's any configuration data for this enet */
    if (MV_ETHUNIT(phyUnit) != phyUnit) {
        DRV_PRINT(DRV_DEBUG_PHYERROR, ("Wrong phyUnit %d\n", phyUnit));
        return FALSE; /* No PHY's configured for this phyUnit */
    }

#ifdef COBRA_TODO
    /* Initialize global switch settings */

    /* Initialize the aging time */

    /* Set the learning properties */
#endif

    /* start auto negogiation on each phy */
    /*phy_reg_write(phyBase, phyAddr, MV_AUTONEG_ADVERT,
                                    MV_ADVERTISE_ALL);*/
    rdvalue |= MV_CTRL_START_AUTONEGOTIATION;
    phy_reg_write(phyBase, phyAddr, MV_PHY_CONTROL, rdvalue);

    /*
     * Wait up to .75 seconds for ALL associated PHYs to finish
     * autonegotiation.  The only way we get out of here sooner is
     * if ALL PHYs are connected AND finish autonegotiation.
     */
    timeout=5;
    for (;;) {

        phyHwStatus = phy_reg_read(phyBase, phyAddr, MV_PHY_STATUS);

        if (MV_AUTONEG_DONE(phyHwStatus)) {
            DRV_PRINT(DRV_DEBUG_PHYSETUP,
                      ("Port %d, Neg Success\n", phyUnit));
            break;
        }
        if (timeout == 0) {
            DRV_PRINT(DRV_DEBUG_PHYSETUP,
                      ("Port %d, Negogiation timeout\n", phyUnit));
            break;
        }
        if (--timeout == 0) {
            DRV_PRINT(DRV_DEBUG_PHYSETUP,
                      ("Port %d, Negogiation timeout\n", phyUnit));
            break;
        }

        mdelay(150);
    }

    /*
     * All PHYs have had adequate time to autonegotiate.
     * Now initialize software status.
     *
     * It's possible that some ports may take a bit longer
     * to autonegotiate; but we can't wait forever.  They'll
     * get noticed by mv_phyCheckStatusChange during regular
     * polling activities.
     */

    if (mv_phyIsLinkAlive(phyUnit)) {
        liveLinks++;
        MV_IS_PHY_ALIVE(phyUnit) = TRUE;
    } else {
        MV_IS_PHY_ALIVE(phyUnit) = FALSE;
    }

    DRV_PRINT(DRV_DEBUG_PHYSETUP,
        ("eth%d: Phy Status=%4.4x\n",
        phyUnit, 
        phy_reg_read(MV_PHYBASE(phyUnit),
                   MV_PHYADDR(phyUnit),
                   MV_PHY_STATUS)));

    mv_VLANInit(phyUnit);

#ifdef DEBUG
    mv_phyShow(phyUnit);
#endif
    return (liveLinks > 0);
}

/******************************************************************************
*
* mv_phyIsDuplexFull - Determines whether the phy ports associated with the
* specified device are FULL or HALF duplex.
*
* RETURNS:
*    1  --> FULL
*    0 --> HALF
*/
int
mv_phyIsFullDuplex(int phyUnit)
{
    uint32_t  phyBase;
    uint32_t  phyAddr;
    uint16_t  phyHwStatus;

    if (!MV_IS_ETHUNIT(phyUnit)) {
        DRV_PRINT(DRV_DEBUG_PHYERROR, ("Wrong phyUnit %d\n", phyUnit));
        return FALSE;
    }

    if (mv_phyIsLinkAlive(phyUnit)) {

        phyBase = MV_PHYBASE(phyUnit);
        phyAddr = MV_PHYADDR(phyUnit);
        phyHwStatus = phy_reg_read(phyBase, phyAddr, MV_PHY_LINK_STATUS);
#ifdef AZALEA_DEBUG
        //printk("marvell_phy.c: DUP= 0x%x, phyUnit=%d mv_phyIsFullDuplex\n",phyHwStatus & MV_LINK_100BASETX_FULL_DUPLEX, phyUnit);
#endif
        if ((phyHwStatus & MV_PHY_LINK_FULL_DUPLEX) == MV_PHY_LINK_FULL_DUPLEX) {
            return TRUE;
        } else {
            return FALSE;
        }
    }
    return FALSE;
}


/******************************************************************************
*
* mv_phyIsSpeed100 - Determines the speed of phy ports associated with the
* specified device.
*
* RETURNS:
*    TRUE --> 100Mbit
*    FALSE --> 10Mbit
*/

BOOL
mv_phySpeed(int phyUnit)
{
    uint16_t  phyHwStatus;
    uint32_t  phyBase;
    uint32_t  phyAddr;

    uint16_t  phyHwStatus_17;
    if (!MV_IS_ETHUNIT(phyUnit)) {
        DRV_PRINT(DRV_DEBUG_PHYERROR, ("Wrong phyUnit %d\n", phyUnit));
        return AG7100_PHY_SPEED_10T;
    }

    if (mv_phyIsLinkAlive(phyUnit)) {

        phyBase = MV_PHYBASE(phyUnit);
        phyAddr = MV_PHYADDR(phyUnit);

        phyHwStatus = phy_reg_read(phyBase, phyAddr, MV_PHY_LINK_STATUS);
        phyHwStatus_17 = phyHwStatus & MV_PHY_LINK_1000BASETX_M;
        switch (phyHwStatus_17) {
        case MV_PHY_LINK_1000BASETX:
            return AG7100_PHY_SPEED_1000T;
        case MV_PHY_LINK_100BASETX:
            return AG7100_PHY_SPEED_100TX;
        case MV_PHY_LINK_10BASETX:
            return AG7100_PHY_SPEED_10T;
        default:
            printf("Error, eth phy speed error\n");
        }
    }

    return AG7100_PHY_SPEED_10T;
}

/*****************************************************************************
*
* mv_phyCheckStatusChange -- checks for significant changes in PHY state.
*
* A "significant change" is:
*     dropped link (e.g. ethernet cable unplugged) OR
*     autonegotiation completed + link (e.g. ethernet cable plugged in)
*
* When a PHY is plugged in, phyLinkGained is called.
* When a PHY is unplugged, phyLinkLost is called.
*/
int
mv_phyIsUp(int phyUnit)
{

    uint16_t        phyHwStatus;
    mvPhyInfo_t   *lastStatus;
    int           linkCount   = 0;
    int           lostLinks   = 0;
    int           gainedLinks = 0;
    uint32_t        phyBase;
    uint32_t        phyAddr;

    if (!MV_IS_ETHUNIT(phyUnit)) {
        DRV_PRINT(DRV_DEBUG_PHYERROR, ("Wrong phyUnit %d\n", phyUnit));
        return 0;
    }

    phyBase = MV_PHYBASE(phyUnit);
    phyAddr = MV_PHYADDR(phyUnit);

    lastStatus = &mvPhyInfo[phyUnit];
    phyHwStatus = phy_reg_read(phyBase, phyAddr, MV_PHY_STATUS);
#ifdef AZALEA_DEBUG
    printf("marvell_phy.c: phyHwStatus 0x%x, phyUnit=%d\n",phyHwStatus, phyUnit);
#endif

    if (lastStatus->isPhyAlive) { /* last known link status was ALIVE */
        /* See if we've lost link */
        if (phyHwStatus & MV_STATUS_LINK_PASS) {
            linkCount++;
        } else {
            lostLinks++;
#ifdef COBRA_TODO
            mv_flushATUDB(phyUnit);
#endif
            DRV_PRINT(DRV_DEBUG_PHYCHANGE,("\nenet%d port%d down\n",
                                           ethUnit, phyUnit));
            lastStatus->isPhyAlive = FALSE;
        }
    } else { /* last known link status was DEAD */
        /* Check for AutoNegotiation complete */
        if (MV_AUTONEG_DONE(phyHwStatus)) {
            gainedLinks++;
            linkCount++;
            DRV_PRINT(DRV_DEBUG_PHYCHANGE,("\nenet%d port%d up\n",
                                           ethUnit, phyUnit));
            lastStatus->isPhyAlive = TRUE;
#ifdef DEBUG
            mv_phyShow(phyUnit);
#endif
        }
    }

    return (linkCount);

#if 0
    if (linkCount == 0) {
        if (lostLinks) {
            /* We just lost the last link for this MAC */
            phyLinkLost(phyUnit);
        }
    } else {
        if (gainedLinks == linkCount) {
            /* We just gained our first link(s) for this MAC */
            phyLinkGained(phyUnit);
        }
    }
#endif
}

void mv_phy_on(int phyUnit){ 
    uint32_t        phyBase; 
    uint32_t        phyAddr; 
    uint16_t        phyHwControl; 

    if (!MV_IS_ETHUNIT(phyUnit)) { 
        return; 
    } 

    printf("set eth phy power on\n"); 
    phyBase = MV_PHYBASE(phyUnit); 
    phyAddr = MV_PHYADDR(phyUnit); 

    phyHwControl = phy_reg_read(phyBase, phyAddr, MV_PHY_CONTROL); 
    phyHwControl &= ~(0x1<<11); 
    phy_reg_write(phyBase, phyAddr, MV_PHY_CONTROL, phyHwControl); 

}; 
void mv_phy_off(int phyUnit){ 

    uint32_t        phyBase; 
    uint32_t        phyAddr; 
    uint16_t        phyHwControl; 

    if (!MV_IS_ETHUNIT(phyUnit)) { 
        return; 
    } 

    phyBase = MV_PHYBASE(phyUnit); 
    phyAddr = MV_PHYADDR(phyUnit); 
    printf("set eth  phy power off\n"); 
    phyHwControl = phy_reg_read(phyBase, phyAddr, MV_PHY_CONTROL); 
    phyHwControl |= (0x1<<11); 
    phy_reg_write(phyBase, phyAddr, MV_PHY_CONTROL, phyHwControl); 
}; 

#define mv_validPhyId(phy) ((phy) == 0)

#ifdef DEBUG

/* Define the registers of interest for a phyShow command */
typedef struct mvRegisterTableEntry_s {
    uint32_t regNum;
    char  *regIdString;
} mvRegisterTableEntry_t;

mvRegisterTableEntry_t mvPhyRegisterTable[] = {
    {MV_PHY_CONTROL,                 "PHY Control                     "},
    {MV_PHY_STATUS,                  "PHY Status                      "},
    {MV_PHY_ID1,                     "PHY Identifier 1                "},
    {MV_PHY_ID2,                     "PHY Identifier 2                "},
    {MV_AUTONEG_ADVERT,              "Auto-Negotiation Advertisement  "},
    {MV_LINK_PARTNER_ABILITY,        "Link Partner Ability            "},
    {MV_AUTONEG_EXPANSION,           "Auto-Negotiation Expansion      "},
    {MV_AUTONEG_NEXTPAGE,            "Auto-Negotiation NEXTPAGE       "},
    {MV_AUTONEG_LINK_PARTNER_NEXTPAGE,"Auto-Negotiation LP NEXTPAGE    "},
    {MV_PHY_GB_CONTROL,              "1000BASE-T Control Register     "},
    {MV_PHY_GB_STATUS,               "1000BASE-T Status  Register     "},
    {MV_PHY_EXTEND_STATUS,           "Extended Status Register        "},
    {MV_PHY_SPECIFIC_CONTROL1,       "PHY_SPECIFIC_CONTROL1           "},
    {MV_PHY_LINK_STATUS,             "PHY_LINK_STATUS                 "},
    {MV_PHY_SPECIFIC_CONTROL2,       "PHY_SPECIFIC_CONTROL2           "},
};
int mvPhyNumRegs = sizeof(mvPhyRegisterTable) / sizeof(mvPhyRegisterTable[0]);

/*****************************************************************************
*
* mv_phyShow - Dump the state of a PHY.
* There are two sets of registers for each phy port:
*  "phy registers" and
*  "switch port registers"
* We dump 'em all, plus the switch global registers.
*/
void
mv_phyShow(int phyUnit)
{
    int     i;
    uint16_t  value;
    uint32_t  phyBase;
    uint32_t  phyAddr;

    if (!mv_validPhyId(phyUnit)) {
        return;
    }

    phyBase        = MV_PHYBASE(phyUnit);
    phyAddr        = MV_PHYADDR(phyUnit);

    printf("PHY state for PHY%d (enet%d, phyBase 0x%8x, phyAddr 0x%x)\n",
           phyUnit,
           MV_ETHUNIT(phyUnit),
           MV_PHYBASE(phyUnit),
           MV_PHYADDR(phyUnit));

    printf("PHY Registers:\n");
    for (i=0; i < mvPhyNumRegs; i++) {

        value = phy_reg_read(phyBase, phyAddr, mvPhyRegisterTable[i].regNum);

        printf("Reg %02d (0x%02x) %s = 0x%08x\n",
               mvPhyRegisterTable[i].regNum,
               mvPhyRegisterTable[i].regNum,
               mvPhyRegisterTable[i].regIdString,
               value);
    }
}
#endif

/*****************************************************************************
*
* mv_phyGet - Get the value of a PHY register (debug only).
*/
uint32_t
mv_phyGet(int phyUnit, uint32_t regnum)
{
    uint32_t  phyBase;
    uint32_t  phyAddr;


    phyBase = MV_PHYBASE(phyUnit);
    phyAddr = MV_PHYADDR(phyUnit);

    return phy_reg_read(phyBase, phyAddr, regnum);
}

/*****************************************************************************
*
* mv_phySet - Modify the value of a PHY register (debug only).
*/
void
mv_phySet(int phyUnit, uint32_t regnum, uint32_t value)
{
    uint32_t  phyBase;
    uint32_t  phyAddr;

    if (mv_validPhyId(phyUnit)) {

        phyBase = MV_PHYBASE(phyUnit);
        phyAddr = MV_PHYADDR(phyUnit);

        phy_reg_write(phyBase, phyAddr, regnum, value);
    }
}

