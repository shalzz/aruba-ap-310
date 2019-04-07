/*
 * Copyright (c) 2011-2012, Code Aurora Forum. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *  * Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *  * Neither the name of Google, Inc. nor the names of its contributors
 *    may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */
#include <common.h>
#include <command.h>
#include <asm/arch-ipq806x/iomap.h>
#include <asm/arch-ipq806x/gpio.h>
#include <asm/io.h>

/*******************************************************
Function description: configure GPIO functinality
Arguments :
unsigned int gpio - Gpio number
unsigned int func - Functionality number
unsigned int dir  - direction 0- i/p, 1- o/p
unsigned int pull - pull up/down, no pull range(0-3)
unsigned int drvstr - range (0 - 7)-> (2- 16)MA steps of 2
unsigned int enable - 1 - Disable, 2- Enable.

Return : None
*******************************************************/


void gpio_tlmm_config(unsigned int gpio, unsigned int func,
                      unsigned int dir, unsigned int pull,
                      unsigned int drvstr, unsigned int enable)
{
        unsigned int val = 0;
        val |= pull;
        val |= func << 2;
        val |= drvstr << 6;
        val |= enable << 9;
        unsigned int *addr = (unsigned int *)GPIO_CONFIG_ADDR(gpio);
        writel(val, addr);
        return;
}

void
gpio_set_as_input(unsigned pin)
{
    gpio_tlmm_config(pin, 0, GPIO_INPUT, GPIO_PULL_UP, GPIO_10MA, GPIO_ENABLE);
}

void
gpio_set_as_output(unsigned pin)
{
    gpio_tlmm_config(pin, 0, GPIO_OUTPUT, GPIO_PULL_UP, GPIO_10MA, GPIO_DISABLE);
}

void
gpio_out(unsigned pin, unsigned value)
{
    unsigned int val = (value << 1);
    unsigned int maddr = GPIO_IN_OUT_ADDR(pin);
    writel(val, maddr);
}

unsigned
gpio_value(unsigned pin)
{
    unsigned val;
    unsigned int maddr = GPIO_IN_OUT_ADDR(pin);

    val = readl(maddr);
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

