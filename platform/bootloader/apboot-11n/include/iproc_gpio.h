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

#ifndef __IPROC_PLAT_GPIO_H
#define __IPROC_PLAT_GPIO_H

#define  CCA_CHIPID                                                   0x18000000

#define  CCA_INT_STS                                                  0x18000020
#define  CCA_INT_STS_BASE                                                  0x020
#define  CCA_INT_STS_DATAMASK                                         0xffffffff
#define  CCA_INT_STS_RDWRMASK                                         0x00000000
#define  CCA_INT_MASK                                                 0x18000024
#define  CCA_INT_MASK_BASE                                                 0x024
#define  CCA_INT_MASK_DATAMASK                                        0xffffffff
#define  CCA_INT_MASK_RDWRMASK                                        0x00000000
#define  CCA_CHIP_CTRL                                                0x18000028
#define  CCA_CHIP_CTRL_BASE                                                0x028
#define  CCA_CHIP_CTRL_CTRL_RST_VAL                                   0x00000000
#define  CCA_CHIP_CTRL_DATAMASK                                       0xffffffff
#define  CCA_CHIP_CTRL_RDWRMASK                                       0x00000000
#define  CCA_CHIP_STS                                                 0x1800002c
#define  CCA_CHIP_STS_BASE                                                 0x02c
#define  CCA_CHIP_STS_RST_VAL                                         0x00000000
#define  CCA_CHIP_STS_DATAMASK                                        0xffffffff
#define  CCA_CHIP_STS_RDWRMASK                                        0x00000000



#define  CCA_GPIO_PULL_UP                                             0x18000058
#define  CCA_GPIO_PULL_UP_BASE                                             0x058
#define  CCA_GPIO_PULL_UP_RST_VAL                                     0x00000000
#define  CCA_GPIO_PULL_UP_DATAMASK                                    0xffffffff
#define  CCA_GPIO_PULL_UP_RDWRMASK                                    0x00000000
#define  CCA_GPIO_PULL_DN                                             0x1800005c
#define  CCA_GPIO_PULL_DN_BASE                                             0x05c
#define  CCA_GPIO_PULL_DN_RST_VAL                                     0x00000000
#define  CCA_GPIO_PULL_DN_DATAMASK                                    0xffffffff
#define  CCA_GPIO_PULL_DN_RDWRMASK                                    0x00000000
#define  CCA_GPIO_INPUT                                               0x18000060
#define  CCA_GPIO_INPUT_BASE                                               0x060
#define  CCA_GPIO_INPUT_RST_VAL                                       0x00000000
#define  CCA_GPIO_INPUT_DATAMASK                                      0xffffffff
#define  CCA_GPIO_INPUT_RDWRMASK                                      0x00000000
#define  CCA_GPIO_OUT                                                 0x18000064
#define  CCA_GPIO_OUT_BASE                                                 0x064
#define  CCA_GPIO_OUT_GPIO_OUT_RST_VAL                                0x00000000
#define  CCA_GPIO_OUT_DATAMASK                                        0xffffffff
#define  CCA_GPIO_OUT_RDWRMASK                                        0x00000000
#define  CCA_GPIO_OUT_EN                                              0x18000068
#define  CCA_GPIO_OUT_EN_BASE                                              0x068
#define  CCA_GPIO_OUT_EN_RST_VAL                                      0x00000000
#define  CCA_GPIO_OUT_EN_DATAMASK                                     0xffffffff
#define  CCA_GPIO_OUT_EN_RDWRMASK                                     0x00000000
#define  CCA_GPIO_CTRL                                                0x1800006c
#define  CCA_GPIO_CTRL_BASE                                                0x06c
#define  CCA_GPIO_CTRL_CTRL_RST_VAL                                   0x00000000
#define  CCA_GPIO_CTRL_DATAMASK                                       0xffffffff
#define  CCA_GPIO_CTRL_RDWRMASK                                       0x00000000
#define  CCA_GPIO_INT_POLARITY                                        0x18000070
#define  CCA_GPIO_INT_POLARITY_BASE                                        0x070
#define  CCA_GPIO_INT_POLARITY_RST_VAL                                0x00000000
#define  CCA_GPIO_INT_POLARITY_DATAMASK                               0xffffffff
#define  CCA_GPIO_INT_POLARITY_RDWRMASK                               0x00000000
#define  CCA_GPIOINT_MASK                                             0x18000074
#define  CCA_GPIOINT_MASK_BASE                                             0x074
#define  CCA_GPIOINT_MASK_RST_VAL                                     0x00000000
#define  CCA_GPIOINT_MASK_DATAMASK                                    0xffffffff
#define  CCA_GPIOINT_MASK_RDWRMASK                                    0x00000000
#define  CCA_GPIO_EVT                                                 0x18000078
#define  CCA_GPIO_EVT_BASE                                                 0x078
#define  CCA_GPIO_EVT_GPIO_EVT_RST_VAL                                0x00000000
#define  CCA_GPIO_EVT_DATAMASK                                        0xffffffff
#define  CCA_GPIO_EVT_RDWRMASK                                        0x00000000
#define  CCA_GPIO_EVTINT_MASK                                         0x1800007c
#define  CCA_GPIO_EVTINT_MASK_BASE                                         0x07c
#define  CCA_GPIO_EVTINT_MASK_EVTINT_MASK_RST_VAL                     0x00000000
#define  CCA_GPIO_EVTINT_MASK_DATAMASK                                0xffffffff
#define  CCA_GPIO_EVTINT_MASK_RDWRMASK                                0x00000000
#define  CCA_WDOG_CTR                                                 0x18000080
#define  CCA_WDOG_CTR_BASE                                                 0x080
#define  CCA_WDOG_CTR_COUNTER_RST_VAL                                 0x00000000
#define  CCA_WDOG_CTR_DATAMASK                                        0xffffffff
#define  CCA_WDOG_CTR_RDWRMASK                                        0x00000000
#define  CCA_GPIO_EVT_INT_POLARITY                                    0x18000084
#define  CCA_GPIO_EVT_INT_POLARITY_BASE                                    0x084
#define  CCA_GPIO_EVT_INT_POLARITY_INT_POLARITY_RST_VAL               0x00000000
#define  CCA_GPIO_EVT_INT_POLARITY_DATAMASK                           0xffffffff
#define  CCA_GPIO_EVT_INT_POLARITY_RDWRMASK                           0x00000000
#define  CCA_GPIO_TMR_VAL                                             0x18000088
#define  CCA_GPIO_TMR_VAL_BASE                                             0x088
#define  CCA_GPIO_TMR_VAL_DATAMASK                                    0xffffffff
#define  CCA_GPIO_TMR_VAL_RDWRMASK                                    0x00000000
#define  CCA_GPIO_TMR_OUT_MASK                                        0x1800008c
#define  CCA_GPIO_TMR_OUT_MASK_BASE                                        0x08c
#define  CCA_GPIO_TMR_OUT_MASK_TMR_OUT_MASK_RST_VAL                   0x00000000
#define  CCA_GPIO_TMR_OUT_MASK_DATAMASK                               0xffffffff
#define  CCA_GPIO_TMR_OUT_MASK_RDWRMASK                               0x00000000

#define IPROC_GPIO_REG_SIZE     (0x50)


/* Chipcommon A GPIO */
#define IPROC_GPIO_CCA_BASE CCA_GPIO_INPUT

#define IPROC_GPIO_CCA_DIN              (CCA_GPIO_INPUT_BASE -  CCA_GPIO_INPUT_BASE)
#define IPROC_GPIO_CCA_DOUT             (CCA_GPIO_OUT_BASE - CCA_GPIO_INPUT_BASE)
#define IPROC_GPIO_CCA_EN               (CCA_GPIO_OUT_EN_BASE - CCA_GPIO_INPUT_BASE)
#define IPROC_GPIO_CCA_INT_LEVEL        (CCA_GPIO_INT_POLARITY_BASE - CCA_GPIO_INPUT_BASE)
#define IPROC_GPIO_CCA_INT_LEVEL_MASK   (CCA_GPIOINT_MASK_BASE - CCA_GPIO_INPUT_BASE)
#define IPROC_GPIO_CCA_INT_EVENT        (CCA_GPIO_EVT_BASE - CCA_GPIO_INPUT_BASE)
#define IPROC_GPIO_CCA_INT_EVENT_MASK   (CCA_GPIO_EVTINT_MASK_BASE - CCA_GPIO_INPUT_BASE)
#define IPROC_GPIO_CCA_WATCHDOG_COUNTER (CCA_WDOG_CTR_BASE - CCA_GPIO_INPUT_BASE)
#define IPROC_GPIO_CCA_INT_EDGE         (CCA_GPIO_EVT_INT_POLARITY_BASE - CCA_GPIO_INPUT_BASE)
#define IPROC_GPIO_CCA_TIMER_VAL        (CCA_GPIO_TMR_VAL_BASE - CCA_GPIO_INPUT_BASE)
#define IPROC_GPIO_CCA_TIMEOUT_MASK     (CCA_GPIO_TMR_OUT_MASK_BASE - CCA_GPIO_INPUT_BASE)
#define IPROC_GPIO_CCA_CLK_DIV          (CCA_CLK_DIV_BASE - CCA_GPIO_INPUT_BASE)
#define IPROC_GPIO_CCA_DEBUG            (CCA_GPIODBG_SEL_BASE - CCA_GPIO_INPUT_BASE)

#if (defined(CONFIG_NS_PLUS))
#define NORTHSTAR_DMU_BASE 0x1803f000
#else
#define NORTHSTAR_DMU_BASE 0x1800c000
#endif
#define IPROC_GPIO_CCA_PULL_UP          (0x01dc)
#define IPROC_GPIO_CCA_PULL_DOWN        (0x01e0)
#define IPROC_GPIO_CCA_CTRL0            (0x01c0)

#define IPROC_GPIO_CCA_HYSTERESIS       (0x01d4)
#define IPROC_GPIO_CCA_SLEWRATE         (0x01d8)

#define IPROC_GPIO_CCA_DS2              (0x01C4)
#define IPROC_GPIO_CCA_DS1              (0x01C8)
#define IPROC_GPIO_CCA_DS0              (0x01CC)

#define IPROC_GPIO_CCA_INPUT_DISABLE    (0x01D0)


#if 0
/* Chipcommon B GPIO */
#define IPROC_GPIO_CCB_BASE 0x18001000

#define IPROC_GPIO_CCB_DIN         (CCB_GP_DATA_IN_BASE)
#define IPROC_GPIO_CCB_DOUT        (CCB_GP_DATA_OUT_BASE)
#define IPROC_GPIO_CCB_EN          (CCB_GP_OUT_EN_BASE)
#define IPROC_GPIO_CCB_INT_TYPE    (CCB_GP_INT_TYPE_BASE)
#define IPROC_GPIO_CCB_INT_DE      (CCB_GP_INT_DE_BASE)
#define IPROC_GPIO_CCB_INT_EDGE    (CCB_GP_INT_EDGE_BASE)
#define IPROC_GPIO_CCB_INT_MASK    (CCB_GP_INT_MSK_BASE)
#define IPROC_GPIO_CCB_INT_STAT    (CCB_GP_INT_STAT_BASE)
#define IPROC_GPIO_CCB_INT_MSTAT   (CCB_GP_INT_MSTAT_BASE)
#define IPROC_GPIO_CCB_INT_CLR     (CCB_GP_INT_CLR_BASE)
#define IPROC_GPIO_CCB_AUX_SEL     (CCB_GP_AUX_SEL_BASE)
#define IPROC_GPIO_CCB_INIT_VAL    (CCB_GP_INIT_VAL_BASE)
#define IPROC_GPIO_CCB_PAD_RES     (CCB_GP_PAD_RES_BASE)
#define IPROC_GPIO_CCB_RES_EN      (CCB_GP_RES_EN_BASE)
#define IPROC_GPIO_CCB_TST_IN      (CCB_GP_TEST_INPUT_BASE)
#define IPROC_GPIO_CCB_TST_OUT     (CCB_GP_TEST_OUTPUT_BASE)
#define IPROC_GPIO_CCB_TST_EN      (CCB_GP_TEST_ENABLE_BASE)
#define IPROC_GPIO_CCB_PRB_EN      (CCB_GP_PRB_ENABLE_BASE)
#define IPROC_GPIO_CCB_PRB_OE      (CCB_GP_PRB_OE_BASE)
#endif


#define REGOFFSET_GPIO_DIN          0x000 /* GPIO Data in register */
#define REGOFFSET_GPIO_DOUT         0x004 /* GPIO Data out register */
#define REGOFFSET_GPIO_EN           0x008 /* GPIO driver enable register */


#define IPROC_GPIO_CCA_ID   (0)
#define IPROC_GPIO_CCB_ID   (1)

#define IPROC_NR_IRQS           (256)
#define IPROC_IRQ_GPIO_0        (IPROC_NR_IRQS)

//copied from C:\iProc\sw\iproc\kernel\linux\arch\arm\plat-iproc\include\mach\irqs.h

//swang ??? documentation is lacking for the GPIO interrupt mapping in GIC
#define IPROC_GPIO_CCA_IRQ_BASE     (IPROC_IRQ_GPIO_0)
#define IPROC_GPIO_CCB_IRQ_BASE     (IPROC_IRQ_GPIO_0 + 32)

#define IPROC_GPIO_CCA_INT          (117)
#define IPROC_CCA_INT_F_GPIOINT     (1)

//#define IPROC_GPIO_CCB_INT          (119)

typedef struct iprocGPIO {
    int id;
    //struct gpio_chip	chip;
    //struct iproc_gpio_cfg	*config;
    void *ioaddr;
    void *intr_ioaddr;
    void *dmu_ioaddr;
    //spinlock_t		 lock;
    int irq_base;
	//struct resource	* resource;
}iproc_gpio_chip;

typedef enum irqTypes
{
	IRQ_TYPE_EDGE_RISING = 1,
	IRQ_TYPE_EDGE_FALLING =2,
	IRQ_TYPE_LEVEL_LOW =4,
	IRQ_TYPE_LEVEL_HIGH =8,
	IRQ_TYPE_EDGE_BOTH = 0x3,
}IRQTYPE;


int iproc_gpio_irq_handler_cca(int irq);

void iprocGPIOInit(void);
iproc_gpio_chip *getGPIODev ( void );
void setGPIOasInput( int pin );
void setGPIOasOutput( int pin, int value );
int getGPIOpinValue ( int pin );
void enableGPIOInterrupt(int pin, IRQTYPE irqtype );
void disableGPIOInterrupt(int pin, IRQTYPE irqtype );
void initGPIOState( void );
void registerGPIOEdgeInterrupt ( void );
void registerGPIOLevelInterrupt ( void );


#endif
