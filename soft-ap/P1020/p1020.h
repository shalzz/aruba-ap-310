#ifndef P1020H
#define P1020H
/* Reg. Defn. below are from P1020 Ref Manual */
#define L2C_DR		0xE40
#define L2C_IER		0xE48 /* pp. 225 */
#define ECM_DR		0xE00
#define ECM_IER		0xE08 /* pp. 256 */
#define DDR_DR		0xE40
#define DDR_IER		0xE48 /* pp. 337 */
#define ELBC_DR		0xB0
#define ELBC_IER	0xB8 /* pp. 569 */
#define PEX_TOR		0xC  /* pp. 727 */
#define PEX_DR		0xE00
#define PEX_IER		0xE08 /* pp. 756 */
#define PEX_MDR		0x20 /* pp. 730 */
#define PEX_MIER	0x28
#define PEX_PCTIE	0x00800000

#define PEX_ERR_CAP_STAT	0xE20
#define PEX_ERR_CAP_R0		0xE28
#define PEX_ERR_CAP_R1		0xE2C
#define PEX_ERR_CAP_R2		0xE30
#define PEX_ERR_CAP_R3		0xE34

struct pex_err_status {
    int 	error;
    u_int32_t	dr;  /* Reg 0xE00 (PEX_ERR_DR) in PEX space pp.756 of P1020 RM */
    u_int32_t	mdr; /* Reg 0x20 (PEX_PME_MES_DR) in PEX space pp. 730 */
    u_int32_t	err_cap_stat; /* Reg 0xE20 */
    u_int32_t	err_cap[4]; /* Reg 0xE28,0xE2C,0xE30,0xE34 */
};
#endif
