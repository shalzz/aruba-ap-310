/* Driver for LCD Module
 *
 * Copyright 2006 ASUSTek
*/

#include <common.h>
#include <config.h>
#if CONFIG_OCTEON_NAC38

#ifdef CFG_LCD_MODULE_SEL

#include "octeon_boot.h"
#include "octeon_hal.h"
#include <command.h>
#include <asm/processor.h>
#include <pci.h>

#ifndef __PPC__
#include <asm/io.h>
#ifdef __MIPS__
/* Macros depend on this variable */
static unsigned long mips_io_port_base = 0;
#endif
#endif

#define CFG_LCD_INIT_DELAY 	10000
#define CFG_LCD_DISPLAY_DELAY	100

#define CLR_LCD_RS	0x01
#define SET_LCD_RS	0x00
#define CLR_LCD_RW	0x10
#define SET_LCD_RW	0x00

#define LCDM_KEY_LEFT	0x80
#define LCDM_KEY_RIGHT  0x02
#define LCDM_KEY_UP	0x01
#define LCDM_KEY_DOWN	0x10
#define LCDM_KEY_MID	0x04


/*
LCD module "chip select" is  BOOT_CE#7
A0 : 0 =  DATA REGISTER, 1 =  INSTRUCTION REGISTER
A4 : 0 = read, 1 = write
other  address : don't care 

ex : 	address 0x00 : read  DATA REGISTER
	address 0x01 : read  INSTRUCTION REGISTER
	address 0x10 : write  DATA REGISTER
	address 0x11 : write  INSTRUCTION REGISTER

============= transfer to boot bus  =====================
// time unit : ns  , buffer 10 ns,  assume Tr,Tf  << 1ns
		2.7~4.5V	4.5~5V	
   Tadr		230		110
   Tce		70		50
   Toe		460		230
   Trh	 	30		20
   Twe		460		230
   Twh	 	30		20
   Tpause	230		110

============= transfer to boot bus  =====================
   Tadr	= (Tc - max( Twe, Toe) - max( Trh, Twh) - Tce)/2
   Tce	= Tasu
   Toe	= max( Tdrsh, Tw)
   Trh	= max(Tahd,Tdrhd)
   Twe	= max( Tdwsh, Tw)
   Twh	= max(Tahd,Tdwhd)
   Tpause= (Tc - max( Twe, Toe) - max( Trh, Twh) - Tce)/2

============= SAMSUNG S6A0069  =====================
// LCD spec.			2.7~4.5V	4.5~5V	
// Tc : E cycle 			>1000		>500
// Tr,Tf :   E rise/fall time		<25		< 20
// Tw : E pulse width		>450		>230
// Tasu : R/W RS setup time	>60		>40
// Tahd : R/W RS hold time		>20		>10
// Tdrsh : DATA read delay time	<360		<120
// Tdrhd : DATA read hold time	>5		>5
// Tdwsh : DATA write setup time	>195		>80
// Tdwhd : DATA write hold time	>10		>10



*/

void InitLCD(void)
{
   int command;

   command = CLR_LCD_RS | CLR_LCD_RW;

   outb(0x3a, CFG_LCD_MODULE_BASE_ADDR + command);	// Function Set, DL=1 8bit, N=1 2 line, F=1 5*8 dots
   udelay(CFG_LCD_INIT_DELAY);

   outb(0x0c, CFG_LCD_MODULE_BASE_ADDR + command);	// Display On, Display, Cursor, blinking off
   udelay(CFG_LCD_INIT_DELAY);

   outb(0x06, CFG_LCD_MODULE_BASE_ADDR + command);	// Entry Mode
   udelay(CFG_LCD_INIT_DELAY);

   outb(0x01, CFG_LCD_MODULE_BASE_ADDR + command);	// Clear Display
   udelay(CFG_LCD_INIT_DELAY);

   outb(0x80, CFG_LCD_MODULE_BASE_ADDR + command);	// Set DDRAM Address 0
   udelay(CFG_LCD_INIT_DELAY);
	
}


int ReadLCDbyte(unsigned char address, unsigned char *data)
{
   int command;
   unsigned char DataValue;
   
   
   if ( address < 6 )
      DataValue = 0x80 + address;
   else if ( address < 24 )
      DataValue = 0xC0 + address - 16; 
   else
   {
      // fail
      return (1);
   }      

   // Set DDRAM Address
   command = CLR_LCD_RS | CLR_LCD_RW;
   outb(DataValue, CFG_LCD_MODULE_BASE_ADDR + command);
   udelay(CFG_LCD_DISPLAY_DELAY);   
       			
   // Read Data from RAM
   command = SET_LCD_RS | SET_LCD_RW;
   *data = inb(CFG_LCD_MODULE_BASE_ADDR + command);
   udelay(CFG_LCD_DISPLAY_DELAY); 
   	    
   return 0;  
}


int WriteLCDbyte(unsigned char address, unsigned char data)
{
   int command;
   unsigned char DataValue;
   
   if ( address < 16 )
      DataValue = 0x80 + address;
   else if ( address < 32 )
      DataValue = 0xC0 + address - 16; 
   else
   {
      // fail
      return (100);
   }

   // Set DDRAM Address
   command = CLR_LCD_RS | CLR_LCD_RW;
   outb(DataValue, CFG_LCD_MODULE_BASE_ADDR + command);
   udelay(CFG_LCD_DISPLAY_DELAY);  
   
   // Write Data to RAM
   command = SET_LCD_RS | CLR_LCD_RW;
   outb(data, CFG_LCD_MODULE_BASE_ADDR + command);
   udelay(CFG_LCD_DISPLAY_DELAY);   
   
}



//-------------------------------------------------------------------------
// LcdReadDDRAM
//-------------------------------------------------------------------------
static int LcdReadDDRAM(void)
{
   int command, i;
   unsigned char DataValue;
         			
   // Set DDRAM Address
   command = CLR_LCD_RS | CLR_LCD_RW;
   DataValue  = 0x80;
   outb(DataValue, CFG_LCD_MODULE_BASE_ADDR + command);
   udelay(CFG_LCD_DISPLAY_DELAY); 

   // Read DDRAM Address   
   printf("\n    ");
   for(i=0; i<16; i++)
      printf("%02X ", i);           
   printf("\n    -----------------------------------------------\n");
  
   for(i=0; i<48; i++)
   {  
      if ( i<24 ) 	
         DataValue = 0x80+i; 
      else
         DataValue = 0xC0+i-24;

      // Set DDRAM Address
      command = CLR_LCD_RS | CLR_LCD_RW;   
      outb(DataValue, CFG_LCD_MODULE_BASE_ADDR + command);
      udelay(CFG_LCD_DISPLAY_DELAY);

      // Read Data from RAM
      command = SET_LCD_RS | SET_LCD_RW;
      DataValue = inb(CFG_LCD_MODULE_BASE_ADDR + command);           
      
      if ( i == 0 )
         printf("00: ");
      else if ( i == 16 )
         printf("10: ");
      else if ( i == 24 )
         printf("40: ");
      else if ( i == 40 )
         printf("50: ");
         
      if ( i == 7 || i == 31 )
         printf("%02X-", DataValue);
      else
         printf("%02X ", DataValue);
         
      if ( i==15 || i==23 || i==39 || i==47)
         printf("\n");
      
   }   	    
   
   return(0);
}


//-------------------------------------------------------------------------
// do_lcdm_init
//-------------------------------------------------------------------------
static int do_lcdm_init(cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
   unsigned char test_buf1[32]= "ASUS LCDM Test 1LCDM GPIO xxxxx.";
   unsigned char test_buf2[32]="ASUS LCDM Test 2LCDM Exit xxxxx.";
   int i, command;
   unsigned long gpio_state;
   InitLCD();
  
   printf("Write LCD Module String:\n");
   for(i=0; i<32; i++)
      WriteLCDbyte(i, test_buf1[i]);

   //octeon_write64(CVMX_GPIO_BIT_CFG5, 0x00);	// Set GPIO 5 to input pin
   gpio_state = octeon_read64(CVMX_GPIO_RX_DAT);


   printf("Press Medium Key to exit\n");
   // Display GPIO state and wait medium key
   while(1)
   {
      gpio_state = octeon_read64(CVMX_GPIO_RX_DAT);

      if ( (gpio_state & LCDM_KEY_MID ) )
         break;

      for(i=26; i<26+5; i++)
      {
         if ( gpio_state & 0x01 )
            WriteLCDbyte(i, '1');
         else
            WriteLCDbyte(i, '0'); 
         gpio_state = gpio_state >> 1;
      }

      udelay(100*1000);		//delay 100ms
   }

   // Display string
   for(i=0; i<32; i++)
      WriteLCDbyte(i, test_buf2[i]);

   // Display GPIO state
   for(i=26; i<26+5; i++)
   {
      if ( gpio_state & 0x01 )
         WriteLCDbyte(i, '1');
      else
         WriteLCDbyte(i, '0'); 
      gpio_state = gpio_state >> 1;
      udelay(100);
   }

   return 0;
}

U_BOOT_CMD(
	lcdm_init, 5, 1, do_lcdm_init,
	"find the LCD Module a\n",
	"LCD Module test\n"
);

#endif
#endif
