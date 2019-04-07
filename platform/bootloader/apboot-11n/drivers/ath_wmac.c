// vim:set ts=4 sw=4 expandtab:
/*
 * (C) Copyright 2002
 * Wolfgang Denk, DENX Software Engineering, wd@denx.de.
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

#include <common.h>
#include <command.h>
#include <malloc.h>
#include <net.h>
#include <asm/io.h>
#ifdef CONFIG_ATHEROS
#include <atheros.h>
#else
#include <ar7240_soc.h>
#endif

#undef DEBUG

#if (CONFIG_COMMANDS & CFG_CMD_NET) && defined(CONFIG_ATH_WMAC)
#ifdef CONFIG_AR7240
static char *names[] = { 
    "ar9330",
    "ar9331",
    "ar9341",
    "ar9342",
    "ar9344",
    0 
};


static unsigned supported[] = {
    AR9330_REV_1_0,
    AR9331_REV_1_0,
    AR9341_REV_1_0,
    AR9342_REV_1_0,
    AR9344_REV_1_0,
    0
};
#endif
#ifdef CONFIG_ATHEROS
#define AR7240_REV_ID RST_REVISION_ID_ADDRESS
#define ar7240_reg_rd ath_reg_rd
static char *names[] = {
    "qca9550",
    0 
};

static unsigned supported[] = {
    0x1130,
    0
};
#endif

static char *
get_name(unsigned id)
{
    int i;

    for (i = 0; names[i] && supported[i]; i++) {
        if (supported[i] == id) {
            return names[i];
        }
    }
    return "unk";
}

static void (*__callback)(int) = 0;

void
ath_wmac_install_callback(void *f)
{
    __callback = (void (*)(int))f;
}

int
ath_wmac_initialize(int secondary)
{
    int card_number = 0;
    static int first = 1;
    unsigned id;

    if (!secondary) {
        printf("Radio: ");
    } else {
        printf(", ");
        card_number = secondary;
    }

    while (first) {
        /*
         * AR_SREV in Wasp returns 0, so just look a the CPU revision ID, 
         * sigh...
         */
        id = ar7240_reg_rd(AR7240_REV_ID);
        id &= 0xfff0;   /* skip minor revision */

        if (!first) {
            printf(", ");
        }

        printf ("%s#%u", get_name(id), card_number);
        first = 0;

        card_number++;
    }
    if (card_number == 0) {
        // no cards found
        printf("none");
    }
    if (__callback) {
       (*__callback)(card_number - secondary);
    }
    printf("\n");

    return card_number;
}
#endif
