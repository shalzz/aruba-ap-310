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

#include <common.h>
#ifdef CONFIG_IPROC
#include <command.h>
#include <configs/iproc_board.h>
#include <asm/arch/iproc.h>
#include <asm/arch/reg_utils.h>
#include "iproc_gpio.h"

#define readl(addr) (*(volatile unsigned int*)(addr))
#define writew(b,addr) ((*(volatile unsigned int *) (addr)) = (b))

#ifndef CONFIG_APBOOT
extern int countLevelHigh;
extern int countLevelLow;
extern int countRisingEdge;
extern int countFalliingEdge;
#endif

volatile int IntReceived = 0;

static iproc_gpio_chip gpioDev;
static int currentIRQType;

static unsigned int _iproc_gpio_readl(iproc_gpio_chip *chip, int reg)
{
    return readl(chip->ioaddr + reg);
}

static void _iproc_gpio_writel(iproc_gpio_chip *chip, unsigned int val, int reg)
{
    writew(val, chip->ioaddr + reg);
}


static int iproc_gpio_to_irq(iproc_gpio_chip *chip, unsigned int pin) {
    return (chip->irq_base + pin);
}

static int iproc_irq_to_gpio(iproc_gpio_chip *chip, unsigned int irq) {
    return (irq - chip->irq_base);
}


static void iproc_gpio_irq_unmask(unsigned int irq, IRQTYPE irq_type)
{
    iproc_gpio_chip *ourchip = &gpioDev;
    int pin;
    unsigned int int_mask;

    pin = iproc_irq_to_gpio(ourchip, irq);
    //irq_type = IRQ_TYPE_EDGE_BOTH;//for now swang //irq_desc[irq].status & IRQ_TYPE_SENSE_MASK;

    if (ourchip->id == IPROC_GPIO_CCA_ID) {
        unsigned int  event_mask; 

        event_mask = _iproc_gpio_readl(ourchip, IPROC_GPIO_CCA_INT_EVENT_MASK);
        int_mask = _iproc_gpio_readl(ourchip, IPROC_GPIO_CCA_INT_LEVEL_MASK);

        if (irq_type & IRQ_TYPE_EDGE_BOTH) {
            event_mask |= 1 << pin;
            _iproc_gpio_writel(ourchip, event_mask, 
                IPROC_GPIO_CCA_INT_EVENT_MASK);
        } else {
            int_mask |= 1 << pin;
            _iproc_gpio_writel(ourchip, int_mask, 
                IPROC_GPIO_CCA_INT_LEVEL_MASK);
        }
    }

}
static void iproc_gpio_irq_mask(unsigned int irq, IRQTYPE irq_type)
{
    iproc_gpio_chip *ourchip = &gpioDev;
    int pin;
    unsigned int int_mask;


    pin = iproc_irq_to_gpio(ourchip, irq);
    //irq_type = IRQ_TYPE_EDGE_BOTH; //for now swang ???//irq_desc[irq].status & IRQ_TYPE_SENSE_MASK;

    if (ourchip->id == IPROC_GPIO_CCA_ID) {
        unsigned int  event_mask;
        
        event_mask = _iproc_gpio_readl(ourchip, IPROC_GPIO_CCA_INT_EVENT_MASK);
        int_mask = _iproc_gpio_readl(ourchip, IPROC_GPIO_CCA_INT_LEVEL_MASK);

        if (irq_type & IRQ_TYPE_EDGE_BOTH) {
            event_mask &= ~(1 << pin);
            _iproc_gpio_writel(ourchip, event_mask,
                IPROC_GPIO_CCA_INT_EVENT_MASK);
        } else {
            int_mask &= ~(1 << pin);
            _iproc_gpio_writel(ourchip, int_mask, 
                IPROC_GPIO_CCA_INT_LEVEL_MASK);
        }
    }
}

static int iproc_gpio_irq_set_type(unsigned int irq, IRQTYPE type)
{
    iproc_gpio_chip *ourchip = &gpioDev;
    int pin;    

    currentIRQType = type;

    pin = iproc_irq_to_gpio(ourchip, irq);

    if (ourchip->id == IPROC_GPIO_CCA_ID) {
        unsigned int  event_pol, int_pol;

        switch (type) {
        case IRQ_TYPE_EDGE_RISING:
            event_pol = _iproc_gpio_readl(ourchip, IPROC_GPIO_CCA_INT_EDGE);
            event_pol &= ~(1 << pin);
            _iproc_gpio_writel(ourchip, event_pol, IPROC_GPIO_CCA_INT_EDGE);
            break;
        case IRQ_TYPE_EDGE_FALLING:
            event_pol = _iproc_gpio_readl(ourchip, IPROC_GPIO_CCA_INT_EDGE);
            event_pol |= (1 << pin);
            _iproc_gpio_writel(ourchip, event_pol, IPROC_GPIO_CCA_INT_EDGE);
            break;
        case IRQ_TYPE_LEVEL_HIGH:
            int_pol = _iproc_gpio_readl(ourchip, IPROC_GPIO_CCA_INT_LEVEL);
            int_pol &= ~(1 << pin);
            _iproc_gpio_writel(ourchip, int_pol, IPROC_GPIO_CCA_INT_LEVEL);
            break;
        case IRQ_TYPE_LEVEL_LOW:
            int_pol = _iproc_gpio_readl(ourchip,IPROC_GPIO_CCA_INT_LEVEL);
            int_pol |= (1 << pin);
            _iproc_gpio_writel(ourchip, int_pol, IPROC_GPIO_CCA_INT_LEVEL);
            break;
        default:
            //printf(KERN_ERR "unsupport irq type !\n");
            return -1;
        }
    }
    return 0;
}

#ifndef CONFIG_APBOOT
void  iproc_gpio_irq_handler(void *data)

{
    unsigned int  val;
    unsigned int  int_mask, event_pol, int_pol, in;
    unsigned int  event_mask, event,tmp = 0;
    int iter;
    iproc_gpio_chip *ourchip = &gpioDev;


    val = readl(ourchip->intr_ioaddr + CCA_INT_STS_BASE);
    
    if (val & IPROC_CCA_INT_F_GPIOINT)
    {

        //Disable GPIO interrupt at top level
        tmp = readl(gpioDev.intr_ioaddr + CCA_INT_MASK_BASE);
        writew ( (tmp & ~IPROC_CCA_INT_F_GPIOINT),gpioDev.intr_ioaddr + CCA_INT_MASK_BASE);

        int_mask = _iproc_gpio_readl(ourchip, IPROC_GPIO_CCA_INT_LEVEL_MASK);
        int_pol = _iproc_gpio_readl(ourchip, IPROC_GPIO_CCA_INT_LEVEL);
        in = _iproc_gpio_readl(ourchip, IPROC_GPIO_CCA_DIN);
        event_mask = _iproc_gpio_readl(ourchip, IPROC_GPIO_CCA_INT_EVENT_MASK);
        event = _iproc_gpio_readl(ourchip, IPROC_GPIO_CCA_INT_EVENT);
        event_pol = _iproc_gpio_readl(ourchip, IPROC_GPIO_CCA_INT_EDGE);

        //24 is the number of GPIOs
        for (iter = 0; iter < 24; iter ++)
        {
           if ( (IRQ_TYPE_EDGE_FALLING == currentIRQType) ||
                (IRQ_TYPE_EDGE_RISING == currentIRQType))
           {
            tmp = event_mask;
            tmp &= event;
            // If there is an event interrupt ( edge triggered )
            if ( tmp & ( 1 << iter ))
            {
                if ((~event_pol)&(1 << iter))
                {
                    countRisingEdge |= 1 << iter;
                    puts("\nRising edge interrupt");
                }
                else
                {
                    countFalliingEdge |= 1 << iter;
                    puts("\nFalling edge interrupt");
                }
                IntReceived = 1;
                //Clear the edge interrupt
                *(volatile unsigned long*)(CCA_GPIO_EVT) |=(1<<iter);
                *(volatile unsigned long*)(CCA_GPIO_EVTINT_MASK) &=~(1<<iter);
            }
           }
           else
           {
               tmp = in ^ int_pol;
               tmp &= int_mask;

               if ( tmp & (1 << iter))
               {
                   if ( int_pol & ( 1 << iter ))
                   {
                       countLevelLow |= 1 << iter;
                       puts("\nLevel low interrupt");

                   }
                   else
                   {
                       countLevelHigh |= 1 << iter;
                       puts("\nLevel high interrupt");

                   }
                   *(volatile unsigned long*)(CCA_GPIOINT_MASK) &=~(1<<iter);
                   IntReceived = 1;
               }
           }

        }
        //Enable GPIO interrupt at top level
        tmp = readl(gpioDev.intr_ioaddr + CCA_INT_MASK_BASE);
        writew ( (tmp |IPROC_CCA_INT_F_GPIOINT),gpioDev.intr_ioaddr + CCA_INT_MASK_BASE);

    }

    return ;
}
#endif

//set a GPIO pin as an input
static int iproc_gpiolib_input(iproc_gpio_chip *ourchip, unsigned gpio)
{
    unsigned int  val;
    unsigned int  nBitMask = 1 << gpio;


    //iproc_gpio_lock(ourchip, flags);

    val = _iproc_gpio_readl(ourchip, REGOFFSET_GPIO_EN);
    val &= ~nBitMask;
    _iproc_gpio_writel(ourchip, val, REGOFFSET_GPIO_EN);

    //iproc_gpio_unlock(ourchip, flags);
    return 0;
}

//set a gpio pin as a output
static int iproc_gpiolib_output(iproc_gpio_chip *ourchip,
                  unsigned gpio, int value)
{


    unsigned long val;
    unsigned int  nBitMask = 1 << gpio;

    //iproc_gpio_lock(ourchip, flags);

    val = _iproc_gpio_readl(ourchip, REGOFFSET_GPIO_EN);
    val |= nBitMask;
    _iproc_gpio_writel(ourchip, val, REGOFFSET_GPIO_EN);

    //iproc_gpio_unlock(ourchip, flags);
    return 0;
}

//set the gpio pin ( by gpio ) to value ( only for output, input is read only
static void iproc_gpiolib_set(iproc_gpio_chip *ourchip,
                unsigned gpio, int value)
{


    unsigned long val;
    unsigned int  nBitMask = 1 << gpio;

    //iproc_gpio_lock(ourchip, flags);


    /* determine the GPIO pin direction 
     */ 
    val = _iproc_gpio_readl(ourchip, REGOFFSET_GPIO_EN);
    val &= nBitMask;

    /* this function only applies to output pin
     */ 
    if (!val)
        return;

    val = _iproc_gpio_readl(ourchip, REGOFFSET_GPIO_DOUT);

    if ( value == 0 ){
        /* Set the pin to zero */
        val &= ~nBitMask;
    }else{
        /* Set the pin to 1 */
        val |= nBitMask;
    }    
    _iproc_gpio_writel(ourchip, val, REGOFFSET_GPIO_DOUT);

    //iproc_gpio_unlock(ourchip, flags);

}

//get the gpio pin value
static int iproc_gpiolib_get(iproc_gpio_chip *ourchip, unsigned gpio)
{


    unsigned int val, offset;
    unsigned int  nBitMask = 1 << gpio;    

    //iproc_gpio_lock(ourchip, flags);
    /* determine the GPIO pin direction 
     */ 
    offset = _iproc_gpio_readl(ourchip, REGOFFSET_GPIO_EN);
    offset &= nBitMask;

    //output pin
    if (offset){
        val = _iproc_gpio_readl(ourchip, REGOFFSET_GPIO_DOUT);
    }
    //input pin
    else {
        val = _iproc_gpio_readl(ourchip, REGOFFSET_GPIO_DIN);    
    }
    val >>= gpio;
    val &= 1;
    //printf("\nGPIO %d, input value reg = 0x%x,,value= %d ,offset %d", gpio, *((unsigned long*)0x18000060),val, offset);
    //iproc_gpio_unlock(ourchip, flags);

    return val;
}
#if 0
static int iproc_gpiolib_to_irq(iproc_gpio_chip *ourchip,
                unsigned offset)
{
    return iproc_gpio_to_irq(ourchip, offset);
}
#endif

void iprocGPIOInit( void )
{
    gpioDev.id = IPROC_GPIO_CCA_ID;
    gpioDev.ioaddr = (void*)IPROC_GPIO_CCA_BASE;
    gpioDev.dmu_ioaddr = (void*)NORTHSTAR_DMU_BASE;
    gpioDev.intr_ioaddr =(void*)CCA_CHIPID ;
    gpioDev.irq_base = IPROC_GPIO_CCA_IRQ_BASE;
}

iproc_gpio_chip *getGPIODev ( void )
{
    return &gpioDev;
}

void setGPIOasInput( int pin )
{
    if ( (pin < 24 ) && ( pin >= 0 ))
    {
        iproc_gpiolib_input(&gpioDev, pin );
    }
}
void setGPIOasOutput( int pin, int value )
{
    if ( (pin < 24 ) && ( pin >= 0 ))
    {
        //Set the pin to be output
        iproc_gpiolib_output(&gpioDev,pin,value);
        //set value
        iproc_gpiolib_set(&gpioDev,pin,value);
    }
}
int getGPIOpinValue ( int pin )
{
    if ( (pin < 24 ) && ( pin >= 0 ))
    {
        return iproc_gpiolib_get(&gpioDev, pin );
    }
    return -1;
}
void enableGPIOInterrupt(int pin, IRQTYPE irqtype )
{
    unsigned int irq;
    unsigned long temp;



#if 0
    volatile unsigned long tempDelay=0;
    if ( *(volatile unsigned long*)0x18000020 & 0x1 )
    {
        post_log("\nInterrupt still on!!!");
    }
    else
    {
        post_log("\nInterrupt not on!!!");
    }
#endif
    if ( (pin < 24 ) && ( pin >= 0 ))
    {
        //Map pin to IRQ number
        irq = iproc_gpio_to_irq(&gpioDev,pin);
        iproc_gpio_irq_set_type(irq,irqtype);
        iproc_gpio_irq_unmask(irq, irqtype);

        //while ( tempDelay < 0xFFFF )
        //{
        //    tempDelay++;
        //}
        //post_log("\n Enable Interrupt");
        //Enable GPIO interrupt at top level
        temp = readl(gpioDev.intr_ioaddr + CCA_INT_MASK_BASE);
        writew ( (temp |IPROC_CCA_INT_F_GPIOINT),gpioDev.intr_ioaddr + CCA_INT_MASK_BASE);
    }

}
void disableGPIOInterrupt(int pin, IRQTYPE irqtype )
{
    unsigned int irq;
    unsigned long temp;

    if ( (pin < 24 ) && ( pin >= 0 ))
    {
        //Map pin to IRQ number
        irq = iproc_gpio_to_irq(&gpioDev,pin);
        //iproc_gpio_irq_set_type(irq,irqtype);
        iproc_gpio_irq_mask(irq,irqtype);

        //Disable GPIO interrupt at top level
        temp = readl(gpioDev.intr_ioaddr + CCA_INT_MASK_BASE);
        writew ( (temp & ~IPROC_CCA_INT_F_GPIOINT),gpioDev.intr_ioaddr + CCA_INT_MASK_BASE);
    }

}
void initGPIOState ( void )
{
    _iproc_gpio_writel(&gpioDev, 0xFFFFFF, IPROC_GPIO_CCA_INT_EVENT);

    _iproc_gpio_writel(&gpioDev, 0x000000, IPROC_GPIO_CCA_INT_LEVEL);

    _iproc_gpio_writel(&gpioDev, 0x000000, IPROC_GPIO_CCA_INT_LEVEL_MASK);

    _iproc_gpio_writel(&gpioDev, 0x000000, IPROC_GPIO_CCA_INT_EVENT_MASK);

    _iproc_gpio_writel(&gpioDev, 0x000000, IPROC_GPIO_CCA_INT_EDGE);

}

#ifndef CONFIG_APBOOT
void registerGPIOEdgeInterrupt ( void )
{
    disable_interrupts();
    gic_disable_interrupt(117);
    irq_install_handler(117,iproc_gpio_irq_handler,NULL );
    gic_config_interrupt(117, 1, IPROC_INTR_EDGE_TRIGGERED, 0, IPROC_GIC_DIST_IPTR_CPU0);
    enable_interrupts();
}
void registerGPIOLevelInterrupt ( void )
{
    disable_interrupts();
    gic_disable_interrupt(117);
    irq_install_handler(117,iproc_gpio_irq_handler,NULL );
    gic_config_interrupt(117, 1, IPROC_INTR_LEVEL_SENSITIVE, 0, IPROC_GIC_DIST_IPTR_CPU0);
    enable_interrupts();
}
#endif

void
gpio_set_as_input(unsigned pin)
{
    unsigned val;

    val = reg32_read((volatile unsigned *)(REGOFFSET_GPIO_EN + CCA_GPIO_INPUT));
    val &= ~(1 << pin);
    reg32_write((volatile unsigned *)(REGOFFSET_GPIO_EN + CCA_GPIO_INPUT), val);
}

void
gpio_set_as_output(unsigned pin)
{
    unsigned val;

    val = reg32_read((volatile unsigned *)(REGOFFSET_GPIO_EN + CCA_GPIO_INPUT));
    val |= (1 << pin);
    reg32_write((volatile unsigned *)(REGOFFSET_GPIO_EN + CCA_GPIO_INPUT), val);
}

void
gpio_out(unsigned pin, unsigned value)
{
    unsigned val;

    val = reg32_read((volatile unsigned *)(REGOFFSET_GPIO_DOUT + CCA_GPIO_INPUT));

    if (value) {
        val |= (1 << pin);
    } else {
        val &= ~(1 << pin);
    }
    reg32_write((volatile unsigned *)(REGOFFSET_GPIO_DOUT + CCA_GPIO_INPUT), val);
}

unsigned
gpio_value(unsigned pin)
{
    unsigned val;

    val = reg32_read((volatile unsigned *)(REGOFFSET_GPIO_DIN + CCA_GPIO_INPUT));

    val >>= pin;
    val &= 0x1;

    return val;
}

int
do_gpio(cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
    int pin, val;

    if (argc != 3) {
        printf("Usage: \n%s\n", cmdtp->usage);
        return 1;
    }

    pin = simple_strtoul(argv[1], (char **)0, 10);
    val = simple_strtoul(argv[2], (char **)0, 10);

    if (val) {
        val = 1;
    }

    printf("Driving pin %u as output %s\n", pin, val & 1 ? "high" : "low");
    gpio_set_as_output(pin);
    gpio_out(pin, val);
    return 0;
}

U_BOOT_CMD(
    gpio,    3,    1,     do_gpio,
    "gpio    - poke GPIO pin\n",                   
    " Usage: gpio \n"
);

#endif /* CONFIG_IPROC */
