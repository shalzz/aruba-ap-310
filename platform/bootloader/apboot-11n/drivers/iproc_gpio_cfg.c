/* * Copyright 2012 Broadcom Corporation
 *
 * This program is the proprietary software of Broadcom Corporation and/or
 * its licensors, and may only be used, duplicated, modified or distributed
 * pursuant to the terms and conditions of a separate, written license
 * agreement executed between you and Broadcom (an "Authorized License").
 * Except as set forth in an Authorized License, Broadcom grants no license
 * (express or implied), right to use, or waiver of any kind with respect to
 * the Software, and Broadcom expressly reserves all rights in and to the
 * Software and all intellectual property rights therein.  IF YOU HAVE NO
 * AUTHORIZED LICENSE, THEN YOU HAVE NO RIGHT TO USE THIS SOFTWARE IN ANY WAY,
 * AND SHOULD IMMEDIATELY NOTIFY BROADCOM AND DISCONTINUE ALL USE OF THE
 * SOFTWARE.
 *
 * Except as expressly set forth in the Authorized License,
 *
 * 1. This program, including its structure, sequence and organization,
 * constitutes the valuable trade secrets of Broadcom, and you shall use
 * all reasonable efforts to protect the confidentiality thereof, and to
 * use this information only in connection with your use of Broadcom
 * integrated circuit products.
 *
 * 2. TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED
 * "AS IS" AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES,
 * REPRESENTATIONS OR WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY, OR
 * OTHERWISE, WITH RESPECT TO THE SOFTWARE.  BROADCOM SPECIFICALLY DISCLAIMS
 * ANY AND ALL IMPLIED WARRANTIES OF TITLE, MERCHANTABILITY, NONINFRINGEMENT,
 * FITNESS FOR A PARTICULAR PURPOSE, LACK OF VIRUSES, ACCURACY OR COMPLETENESS,
 * QUIET ENJOYMENT, QUIET POSSESSION OR CORRESPONDENCE TO DESCRIPTION. YOU
 * ASSUME THE ENTIRE RISK ARISING OUT OF USE OR PERFORMANCE OF THE SOFTWARE.
 *
 * 3. TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL BROADCOM OR
 * ITS LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL, INCIDENTAL, SPECIAL,
 * INDIRECT, OR EXEMPLARY DAMAGES WHATSOEVER ARISING OUT OF OR IN ANY WAY
 * RELATING TO YOUR USE OF OR INABILITY TO USE THE SOFTWARE EVEN IF BROADCOM
 * HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES; OR (ii) ANY AMOUNT IN
 * EXCESS OF THE AMOUNT ACTUALLY PAID FOR THE SOFTWARE ITSELF OR U.S. $1,
 * WHICHEVER IS GREATER. THESE LIMITATIONS SHALL APPLY NOTWITHSTANDING ANY
 * FAILURE OF ESSENTIAL PURPOSE OF ANY LIMITED REMEDY.
 */

#include <common.h>
#ifdef CONFIG_IPROC
#include "iproc_gpio.h"
#include "iproc_gpio_cfg.h"

#define readl(addr) (*(volatile unsigned int*)(addr))
#define writel(b,addr) ((*(volatile unsigned int *) (addr)) = (b))
#define __raw_readl readl
#define __raw_writel writel


static regValuePair curRegValue[]=
{
	{NORTHSTAR_DMU_BASE + IPROC_GPIO_CCA_CTRL0,            0},
	{NORTHSTAR_DMU_BASE + IPROC_GPIO_CCA_DS2,              0},
	{NORTHSTAR_DMU_BASE + IPROC_GPIO_CCA_DS1,              0},
	{NORTHSTAR_DMU_BASE + IPROC_GPIO_CCA_DS0,              0},
	{NORTHSTAR_DMU_BASE + IPROC_GPIO_CCA_INPUT_DISABLE,    0},
	{NORTHSTAR_DMU_BASE + IPROC_GPIO_CCA_HYSTERESIS,       0},
	{NORTHSTAR_DMU_BASE + IPROC_GPIO_CCA_SLEWRATE,         0},
	{NORTHSTAR_DMU_BASE + IPROC_GPIO_CCA_PULL_UP,          0},
	{NORTHSTAR_DMU_BASE + IPROC_GPIO_CCA_PULL_DOWN,        0},

};

void iproc_gpio_saveCFGRegisters( void )
{
	int i, size;
	size = sizeof ( curRegValue )/ sizeof( regValuePair );
	for ( i = 0; i < size; i++ )
	{
		curRegValue[i].value = __raw_readl(curRegValue[i].regAddr);
	}
}
int iproc_gpio_restoreRegisters( void )
{
	int i, size;
	size = sizeof ( curRegValue )/ sizeof( regValuePair );
	for ( i = 0; i < size; i++ )
	{
		__raw_writel(curRegValue[i].value,curRegValue[i].regAddr);
		//if ( curRegValue[i].value !=  __raw_readl(curRegValue[i].regAddr))
		//{
		//	return -1;
		//}
	}
	return 0;
}
//set pin configuration: GPIO or iproc function
int iproc_gpio_set_config(iproc_gpio_chip *chip,
			      unsigned int off, unsigned int cfg)
{
    unsigned long aux_sel;
    unsigned int aux_sel_reg;
    unsigned int base;

    base = 0;
    if (chip->id == IPROC_GPIO_CCA_ID) {
        aux_sel_reg = IPROC_GPIO_CCA_CTRL0;
        base = (unsigned int)chip->dmu_ioaddr;
    }
#if 0
    if (chip->id == IPROC_GPIO_CCB_ID) {
        aux_sel_reg = IPROC_GPIO_CCB_AUX_SEL;
        base = (unsigned int)chip->ioaddr;
    }
#endif
    aux_sel = __raw_readl(base + aux_sel_reg);

    switch (cfg) {
        case IPROC_GPIO_GENERAL:
            if (chip->id == IPROC_GPIO_CCA_ID) {
                aux_sel |= (1 << off);
            } else {
                aux_sel &= ~(1 << off);
            }
            break;            
        case IPROC_GPIO_AUX_FUN:
            if (chip->id == IPROC_GPIO_CCA_ID) {
                aux_sel &= ~(1 << off);
            } else {
                aux_sel |= (1 << off);
            }
            break;
        default:
            return -1;
    }
    __raw_writel(aux_sel, base + aux_sel_reg);

    return 0;
}

//Get pin configuration: GPIO or iproc
unsigned iproc_gpio_get_config(iproc_gpio_chip *chip,
				   unsigned int off)
{
    unsigned long aux_sel;
    unsigned int aux_sel_reg;
    unsigned int base;

    base = 0;
    if (chip->id == IPROC_GPIO_CCA_ID) {
        aux_sel_reg = IPROC_GPIO_CCA_CTRL0;
        base = (unsigned int)chip->dmu_ioaddr;
    }
#if 0
    if (chip->id == IPROC_GPIO_CCB_ID) {
        aux_sel_reg = IPROC_GPIO_CCB_AUX_SEL;
        base = (unsigned int)chip->ioaddr;
    }
#endif
    aux_sel = __raw_readl(base + aux_sel_reg);

    if (aux_sel & (1 << off)) {
        if (chip->id == IPROC_GPIO_CCA_ID) {
            return IPROC_GPIO_GENERAL;
        } else {
            return IPROC_GPIO_AUX_FUN;
        }
    }
    else {
        if (chip->id == IPROC_GPIO_CCA_ID) {
            return IPROC_GPIO_AUX_FUN;
        } else {
            return IPROC_GPIO_GENERAL;
        }        
    }        
}


int iproc_gpio_setpull_updown(iproc_gpio_chip *chip,
			    unsigned int off, iproc_gpio_pull_t pull)
{
    unsigned int base;
    base = 0;

    if (chip->id == IPROC_GPIO_CCA_ID) {
        unsigned long pull_up, pull_down;

        base = (unsigned int)chip->dmu_ioaddr;

        pull_up = __raw_readl(base + IPROC_GPIO_CCA_PULL_UP);
        pull_down = __raw_readl(base + IPROC_GPIO_CCA_PULL_DOWN);

        switch (pull) {
            case IPROC_GPIO_PULL_UP:
                pull_up |= (1 << off);
                pull_down &= ~(1 << off);
                break;
            case IPROC_GPIO_PULL_DOWN:
                pull_up &= ~(1 << off);
                pull_down |= (1 << off);
                break;
            case IPROC_GPIO_PULL_NONE:
                pull_up &= ~(1 << off);
                pull_down &= ~(1 << off);
                break;
            default:
                return -1;
        }
        __raw_writel(pull_up, base + IPROC_GPIO_CCA_PULL_UP);
        __raw_writel(pull_down, base + IPROC_GPIO_CCA_PULL_DOWN);

    }
#if 0
    if (chip->id == IPROC_GPIO_CCB_ID) {
        unsigned long pad_res, res_en;        

        base = (unsigned int)chip->ioaddr;

        pad_res = __raw_readl(base + IPROC_GPIO_CCB_PAD_RES);
        res_en = __raw_readl(base + IPROC_GPIO_CCB_RES_EN);

        switch (pull) {
            case IPROC_GPIO_PULL_UP:
                pad_res |= (1 << off);
                res_en |= (1 << off);
                break;
            case IPROC_GPIO_PULL_DOWN:
                pad_res &= ~(1 << off);
                res_en |= (1 << off);
                break;
            case IPROC_GPIO_PULL_NONE:           
                res_en &= ~(1 << off);
                break;                
            default:
                return -EINVAL;
        }
        __raw_writel(pad_res, base + IPROC_GPIO_CCB_PAD_RES);
        __raw_writel(res_en, base + IPROC_GPIO_CCB_RES_EN);
    }
#endif
    return 0;
}


iproc_gpio_pull_t iproc_gpio_getpull_updown(iproc_gpio_chip *chip,
					unsigned int off)
{

    unsigned int base;
    base = 0;

    if (chip->id == IPROC_GPIO_CCA_ID) {
        unsigned long pull_up, pull_down;

        base = (unsigned int)chip->dmu_ioaddr;

        pull_up = __raw_readl(base + IPROC_GPIO_CCA_PULL_UP);
        pull_down = __raw_readl(base + IPROC_GPIO_CCA_PULL_DOWN);
        pull_up &= 1 << off;
        pull_down &= 1 << off;

        if (pull_up ^ pull_down) {
            if (pull_up) {
                return IPROC_GPIO_PULL_UP;
            } else {
                return IPROC_GPIO_PULL_DOWN;
            }                
        } else if(!pull_up) {
            return IPROC_GPIO_PULL_NONE;
        }
    }
#if 0
    if (chip->id == IPROC_GPIO_CCB_ID) {
        unsigned long pad_res, res_en;        

        base = (unsigned int)chip->ioaddr;

        pad_res = __raw_readl(base + IPROC_GPIO_CCB_PAD_RES);
        res_en = __raw_readl(base + IPROC_GPIO_CCB_RES_EN);

        pad_res &= 1 << off;
        res_en &= 1 << off;

        if (res_en) {
            if (pad_res) {
                return IPROC_GPIO_PULL_UP;
            } else {
                return IPROC_GPIO_PULL_DOWN;
            }
        } else {
            return IPROC_GPIO_PULL_NONE;
        }
    }    
#endif
    return IPROC_GPIO_PULL_NONE;
}
int iproc_gpio_setHyeteresis(iproc_gpio_chip *chip,
			    unsigned int off, HysteresisEnable enableDisable)
{
    unsigned int base;
    base = 0;

        unsigned long hysteresis;

        base = (unsigned int)chip->dmu_ioaddr;

        hysteresis = __raw_readl(base + IPROC_GPIO_CCA_HYSTERESIS);

        switch (enableDisable) {
            case ENABLE:
            	hysteresis |= (1 << off);
                break;
            case DISABLE:
            	hysteresis &= ~(1 << off);
                break;
            default:
                return -1;
        }
        __raw_writel(hysteresis, base + IPROC_GPIO_CCA_HYSTERESIS);

    return 0;
}


HysteresisEnable iproc_gpio_getHyeteresis(iproc_gpio_chip *chip,
					unsigned int off)
{

	unsigned long hysteresis;
    unsigned int base;
    base = 0;

        base = (unsigned int)chip->dmu_ioaddr;

        hysteresis = __raw_readl(base + IPROC_GPIO_CCA_HYSTERESIS);

        hysteresis &= 1 << off;

        if (hysteresis)
        {
            return ENABLE;
        }
        else
        {
            return DISABLE;
        }
}
int iproc_gpio_setSlewrate(iproc_gpio_chip *chip,
			    unsigned int off, SlewRateCfg type)
{
    unsigned int base;
    unsigned long slewRate;
    base = 0;

        base = (unsigned int)chip->dmu_ioaddr;

        slewRate = __raw_readl(base + IPROC_GPIO_CCA_SLEWRATE);

        switch (type) {
            case SLEWED_EDGE:
            	slewRate |= (1 << off);
                break;
            case FAST_EDGE:
            	slewRate &= ~(1 << off);
                break;
            default:
                return -1;
        }
        __raw_writel(slewRate, base + IPROC_GPIO_CCA_SLEWRATE);

    return 0;
}


SlewRateCfg iproc_gpio_getSlewrate(iproc_gpio_chip *chip,
					unsigned int off)
{

    unsigned int base;
    unsigned long slewRate;
    base = 0;


        base = (unsigned int)chip->dmu_ioaddr;

        slewRate = __raw_readl(base + IPROC_GPIO_CCA_SLEWRATE);

        slewRate &= 1 << off;

        if (slewRate)
        {
            return SLEWED_EDGE;
        }
        else
        {
            return FAST_EDGE;
        }
}

int iproc_gpio_setDriveStrength(iproc_gpio_chip *chip,
			    unsigned int off, driveStrengthConfig ds)
{
    unsigned int base;
    base = 0;
    unsigned long ds0,ds1,ds2;



        base = (unsigned int)chip->dmu_ioaddr;

        ds2 = __raw_readl(base + IPROC_GPIO_CCA_DS2);
        ds1 = __raw_readl(base + IPROC_GPIO_CCA_DS1);
        ds0 = __raw_readl(base + IPROC_GPIO_CCA_DS0);


        switch (ds) {
            case d_2mA:
            	ds2 &= ~(1 << off);
            	ds1 &= ~(1 << off);
            	ds0 &= ~(1 << off);
                break;
            case d_4mA:
            	ds2 &= ~(1 << off);
            	ds1 &= ~(1 << off);
            	ds0 |= (1 << off);
                break;
            case d_6mA:
            	ds2 &= ~(1 << off);
            	ds1 |=  (1 << off);
            	ds0 &= ~(1 << off);
                break;
            case d_8mA:
            	ds2 &= ~(1 << off);
            	ds1 |=  (1 << off);
            	ds0 |=  (1 << off);
                break;
            case d_10mA:
            	ds2 |=  (1 << off);
            	ds1 &= ~(1 << off);
            	ds0 &= ~(1 << off);
                break;
            case d_12mA:
            	ds2 |=  (1 << off);
            	ds1 &= ~(1 << off);
            	ds0 |=  (1 << off);
                break;
            case d_14mA:
            	ds2 |=  (1 << off);
            	ds1 |=  (1 << off);
            	ds0 &=  ~(1 << off);
                break;
            case d_16mA:
            	ds2 |=  (1 << off);
            	ds1 |=  (1 << off);
            	ds0 |=  (1 << off);
                break;

            default:
                return -1;
        }
        __raw_writel(ds2, base + IPROC_GPIO_CCA_DS2);
        __raw_writel(ds1, base + IPROC_GPIO_CCA_DS1);
        __raw_writel(ds0, base + IPROC_GPIO_CCA_DS0);
    return 0;
}
driveStrengthConfig iproc_gpio_getDriveStrength(iproc_gpio_chip *chip,
					unsigned int offset)
{

    unsigned int base;
    base = 0;
    unsigned long ds0,ds1,ds2, ds;


        base = (unsigned int)chip->dmu_ioaddr;

        ds2 = __raw_readl(base + IPROC_GPIO_CCA_DS2);
        ds1 = __raw_readl(base + IPROC_GPIO_CCA_DS1);
        ds0 = __raw_readl(base + IPROC_GPIO_CCA_DS0);

        ds0 = (( 1 << offset ) & ds0 ) >> offset;
        ds1 = (( 1 << offset ) & ds1 ) >> offset;
        ds2 = (( 1 << offset ) & ds2 ) >> offset;
        ds = ((ds2  << 2 ) | (ds1 << 1 ) | ds0 );
        return ( (driveStrengthConfig)ds);

}
int iproc_gpio_setInputdisable (iproc_gpio_chip *chip,
			    unsigned int off, InputDisableCfg enableDisable)
{
    unsigned int base;
    base = 0;
    unsigned long inputDisabledCfg;


        base = (unsigned int)chip->dmu_ioaddr;

        inputDisabledCfg = __raw_readl(base + IPROC_GPIO_CCA_INPUT_DISABLE);

        switch (enableDisable) {
            case INPUT_DISABLE:
            	inputDisabledCfg |= (1 << off);
                break;
            case INPUT_ENABLE:
            	inputDisabledCfg &= ~(1 << off);
                break;
            default:
                return -1;
        }
        __raw_writel(inputDisabledCfg, base + IPROC_GPIO_CCA_INPUT_DISABLE);

    return 0;
}


InputDisableCfg iproc_gpio_getInputdisable(iproc_gpio_chip *chip,
					unsigned int off)
{

	unsigned long inputDisabledCfg;
    unsigned int base;
    base = 0;


        base = (unsigned int)chip->dmu_ioaddr;

        inputDisabledCfg = __raw_readl(base + IPROC_GPIO_CCA_INPUT_DISABLE);

        inputDisabledCfg &= 1 << off;

        if (inputDisabledCfg)
        {
            return INPUT_DISABLE;
        }
        else
        {
            return INPUT_ENABLE;
        }
}
#endif /* CONFIG_IPROC */
