#ifndef M41T83_H
#define M41T83_H

/************************** I2C Address ***************************************/
#define M41T83_ADDRESS		    0x68		// I2C Address 0100 00 + ADDR + R/W

/************************** I2C Registers *************************************/
#define M41T83_TENTHS_SEC_REG	0x00		
#define M41T83_SEC_REG  		0x01		
#define M41T83_MIN_REG      	0x02	
#define M41T83_HOUR_REG     	0x03
#define M41T83_DAY_REG      	0x04
#define M41T83_DATE_REG      	0x05
#define M41T83_MON_REG      	0x06
#define M41T83_YEAR_REG      	0x07
#define M41T83_WDOG_REG      	0x09
#define M41T83_A1_MON_REG      	0x0A
#define M41T83_A1_DATE_REG     	0x0B
#define M41T83_A1_HOUR_REG     	0x0C
#define M41T83_A1_MIN_REG      	0x0D
#define M41T83_A1_SEC_REG      	0x0E
#define M41T83_FLAGS_REG      	0x0F

#if 0
struct M41T83_sBit{
       unsigned char B0:1;
       unsigned char B1:1;
       unsigned char B2:1;
       unsigned char B3:1;
       unsigned char B4:1;
       unsigned char B5:1;
       unsigned char B6:1;
       unsigned char B7:1;
};
#endif

struct M41T83_sTenthsSec{
	unsigned char TenthsSec:4;
	unsigned char HredsSec:4;
};
union M41T83_uTenthsSec{
	unsigned char all;
	struct M41T83_sTenthsSec TenthsSec;
};

struct M41T83_sSec{
	unsigned char Sec:4;
	unsigned char TenSec:3;
	unsigned char ST:1;
};
union M41T83_uSec{
	unsigned char all;
	struct M41T83_sSec Sec;
};

struct M41T83_sMin{
	unsigned char Min:4;
	unsigned char TenMin:3;
	unsigned char zero:1;
};
union M41T83_uMin{
	unsigned char all;
	struct M41T83_sMin Min;
};

struct M41T83_sHour{
	unsigned char Hour:4;
	unsigned char TenHour:2;
	unsigned char CB0:1;
	unsigned char CB1:1;
};
union M41T83_uHour{
	unsigned char all;
	struct M41T83_sHour Hour;
};

struct M41T83_sDay{
	unsigned char Day:3;
	unsigned char zero:5;
};
union M41T83_uDay{
	unsigned char all;
	struct M41T83_sDay Day;
};

struct M41T83_sDate{
	unsigned char Date:4;
	unsigned char TenDate:2;
	unsigned char zero:2;
};
union M41T83_uDate{
	unsigned char all;
	struct M41T83_sDate Date;
};

struct M41T83_sMon{
	unsigned char Mon:4;
	unsigned char TenMon:1;
	unsigned char zero:3;
};
union M41T83_uMon{
	unsigned char all;
	struct M41T83_sMon Mon;
};

struct M41T83_sYear{
	unsigned char Year:4;
	unsigned char TenYear:4;
};
union M41T83_uYear{
	unsigned char all;
	struct M41T83_sYear Year;
};

struct M41T83_sDC{
	unsigned char DC0:1;
	unsigned char DC1:1;
	unsigned char DC2:1;
	unsigned char DC3:1;
	unsigned char DC4:1;
	unsigned char DCS:1;
	unsigned char FT:1;
	unsigned char OUT:1;
};
union M41T83_uDC{
	unsigned char all;
	struct M41T83_sDC DC;
};

struct M41T83_sWatchdog{
	unsigned char RB0:1;
	unsigned char RB1:1;
	unsigned char BMB0:1;
	unsigned char BMB1:1;
	unsigned char BMB2:1;
	unsigned char BMB3:1;
	unsigned char BMB4:1;
	unsigned char OFIE:1;
};
union M41T83_uWatchdog{
	unsigned char all;
	struct M41T83_sWatchdog Watchdog;
};

struct M41T83_sA1Mon{
	unsigned char Mon:4;
	unsigned char TenMon:1;
	unsigned char ABE:1;
	unsigned char SQWE:1;
	unsigned char IE:1;
};
union M41T83_uA1Mon{
	unsigned char all;
	struct M41T83_sA1Mon Mon;
};

struct M41T83_sA1Date{
	unsigned char Date:4;
	unsigned char TenDate:2;
	unsigned char RPT15:1;
	unsigned char RPT14:1;
};
union M41T83_uA1Date{
	unsigned char all;
	struct M41T83_sA1Date Date;
};

struct M41T83_sA1Hour{
	unsigned char Hour:4;
	unsigned char TenHour:2;
	unsigned char HT:1;
	unsigned char RPT13:1;
};
union M41T83_uA1Hour{
	unsigned char all;
	struct M41T83_sA1Hour Hour;
};

struct M41T83_sA1Min{
	unsigned char Hour:4;
	unsigned char TenMin:3;
	unsigned char RPT12:1;
};
union M41T83_uA1Min{
	unsigned char all;
	struct M41T83_sA1Min Min;
};

struct M41T83_sA1Sec{
	unsigned char Sec:4;
	unsigned char TenSec:3;
	unsigned char RPT11:1;
};
union M41T83_uA1Sec{
	unsigned char all;
	struct M41T83_sA1Sec Sec;
};

struct M41T83_sFlags{
	unsigned char Zero:2;
	unsigned char OF:1;
	unsigned char TF:1;
	unsigned char BL:1;
	unsigned char AF2:1;
	unsigned char AF1:1;
	unsigned char WDF:1;
};
union M41T83_uFlags{
	unsigned char all;
	struct M41T83_sFlags Flags;
};

struct M41T83_sTimerCount{
	unsigned char value;
};

struct M41T83_sTimerControl{
	unsigned char TD0:1;
	unsigned char TD1:1;
	unsigned char Zero:3;
	unsigned char TIE:1;
	unsigned char TP:1;
	unsigned char TE:1;
};
union M41T83_uTimerControl{
	unsigned char all;
	struct M41T83_sTimerControl TimerControl;
};

struct M41T83_sAC{
	unsigned char AC0:1;
	unsigned char AC1:1;
	unsigned char AC2:1;
	unsigned char AC3:1;
	unsigned char AC4:1;
	unsigned char AC5:1;
	unsigned char AC6:1;
	unsigned char ACS:1;
};
union M41T83_uAC{
	unsigned char all;
	struct M41T83_sAC AC;
};

struct M41T83_sSQW{
	unsigned char OTP:1;
	unsigned char AL2E:1;
	unsigned char Zero:2;
	unsigned char RS0:1;
	unsigned char RS1:1;
	unsigned char RS2:1;
	unsigned char RS3:1;
};
union M41T83_uSQW{
	unsigned char all;
	struct M41T83_sSQW SQW;
};

struct M41T83_sA2Mon{
	unsigned char Mon:4;
	unsigned char TenMon:1;
	unsigned char Zer0:2;
	unsigned char IE:1;
};
union M41T83_uA2Mon{
	unsigned char all;
	struct M41T83_sA2Mon Mon;
};

struct M41T83_sA2Date{
	unsigned char Date:4;
	unsigned char TenDate:2;
	unsigned char RPT25:1;
	unsigned char RPT24:1;
};
union M41T83_uA2Date{
	unsigned char all;
	struct M41T83_sA2Date Date;
};

struct M41T83_sA2Hour{
	unsigned char Hour:4;
	unsigned char TenHour:2;
	unsigned char Zero:1;
	unsigned char RPT23:1;
};
union M41T83_uA2Hour{
	unsigned char all;
	struct M41T83_sA2Hour Hour;
};

struct M41T83_sA2Min{
	unsigned char Hour:4;
	unsigned char TenMin:3;
	unsigned char RPT22:1;
};
union M41T83_uA2Min{
	unsigned char all;
	struct M41T83_sA2Min Min;
};

struct M41T83_sA2Sec{
	unsigned char Sec:4;
	unsigned char TenSec:3;
	unsigned char RPT21:1;
};
union M41T83_uA2Sec{
	unsigned char all;
	struct M41T83_sA2Sec Sec;
};

struct M41T83_sTimestamp {
	union M41T83_uTenthsSec TenthsSec;
	union M41T83_uSec Sec;
	union M41T83_uMin Min;
	union M41T83_uHour Hour;
	union M41T83_uDay Day;
	union M41T83_uDate Date;
	union M41T83_uMon Mon;
	union M41T83_uYear Year;
};

struct M41T83_sEigerSRAM {
	unsigned short access;
	unsigned char rtc_frozen_time[8];
};

typedef struct {
	union M41T83_uTenthsSec TenthsSec;
	union M41T83_uSec Sec;
	union M41T83_uMin Min;
	union M41T83_uHour Hour;
	union M41T83_uDay Day;
	union M41T83_uDate Date;
	union M41T83_uMon Mon;
	union M41T83_uYear Year;
	union M41T83_uDC DC;
	union M41T83_uWatchdog Watchdog;
	union M41T83_uA1Mon A1Mon;
	union M41T83_uA1Date A1Date;
	union M41T83_uA1Hour A1Hour;
	union M41T83_uA1Min A1Min;
	union M41T83_uA1Sec A1Sec;
	union M41T83_uFlags Flags;
	struct M41T83_sTimerCount TimerCount;
	union M41T83_uTimerControl TimerControl;
	union M41T83_uAC AC;
	union M41T83_uSQW SQW;
	union M41T83_uA2Mon A2Mon;
	union M41T83_uA2Date A2Date;
	struct M41T83_sEigerSRAM SRAM;
	//union M41T83_uA2Hour A2Hour;
	//union M41T83_uA2Min A2Min;
	//union M41T83_uA2Sec A2Sec;
	//unsigned char UserSRAM[7];
} M41T83Regs;

#endif
