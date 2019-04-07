
// vim:set ts=4 sw=4 expandtab:
#include <config.h>
#ifdef CONFIG_LTC_4266
#include <common.h>
#include <i2c.h>
#include <exports.h>
#include "poe_4266.h"
#include "ltc4266.h"

#define mdelay(a) udelay(a * 1000)

//extern uchar
//i2c_read (uchar dev_addr, unsigned int offset, int alen, uchar * data, int len);
//extern uchar
//i2c_write (uchar dev_addr, unsigned int offset, int alen, uchar * data, int len);
extern void gpio_out(unsigned, unsigned);

//  Variable Declarations and Structure Definitions
char gQuarterOhm;
char gAfOnly;      
struct quadStruct quadArray[gcNumQuads];
struct portStruct portArray[gcNumPorts];

uchar swI2cWrite(uchar chip, uchar address, uchar data) {
    i2c_write(chip, address, 1, &data, 1);
    return 0;
}
	
uchar swI2cRead(uchar chip, uchar address) {
   uchar data;
	
   i2c_read(chip, address, 1, &data, 1);
   return data;
}

///////////////////////////////////////////////////////////////////////////////
//  initRegMap
//
//  Description:
//      Initializes local register map including loading values from flash. 
//      register map (in RAM).
void initRegMap( void ) {
   int i;
    
   // Rebuild global_control byte
   gAfOnly        = 0;
   
   // Load all port priorities
   for (i = 0; i < gcNumPorts; i++) {
      // Mark priority dirty bit to force reorder of linked list 
      portArray[i].msdReq      = TRUE;
      portArray[i].disable     = 0;
      portArray[i].msd         = 0;
      portArray[i].legEn       = 0;
   }
}   

///////////////////////////////////////////////////////////////////////////////
// portInitAll
//
//  Description:
//      Blindly broadcast-initializes all LTC PSEs. Then initializes port and 
//      quad data structures. A simultaneous poll of  device presence is 
//      performed to fill out "present" field. MSD is marked as "dirty" during 
//      the flash load process. Thus, during first main loop, msd will be
//      initialized and repaired. 
//      Note: Many port/quad fields simply contain static values and attempt
//      to precalculate various values at the expense of add'l storage.
void portInitAll( void ) {
   int quad, port;
   uchar currChip;
   
   ////////////////////////////////////////////////////////////////
   // Program (initialize) all devices simultaneously
   // Setup the chip address
   ////////////////////////////////////////////////////////////////

   currChip = cBcastAddr; 

   // Reset the chip and its interrupts
   swI2cWrite(currChip, rstpb,   msk_rstpb_intclr   | msk_rstpb_rstall);
   mdelay(10);

   // Set interrupt mask so that
   // interrupts will toggle _int pin/throw ARA
   swI2cWrite(currChip, intmask, (msk_intmask_tstart | msk_intmask_tcut | 
                                  msk_intmask_class  | msk_intmask_det  | 
                                  msk_intmask_dis    | msk_intmask_pwrgd   ) );

   // Clear (all chips) powerup supply interrupts 
   swI2cRead(currChip, supevn_cor);

   // Set opmode = semi-auto for port 1&2, opmode = shutdown for port 3&4
   swI2cWrite(currChip, opmd, 0x0A);

   // Enable DC disconnect, support hotplug, so remove here
   // swI2cWrite(currChip, disena, (msk_disena_dc4 | msk_disena_dc3 | 
   //                               msk_disena_dc2 | msk_disena_dc1 ) );

   // Enable midspan according to flash configuration
   // if (global_control & msk_mid) {
   //    swI2cWrite(currChip, midspan, (msk_midspan_midsp4 | msk_midspan_midsp3 | 
   //                                   msk_midspan_midsp2 | msk_midspan_midsp1 ) );
   // }                                 

   // Enable detect on change to reduce interrupt overhead
   //swI2cWrite(currChip, mconf, (msk_mconf_intena | msk_mconf_detchg) );
   
   // Enabled high power mode
   swI2cWrite(currChip, hpen, ( msk_hpen_hpen2 | msk_hpen_hpen1 ) );

   // swI2cWrite(currChip, hpen, (msk_hpen_hpen4 | msk_hpen_hpen3 | 
   //                             msk_hpen_hpen2 | msk_hpen_hpen1 ) );
   // Enable ping pong in AT mode, else in AF mode leave it off
   if (!gAfOnly) {
      // AT mode
      swI2cWrite(currChip, hpmd1, msk_hpmd1_pongen);
      swI2cWrite(currChip, hpmd2, msk_hpmd2_pongen);
      // swI2cWrite(currChip, hpmd3, msk_hpmd3_pongen);
      // swI2cWrite(currChip, hpmd4, msk_hpmd4_pongen);
   }
   // TODO POE++

   // Set 0.25ohm/0.50ohm
   gQuarterOhm = 1;
   if (gQuarterOhm ) {
      //port 1
      swI2cWrite(currChip, cut1, 0xE2);
      swI2cWrite(currChip, lim1, 0xC0);
      //port 2
      swI2cWrite(currChip, cut2, 0xE2);
      swI2cWrite(currChip, lim2, 0xC0);
      //port 3
      //swI2cWrite(currChip, cut3, 0xD4);
      //swI2cWrite(currChip, lim3, 0x80);
      //port 4
      //swI2cWrite(currChip, cut4, 0xD4);
      //swI2cWrite(currChip, lim4, 0x80);
   }

   ///////////////////////////////
   // Initialize data structures
   ///////////////////////////////

   for (quad=0; quad<gcNumQuads; quad++) {
      // (0x20-0x2F, every 4th unused)
      int quadAddr;
      quadAddr  = cI2cBase4266;
      quadAddr += ((quad / 3) * 4);
      quadAddr += (quad % 3);
      
      ///////////////////////////////
      // Initialize Quad structure             
      quadArray[quad].i2cAddr  = quadAddr; // quad i2c addr 
                                                                             
      quadArray[quad].pFirstPort = &portArray[quad<<2];
      quadArray[quad].legacyToken = FALSE;
      quadArray[quad].legacyAttempts = 0;

      // Poll i2c to determine if chip is physically present
      if (swI2cRead(quadArray[quad].i2cAddr, id) == 0x64) {
         quadArray[quad].present = 1;
      } else {
         quadArray[quad].present = 0;
      }   
      
      for (port=0; port<gcNumPortsPerQuad; port++) {
         int index = (quad * gcNumPortsPerQuad) + port;
         
         ///////////////////////////////
         // Initialize Port structure             
         portArray[index].i2cAddr    = quadArray[quad].i2cAddr;  
                                                          // port i2c addr (0x20-0x2F, every 4th unused)
         portArray[index].number     = index;             // port number (0 thru 47)
         portArray[index].offset     = port;              // port offset inside quad (0 thru 3)
         portArray[index].lMask      = 0x1  << port;      // one-hot lower nibble mask
         portArray[index].uMask      = 0x10 << port;      // one-hot upper nibble mask
         portArray[index].state      = PORT_OFF;          // port state 
         portArray[index].event      = EVENT_NONE;        // port event latch
         portArray[index].present    = quadArray[quad].present;
                                                          // port present
         portArray[index].coherencyCnt = 0;               // coherency counter starts at zero
         //portArray[index].disable    = 0;               // Disables loaded from flash!!
         //portArray[index].msd        = FALSE;           // MSD loaded from flash!!
         //portArray[index].legen      = FALSE;           // Legacy Enable loaded from flash!!
         portArray[index].cut        = 0;                 // Current CUT programmed in LTC4266
         portArray[index].lim        = 0;                 // Current LIM programmed in LTC4266
         portArray[index].newcut     = 0x22;                 // New CUT
         portArray[index].newlim     = 0xc0;                 // New LIM
         portArray[index].currentport          = 0;       // Port current 
         portArray[index].currentNumLocsFilled = 0;       // Number of window slots filled
         portArray[index].currentIndex         = 0;       // Location of newest current sample
         // currentArray[8];                              // Circular buffer of current samples
         portArray[index].voltage    = 0;                 // Port voltage 
         
         // Clear service flags
         // portArray[index].msdReq      = FALSE;         // Managed by flash initialization
         portArray[index].disableReq  = FALSE;            // Clear port disable req at reset
         portChangeDisable( &portArray[index] );          // Sets/clears detcls enables etc
      }
   }
}

//////////////////////////////////////////////////////////////////////////////
//  portTurnOffRevoke
//
//  Description:
//      Revoke port power by turning off port power and cleaning up 
//      related statistics
//  Arguments:
//      p        - Pointer to port to turn off
//      reEnable - Set to true if port should be reenabled for subsequent
//                 detection and classification
void portTurnOffRevoke( struct portStruct * p, uchar reEnable ) {
   uchar chip = p->i2cAddr;

   swI2cWrite(chip, pwrpb, p->uMask); // Turning off Power PB

   p->state = PORT_OFF;
   p->event = EVENT_REVOKED;
   
   // Update global stats
   p->currentport = 0;
   p->voltage = 0;
   p->cut = 0;
   p->lim = 0;
   p->currentNumLocsFilled = 0;
   p->currentIndex         = 0;
   p->disable = 0;

   if ( reEnable ) { 
      // Reenable Detect and Class (cleared by pushbutton off)
      // for this port only (pushbutton optimizes rd/mod/wr)
      swI2cWrite(chip, detpb, (p->lMask | p->uMask) );
   }   
}

///////////////////////////////////////////////////////////////////////////////
//  portChangeDisable
//
//  Description:
//      Enables detection and classification for a port. Assumes port is 
//    otherwise correctly configured (already).
//  Arguments:
//      p -     Pointer to port to enable
void portChangeDisable( struct portStruct * p ) {
   if (p->disable) {
      // Disable port
      if ( (p->state == PORT_TURNING_ON) || 
           (p->state == PORT_ON        ) || 
           (p->state == PORT_ON_LEGACY ) ) {
         portTurnOffRevoke( p, TRUE);
      } else {
         struct quadStruct * q = portLookupQuad( p );
   
         // Turning off Power PB
         swI2cWrite(p->i2cAddr, 
                    pwrpb,
                    p->uMask );
         
         // If this port has legacy token, give it up
         if ( q->legacyToken == (p->offset + 1) ) {
            portExitLegacyMode( q, p );
         }
      }
      p->state = PORT_DISABLED;
   } else {
      // Re-enabling port
      swI2cWrite(p->i2cAddr, 
                 detpb,
                 (p->uMask | p->lMask) );
      p->state = PORT_OFF;
   }              
}



///////////////////////////////////////////////////////////////////////////////
//  portExitLegacyMode
//
//  Description:
//      Exit legacy mode, restoring AF mode setting and FastIV
//  Arguments:
//      p        - Pointer to port which will exit legacy mode
void portExitLegacyMode( struct quadStruct * q, struct portStruct * p ) {
   uchar chip = p->i2cAddr;
   uchar rdVal;
             
   // Free up token
   q->legacyToken    = 0;
   q->legacyAttempts = 0;
   
   // Pull the port out of legacy mode
   if (gAfOnly) {
      // AF mode
      swI2cWrite(chip, (hpmd1 + (p->offset * 5)), 0x00 );
   } else {
      // AT mode
      swI2cWrite(chip, (hpmd1 + (p->offset * 5)), msk_hpmd1_pongen );
   }
   
   // Turn back on port class
   swI2cWrite(chip, detpb, p->uMask);
   
   // Turn off global FastIV
   rdVal  = swI2cRead(chip, mconf);
   rdVal &= ~msk_mconf_fasti;
   swI2cWrite(chip, mconf, rdVal);
}

///////////////////////////////////////////////////////////////////////////////
//  portLookupQuad
//
//  Description:
//      Determine which quad this port resides in
//  Arguments:
//      p        - Pointer to port to lookup
//  Return Value:
//      Pointer to quad which port resides in
struct quadStruct * portLookupQuad( struct portStruct * p ) { 
   return &quadArray[ (p->number)>>2 ];
}

///////////////////////////////////////////////////////////////////////////////
//  xlateEncodedToLimit
//
//  Description:
//       Translates Limit from register encoded value to a highly encoded
//      description used by the LTC4266. Thus the ungainly 4266 encoding 
//      is further abstracted from the application. However, the encoding
//      is still non-monotonic.
//  Arguments:
//      limEnc - The register map encoded value (0 to 19)
//  Return Value:
//      LTC4266 encoded value to be written to PSE device
uchar xlateEncodedToLimit(uchar limEnc) {
   switch (limEnc) {
      case(1):  return 0x88;
      case(2):  return 0x08;
      case(3):  return 0x89;
      case(4):  return 0x80;
      case(5):  return 0x8A;
      case(6):  return 0x90;
      case(7):  return 0x9A;
      case(8):  return 0xC0;
      case(9):  return 0xCA;
      case(10): return 0xD0;
      case(11): return 0xDA;
      case(12): return 0x10;
      case(13): return 0x49;
      case(14): return 0x40;
      case(15): return 0x4A;
      case(16): return 0x50;
      case(17): return 0x5A;
      case(18): return 0x60;
      case(19): return 0x52;
      default:  return 0x80;
   }
}

///////////////////////////////////////////////////////////////////////////////
//  portForceOn
//
//  Description:
//      Use the s/w requested cut/lim values and turn on the port. 
//  Arguments:
//      p             - Pointer to port to (force) turn on
//      userInitiated - Override LTC4266 detect == good checks
void portForceOn( struct portStruct * p, unsigned char userInitiated ) {
   uchar cut = p->newcut;
   uchar lim = xlateEncodedToLimit(p->newlim);
   uchar chip = p->i2cAddr;
   int cutadr;
   int limadr;

   // Don't proceed if the port is already on...
   if ( (p->state == PORT_TURNING_ON) ||
        (p->state == PORT_ON        ) ||
        (p->state == PORT_ON_LEGACY ) ) {
      return;
   }

   cutadr = cut1 + 5*p->offset;
   limadr = cutadr + 1;
   p->cut = cut;
   if (gQuarterOhm) {
      cut |= 0xC0;
   }
   p->lim = p->newlim;
   p->disable = 1;

   swI2cWrite(chip, cutadr, cut);
   swI2cWrite(chip, limadr, lim);

   ///////////////////////////
   // Turn on port power

   // Force port on using manual mode if previous detect result != good
   if ( userInitiated ) {
      // Force port to manual mode
      unsigned char tmpOpmd = swI2cRead(chip, opmd);
      tmpOpmd &= ~(0x3<<(p->offset*2));
      tmpOpmd |=  (0x1<<(p->offset*2));
      swI2cWrite(chip, opmd, tmpOpmd);

      // Turn on Power PB
      swI2cWrite(chip, pwrpb, p->lMask);

      // Wait more than 350ms for pbOn to be accepted
      // (a detect may be in progress)
      // NOTE: This could be restructured so it did not stall
      // the entire RTOS...but the user-initiated forceOn
      // is of dubious usefulness, so this code is not optimized.
      mdelay(250);
      mdelay(100);

      // Force port back to semi-auto mode
      tmpOpmd &= ~(0x3<<(p->offset*2));
      tmpOpmd |=  (0x2<<(p->offset*2));
      swI2cWrite(chip, opmd, tmpOpmd);

   // Normal port power-up
   } else {
      swI2cWrite(chip, pwrpb, p->lMask); // Turn on Power PB
   }

   // Mark port as on
   p->state = PORT_TURNING_ON;
}

void poeEnable(void) {
   if (swI2cRead(cI2cBase4266, id) != 0x64) 
      return;
   portForceOn(&portArray[0],TRUE);
   portForceOn(&portArray[1],TRUE);
   gpio_out(GPIO_PSE1_GREEN_LED, 0);
   gpio_out(GPIO_PSE2_GREEN_LED, 0);
}

void poeDisable(void) {
   if (swI2cRead(cI2cBase4266, id) != 0x64) 
      return;
   portChangeDisable(&portArray[0]);   
   portChangeDisable(&portArray[1]);   
   gpio_out(GPIO_PSE1_GREEN_LED, 1);
   gpio_out(GPIO_PSE2_GREEN_LED, 1);
}

// Init poe for uboot, port 0&1 in disable status 
void poeInit(void) {
   if (swI2cRead(cI2cBase4266, id) != 0x64) 
      return;
   initRegMap();
   portInitAll();
}
#endif
