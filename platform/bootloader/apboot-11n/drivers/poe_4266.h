
#ifndef _POE_4266_h
#define _POE_4266_h

#ifdef CONFIG_LTC_4266
#define FALSE 0
#define TRUE  1

#define I2C_ARA_ADDR     0x0C
#define I2C_READ_FLAG    0x01

#define gcNumQuads         1
#define gcNumPortsPerQuad   4
#define gcNumPorts         gcNumQuads*gcNumPortsPerQuad // 4

#define cBcastAddr      0x30 //all LTC4266s will respond to this address
#define cI2cBase4266    0x21 //for Tamdhu, i2c slave address is b010_0001

// Internal State and Structure
//  Made "public" only for read, should not be written outside this module
enum enum_portClass {CLASS_0 = 6,
							CLASS_1 = 1,
							CLASS_2 = 2,
							CLASS_3 = 3,
							CLASS_4 = 4,
							CLASS_7 = 7
							};

enum enum_portState {PORT_UNKNOWN    =  0,
							PORT_SHORT      =  1,
							PORT_CAPHI      =  2,
							PORT_RLOW       =  3,
							PORT_GOOD       =  4,
							PORT_RHIGH      =  5,
							PORT_OPEN       =  6,
							PORT_CHARGED    =  7,
							PORT_OFF        =  8,
							PORT_TURNING_ON =  9,
							PORT_ON         = 10,
							PORT_ON_LEGACY  = 11,
							PORT_FATAL      = 12,
							PORT_BAD_CLS    = 13,
							PORT_DISABLED   = 15
							};

enum enum_portEvent {EVENT_NONE            = 0,
							EVENT_DISCONNECT      = 1,
							EVENT_TCUT            = 2,
							EVENT_TSTART          = 3,
							EVENT_DENIED          = 4,
							EVENT_REVOKED         = 5,
							EVENT_COHERENCY       = 6,
							EVENT_COHERENCY_FATAL = 7
							};

struct portStruct {
  					    uchar i2cBus;       // port i2c bus number (0 or 1)
  	 				    uchar i2cAddr;      // port i2c address    (0x20 thru 0x2F, every 4th unused)
  	 				    uchar number;       // port soft number (0 thru 47)
  	 				    uchar offset;       // port offset inside quad (0 thru 3)
  	 				    uchar lMask;        // one-hot lower nibble mask
  	 				    uchar uMask;        // one-hot upper nibble mask
  	 				    uchar present;      // port present (boolean)
  	 				    uchar coherencyCnt; // coherency counter
  	 				    uchar msd;          // Single bit value representing Masked Shutdown
  	 				                        // Written to LTC4266, event handled in h/w
  	 				                        // Stored in flash
  	 				    uchar legEn;        // Single bit value representing Legacy Enable
  	 				                        // Stored in flash
  	 				    uchar disable;      // Single bit value representing port disable state
  	 				                        // Stored in flash
  	 				    uchar state;        // port state
  	 				    uchar event;        // port most recent event
  	 				    uchar cut;          // Current CUT programmed in LTC4266
  	 				    uchar lim;          // Current LIM programmed in LTC4266
  	 				    uchar newcut;       // New CUT to be used on Port Power Up or Change CUT/LIM
  	 				                        // commands are issued
  	 				    uchar newlim;       // New LIM to be used on Port Power Up or Change CUT/LIM
  	 				                        // commands are issued
  	 				    uint  currentport;      // Port current (averaged over window)
  	 				    uchar currentNumLocsFilled; // Number of window slots filled with current samples
  	 				                        // (circular buffer size is zero as port comes up
  	 				                        //  and then wraps when full)
  	 				    uchar currentIndex; // Location of newest current sample in circular buffer
  	 				                        // (reset to zero when port comes up, wraps thereafter)
  	 				    uint  currentArray[8]; // Circular buffer of current samples in native units
  	 				                        // Used by window/averaging function to smooth samples
  	 				    uint  voltage;      // Instantaneous port voltage  	 				    
            
  	 				    uchar priorityReq;  // Priority "dirty" bit (reordering is required)
  	 				    uchar msdReq;       // Boolean value representing change of msd state
  	 				    uchar disableReq;   // Boolean value representing port turn off request
  	 				    };

struct quadStruct {uchar i2cBus;                   // quad i2c bus number (0 or 1)
  	 				    uchar i2cAddr;                  // quad i2c address    (0x20 thru 0x2F)
  	 				    uchar present;                  // quad physically present
  	 				    uchar legacyToken;              // quad legacy detection token (only one port at a time)
  	 				                                    //  this protects FastIV resource
  	 				    uchar legacyAttempts;           // quad legacy detection attempt counter
  	 				    struct portStruct * pFirstPort; // pointer to first port in this quad
  	 				   };

// Function Prototypes
extern void portInitAll( void );
extern void portTurnOffRevoke( struct portStruct * p, uchar reEnable );
extern void portChangeDisable( struct portStruct * p );
extern void portExitLegacyMode( struct quadStruct * q, struct portStruct * p );
extern struct quadStruct * portLookupQuad( struct portStruct * p );
extern void poeInit(void) ;

#endif
#endif
