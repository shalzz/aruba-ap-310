/******************************************
*  fulfil two interface used by sysmon.c
*  interface @1 sysmon_list[]
*            @2 sysmon_table[]
*	    RichardXY_huang@asus.com.cn
*
********************************************/
#include <common.h>

#if CONFIG_POST & CFG_POST_SYSMON
#include <i2c.h>
//#include "sysmon.h"

//#include "w83792d.h"


//#define CFG_I2C_SYSMON_W83792D_ADDR 0x2c
#define W83792D_REG_CONFIG		0x40
#define W83792D_REG_VID_FANDIV		0x47
#define W83792D_REG_CHIPID		0x49
#define W83792D_REG_WCHIPID		0x58
#define W83792D_REG_CHIPMAN		0x4F
#define W83792D_REG_PIN			0x4B
#define W83792D_REG_I2C_SUBADDR		0x4A

#define W83792D_REG_ALARM1 0xA9		/* realtime status register1 */
#define W83792D_REG_ALARM2 0xAA		/* realtime status register2 */
#define W83792D_REG_ALARM3 0xAB		/* realtime status register3 */
#define W83792D_REG_CHASSIS 0x42	/* Bit 5: Case Open status bit */
#define W83792D_REG_CHASSIS_CLR 0x44	/* Bit 7: Case Open CLR_CHS/Reset bit */

/* control in0/in1 's limit modifiability */
#define W83792D_REG_VID_IN_B		0x17

#define W83792D_REG_VBAT		0x5D
#define W83792D_REG_I2C_ADDR		0x48

#define W83792D_REG_BANK		0x4E
#define W83792D_REG_TEMP2_CONFIG	0xC2
#define W83792D_REG_TEMP3_CONFIG	0xCA

typedef struct sysmon_s sysmon_t;
typedef struct sysmon_table_s sysmon_table_t;

struct sysmon_s
{
    uchar   chip;
    void (*init)(sysmon_t *);
    int (*read)(sysmon_t *, uint8_t);
};

struct sysmon_table_s
{
    char *          name;
    char *          unit_name;
    sysmon_t *      sysmon;
    void            (*exec_before)(sysmon_table_t *);
    void            (*exec_after)(sysmon_table_t *);
    float           (*get_value)(sysmon_t *, uint8_t *, uint8_t *);
    uint8_t * addr;
    uint8_t * mask;
};


static void sysmon_w83792d_init(sysmon_t *);

static int w83792d_read_value(sysmon_t *, u8);

//float result;

static int w83792d_get_val_t(sysmon_t *this, uint8_t *addr, uint8_t *mask, char *result)
{
    int val, res;
    char * p;

    val = this->read(this, addr[0]);
    res = val & mask[0];
    if ( res > 123 )
	res = 0;
    p = result + sprintf(result,"%d", res);
    val = this->read(this, addr[1]);

    if((res!=0 )&& (val & mask[1])) {
	*p++ = '.';
	*p++ = '5';
    }
    *p++ = '\0';

    return 0;
}

static int w83792d_get_val_v1(sysmon_t *this, uint8_t *add, uint8_t *mask, char *result)
{
    int val, res;
    char *p;
    
    val = this->read(this, add[0]);
    res = ( val & mask[0] ) << 3;
    val = this->read(this, add[1]) & mask[1];
    mask[1] = mask[1]>>2;
    while ( mask[1] != 0){
    	mask[1] = mask[1] >> 2;
	val = val >> 2;
    }
    
    res += val << 1;

    p = result + sprintf(result,"%d", res);
    *p++ = '\0';
    return 0;
}

static int w83792d_get_val_v2(sysmon_t *this, uint8_t *add, uint8_t *mask, char *result)
{
    int val, res;
    char *p;
    val = this->read(this, add[0]);
    res = ( val & mask[0] ) << 4;
    val = this->read(this, add[1]) & mask[1];
    
    mask[1] = mask[1]>>2;
    while( mask[1] != 0){
	mask[1] = mask[1] >> 2;
	val = val >> 2;
    }
					  
    res += val << 2;

    p = result + sprintf(result,"%d", res);
    *p++ = '\0';

    return res;
}

static int w83792d_get_val_v3(sysmon_t *this, uint8_t *add, uint8_t *mask, char *result)
{
    int val, res;
    char *p;
    val = this->read(this, add[0]);
    res = ( val & mask[0] ) << 4;
    val = this->read(this, add[0]) & mask[1];

    
    mask[1] = mask[1]>>2;
    while( mask[1] != 0){
	mask[1] = mask[1] >> 2;
	val = val >> 2;
    }
    
    res += val << 2;
    res = res*(10+28);
    p = result + sprintf(result,"%d", res/10);
    *p++ = '.';
    *p++ = '0' + res % 10;
    *p++ = '\0';
    return res;
}

static int w83792d_get_val_v4(sysmon_t *this, uint8_t *add, uint8_t *mask, char *result)
{
    int val, res;
    char *p;
    val = this->read(this, add[0]);
    res = ( val & mask[0] ) << 3;
    res *=3;
    
    val = this->read(this, add[1]) & mask[1];

    mask[1] = mask[1]>>2;
    while( mask[1] != 0){
       	mask[1] = mask[1] >> 2;
	val = val >> 2;
    }
	 
    res += val << 2;

    p = result + sprintf(result,"%d", res);
    *p++ = '\0';

    return res;
}

static int w83792d_get_val_v5(sysmon_t *this, uint8_t *add, uint8_t *mask, char *result)
{
    int val, res;
    char *p;
    val = this->read(this, add[0]);
    res = ( val & mask[0] ) << 3;
    res *=3;
    p = result + sprintf(result,"%d", res);
    *p++ = '\0';
    return res;
}


static int w83792d_get_val_v6(sysmon_t *this, uint8_t *add, uint8_t *mask, char *result)
{
    int val, res;
    char *p;
    val = this->read(this, add[0]);
    res = ( val & mask[0] ) << 4;
    p = result + sprintf(result,"%d", res);
    *p++ = '\0';
    return res;
}


static int w83792d_get_val_f(sysmon_t *this, uint8_t *add, uint8_t *mask, char *result )
{

    int val, res, div;
    char *p;
    val = this->read(this, add[0]) & mask[0];
    div = this->read(this, add[1]) & mask[1];

     mask[1] = mask[1]>>4;
     while( mask[1] != 0){
	 mask[1] = mask[1] >> 4;
	 div = div >> 4;
     }

    switch(div)
    {
       case 0: div=1; break;
       case 1: div=2; break;
       case 2: div=3; break;
       case 3: div=4; break;
       case 4: div=5; break;
       case 5: div=6; break;
       case 6: div=7; break;
       case 7: div=8; break;
    }

    res=div*val;
   
     if ( val==0xff )
	p = result + sprintf(result,"0");
     else
        p = result + sprintf(result,"%d", 1350000/res);
        
     *p++ = '\0';

     if ( val==0xff )
        return (0);
     else
        return (1350000/res);

}


static sysmon_t sysmon_w83792d =
	{CFG_I2C_SYSMON_W83792D_ADDR, sysmon_w83792d_init, w83792d_read_value};




//interface 1

sysmon_t * sysmon_list[] = 
{
    &sysmon_w83792d,
    NULL
};

uint8_t add_t1[] = {0x27, 0xaf};
uint8_t add_t2[] = {0xc0, 0xc1};
uint8_t add_t3[] = {0xc8, 0xc9};
uint8_t mask_t[] = {0xff, (0x1<<7)};

uint8_t add_v1[] = {0x20, 0xae};
uint8_t add_v2[] = {0x21, 0xae};
uint8_t add_v3[] = {0x22, 0xae};
uint8_t add_v4[] = {0x23, 0xae};

uint8_t add_v5[] = {0x24, 0xaf};
uint8_t add_v6[] = {0x25, 0xaf};
uint8_t add_v7[] = {0x26, 0xaf};

uint8_t add_v8[] = {0xb0};
uint8_t add_v9[] = {0xb1};

uint8_t mask_v5[] = {0xff};

uint8_t mask_v1[] = {0xff, (0x3<<0)};
uint8_t mask_v2[] = {0xff, (0x3<<2)};
uint8_t mask_v3[] = {0xff, (0x3<<4)};
uint8_t mask_v4[] = {0xff, (0x3<<6)};

uint8_t add_f1[] = {0x28, 0x47};
uint8_t add_f2[] = {0x29, 0x47};
uint8_t add_f3[] = {0x2a, 0x5b};
uint8_t add_f4[] = {0xb8, 0x5b};
uint8_t add_f5[] = {0xb9, 0x5c};
uint8_t add_f6[] = {0xbe, 0x9e};

uint8_t mask_f1[]= {0xff, (0x7<<0)};
uint8_t mask_f2[]= {0xff, (0x7<<4)};
//interface 2



sysmon_table_t sysmon_table[] =
{
    {"CPU temperature", "C", &sysmon_w83792d, NULL, NULL, w83792d_get_val_t, add_t1, mask_t},//H 0x27 L 0xaf <7>
    {"SYS temperature", "C", &sysmon_w83792d, NULL, NULL, w83792d_get_val_t, add_t2, mask_t},//H 0xC0 L 0xc1 <7>
    {"HDD temperature", "C", &sysmon_w83792d, NULL, NULL, w83792d_get_val_t, add_t3, mask_t},//H 0xC8 L 0xc9 <7>

    {"Core voltage   ", "mV", &sysmon_w83792d, NULL, NULL, w83792d_get_val_v1, add_v1, mask_v1},//H 0x20 L 0xAE,<0> <1>
    {" +1.8   V      ", "mV", &sysmon_w83792d, NULL, NULL, w83792d_get_val_v1, add_v2, mask_v2},//H 0x21 L 0xAE, <2> <3>
    {" +3     V      ", "mV", &sysmon_w83792d, NULL, NULL, w83792d_get_val_v2, add_v3, mask_v3},//H 0x22 L 0xAE, <4> <5>
    {" +12    V      ", "mV", &sysmon_w83792d, NULL, NULL, w83792d_get_val_v3, add_v4, mask_v4},//H 0x23 L 0xAE, <6> <7>

    {" DDR voltage   ", "mV", &sysmon_w83792d, NULL, NULL, w83792d_get_val_v2, add_v5, mask_v1},//H 0x24 L 0xAF, <0> <1>
    {" +2.5   V      ", "mV", &sysmon_w83792d, NULL, NULL, w83792d_get_val_v2, add_v6, mask_v2},//H 0x25 L 0xAF, <2> <3>
    {" VCC  +5V      ", "mV", &sysmon_w83792d, NULL, NULL, w83792d_get_val_v4, add_v7, mask_v3},//H 0x26 L 0xAF, <4> <5>
    {" VSB  +5V      ", "mV", &sysmon_w83792d, NULL, NULL, w83792d_get_val_v5, add_v8, mask_v5},//H 0xb0 L ??
    {" VBAT          ", "mV", &sysmon_w83792d, NULL, NULL, w83792d_get_val_v6, add_v9, mask_v5},//H 0xb1 L ??
    
    {" CPU    FAN    ", "RPM", &sysmon_w83792d, NULL, NULL, w83792d_get_val_f, add_f1, mask_f1},//data 0x28 divisor 0x47 <2:0>
    {" SYSTEM FAN    ", "RPM", &sysmon_w83792d, NULL, NULL, w83792d_get_val_f, add_f2, mask_f2},//data 0x29 divisor ox47 <6:4>
    {" HDD    FAN    ", "RPM", &sysmon_w83792d, NULL, NULL, w83792d_get_val_f, add_f3, mask_f1},//data 0x2a divisor 0x5b <2:0>

    {" CPU2   FAN    ", "RPM", &sysmon_w83792d, NULL, NULL, w83792d_get_val_f, add_f4, mask_f2},//data 0xb8 divisor 0x5b <6:4>
    {" FAN5          ", "RPM", &sysmon_w83792d, NULL, NULL, w83792d_get_val_f, add_f5, mask_f1},//data 0xb9 divisor 0x5c <2:0>
    {" FAN6          ", "RPM", &sysmon_w83792d, NULL, NULL, w83792d_get_val_f, add_f6, mask_f1},//data 0xbe divisor 0x9e <2:0>
    
};


extern int sysmon_table_size = sizeof(sysmon_table) / sizeof(sysmon_table[0]);

/************************************************************************/
static int w83792d_read_value(sysmon_t *this, uint8_t reg)
{
    uint8_t temp;
    int val;
    i2c_read(this->chip, reg, 1, &temp, 1);
    val = temp;
    return (val);
}


static int w83792d_write_value(sysmon_t *this, u8 reg, u8 value)
{
    uint8_t val;
    val = (uint8_t)value;
    i2c_write(this->chip, reg, 1, &val, 1);
    return 0;
}


/*
* modified from the linux driver w83792d.c
*w83792d_init_client(struct i2c_client *client)
* RichardXY_Huang@asus.com.cn
*/

int dump_w83792d_reg(sysmon_t *this)
{
   uint16_t i, j;

   printf("Dump w83792d \n");
    for(i=0; i<256; i+=16)
    {
       printf("%04X: ", i);
       for(j=0; j<16; j++)
       {
          if ( j==7 )
             printf("%02x-", w83792d_read_value(this, i+j) & 0xff);
          else
             printf("%02x ", w83792d_read_value(this, i+j) & 0xff);
       }
       printf("\n");
    }

    return 0;
}

void sysmon_w83792d_init(sysmon_t *this)
{
    unsigned char  temp2_cfg, temp3_cfg, vid_in_b;
    
    //w83792d_write_value(this, W83792D_REG_CONFIG, 0x80);

    /* Clear the bit6 of W83792D_REG_VID_IN_B(set it into 0):
       W83792D_REG_VID_IN_B bit6 = 0: the high/low limit of
       vin0/vin1 can be modified by user;
       W83792D_REG_VID_IN_B bit6 = 1: the high/low limit of
       vin0/vin1 auto-updated, can NOT be modified by user. */
    //vid_in_b = w83792d_read_value(this, W83792D_REG_VID_IN_B);
    //w83792d_write_value(this, W83792D_REG_VID_IN_B, vid_in_b & 0xbf);
    
    //temp2_cfg = w83792d_read_value(this, W83792D_REG_TEMP2_CONFIG);
    //temp3_cfg = w83792d_read_value(this, W83792D_REG_TEMP3_CONFIG);
    //w83792d_write_value(this, W83792D_REG_TEMP2_CONFIG, temp2_cfg & 0xe6);
    //w83792d_write_value(this, W83792D_REG_TEMP3_CONFIG, temp3_cfg & 0xe6);
    /* Start monitoring */
    //w83792d_write_value(this, W83792D_REG_CONFIG, (w83792d_read_value(this, W83792D_REG_CONFIG) & 0xf7) | 0x01);
/*
    temp3_cfg = w83792d_read_value(this, 0x4b);
    w83792d_write_value(this, 0x4b, temp3_cfg | 0x04);	// enable fan 7

    temp3_cfg = w83792d_read_value(this, 0x59);		
    w83792d_write_value(this, 0x59, temp3_cfg & 0x8f);	// set tempearture1, 2, 3 to thermistor 

    temp3_cfg = w83792d_read_value(this, 0x5d);		
    w83792d_write_value(this, 0x5d, temp3_cfg | 0x01);	// enable vbat

    w83792d_write_value(this, 0x47, 0x99);		// enable fan 1, 2
    w83792d_write_value(this, 0x5b, 0x99);		// enable fan 3, 4
    w83792d_write_value(this, 0x5c, 0x19);		// enable fan 5
    w83792d_write_value(this, 0x9e, 0x09);		// enable fan 7
*/
    //dump_w83792d_reg(this);
}

#endif
