/*******************************************************************************
Copyright (C) Marvell International Ltd. and its affiliates

********************************************************************************
Marvell GPL License Option

If you received this File from Marvell, you may opt to use, redistribute and/or 
modify this File in accordance with the terms and conditions of the General 
Public License Version 2, June 1991 (the "GPL License"), a copy of which is 
available along with the File in the license.txt file or by writing to the Free 
Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 or 
on the worldwide web at http://www.gnu.org/licenses/gpl.txt. 

THE FILE IS DISTRIBUTED AS-IS, WITHOUT WARRANTY OF ANY KIND, AND THE IMPLIED 
WARRANTIES OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE ARE EXPRESSLY 
DISCLAIMED.  The GPL License provides additional details about this warranty 
disclaimer.

*******************************************************************************/

/*
 * serial.c - serial support.
 */

#include <common.h>
#include <command.h>
#include "uart/mvUart.h"
#include "cpu/mvCpu.h"

#if defined(MV78XX0)
extern unsigned int whoAmI(void);
#else
    #define whoAmI()    0
#endif

extern void print_mvBanner(void);

int serial_init (void)
{
	DECLARE_GLOBAL_DATA_PTR;
    
    int clock_divisor = (CONFIG_SYS_TCLK / 16)/gd->baudrate;

	if (whoAmI() == MASTER_CPU)
    {	
#ifdef CONFIG_SYS_INIT_CHAN1
	mvUartInit(0, clock_divisor, mvUartBase(0));
#endif
    }
    
#if defined(MV78XX0)
    if ((whoAmI() == MASTER_CPU) || (whoAmI() == SLAVE_CPU))
#endif
    {
#ifdef CONFIG_SYS_INIT_CHAN2
	mvUartInit(1, clock_divisor, mvUartBase(1));
#endif
    }
        /* print banner */
        print_mvBanner();

	return (0);
}

void
serial_putc(const char c)
{
#if defined(CONFIG_MV_SMP) || (defined(MV78XX0) && defined(MV78200))
        if (c == '\n')
                mvUartPutc((whoAmI())%2, '\r');

        mvUartPutc((whoAmI())%2, c);
#else
	if (c == '\n')
		mvUartPutc(CONFIG_SYS_DUART_CHAN, '\r');

	mvUartPutc(CONFIG_SYS_DUART_CHAN, c);
#endif
}

int
serial_getc(void)
{	
#if defined(CONFIG_MV_SMP) || (defined(MV78XX0) && defined(MV78200))
        return mvUartGetc((whoAmI())%2);
#else
	return mvUartGetc(CONFIG_SYS_DUART_CHAN);
#endif
}

int
serial_tstc(void)
{
#if defined(CONFIG_MV_SMP) || (defined(MV78XX0) && defined(MV78200))
        return mvUartTstc((whoAmI())%2);
#else
	return mvUartTstc(CONFIG_SYS_DUART_CHAN);
#endif
}

void
serial_setbrg (void)
{
	DECLARE_GLOBAL_DATA_PTR;
	
	int clock_divisor = (CONFIG_SYS_TCLK / 16)/gd->baudrate;

#ifdef CONFIG_SYS_INIT_CHAN1
	mvUartInit(0, clock_divisor, mvUartBase(0));
#endif
#ifdef CONFIG_SYS_INIT_CHAN2
	mvUartInit(1, clock_divisor, mvUartBase(1));
#endif
}


void
serial_puts (const char *s)
{
	while (*s) {
		serial_putc (*s++);
	}
}

#if defined(CONFIG_CMD_KGDB)
void
kgdb_serial_init(void)
{
}

void
putDebugChar (int c)
{
	serial_putc (c);
}

void
putDebugStr (const char *str)
{
	serial_puts (str);
}

int
getDebugChar (void)
{
	return serial_getc();
}

void
kgdb_interruptible (int yes)
{
	return;
}
#endif	/* CFG_CMD_KGDB	*/
