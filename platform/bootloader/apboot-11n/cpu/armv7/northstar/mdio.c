#include <config.h>
#include <common.h>
#include <asm/arch/iproc_regs.h>
#include "asm/arch/socregs.h"
#include "asm/arch/reg_utils.h"

/* ChipcommonB MDIO register set */
/* MII register definition */
#define  MII_MGMT              ChipcommonB_MII_Management_Control /*0x18003000*/
#define  MII_CMD_DATA          ChipcommonB_MII_Management_Command_Data /*0x18003004*/
/* fields in MII_MGMT */
#define MII_MGMT_BYP_MASK		0x00000400
#define MII_MGMT_BYP_SHIFT	    10
#define MII_MGMT_EXP_MASK		0x00000200
#define MII_MGMT_EXP_SHIFT	    9
#define MII_MGMT_BSY_MASK		0x00000100
#define MII_MGMT_BSY_SHIFT	    8
#define MII_MGMT_PRE_MASK		0x00000080
#define MII_MGMT_PRE_SHIFT	    7
#define MII_MGMT_MDCDIV_MASK	0x0000007f
#define MII_MGMT_MDCDIV_SHIFT	0
/* fields in MII_CMD_DATA */
#define MII_CMD_DATA_SB_MASK		0xc0000000
#define MII_CMD_DATA_SB_SHIFT	    30
#define MII_CMD_DATA_OP_MASK		0x30000000
#define MII_CMD_DATA_OP_SHIFT	    28
#define MII_CMD_DATA_PA_MASK		0x0f800000
#define MII_CMD_DATA_PA_SHIFT	    23
#define MII_CMD_DATA_RA_MASK		0x007c0000
#define MII_CMD_DATA_RA_SHIFT	    18
#define MII_CMD_DATA_TA_MASK		0x00030000
#define MII_CMD_DATA_TA_SHIFT	    16
#define MII_CMD_DATA_DATA_MASK		0x0000ffff
#define MII_CMD_DATA_DATA_SHIFT	    0

#define MII_TRIES 100000
#define MII_POLL_USEC                     20

/* 
 * dev_type = 0: LOCAL device
 * dev_type = 1: EXTERNAL device
 */
int
ccb_mii_read(int dev_type, int phy_addr, int reg_off, uint16_t *data)
{
    int i;
    uint32_t ctrl = 0;

    ctrl = 0xcd;
    if (dev_type == 0) {
        ctrl &= ~MII_MGMT_EXP_MASK;
    } else {
        ctrl |= MII_MGMT_EXP_MASK;
    }
    reg32_write(MII_MGMT, ctrl);

    for (i = 0; i < MII_TRIES; i++) {
        ctrl = reg32_read(MII_MGMT);
        if (!(ctrl & MII_MGMT_BSY_MASK)) {
            break;
        }
        udelay(MII_POLL_USEC);
    }
    if (i >= MII_TRIES) {
        printf("MDIO ERROR: %s: BUSY stuck: ctrl=0x%x, count=%d\n", __FUNCTION__, ctrl, i);
        return 1;
    }

    ctrl = ((1 << MII_CMD_DATA_SB_SHIFT) & MII_CMD_DATA_SB_MASK) |
        ((2 << MII_CMD_DATA_OP_SHIFT) & MII_CMD_DATA_OP_MASK) |
        ((phy_addr << MII_CMD_DATA_PA_SHIFT) & MII_CMD_DATA_PA_MASK) |
        ((reg_off << MII_CMD_DATA_RA_SHIFT) & MII_CMD_DATA_RA_MASK) |
        ((2 << MII_CMD_DATA_TA_SHIFT) & MII_CMD_DATA_TA_MASK);
    reg32_write(MII_CMD_DATA, ctrl);


    for (i = 0; i < MII_TRIES; i++) {
        ctrl = reg32_read(MII_MGMT);
        if (!(ctrl & MII_MGMT_BSY_MASK)) {
            break;
        }
        udelay(MII_POLL_USEC);
    }
    if (i >= MII_TRIES) {
        printf("MDIO ERROR: \n%s: BUSY stuck: ctrl=0x%x, count=%d\n", __FUNCTION__, ctrl, i);
        return 1;
    }

    ctrl = reg32_read(MII_CMD_DATA);
	*data = ctrl & 0xffff;

    return 0;
}

int
ccb_mii_write(int dev_type, int phy_addr, int reg_off, uint16_t data)
{
    int i;
    uint32_t ctrl = 0;

    ctrl = 0xcd;
    if (dev_type == 0) {
        ctrl &= ~MII_MGMT_EXP_MASK;
    } else {
        ctrl |= MII_MGMT_EXP_MASK;
    }
    reg32_write(MII_MGMT, ctrl);

    for (i = 0; i < MII_TRIES; i++) {
        ctrl = reg32_read(MII_MGMT);
        if (!(ctrl & MII_MGMT_BSY_MASK)) {
            break;
        }
        udelay(MII_POLL_USEC);
    }
    if (i >= MII_TRIES) {
        printf("\n%s: BUSY stuck: ctrl=0x%x, count=%d\n", __FUNCTION__, ctrl, i);
        return 1;
    }

    ctrl = ((1 << MII_CMD_DATA_SB_SHIFT) & MII_CMD_DATA_SB_MASK) |
        ((1 << MII_CMD_DATA_OP_SHIFT) & MII_CMD_DATA_OP_MASK) |
        ((phy_addr << MII_CMD_DATA_PA_SHIFT) & MII_CMD_DATA_PA_MASK) |
        ((reg_off << MII_CMD_DATA_RA_SHIFT) & MII_CMD_DATA_RA_MASK) |
        ((2 << MII_CMD_DATA_TA_SHIFT) & MII_CMD_DATA_TA_MASK) |
        ((data << MII_CMD_DATA_DATA_SHIFT) & MII_CMD_DATA_DATA_MASK);
    reg32_write(MII_CMD_DATA, ctrl);


    for (i = 0; i < MII_TRIES; i++) {
        ctrl = reg32_read(MII_MGMT);
        if (!(ctrl & MII_MGMT_BSY_MASK)) {
            break;
        }
        udelay(MII_POLL_USEC);
    }
    if (i >= MII_TRIES) {
        printf("\n%s: BUSY stuck: ctrl=0x%x, count=%d\n", __FUNCTION__, ctrl, i);
        return 1;
    }

    return 0;
}
