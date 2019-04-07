/*
 * $Id: shmoo_ddr40.c,v 1.53.2.1.4.2 2012/11/14 17:10:30 mbasoglu Exp $
 * $Copyright: (c) 2011 Broadcom Corp.
 * All Rights Reserved.$
 *
 * DDR3 Memory support
 */
#include <config.h>
#include <common.h>
#include "asm/arch/socregs.h"
#include "asm/iproc/iproc_common.h"
#include "asm/iproc/reg_utils.h"
#define BCM_ARAD_SUPPORT 1
#define SAL_BOOT_QUICKTURN 0

#define STATIC static

#define SOC_IF_ERROR_RETURN(x)	(x)
#define SOC_DEBUG(x,y) printf(y)
#define SOC_DBG_DDR	1
#define SOC_DEBUG_CHECK(x) (x)
//#define soc_cm_debug(x,y,...)	printf(y,...);
#define soc_cm_print	printf
#define sal_alloc(x,y)	malloc(x)
#define sal_memset(x,y,z)	memset(x,y,z)
#define sal_free(x)	free(x)
#define sal_sprintf sprintf

#define DK_ERR "Error: "
#define DK_VERBOSE "Log: "

#define TRUE (1)
#define FALSE (0)

#define BCM_DDR3_SUPPORT (1)
#define SOC_IS_KATANA(unit)	(0)
#define SOC_IS_CALADAN3(unit)	(0)
#define SOC_IS_ARAD(unit)	(1)

#ifdef BCM_DDR3_SUPPORT
#include <config.h>
#include "asm/iproc/ddr40.h"
#include "asm/iproc/shmoo_ddr40.h"
#include "asm/iproc/ddr_bist.h"

#define DDR_TIMEOUT_10mS 1000
#define NUM_DATA 8
#define SET_UNOVR_STEP(v) ( 0x20000 | ( (v) & 0x3F ) )  /* OVR_FORCE = OVR_EN = 1, OVR_STEP = v */
#define SET_OVR_STEP(v) ( 0x30000 | ( (v) & 0x3F ) )    /* OVR_FORCE = OVR_EN = 1, OVR_STEP = v */

#define RD_EN_DQS_CORROLATION 2
#define MAX_SHMOO_INTERFACES 32

/***********************************************************************
* DDR Initialization Functions
***********************************************************************/
#if (defined(CONFIG_HELIX4) || defined(CONFIG_KATANA2))
const uint32 shmoo_dram_config = 3; //0x00000000;
#elif (defined(CONFIG_HURRICANE2))
const uint32 shmoo_dram_config = 1; //0x00000000;
#elif (defined(CONFIG_NS_PLUS))
#if (defined(CONFIG_DDR32))
const uint32 shmoo_dram_config = 3; //0x00000000;
#else
const uint32 shmoo_dram_config = 1; //0x00000000;
#endif /* (defined(CONFIG_DDR32)) */
#else
const uint32 shmoo_dram_config = 0;
#endif
const uint32 drc_channel_set[8] = {DRCA, DRCB, DRCC, DRCD, DRCE, DRCF, DRCG, DRCH};

#endif


typedef struct soc_timeout_struct_t {
	uint32	ms;
	uint32 ref;
	uint32 to;
} soc_timeout_t;

typedef unsigned int sal_usecs_t;

void soc_cm_debug(const char *msg, const char *ptFormatStr , ...)
{
    char __tmp_buf[ 128 ];
    va_list arg;
	uint32 len = strlen(msg);

	strcpy(__tmp_buf, msg);

    va_start( arg , ptFormatStr );
    vsprintf( &__tmp_buf[ len ] , ptFormatStr , arg ); 
    va_end( arg );

    printf( __tmp_buf );

    return;

}

void
soc_timeout_init(soc_timeout_t * soc_timeout, sal_usecs_t to_val, int dummy)
{
	//printf("soc_timeout_init: Enter\n");
	soc_timeout->ms = (to_val + 1000) / 1000; /* Convert into milli seconds */
	soc_timeout->ref = get_timer(0);
	soc_timeout->to = soc_timeout->ref + soc_timeout->ms;
	//printf("soc_timeout_init: Exit\n");
	return;
}

int 
soc_timeout_check(soc_timeout_t * soc_timeout)
{	
	//printf("soc_timeout_check: Enter\n");
	if(get_timer(soc_timeout->ref) > soc_timeout->ms) 
	{
		return (1); /* timeout */
	}
	//printf("soc_timeout_check: Exit\n");
	return (0);
}

void
sal_usleep(uint32 usec)
{
	soc_timeout_t to;
	soc_timeout_init(&to, usec, 0);
	while(!soc_timeout_check(&to)); 
}

void
soc_ddr40_set_shmoo_dram_config(uint32 unit, uint32 dram_config)
{
    //shmoo_dram_config = dram_config;
}

int soc_reg32_get(volatile uint32 *reg, uint32 *rvp)
{
	*rvp = *reg;
	return SOC_E_NONE;
}

int soc_reg32_set(volatile uint32 *reg, uint32 rv)
{
	//printf("soc_reg32_set: addr: 0x%08x, val: 0x%08x\n", reg, rv);
	*reg = rv;
	return SOC_E_NONE;
}

uint32
_check_dram(int ci)
{
#if (defined(CONFIG_NS_PLUS))
#if (!defined(CONFIG_DDR32))
	uint32_t status, otp_status;
	status = get_otp_mem(0x0c, &otp_status);
	if (!status) {
		if ( ((otp_status>>21)&0x07ff) == 0x25) {
			// 32bit ddr
			uint32 shmoo_dram_config32 = 3;
		    return (shmoo_dram_config32 & (0x1 << ci));
		}
	}
#endif /* (!defined(CONFIG_DDR32)) */
#endif /* (defined(CONFIG_NS_PLUS)) */
    return (shmoo_dram_config & (0x1 << ci));
}

uint32
_check_dram_either(int ci)
{
    uint32 result;

    result = _check_dram(ci);

    if(ci & 0x1) {
        result |= _check_dram(ci - 1);
    } else {
        result |= _check_dram(ci + 1);
    }

    return result;
}

uint32
_check_dram_both(int ci)
{
    uint32 result;

    if(_check_dram(ci)){
        result = 1;
    } else {
        result = 0;
    }

    if(ci & 0x1) {
        if(!_check_dram(ci - 1)) {
            result = 0;
        }
    } else {
        if(!_check_dram(ci + 1)) {
            result = 0;
        }
    }

    return result;
}

const size_mem_type_set_t smts[MEM_COUNT] = { 
            {0, 2}, /* MEM_4G, 14b */
            {1, 1}, /* MEM_2G, 13b */
            {2, 0} /* MEM_1G, 12b */
};

int
_soc_ddr40_mem_rowstoloc(uint32 rows)
{
    switch(rows) {
        case MEM_ROWS_32K:
            return MEM_4G;
        case MEM_ROWS_16K:
            return MEM_2G;
        default:
            return MEM_1G;
    }
}

int
_soc_ddr40_mem_gradetoloc(uint32 grade)
{
    switch(grade) {
        case MEM_GRADE_080808:
            return GRADE_080808;
        case MEM_GRADE_090909:
            return GRADE_090909;
        case MEM_GRADE_101010:
            return GRADE_101010;
        case MEM_GRADE_111111:
            return GRADE_111111;
        case MEM_GRADE_121212:
            return GRADE_121212;
        case MEM_GRADE_131313:
            return GRADE_131313;
        default:
            return GRADE_DEFAULT;
    }
}

const  phy_freq_div_set_t pfds[PHY_COUNT][FREQ_COUNT] = {
/*                 400      500       533       667       800      933      1066 */
/* PHY_ RSVP */ {{-1,-1}, {-1,-1},  {-1,-1},  {-1,-1},  {-1,-1}, {-1,-1},  {-1,-1}},
/* PHY_ NS   */ {{80,10}, {100,10}, {96,9},   {80,6},   {80,5},  {168,9},  {192,9}},
/* PHY_ ENG  */ {{80,10}, {100,10}, {96,9},   {80,6},   {80,5},  {168,9},  {192,9}},
/* PHY_ AND  */ {{80,10}, {100,10}, {96,9},   {80,6},   {80,5},  {112,6},  {192,9}},
/* PHY_ CE   */ {{-1,-1}, {-1,-1},  {-1,-1},  {-1,-1},  {-1,-1}, {-1,-1},  {-1,-1}}
};

#if 1 /* Uncomment this when required */
static void
_shmoo_print_diagnostics(int unit, int ci, int wl) {

    int x, y;
    uint32 data;

    x = 0x004C;
    if(DDR40_REG_READ(unit, ci, 0x00, x, &data) == SOC_E_NONE) {
            printf("Address = 0x%04X\t\tData = 0x%08X\t\tVDL CALIB STATUS\n", x, data);
        } else {
            printf("Address = 0x%04X\t\tERROR            \t\tVDL CALIB STATUS\n", x);
    }
    x = 0x0058;
    if(DDR40_REG_READ(unit, ci, 0x00, x, &data) == SOC_E_NONE) {
            printf("Address = 0x%04X\t\tData = 0x%08X\t\tVDL RD EN CALIB STATUS\n", x, data);
        } else {
            printf("Address = 0x%04X\t\tERROR            \t\tVDL RD EN CALIB STATUS\n", x);
    }
    x = 0x0050;
    if(DDR40_REG_READ(unit, ci, 0x00, x, &data) == SOC_E_NONE) {
            printf("Address = 0x%04X\t\tData = 0x%08X\t\tVDL DQ/DQS CALIB STATUS\n", x, data);
        } else {
            printf("Address = 0x%04X\t\tERROR            \t\tVDL DQ/DQS CALIB STATUS\n", x);
    }
    x = 0x0054;
    if(DDR40_REG_READ(unit, ci, 0x00, x, &data) == SOC_E_NONE) {
            printf("Address = 0x%04X\t\tData = 0x%08X\t\tVDL WR DQ CALIB STATUS\n", x, data);
        } else {
            printf("Address = 0x%04X\t\tERROR            \t\tVDL WR DQ CALIB STATUS\n", x);
    }
    x = 0x0030;
    if(DDR40_REG_READ(unit, ci, 0x00, x, &data) == SOC_E_NONE) {
            printf("Address = 0x%04X\t\tData = 0x%08X\t\tVDL OVRIDE BYTE CTL\n", x, data);
        } else {
            printf("Address = 0x%04X\t\tERROR            \t\tVDL OVRIDE BYTE CTL\n", x);
    }
    x = 0x0034;
    if(DDR40_REG_READ(unit, ci, 0x00, x, &data) == SOC_E_NONE) {
            printf("Address = 0x%04X\t\tData = 0x%08X\t\tVDL OVRIDE BIT CTL\n", x, data);
        } else {
            printf("Address = 0x%04X\t\tERROR            \t\tVDL OVRIDE BIT CTL\n", x);
    }
    x = 0x003C;
    if(DDR40_REG_READ(unit, ci, 0x00, x, &data) == SOC_E_NONE) {
            printf("Address = 0x%04X\t\tData = 0x%08X\t\tZQ PVT COMP CTL\n", x, data);
        } else {
            printf("Address = 0x%04X\t\tERROR            \t\tZQ PVT COMP CTL\n", x);
    }
    x = 0x006C;
    if(DDR40_REG_READ(unit, ci, 0x00, x, &data) == SOC_E_NONE) {
            printf("Address = 0x%04X\t\tData = 0x%08X\t\tVREF DAC CONTROL\n", x, data);
        } else {
            printf("Address = 0x%04X\t\tERROR            \t\tVREF DAC CONTROL\n", x);
    }
    if(wl == 0) {
        y = 0;
    } else {
#ifdef CONFIG_IPROC_DDR_ECC
        if(wl == 2) {
            y = 0x0400;
        } else {
#endif
            y = 0x0200;
#ifdef CONFIG_IPROC_DDR_ECC
        }
#endif
    }
    x = y + 0x0360;
    if(DDR40_REG_READ(unit, ci, 0x00, x, &data) == SOC_E_NONE) {
            printf("Address = 0x%04X\t\tData = 0x%08X\t\tREAD DATA DLY\n", x, data);
        } else {
            printf("Address = 0x%04X\t\tERROR            \t\tREAD DATA DLY\n", x);
    }
    x = y + 0x0200;
    if(DDR40_REG_READ(unit, ci, 0x00, x, &data) == SOC_E_NONE) {
            printf("Address = 0x%04X\t\tData = 0x%08X\t\tVDL OVRIDE BYTE RD EN\n", x, data);
        } else {
            printf("Address = 0x%04X\t\tERROR            \t\tVDL OVRIDE BYTE RD EN\n", x);
    }
    x = y + 0x0274;
    if(DDR40_REG_READ(unit, ci, 0x00, x, &data) == SOC_E_NONE) {
            printf("Address = 0x%04X\t\tData = 0x%08X\t\tVDL OVRIDE BYTE0 BIT RD EN\n", x, data);
        } else {
            printf("Address = 0x%04X\t\tERROR            \t\tVDL OVRIDE BYTE0 BIT RD EN\n", x);
    }
#ifdef CONFIG_IPROC_DDR_ECC
    if(wl != 2) {
#endif
        x = y + 0x0314;
        if(DDR40_REG_READ(unit, ci, 0x00, x, &data) == SOC_E_NONE) {
                printf("Address = 0x%04X\t\tData = 0x%08X\t\tVDL OVRIDE BYTE1 BIT RD EN\n", x, data);
            } else {
                printf("Address = 0x%04X\t\tERROR            \t\tVDL OVRIDE BYTE1 BIT RD EN\n", x);
        }
#ifdef CONFIG_IPROC_DDR_ECC
    }
#endif
    x = y + 0x0234;
    if(DDR40_REG_READ(unit, ci, 0x00, x, &data) == SOC_E_NONE) {
            printf("Address = 0x%04X\t\tData = 0x%08X\t\tVDL OVRIDE BYTE0 BIT0 R DQ\n", x, data);
        } else {
            printf("Address = 0x%04X\t\tERROR            \t\tVDL OVRIDE BYTE0 BIT0 R DQ\n", x);
    }
#ifdef CONFIG_IPROC_DDR_ECC
    if(wl != 2) {
#endif
        x = y + 0x02D4;
        if(DDR40_REG_READ(unit, ci, 0x00, x, &data) == SOC_E_NONE) {
                printf("Address = 0x%04X\t\tData = 0x%08X\t\tVDL OVRIDE BYTE1 BIT0 R DQ\n", x, data);
            } else {
                printf("Address = 0x%04X\t\tERROR            \t\tVDL OVRIDE BYTE1 BIT0 R DQ\n", x);
        }
#ifdef CONFIG_IPROC_DDR_ECC
    }
#endif
    x = y + 0x0208;
    if(DDR40_REG_READ(unit, ci, 0x00, x, &data) == SOC_E_NONE) {
            printf("Address = 0x%04X\t\tData = 0x%08X\t\tVDL OVRIDE BYTE0 R DQS\n", x, data);
        } else {
            printf("Address = 0x%04X\t\tERROR            \t\tVDL OVRIDE BYTE0 R DQS\n", x);
    }
#ifdef CONFIG_IPROC_DDR_ECC
    if(wl != 2) {
#endif
        x = y + 0x02A8;
        if(DDR40_REG_READ(unit, ci, 0x00, x, &data) == SOC_E_NONE) {
                printf("Address = 0x%04X\t\tData = 0x%08X\t\tVDL OVRIDE BYTE1 R DQS\n", x, data);
            } else {
                printf("Address = 0x%04X\t\tERROR            \t\tVDL OVRIDE BYTE1 R DQS\n", x);
        }
#ifdef CONFIG_IPROC_DDR_ECC
    }
#endif
    x = y + 0x0204;
    if(DDR40_REG_READ(unit, ci, 0x00, x, &data) == SOC_E_NONE) {
            printf("Address = 0x%04X\t\tData = 0x%08X\t\tVDL OVRIDE BYTE0 WR DQ\n", x, data);
        } else {
            printf("Address = 0x%04X\t\tERROR            \t\tVDL OVRIDE BYTE0 WR DQ\n", x);
    }
    x = y + 0x0210;
    if(DDR40_REG_READ(unit, ci, 0x00, x, &data) == SOC_E_NONE) {
            printf("Address = 0x%04X\t\tData = 0x%08X\t\tVDL OVRIDE BYTE0 BIT WR DQ\n", x, data);
        } else {
            printf("Address = 0x%04X\t\tERROR            \t\tVDL OVRIDE BYTE0 BIT WR DQ\n", x);
    }
#ifdef CONFIG_IPROC_DDR_ECC
    if(wl != 2) {
#endif
        x = y + 0x02A4;
        if(DDR40_REG_READ(unit, ci, 0x00, x, &data) == SOC_E_NONE) {
                printf("Address = 0x%04X\t\tData = 0x%08X\t\tVDL OVRIDE BYTE1 WR DQ\n", x, data);
            } else {
                printf("Address = 0x%04X\t\tERROR            \t\tVDL OVRIDE BYTE1 WR DQ\n", x);
        }
        x = y + 0x02B0;
        if(DDR40_REG_READ(unit, ci, 0x00, x, &data) == SOC_E_NONE) {
                printf("Address = 0x%04X\t\tData = 0x%08X\t\tVDL OVRIDE BYTE1 BIT WR DQ\n", x, data);
            } else {
                printf("Address = 0x%04X\t\tERROR            \t\tVDL OVRIDE BYTE1 BIT WR DQ\n", x);
        }
#ifdef CONFIG_IPROC_DDR_ECC
    }
#endif
}
#endif

int _soc_ddr40_pll_calc(uint32 freq,int *ndiv, int *mdiv) {

    uint32 m,n, diff, err, nval=0, mval=0;

    err = freq; /* Maximum error */

    for (n=120; n>=60; n--) {
        m = (n * 50 / freq); /* Round Down */
        diff = (50 * n / m) - freq;
        if (diff < err) {
            err = diff;
            nval = n;
            mval = m;
        }
        m++;                /* Round Up */
        diff = freq - (50 * n / m);
        if (diff < err) {
            err = diff;
            nval = n;
            mval = m;
        }
        if (err == 0) {
            break;
        }
    }

    if ((ndiv != NULL) && (mdiv != NULL)) {
        *ndiv = (int)nval;
        *mdiv = (int)mval;
    }
    printf("Freq=%d n=%d m=%d err=%d\n", freq, nval, mval, err);
    return err;
}

#if 0
int
soc_ddr40_phy_pll_ctl(int unit, int ci, uint32 freq, uint32 phyType, int stat)
{
    uint32 rval;
    sal_usecs_t to_val;
    soc_timeout_t to;
    uint32 lock0, lock1, lock2, lock3;
	uint32 pllfreq;

    /* Per Speed & Grade Parameters */
    int ndiv, mdiv, freq_loc;

    freq_loc = _soc_ddr40_phy_freqtoloc(freq);

    if (freq_loc < 0) {
        return SOC_E_INTERNAL; /* Unsupported Frequency */
    }

    if (SAL_BOOT_QUICKTURN) {
            to_val = 10000000;  /* 10 Sec */
    } else {
            to_val = 50000;     /* 50 mS */
    }
    
    ndiv = pfds[phyType][freq_loc].ndiv;
    mdiv = pfds[phyType][freq_loc].mdiv;

    pllfreq = soc_property_get(unit, spn_DDR3_PLL_MHZ, freq);

    if (pllfreq != freq) {
        _soc_ddr40_pll_calc(pllfreq, &ndiv, &mdiv);
    }

    if (!stat) {
        switch(phyType) {
            case DDR_PHYTYPE_RSVP:
                break;
            case DDR_PHYTYPE_NS:
                printf("A01. Reset DDR PLL\n");
                SOC_IF_ERROR_RETURN(READ_TOP_SOFT_RESET_REG_2r(unit,&rval));
                soc_reg_field_set(unit, TOP_SOFT_RESET_REG_2r, &rval,
                                            TOP_DDR3_PLL_RST_Lf, 0);
                soc_reg_field_set(unit, TOP_SOFT_RESET_REG_2r, &rval,
                                            TOP_DDR3_PLL_POST_RST_Lf, 0);
                SOC_IF_ERROR_RETURN(WRITE_TOP_SOFT_RESET_REG_2r(unit,rval));

                printf("A02. Set PLL div.(mdiv/ndiv:%d/%d)\n",mdiv,ndiv);
                SOC_IF_ERROR_RETURN(soc_reg_field32_modify(unit,DDR3_PLL_CTRL_REGISTER_3r,
                            REG_PORT_ANY, NDIV_INTf, ndiv));
                SOC_IF_ERROR_RETURN(soc_reg_field32_modify(unit,DDR3_PLL_CTRL_REGISTER_4r,
                            REG_PORT_ANY, CH0_MDIVf, mdiv));

                printf("A03. Unreset PLL (mdiv:%d ndiv:%d)\n",mdiv,ndiv);
                SOC_IF_ERROR_RETURN(READ_TOP_SOFT_RESET_REG_2r(unit,&rval));
                soc_reg_field_set(unit, TOP_SOFT_RESET_REG_2r, &rval,
                                            TOP_DDR3_PLL_RST_Lf, 1);
                SOC_IF_ERROR_RETURN(WRITE_TOP_SOFT_RESET_REG_2r(unit,rval));

                printf("A04. Wait for PLL lock\n");
                to_val = 50000; /* 50 mS */
                soc_timeout_init(&to, to_val, 0);
                do {
                    SOC_IF_ERROR_RETURN(READ_DDR3_PLL_STATUSr(unit,&rval));
                    if (soc_reg_field_get(unit, DDR3_PLL_STATUSr, rval, DDR3_PLL_LOCKf)) {
                        break;
                    }
                    if (soc_timeout_check(&to)) {
                        soc_cm_debug(DK_ERR, "Timed out waiting for DDR3 PLL to Lock\n");
                        return SOC_E_TIMEOUT;
                    }
                } while (TRUE);

                printf("A05. Unreset post PLL lock\n");
                SOC_IF_ERROR_RETURN(READ_TOP_SOFT_RESET_REG_2r(unit,&rval));
                soc_reg_field_set(unit, TOP_SOFT_RESET_REG_2r, &rval, TOP_DDR3_PLL_POST_RST_Lf, 1);
                SOC_IF_ERROR_RETURN(WRITE_TOP_SOFT_RESET_REG_2r(unit,rval));

                printf("A06. Wait for a while after lock\n");
                sal_usleep(2000);
                break;
            case DDR_PHYTYPE_ENG:
            /*    printf("A01. Reset DPRC\n"));
                SOC_IF_ERROR_RETURN(DRC_REG_READ(unit, DRCALL, DRC_SPARE_REGISTER_3r, &rval));
                soc_reg_field_set(unit, DRCBROADCAST_SPARE_REGISTER_3r, &rval,
                                                        DPRC_ALIGN_PHY_RSTNf, 0);
                soc_reg_field_set(unit, DRCBROADCAST_SPARE_REGISTER_3r, &rval,
                                                   CLK_DIV_RSTNf, 0);
                SOC_IF_ERROR_RETURN(DRC_REG_WRITE(unit, DRCALL, DRC_SPARE_REGISTER_3r, rval));

                printf("A02. Bypass PLL mode\n"));
                SOC_IF_ERROR_RETURN(DRC_REG_READ(unit, DRCALL, DRC_DPI_POWERr, &rval));
                soc_reg_field_set(unit, DRCBROADCAST_DPI_POWERr, &rval,
                                                        BYPASS_PLLf, 0);
                SOC_IF_ERROR_RETURN(DRC_REG_WRITE(unit, DRCALL, DRC_DPI_POWERr, rval));

                printf("A02. Set PLL div.(mdiv/ndiv:%d/%d)\n",mdiv,ndiv));
                SOC_IF_ERROR_RETURN(soc_reg_field32_modify(unit,DDR3_PLL_CTRL_REGISTER_3r,
                          REG_PORT_ANY, NDIV_INTf, ndiv));
                SOC_IF_ERROR_RETURN(soc_reg_field32_modify(unit,DDR3_PLL_CTRL_REGISTER_4r,
                          REG_PORT_ANY, CH0_MDIVf, mdiv));

                printf("A03. Enable clock dividers\n"));
                SOC_IF_ERROR_RETURN(DRC_REG_READ(unit, DRCALL, DRC_SPARE_REGISTER_3r, &rval));
                soc_reg_field_set(unit, DRCBROADCAST_SPARE_REGISTER_3r, &rval,
                                                   CLK_DIV_RSTNf, 1);
                SOC_IF_ERROR_RETURN(DRC_REG_WRITE(unit, DRCALL, DRC_SPARE_REGISTER_3r, rval));
				
				printf("A04. Wait for a while after clock divider enable\n"));
                sal_usleep(2000);

                printf("A05. Unreset DPRC\n"));
                SOC_IF_ERROR_RETURN(DRC_REG_READ(unit, DRCALL, DRC_SPARE_REGISTER_3r, &rval));
                soc_reg_field_set(unit, DRCBROADCAST_SPARE_REGISTER_3r, &rval,
                                                        DPRC_ALIGN_PHY_RSTNf, 1);
                SOC_IF_ERROR_RETURN(DRC_REG_WRITE(unit, DRCALL, DRC_SPARE_REGISTER_3r, rval));

                printf("A06. Wait for a while after clock divider enable\n"));
                sal_usleep(2000); */
                break;
            case DDR_PHYTYPE_AND:
                printf("A01. Reset DDR PLL\n");
                SOC_IF_ERROR_RETURN( READ_CX_DDR03_PLL_RESETr( unit, &rval ) );
                soc_reg_field_set(unit, CX_DDR03_PLL_RESETr, &rval, RESET_Nf, 0 );
                soc_reg_field_set(unit, CX_DDR03_PLL_RESETr, &rval, POST_RESET_Nf, 0 );
                SOC_IF_ERROR_RETURN( WRITE_CX_DDR03_PLL_RESETr( unit, rval ) );

                printf("A02. Set PLL div.(mdiv/ndiv:%d/%d)\n",mdiv,ndiv);
                SOC_IF_ERROR_RETURN( READ_CX_DDR03_PLL_CHANNEL_0r( unit, &rval ) );
                soc_reg_field_set(unit, CX_DDR03_PLL_CHANNEL_0r, &rval, MDIVf, mdiv );
                SOC_IF_ERROR_RETURN( WRITE_CX_DDR03_PLL_CHANNEL_0r( unit, rval ) );
                SOC_IF_ERROR_RETURN( READ_CX_DDR03_PLL_NDIV_INTEGERr( unit, &rval ) );
                soc_reg_field_set(unit, CX_DDR03_PLL_NDIV_INTEGERr, &rval, NDIV_INTf, ndiv );
                SOC_IF_ERROR_RETURN( WRITE_CX_DDR03_PLL_NDIV_INTEGERr( unit, rval ) );

                printf("A03. Unreset PLL (mdiv:%d ndiv:%d)\n",mdiv,ndiv);
                SOC_IF_ERROR_RETURN( READ_CX_DDR03_PLL_RESETr( unit, &rval ) );
                soc_reg_field_set(unit, CX_DDR03_PLL_RESETr, &rval, RESET_Nf, 1 );
                SOC_IF_ERROR_RETURN( WRITE_CX_DDR03_PLL_RESETr( unit, rval ) );

                printf("A04. Wait for PLL lock\n");
                soc_timeout_init(&to, to_val, 0);
                do {
                    SOC_IF_ERROR_RETURN( READ_CX_DDR0_PLL_STATUSr( unit, &rval ) );
                    lock0 = soc_reg_field_get( unit, CX_DDR0_PLL_STATUSr, rval, LOCKf );
                    SOC_IF_ERROR_RETURN( READ_CX_DDR1_PLL_STATUSr( unit, &rval ) );
                    lock1 = soc_reg_field_get( unit, CX_DDR1_PLL_STATUSr, rval, LOCKf );
                    SOC_IF_ERROR_RETURN( READ_CX_DDR2_PLL_STATUSr( unit, &rval ) );
                    lock2 = soc_reg_field_get( unit, CX_DDR2_PLL_STATUSr, rval, LOCKf );
                    SOC_IF_ERROR_RETURN( READ_CX_DDR3_PLL_STATUSr( unit, &rval ) );
                    lock3 = soc_reg_field_get( unit, CX_DDR3_PLL_STATUSr, rval, LOCKf );
                    if ( lock0 && lock1 && lock2 && lock3 ) {
                        break;
                    }
                    if (soc_timeout_check(&to)) {
                        soc_cm_debug(DK_ERR, "Timed out waiting for DDR3 PLLs to Lock -- lock0[%d] lock1[%d] lock2[%d] lock3[%d]\n",
                                            lock0, lock1, lock2, lock3 );
                        return SOC_E_TIMEOUT;
                    }
                } while ( !SAL_BOOT_QUICKTURN );

                printf("A05. Unreset post PLL lock\n") );
                SOC_IF_ERROR_RETURN( READ_CX_DDR03_PLL_RESETr( unit, &rval ) );
                soc_reg_field_set(unit, CX_DDR03_PLL_RESETr, &rval, POST_RESET_Nf, 1 );
                SOC_IF_ERROR_RETURN( WRITE_CX_DDR03_PLL_RESETr( unit, rval ) );

                printf("A06. Wait for a while after lock\n");
                sal_usleep(2000);
                break;
            case DDR_PHYTYPE_CE:
                break;
            default:
                soc_cm_debug(DK_ERR, "Error. Unsupported PHY type\n");
                return SOC_E_FAIL;
        }
    } else {
    /* report only */
        switch(phyType) {
            case DDR_PHYTYPE_RSVP:
                break;
            case DDR_PHYTYPE_NS:
                SOC_IF_ERROR_RETURN(READ_TOP_SOFT_RESET_REG_2r(unit,&rval));
                soc_cm_print ("TOP_SOFT_RESET_REG:%x\n", rval);
                SOC_IF_ERROR_RETURN(READ_DDR3_PLL_CTRL_REGISTER_3r(unit,&rval));
                soc_cm_print ("DDR3_PLL_CTRL_REGISTER_3r:%x\n", rval);
                SOC_IF_ERROR_RETURN(READ_DDR3_PLL_CTRL_REGISTER_4r(unit,&rval));
                soc_cm_print ("DDR3_PLL_CTRL_REGISTER_4r:%x\n", rval);
                break;
            case DDR_PHYTYPE_ENG:
                break;
            case DDR_PHYTYPE_AND:
                break;
            case DDR_PHYTYPE_CE:
                break;
            default:
                soc_cm_debug(DK_ERR, "Error. Unsupported PHY type\n");
                return SOC_E_FAIL;
        }
    }

    return SOC_E_NONE;
}

#endif
/***********************************************************************
* Functions to  set/report the PVT params
***********************************************************************/
int
_soc_ddr40_phy_PVT_ctl(int unit, int ci, uint32 phyType, int stat)
{
    /* WORKS ON ONLY ONE CI; DOES NOT ITERATE OVER ALL CIs. */
    /* ONLY EVEN CIs SHOULD BE PASSED */
    
    uint32 rval;
    sal_usecs_t to_val;
    soc_timeout_t to;
    
    /* uint32 mhz; */
    int ciC;
/*    uint32 data; */
    
    if (!stat) {
        switch(phyType) {
            case DDR_PHYTYPE_RSVP:
                break;
            case DDR_PHYTYPE_NS:
            case DDR_PHYTYPE_ENG:
            case DDR_PHYTYPE_AND:
                if (SAL_BOOT_QUICKTURN) {
                    to_val = 10000000;  /* 10 Sec */
                } else {
                    to_val = 50000;     /* 50 mS */
                }
                
                ciC = ci & 0xFFFFFFFE;

                printf("E01. Reset Vref before Shmoo\n");
                rval = 0x820;
                SOC_IF_ERROR_RETURN(MODIFY_DDR40_PHY_CONTROL_REGS_VREF_DAC_CONTROLr(unit, ciC, rval, 0xFFF));
                
                printf("D04. Calibrate ZQ (ddr40_phy_calib_zq) before Shmoo\n");
                
                SOC_IF_ERROR_RETURN(READ_DDR40_PHY_CONTROL_REGS_ZQ_PVT_COMP_CTLr(unit,ciC,&rval));
                DDR40_SET_FIELD(rval,DDR40_PHY_CONTROL_REGS,ZQ_PVT_COMP_CTL,SAMPLE_EN,0);
                SOC_IF_ERROR_RETURN(WRITE_DDR40_PHY_CONTROL_REGS_ZQ_PVT_COMP_CTLr(unit,ciC,rval));

                SOC_IF_ERROR_RETURN(READ_DDR40_PHY_CONTROL_REGS_ZQ_PVT_COMP_CTLr(unit,ciC,&rval));
                DDR40_SET_FIELD(rval,DDR40_PHY_CONTROL_REGS,ZQ_PVT_COMP_CTL,SAMPLE_EN,1);
                SOC_IF_ERROR_RETURN(WRITE_DDR40_PHY_CONTROL_REGS_ZQ_PVT_COMP_CTLr(unit,ciC,rval));

                soc_timeout_init(&to, to_val, 0);
                do {
                    SOC_IF_ERROR_RETURN(READ_DDR40_PHY_CONTROL_REGS_ZQ_PVT_COMP_CTLr(unit,ciC,&rval));
                    if (DDR40_GET_FIELD(rval,  DDR40_PHY_CONTROL_REGS, ZQ_PVT_COMP_CTL, SAMPLE_DONE)) {
                        break;
                    }
                    if (soc_timeout_check(&to)) {
                        soc_cm_debug(DK_ERR, "CI%d: Timed out waiting for ZQ Calibration\n", ciC);
                        return SOC_E_TIMEOUT;
                    }
                } while (TRUE);

            /*    SOC_IF_ERROR_RETURN(READ_CI_PHY_CONTROLr(unit,0,&data));
                mhz = soc_reg_field_get(unit, CI_PHY_CONTROLr, data, DDR_MHZf);
                mhz = SOC_DDR3_CLOCK_MHZ(unit);
            */
            
                if (!SAL_BOOT_QUICKTURN) {
                    uint32 rd_en_byte_mode=0, rd_en_byte_vdl_steps=0, rd_en_bit_vdl_offset=0;

                    printf("D07. VDL Calibration before Shmoo\n");
                    rval = 0;
                    SOC_IF_ERROR_RETURN(WRITE_DDR40_PHY_CONTROL_REGS_VDL_CALIBRATEr(unit,ciC,rval));

                    rval = 0;
                    DDR40_SET_FIELD(rval,DDR40_PHY_CONTROL_REGS,VDL_CALIBRATE,CALIB_FAST,1);
                    DDR40_SET_FIELD(rval,DDR40_PHY_CONTROL_REGS,VDL_CALIBRATE,CALIB_ONCE,1);
                    DDR40_SET_FIELD(rval,DDR40_PHY_CONTROL_REGS,VDL_CALIBRATE,CALIB_AUTO,1);
                    SOC_IF_ERROR_RETURN(WRITE_DDR40_PHY_CONTROL_REGS_VDL_CALIBRATEr(unit,ciC,rval));
                    soc_timeout_init(&to, to_val, 0);
                    do {
                        SOC_IF_ERROR_RETURN(READ_DDR40_PHY_CONTROL_REGS_VDL_CALIB_STATUSr(unit,ciC,&rval));
                        if (DDR40_GET_FIELD(rval,  DDR40_PHY_CONTROL_REGS, VDL_CALIB_STATUS, CALIB_IDLE)) {
                            break;
                        }
                        if (soc_timeout_check(&to)) {
                            soc_cm_debug(DK_ERR, "CI%d: Timed out waiting for VDL Calibration Idle\n", ciC);
                            return SOC_E_TIMEOUT;
                        }
                    } while (TRUE);
                    SOC_IF_ERROR_RETURN(READ_DDR40_PHY_CONTROL_REGS_VDL_CALIB_STATUSr(unit,ciC,&rval));
                   // if (soc_property_get(unit, spn_DIAG_EMULATOR_PARTIAL_INIT, 0x0) == 0) {
                        if (0 == DDR40_GET_FIELD(rval,  DDR40_PHY_CONTROL_REGS, VDL_CALIB_STATUS, CALIB_LOCK)) {
                            soc_cm_debug(DK_ERR, "CI%d: VDL Calibration Did Not Lock reg=%x \n", ciC, rval);
                            return SOC_E_FAIL;
                        }
                    //}

                    printf("VDL calibration result: 0x%0x (cal_steps = %d)\n", rval, DDR40_GET_FIELD(rval,  DDR40_PHY_CONTROL_REGS, VDL_CALIB_STATUS, CALIB_TOTAL) >> 4);

                    /* clear VDL calib control */
                    SOC_IF_ERROR_RETURN(WRITE_DDR40_PHY_CONTROL_REGS_VDL_CALIBRATEr(unit,ciC,0));

                    SOC_IF_ERROR_RETURN(READ_DDR40_PHY_CONTROL_REGS_VDL_RD_EN_CALIB_STATUSr(unit,ciC,&rval));

                    rd_en_byte_mode = DDR40_GET_FIELD(rval,  DDR40_PHY_CONTROL_REGS, VDL_RD_EN_CALIB_STATUS, RD_EN_CALIB_BYTE_SEL);
                    rd_en_byte_vdl_steps = DDR40_GET_FIELD(rval,  DDR40_PHY_CONTROL_REGS, VDL_RD_EN_CALIB_STATUS, RD_EN_CALIB_TOTAL) >> 4;
                    rd_en_bit_vdl_offset = DDR40_GET_FIELD(rval,  DDR40_PHY_CONTROL_REGS, VDL_RD_EN_CALIB_STATUS, RD_EN_CALIB_BIT_OFFSET);
#if 0
					COMPILER_REFERENCE(rd_en_byte_mode);
                    COMPILER_REFERENCE(rd_en_byte_vdl_steps);
                    COMPILER_REFERENCE(rd_en_bit_vdl_offset);
#endif
                    soc_timeout_init(&to, to_val, 0);
                    do {
                        SOC_IF_ERROR_RETURN(READ_DDR40_PHY_CONTROL_REGS_VDL_CALIB_STATUSr(unit,ciC,&rval));
                        if (DDR40_GET_FIELD(rval,  DDR40_PHY_CONTROL_REGS, VDL_CALIB_STATUS, CALIB_IDLE)) {
                            break;
                        }
                        if (soc_timeout_check(&to)) {
                            soc_cm_debug(DK_ERR, "CI%d: Timed out waiting for VDL Calibration Idle(1)\n", ciC);
                            return SOC_E_TIMEOUT;
                        }
                    } while (TRUE);
                    SOC_IF_ERROR_RETURN(READ_DDR40_PHY_CONTROL_REGS_VDL_CALIB_STATUSr(unit,ciC,&rval));
                    ///if (soc_property_get(unit, spn_DIAG_EMULATOR_PARTIAL_INIT, 0x0) == 0) {
                        if (0 == DDR40_GET_FIELD(rval,  DDR40_PHY_CONTROL_REGS, VDL_CALIB_STATUS, CALIB_LOCK)) {
                            soc_cm_debug(DK_ERR, "CI%d: VDL Calibration Did Not Lock(1)\n", ciC);
                            return SOC_E_FAIL;
                        } 
                   // }
                } else {
                    printf("D07. VDL Calibration SKIPPED before Shmoo\n");
                }/* Not QUICKTURN */
                break;
            case DDR_PHYTYPE_CE:
                break;
            default:
                soc_cm_debug(DK_ERR, "Error. Unsupported PHY type\n");
                return SOC_E_FAIL;
        }
    } else {
    /* report only */
        switch(phyType) {
            case DDR_PHYTYPE_RSVP:
                break;
            case DDR_PHYTYPE_NS:
                break;
            case DDR_PHYTYPE_ENG:
                break;
            case DDR_PHYTYPE_AND:
                break;
            case DDR_PHYTYPE_CE:
                break;
            default:
                soc_cm_debug(DK_ERR, "Error. Unsupported PHY type\n");
                return SOC_E_FAIL;
        }
    }
    return SOC_E_NONE;
}

/***********************************************************************
* Functions to  Calibrate the PHY
***********************************************************************/
int
soc_ddr40_phy_calibrate(int unit, int ci, uint32 phyType, int stat)
{
    uint32 rval;
/*	uint64 rval64; */
    soc_timeout_t to;
    sal_usecs_t to_val;
    /* int mhz;  CI_PHY_CONTROL */
    int ciC;
/*    uint32 data; */
#if 0
    int freq_loc, grade_loc;
    
    //freq_loc = _soc_ddr40_phy_freqtoloc(SOC_DDR3_CLOCK_MHZ(unit));
    //grade_loc = _soc_ddr40_mem_gradetoloc(SOC_DDR3_MEM_GRADE(unit));

    if (freq_loc < 0) {
        return SOC_E_INTERNAL; /* Unsupported Frequency */
    }
#endif
    if (SAL_BOOT_QUICKTURN) {
            to_val = 10000000;  /* 10 Sec */
    } else {
            to_val = 50000;     /* 50 mS */
    }

    if (!stat) {
        switch(phyType) {
            case DDR_PHYTYPE_RSVP:
                break;
            case DDR_PHYTYPE_ENG:
            /*    _soc_ddr40_arad_phy_init_mem_set_wrapper(unit, &ENGfgmss, freq_loc, grade_loc); */
                printf("C01. Check Power Up Reset_Bar\n");
                for (ciC = 0; ciC < MAX_SHMOO_INTERFACES; ciC+=2) { /* Only Even CIs */
                    if(!_check_dram_either(ciC)) {
                        continue;
                    }
                    soc_timeout_init(&to, to_val, 0);
                }
                
                printf("C02. Config and Release PLL from reset\n");
                for (ciC = 0; ciC < MAX_SHMOO_INTERFACES; ciC+=2) { /* Only Even CIs */
                    if(!_check_dram_either(ciC)) {
                        continue;
                    }
#if 1
                    /* Divider */
                    SOC_IF_ERROR_RETURN(READ_DDR40_PHY_CONTROL_REGS_PLL_DIVIDERSr(unit,ciC,&rval));
                    DDR40_SET_FIELD(rval,DDR40_PHY_CONTROL_REGS,PLL_DIVIDERS,NDIV,16); //Srini
                    DDR40_SET_FIELD(rval,DDR40_PHY_CONTROL_REGS,PLL_DIVIDERS,POST_DIV,1);
                    SOC_IF_ERROR_RETURN(WRITE_DDR40_PHY_CONTROL_REGS_PLL_DIVIDERSr(unit,ciC,rval));
#endif     
					/* Reset */
                    SOC_IF_ERROR_RETURN(READ_DDR40_PHY_CONTROL_REGS_PLL_CONFIGr(unit,ciC,&rval));
                    DDR40_SET_FIELD(rval,DDR40_PHY_CONTROL_REGS,PLL_CONFIG,RESET,0);
                    SOC_IF_ERROR_RETURN(WRITE_DDR40_PHY_CONTROL_REGS_PLL_CONFIGr(unit,ciC,rval));

                    SOC_IF_ERROR_RETURN(READ_DDR40_PHY_CONTROL_REGS_PLL_CONFIGr(unit,ciC,&rval));
                    DDR40_SET_FIELD(rval,DDR40_PHY_CONTROL_REGS,PLL_CONFIG,RESET,1);
                    SOC_IF_ERROR_RETURN(WRITE_DDR40_PHY_CONTROL_REGS_PLL_CONFIGr(unit,ciC,rval));
                    DDR40_SET_FIELD(rval,DDR40_PHY_CONTROL_REGS,PLL_CONFIG,RESET,0);
                    SOC_IF_ERROR_RETURN(WRITE_DDR40_PHY_CONTROL_REGS_PLL_CONFIGr(unit,ciC,rval));
                }
                
                printf("C03. Poll PLL Lock\n");
                for (ciC = 0; ciC < MAX_SHMOO_INTERFACES; ciC+=2) { /* Only Even CIs */
                    if(!_check_dram_either(ciC)) {
                        continue;
                    }
                    soc_timeout_init(&to, to_val, 0);
                    do {
                        SOC_IF_ERROR_RETURN(READ_DDR40_PHY_CONTROL_REGS_PLL_STATUSr(unit,ciC,&rval));
                        if (DDR40_GET_FIELD(rval,  DDR40_PHY_CONTROL_REGS, PLL_STATUS, LOCK)) {
                            break;
                        }
                        if (soc_timeout_check(&to)) {
                            soc_cm_debug(DK_ERR, "CI%d: Timed out waiting for DDR PHY PLL to Lock\n", ciC);
                            return SOC_E_TIMEOUT;
                        }
                    } while (TRUE);
                }
                
                printf("C04. Calibrate ZQ (ddr40_phy_calib_zq)\n");
                for (ciC = 0; ciC < MAX_SHMOO_INTERFACES; ciC+=2) { /* Only Even CIs */
                    if(!_check_dram_either(ciC)) {
                        continue;
                    }
                    SOC_IF_ERROR_RETURN(READ_DDR40_PHY_CONTROL_REGS_ZQ_PVT_COMP_CTLr(unit,ciC,&rval));
                    DDR40_SET_FIELD(rval,DDR40_PHY_CONTROL_REGS,ZQ_PVT_COMP_CTL,SAMPLE_EN,0);
                    SOC_IF_ERROR_RETURN(WRITE_DDR40_PHY_CONTROL_REGS_ZQ_PVT_COMP_CTLr(unit,ciC,rval));

                    SOC_IF_ERROR_RETURN(READ_DDR40_PHY_CONTROL_REGS_ZQ_PVT_COMP_CTLr(unit,ciC,&rval));
                    DDR40_SET_FIELD(rval,DDR40_PHY_CONTROL_REGS,ZQ_PVT_COMP_CTL,SAMPLE_EN,1);
                    SOC_IF_ERROR_RETURN(WRITE_DDR40_PHY_CONTROL_REGS_ZQ_PVT_COMP_CTLr(unit,ciC,rval));
                }
                for (ciC = 0; ciC < MAX_SHMOO_INTERFACES; ciC+=2) { /* Only Even CIs */
                    if(!_check_dram_either(ciC)) {
                        continue;
                    }
                    soc_timeout_init(&to, to_val, 0);
                    do {
                        SOC_IF_ERROR_RETURN(READ_DDR40_PHY_CONTROL_REGS_ZQ_PVT_COMP_CTLr(unit,ciC,&rval));
                        if (DDR40_GET_FIELD(rval,  DDR40_PHY_CONTROL_REGS, ZQ_PVT_COMP_CTL, SAMPLE_DONE)) {
                            break;
                        }
                        if (soc_timeout_check(&to)) {
                            soc_cm_debug(DK_ERR, "CI%d: Timed out waiting for ZQ Calibration\n", ciC);
                            return SOC_E_TIMEOUT;
                        }
                    } while (TRUE);
                }
                
                printf(
                    "C05. DDR PHY VTT On (Virtual VTT setup) DISABLE all Virtual VTT\n");
                for (ciC = 0; ciC < MAX_SHMOO_INTERFACES; ciC+=2) { /* Only Even CIs */
                    if(!_check_dram_either(ciC)) {
                        continue;
                    }
                    rval = 0x3f1ffff;  /* all bits except RESET and CKE */
                    SOC_IF_ERROR_RETURN(WRITE_DDR40_PHY_CONTROL_REGS_VIRTUAL_VTT_CONNECTIONSr(unit,ciC,rval));

                    rval = 0; /* AGUTMANN disable VTT override , previously 0x1ffff; */    
                    SOC_IF_ERROR_RETURN(WRITE_DDR40_PHY_CONTROL_REGS_VIRTUAL_VTT_OVERRIDEr(unit,ciC,rval));

                }

                printf("C06. DDR40_PHY_DDR3_MISC\n");
                for (ciC = 0; ciC < MAX_SHMOO_INTERFACES; ciC+=2) { /* Only Even CIs */
                    if(!_check_dram_either(ciC)) {
                        continue;
                    }
                    rval = 0;
                    DDR40_SET_FIELD(rval,DDR40_PHY_CONTROL_REGS,DRIVE_PAD_CTL,VDDO_VOLTS,1);
                    DDR40_SET_FIELD(rval,DDR40_PHY_CONTROL_REGS,DRIVE_PAD_CTL,RT120B_G,1);
                    DDR40_SET_FIELD(rval,DDR40_PHY_CONTROL_REGS,DRIVE_PAD_CTL,RT60B,1);
                    SOC_IF_ERROR_RETURN(WRITE_DDR40_PHY_CONTROL_REGS_DRIVE_PAD_CTLr(unit,ciC,rval));

                    rval = 0;
                    DDR40_SET_FIELD(rval,DDR40_PHY_WORD_LANE_0,DRIVE_PAD_CTL,VDDO_VOLTS,1);
                    DDR40_SET_FIELD(rval,DDR40_PHY_WORD_LANE_0,DRIVE_PAD_CTL,RT120B_G,1);
					DDR40_SET_FIELD(rval,DDR40_PHY_WORD_LANE_0,DRIVE_PAD_CTL,NO_DQS_RD,0);
                    SOC_IF_ERROR_RETURN(WRITE_DDR40_PHY_WORD_LANE_0_DRIVE_PAD_CTLr(unit,ciC,rval));
                    SOC_IF_ERROR_RETURN(WRITE_DDR40_PHY_WORD_LANE_1_DRIVE_PAD_CTLr(unit,ciC,rval));
#ifdef CONFIG_IPROC_DDR_ECC
                    SOC_IF_ERROR_RETURN(WRITE_DDR40_PHY_ECC_LANE_DRIVE_PAD_CTLr(unit,ciC,rval));
#endif
                }
                
            /*    SOC_IF_ERROR_RETURN(READ_CI_PHY_CONTROLr(unit,0,&data));
                mhz = soc_reg_field_get(unit, CI_PHY_CONTROLr, data, DDR_MHZf);
                mhz = SOC_DDR3_CLOCK_MHZ(unit);
            */
            
                if (!SAL_BOOT_QUICKTURN) {
                  uint32 rd_en_byte_mode=0, rd_en_byte_vdl_steps=0, rd_en_bit_vdl_offset=0;

                    printf("C07. VDL Calibration\n");
                    for (ciC = 0; ciC < MAX_SHMOO_INTERFACES; ciC+=2) { /* Only Even CIs */
                        if(!_check_dram_either(ciC)) {
                            continue;
                        }
                        rval = 0;
                        SOC_IF_ERROR_RETURN(WRITE_DDR40_PHY_CONTROL_REGS_VDL_CALIBRATEr(unit,ciC,rval));

                        rval = 0;
                        DDR40_SET_FIELD(rval,DDR40_PHY_CONTROL_REGS,VDL_CALIBRATE,CALIB_FAST,1);
                        DDR40_SET_FIELD(rval,DDR40_PHY_CONTROL_REGS,VDL_CALIBRATE,CALIB_ONCE,1);
                        DDR40_SET_FIELD(rval,DDR40_PHY_CONTROL_REGS,VDL_CALIBRATE,CALIB_AUTO,1);
                        SOC_IF_ERROR_RETURN(WRITE_DDR40_PHY_CONTROL_REGS_VDL_CALIBRATEr(unit,ciC,rval));
                    }
                    for (ciC = 0; ciC < MAX_SHMOO_INTERFACES; ciC+=2) { /* Only Even CIs */
                        if(!_check_dram_either(ciC)) {
                            continue;
                        }
                        soc_timeout_init(&to, to_val, 0);
                        do {
							printf("C07.1\n");
                            SOC_IF_ERROR_RETURN(READ_DDR40_PHY_CONTROL_REGS_VDL_CALIB_STATUSr(unit,ciC,&rval));
                            if (DDR40_GET_FIELD(rval,  DDR40_PHY_CONTROL_REGS, VDL_CALIB_STATUS, CALIB_IDLE)) {
								printf("C07.2\n");
                                break;
                            }
															printf("C07.3\n");
                            if (soc_timeout_check(&to)) {
                                soc_cm_debug(DK_ERR, "CI%d: Timed out waiting for VDL Calibration Idle\n", ciC);
                                return SOC_E_TIMEOUT;
                            }
                        } while (TRUE);
						printf("C07.4\n");
                        SOC_IF_ERROR_RETURN(READ_DDR40_PHY_CONTROL_REGS_VDL_CALIB_STATUSr(unit,ciC,&rval));
						printf("C07.4.1\n");
                        //if (soc_property_get(unit, spn_DIAG_EMULATOR_PARTIAL_INIT, 0x0) == 0) {
                            if (0 == DDR40_GET_FIELD(rval,  DDR40_PHY_CONTROL_REGS, VDL_CALIB_STATUS, CALIB_LOCK)) {
								printf("C07.4.2\n");
                                soc_cm_debug(DK_ERR, "CI%d: VDL Calibration Did Not Lock reg=%x \n", ciC, rval);
								printf("C07.4.3\n");
                                return SOC_E_FAIL;
                            } 
                        //}
							printf("C07.4.4\n");

                        //printf("VDL calibration result: 0x%0x (cal_steps = %d)\n", \
                         //          rval, DDR40_GET_FIELD(rval,  DDR40_PHY_CONTROL_REGS, VDL_CALIB_STATUS, CALIB_TOTAL) >> 4));
						//while(1); // Srini
                        printf("VDL calibration result: 0x%0x (cal_steps = %d)\n", rval, DDR40_GET_FIELD(rval,  DDR40_PHY_CONTROL_REGS, VDL_CALIB_STATUS, CALIB_TOTAL) >> 4);

						printf("C07.4.5\n");

                        /* clear VDL calib control */
                        SOC_IF_ERROR_RETURN(WRITE_DDR40_PHY_CONTROL_REGS_VDL_CALIBRATEr(unit,ciC,0));
						printf("C07.4.6\n");

                    }
					printf("C07.5\n");
                    for (ciC = 0; ciC < MAX_SHMOO_INTERFACES; ciC+=2) { /* Only Even CIs */
                        if(!_check_dram_either(ciC)) {
                            continue;
                        }

                        SOC_IF_ERROR_RETURN(READ_DDR40_PHY_CONTROL_REGS_VDL_RD_EN_CALIB_STATUSr(unit,ciC,&rval));
                        rd_en_byte_mode = DDR40_GET_FIELD(rval,  DDR40_PHY_CONTROL_REGS, VDL_RD_EN_CALIB_STATUS, RD_EN_CALIB_BYTE_SEL);
                        rd_en_byte_vdl_steps = DDR40_GET_FIELD(rval,  DDR40_PHY_CONTROL_REGS, VDL_RD_EN_CALIB_STATUS, RD_EN_CALIB_TOTAL) >> 4;
                        rd_en_bit_vdl_offset = DDR40_GET_FIELD(rval,  DDR40_PHY_CONTROL_REGS, VDL_RD_EN_CALIB_STATUS, RD_EN_CALIB_BIT_OFFSET);
                        //COMPILER_REFERENCE(rd_en_byte_mode);
                        //COMPILER_REFERENCE(rd_en_byte_vdl_steps);
                        //COMPILER_REFERENCE(rd_en_bit_vdl_offset);

                        /* rval = 0;
                           SOC_IF_ERROR_RETURN(WRITE_DDR40_PHY_CONTROL_REGS_VDL_CALIBRATEr(unit,ci,rval));
                           DDR40_SET_FIELD(rval,DDR40_PHY_CONTROL_REGS,VDL_CALIBRATE,CALIB_STEPS,1);
                           SOC_IF_ERROR_RETURN(WRITE_DDR40_PHY_CONTROL_REGS_VDL_CALIBRATEr(unit,ci,rval));
                        */
                    }
                    for (ciC = 0; ciC < MAX_SHMOO_INTERFACES; ciC+=2) { /* Only Even CIs */
                        if(!_check_dram_either(ciC)) {
                            continue;
                        }
                        soc_timeout_init(&to, to_val, 0);
                        do {
                            SOC_IF_ERROR_RETURN(READ_DDR40_PHY_CONTROL_REGS_VDL_CALIB_STATUSr(unit,ciC,&rval));
                            if (DDR40_GET_FIELD(rval,  DDR40_PHY_CONTROL_REGS, VDL_CALIB_STATUS, CALIB_IDLE)) {
                                break;
                            }
                            if (soc_timeout_check(&to)) {
                                soc_cm_debug(DK_ERR, "CI%d: Timed out waiting for VDL Calibration Idle(1)\n", ciC);
                                return SOC_E_TIMEOUT;
                            }
                        } while (TRUE);
                        SOC_IF_ERROR_RETURN(READ_DDR40_PHY_CONTROL_REGS_VDL_CALIB_STATUSr(unit,ciC,&rval));
                        //if (soc_property_get(unit, spn_DIAG_EMULATOR_PARTIAL_INIT, 0x0) == 0) {
                            if (0 == DDR40_GET_FIELD(rval,  DDR40_PHY_CONTROL_REGS, VDL_CALIB_STATUS, CALIB_LOCK)) {
                                soc_cm_debug(DK_ERR, "CI%d: VDL Calibration Did Not Lock(1)\n", ciC);
                                return SOC_E_FAIL;
                            } 
                        //}
                    }
                } else {
                    printf("C07. VDL Calibration SKIPPED\n");
                }/* Not QUICKTURN */

                printf(
                    "C08. DDR40_PHY_DDR3_MISC : Start DDR40_PHY_RDLY_ODT....\n");
                for (ciC = 0; ciC < MAX_SHMOO_INTERFACES; ciC+=2) { /* Only Even CIs */
                    if(!_check_dram_either(ciC)) {
                        continue;
                    }
                    rval = 0;
                    #if 0 /* def BCM_ARAD_SUPPORT */
                    DDR40_SET_FIELD(rval, DDR40_PHY_WORD_LANE_0, READ_DATA_DLY, RD_DATA_DLY, arad_dram_read_data_dly_get(unit, SOC_DDR3_CLOCK_MHZ(unit)));
                    #else
                    DDR40_SET_FIELD(rval, DDR40_PHY_WORD_LANE_0, READ_DATA_DLY, RD_DATA_DLY, 1);
                    #endif
                    SOC_IF_ERROR_RETURN(WRITE_DDR40_PHY_WORD_LANE_0_READ_DATA_DLYr(unit, ciC, rval));
                    SOC_IF_ERROR_RETURN(WRITE_DDR40_PHY_WORD_LANE_1_READ_DATA_DLYr(unit, ciC, rval));
#ifdef CONFIG_IPROC_DDR_ECC
                    SOC_IF_ERROR_RETURN(WRITE_DDR40_PHY_ECC_LANE_READ_DATA_DLYr(unit, ciC, rval));
#endif

                    rval = 0;
                    DDR40_SET_FIELD(rval,DDR40_PHY_WORD_LANE_0,READ_CONTROL,DQ_ODT_ENABLE,1);
                    DDR40_SET_FIELD(rval,DDR40_PHY_WORD_LANE_0,READ_CONTROL,DQ_ODT_LE_ADJ,1);
                    DDR40_SET_FIELD(rval,DDR40_PHY_WORD_LANE_0,READ_CONTROL,DQ_ODT_TE_ADJ,1);
                    SOC_IF_ERROR_RETURN(WRITE_DDR40_PHY_WORD_LANE_0_READ_CONTROLr(unit,ciC,rval));
                    SOC_IF_ERROR_RETURN(WRITE_DDR40_PHY_WORD_LANE_1_READ_CONTROLr(unit,ciC,rval));
#ifdef CONFIG_IPROC_DDR_ECC
                    SOC_IF_ERROR_RETURN(WRITE_DDR40_PHY_ECC_LANE_READ_CONTROLr(unit,ciC,rval));
#endif

                    rval = 3;
                    SOC_IF_ERROR_RETURN(WRITE_DDR40_PHY_WORD_LANE_0_WR_PREAMBLE_MODEr(unit,ciC,rval));
                    SOC_IF_ERROR_RETURN(WRITE_DDR40_PHY_WORD_LANE_1_WR_PREAMBLE_MODEr(unit,ciC,rval));
#ifdef CONFIG_IPROC_DDR_ECC
                    SOC_IF_ERROR_RETURN(WRITE_DDR40_PHY_ECC_LANE_WR_PREAMBLE_MODEr(unit,ciC,rval));
#endif
                }

                printf("C09. Start ddr40_phy_autoidle_on (MEM_SYS_PARAM_PHY_AUTO_IDLE) ....\n");
                for (ciC = 0; ciC < MAX_SHMOO_INTERFACES; ciC+=2) { /* Only Even CIs */
                    if(!_check_dram_either(ciC)) {
                        continue;
                    }
                    rval = 0;
                    DDR40_SET_FIELD(rval,DDR40_PHY_WORD_LANE_0,IDLE_PAD_CONTROL,AUTO_DQ_IDDQ_MODE,1);
                    DDR40_SET_FIELD(rval,DDR40_PHY_WORD_LANE_0,IDLE_PAD_CONTROL,AUTO_DQ_RXENB_MODE,1);
                    SOC_IF_ERROR_RETURN(WRITE_DDR40_PHY_WORD_LANE_0_IDLE_PAD_CONTROLr(unit,ciC,rval));
                    SOC_IF_ERROR_RETURN(WRITE_DDR40_PHY_WORD_LANE_1_IDLE_PAD_CONTROLr(unit,ciC,rval));
#ifdef CONFIG_IPROC_DDR_ECC
                    SOC_IF_ERROR_RETURN(WRITE_DDR40_PHY_ECC_LANE_IDLE_PAD_CONTROLr(unit,ciC,rval));
#endif
                }

                printf("C10. Wait for Phy Ready\n");
#if 0
                for (ciC = 0; ciC < MAX_SHMOO_INTERFACES; ciC+=2) { /* Only Even CIs */
                    if(!_check_dram_either(ciC)) {
                        continue;
                    }
                    soc_timeout_init(&to, to_val, 0);
                    do {
                        SOC_IF_ERROR_RETURN(DRC_REG_READ(unit, drc_channel_set[ciC>>1], DRC_DPI_STATUSr, &rval));
                        if (soc_reg_field_get(unit, DRCA_DPI_STATUSr, rval, READYf)) {
                            break;
                        }
                        if (soc_timeout_check(&to)) {
                            soc_cm_debug(DK_ERR, "CI%d: Timed out waiting for PHY Ready\n", ciC);
                            return SOC_E_TIMEOUT;
                        }
                    } while (TRUE);
                }
#endif
				sal_usleep(50000); /* 50 ms */

                break;
            case DDR_PHYTYPE_CE:
                break;
            default:
                soc_cm_debug(DK_ERR, "Error. Unsupported PHY type\n");
                return SOC_E_FAIL;
        }
    } else {
        /* report only */
        switch(phyType) {
            case DDR_PHYTYPE_RSVP:
                break;
            case DDR_PHYTYPE_NS:
                break;
            case DDR_PHYTYPE_ENG:
                break;
            case DDR_PHYTYPE_AND:
                break;
            case DDR_PHYTYPE_CE:
                break;
            default:
                soc_cm_debug(DK_ERR, "Error. Unsupported PHY type\n");
                return SOC_E_FAIL;
        }
    }
    return SOC_E_NONE;
}

/***********************************************************************
* Functions to reset the controller
***********************************************************************/
int
soc_ddr40_ctlr_reset   (int unit, int ci, uint32 ctlType, int stat)
{
    return SOC_E_NONE;
}

/***********************************************************************
* Functions to set/report the Controller Parameters
***********************************************************************/
int
soc_ddr40_ctlr_ctl(int unit, int ci, uint32 ctlType, int stat)
{

	    return SOC_E_NONE;
}

#if 0
/***********************************************************************
* Functions to Read / Write / Modify DDR Memory
***********************************************************************/
int _DDRWrRdComplete(int unit, int ci, uint32 uTimeout)
{
    uint32 i;
    uint32 data = 0;
    uint32 uAck = 0;

    /* wait for the ACK to indicate rd/wr op is finished */
    for (i = 0; i < uTimeout; i++) {
        SOC_IF_ERROR_RETURN(READ_CI_MEM_ACC_CTRLr(unit, ci, &data));
        uAck = soc_reg_field_get(unit, CI_MEM_ACC_CTRLr, data, MEM_ACC_ACKf);
        if (uAck) {
            return SOC_E_NONE;
        }
        sal_udelay(10);
    }

    /* timed out */
    return (SOC_E_TIMEOUT);
}
#endif
int soc_ddr40_read(int unit, int ci, uint32 addr, uint32 * pData0,
                   uint32 * pData1, uint32 * pData2, uint32 * pData3,
                   uint32 * pData4, uint32 * pData5, uint32 * pData6,
                   uint32 * pData7)
{
	uint32 word[8], i, *pData;
	pData = (uint32 *)addr;
	/* Read 32 bits and use lower 16 bits */
	for(i = 0; i < 8; i ++) {
		word[i] = *pData & 0xFFFF;
		pData++;
		word[i] |= *pData << 16;
		pData++;
	}

	*pData0 = word[0];
	*pData1 = word[1];
	*pData2 = word[2];
	*pData3 = word[3];
	*pData4 = word[4];
	*pData5 = word[5];
	*pData6 = word[6];
	*pData7 = word[7];

    return SOC_E_NONE;
}

int soc_ddr40_write(int unit, int ci, uint32 addr, uint32 uData0,
                    uint32 uData1, uint32 uData2, uint32 uData3,
                    uint32 uData4, uint32 uData5, uint32 uData6, uint32 uData7)
{

	uint32 word[8], i;
	uint16 *pData;
	pData = (uint16 *)addr;

	word[0] = uData0;
	word[1] = uData1;
	word[2] = uData2;
	word[3] = uData3;
	word[4] = uData4;
	word[5] = uData5;
	word[6] = uData6;
	word[7] = uData7;

	/* Read 32 bits and use lower 16 bits */
	for(i = 0; i < 8; i ++) {
		*pData = (uint16) (word[i] & 0xFFFF);
		pData++;
		pData++;
		*pData = (uint16) (word[i] >> 16);
		pData++;
		pData++;
	}

	return SOC_E_NONE;
}

/***********************************************************************
* Functions to Read / Write / Modify DDR Phy Registers
***********************************************************************/
int
soc_ddr40_phy_reg_ci_read(int unit, int ci, uint32 reg_addr, uint32 * reg_data)
{
#if (defined(CONFIG_HELIX4) || defined(CONFIG_HURRICANE2) || defined(CONFIG_NS_PLUS) || defined(CONFIG_KATANA2))
	if(ci == 0){
		*reg_data = *(volatile uint32 *)(DDR_PHY_CONTROL_REGS_REVISION + reg_addr);
		return(SOC_E_NONE);
	}
	return(SOC_E_INTERNAL); 
#else
       return SOC_E_INTERNAL;     /* Unsupported configuration */
#endif
}

int
soc_ddr40_phy_reg_ci_write(int unit, int ci, uint32 reg_addr, uint32 reg_data)
{
#if (defined(CONFIG_HELIX4) || defined(CONFIG_HURRICANE2) || defined(CONFIG_NS_PLUS) || defined(CONFIG_KATANA2))
	if(ci == 0){
		*(volatile uint32 *)(reg_addr + DDR_PHY_CONTROL_REGS_REVISION)= reg_data;
		return(SOC_E_NONE);
	}
	return(SOC_E_INTERNAL); 
#else
       return SOC_E_INTERNAL;     /* Unsupported configuration */
#endif
}

int
soc_ddr40_phy_reg_ci_modify(int unit, uint32 ci, uint32 reg_addr, uint32 data,
                            uint32 mask)
{
	volatile uint32 * reg;
#if (defined(CONFIG_HELIX4) || defined(CONFIG_HURRICANE2) || defined(CONFIG_NS_PLUS) || defined(CONFIG_KATANA2))
	if(ci == 0){
		reg = (volatile uint32 *)(reg_addr + DDR_PHY_CONTROL_REGS_REVISION);
		*reg = (*reg & (~mask)) | (data & mask);
		return(SOC_E_NONE);
	}
	return(SOC_E_INTERNAL); 
#else
       return SOC_E_INTERNAL;     /* Unsupported configuration */
#endif
}


/***********************************************************************
* Shmoo Functions
***********************************************************************/

uint32 _get_random(void)
{
#if 0
    static uint32 m_w = 6483;    /* must not be zero */
    static uint32 m_z = 31245;    /* must not be zero */
    
    m_z = 36969 * (m_z & 65535) + (m_z >> 16);
    m_w = 18000 * (m_w & 65535) + (m_w >> 16);
    return (m_z << 16) + m_w;  /* 32-bit result */
#else
	return 0;
#endif
}

/* Generate NUM_DATA uint32 data for memory accesses */
static void _fill_in_data(uint32 * data)
{
    int32 i;

    for (i = 0; i < NUM_DATA; i++) {
        if (i & 0x1) {
            data[i] = 0xa5a5a500 | i;
        } else {
            data[i] = 0x5a5a5a00 | i;
        }
    }
    return;
}

STATIC void _test_mem_write(int unit, int ci, int addr, uint32 * uDataWR)
{
    if(SOC_IS_KATANA(unit) || SOC_IS_CALADAN3(unit)) {
        int32 i;
        for (i = 0; i < NUM_DATA / 8; i++) {
            soc_ddr40_write(unit, ci, addr + i, uDataWR[i * 8],
                        uDataWR[(i * 8) + 1], uDataWR[(i * 8) + 2],
                        uDataWR[(i * 8) + 3], uDataWR[(i * 8) + 4],
                        uDataWR[(i * 8) + 5], uDataWR[(i * 8) + 6],
                        uDataWR[(i * 8) + 7]);
        }
    }
}

STATIC uint32 _test_rd_data_fifo_status(int unit, int ci, int wl, int loop)
{
    int i;
    uint32 addr;
    uint32 uDataRD[NUM_DATA];
    uint32 rval = 0;
    uint32 result_fail = 1;

    if(!SOC_IS_KATANA(unit) && !SOC_IS_CALADAN3(unit)) {
        return result_fail;
    }

    addr = (_get_random() & 0xFFFFFF);
    for (i = 0; i < NUM_DATA / 8; i++) {
        soc_ddr40_read(unit, (ci + wl), addr + i, &uDataRD[(i * 8)],
                       &uDataRD[(i * 8) + 1], &uDataRD[(i * 8) + 2],
                       &uDataRD[(i * 8) + 3], &uDataRD[(i * 8) + 4],
                       &uDataRD[(i * 8) + 5], &uDataRD[(i * 8) + 6],
                       &uDataRD[(i * 8) + 7]);
    }
    rval = 0;
    if (wl == 0) {
        READ_DDR40_PHY_WORD_LANE_0_READ_FIFO_STATUSr(unit, ci, &rval);
    } else {
#ifdef CONFIG_IPROC_DDR_ECC
        if(wl == 2) {
            READ_DDR40_PHY_ECC_LANE_READ_FIFO_STATUSr(unit, ci, &rval);
        } else {
#endif
            READ_DDR40_PHY_WORD_LANE_1_READ_FIFO_STATUSr(unit, ci, &rval);
#ifdef CONFIG_IPROC_DDR_ECC
        }
#endif
    }

    result_fail = (rval>0) ? 0xffff : 0;

    return result_fail;
}

#ifdef BCM_ARAD_SUPPORT
uint32
_test_func_self_test_1_arad(int unit, int ci, int wl, int loop, uint32 flag)
{
    uint32 i;
    int j;
    uint32 rval = 0;
    uint32 result_fail = 0;
    uint32 wdata[8];
    uint32 rdata;
    int k;
    uint32 data, fifo_status0, fifo_status1;
    
    uint32 startAddr;
    
    uint32 efold = 0;

#ifdef CONFIG_IPROC_DDR_ECC
    iproc_clear_ecc_syndrome();
#endif
 
    j = 0;
    while (j < loop) {
        rval = 0;
        SOC_IF_ERROR_RETURN(DRC_REG_READ(unit, drc_channel_set[ci>>1], DRC_BIST_CONFIGURATIONSr, &rval));
        if(flag & 0x4) {
            soc_reg_field_set(unit, DRCA_BIST_CONFIGURATIONSr, &rval, WRITE_WEIGHTf, 0);
            soc_reg_field_set(unit, DRCA_BIST_CONFIGURATIONSr, &rval, READ_WEIGHTf, 255);
            soc_reg_field_set(unit, DRCA_BIST_CONFIGURATIONSr, &rval, PATTERN_BIT_MODEf, 0);
            soc_reg_field_set(unit, DRCA_BIST_CONFIGURATIONSr, &rval, PRBS_MODEf, 0);
            soc_reg_field_set(unit, DRCA_BIST_CONFIGURATIONSr, &rval, CONS_ADDR_8_BANKSf, 1);
            soc_reg_field_set(unit, DRCA_BIST_CONFIGURATIONSr, &rval, ADDRESS_SHIFT_MODEf, 0);
            soc_reg_field_set(unit, DRCA_BIST_CONFIGURATIONSr, &rval, DATA_SHIFT_MODEf, 0);
            soc_reg_field_set(unit, DRCA_BIST_CONFIGURATIONSr, &rval, DATA_ADDR_MODEf, 0);
        } else {
            soc_reg_field_set(unit, DRCA_BIST_CONFIGURATIONSr, &rval, WRITE_WEIGHTf, 255);
            soc_reg_field_set(unit, DRCA_BIST_CONFIGURATIONSr, &rval, READ_WEIGHTf, 255);
            soc_reg_field_set(unit, DRCA_BIST_CONFIGURATIONSr, &rval, PATTERN_BIT_MODEf, 0);
            soc_reg_field_set(unit, DRCA_BIST_CONFIGURATIONSr, &rval, PRBS_MODEf, 1);
            soc_reg_field_set(unit, DRCA_BIST_CONFIGURATIONSr, &rval, CONS_ADDR_8_BANKSf, 1);
            soc_reg_field_set(unit, DRCA_BIST_CONFIGURATIONSr, &rval, ADDRESS_SHIFT_MODEf, 0);
            soc_reg_field_set(unit, DRCA_BIST_CONFIGURATIONSr, &rval, DATA_SHIFT_MODEf, 0);
            soc_reg_field_set(unit, DRCA_BIST_CONFIGURATIONSr, &rval, DATA_ADDR_MODEf, 0);
        }

        SOC_IF_ERROR_RETURN(DRC_REG_WRITE(unit, drc_channel_set[ci>>1], DRC_BIST_CONFIGURATIONSr, rval));

        SOC_IF_ERROR_RETURN(DRC_REG_READ(unit, drc_channel_set[ci>>1], DRC_BIST_NUMBER_OF_ACTIONSr, &rval));
        soc_reg_field_set(unit, DRCA_BIST_NUMBER_OF_ACTIONSr, &rval, BIST_NUM_ACTIONSf, 510);
        SOC_IF_ERROR_RETURN(DRC_REG_WRITE(unit, drc_channel_set[ci>>1], DRC_BIST_NUMBER_OF_ACTIONSr, rval));

        /* startAddr = _get_random() & 0xffffff; */
        startAddr = 0x0;
        SOC_IF_ERROR_RETURN(DRC_REG_READ(unit, drc_channel_set[ci>>1], DRC_BIST_START_ADDRESSr, &rval));
        soc_reg_field_set(unit, DRCA_BIST_START_ADDRESSr, &rval, BIST_START_ADDRESSf, startAddr);
        SOC_IF_ERROR_RETURN(DRC_REG_WRITE(unit, drc_channel_set[ci>>1], DRC_BIST_START_ADDRESSr, rval));

        SOC_IF_ERROR_RETURN(DRC_REG_READ(unit, drc_channel_set[ci>>1], DRC_BIST_END_ADDRESSr, &rval));
        soc_reg_field_set(unit, DRCA_BIST_END_ADDRESSr, &rval, BIST_END_ADDRESSf, 0xFFFFFF);
        SOC_IF_ERROR_RETURN(DRC_REG_WRITE(unit, drc_channel_set[ci>>1], DRC_BIST_END_ADDRESSr, rval));

        if(flag & 0x4) {
            for(k = 0; k < 8; k+=2) {
                wdata[k] = 0x0;
                wdata[k+1] = 0xFFFFFFFF;
            }
        } else {
            for(k = 0; k < 8; k++) {
                wdata[k] = _get_random();
            }
        }
        
        SOC_IF_ERROR_RETURN
            (DRC_REG_WRITE(unit, drc_channel_set[ci>>1], DRC_BIST_PATTERN_WORD_0r, wdata[0]));
        SOC_IF_ERROR_RETURN
            (DRC_REG_WRITE(unit, drc_channel_set[ci>>1], DRC_BIST_PATTERN_WORD_1r, wdata[1]));
        SOC_IF_ERROR_RETURN
            (DRC_REG_WRITE(unit, drc_channel_set[ci>>1], DRC_BIST_PATTERN_WORD_2r, wdata[2]));
        SOC_IF_ERROR_RETURN
            (DRC_REG_WRITE(unit, drc_channel_set[ci>>1], DRC_BIST_PATTERN_WORD_3r, wdata[3]));
        SOC_IF_ERROR_RETURN
            (DRC_REG_WRITE(unit, drc_channel_set[ci>>1], DRC_BIST_PATTERN_WORD_4r, wdata[4]));
        SOC_IF_ERROR_RETURN
            (DRC_REG_WRITE(unit, drc_channel_set[ci>>1], DRC_BIST_PATTERN_WORD_5r, wdata[5]));
        SOC_IF_ERROR_RETURN
            (DRC_REG_WRITE(unit, drc_channel_set[ci>>1], DRC_BIST_PATTERN_WORD_6r, wdata[6]));
        SOC_IF_ERROR_RETURN
            (DRC_REG_WRITE(unit, drc_channel_set[ci>>1], DRC_BIST_PATTERN_WORD_7r, wdata[7]));

        SOC_IF_ERROR_RETURN(DRC_REG_READ(unit, drc_channel_set[ci>>1], DRC_BIST_CONFIGURATIONSr, &rval));
        soc_reg_field_set(unit, DRCA_BIST_CONFIGURATIONSr, &rval, IND_WR_RD_ADDR_MODEf, 1);
        SOC_IF_ERROR_RETURN(DRC_REG_WRITE(unit, drc_channel_set[ci>>1], DRC_BIST_CONFIGURATIONSr, rval));

        SOC_IF_ERROR_RETURN(DRC_REG_READ(unit, drc_channel_set[ci>>1], DRC_BIST_CONFIGURATIONSr, &rval));
        soc_reg_field_set(unit, DRCA_BIST_CONFIGURATIONSr, &rval, BIST_ENf, 1);
        SOC_IF_ERROR_RETURN(DRC_REG_WRITE(unit, drc_channel_set[ci>>1], DRC_BIST_CONFIGURATIONSr, rval));

        i = 0;
        do {
            sal_usleep(20);
            SOC_IF_ERROR_RETURN(DRC_REG_READ(unit, drc_channel_set[ci>>1], DRC_BIST_STATUSESr, &rval));
            if (soc_reg_field_get(unit, DRCA_BIST_STATUSESr, rval, BIST_FINISHEDf)) {
                SOC_IF_ERROR_RETURN(DRC_REG_READ(unit, drc_channel_set[ci>>1], DRC_BIST_CONFIGURATIONSr, &rval));
                soc_reg_field_set(unit, DRCA_BIST_CONFIGURATIONSr, &rval, BIST_ENf, 0);
                SOC_IF_ERROR_RETURN(DRC_REG_WRITE(unit, drc_channel_set[ci>>1], DRC_BIST_CONFIGURATIONSr, rval));

                SOC_IF_ERROR_RETURN(DRC_REG_READ(unit, drc_channel_set[ci>>1], DRC_BIST_ERROR_OCCURREDr, &rval));
                rdata = soc_reg_field_get(unit, DRCA_BIST_ERROR_OCCURREDr, rval, ERR_OCCURREDf);
                if(wl == 0) {
                    efold = rdata & 0xFFFF;
                } else {
#ifdef CONFIG_IPROC_DDR_ECC
                    if(wl == 2) {
                        if(iproc_read_ecc_syndrome()) {
                            efold = 0xFFFF;
                        } else {
                            efold = ((rdata >> 16) & 0xFFFF) | (rdata & 0xFFFF);
                        }
                        iproc_clear_ecc_syndrome();
                    } else {
#endif
                        efold = (rdata >> 16) & 0xFFFF;
#ifdef CONFIG_IPROC_DDR_ECC
                    }
#endif
                }
                if(efold != 0) {
                    result_fail = 1;
                }
                break;
            }
            if (i > 10000) {
                soc_cm_debug(DK_ERR, "Functional Self-Test timeout (1) \n");
                result_fail = 0xFFFF;
                SOC_IF_ERROR_RETURN(DRC_REG_READ(unit, drc_channel_set[ci>>1], DRC_BIST_CONFIGURATIONSr, &rval));
                soc_reg_field_set(unit, DRCA_BIST_CONFIGURATIONSr, &rval, BIST_ENf, 0);
                SOC_IF_ERROR_RETURN(DRC_REG_WRITE(unit, drc_channel_set[ci>>1], DRC_BIST_CONFIGURATIONSr, rval));
                return SOC_E_FAIL;
            }
            i++;
        } while (TRUE);
        j++;
    }

    if ((result_fail != 0) || (flag & 0x2))  {
        if((flag & 0x1) == 0) { 
            if(efold & 0xFF00) {
                efold |= 0xFF00;
            }
            if(efold & 0x00FF) {
                efold |= 0x00FF;
            }
        }
        if((flag & 0x2) != 0) {
            if(wl == 0) {
                READ_DDR40_PHY_WORD_LANE_0_READ_FIFO_STATUSr(unit, ci, &data);
                fifo_status0 = DDR40_GET_FIELD(data, DDR40_PHY_WORD_LANE_0, READ_FIFO_STATUS,
                                                STATUS0);
                fifo_status1 = DDR40_GET_FIELD(data, DDR40_PHY_WORD_LANE_0, READ_FIFO_STATUS,
                                                STATUS1);
            } else {
#ifdef CONFIG_IPROC_DDR_ECC
                if(wl == 2) {
                    READ_DDR40_PHY_ECC_LANE_READ_FIFO_STATUSr(unit, ci, &data);
                    fifo_status0 = DDR40_GET_FIELD(data, DDR40_PHY_ECC_LANE, READ_FIFO_STATUS,
                                                    STATUS0);
                    fifo_status1 = 0;
                } else {
#endif
                    READ_DDR40_PHY_WORD_LANE_1_READ_FIFO_STATUSr(unit, ci, &data);
                    fifo_status0 = DDR40_GET_FIELD(data, DDR40_PHY_WORD_LANE_1, READ_FIFO_STATUS,
                                                    STATUS0);
                    fifo_status1 = DDR40_GET_FIELD(data, DDR40_PHY_WORD_LANE_1, READ_FIFO_STATUS,
                                                    STATUS1);
#ifdef CONFIG_IPROC_DDR_ECC
                }
#endif
            }
            efold = 0x0000;
            if(fifo_status0 != 0) {
                efold |= 0x00FF;
            }
            if(fifo_status1 != 0) {
                efold |= 0xFF00;
            }
            if(efold != 0x0) {
                result_fail = 2;
            }
        /*    soc_cm_print("FIFO-0: %d\tFIFO-1: %d\n", fifo_status0, fifo_status1); */
        }
        if(result_fail != 0xFFFF) {
            result_fail = efold;
        }
    }

    return result_fail;
}
#endif

/* Flag Settings:
 * flag[0]: 0 - Mask errors per byte
 *          1 - Do not mask errors
 * flag[1]: 0 - Compare data
 *          1 - Check FIFO status
 * flag[2]: 0 - Disable MPR
 *          1 - Enable MPR
 */

uint32
_test_func_self_test_1(int unit, int ci, int wl, int loop, uint32 flag)
{
	 if(SOC_IS_ARAD(unit)) {
        return _test_func_self_test_1_arad(unit, ci, wl, loop, flag);
    } else {
        return SOC_E_INTERNAL;     /* Unsupported configuration */
    }
}

#ifdef BCM_ARAD_SUPPORT
uint32
_test_func_self_test_2_arad(int unit, int ci, int wl, int loop)
{
    int i, j;
    uint32 rval = 0;
    uint32 result_fail = 0;
    uint32 wdata[8];
    int k;

    uint32 startAddr;
  
    /* Alternate data test */
    j = 0;
    while (j < loop) {
        rval = 0;
        SOC_IF_ERROR_RETURN(DRC_REG_READ(unit, drc_channel_set[ci>>1], DRC_BIST_CONFIGURATIONSr, &rval));
        soc_reg_field_set(unit, DRCA_BIST_CONFIGURATIONSr, &rval, WRITE_WEIGHTf, 255);
        soc_reg_field_set(unit, DRCA_BIST_CONFIGURATIONSr, &rval, READ_WEIGHTf, 255);
        soc_reg_field_set(unit, DRCA_BIST_CONFIGURATIONSr, &rval, PATTERN_BIT_MODEf, 0);
        soc_reg_field_set(unit, DRCA_BIST_CONFIGURATIONSr, &rval, PRBS_MODEf, 1);
        soc_reg_field_set(unit, DRCA_BIST_CONFIGURATIONSr, &rval, CONS_ADDR_8_BANKSf, 1);
        soc_reg_field_set(unit, DRCA_BIST_CONFIGURATIONSr, &rval, ADDRESS_SHIFT_MODEf, 0);
        soc_reg_field_set(unit, DRCA_BIST_CONFIGURATIONSr, &rval, DATA_SHIFT_MODEf, 0);
        soc_reg_field_set(unit, DRCA_BIST_CONFIGURATIONSr, &rval, DATA_ADDR_MODEf, 0);
        SOC_IF_ERROR_RETURN(DRC_REG_WRITE(unit, drc_channel_set[ci>>1], DRC_BIST_CONFIGURATIONSr, rval));

        SOC_IF_ERROR_RETURN(DRC_REG_READ(unit, drc_channel_set[ci>>1], DRC_BIST_NUMBER_OF_ACTIONSr, &rval));
        soc_reg_field_set(unit, DRCA_BIST_NUMBER_OF_ACTIONSr, &rval, BIST_NUM_ACTIONSf, 510);
        SOC_IF_ERROR_RETURN(DRC_REG_WRITE(unit, drc_channel_set[ci>>1], DRC_BIST_NUMBER_OF_ACTIONSr, rval));

        /* startAddr = _get_random() & 0xffffff; */
        startAddr =0x0;
        SOC_IF_ERROR_RETURN(DRC_REG_READ(unit, drc_channel_set[ci>>1], DRC_BIST_START_ADDRESSr, &rval));
        soc_reg_field_set(unit, DRCA_BIST_START_ADDRESSr, &rval, BIST_START_ADDRESSf, startAddr);
        SOC_IF_ERROR_RETURN(DRC_REG_WRITE(unit, drc_channel_set[ci>>1], DRC_BIST_START_ADDRESSr, rval));

        SOC_IF_ERROR_RETURN(DRC_REG_READ(unit, drc_channel_set[ci>>1], DRC_BIST_END_ADDRESSr, &rval));
        soc_reg_field_set(unit, DRCA_BIST_END_ADDRESSr, &rval, BIST_END_ADDRESSf, 0xFFFFFF);
        SOC_IF_ERROR_RETURN(DRC_REG_WRITE(unit, drc_channel_set[ci>>1], DRC_BIST_END_ADDRESSr, rval));

        for(k = 0; k < 8; k++) {
            wdata[k] = _get_random();
        }
        
        SOC_IF_ERROR_RETURN
            (DRC_REG_WRITE(unit, drc_channel_set[ci>>1], DRC_BIST_PATTERN_WORD_0r, wdata[0]));
        SOC_IF_ERROR_RETURN
            (DRC_REG_WRITE(unit, drc_channel_set[ci>>1], DRC_BIST_PATTERN_WORD_1r, wdata[1]));
        SOC_IF_ERROR_RETURN
            (DRC_REG_WRITE(unit, drc_channel_set[ci>>1], DRC_BIST_PATTERN_WORD_2r, wdata[2]));
        SOC_IF_ERROR_RETURN
            (DRC_REG_WRITE(unit, drc_channel_set[ci>>1], DRC_BIST_PATTERN_WORD_3r, wdata[3]));
        SOC_IF_ERROR_RETURN
            (DRC_REG_WRITE(unit, drc_channel_set[ci>>1], DRC_BIST_PATTERN_WORD_4r, wdata[4]));
        SOC_IF_ERROR_RETURN
            (DRC_REG_WRITE(unit, drc_channel_set[ci>>1], DRC_BIST_PATTERN_WORD_5r, wdata[5]));
        SOC_IF_ERROR_RETURN
            (DRC_REG_WRITE(unit, drc_channel_set[ci>>1], DRC_BIST_PATTERN_WORD_6r, wdata[6]));
        SOC_IF_ERROR_RETURN
            (DRC_REG_WRITE(unit, drc_channel_set[ci>>1], DRC_BIST_PATTERN_WORD_7r, wdata[7]));

        SOC_IF_ERROR_RETURN(DRC_REG_READ(unit, drc_channel_set[ci>>1], DRC_BIST_CONFIGURATIONSr, &rval));
        soc_reg_field_set(unit, DRCA_BIST_CONFIGURATIONSr, &rval, IND_WR_RD_ADDR_MODEf, 1);
        SOC_IF_ERROR_RETURN(DRC_REG_WRITE(unit, drc_channel_set[ci>>1], DRC_BIST_CONFIGURATIONSr, rval));

        SOC_IF_ERROR_RETURN(DRC_REG_READ(unit, drc_channel_set[ci>>1], DRC_BIST_CONFIGURATIONSr, &rval));
        soc_reg_field_set(unit, DRCA_BIST_CONFIGURATIONSr, &rval, BIST_ENf, 1);
        SOC_IF_ERROR_RETURN(DRC_REG_WRITE(unit, drc_channel_set[ci>>1], DRC_BIST_CONFIGURATIONSr, rval));

        i = 0;
        do {
            sal_usleep(20);
            SOC_IF_ERROR_RETURN(DRC_REG_READ(unit, drc_channel_set[ci>>1], DRC_BIST_STATUSESr, &rval));
            if (soc_reg_field_get(unit, DRCA_BIST_STATUSESr, rval, BIST_FINISHEDf)) {
                SOC_IF_ERROR_RETURN(DRC_REG_READ(unit, drc_channel_set[ci>>1], DRC_BIST_CONFIGURATIONSr, &rval));
                soc_reg_field_set(unit, DRCA_BIST_CONFIGURATIONSr, &rval, BIST_ENf, 0);
                SOC_IF_ERROR_RETURN(DRC_REG_WRITE(unit, drc_channel_set[ci>>1], DRC_BIST_CONFIGURATIONSr, rval));

                SOC_IF_ERROR_RETURN(DRC_REG_READ(unit, drc_channel_set[ci>>1], DRC_BIST_ERROR_OCCURREDr, &rval));
                result_fail = soc_reg_field_get(unit, DRCA_BIST_ERROR_OCCURREDr, rval, ERR_OCCURREDf);
                break;
            }
            if (i > 10000) {
                soc_cm_debug(DK_ERR, "Functional Self-Test timeout (1) \n");
                result_fail = 0xFFFF;
                SOC_IF_ERROR_RETURN(DRC_REG_READ(unit, drc_channel_set[ci>>1], DRC_BIST_CONFIGURATIONSr, &rval));
                soc_reg_field_set(unit, DRCA_BIST_CONFIGURATIONSr, &rval, BIST_ENf, 0);
                SOC_IF_ERROR_RETURN(DRC_REG_WRITE(unit, drc_channel_set[ci>>1], DRC_BIST_CONFIGURATIONSr, rval));
                return SOC_E_FAIL;
            }
            i++;
        } while (TRUE);
        j++;
    }

    if (result_fail > 0) {
        result_fail = 0xffff;
    }

    return result_fail;
}
#endif

uint32
_test_func_self_test_2(int unit, int ci, int wl, int loop)
{
    if(SOC_IS_ARAD(unit)) {
        return _test_func_self_test_2_arad(unit, ci, wl, loop);
    } else {
        return SOC_E_INTERNAL;     /* Unsupported configuration */
    }
}

STATIC uint32 _test_mem_read(int unit, int ci, uint32 addr, uint32 * data_exp)
{
    int i, j;
    uint32 uDataRD[NUM_DATA];
    uint32 result = 0;

    if(!SOC_IS_KATANA(unit) && !SOC_IS_CALADAN3(unit)) {
        return result;
    }

    for (i = 0; i < NUM_DATA / 8; i++) {
        soc_ddr40_read(unit, ci, addr + i, &uDataRD[(i * 8)],
                       &uDataRD[(i * 8) + 1], &uDataRD[(i * 8) + 2],
                       &uDataRD[(i * 8) + 3], &uDataRD[(i * 8) + 4],
                       &uDataRD[(i * 8) + 5], &uDataRD[(i * 8) + 6],
                       &uDataRD[(i * 8) + 7]);

        if (data_exp != NULL) {
            for (j = 0; j < 8; j++) {
                if (data_exp[(i * 8) + j] != uDataRD[(i * 8) + j]) {
                    result |= data_exp[(i * 8) + j] ^ uDataRD[(i * 8) + j];
                }
            }
            result |= (result >> 16);
            result &= 0xFFFF;
        }
    }
    return result;
}

STATIC uint32 _test_mem_wr_rd(int unit, int ci, uint32 addr)
{
    uint32 uDataWR[NUM_DATA];
    uint32 result = 0;

    if(!SOC_IS_KATANA(unit) && !SOC_IS_CALADAN3(unit)) {
        return result;
    }

    _fill_in_data(uDataWR);
    _test_mem_write(unit, ci, addr, uDataWR);

    result = _test_mem_read(unit, ci, addr, uDataWR);

    return result;
}

/* Prepare for shmoo-calibration */
int _soc_ddr_shmoo_prepare_for_shmoo(int unit, int ci)
{
    int wl;
    uint32 data;
    int timeout_cnt, calib_steps, clock_period;
    int tmp_ps;

    wl = 0;
    /* I want to display all STATUS value here and report */
	printf("_soc_ddr_shmoo_prepare_for_shmoo: Enter\n");
    READ_DDR40_PHY_CONTROL_REGS_PLL_STATUSr(unit, ci, &data);
    printf("(WL=%d) data = 0x%x \n", wl, data);
    printf("(WL=%d) PLL_STATUS : LOCK_LOST = 0x%x \n", wl, DDR40_GET_FIELD(data, DDR40_PHY_CONTROL_REGS, PLL_STATUS, LOCK_LOST));
    
    printf("(WL=%d) PLL_STATUS : LOCK      = 0x%x \n", wl, \
                 DDR40_GET_FIELD(data, DDR40_PHY_CONTROL_REGS, PLL_STATUS, \
                                 LOCK));
    READ_DDR40_PHY_CONTROL_REGS_ZQ_PVT_COMP_CTLr(unit, ci, &data);
    
    printf("(WL=%d) data      = 0x%x \n", wl, data);
    
    printf \
    ("(WL=%d) ZQ_PVT_COMP_CTL : PD_COMP          = 0x%x \n", wl, \
                 DDR40_GET_FIELD(data, DDR40_PHY_CONTROL_REGS, ZQ_PVT_COMP_CTL, \
                                 PD_COMP));
    
    printf(
    "(WL=%d) ZQ_PVT_COMP_CTL : ND_COMP          = 0x%x \n", wl,
                 DDR40_GET_FIELD(data, DDR40_PHY_CONTROL_REGS, ZQ_PVT_COMP_CTL,
                                 ND_COMP));
    if (wl == 0) {
        READ_DDR40_PHY_WORD_LANE_0_READ_CONTROLr(unit, ci, &data);
        
        printf(
        "(WL=%d) data      = 0x%x \n", wl, data);
        
        printf(
            "(WL=%d) PHY_WORD_LANE_READ_CONTROL : DQ_ODT_ENABLE = 0x%x \n",
             wl, DDR40_GET_FIELD(data, DDR40_PHY_WORD_LANE_0, READ_CONTROL,
                                 DQ_ODT_ENABLE));
        
        printf
            ("(WL=%d) PHY_WORD_LANE_READ_CONTROL : DQ_ODT_LE_ADJ = 0x%x \n",
             wl, DDR40_GET_FIELD(data, DDR40_PHY_WORD_LANE_0, READ_CONTROL,
                                 DQ_ODT_LE_ADJ));
        
        printf
            ("(WL=%d) PHY_WORD_LANE_READ_CONTROL : DQ_ODT_TE_ADJ = 0x%x \n",
             wl, DDR40_GET_FIELD(data, DDR40_PHY_WORD_LANE_0, READ_CONTROL,
                                 DQ_ODT_TE_ADJ));
    } else {                    /* if (wl==0) */
#ifdef CONFIG_IPROC_DDR_ECC
        if(wl == 2) {
            READ_DDR40_PHY_ECC_LANE_READ_CONTROLr(unit, ci, &data);
            
            printf(
            ("(WL=%d) data      = 0x%x \n", wl, data));
            
            printf(
                ("(WL=%d) PHY_ECC_LANE_READ_CONTROL : DQ_ODT_ENABLE = 0x%x \n",
                 wl, DDR40_GET_FIELD(data, DDR40_PHY_ECC_LANE, READ_CONTROL,
                                     DQ_ODT_ENABLE)));
            
            printf(
                ("(WL=%d) PHY_ECC_LANE_READ_CONTROL : DQ_ODT_LE_ADJ = 0x%x \n",
                 wl, DDR40_GET_FIELD(data, DDR40_PHY_ECC_LANE, READ_CONTROL,
                                     DQ_ODT_LE_ADJ)));
            
            printf(
                ("(WL=%d) PHY_ECC_LANE_READ_CONTROL : DQ_ODT_TE_ADJ = 0x%x \n",
                 wl, DDR40_GET_FIELD(data, DDR40_PHY_ECC_LANE, READ_CONTROL,
                                     DQ_ODT_TE_ADJ)));
        } else {
#endif
            READ_DDR40_PHY_WORD_LANE_1_READ_CONTROLr(unit, ci, &data);
            
            printf(
            ("(WL=%d) data      = 0x%x \n", wl, data));
            
            printf(
                ("(WL=%d) PHY_WORD_LANE_READ_CONTROL : DQ_ODT_ENABLE = 0x%x \n",
                 wl, DDR40_GET_FIELD(data, DDR40_PHY_WORD_LANE_1, READ_CONTROL,
                                     DQ_ODT_ENABLE)));
            
            printf(
                ("(WL=%d) PHY_WORD_LANE_READ_CONTROL : DQ_ODT_LE_ADJ = 0x%x \n",
                 wl, DDR40_GET_FIELD(data, DDR40_PHY_WORD_LANE_1, READ_CONTROL,
                                     DQ_ODT_LE_ADJ)));
            
            printf(
                ("(WL=%d) PHY_WORD_LANE_READ_CONTROL : DQ_ODT_TE_ADJ = 0x%x \n",
                 wl, DDR40_GET_FIELD(data, DDR40_PHY_WORD_LANE_1, READ_CONTROL,
                                     DQ_ODT_TE_ADJ)));
#ifdef CONFIG_IPROC_DDR_ECC
        }
#endif
    }
    READ_DDR40_PHY_CONTROL_REGS_VDL_CALIBRATEr(unit, ci, &data);
    printf("(WL=%d) data      = 0x%x \n", wl, data);
    printf("(WL=%d) VDL_CALIBRATE : CALIB_FAST       = 0x%x \n", wl,
                 DDR40_GET_FIELD(data, DDR40_PHY_CONTROL_REGS, VDL_CALIBRATE,
                                 CALIB_FAST));
    printf("(WL=%d) VDL_CALIBRATE : CALIB_ONCE       = 0x%x \n", wl,
                 DDR40_GET_FIELD(data, DDR40_PHY_CONTROL_REGS, VDL_CALIBRATE,
                                 CALIB_ONCE));
    printf("(WL=%d) VDL_CALIBRATE : CALIB_ALWAYS     = 0x%x \n", wl,
                 DDR40_GET_FIELD(data, DDR40_PHY_CONTROL_REGS, VDL_CALIBRATE,
                                 CALIB_ALWAYS));
    printf("(WL=%d) VDL_CALIBRATE : CALIB_TEST       = 0x%x \n", wl,
                 DDR40_GET_FIELD(data, DDR40_PHY_CONTROL_REGS, VDL_CALIBRATE,
                                 CALIB_TEST));
    printf("(WL=%d) VDL_CALIBRATE : CALIB_CLOCKS     = 0x%x \n", wl,
                 DDR40_GET_FIELD(data, DDR40_PHY_CONTROL_REGS, VDL_CALIBRATE,
                                 CALIB_CLOCKS));
    printf("(WL=%d) VDL_CALIBRATE : CALIB_BYTE       = 0x%x \n", wl,
                 DDR40_GET_FIELD(data, DDR40_PHY_CONTROL_REGS, VDL_CALIBRATE,
                                 CALIB_BYTE));
    printf("(WL=%d) VDL_CALIBRATE : CALIB_PHYBIST    = 0x%x \n", wl,
                 DDR40_GET_FIELD(data, DDR40_PHY_CONTROL_REGS, VDL_CALIBRATE,
                                 CALIB_PHYBIST));
    printf("(WL=%d) VDL_CALIBRATE : CALIB_FTM        = 0x%x \n", wl,
                 DDR40_GET_FIELD(data, DDR40_PHY_CONTROL_REGS, VDL_CALIBRATE,
                                 CALIB_FTM));
    printf("(WL=%d) VDL_CALIBRATE : CALIB_AUTO       = 0x%x \n", wl,
                 DDR40_GET_FIELD(data, DDR40_PHY_CONTROL_REGS, VDL_CALIBRATE,
                                 CALIB_AUTO));
    printf("(WL=%d) VDL_CALIBRATE : CALIB_STEPS      = 0x%x \n", wl,
                 DDR40_GET_FIELD(data, DDR40_PHY_CONTROL_REGS, VDL_CALIBRATE,
                                 CALIB_STEPS));
    printf("(WL=%d) VDL_CALIBRATE : CALIB_DQS_PAIR   = 0x%x \n", wl,
                 DDR40_GET_FIELD(data, DDR40_PHY_CONTROL_REGS, VDL_CALIBRATE,
                                 CALIB_DQS_PAIR));
    printf("(WL=%d) VDL_CALIBRATE : CALIB_DQS_CLOCKS = 0x%x \n", wl,
                 DDR40_GET_FIELD(data, DDR40_PHY_CONTROL_REGS, VDL_CALIBRATE,
                                 CALIB_DQS_CLOCKS));
    printf("(WL=%d) VDL_CALIBRATE : CALIB_BIT_OFFSET = 0x%x \n", wl,
                 DDR40_GET_FIELD(data, DDR40_PHY_CONTROL_REGS, VDL_CALIBRATE,
                                 CALIB_BIT_OFFSET));
    printf("(WL=%d) VDL_CALIBRATE : RD_EN_CAL        = 0x%x \n", wl,
                 DDR40_GET_FIELD(data, DDR40_PHY_CONTROL_REGS, VDL_CALIBRATE,
                                 RD_EN_CAL));
    printf("(WL=%d) VDL_CALIBRATE : BIT_CAL          = 0x%x \n", wl,
                 DDR40_GET_FIELD(data, DDR40_PHY_CONTROL_REGS, VDL_CALIBRATE,
                                 BIT_CAL));
    printf("(WL=%d) VDL_CALIBRATE : SET_MR_MPR       = 0x%x \n", wl,
                 DDR40_GET_FIELD(data, DDR40_PHY_CONTROL_REGS, VDL_CALIBRATE,
                                 SET_MR_MPR));
    printf("(WL=%d) VDL_CALIBRATE : DQ0_ONLY         = 0x%x \n", wl,
                 DDR40_GET_FIELD(data, DDR40_PHY_CONTROL_REGS, VDL_CALIBRATE,
                                 DQ0_ONLY));
    printf("(WL=%d) VDL_CALIBRATE : SET_WR_DQ        = 0x%x \n", wl,
                 DDR40_GET_FIELD(data, DDR40_PHY_CONTROL_REGS, VDL_CALIBRATE,
                                 SET_WR_DQ));
    printf("(WL=%d) VDL_CALIBRATE : BIT_REFRESH      = 0x%x \n", wl,
                 DDR40_GET_FIELD(data, DDR40_PHY_CONTROL_REGS, VDL_CALIBRATE,
                                 BIT_REFRESH));
    printf("(WL=%d) VDL_CALIBRATE : RD_DLY_CAL       = 0x%x \n", wl,
                 DDR40_GET_FIELD(data, DDR40_PHY_CONTROL_REGS, VDL_CALIBRATE,
                                 RD_DLY_CAL));
    printf("(WL=%d) VDL_CALIBRATE : EXIT_IN_SR       = 0x%x \n", wl,
                 DDR40_GET_FIELD(data, DDR40_PHY_CONTROL_REGS, VDL_CALIBRATE,
                                 EXIT_IN_SR));
    printf("(WL=%d) VDL_CALIBRATE : SKIP_RST         = 0x%x \n", wl,
                 DDR40_GET_FIELD(data, DDR40_PHY_CONTROL_REGS, VDL_CALIBRATE,
                                 SKIP_RST));
    printf("(WL=%d) VDL_CALIBRATE : AUTO_INIT        = 0x%x \n", wl,
                 DDR40_GET_FIELD(data, DDR40_PHY_CONTROL_REGS, VDL_CALIBRATE,
                                 AUTO_INIT));
    printf("(WL=%d) VDL_CALIBRATE : USE_STRAPS       = 0x%x \n", wl,
                 DDR40_GET_FIELD(data, DDR40_PHY_CONTROL_REGS, VDL_CALIBRATE,
                                 USE_STRAPS));
    READ_DDR40_PHY_CONTROL_REGS_VDL_CALIB_STATUSr(unit, ci, &data);
    printf("(WL=%d) data      = 0x%x \n", wl, data);
    printf
        ("(WL=%d) VDL_CALIB_STATUS : CALIB_LOCK                           = 0x%x \n",
         wl, DDR40_GET_FIELD(data, DDR40_PHY_CONTROL_REGS, VDL_CALIB_STATUS,
                             CALIB_LOCK));
    printf
        ("(WL=%d) VDL_CALIB_STATUS : CALIB_IDLE                           = 0x%x \n",
         wl, DDR40_GET_FIELD(data, DDR40_PHY_CONTROL_REGS, VDL_CALIB_STATUS,
                             CALIB_IDLE));
    printf
        ("(WL=%d) VDL_CALIB_STATUS : CALIB_BYTE_SEL                       = 0x%x \n",
         wl, DDR40_GET_FIELD(data, DDR40_PHY_CONTROL_REGS, VDL_CALIB_STATUS,
                             CALIB_BYTE_SEL));
    printf
        ("(WL=%d) VDL_CALIB_STATUS : CALIB_BIT_OFFSET  set if byte mode   = 0x%x \n",
         wl, DDR40_GET_FIELD(data, DDR40_PHY_CONTROL_REGS, VDL_CALIB_STATUS,
                             CALIB_BIT_OFFSET));
    printf
        ("(WL=%d) NOTE: For single step calibration total result, please see below \n",
         wl);
    READ_DDR40_PHY_CONTROL_REGS_VDL_DQ_CALIB_STATUSr(unit, ci, &data);
    printf("(WL=%d) data      = 0x%x \n", wl, data);
    printf
        ("(WL=%d) VDL_DQ_CALIB_STATUS : DQ_CALIB_LOCK                     = 0x%x \n",
         wl, DDR40_GET_FIELD(data, DDR40_PHY_CONTROL_REGS, VDL_DQ_CALIB_STATUS,
                             DQ_CALIB_LOCK));
    printf
        ("(WL=%d) VDL_DQ_CALIB_STATUS : DQS_CALIB_LOCK                    = 0x%x \n",
         wl, DDR40_GET_FIELD(data, DDR40_PHY_CONTROL_REGS, VDL_DQ_CALIB_STATUS,
                             DQS_CALIB_LOCK));
    printf
        ("(WL=%d) VDL_DQ_CALIB_STATUS : DQS_CALIB_MODE    DQS(1=pair)     = 0x%x \n",
         wl, DDR40_GET_FIELD(data, DDR40_PHY_CONTROL_REGS, VDL_DQ_CALIB_STATUS,
                             DQS_CALIB_MODE));
    printf
        ("(WL=%d) VDL_DQ_CALIB_STATUS : DQS_CALIB_CLOCKS  DQS(0=half bit) = 0x%x \n",
         wl, DDR40_GET_FIELD(data, DDR40_PHY_CONTROL_REGS, VDL_DQ_CALIB_STATUS,
                             DQS_CALIB_CLOCKS));
    printf
        ("(WL=%d) VDL_DQ_CALIB_STATUS : DQ_CALIB_TOTAL    DQ (steps)      = 0x%x \n",
         wl, DDR40_GET_FIELD(data, DDR40_PHY_CONTROL_REGS, VDL_DQ_CALIB_STATUS,
                             DQ_CALIB_TOTAL) >> 4);
    printf
        ("(WL=%d) VDL_DQ_CALIB_STATUS : DQS_CALIB_TOTAL   DQS (steps)     = 0x%x \n",
         wl, DDR40_GET_FIELD(data, DDR40_PHY_CONTROL_REGS, VDL_DQ_CALIB_STATUS,
                             DQS_CALIB_TOTAL) >> 4);
    READ_DDR40_PHY_CONTROL_REGS_VDL_WR_CHAN_CALIB_STATUSr(unit, ci, &data);
    printf("(WL=%d) data      = 0x%x \n", wl, data);
    printf
        ("(WL=%d) VDL_WR_CHAN_CALIB_STATUS : WR_CHAN_CALIB_LOCK                                          = 0x%x \n",
         wl, DDR40_GET_FIELD(data, DDR40_PHY_CONTROL_REGS,
                             VDL_WR_CHAN_CALIB_STATUS, WR_CHAN_CALIB_LOCK));
    printf
        ("(WL=%d) VDL_WR_CHAN_CALIB_STATUS : WR_CHAN_CALIB_BYTE_SEL   (1=byte)                           = 0x%x \n",
         wl, DDR40_GET_FIELD(data, DDR40_PHY_CONTROL_REGS,
                             VDL_WR_CHAN_CALIB_STATUS,
                             WR_CHAN_CALIB_BYTE_SEL));
    printf
        ("(WL=%d) VDL_WR_CHAN_CALIB_STATUS : WR_CHAN_CALIB_CLOCKS     (0=1/2bit)                         = 0x%x \n",
         wl, DDR40_GET_FIELD(data, DDR40_PHY_CONTROL_REGS,
                             VDL_WR_CHAN_CALIB_STATUS, WR_CHAN_CALIB_CLOCKS));
    printf
        ("(WL=%d) VDL_WR_CHAN_CALIB_STATUS : WR_CHAN_CALIB_TOTAL      (steps)                            = 0x%x \n",
         wl, DDR40_GET_FIELD(data, DDR40_PHY_CONTROL_REGS,
                             VDL_WR_CHAN_CALIB_STATUS,
                             WR_CHAN_CALIB_TOTAL) >> 4);
    printf
        ("(WL=%d) VDL_WR_CHAN_CALIB_STATUS : WR_CHAN_CALIB_BIT_OFFSET (in byte mode, setting for bit vdl)= 0x%x \n",
         wl, DDR40_GET_FIELD(data, DDR40_PHY_CONTROL_REGS,
                             VDL_WR_CHAN_CALIB_STATUS,
                             WR_CHAN_CALIB_BIT_OFFSET));
    READ_DDR40_PHY_CONTROL_REGS_VDL_RD_EN_CALIB_STATUSr(unit, ci, &data);
    printf("(WL=%d) data      = 0x%x \n", wl, data);
    printf
        ("(WL=%d) VDL_RD_EN_CALIB_STATUS : RD_EN_CALIB_LOCK                                          = 0x%x \n",
         wl, DDR40_GET_FIELD(data, DDR40_PHY_CONTROL_REGS,
                             VDL_RD_EN_CALIB_STATUS, RD_EN_CALIB_LOCK));
    printf
        ("(WL=%d) VDL_RD_EN_CALIB_STATUS : RD_EN_CALIB_BYTE_SEL   (1=byte)                           = 0x%x \n",
         wl, DDR40_GET_FIELD(data, DDR40_PHY_CONTROL_REGS,
                             VDL_RD_EN_CALIB_STATUS, RD_EN_CALIB_BYTE_SEL));
    printf
        ("(WL=%d) VDL_RD_EN_CALIB_STATUS : RD_EN_CALIB_CLOCKS     (0=1/2bit)                         = 0x%x \n",
         wl, DDR40_GET_FIELD(data, DDR40_PHY_CONTROL_REGS,
                             VDL_RD_EN_CALIB_STATUS, RD_EN_CALIB_CLOCKS));
    printf
        ("(WL=%d) VDL_RD_EN_CALIB_STATUS : RD_EN_CALIB_TOTAL      (steps)                            = 0x%x \n",
         wl, DDR40_GET_FIELD(data, DDR40_PHY_CONTROL_REGS,
                             VDL_RD_EN_CALIB_STATUS, RD_EN_CALIB_TOTAL) >> 4);
    printf
        ("(WL=%d) VDL_RD_EN_CALIB_STATUS : RD_EN_CALIB_BIT_OFFSET (in byte mode, setting for bit vdl)= 0x%x \n",
         wl, DDR40_GET_FIELD(data, DDR40_PHY_CONTROL_REGS,
                             VDL_RD_EN_CALIB_STATUS, RD_EN_CALIB_BIT_OFFSET));

    /* Calculate VDL calibration steps */
    /* calib steps */
    WRITE_DDR40_PHY_CONTROL_REGS_VDL_CALIBRATEr(unit, ci, 0x0);
    WRITE_DDR40_PHY_CONTROL_REGS_VDL_CALIBRATEr(unit, ci, 0x200);       /* calib_auto = 1 */
    data = 0;
    timeout_cnt = 0;
    calib_steps = 0;
    do {
        sal_usleep(10);
        READ_DDR40_PHY_CONTROL_REGS_VDL_CALIB_STATUSr(unit, ci, &data);
        if (DDR40_GET_FIELD
            (data, DDR40_PHY_CONTROL_REGS, VDL_CALIB_STATUS, CALIB_IDLE)) {
            calib_steps =
                DDR40_GET_FIELD(data, DDR40_PHY_CONTROL_REGS, VDL_CALIB_STATUS,
                                CALIB_TOTAL);
            if (DDR40_GET_FIELD
                (data, DDR40_PHY_CONTROL_REGS, VDL_CALIB_STATUS, CALIB_LOCK))
                printf("(WL=%d) VDL_CALIB_STATUS : NOT CALIB_LOCK \n",
                             wl);
            break;              /* Check to and wait till we find idle state */
        }                       /* if ( DDR40_GET_FIELD(data, DDR40_PHY_CONTROL_REGS, VDL_CALIB_STATUS, CALIB_LOCK   ) ) */
        if (timeout_cnt > 20000) {
            soc_cm_print("REWRITE Memory Register Mode failed\n");
            break;
        }
        timeout_cnt += 1;
    } while (TRUE);
    READ_DDR40_PHY_CONTROL_REGS_VDL_CALIB_STATUSr(unit, ci, &data);
    clock_period = 1000000 / SOC_DDR3_CLOCK_MHZ(unit);

    printf ("-----------------------------------------\n");
    printf ("---  Single STEP Calibration          ---\n");
    printf ("-----------------------------------------\n");
    printf
        ("(WL=%d) VDL_CALIB_STATUS : CALIB_LOCK        = %0d        \n", wl,
         DDR40_GET_FIELD(data, DDR40_PHY_CONTROL_REGS, VDL_CALIB_STATUS,
                         CALIB_LOCK));
    printf
        ("(WL=%d) VDL_CALIB_STATUS : CALIB_IDLE        = %0d        \n", wl,
         DDR40_GET_FIELD(data, DDR40_PHY_CONTROL_REGS, VDL_CALIB_STATUS,
                         CALIB_IDLE));
    printf
        ("(WL=%d) VDL_CALIB_STATUS : 360'CALIB_TOTAL   = %0d (steps)\n", wl,
         calib_steps);
    printf
        ("(WL=%d) VDL_CALIB_STATUS : 90' CALIB TOTAL   = %0d (steps)\n", wl,
         calib_steps >> 2);
    printf
        ("(WL=%d) VDL_CALIB_STATUS : 360'   steps time = %0d (ps)   \n", wl,
         clock_period);
    tmp_ps = clock_period / (calib_steps >> 2);
    printf
        ("(WL=%d) VDL_CALIB_STATUS : 90'    step  time = %0d.%0d (ps)   \n",
         wl, tmp_ps,
         ((clock_period * 1000 / (calib_steps >> 2)) - (tmp_ps * 1000)));
    tmp_ps = clock_period / calib_steps;
    printf
        ("(WL=%d) VDL_CALIB_STATUS : Single step  time = %0d.%0d (ps)   \n",
         wl, tmp_ps, ((clock_period * 1000 / calib_steps) - (tmp_ps * 1000)));

    /* Do one dummy memory read to fix RTL bug on RD_FIFO write pointer coming out of reset bug */
    data = _test_mem_read(unit, (ci + wl), 0, NULL);

    return SOC_E_NONE;
} /* _soc_ddr_shmoo_prepare_for_shmoo */


/* RESET MEMORY and Rewrite MRS register */

#ifdef BCM_ARAD_SUPPORT
int _soc_arad_mem_reset_and_init_after_shmoo_addr(int unit, int ci)
{
#if 0 // Srini - to be modified
    int timeout_cnt;
    uint32 rval;
#ifdef BCM_ARAD_SUPPORT
    uint8 is_valid[ARAD_HW_DRAM_INTERFACES_MAX] = {0};

    is_valid[ci>>1] = 1;

    arad_mgmt_dram_init_drc_soft_init(unit, is_valid, 1);
    sal_usleep(2000);
    arad_mgmt_dram_init_drc_soft_init(unit, is_valid, 0);
#endif

    timeout_cnt = 0;
    do {
        sal_usleep(10);
        SOC_IF_ERROR_RETURN(DRC_REG_READ(unit, drc_channel_set[ci>>1], DRC_DRAM_INIT_FINISHEDr, &rval));
        if (soc_reg_field_get(unit, DRCA_DRAM_INIT_FINISHEDr, rval, DRAM_INIT_FINISHEDf)) {
            break;
        }
        if (timeout_cnt > 20000) {
            soc_cm_debug(DK_ERR, "CI%d: Timed out DRAM re-initialization\n", ci);
            return SOC_E_TIMEOUT;
        }
        timeout_cnt += 1;
    } while (TRUE);
#endif
#if 0
	*((unsigned int *)DDR_DENALI_CTL_169) |= 0xf;
	sal_usleep(100);
	*((unsigned int *)DDR_DENALI_CTL_169) &= ~0xf;
#endif
    return SOC_E_NONE;
}
#endif
#if 1
int
_soc_mem_reset_and_init_after_shmoo_addr(int unit, int ci)
{
    if(SOC_IS_ARAD(unit)) {
        return _soc_arad_mem_reset_and_init_after_shmoo_addr(unit, ci);
    } else {
        return SOC_E_INTERNAL;     /* Unsupported configuration */
    }
}

/* Initialize VDL 90' result from VDL Calibration */
int _shmoo_init_vdl_result(int unit, int ci, int wl, int self_test, uint32 * result)
{
    int byte, bit;
    uint32 data_expected[NUM_DATA];
    uint32 data, setting, setting_steps, setting_byte_sel;
    int32 steps_calib_total;
    int calib_steps;

    _fill_in_data(data_expected);
    if (self_test == 0) {
        _test_mem_write(unit, (ci + wl), 0, data_expected);
    }

    /* RD_DATA_DELAY */
    data = 1; /* 0 is undefined for RD_DATA_DELAY */
    if(wl == 0) {
        WRITE_DDR40_PHY_WORD_LANE_0_READ_DATA_DLYr(unit, ci, data);
            
    } else {
#ifdef CONFIG_IPROC_DDR_ECC
        if(wl == 2) {
            WRITE_DDR40_PHY_ECC_LANE_READ_DATA_DLYr(unit, ci, data);
        } else {
#endif
            WRITE_DDR40_PHY_WORD_LANE_1_READ_DATA_DLYr(unit, ci, data);
#ifdef CONFIG_IPROC_DDR_ECC
        }
#endif
    }

    /* RD_EN */
    READ_DDR40_PHY_CONTROL_REGS_VDL_RD_EN_CALIB_STATUSr(unit, ci, &data);
    setting_byte_sel =
        DDR40_GET_FIELD(data, DDR40_PHY_CONTROL_REGS, VDL_RD_EN_CALIB_STATUS,
                        RD_EN_CALIB_BYTE_SEL);
    setting_steps =
        DDR40_GET_FIELD(data, DDR40_PHY_CONTROL_REGS, VDL_RD_EN_CALIB_STATUS,
                        RD_EN_CALIB_TOTAL) >> 4;
	if(setting_byte_sel) {
		setting = (setting_steps * 3) >> 1;
	} else {
		setting = setting_steps >> 1;
	}
    setting_byte_sel = 1;
    if (wl == 0) {
        setting += SHMOO_CI02_WL0_OFFSET_RD_EN;
    } else {
#ifdef CONFIG_IPROC_DDR_ECC
        if(wl == 2) {
            setting += SHMOO_CI02_WL0_OFFSET_RD_EN;
        } else {
#endif
            setting += SHMOO_CI0_WL1_OFFSET_RD_EN;
#ifdef CONFIG_IPROC_DDR_ECC
        }
#endif
    }
    if (setting & 0x80000000) {
        setting = 0;
    } else if (setting > 63) {
        setting = 63;
    }
    DDR40_SET_FIELD(setting, DDR40_PHY_CONTROL_REGS, VDL_OVRIDE_BYTE_CTL,
                    BYTE_SEL, setting_byte_sel);
    data = SET_OVR_STEP(setting);
    if (wl == 0) {
        WRITE_DDR40_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE_RD_ENr(unit, ci, data);
        WRITE_DDR40_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE0_BIT_RD_ENr(unit, ci,
                                                                data);
        WRITE_DDR40_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE1_BIT_RD_ENr(unit, ci,
                                                                data);
    } else {
#ifdef CONFIG_IPROC_DDR_ECC
        if(wl == 2) {
            WRITE_DDR40_PHY_ECC_LANE_VDL_OVRIDE_BYTE_RD_ENr(unit, ci, data);
            WRITE_DDR40_PHY_ECC_LANE_VDL_OVRIDE_BYTE_BIT_RD_ENr(unit, ci,
                                                                    data);
        } else {
#endif
            WRITE_DDR40_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE_RD_ENr(unit, ci, data);
            WRITE_DDR40_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE0_BIT_RD_ENr(unit, ci,
                                                                    data);
            WRITE_DDR40_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE1_BIT_RD_ENr(unit, ci,
                                                                    data);
#ifdef CONFIG_IPROC_DDR_ECC
        }
#endif
    }

    /* RD_DQ */
    READ_DDR40_PHY_CONTROL_REGS_VDL_DQ_CALIB_STATUSr(unit, ci, &data);
    steps_calib_total =
        DDR40_GET_FIELD(data, DDR40_PHY_CONTROL_REGS, VDL_DQ_CALIB_STATUS,
                        DQ_CALIB_TOTAL) >> 4;
    setting = steps_calib_total;
    if (wl == 0) {
        setting += SHMOO_CI02_WL0_OFFSET_RD_DQ;
    } else {
#ifdef CONFIG_IPROC_DDR_ECC
        if(wl == 2) {
            setting += SHMOO_CI02_WL0_OFFSET_RD_DQ;
        } else {
#endif
            setting += SHMOO_CI0_WL1_OFFSET_RD_DQ;
#ifdef CONFIG_IPROC_DDR_ECC
        }
#endif
    }
    if (setting & 0x80000000) {
        setting = 0;
    } else if (setting > 63) {
        setting = 63;
    }
    data = SET_OVR_STEP(setting);
#ifdef CONFIG_IPROC_DDR_ECC
        if(wl == 2) {
            for (byte = 0; byte < 1; byte++) {
                for (bit = 0; bit < 4; bit++) {
                    WRITE_DDR40_PHY_WORD_LANE_x_VDL_OVRIDE_BYTEy_BITz_R_Pr(wl, byte,
                                                                           bit, unit,
                                                                           ci, data);
                    WRITE_DDR40_PHY_WORD_LANE_x_VDL_OVRIDE_BYTEy_BITz_R_Nr(wl, byte,
                                                                           bit, unit,
                                                                           ci, data);
                }
            }
        } else {
#endif
            for (byte = 0; byte < 2; byte++) {
                for (bit = 0; bit < 8; bit++) {
                    WRITE_DDR40_PHY_WORD_LANE_x_VDL_OVRIDE_BYTEy_BITz_R_Pr(wl, byte,
                                                                           bit, unit,
                                                                           ci, data);
                    WRITE_DDR40_PHY_WORD_LANE_x_VDL_OVRIDE_BYTEy_BITz_R_Nr(wl, byte,
                                                                           bit, unit,
                                                                           ci, data);
                }
            }
#ifdef CONFIG_IPROC_DDR_ECC
        }
#endif

    /* RD_DQS */
    READ_DDR40_PHY_CONTROL_REGS_VDL_DQ_CALIB_STATUSr(unit, ci, &data);
    setting_byte_sel =
        DDR40_GET_FIELD(data, DDR40_PHY_CONTROL_REGS, VDL_DQ_CALIB_STATUS,
                        DQS_CALIB_MODE);
    setting_steps =
        DDR40_GET_FIELD(data, DDR40_PHY_CONTROL_REGS, VDL_DQ_CALIB_STATUS,
                        DQS_CALIB_TOTAL) >> 4;
    calib_steps =
                DDR40_GET_FIELD(data, DDR40_PHY_CONTROL_REGS, VDL_CALIB_STATUS,
                                CALIB_TOTAL);
    printf("calib_steps: %d\n", calib_steps);
    setting = (setting_steps * (1 + setting_byte_sel));
    if (wl == 0) {
        setting += SHMOO_CI02_WL0_OFFSET_RD_DQS;
    } else {
#ifdef CONFIG_IPROC_DDR_ECC
        if(wl == 2) {
            setting += SHMOO_CI02_WL0_OFFSET_RD_DQS;
        } else {
#endif
            setting += SHMOO_CI0_WL1_OFFSET_RD_DQS;
#ifdef CONFIG_IPROC_DDR_ECC
        }
#endif
    }
    if (setting & 0x80000000) {
        setting = 0;
    } else if (setting > 63) {
        setting = 63;
    }
    data = SET_OVR_STEP(setting);
    if (wl == 0) {
        WRITE_DDR40_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE0_R_Pr(unit, ci, data);
        WRITE_DDR40_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE0_R_Nr(unit, ci, data);
        WRITE_DDR40_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE1_R_Pr(unit, ci, data);
        WRITE_DDR40_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE1_R_Nr(unit, ci, data);
    } else {
#ifdef CONFIG_IPROC_DDR_ECC
        if(wl == 2) {
            WRITE_DDR40_PHY_ECC_LANE_VDL_OVRIDE_BYTE_R_Pr(unit, ci, data);
            WRITE_DDR40_PHY_ECC_LANE_VDL_OVRIDE_BYTE_R_Nr(unit, ci, data);
        } else {
#endif
        WRITE_DDR40_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE0_R_Pr(unit, ci, data);
        WRITE_DDR40_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE0_R_Nr(unit, ci, data);
        WRITE_DDR40_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE1_R_Pr(unit, ci, data);
        WRITE_DDR40_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE1_R_Nr(unit, ci, data);
#ifdef CONFIG_IPROC_DDR_ECC
        }
#endif
    }

    /* WR_DQ  */
    /* bit mode */
    READ_DDR40_PHY_CONTROL_REGS_VDL_WR_CHAN_CALIB_STATUSr(unit, ci, &data);
    setting_byte_sel =
        DDR40_GET_FIELD(data, DDR40_PHY_CONTROL_REGS, VDL_WR_CHAN_CALIB_STATUS,
                        WR_CHAN_CALIB_BYTE_SEL);
    setting_steps =
        DDR40_GET_FIELD(data, DDR40_PHY_CONTROL_REGS, VDL_WR_CHAN_CALIB_STATUS,
                        WR_CHAN_CALIB_TOTAL) >> 4;
    setting = setting_steps;
    if(SOC_IS_ARAD(unit)) {
        /* Same offset applies to both WLs */
        setting += SHMOO_CI02_WL0_OFFSET_WR_DQ;
    } else {
        if (wl == 0) {
            setting += SHMOO_CI02_WL0_OFFSET_WR_DQ;
        } else {
#ifdef CONFIG_IPROC_DDR_ECC
            if(wl == 2) {
                setting += SHMOO_CI02_WL0_OFFSET_WR_DQ;
            } else {
#endif
                setting += SHMOO_CI0_WL1_OFFSET_WR_DQ;
#ifdef CONFIG_IPROC_DDR_ECC
            }
#endif
        }
    }
    if (setting & 0x80000000) {
        setting = 0;
    } else if (setting > 63) {
        setting = 63;
    }
    DDR40_SET_FIELD(setting, DDR40_PHY_CONTROL_REGS, VDL_OVRIDE_BYTE_CTL,
                    BYTE_SEL, setting_byte_sel);
    data = SET_OVR_STEP(setting);
#ifdef CONFIG_IPROC_DDR_ECC
    if(wl == 2) {
        for (byte = 0; byte < 1; byte++) {
            for (bit = 0; bit < 4; bit++) {
                WRITE_DDR40_PHY_WORD_LANE_x_VDL_OVRIDE_BYTEy_BITz_Wr(wl, byte, bit,
                                                                     unit, ci,
                                                                     data);
            }
        }
    } else {
#endif
    for (byte = 0; byte < 2; byte++) {
        for (bit = 0; bit < 8; bit++) {
            WRITE_DDR40_PHY_WORD_LANE_x_VDL_OVRIDE_BYTEy_BITz_Wr(wl, byte, bit,
                                                                 unit, ci,
                                                                 data);
        }
    }
#ifdef CONFIG_IPROC_DDR_ECC
    }
#endif
    /* byte */
    data = SET_OVR_STEP(0);
    if (wl == 0) {
        WRITE_DDR40_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE0_Wr(unit, ci, data);
        WRITE_DDR40_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE1_Wr(unit, ci, data);
    } else {
#ifdef CONFIG_IPROC_DDR_ECC
        if(wl == 2) {
            WRITE_DDR40_PHY_ECC_LANE_VDL_OVRIDE_BYTE_Wr(unit, ci, data);
        } else {
#endif
            WRITE_DDR40_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE0_Wr(unit, ci, data);
            WRITE_DDR40_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE1_Wr(unit, ci, data);
#ifdef CONFIG_IPROC_DDR_ECC
        }
#endif
    }

    /* ADDR   */
    if ( wl == 0 ) {
        /* Hardware adjust ADDR 90' prior VDL
         * Because we cannot trust ADDR Calibration result as it reports 0 steps
         * We then will use WR_DQ 90' calibration result to adjust ADDR (by 90') to become 180' for the address
         * Excluded below and use WR_DQ instead
         * setting_byte_sel = DDR40_GET_FIELD(data, DDR40_PHY_CONTROL_REGS, VDL_CALIB_STATUS, CALIB_BYTE_SEL  ) ;
         * setting_steps = DDR40_GET_FIELD(data, DDR40_PHY_CONTROL_REGS, VDL_CALIB_STATUS, CALIB_TOTAL )>>4 ;
         * setting = (setting_steps*(1+setting_byte_sel));
         */ 
        READ_DDR40_PHY_CONTROL_REGS_VDL_WR_CHAN_CALIB_STATUSr(unit, ci, &data);
        setting_byte_sel =
            DDR40_GET_FIELD(data, DDR40_PHY_CONTROL_REGS,
                            VDL_WR_CHAN_CALIB_STATUS, WR_CHAN_CALIB_BYTE_SEL);
        setting_steps =
            DDR40_GET_FIELD(data, DDR40_PHY_CONTROL_REGS,
                            VDL_WR_CHAN_CALIB_STATUS,
                            WR_CHAN_CALIB_TOTAL) >> 4;
        setting = setting_steps;
        DDR40_SET_FIELD(setting, DDR40_PHY_CONTROL_REGS, VDL_OVRIDE_BYTE_CTL,
                        BYTE_SEL, setting_byte_sel);
        data = SET_OVR_STEP(setting);

        /* Temporary Address Patch */
        data = SET_OVR_STEP(40);
        WRITE_DDR40_PHY_CONTROL_REGS_VDL_OVRIDE_BIT_CTLr(unit, ci, data); 
    }

    /* Shmoo  */
    /* Do one dummy memory read to fix RTL bug on RD_FIFO write pointer coming out of reset bug */
    data = _test_mem_read(unit, (ci + wl), 0, NULL);
    
    /* Clear the FIFO error state */
    WRITE_DDR40_PHY_WORD_LANE_0_READ_FIFO_CLEARr(unit, ci, 0);
    WRITE_DDR40_PHY_WORD_LANE_1_READ_FIFO_CLEARr(unit, ci, 0);
#ifdef CONFIG_IPROC_DDR_ECC
    WRITE_DDR40_PHY_ECC_LANE_READ_FIFO_CLEARr(unit, ci, 0);
#endif
    return SOC_E_NONE;
}

#if 0 /* Uncomment this when required */
int
_shmoo_rd_en_rd_data_dly(int unit, int ci, int wl, int self_test, uint32 * result)
{
    int i, j;
    uint32 data;
    uint32 data_expected[NUM_DATA];
    int see_pass, see_fail, rd_en_pass, rd_en_fail, rd_en_size;
    uint32 dataB1p, dataB1n;

    int rd_data_dly_pass_sum = 0;
    int rd_data_dly_pass_count = 0;
    int rd_data_dly_pass_avg = 0;
    int rd_en_window_found = 0;

    _fill_in_data(data_expected);
    if (self_test == 0) {
        _test_mem_write(unit, (ci + wl), 0, data_expected);
    }

    /* Clear the FIFO error state */
    WRITE_DDR40_PHY_WORD_LANE_0_READ_FIFO_CLEARr(unit, ci, 0);
    WRITE_DDR40_PHY_WORD_LANE_1_READ_FIFO_CLEARr(unit, ci, 0);
    WRITE_DDR40_PHY_WORD_LANE_0_READ_FIFO_CLEARr(unit, ci, 1);
    WRITE_DDR40_PHY_WORD_LANE_1_READ_FIFO_CLEARr(unit, ci, 1);
    /* Clear any error status from previous run */
    WRITE_DDR40_PHY_WORD_LANE_0_READ_FIFO_STATUSr(unit, ci, 1);
    WRITE_DDR40_PHY_WORD_LANE_0_READ_FIFO_STATUSr(unit, ci, 0);
    WRITE_DDR40_PHY_WORD_LANE_1_READ_FIFO_STATUSr(unit, ci, 1);
    WRITE_DDR40_PHY_WORD_LANE_1_READ_FIFO_STATUSr(unit, ci, 0);
#ifdef CONFIG_IPROC_DDR_ECC
    /* Clear the FIFO error state */
    WRITE_DDR40_PHY_ECC_LANE_READ_FIFO_CLEARr(unit, ci, 0);
    WRITE_DDR40_PHY_ECC_LANE_READ_FIFO_CLEARr(unit, ci, 1);
    /* Clear any error status from previous run */
    WRITE_DDR40_PHY_ECC_LANE_READ_FIFO_STATUSr(unit, ci, 1);
    WRITE_DDR40_PHY_ECC_LANE_READ_FIFO_STATUSr(unit, ci, 0);
#endif

    /* RD_EN should be good by this point. We now find the best RD_DATA_DLY (for fast speed, we start from 3 and up) */
    rd_en_size = 0;

    if(wl == 0) {
        READ_DDR40_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE1_R_Pr(unit, ci, &dataB1p);
        READ_DDR40_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE1_R_Nr(unit, ci, &dataB1n);
        dataB1p =
            DDR40_GET_FIELD(dataB1p, DDR40_PHY_WORD_LANE_0, VDL_OVRIDE_BYTE1_R_P,
                            OVR_STEP);
        dataB1n =
            DDR40_GET_FIELD(dataB1n, DDR40_PHY_WORD_LANE_0, VDL_OVRIDE_BYTE1_R_N,
                            OVR_STEP);
    } else {
#ifdef CONFIG_IPROC_DDR_ECC
        if(wl == 2) {
            READ_DDR40_PHY_ECC_LANE_VDL_OVRIDE_BYTE_R_Pr(unit, ci, &dataB1p);
            READ_DDR40_PHY_ECC_LANE_VDL_OVRIDE_BYTE_R_Nr(unit, ci, &dataB1n);
            dataB1p =
                DDR40_GET_FIELD(dataB1p, DDR40_PHY_ECC_LANE, VDL_OVRIDE_BYTE_R_P,
                                OVR_STEP);
            dataB1n =
                DDR40_GET_FIELD(dataB1n, DDR40_PHY_ECC_LANE, VDL_OVRIDE_BYTE_R_N,
                                OVR_STEP);
        } else {
#endif
            READ_DDR40_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE1_R_Pr(unit, ci, &dataB1p);
            READ_DDR40_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE1_R_Nr(unit, ci, &dataB1n);
            dataB1p =
                DDR40_GET_FIELD(dataB1p, DDR40_PHY_WORD_LANE_1, VDL_OVRIDE_BYTE1_R_P,
                                OVR_STEP);
            dataB1n =
                DDR40_GET_FIELD(dataB1n, DDR40_PHY_WORD_LANE_1, VDL_OVRIDE_BYTE1_R_N,
                                OVR_STEP);
#ifdef CONFIG_IPROC_DDR_ECC
        }
#endif
    }

    /* Find Best RD_DATA_DLY  */
    for (j = 1; j < 8; ++j) {
        /* For every single RD_DATA_DLY, we shmoo RD_EN to find the largest window */
        data = j;
        if (wl == 0) {
            WRITE_DDR40_PHY_WORD_LANE_0_READ_DATA_DLYr(unit, ci, data);
        } else {
#ifdef CONFIG_IPROC_DDR_ECC
            if(wl == 2) {
                WRITE_DDR40_PHY_ECC_LANE_READ_DATA_DLYr(unit, ci, data);
            } else {
#endif
                WRITE_DDR40_PHY_WORD_LANE_1_READ_DATA_DLYr(unit, ci, data);
#ifdef CONFIG_IPROC_DDR_ECC
            }
#endif
        }
        see_pass = 0;
        see_fail = 0;
        rd_en_pass = 0;
        rd_en_fail = 0;
        rd_en_size = 0;
        rd_en_window_found = 0;
        for (i = 0; i < 64; ++i) {
            data = SET_OVR_STEP(i);

            /* RD_EN */
            if (wl == 0) {
                WRITE_DDR40_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE_RD_ENr(unit, ci, data);
                WRITE_DDR40_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE0_BIT_RD_ENr(unit,
                                                                        ci,
                                                                        data);
                WRITE_DDR40_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE1_BIT_RD_ENr(unit,
                                                                        ci,
                                                                        data);
            } else {
#ifdef CONFIG_IPROC_DDR_ECC
                if(wl == 2) {
                    WRITE_DDR40_PHY_ECC_LANE_VDL_OVRIDE_BYTE_RD_ENr(unit, ci, data);
                    WRITE_DDR40_PHY_ECC_LANE_VDL_OVRIDE_BYTE_BIT_RD_ENr(unit,
                                                                            ci,
                                                                            data);
                } else {
#endif
                    WRITE_DDR40_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE_RD_ENr(unit, ci, data);
                    WRITE_DDR40_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE0_BIT_RD_ENr(unit,
                                                                            ci,
                                                                            data);
                    WRITE_DDR40_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE1_BIT_RD_ENr(unit,
                                                                            ci,
                                                                            data);
#ifdef CONFIG_IPROC_DDR_ECC
                }
#endif
            }

            /* Clear the FIFO error state */
            WRITE_DDR40_PHY_WORD_LANE_0_READ_FIFO_CLEARr(unit, ci, 0);
            WRITE_DDR40_PHY_WORD_LANE_1_READ_FIFO_CLEARr(unit, ci, 0);
#ifdef CONFIG_IPROC_DDR_ECC
            WRITE_DDR40_PHY_ECC_LANE_READ_FIFO_CLEARr(unit, ci, 0);
#endif
            sal_usleep(9);
            if (self_test==0) {
                result[i] = _test_mem_read(unit, (ci+wl), 0, data_expected);
            } else if (self_test==2) {
                result[i] = _test_rd_data_fifo_status(unit, ci, wl, 1); 
            } else {
                result[i] =
                    _test_func_self_test_1(unit, ci, wl, 1, 0);
            }

            if (result[i] == 0) {       /* PASS */
                if (see_pass == 0) {
                    rd_en_pass = i;
                    see_pass = 1;
                }
            } else {
                if (see_pass==1 && see_fail==0) {
                    rd_en_fail = i;
                    see_fail = 1;
                }
            }

            if(rd_en_window_found == 0) {
                if (see_pass==1 && see_fail==1) { 
                    rd_en_size = rd_en_fail - rd_en_pass;
                    if(rd_en_size < 3) {
                        see_pass = 0;
                        see_fail = 0;
                    } else {
                        rd_data_dly_pass_sum += j;
                        rd_data_dly_pass_count++;
                        rd_en_window_found = 1;
                    }
                } else if (see_pass==1 && i==63) { 
                    rd_en_size = i - rd_en_pass;
                    if(rd_en_size < 3) {
                        see_pass = 0;
                        see_fail = 0;
                    } else {
                        rd_data_dly_pass_sum += j;
                        rd_data_dly_pass_count++;
                        rd_en_window_found = 1;
                    }
                }
            }
        }
    }

    if(rd_data_dly_pass_count != 0) {
        rd_data_dly_pass_avg = rd_data_dly_pass_sum / rd_data_dly_pass_count;
    } else {
        rd_data_dly_pass_avg = 1;
    }

    /* set RD_EN and RD_DATA_DLY */
    /* For every single RD_DATA_DLY, we shmoo RD_EN to find the largest window */
    data = rd_data_dly_pass_avg;
    if (wl == 0) {
        WRITE_DDR40_PHY_WORD_LANE_0_READ_DATA_DLYr(unit, ci, data);
    } else {
#ifdef CONFIG_IPROC_DDR_ECC
        if(wl == 2) {
            WRITE_DDR40_PHY_ECC_LANE_READ_DATA_DLYr(unit, ci, data);
        } else {
#endif
            WRITE_DDR40_PHY_WORD_LANE_1_READ_DATA_DLYr(unit, ci, data);
#ifdef CONFIG_IPROC_DDR_ECC
        }
#endif
    }

    /* Do one dummy memory read to fix RTL bug on RD_FIFO write pointer coming out of reset bug */
    data = _test_mem_read(unit, (ci + wl), 0, NULL);
    for (i = 0; i < 64; ++i) {
        data = SET_OVR_STEP(i);

        /* RD_EN */
        if (wl == 0) {
            WRITE_DDR40_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE_RD_ENr(unit, ci, data);
            WRITE_DDR40_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE0_BIT_RD_ENr(unit, ci,
                                                                    data);
            WRITE_DDR40_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE1_BIT_RD_ENr(unit, ci,
                                                                    data);
        } else {
#ifdef CONFIG_IPROC_DDR_ECC
            if(wl == 2) {
                WRITE_DDR40_PHY_ECC_LANE_VDL_OVRIDE_BYTE_RD_ENr(unit, ci, data);
                WRITE_DDR40_PHY_ECC_LANE_VDL_OVRIDE_BYTE_BIT_RD_ENr(unit,
                                                                        ci,
                                                                        data);
            } else {
#endif
                WRITE_DDR40_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE_RD_ENr(unit, ci, data);
                WRITE_DDR40_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE0_BIT_RD_ENr(unit, ci,
                                                                        data);
                WRITE_DDR40_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE1_BIT_RD_ENr(unit, ci,
                                                                        data);
#ifdef CONFIG_IPROC_DDR_ECC
            }
#endif
        }

        /* Clear the FIFO error state */
        WRITE_DDR40_PHY_WORD_LANE_0_READ_FIFO_CLEARr(unit, ci, 0);
        WRITE_DDR40_PHY_WORD_LANE_1_READ_FIFO_CLEARr(unit, ci, 0);
#ifdef CONFIG_IPROC_DDR_ECC
        WRITE_DDR40_PHY_ECC_LANE_READ_FIFO_CLEARr(unit, ci, 0);
#endif
        sal_usleep(9);
        if (self_test == 0) {
            result[i] = _test_mem_read(unit, (ci + wl), 0, data_expected);
        } else if (self_test==2) {
            result[i] = _test_rd_data_fifo_status(unit, ci, wl, 1);
        } else {
            result[i] =
                _test_func_self_test_1(unit, ci, wl, 1, 0);
        }
    }

    return SOC_E_NONE;
}
#endif

int
_shmoo_rd_data_dly(int unit, int ci, int wl, int self_test, uint32 * result)
{
    int i, j;
    uint32 data;
    uint32 data_expected[NUM_DATA];
    int see_pass, see_fail, rd_en_pass, rd_en_fail, rd_en_size;
    uint32 dataB1p, dataB1n;
    uint32 result0;

    int rd_data_dly_pass_sum = 0;
    int rd_data_dly_pass_count = 0;
    int rd_data_dly_pass_avg = 0;
    int rd_en_window_found = 0;

    _fill_in_data(data_expected);
    if (self_test == 0) {
        _test_mem_write(unit, (ci + wl), 0, data_expected);
    }

    /* Clear the FIFO error state */
    WRITE_DDR40_PHY_WORD_LANE_0_READ_FIFO_CLEARr(unit, ci, 0);
    WRITE_DDR40_PHY_WORD_LANE_1_READ_FIFO_CLEARr(unit, ci, 0);
    WRITE_DDR40_PHY_WORD_LANE_0_READ_FIFO_CLEARr(unit, ci, 1);
    WRITE_DDR40_PHY_WORD_LANE_1_READ_FIFO_CLEARr(unit, ci, 1);
    /* Clear any error status from previous run */
    WRITE_DDR40_PHY_WORD_LANE_0_READ_FIFO_STATUSr(unit, ci, 1);
    WRITE_DDR40_PHY_WORD_LANE_0_READ_FIFO_STATUSr(unit, ci, 0);
    WRITE_DDR40_PHY_WORD_LANE_1_READ_FIFO_STATUSr(unit, ci, 1);
    WRITE_DDR40_PHY_WORD_LANE_1_READ_FIFO_STATUSr(unit, ci, 0);
#ifdef CONFIG_IPROC_DDR_ECC
    /* Clear the FIFO error state */
    WRITE_DDR40_PHY_ECC_LANE_READ_FIFO_CLEARr(unit, ci, 0);
    WRITE_DDR40_PHY_ECC_LANE_READ_FIFO_CLEARr(unit, ci, 1);
    /* Clear any error status from previous run */
    WRITE_DDR40_PHY_ECC_LANE_READ_FIFO_STATUSr(unit, ci, 1);
    WRITE_DDR40_PHY_ECC_LANE_READ_FIFO_STATUSr(unit, ci, 0);
#endif

    /* RD_EN should be good by this point. We now find the best RD_DATA_DLY (for fast speed, we start from 3 and up) */
    rd_en_size = 0;

    if(wl == 0) {
        READ_DDR40_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE1_R_Pr(unit, ci, &dataB1p);
        READ_DDR40_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE1_R_Nr(unit, ci, &dataB1n);
        dataB1p =
            DDR40_GET_FIELD(dataB1p, DDR40_PHY_WORD_LANE_0, VDL_OVRIDE_BYTE1_R_P,
                            OVR_STEP);
        dataB1n =
            DDR40_GET_FIELD(dataB1n, DDR40_PHY_WORD_LANE_0, VDL_OVRIDE_BYTE1_R_N,
                            OVR_STEP);
    } else {
#ifdef CONFIG_IPROC_DDR_ECC
        if(wl == 2) {
            READ_DDR40_PHY_ECC_LANE_VDL_OVRIDE_BYTE_R_Pr(unit, ci, &dataB1p);
            READ_DDR40_PHY_ECC_LANE_VDL_OVRIDE_BYTE_R_Nr(unit, ci, &dataB1n);
            dataB1p =
                DDR40_GET_FIELD(dataB1p, DDR40_PHY_ECC_LANE, VDL_OVRIDE_BYTE_R_P,
                                OVR_STEP);
            dataB1n =
                DDR40_GET_FIELD(dataB1n, DDR40_PHY_ECC_LANE, VDL_OVRIDE_BYTE_R_N,
                                OVR_STEP);
        } else {
#endif
            READ_DDR40_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE1_R_Pr(unit, ci, &dataB1p);
            READ_DDR40_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE1_R_Nr(unit, ci, &dataB1n);
            dataB1p =
                DDR40_GET_FIELD(dataB1p, DDR40_PHY_WORD_LANE_1, VDL_OVRIDE_BYTE1_R_P,
                                OVR_STEP);
            dataB1n =
                DDR40_GET_FIELD(dataB1n, DDR40_PHY_WORD_LANE_1, VDL_OVRIDE_BYTE1_R_N,
                                OVR_STEP);
#ifdef CONFIG_IPROC_DDR_ECC
        }
#endif
    }

    /* Find Best RD_DATA_DLY  */
    for (j = 1; j < 8; ++j) {
        /* For every single RD_DATA_DLY, we shmoo RD_EN to find the largest window */
        data = j;
        if (wl == 0) {
            WRITE_DDR40_PHY_WORD_LANE_0_READ_DATA_DLYr(unit, ci, data);
        } else {
#ifdef CONFIG_IPROC_DDR_ECC
            if(wl == 2) {
                WRITE_DDR40_PHY_ECC_LANE_READ_DATA_DLYr(unit, ci, data);
            } else {
#endif
                WRITE_DDR40_PHY_WORD_LANE_1_READ_DATA_DLYr(unit, ci, data);
#ifdef CONFIG_IPROC_DDR_ECC
            }
#endif
        }
        see_pass = 0;
        see_fail = 0;
        rd_en_pass = 0;
        rd_en_fail = 0;
        rd_en_size = 0;
        rd_en_window_found = 0;
        printf ("RD_DATA_DLY Iter: %d\t", j); 
        for (i = 0; i < 64; ++i) {
            data = SET_OVR_STEP(i);

            /* RD_EN */
            if (wl == 0) {
                WRITE_DDR40_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE_RD_ENr(unit, ci, data);
                WRITE_DDR40_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE0_BIT_RD_ENr(unit,
                                                                        ci,
                                                                        data);
                WRITE_DDR40_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE1_BIT_RD_ENr(unit,
                                                                        ci,
                                                                        data);
            } else {
#ifdef CONFIG_IPROC_DDR_ECC
                if(wl == 2) {
                    WRITE_DDR40_PHY_ECC_LANE_VDL_OVRIDE_BYTE_RD_ENr(unit, ci, data);
                    WRITE_DDR40_PHY_ECC_LANE_VDL_OVRIDE_BYTE_BIT_RD_ENr(unit,
                                                                            ci,
                                                                            data);
                } else {
#endif
                    WRITE_DDR40_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE_RD_ENr(unit, ci, data);
                    WRITE_DDR40_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE0_BIT_RD_ENr(unit,
                                                                            ci,
                                                                            data);
                    WRITE_DDR40_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE1_BIT_RD_ENr(unit,
                                                                            ci,
                                                                            data);
#ifdef CONFIG_IPROC_DDR_ECC
                }
#endif
            }

            /* Clear the FIFO error state */
            WRITE_DDR40_PHY_WORD_LANE_0_READ_FIFO_CLEARr(unit, ci, 0);
            WRITE_DDR40_PHY_WORD_LANE_1_READ_FIFO_CLEARr(unit, ci, 0);
#ifdef CONFIG_IPROC_DDR_ECC
            WRITE_DDR40_PHY_ECC_LANE_READ_FIFO_CLEARr(unit, ci, 0);
#endif
            sal_usleep(9);
            if (self_test==0) {
                result[i] = _test_mem_read(unit, (ci+wl), 0, data_expected);
            } else if (self_test==2) {
                result[i] = _test_rd_data_fifo_status(unit, ci, wl, 1); 
            } else {
                result[i] =
                    _test_func_self_test_1(unit, ci, wl, 1, 3);
            }
            if (result[i] == 0) {       /* PASS */
                printf ("+"); 
                if (see_pass == 0) {
                    rd_en_pass = i;
                    see_pass = 1;
                }
            } else {
                printf ("-"); 
                if (see_pass==1 && see_fail==0) {
                    rd_en_fail = i;
                    see_fail = 1;
                }
            }

            if(rd_en_window_found == 0) {
                if (see_pass==1 && see_fail==1) { 
                    rd_en_size = rd_en_fail - rd_en_pass;
                    if(rd_en_size < 3) {
                        see_pass = 0;
                        see_fail = 0;
                    } else {
                        rd_data_dly_pass_sum += j;
                        rd_data_dly_pass_count++;
                        rd_en_window_found = 1;
                    }
                } else if (see_pass==1 && i==63) { 
                    rd_en_size = i - rd_en_pass;
                    if(rd_en_size < 3) {
                        see_pass = 0;
                        see_fail = 0;
                    } else {
                        rd_data_dly_pass_sum += j;
                        rd_data_dly_pass_count++;
                        rd_en_window_found = 1;
                    }
                }
            }
        }
        printf ("\n"); 
        if(rd_data_dly_pass_count != 0) {
            rd_data_dly_pass_avg = rd_data_dly_pass_sum + 1;
            break;
        }
        printf ("RD_DATA_DLY Iter: %d\tCount: %d\tSum: %2d\n", j, rd_data_dly_pass_count, rd_data_dly_pass_sum); 
    }
/*
    if(rd_data_dly_pass_count != 0) {
        rd_data_dly_pass_avg = rd_data_dly_pass_sum / rd_data_dly_pass_count;
    } else {
        rd_data_dly_pass_avg = 1;
    } */

    if(rd_data_dly_pass_count == 0) {
        rd_data_dly_pass_avg = 4;
        printf ("Did not find valid RD_DATA_DELAY. Forcing RD_DATA_DELAY = %d\n", rd_data_dly_pass_avg);
    } else {
      /*  if(wl == 0) {
        rd_data_dly_pass_avg = 7;
        printf("Found valid RD_DATA_DELAY but forcing RD_DATA_DELAY = %d\n", rd_data_dly_pass_avg));
        } */
      /*  if(wl == 1) {
            rd_data_dly_pass_avg = result0;
            printf("Found valid RD_DATA_DELAY but copying WL0 RD_DATA_DELAY = %d\n", rd_data_dly_pass_avg));
        } */
    }

    /* set RD_EN and RD_DATA_DLY */
    /* For every single RD_DATA_DLY, we shmoo RD_EN to find the largest window */
    data = rd_data_dly_pass_avg;
    if (wl == 0) {
        printf ("Switching to RD_DATA_DELAY Step  : %2d (WL = 0)\n", data);
        // result0 = data;
        WRITE_DDR40_PHY_WORD_LANE_0_READ_DATA_DLYr(unit, ci, data);
    } else {
#ifdef CONFIG_IPROC_DDR_ECC
        if(wl == 2) {
            READ_DDR40_PHY_WORD_LANE_1_READ_DATA_DLYr(unit, ci, &result0);
            result0 =
                DDR40_GET_FIELD(result0, DDR40_PHY_WORD_LANE_1, READ_DATA_DLY, RD_DATA_DLY);
            if(data > result0) {
                printf ("Equalizing to RD_DATA_DELAY Step : %2d (All WLs)\n", data);
                WRITE_DDR40_PHY_WORD_LANE_0_READ_DATA_DLYr(unit, ci, data);
                WRITE_DDR40_PHY_WORD_LANE_1_READ_DATA_DLYr(unit, ci, data);
                WRITE_DDR40_PHY_ECC_LANE_READ_DATA_DLYr(unit, ci, data);
            } else {
                printf ("Copying WL1 RD_DATA_DELAY Step   : %2d (WL = 2)\n", result0);
                WRITE_DDR40_PHY_ECC_LANE_READ_DATA_DLYr(unit, ci, result0);
            }
        } else {
#endif
            READ_DDR40_PHY_WORD_LANE_0_READ_DATA_DLYr(unit, ci, &result0);
            result0 =
                DDR40_GET_FIELD(result0, DDR40_PHY_WORD_LANE_0, READ_DATA_DLY, RD_DATA_DLY);
            if(data > result0) {
                printf ("Equalizing to RD_DATA_DELAY Step : %2d (Both WLs)\n", data);
                WRITE_DDR40_PHY_WORD_LANE_0_READ_DATA_DLYr(unit, ci, data);
                WRITE_DDR40_PHY_WORD_LANE_1_READ_DATA_DLYr(unit, ci, data);
            } else {
                printf ("Copying WL0 RD_DATA_DELAY Step   : %2d (WL = 1)\n", result0);
                WRITE_DDR40_PHY_WORD_LANE_1_READ_DATA_DLYr(unit, ci, result0);
            }
#ifdef CONFIG_IPROC_DDR_ECC
        }
#endif
    }

    /* Clear the FIFO error state */
    WRITE_DDR40_PHY_WORD_LANE_0_READ_FIFO_CLEARr(unit, ci, 0);
    WRITE_DDR40_PHY_WORD_LANE_1_READ_FIFO_CLEARr(unit, ci, 0);
#ifdef CONFIG_IPROC_DDR_ECC
    WRITE_DDR40_PHY_ECC_LANE_READ_FIFO_CLEARr(unit, ci, 0);
#endif

    return SOC_E_NONE;
}

STATIC int _shmoo_rd_dq(int unit, int ci, int wl)
{
    uint32 data;
    uint32 init_rd_dqs_step;
    uint32 init_rd_en_step;
    uint32 new_rd_dqs_step;
    uint32 new_rd_en_step;
    uint32 new_rd_dq_step0 = 0xFFFFFFFF;
    uint32 new_rd_dq_step1 = 0xFFFFFFFF;
    uint32 result;
    int byte, bit;
    int i;
    int fail_count0 = 0;
    int pass_count0 = 0;
    int max_pass0 = 0;
	int first_fail0 = 63;
    int fail_count1 = 0;
    int pass_count1 = 0;
    int max_pass1 = 0;
	int first_fail1 = 63;
	uint32 new_rd_en_avg_step;
    
    if(wl == 0) {
        READ_DDR40_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE0_R_Pr(unit, ci, &data);
        init_rd_dqs_step =
            DDR40_GET_FIELD(data, DDR40_PHY_WORD_LANE_0, VDL_OVRIDE_BYTE0_R_P,
                            OVR_STEP);
        READ_DDR40_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE0_BIT_RD_ENr(unit, ci, &data);
        init_rd_en_step =
            DDR40_GET_FIELD(data, DDR40_PHY_WORD_LANE_0, VDL_OVRIDE_BYTE0_BIT_RD_EN,
                            OVR_STEP);
        new_rd_dqs_step = 0;
        if(init_rd_en_step > (init_rd_dqs_step >> RD_EN_DQS_CORROLATION)) {
            new_rd_en_step = init_rd_en_step - (init_rd_dqs_step >> RD_EN_DQS_CORROLATION);
        } else {
            new_rd_en_step = 0;
        }
        new_rd_en_avg_step = new_rd_en_step;
        data = SET_OVR_STEP(new_rd_dqs_step);
        WRITE_DDR40_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE0_R_Pr(unit, ci, data);
        WRITE_DDR40_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE0_R_Nr(unit, ci, data);
        data = SET_OVR_STEP(new_rd_en_step);
        WRITE_DDR40_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE0_BIT_RD_ENr(unit, ci, data);
        printf("Initial RD_DQS Setting   (Byte 0): %2d\n", init_rd_dqs_step);
        printf("Initial RD_EN Setting    (Byte 0): %2d\n", init_rd_en_step);
        printf("New RD_DQS Setting       (Byte 0): %2d\n", new_rd_dqs_step);
        printf("New RD_EN Setting        (Byte 0): %2d\n", new_rd_en_step);
        READ_DDR40_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE1_R_Pr(unit, ci, &data);
        init_rd_dqs_step =
            DDR40_GET_FIELD(data, DDR40_PHY_WORD_LANE_0, VDL_OVRIDE_BYTE1_R_P,
                            OVR_STEP);
        READ_DDR40_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE1_BIT_RD_ENr(unit, ci, &data);
        init_rd_en_step =
            DDR40_GET_FIELD(data, DDR40_PHY_WORD_LANE_0, VDL_OVRIDE_BYTE1_BIT_RD_EN,
                            OVR_STEP);
        new_rd_dqs_step = 0;
        if(init_rd_en_step > (init_rd_dqs_step >> RD_EN_DQS_CORROLATION)) {
            new_rd_en_step = init_rd_en_step - (init_rd_dqs_step >> RD_EN_DQS_CORROLATION);
        } else {
            new_rd_en_step = 0;
        }
        new_rd_en_avg_step = (new_rd_en_avg_step + new_rd_en_step) >> 1;
        data = SET_OVR_STEP(new_rd_dqs_step);
        WRITE_DDR40_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE1_R_Pr(unit, ci, data);
        WRITE_DDR40_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE1_R_Nr(unit, ci, data);
        data = SET_OVR_STEP(new_rd_en_step);
        WRITE_DDR40_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE1_BIT_RD_ENr(unit, ci, data);
        printf("Initial RD_DQS Setting   (Byte 1): %2d\n", init_rd_dqs_step);
        printf("Initial RD_EN Setting    (Byte 1): %2d\n", init_rd_en_step);
        printf("New RD_DQS Setting       (Byte 1): %2d\n", new_rd_dqs_step);
        printf("New RD_EN Setting        (Byte 1): %2d\n", new_rd_en_step);
        data = SET_OVR_STEP(new_rd_en_avg_step);
        WRITE_DDR40_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE_RD_ENr(unit, ci, data);
    } else {
#ifdef CONFIG_IPROC_DDR_ECC
        if(wl == 2) {
            READ_DDR40_PHY_ECC_LANE_VDL_OVRIDE_BYTE_R_Pr(unit, ci, &data);
            init_rd_dqs_step =
                DDR40_GET_FIELD(data, DDR40_PHY_ECC_LANE, VDL_OVRIDE_BYTE_R_P,
                                OVR_STEP);
            READ_DDR40_PHY_ECC_LANE_VDL_OVRIDE_BYTE_BIT_RD_ENr(unit, ci, &data);
            init_rd_en_step =
                DDR40_GET_FIELD(data, DDR40_PHY_ECC_LANE, VDL_OVRIDE_BYTE_BIT_RD_EN,
                                OVR_STEP);
            new_rd_dqs_step = 0;
            if(init_rd_en_step > (init_rd_dqs_step >> RD_EN_DQS_CORROLATION)) {
                new_rd_en_step = init_rd_en_step - (init_rd_dqs_step >> RD_EN_DQS_CORROLATION);
            } else {
                new_rd_en_step = 0;
            }
            new_rd_en_avg_step = new_rd_en_step;
            data = SET_OVR_STEP(new_rd_dqs_step);
            WRITE_DDR40_PHY_ECC_LANE_VDL_OVRIDE_BYTE_R_Pr(unit, ci, data);
            WRITE_DDR40_PHY_ECC_LANE_VDL_OVRIDE_BYTE_R_Nr(unit, ci, data);
            data = SET_OVR_STEP(new_rd_en_step);
            WRITE_DDR40_PHY_ECC_LANE_VDL_OVRIDE_BYTE_BIT_RD_ENr(unit, ci, data);
            printf("Initial RD_DQS Setting   (Byte 0): %2d\n", init_rd_dqs_step);
            printf("Initial RD_EN Setting    (Byte 0): %2d\n", init_rd_en_step);
            printf("New RD_DQS Setting       (Byte 0): %2d\n", new_rd_dqs_step);
            printf("New RD_EN Setting        (Byte 0): %2d\n", new_rd_en_step);
            data = SET_OVR_STEP(new_rd_en_avg_step);
            WRITE_DDR40_PHY_ECC_LANE_VDL_OVRIDE_BYTE_RD_ENr(unit, ci, data);
        } else {
#endif
            READ_DDR40_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE0_R_Pr(unit, ci, &data);
            init_rd_dqs_step =
                DDR40_GET_FIELD(data, DDR40_PHY_WORD_LANE_1, VDL_OVRIDE_BYTE0_R_P,
                                OVR_STEP);
            READ_DDR40_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE0_BIT_RD_ENr(unit, ci, &data);
            init_rd_en_step =
                DDR40_GET_FIELD(data, DDR40_PHY_WORD_LANE_1, VDL_OVRIDE_BYTE0_BIT_RD_EN,
                                OVR_STEP);
            new_rd_dqs_step = 0;
            if(init_rd_en_step > (init_rd_dqs_step >> RD_EN_DQS_CORROLATION)) {
                new_rd_en_step = init_rd_en_step - (init_rd_dqs_step >> RD_EN_DQS_CORROLATION);
            } else {
                new_rd_en_step = 0;
            }
            new_rd_en_avg_step = new_rd_en_step;
            data = SET_OVR_STEP(new_rd_dqs_step);
            WRITE_DDR40_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE0_R_Pr(unit, ci, data);
            WRITE_DDR40_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE0_R_Nr(unit, ci, data);
            data = SET_OVR_STEP(new_rd_en_step);
            WRITE_DDR40_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE0_BIT_RD_ENr(unit, ci, data);
            printf("Initial RD_DQS Setting   (Byte 0): %2d\n", init_rd_dqs_step);
            printf("Initial RD_EN Setting    (Byte 0): %2d\n", init_rd_en_step);
            printf("New RD_DQS Setting       (Byte 0): %2d\n", new_rd_dqs_step);
            printf("New RD_EN Setting        (Byte 0): %2d\n", new_rd_en_step);
            READ_DDR40_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE1_R_Pr(unit, ci, &data);
            init_rd_dqs_step =
                DDR40_GET_FIELD(data, DDR40_PHY_WORD_LANE_1, VDL_OVRIDE_BYTE1_R_P,
                                OVR_STEP);
            READ_DDR40_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE1_BIT_RD_ENr(unit, ci, &data);
            init_rd_en_step =
                DDR40_GET_FIELD(data, DDR40_PHY_WORD_LANE_1, VDL_OVRIDE_BYTE1_BIT_RD_EN,
                                OVR_STEP);
            new_rd_dqs_step = 0;
            if(init_rd_en_step > (init_rd_dqs_step >> RD_EN_DQS_CORROLATION)) {
                new_rd_en_step = init_rd_en_step - (init_rd_dqs_step >> RD_EN_DQS_CORROLATION);
            } else {
                new_rd_en_step = 0;
            }
            new_rd_en_avg_step = (new_rd_en_avg_step + new_rd_en_step) >> 1;
            data = SET_OVR_STEP(new_rd_dqs_step);
            WRITE_DDR40_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE1_R_Pr(unit, ci, data);
            WRITE_DDR40_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE1_R_Nr(unit, ci, data);
            data = SET_OVR_STEP(new_rd_en_step);
            WRITE_DDR40_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE1_BIT_RD_ENr(unit, ci, data);
            printf("Initial RD_DQS Setting   (Byte 1): %2d\n", init_rd_dqs_step);
            printf("Initial RD_EN Setting    (Byte 1): %2d\n", init_rd_en_step);
            printf("New RD_DQS Setting       (Byte 1): %2d\n", new_rd_dqs_step);
            printf("New RD_EN Setting        (Byte 1): %2d\n", new_rd_en_step);
            data = SET_OVR_STEP(new_rd_en_avg_step);
            WRITE_DDR40_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE_RD_ENr(unit, ci, data);
#ifdef CONFIG_IPROC_DDR_ECC
        }
#endif
    }
    
    for (i = 0; i < 64; ++i) {
        data = SET_OVR_STEP(i);

        /* RD_DQ */
#ifdef CONFIG_IPROC_DDR_ECC
        if(wl == 2) {
            for (byte = 0; byte < 1; byte++) {
                for (bit = 0; bit < 4; bit++) {
                    WRITE_DDR40_PHY_WORD_LANE_x_VDL_OVRIDE_BYTEy_BITz_R_Pr(wl,
                                                                           byte,
                                                                           bit,
                                                                           unit,
                                                                           ci,
                                                                           data);
                    WRITE_DDR40_PHY_WORD_LANE_x_VDL_OVRIDE_BYTEy_BITz_R_Nr(wl,
                                                                           byte,
                                                                           bit,
                                                                           unit,
                                                                           ci,
                                                                           data);
                }
            }
        } else {
#endif
            for (byte = 0; byte < 2; byte++) {
                for (bit = 0; bit < 8; bit++) {
                    WRITE_DDR40_PHY_WORD_LANE_x_VDL_OVRIDE_BYTEy_BITz_R_Pr(wl,
                                                                           byte,
                                                                           bit,
                                                                           unit,
                                                                           ci,
                                                                           data);
                    WRITE_DDR40_PHY_WORD_LANE_x_VDL_OVRIDE_BYTEy_BITz_R_Nr(wl,
                                                                           byte,
                                                                           bit,
                                                                           unit,
                                                                           ci,
                                                                           data);
                }
            }
#ifdef CONFIG_IPROC_DDR_ECC
        }
#endif

        /* Clear the FIFO error state */
        WRITE_DDR40_PHY_WORD_LANE_0_READ_FIFO_CLEARr(unit, ci, 0);
        WRITE_DDR40_PHY_WORD_LANE_1_READ_FIFO_CLEARr(unit, ci, 0);
#ifdef CONFIG_IPROC_DDR_ECC
        WRITE_DDR40_PHY_ECC_LANE_READ_FIFO_CLEARr(unit, ci, 0);
#endif
        sal_usleep(9);
        result = _test_func_self_test_1(unit, ci, wl, 1, 0);

        if(new_rd_dq_step0 == 0xFFFFFFFF) {
            if((result & 0x00FF) != 0) {
                if(first_fail0 == 63) {
                    first_fail0 = i;
                }
                if(max_pass0 < pass_count0) {
                    max_pass0 = pass_count0;
                }
                pass_count0 = 0;
                fail_count0++;
            } else {
                fail_count0 = 0;
                pass_count0++;
            }
            if((max_pass0 >= 5) && (fail_count0 == 5)) {
                new_rd_dq_step0 = i;
            }
        }
        if(new_rd_dq_step1 == 0xFFFFFFFF) {
            if((result & 0xFF00) != 0) {
                if(first_fail1 == 63) {
                    first_fail1 = i;
                }
                if(max_pass1 < pass_count1) {
                    max_pass1 = pass_count1;
                }
                pass_count1 = 0;
                fail_count1++;
            } else {
                fail_count1 = 0;
                pass_count1++;
            }
            if((max_pass1 >= 5) && (fail_count1 == 5)) {
                new_rd_dq_step1 = i;
            }
        }
        if((new_rd_dq_step0 != 0xFFFFFFFF) && (new_rd_dq_step1 != 0xFFFFFFFF)) {
            break;
        }
    }
    if(i == 64) {
        if(new_rd_dq_step0 == 0xFFFFFFFF) {
            new_rd_dq_step0 = first_fail0;
        }
        if(new_rd_dq_step1 == 0xFFFFFFFF) {
            new_rd_dq_step1 = first_fail1;
        }
    }

#ifdef CONFIG_IPROC_DDR_ECC
    if(wl == 2) {
        printf("Switching to RD_DQ Step (Byte 0) : %2d\n", new_rd_dq_step0);
    } else {
#endif
        printf("Switching to RD_DQ Step (Byte 0) : %2d\n", new_rd_dq_step0);
        printf("Switching to RD_DQ Step (Byte 1) : %2d\n", new_rd_dq_step1);
#ifdef CONFIG_IPROC_DDR_ECC
    }
#endif

#ifdef CONFIG_IPROC_DDR_ECC
    if(wl == 2) {
        data = SET_OVR_STEP(new_rd_dq_step0);
        for (bit = 0; bit < 4; bit++) {
            WRITE_DDR40_PHY_WORD_LANE_x_VDL_OVRIDE_BYTEy_BITz_R_Pr(wl, 0,
                                                                   bit, unit,
                                                                   ci, data);
            WRITE_DDR40_PHY_WORD_LANE_x_VDL_OVRIDE_BYTEy_BITz_R_Nr(wl, 0,
                                                                   bit, unit,
                                                                   ci, data);
        }
    } else {
#endif
        data = SET_OVR_STEP(new_rd_dq_step0);
        for (bit = 0; bit < 8; bit++) {
            WRITE_DDR40_PHY_WORD_LANE_x_VDL_OVRIDE_BYTEy_BITz_R_Pr(wl, 0,
                                                                   bit, unit,
                                                                   ci, data);
            WRITE_DDR40_PHY_WORD_LANE_x_VDL_OVRIDE_BYTEy_BITz_R_Nr(wl, 0,
                                                                   bit, unit,
                                                                   ci, data);
        }
        data = SET_OVR_STEP(new_rd_dq_step1);
        for (bit = 0; bit < 8; bit++) {
            WRITE_DDR40_PHY_WORD_LANE_x_VDL_OVRIDE_BYTEy_BITz_R_Pr(wl, 1,
                                                                   bit, unit,
                                                                   ci, data);
            WRITE_DDR40_PHY_WORD_LANE_x_VDL_OVRIDE_BYTEy_BITz_R_Nr(wl, 1,
                                                                   bit, unit,
                                                                   ci, data);
        }
#ifdef CONFIG_IPROC_DDR_ECC
    }
#endif

    WRITE_DDR40_PHY_WORD_LANE_0_READ_FIFO_CLEARr(unit, ci, 0);
    WRITE_DDR40_PHY_WORD_LANE_1_READ_FIFO_CLEARr(unit, ci, 0);
#ifdef CONFIG_IPROC_DDR_ECC
    WRITE_DDR40_PHY_ECC_LANE_READ_FIFO_CLEARr(unit, ci, 0);
#endif
    return SOC_E_NONE;

}

STATIC int _shmoo_rd_en_rd_dq(int unit, int ci, int wl, int self_test, vref_word_shmoo *vwsPtr)
{
    uint32 data;
    int byte, bit;
    int i, j;

    uint32 data_expected[NUM_DATA];
	//printf("_shmoo_rd_en_rd_dq: Enter\n");
    _fill_in_data(data_expected);
    if (self_test == 0) {
        _test_mem_write(unit, (ci + wl), 0, data_expected);
    }
  	//printf("_shmoo_rd_en_rd_dq: mid point\n");  
    for (i = 0; i < 64; ++i) {
        data = SET_OVR_STEP(i);

        /* RD_DQ */
#ifdef CONFIG_IPROC_DDR_ECC
        if(wl == 2) {
            for (byte = 0; byte < 1; byte++) {
                for (bit = 0; bit < 4; bit++) {
                    WRITE_DDR40_PHY_WORD_LANE_x_VDL_OVRIDE_BYTEy_BITz_R_Pr(wl,
                                                                           byte,
                                                                           bit,
                                                                           unit,
                                                                           ci,
                                                                           data);
                    WRITE_DDR40_PHY_WORD_LANE_x_VDL_OVRIDE_BYTEy_BITz_R_Nr(wl,
                                                                           byte,
                                                                           bit,
                                                                           unit,
                                                                           ci,
                                                                           data);
                }
            }
        } else {
#endif
            for (byte = 0; byte < 2; byte++) {
                for (bit = 0; bit < 8; bit++) {
                    WRITE_DDR40_PHY_WORD_LANE_x_VDL_OVRIDE_BYTEy_BITz_R_Pr(wl,
                                                                           byte,
                                                                           bit,
                                                                           unit,
                                                                           ci,
                                                                           data);
                    WRITE_DDR40_PHY_WORD_LANE_x_VDL_OVRIDE_BYTEy_BITz_R_Nr(wl,
                                                                           byte,
                                                                           bit,
                                                                           unit,
                                                                           ci,
                                                                           data);
                }
            }
#ifdef CONFIG_IPROC_DDR_ECC
        }
#endif

        for (j = 0; j < 64; ++j) {
            data = SET_OVR_STEP(j);

            /* RD_EN */
            if (wl == 0) {
                WRITE_DDR40_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE_RD_ENr(unit, ci, data);
                WRITE_DDR40_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE0_BIT_RD_ENr(unit, ci,
                                                                        data);
                WRITE_DDR40_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE1_BIT_RD_ENr(unit, ci,
                                                                        data);
            } else {
#ifdef CONFIG_IPROC_DDR_ECC
                if(wl == 2) {
                    WRITE_DDR40_PHY_ECC_LANE_VDL_OVRIDE_BYTE_RD_ENr(unit, ci, data);
                    WRITE_DDR40_PHY_ECC_LANE_VDL_OVRIDE_BYTE_BIT_RD_ENr(unit, ci,
                                                                            data);
                } else {
#endif
                    WRITE_DDR40_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE_RD_ENr(unit, ci, data);
                    WRITE_DDR40_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE0_BIT_RD_ENr(unit, ci,
                                                                            data);
                    WRITE_DDR40_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE1_BIT_RD_ENr(unit, ci,
                                                                            data);
            }
#ifdef CONFIG_IPROC_DDR_ECC
        }
#endif

            /* Clear the FIFO error state */
            WRITE_DDR40_PHY_WORD_LANE_0_READ_FIFO_CLEARr(unit, ci, 0);
            WRITE_DDR40_PHY_WORD_LANE_1_READ_FIFO_CLEARr(unit, ci, 0);
#ifdef CONFIG_IPROC_DDR_ECC
            WRITE_DDR40_PHY_ECC_LANE_READ_FIFO_CLEARr(unit, ci, 0);
#endif
            sal_usleep(9);
            if (self_test == 0) {
                (*vwsPtr)[i].raw_result[j] = _test_mem_read(unit, (ci + wl), 0, data_expected);
            } else if (self_test==2) {
                (*vwsPtr)[i].raw_result[j] = _test_rd_data_fifo_status(unit, ci, wl, 1);
            } else {
                if(SOC_IS_KATANA(unit) || SOC_IS_CALADAN3(unit)) {
                    (*vwsPtr)[i].raw_result[j] =
                        _test_func_self_test_1(unit, ci, wl, 1, 0);
                } else {
					//printf("_shmoo_rd_en_rd_dq: before calling _test_func_self_test_1\n");
					//printf("_shmoo_rd_en_rd_dq: ptr: 0x%08x, i: %d, j: %d\n", (*vwsPtr)[i].raw_result, i, j);
                    (*vwsPtr)[i].raw_result[j] = _test_func_self_test_1(unit, ci, wl, 1, 0); /* MPR = 4 */
					//printf("_shmoo_rd_en_rd_dq: after calling _test_func_self_test_1\n");
                }
            }
        }
    }

    WRITE_DDR40_PHY_WORD_LANE_0_READ_FIFO_CLEARr(unit, ci, 0);
    WRITE_DDR40_PHY_WORD_LANE_1_READ_FIFO_CLEARr(unit, ci, 0);
#ifdef CONFIG_IPROC_DDR_ECC
    WRITE_DDR40_PHY_ECC_LANE_READ_FIFO_CLEARr(unit, ci, 0);
#endif
	//printf("_shmoo_rd_en_rd_dq: Exit\n");
    return SOC_E_NONE;

}

#if 0 /* Uncomment this when required */
STATIC int _shmoo_rd_en(int unit, int ci, int wl, int self_test, uint32 * result)
{
    uint32 data;
    int i;

    uint32 data_expected[NUM_DATA];

    _fill_in_data(data_expected);
    if (self_test == 0) {
        _test_mem_write(unit, (ci + wl), 0, data_expected);
    }
    
    for (i = 0; i < 64; ++i) {
        data = SET_OVR_STEP(i);

        /* RD_EN */
        if (wl == 0) {
            WRITE_DDR40_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE_RD_ENr(unit, ci, data);
            WRITE_DDR40_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE0_BIT_RD_ENr(unit, ci,
                                                                    data);
            WRITE_DDR40_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE1_BIT_RD_ENr(unit, ci,
                                                                    data);
        } else {
#ifdef CONFIG_IPROC_DDR_ECC
            if(wl == 2) {
                WRITE_DDR40_PHY_ECC_LANE_VDL_OVRIDE_BYTE_RD_ENr(unit, ci, data);
                WRITE_DDR40_PHY_ECC_LANE_VDL_OVRIDE_BYTE_BIT_RD_ENr(unit, ci,
                                                                        data);
            } else {
#endif
                WRITE_DDR40_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE_RD_ENr(unit, ci, data);
                WRITE_DDR40_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE0_BIT_RD_ENr(unit, ci,
                                                                        data);
                WRITE_DDR40_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE1_BIT_RD_ENr(unit, ci,
                                                                        data);
#ifdef CONFIG_IPROC_DDR_ECC
            }
#endif
        }

        /* Clear the FIFO error state */
        WRITE_DDR40_PHY_WORD_LANE_0_READ_FIFO_CLEARr(unit, ci, 0);
        WRITE_DDR40_PHY_WORD_LANE_1_READ_FIFO_CLEARr(unit, ci, 0);
#ifdef CONFIG_IPROC_DDR_ECC
        WRITE_DDR40_PHY_ECC_LANE_READ_FIFO_CLEARr(unit, ci, 0);
#endif
        sal_usleep(9);
        if (self_test == 0) {
            result[i] = _test_mem_read(unit, (ci + wl), 0, data_expected);
        } else if (self_test==2) {
            result[i] = _test_rd_data_fifo_status(unit, ci, wl, 1);
        } else {
            if(SOC_IS_KATANA(unit)) {
                result[i] =
                    _test_func_self_test_1(unit, ci, wl, 1, 3);
            } else {
                result[i] =
                    _test_func_self_test_1(unit, ci, wl, 1, 3); /* MPR = 4 */
            }
        }
    }

    WRITE_DDR40_PHY_WORD_LANE_0_READ_FIFO_CLEARr(unit, ci, 0);
    WRITE_DDR40_PHY_WORD_LANE_1_READ_FIFO_CLEARr(unit, ci, 0);
#ifdef CONFIG_IPROC_DDR_ECC
    WRITE_DDR40_PHY_ECC_LANE_READ_FIFO_CLEARr(unit, ci, 0);
#endif
    return SOC_E_NONE;

}
#endif

STATIC int _shmoo_rd_dqs(int unit, int ci, int wl, int self_test, vref_word_shmoo *vwsPtr)
{
    int32 i, vref;
    uint32 data;
    uint32 data_expected[NUM_DATA];
    
    uint32 init_rd_en_step0, init_rd_en_step1;
    uint32 new_rd_en_step, new_rd_en_byte_step;

    _fill_in_data(data_expected);
    if (self_test == 0) {
        _test_mem_write(unit, (ci + wl), 0, data_expected);
    }

    /* Do one dummy memory read to fix RTL bug on RD_FIFO write pointer coming out of reset bug */
    data = _test_mem_read(unit, (ci + wl), 0, NULL);
    if(wl == 0) {
        READ_DDR40_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE0_BIT_RD_ENr(unit, ci, &data);
        init_rd_en_step0 =
            DDR40_GET_FIELD(data, DDR40_PHY_WORD_LANE_0, VDL_OVRIDE_BYTE0_BIT_RD_EN,
                            OVR_STEP);
        READ_DDR40_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE1_BIT_RD_ENr(unit, ci, &data);
        init_rd_en_step1 =
            DDR40_GET_FIELD(data, DDR40_PHY_WORD_LANE_0, VDL_OVRIDE_BYTE1_BIT_RD_EN,
                            OVR_STEP);
    } else {
#ifdef CONFIG_IPROC_DDR_ECC
        if(wl == 2) {
            READ_DDR40_PHY_ECC_LANE_VDL_OVRIDE_BYTE_BIT_RD_ENr(unit, ci, &data);
            init_rd_en_step0 =
                DDR40_GET_FIELD(data, DDR40_PHY_ECC_LANE, VDL_OVRIDE_BYTE_BIT_RD_EN,
                                OVR_STEP);
        } else {
#endif
            READ_DDR40_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE0_BIT_RD_ENr(unit, ci, &data);
            init_rd_en_step0 =
                DDR40_GET_FIELD(data, DDR40_PHY_WORD_LANE_1, VDL_OVRIDE_BYTE0_BIT_RD_EN,
                                OVR_STEP);
            READ_DDR40_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE1_BIT_RD_ENr(unit, ci, &data);
            init_rd_en_step1 =
                DDR40_GET_FIELD(data, DDR40_PHY_WORD_LANE_1, VDL_OVRIDE_BYTE1_BIT_RD_EN,
                                OVR_STEP);
#ifdef CONFIG_IPROC_DDR_ECC
        }
#endif
    }
    for(vref = 16; vref < 49; vref++) {
        data = (vref << 6) | vref;
        MODIFY_DDR40_PHY_CONTROL_REGS_VREF_DAC_CONTROLr(unit, ci, data, 0xFFF);
        for (i = 0; i < 64; ++i) {

            /* RD_DQS */
            if (wl == 0) {
                data = SET_OVR_STEP(i);
                WRITE_DDR40_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE0_R_Pr(unit, ci, data);
                WRITE_DDR40_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE0_R_Nr(unit, ci, data);
                WRITE_DDR40_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE1_R_Pr(unit, ci, data);
                WRITE_DDR40_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE1_R_Nr(unit, ci, data);
                new_rd_en_step = init_rd_en_step0 + (i >> RD_EN_DQS_CORROLATION);
                (*vwsPtr)[vref].uncapped_rd_en_step[0] = new_rd_en_step;
                if(new_rd_en_step > 63) {
                    new_rd_en_step = 63;
                }
                new_rd_en_byte_step = new_rd_en_step;
                data = SET_OVR_STEP(new_rd_en_step);
                WRITE_DDR40_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE0_BIT_RD_ENr(unit, ci, data);
                new_rd_en_step = init_rd_en_step1 + (i >> RD_EN_DQS_CORROLATION);
                (*vwsPtr)[vref].uncapped_rd_en_step[1] = new_rd_en_step;
                if(new_rd_en_step > 63) {
                    new_rd_en_step = 63;
                }
                new_rd_en_byte_step += new_rd_en_step;
                data = SET_OVR_STEP(new_rd_en_step);
                WRITE_DDR40_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE1_BIT_RD_ENr(unit, ci, data);
                new_rd_en_byte_step = new_rd_en_byte_step >> 1;
                data = SET_OVR_STEP(new_rd_en_byte_step);
                WRITE_DDR40_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE_RD_ENr(unit, ci, data);
            } else {
#ifdef CONFIG_IPROC_DDR_ECC
                if(wl == 2) {
                    data = SET_OVR_STEP(i);
                    WRITE_DDR40_PHY_ECC_LANE_VDL_OVRIDE_BYTE_R_Pr(unit, ci, data);
                    WRITE_DDR40_PHY_ECC_LANE_VDL_OVRIDE_BYTE_R_Nr(unit, ci, data);
                    new_rd_en_step = init_rd_en_step0 + (i >> RD_EN_DQS_CORROLATION);
                    (*vwsPtr)[vref].uncapped_rd_en_step[0] = new_rd_en_step;
                    if(new_rd_en_step > 63) {
                        new_rd_en_step = 63;
                    }
                    new_rd_en_byte_step = new_rd_en_step;
                    data = SET_OVR_STEP(new_rd_en_step);
                    WRITE_DDR40_PHY_ECC_LANE_VDL_OVRIDE_BYTE_BIT_RD_ENr(unit, ci, data);
                    data = SET_OVR_STEP(new_rd_en_byte_step);
                    WRITE_DDR40_PHY_ECC_LANE_VDL_OVRIDE_BYTE_RD_ENr(unit, ci, data);
                } else {
#endif
                    data = SET_OVR_STEP(i);
                    WRITE_DDR40_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE0_R_Pr(unit, ci, data);
                    WRITE_DDR40_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE0_R_Nr(unit, ci, data);
                    WRITE_DDR40_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE1_R_Pr(unit, ci, data);
                    WRITE_DDR40_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE1_R_Nr(unit, ci, data);
                    new_rd_en_step = init_rd_en_step0 + (i >> RD_EN_DQS_CORROLATION);
                    (*vwsPtr)[vref].uncapped_rd_en_step[0] = new_rd_en_step;
                    if(new_rd_en_step > 63) {
                        new_rd_en_step = 63;
                    }
                    new_rd_en_byte_step = new_rd_en_step;
                    data = SET_OVR_STEP(new_rd_en_step);
                    WRITE_DDR40_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE0_BIT_RD_ENr(unit, ci, data);
                    new_rd_en_step = init_rd_en_step1 + (i >> RD_EN_DQS_CORROLATION);
                    (*vwsPtr)[vref].uncapped_rd_en_step[1] = new_rd_en_step;
                    if(new_rd_en_step > 63) {
                        new_rd_en_step = 63;
                    }
                    new_rd_en_byte_step += new_rd_en_step;
                    data = SET_OVR_STEP(new_rd_en_step);
                    WRITE_DDR40_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE1_BIT_RD_ENr(unit, ci, data);
                    new_rd_en_byte_step = new_rd_en_byte_step >> 1;
                    data = SET_OVR_STEP(new_rd_en_byte_step);
                    WRITE_DDR40_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE_RD_ENr(unit, ci, data);
#ifdef CONFIG_IPROC_DDR_ECC
                }
#endif
            }

            /* Clear the FIFO error state */
            WRITE_DDR40_PHY_WORD_LANE_0_READ_FIFO_CLEARr(unit, ci, 0);
            WRITE_DDR40_PHY_WORD_LANE_1_READ_FIFO_CLEARr(unit, ci, 0);
#ifdef CONFIG_IPROC_DDR_ECC
            WRITE_DDR40_PHY_ECC_LANE_READ_FIFO_CLEARr(unit, ci, 0);
#endif
            sal_usleep(9);
            if (self_test == 0) {
                (*vwsPtr)[vref].raw_result[i] = _test_mem_read(unit, (ci + wl), 0, data_expected);
            } else if (self_test == 2) {
                (*vwsPtr)[vref].raw_result[i] = _test_rd_data_fifo_status(unit, ci, wl, 1);
            } else {
                (*vwsPtr)[vref].raw_result[i] =
                    _test_func_self_test_1(unit, ci, wl, 1, 0);
            }
        }
    }

    /* Clear the FIFO error state */
    WRITE_DDR40_PHY_WORD_LANE_0_READ_FIFO_CLEARr(unit, ci, 0);
    WRITE_DDR40_PHY_WORD_LANE_1_READ_FIFO_CLEARr(unit, ci, 0);
#ifdef CONFIG_IPROC_DDR_ECC
    WRITE_DDR40_PHY_ECC_LANE_READ_FIFO_CLEARr(unit, ci, 0);
#endif
    return SOC_E_NONE;

}
                       
/* Make a shmoo with the write dq vdl values */
STATIC int _shmoo_wr_dq(int unit, int ci, int wl, int self_test, uint32 * result)
{
    int32 i;
    int32 byte, bit;
    uint32 data;
    uint32 data_expected[NUM_DATA];

    _fill_in_data(data_expected);
    if (self_test == 0) {
        _test_mem_write(unit, (ci + wl), 0, data_expected);
    }

    /* Do one dummy memory read to fix RTL bug on RD_FIFO write pointer coming out of reset bug */
    data = _test_mem_read(unit, (ci + wl), 0, NULL);
    for (i = 0; i < 64; ++i) {
        data = SET_OVR_STEP(i);

        /* WR_DQ */
#ifdef CONFIG_IPROC_DDR_ECC
        if(wl == 2) {
            for (byte = 0; byte < 1; byte++) {
                for (bit = 0; bit < 4; bit++) {
                    WRITE_DDR40_PHY_WORD_LANE_x_VDL_OVRIDE_BYTEy_BITz_Wr(wl, byte,
                                                                         bit, unit,
                                                                         ci, data);
                }
            }
        } else {
#endif
            for (byte = 0; byte < 2; byte++) {
                for (bit = 0; bit < 8; bit++) {
                    WRITE_DDR40_PHY_WORD_LANE_x_VDL_OVRIDE_BYTEy_BITz_Wr(wl, byte,
                                                                         bit, unit,
                                                                         ci, data);
                }
            }
#ifdef CONFIG_IPROC_DDR_ECC
        }
#endif

        /* Clear the FIFO error state */
        WRITE_DDR40_PHY_WORD_LANE_0_READ_FIFO_CLEARr(unit, ci, 0);
        WRITE_DDR40_PHY_WORD_LANE_1_READ_FIFO_CLEARr(unit, ci, 0);
#ifdef CONFIG_IPROC_DDR_ECC
        WRITE_DDR40_PHY_ECC_LANE_READ_FIFO_CLEARr(unit, ci, 0);
#endif
        sal_usleep(9);
        if (self_test==0) {
            result[i] = _test_mem_read(unit, (ci+wl), 0, data_expected); 
        } else if (self_test==2) {
            result[i] = _test_rd_data_fifo_status(unit, ci, wl, 1); 
        } else {
            result[i] =
                _test_func_self_test_1(unit, ci, wl, 1, 1);
        }
    }

    /* Clear the FIFO error state */
    WRITE_DDR40_PHY_WORD_LANE_0_READ_FIFO_CLEARr(unit, ci, 0);
    WRITE_DDR40_PHY_WORD_LANE_1_READ_FIFO_CLEARr(unit, ci, 0);
#ifdef CONFIG_IPROC_DDR_ECC
    WRITE_DDR40_PHY_ECC_LANE_READ_FIFO_CLEARr(unit, ci, 0);
#endif
    return SOC_E_NONE;
}

/* Make a shmoo with the Address */
STATIC int _shmoo_addr(int unit, int ci, int wl, int self_test, uint32 * result)
{
    uint32 data;
    uint32 setting, setting_idx;
    uint32 see_pass, see_fail_after_pass;
    uint32 start_vdl_val, pass_start, pass_end;
    uint32 use_byte_sel;
    
    pass_start = 64;            /* 64 means "have not found pass yet" */
    pass_end = 64;              /* 64 means "have not found pass yet" */
    start_vdl_val = 0;

    for(use_byte_sel = 0; use_byte_sel <= 1; use_byte_sel++) {
        /* Find pass_end of window       */
        see_pass = 0;
        see_fail_after_pass = 0;

        /* Do one dummy memory read to fix RTL bug on RD_FIFO write pointer coming out of reset bug */
        data = _test_mem_read(unit, (ci + wl), 0, NULL);
        for (setting = start_vdl_val; setting < 64; setting++) {
            data = SET_OVR_STEP(setting);
            DDR40_SET_FIELD(data, DDR40_PHY_CONTROL_REGS, VDL_OVRIDE_BIT_CTL,
                            BYTE_SEL, use_byte_sel);
            WRITE_DDR40_PHY_CONTROL_REGS_VDL_OVRIDE_BIT_CTLr(unit, ci, data);
            data = SET_OVR_STEP(setting);
            DDR40_SET_FIELD(data, DDR40_PHY_CONTROL_REGS, VDL_OVRIDE_BYTE_CTL,
                            BYTE_SEL, use_byte_sel);
            WRITE_DDR40_PHY_CONTROL_REGS_VDL_OVRIDE_BYTE_CTLr(unit, ci, data);
            if (setting == 0) {
                _soc_mem_reset_and_init_after_shmoo_addr(unit, ci);
            }

            /* Clear the FIFO error state */
            WRITE_DDR40_PHY_WORD_LANE_0_READ_FIFO_CLEARr(unit, ci, 0);
            WRITE_DDR40_PHY_WORD_LANE_1_READ_FIFO_CLEARr(unit, ci, 0);
#ifdef CONFIG_IPROC_DDR_ECC
            WRITE_DDR40_PHY_ECC_LANE_READ_FIFO_CLEARr(unit, ci, 0);
#endif
            READ_DDR40_PHY_CONTROL_REGS_VDL_OVRIDE_BIT_CTLr(unit, ci, &data);
            sal_usleep(9);
            setting_idx = (64 * use_byte_sel) + setting;
            if (self_test == 0) {
                result[setting_idx] = _test_mem_wr_rd(unit, (ci + wl), 0);
            } else {
                result[setting_idx] =
                    _test_func_self_test_2(unit, ci, wl, 1);
            }
            if (see_pass == 0 && see_fail_after_pass == 0) {        /* First time and so far see no pass */
                if (result[setting_idx] == 0) {     /* result_fail = 0 means PASSED */
                    see_pass = 1;
                    pass_start = setting;
                }
                /* result_fail = 0 means PASSED */
            } else {
                if (see_fail_after_pass == 0) {     /* seeing FAIL for the first time */
                    if (result[setting_idx] > 0) {  /* seeing FAIL for the first time */
                        see_fail_after_pass = 1;
                        pass_end = setting - 1;
                    } else {
                        pass_end = setting;
                    }
                }
            }
            if(result[setting_idx] > 0) {
                _soc_mem_reset_and_init_after_shmoo_addr(unit, ci);
            }
        }
        
        setting_idx = 128 + (2 * use_byte_sel);
        result[setting_idx] = pass_start;
        setting_idx = 128 + (2 * use_byte_sel) + 1;
        result[setting_idx] = pass_end;
    }
    
    WRITE_DDR40_PHY_WORD_LANE_0_READ_FIFO_CLEARr(unit, ci, 0); 
    WRITE_DDR40_PHY_WORD_LANE_1_READ_FIFO_CLEARr(unit, ci, 0); 
#ifdef CONFIG_IPROC_DDR_ECC
    WRITE_DDR40_PHY_ECC_LANE_READ_FIFO_CLEARr(unit, ci, 0);
#endif
    
    return SOC_E_NONE;
}

/* Make a shmoo with the write data mask vdl values */
STATIC int _shmoo_wr_dm(int unit, int ci, int wl, uint32 * result)
{
    soc_cm_print
        ("   ------------------------------------------------------\n");
    soc_cm_print
        ("     THIS SHMOO WR_DM feature is NOT SUPPORTED ........  \n");
    soc_cm_print
        ("   ------------------------------------------------------\n");
    return SOC_E_NONE;
}

STATIC int _calib_all(int unit, int ci, uint32 *result, uint32 size, vref_word_shmoo *vwsPtr, int flag, uint32 *Xpos, int type)
{
    int i, bit, vref, byte, bit_loc;
    int passStart;
    int failStart;
    int passLength;
    int maxPassStart;
    int maxPassLength;
    int maxMidPoint;
    int calib_steps;
    uint32 data;
    int ninty_deg_steps, rd_dq_steps, Tfix_steps;
    int mPLover, mPLunder;
    int vrefCap;
    
    vrefCap = (type == SHMOO_RD_DQ) ? 49 : 1;
    vref = (type == SHMOO_RD_DQ) ? 16 : 0;
    
    for(; vref < vrefCap; vref++) {
        for(bit = 0; bit < 16; bit++) {
            passStart = -1;
            failStart = -1;
            passLength = -1;
            maxPassStart = -2;
            maxPassLength = -2;
            maxMidPoint = -2;
            for(i = 0; i < size; i++) {
                if((result[i] & (0x1 << bit)) == 0) {
                    (*vwsPtr)[vref].bs[bit].result[i >> 2] = (((*vwsPtr)[vref].bs[bit].result[i >> 2]) & (~(3 << ((i %4) * 2)))) | (1 << ((i %4) * 2)); //'+';
                    if(passStart < 0) {
                        passStart = i;
                        passLength = 1;
                        failStart = -1;
                    } else {
                        passLength++;
                    }
                    if(i == size - 1) {
                        if(maxPassLength < passLength) {
                            maxPassStart = passStart;
                            maxPassLength = passLength;
                        }
                    }
                } else {
                    (*vwsPtr)[vref].bs[bit].result[i >> 2] = (((*vwsPtr)[vref].bs[bit].result[i >> 2]) & (~(3 << ((i %4) * 2)))) | (3 << ((i %4) * 2)); //'-';
                    if(failStart < 0) {
                        failStart = i;
                        if(maxPassLength < passLength) {
                            maxPassStart = passStart;
                            maxPassLength = passLength;
                        }
                        passStart = -1;
                        passLength = -1;
                    }
                }
            }
            (*vwsPtr)[vref].bs[bit].result[size >> 2] = (((*vwsPtr)[vref].bs[bit].result[size >> 2]) & (~(3 << ((size %4) * 2)))) | (0 << ((size %4) * 2));//'\0'
            /* SINGLE CALIBRATION */
            switch(type) {
                case SHMOO_ADDRC:
                    WRITE_DDR40_PHY_CONTROL_REGS_VDL_CALIBRATEr(unit, ci, 0x0);
                    WRITE_DDR40_PHY_CONTROL_REGS_VDL_CALIBRATEr(unit, ci, 0x200);
                    data = 0;
                    calib_steps = 0;
                    READ_DDR40_PHY_CONTROL_REGS_VDL_CALIB_STATUSr(unit, ci, &data);
                    calib_steps =
                        DDR40_GET_FIELD(data, DDR40_PHY_CONTROL_REGS, VDL_CALIB_STATUS,
                            CALIB_TOTAL);
                    READ_DDR40_PHY_CONTROL_REGS_VDL_DQ_CALIB_STATUSr(unit, ci, &data);
                    rd_dq_steps =
                        DDR40_GET_FIELD(data, DDR40_PHY_CONTROL_REGS, VDL_DQ_CALIB_STATUS,
                            DQ_CALIB_TOTAL) >> 4;
                    ninty_deg_steps = calib_steps >> 2;
                    Tfix_steps = ninty_deg_steps - rd_dq_steps;
                    if((maxPassStart + maxPassLength) > 64) {
                        mPLover = maxPassStart + maxPassLength - 64;
                        mPLunder = maxPassLength - mPLover;
                        maxPassLength = (3 * (mPLover - 1)) + (Tfix_steps << 1) - maxPassStart + 1;
                        maxMidPoint = (maxPassStart + maxPassStart + maxPassLength) >> 1;
                        if(maxMidPoint > 63) {
                            maxMidPoint = 63;
                        }
                    } else {
                        if((result[63] & (0x1 << bit)) == 0) {
                            i = 21 - ((Tfix_steps << 1) / (3 * rd_dq_steps));
                            /* X = 63/3 - 2Tfix / 3Tstep */
                            if(i < 0) {
                                i = 64;
                            } else {
                                i += 64;
                            }
                            for(; i < size; i++) {
                                if((result[i] & (0x1 << bit)) != 0) {
                                    maxPassLength += i - 64;
                                    break;
                                }
                            }
                            mPLover = maxPassStart + maxPassLength - 64;
                            mPLunder = maxPassLength - mPLover;
                            maxPassLength = (3 * (mPLover - 1)) + (Tfix_steps << 1) - maxPassStart + 1;
                            maxMidPoint = (maxPassStart + maxPassStart + maxPassLength) >> 1;
                            if(maxMidPoint > 63) {
                                maxMidPoint = 63;
                            }
                        } else {
                            maxMidPoint = (maxPassStart + maxPassStart + maxPassLength) >> 1;
                        }
                    }
                    break;
                default:
                    maxMidPoint = (maxPassStart + maxPassStart + maxPassLength) >> 1;
                    break;
            }
            if(type != SHMOO_WR_DQ) {
                if(maxMidPoint != -2) {
                    (*vwsPtr)[vref].bs[bit].result[maxMidPoint >> 2] = (((*vwsPtr)[vref].bs[bit].result[maxMidPoint >> 2]) & (~(3 << ((maxMidPoint %4) *2)))) | (2 << ((maxMidPoint %4) *2));//'X';
                }
                (*vwsPtr)[vref].step[bit] = maxMidPoint;
                Xpos[bit] = maxMidPoint;
            }
        }
        if(type == SHMOO_WR_DQ) {
            for(byte = 0; byte < 2; byte++) {
                passStart = -1;
                failStart = -1;
                passLength = -1;
                maxPassStart = -2;
                maxPassLength = -20;
                maxMidPoint = -2;
                for(i = 0; i < size; i++) {
                    if(((result[i] >> (byte << 3)) & 0xFF) == 0) {
                        if(passStart < 0) {
                            passStart = i;
                            passLength = 1;
                            failStart = -1;
                        } else {
                            passLength++;
                        }
                        if(i == size - 1) {
                            if(maxPassLength < (passLength - 7)) {
                                maxPassStart = passStart;
                                maxPassLength = passLength;
                            }
                        }
                    } else {
                        if(failStart < 0) {
                            failStart = i;
                            if(maxPassLength < (passLength - 7)) {
                                maxPassStart = passStart;
                                maxPassLength = passLength;
                            }
                            passStart = -1;
                            passLength = -1;
                        }
                    }
                }
                maxMidPoint = (maxPassStart + maxPassStart + maxPassLength) >> 1;
                for(bit = 0; bit < 8; bit++) {
                    bit_loc = (byte << 3) + bit;
                    if((maxMidPoint >= 0) && (maxMidPoint <= 63)) {
                        (*vwsPtr)[vref].bs[bit_loc].result[maxMidPoint >> 2] = (((*vwsPtr)[vref].bs[bit_loc].result[maxMidPoint >> 2]) & (~(3 << ((maxMidPoint %4) *2)))) | (2 << ((maxMidPoint %4) *2));//'X';
                    }
                    (*vwsPtr)[vref].step[bit_loc] = maxMidPoint;
                    Xpos[bit_loc] = maxMidPoint;
                }
            }
        }
    }

    if (mPLunder); /* Pacify Warning */
    return SOC_E_NONE;
}

STATIC int _calib_all2(int unit, int ci, int wl, uint32 size, vref_word_shmoo *vwsPtr, int flag, uint32 *Xpos, int type)
{
    int i, bit, vref, byte, bit_loc;
    int passStart;
    int failStart;
    int passLength;
    int maxPassStart;
    int maxPassLength;
    int maxMidPoint;
    uint32 data;
    int vrefAvg;
    int overallmPL0 = -3;
    int overallmPL1 = -3;
    int overallmPLiter0 = -3;
    int overallmPLiter1 = -3;
	int maxMidPoint0 = -4;
    int maxMidPoint1 = -4;
    
    for(vref = 16; vref < 49; vref++) {
        for(bit = 0; bit < 16; bit++) {
            passStart = -1;
            failStart = -1;
            passLength = -1;
            maxPassStart = -2;
            maxPassLength = -2;
            maxMidPoint = -2;
            for(i = 0; i < size; i++) {
                if(((*vwsPtr)[vref].raw_result[i] & (0x1 << bit)) == 0) {
                    (*vwsPtr)[vref].bs[bit].result[i >> 2] = (((*vwsPtr)[vref].bs[bit].result[i >> 2]) & (~(3 << ((i %4) * 2)))) | (1 << ((i %4) * 2));//'+';
                    if(passStart < 0) {
                        passStart = i;
                        passLength = 1;
                        failStart = -1;
                    } else {
                        passLength++;
                    }
                    if(i == size - 1) {
                        if(maxPassLength < passLength) {
                            maxPassStart = passStart;
                            maxPassLength = passLength;
                        }
                    }
                } else {
                    (*vwsPtr)[vref].bs[bit].result[i >> 2] = (((*vwsPtr)[vref].bs[bit].result[i >> 2]) & (~(3 << ((i %4) * 2)))) | (3 << ((i %4) * 2)); //'-';
                    if(failStart < 0) {
                        failStart = i;
                        if(maxPassLength < passLength) {
                            maxPassStart = passStart;
                            maxPassLength = passLength;
                        }
                        passStart = -1;
                        passLength = -1;
                    }
                }
            }
            (*vwsPtr)[vref].bs[bit].result[size >> 2] = (((*vwsPtr)[vref].bs[bit].result[size >> 2]) & (~(3 << ((size %4) * 2)))) | (0 << ((size %4) * 2));//'\0'
            /* SINGLE CALIBRATION */
            maxMidPoint = (maxPassStart + maxPassStart + maxPassLength) >> 1;
            (*vwsPtr)[vref].step[bit] = maxMidPoint;
            Xpos[bit] = maxMidPoint;
        }
        for(byte = 0; byte < 2; byte++) {
            passStart = -1;
            failStart = -1;
            passLength = -1;
            maxPassStart = -2;
            maxPassLength = -2;
            maxMidPoint = -2;
            for(i = 0; i < size; i++) {
                if((((*vwsPtr)[vref].raw_result[i] >> (byte << 3)) & 0xFF) == 0) {
                    (*vwsPtr)[vref].bytes[byte].result[i >> 2] = (((*vwsPtr)[vref].bytes[byte].result[i >> 2]) & (~(3 << ((i %4) * 2)))) | (1 << ((i %4) * 2));//'+';
                    if(passStart < 0) {
                        passStart = i;
                        passLength = 1;
                        failStart = -1;
                    } else {
                        passLength++;
                    }
                    if(i == size - 1) {
                        if(maxPassLength < passLength) {
                            maxPassStart = passStart;
                            maxPassLength = passLength;
                        }
                    }
                } else {
                    (*vwsPtr)[vref].bytes[byte].result[i >> 2] = (((*vwsPtr)[vref].bytes[byte].result[i >> 2]) & (~(3 << ((i %4) * 2)))) | (3 << ((i %4) * 2));//'-';
                    if(failStart < 0) {
                        failStart = i;
                        if(maxPassLength < passLength) {
                            maxPassStart = passStart;
                            maxPassLength = passLength;
                        }
                        passStart = -1;
                        passLength = -1;
                    }
                }
            }
            maxMidPoint = (maxPassStart + maxPassStart + maxPassLength) >> 1;
            for(bit = 0; bit < 8; bit++) {
                bit_loc = (byte << 3) + bit;
                if((maxMidPoint >= 0) && (maxMidPoint <= 63)) {
                    (*vwsPtr)[vref].bs[bit_loc].result[maxMidPoint >> 2] = (((*vwsPtr)[vref].bs[bit_loc].result[maxMidPoint >> 2]) & (~(3 << ((maxMidPoint %4) *2)))) | (2 << ((maxMidPoint %4) *2));//'X';
                }
                (*vwsPtr)[vref].step[bit_loc] = maxMidPoint;
            }
			(*vwsPtr)[vref].bytes[byte].result[maxMidPoint >> 2] = (((*vwsPtr)[vref].bytes[byte].result[maxMidPoint >> 2]) & (~(3 << ((maxMidPoint %4) *2)))) | (2 << ((maxMidPoint %4) *2));//'X';
			(*vwsPtr)[vref].bytes[byte].result[size >> 2] = (((*vwsPtr)[vref].bytes[byte].result[size >> 2]) & (~(3 << ((size %4) * 2)))) | (0 << ((size %4) * 2));//'\0'
            if(byte) {
                if(overallmPL1 < maxPassLength) {
                    overallmPL1 = maxPassLength;
                    overallmPLiter1 = vref;
                    maxMidPoint1 = maxMidPoint;
                }
            } else {
                if(overallmPL0 < maxPassLength) {
                    overallmPL0 = maxPassLength;
                    overallmPLiter0 = vref;
                    maxMidPoint0 = maxMidPoint;
                }
            }
        }
    }
    
    for(bit = 0; bit < 8; bit++) {
        Xpos[bit] = maxMidPoint0;
    }
    for(bit = 8; bit < 16; bit++) {
        Xpos[bit] = maxMidPoint1;
    }
    
    vrefAvg = (overallmPLiter0 + overallmPLiter1) >> 1;
    printf("Switching to Vref Step           : %2d\n", vrefAvg);
    data = (vrefAvg << 6) | vrefAvg;
    MODIFY_DDR40_PHY_CONTROL_REGS_VREF_DAC_CONTROLr(unit, ci, data, 0xFFF);
    
    return SOC_E_NONE;
}

STATIC int _calib_all3(int unit, int ci, int wl, uint32 size, vref_word_shmoo *vwsPtr, int flag, uint32 *Xpos, int type)
{
    int i, bit, vref;
    int passStart;
    int failStart;
    int passLength;
    int maxPassStart;
    int maxPassLength;
    int maxMidPoint;
    uint32 data;
    int overallmPL0 = 0;
    int overallmPL1 = 0;
    int overallmPLiter0 = 0;
    int overallmPLiter1 = 0;
    int maxMidPoint0 = 0;
    int maxMidPoint1 = 0;
    int yMidPoint0 = 0;
    int yMidPoint1 = 0;
    
    for(vref = 0; vref < 64; vref++) {
        for(bit = 0; bit < 16; bit++) {
            passStart = -1;
            failStart = -1;
            passLength = -1;
            maxPassStart = -2;
            maxPassLength = -2;
            maxMidPoint = -2;
            for(i = 0; i < size; i++) {
                if(((*vwsPtr)[vref].raw_result[i] & (0x1 << bit)) == 0) {
                    (*vwsPtr)[vref].bs[bit].result[i >> 2] = (((*vwsPtr)[vref].bs[bit].result[i >> 2]) & (~(3 << ((i %4) * 2)))) | (1 << ((i %4) * 2));//'+';
                    if(passStart < 0) {
                        passStart = i;
                        passLength = 1;
                        failStart = -1;
                    } else {
                        passLength++;
                    }
                    if(i == size - 1) {
                        if(maxPassLength < passLength) {
                            maxPassStart = passStart;
                            maxPassLength = passLength;
                        }
                    }
                } else {
                    (*vwsPtr)[vref].bs[bit].result[i >> 2] = (((*vwsPtr)[vref].bs[bit].result[i >> 2]) & (~(3 << ((i %4) * 2)))) | (3 << ((i %4) * 2)); //'-';
                    if(failStart < 0) {
                        failStart = i;
                        if(maxPassLength < passLength) {
                            maxPassStart = passStart;
                            maxPassLength = passLength;
                        }
                        passStart = -1;
                        passLength = -1;
                    }
                }
            }
            (*vwsPtr)[vref].bs[bit].result[size >> 2] = (((*vwsPtr)[vref].bs[bit].result[size >> 2]) & (~(3 << ((size %4) * 2)))) | (0 << ((size %4) * 2));//'\0'
            /* SINGLE CALIBRATION */
            maxMidPoint = (maxPassStart + maxPassStart + maxPassLength) >> 1;
            if(maxMidPoint != -2) {
                (*vwsPtr)[vref].bs[bit].result[maxMidPoint >> 2] = (((*vwsPtr)[vref].bs[bit].result[maxMidPoint >> 2]) & (~(3 << ((maxMidPoint %4) *2)))) | (2 << ((maxMidPoint %4) *2));//'X';
            }
            (*vwsPtr)[vref].step[bit] = maxMidPoint;
            if(bit >> 3) {
                if(maxPassLength >= 5) {
                    overallmPL1++;
                    overallmPLiter1 += vref;
                }
            } else {
                if(maxPassLength >= 5) {
                    overallmPL0++;
                    overallmPLiter0 += vref;
                }
            }
        }
    }

    if(overallmPL0 != 0) {
        yMidPoint0 = overallmPLiter0 / overallmPL0;
        for(bit = 0; bit < 8; bit++) {
            maxMidPoint0 += (*vwsPtr)[yMidPoint0].step[bit];
        }
        maxMidPoint0 = maxMidPoint0 >> 3;
    }
    if(overallmPL1 != 0) {
        yMidPoint1 = overallmPLiter1 / overallmPL1;
        for(bit = 8; bit < 16; bit++) {
            maxMidPoint1 += (*vwsPtr)[yMidPoint1].step[bit];
        }
        maxMidPoint1 = maxMidPoint1 >> 3;
    }
    
    for(bit = 0; bit < 8; bit++) {
        Xpos[bit] = maxMidPoint0;
    }
    for(bit = 8; bit < 16; bit++) {
        Xpos[bit] = maxMidPoint1;
    }

    /* RD_DQ */
#ifdef CONFIG_IPROC_DDR_ECC
    if(wl == 2) {
        data = SET_OVR_STEP(yMidPoint0);
        printf("Switching to RD_DQ Step (Byte 0) : %2d\n", yMidPoint0);
        for (bit = 0; bit < 4; bit++) {
            WRITE_DDR40_PHY_WORD_LANE_x_VDL_OVRIDE_BYTEy_BITz_R_Pr(wl,
                                                                   0,
                                                                   bit,
                                                                   unit,
                                                                   ci,
                                                                   data);
            WRITE_DDR40_PHY_WORD_LANE_x_VDL_OVRIDE_BYTEy_BITz_R_Nr(wl,
                                                                   0,
                                                                   bit,
                                                                   unit,
                                                                   ci,
                                                                   data);
        }
    } else {
#endif
        data = SET_OVR_STEP(yMidPoint0);
        printf("Switching to RD_DQ Step (Byte 0) : %2d\n", yMidPoint0);
        for (bit = 0; bit < 8; bit++) {
            WRITE_DDR40_PHY_WORD_LANE_x_VDL_OVRIDE_BYTEy_BITz_R_Pr(wl,
                                                                   0,
                                                                   bit,
                                                                   unit,
                                                                   ci,
                                                                   data);
            WRITE_DDR40_PHY_WORD_LANE_x_VDL_OVRIDE_BYTEy_BITz_R_Nr(wl,
                                                                   0,
                                                                   bit,
                                                                   unit,
                                                                   ci,
                                                                   data);
        }
        data = SET_OVR_STEP(yMidPoint1);
        printf("Switching to RD_DQ Step (Byte 1) : %2d\n", yMidPoint1);
        for (bit = 0; bit < 8; bit++) {
            WRITE_DDR40_PHY_WORD_LANE_x_VDL_OVRIDE_BYTEy_BITz_R_Pr(wl,
                                                                   1,
                                                                   bit,
                                                                   unit,
                                                                   ci,
                                                                   data);
            WRITE_DDR40_PHY_WORD_LANE_x_VDL_OVRIDE_BYTEy_BITz_R_Nr(wl,
                                                                   1,
                                                                   bit,
                                                                   unit,
                                                                   ci,
                                                                   data);
        }
#ifdef CONFIG_IPROC_DDR_ECC
    }
#endif
    
    return SOC_E_NONE;
}

/* Get Init steps                                                                                                           */
int
_soc_ddr40_shmoo_get_init_step(int unit, int ci, soc_ddr_shmoo_param_t *sp)
{
    uint32 data;
    uint32 calib_total;
    int byte, bit, en, n;

    switch (sp->type) {
    case SHMOO_RD_EN:
        READ_DDR40_PHY_CONTROL_REGS_VDL_RD_EN_CALIB_STATUSr(unit, ci, &data);
        calib_total =
            DDR40_GET_FIELD(data, DDR40_PHY_CONTROL_REGS,
                            VDL_RD_EN_CALIB_STATUS, RD_EN_CALIB_TOTAL) >> 4;
        if (sp->wl == 0) {
            READ_DDR40_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE0_BIT_RD_ENr(unit, ci, &data);    /* Byte 0 */
            sp->init_step[0] =
                (DDR40_GET_FIELD
                 (data, DDR40_PHY_WORD_LANE_0, VDL_OVRIDE_BYTE0_BIT_RD_EN,
                  OVR_EN)) ? DDR40_GET_FIELD(data, DDR40_PHY_WORD_LANE_0,
                                             VDL_OVRIDE_BYTE0_BIT_RD_EN,
                                             OVR_STEP) : calib_total;
            READ_DDR40_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE1_BIT_RD_ENr(unit, ci, &data);    /* Byte 1 */
            sp->init_step[1] =
                (DDR40_GET_FIELD
                 (data, DDR40_PHY_WORD_LANE_0, VDL_OVRIDE_BYTE1_BIT_RD_EN,
                  OVR_EN)) ? DDR40_GET_FIELD(data, DDR40_PHY_WORD_LANE_0,
                                             VDL_OVRIDE_BYTE1_BIT_RD_EN,
                                             OVR_STEP) : calib_total;
        } else {
#ifdef CONFIG_IPROC_DDR_ECC
            if(sp->wl == 2) {
                READ_DDR40_PHY_ECC_LANE_VDL_OVRIDE_BYTE_BIT_RD_ENr(unit, ci, &data);    /* Byte 0 */
                sp->init_step[0] =
                    (DDR40_GET_FIELD
                     (data, DDR40_PHY_ECC_LANE, VDL_OVRIDE_BYTE_BIT_RD_EN,
                      OVR_EN)) ? DDR40_GET_FIELD(data, DDR40_PHY_ECC_LANE,
                                                 VDL_OVRIDE_BYTE_BIT_RD_EN,
                                                 OVR_STEP) : calib_total;
            } else {
#endif
                READ_DDR40_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE0_BIT_RD_ENr(unit, ci, &data);    /* Byte 0 */
                sp->init_step[0] =
                    (DDR40_GET_FIELD
                     (data, DDR40_PHY_WORD_LANE_1, VDL_OVRIDE_BYTE0_BIT_RD_EN,
                      OVR_EN)) ? DDR40_GET_FIELD(data, DDR40_PHY_WORD_LANE_1,
                                                 VDL_OVRIDE_BYTE0_BIT_RD_EN,
                                                 OVR_STEP) : calib_total;
                READ_DDR40_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE1_BIT_RD_ENr(unit, ci, &data);    /* Byte 1 */
                sp->init_step[1] =
                    (DDR40_GET_FIELD
                     (data, DDR40_PHY_WORD_LANE_1, VDL_OVRIDE_BYTE1_BIT_RD_EN,
                      OVR_EN)) ? DDR40_GET_FIELD(data, DDR40_PHY_WORD_LANE_1,
                                                 VDL_OVRIDE_BYTE1_BIT_RD_EN,
                                                 OVR_STEP) : calib_total;
#ifdef CONFIG_IPROC_DDR_ECC
            }
#endif
        }
        if (sp->wl == 0) {
            READ_DDR40_PHY_WORD_LANE_0_READ_DATA_DLYr(unit, ci, &data);
            sp->init_step[2] = data;
        } else {
#ifdef CONFIG_IPROC_DDR_ECC
            if(sp->wl == 2) {
                READ_DDR40_PHY_ECC_LANE_READ_DATA_DLYr(unit, ci, &data);
                sp->init_step[2] = data;
            } else {
#endif
                READ_DDR40_PHY_WORD_LANE_1_READ_DATA_DLYr(unit, ci, &data);
                sp->init_step[2] = data;
#ifdef CONFIG_IPROC_DDR_ECC
            }
#endif
        }
        break;
    case SHMOO_RD_DQ:
        READ_DDR40_PHY_CONTROL_REGS_VDL_DQ_CALIB_STATUSr(unit, ci, &data);
        calib_total =
            DDR40_GET_FIELD(data, DDR40_PHY_CONTROL_REGS, VDL_DQ_CALIB_STATUS,
                            DQ_CALIB_TOTAL) >> 4;
        n = 0;
#ifdef CONFIG_IPROC_DDR_ECC
        if(sp->wl == 2) {
            for (byte = 0; byte < 1; byte++) {
                for (bit = 0; bit < 4; bit++) {
                    READ_DDR40_PHY_WORD_LANE_x_VDL_OVRIDE_BYTEy_BITz_R_Pr(sp->wl, byte,
                                                                          bit,
                                                                          unit, ci,
                                                                          &data);
                    en = DDR40_GET_FIELD(data, DDR40_PHY_ECC_LANE,
                                         VDL_OVRIDE_BYTE_BIT0_R_P, OVR_EN);
                    sp->init_step[n] =
                        (en) ? GET_FIELD(data, DDR40_PHY_ECC_LANE,
                                         VDL_OVRIDE_BYTE_BIT0_R_P,
                                         OVR_STEP) : calib_total;
                    n++;
                }
            }
        } else {
#endif
            for (byte = 0; byte < 2; byte++) {
                for (bit = 0; bit < 8; bit++) {
                    READ_DDR40_PHY_WORD_LANE_x_VDL_OVRIDE_BYTEy_BITz_R_Pr(sp->wl, byte,
                                                                          bit,
                                                                          unit, ci,
                                                                          &data);
                    en = DDR40_GET_FIELD(data, DDR40_PHY_WORD_LANE_0,
                                         VDL_OVRIDE_BYTE0_BIT0_R_P, OVR_EN);
                    sp->init_step[n] =
                        (en) ? GET_FIELD(data, DDR40_PHY_WORD_LANE_0,
                                         VDL_OVRIDE_BYTE0_BIT0_R_P,
                                         OVR_STEP) : calib_total;
                    n++;
                }
            }
#ifdef CONFIG_IPROC_DDR_ECC
        }
#endif
        break;
    case SHMOO_WR_DQ:
        READ_DDR40_PHY_CONTROL_REGS_VDL_WR_CHAN_CALIB_STATUSr(unit, ci, &data);
        calib_total =
            DDR40_GET_FIELD(data, DDR40_PHY_CONTROL_REGS,
                            VDL_WR_CHAN_CALIB_STATUS,
                            WR_CHAN_CALIB_TOTAL) >> 4;
        n = 0;
#ifdef CONFIG_IPROC_DDR_ECC
        if(sp->wl == 2) {
            for (byte = 0; byte < 1; byte++) {
                for (bit = 0; bit < 4; bit++) {
                    READ_DDR40_PHY_WORD_LANE_x_VDL_OVRIDE_BYTEy_BITz_Wr(sp->wl, byte,
                                                                        bit, unit,
                                                                        ci, &data);
                    en = DDR40_GET_FIELD(data, DDR40_PHY_ECC_LANE,
                                         VDL_OVRIDE_BYTE_BIT0_W, OVR_EN);
                    sp->init_step[n] =
                        (en) ? DDR40_GET_FIELD(data, DDR40_PHY_ECC_LANE,
                                               VDL_OVRIDE_BYTE_BIT0_W,
                                               OVR_STEP) : calib_total;
                    n++;
                }
            }
        } else {
#endif
            for (byte = 0; byte < 2; byte++) {
                for (bit = 0; bit < 8; bit++) {
                    READ_DDR40_PHY_WORD_LANE_x_VDL_OVRIDE_BYTEy_BITz_Wr(sp->wl, byte,
                                                                        bit, unit,
                                                                        ci, &data);
                    en = DDR40_GET_FIELD(data, DDR40_PHY_WORD_LANE_0,
                                         VDL_OVRIDE_BYTE0_BIT0_W, OVR_EN);
                    sp->init_step[n] =
                        (en) ? DDR40_GET_FIELD(data, DDR40_PHY_WORD_LANE_0,
                                               VDL_OVRIDE_BYTE0_BIT0_W,
                                               OVR_STEP) : calib_total;
                    n++;
                }
            }
#ifdef CONFIG_IPROC_DDR_ECC
        }
#endif
        break;
    case SHMOO_ADDRC:
        break;
    case SHMOO_WR_DM:
        READ_DDR40_PHY_CONTROL_REGS_VDL_WR_CHAN_CALIB_STATUSr(unit, ci, &data);
        calib_total =
            DDR40_GET_FIELD(data, DDR40_PHY_CONTROL_REGS,
                            VDL_WR_CHAN_CALIB_STATUS,
                            WR_CHAN_CALIB_TOTAL) >> 4;
        if (sp->wl == 0) {
            READ_DDR40_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE0_DM_Wr(unit, ci, &data);
            sp->init_step[0] =
                (DDR40_GET_FIELD
                 (data, DDR40_PHY_WORD_LANE_0, VDL_OVRIDE_BYTE0_DM_W,
                  OVR_EN)) ? DDR40_GET_FIELD(data, DDR40_PHY_WORD_LANE_0,
                                             VDL_OVRIDE_BYTE0_DM_W,
                                             OVR_STEP) : calib_total;
            READ_DDR40_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE1_DM_Wr(unit, ci, &data);
            sp->init_step[1] =
                (DDR40_GET_FIELD
                 (data, DDR40_PHY_WORD_LANE_0, VDL_OVRIDE_BYTE1_DM_W,
                  OVR_EN)) ? DDR40_GET_FIELD(data, DDR40_PHY_WORD_LANE_0,
                                             VDL_OVRIDE_BYTE1_DM_W,
                                             OVR_STEP) : calib_total;
        } else {
#ifdef CONFIG_IPROC_DDR_ECC
            if(sp->wl == 2) {
                READ_DDR40_PHY_ECC_LANE_VDL_OVRIDE_BYTE_DM_Wr(unit, ci, &data);
                sp->init_step[0] =
                    (DDR40_GET_FIELD
                     (data, DDR40_PHY_ECC_LANE, VDL_OVRIDE_BYTE_DM_W,
                      OVR_EN)) ? DDR40_GET_FIELD(data, DDR40_PHY_ECC_LANE,
                                                 VDL_OVRIDE_BYTE_DM_W,
                                                 OVR_STEP) : calib_total;
            } else {
#endif
                READ_DDR40_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE0_DM_Wr(unit, ci, &data);
                sp->init_step[0] =
                    (DDR40_GET_FIELD
                     (data, DDR40_PHY_WORD_LANE_1, VDL_OVRIDE_BYTE0_DM_W,
                      OVR_EN)) ? DDR40_GET_FIELD(data, DDR40_PHY_WORD_LANE_1,
                                                 VDL_OVRIDE_BYTE0_DM_W,
                                                 OVR_STEP) : calib_total;
                READ_DDR40_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE1_DM_Wr(unit, ci, &data);
                sp->init_step[1] =
                    (DDR40_GET_FIELD
                     (data, DDR40_PHY_WORD_LANE_1, VDL_OVRIDE_BYTE1_DM_W,
                      OVR_EN)) ? DDR40_GET_FIELD(data, DDR40_PHY_WORD_LANE_1,
                                                 VDL_OVRIDE_BYTE1_DM_W,
                                                 OVR_STEP) : calib_total;
#ifdef CONFIG_IPROC_DDR_ECC
            }
#endif
        }
        break;
    default:
        break;
    }
    return SOC_E_NONE;
}

int
_soc_ddr40_shmoo_do(int unit, int ci, soc_ddr_shmoo_param_t *sp)
{
    switch (sp->type) {
    case SHMOO_INIT_VDL_RESULT:
        return _shmoo_init_vdl_result(unit, ci, sp->wl, 1, sp->result);
    case SHMOO_RD_EN:
        _shmoo_rd_data_dly(unit, ci, sp->wl, 1, sp->result);
		return _shmoo_rd_en_rd_dq(unit, ci, sp->wl, 1, sp->vwsPtr);
    case SHMOO_RD_DQ:
        _shmoo_rd_dq(unit, ci, sp->wl);
        return _shmoo_rd_dqs(unit, ci, sp->wl, 1, sp->vwsPtr);
    case SHMOO_WR_DQ:
        return _shmoo_wr_dq(unit, ci, sp->wl, 1, sp->result);
    case SHMOO_ADDRC:
        return _shmoo_addr(unit, ci, sp->wl, 1, sp->result);
    case SHMOO_WR_DM:
        return _shmoo_wr_dm(unit, ci, sp->wl, sp->result);
    }
    return SOC_E_PARAM;
}

int
_soc_ddr40_shmoo_calib(int unit, int ci, soc_ddr_shmoo_param_t *sp)
{
    switch (sp->type) {
    case SHMOO_INIT_VDL_RESULT:
        return SOC_E_NONE;
    case SHMOO_RD_EN:
        return _calib_all3(unit, ci, sp->wl, 64, sp->vwsPtr, 0, sp->new_step, sp->type);
    case SHMOO_RD_DQ:
        return _calib_all2(unit, ci, sp->wl, 64, sp->vwsPtr, 0, sp->new_step, sp->type);
    case SHMOO_WR_DQ:
        return _calib_all(unit, ci, sp->result, 64, sp->vwsPtr, 0, sp->new_step, sp->type);
    case SHMOO_ADDRC:
        return _calib_all(unit, ci, sp->result, 128, sp->vwsPtr, 0, sp->new_step, sp->type);
    }
    return SOC_E_PARAM;
}

int _soc_ddr40_shmoo_plot(soc_ddr_shmoo_param_t *sp)
{
    int i, j;
    int vref, vref_count;
    int byte_plot;
    char str[65];
    vref_word_shmoo *vwsPtr = sp->vwsPtr;

    str[64] = 0;

    soc_cm_print("\n");

    switch (sp->type) {
    case SHMOO_INIT_VDL_RESULT:
        return SOC_E_NONE;
        break;
    case SHMOO_RD_EN:
        byte_plot = 1;
        soc_cm_print("**** RD_EN  Shmoo (WL=%d)\n", sp->wl);
        break;
    case SHMOO_RD_DQ:
        byte_plot = 1;
        soc_cm_print("**** RD_DQS  Shmoo (WL=%d)\n", sp->wl);
        break;
    case SHMOO_WR_DQ:
        byte_plot = 1;
        soc_cm_print("**** WR_DQ  Shmoo (WL=%d)\n", sp->wl);
        break;
    case SHMOO_ADDRC:
        byte_plot = 0;
        soc_cm_print("**** ADDRC  Shmoo\n");
        /*soc_cm_print("**** ADDRC  Shmoo (WL=%d)\n", sp->wl);*/
        break;
    default:
        byte_plot = 0;
        soc_cm_print("**** ?????  Shmoo (WL=%d)\n", sp->wl);
        break;
    }
    soc_cm_print("\n");
    
    if(sp->type == SHMOO_WR_DQ) {
        for (i = 0; i < 64; i++) {
            str[i] = '0' + (i/10);
        }
        soc_cm_print("    %s\n", str);

        for ( i = 0; i < 64; i++ ) {
            str[i] = '0' + (i%10);
        }
        soc_cm_print("    %s\n", str);

        vref_count = 16;
        
		for (vref = 0; vref < vref_count; vref++) {
			soc_cm_print(" %02d ", vref);
			for(i = 0; i < 33; i++) {
				for(j = 0; j < 4; j++) {
					switch((((*vwsPtr)[0].bs[vref].result[i] >> (j * 2))) & 0x3) {
						case 0:
							soc_cm_print("\n");
							goto next_line1;
						case 1:
							soc_cm_print("+");
							break;
						case 2:
							soc_cm_print("X");
							break;
						case 3:
							soc_cm_print("-");
							break;
					}
				}
			}
next_line1:
			j++;

				//soc_cm_print(" %02d %s\n", vref, (*vwsPtr)[0].bs[vref].result);
		}
        
        soc_cm_print("\n");
        return SOC_E_NONE;
    }


    if (sp->type == SHMOO_ADDRC) {
        soc_cm_print("\n BYTE_SEL = 0  \n");
    } else {
        soc_cm_print("\n BYTE = 0  \n");
    }

    for (i = 0; i < 64; i++) {
        str[i] = '0' + (i/10);
    }
    soc_cm_print("    %s\n", str);

    for ( i = 0; i < 64; i++ ) {
        str[i] = '0' + (i%10);
    }
    soc_cm_print("    %s\n", str);

    vref_count = (sp->type == SHMOO_RD_DQ) ? 49 : 1;
    vref = (sp->type == SHMOO_RD_DQ) ? 16 : 0;
    
    vref_count = (sp->type == SHMOO_RD_EN) ? 64 : vref_count;

    for (; vref < vref_count; vref++) {
        if(sp->type == SHMOO_RD_DQ) {
			soc_cm_print(" %02d ", vref);
			for(i = 0; i < 33; i++) {
				for(j = 0; j < 4; j++) {
					switch((((*vwsPtr)[vref].bytes[0].result[i] >> (j * 2))) & 0x3) {
						case 0:
							soc_cm_print("\n");
							goto next_line2;
						case 1:
							soc_cm_print("+");
							break;
						case 2:
							soc_cm_print("X");
							break;
						case 3:
							soc_cm_print("-");
							break;
					}
				}
			}
            //soc_cm_print(" %02d %s\n", vref, (*vwsPtr)[vref].bytes[0].result);
        /*    for(i = 0; i < 8; i++) {
                soc_cm_print(" %02d-%02d %s\n", vref, i, (*vwsPtr)[vref].bs[i].result);
            } */
        } else if(sp->type == SHMOO_RD_EN) {
            //soc_cm_print(" %02d %s\n", vref, (*vwsPtr)[vref].bs[0].result);
			soc_cm_print(" %02d ", vref);
			for(i = 0; i < 33; i++) {
				for(j = 0; j < 4; j++) {
					switch((((*vwsPtr)[vref].bs[0].result[i] >> (j * 2))) & 0x3) {
						case 0:
							soc_cm_print("\n");
							goto next_line2;
						case 1:
							soc_cm_print("+");
							break;
						case 2:
							soc_cm_print("X");
							break;
						case 3:
							soc_cm_print("-");
							break;
					}
				}
			}
        } else if(sp->type == SHMOO_WR_DQ) {
            //soc_cm_print(" %02d %s\n", vref, (*vwsPtr)[vref].bs[0].result);
			soc_cm_print(" %02d ", vref);
			for(i = 0; i < 33; i++) {
				for(j = 0; j < 4; j++) {
					switch((((*vwsPtr)[vref].bs[0].result[i] >> (j * 2))) & 0x3) {
						case 0:
							soc_cm_print("\n");
							goto next_line2;
						case 1:
							soc_cm_print("+");
							break;
						case 2:
							soc_cm_print("X");
							break;
						case 3:
							soc_cm_print("-");
							break;
					}
				}
			}
        } else if((sp->type == SHMOO_ADDRC)) {
            soc_cm_print(" %02d ", vref);
			for(i = 0; i < 16; i++) {
				for(j = 0; j < 4; j++) {
					switch((((*vwsPtr)[vref].bs[0].result[i] >> (j * 2))) & 0x3) {
						case 0:
							soc_cm_print("\n");
							goto next_line2;
						case 1:
							soc_cm_print("+");
							break;
						case 2:
							soc_cm_print("X");
							break;
						case 3:
							soc_cm_print("-");
							break;
					}
				}
			}
            soc_cm_print("\n");

#if 0
            for(i = 0; i < 64; i++) {
                soc_cm_print("%c", (*vwsPtr)[vref].bs[0].result[i]);
            }
            soc_cm_print("\n");
#endif
        } else {
            soc_cm_print("UNKNOWN SHMOO\n");
            return -1;
        }
next_line2:
		j++;
	}
    soc_cm_print("\n");

    if (sp->type == SHMOO_ADDRC) {
        soc_cm_print("\n BYTE_SEL = 1  \n");
    } else {
        soc_cm_print("\n BYTE = 1  \n");
    }

    for (i = 0; i < 64; i++)
        str[i] = '0' + (i / 10);
    soc_cm_print("    %s\n", str);

    for (i = 0; i < 64; i++)
        str[i] = '0' + (i % 10);
    soc_cm_print("    %s\n", str);
    
    vref = (sp->type == SHMOO_RD_DQ) ? 16 : 0;
        
    for (; vref < vref_count; vref++) {
        if(sp->type == SHMOO_RD_DQ) {
            //soc_cm_print(" %02d %s\n", vref, (*vwsPtr)[vref].bytes[1].result);
			soc_cm_print(" %02d ", vref);
			for(i = 0; i < 33; i++) {
				for(j = 0; j < 4; j++) {
					switch((((*vwsPtr)[vref].bytes[1].result[i] >> (j * 2))) & 0x3) {
						case 0:
							soc_cm_print("\n");
							goto next_line3;
						case 1:
							soc_cm_print("+");
							break;
						case 2:
							soc_cm_print("X");
							break;
						case 3:
							soc_cm_print("-");
							break;
					}
				}
			}

        /*    for(i = 8; i < 16; i++) {
                soc_cm_print(" %02d-%02d %s\n", vref, i, (*vwsPtr)[vref].bs[i].result);
            } */
        } else if(sp->type == SHMOO_RD_EN) {
            //soc_cm_print(" %02d %s\n", vref, (*vwsPtr)[vref].bs[8].result);
			soc_cm_print(" %02d ", vref);
			for(i = 0; i < 33; i++) {
				for(j = 0; j < 4; j++) {
					switch((((*vwsPtr)[vref].bs[8].result[i] >> (j * 2))) & 0x3) {
						case 0:
							soc_cm_print("\n");
							goto next_line3;
						case 1:
							soc_cm_print("+");
							break;
						case 2:
							soc_cm_print("X");
							break;
						case 3:
							soc_cm_print("-");
							break;
					}
				}
			}

        } else if(sp->type == SHMOO_WR_DQ) {
            //soc_cm_print(" %02d %s\n", vref, (*vwsPtr)[vref].bs[8].result);
			soc_cm_print(" %02d ", vref);
			for(i = 0; i < 33; i++) {
				for(j = 0; j < 4; j++) {
					switch((((*vwsPtr)[vref].bs[8].result[i] >> (j * 2))) & 0x3) {
						case 0:
							soc_cm_print("\n");
							goto next_line3;
						case 1:
							soc_cm_print("+");
							break;
						case 2:
							soc_cm_print("X");
							break;
						case 3:
							soc_cm_print("-");
							break;
					}
				}
			}

        } else if((sp->type == SHMOO_ADDRC)) {
            soc_cm_print(" %02d ", vref);
			for(i = 16; i < 32; i++) {
				for(j = 0; j < 4; j++) {
					switch((((*vwsPtr)[vref].bs[0].result[i] >> (j * 2))) & 0x3) {
						case 0:
							soc_cm_print("\n");
							goto next_line3;
						case 1:
							soc_cm_print("+");
							break;
						case 2:
							soc_cm_print("X");
							break;
						case 3:
							soc_cm_print("-");
							break;
					}
				}
			}
            soc_cm_print("\n");

#if 0
            soc_cm_print(" %02d ", vref);
            for(i = 64; i < 128; i++) {
                soc_cm_print("%c", (*vwsPtr)[vref].bs[0].result[i]);
            }
            soc_cm_print("\n");
#endif
        } else {
            soc_cm_print("UNKNOWN SHMOO\n");
            return -1;
        }
next_line3:
		j++;
    }
    soc_cm_print("\n");

    if (byte_plot) ; /* Pacify Warning */
    return SOC_E_NONE;
}

int
_soc_ddr40_shmoo_set_new_step(int unit, int ci, soc_ddr_shmoo_param_t *sp)
{
    uint32 data;
    int byte, bit, n;
    int indx;
    uint32 init_rd_dqs_step;
    uint32 init_rd_en_step;
    uint32 new_rd_dqs_step;
    uint32 new_rd_en_step;
    uint32 vref_pick;
    uint32 new_rd_en_avg_step;
    vref_word_shmoo *vwsPtr = sp->vwsPtr;

    switch (sp->type) {
    case SHMOO_RD_EN:
        if (sp->wl == 0) {
            n = sp->new_step[0];
            data = SET_OVR_STEP(n);
            WRITE_DDR40_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE0_BIT_RD_ENr(unit, ci,
                                                                    data);
            n = sp->new_step[8];
            data = SET_OVR_STEP(n);
            WRITE_DDR40_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE1_BIT_RD_ENr(unit, ci,
                                                                    data);
            n = (sp->new_step[0] + sp->new_step[8]) >> 1;
            data = SET_OVR_STEP(n);
            WRITE_DDR40_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE_RD_ENr(unit, ci, data);
        } else {
#ifdef CONFIG_IPROC_DDR_ECC
            if(sp->wl == 2) {
                n = sp->new_step[0];
                data = SET_OVR_STEP(n);
                WRITE_DDR40_PHY_ECC_LANE_VDL_OVRIDE_BYTE_BIT_RD_ENr(unit, ci,
                                                                        data);
                WRITE_DDR40_PHY_ECC_LANE_VDL_OVRIDE_BYTE_RD_ENr(unit, ci, data);
            } else {
#endif
                n = sp->new_step[0];
                data = SET_OVR_STEP(n);
                WRITE_DDR40_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE0_BIT_RD_ENr(unit, ci,
                                                                        data);
                n = sp->new_step[8];
                data = SET_OVR_STEP(n);
                WRITE_DDR40_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE1_BIT_RD_ENr(unit, ci,
                                                                        data);
                n = (sp->new_step[0] + sp->new_step[8]) >> 1;
                data = SET_OVR_STEP(n);
                WRITE_DDR40_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE_RD_ENr(unit, ci, data);
#ifdef CONFIG_IPROC_DDR_ECC
            }
#endif
        }
        break;
    case SHMOO_RD_DQ:
        READ_DDR40_PHY_CONTROL_REGS_VREF_DAC_CONTROLr(unit, ci, &data);
        vref_pick =
                DDR40_GET_FIELD(data, DDR40_PHY_CONTROL_REGS, VREF_DAC_CONTROL,
                                DAC0);
        if (sp->wl == 0) {
            READ_DDR40_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE0_R_Pr(unit, ci, &data);
            init_rd_dqs_step =
                DDR40_GET_FIELD(data, DDR40_PHY_WORD_LANE_0, VDL_OVRIDE_BYTE0_R_P,
                                OVR_STEP);
            init_rd_en_step = (*vwsPtr)[vref_pick].uncapped_rd_en_step[0];
            new_rd_dqs_step = sp->new_step[0];
            new_rd_en_step = init_rd_en_step - ((init_rd_dqs_step - new_rd_dqs_step) >> RD_EN_DQS_CORROLATION);
            printf("Switching to RD_DQS Step (Byte 0): %2d\n", new_rd_dqs_step);
            printf("Switching to RD_EN Step  (Byte 0): %2d\n", new_rd_en_step);
            data = SET_OVR_STEP(new_rd_dqs_step);
            WRITE_DDR40_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE0_R_Pr(unit, ci, data);
            WRITE_DDR40_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE0_R_Nr(unit, ci, data);
            data = SET_OVR_STEP(new_rd_en_step);
            WRITE_DDR40_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE0_BIT_RD_ENr(unit, ci, data);
            READ_DDR40_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE1_R_Pr(unit, ci, &data);
            new_rd_en_avg_step = new_rd_en_step;
            init_rd_dqs_step =
                DDR40_GET_FIELD(data, DDR40_PHY_WORD_LANE_0, VDL_OVRIDE_BYTE1_R_P,
                                OVR_STEP);
            init_rd_en_step = (*vwsPtr)[vref_pick].uncapped_rd_en_step[1];
            new_rd_dqs_step = sp->new_step[8];
            new_rd_en_step = init_rd_en_step - ((init_rd_dqs_step - new_rd_dqs_step) >> RD_EN_DQS_CORROLATION);
            printf("Switching to RD_DQS Step (Byte 1): %2d\n", new_rd_dqs_step);
            printf("Switching to RD_EN Step  (Byte 1): %2d\n", new_rd_en_step);
            data = SET_OVR_STEP(new_rd_dqs_step);
            WRITE_DDR40_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE1_R_Pr(unit, ci, data);
            WRITE_DDR40_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE1_R_Nr(unit, ci, data);
            data = SET_OVR_STEP(new_rd_en_step);
            WRITE_DDR40_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE1_BIT_RD_ENr(unit, ci, data);
            new_rd_en_avg_step = (new_rd_en_avg_step + new_rd_en_step) >> 1;
            data = SET_OVR_STEP(new_rd_en_avg_step);
            WRITE_DDR40_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE_RD_ENr(unit, ci, data);
        } else {
#ifdef CONFIG_IPROC_DDR_ECC
            if(sp->wl == 2) {
                READ_DDR40_PHY_ECC_LANE_VDL_OVRIDE_BYTE_R_Pr(unit, ci, &data);
                init_rd_dqs_step =
                    DDR40_GET_FIELD(data, DDR40_PHY_ECC_LANE, VDL_OVRIDE_BYTE_R_P,
                                    OVR_STEP);
                init_rd_en_step = (*vwsPtr)[vref_pick].uncapped_rd_en_step[0];
                new_rd_dqs_step = sp->new_step[0];
                new_rd_en_step = init_rd_en_step - ((init_rd_dqs_step - new_rd_dqs_step) >> RD_EN_DQS_CORROLATION); 
                printf("Switching to RD_DQS Step (Byte 0): %2d\n", new_rd_dqs_step);
                printf("Switching to RD_EN Step  (Byte 0): %2d\n", new_rd_en_step);
                data = SET_OVR_STEP(new_rd_dqs_step);
                WRITE_DDR40_PHY_ECC_LANE_VDL_OVRIDE_BYTE_R_Pr(unit, ci, data);
                WRITE_DDR40_PHY_ECC_LANE_VDL_OVRIDE_BYTE_R_Nr(unit, ci, data);
                data = SET_OVR_STEP(new_rd_en_step);
                WRITE_DDR40_PHY_ECC_LANE_VDL_OVRIDE_BYTE_BIT_RD_ENr(unit, ci, data);
                new_rd_en_avg_step = new_rd_en_step;
                data = SET_OVR_STEP(new_rd_en_avg_step);
                WRITE_DDR40_PHY_ECC_LANE_VDL_OVRIDE_BYTE_RD_ENr(unit, ci, data);
            } else {
#endif
                READ_DDR40_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE0_R_Pr(unit, ci, &data);
                init_rd_dqs_step =
                    DDR40_GET_FIELD(data, DDR40_PHY_WORD_LANE_1, VDL_OVRIDE_BYTE0_R_P,
                                    OVR_STEP);
                init_rd_en_step = (*vwsPtr)[vref_pick].uncapped_rd_en_step[0];
                new_rd_dqs_step = sp->new_step[0];
                new_rd_en_step = init_rd_en_step - ((init_rd_dqs_step - new_rd_dqs_step) >> RD_EN_DQS_CORROLATION); 
                printf("Switching to RD_DQS Step (Byte 0): %2d\n", new_rd_dqs_step);
                printf("Switching to RD_EN Step  (Byte 0): %2d\n", new_rd_en_step);
                data = SET_OVR_STEP(new_rd_dqs_step);
                WRITE_DDR40_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE0_R_Pr(unit, ci, data);
                WRITE_DDR40_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE0_R_Nr(unit, ci, data);
                data = SET_OVR_STEP(new_rd_en_step);
                WRITE_DDR40_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE0_BIT_RD_ENr(unit, ci, data);
                READ_DDR40_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE1_R_Pr(unit, ci, &data);
                new_rd_en_avg_step = new_rd_en_step;
                init_rd_dqs_step =
                    DDR40_GET_FIELD(data, DDR40_PHY_WORD_LANE_1, VDL_OVRIDE_BYTE1_R_P,
                                    OVR_STEP);
                init_rd_en_step = (*vwsPtr)[vref_pick].uncapped_rd_en_step[1];
                new_rd_dqs_step = sp->new_step[8];
                new_rd_en_step = init_rd_en_step - ((init_rd_dqs_step - new_rd_dqs_step) >> RD_EN_DQS_CORROLATION); 
                printf("Switching to RD_DQS Step (Byte 1): %2d\n", new_rd_dqs_step);
                printf("Switching to RD_EN Step  (Byte 1): %2d\n", new_rd_en_step);
                data = SET_OVR_STEP(new_rd_dqs_step);
                WRITE_DDR40_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE1_R_Pr(unit, ci, data);
                WRITE_DDR40_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE1_R_Nr(unit, ci, data);
                data = SET_OVR_STEP(new_rd_en_step);
                WRITE_DDR40_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE1_BIT_RD_ENr(unit, ci, data);
                new_rd_en_avg_step = (new_rd_en_avg_step + new_rd_en_step) >> 1;
                data = SET_OVR_STEP(new_rd_en_avg_step);
                WRITE_DDR40_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE_RD_ENr(unit, ci, data);
#ifdef CONFIG_IPROC_DDR_ECC
            }
#endif
        }
        break;
    case SHMOO_WR_DQ:
        n = 0;
#ifdef CONFIG_IPROC_DDR_ECC
        if(sp->wl == 2) {
            for (byte = 0; byte < 1; byte++) {
                for (bit = 0; bit < 4; bit++) {
                    data = SET_OVR_STEP(sp->new_step[n]);
                    WRITE_DDR40_PHY_WORD_LANE_x_VDL_OVRIDE_BYTEy_BITz_Wr(sp->wl, byte,
                                                                         bit, unit,
                                                                         ci, data);
                    n++;
                }
            }
        } else {
#endif
            for (byte = 0; byte < 2; byte++) {
                for (bit = 0; bit < 8; bit++) {
                    data = SET_OVR_STEP(sp->new_step[n]);
                    indx = (sp->wl*4) + (byte*2) + (bit/4);
                    //COMPILER_REFERENCE(indx);
                    WRITE_DDR40_PHY_WORD_LANE_x_VDL_OVRIDE_BYTEy_BITz_Wr(sp->wl, byte,
                                                                         bit, unit,
                                                                         ci, data);
                    n++;
                }
            }
#ifdef CONFIG_IPROC_DDR_ECC
        }
#endif
        break;
    case SHMOO_ADDRC:
        n = 0;
        if (sp->wl == 0) {
            data = SET_OVR_STEP(sp->new_step[n]);
            DDR40_SET_FIELD(data, DDR40_PHY_CONTROL_REGS, VDL_OVRIDE_BIT_CTL,
                            BYTE_SEL, 0);
            WRITE_DDR40_PHY_CONTROL_REGS_VDL_OVRIDE_BIT_CTLr(unit, ci, data);
            DDR40_SET_FIELD(data, DDR40_PHY_CONTROL_REGS, VDL_OVRIDE_BYTE_CTL,
                            BYTE_SEL, 0);
            WRITE_DDR40_PHY_CONTROL_REGS_VDL_OVRIDE_BYTE_CTLr(unit, ci, 0);
            sal_usleep(2000);
            _soc_mem_reset_and_init_after_shmoo_addr(unit, ci);
            n = _test_func_self_test_2(unit, ci, 0, 1);
        }
        break;
    default:
        break;
    }
    return SOC_E_NONE;
}

/***********************************************************************
* Functions to schmoo the DDR vdl/voltage etc.
***********************************************************************/
int
soc_ddr40_shmoo_ctl(int unit, int ci, uint32 phyType, uint32 ctlType, int stat, int isplot)
{
    soc_ddr_shmoo_param_t *sp;

    if (!stat) {
        switch(ctlType) {
            case DDR_CTLR_TRSVP:
                break;
            case DDR_CTLR_T0:
                if((ci & 0x1) == 0) {
                    if(_check_dram_either(ci)) {
                        SOC_IF_ERROR_RETURN(_soc_ddr40_phy_PVT_ctl(unit, ci, 1, stat));
                        SOC_IF_ERROR_RETURN(_soc_ddr_shmoo_prepare_for_shmoo(unit, ci));
                    }
                }
                sp = sal_alloc(sizeof(soc_ddr_shmoo_param_t), "Shmoo_Params");
                if (sp == NULL) {
                    return(SOC_E_MEMORY);
                }
                sal_memset(sp, 0, sizeof(soc_ddr_shmoo_param_t));
                sp->vwsPtr = sal_alloc(sizeof(vref_word_shmoo), "Shmoo_vws");
                if (sp->vwsPtr == NULL) {
                    sal_free(sp);
                    return(SOC_E_MEMORY);
                }
                for(sp->wl = 0; sp->wl < 2; sp->wl++) {
                    if(!_check_dram(ci+(sp->wl))) {
                        continue;
                    }
                    for(sp->type = 0; sp->type < SHMOO_ADDRC; sp->type++) {
                        _soc_ddr40_shmoo_get_init_step(unit, ci, sp);
                        _soc_ddr40_shmoo_do(unit, ci, sp);
                        _soc_ddr40_shmoo_calib(unit,ci, sp);
                        _soc_ddr40_shmoo_set_new_step(unit, ci, sp);
                        if (isplot) {
                            _soc_ddr40_shmoo_plot(sp);
                        }
                    }
                    if((_check_dram_both(ci+(sp->wl)) && (sp->wl == 1)) || (!_check_dram_both(ci+(sp->wl)) && _check_dram_either(ci+(sp->wl)))) {                       
                        sp->wl = 0;
                        for(sp->type = SHMOO_ADDRC; sp->type < SHMOO_WR_DM; sp->type++) {
                            _soc_ddr40_shmoo_get_init_step(unit, ci, sp);
                            _soc_ddr40_shmoo_do(unit, ci, sp);
                            _soc_ddr40_shmoo_calib(unit,ci,sp);
                            _soc_ddr40_shmoo_set_new_step(unit, ci, sp);
                            if (isplot) {
                                _soc_ddr40_shmoo_plot(sp);
                            }
                        }
                        sp->wl = 1;
                    }
                }
                if (sp->vwsPtr != NULL) {
                    sal_free(sp->vwsPtr);
                }
                if (sp != NULL) {
                    sal_free(sp);
                }
                soc_cm_debug(DK_VERBOSE, "DDR Tuning Complete\n");
                break;
            case DDR_CTLR_T1:
				printf("DDR_CTLR_T1\n");
                if((ci & 0x1) == 0) {
                    if(_check_dram_either(ci)) {
                        SOC_IF_ERROR_RETURN(_soc_ddr40_phy_PVT_ctl(unit, ci, 1, stat));
                        SOC_IF_ERROR_RETURN(_soc_ddr_shmoo_prepare_for_shmoo(unit, ci));
                    }
                }
				printf("sizeof(soc_ddr_shmoo_param_t) = %x\n", sizeof(soc_ddr_shmoo_param_t));

                sp = DDR_SHMOO_PARAM_MEM_PTR; //sal_alloc(sizeof(soc_ddr_shmoo_param_t), "Shmoo_Params");
                if (sp == NULL) {
                    return(SOC_E_MEMORY);
                }
				printf("sal_memset\n");
                //sal_memset(sp, 0, sizeof(soc_ddr_shmoo_param_t));
				printf("sizeof(vref_word_shmoo) = %x\n", sizeof(vref_word_shmoo));
                sp->vwsPtr = DDR_SHMOO_VREFW_MEM_PTR; //sal_alloc(sizeof(vref_word_shmoo), "Shmoo_vws");
                if (sp->vwsPtr == NULL) {
                    //sal_free(sp);
                    return(SOC_E_MEMORY);
                }
				printf("BEGIN SHMOO\n");
                for(sp->wl = 0; sp->wl < 3; sp->wl++) {
#ifdef CONFIG_IPROC_DDR_ECC
                    if(sp->wl == 2) {
                        if(!_check_dram_either(ci)) {
                            continue;
                        }
                    } else {
#endif
                        if(!_check_dram(ci+(sp->wl))) {
                            continue;
                        }
#ifdef CONFIG_IPROC_DDR_ECC
                    }
#endif
                    sp->type = SHMOO_INIT_VDL_RESULT;
                    printf("BEFORE SHMOO: Type = %d\tCI = %d\tWL = %d\n", sp->type, ci, sp->wl);
                    _shmoo_print_diagnostics(unit, ci, sp->wl);
                    _soc_ddr40_shmoo_get_init_step(unit, ci, sp);
                    _soc_ddr40_shmoo_do(unit, ci, sp);
                    _soc_ddr40_shmoo_calib(unit,ci, sp);
                    _soc_ddr40_shmoo_set_new_step(unit, ci, sp);
                    printf("AFTER SHMOO:  Type = %d\tCI = %d\tWL = %d\n", sp->type, ci, sp->wl);
                    _shmoo_print_diagnostics(unit, ci, sp->wl);
                    if (isplot || SOC_DEBUG_CHECK(SOC_DBG_DDR)) {
                        _soc_ddr40_shmoo_plot(sp);
                    }
                }
                for(sp->wl = 0; sp->wl < 3; sp->wl++) {
#ifdef CONFIG_IPROC_DDR_ECC
                    if(sp->wl == 2) {
                        if(!_check_dram_either(ci)) {
                            continue;
                        }

                        printf("Enabling DDR ECC reporting\n");
                        /* Clear DDR interrupts if any */
                        *(unsigned int *)(DDR_DENALI_CTL_213) = 0x00FFFFFF;
                        __udelay(1000);
                        reg32_set_bits((volatile uint32_t *)DDR_DENALI_CTL_67, 0x01); //Disable auto correction
                        reg32_set_bits((volatile uint32_t *)DDR_DENALI_CTL_66, 0x01); //Enable ECC
                        iproc_clear_ecc_syndrome();

                    } else {
#endif
                        if(!_check_dram(ci+(sp->wl))) {
                            continue;
                        }
#ifdef CONFIG_IPROC_DDR_ECC
                    }
#endif
                    sp->type = SHMOO_RD_EN;
                    printf("BEFORE SHMOO: Type = %d\tCI = %d\tWL = %d\n", sp->type, ci, sp->wl);
                    _shmoo_print_diagnostics(unit, ci, sp->wl);
                    _soc_ddr40_shmoo_get_init_step(unit, ci, sp);
                    _soc_ddr40_shmoo_do(unit, ci, sp);
                    _soc_ddr40_shmoo_calib(unit,ci, sp);
                    _soc_ddr40_shmoo_set_new_step(unit, ci, sp);
                    printf("AFTER SHMOO:  Type = %d\tCI = %d\tWL = %d\n", sp->type, ci, sp->wl);
                    _shmoo_print_diagnostics(unit, ci, sp->wl);
                    if (isplot || SOC_DEBUG_CHECK(SOC_DBG_DDR)) {
                        _soc_ddr40_shmoo_plot(sp);
                    }
                    sp->type = SHMOO_RD_DQ;
                    printf("BEFORE SHMOO: Type = %d\tCI = %d\tWL = %d\n", sp->type, ci, sp->wl);
                    _shmoo_print_diagnostics(unit, ci, sp->wl);
                    _soc_ddr40_shmoo_get_init_step(unit, ci, sp);
                    _soc_ddr40_shmoo_do(unit, ci, sp);
                    _soc_ddr40_shmoo_calib(unit,ci, sp);
                    _soc_ddr40_shmoo_set_new_step(unit, ci, sp);
                    printf("AFTER SHMOO:  Type = %d\tCI = %d\tWL = %d\n", sp->type, ci, sp->wl);
                    _shmoo_print_diagnostics(unit, ci, sp->wl);
                    if (isplot || SOC_DEBUG_CHECK(SOC_DBG_DDR)) {
                        _soc_ddr40_shmoo_plot(sp);
                    }
                    sp->type = SHMOO_WR_DQ;
                    printf("BEFORE SHMOO: Type = %d\tCI = %d\tWL = %d\n", sp->type, ci, sp->wl);
                    _shmoo_print_diagnostics(unit, ci, sp->wl);
                    _soc_ddr40_shmoo_get_init_step(unit, ci, sp);
                    _soc_ddr40_shmoo_do(unit, ci, sp);
                    _soc_ddr40_shmoo_calib(unit,ci, sp);
                    _soc_ddr40_shmoo_set_new_step(unit, ci, sp);
                    printf("AFTER SHMOO:  Type = %d\tCI = %d\tWL = %d\n", sp->type, ci, sp->wl);
                    _shmoo_print_diagnostics(unit, ci, sp->wl);
                    if (isplot || SOC_DEBUG_CHECK(SOC_DBG_DDR)) {
                        _soc_ddr40_shmoo_plot(sp);
                    }
#if 0 // Srini - revisit - shutdown address shmoo
                    if((_check_dram_both(ci) && (sp->wl == 1)) || (!_check_dram_both(ci) && _check_dram_either(ci) && (sp->wl != 2))) {
                        sp->wl = 0;
                        for(sp->type = SHMOO_ADDRC; sp->type < SHMOO_WR_DM; sp->type++) {
                            printf("BEFORE SHMOO: Type = %d\tCI = %d\tWL = Both\n", sp->type, ci);
                            _shmoo_print_diagnostics(unit, ci, sp->wl);
                            _soc_ddr40_shmoo_get_init_step(unit, ci, sp);
                            _soc_ddr40_shmoo_do(unit, ci, sp);
                            _soc_ddr40_shmoo_calib(unit,ci,sp);
                            _soc_ddr40_shmoo_set_new_step(unit, ci, sp);
                            printf("AFTER SHMOO:  Type = %d\tCI = %d\tWL = Both\n", sp->type, ci);
                            _shmoo_print_diagnostics(unit, ci, sp->wl);
                            if (isplot || SOC_DEBUG_CHECK(SOC_DBG_DDR)) {
                                _soc_ddr40_shmoo_plot(sp);
                            }
                        }
                        sp->wl = 1;
					}
#endif
                }
                if (sp->vwsPtr != NULL) {
                    //sal_free(sp->vwsPtr);
                }
                if (sp != NULL) {
                    //sal_free(sp);
                }
                soc_cm_debug(DK_VERBOSE, "DDR Tuning Complete\n");
                break;
            case DDR_CTLR_T2:
                if((ci & 0x1) == 0) {
                    if(_check_dram_either(ci)) {
                        SOC_IF_ERROR_RETURN(_soc_ddr40_phy_PVT_ctl(unit, ci, 1, stat));
                        SOC_IF_ERROR_RETURN(_soc_ddr_shmoo_prepare_for_shmoo(unit, ci));
                    }
                }
                sp = sal_alloc(sizeof(soc_ddr_shmoo_param_t), "Shmoo_Params");
                if (sp == NULL) {
                    return(SOC_E_MEMORY);
                }
                sal_memset(sp, 0, sizeof(soc_ddr_shmoo_param_t));
                sp->vwsPtr = sal_alloc(sizeof(vref_word_shmoo), "Shmoo_vws");
                if (sp->vwsPtr == NULL) {
                    sal_free(sp);
                    return(SOC_E_MEMORY);
                }
                for(sp->wl = 0; sp->wl < 2; sp->wl++) {
                    if(!_check_dram(ci+(sp->wl))) {
                        continue;
                    }
                    sp->type = SHMOO_INIT_VDL_RESULT;
                    printf("BEFORE SHMOO: Type = %d\tCI = %d\tWL = %d\n", sp->type, ci, sp->wl);
                    _shmoo_print_diagnostics(unit, ci, sp->wl);
                    _soc_ddr40_shmoo_get_init_step(unit, ci, sp);
                    _soc_ddr40_shmoo_do(unit, ci, sp);
                    _soc_ddr40_shmoo_calib(unit,ci, sp);
                    _soc_ddr40_shmoo_set_new_step(unit, ci, sp);
                    printf("AFTER SHMOO:  Type = %d\tCI = %d\tWL = %d\n", sp->type, ci, sp->wl);
                    _shmoo_print_diagnostics(unit, ci, sp->wl);
                    if (isplot || SOC_DEBUG_CHECK(SOC_DBG_DDR)) {
                        _soc_ddr40_shmoo_plot(sp);
                    }
                }
                for(sp->wl = 0; sp->wl < 2; sp->wl++) {
                    if(!_check_dram(ci+(sp->wl))) {
                        continue;
                    }
                    sp->type = SHMOO_RD_EN;
                    printf("BEFORE SHMOO: Type = %d\tCI = %d\tWL = %d\n", sp->type, ci, sp->wl);
                    _shmoo_print_diagnostics(unit, ci, sp->wl);
                    _soc_ddr40_shmoo_get_init_step(unit, ci, sp);
                    _soc_ddr40_shmoo_do(unit, ci, sp);
                    _soc_ddr40_shmoo_calib(unit,ci, sp);
                    _soc_ddr40_shmoo_set_new_step(unit, ci, sp);
                    printf("AFTER SHMOO:  Type = %d\tCI = %d\tWL = %d\n", sp->type, ci, sp->wl);
                    _shmoo_print_diagnostics(unit, ci, sp->wl);
                    if (isplot || SOC_DEBUG_CHECK(SOC_DBG_DDR)) {
                        _soc_ddr40_shmoo_plot(sp);
                    }
                    sp->type = SHMOO_RD_DQ;
                    printf("BEFORE SHMOO: Type = %d\tCI = %d\tWL = %d\n", sp->type, ci, sp->wl);
                    _shmoo_print_diagnostics(unit, ci, sp->wl);
                    _soc_ddr40_shmoo_get_init_step(unit, ci, sp);
                    _soc_ddr40_shmoo_do(unit, ci, sp);
                    _soc_ddr40_shmoo_calib(unit,ci, sp);
                    _soc_ddr40_shmoo_set_new_step(unit, ci, sp);
                    printf("AFTER SHMOO:  Type = %d\tCI = %d\tWL = %d\n", sp->type, ci, sp->wl);
                    _shmoo_print_diagnostics(unit, ci, sp->wl);
                    if (isplot || SOC_DEBUG_CHECK(SOC_DBG_DDR)) {
                        _soc_ddr40_shmoo_plot(sp);
                    }
                    sp->type = SHMOO_WR_DQ;
                    printf("BEFORE SHMOO: Type = %d\tCI = %d\tWL = %d\n", sp->type, ci, sp->wl);
                    _shmoo_print_diagnostics(unit, ci, sp->wl);
                    _soc_ddr40_shmoo_get_init_step(unit, ci, sp);
                    _soc_ddr40_shmoo_do(unit, ci, sp);
                    _soc_ddr40_shmoo_calib(unit,ci, sp);
                    _soc_ddr40_shmoo_set_new_step(unit, ci, sp);
                    printf("AFTER SHMOO:  Type = %d\tCI = %d\tWL = %d\n", sp->type, ci, sp->wl);
                    _shmoo_print_diagnostics(unit, ci, sp->wl);
                    if (isplot || SOC_DEBUG_CHECK(SOC_DBG_DDR)) {
                        _soc_ddr40_shmoo_plot(sp);
                    }
                    if((_check_dram_both(ci+(sp->wl)) && (sp->wl == 1)) || (!_check_dram_both(ci+(sp->wl)) && _check_dram_either(ci+(sp->wl)))) {
                        sp->wl = 0;
                        for(sp->type = SHMOO_ADDRC; sp->type < SHMOO_WR_DM; sp->type++) {
                            printf("BEFORE SHMOO: Type = %d\tCI = %d\tWL = Both\n", sp->type, ci);
                            _shmoo_print_diagnostics(unit, ci, sp->wl);
                            _soc_ddr40_shmoo_get_init_step(unit, ci, sp);
                            _soc_ddr40_shmoo_do(unit, ci, sp);
                            _soc_ddr40_shmoo_calib(unit,ci,sp);
                            _soc_ddr40_shmoo_set_new_step(unit, ci, sp);
                            printf("AFTER SHMOO:  Type = %d\tCI = %d\tWL = Both\n", sp->type, ci);
                            _shmoo_print_diagnostics(unit, ci, sp->wl);
                            if (isplot || SOC_DEBUG_CHECK(SOC_DBG_DDR)) {
                                _soc_ddr40_shmoo_plot(sp);
                            }
                        }
                        sp->wl = 1;
                    }
                }
                if (sp->vwsPtr != NULL) {
                    sal_free(sp->vwsPtr);
                }
                if (sp != NULL) {
                    sal_free(sp);
                }
                soc_cm_debug(DK_VERBOSE, "DDR Tuning Complete\n");
                break;
            case DDR_CTLR_T3:
                break;
            default:
                break;
        }
    } else {
        /* report only */
        switch(ctlType) {
            case DDR_CTLR_TRSVP:
                break;
            case DDR_CTLR_T0:
                break;
            case DDR_CTLR_T1:
                break;
            case DDR_CTLR_T2:
                break;
            case DDR_CTLR_T3:
                break;
            default:
                break;
        }
    }
    return SOC_E_NONE;
}

#define OVR_COMPRESS(_v) (((_v) & 0x3f)  | \
                           (((_v) & 0x100) ? 0x40 : 0) | \
                           (((_v) & 0x10000) ? 0x80 : 0))

#if 0
int soc_ddr40_shmoo_savecfg(int unit, int ci)
{
    char name_str[32], val_str[96];
    uint32 value[4], valarr[4];

    if (soc_mem_config_set == NULL) {
        return SOC_E_UNAVAIL;
    }

    /* SHMOO_RD_DATA_DLY: */
    READ_DDR40_PHY_WORD_LANE_0_READ_DATA_DLYr(unit, ci, (uint32 *)&valarr[0]); /* Only Bits 0..2 */
    READ_DDR40_PHY_WORD_LANE_1_READ_DATA_DLYr(unit, ci, (uint32 *)&valarr[1]); /* Only Bits 0..2 */
    value[0] = ((valarr[1] & 0x7) << 8) | (valarr[0] & 0x7);
    sal_sprintf(name_str, "%s_ci%d.%d", spn_DDR3_TUNE_RD_DATA_DLY, ci, unit);
    sal_sprintf(val_str, "0x%08x", value[0]);
    if (soc_mem_config_set(name_str, val_str) < 0) {
         return SOC_E_MEMORY;
    }

    /* SHMOO_RD_EN: */
    READ_DDR40_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE0_BIT_RD_ENr(unit, ci, (uint32 *)&valarr[0]);
    READ_DDR40_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE1_BIT_RD_ENr(unit, ci, (uint32 *)&valarr[1]);
    READ_DDR40_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE_RD_ENr(unit, ci, (uint32 *)&valarr[2]);
    value[0] = OVR_COMPRESS(valarr[0]) | (OVR_COMPRESS(valarr[1]) << 8) | (OVR_COMPRESS(valarr[2]) << 16);
    READ_DDR40_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE0_BIT_RD_ENr(unit, ci, (uint32 *)&valarr[0]);
    READ_DDR40_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE1_BIT_RD_ENr(unit, ci, (uint32 *)&valarr[1]);
    READ_DDR40_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE_RD_ENr(unit, ci, (uint32 *)&valarr[2]);
    value[1] = OVR_COMPRESS(valarr[0]) | (OVR_COMPRESS(valarr[1]) << 8) | (OVR_COMPRESS(valarr[2]) << 16);
    sal_sprintf(name_str, "%s_ci%d.%d", spn_DDR3_TUNE_RD_EN, ci, unit);
    sal_sprintf(val_str, "0x%08x,0x%08x", value[0], value[1]);
    if (soc_mem_config_set(name_str, val_str) < 0) {
         return SOC_E_MEMORY;
    }

    /* SHMOO_RD_DQ : */
    READ_DDR40_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE0_BIT0_R_Pr(unit, ci, (uint32 *)&valarr[0]);
    READ_DDR40_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE0_BIT1_R_Pr(unit, ci, (uint32 *)&valarr[1]);
    READ_DDR40_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE0_BIT2_R_Pr(unit, ci, (uint32 *)&valarr[2]);
    READ_DDR40_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE0_BIT3_R_Pr(unit, ci, (uint32 *)&valarr[3]);
    value[0] = OVR_COMPRESS(valarr[0]) |  (OVR_COMPRESS(valarr[1]) << 8) |
               (OVR_COMPRESS(valarr[2]) << 16) | (OVR_COMPRESS(valarr[3]) << 24);
    READ_DDR40_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE0_BIT4_R_Pr(unit, ci, (uint32 *)&valarr[0]);
    READ_DDR40_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE0_BIT5_R_Pr(unit, ci, (uint32 *)&valarr[1]);
    READ_DDR40_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE0_BIT6_R_Pr(unit, ci, (uint32 *)&valarr[2]);
    READ_DDR40_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE0_BIT7_R_Pr(unit, ci, (uint32 *)&valarr[3]);
    value[1] = OVR_COMPRESS(valarr[0]) |  (OVR_COMPRESS(valarr[1]) << 8) |
               (OVR_COMPRESS(valarr[2]) << 16) | (OVR_COMPRESS(valarr[3]) << 24);
    READ_DDR40_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE1_BIT0_R_Pr(unit, ci, (uint32 *)&valarr[0]);
    READ_DDR40_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE1_BIT1_R_Pr(unit, ci, (uint32 *)&valarr[1]);
    READ_DDR40_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE1_BIT2_R_Pr(unit, ci, (uint32 *)&valarr[2]);
    READ_DDR40_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE1_BIT3_R_Pr(unit, ci, (uint32 *)&valarr[3]);
    value[2] = OVR_COMPRESS(valarr[0]) |  (OVR_COMPRESS(valarr[1]) << 8) |
               (OVR_COMPRESS(valarr[2]) << 16) | (OVR_COMPRESS(valarr[3]) << 24);
    READ_DDR40_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE1_BIT4_R_Pr(unit, ci, (uint32 *)&valarr[0]);
    READ_DDR40_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE1_BIT5_R_Pr(unit, ci, (uint32 *)&valarr[1]);
    READ_DDR40_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE1_BIT6_R_Pr(unit, ci, (uint32 *)&valarr[2]);
    READ_DDR40_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE1_BIT7_R_Pr(unit, ci, (uint32 *)&valarr[3]);
    value[3] = OVR_COMPRESS(valarr[0]) |  (OVR_COMPRESS(valarr[1]) << 8) |
               (OVR_COMPRESS(valarr[2]) << 16) | (OVR_COMPRESS(valarr[3]) << 24);
    sal_sprintf(name_str, "%s_ci%d.%d", spn_DDR3_TUNE_RD_DQ_WL0_RP, ci, unit);
    sal_sprintf(val_str, "0x%08x,0x%08x,0x%08x,0x%08x", value[0], value[1], value[2], value[3]);
    if (soc_mem_config_set(name_str, val_str) < 0) {
         return SOC_E_MEMORY;
    }

    READ_DDR40_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE0_BIT0_R_Pr(unit, ci, (uint32 *)&valarr[0]);
    READ_DDR40_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE0_BIT1_R_Pr(unit, ci, (uint32 *)&valarr[1]);
    READ_DDR40_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE0_BIT2_R_Pr(unit, ci, (uint32 *)&valarr[2]);
    READ_DDR40_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE0_BIT3_R_Pr(unit, ci, (uint32 *)&valarr[3]);
    value[0] = OVR_COMPRESS(valarr[0]) |  (OVR_COMPRESS(valarr[1]) << 8) |
               (OVR_COMPRESS(valarr[2]) << 16) | (OVR_COMPRESS(valarr[3]) << 24);
    READ_DDR40_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE0_BIT4_R_Pr(unit, ci, (uint32 *)&valarr[0]);
    READ_DDR40_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE0_BIT5_R_Pr(unit, ci, (uint32 *)&valarr[1]);
    READ_DDR40_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE0_BIT6_R_Pr(unit, ci, (uint32 *)&valarr[2]);
    READ_DDR40_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE0_BIT7_R_Pr(unit, ci, (uint32 *)&valarr[3]);
    value[1] = OVR_COMPRESS(valarr[0]) |  (OVR_COMPRESS(valarr[1]) << 8) |
               (OVR_COMPRESS(valarr[2]) << 16) | (OVR_COMPRESS(valarr[3]) << 24);
    READ_DDR40_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE1_BIT0_R_Pr(unit, ci, (uint32 *)&valarr[0]);
    READ_DDR40_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE1_BIT1_R_Pr(unit, ci, (uint32 *)&valarr[1]);
    READ_DDR40_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE1_BIT2_R_Pr(unit, ci, (uint32 *)&valarr[2]);
    READ_DDR40_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE1_BIT3_R_Pr(unit, ci, (uint32 *)&valarr[3]);
    value[2] = OVR_COMPRESS(valarr[0]) |  (OVR_COMPRESS(valarr[1]) << 8) |
               (OVR_COMPRESS(valarr[2]) << 16) | (OVR_COMPRESS(valarr[3]) << 24);
    READ_DDR40_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE1_BIT4_R_Pr(unit, ci, (uint32 *)&valarr[0]);
    READ_DDR40_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE1_BIT5_R_Pr(unit, ci, (uint32 *)&valarr[1]);
    READ_DDR40_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE1_BIT6_R_Pr(unit, ci, (uint32 *)&valarr[2]);
    READ_DDR40_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE1_BIT7_R_Pr(unit, ci, (uint32 *)&valarr[3]);
    value[3] = OVR_COMPRESS(valarr[0]) |  (OVR_COMPRESS(valarr[1]) << 8) |
               (OVR_COMPRESS(valarr[2]) << 16) | (OVR_COMPRESS(valarr[3]) << 24);
    sal_sprintf(name_str, "%s_ci%d.%d", spn_DDR3_TUNE_RD_DQ_WL1_RP, ci, unit);
    sal_sprintf(val_str, "0x%08x,0x%08x,0x%08x,0x%08x", value[0], value[1], value[2], value[3]);
    if (soc_mem_config_set(name_str, val_str) < 0) {
         return SOC_E_MEMORY;
    }

    READ_DDR40_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE0_BIT0_R_Nr(unit, ci, (uint32 *)&valarr[0]);
    READ_DDR40_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE0_BIT1_R_Nr(unit, ci, (uint32 *)&valarr[1]);
    READ_DDR40_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE0_BIT2_R_Nr(unit, ci, (uint32 *)&valarr[2]);
    READ_DDR40_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE0_BIT3_R_Nr(unit, ci, (uint32 *)&valarr[3]);
    value[0] = OVR_COMPRESS(valarr[0]) |  (OVR_COMPRESS(valarr[1]) << 8) |
               (OVR_COMPRESS(valarr[2]) << 16) | (OVR_COMPRESS(valarr[3]) << 24);
    READ_DDR40_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE0_BIT4_R_Nr(unit, ci, (uint32 *)&valarr[0]);
    READ_DDR40_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE0_BIT5_R_Nr(unit, ci, (uint32 *)&valarr[1]);
    READ_DDR40_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE0_BIT6_R_Nr(unit, ci, (uint32 *)&valarr[2]);
    READ_DDR40_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE0_BIT7_R_Nr(unit, ci, (uint32 *)&valarr[3]);
    value[1] = OVR_COMPRESS(valarr[0]) |  (OVR_COMPRESS(valarr[1]) << 8) |
               (OVR_COMPRESS(valarr[2]) << 16) | (OVR_COMPRESS(valarr[3]) << 24);
    READ_DDR40_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE1_BIT0_R_Nr(unit, ci, (uint32 *)&valarr[0]);
    READ_DDR40_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE1_BIT1_R_Nr(unit, ci, (uint32 *)&valarr[1]);
    READ_DDR40_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE1_BIT2_R_Nr(unit, ci, (uint32 *)&valarr[2]);
    READ_DDR40_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE1_BIT3_R_Nr(unit, ci, (uint32 *)&valarr[3]);
    value[2] = OVR_COMPRESS(valarr[0]) |  (OVR_COMPRESS(valarr[1]) << 8) |
               (OVR_COMPRESS(valarr[2]) << 16) | (OVR_COMPRESS(valarr[3]) << 24);
    READ_DDR40_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE1_BIT4_R_Nr(unit, ci, (uint32 *)&valarr[0]);
    READ_DDR40_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE1_BIT5_R_Nr(unit, ci, (uint32 *)&valarr[1]);
    READ_DDR40_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE1_BIT6_R_Nr(unit, ci, (uint32 *)&valarr[2]);
    READ_DDR40_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE1_BIT7_R_Nr(unit, ci, (uint32 *)&valarr[3]);
    value[3] = OVR_COMPRESS(valarr[0]) |  (OVR_COMPRESS(valarr[1]) << 8) |
               (OVR_COMPRESS(valarr[2]) << 16) | (OVR_COMPRESS(valarr[3]) << 24);
    sal_sprintf(name_str, "%s_ci%d.%d", spn_DDR3_TUNE_RD_DQ_WL0_RN, ci, unit);
    sal_sprintf(val_str, "0x%08x,0x%08x,0x%08x,0x%08x", value[0], value[1], value[2], value[3]);
    if (soc_mem_config_set(name_str, val_str) < 0) {
         return SOC_E_MEMORY;
    }

    READ_DDR40_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE0_BIT0_R_Nr(unit, ci, (uint32 *)&valarr[0]);
    READ_DDR40_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE0_BIT1_R_Nr(unit, ci, (uint32 *)&valarr[1]);
    READ_DDR40_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE0_BIT2_R_Nr(unit, ci, (uint32 *)&valarr[2]);
    READ_DDR40_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE0_BIT3_R_Nr(unit, ci, (uint32 *)&valarr[3]);
    value[0] = OVR_COMPRESS(valarr[0]) |  (OVR_COMPRESS(valarr[1]) << 8) |
               (OVR_COMPRESS(valarr[2]) << 16) | (OVR_COMPRESS(valarr[3]) << 24);
    READ_DDR40_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE0_BIT4_R_Nr(unit, ci, (uint32 *)&valarr[0]);
    READ_DDR40_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE0_BIT5_R_Nr(unit, ci, (uint32 *)&valarr[1]);
    READ_DDR40_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE0_BIT6_R_Nr(unit, ci, (uint32 *)&valarr[2]);
    READ_DDR40_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE0_BIT7_R_Nr(unit, ci, (uint32 *)&valarr[3]);
    value[1] = OVR_COMPRESS(valarr[0]) |  (OVR_COMPRESS(valarr[1]) << 8) |
               (OVR_COMPRESS(valarr[2]) << 16) | (OVR_COMPRESS(valarr[3]) << 24);
    READ_DDR40_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE1_BIT0_R_Nr(unit, ci, (uint32 *)&valarr[0]);
    READ_DDR40_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE1_BIT1_R_Nr(unit, ci, (uint32 *)&valarr[1]);
    READ_DDR40_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE1_BIT2_R_Nr(unit, ci, (uint32 *)&valarr[2]);
    READ_DDR40_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE1_BIT3_R_Nr(unit, ci, (uint32 *)&valarr[3]);
    value[2] = OVR_COMPRESS(valarr[0]) |  (OVR_COMPRESS(valarr[1]) << 8) |
               (OVR_COMPRESS(valarr[2]) << 16) | (OVR_COMPRESS(valarr[3]) << 24);
    READ_DDR40_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE1_BIT4_R_Nr(unit, ci, (uint32 *)&valarr[0]);
    READ_DDR40_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE1_BIT5_R_Nr(unit, ci, (uint32 *)&valarr[1]);
    READ_DDR40_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE1_BIT6_R_Nr(unit, ci, (uint32 *)&valarr[2]);
    READ_DDR40_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE1_BIT7_R_Nr(unit, ci, (uint32 *)&valarr[3]);
    value[3] = OVR_COMPRESS(valarr[0]) |  (OVR_COMPRESS(valarr[1]) << 8) |
               (OVR_COMPRESS(valarr[2]) << 16) | (OVR_COMPRESS(valarr[3]) << 24);
    sal_sprintf(name_str, "%s_ci%d.%d", spn_DDR3_TUNE_RD_DQ_WL1_RN, ci, unit);
    sal_sprintf(val_str, "0x%08x,0x%08x,0x%08x,0x%08x", value[0], value[1], value[2], value[3]);
    if (soc_mem_config_set(name_str, val_str) < 0) {
         return SOC_E_MEMORY;
    }

    /* SHMOO_RD_DQS: */
    READ_DDR40_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE0_R_Pr(unit, ci, (uint32 *)&valarr[0]);
    READ_DDR40_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE0_R_Nr(unit, ci, (uint32 *)&valarr[1]);
    READ_DDR40_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE1_R_Pr(unit, ci, (uint32 *)&valarr[2]);
    READ_DDR40_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE1_R_Nr(unit, ci, (uint32 *)&valarr[3]);
    value[0] = OVR_COMPRESS(valarr[0]) |  (OVR_COMPRESS(valarr[1]) << 8) |
               (OVR_COMPRESS(valarr[2]) << 16) | (OVR_COMPRESS(valarr[3]) << 24);
    READ_DDR40_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE0_R_Pr(unit, ci, (uint32 *)&valarr[0]);
    READ_DDR40_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE0_R_Nr(unit, ci, (uint32 *)&valarr[1]);
    READ_DDR40_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE1_R_Pr(unit, ci, (uint32 *)&valarr[2]);
    READ_DDR40_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE1_R_Nr(unit, ci, (uint32 *)&valarr[3]);
    value[1] = OVR_COMPRESS(valarr[0]) |  (OVR_COMPRESS(valarr[1]) << 8) |
               (OVR_COMPRESS(valarr[2]) << 16) | (OVR_COMPRESS(valarr[3]) << 24);
    sal_sprintf(name_str, "%s_ci%d.%d", spn_DDR3_TUNE_RD_DQS, ci, unit);
    sal_sprintf(val_str, "0x%08x,0x%08x", value[0], value[1]);
    if (soc_mem_config_set(name_str, val_str) < 0) {
         return SOC_E_MEMORY;
    }

    /* SHMOO_VREF: */
    READ_DDR40_PHY_CONTROL_REGS_VREF_DAC_CONTROLr(unit, ci, (uint32 *)&valarr[0]);
    sal_sprintf(name_str, "%s_ci%d.%d", spn_DDR3_TUNE_VREF, ci, unit);
    sal_sprintf(val_str, "0x%08x", valarr[0]);
    if (soc_mem_config_set(name_str, val_str) < 0) {
         return SOC_E_MEMORY;
    }

    /* SHMOO_WR_DQ : */
    READ_DDR40_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE0_Wr(unit, ci, (uint32 *)&valarr[0]);
    READ_DDR40_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE1_Wr(unit, ci, (uint32 *)&valarr[1]);
    READ_DDR40_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE0_Wr(unit, ci, (uint32 *)&valarr[2]);
    READ_DDR40_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE1_Wr(unit, ci, (uint32 *)&valarr[3]);
    value[0] = OVR_COMPRESS(valarr[0]) |  (OVR_COMPRESS(valarr[1]) << 8) |
               (OVR_COMPRESS(valarr[2]) << 16) | (OVR_COMPRESS(valarr[3]) << 24);
    sal_sprintf(name_str, "%s_ci%d.%d", spn_DDR3_TUNE_WR_DQ, ci, unit);
    sal_sprintf(val_str, "0x%08x", value[0]);
    if (soc_mem_config_set(name_str, val_str) < 0) {
         return SOC_E_MEMORY;
    }

    READ_DDR40_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE0_BIT0_Wr(unit, ci, (uint32 *)&valarr[0]);
    READ_DDR40_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE0_BIT1_Wr(unit, ci, (uint32 *)&valarr[1]);
    READ_DDR40_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE0_BIT2_Wr(unit, ci, (uint32 *)&valarr[2]);
    READ_DDR40_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE0_BIT3_Wr(unit, ci, (uint32 *)&valarr[3]);
    value[0] = OVR_COMPRESS(valarr[0]) |  (OVR_COMPRESS(valarr[1]) << 8) |
               (OVR_COMPRESS(valarr[2]) << 16) | (OVR_COMPRESS(valarr[3]) << 24);
    READ_DDR40_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE0_BIT4_Wr(unit, ci, (uint32 *)&valarr[0]);
    READ_DDR40_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE0_BIT5_Wr(unit, ci, (uint32 *)&valarr[1]);
    READ_DDR40_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE0_BIT6_Wr(unit, ci, (uint32 *)&valarr[2]);
    READ_DDR40_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE0_BIT7_Wr(unit, ci, (uint32 *)&valarr[3]);
    value[1] = OVR_COMPRESS(valarr[0]) |  (OVR_COMPRESS(valarr[1]) << 8) |
               (OVR_COMPRESS(valarr[2]) << 16) | (OVR_COMPRESS(valarr[3]) << 24);
    READ_DDR40_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE1_BIT0_Wr(unit, ci, (uint32 *)&valarr[0]);
    READ_DDR40_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE1_BIT1_Wr(unit, ci, (uint32 *)&valarr[1]);
    READ_DDR40_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE1_BIT2_Wr(unit, ci, (uint32 *)&valarr[2]);
    READ_DDR40_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE1_BIT3_Wr(unit, ci, (uint32 *)&valarr[3]);
    value[2] = OVR_COMPRESS(valarr[0]) |  (OVR_COMPRESS(valarr[1]) << 8) |
               (OVR_COMPRESS(valarr[2]) << 16) | (OVR_COMPRESS(valarr[3]) << 24);
    READ_DDR40_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE1_BIT4_Wr(unit, ci, (uint32 *)&valarr[0]);
    READ_DDR40_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE1_BIT5_Wr(unit, ci, (uint32 *)&valarr[1]);
    READ_DDR40_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE1_BIT6_Wr(unit, ci, (uint32 *)&valarr[2]);
    READ_DDR40_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE1_BIT7_Wr(unit, ci, (uint32 *)&valarr[3]);
    value[3] = OVR_COMPRESS(valarr[0]) |  (OVR_COMPRESS(valarr[1]) << 8) |
               (OVR_COMPRESS(valarr[2]) << 16) | (OVR_COMPRESS(valarr[3]) << 24);
    sal_sprintf(name_str, "%s_ci%d.%d", spn_DDR3_TUNE_WR_DQ_WL0, ci, unit);
    sal_sprintf(val_str, "0x%08x,0x%08x,0x%08x,0x%08x", value[0], value[1], value[2], value[3]);
    if (soc_mem_config_set(name_str, val_str) < 0) {
         return SOC_E_MEMORY;
    }

    READ_DDR40_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE0_BIT0_Wr(unit, ci, (uint32 *)&valarr[0]);
    READ_DDR40_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE0_BIT1_Wr(unit, ci, (uint32 *)&valarr[1]);
    READ_DDR40_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE0_BIT2_Wr(unit, ci, (uint32 *)&valarr[2]);
    READ_DDR40_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE0_BIT3_Wr(unit, ci, (uint32 *)&valarr[3]);
    value[0] = OVR_COMPRESS(valarr[0]) |  (OVR_COMPRESS(valarr[1]) << 8) |
               (OVR_COMPRESS(valarr[2]) << 16) | (OVR_COMPRESS(valarr[3]) << 24);
    READ_DDR40_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE0_BIT4_Wr(unit, ci, (uint32 *)&valarr[0]);
    READ_DDR40_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE0_BIT5_Wr(unit, ci, (uint32 *)&valarr[1]);
    READ_DDR40_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE0_BIT6_Wr(unit, ci, (uint32 *)&valarr[2]);
    READ_DDR40_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE0_BIT7_Wr(unit, ci, (uint32 *)&valarr[3]);
    value[1] = OVR_COMPRESS(valarr[0]) |  (OVR_COMPRESS(valarr[1]) << 8) |
               (OVR_COMPRESS(valarr[2]) << 16) | (OVR_COMPRESS(valarr[3]) << 24);
    READ_DDR40_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE1_BIT0_Wr(unit, ci, (uint32 *)&valarr[0]);
    READ_DDR40_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE1_BIT1_Wr(unit, ci, (uint32 *)&valarr[1]);
    READ_DDR40_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE1_BIT2_Wr(unit, ci, (uint32 *)&valarr[2]);
    READ_DDR40_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE1_BIT3_Wr(unit, ci, (uint32 *)&valarr[3]);
    value[2] = OVR_COMPRESS(valarr[0]) |  (OVR_COMPRESS(valarr[1]) << 8) |
               (OVR_COMPRESS(valarr[2]) << 16) | (OVR_COMPRESS(valarr[3]) << 24);
    READ_DDR40_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE1_BIT4_Wr(unit, ci, (uint32 *)&valarr[0]);
    READ_DDR40_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE1_BIT5_Wr(unit, ci, (uint32 *)&valarr[1]);
    READ_DDR40_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE1_BIT6_Wr(unit, ci, (uint32 *)&valarr[2]);
    READ_DDR40_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE1_BIT7_Wr(unit, ci, (uint32 *)&valarr[3]);
    value[3] = OVR_COMPRESS(valarr[0]) |  (OVR_COMPRESS(valarr[1]) << 8) |
               (OVR_COMPRESS(valarr[2]) << 16) | (OVR_COMPRESS(valarr[3]) << 24);
    sal_sprintf(name_str, "%s_ci%d.%d", spn_DDR3_TUNE_WR_DQ_WL1, ci, unit);
    sal_sprintf(val_str, "0x%08x,0x%08x,0x%08x,0x%08x", value[0], value[1], value[2], value[3]);
    if (soc_mem_config_set(name_str, val_str) < 0) {
         return SOC_E_MEMORY;
    }

    /* SHMOO_ADDRC: */
    READ_DDR40_PHY_CONTROL_REGS_VDL_OVRIDE_BIT_CTLr(unit, ci, (uint32 *)&valarr[0]);
    READ_DDR40_PHY_CONTROL_REGS_VDL_OVRIDE_BYTE_CTLr(unit, ci, (uint32 *)&valarr[1]);
    value[0] = OVR_COMPRESS(valarr[0]) | (OVR_COMPRESS(valarr[1]) << 8);
    sal_sprintf(name_str, "%s_ci%d.%d", spn_DDR3_TUNE_ADDRC, ci, unit);
    sal_sprintf(val_str, "0x%08x", value[0]);
    if (soc_mem_config_set(name_str, val_str) < 0) {
         return SOC_E_MEMORY;
    }
    return SOC_E_NONE;
}

#define OVR_EXPAND(_v) (((_v) & 0x3f)  | (0x20000) | \
                           (((_v)&0x40) ? 0x100 : 0) | \
                           (((_v)&0x80) ? 0x10000 : 0))

int soc_ddr40_shmoo_restorecfg(int unit, int ci)
{
    int value[4], valarr[4];

    /* SHMOO_RD_DATA_DLY: */
    if (soc_property_ci_get_csv(unit, ci, spn_DDR3_TUNE_RD_DATA_DLY,4,value) != 1) {
        return SOC_E_CONFIG;
    }
    valarr[0] = value[0] & 0x7;
    valarr[1] = (value[0] >> 8) & 0x7;
    WRITE_DDR40_PHY_WORD_LANE_0_READ_DATA_DLYr(unit, ci, valarr[0]);
    WRITE_DDR40_PHY_WORD_LANE_1_READ_DATA_DLYr(unit, ci, valarr[1]);

    /* SHMOO_RD_EN: */
    if (soc_property_ci_get_csv(unit, ci, spn_DDR3_TUNE_RD_EN,4,value) != 2) {
        return SOC_E_CONFIG;
    }
    valarr[0] = OVR_EXPAND(value[0]);
    valarr[1] = OVR_EXPAND(value[0]>>8);
    valarr[2] = OVR_EXPAND(value[0]>>16);
    WRITE_DDR40_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE0_BIT_RD_ENr(unit, ci, valarr[0]);
    WRITE_DDR40_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE1_BIT_RD_ENr(unit, ci, valarr[1]);
    WRITE_DDR40_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE_RD_ENr(unit, ci, valarr[2]);
    valarr[0] = OVR_EXPAND(value[1]);
    valarr[1] = OVR_EXPAND(value[1]>>8);
    valarr[2] = OVR_EXPAND(value[1]>>16);
    WRITE_DDR40_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE0_BIT_RD_ENr(unit, ci, valarr[0]);
    WRITE_DDR40_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE1_BIT_RD_ENr(unit, ci, valarr[1]);
    WRITE_DDR40_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE_RD_ENr(unit, ci, valarr[2]);

    /* SHMOO_RD_DQ : */
    if (soc_property_ci_get_csv(unit, ci, spn_DDR3_TUNE_RD_DQ_WL0_RP,4,value) != 4) {
        return SOC_E_CONFIG;
    }
    valarr[0] = OVR_EXPAND(value[0]);
    valarr[1] = OVR_EXPAND(value[0]>>8);
    valarr[2] = OVR_EXPAND(value[0]>>16);
    valarr[3] = OVR_EXPAND(value[0]>>24);
    WRITE_DDR40_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE0_BIT0_R_Pr(unit, ci, valarr[0]);
    WRITE_DDR40_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE0_BIT1_R_Pr(unit, ci, valarr[1]);
    WRITE_DDR40_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE0_BIT2_R_Pr(unit, ci, valarr[2]);
    WRITE_DDR40_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE0_BIT3_R_Pr(unit, ci, valarr[3]);
    valarr[0] = OVR_EXPAND(value[1]);
    valarr[1] = OVR_EXPAND(value[1]>>8);
    valarr[2] = OVR_EXPAND(value[1]>>16);
    valarr[3] = OVR_EXPAND(value[1]>>24);
    WRITE_DDR40_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE0_BIT4_R_Pr(unit, ci, valarr[0]);
    WRITE_DDR40_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE0_BIT5_R_Pr(unit, ci, valarr[1]);
    WRITE_DDR40_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE0_BIT6_R_Pr(unit, ci, valarr[2]);
    WRITE_DDR40_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE0_BIT7_R_Pr(unit, ci, valarr[3]);
    valarr[0] = OVR_EXPAND(value[2]);
    valarr[1] = OVR_EXPAND(value[2]>>8);
    valarr[2] = OVR_EXPAND(value[2]>>16);
    valarr[3] = OVR_EXPAND(value[2]>>24);
    WRITE_DDR40_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE1_BIT0_R_Pr(unit, ci, valarr[0]);
    WRITE_DDR40_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE1_BIT1_R_Pr(unit, ci, valarr[1]);
    WRITE_DDR40_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE1_BIT2_R_Pr(unit, ci, valarr[2]);
    WRITE_DDR40_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE1_BIT3_R_Pr(unit, ci, valarr[3]);
    valarr[0] = OVR_EXPAND(value[3]);
    valarr[1] = OVR_EXPAND(value[3]>>8);
    valarr[2] = OVR_EXPAND(value[3]>>16);
    valarr[3] = OVR_EXPAND(value[3]>>24);
    WRITE_DDR40_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE1_BIT4_R_Pr(unit, ci, valarr[0]);
    WRITE_DDR40_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE1_BIT5_R_Pr(unit, ci, valarr[1]);
    WRITE_DDR40_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE1_BIT6_R_Pr(unit, ci, valarr[2]);
    WRITE_DDR40_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE1_BIT7_R_Pr(unit, ci, valarr[3]);

    if (soc_property_ci_get_csv(unit, ci, spn_DDR3_TUNE_RD_DQ_WL1_RP,4,value) != 4) {
        return SOC_E_CONFIG;
    }
    valarr[0] = OVR_EXPAND(value[0]);
    valarr[1] = OVR_EXPAND(value[0]>>8);
    valarr[2] = OVR_EXPAND(value[0]>>16);
    valarr[3] = OVR_EXPAND(value[0]>>24);
    WRITE_DDR40_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE0_BIT0_R_Pr(unit, ci, valarr[0]);
    WRITE_DDR40_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE0_BIT1_R_Pr(unit, ci, valarr[1]);
    WRITE_DDR40_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE0_BIT2_R_Pr(unit, ci, valarr[2]);
    WRITE_DDR40_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE0_BIT3_R_Pr(unit, ci, valarr[3]);
    valarr[0] = OVR_EXPAND(value[1]);
    valarr[1] = OVR_EXPAND(value[1]>>8);
    valarr[2] = OVR_EXPAND(value[1]>>16);
    valarr[3] = OVR_EXPAND(value[1]>>24);
    WRITE_DDR40_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE0_BIT4_R_Pr(unit, ci, valarr[0]);
    WRITE_DDR40_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE0_BIT5_R_Pr(unit, ci, valarr[1]);
    WRITE_DDR40_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE0_BIT6_R_Pr(unit, ci, valarr[2]);
    WRITE_DDR40_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE0_BIT7_R_Pr(unit, ci, valarr[3]);
    valarr[0] = OVR_EXPAND(value[2]);
    valarr[1] = OVR_EXPAND(value[2]>>8);
    valarr[2] = OVR_EXPAND(value[2]>>16);
    valarr[3] = OVR_EXPAND(value[2]>>24);
    WRITE_DDR40_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE1_BIT0_R_Pr(unit, ci, valarr[0]);
    WRITE_DDR40_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE1_BIT1_R_Pr(unit, ci, valarr[1]);
    WRITE_DDR40_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE1_BIT2_R_Pr(unit, ci, valarr[2]);
    WRITE_DDR40_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE1_BIT3_R_Pr(unit, ci, valarr[3]);
    valarr[0] = OVR_EXPAND(value[3]);
    valarr[1] = OVR_EXPAND(value[3]>>8);
    valarr[2] = OVR_EXPAND(value[3]>>16);
    valarr[3] = OVR_EXPAND(value[3]>>24);
    WRITE_DDR40_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE1_BIT4_R_Pr(unit, ci, valarr[0]);
    WRITE_DDR40_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE1_BIT5_R_Pr(unit, ci, valarr[1]);
    WRITE_DDR40_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE1_BIT6_R_Pr(unit, ci, valarr[2]);
    WRITE_DDR40_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE1_BIT7_R_Pr(unit, ci, valarr[3]);

    if (soc_property_ci_get_csv(unit, ci, spn_DDR3_TUNE_RD_DQ_WL0_RN,4,value) != 4) {
        return SOC_E_CONFIG;
    }
    valarr[0] = OVR_EXPAND(value[0]);
    valarr[1] = OVR_EXPAND(value[0]>>8);
    valarr[2] = OVR_EXPAND(value[0]>>16);
    valarr[3] = OVR_EXPAND(value[0]>>24);
    WRITE_DDR40_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE0_BIT0_R_Nr(unit, ci, valarr[0]);
    WRITE_DDR40_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE0_BIT1_R_Nr(unit, ci, valarr[1]);
    WRITE_DDR40_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE0_BIT2_R_Nr(unit, ci, valarr[2]);
    WRITE_DDR40_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE0_BIT3_R_Nr(unit, ci, valarr[3]);
    valarr[0] = OVR_EXPAND(value[1]);
    valarr[1] = OVR_EXPAND(value[1]>>8);
    valarr[2] = OVR_EXPAND(value[1]>>16);
    valarr[3] = OVR_EXPAND(value[3]>>24);
    WRITE_DDR40_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE0_BIT4_R_Nr(unit, ci, valarr[0]);
    WRITE_DDR40_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE0_BIT5_R_Nr(unit, ci, valarr[1]);
    WRITE_DDR40_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE0_BIT6_R_Nr(unit, ci, valarr[2]);
    WRITE_DDR40_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE0_BIT7_R_Nr(unit, ci, valarr[3]);
    valarr[0] = OVR_EXPAND(value[2]);
    valarr[1] = OVR_EXPAND(value[2]>>8);
    valarr[2] = OVR_EXPAND(value[2]>>16);
    valarr[3] = OVR_EXPAND(value[2]>>24);
    WRITE_DDR40_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE1_BIT0_R_Nr(unit, ci, valarr[0]);
    WRITE_DDR40_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE1_BIT1_R_Nr(unit, ci, valarr[1]);
    WRITE_DDR40_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE1_BIT2_R_Nr(unit, ci, valarr[2]);
    WRITE_DDR40_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE1_BIT3_R_Nr(unit, ci, valarr[3]);
    valarr[0] = OVR_EXPAND(value[3]);
    valarr[1] = OVR_EXPAND(value[3]>>8);
    valarr[2] = OVR_EXPAND(value[3]>>16);
    valarr[3] = OVR_EXPAND(value[3]>>24);
    WRITE_DDR40_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE1_BIT4_R_Nr(unit, ci, valarr[0]);
    WRITE_DDR40_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE1_BIT5_R_Nr(unit, ci, valarr[1]);
    WRITE_DDR40_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE1_BIT6_R_Nr(unit, ci, valarr[2]);
    WRITE_DDR40_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE1_BIT7_R_Nr(unit, ci, valarr[3]);

    if (soc_property_ci_get_csv(unit, ci, spn_DDR3_TUNE_RD_DQ_WL1_RN,4,value) != 4) {
        return SOC_E_CONFIG;
    }
    valarr[0] = OVR_EXPAND(value[0]);
    valarr[1] = OVR_EXPAND(value[0]>>8);
    valarr[2] = OVR_EXPAND(value[0]>>16);
    valarr[3] = OVR_EXPAND(value[0]>>24);
    WRITE_DDR40_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE0_BIT0_R_Nr(unit, ci, valarr[0]);
    WRITE_DDR40_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE0_BIT1_R_Nr(unit, ci, valarr[1]);
    WRITE_DDR40_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE0_BIT2_R_Nr(unit, ci, valarr[2]);
    WRITE_DDR40_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE0_BIT3_R_Nr(unit, ci, valarr[3]);
    valarr[0] = OVR_EXPAND(value[1]);
    valarr[1] = OVR_EXPAND(value[1]>>8);
    valarr[2] = OVR_EXPAND(value[1]>>16);
    valarr[3] = OVR_EXPAND(value[1]>>24);
    WRITE_DDR40_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE0_BIT4_R_Nr(unit, ci, valarr[0]);
    WRITE_DDR40_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE0_BIT5_R_Nr(unit, ci, valarr[1]);
    WRITE_DDR40_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE0_BIT6_R_Nr(unit, ci, valarr[2]);
    WRITE_DDR40_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE0_BIT7_R_Nr(unit, ci, valarr[3]);
    valarr[0] = OVR_EXPAND(value[2]);
    valarr[1] = OVR_EXPAND(value[2]>>8);
    valarr[2] = OVR_EXPAND(value[2]>>16);
    valarr[3] = OVR_EXPAND(value[2]>>24);
    WRITE_DDR40_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE1_BIT0_R_Nr(unit, ci, valarr[0]);
    WRITE_DDR40_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE1_BIT1_R_Nr(unit, ci, valarr[1]);
    WRITE_DDR40_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE1_BIT2_R_Nr(unit, ci, valarr[2]);
    WRITE_DDR40_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE1_BIT3_R_Nr(unit, ci, valarr[3]);
    valarr[0] = OVR_EXPAND(value[3]);
    valarr[1] = OVR_EXPAND(value[3]>>8);
    valarr[2] = OVR_EXPAND(value[3]>>16);
    valarr[3] = OVR_EXPAND(value[3]>>24);
    WRITE_DDR40_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE1_BIT4_R_Nr(unit, ci, valarr[0]);
    WRITE_DDR40_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE1_BIT5_R_Nr(unit, ci, valarr[1]);
    WRITE_DDR40_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE1_BIT6_R_Nr(unit, ci, valarr[2]);
    WRITE_DDR40_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE1_BIT7_R_Nr(unit, ci, valarr[3]);


    /* SHMOO_RD_DQS: */
    if (soc_property_ci_get_csv(unit, ci, spn_DDR3_TUNE_RD_DQS,4,value) != 2) {
        return SOC_E_CONFIG;
    }
    valarr[0] = OVR_EXPAND(value[0]);
    valarr[1] = OVR_EXPAND(value[0]>>8);
    valarr[2] = OVR_EXPAND(value[0]>>16);
    valarr[3] = OVR_EXPAND(value[0]>>24);
    WRITE_DDR40_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE0_R_Pr(unit, ci, valarr[0]);
    WRITE_DDR40_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE0_R_Nr(unit, ci, valarr[1]);
    WRITE_DDR40_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE1_R_Pr(unit, ci, valarr[2]);
    WRITE_DDR40_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE1_R_Nr(unit, ci, valarr[3]);
    valarr[0] = OVR_EXPAND(value[1]);
    valarr[1] = OVR_EXPAND(value[1]>>8);
    valarr[2] = OVR_EXPAND(value[1]>>16);
    valarr[3] = OVR_EXPAND(value[1]>>24);
    WRITE_DDR40_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE0_R_Pr(unit, ci, valarr[0]);
    WRITE_DDR40_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE0_R_Nr(unit, ci, valarr[1]);
    WRITE_DDR40_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE1_R_Pr(unit, ci, valarr[2]);
    WRITE_DDR40_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE1_R_Nr(unit, ci, valarr[3]);

    /* SHMOO_VREF: */
    if (soc_property_ci_get_csv(unit, ci, spn_DDR3_TUNE_VREF,2,value) != 1) {
        return SOC_E_CONFIG;
    }
    WRITE_DDR40_PHY_CONTROL_REGS_VREF_DAC_CONTROLr(unit, ci, value[0]);

    /* SHMOO_WR_DQ : */
    if (soc_property_ci_get_csv(unit, ci, spn_DDR3_TUNE_WR_DQ,2,value) != 1) {
        return SOC_E_CONFIG;
    }
    valarr[0] = OVR_EXPAND(value[0]);
    valarr[1] = OVR_EXPAND(value[0]>>8);
    valarr[2] = OVR_EXPAND(value[0]>>16);
    valarr[3] = OVR_EXPAND(value[0]>>24);
    WRITE_DDR40_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE0_Wr(unit, ci, valarr[0]);
    WRITE_DDR40_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE1_Wr(unit, ci, valarr[1]);
    WRITE_DDR40_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE0_Wr(unit, ci, valarr[2]);
    WRITE_DDR40_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE1_Wr(unit, ci, valarr[3]);

    if (soc_property_ci_get_csv(unit, ci, spn_DDR3_TUNE_WR_DQ_WL0,4,value) != 4) {
        return SOC_E_CONFIG;
    }
    valarr[0] = OVR_EXPAND(value[0]);
    valarr[1] = OVR_EXPAND(value[0]>>8);
    valarr[2] = OVR_EXPAND(value[0]>>16);
    valarr[3] = OVR_EXPAND(value[0]>>24);
    WRITE_DDR40_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE0_BIT0_Wr(unit, ci, valarr[0]);
    WRITE_DDR40_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE0_BIT1_Wr(unit, ci, valarr[1]);
    WRITE_DDR40_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE0_BIT2_Wr(unit, ci, valarr[2]);
    WRITE_DDR40_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE0_BIT3_Wr(unit, ci, valarr[3]);
    valarr[0] = OVR_EXPAND(value[1]);
    valarr[1] = OVR_EXPAND(value[1]>>8);
    valarr[2] = OVR_EXPAND(value[1]>>16);
    valarr[3] = OVR_EXPAND(value[1]>>24);
    WRITE_DDR40_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE0_BIT4_Wr(unit, ci, valarr[0]);
    WRITE_DDR40_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE0_BIT5_Wr(unit, ci, valarr[1]);
    WRITE_DDR40_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE0_BIT6_Wr(unit, ci, valarr[2]);
    WRITE_DDR40_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE0_BIT7_Wr(unit, ci, valarr[3]);
    valarr[0] = OVR_EXPAND(value[2]);
    valarr[1] = OVR_EXPAND(value[2]>>8);
    valarr[2] = OVR_EXPAND(value[2]>>16);
    valarr[3] = OVR_EXPAND(value[2]>>24);
    WRITE_DDR40_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE1_BIT0_Wr(unit, ci, valarr[0]);
    WRITE_DDR40_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE1_BIT1_Wr(unit, ci, valarr[1]);
    WRITE_DDR40_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE1_BIT2_Wr(unit, ci, valarr[2]);
    WRITE_DDR40_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE1_BIT3_Wr(unit, ci, valarr[3]);
    valarr[0] = OVR_EXPAND(value[3]);
    valarr[1] = OVR_EXPAND(value[3]>>8);
    valarr[2] = OVR_EXPAND(value[3]>>16);
    valarr[3] = OVR_EXPAND(value[3]>>24);
    WRITE_DDR40_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE1_BIT4_Wr(unit, ci, valarr[0]);
    WRITE_DDR40_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE1_BIT5_Wr(unit, ci, valarr[1]);
    WRITE_DDR40_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE1_BIT6_Wr(unit, ci, valarr[2]);
    WRITE_DDR40_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE1_BIT7_Wr(unit, ci, valarr[3]);

    if (soc_property_ci_get_csv(unit, ci, spn_DDR3_TUNE_WR_DQ_WL1,4,value) != 4) {
        return SOC_E_CONFIG;
    }
    valarr[0] = OVR_EXPAND(value[0]);
    valarr[1] = OVR_EXPAND(value[0]>>8);
    valarr[2] = OVR_EXPAND(value[0]>>16);
    valarr[3] = OVR_EXPAND(value[0]>>24);
    WRITE_DDR40_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE0_BIT0_Wr(unit, ci, valarr[0]);
    WRITE_DDR40_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE0_BIT1_Wr(unit, ci, valarr[1]);
    WRITE_DDR40_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE0_BIT2_Wr(unit, ci, valarr[2]);
    WRITE_DDR40_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE0_BIT3_Wr(unit, ci, valarr[3]);
    valarr[0] = OVR_EXPAND(value[1]);
    valarr[1] = OVR_EXPAND(value[1]>>8);
    valarr[2] = OVR_EXPAND(value[1]>>16);
    valarr[3] = OVR_EXPAND(value[1]>>24);
    WRITE_DDR40_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE0_BIT4_Wr(unit, ci, valarr[0]);
    WRITE_DDR40_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE0_BIT5_Wr(unit, ci, valarr[1]);
    WRITE_DDR40_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE0_BIT6_Wr(unit, ci, valarr[2]);
    WRITE_DDR40_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE0_BIT7_Wr(unit, ci, valarr[3]);
    valarr[0] = OVR_EXPAND(value[2]);
    valarr[1] = OVR_EXPAND(value[2]>>8);
    valarr[2] = OVR_EXPAND(value[2]>>16);
    valarr[3] = OVR_EXPAND(value[2]>>24);
    WRITE_DDR40_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE1_BIT0_Wr(unit, ci, valarr[0]);
    WRITE_DDR40_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE1_BIT1_Wr(unit, ci, valarr[1]);
    WRITE_DDR40_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE1_BIT2_Wr(unit, ci, valarr[2]);
    WRITE_DDR40_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE1_BIT3_Wr(unit, ci, valarr[3]);
    valarr[0] = OVR_EXPAND(value[3]);
    valarr[1] = OVR_EXPAND(value[3]>>8);
    valarr[2] = OVR_EXPAND(value[3]>>16);
    valarr[3] = OVR_EXPAND(value[3]>>24);
    WRITE_DDR40_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE1_BIT4_Wr(unit, ci, valarr[0]);
    WRITE_DDR40_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE1_BIT5_Wr(unit, ci, valarr[1]);
    WRITE_DDR40_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE1_BIT6_Wr(unit, ci, valarr[2]);
    WRITE_DDR40_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE1_BIT7_Wr(unit, ci, valarr[3]);

    /* SHMOO_ADDRC: */
    if (soc_property_ci_get_csv(unit, ci, spn_DDR3_TUNE_ADDRC,2,value) != 1) {
        return SOC_E_CONFIG;
    }
    valarr[0] = OVR_EXPAND(value[0]);
    valarr[1] = OVR_EXPAND(value[0]>>8);
    WRITE_DDR40_PHY_CONTROL_REGS_VDL_OVRIDE_BIT_CTLr(unit, ci, valarr[0]);
    WRITE_DDR40_PHY_CONTROL_REGS_VDL_OVRIDE_BYTE_CTLr(unit, ci, valarr[1]);

    /* Clear the FIFO error state */
    WRITE_DDR40_PHY_WORD_LANE_0_READ_FIFO_CLEARr(unit, ci, 0);
    WRITE_DDR40_PHY_WORD_LANE_1_READ_FIFO_CLEARr(unit, ci, 0);

    return SOC_E_NONE;
}
#endif

#if defined(CONFIG_IPROC_DDR_ECC)
static void
_shmoo_print_ecc_diagnostics(int unit, int ci) {

    int x, y;
    uint32 data;

    x = 0x004C;
    if(DDR40_REG_READ(unit, ci, 0x00, x, &data) == SOC_E_NONE) {
            printf("Address = 0x%04X\t\tData = 0x%08X\t\tVDL CALIB STATUS\n", x, data);
        } else {
            printf("Address = 0x%04X\t\tERROR            \t\tVDL CALIB STATUS\n", x);
    }
    x = 0x0058;
    if(DDR40_REG_READ(unit, ci, 0x00, x, &data) == SOC_E_NONE) {
            printf("Address = 0x%04X\t\tData = 0x%08X\t\tVDL RD EN CALIB STATUS\n", x, data);
        } else {
            printf("Address = 0x%04X\t\tERROR            \t\tVDL RD EN CALIB STATUS\n", x);
    }
    x = 0x0050;
    if(DDR40_REG_READ(unit, ci, 0x00, x, &data) == SOC_E_NONE) {
            printf("Address = 0x%04X\t\tData = 0x%08X\t\tVDL DQ/DQS CALIB STATUS\n", x, data);
        } else {
            printf("Address = 0x%04X\t\tERROR            \t\tVDL DQ/DQS CALIB STATUS\n", x);
    }
    x = 0x0054;
    if(DDR40_REG_READ(unit, ci, 0x00, x, &data) == SOC_E_NONE) {
            printf("Address = 0x%04X\t\tData = 0x%08X\t\tVDL WR DQ CALIB STATUS\n", x, data);
        } else {
            printf("Address = 0x%04X\t\tERROR            \t\tVDL WR DQ CALIB STATUS\n", x);
    }
    x = 0x0030;
    if(DDR40_REG_READ(unit, ci, 0x00, x, &data) == SOC_E_NONE) {
            printf("Address = 0x%04X\t\tData = 0x%08X\t\tVDL OVRIDE BYTE CTL\n", x, data);
        } else {
            printf("Address = 0x%04X\t\tERROR            \t\tVDL OVRIDE BYTE CTL\n", x);
    }
    x = 0x0034;
    if(DDR40_REG_READ(unit, ci, 0x00, x, &data) == SOC_E_NONE) {
            printf("Address = 0x%04X\t\tData = 0x%08X\t\tVDL OVRIDE BIT CTL\n", x, data);
        } else {
            printf("Address = 0x%04X\t\tERROR            \t\tVDL OVRIDE BIT CTL\n", x);
    }
    x = 0x003C;
    if(DDR40_REG_READ(unit, ci, 0x00, x, &data) == SOC_E_NONE) {
            printf("Address = 0x%04X\t\tData = 0x%08X\t\tZQ PVT COMP CTL\n", x, data);
        } else {
            printf("Address = 0x%04X\t\tERROR            \t\tZQ PVT COMP CTL\n", x);
    }
    x = 0x006C;
    if(DDR40_REG_READ(unit, ci, 0x00, x, &data) == SOC_E_NONE) {
            printf("Address = 0x%04X\t\tData = 0x%08X\t\tVREF DAC CONTROL\n", x, data);
        } else {
            printf("Address = 0x%04X\t\tERROR            \t\tVREF DAC CONTROL\n", x);
    }
    y = 0x0400;
    x = y + 0x0360;
    if(DDR40_REG_READ(unit, ci, 0x00, x, &data) == SOC_E_NONE) {
            printf("Address = 0x%04X\t\tData = 0x%08X\t\tREAD DATA DLY\n", x, data);
        } else {
            printf("Address = 0x%04X\t\tERROR            \t\tREAD DATA DLY\n", x);
    }
    x = y + 0x0200;
    if(DDR40_REG_READ(unit, ci, 0x00, x, &data) == SOC_E_NONE) {
            printf("Address = 0x%04X\t\tData = 0x%08X\t\tVDL OVRIDE BYTE RD EN\n", x, data);
        } else {
            printf("Address = 0x%04X\t\tERROR            \t\tVDL OVRIDE BYTE RD EN\n", x);
    }
    x = y + 0x0274;
    if(DDR40_REG_READ(unit, ci, 0x00, x, &data) == SOC_E_NONE) {
            printf("Address = 0x%04X\t\tData = 0x%08X\t\tVDL OVRIDE BYTE BIT RD EN\n", x, data);
        } else {
            printf("Address = 0x%04X\t\tERROR            \t\tVDL OVRIDE BYTE BIT RD EN\n", x);
    }
    x = y + 0x0234;
    if(DDR40_REG_READ(unit, ci, 0x00, x, &data) == SOC_E_NONE) {
            printf("Address = 0x%04X\t\tData = 0x%08X\t\tVDL OVRIDE BYTE BIT0 R DQ\n", x, data);
        } else {
            printf("Address = 0x%04X\t\tERROR            \t\tVDL OVRIDE BYTE BIT0 R DQ\n", x);
    }
    x = y + 0x0208;
    if(DDR40_REG_READ(unit, ci, 0x00, x, &data) == SOC_E_NONE) {
            printf("Address = 0x%04X\t\tData = 0x%08X\t\tVDL OVRIDE BYTE R DQS\n", x, data);
        } else {
            printf("Address = 0x%04X\t\tERROR            \t\tVDL OVRIDE BYTE R DQS\n", x);
    }
    x = y + 0x0204;
    if(DDR40_REG_READ(unit, ci, 0x00, x, &data) == SOC_E_NONE) {
            printf("Address = 0x%04X\t\tData = 0x%08X\t\tVDL OVRIDE BYTE WR DQ\n", x, data);
        } else {
            printf("Address = 0x%04X\t\tERROR            \t\tVDL OVRIDE BYTE WR DQ\n", x);
    }
    x = y + 0x0210;
    if(DDR40_REG_READ(unit, ci, 0x00, x, &data) == SOC_E_NONE) {
            printf("Address = 0x%04X\t\tData = 0x%08X\t\tVDL OVRIDE BYTE BIT WR DQ\n", x, data);
        } else {
            printf("Address = 0x%04X\t\tERROR            \t\tVDL OVRIDE BYTE BIT WR DQ\n", x);
    }
}

int
shmoo_ecc_rd_data_dly(void)
{
    int i, j;
    uint32_t data;
    uint32_t result;
    uint32_t unit = 0;
    int ci = 0;
    int wl = 0;

    /* Clear the FIFO error state */
	reg32_write((volatile uint32_t *)DDR_PHY_ECC_LANE_READ_FIFO_CLEAR, 0x0);

    /* Find Best RD_DATA_DLY  */
    for (j = 1; j < 8; ++j) {
        /* For every single RD_DATA_DLY, we shmoo RD_EN to find the largest window */
        data = j;
		reg32_write((volatile uint32_t *)DDR_PHY_ECC_LANE_READ_DATA_DLY, data);
        printf ("RD_DATA_DLY (%d): ", j); 

        for (i = 0; i < 64; ++i) {
            data = SET_OVR_STEP(i);
            /* RD_EN */
            reg32_write((volatile uint32_t *)DDR_PHY_ECC_LANE_VDL_OVRIDE_BYTE_RD_EN, data);
            reg32_write((volatile uint32_t *)DDR_PHY_ECC_LANE_VDL_OVRIDE_BYTE_BIT_RD_EN, data);

            /* Clear the FIFO error state */
            reg32_write((volatile uint32_t *)DDR_PHY_ECC_LANE_READ_FIFO_CLEAR, 0x0);
            iproc_clear_ecc_syndrome();
            __udelay(9);

            _test_func_self_test_1(unit, ci, wl, 1, 0);
            result = iproc_read_ecc_syndrome();

            if (result == 0) {       /* PASS */
                printf ("+");
            } else {
                printf ("-");
            }
        }
        printf ("\n");
    }

    /* Clear the FIFO error state */
	reg32_write((volatile uint32_t *)DDR_PHY_ECC_LANE_READ_FIFO_CLEAR, 0x0);
    iproc_clear_ecc_syndrome();

    return SOC_E_NONE;
}

int
shmoo_ecc_mini(int mode)
{
    uint32_t result;
    uint32_t unit = 0;
    int ci = 0;
    int wl = 0;

    _shmoo_print_ecc_diagnostics(unit, ci);

    /* Clear the FIFO error state */
	reg32_write((volatile uint32_t *)DDR_PHY_ECC_LANE_READ_FIFO_CLEAR, 0x0);
    iproc_clear_ecc_syndrome();
    __udelay(9);

    _test_func_self_test_1(unit, ci, wl, 1, 0);
    if(mode == 0) {
        result = reg32_read((volatile uint32_t *)DDR_PHY_ECC_LANE_READ_FIFO_STATUS) & 0xF;
    }
    else {
        result = iproc_read_ecc_syndrome();
    }

    /* Clear the FIFO error state */
	reg32_write((volatile uint32_t *)DDR_PHY_ECC_LANE_READ_FIFO_CLEAR, 0x0);
    iproc_clear_ecc_syndrome();

    return result;
}

int
shmoo_ecc_full(int mode)
{
    int i;  // RD_EN
    int j;  // RD_DATA_DLY
    int x;  // RD_DQS
    int y;  // RD_DQ
    int z;  // WR_DQ
    uint32_t data;
    uint32_t result;
    uint32_t unit = 0;
    int ci = 0;
    int wl = 0;

    /* Clear the FIFO error state */
	reg32_write((volatile uint32_t *)DDR_PHY_ECC_LANE_READ_FIFO_CLEAR, 0x0);

    
    
    for (x = 0; x < 64; x+=8) {                 // RD_DQS

        data = SET_OVR_STEP(x);
        reg32_write((volatile uint32_t *)DDR_PHY_ECC_LANE_VDL_OVRIDE_BYTE_R_P, data);
        reg32_write((volatile uint32_t *)DDR_PHY_ECC_LANE_VDL_OVRIDE_BYTE_R_N, data);

        for (y = 0; y < 64; y+=8) {             // RD_DQ

            data = SET_OVR_STEP(y);
            reg32_write((volatile uint32_t *)DDR_PHY_ECC_LANE_VDL_OVRIDE_BYTE_BIT0_R_P, data);
            reg32_write((volatile uint32_t *)DDR_PHY_ECC_LANE_VDL_OVRIDE_BYTE_BIT0_R_N, data);
            reg32_write((volatile uint32_t *)DDR_PHY_ECC_LANE_VDL_OVRIDE_BYTE_BIT1_R_P, data);
            reg32_write((volatile uint32_t *)DDR_PHY_ECC_LANE_VDL_OVRIDE_BYTE_BIT1_R_N, data);
            reg32_write((volatile uint32_t *)DDR_PHY_ECC_LANE_VDL_OVRIDE_BYTE_BIT2_R_P, data);
            reg32_write((volatile uint32_t *)DDR_PHY_ECC_LANE_VDL_OVRIDE_BYTE_BIT2_R_N, data);
            reg32_write((volatile uint32_t *)DDR_PHY_ECC_LANE_VDL_OVRIDE_BYTE_BIT3_R_P, data);
            reg32_write((volatile uint32_t *)DDR_PHY_ECC_LANE_VDL_OVRIDE_BYTE_BIT3_R_N, data);

            for (z = 0; z < 64; z+=8) {         // WR_DQ

                data = SET_OVR_STEP(z);
                reg32_write((volatile uint32_t *)DDR_PHY_ECC_LANE_VDL_OVRIDE_BYTE_BIT0_W, data);
                reg32_write((volatile uint32_t *)DDR_PHY_ECC_LANE_VDL_OVRIDE_BYTE_BIT1_W, data);
                reg32_write((volatile uint32_t *)DDR_PHY_ECC_LANE_VDL_OVRIDE_BYTE_BIT2_W, data);
                reg32_write((volatile uint32_t *)DDR_PHY_ECC_LANE_VDL_OVRIDE_BYTE_BIT3_W, data);

                printf ("\t\tRD_DQS: %2d\tRD_DQ: %2d\tWR_DQ: %2d\n", x, y, z);

                for (j = 1; j < 8; ++j) {       // RD_DATA_DLY
                    data = j;
                    reg32_write((volatile uint32_t *)DDR_PHY_ECC_LANE_READ_DATA_DLY, data);

                    printf ("RD_DATA_DLY (%d): ", j); 
                    for (i = 0; i < 64; ++i) {  // RD_EN
                        data = SET_OVR_STEP(i);

                        reg32_write((volatile uint32_t *)DDR_PHY_ECC_LANE_VDL_OVRIDE_BYTE_RD_EN, data);
                        reg32_write((volatile uint32_t *)DDR_PHY_ECC_LANE_VDL_OVRIDE_BYTE_BIT_RD_EN, data);

                        /* Clear the FIFO error state */
                        reg32_write((volatile uint32_t *)DDR_PHY_ECC_LANE_READ_FIFO_CLEAR, 0x0);
                        iproc_clear_ecc_syndrome();
                        __udelay(9);

                        _test_func_self_test_1(unit, ci, wl, 1, 0);
                        if(mode == 0) {
                            result = reg32_read((volatile uint32_t *)DDR_PHY_ECC_LANE_READ_FIFO_STATUS) & 0xF;
                        }
                        else {
                            result = iproc_read_ecc_syndrome();
                        }

                        if (result == 0) {
                            printf ("+");
                        } else {
                            printf ("-");
                        }
                    }
                    printf ("\n");
                }
            }
        }
    }

    /* Clear the FIFO error state */
	reg32_write((volatile uint32_t *)DDR_PHY_ECC_LANE_READ_FIFO_CLEAR, 0x0);
    iproc_clear_ecc_syndrome();

    return SOC_E_NONE;
}

#endif
#endif                          /* DDR3 Support */
