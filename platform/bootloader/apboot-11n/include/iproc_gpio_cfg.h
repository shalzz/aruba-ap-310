/*
 * Copyright 2012 Broadcom Corporation
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


#ifndef __PLAT_GPIO_CFG_H
#define __PLAT_GPIO_CFG_H


typedef unsigned int iproc_gpio_drvstr_t;

#define IPROC_GPIO_GENERAL  1
#define IPROC_GPIO_AUX_FUN  0

/* Define values for the pull-{up,down} available for each gpio pin.
 *
 * These values control the state of the weak pull-{up,down} resistors.
 */
typedef enum gpioPullCfg
{
	IPROC_GPIO_PULL_NONE,
	IPROC_GPIO_PULL_DOWN,
	IPROC_GPIO_PULL_UP,
}iproc_gpio_pull_t;
typedef enum config
{
	DISABLE,
	ENABLE,
}HysteresisEnable;
typedef enum slewRate
{
	FAST_EDGE,
	SLEWED_EDGE,
}SlewRateCfg;
typedef enum driveStrength
{
    d_2mA,
    d_4mA,
    d_6mA,
    d_8mA,
    d_10mA,
    d_12mA,
    d_14mA,
    d_16mA,
}driveStrengthConfig;

typedef struct regValue {
	unsigned long regAddr;
	unsigned long value;
}regValuePair;

typedef enum inputDisable
{
	INPUT_ENABLE,
	INPUT_DISABLE
}InputDisableCfg;

/* internal gpio functions */
extern int iproc_gpio_setpull_updown(iproc_gpio_chip *chip,
			    unsigned int off, iproc_gpio_pull_t pull);

extern iproc_gpio_pull_t iproc_gpio_getpull_updown(iproc_gpio_chip *chip,
					unsigned int off);

extern int iproc_gpio_set_config(iproc_gpio_chip *chip,
			      unsigned int off, unsigned int cfg);
			      
unsigned iproc_gpio_get_config(iproc_gpio_chip *chip,
				   unsigned int off);
int iproc_gpio_setHyeteresis(iproc_gpio_chip *chip,
			    unsigned int off, HysteresisEnable enableDisable);
HysteresisEnable iproc_gpio_getHyeteresis(iproc_gpio_chip *chip,
					unsigned int off);
int iproc_gpio_setSlewrate(iproc_gpio_chip *chip,
			    unsigned int off, SlewRateCfg type);
SlewRateCfg iproc_gpio_getSlewrate(iproc_gpio_chip *chip,
					unsigned int off);
int iproc_gpio_setDriveStrength(iproc_gpio_chip *chip,
			    unsigned int off, driveStrengthConfig ds);
driveStrengthConfig iproc_gpio_getDriveStrength(iproc_gpio_chip *chip,
					unsigned int off);
int iproc_gpio_restoreRegisters( void );
void iproc_gpio_saveCFGRegisters( void );
InputDisableCfg iproc_gpio_getInputdisable(iproc_gpio_chip *chip,
					unsigned int off);
int iproc_gpio_setInputdisable (iproc_gpio_chip *chip,
			    unsigned int off, InputDisableCfg enableDisable);
#endif				   
