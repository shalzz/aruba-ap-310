/*
 * (C) Copyright 2004,2005
 * Cavium Networks
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

#ifndef __OCTEON_CSR_H__
#define __OCTEON_CSR_H__


/****************************   CSR address defines */
#define  CVMX_FPA_BIST_STATUS                                 CVMX_ADD_IO_SEG(0x00011800280000E8ull)
#define  CVMX_GMXX_BIST(block_id)                            (CVMX_ADD_IO_SEG(0x0001180008000400ull+((block_id)*0x8000000ull)))
#define  CVMX_IOB_BIST_STATUS                                 CVMX_ADD_IO_SEG(0x00011800F00007F8ull)
#define  CVMX_IPD_BIST_STATUS                                 CVMX_ADD_IO_SEG(0x00014F00000007F8ull)
#define  CVMX_KEY_BIST_REG                                    CVMX_ADD_IO_SEG(0x0001180020000018ull)
#define  CVMX_MIO_BOOT_BIST_STAT                              CVMX_ADD_IO_SEG(0x00011800000000F8ull)
#define  CVMX_CIU_BIST                                        CVMX_ADD_IO_SEG(0x0001070000000730ull)
#define  CVMX_CIU_SOFT_BIST                                   CVMX_ADD_IO_SEG(0x0001070000000738ull)
#define  CVMX_NPI_BIST_STATUS                                 CVMX_ADD_IO_SEG(0x00011F00000003F8ull)
#define  CVMX_PIP_BIST_STATUS                                 CVMX_ADD_IO_SEG(0x00011800A0000000ull)
#define  CVMX_PKO_REG_BIST_RESULT                             CVMX_ADD_IO_SEG(0x0001180050000080ull)
#define  CVMX_POW_BIST_STAT                                   CVMX_ADD_IO_SEG(0x00016700000003F8ull)
#define  CVMX_RNM_BIST_STATUS                                 CVMX_ADD_IO_SEG(0x0001180040000008ull)
#define  CVMX_SPXX_BIST_STAT(block_id)                       (CVMX_ADD_IO_SEG(0x00011800900007F8ull+((block_id)*0x8000000ull)))
#define  CVMX_TIM_REG_BIST_RESULT                             CVMX_ADD_IO_SEG(0x0001180058000080ull)
#define  CVMX_TRA_BIST_STATUS                                 CVMX_ADD_IO_SEG(0x00011800A8000010ull)
#define  CVMX_ZIP_CMD_BIST_RESULT                             CVMX_ADD_IO_SEG(0x0001180038000080ull)
#define  CVMX_DFA_BST0                                        CVMX_ADD_IO_SEG(0x00011800300007F0ull)
#define  CVMX_DFA_BST1                                        CVMX_ADD_IO_SEG(0x00011800300007F8ull)
#define  CVMX_L2D_BST0                                        CVMX_ADD_IO_SEG(0x0001180080000780ull)
#define  CVMX_L2D_BST1                                        CVMX_ADD_IO_SEG(0x0001180080000788ull)
#define  CVMX_L2D_BST2                                        CVMX_ADD_IO_SEG(0x0001180080000790ull)
#define  CVMX_L2D_BST3                                        CVMX_ADD_IO_SEG(0x0001180080000798ull)
#define  CVMX_L2C_BST2                                        CVMX_ADD_IO_SEG(0x00011800800007E8ull)
#define  CVMX_L2C_BST1                                        CVMX_ADD_IO_SEG(0x00011800800007F0ull)
#define  CVMX_L2C_BST0                                        CVMX_ADD_IO_SEG(0x00011800800007F8ull)
#define  CVMX_MIO_BOOT_REG_CFG0                               CVMX_ADD_IO_SEG(0x0001180000000000ull)
#define  CVMX_MIO_BOOT_REG_CFGX(offset)                      (CVMX_ADD_IO_SEG(0x0001180000000008ull+((offset)*8)-8*1))
#define  CVMX_MIO_BOOT_REG_TIM0                               CVMX_ADD_IO_SEG(0x0001180000000040ull)
#define  CVMX_MIO_BOOT_REG_TIMX(offset)                      (CVMX_ADD_IO_SEG(0x0001180000000048ull+((offset)*8)-8*1))
#define  CVMX_MIO_BOOT_LOC_CFGX(offset)                      (CVMX_ADD_IO_SEG(0x0001180000000080ull+((offset)*8)))
#define  CVMX_MIO_BOOT_LOC_ADR                                CVMX_ADD_IO_SEG(0x0001180000000090ull)
#define  CVMX_MIO_BOOT_LOC_DAT                                CVMX_ADD_IO_SEG(0x0001180000000098ull)
#define  CVMX_MIO_TWS_SW_TWSI                                 CVMX_ADD_IO_SEG(0x0001180000001000ull)
#define  CVMX_MIO_TWS_TWSI_SW                                 CVMX_ADD_IO_SEG(0x0001180000001008ull)
#define  CVMX_PKO_REG_GMX_PORT_MODE                           CVMX_ADD_IO_SEG(0x0001180050000018ull)
#define  CVMX_GMXX_TX_PRTS(block_id)                         (CVMX_ADD_IO_SEG(0x0001180008000480ull+((block_id)*0x8000000ull)))
#define  CVMX_SPXX_DBG_DESKEW_CTL(block_id)                  (CVMX_ADD_IO_SEG(0x0001180090000368ull+((block_id)*0x8000000ull)))
#define  CVMX_SRXX_COM_CTL(block_id)                         (CVMX_ADD_IO_SEG(0x0001180090000200ull+((block_id)*0x8000000ull)))
#define  CVMX_SRXX_SPI4_CALX(offset,block_id)                (CVMX_ADD_IO_SEG(0x0001180090000000ull+((offset)*8)+((block_id)*0x8000000ull)))
#define  CVMX_SRXX_SPI4_STAT(block_id)                       (CVMX_ADD_IO_SEG(0x0001180090000208ull+((block_id)*0x8000000ull)))
#define  CVMX_STXX_ARB_CTL(block_id)                         (CVMX_ADD_IO_SEG(0x0001180090000608ull+((block_id)*0x8000000ull)))
#define  CVMX_GMXX_TX_SPI_MAX(block_id)                      (CVMX_ADD_IO_SEG(0x00011800080004B0ull+((block_id)*0x8000000ull)))
#define  CVMX_GMXX_TX_SPI_THRESH(block_id)                   (CVMX_ADD_IO_SEG(0x00011800080004B8ull+((block_id)*0x8000000ull)))
#define  CVMX_GMXX_TX_SPI_CTL(block_id)                      (CVMX_ADD_IO_SEG(0x00011800080004C0ull+((block_id)*0x8000000ull)))
#define  CVMX_STXX_SPI4_DAT(block_id)                        (CVMX_ADD_IO_SEG(0x0001180090000628ull+((block_id)*0x8000000ull)))
#define  CVMX_SPXX_BIST_STAT(block_id)                       (CVMX_ADD_IO_SEG(0x00011800900007F8ull+((block_id)*0x8000000ull)))
#define  CVMX_STXX_SPI4_CALX(offset,block_id)                (CVMX_ADD_IO_SEG(0x0001180090000400ull+((offset)*8)+((block_id)*0x8000000ull)))
#define  CVMX_STXX_SPI4_STAT(block_id)                       (CVMX_ADD_IO_SEG(0x0001180090000630ull+((block_id)*0x8000000ull)))
#define  CVMX_STXX_COM_CTL(block_id)                         (CVMX_ADD_IO_SEG(0x0001180090000600ull+((block_id)*0x8000000ull)))
#define  CVMX_PKO_REG_CMD_BUF                                 CVMX_ADD_IO_SEG(0x0001180050000010ull)
#define  CVMX_PKO_REG_READ_IDX                                CVMX_ADD_IO_SEG(0x0001180050000008ull)
#define  CVMX_PKO_MEM_DEBUG9                                  CVMX_ADD_IO_SEG(0x0001180050001148ull)
#define  CVMX_SPXX_CLK_STAT(block_id)                        (CVMX_ADD_IO_SEG(0x0001180090000350ull+((block_id)*0x8000000ull)))
#define  CVMX_GMXX_PRTX_CFG(offset,block_id)                 (CVMX_ADD_IO_SEG(0x0001180008000010ull+((offset)*2048)+((block_id)*0x8000000ull)))
#define  CVMX_GMXX_TXX_CLK(offset,block_id)                  (CVMX_ADD_IO_SEG(0x0001180008000208ull+((offset)*2048)+((block_id)*0x8000000ull)))
#define  CVMX_GMXX_TXX_SLOT(offset,block_id)                 (CVMX_ADD_IO_SEG(0x0001180008000220ull+((offset)*2048)+((block_id)*0x8000000ull)))
#define  CVMX_GMXX_TXX_BURST(offset,block_id)                (CVMX_ADD_IO_SEG(0x0001180008000228ull+((offset)*2048)+((block_id)*0x8000000ull)))
#define  CVMX_L2C_CFG                                         CVMX_ADD_IO_SEG(0x0001180080000000ull)
#define  CVMX_L2T_ERR                                         CVMX_ADD_IO_SEG(0x0001180080000008ull)
#define  CVMX_L2D_ERR                                         CVMX_ADD_IO_SEG(0x0001180080000010ull)




#define  CVMX_DFA_BST0                                        CVMX_ADD_IO_SEG(0x00011800300007F0ull)
#define  CVMX_DFA_BST1                                        CVMX_ADD_IO_SEG(0x00011800300007F8ull)
#define  CVMX_DFA_CFG                                         CVMX_ADD_IO_SEG(0x0001180030000000ull)
#define  CVMX_DFA_DBELL                                       CVMX_ADD_IO_SEG(0x0001370000000000ull)
#define  CVMX_DFA_DDR2_ADDR                                   CVMX_ADD_IO_SEG(0x0001180030000210ull)
#define  CVMX_DFA_DDR2_BUS                                    CVMX_ADD_IO_SEG(0x0001180030000080ull)
#define  CVMX_DFA_DDR2_CFG                                    CVMX_ADD_IO_SEG(0x0001180030000208ull)
#define  CVMX_DFA_DDR2_COMP                                   CVMX_ADD_IO_SEG(0x0001180030000090ull)
#define  CVMX_DFA_DDR2_EMRS                                   CVMX_ADD_IO_SEG(0x0001180030000268ull)
#define  CVMX_DFA_DDR2_FCNT                                   CVMX_ADD_IO_SEG(0x0001180030000078ull)
#define  CVMX_DFA_DDR2_MRS                                    CVMX_ADD_IO_SEG(0x0001180030000260ull)
#define  CVMX_DFA_DDR2_OPT                                    CVMX_ADD_IO_SEG(0x0001180030000070ull)
#define  CVMX_DFA_DDR2_PLL                                    CVMX_ADD_IO_SEG(0x0001180030000088ull)
#define  CVMX_DFA_DDR2_TMG                                    CVMX_ADD_IO_SEG(0x0001180030000218ull)
#define  CVMX_DFA_DIFCTL                                      CVMX_ADD_IO_SEG(0x0001370600000000ull)
#define  CVMX_DFA_DIFRDPTR                                    CVMX_ADD_IO_SEG(0x0001370200000000ull)
#define  CVMX_DFA_ECLKCFG                                     CVMX_ADD_IO_SEG(0x0001180030000200ull)
#define  CVMX_DFA_ERR                                         CVMX_ADD_IO_SEG(0x0001180030000028ull)
#define  CVMX_DFA_MEMCFG0                                     CVMX_ADD_IO_SEG(0x0001180030000008ull)
#define  CVMX_DFA_MEMCFG1                                     CVMX_ADD_IO_SEG(0x0001180030000010ull)
#define  CVMX_DFA_MEMCFG2                                     CVMX_ADD_IO_SEG(0x0001180030000060ull)
#define  CVMX_DFA_MEMFADR                                     CVMX_ADD_IO_SEG(0x0001180030000030ull)
#define  CVMX_DFA_MEMFCR                                      CVMX_ADD_IO_SEG(0x0001180030000038ull)
#define  CVMX_DFA_MEMRLD                                      CVMX_ADD_IO_SEG(0x0001180030000018ull)
#define  CVMX_DFA_NCBCTL                                      CVMX_ADD_IO_SEG(0x0001180030000020ull)


#define  CVMX_LMC_COMP_CTL                                    CVMX_ADD_IO_SEG(0x0001180088000028ull)
#define  CVMX_LMC_CTL                                         CVMX_ADD_IO_SEG(0x0001180088000010ull)
#define  CVMX_LMC_CTL1                                        CVMX_ADD_IO_SEG(0x0001180088000090ull)
#define  CVMX_LMC_DCLK_CNT_HI                                 CVMX_ADD_IO_SEG(0x0001180088000070ull)
#define  CVMX_LMC_DCLK_CNT_LO                                 CVMX_ADD_IO_SEG(0x0001180088000068ull)
#define  CVMX_LMC_DDR2_CTL                                    CVMX_ADD_IO_SEG(0x0001180088000018ull)
#define  CVMX_LMC_DELAY_CFG                                   CVMX_ADD_IO_SEG(0x0001180088000088ull)
#define  CVMX_LMC_ECC_SYND                                    CVMX_ADD_IO_SEG(0x0001180088000038ull)
#define  CVMX_LMC_FADR                                        CVMX_ADD_IO_SEG(0x0001180088000020ull)
#define  CVMX_LMC_IFB_CNT_HI                                  CVMX_ADD_IO_SEG(0x0001180088000050ull)
#define  CVMX_LMC_IFB_CNT_LO                                  CVMX_ADD_IO_SEG(0x0001180088000048ull)
#define  CVMX_LMC_MEM_CFG0                                    CVMX_ADD_IO_SEG(0x0001180088000000ull)
#define  CVMX_LMC_MEM_CFG1                                    CVMX_ADD_IO_SEG(0x0001180088000008ull)
#define  CVMX_LMC_OPS_CNT_HI                                  CVMX_ADD_IO_SEG(0x0001180088000060ull)
#define  CVMX_LMC_OPS_CNT_LO                                  CVMX_ADD_IO_SEG(0x0001180088000058ull)
#define  CVMX_LMC_PLL_BWCTL                                   CVMX_ADD_IO_SEG(0x0001180088000040ull)
#define  CVMX_LMC_RODT_CTL                                    CVMX_ADD_IO_SEG(0x0001180088000078ull)
#define  CVMX_LMC_WODT_CTL                                    CVMX_ADD_IO_SEG(0x0001180088000030ull)
#define  CVMX_LMC_WODT_CTL0                                   CVMX_ADD_IO_SEG(0x0001180088000030ull)
#define  CVMX_LMC_WODT_CTL1                                   CVMX_ADD_IO_SEG(0x0001180088000080ull)


#define  CVMX_L2D_FUS3                                        CVMX_ADD_IO_SEG(0x00011800800007B8ull)


#define  CVMX_GMXX_RXX_INT_REG(offset,block_id)              (CVMX_ADD_IO_SEG(0x0001180008000000ull+((offset)*2048)+((block_id)*0x8000000ull)))
#define  CVMX_GMXX_RXX_RX_INBND(offset,block_id)             (CVMX_ADD_IO_SEG(0x0001180008000060ull+((offset)*2048)+((block_id)*0x8000000ull)))
#define  CVMX_PKO_REG_FLAGS                                   CVMX_ADD_IO_SEG(0x0001180050000000ull)
#define  CVMX_SPXX_CLK_CTL(block_id)                         (CVMX_ADD_IO_SEG(0x0001180090000348ull+((block_id)*0x8000000ull)))
#define  CVMX_SPXX_DRV_CTL(block_id)                         (CVMX_ADD_IO_SEG(0x0001180090000358ull+((block_id)*0x8000000ull)))
#define  CVMX_SPXX_TRN4_CTL(block_id)                        (CVMX_ADD_IO_SEG(0x0001180090000360ull+((block_id)*0x8000000ull)))
#define  CVMX_GMXX_TX_OVR_BP(block_id)                       (CVMX_ADD_IO_SEG(0x00011800080004C8ull+((block_id)*0x8000000ull)))
#define  CVMX_ASXX_RX_PRT_EN(block_id)                       (CVMX_ADD_IO_SEG(0x00011800B0000000ull+((block_id)*0x8000000ull)))
#define  CVMX_ASXX_TX_PRT_EN(block_id)                       (CVMX_ADD_IO_SEG(0x00011800B0000008ull+((block_id)*0x8000000ull)))
#define  CVMX_ASXX_RX_CLK_SETX(offset,block_id)              (CVMX_ADD_IO_SEG(0x00011800B0000020ull+((offset)*8)+((block_id)*0x8000000ull)))
#define  CVMX_ASXX_TX_CLK_SETX(offset,block_id)              (CVMX_ADD_IO_SEG(0x00011800B0000048ull+((offset)*8)+((block_id)*0x8000000ull)))
#define  CVMX_GMXX_TXX_THRESH(offset,block_id)               (CVMX_ADD_IO_SEG(0x0001180008000210ull+((offset)*2048)+((block_id)*0x8000000ull)))
#define  CVMX_ASXX_TX_HI_WATERX(offset,block_id)             (CVMX_ADD_IO_SEG(0x00011800B0000080ull+((offset)*8)+((block_id)*0x8000000ull)))
#define  CVMX_PKO_MEM_QUEUE_PTRS                              CVMX_ADD_IO_SEG(0x0001180050001000ull)
#define  CVMX_IPD_CTL_STATUS                                  CVMX_ADD_IO_SEG(0x00014F0000000018ull)
#define  CVMX_FPA_CTL_STATUS                                  CVMX_ADD_IO_SEG(0x0001180028000050ull)
#define  CVMX_FPA_FPF1_MARKS                                  CVMX_ADD_IO_SEG(0x0001180028000008ull)
#define  CVMX_PIP_PRT_CFGX(offset)                           (CVMX_ADD_IO_SEG(0x00011800A0000200ull+((offset)*8)))
#define  CVMX_PIP_PRT_TAGX(offset)                           (CVMX_ADD_IO_SEG(0x00011800A0000400ull+((offset)*8)))
#define  CVMX_IPD_1ST_MBUFF_SKIP                              CVMX_ADD_IO_SEG(0x00014F0000000000ull)
#define  CVMX_IPD_NOT_1ST_MBUFF_SKIP                          CVMX_ADD_IO_SEG(0x00014F0000000008ull)
#define  CVMX_IPD_WQE_FPA_QUEUE                               CVMX_ADD_IO_SEG(0x00014F0000000020ull)
#define  CVMX_IPD_PACKET_MBUFF_SIZE                           CVMX_ADD_IO_SEG(0x00014F0000000010ull)
#define  CVMX_IPD_1st_NEXT_PTR_BACK                           CVMX_ADD_IO_SEG(0x00014F0000000150ull)
#define  CVMX_IPD_2nd_NEXT_PTR_BACK                           CVMX_ADD_IO_SEG(0x00014F0000000158ull)
#define  CVMX_IPD_PORTX_BP_PAGE_CNT(offset)                  (CVMX_ADD_IO_SEG(0x00014F0000000028ull+((offset)*8)))
#define  CVMX_IPD_PORT_BP_COUNTERS_PAIRX(offset)             (CVMX_ADD_IO_SEG(0x00014F00000001B8ull+((offset)*8)))
#define  CVMX_IPD_SUB_PORT_BP_PAGE_CNT                        CVMX_ADD_IO_SEG(0x00014F0000000148ull)
#define  CVMX_IPD_INT_SUM                                     CVMX_ADD_IO_SEG(0x00014F0000000168ull)
#define  CVMX_NPI_DBG_SELECT                                  CVMX_ADD_IO_SEG(0x00011F0000000008ull)
#define  CVMX_DBG_DATA                                        CVMX_ADD_IO_SEG(0x00011F00000001E8ull)
#define  CVMX_PKO_MEM_COUNT0                                  CVMX_ADD_IO_SEG(0x0001180050001080ull)
#define  CVMX_PKO_MEM_COUNT1                                  CVMX_ADD_IO_SEG(0x0001180050001088ull)
#define  CVMX_GPIO_RX_DAT                                     CVMX_ADD_IO_SEG(0x0001070000000880ull)
#define  CVMX_PKO_REG_FLAGS                                   CVMX_ADD_IO_SEG(0x0001180050000000ull)
#define  CVMX_NPI_CTL_STATUS                                  CVMX_ADD_IO_SEG(0x00011F0000000010ull)
#define  CVMX_NPI_PCI_INT_ARB_CFG                             CVMX_ADD_IO_SEG(0x00011F0000000130ull)
#define  CVMX_NPI_PCI_CFG00                                   CVMX_ADD_IO_SEG(0x00011F0000001800ull)
#define  CVMX_NPI_PCI_CFG01                                   CVMX_ADD_IO_SEG(0x00011F0000001804ull)
#define  CVMX_NPI_PCI_CFG02                                   CVMX_ADD_IO_SEG(0x00011F0000001808ull)
#define  CVMX_NPI_PCI_CFG03                                   CVMX_ADD_IO_SEG(0x00011F000000180Cull)
#define  CVMX_NPI_PCI_CFG04                                   CVMX_ADD_IO_SEG(0x00011F0000001810ull)
#define  CVMX_NPI_PCI_CFG05                                   CVMX_ADD_IO_SEG(0x00011F0000001814ull)
#define  CVMX_NPI_PCI_CFG06                                   CVMX_ADD_IO_SEG(0x00011F0000001818ull)
#define  CVMX_NPI_PCI_CFG07                                   CVMX_ADD_IO_SEG(0x00011F000000181Cull)
#define  CVMX_NPI_PCI_CFG08                                   CVMX_ADD_IO_SEG(0x00011F0000001820ull)
#define  CVMX_NPI_PCI_CFG09                                   CVMX_ADD_IO_SEG(0x00011F0000001824ull)
#define  CVMX_NPI_PCI_CFG10                                   CVMX_ADD_IO_SEG(0x00011F0000001828ull)
#define  CVMX_NPI_PCI_CFG11                                   CVMX_ADD_IO_SEG(0x00011F000000182Cull)
#define  CVMX_NPI_PCI_CFG12                                   CVMX_ADD_IO_SEG(0x00011F0000001830ull)
#define  CVMX_NPI_PCI_CFG13                                   CVMX_ADD_IO_SEG(0x00011F0000001834ull)
#define  CVMX_NPI_PCI_CFG15                                   CVMX_ADD_IO_SEG(0x00011F000000183Cull)
#define  CVMX_NPI_PCI_CFG16                                   CVMX_ADD_IO_SEG(0x00011F0000001840ull)
#define  CVMX_NPI_PCI_CFG17                                   CVMX_ADD_IO_SEG(0x00011F0000001844ull)
#define  CVMX_NPI_PCI_CFG18                                   CVMX_ADD_IO_SEG(0x00011F0000001848ull)
#define  CVMX_NPI_PCI_CFG19                                   CVMX_ADD_IO_SEG(0x00011F000000184Cull)
#define  CVMX_NPI_PCI_CFG20                                   CVMX_ADD_IO_SEG(0x00011F0000001850ull)
#define  CVMX_NPI_PCI_CFG21                                   CVMX_ADD_IO_SEG(0x00011F0000001854ull)
#define  CVMX_NPI_PCI_CFG22                                   CVMX_ADD_IO_SEG(0x00011F0000001858ull)
#define  CVMX_NPI_PCI_CFG56                                   CVMX_ADD_IO_SEG(0x00011F00000018E0ull)
#define  CVMX_NPI_PCI_CFG57                                   CVMX_ADD_IO_SEG(0x00011F00000018E4ull)
#define  CVMX_NPI_PCI_CFG58                                   CVMX_ADD_IO_SEG(0x00011F00000018E8ull)
#define  CVMX_NPI_PCI_CFG59                                   CVMX_ADD_IO_SEG(0x00011F00000018ECull)
#define  CVMX_NPI_PCI_CFG60                                   CVMX_ADD_IO_SEG(0x00011F00000018F0ull)
#define  CVMX_NPI_PCI_CFG61                                   CVMX_ADD_IO_SEG(0x00011F00000018F4ull)
#define  CVMX_NPI_PCI_CFG62                                   CVMX_ADD_IO_SEG(0x00011F00000018F8ull)
#define  CVMX_NPI_PCI_CFG63                                   CVMX_ADD_IO_SEG(0x00011F00000018FCull)
#define  CVMX_NPI_PCI_CTL_STATUS_2                            CVMX_ADD_IO_SEG(0x00011F000000118Cull)
#define  CVMX_NPI_PCI_READ_CMD                                CVMX_ADD_IO_SEG(0x00011F0000000048ull)
#define  CVMX_NPI_PCI_READ_CMD_6                              CVMX_ADD_IO_SEG(0x00011F0000001180ull)
#define  CVMX_NPI_PCI_READ_CMD_C                              CVMX_ADD_IO_SEG(0x00011F0000001184ull)
#define  CVMX_NPI_PCI_READ_CMD_E                              CVMX_ADD_IO_SEG(0x00011F0000001188ull)
#define  CVMX_NPI_PCI_INT_SUM2                                CVMX_ADD_IO_SEG(0x00011F0000001198ull)
#define  CVMX_NPI_PCI_INT_ENB2                                CVMX_ADD_IO_SEG(0x00011F00000011A0ull)
#define  CVMX_NPI_PCI_SCM_REG                                 CVMX_ADD_IO_SEG(0x00011F00000011A8ull)
#define  CVMX_NPI_PCI_TSR_REG                                 CVMX_ADD_IO_SEG(0x00011F00000011B0ull)
#define  CVMX_PCI_READ_CMD_6                                                 (0x0000000000000180ull)
#define  CVMX_PCI_READ_CMD_C                                                 (0x0000000000000184ull)
#define  CVMX_PCI_READ_CMD_E                                                 (0x0000000000000188ull)
#define  CVMX_CIU_SOFT_PRST                                   CVMX_ADD_IO_SEG(0x0001070000000748ull)
#define  CVMX_NPI_PCI_BAR1_INDEXX(offset)                    (CVMX_ADD_IO_SEG(0x00011F0000001100ull+((offset)*4)))
#define  CVMX_IPD_PTR_COUNT                                   CVMX_ADD_IO_SEG(0x00014F0000000320ull)
#define  CVMX_ASXX_PRT_LOOP(block_id)                        (CVMX_ADD_IO_SEG(0x00011800B0000040ull+((block_id)*0x8000000ull)))
#define  CVMX_L2C_SPAR0                                       CVMX_ADD_IO_SEG(0x0001180080000068ull)
#define  CVMX_L2C_SPAR1                                       CVMX_ADD_IO_SEG(0x0001180080000070ull)
#define  CVMX_L2C_SPAR2                                       CVMX_ADD_IO_SEG(0x0001180080000078ull)
#define  CVMX_L2C_SPAR3                                       CVMX_ADD_IO_SEG(0x0001180080000080ull)
#define  CVMX_L2C_SPAR4                                       CVMX_ADD_IO_SEG(0x0001180080000088ull)
#define  CVMX_GMXX_INF_MODE(block_id)                        (CVMX_ADD_IO_SEG(0x00011800080007F8ull+((block_id)*0x8000000ull)))
#define  CVMX_SMI_CMD                                         CVMX_ADD_IO_SEG(0x0001180000001800ull)
#define  CVMX_SMI_WR_DAT                                      CVMX_ADD_IO_SEG(0x0001180000001808ull)
#define  CVMX_SMI_RD_DAT                                      CVMX_ADD_IO_SEG(0x0001180000001810ull)
#define  CVMX_SMI_CLK                                         CVMX_ADD_IO_SEG(0x0001180000001818ull)
#define  CVMX_SMI_EN                                          CVMX_ADD_IO_SEG(0x0001180000001820ull)
#define  CVMX_GMXX_RXX_FRM_CTL(offset,block_id)              (CVMX_ADD_IO_SEG(0x0001180008000018ull+((offset)*2048)+((block_id)*0x8000000ull)))
#define  CVMX_LMC_DCLK_CNT_LO                                 CVMX_ADD_IO_SEG(0x0001180088000068ull)
#define  CVMX_LMC_DCLK_CNT_HI                                 CVMX_ADD_IO_SEG(0x0001180088000070ull)
#define  CVMX_MIO_FUS_RCMD                                    CVMX_ADD_IO_SEG(0x0001180000001500ull)
#define  CVMX_CIU_FUSE                                        CVMX_ADD_IO_SEG(0x0001070000000728ull)
#define  CVMX_CIU_PP_RST                                      CVMX_ADD_IO_SEG(0x0001070000000700ull)
#define  CVMX_L2C_DBG                                         CVMX_ADD_IO_SEG(0x0001180080000030ull)
#define  CVMX_IPD_CLK_COUNT                                   CVMX_ADD_IO_SEG(0x00014F0000000338ull)
#define  CVMX_NPI_MEM_ACCESS_SUBID3                           CVMX_ADD_IO_SEG(0x00011F0000000028ull)
#define  CVMX_NPI_MEM_ACCESS_SUBID4                           CVMX_ADD_IO_SEG(0x00011F0000000030ull)
#define  CVMX_NPI_MEM_ACCESS_SUBID5                           CVMX_ADD_IO_SEG(0x00011F0000000038ull)
#define  CVMX_NPI_MEM_ACCESS_SUBID6                           CVMX_ADD_IO_SEG(0x00011F0000000040ull)
#define  CVMX_PIP_SFT_RST                                     CVMX_ADD_IO_SEG(0x00011800A0000030ull)

#define  CVMX_LMC_RODT_COMP_CTL                               CVMX_ADD_IO_SEG(0x0001180088000090ull)
#define CVMX_LMC_PLL_CTL                                        CVMX_ADD_IO_SEG(0x00011800880000A8ull)
#define  CVMX_PKO_MEM_DEBUG8                                  CVMX_ADD_IO_SEG(0x0001180050001140ull)
#define CVMX_LMC_PLL_STATUS                                     CVMX_ADD_IO_SEG(0x00011800880000B0ull)


/* Determine GPIO register addresses at runtime so that bootloader image can work on both
** pass 1 and pass 2 chips */
#define  CVMX_GPIO_BIT_CFGX(offset)                          (CVMX_ADD_IO_SEG(0x0001070000000800ull+((offset)*8)))
#define  CVMX_GPIO_XBIT_CFGX(offset)                         (CVMX_ADD_IO_SEG(0x0001070000000900ull+((offset)*8)-8*16))
static inline uint64_t cvmx_gpio_set(void)
{
    if (octeon_get_proc_id() == 0x000d0000)
        return((0x8001070000000808ull));
    else
        return((0x8001070000000888ull));
}
#define CVMX_GPIO_TX_SET    cvmx_gpio_set()
static inline uint64_t cvmx_gpio_clr(void)
{
    if (octeon_get_proc_id() == 0x000d0000)
        return((0x8001070000000810ull));
    else
        return((0x8001070000000890ull));
}
#define CVMX_GPIO_TX_CLR    cvmx_gpio_clr()


static inline void *cvmx_phys_to_ptr(uint64_t physical_address)
{
    return (void*)(unsigned long)physical_address;
}
static inline uint64_t cvmx_ptr_to_phys(void *ptr)
{
    return CAST64(ptr);
}

#ifndef __CVMX_SCRATCH_H__
#define CVMX_SCRATCH_BASE       ((CVMX_HW_BASE+CVMX_REG_OFFSET))
static inline uint64_t cvmx_scratch_read64(uint64_t address)
{
    return(octeon_read64(0xffffffffffff8000ull + address));
}
#endif

static inline uint64_t cvmx_build_io_address(uint64_t major_did, uint64_t sub_did)
{
    return ((0x1ull << 48) | (major_did << 43) | (sub_did << 40));
}
static inline uint64_t cvmx_build_mask(uint64_t bits)
{
    return ~((~0x0ull) << bits);
}
static inline uint64_t cvmx_build_bits(uint64_t high_bit, uint64_t low_bit, uint64_t value)
{
    return ((value & cvmx_build_mask(high_bit - low_bit + 1)) << low_bit);
}


/**
 * NPI_PCI_INT_ARB_CFG = Configuration For PCI Arbiter
 *  Controls operation of the Internal PCI Arbiter.  This register should
 *  only be written when PRST# is asserted.  NPI_PCI_INT_ARB_CFG[EN] should
 *  only be set when Octane is a host.
 *
 * - Access using the CVMX_NPI_PCI_INT_ARB_CFG CSR
 */
typedef union
{
    uint64_t u64;
    struct
    {
#if __BYTE_ORDER == __BIG_ENDIAN
        uint64_t reserved                : 59;      /**< Reserved */
        uint64_t en                      : 1;       /**< Internal arbiter enable. */
        uint64_t park_mod                : 1;       /**< Bus park mode. 0=park on last, 1=park on device. */
        uint64_t park_dev                : 3;       /**< Bus park device. 0-3 External device, 4 = Octane. */
#else
        uint64_t park_dev                : 3;
        uint64_t park_mod                : 1;
        uint64_t en                      : 1;
        uint64_t reserved                : 59;
#endif
    } s;
} cvmx_npi_pci_int_arb_cfg_t;


typedef enum
{
    CVMX_PKO_SUCCESS,
    CVMX_PKO_INVALID_PORT,
    CVMX_PKO_INVALID_QUEUE,
    CVMX_PKO_INVALID_PRIORITY,
    CVMX_PKO_NO_MEMORY
} cvmx_pko_status_t;

/**
 * Tag type definitions
 */
typedef enum
{
    CVMX_POW_TAG_TYPE_ORDERED   = 0L,   /**< Tag ordering is maintained */
    CVMX_POW_TAG_TYPE_ATOMIC    = 1L,   /**< Tag ordering is maintained, and at most one PP has the tag */
    CVMX_POW_TAG_TYPE_NULL      = 2L,   /**< The work queue entry from the order
                                            - NEVER tag switch from NULL to NULL */
    CVMX_POW_TAG_TYPE_NULL_NULL = 3L    /**< A tag switch to NULL, and there is no space reserved in POW
                                            - NEVER tag switch to NULL_NULL
                                            - NEVER tag switch from NULL_NULL
                                            - NULL_NULL is entered at the beginning of time and on a deschedule.
                                            - NULL_NULL can be exited by a new work request. A NULL_SWITCH load can also switch the state to NULL */
} cvmx_pow_tag_type_t;

typedef enum {
   CVMX_MIPS_SPACE_XKSEG = 3LL,
   CVMX_MIPS_SPACE_XKPHYS = 2LL,
   CVMX_MIPS_SPACE_XSSEG = 1LL,
   CVMX_MIPS_SPACE_XUSEG = 0LL
} cvmx_mips_space_t;

typedef enum {
   CVMX_MIPS_XKSEG_SPACE_KSEG0 = 0LL,
   CVMX_MIPS_XKSEG_SPACE_KSEG1 = 1LL,
   CVMX_MIPS_XKSEG_SPACE_SSEG = 2LL,
   CVMX_MIPS_XKSEG_SPACE_KSEG3 = 3LL
} cvmx_mips_xkseg_space_t;

// decodes <14:13> of a kseg3 window address
typedef enum {
   CVMX_ADD_WIN_SCR = 0L,
   CVMX_ADD_WIN_DMA = 1L,   // see cvmx_add_win_dma_dec_t for further decode
   CVMX_ADD_WIN_UNUSED = 2L,
   CVMX_ADD_WIN_UNUSED2 = 3L
} cvmx_add_win_dec_t;

// decode within DMA space
typedef enum {
   CVMX_ADD_WIN_DMA_ADD = 0L,     // add store data to the write buffer entry, allocating it if necessary
   CVMX_ADD_WIN_DMA_SENDMEM = 1L, // send out the write buffer entry to DRAM
                                     // store data must be normal DRAM memory space address in this case
   CVMX_ADD_WIN_DMA_SENDDMA = 2L, // send out the write buffer entry as an IOBDMA command
                                     // see CVMX_ADD_WIN_DMA_SEND_DEC for data contents
   CVMX_ADD_WIN_DMA_SENDIO = 3L,  // send out the write buffer entry as an IO write
                                     // store data must be normal IO space address in this case
   CVMX_ADD_WIN_DMA_SENDSINGLE = 4L, // send out a single-tick command on the NCB bus
                                        // no write buffer data needed/used
} cvmx_add_win_dma_dec_t;


typedef enum {
   CVMX_IPD_OPC_MODE_STT = 0LL,   /* All blocks DRAM, not cached in L2 */
   CVMX_IPD_OPC_MODE_STF = 1LL,   /* All bloccks into  L2 */
   CVMX_IPD_OPC_MODE_STF1_STT = 2LL,   /* 1st block L2, rest DRAM */
   CVMX_IPD_OPC_MODE_STF2_STT = 3LL    /* 1st, 2nd blocks L2, rest DRAM */
} cvmx_ipd_mode_t;


typedef enum
{
    CVMX_PIP_PORT_CFG_MODE_NONE = 0ull,  /**< Packet input doesn't perform any
                                            processing of the input packet. */
    CVMX_PIP_PORT_CFG_MODE_SKIPL2 = 1ull,/**< Full packet processing is performed
                                            with pointer starting at the L2
                                            (ethernet MAC) header. */
    CVMX_PIP_PORT_CFG_MODE_SKIPIP = 2ull /**< Input packets are assumed to be IP.
                                            Results from non IP packets is
                                            undefined. Pointers reference the
                                            beginning of the IP header. */
} cvmx_pip_port_parse_mode_t;



typedef enum {
   CVMX_FAU_OP_SIZE_8  = 0,
   CVMX_FAU_OP_SIZE_16 = 1,
   CVMX_FAU_OP_SIZE_32 = 2,
   CVMX_FAU_OP_SIZE_64 = 3
} cvmx_fau_op_size_t;


typedef union
{
    uint64_t u64;
    struct
    {
#if __BYTE_ORDER == __BIG_ENDIAN
        uint64_t reserved                : 53;      /**< Reserved */
        uint64_t stxcal                  : 1;       /**< The transistion from Sync to Calendar on status
                                                         channel */
        uint64_t spare1                  : 1;       /**< Spare bit */
        uint64_t srxtrn                  : 1;       /**< Saw a good data training sequence */
        uint64_t s4clk1                  : 1;       /**< Saw '1' on Spi4 transmit status forward clk input */
        uint64_t s4clk0                  : 1;       /**< Saw '0' on Spi4 transmit status forward clk input */
        uint64_t d4clk1                  : 1;       /**< Saw '1' on Spi4 receive data forward clk input */
        uint64_t d4clk0                  : 1;       /**< Saw '0' on Spi4 receive data forward clk input */
        uint64_t spare4                  : 4;       /**< Spare */
#else
        uint64_t spare4                  : 4;
        uint64_t d4clk0                  : 1;
        uint64_t d4clk1                  : 1;
        uint64_t s4clk0                  : 1;
        uint64_t s4clk1                  : 1;
        uint64_t srxtrn                  : 1;
        uint64_t spare1                  : 1;
        uint64_t stxcal                  : 1;
        uint64_t reserved                : 53;
#endif
    } s;
} cvmx_spxx_clk_stat_t;
typedef union
{
    uint64_t u64;
    struct
    {
#if __BYTE_ORDER == __BIG_ENDIAN
        uint64_t reserved                : 48;      /**< MBZ */
        uint64_t inc                     : 8;       /**< Increment to add to current index for next index */
        uint64_t index                   : 8;       /**< Index to use for next memory CSR read */
#else
        uint64_t index                   : 8;
        uint64_t inc                     : 8;
        uint64_t reserved                : 48;
#endif
    } s;
} cvmx_pko_reg_read_idx_t;
typedef union
{
    uint64_t u64;
    struct
    {
#if __BYTE_ORDER == __BIG_ENDIAN
        uint64_t reserved                : 60;      /**< Reserved */
        uint64_t slottime                : 1;       /**< Slot Time for Half-Duplex operation
                                                         - 0 = 512 bitimes (10/100Mbs operation)
                                                         - 1 = 4096 bitimes (1000Mbs operation) */
        uint64_t duplex                  : 1;       /**< Duplex
                                                         - 0 = Half Duplex (collisions/extentions/bursts)
                                                         - 1 = Full Duplex */
        uint64_t speed                   : 1;       /**< Link Speed
                                                         - 0 = 10/100Mbs operation
                                                         - 1 = 1000Mbs operation */
        uint64_t en                      : 1;       /**< Link Enable */
#else
        uint64_t en                      : 1;
        uint64_t speed                   : 1;
        uint64_t duplex                  : 1;
        uint64_t slottime                : 1;
        uint64_t reserved                : 60;
#endif
    } s;
} cvmx_gmxx_prtx_cfg_t;


typedef union
{
    uint64_t u64;
    struct
    {
#if __BYTE_ORDER == __BIG_ENDIAN
        uint64_t reserved4               : 37;      /**< Must be zero */
        uint64_t qos                     : 3;       /**< Default POW QoS queue. Can be overridden be overridden by options below */
        uint64_t qos_wat                 : 4;       /**< Bitfield enabling the QoS watcher look up tables. Four per port. */
        uint64_t reserved3               : 1;       /**< Must be zero */
        uint64_t spare                   : 1;       /**< Must be zero */
        uint64_t qos_diff                : 1;       /**< Set to use the IP diffserv field to determine the queue in the POW */
        uint64_t qos_vlan                : 1;       /**< Set to use the VLAN tag to determine the queue in the POW */
        uint64_t reserved2               : 3;       /**< Must be zero */
        uint64_t crc_en                  : 1;       /**< Set to enable hardware CRC32 checking */
        uint64_t reserved1               : 2;       /**< Must be zero */
        cvmx_pip_port_parse_mode_t mode  : 2;       /**< Controls the amount of packet processing performed by the input hardware */
        uint64_t reserved0               : 1;       /**< Must be zero */
        uint64_t skip                    : 7;       /**< Number of 8 byte works to skip in the beginning of a packet buffer */
#else
        uint64_t skip                    : 7;
        uint64_t reserved0               : 1;
        cvmx_pip_port_parse_mode_t mode  : 2;
        uint64_t reserved1               : 2;
        uint64_t crc_en                  : 1;
        uint64_t reserved2               : 3;
        uint64_t qos_vlan                : 1;
        uint64_t qos_diff                : 1;
        uint64_t spare                   : 1;
        uint64_t reserved3               : 1;
        uint64_t qos_wat                 : 4;
        uint64_t qos                     : 3;
        uint64_t reserved4               : 37;
#endif
    } s;
} cvmx_pip_port_cfg_t;


typedef union
{
    uint64_t u64;
    struct
    {
#if __BYTE_ORDER == __BIG_ENDIAN
        uint64_t reserved0               : 39;      /**< Must be zero */
        uint64_t inc_prt_flag            : 1;       /**< Sets whether the hardware input port is included in hash */
        uint64_t ip6_dprt_flag           : 1;       /**< Sets whether ipv6 includes destination port for tcp and udp in hash */
        uint64_t ip4_dprt_flag           : 1;       /**< Sets whether ipv4 includes destination port for tcp and udp in hash */
        uint64_t ip6_sprt_flag           : 1;       /**< Sets whether ipv6 includes source port for tcp and udp in hash */
        uint64_t ip4_sprt_flag           : 1;       /**< Sets whether ipv4 includes source port for tcp and udp in hash */
        uint64_t ip6_nxth_flag           : 1;       /**< Sets whether ipv6 includes next header in hash */
        uint64_t ip4_pctl_flag           : 1;       /**< Sets whether ipv4 includes protocol in hash */
        uint64_t ip6_dst_flag            : 1;       /**< Sets whether ipv6 includes destination address in tag hash */
        uint64_t ip4_dst_flag            : 1;       /**< Sets whether ipv4 includes destination address in tag hash */
        uint64_t ip6_src_flag            : 1;       /**< Sets whether ipv6 includes source address in tag hash */
        uint64_t ip4_src_flag            : 1;       /**< Sets whether ipv4 includes source address in tag hash */
        cvmx_pow_tag_type_t tcp6_tag_type : 2;      /**< Tag type to use if the packet is IPv6 TCP */
        cvmx_pow_tag_type_t tcp4_tag_type : 2;      /**< Tag type to use if the packet is IPv4 TCP */
        cvmx_pow_tag_type_t ip6_tag_type : 2;       /**< Tag type to use if the packet is IPv6 but not TCP */
        cvmx_pow_tag_type_t ip4_tag_type : 2;       /**< Tag type to use if the packet is IPv4 but not TCP */
        cvmx_pow_tag_type_t non_tag_type : 2;       /**< Tag type to use if the packet is not IPv6 or IPv4 */
        uint64_t grp                     : 4;       /**< POW group to use for packets from this port */
#else
        uint64_t grp                     : 4;
        cvmx_pow_tag_type_t non_tag_type : 2;
        cvmx_pow_tag_type_t ip4_tag_type : 2;
        cvmx_pow_tag_type_t ip6_tag_type : 2;
        cvmx_pow_tag_type_t tcp4_tag_type : 2;
        cvmx_pow_tag_type_t tcp6_tag_type : 2;
        uint64_t ip4_src_flag            : 1;
        uint64_t ip6_src_flag            : 1;
        uint64_t ip4_dst_flag            : 1;
        uint64_t ip6_dst_flag            : 1;
        uint64_t ip4_pctl_flag           : 1;
        uint64_t ip6_nxth_flag           : 1;
        uint64_t ip4_sprt_flag           : 1;
        uint64_t ip6_sprt_flag           : 1;
        uint64_t ip4_dprt_flag           : 1;
        uint64_t ip6_dprt_flag           : 1;
        uint64_t inc_prt_flag            : 1;
        uint64_t reserved0               : 39;
#endif
    } s;
} cvmx_pip_port_tag_cfg_t;


typedef union
{
    uint64_t u64;
    struct
    {
#if __BYTE_ORDER == __BIG_ENDIAN
        uint64_t reserved                : 41;      /**< Must be zero */
        uint64_t pool                    : 3;       /**< Hardware pool to use */
        uint64_t reserved2               : 7;       /**< Must be zero */
        uint64_t size                    : 13;      /**< Size of the pool blocks */
#else
        uint64_t size                    : 13;
        uint64_t reserved2               : 7;
        uint64_t pool                    : 3;
        uint64_t reserved                : 41;
#endif
    } s;
} cvmx_pko_pool_cfg_t;
typedef union
{
    uint64_t u64;
    struct cvmx_pko_mem_debug9_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN
        uint64_t reserved_49_63          : 15;
        uint64_t ptrs0                   : 17;      /**< Internal state */
        uint64_t reserved_0_31           : 32;
#else
        uint64_t reserved_0_31           : 32;
        uint64_t ptrs0                   : 17;
        uint64_t reserved_49_63          : 15;
#endif
    } s;
    struct cvmx_pko_mem_debug9_cn3020
    {
#if __BYTE_ORDER == __BIG_ENDIAN
        uint64_t reserved_28_63          : 36;      /**< MBZ */
        uint64_t doorbell                : 20;      /**< Doorbell count */
        uint64_t reserved_5_7            : 3;
        uint64_t s_tail                  : 1;       /**< Static tail */
        uint64_t static_q                : 1;       /**< Static priority */
        uint64_t qos                     : 3;       /**< QOS mask to enable the queue when set */
#else
        uint64_t qos                     : 3;
        uint64_t static_q                : 1;
        uint64_t s_tail                  : 1;
        uint64_t reserved_5_7            : 3;
        uint64_t doorbell                : 20;
        uint64_t reserved_28_63          : 36;
#endif
    } cn3020;
    struct cvmx_pko_mem_debug9_cn3020    cn30xx;
    struct cvmx_pko_mem_debug9_cn3020    cn31xx;
    struct cvmx_pko_mem_debug9_cn36xx
    {
#if __BYTE_ORDER == __BIG_ENDIAN
        uint64_t reserved_28_63          : 36;      /**< MBZ */
        uint64_t doorbell                : 20;      /**< Doorbell count */
        uint64_t reserved_6_7            : 2;
        uint64_t static_p                : 1;       /**< Static priority (port) */
        uint64_t s_tail                  : 1;       /**< Static tail */
        uint64_t static_q                : 1;       /**< Static priority */
        uint64_t qos                     : 3;       /**< QOS mask to enable the queue when set */
#else
        uint64_t qos                     : 3;
        uint64_t static_q                : 1;
        uint64_t s_tail                  : 1;
        uint64_t static_p                : 1;
        uint64_t reserved_6_7            : 2;
        uint64_t doorbell                : 20;
        uint64_t reserved_28_63          : 36;
#endif
    } cn36xx;
    struct cvmx_pko_mem_debug9_cn36xx    cn38xx;
    struct cvmx_pko_mem_debug9_cn36xx    cn38xxp2;
    struct cvmx_pko_mem_debug9_cn50xx
    {
#if __BYTE_ORDER == __BIG_ENDIAN
        uint64_t reserved_49_63          : 15;      /**< MBZ */
        uint64_t ptrs0                   : 17;      /**< Internal state */
        uint64_t reserved_17_31          : 15;      /**< MBS */
        uint64_t ptrs3                   : 17;      /**< Internal state */
#else
        uint64_t ptrs3                   : 17;
        uint64_t reserved_17_31          : 15;
        uint64_t ptrs0                   : 17;
        uint64_t reserved_49_63          : 15;
#endif
    } cn50xx;
    struct cvmx_pko_mem_debug9_cn50xx    cn56xx;
    struct cvmx_pko_mem_debug9_cn50xx    cn58xx;
} cvmx_pko_mem_debug9_t;

typedef union
{
    uint64_t u64;
    struct
    {
#if __BYTE_ORDER == __BIG_ENDIAN
        uint64_t reserved                : 60;      /**< Reserved */
        uint64_t duplex                  : 1;       /**< RGMII Inbound LinkDuplex
                                                         - 0=half-duplex
                                                         - 1=full-duplex */
        uint64_t speed                   : 2;       /**< RGMII Inbound LinkSpeed
                                                         - 00=2.5MHz
                                                         - 01=25MHz
                                                         - 10=125MHz
                                                         - 11=Reserved */
        uint64_t status                  : 1;       /**< RGMII Inbound LinkStatus
                                                         - 0=down
                                                         - 1=up */
#else
        uint64_t status                  : 1;
        uint64_t speed                   : 2;
        uint64_t duplex                  : 1;
        uint64_t reserved                : 60;
#endif
    } s;
} cvmx_gmxx_rxx_rx_inbnd_t;

/**
 * cvmx_gmx#_rx#_int_reg
 *
 * GMX_RX_INT_REG = Interrupt Register
 *
 *
 * Notes:
 * (1) exceptions will only be raised to the control processor if the
 *     corresponding bit in the GMX_RX_INT_EN register is set.
 * 
 * (2) exception conditions 10:0 can also set the rcv/opcode in the received
 *     packet's workQ entry.  The GMX_RX_FRM_CHK register provides a bit mask
 *     for configuring which conditions set the error.
 * 
 * (3) in half duplex operation, the expectation is that collisions will appear
 *     as either MINERR o r CAREXT errors.
 * 
 * (4) JABBER - An RX Jabber error indicates that a packet was received which
 *              is longer than the maximum allowed packet as defined by the
 *              system.  GMX will truncate the packet at the JABBER count.
 *              Failure to do so could lead to system instabilty.
 * 
 * (5) NIBERR - This error is illegal at 1000Mbs speeds
 *              (GMX_RX_PRT_CFG[SPEED]==0) and will never assert.
 * 
 * (6) MAXERR - for untagged frames, the total frame DA+SA+TL+DATA+PAD+FCS >
 *              GMX_RX_FRM_MAX.  For tagged frames, DA+SA+VLAN+TL+DATA+PAD+FCS
 *              > GMX_RX_FRM_MAX + 4*VLAN_VAL + 4*VLAN_STACKED.
 * 
 * (7) MINERR - total frame DA+SA+TL+DATA+PAD+FCS < GMX_RX_FRM_MIN.
 * 
 * (8) ALNERR - Indicates that the packet received was not an integer number of
 *              bytes.  If FCS checking is enabled, ALNERR will only assert if
 *              the FCS is bad.  If FCS checking is disabled, ALNERR will
 *              assert in all non-integer frame cases.
 * 
 * (9) Collisions - Collisions can only occur in half-duplex mode.  A collision
 *                  is assumed by the receiver when one of the following
 *                  conditions occur...
 * 
 *                  . frame < GMX_RX_FRM_MIN - MINERR
 *                  . carrier extend error   - CAREXT
 *                  . transfer ended before slottime (only 1000Mbs - in cases where SLOTTIME > GMX_RX_FRM_MIN) - COLDET
 * 
 * (A) LENERR - Length errors occur when the received packet does not match the
 *              length field.  LENERR is only checked for packets between 64
 *              and 1500 bytes.  For untagged frames, the length must exact
 *              match.  For tagged frames the length or length+4 must match.
 * 
 * (B) PCTERR - checks that the frame transtions from PREAMBLE=>SFD=>DATA.
 *              Does not check the number of PREAMBLE cycles.
 * 
 * (C) In spi4 mode, all spi4 ports use prt0 for interrupt logging.
 */
typedef union
{
    uint64_t u64;
    struct cvmx_gmxx_rxx_int_reg_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN
        uint64_t reserved_20_63          : 44;
        uint64_t pause_drp               : 1;       /**< Pause packet was dropped due to full GMX RX FIFO */
        uint64_t phy_dupx                : 1;       /**< Change in the RMGII inbound LinkDuplex */
        uint64_t phy_spd                 : 1;       /**< Change in the RMGII inbound LinkSpeed */
        uint64_t phy_link                : 1;       /**< Change in the RMGII inbound LinkStatus */
        uint64_t ifgerr                  : 1;       /**< Interframe Gap Violation
                                                         Does not necessarily indicate a failure */
        uint64_t coldet                  : 1;       /**< Collision Detection */
        uint64_t falerr                  : 1;       /**< False carrier error */
        uint64_t rsverr                  : 1;       /**< RGMII reserved opcodes */
        uint64_t pcterr                  : 1;       /**< Bad Preamble / Protocol */
        uint64_t ovrerr                  : 1;       /**< RX FIFO Overflow */
        uint64_t niberr                  : 1;       /**< Nibble error (hi_nibble != lo_nibble) */
        uint64_t skperr                  : 1;       /**< Skipper error */
        uint64_t rcverr                  : 1;       /**< Frame was received with RMGII Data reception error */
        uint64_t lenerr                  : 1;       /**< Frame was received with length error */
        uint64_t alnerr                  : 1;       /**< Frame was received with an alignment error */
        uint64_t fcserr                  : 1;       /**< Frame was received with FCS/CRC error */
        uint64_t jabber                  : 1;       /**< Frame was received with length > sys_length */
        uint64_t maxerr                  : 1;       /**< Frame was received with length > max_length */
        uint64_t carext                  : 1;       /**< RGMII carrier extend error */
        uint64_t minerr                  : 1;       /**< Frame was received with length < min_length */
#else
        uint64_t minerr                  : 1;
        uint64_t carext                  : 1;
        uint64_t maxerr                  : 1;
        uint64_t jabber                  : 1;
        uint64_t fcserr                  : 1;
        uint64_t alnerr                  : 1;
        uint64_t lenerr                  : 1;
        uint64_t rcverr                  : 1;
        uint64_t skperr                  : 1;
        uint64_t niberr                  : 1;
        uint64_t ovrerr                  : 1;
        uint64_t pcterr                  : 1;
        uint64_t rsverr                  : 1;
        uint64_t falerr                  : 1;
        uint64_t coldet                  : 1;
        uint64_t ifgerr                  : 1;
        uint64_t phy_link                : 1;
        uint64_t phy_spd                 : 1;
        uint64_t phy_dupx                : 1;
        uint64_t pause_drp               : 1;
        uint64_t reserved_20_63          : 44;
#endif
    } s;
    struct cvmx_gmxx_rxx_int_reg_cn3020
    {
#if __BYTE_ORDER == __BIG_ENDIAN
        uint64_t reserved_19_63          : 45;
        uint64_t phy_dupx                : 1;       /**< Change in the RMGII inbound LinkDuplex */
        uint64_t phy_spd                 : 1;       /**< Change in the RMGII inbound LinkSpeed */
        uint64_t phy_link                : 1;       /**< Change in the RMGII inbound LinkStatus */
        uint64_t ifgerr                  : 1;       /**< Interframe Gap Violation
                                                         Does not necessarily indicate a failure */
        uint64_t coldet                  : 1;       /**< Collision Detection */
        uint64_t falerr                  : 1;       /**< False carrier error */
        uint64_t rsverr                  : 1;       /**< RGMII reserved opcodes */
        uint64_t pcterr                  : 1;       /**< Bad Preamble / Protocol */
        uint64_t ovrerr                  : 1;       /**< RX FIFO Overflow */
        uint64_t niberr                  : 1;       /**< Nibble error (hi_nibble != lo_nibble) */
        uint64_t skperr                  : 1;       /**< Skipper error */
        uint64_t rcverr                  : 1;       /**< Frame was received with RMGII Data reception error */
        uint64_t lenerr                  : 1;       /**< Frame was received with length error */
        uint64_t alnerr                  : 1;       /**< Frame was received with an alignment error */
        uint64_t fcserr                  : 1;       /**< Frame was received with FCS/CRC error */
        uint64_t jabber                  : 1;       /**< Frame was received with length > sys_length */
        uint64_t maxerr                  : 1;       /**< Frame was received with length > max_length */
        uint64_t carext                  : 1;       /**< RGMII carrier extend error */
        uint64_t minerr                  : 1;       /**< Frame was received with length < min_length */
#else
        uint64_t minerr                  : 1;
        uint64_t carext                  : 1;
        uint64_t maxerr                  : 1;
        uint64_t jabber                  : 1;
        uint64_t fcserr                  : 1;
        uint64_t alnerr                  : 1;
        uint64_t lenerr                  : 1;
        uint64_t rcverr                  : 1;
        uint64_t skperr                  : 1;
        uint64_t niberr                  : 1;
        uint64_t ovrerr                  : 1;
        uint64_t pcterr                  : 1;
        uint64_t rsverr                  : 1;
        uint64_t falerr                  : 1;
        uint64_t coldet                  : 1;
        uint64_t ifgerr                  : 1;
        uint64_t phy_link                : 1;
        uint64_t phy_spd                 : 1;
        uint64_t phy_dupx                : 1;
        uint64_t reserved_19_63          : 45;
#endif
    } cn3020;
    struct cvmx_gmxx_rxx_int_reg_cn3020  cn30xx;
    struct cvmx_gmxx_rxx_int_reg_cn3020  cn31xx;
    struct cvmx_gmxx_rxx_int_reg_cn3020  cn36xx;
    struct cvmx_gmxx_rxx_int_reg_cn3020  cn38xx;
    struct cvmx_gmxx_rxx_int_reg_cn3020  cn38xxp2;
    struct cvmx_gmxx_rxx_int_reg_cn50xx
    {
#if __BYTE_ORDER == __BIG_ENDIAN
        uint64_t reserved_20_63          : 44;
        uint64_t pause_drp               : 1;       /**< Pause packet was dropped due to full GMX RX FIFO */
        uint64_t phy_dupx                : 1;       /**< Change in the RMGII inbound LinkDuplex */
        uint64_t phy_spd                 : 1;       /**< Change in the RMGII inbound LinkSpeed */
        uint64_t phy_link                : 1;       /**< Change in the RMGII inbound LinkStatus */
        uint64_t ifgerr                  : 1;       /**< Interframe Gap Violation
                                                         Does not necessarily indicate a failure */
        uint64_t coldet                  : 1;       /**< Collision Detection */
        uint64_t falerr                  : 1;       /**< False carrier error or extend error after slottime */
        uint64_t rsverr                  : 1;       /**< RGMII reserved opcodes */
        uint64_t pcterr                  : 1;       /**< Bad Preamble / Protocol */
        uint64_t ovrerr                  : 1;       /**< RX FIFO Overflow */
        uint64_t niberr                  : 1;       /**< Nibble error (hi_nibble != lo_nibble) */
        uint64_t skperr                  : 1;       /**< Skipper error */
        uint64_t rcverr                  : 1;       /**< Frame was received with RMGII Data reception error */
        uint64_t reserved_6_6            : 1;
        uint64_t alnerr                  : 1;       /**< Frame was received with an alignment error */
        uint64_t fcserr                  : 1;       /**< Frame was received with FCS/CRC error */
        uint64_t jabber                  : 1;       /**< Frame was received with length > sys_length */
        uint64_t reserved_2_2            : 1;
        uint64_t carext                  : 1;       /**< RGMII carrier extend error */
        uint64_t reserved_0_0            : 1;
#else
        uint64_t reserved_0_0            : 1;
        uint64_t carext                  : 1;
        uint64_t reserved_2_2            : 1;
        uint64_t jabber                  : 1;
        uint64_t fcserr                  : 1;
        uint64_t alnerr                  : 1;
        uint64_t reserved_6_6            : 1;
        uint64_t rcverr                  : 1;
        uint64_t skperr                  : 1;
        uint64_t niberr                  : 1;
        uint64_t ovrerr                  : 1;
        uint64_t pcterr                  : 1;
        uint64_t rsverr                  : 1;
        uint64_t falerr                  : 1;
        uint64_t coldet                  : 1;
        uint64_t ifgerr                  : 1;
        uint64_t phy_link                : 1;
        uint64_t phy_spd                 : 1;
        uint64_t phy_dupx                : 1;
        uint64_t pause_drp               : 1;
        uint64_t reserved_20_63          : 44;
#endif
    } cn50xx;
    struct cvmx_gmxx_rxx_int_reg_s       cn56xx;
    struct cvmx_gmxx_rxx_int_reg_s       cn58xx;
} cvmx_gmxx_rxx_int_reg_t;

typedef union
{
    uint64_t                u64;
    struct
    {
        cvmx_fau_op_size_t  size1       : 2; /**< The size of the reg1 operation - could be 8, 16, 32, or 64 bits */
        cvmx_fau_op_size_t  size0       : 2; /**< The size of the reg0 operation - could be 8, 16, 32, or 64 bits */
        uint64_t            subone1     : 1; /**< If set, subtract 1, if clear, subtract packet size */
        uint64_t            reg1        :11; /**< The register, subtract will be done if reg1 is non-zero */
        uint64_t            subone0     : 1; /**< If set, subtract 1, if clear, subtract packet size */
        uint64_t            reg0        :11; /**< The register, subtract will be done if reg0 is non-zero */
        uint64_t            unused      : 2; /**< Must be zero */
        uint64_t            wqp         : 1; /**< If set and rsp is set, word3 contains a pointer to a work queue entry */
        uint64_t            rsp         : 1; /**< If set, the hardware will send a response when done */
        uint64_t            gather      : 1; /**< If set, the supplied pkt_ptr is really a pointer to a list of pkt_ptr's */
        uint64_t            ipoffp1     : 7; /**< If ipoffp1 is non zero, (ipoffp1-1) is the number of bytes to IP header,
                                                and the hardware will calculate and insert the  UDP/TCP checksum */
        uint64_t            ignore_i    : 1; /**< If set, ignore the I bit (force to zero) from all pointer structures */
        uint64_t            dontfree    : 1; /**< If clear, the hardware will attempt to free the buffers containing the packet */
        uint64_t            segs        : 6; /**< The total number of segs in the packet, if gather set, also gather list length */
        uint64_t            total_bytes :16; /**< Including L2, but no trailing CRC */
    } s;
} cvmx_pko_command_word0_t;
typedef union
{
    uint64_t u64;
    struct
    {
#if __BYTE_ORDER == __BIG_ENDIAN
        uint64_t reserved                : 60;      /**< Reserved */
        uint64_t st_en                   : 1;       /**< Status channel enabled */
        uint64_t spare                   : 2;       /**< Spare bits */
        uint64_t inf_en                  : 1;       /**< Interface enable */
#else
        uint64_t inf_en                  : 1;
        uint64_t spare                   : 2;
        uint64_t st_en                   : 1;
        uint64_t reserved                : 60;
#endif
    } s;
} cvmx_stxx_com_ctl_t;

typedef union
{
    uint64_t u64;
    struct
    {
#if __BYTE_ORDER == __BIG_ENDIAN
        uint64_t reserved1               : 56;      /**< Reserved */
        uint64_t reserved0               : 2;       /**< Reserved */
        uint64_t afce                    : 1;       /**< Auto Flow Control Enable bit */
        uint64_t loop                    : 1;       /**< Loopback bit */
        uint64_t out2                    : 1;       /**< OUT2 output bit */
        uint64_t out1                    : 1;       /**< OUT1 output bit */
        uint64_t rts                     : 1;       /**< RTS output bit */
        uint64_t dtr                     : 1;       /**< DTR output bit */
#else
        uint64_t dtr                     : 1;
        uint64_t rts                     : 1;
        uint64_t out1                    : 1;
        uint64_t out2                    : 1;
        uint64_t loop                    : 1;
        uint64_t afce                    : 1;
        uint64_t reserved0               : 2;
        uint64_t reserved1               : 56;
#endif
    } s;
} cvmx_uart_mcr_t;

typedef union
{
    uint64_t u64;
    struct
    {
#if __BYTE_ORDER == __BIG_ENDIAN
        uint64_t reserved                : 56;      /**< Reserved */
        uint64_t ferr                    : 1;       /**< Error in Receiver FIFO bit */
        uint64_t temt                    : 1;       /**< Transmitter Empty bit */
        uint64_t thre                    : 1;       /**< Transmitter Holding Register Empty bit */
        uint64_t bi                      : 1;       /**< Break Interrupt bit */
        uint64_t fe                      : 1;       /**< Framing Error bit */
        uint64_t pe                      : 1;       /**< Parity Error bit */
        uint64_t oe                      : 1;       /**< Overrun Error bit */
        uint64_t dr                      : 1;       /**< Data Ready bit */
#else
        uint64_t dr                      : 1;
        uint64_t oe                      : 1;
        uint64_t pe                      : 1;
        uint64_t fe                      : 1;
        uint64_t bi                      : 1;
        uint64_t thre                    : 1;
        uint64_t temt                    : 1;
        uint64_t ferr                    : 1;
        uint64_t reserved                : 56;
#endif
    } s;
} cvmx_uart_lsr_t;


typedef union
{
    uint64_t u64;
    struct
    {
#if __BYTE_ORDER == __BIG_ENDIAN
        uint64_t reserved1               : 56;      /**< Reserved */
        uint64_t rxtrig                  : 2;       /**< RX Trigger */
        uint64_t txtrig                  : 2;       /**< TX Trigger */
        uint64_t reserved0               : 1;       /**< Reserved */
        uint64_t txfr                    : 1;       /**< TX FIFO reset */
        uint64_t rxfr                    : 1;       /**< RX FIFO reset */
        uint64_t en                      : 1;       /**< FIFO enable */
#else
        uint64_t en                      : 1;
        uint64_t rxfr                    : 1;
        uint64_t txfr                    : 1;
        uint64_t reserved0               : 1;
        uint64_t txtrig                  : 2;
        uint64_t rxtrig                  : 2;
        uint64_t reserved1               : 56;
#endif
    } s;
} cvmx_uart_fcr_t;

typedef enum
{
    CVMX_UART_BITS5 = 0,
    CVMX_UART_BITS6 = 1,
    CVMX_UART_BITS7 = 2,
    CVMX_UART_BITS8 = 3
} cvmx_uart_bits_t;
typedef union
{
    uint64_t u64;
    struct
    {
#if __BYTE_ORDER == __BIG_ENDIAN
        uint64_t reserved1               : 56;      /**< Reserved */
        uint64_t dlab                    : 1;       /**< Divisor Latch Address bit */
        uint64_t brk                     : 1;       /**< Break Control bit */
        uint64_t reserved0               : 1;       /**< Reserved */
        uint64_t eps                     : 1;       /**< Parity Select bit */
        uint64_t pen                     : 1;       /**< Parity Enable bit */
        uint64_t stop                    : 1;       /**< Stop Control bit */
        cvmx_uart_bits_t cls             : 2;       /**< Data bits per character */
#else
        cvmx_uart_bits_t cls             : 2;
        uint64_t stop                    : 1;
        uint64_t pen                     : 1;
        uint64_t eps                     : 1;
        uint64_t reserved0               : 1;
        uint64_t brk                     : 1;
        uint64_t dlab                    : 1;
        uint64_t reserved1               : 56;
#endif
    } s;
} cvmx_uart_lcr_t;

typedef union
{
    uint64_t u64;
    struct
    {
#if __BYTE_ORDER == __BIG_ENDIAN
        uint64_t reserved                : 8;       /**< Reserved for future use. Set to zero */
        uint64_t timer                   : 4;       /**< Four general timer interrupts */
        uint64_t key_zero                : 1;       /**< Key zeroization interrupt. Set when the external key
                                                         zeroization pin has a 0->1 edge. */
        uint64_t ipd_drp                 : 1;       /**< One IPD packet drop interrupt. Indicates that IPD
                                                         dropped a packet.
                                                         - This bit is the sum of 8 bits held in IPD - one per QOS.
                                                         - IPD should also have 8 local mask bits. */
        uint64_t gmx_drp                 : 2;       /**< Two GMX packet drop interrupts, one per GMX */
        uint64_t trace                   : 1;       /**< Trace buffer interrupt. L2C has the CMB trace buffer */
        uint64_t rml                     : 1;       /**< RML is an N2 unit that:
                                                         - Provides indirect CSR access for Octeon
                                                             CSR's that cannot directly be accessed from
                                                             the NCB bus.
                                                         - Provides interrupt accumulation for
                                                             attached CSR's/units and or's them together
                                                             into the single wire fed into CIU.
                                                             The RML-based access methods are slower than direct
                                                         NCB access, so it contains CSR's and interrupts
                                                         intended to be accessed rarely. The interrupts are
                                                         typically error interrupts. The following are included
                                                         in the interrupts ored together:
                                                         - LMC SECDED failure information
                                                         - RGMII error information
                                                         - SPI-4.2 error information
                                                         - DFA SECDED failure information
                                                         - Many others */
        uint64_t twsi                    : 1;       /**< TWSI interrupt */
        uint64_t wdog_sum                : 1;       /**< Watchdog summary */
        uint64_t pci_msi                 : 4;       /**< PCI MSI - The PCI MSI bit array is 64-bits.
                                                         External devices can set bits via the
                                                         PCI_MSI_RCV BAR0 register.
                                                         - First bit is the or of <15:0>
                                                         - Second bit is the or of <31:16>
                                                         - Third bit is the or of <47:32>
                                                         - Fourth bit is the or of <63:48> */
        uint64_t pci_int                 : 4;       /**< PCI INTA/B/C/D when Octeon is in host mode */
        uint64_t uart                    : 2;       /**< Two uart interrupts */
        uint64_t mbox                    : 2;       /**< 2 mailbox interrupts. Each PP can only see
                                                         the two bits associated with its mailbox
                                                         register.
                                                         - First bit is the or of mailbox<15:0>
                                                         - Second bit is the or of mailbox<31:16> */
        uint64_t gpio                    : 16;      /**< One per GPIO pin. Each bit is RO if the
                                                         interrupt is level-sensitive, R/W1C if
                                                         edge-triggered. */
        uint64_t workq                   : 16;      /**< Work queue interrupt - one per group */
#else
        uint64_t workq                   : 16;
        uint64_t gpio                    : 16;
        uint64_t mbox                    : 2;
        uint64_t uart                    : 2;
        uint64_t pci_int                 : 4;
        uint64_t pci_msi                 : 4;
        uint64_t wdog_sum                : 1;
        uint64_t twsi                    : 1;
        uint64_t rml                     : 1;
        uint64_t trace                   : 1;
        uint64_t gmx_drp                 : 2;
        uint64_t ipd_drp                 : 1;
        uint64_t key_zero                : 1;
        uint64_t timer                   : 4;
        uint64_t reserved                : 8;
#endif
    } s;
} cvmx_ciu_intx0_t;

typedef union
{
    void*           ptr;
    uint64_t        u64;
    struct
    {
        uint64_t    i    : 1; /**< if set, invert the "free" pick of the overall packet. HW always sets this bit to 0 on inbound packet */
        uint64_t    back : 4; /**< Indicates the amount to back up to get to the buffer start in cache lines. In most cases
                                this is less than one complete cache line, so the value is zero */
        uint64_t    pool : 3; /**< The pool that the buffer came from / goes to */
        uint64_t    size :16; /**< The size of the segment pointed to by addr (in bytes) */
        uint64_t    addr :40; /**< Pointer to the first byte of the data, NOT buffer */
    } s;
} cvmx_buf_ptr_t;


/**
 * Definition of the hardware structure used to configure an
 * output queue.
 */
typedef union
{
    uint64_t u64;
    struct
    {
#if __BYTE_ORDER == __BIG_ENDIAN
        uint64_t s_tail                  : 1;       /**< Set if this QID is the tail of the static queues */
        uint64_t static_p                : 1;       /**< Set if any QID in this PID has static priority */
        uint64_t static_q                : 1;       /**< Set if this QID has static priority */
        uint64_t qos_mask                : 8;       /**< Mask to control priority across 8 QOS rounds */
        uint64_t buf_ptr                 : 36;      /**< Command buffer pointer, <23:17> MBZ */
        uint64_t tail                    : 1;       /**< Set if this QID is the tail of the queue array */
        uint64_t index                   : 3;       /**< Index (distance from head) in the queue array */
        uint64_t port                    : 6;       /**< Port ID to which this queue is mapped */
        uint64_t queue                   : 7;       /**< Queue ID */
#else
        uint64_t queue                   : 7;
        uint64_t port                    : 6;
        uint64_t index                   : 3;
        uint64_t tail                    : 1;
        uint64_t buf_ptr                 : 36;
        uint64_t qos_mask                : 8;
        uint64_t static_q                : 1;
        uint64_t static_p                : 1;
        uint64_t s_tail                  : 1;
#endif
    } s;
} cvmx_pko_queue_cfg_t;

typedef union
{
    uint64_t u64;
    struct
    {
#if __BYTE_ORDER == __BIG_ENDIAN
        uint64_t v                       : 1;       /**< Valid bit
                                                         - Set on a write (should always be written with
                                                         a 1)
                                                         - Cleared when a TWSI master mode op completes
                                                         - Cleared when a TWSI configuration register
                                                         access completes
                                                         - Cleared when the TWSI device reads the
                                                         register if SLONLY==1 */
        uint64_t slonly                  : 1;       /**< Slave Only Mode
                                                         - No operation is initiated with a write when
                                                         this bit is set - only D field is updated in
                                                         this case
                                                         - When clear, a write initiates either a TWSI
                                                         master-mode operation or a TWSI configuration
                                                         register access */
        uint64_t flop                    : 1;       /**< Unused, retains state */
        uint64_t op                      : 4;       /**< Opcode field - When the register is written with
                                                         SLONLY==0, initiate a read or write:
                                                         - 0000 => 7-bit Byte Master Mode TWSI Op
                                                         - 0001 => 7-bit Combined Byte Master Mode Op
                                                         - 0010 => 10-bit Byte Master Mode TWSI Op
                                                         - 0011 => 10-bit Combined Byte Master Mode Op
                                                         - 0100 => TWSI Master Clock Register
                                                         - 0110 => See EOP field
                                                         - 1000 => 7-bit 4-byte Master Mode TWSI Op
                                                         - 1001 => 7-bit 4-byte Comb. Master Mode Op
                                                         - 1010 => 10-bit 4-byte Master Mode TWSI Op
                                                         - 1011 => 10-bit 4-byte Comb. Master Mode Op */
        uint64_t r                       : 1;       /**< Read bit or result
                                                         - If set on a write when SLONLY==0, the
                                                         operation is a read
                                                         - On a read, this bit returns the result
                                                         indication for the most recent master mode
                                                         operation (1 = success, 0 = fail) */
        uint64_t flops                   : 6;       /**< Unused, but retain state */
        uint64_t a                       : 10;      /**< Address field
                                                         - the address of the remote device for a master
                                                         mode operation
                                                         - A<9:7> are only used for 10-bit addressing */
        uint64_t ia                      : 5;       /**< Internal Address - Used when launching a combined
                                                         master mode operation (lower 3 bits are
                                                         contained in the EOP_IA field) */
        uint64_t eop_ia                  : 3;       /**< Extra opcode (when OP<3:0> == 0110 and SLONLY==0):
                                                         - 000 => TWSI Slave Address Register
                                                         - 001 => TWSI Data Register
                                                         - 010 => TWSI Control Register
                                                         - 011 => TWSI Clock Control Register (when R == 0)
                                                         - 011 => TWSI Status Register (when R == 1)
                                                         - 100 => TWSI Extended Slave Register
                                                         - 111 => TWSI Soft Reset Register
                                                         Also the lower 3 bits of Internal Address when
                                                         launching a combined master mode operation */
        uint64_t d                       : 32;      /**< Data Field
                                                         Used on a write when
                                                         - initiating a master-mode write (SLONLY==0)
                                                         - writing a TWSI config register (SLONLY==0)
                                                         - a slave mode write (SLONLY==1)
                                                         The read value is updated by
                                                         - a write to this register
                                                         - master mode completion (contains error code)
                                                         - TWSI config register read (contains result) */
#else
        uint64_t d                       : 32;
        uint64_t eop_ia                  : 3;
        uint64_t ia                      : 5;
        uint64_t a                       : 10;
        uint64_t flops                   : 6;
        uint64_t r                       : 1;
        uint64_t op                      : 4;
        uint64_t flop                    : 1;
        uint64_t slonly                  : 1;
        uint64_t v                       : 1;
#endif
    } s;
} cvmx_mio_tws_sw_twsi_t;



/**
 * HW decode / err_code in work queue entry
 */
typedef union
{
    uint64_t                 u64;

    /** Use this struct if the hardware determines that the packet is IP */
    struct
    {
        uint64_t               bufs          : 8; /**< HW sets this to the number of buffers used by this packet */
        uint64_t               ip_offset     : 8; /**< HW sets to the number of L2 bytes prior to the IP */
        uint64_t               vlan_valid    : 1; /**< set to 1 if we found VLAN in the L2 */
        uint64_t               unassigned    : 2;
        uint64_t               vlan_cfi      : 1; /**< HW sets to the VLAN CFI flag (valid when vlan_valid) */
        uint64_t               vlan_id       :12; /**< HW sets to the VLAN_ID field (valid when vlan_valid) */

        uint64_t               unassigned2   :12;
        uint64_t               dec_ipcomp    : 1; /**< the packet needs to be decompressed */
        uint64_t               tcp_or_udp    : 1; /**< the packet is either TCP or UDP */
        uint64_t               dec_ipsec     : 1; /**< the packet needs to be decrypted (ESP or AH) */
        uint64_t               is_v6         : 1; /**< the packet is IPv6 */

        // (rcv_error, not_IP, IP_exc, is_frag, L4_error, software, etc.)

        uint64_t               software      : 1; /**< reserved for software use, hardware will clear on packet creation */
        // exceptional conditions below
        uint64_t               L4_error      : 1; /**< the receive interface hardware detected an L4 error (only applies if !is_frag)
                                                    (only applies if !rcv_error && !not_IP && !IP_exc && !is_frag)
                                                    failure indicated in err_code below, decode:
                                                    - 1  = TCP (UDP) packet not long enough to cover TCP (UDP) header
                                                    - 2  = illegal TCP/UDP port (either source or dest port is zero)
                                                    - 3  = TCP/UDP checksum failure
                                                    - 4  = TCP/UDP length check (TCP/UDP length does not match IP length)
                                                    - 8  = TCP flags = FIN only
                                                    - 9  = TCP flags = 0
                                                    - 10 = TCP flags = FIN+RST+*
                                                    - 11 = TCP flags = SYN+URG+*
                                                    - 12 = TCP flags = SYN+RST+*
                                                    - 13 = TCP flags = SYN+FIN+* */
        uint64_t               is_frag       : 1; /**< set if the packet is a fragment */
        uint64_t               IP_exc        : 1; /**< the receive interface hardware detected an IP error / exception
                                                    (only applies if !rcv_error && !not_IP) failure indicated in err_code below, decode:
                                                    - 1 = not IPv4 or IPv6
                                                    - 2 = IPv4 header checksum violation
                                                    - 3 = malformed (packet not long enough to cover IP hdr, or not long enough to cover len in IP hdr)
                                                    - 4 = TTL / hop count equal zero
                                                    - 5 = IPv4 options / IPv6 early extension headers */
        uint64_t               is_bcast      : 1; /**< set if the hardware determined that the packet is a broadcast */
        uint64_t               is_mcast      : 1; /**< set if the hardware determined that the packet is a multi-cast */
        uint64_t               not_IP        : 1; /**< set if the packet may not be IP (must be zero in this case) */
        uint64_t               rcv_error     : 1; /**< the receive interface hardware detected a receive error (must be zero in this case) */
        /* lower err_code = first-level descriptor of the work */
        /* zero for packet submitted by hardware that isn't on the slow path */

        uint64_t               err_code      : 8; /**< type is cvmx_pip_err_t */
    } s;

    /**< use this to get at the 16 vlan bits */
    struct
    {
        uint64_t               unused1       :16;
        uint64_t               vlan          :16;
        uint64_t               unused2       :32;
    } svlan;

    /**< use this struct if the hardware could not determine that the packet is ip */
    struct
    {
        uint64_t               bufs          : 8; /**< HW sets this to the number of buffers used by this packet */
        uint64_t               unused        : 8;
        uint64_t               vlan_valid    : 1; /**< set to 1 if we found VLAN in the L2 */
        uint64_t               unassigned    : 2;
        uint64_t               vlan_cfi      : 1; /**< HW sets to the VLAN CFI flag (valid when vlan_valid) */
        uint64_t               vlan_id       :12; /**< HW sets to the VLAN_ID field (valid when vlan_valid) */

        uint64_t               unassigned2   :16;
        uint64_t               software      : 1; /**< reserved for software use, hardware will clear on packet creation */
        uint64_t               unassigned3   : 1;
        uint64_t               is_rarp       : 1; /**< set if the hardware determined that the packet is rarp */
        uint64_t               is_arp        : 1; /**< set if the hardware determined that the packet is arp */
        uint64_t               is_bcast      : 1; /**< set if the hardware determined that the packet is a broadcast */
        uint64_t               is_mcast      : 1; /**< set if the hardware determined that the packet is a multi-cast */
        uint64_t               not_IP        : 1; /**< set if the packet may not be IP (must be one in this case) */
        uint64_t               rcv_error     : 1; /**< the receive interface hardware detected a receive error.
                                                    Failure indicated in err_code below, decode:
                                                    - 1 = min frame error (pkt len < min frame len)
                                                    - 2 = Frame carrier extend error
                                                    - 3 = max frame error (pkt len > max frame len)
                                                    - 4 = very long frame error - frame is truncated (pkt len > sys frame len)
                                                    - 5 = FCS error (GMX)
                                                    - 6 = nibble error (data not byte multiple - 100M and 10M only)
                                                    - 7 = length mismatch (len did not match len in L2 length/type)
                                                    - 8 = Frame error (some or all bits marked err)
                                                    - 9 = packet was not large enough to pass the skipper - no inspection could occur
                                                    - 10 = studder error (data not repeated - 100M and 10M only)
                                                    - 11 = partially received packet (buffering/bandwidth not adequate)
                                                    - 12 = FCS error (PIP)
                                                    - 15 = packet was not large enough to pass the skipper - no inspection could occur */
        /* lower err_code = first-level descriptor of the work */
        /* zero for packet submitted by hardware that isn't on the slow path */
        uint64_t               err_code       : 8; // type is cvmx_pip_err_t (union, so can't use directly
    } snoip;

} cvmx_pip_wqe_word2;
/**
 * Work queue entry format
 *
 * must be 8-byte aligned
 */
typedef struct
{

    /*****************************************************************
     * WORD 0
     *  HW WRITE: the following 64 bits are filled by HW when a packet arrives
     */

    /**
     * raw chksum result generated by the HW
     */
    uint16_t                   hw_chksum;
    /**
     * Field unused by hardware - available for software
     */
    uint8_t                    unused;
    /**
     * Next pointer used by hardware for list maintenance.
     * May be written/read by HW before the work queue
     *           entry is scheduled to a PP
     * (Only 36 bits used in Octeon 1)
     */
    uint64_t                   next_ptr      : 40;


    /*****************************************************************
     * WORD 1
     *  HW WRITE: the following 64 bits are filled by HW when a packet arrives
     */

    /**
     * HW sets to the total number of bytes in the packet
     */
    uint64_t                   len           :16;
    /**
     * HW sets this to input physical port
     */
    uint64_t                   ipprt         : 6;

    /**
     * HW sets this to what it thought the priority of the input packet was
     */
    uint64_t                   qos           : 3;

    /**
     * the group that the work queue entry will be scheduled to
     */
    uint64_t                   grp           : 4;
    /**
     * the type of the tag (ORDERED, ATOMIC, NULL)
     */
    cvmx_pow_tag_type_t        tag_type      : 3;
    /**
     * the synchronization/ordering tag
     */
    uint64_t                   tag           :32;

    /**
     * WORD 2
     *   HW WRITE: the following 64-bits are filled in by hardware when a packet arrives
     *   This indicates a variety of status and error conditions.
     */
    cvmx_pip_wqe_word2       word2;

    /**
     * Pointer to the first segment of the packet.
     */
    cvmx_buf_ptr_t             packet_ptr;

    /**
     *   HW WRITE: octeon will fill in a programmable amount from the
     *             packet, up to (at most, but perhaps less) the amount
     *             needed to fill the work queue entry to 128 bytes
     *   If the packet is recognized to be IP, the hardware starts (except that
     *   the IPv4 header is padded for appropriate alignment) writing here where
     *   the IP header starts.
     *   If the packet is not recognized to be IP, the hardware starts writing
     *   the beginning of the packet here.
     */
    uint8_t packet_data[96];


    /**
     * If desired, SW can make the work Q entry any length. For the
     * purposes of discussion here, Assume 128B always, as this is all that
     * the hardware deals with.
     *
     */

} cvmx_wqe_t  __attribute__ ((aligned (128)));



typedef union {

   uint64_t         u64;

   struct {
      cvmx_mips_space_t          R   : 2;
      uint64_t               offset :62;
   } sva; // mapped or unmapped virtual address

   struct {
      uint64_t               zeroes :33;
      uint64_t               offset :31;
   } suseg; // mapped USEG virtual addresses (typically)

   struct {
      uint64_t                ones  :33;
      cvmx_mips_xkseg_space_t   sp   : 2;
      uint64_t               offset :29;
   } sxkseg; // mapped or unmapped virtual address

   struct {
      cvmx_mips_space_t          R   : 2; // CVMX_MIPS_SPACE_XKPHYS in this case
      uint64_t                 cca  : 3; // ignored by octeon
      uint64_t                 mbz  :10;
      uint64_t                  pa  :49; // physical address
   } sxkphys; // physical address accessed through xkphys unmapped virtual address

   struct {
      uint64_t                 mbz  :15;
      uint64_t                is_io : 1; // if set, the address is uncached and resides on MCB bus
      uint64_t                 did  : 8; // the hardware ignores this field when is_io==0, else device ID
      uint64_t                unaddr: 4; // the hardware ignores <39:36> in Octeon I
      uint64_t               offset :36;
   } sphys; // physical address

   struct {
      uint64_t               zeroes :24; // techically, <47:40> are dont-cares
      uint64_t                unaddr: 4; // the hardware ignores <39:36> in Octeon I
      uint64_t               offset :36;
   } smem; // physical mem address

   struct {
      uint64_t                 mem_region  :2;
      uint64_t                 mbz  :13;
      uint64_t                is_io : 1; // 1 in this case
      uint64_t                 did  : 8; // the hardware ignores this field when is_io==0, else device ID
      uint64_t                unaddr: 4; // the hardware ignores <39:36> in Octeon I
      uint64_t               offset :36;
   } sio; // physical IO address

   struct {
      uint64_t                ones   : 49;
      cvmx_add_win_dec_t   csrdec : 2;    // CVMX_ADD_WIN_SCR (0) in this case
      uint64_t                addr   : 13;
   } sscr; // scratchpad virtual address - accessed through a window at the end of kseg3

   // there should only be stores to IOBDMA space, no loads
   struct {
      uint64_t                ones   : 49;
      cvmx_add_win_dec_t   csrdec : 2;    // CVMX_ADD_WIN_DMA (1) in this case
      uint64_t                unused2: 3;
      cvmx_add_win_dma_dec_t type : 3;
      uint64_t                addr   : 7;
   } sdma; // IOBDMA virtual address - accessed through a window at the end of kseg3

   struct {
      uint64_t                didspace : 24;
      uint64_t                unused   : 40;
   } sfilldidspace;

} cvmx_addr_t;

typedef union
{
    uint64_t u64;
    struct
    {
#if __BYTE_ORDER == __BIG_ENDIAN
        uint64_t reserved                : 58;      /**< MBZ */
        uint64_t mode1                   : 3;       /**< # of GMX1 ports = 16 >> MODE1, 0 <= MODE1 <=4 */
        uint64_t mode0                   : 3;       /**< # of GMX0 ports = 16 >> MODE0, 0 <= MODE0 <=4 */
#else
        uint64_t mode0                   : 3;
        uint64_t mode1                   : 3;
        uint64_t reserved                : 58;
#endif
    } s;
} cvmx_pko_reg_gmx_port_mode_t;

typedef union
{
    uint64_t u64;
    struct
    {
#if __BYTE_ORDER == __BIG_ENDIAN
        uint64_t reserved                : 51;      /**< Reserved */
        uint64_t trntest                 : 1;       /**< Training Test Mode
                                                         This bit is only for initial bringup
                                                         (spx_csr__spi4_trn_test_mode) */
        uint64_t jitter                  : 3;       /**< Accounts for jitter when the macro sequence is
                                                         locking.  The value is how many consecutive
                                                         transititions before declaring en edge.  Minimum
                                                         value is 1.  This parameter must be set for Spi4
                                                         mode using auto-bit deskew.
                                                         (spx_csr__spi4_mac_jitter) */
        uint64_t clr_boot                : 1;       /**< Clear the macro boot sequence mode bit
                                                         (spx_csr__spi4_mac_clr_boot) */
        uint64_t set_boot                : 1;       /**< Enable the macro boot sequence mode bit
                                                         (spx_csr__spi4_mac_set_boot) */
        uint64_t maxdist                 : 5;       /**< This field defines how far from center the
                                                         deskew logic will search in a single macro
                                                         sequence (spx_csr__spi4_mac_iters) */
        uint64_t macro_en                : 1;       /**< Allow the macro sequence to center the sample
                                                         point in the data window through hardware
                                                         (spx_csr__spi4_mac_trn_en) */
        uint64_t mux_en                  : 1;       /**< Enable the hardware machine that selects the
                                                         proper coarse FLOP selects
                                                         (spx_csr__spi4_mux_trn_en) */
#else
        uint64_t mux_en                  : 1;
        uint64_t macro_en                : 1;
        uint64_t maxdist                 : 5;
        uint64_t set_boot                : 1;
        uint64_t clr_boot                : 1;
        uint64_t jitter                  : 3;
        uint64_t trntest                 : 1;
        uint64_t reserved                : 51;
#endif
    } s;
} cvmx_spxx_trn4_ctl_t;
typedef union
{
    uint64_t u64;
    struct
    {
#if __BYTE_ORDER == __BIG_ENDIAN
        uint64_t reserved                : 56;      /**< Reserved */
        uint64_t prts                    : 4;       /**< Number of ports in the receiver (write: ports - 1)
                                                         - 0:  1 port
                                                         - 1:  2 ports
                                                         - 2:  3 ports
                                                         -
                                                         - 15: 16 ports */
        uint64_t st_en                   : 1;       /**< Status channel enabled
                                                         This is to allow configs without a status channel.
                                                         This bit should not be modified once the
                                                         interface is enabled. */
        uint64_t spare                   : 2;       /**< Spare bits */
        uint64_t inf_en                  : 1;       /**< Interface enable
                                                         The master switch that enables the entire
                                                         interface. SRX will not validiate any data until
                                                         this bit is set. This bit should not be modified
                                                         once the interface is enabled. */
#else
        uint64_t inf_en                  : 1;
        uint64_t spare                   : 2;
        uint64_t st_en                   : 1;
        uint64_t prts                    : 4;
        uint64_t reserved                : 56;
#endif
    } s;
} cvmx_srxx_com_ctl_t;


typedef union
{
    uint64_t u64;
    struct
    {
#if __BYTE_ORDER == __BIG_ENDIAN
        uint64_t reserved                : 55;      /**< Reserved */
        uint64_t reset                   : 1;       /**< When set '1' causes a reset of the IPD, except
                                                         RSL. */
        uint64_t addpkt                  : 1;       /**< When IPD_CTL_STATUS[ADDPKT] is set,
                                                         IPD_PORT_BP_COUNTERS_PAIR(port)[CNT_VAL]
                                                         WILL be incremented by one for every work
                                                         queue entry that is sent to POW.
                                                         PASS-2 Field. */
        uint64_t naddbuf                 : 1;       /**< When IPD_CTL_STATUS[NADDBUF] is set,
                                                         IPD_PORT_BP_COUNTERS_PAIR(port)[CNT_VAL]
                                                         WILL NOT be incremented when IPD allocates a
                                                         buffer for a packet on the port.
                                                         PASS-2 Field. */
        uint64_t pkt_lend                : 1;       /**< Changes PKT to little endian writes to L2C */
        uint64_t wqe_lend                : 1;       /**< Changes WQE to little endian writes to L2C */
        uint64_t pbp_en                  : 1;       /**< Port back pressure enable. When set '1' enables
                                                         the sending of port level backpressure to the
                                                         Octane input-ports. Once enabled the sending of
                                                         port-level-backpressure can not be disabled by
                                                         changing the value of this bit. */
        cvmx_ipd_mode_t opc_mode         : 2;       /**< 0 ==> All packet data (and next buffer pointers)
                                                         is written through to memory.
                                                         1 ==> All packet data (and next buffer pointers) is
                                                         written into the cache.
                                                         2 ==> The first aligned cache block holding the
                                                         packet data (and initial next buffer pointer) is
                                                         written to the L2 cache, all remaining cache blocks
                                                         are not written to the L2 cache.
                                                         3 ==> The first two aligned cache blocks holding
                                                         the packet data (and initial next buffer pointer)
                                                         are written to the L2 cache, all remaining cache
                                                         blocks are not written to the L2 cache. */
        uint64_t ipd_en                  : 1;       /**< When set '1' enable the operation of the IPD. */
#else
        uint64_t ipd_en                  : 1;
        cvmx_ipd_mode_t opc_mode         : 2;
        uint64_t pbp_en                  : 1;
        uint64_t wqe_lend                : 1;
        uint64_t pkt_lend                : 1;
        uint64_t naddbuf                 : 1;
        uint64_t addpkt                  : 1;
        uint64_t reset                   : 1;
        uint64_t reserved                : 55;
#endif
    } s;
} cvmx_ipd_ctl_status_t;
typedef union
{
    uint64_t u64;
    struct
    {
#if __BYTE_ORDER == __BIG_ENDIAN
        uint64_t reserved                : 60;      /**< MBZ */
        uint64_t reset                   : 1;       /**< Reset oneshot pulse */
        uint64_t store_le                : 1;       /**< Force STORE0 byte write address to little endian */
        uint64_t ena_dwb                 : 1;       /**< Set to enable DontWriteBacks */
        uint64_t ena_pko                 : 1;       /**< Set to enable the PKO picker */
#else
        uint64_t ena_pko                 : 1;
        uint64_t ena_dwb                 : 1;
        uint64_t store_le                : 1;
        uint64_t reset                   : 1;
        uint64_t reserved                : 60;
#endif
    } s;
} cvmx_pko_reg_flags_t;

typedef union
{
    uint64_t         u64;

    cvmx_wqe_t *wqp;

    // response to new work request loads
    struct
    {
        uint64_t       no_work : 1;   // set when no new work queue entry was returned
        // If there was de-scheduled work, the HW will definitely
        // return it. When this bit is set, it could mean
        // either mean:
        //   - There was no work, or
        //   - There was no work that the HW could find. This
        //     case can happen, regardless of the wait bit value
        //     in the original request, when there is work
        //     in the IQ's that is too deep down the list.
        uint64_t       unused  : 23;
        uint64_t       addr    : 40;  // 36 in O1 -- the work queue pointer
    } s_work;

    // response to NULL_RD request loads
    struct
    {
        uint64_t       unused  : 62;
        uint64_t       state    : 2;  // of type cvmx_pow_tag_type_t
        // state is one of the following:
        //       CVMX_POW_TAG_TYPE_ORDERED
        //       CVMX_POW_TAG_TYPE_ATOMIC
        //       CVMX_POW_TAG_TYPE_NULL
        //       CVMX_POW_TAG_TYPE_NULL_NULL
    } s_null_rd;

} cvmx_pow_tag_load_resp_t;




typedef union
{
    uint64_t u64;

    // address for new work request loads (did<2:0> == 0)
    struct
    {
        uint64_t                mem_region  :2;
        uint64_t                mbz  :13;
        uint64_t                is_io  : 1;    // must be one
        uint64_t                did    : 8;    // the ID of POW -- did<2:0> == 0 in this case
        uint64_t                unaddr : 4;
        uint64_t                unused :32;
        uint64_t                wait   : 1;    // if set, don't return load response until work is available
        uint64_t                mbzl   : 3;    // must be zero
    } swork; // physical address


    // address for NULL_RD request (did<2:0> == 4)
    // when this is read, HW attempts to change the state to NULL if it is NULL_NULL
    // (the hardware cannot switch from NULL_NULL to NULL if a POW entry is not available -
    // software may need to recover by finishing another piece of work before a POW
    // entry can ever become available.)
    struct
    {
        uint64_t                mem_region  :2;
        uint64_t                mbz  :13;
        uint64_t                is_io  : 1;    // must be one
        uint64_t                did    : 8;    // the ID of POW -- did<2:0> == 4 in this case
        uint64_t                unaddr : 4;
        uint64_t                unused :33;
        uint64_t                mbzl   : 3;    // must be zero
    } snull_rd; // physical address

    // address for CSR accesses
    struct
    {
        uint64_t                mem_region  :2;
        uint64_t                mbz  :13;
        uint64_t                is_io  : 1;    // must be one
        uint64_t                did    : 8;    // the ID of POW -- did<2:0> == 7 in this case
        uint64_t                unaddr : 4;
        uint64_t                csraddr:36;    // only 36 bits in O1, addr<2:0> must be zero
    } stagcsr; // physical address

} cvmx_pow_load_addr_t;


typedef union
{
    uint64_t u64;
    struct
    {
#if __BYTE_ORDER == __BIG_ENDIAN
        uint64_t reserved                : 42;      /**< Must be zero */
        uint64_t fpf_wr                  : 11;      /**< When the number of free-page-pointers in a
                                                         queue exceeds this value the FPA will write
                                                         32-page-pointers of that queue to DRAM.
                                                         The MAX value for this field should be
                                                         FPA_FPF0_SIZE[FPF_SIZ]-2. */
        uint64_t fpf_rd                  : 11;      /**< When the number of free-page-pointers in a
                                                         queue drops below this value amd there are
                                                         free-page-pointers in DRAM, the FPA will
                                                         read one page (32 pointers) from DRAM.
                                                         This maximum value for this field should be
                                                         FPA_FPF0_SIZE[FPF_SIZ]-34. The min number
                                                         for this would be 16. */
#else
        uint64_t fpf_rd                  : 11;
        uint64_t fpf_wr                  : 11;
        uint64_t reserved                : 42;
#endif
    } s;
} cvmx_fpa_fpf_marks_t;

typedef union
{
    uint64_t u64;
    struct
    {
#if __BYTE_ORDER == __BIG_ENDIAN
        uint64_t reserved                : 46;      /**< Reserved */
        uint64_t reset                   : 1;       /**< When set causes a reset of the FPA with the
                                                         exception of the RSL. This is a PASS-2 field. */
        uint64_t use_ldt                 : 1;       /**< When clear '0' the FPA will use LDT to load
                                                         pointers from the L2C. This is a PASS-2 field. */
        uint64_t use_stt                 : 1;       /**< When clear '0' the FPA will use STT to store
                                                         pointers to the L2C. This is a PASS-2 field. */
        uint64_t enb                     : 1;       /**< Must be set to 1 AFTER writing all config registers
                                                         and 10 cycles have past. If any of the config
                                                         register are written after writing this bit the
                                                         FPA may begin to operate incorrectly. */
        uint64_t mem1_err                : 7;       /**< Causes a flip of the ECC bit associated 38:32
                                                         respective to bit 6:0 of this field, for FPF
                                                         FIFO 1. */
        uint64_t mem0_err                : 7;       /**< Causes a flip of the ECC bit associated 38:32
                                                         respective to bit 6:0 of this field, for FPF
                                                         FIFO 0. */
#else
        uint64_t mem0_err                : 7;
        uint64_t mem1_err                : 7;
        uint64_t enb                     : 1;
        uint64_t use_stt                 : 1;
        uint64_t use_ldt                 : 1;
        uint64_t reset                   : 1;
        uint64_t reserved                : 46;
#endif
    } s;
} cvmx_fpa_ctl_status_t;
typedef union
{
    uint64_t                u64;
    struct
    {
        cvmx_mips_space_t   mem_space   : 2;    /**< Must CVMX_IO_SEG */
        uint64_t            reserved    :13;    /**< Must be zero */
        uint64_t            is_io       : 1;    /**< Must be one */
        uint64_t            did         : 8;    /**< The ID of the device on the non-coherent bus */
        uint64_t            reserved2   : 4;    /**< Must be zero */
        uint64_t            reserved3   :18;    /**< Must be zero */
        uint64_t            port        : 6;    /**< The hardware likes to have the output port in addition to the output queue */
        uint64_t            queue       : 9;    /**< The output queue to send the packet to (0-127 are legal) */
        uint64_t            reserved4   : 3;    /**< Must be zero */
   } s;
} cvmx_pko_doorbell_address_t;

typedef union
{
    uint64_t u64;
    struct
    {
#if __BYTE_ORDER == __BIG_ENDIAN
        uint64_t reserved                : 58;      /**< Must be zero */
        uint64_t skip_sz                 : 6;       /**< The number of 8-byte words from the top of the
                                                         1st MBUFF that the IPD will store the next-pointer.
                                                         Legal values for this field are 0 to 32, but the
                                                         SKIP_SZ+16 <= IPD_PACKET_MBUFF_SIZE[MB_SIZE]. */
#else
        uint64_t skip_sz                 : 6;
        uint64_t reserved                : 58;
#endif
    } s;
} cvmx_ipd_mbuff_first_skip_t;
typedef union
{
    uint64_t u64;
    struct
    {
#if __BYTE_ORDER == __BIG_ENDIAN
        uint64_t reserved                : 60;      /**< Must be zero */
        uint64_t back                    : 4;       /**< Used to find head of buffer from the nxt-hdr-ptr. */
#else
        uint64_t back                    : 4;
        uint64_t reserved                : 60;
#endif
    } s;
} cvmx_ipd_first_next_ptr_back_t;

typedef cvmx_ipd_mbuff_first_skip_t cvmx_ipd_mbuff_not_first_skip_t;
typedef cvmx_ipd_first_next_ptr_back_t cvmx_ipd_second_next_ptr_back_t;


typedef union
{
    uint64_t u64;
    struct
    {
#if __BYTE_ORDER == __BIG_ENDIAN
        uint64_t reserved                : 61;      /**< Must be zero */
        uint64_t wqe_pool                : 3;       /**< Which FPA Queue to fetch page-pointers from for WQE's. */
#else
        uint64_t wqe_pool                : 3;
        uint64_t reserved                : 61;
#endif
    } s;
} cvmx_ipd_wqe_fpa_pool_t;

typedef union
{
    uint64_t u64;
    struct
    {
#if __BYTE_ORDER == __BIG_ENDIAN
        uint64_t reserved                : 52;      /**< Must be zero */
        uint64_t mb_size                 : 12;      /**< The number of 8-byte words in a MBUF. This
                                                         must be a number in the range of 32 to 2048.
                                                         This is also the size of the FPA's Queue-0 Free-Page. */
#else
        uint64_t mb_size                 : 12;
        uint64_t reserved                : 52;
#endif
    } s;
} cvmx_ipd_mbuff_size_t;












/**
 * PCI_CFG00 = First 32-bits of PCI config space
 * 
 * Access using the CVMX_PCI_CFG00 CSR
 */
typedef union
{
    uint32_t u32;
    struct
    {
#if __BYTE_ORDER == __BIG_ENDIAN
        uint64_t devid                   : 16;      /**< This is the device ID for OCTEON-2P */
        uint64_t vendid                  : 16;      /**< This is the Cavium's vendor ID */
#else
        uint64_t vendid                  : 16;
        uint64_t devid                   : 16;
#endif
    } s;
} cvmx_pci_cfg00_t;


/**
 * PCI_CFG01 = Second 32-bits of PCI config space (Command/Status Register)
 *
 * - Access using the CVMX_PCI_CFG01 CSR
 */
typedef union
{
    uint32_t u32;
    struct
    {
#if __BYTE_ORDER == __BIG_ENDIAN
        uint64_t dpe                     : 1;       /**< Detected Parity Error */
        uint64_t sse                     : 1;       /**< Signaled System Error */
        uint64_t rma                     : 1;       /**< Received Master Abort */
        uint64_t rta                     : 1;       /**< Received Target Abort */
        uint64_t sta                     : 1;       /**< Signaled Target Abort */
        uint64_t devt                    : 2;       /**< DEVSEL# timing (for PCI only/for PCIX = don?t care) */
        uint64_t mdpe                    : 1;       /**< Master Data Parity Error */
        uint64_t fbb                     : 1;       /**< Fast Back-to-Back Transactions Capable
                                                         Mode               1 = PCI Mode     0 = PCIX Mode
                                                         Dependent */
        uint64_t reserved                : 1;       /**< Reserved */
        uint64_t m66                     : 1;       /**< 66MHz Capable */
        uint64_t cle                     : 1;       /**< Capabilities List Enable */
        uint64_t i_stat                  : 1;       /**< When INTx# is asserted by N3 this bit will be set.
                                                         When deasserted by N3 this bit will be cleared. */
        uint64_t reserved1               : 8;       /**< Reserved */
        uint64_t i_dis                   : 1;       /**< When asserted '1' disables the generation of INTx#
                                                         by N3. When disabled '0' allows assertion of INTx#
                                                         by N3. */
        uint64_t fbbe                    : 1;       /**< Fast Back to Back Transaction Enable */
        uint64_t see                     : 1;       /**< System Error Enable */
        uint64_t ads                     : 1;       /**< Address/Data Stepping */
        uint64_t pee                     : 1;       /**< PERR# Enable */
        uint64_t vps                     : 1;       /**< VGA Palette Snooping */
        uint64_t mwice                   : 1;       /**< Memory Write & Invalidate Command Enable */
        uint64_t scse                    : 1;       /**< Special Cycle Snooping Enable */
        uint64_t me                      : 1;       /**< Master Enable */
        uint64_t msae                    : 1;       /**< Memory Space Access Enable */
        uint64_t isae                    : 1;       /**< I/O Space Access Enable */
#else
        uint64_t isae                    : 1;
        uint64_t msae                    : 1;
        uint64_t me                      : 1;
        uint64_t scse                    : 1;
        uint64_t mwice                   : 1;
        uint64_t vps                     : 1;
        uint64_t pee                     : 1;
        uint64_t ads                     : 1;
        uint64_t see                     : 1;
        uint64_t fbbe                    : 1;
        uint64_t i_dis                   : 1;
        uint64_t reserved1               : 8;
        uint64_t i_stat                  : 1;
        uint64_t cle                     : 1;
        uint64_t m66                     : 1;
        uint64_t reserved                : 1;
        uint64_t fbb                     : 1;
        uint64_t mdpe                    : 1;
        uint64_t devt                    : 2;
        uint64_t sta                     : 1;
        uint64_t rta                     : 1;
        uint64_t rma                     : 1;
        uint64_t sse                     : 1;
        uint64_t dpe                     : 1;
#endif
    } s;
} cvmx_pci_cfg01_t;


/**
 * PCI_CFG02 = Third 32-bits of PCI config space (Class Code / Revision ID)
 *
 * - Access using the CVMX_PCI_CFG02 CSR
 */
typedef union
{
    uint32_t u32;
    struct
    {
#if __BYTE_ORDER == __BIG_ENDIAN
        uint64_t cc                      : 24;      /**< Class Code (Network Encryption/Decryption Class) */
        uint64_t rid                     : 8;       /**< Revision ID */
#else
        uint64_t rid                     : 8;
        uint64_t cc                      : 24;
#endif
    } s;
} cvmx_pci_cfg02_t;


/**
 * PCI_CFG03 = Fourth 32-bits of PCI config space (BIST, HEADER Type, Latency timer, line size)
 *
 * - Access using the CVMX_PCI_CFG03 CSR
 */
typedef union
{
    uint32_t u32;
    struct
    {
#if __BYTE_ORDER == __BIG_ENDIAN
        uint64_t bcap                    : 1;       /**< BIST Capable */
        uint64_t brb                     : 1;       /**< BIST Request/busy bit
                                                         Note: N3 does not support PCI BIST, therefore
                                                         this bit should remain zero. */
        uint64_t reserved                : 2;       /**< Reserved */
        uint64_t bcod                    : 4;       /**< BIST Code */
        uint64_t ht                      : 8;       /**< Header Type (Type 0) */
        uint64_t lt                      : 8;       /**< Latency Timer
                                                         (0=PCI)                 (0=PCI)
                                                         (0x40=PCIX)             (0x40=PCIX) */
        uint64_t cls                     : 8;       /**< Cache Line Size */
#else
        uint64_t cls                     : 8;
        uint64_t lt                      : 8;
        uint64_t ht                      : 8;
        uint64_t bcod                    : 4;
        uint64_t reserved                : 2;
        uint64_t brb                     : 1;
        uint64_t bcap                    : 1;
#endif
    } s;
} cvmx_pci_cfg03_t;


/**
 * PCI_CFG04 = Fifth 32-bits of PCI config space (Base Address Register 0 - Low)
 *
 *  Description: BAR0: 4KB 64-bit Prefetchable Memory Space
 *        [0]:     0 (Memory Space)
 *        [2:1]:   2 (64bit memory decoder)
 *        [3]:     1 (Prefetchable)
 *        [11:4]:  RAZ (to imply 4KB space)
 *        [31:12]: RW (User may define base address)
 *
 * - Access using the CVMX_PCI_CFG04 CSR
 */
typedef union
{
    uint32_t u32;
    struct
    {
#if __BYTE_ORDER == __BIG_ENDIAN
        uint64_t lbase                   : 20;      /**< Base Address[31:12] */
        uint64_t lbasez                  : 8;       /**< Base Address[11:4] (Read as Zero) */
        uint64_t pf                      : 1;       /**< Prefetchable Space */
        uint64_t typ                     : 2;       /**< Type (00=32b/01=below 1MB/10=64b/11=RSV) */
        uint64_t mspc                    : 1;       /**< Memory Space Indicator */
#else
        uint64_t mspc                    : 1;
        uint64_t typ                     : 2;
        uint64_t pf                      : 1;
        uint64_t lbasez                  : 8;
        uint64_t lbase                   : 20;
#endif
    } s;
} cvmx_pci_cfg04_t;


/**
 * PCI_CFG05 = Sixth 32-bits of PCI config space (Base Address Register 0 - High)
 *
 * - Access using the CVMX_PCI_CFG05 CSR
 */
typedef union
{
    uint32_t u32;
    struct
    {
#if __BYTE_ORDER == __BIG_ENDIAN
        uint64_t hbase                   : 32;      /**< Base Address[63:32] */
#else
        uint64_t hbase                   : 32;
#endif
    } s;
} cvmx_pci_cfg05_t;


/**
 * PCI_CFG06 = Seventh 32-bits of PCI config space (Base Address Register 1 - Low)
 *
 *  Description: BAR1: 128MB 64-bit Prefetchable Memory Space
 *        [0]:     0 (Memory Space)
 *        [2:1]:   2 (64bit memory decoder)
 *        [3]:     1 (Prefetchable)
 *        [26:4]:  RAZ (to imply 128MB space)
 *        [31:27]: RW (User may define base address)
 *
 * - Access using the CVMX_PCI_CFG06 CSR
 */
typedef union
{
    uint32_t u32;
    struct
    {
#if __BYTE_ORDER == __BIG_ENDIAN
        uint64_t lbase                   : 5;       /**< Base Address[31:27] */
        uint64_t lbasez                  : 23;      /**< Base Address[26:4] (Read as Zero) */
        uint64_t pf                      : 1;       /**< Prefetchable Space */
        uint64_t typ                     : 2;       /**< Type (00=32b/01=below 1MB/10=64b/11=RSV) */
        uint64_t mspc                    : 1;       /**< Memory Space Indicator */
#else
        uint64_t mspc                    : 1;
        uint64_t typ                     : 2;
        uint64_t pf                      : 1;
        uint64_t lbasez                  : 23;
        uint64_t lbase                   : 5;
#endif
    } s;
} cvmx_pci_cfg06_t;


/**
 * PCI_CFG07 = Eighth 32-bits of PCI config space (Base Address Register 1 - High)
 *
 * - Access using the CVMX_PCI_CFG07 CSR
 */
typedef union
{
    uint32_t u32;
    struct
    {
#if __BYTE_ORDER == __BIG_ENDIAN
        uint64_t hbase                   : 32;      /**< Base Address[63:32] */
#else
        uint64_t hbase                   : 32;
#endif
    } s;
} cvmx_pci_cfg07_t;


/**
 * PCI_CFG08 = Ninth 32-bits of PCI config space (Base Address Register 2 - Low)
 *
 *  Description: BAR1: 2^39 (512GB) 64-bit Prefetchable Memory Space
 *        [0]:     0 (Memory Space)
 *        [2:1]:   2 (64bit memory decoder)
 *        [3]:     1 (Prefetchable)
 *        [31:4]:  RAZ
 *
 * - Access using the CVMX_PCI_CFG08 CSR
 */
typedef union
{
    uint32_t u32;
    struct
    {
#if __BYTE_ORDER == __BIG_ENDIAN
        uint64_t lbasez                  : 28;      /**< Base Address[31:4] (Read as Zero) */
        uint64_t pf                      : 1;       /**< Prefetchable Space */
        uint64_t typ                     : 2;       /**< Type (00=32b/01=below 1MB/10=64b/11=RSV) */
        uint64_t mspc                    : 1;       /**< Memory Space Indicator */
#else
        uint64_t mspc                    : 1;
        uint64_t typ                     : 2;
        uint64_t pf                      : 1;
        uint64_t lbasez                  : 28;
#endif
    } s;
} cvmx_pci_cfg08_t;


/**
 * PCI_CFG09 = Tenth 32-bits of PCI config space (Base Address Register 2 - High)
 *
 * - Access using the CVMX_PCI_CFG09 CSR
 */
typedef union
{
    uint32_t u32;
    struct
    {
#if __BYTE_ORDER == __BIG_ENDIAN
        uint64_t hbase                   : 25;      /**< Base Address[63:39] */
        uint64_t hbasez                  : 7;       /**< Base Address[38:31] (Read as Zero) */
#else
        uint64_t hbasez                  : 7;
        uint64_t hbase                   : 25;
#endif
    } s;
} cvmx_pci_cfg09_t;


/**
 * PCI_CFG10 = Eleventh 32-bits of PCI config space (Card Bus CIS Pointer)
 *
 * - Access using the CVMX_PCI_CFG10 CSR
 */
typedef union
{
    uint32_t u32;
    struct
    {
#if __BYTE_ORDER == __BIG_ENDIAN
        uint64_t cisp                    : 32;      /**< CardBus CIS Pointer (UNUSED) */
#else
        uint64_t cisp                    : 32;
#endif
    } s;
} cvmx_pci_cfg10_t;


/**
 * PCI_CFG11 = Twelfth 32-bits of PCI config space (SubSystem ID/Subsystem Vendor ID Register)
 *
 * - Access using the CVMX_PCI_CFG11 CSR
 */
typedef union
{
    uint32_t u32;
    struct
    {
#if __BYTE_ORDER == __BIG_ENDIAN
        uint64_t ssid                    : 16;      /**< SubSystem ID */
        uint64_t ssvid                   : 16;      /**< Subsystem Vendor ID */
#else
        uint64_t ssvid                   : 16;
        uint64_t ssid                    : 16;
#endif
    } s;
} cvmx_pci_cfg11_t;


/**
 * PCI_CFG12 = Thirteenth 32-bits of PCI config space (Expansion ROM Base Address Register)
 *
 * - Access using the CVMX_PCI_CFG12 CSR
 */
typedef union
{
    uint32_t u32;
    struct
    {
#if __BYTE_ORDER == __BIG_ENDIAN
        uint64_t erbar                   : 16;      /**< Expansion ROM Base Address[31:16] 64KB in size */
        uint64_t erbarz                  : 5;       /**< Expansion ROM Base Base Address (Read as Zero) */
        uint64_t reserved                : 10;      /**< Reserved */
        uint64_t erbar_en                : 1;       /**< Expansion ROM Address Decode Enable */
#else
        uint64_t erbar_en                : 1;
        uint64_t reserved                : 10;
        uint64_t erbarz                  : 5;
        uint64_t erbar                   : 16;
#endif
    } s;
} cvmx_pci_cfg12_t;


/**
 * PCI_CFG13 = Fourteenth 32-bits of PCI config space (Capabilities Pointer Register)
 *
 * - Access using the CVMX_PCI_CFG13 CSR
 */
typedef union
{
    uint32_t u32;
    struct
    {
#if __BYTE_ORDER == __BIG_ENDIAN
        uint64_t reserved                : 24;      /**< Reserved */
        uint64_t cp                      : 8;       /**< Capabilities Pointer */
#else
        uint64_t cp                      : 8;
        uint64_t reserved                : 24;
#endif
    } s;
} cvmx_pci_cfg13_t;


/**
 * PCI_CFG15 = Sixteenth 32-bits of PCI config space (INT/ARB/LATENCY Register)
 *
 * - Access using the CVMX_PCI_CFG15 CSR
 */
typedef union
{
    uint32_t u32;
    struct
    {
#if __BYTE_ORDER == __BIG_ENDIAN
        uint64_t ml                      : 8;       /**< Maximum Latency */
        uint64_t mg                      : 8;       /**< Minimum Grant */
        uint64_t inta                    : 8;       /**< Interrupt Pin (INTA#) */
        uint64_t il                      : 8;       /**< Interrupt Line */
#else
        uint64_t il                      : 8;
        uint64_t inta                    : 8;
        uint64_t mg                      : 8;
        uint64_t ml                      : 8;
#endif
    } s;
} cvmx_pci_cfg15_t;


/**
 * PCI_CFG16 = Seventeenth 32-bits of PCI config space (Target Implementation Register)
 *
 * - Access using the CVMX_PCI_CFG16 CSR
 */
typedef union
{
    uint32_t u32;
    struct
    {
#if __BYTE_ORDER == __BIG_ENDIAN
        uint64_t trdnpr                  : 1;       /**< Target Read Delayed Transaction for I/O and
                                                         non-prefetchable regions discarded. */
        uint64_t trdard                  : 1;       /**< Target Read Delayed Transaction for all regions
                                                         discarded. */
        uint64_t rdsati                  : 1;       /**< Target(I/O and Memory) Read Delayed/Split at
                                                         timeout/immediately (default timeout). */
        uint64_t trdrs                   : 1;       /**< Target(I/O and Memory) Read Delayed/Split or Retry
                                                         select (of the application interface is not ready)
                                                         0 = Delayed Split Transaction
                                                         1 = Retry Transaction (always Immediate Retry, no
                                                         AT_REQ to application). */
        uint64_t trtae                   : 1;       /**< Target(I/O and Memory) Read Target Abort Enable
                                                         (if application interface is not ready at the
                                                         latency timeout).
                                                         Note: N3 as target will never target-abort,
                                                         therefore this bit should never be set. */
        uint64_t twsei                   : 1;       /**< Target(I/O) Write Split Enable (at timeout /
                                                         iately; default timeout) */
        uint64_t twsen                   : 1;       /**< T(I/O) write split Enable (if the application
                                                         interface is not ready) */
        uint64_t twtae                   : 1;       /**< Target(I/O and Memory) Write Target Abort Enable
                                                         (if the application interface is not ready at the
                                                         start of the cycle).
                                                         Note: N3 as target will never target-abort,
                                                         therefore this bit should never be set. */
        uint64_t tmae                    : 1;       /**< Target(Read/Write) Master Abort Enable; check
                                                         at the start of each transaction.
                                                         Note: This bit can be used to force a Master
                                                         Abort when N3 is acting as the intended target
                                                         device. */
        uint64_t tslte                   : 3;       /**< Target Subsequent(2nd-last) Latency Timeout Enable
                                                         Valid range: [1..7] and 0=8. */
        uint64_t tilt                    : 4;       /**< Target Initial(1st data) Latency Timeout in PCI
                                                         ModeValid range: [8..15] and 0=16. */
        uint64_t pbe                     : 12;      /**< Programmable Boundary Enable to disconnect/prefetch
                                                         for target burst read cycles to prefetchable
                                                         region in PCI. A value of 1 indicates end of
                                                         boundary (64 KB down to 16 Bytes). */
        uint64_t dppmr                   : 1;       /**< Disconnect/Prefetch to prefetchable memory
                                                         regions Enable. Prefetchable memory regions
                                                         are always disconnected on a region boundary.
                                                         Non-prefetchable regions for PCI are always
                                                         disconnected on the first transfer.
                                                         Note: N3 as target will never target-disconnect,
                                                         therefore this bit should never be set. */
        uint64_t reserved                : 1;       /**< Reserved */
        uint64_t tswc                    : 1;       /**< Target Split Write Control
                                                         0 = Blocks all requests except PMW
                                                         1 = Blocks all requests including PMW until
                                                         split completion occurs. */
        uint64_t mltd                    : 1;       /**< Master Latency Timer Disable */
#else
        uint64_t mltd                    : 1;
        uint64_t tswc                    : 1;
        uint64_t reserved                : 1;
        uint64_t dppmr                   : 1;
        uint64_t pbe                     : 12;
        uint64_t tilt                    : 4;
        uint64_t tslte                   : 3;
        uint64_t tmae                    : 1;
        uint64_t twtae                   : 1;
        uint64_t twsen                   : 1;
        uint64_t twsei                   : 1;
        uint64_t trtae                   : 1;
        uint64_t trdrs                   : 1;
        uint64_t rdsati                  : 1;
        uint64_t trdard                  : 1;
        uint64_t trdnpr                  : 1;
#endif
    } s;
} cvmx_pci_cfg16_t;


/**
 * PCI_CFG17 = Eighteenth 32-bits of PCI config space (Target Split Completion Message
 *            Enable Register)
 *
 * - Access using the CVMX_PCI_CFG17 CSR
 */
typedef union
{
    uint32_t u32;
    struct
    {
#if __BYTE_ORDER == __BIG_ENDIAN
        uint64_t tscme                   : 32;      /**< Target Split Completion Message Enable */
#else
        uint64_t tscme                   : 32;
#endif
    } s;
} cvmx_pci_cfg17_t;


/**
 * PCI_CFG18 = Nineteenth 32-bits of PCI config space (Target Delayed/Split Request
 *                         Pending Sequences)
 *
 * - Access using the CVMX_PCI_CFG18 CSR
 */
typedef union
{
    uint32_t u32;
    struct
    {
#if __BYTE_ORDER == __BIG_ENDIAN
        uint64_t tdsrps                  : 32;      /**< Target Delayed/Split Request Pending Sequences */
#else
        uint64_t tdsrps                  : 32;
#endif
    } s;
} cvmx_pci_cfg18_t;


/**
 * PCI_CFG19 = Twentieth 32-bits of PCI config space (Master/Target Implementation Register)
 *
 * - Access using the CVMX_PCI_CFG19 CSR
 */
typedef union
{
    uint32_t u32;
    struct
    {
#if __BYTE_ORDER == __BIG_ENDIAN
        uint64_t mrbcm                   : 1;       /**< Master Request (Memory Read) Byte Count/Byte
                                                         Enable select.
                                                         0 = Byte Enables valid. In PCI mode, a burst
                                                         transaction cannot be performed using
                                                         Memory Read command=4?h6.
                                                         1 = DWORD Byte Count valid (default). In PCI
                                                         Mode, the memory read byte enables are
                                                         automatically generated by the core.
                                                         Note: N3 Master Request transaction sizes are
                                                         always determined through the
                                                         am_attr[<35:32>|<7:0>] field. */
        uint64_t mrbci                   : 1;       /**< Master Request (I/O and CR cycles) byte count/byte
                                                         enable select.
                                                         0 = Byte Enables valid (default)
                                                         1 = DWORD byte count valid
                                                         Note: For N3K, This bit must always be zero
                                                         for proper operation (in support of
                                                         Type0/1 Cfg Space accesses which require byte
                                                         enable generation directly from a read mask). */
        uint64_t mdwe                    : 1;       /**< Master (Retry) Deferred Write Enable (allow
                                                         read requests to pass).
                                                         NOTE: Applicable to PCI Mode I/O and memory
                                                         transactions only.
                                                         0 = New read requests are NOT accepted until
                                                         the current write cycle completes. [Reads
                                                         cannot pass writes]
                                                         1 = New read requests are accepted, even when
                                                         there is a write cycle pending [Reads can
                                                         pass writes]. */
        uint64_t mdre                    : 1;       /**< Master (Retry) Deferred Read Enable (Allows
                                                         read/write requests to pass).
                                                         NOTE: Applicable to PCI mode I/O and memory
                                                         transactions only.
                                                         0 = New read/write requests are NOT accepted
                                                         until the current read cycle completes.
                                                         [Read/write requests CANNOT pass reads]
                                                         1 = New read/write requests are accepted, even
                                                         when there is a read cycle pending.
                                                         [Read/write requests CAN pass reads] */
        uint64_t mdrimc                  : 1;       /**< Master I/O Deferred/Split Request Outstanding
                                                         Maximum Count
                                                         0 = 4Ch[26:24]
                                                         1 = 1 */
        uint64_t mdrrmc                  : 3;       /**< Master Deferred Read Request Outstanding Max
                                                         Count (PCI only).
                                                         CR4C[26:24]  Max SAC cycles   MAX DAC cycles
                                                         000              8                4
                                                         001              1                0
                                                         010              2                1
                                                         011              3                1
                                                         100              4                2
                                                         101              5                2
                                                         110              6                3
                                                         111              7                3
                                                         For example, if these bits are programmed to
                                                         100, the core can support 2 DAC cycles, 4 SAC
                                                         cycles or a combination of 1 DAC and 2 SAC cycles.
                                                         NOTE: For the PCI-X maximum outstanding split
                                                         transactions, refer to CRE0[22:20] */
        uint64_t tmes                    : 8;       /**< Target/Master Error Sequence # */
        uint64_t teci                    : 1;       /**< Target Error Command Indication
                                                         0 = Delayed/Split
                                                         1 = Others */
        uint64_t tmei                    : 1;       /**< Target/Master Error Indication
                                                         0 = Target
                                                         1 = Master */
        uint64_t tmse                    : 1;       /**< Target/Master System Error. This bit is set
                                                         whenever ATM_SERR_O is active. */
        uint64_t tmdpes                  : 1;       /**< Target/Master Data PERR# error status. This
                                                         bit is set whenever ATM_DATA_PERR_O is active. */
        uint64_t tmapes                  : 1;       /**< Target/Master Address PERR# error status. This
                                                         bit is set whenever ATM_ADDR_PERR_O is active. */
        uint64_t reserved                : 2;       /**< Reserved */
        uint64_t tibcd                   : 1;       /**< Target Illegal I/O DWORD byte combinations detected. */
        uint64_t tibde                   : 1;       /**< Target Illegal I/O DWORD byte detection enable */
        uint64_t reserved1               : 1;       /**< Reserved */
        uint64_t tidomc                  : 1;       /**< Target I/O Delayed/Split request outstanding
                                                         maximum count.
                                                         0 = 4Ch[4:0],
                                                         1 = 1 */
        uint64_t tdomc                   : 5;       /**< Target Delayed/Split request outstanding maximum
                                                         count. [1..31] and 0=32.
                                                         NOTE: If the user programs these bits beyond the
                                                         Designed Maximum outstanding count, then the
                                                         designed maximum table depth will be used instead.
                                                         No additional Deferred/Split transactions will be
                                                         accepted if this outstanding maximum count
                                                         is reached. Furthermore, no additional
                                                         deferred/split transactions will be accepted if
                                                         the I/O delay/ I/O Split Request outstanding
                                                         maximum is reached. */
#else
        uint64_t tdomc                   : 5;
        uint64_t tidomc                  : 1;
        uint64_t reserved1               : 1;
        uint64_t tibde                   : 1;
        uint64_t tibcd                   : 1;
        uint64_t reserved                : 2;
        uint64_t tmapes                  : 1;
        uint64_t tmdpes                  : 1;
        uint64_t tmse                    : 1;
        uint64_t tmei                    : 1;
        uint64_t teci                    : 1;
        uint64_t tmes                    : 8;
        uint64_t mdrrmc                  : 3;
        uint64_t mdrimc                  : 1;
        uint64_t mdre                    : 1;
        uint64_t mdwe                    : 1;
        uint64_t mrbci                   : 1;
        uint64_t mrbcm                   : 1;
#endif
    } s;
} cvmx_pci_cfg19_t;


/**
 * PCI_CFG20 = Twenty-first 32-bits of PCI config space (Master Deferred/Split Sequence Pending)
 *
 * - Access using the CVMX_PCI_CFG20 CSR
 */
typedef union
{
    uint32_t u32;
    struct
    {
#if __BYTE_ORDER == __BIG_ENDIAN
        uint64_t mdsp                    : 32;      /**< Master Deferred/Split sequence Pending */
#else
        uint64_t mdsp                    : 32;
#endif
    } s;
} cvmx_pci_cfg20_t;


/**
 * PCI_CFG21 = Twenty-second 32-bits of PCI config space (Master Split Completion Message Register)
 *
 * - Access using the CVMX_PCI_CFG21 CSR
 */
typedef union
{
    uint32_t u32;
    struct
    {
#if __BYTE_ORDER == __BIG_ENDIAN
        uint64_t scmre                   : 32;      /**< Master Split Completion message received with
                                                         error message. */
#else
        uint64_t scmre                   : 32;
#endif
    } s;
} cvmx_pci_cfg21_t;


/**
 * PCI_CFG22 = Twenty-third 32-bits of PCI config space (Master Arbiter Control Register)
 *
 * - Access using the CVMX_PCI_CFG22 CSR
 */
typedef union
{
    uint32_t u32;
    struct
    {
#if __BYTE_ORDER == __BIG_ENDIAN
        uint64_t mac                     : 7;       /**< Master Arbiter Control
                                                         [31:26]: Used only in Fixed Priority mode
                                                         (when [25]=1)
                                                         [31:30]: MSI Request
                                                         00 = Highest Priority
                                                         01 = Medium Priority
                                                         10 = Lowest Priority
                                                         11 = RESERVED
                                                         [29:28]: Target Split Completion
                                                         00 = Highest Priority
                                                         01 = Medium Priority
                                                         10 = Lowest Priority
                                                         11 = RESERVED
                                                         [27:26]: New Request; Deferred Read,Deferred Write
                                                         00 = Highest Priority
                                                         01 = Medium Priority
                                                         10 = Lowest Priority
                                                         11 = RESERVED
                                                         [25]: Fixed/Round Robin Priority Selector
                                                         0 = Round Robin
                                                         1 = Fixed */
        uint64_t reserved                : 6;       /**< Reserved */
        uint64_t flush                   : 1;       /**< AM_DO_FLUSH_I control
                                                         NOTE: This bit MUST BE ONE for proper N3K operation */
        uint64_t mra                     : 1;       /**< Master Retry Aborted */
        uint64_t mtta                    : 1;       /**< Master TRDY timeout aborted */
        uint64_t mrv                     : 8;       /**< Master Retry Value [1..255] and 0=infinite */
        uint64_t mttv                    : 8;       /**< Master TRDY timeout value [1..255] and 0=disabled
                                                         Note: N3 does not support master TRDY timeout
                                                         (target is expected to be well behaved). */
#else
        uint64_t mttv                    : 8;
        uint64_t mrv                     : 8;
        uint64_t mtta                    : 1;
        uint64_t mra                     : 1;
        uint64_t flush                   : 1;
        uint64_t reserved                : 6;
        uint64_t mac                     : 7;
#endif
    } s;
} cvmx_pci_cfg22_t;


/**
 * PCI_CFG56 = Fifty-seventh 32-bits of PCI config space (PCIX Capabilities Register)
 *
 * - Access using the CVMX_PCI_CFG56 CSR
 */
typedef union
{
    uint32_t u32;
    struct
    {
#if __BYTE_ORDER == __BIG_ENDIAN
        uint64_t reserved                : 9;       /**< RESERVED */
        uint64_t most                    : 3;       /**< Maximum outstanding Split transactions
                                                         [000b=1..111b=32] */
        uint64_t mmbc                    : 2;       /**< Maximum Memory Byte Count
                                                         [0=512B,1=1024B,2=2048B,3=4096B] */
        uint64_t roe                     : 1;       /**< Relaxed Ordering Enable */
        uint64_t dpere                   : 1;       /**< Data Parity Error Recovery Enable */
        uint64_t ncp                     : 8;       /**< Next Capability Pointer */
        uint64_t pxcid                   : 8;       /**< PCI-X Capability ID */
#else
        uint64_t pxcid                   : 8;
        uint64_t ncp                     : 8;
        uint64_t dpere                   : 1;
        uint64_t roe                     : 1;
        uint64_t mmbc                    : 2;
        uint64_t most                    : 3;
        uint64_t reserved                : 9;
#endif
    } s;
} cvmx_pci_cfg56_t;


/**
 * PCI_CFG57 = Fifty-eigth 32-bits of PCI config space (PCIX Status Register)
 *
 * - Access using the CVMX_PCI_CFG57 CSR
 */
typedef union
{
    uint32_t u32;
    struct
    {
#if __BYTE_ORDER == __BIG_ENDIAN
        uint64_t reserved                : 2;       /**< RESERVED */
        uint64_t scemr                   : 1;       /**< Split Completion Error Message Received */
        uint64_t mcrsd                   : 3;       /**< Maximum Cumulative Read Size designed */
        uint64_t mostd                   : 3;       /**< Maximum Outstanding Split transaction designed */
        uint64_t mmrbcd                  : 2;       /**< Maximum Memory Read byte count designed */
        uint64_t dc                      : 1;       /**< Device Complexity
                                                         0 = Simple Device
                                                         1 = Bridge Device */
        uint64_t usc                     : 1;       /**< Unexpected Split Completion */
        uint64_t scd                     : 1;       /**< Split Completion Discarded */
        uint64_t m133                    : 1;       /**< 133MHz Capable */
        uint64_t w64                     : 1;       /**< Indicates a 32b(=0) or 64b(=1) device */
        uint64_t bn                      : 8;       /**< Bus Number. Updated on all configuration write
                                                         (0x11=PCI)             cycles. It?s value is dependent upon the PCI/X
                                                         (0xFF=PCIX)            mode. */
        uint64_t dn                      : 5;       /**< Device Number. Updated on all configuration
                                                         write cycles. */
        uint64_t fn                      : 3;       /**< Function Number */
#else
        uint64_t fn                      : 3;
        uint64_t dn                      : 5;
        uint64_t bn                      : 8;
        uint64_t w64                     : 1;
        uint64_t m133                    : 1;
        uint64_t scd                     : 1;
        uint64_t usc                     : 1;
        uint64_t dc                      : 1;
        uint64_t mmrbcd                  : 2;
        uint64_t mostd                   : 3;
        uint64_t mcrsd                   : 3;
        uint64_t scemr                   : 1;
        uint64_t reserved                : 2;
#endif
    } s;
} cvmx_pci_cfg57_t;


/**
 * PCI_CFG58 = Fifty-ninth 32-bits of PCI config space (Power Management Capabilities Register)
 *
 * - Access using the CVMX_PCI_CFG58 CSR
 */
typedef union
{
    uint32_t u32;
    struct
    {
#if __BYTE_ORDER == __BIG_ENDIAN
        uint64_t pmes                    : 5;       /**< PME Support (D0 to D3cold) */
        uint64_t d2s                     : 1;       /**< D2_Support */
        uint64_t d1s                     : 1;       /**< D1_Support */
        uint64_t auxc                    : 3;       /**< AUX_Current (0..375mA) */
        uint64_t dsi                     : 1;       /**< Device Specific Initialization */
        uint64_t reserved                : 1;       /**< Reserved */
        uint64_t pmec                    : 1;       /**< PME Clock */
        uint64_t pcimiv                  : 3;       /**< Indicates the version of the PCI
                                                         Management
                                                         Interface Specification with which the core
                                                         complies.
                                                         010b = Complies with PCI Management Interface
                                                         Specification Revision 1.1 */
        uint64_t ncp                     : 8;       /**< Next Capability Pointer */
        uint64_t pmcid                   : 8;       /**< Power Management Capability ID */
#else
        uint64_t pmcid                   : 8;
        uint64_t ncp                     : 8;
        uint64_t pcimiv                  : 3;
        uint64_t pmec                    : 1;
        uint64_t reserved                : 1;
        uint64_t dsi                     : 1;
        uint64_t auxc                    : 3;
        uint64_t d1s                     : 1;
        uint64_t d2s                     : 1;
        uint64_t pmes                    : 5;
#endif
    } s;
} cvmx_pci_cfg58_t;


/**
 * PCI_CFG59 = Sixtieth 32-bits of PCI config space (Power Management Data/PMCSR Register(s))
 *
 * - Access using the CVMX_PCI_CFG59 CSR
 */
typedef union
{
    uint32_t u32;
    struct
    {
#if __BYTE_ORDER == __BIG_ENDIAN
        uint64_t pmdia                   : 8;       /**< Power Management data input from application
                                                         (PME_DATA) */
        uint64_t bpccen                  : 1;       /**< BPCC_En (bus power/clock control) enable */
        uint64_t bd3h                    : 1;       /**< B2_B3#, B2/B3 Support for D3hot */
        uint64_t reserved                : 6;       /**< Reserved */
        uint64_t pmess                   : 1;       /**< PME_Status sticky bit */
        uint64_t pmedsia                 : 2;       /**< PME_Data_Scale input from application
                                                         Device                  (PME_DATA_SCALE[1:0])
                                                         Specific */
        uint64_t pmds                    : 4;       /**< Power Management Data_select */
        uint64_t pmeens                  : 1;       /**< PME_En sticky bit */
        uint64_t reserved1               : 6;       /**< RESERVED */
        uint64_t ps                      : 2;       /**< Power State (D0 to D3)
                                                         The N2 DOES NOT support D1/D2 Power Management
                                                         states, therefore writing to this register has
                                                         no effect (please refer to the PCI Power
                                                         Management
                                                         Specification v1.1 for further details about
                                                         it?s R/W nature. This is not a conventional
                                                         R/W style register. */
#else
        uint64_t ps                      : 2;
        uint64_t reserved1               : 6;
        uint64_t pmeens                  : 1;
        uint64_t pmds                    : 4;
        uint64_t pmedsia                 : 2;
        uint64_t pmess                   : 1;
        uint64_t reserved                : 6;
        uint64_t bd3h                    : 1;
        uint64_t bpccen                  : 1;
        uint64_t pmdia                   : 8;
#endif
    } s;
} cvmx_pci_cfg59_t;


/**
 * PCI_CFG60 = Sixty-first 32-bits of PCI config space (MSI Capabilities Register)
 *
 * - Access using the CVMX_PCI_CFG60 CSR
 */
typedef union
{
    uint32_t u32;
    struct
    {
#if __BYTE_ORDER == __BIG_ENDIAN
        uint64_t reserved                : 8;       /**< RESERVED */
        uint64_t m64                     : 1;       /**< 32/64 b message */
        uint64_t mme                     : 3;       /**< Multiple Message Enable(1,2,4,8,16,32) */
        uint64_t mmc                     : 3;       /**< Multiple Message Capable(0=1,1=2,2=4,3=8,4=16,5=32) */
        uint64_t msien                   : 1;       /**< MSI Enable */
        uint64_t ncp                     : 8;       /**< Next Capability Pointer */
        uint64_t msicid                  : 8;       /**< MSI Capability ID */
#else
        uint64_t msicid                  : 8;
        uint64_t ncp                     : 8;
        uint64_t msien                   : 1;
        uint64_t mmc                     : 3;
        uint64_t mme                     : 3;
        uint64_t m64                     : 1;
        uint64_t reserved                : 8;
#endif
    } s;
} cvmx_pci_cfg60_t;


/**
 * PCI_CFG61 = Sixty-second 32-bits of PCI config space (MSI Lower Address Register)
 *
 * - Access using the CVMX_PCI_CFG61 CSR
 */
typedef union
{
    uint32_t u32;
    struct
    {
#if __BYTE_ORDER == __BIG_ENDIAN
        uint64_t msi31t2                 : 30;      /**< App Specific MSI Address [31:2] */
        uint64_t reserved                : 2;       /**< RESERVED */
#else
        uint64_t reserved                : 2;
        uint64_t msi31t2                 : 30;
#endif
    } s;
} cvmx_pci_cfg61_t;


/**
 * PCI_CFG62 = Sixty-third 32-bits of PCI config space (MSI Upper Address Register)
 *
 * - Access using the CVMX_PCI_CFG62 CSR
 */
typedef union
{
    uint32_t u32;
    struct
    {
#if __BYTE_ORDER == __BIG_ENDIAN
        uint64_t msi                     : 32;      /**< MSI Address [63:32] */
#else
        uint64_t msi                     : 32;
#endif
    } s;
} cvmx_pci_cfg62_t;


/**
 * PCI_CFG63 = Sixty-fourth 32-bits of PCI config space (MSI Message Data Register)
 *
 * - Access using the CVMX_PCI_CFG63 CSR
 */
typedef union
{
    uint32_t u32;
    struct
    {
#if __BYTE_ORDER == __BIG_ENDIAN
        uint64_t reserved                : 16;      /**< RESERVED */
        uint64_t msimd                   : 16;      /**< MSI Message Data */
#else
        uint64_t msimd                   : 16;
        uint64_t reserved                : 16;
#endif
    } s;
} cvmx_pci_cfg63_t;



/**
 * cvmx_pci_ctl_status_2
 *
 * PCI_CTL_STATUS_2 = PCI Control Status 2 Register
 * 
 * Control status register accessable from both PCI and NCB.
 */
typedef union
{
    uint32_t u32;
    struct cvmx_pci_ctl_status_2_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN
        uint32_t reserved_29_31          : 3;
        uint32_t bb1_hole                : 3;       /**< Big BAR 1 Hole
                                                         NOT IN PASS 1 NOR PASS 2
                                                         When PCI_CTL_STATUS_2[BB1]=1, this field defines
                                                         an encoded size of the upper BAR1 region which
                                                         OCTEON will mask out (ie: not respond to).
                                                         (see definition of BB1_HOLE and BB1_SIZ encodings
                                                         in the PCI_CTL_STATUS_2[BB1] definition below). */
        uint32_t bb1_siz                 : 1;       /**< Big BAR 1 Size
                                                         NOT IN PASS 1 NOR PASS 2
                                                         When PCI_CTL_STATUS_2[BB1]=1, this field defines
                                                         the programmable SIZE of BAR 1.
                                                            0: 1GB / 1: 2GB */
        uint32_t bb_ca                   : 1;       /**< Set to '1' for Big Bar Mode to do STT/LDT L2C
                                                         operations.
                                                         NOT IN PASS 1 NOR PASS 2 */
        uint32_t bb_es                   : 2;       /**< Big Bar Node Endian Swap Mode
                                                            0: No Swizzle
                                                            1: Byte Swizzle (per-QW)
                                                            2: Byte Swizzle (per-LW)
                                                            3: LongWord Swizzle
                                                         NOT IN PASS 1 NOR PASS 2 */
        uint32_t bb1                     : 1;       /**< Big Bar 1 Enable
                                                         NOT IN PASS 1 NOR PASS 2
                                                         When PCI_CTL_STATUS_2[BB1] is set, the following differences
                                                         occur:
                                                         - OCTEON's BAR1 becomes somewhere in the range 512-2048 MB rather
                                                           than the default 128MB.
                                                         - The following table indicates the effective size of
                                                           BAR1 when BB1 is set:
                                                             BB1_SIZ   BB1_HOLE  Effective size    Comment
                                                           +++++++++++++++++++++++++++++++++++++++++++++++++++++++++
                                                                0          0         1024 MB      Normal 1GB BAR
                                                                0          1         1008 MB      1 GB, 16 MB hole
                                                                0          2          992 MB      1 GB, 32 MB hole
                                                                0          3          960 MB      1 GB, 64 MB hole
                                                                0          4          896 MB      1 GB,128 MB hole
                                                                0          5          768 MB      1 GB,256 MB hole
                                                                0          6          512 MB      1 GB,512 MB hole
                                                                0          7         Illegal
                                                                1          0         2048 MB      Normal 2GB BAR
                                                                1          1         2032 MB      2 GB, 16 MB hole
                                                                1          2         2016 MB      2 GB, 32 MB hole
                                                                1          3         1984 MB      2 GB, 64 MB hole
                                                                1          4         1920 MB      2 GB,128 MB hole
                                                                1          5         1792 MB      2 GB,256 MB hole
                                                                1          6         1536 MB      2 GB,512 MB hole
                                                                1          7         Illegal
                                                         - When BB1_SIZ is 0: PCI_CFG06[LBASE<2:0>] reads as zero
                                                           and are ignored on write. BAR1 is an entirely ordinary
                                                           1 GB (power-of-two) BAR in all aspects when BB1_HOLE is 0.
                                                           When BB1_HOLE is not zero, BAR1 addresses are programmed
                                                           as if the BAR were 1GB, but, OCTEON does not respond
                                                           to addresses in the programmed holes.
                                                         - When BB1_SIZ is 1: PCI_CFG06[LBASE<3:0>] reads as zero
                                                           and are ignored on write. BAR1 is an entirely ordinary
                                                           2 GB (power-of-two) BAR in all aspects when BB1_HOLE is 0.
                                                           When BB1_HOLE is not zero, BAR1 addresses are programmed
                                                           as if the BAR were 2GB, but, OCTEON does not respond
                                                           to addresses in the programmed holes.
                                                         - Note that the BB1_HOLE value has no effect on the
                                                           PCI_CFG06[LBASE] behavior. BB1_HOLE only affects whether
                                                           OCTEON accepts an address. BB1_SIZ does affect PCI_CFG06[LBASE]
                                                           behavior, however.
                                                         - The first 128MB, i.e. addresses on the PCI bus in the range
                                                             BAR1+0          .. BAR1+0x07FFFFFF
                                                           access OCTEON's DRAM addresses with PCI_BAR1_INDEX CSR's
                                                           as before
                                                         - The remaining address space, i.e. addresses
                                                           on the PCI bus in the range
                                                              BAR1+0x08000000 .. BAR1+size-1,
                                                           where size is the size of BAR1 as selected by the above
                                                           table (based on the BB1_SIZ and BB1_HOLE values), are mapped to
                                                           OCTEON physical DRAM addresses as follows:
                                                                   PCI Address Range         OCTEON Physical Address Range
                                                           ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
                                                            BAR1+0x08000000 .. BAR1+size-1 | 0x80000000 .. 0x77FFFFFF+size
                                                           and PCI_CTL_STATUS_2[BB_ES] is the endian-swap and
                                                           PCI_CTL_STATUS_2[BB_CA] is the L2 cache allocation bit
                                                           for these references.
                                                           The consequences of any burst that crosses the end of the PCI
                                                           Address Range for BAR1 are unpredicable.
                                                         - The consequences of any burst access that crosses the boundary
                                                           between BAR1+0x07FFFFFF and BAR1+0x08000000 are unpredictable in PCI-X
                                                           mode. OCTEON may disconnect PCI references at this boundary. */
        uint32_t bb0                     : 1;       /**< Big Bar 0 Enable
                                                         NOT IN PASS 1 NOR PASS 2
                                                         When PCI_CTL_STATUS_2[BB0] is set, the following
                                                         differences occur:
                                                         - OCTEON's BAR0 becomes 2GB rather than the default 4KB.
                                                           PCI_CFG04[LBASE<18:0>] reads as zero and is ignored on write.
                                                         - OCTEON's BAR0 becomes burstable. (When BB0 is clear, OCTEON
                                                           single-phase disconnects PCI BAR0 reads and PCI/PCI-X BAR0
                                                           writes, and splits (burstably) PCI-X BAR0 reads.)
                                                         - The first 4KB, i.e. addresses on the PCI bus in the range
                                                               BAR0+0      .. BAR0+0xFFF
                                                           access OCTEON's PCI-type CSR's as when BB0 is clear.
                                                         - The remaining address space, i.e. addresses on the PCI bus
                                                           in the range
                                                               BAR0+0x1000 .. BAR0+0x7FFFFFFF
                                                           are mapped to OCTEON physical DRAM addresses as follows:
                                                              PCI Address Range                  OCTEON Physical Address Range
                                                           ------------------------------------+------------------------------
                                                            BAR0+0x00001000 .. BAR0+0x0FFFFFFF | 0x000001000 .. 0x00FFFFFFF
                                                            BAR0+0x10000000 .. BAR0+0x1FFFFFFF | 0x410000000 .. 0x41FFFFFFF
                                                            BAR0+0x20000000 .. BAR0+0x7FFFFFFF | 0x020000000 .. 0x07FFFFFFF
                                                           and PCI_CTL_STATUS_2[BB_ES] is the endian-swap and
                                                           PCI_CTL_STATUS_2[BB_CA] is the L2 cache allocation bit
                                                           for these references.
                                                           The consequences of any burst that crosses the end of the PCI
                                                           Address Range for BAR0 are unpredicable.
                                                         - The consequences of any burst access that crosses the boundary
                                                           between BAR0+0xFFF and BAR0+0x1000 are unpredictable in PCI-X
                                                           mode. OCTEON may disconnect PCI references at this boundary.
                                                         - The results of any burst read that crosses the boundary
                                                           between BAR0+0x0FFFFFFF and BAR0+0x10000000 are unpredictable.
                                                           The consequences of any burst write that crosses this same
                                                           boundary are unpredictable.
                                                         - The results of any burst read that crosses the boundary
                                                           between BAR0+0x1FFFFFFF and BAR0+0x20000000 are unpredictable.
                                                           The consequences of any burst write that crosses this same
                                                           boundary are unpredictable. */
        uint32_t erst_n                  : 1;       /**< Reset active Low. PASS-2 */
        uint32_t bar2pres                : 1;       /**< From fuse block. When fuse(MIO_FUS_DAT3[BAR2_EN])
                                                         is NOT blown the value of this field is '0' after
                                                         reset and BAR2 is NOT present. When the fuse IS
                                                         blown the value of this field is '1' after reset
                                                         and BAR2 is present. Note that SW can change this
                                                         field after reset. This is a PASS-2 field. */
        uint32_t scmtyp                  : 1;       /**< Split Completion Message CMD Type (0=RD/1=WR)
                                                         When SCM=1, SCMTYP specifies the CMD intent (R/W) */
        uint32_t scm                     : 1;       /**< Split Completion Message Detected (Read or Write) */
        uint32_t en_wfilt                : 1;       /**< When '1' the window-access filter is enabled.
                                                         Unfilter writes are:
                                                         MIO, SubId0
                                                         MIO, SubId7
                                                         NPI, SubId0
                                                         NPI, SubId7
                                                         POW, SubId7
                                                         DFA, SubId7
                                                         IPD, SubId7
                                                         Unfiltered Reads are:
                                                         MIO, SubId0
                                                         MIO, SubId7
                                                         NPI, SubId0
                                                         NPI, SubId7
                                                         POW, SubId1
                                                         POW, SubId2
                                                         POW, SubId3
                                                         POW, SubId7
                                                         DFA, SubId7
                                                         IPD, SubId7 */
        uint32_t reserved_14_14          : 1;
        uint32_t ap_pcix                 : 1;       /**< PCX Core Mode status (0=PCI Bus/1=PCIX)
                                                         If one or more of PCI_DEVSEL_N, PCI_STOP_N, and
                                                         PCI_TRDY_N are asserted at the rising edge of
                                                         PCI_RST_N, the device enters PCI-X mode.
                                                         Otherwise, the device enters conventional PCI
                                                         mode at the rising edge of RST#. */
        uint32_t ap_64ad                 : 1;       /**< PCX Core Bus status (0=32b Bus/1=64b Bus)
                                                         When PCI_RST_N pin is de-asserted, the state
                                                         of PCI_REQ64_N(driven by central agent) determines
                                                         the width of the PCI/X bus. */
        uint32_t b12_bist                : 1;       /**< Bist Status For Memeory In B12 */
        uint32_t pmo_amod                : 1;       /**< PMO-ARB Mode (0=FP[HP=CMD1,LP=CMD0]/1=RR) */
        uint32_t pmo_fpc                 : 3;       /**< PMO-ARB Fixed Priority Counter
                                                         When PMO_AMOD=0 (FP mode), this field represents
                                                         the # of CMD1 requests that are issued (at higher
                                                         priority) before a single lower priority CMD0
                                                         is allowed to issue (to ensure foward progress).
                                                            0: 1 CMD1 Request issued before CMD0 allowed
                                                           ...
                                                            7: 8 CMD1 Requests issued before CMD0 allowed */
        uint32_t tsr_hwm                 : 3;       /**< Target Split-Read ADB(allowable disconnect boundary)
                                                         High Water Mark.
                                                         Specifies the number of ADBs(128 Byte aligned chunks)
                                                         that are accumulated(pending) BEFORE the Target Split
                                                         completion is attempted on the PCI bus.
                                                             0: RESERVED/ILLEGAL
                                                             1: 2 Pending ADBs (129B-256B)
                                                             2: 3 Pending ADBs (257B-384B)
                                                             3: 4 Pending ADBs (385B-512B)
                                                             4: 5 Pending ADBs (513B-640B)
                                                             5: 6 Pending ADBs (641B-768B)
                                                             6: 7 Pending ADBs (769B-896B)
                                                             7: 8 Pending ADBs (897B-1024B)
                                                         Example: Suppose a 1KB target memory request with
                                                         starting byte offset address[6:0]=0x7F is split by
                                                         the OCTEON and the TSR_HWM=1(2 ADBs).
                                                         The OCTEON will start the target split completion
                                                         on the PCI Bus after 1B(1st ADB)+128B(2nd ADB)=129B
                                                         of data have been received from memory (even though
                                                         the remaining 895B has not yet been received). The
                                                         OCTEON will continue the split completion until it
                                                         has consumed all of the pended split data. If the
                                                         full transaction length(1KB) of data was NOT entirely
                                                         transferred, then OCTEON will terminate the split
                                                         completion and again wait for another 2 ADB-aligned data
                                                         chunks(256B) of pended split data to be received from
                                                         memory before starting another split completion request.
                                                         This allows Octeon (as split completer), to send back
                                                         multiple split completions for a given large split
                                                         transaction without having to wait for the entire
                                                         transaction length to be received from memory.
                                                         NOTE: For split transaction sizes 'smaller' than the
                                                         specified TSR_HWM value, the split completion
                                                         is started when the last datum has been received from
                                                         memory.
                                                         NOTE: It is IMPERATIVE that this field NEVER BE
                                                         written to a ZERO value. A value of zero is
                                                         reserved/illegal and can result in PCIX bus hangs). */
        uint32_t bar2_enb                : 1;       /**< When set '1' BAR2 is enable and will respond when
                                                         clear '0' BAR2 access will be target-aborted. */
        uint32_t bar2_esx                : 2;       /**< Value will be XORed with pci-address[37:36] to
                                                         determine the endian swap mode. */
        uint32_t bar2_cax                : 1;       /**< Value will be XORed with pci-address[38] to
                                                         determine the L2 cache attribute. */
#else
        uint32_t bar2_cax                : 1;
        uint32_t bar2_esx                : 2;
        uint32_t bar2_enb                : 1;
        uint32_t tsr_hwm                 : 3;
        uint32_t pmo_fpc                 : 3;
        uint32_t pmo_amod                : 1;
        uint32_t b12_bist                : 1;
        uint32_t ap_64ad                 : 1;
        uint32_t ap_pcix                 : 1;
        uint32_t reserved_14_14          : 1;
        uint32_t en_wfilt                : 1;
        uint32_t scm                     : 1;
        uint32_t scmtyp                  : 1;
        uint32_t bar2pres                : 1;
        uint32_t erst_n                  : 1;
        uint32_t bb0                     : 1;
        uint32_t bb1                     : 1;
        uint32_t bb_es                   : 2;
        uint32_t bb_ca                   : 1;
        uint32_t bb1_siz                 : 1;
        uint32_t bb1_hole                : 3;
        uint32_t reserved_29_31          : 3;
#endif
    } s;
    struct cvmx_pci_ctl_status_2_cn3020
    {
#if __BYTE_ORDER == __BIG_ENDIAN
        uint32_t reserved_20_31          : 12;
        uint32_t erst_n                  : 1;       /**< Reset active Low. */
        uint32_t bar2pres                : 1;       /**< From fuse block. When fuse(MIO_FUS_DAT3[BAR2_EN])
                                                         is NOT blown the value of this field is '0' after
                                                         reset and BAR2 is NOT present. When the fuse IS
                                                         blown the value of this field is '1' after reset
                                                         and BAR2 is present. Note that SW can change this
                                                         field after reset. */
        uint32_t scmtyp                  : 1;       /**< Split Completion Message CMD Type (0=RD/1=WR)
                                                         When SCM=1, SCMTYP specifies the CMD intent (R/W) */
        uint32_t scm                     : 1;       /**< Split Completion Message Detected (Read or Write) */
        uint32_t en_wfilt                : 1;       /**< When '1' the window-access filter is enabled.
                                                         Unfilter writes are:
                                                         MIO,  SubId0
                                                         MIO,  SubId7
                                                         NPI,  SubId0
                                                         NPI,  SubId7
                                                         POW,  SubId7
                                                         DFA,  SubId7
                                                         IPD,  SubId7
                                                         USBN, SubId7
                                                         Unfiltered Reads are:
                                                         MIO,  SubId0
                                                         MIO,  SubId7
                                                         NPI,  SubId0
                                                         NPI,  SubId7
                                                         POW,  SubId1
                                                         POW,  SubId2
                                                         POW,  SubId3
                                                         POW,  SubId7
                                                         DFA,  SubId7
                                                         IPD,  SubId7
                                                         USBN, SubId7 */
        uint32_t reserved_14_14          : 1;
        uint32_t ap_pcix                 : 1;       /**< PCX Core Mode status (0=PCI Bus/1=PCIX) */
        uint32_t ap_64ad                 : 1;       /**< PCX Core Bus status (0=32b Bus/1=64b Bus) */
        uint32_t b12_bist                : 1;       /**< Bist Status For Memeory In B12 */
        uint32_t pmo_amod                : 1;       /**< PMO-ARB Mode (0=FP[HP=CMD1,LP=CMD0]/1=RR) */
        uint32_t pmo_fpc                 : 3;       /**< PMO-ARB Fixed Priority Counter
                                                         When PMO_AMOD=0 (FP mode), this field represents
                                                         the # of CMD1 requests that are issued (at higher
                                                         priority) before a single lower priority CMD0
                                                         is allowed to issue (to ensure foward progress).
                                                            0: 1 CMD1 Request issued before CMD0 allowed
                                                           ...
                                                            7: 8 CMD1 Requests issued before CMD0 allowed */
        uint32_t tsr_hwm                 : 3;       /**< Target Split-Read ADB(allowable disconnect boundary)
                                                         High Water Mark.
                                                         Specifies the number of ADBs(128 Byte aligned chunks)
                                                         that are accumulated(pending) BEFORE the Target Split
                                                         completion is attempted on the PCI bus.
                                                             0: RESERVED/ILLEGAL
                                                             1: 2 Pending ADBs (129B-256B)
                                                             2: 3 Pending ADBs (257B-384B)
                                                             3: 4 Pending ADBs (385B-512B)
                                                             4: 5 Pending ADBs (513B-640B)
                                                             5: 6 Pending ADBs (641B-768B)
                                                             6: 7 Pending ADBs (769B-896B)
                                                             7: 8 Pending ADBs (897B-1024B)
                                                         Example: Suppose a 1KB target memory request with
                                                         starting byte offset address[6:0]=0x7F is split by
                                                         the OCTEON and the TSR_HWM=1(2 ADBs).
                                                         The OCTEON will start the target split completion
                                                         on the PCI Bus after 1B(1st ADB)+128B(2nd ADB)=129B
                                                         of data have been received from memory (even though
                                                         the remaining 895B has not yet been received). The
                                                         OCTEON will continue the split completion until it
                                                         has consumed all of the pended split data. If the
                                                         full transaction length(1KB) of data was NOT entirely
                                                         transferred, then OCTEON will terminate the split
                                                         completion and again wait for another 2 ADB-aligned data
                                                         chunks(256B) of pended split data to be received from
                                                         memory before starting another split completion request.
                                                         This allows Octeon (as split completer), to send back
                                                         multiple split completions for a given large split
                                                         transaction without having to wait for the entire
                                                         transaction length to be received from memory.
                                                         NOTE: For split transaction sizes 'smaller' than the
                                                         specified TSR_HWM value, the split completion
                                                         is started when the last datum has been received from
                                                         memory.
                                                         NOTE: It is IMPERATIVE that this field NEVER BE
                                                         written to a ZERO value. A value of zero is
                                                         reserved/illegal and can result in PCIX bus hangs). */
        uint32_t bar2_enb                : 1;       /**< When set '1' BAR2 is enable and will respond when
                                                         clear '0' BAR2 access will be target-aborted. */
        uint32_t bar2_esx                : 2;       /**< Value will be XORed with pci-address[37:36] to
                                                         determine the endian swap mode. */
        uint32_t bar2_cax                : 1;       /**< Value will be XORed with pci-address[38] to
                                                         determine the L2 cache attribute. */
#else
        uint32_t bar2_cax                : 1;
        uint32_t bar2_esx                : 2;
        uint32_t bar2_enb                : 1;
        uint32_t tsr_hwm                 : 3;
        uint32_t pmo_fpc                 : 3;
        uint32_t pmo_amod                : 1;
        uint32_t b12_bist                : 1;
        uint32_t ap_64ad                 : 1;
        uint32_t ap_pcix                 : 1;
        uint32_t reserved_14_14          : 1;
        uint32_t en_wfilt                : 1;
        uint32_t scm                     : 1;
        uint32_t scmtyp                  : 1;
        uint32_t bar2pres                : 1;
        uint32_t erst_n                  : 1;
        uint32_t reserved_20_31          : 12;
#endif
    } cn3020;
    struct cvmx_pci_ctl_status_2_s       cn30xx;
    struct cvmx_pci_ctl_status_2_cn3020  cn31xx;
    struct cvmx_pci_ctl_status_2_s       cn36xx;
    struct cvmx_pci_ctl_status_2_s       cn38xx;
    struct cvmx_pci_ctl_status_2_cn3020  cn38xxp2;
    struct cvmx_pci_ctl_status_2_s       cn56xx;
    struct cvmx_pci_ctl_status_2_s       cn58xx;
} cvmx_pci_ctl_status_2_t;


/**
 * PCI_BAR1_INDEX(0..31) = PCI Index(0..31) Register
 *
 *  Contains address index and control bits for access to memory ranges of Bar-1,
 *  when PCI supplied address-bits [26:22] == (0..31).
 *
 * - Access using the CVMX_PCI_BAR1_INDEXX(0..31) CSR
 */
typedef union
{
    uint32_t u32;
    struct
    {
#if __BYTE_ORDER == __BIG_ENDIAN
        uint64_t reserved                : 14;      /**< Reserved */
        uint64_t addr_idx                : 14;      /**< Address bits [35:22] sent to L2C */
        uint64_t ca                      : 1;       /**< Set '1' when access is to be cached in L2. */
        uint64_t end_swp                 : 2;       /**< Endian Swap Mode */
        uint64_t addr_v                  : 1;       /**< Set '1' when the selected address range is valid. */
#else
        uint64_t addr_v                  : 1;
        uint64_t end_swp                 : 2;
        uint64_t ca                      : 1;
        uint64_t addr_idx                : 14;
        uint64_t reserved                : 14;
#endif
    } s;
} cvmx_pci_bar1_indexx_t;



/**
 * IPD_PTR_COUNT = IPD Page Pointer Count
 *  Shows the number of WQE and Packet Page Pointers stored in the IPD.
 *
 * - Access using the CVMX_IPD_PTR_COUNT CSR
 */
typedef union
{
    uint64_t u64;
    struct
    {
#if __BYTE_ORDER == __BIG_ENDIAN
        uint64_t reserved                : 45;      /**< Reserved */
        uint64_t pktv_cnt                : 1;       /**< PKT Ptr Valid. PASS2 Field */
        uint64_t wqev_cnt                : 1;       /**< WQE Ptr Valid. This value is '1' when a WQE
                                                         is being for use by the IPD. The value of this
                                                         field shoould be added to tha value of the
                                                         WQE_PCNT field, of this register, for a total
                                                         count of the WQE Page Pointers being held by IPD.
                                                         PASS2 Field. */
        uint64_t pfif_cnt                : 3;       /**< See PKT_PCNT. */
        uint64_t pkt_pcnt                : 7;       /**< This value plus PFIF_CNT plus 36 is the number
                                                         of PKT Page Pointers in IPD. */
        uint64_t wqe_pcnt                : 7;       /**< Number of page pointers for WQE storage that are
                                                         buffered in the IPD. The total count is the value
                                                         of this buffer plus the field [WQEV_CNT]. For
                                                         PASS-1 (which does not have the WQEV_CNT field)
                                                         when the value of this register is '0' there still
                                                         may be 1 pointer being help by IPD. */
#else
        uint64_t wqe_pcnt                : 7;
        uint64_t pkt_pcnt                : 7;
        uint64_t pfif_cnt                : 3;
        uint64_t wqev_cnt                : 1;
        uint64_t pktv_cnt                : 1;
        uint64_t reserved                : 45;
#endif
    } s;
} cvmx_ipd_ptr_count_t;

/**
 * GMX_INF_MODE = Interface Mode
 *
 * - Access using the CVMX_GMXX_INF_MODE(0..) CSR
 */
typedef union
{
    uint64_t u64;
    struct
    {
#if __BYTE_ORDER == __BIG_ENDIAN
        uint64_t reserved                : 61;      /**< Reserved */
        uint64_t p0mii                   : 1;       /**< Port 0 Interface Mode
                                                         0: Port 0 is RGMII
                                                         1: Port 0 is MII */
        uint64_t en                      : 1;       /**< Interface Enable */
        uint64_t type                    : 1;       /**< Port 1/2 Interface Mode
                                                         0: Ports 1 and 2 are RGMII
                                                         1: Port  1 is GMII/MII, Port 2 is unused
                                                         GMII/MII is selected by GMX_PRT1_CFG[SPEED] */
#else
        uint64_t type                    : 1;
        uint64_t en                      : 1;
        uint64_t p0mii                   : 1;
        uint64_t reserved                : 61;
#endif
    } s;
} cvmx_gmxx_inf_mode_t;




/**
 * SMI_CLK = Clock Control Register
 *
 * - Access using the CVMX_SMI_CLK CSR
 */
typedef union
{
    uint64_t u64;
    struct
    {
#if __BYTE_ORDER == __BIG_ENDIAN
        uint64_t reserved                : 51;      /**< Reserved */
        uint64_t preamble                : 1;       /**< Send PREAMBLE on SMI transacton */
        uint64_t sample                  : 4;       /**< When to sample read data
                                                         (number of eclks after the rising edge of mdc) */
        uint64_t phase                   : 8;       /**< MDC Clock Phase
                                                         (number of eclks that make up an mdc phase) */
#else
        uint64_t phase                   : 8;
        uint64_t sample                  : 4;
        uint64_t preamble                : 1;
        uint64_t reserved                : 51;
#endif
    } s;
} cvmx_smi_clk_t;


/**
 * SMI_CMD = Force a Read/Write command to the PHY
 * Writes to this register will create SMI xactions.  Software will poll on (depending on the xaction type).
 *
 * - Access using the CVMX_SMI_CMD CSR
 */
typedef union
{
    uint64_t u64;
    struct
    {
#if __BYTE_ORDER == __BIG_ENDIAN
        uint64_t reserved                : 47;      /**< Reserved */
        uint64_t phy_op                  : 1;       /**< PHY Opcode
                                                         0=write
                                                         1=read */
        uint64_t reserved1               : 3;       /**< Reserved */
        uint64_t phy_adr                 : 5;       /**< PHY Address */
        uint64_t reserved0               : 3;       /**< Reserved */
        uint64_t reg_adr                 : 5;       /**< PHY Register Offset */
#else
        uint64_t reg_adr                 : 5;
        uint64_t reserved0               : 3;
        uint64_t phy_adr                 : 5;
        uint64_t reserved1               : 3;
        uint64_t phy_op                  : 1;
        uint64_t reserved                : 47;
#endif
    } s;
} cvmx_smi_cmd_t;


/**
 * SMI_EN = Enable the SMI interface
 *
 * - Access using the CVMX_SMI_EN CSR
 */
typedef union
{
    uint64_t u64;
    struct
    {
#if __BYTE_ORDER == __BIG_ENDIAN
        uint64_t reserved                : 63;      /**< Reserved */
        uint64_t en                      : 1;       /**< Interface enable
                                                         0=SMI Interface is down / no transactions, no MDC
                                                         1=SMI Interface is up */
#else
        uint64_t en                      : 1;
        uint64_t reserved                : 63;
#endif
    } s;
} cvmx_smi_en_t;


/**
 * SMI_RD_DAT = SMI Read Data
 * VAL will assert when the read xaction completes.  A read to this register
 * will clear VAL.  PENDING indicates that an SMI RD transaction is in flight.
 *
 * - Access using the CVMX_SMI_RD_DAT CSR
 */
typedef union
{
    uint64_t u64;
    struct
    {
#if __BYTE_ORDER == __BIG_ENDIAN
        uint64_t reserved                : 46;      /**< Reserved */
        uint64_t pending                 : 1;       /**< Read Xaction Pending */
        uint64_t val                     : 1;       /**< Read Data Valid */
        uint64_t dat                     : 16;      /**< Read Data */
#else
        uint64_t dat                     : 16;
        uint64_t val                     : 1;
        uint64_t pending                 : 1;
        uint64_t reserved                : 46;
#endif
    } s;
} cvmx_smi_rd_dat_t;


/**
 * SMI_WR_DAT = SMI Write Data
 * VAL will assert when the write xaction completes.  A read to this register
 * will clear VAL.  PENDING indicates that an SMI WR transaction is in flight.
 *
 * - Access using the CVMX_SMI_WR_DAT CSR
 */
typedef union
{
    uint64_t u64;
    struct
    {
#if __BYTE_ORDER == __BIG_ENDIAN
        uint64_t reserved                : 46;      /**< Reserved */
        uint64_t pending                 : 1;       /**< Write Xaction Pending */
        uint64_t val                     : 1;       /**< Write Data Valid */
        uint64_t dat                     : 16;      /**< Write Data */
#else
        uint64_t dat                     : 16;
        uint64_t val                     : 1;
        uint64_t pending                 : 1;
        uint64_t reserved                : 46;
#endif
    } s;
} cvmx_smi_wr_dat_t;



/**
 * NPI_CTL_STATUS = NPI's Control Status Register
 *  Contains control ans status for NPI.
 *
 * - Access using the CVMX_NPI_CTL_STATUS CSR
 */
typedef union
{
    uint64_t u64;
    struct
    {
#if __BYTE_ORDER == __BIG_ENDIAN
        uint64_t reserved                : 1;       /**< Reserved */
        uint64_t chip_rev                : 8;       /**< The revision of the N3.
                                                         register will be disabled. */
        uint64_t dis_pniw                : 1;       /**< When asserted '1' access from the PNI Window
                                                         Registers are disabled. */
        uint64_t out3_enb                : 1;       /**< When asserted '1' the output3 engine is enabled.
                                                         After enabling the values of the associated
                                                         Address and Size Register should not be changed. */
        uint64_t out2_enb                : 1;       /**< When asserted '1' the output2 engine is enabled.
                                                         After enabling the values of the associated
                                                         Address and Size Register should not be changed. */
        uint64_t out1_enb                : 1;       /**< When asserted '1' the output1 engine is enabled.
                                                         After enabling the values of the associated
                                                         Address and Size Register should not be changed. */
        uint64_t out0_enb                : 1;       /**< When asserted '1' the output0 engine is enabled.
                                                         After enabling the values of the associated
                                                         Address and Size Register should not be changed. */
        uint64_t ins3_enb                : 1;       /**< When asserted '1' the gather3 engine is enabled.
                                                         After enabling the values of the associated
                                                         Address and Size Register should not be changed.
                                                         write operation. */
        uint64_t ins2_enb                : 1;       /**< When asserted '1' the gather2 engine is enabled.
                                                         After enabling the values of the associated
                                                         Address and Size Register should not be changed. */
        uint64_t ins1_enb                : 1;       /**< When asserted '1' the gather1 engine is enabled.
                                                         After enabling the values of the associated
                                                         Address and Size Register should not be changed. */
        uint64_t ins0_enb                : 1;       /**< When asserted '1' the gather0 engine is enabled.
                                                         After enabling the values of the associated
                                                         Address and Size Register should not be changed. */
        uint64_t ins3_64b                : 1;       /**< When asserted '1' the instructions read by the
                                                         gather3 engine are 64-Byte instructions, when
                                                         de-asserted '0' instructions are 32-byte. */
        uint64_t ins2_64b                : 1;       /**< When asserted '1' the instructions read by the
                                                         gather2 engine are 64-Byte instructions, when
                                                         de-asserted '0' instructions are 32-byte. */
        uint64_t ins1_64b                : 1;       /**< When asserted '1' the instructions read by the
                                                         gather1 engine are 64-Byte instructions, when
                                                         de-asserted '0' instructions are 32-byte. */
        uint64_t ins0_64b                : 1;       /**< When asserted '1' the instructions read by the
                                                         gather0 engine are 64-Byte instructions, when
                                                         de-asserted '0' instructions are 32-byte. */
        uint64_t pci_wdis                : 1;       /**< When set '1' disables access to registers in
                                                         PNI address range 0x1000 - 0x17FF from the PCI. */
        uint64_t wait_com                : 1;       /**< When set '1' casues the NPI to wait for a commit
                                                         from the L2C before sending additional access to
                                                         the L2C from the PCI. */
        uint64_t spares1                 : 3;       /**< These bits are reserved and should be set to 0. */
        uint64_t max_word                : 5;       /**< The maximum number of words to merge into a single
                                                         write operation from the PPs to the PCI. Legal
                                                         values are 1 to 32, where a '0' is treated as 32. */
        uint64_t spares0                 : 22;      /**< These bits are reserved and should be set to 0. */
        uint64_t timer                   : 10;      /**< When the NPI starts a PP to PCI write it will wait
                                                         no longer than the value of TIMER in eclks to
                                                         merge additional writes from the PPs into 1
                                                         large write. The values for this field is 1 to
                                                         1024 where a value of '0' is treated as 1024. */
#else
        uint64_t timer                   : 10;
        uint64_t spares0                 : 22;
        uint64_t max_word                : 5;
        uint64_t spares1                 : 3;
        uint64_t wait_com                : 1;
        uint64_t pci_wdis                : 1;
        uint64_t ins0_64b                : 1;
        uint64_t ins1_64b                : 1;
        uint64_t ins2_64b                : 1;
        uint64_t ins3_64b                : 1;
        uint64_t ins0_enb                : 1;
        uint64_t ins1_enb                : 1;
        uint64_t ins2_enb                : 1;
        uint64_t ins3_enb                : 1;
        uint64_t out0_enb                : 1;
        uint64_t out1_enb                : 1;
        uint64_t out2_enb                : 1;
        uint64_t out3_enb                : 1;
        uint64_t dis_pniw                : 1;
        uint64_t chip_rev                : 8;
        uint64_t reserved                : 1;
#endif
    } s;
} cvmx_npi_ctl_status_t;



typedef union
{
    uint64_t u64;
    struct
    {
#if __BYTE_ORDER == __BIG_ENDIAN
        uint64_t reserved                : 56;      /**< Reserved */
        uint64_t vlan_len                : 1;       /**< When set, disables the length check for VLAN pkts
                                                         (PASS2 only) */
        uint64_t pre_free                : 1;       /**< Allows for less strict PREAMBLE checking.
                                                         0 - 7 cycles of PREAMBLE followed by SFD
                                                         (PASS2 only) */
        uint64_t ctl_smac                : 1;       /**< Control Pause Frames can match station SMAC */
        uint64_t ctl_mcst                : 1;       /**< Control Pause Frames can match globally assign
                                                         Multicast address */
        uint64_t ctl_bck                 : 1;       /**< Forward pause information to TX block */
        uint64_t ctl_drp                 : 1;       /**< Drop Control Pause Frames */
        uint64_t pre_strp                : 1;       /**< Strip off the preamble (when present)
                                                         0=PREAMBLE+SFD is sent to core as part of frame
                                                         1=PREAMBLE+SFD is dropped */
        uint64_t pre_chk                 : 1;       /**< This port is configured to send PREAMBLE+SFD
                                                         to begin every frame.  GMX checks that the
                                                         PREAMBLE is sent correctly */
#else
        uint64_t pre_chk                 : 1;
        uint64_t pre_strp                : 1;
        uint64_t ctl_drp                 : 1;
        uint64_t ctl_bck                 : 1;
        uint64_t ctl_mcst                : 1;
        uint64_t ctl_smac                : 1;
        uint64_t pre_free                : 1;
        uint64_t vlan_len                : 1;
        uint64_t reserved                : 56;
#endif
    } s;
} cvmx_gmxx_rxx_frm_ctl_t;



/**
 * - Access using the CVMX_MIO_BOOT_REG_TIMX(1..7) CSR
 */

typedef union
{
    uint64_t u64;
    struct
    {
#if __BYTE_ORDER == __BIG_ENDIAN
        uint64_t pagem                   : 1;       /**< Region (1..7) page mode */
        uint64_t waitm                   : 1;       /**< Region (1..7) wait mode */
        uint64_t pages                   : 2;       /**< Region (1..7) page size (not in pass 1)*/
        uint64_t ale                     : 6;       /**< Region (1..7) ALE count (CN31XX only)*/
        uint64_t page                    : 6;       /**< Region (1..7) page count */
        uint64_t wait                    : 6;       /**< Region (1..7) wait count */
        uint64_t pause                   : 6;       /**< Region (1..7) pause count */
        uint64_t wr_hld                  : 6;       /**< Region (1..7) write hold count */
        uint64_t rd_hld                  : 6;       /**< Region (1..7) read hold count */
        uint64_t we                      : 6;       /**< Region (1..7) write enable count */
        uint64_t oe                      : 6;       /**< Region (1..7) output enable count */
        uint64_t ce                      : 6;       /**< Region (1..7) chip enable count */
        uint64_t adr                     : 6;       /**< Region (1..7) address count */
#else
        uint64_t adr                     : 6;
        uint64_t ce                      : 6;
        uint64_t oe                      : 6;
        uint64_t we                      : 6;
        uint64_t rd_hld                  : 6;
        uint64_t wr_hld                  : 6;
        uint64_t pause                   : 6;
        uint64_t wait                    : 6;
        uint64_t page                    : 6;
        uint64_t ale                     : 6;
        uint64_t pages                   : 2;
        uint64_t waitm                   : 1;
        uint64_t pagem                   : 1;
#endif
    } s;
} cvmx_mio_boot_reg_timx_t;



/**
 * - Access using the CVMX_MIO_BOOT_REG_CFGX(1..7) CSR
 */
typedef union
{
    uint64_t u64;
    struct
    {
#if __BYTE_ORDER == __BIG_ENDIAN
        uint64_t reserved                : 27;      /**< Reserved */
        uint64_t sam                     : 1;       /**< Region (1..7) SAM mode */
        uint64_t we_ext                  : 2;       /**< Region (1..7) write enable count extension */
        uint64_t oe_ext                  : 2;       /**< Region (1..7) output enable count extension */
        uint64_t en                      : 1;       /**< Region (1..7) enable */
        uint64_t orbit                   : 1;       /**< Region (1..7) or bit */
        uint64_t ale                     : 1;       /**< Region (1..7) ALE mode */
        uint64_t width                   : 1;       /**< Region (1..7) bus width */
        uint64_t size                    : 12;      /**< Region (1..7) size */
        uint64_t base                    : 16;      /**< Region (1..7) base address */
#else
        uint64_t base                    : 16;
        uint64_t size                    : 12;
        uint64_t width                   : 1;
        uint64_t ale                     : 1;
        uint64_t orbit                   : 1;
        uint64_t en                      : 1;
        uint64_t oe_ext                  : 2;
        uint64_t we_ext                  : 2;
        uint64_t sam                     : 1;
        uint64_t reserved                : 27;
#endif
    } s;
} cvmx_mio_boot_reg_cfgx_t;



/**
 * Access using the CVMX_MIO_FUS_RCMD CSR
 */
typedef union
{
    uint64_t u64;
    struct
    {
#if __BYTE_ORDER == __BIG_ENDIAN
        uint64_t reserved3               : 40;      /**< Reserved */
        uint64_t dat                     : 8;       /**< 8bits of fuse data */
        uint64_t reserved2               : 3;       /**< Reserved */
        uint64_t pend                    : 1;       /**< SW sets this bit on a write to start FUSE read
                                                         operation.  HW clears when read is complete and
                                                         the DAT is valid */
        uint64_t reserved1               : 3;       /**< Reserved */
        uint64_t efuse                   : 1;       /**< When set, return data from the efuse storage
                                                         rather than the local storage for the 320 HW fuses */
        uint64_t reserved0               : 1;       /**< Reserved */
        uint64_t addr                    : 7;       /**< The byte address of the fuse to read */
#else
        uint64_t addr                    : 7;
        uint64_t reserved0               : 1;
        uint64_t efuse                   : 1;
        uint64_t reserved1               : 3;
        uint64_t pend                    : 1;
        uint64_t reserved2               : 3;
        uint64_t dat                     : 8;
        uint64_t reserved3               : 40;
#endif
    } s;
} cvmx_mio_fus_rcmd_t;



typedef union
{
    uint64_t u64;
    struct
    {
#if __BYTE_ORDER == __BIG_ENDIAN
        uint64_t reserved                : 49;      /**< RESERVED */
        uint64_t lfb_enum                : 4;       /**< Specifies the LFB Entry# which is to be captured. */
        uint64_t lfb_dmp                 : 1;       /**< LFB Dump Enable: When written(=1), the contents of
                                                         the LFB specified by LFB_ENUM[3:0] are captured
                                                         into the L2C_LFB(0/1/2) registers. */
        uint64_t ppnum                   : 4;       /**< When L2C_DBG[L2T] or L2C_DBG[L2D] or L2C_DBG[FINV]
                                                         is enabled, this field determines which one-of-16
                                                         PPs is selected as the diagnostic PP. */
        uint64_t set                     : 3;       /**< When L2C_DBG[L2T] or L2C_DBG[L2D] or L2C_DBG[FINV]
                                                         is enabled, this field determines 1-of-8 targeted
                                                         sets to act upon. */
        uint64_t finv                    : 1;       /**< Flush-Invalidate.
                                                         When flush-invalidate is enable (FINV=1), all STF
                                                         write commands generated from the diagnostic PP
                                                         (L2C_DBG[PPNUM]) will invalidate the specified set
                                                         (L2C_DBG[SET]) at the index specified in the STF
                                                         address[16:7]. If a dirty block is detected (D=1), it is
                                                         written back to memory. The contents of the invalid
                                                         L2 Cache line is also 'scrubbed' with the STF write data.
                                                         NOTE: An STF command with write data=ZEROES can be
                                                         generated by SW using the Prefetch instruction with
                                                         Hint=30d "prepare for Store", followed by a SYNCW.
                                                         What is seen at the L2C as an STF w/wrdcnt=0 with all
                                                         of its mask bits clear (indicates zero-fill data).
                                                         A flush-invalidate will 'force-hit' the L2 cache at
                                                         [index,set] and invalidate the entry (V=0/D=0/U=0).
                                                         If the cache block is dirty, it is also written back
                                                         to memory. The DuTag state is probed/updated as normal
                                                         for an STF request.
                                                         TYPICAL APPLICATIONS:
                                                         1) L2 Tag/Data ECC SW Recovery
                                                         2) Cache Unlocking
                                                         NOTE: A Flush-invalidate to a LOCKED cache line will
                                                         always write the LOCKED line back to memory (since
                                                         LOCKED tag state is encoded as V=0/D=1). */
        uint64_t l2d                     : 1;       /**< When enabled (and L2C_DBG[L2T]=0), fill data is
                                                         returned directly from the L2 Data Store
                                                         (regardless of hit/miss) when an LDD instruction is
                                                         issued from a PP determined by the L2C_DBG[PPNUM]
                                                         field. The selected set# is determined by the
                                                         L2C_DBG[SET] field, and the index is determined
                                                         from the address[16:7] associated with the LDD
                                                         instruction.
                                                         This 'force-hit' will NOT alter the current L2 Tag
                                                         state OR the DuTag state. */
        uint64_t l2t                     : 1;       /**< When enabled, L2 Tag information [V,D,L,U,TAG]
                                                         is returned on the low order bits of the data bus
                                                         when an LDD instruction is issued from a PP determined
                                                         by the L2C_DBG[PPNUM] field. The selected
                                                         set# is determined by the L2C_DBG[SET] field, and
                                                         the index is determined from the address[16:7]
                                                         associated with the LDD instruction.
                                                         This 'force-hit' will NOT alter the current L2 Tag
                                                         state OR the DuTag state. */
#else
        uint64_t l2t                     : 1;
        uint64_t l2d                     : 1;
        uint64_t finv                    : 1;
        uint64_t set                     : 3;
        uint64_t ppnum                   : 4;
        uint64_t lfb_dmp                 : 1;
        uint64_t lfb_enum                : 4;
        uint64_t reserved                : 49;
#endif
    } s;
} cvmx_l2c_dbg_t;














/**
 * cvmx_lmc_comp_ctl
 *
 * LMC_COMP_CTL = LMC Compensation control
 *
 */
typedef union
{
    uint64_t u64;
    struct cvmx_lmc_comp_ctl_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN
        uint64_t reserved_32_63          : 32;
        uint64_t nctl_csr                : 4;       /**< Compensation control bits */
        uint64_t nctl_clk                : 4;       /**< Compensation control bits */
        uint64_t nctl_cmd                : 4;       /**< Compensation control bits */
        uint64_t nctl_dat                : 4;       /**< Compensation control bits */
        uint64_t pctl_csr                : 4;       /**< Compensation control bits */
        uint64_t pctl_clk                : 4;       /**< Compensation control bits */
        uint64_t pctl_cmd                : 4;       /**< Compensation control bits */
        uint64_t pctl_dat                : 4;       /**< Compensation control bits */
#else
        uint64_t pctl_dat                : 4;
        uint64_t pctl_cmd                : 4;
        uint64_t pctl_clk                : 4;
        uint64_t pctl_csr                : 4;
        uint64_t nctl_dat                : 4;
        uint64_t nctl_cmd                : 4;
        uint64_t nctl_clk                : 4;
        uint64_t nctl_csr                : 4;
        uint64_t reserved_32_63          : 32;
#endif
    } s;
    struct cvmx_lmc_comp_ctl_s           cn3020;
    struct cvmx_lmc_comp_ctl_s           cn30xx;
    struct cvmx_lmc_comp_ctl_s           cn31xx;
    struct cvmx_lmc_comp_ctl_s           cn36xx;
    struct cvmx_lmc_comp_ctl_s           cn38xx;
    struct cvmx_lmc_comp_ctl_s           cn38xxp2;
} cvmx_lmc_comp_ctl_t;



/**
 * cvmx_lmc_ctl
 *
 * LMC_CTL = LMC Control
 * This register is an assortment of various control fields needed by the memory controller
 */
typedef union
{
    uint64_t u64;
    struct cvmx_lmc_ctl_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN
        uint64_t reserved_32_63          : 32;
        uint64_t ddr__nctl               : 4;       /**< DDR nctl from compensation circuit
                                                         The encoded value on this will adjust the drive strength
                                                         of the DDR DQ pulldns. */
        uint64_t ddr__pctl               : 4;       /**< DDR pctl from compensation circuit
                                                         The encoded value on this will adjust the drive strength
                                                         of the DDR DQ pullup. */
        uint64_t slow_scf                : 1;       /**< 1=SCF has pass1 latency, 0=SCF has 1 cycle lower latency
                                                         when compared to pass1
                                                         NOTE - This bit has NO effect in PASS1 */
        uint64_t xor_bank                : 1;       /**< If (XOR_BANK == 1), then
                                                           bank[n:0]=address[n+7:7] ^ address[n+7+5:7+5]
                                                         else
                                                           bank[n:0]=address[n+7:7]
                                                         where n=1 for a 4 bank part and n=2 for an 8 bank part */
        uint64_t max_write_batch         : 4;       /**< Maximum number of consecutive writes to service before
                                                         allowing reads to interrupt. */
        uint64_t pll_div2                : 1;       /**< PLL Div2. */
        uint64_t pll_bypass              : 1;       /**< PLL Bypass. */
        uint64_t rdimm_ena               : 1;       /**< Registered DIMM Enable - When set allows the use
                                                         of JEDEC Registered DIMMs which require Write
                                                         data to be registered in the controller. */
        uint64_t r2r_slot                : 1;       /**< R2R Slot Enable: When set, all read-to-read trans
                                                         will slot an additional 1 cycle data bus bubble to
                                                         avoid DQ/DQS bus contention. This is only a CYA bit,
                                                         in case the "built-in" DIMM and RANK crossing logic
                                                         which should auto-detect and perfectly slot
                                                         read-to-reads to the same DIMM/RANK. */
        uint64_t inorder_mwf             : 1;       /**< When set, forces LMC_MWF (writes) into strict, in-order
                                                         mode.  When clear, writes may be serviced out of order
                                                         (optimized to keep multiple banks active).
                                                         This bit is ONLY to be set at power-on and
                                                         should not be set for normal use.
                                                         NOTE: For PASS1, set as follows:
                                                             DDR-I -> 1
                                                             DDR-II -> 0
                                                         For Pass2, this bit is RA0, write ignore (this feature
                                                         is permanently disabled) */
        uint64_t inorder_mrf             : 1;       /**< When set, forces LMC_MRF (reads) into strict, in-order
                                                         mode.  When clear, reads may be serviced out of order
                                                         (optimized to keep multiple banks active).
                                                         This bit is ONLY to be set at power-on and
                                                         should not be set for normal use.
                                                         NOTE: For PASS1, set as follows:
                                                             DDR-I -> 1
                                                             DDR-II -> 0
                                                         For Pass2, this bit should be written ZERO for
                                                         DDR I & II */
        uint64_t reserved_10_11          : 2;
        uint64_t fprch2                  : 1;       /**< Front Porch Enable: When set, the turn-off
                                                         time for the DDR_DQ/DQS drivers is 1 dclk earlier.
                                                         This bit should typically be set. */
        uint64_t bprch                   : 1;       /**< Back Porch Enable: When set, the turn-on time for
                                                         the DDR_DQ/DQS drivers is delayed an additional DCLK
                                                         cycle. This should be set to one whenever both SILO_HC
                                                         and SILO_QC are set. */
        uint64_t sil_lat                 : 2;       /**< SILO Latency: On reads, determines how many additional
                                                         dclks to wait (on top of TCL+1+TSKW) before pulling
                                                         data out of the pad silos.
                                                             - 00: illegal
                                                             - 01: 1 dclks
                                                             - 10: 2 dclks
                                                             - 11: illegal
                                                         This should always be set to 1. */
        uint64_t tskw                    : 2;       /**< This component is a representation of total BOARD
                                                         DELAY on DQ (used in the controller to determine the
                                                         R->W spacing to avoid DQS/DQ bus conflicts). Enter
                                                         the largest of the per byte Board delay
                                                             - 00: 0 dclk
                                                             - 01: 1 dclks
                                                             - 10: 2 dclks
                                                             - 11: 3 dclks */
        uint64_t qs_dic                  : 2;       /**< QS Drive Strength Control (DDR1):
                                                         & DDR2 Termination Resistor Setting
                                                         When in DDR2, a non Zero value in this register
                                                         enables the On Die Termination (ODT) in DDR parts.
                                                         These two bits are loaded into the RTT
                                                         portion of the EMRS register bits A6 & A2. If DDR2's
                                                         termination (for the memory's DQ/DQS/DM pads) is not
                                                         desired, set it to 00. If it is, chose between
                                                         01 for 75 ohm and 10 for 150 ohm termination.
                                                             00 = ODT Disabled
                                                             01 = 75 ohm Termination
                                                             10 = 150 ohm Termination
                                                             11 = 50 ohm Termination
                                                         Octeon, on writes, by default, drives the 4/8 ODT
                                                         pins (64/128b mode) based on what the masks
                                                         (LMC_WODT_CTL) are programmed to.
                                                         LMC_DDR2_CTL->ODT_ENA enables Octeon to drive ODT pins
                                                         for READS. LMC_RODT_CTL needs to be programmed based
                                                         on the system's needs for ODT. */
        uint64_t dic                     : 2;       /**< Drive Strength Control:
                                                         For DDR-I/II Mode, DIC[0] is
                                                         loaded into the Extended Mode Register (EMRS) A1 bit
                                                         during initialization. (see DDR-I data sheet EMRS
                                                         description)
                                                             0 = Normal
                                                             1 = Reduced
                                                         For DDR-II Mode, DIC[1] is used to load into EMRS
                                                         bit 10 - DQSN Enable/Disable field. By default, we
                                                         program the DDR's to drive the DQSN also. Set it to
                                                         1 if DQSN should be Hi-Z.
                                                             0 - DQSN Enable
                                                             1 - DQSN Disable */
#else
        uint64_t dic                     : 2;
        uint64_t qs_dic                  : 2;
        uint64_t tskw                    : 2;
        uint64_t sil_lat                 : 2;
        uint64_t bprch                   : 1;
        uint64_t fprch2                  : 1;
        uint64_t reserved_10_11          : 2;
        uint64_t inorder_mrf             : 1;
        uint64_t inorder_mwf             : 1;
        uint64_t r2r_slot                : 1;
        uint64_t rdimm_ena               : 1;
        uint64_t pll_bypass              : 1;
        uint64_t pll_div2                : 1;
        uint64_t max_write_batch         : 4;
        uint64_t xor_bank                : 1;
        uint64_t slow_scf                : 1;
        uint64_t ddr__pctl               : 4;
        uint64_t ddr__nctl               : 4;
        uint64_t reserved_32_63          : 32;
#endif
    } s;
    struct cvmx_lmc_ctl_cn3020
    {
#if __BYTE_ORDER == __BIG_ENDIAN
        uint64_t reserved_32_63          : 32;
        uint64_t ddr__nctl               : 4;       /**< DDR nctl from compensation circuit
                                                         The encoded value on this will adjust the drive strength
                                                         of the DDR DQ pulldns. */
        uint64_t ddr__pctl               : 4;       /**< DDR pctl from compensation circuit
                                                         The encoded value on this will adjust the drive strength
                                                         of the DDR DQ pullup. */
        uint64_t slow_scf                : 1;       /**< 1=SCF has pass1 latency, 0=SCF has 1 cycle lower latency
                                                         when compared to pass1 */
        uint64_t xor_bank                : 1;       /**< If (XOR_BANK == 1), then
                                                           bank[n:0]=address[n+7:7] ^ address[n+7+5:7+5]
                                                         else
                                                           bank[n:0]=address[n+7:7]
                                                         where n=1 for a 4 bank part and n=2 for an 8 bank part */
        uint64_t max_write_batch         : 4;       /**< Maximum number of consecutive writes to service before
                                                         allowing reads to interrupt. */
        uint64_t pll_div2                : 1;       /**< PLL Div2. */
        uint64_t pll_bypass              : 1;       /**< PLL Bypass. */
        uint64_t rdimm_ena               : 1;       /**< Registered DIMM Enable - When set allows the use
                                                         of JEDEC Registered DIMMs which require Write
                                                         data to be registered in the controller. */
        uint64_t r2r_slot                : 1;       /**< R2R Slot Enable: When set, all read-to-read trans
                                                         will slot an additional 1 cycle data bus bubble to
                                                         avoid DQ/DQS bus contention. This is only a CYA bit,
                                                         in case the "built-in" DIMM and RANK crossing logic
                                                         which should auto-detect and perfectly slot
                                                         read-to-reads to the same DIMM/RANK. */
        uint64_t reserved_13_13          : 1;
        uint64_t inorder_mrf             : 1;       /**< Always set to zero */
        uint64_t dreset                  : 1;       /**< Dclk domain reset.  The reset signal that is used by the
                                                         Dclk domain is (DRESET || ECLK_RESET). */
        uint64_t mode32b                 : 1;       /**< 32b data Path Mode
                                                         Set to 1 if we use only 32 DQ pins
                                                         0 for 64b DQ mode. */
        uint64_t fprch2                  : 1;       /**< Front Porch Enable: When set, the turn-off
                                                         time for the DDR_DQ/DQS drivers is 1 dclk earlier.
                                                         This bit should typically be set. */
        uint64_t bprch                   : 1;       /**< Back Porch Enable: When set, the turn-on time for
                                                         the DDR_DQ/DQS drivers is delayed an additional DCLK
                                                         cycle. This should be set to one whenever both SILO_HC
                                                         and SILO_QC are set. */
        uint64_t sil_lat                 : 2;       /**< SILO Latency: On reads, determines how many additional
                                                         dclks to wait (on top of TCL+1+TSKW) before pulling
                                                         data out of the pad silos.
                                                             - 00: illegal
                                                             - 01: 1 dclks
                                                             - 10: 2 dclks
                                                             - 11: illegal
                                                         This should always be set to 1. */
        uint64_t tskw                    : 2;       /**< This component is a representation of total BOARD
                                                         DELAY on DQ (used in the controller to determine the
                                                         R->W spacing to avoid DQS/DQ bus conflicts). Enter
                                                         the largest of the per byte Board delay
                                                             - 00: 0 dclk
                                                             - 01: 1 dclks
                                                             - 10: 2 dclks
                                                             - 11: 3 dclks */
        uint64_t qs_dic                  : 2;       /**< QS Drive Strength Control (DDR1):
                                                         & DDR2 Termination Resistor Setting
                                                         When in DDR2, a non Zero value in this register
                                                         enables the On Die Termination (ODT) in DDR parts.
                                                         These two bits are loaded into the RTT
                                                         portion of the EMRS register bits A6 & A2. If DDR2's
                                                         termination (for the memory's DQ/DQS/DM pads) is not
                                                         desired, set it to 00. If it is, chose between
                                                         01 for 75 ohm and 10 for 150 ohm termination.
                                                             00 = ODT Disabled
                                                             01 = 75 ohm Termination
                                                             10 = 150 ohm Termination
                                                             11 = 50 ohm Termination
                                                         Octeon, on writes, by default, drives the 8 ODT
                                                         pins based on what the masks (LMC_WODT_CTL1 & 2)
                                                         are programmed to. LMC_DDR2_CTL->ODT_ENA
                                                         enables Octeon to drive ODT pins for READS.
                                                         LMC_RODT_CTL needs to be programmed based on
                                                         the system's needs for ODT. */
        uint64_t dic                     : 2;       /**< Drive Strength Control:
                                                         For DDR-I/II Mode, DIC[0] is
                                                         loaded into the Extended Mode Register (EMRS) A1 bit
                                                         during initialization. (see DDR-I data sheet EMRS
                                                         description)
                                                             0 = Normal
                                                             1 = Reduced
                                                         For DDR-II Mode, DIC[1] is used to load into EMRS
                                                         bit 10 - DQSN Enable/Disable field. By default, we
                                                         program the DDR's to drive the DQSN also. Set it to
                                                         1 if DQSN should be Hi-Z.
                                                             0 - DQSN Enable
                                                             1 - DQSN Disable */
#else
        uint64_t dic                     : 2;
        uint64_t qs_dic                  : 2;
        uint64_t tskw                    : 2;
        uint64_t sil_lat                 : 2;
        uint64_t bprch                   : 1;
        uint64_t fprch2                  : 1;
        uint64_t mode32b                 : 1;
        uint64_t dreset                  : 1;
        uint64_t inorder_mrf             : 1;
        uint64_t reserved_13_13          : 1;
        uint64_t r2r_slot                : 1;
        uint64_t rdimm_ena               : 1;
        uint64_t pll_bypass              : 1;
        uint64_t pll_div2                : 1;
        uint64_t max_write_batch         : 4;
        uint64_t xor_bank                : 1;
        uint64_t slow_scf                : 1;
        uint64_t ddr__pctl               : 4;
        uint64_t ddr__nctl               : 4;
        uint64_t reserved_32_63          : 32;
#endif
    } cn3020;
    struct cvmx_lmc_ctl_cn3020           cn30xx;
    struct cvmx_lmc_ctl_cn3020           cn31xx;
    struct cvmx_lmc_ctl_cn36xx
    {
#if __BYTE_ORDER == __BIG_ENDIAN
        uint64_t reserved_32_63          : 32;
        uint64_t ddr__nctl               : 4;       /**< DDR nctl from compensation circuit
                                                         The encoded value on this will adjust the drive strength
                                                         of the DDR DQ pulldns. */
        uint64_t ddr__pctl               : 4;       /**< DDR pctl from compensation circuit
                                                         The encoded value on this will adjust the drive strength
                                                         of the DDR DQ pullup. */
        uint64_t slow_scf                : 1;       /**< 1=SCF has pass1 latency, 0=SCF has 1 cycle lower latency
                                                         when compared to pass1
                                                         NOTE - This bit has NO effect in PASS1 */
        uint64_t xor_bank                : 1;       /**< If (XOR_BANK == 1), then
                                                           bank[n:0]=address[n+7:7] ^ address[n+7+5:7+5]
                                                         else
                                                           bank[n:0]=address[n+7:7]
                                                         where n=1 for a 4 bank part and n=2 for an 8 bank part */
        uint64_t max_write_batch         : 4;       /**< Maximum number of consecutive writes to service before
                                                         allowing reads to interrupt. */
        uint64_t reserved_16_17          : 2;
        uint64_t rdimm_ena               : 1;       /**< Registered DIMM Enable - When set allows the use
                                                         of JEDEC Registered DIMMs which require Write
                                                         data to be registered in the controller. */
        uint64_t r2r_slot                : 1;       /**< R2R Slot Enable: When set, all read-to-read trans
                                                         will slot an additional 1 cycle data bus bubble to
                                                         avoid DQ/DQS bus contention. This is only a CYA bit,
                                                         in case the "built-in" DIMM and RANK crossing logic
                                                         which should auto-detect and perfectly slot
                                                         read-to-reads to the same DIMM/RANK. */
        uint64_t inorder_mwf             : 1;       /**< When set, forces LMC_MWF (writes) into strict, in-order
                                                         mode.  When clear, writes may be serviced out of order
                                                         (optimized to keep multiple banks active).
                                                         This bit is ONLY to be set at power-on and
                                                         should not be set for normal use.
                                                         NOTE: For PASS1, set as follows:
                                                             DDR-I -> 1
                                                             DDR-II -> 0
                                                         For Pass2, this bit is RA0, write ignore (this feature
                                                         is permanently disabled) */
        uint64_t inorder_mrf             : 1;       /**< When set, forces LMC_MRF (reads) into strict, in-order
                                                         mode.  When clear, reads may be serviced out of order
                                                         (optimized to keep multiple banks active).
                                                         This bit is ONLY to be set at power-on and
                                                         should not be set for normal use.
                                                         NOTE: For PASS1, set as follows:
                                                             DDR-I -> 1
                                                             DDR-II -> 0
                                                         For Pass2, this bit should be written ZERO for
                                                         DDR I & II */
        uint64_t set_zero                : 1;       /**< Reserved. Always Set this Bit to Zero */
        uint64_t mode128b                : 1;       /**< 128b data Path Mode
                                                         Set to 1 if we use all 128 DQ pins
                                                         0 for 64b DQ mode. */
        uint64_t fprch2                  : 1;       /**< Front Porch Enable: When set, the turn-off
                                                         time for the DDR_DQ/DQS drivers is 1 dclk earlier.
                                                         This bit should typically be set. */
        uint64_t bprch                   : 1;       /**< Back Porch Enable: When set, the turn-on time for
                                                         the DDR_DQ/DQS drivers is delayed an additional DCLK
                                                         cycle. This should be set to one whenever both SILO_HC
                                                         and SILO_QC are set. */
        uint64_t sil_lat                 : 2;       /**< SILO Latency: On reads, determines how many additional
                                                         dclks to wait (on top of TCL+1+TSKW) before pulling
                                                         data out of the pad silos.
                                                             - 00: illegal
                                                             - 01: 1 dclks
                                                             - 10: 2 dclks
                                                             - 11: illegal
                                                         This should always be set to 1. */
        uint64_t tskw                    : 2;       /**< This component is a representation of total BOARD
                                                         DELAY on DQ (used in the controller to determine the
                                                         R->W spacing to avoid DQS/DQ bus conflicts). Enter
                                                         the largest of the per byte Board delay
                                                             - 00: 0 dclk
                                                             - 01: 1 dclks
                                                             - 10: 2 dclks
                                                             - 11: 3 dclks */
        uint64_t qs_dic                  : 2;       /**< QS Drive Strength Control (DDR1):
                                                         & DDR2 Termination Resistor Setting
                                                         When in DDR2, a non Zero value in this register
                                                         enables the On Die Termination (ODT) in DDR parts.
                                                         These two bits are loaded into the RTT
                                                         portion of the EMRS register bits A6 & A2. If DDR2's
                                                         termination (for the memory's DQ/DQS/DM pads) is not
                                                         desired, set it to 00. If it is, chose between
                                                         01 for 75 ohm and 10 for 150 ohm termination.
                                                             00 = ODT Disabled
                                                             01 = 75 ohm Termination
                                                             10 = 150 ohm Termination
                                                             11 = 50 ohm Termination
                                                         Octeon, on writes, by default, drives the 4/8 ODT
                                                         pins (64/128b mode) based on what the masks
                                                         (LMC_WODT_CTL) are programmed to.
                                                         LMC_DDR2_CTL->ODT_ENA enables Octeon to drive ODT pins
                                                         for READS. LMC_RODT_CTL needs to be programmed based
                                                         on the system's needs for ODT. */
        uint64_t dic                     : 2;       /**< Drive Strength Control:
                                                         For DDR-I/II Mode, DIC[0] is
                                                         loaded into the Extended Mode Register (EMRS) A1 bit
                                                         during initialization. (see DDR-I data sheet EMRS
                                                         description)
                                                             0 = Normal
                                                             1 = Reduced
                                                         For DDR-II Mode, DIC[1] is used to load into EMRS
                                                         bit 10 - DQSN Enable/Disable field. By default, we
                                                         program the DDR's to drive the DQSN also. Set it to
                                                         1 if DQSN should be Hi-Z.
                                                             0 - DQSN Enable
                                                             1 - DQSN Disable */
#else
        uint64_t dic                     : 2;
        uint64_t qs_dic                  : 2;
        uint64_t tskw                    : 2;
        uint64_t sil_lat                 : 2;
        uint64_t bprch                   : 1;
        uint64_t fprch2                  : 1;
        uint64_t mode128b                : 1;
        uint64_t set_zero                : 1;
        uint64_t inorder_mrf             : 1;
        uint64_t inorder_mwf             : 1;
        uint64_t r2r_slot                : 1;
        uint64_t rdimm_ena               : 1;
        uint64_t reserved_16_17          : 2;
        uint64_t max_write_batch         : 4;
        uint64_t xor_bank                : 1;
        uint64_t slow_scf                : 1;
        uint64_t ddr__pctl               : 4;
        uint64_t ddr__nctl               : 4;
        uint64_t reserved_32_63          : 32;
#endif
    } cn36xx;
    struct cvmx_lmc_ctl_cn36xx           cn38xx;
    struct cvmx_lmc_ctl_cn36xx           cn38xxp2;
    struct cvmx_lmc_ctl_cn50xx
    {
#if __BYTE_ORDER == __BIG_ENDIAN
        uint64_t reserved_32_63          : 32;
        uint64_t ddr__nctl               : 4;       /**< DDR nctl from compensation circuit
                                                         The encoded value on this will adjust the drive strength
                                                         of the DDR DQ pulldns. */
        uint64_t ddr__pctl               : 4;       /**< DDR pctl from compensation circuit
                                                         The encoded value on this will adjust the drive strength
                                                         of the DDR DQ pullup. */
        uint64_t slow_scf                : 1;       /**< 1=SCF has pass1 latency, 0=SCF has 1 cycle lower latency
                                                         when compared to pass1
                                                         NOTE - This bit has NO effect in PASS1 */
        uint64_t xor_bank                : 1;       /**< If (XOR_BANK == 1), then
                                                           bank[n:0]=address[n+7:7] ^ address[n+7+5:7+5]
                                                         else
                                                           bank[n:0]=address[n+7:7]
                                                         where n=1 for a 4 bank part and n=2 for an 8 bank part */
        uint64_t max_write_batch         : 4;       /**< Maximum number of consecutive writes to service before
                                                         allowing reads to interrupt. */
        uint64_t reserved_17_17          : 1;
        uint64_t pll_bypass              : 1;       /**< PLL Bypass. */
        uint64_t rdimm_ena               : 1;       /**< Registered DIMM Enable - When set allows the use
                                                         of JEDEC Registered DIMMs which require Write
                                                         data to be registered in the controller. */
        uint64_t r2r_slot                : 1;       /**< R2R Slot Enable: When set, all read-to-read trans
                                                         will slot an additional 1 cycle data bus bubble to
                                                         avoid DQ/DQS bus contention. This is only a CYA bit,
                                                         in case the "built-in" DIMM and RANK crossing logic
                                                         which should auto-detect and perfectly slot
                                                         read-to-reads to the same DIMM/RANK. */
        uint64_t inorder_mwf             : 1;       /**< When set, forces LMC_MWF (writes) into strict, in-order
                                                         mode.  When clear, writes may be serviced out of order
                                                         (optimized to keep multiple banks active).
                                                         This bit is ONLY to be set at power-on and
                                                         should not be set for normal use.
                                                         NOTE: For PASS1, set as follows:
                                                             DDR-I -> 1
                                                             DDR-II -> 0
                                                         For Pass2, this bit is RA0, write ignore (this feature
                                                         is permanently disabled) */
        uint64_t inorder_mrf             : 1;       /**< When set, forces LMC_MRF (reads) into strict, in-order
                                                         mode.  When clear, reads may be serviced out of order
                                                         (optimized to keep multiple banks active).
                                                         This bit is ONLY to be set at power-on and
                                                         should not be set for normal use.
                                                         NOTE: For PASS1, set as follows:
                                                             DDR-I -> 1
                                                             DDR-II -> 0
                                                         For Pass2, this bit should be written ZERO for
                                                         DDR I & II */
        uint64_t dreset                  : 1;       /**< Dclk domain reset.  The reset signal that is used by the
                                                         Dclk domain is (DRESET || ECLK_RESET). */
        uint64_t mode32b                 : 1;       /**< 32b data Path Mode
                                                         Set to 1 if we use 32 DQ pins
                                                         0 for 16b DQ mode. */
        uint64_t fprch2                  : 1;       /**< Front Porch Enable: When set, the turn-off
                                                         time for the DDR_DQ/DQS drivers is 1 dclk earlier.
                                                         This bit should typically be set. */
        uint64_t bprch                   : 1;       /**< Back Porch Enable: When set, the turn-on time for
                                                         the DDR_DQ/DQS drivers is delayed an additional DCLK
                                                         cycle. This should be set to one whenever both SILO_HC
                                                         and SILO_QC are set. */
        uint64_t sil_lat                 : 2;       /**< SILO Latency: On reads, determines how many additional
                                                         dclks to wait (on top of TCL+1+TSKW) before pulling
                                                         data out of the pad silos.
                                                             - 00: illegal
                                                             - 01: 1 dclks
                                                             - 10: 2 dclks
                                                             - 11: illegal
                                                         This should always be set to 1. */
        uint64_t tskw                    : 2;       /**< This component is a representation of total BOARD
                                                         DELAY on DQ (used in the controller to determine the
                                                         R->W spacing to avoid DQS/DQ bus conflicts). Enter
                                                         the largest of the per byte Board delay
                                                             - 00: 0 dclk
                                                             - 01: 1 dclks
                                                             - 10: 2 dclks
                                                             - 11: 3 dclks */
        uint64_t qs_dic                  : 2;       /**< QS Drive Strength Control (DDR1):
                                                         & DDR2 Termination Resistor Setting
                                                         When in DDR2, a non Zero value in this register
                                                         enables the On Die Termination (ODT) in DDR parts.
                                                         These two bits are loaded into the RTT
                                                         portion of the EMRS register bits A6 & A2. If DDR2's
                                                         termination (for the memory's DQ/DQS/DM pads) is not
                                                         desired, set it to 00. If it is, chose between
                                                         01 for 75 ohm and 10 for 150 ohm termination.
                                                             00 = ODT Disabled
                                                             01 = 75 ohm Termination
                                                             10 = 150 ohm Termination
                                                             11 = 50 ohm Termination
                                                         Octeon, on writes, by default, drives the 4/8 ODT
                                                         pins (64/128b mode) based on what the masks
                                                         (LMC_WODT_CTL) are programmed to.
                                                         LMC_DDR2_CTL->ODT_ENA enables Octeon to drive ODT pins
                                                         for READS. LMC_RODT_CTL needs to be programmed based
                                                         on the system's needs for ODT. */
        uint64_t dic                     : 2;       /**< Drive Strength Control:
                                                         For DDR-I/II Mode, DIC[0] is
                                                         loaded into the Extended Mode Register (EMRS) A1 bit
                                                         during initialization. (see DDR-I data sheet EMRS
                                                         description)
                                                             0 = Normal
                                                             1 = Reduced
                                                         For DDR-II Mode, DIC[1] is used to load into EMRS
                                                         bit 10 - DQSN Enable/Disable field. By default, we
                                                         program the DDR's to drive the DQSN also. Set it to
                                                         1 if DQSN should be Hi-Z.
                                                             0 - DQSN Enable
                                                             1 - DQSN Disable */
#else
        uint64_t dic                     : 2;
        uint64_t qs_dic                  : 2;
        uint64_t tskw                    : 2;
        uint64_t sil_lat                 : 2;
        uint64_t bprch                   : 1;
        uint64_t fprch2                  : 1;
        uint64_t mode32b                 : 1;
        uint64_t dreset                  : 1;
        uint64_t inorder_mrf             : 1;
        uint64_t inorder_mwf             : 1;
        uint64_t r2r_slot                : 1;
        uint64_t rdimm_ena               : 1;
        uint64_t pll_bypass              : 1;
        uint64_t reserved_17_17          : 1;
        uint64_t max_write_batch         : 4;
        uint64_t xor_bank                : 1;
        uint64_t slow_scf                : 1;
        uint64_t ddr__pctl               : 4;
        uint64_t ddr__nctl               : 4;
        uint64_t reserved_32_63          : 32;
#endif
    } cn50xx;
    struct cvmx_lmc_ctl_cn58xx
    {
#if __BYTE_ORDER == __BIG_ENDIAN
        uint64_t reserved_32_63          : 32;
        uint64_t ddr__nctl               : 4;       /**< DDR nctl from compensation circuit
                                                         The encoded value on this will adjust the drive strength
                                                         of the DDR DQ pulldns. */
        uint64_t ddr__pctl               : 4;       /**< DDR pctl from compensation circuit
                                                         The encoded value on this will adjust the drive strength
                                                         of the DDR DQ pullup. */
        uint64_t slow_scf                : 1;       /**< 1=SCF has pass1 latency, 0=SCF has 1 cycle lower latency
                                                         when compared to pass1
                                                         NOTE - This bit has NO effect in PASS1 */
        uint64_t xor_bank                : 1;       /**< If (XOR_BANK == 1), then
                                                           bank[n:0]=address[n+7:7] ^ address[n+7+5:7+5]
                                                         else
                                                           bank[n:0]=address[n+7:7]
                                                         where n=1 for a 4 bank part and n=2 for an 8 bank part */
        uint64_t max_write_batch         : 4;       /**< Maximum number of consecutive writes to service before
                                                         allowing reads to interrupt. */
        uint64_t reserved_16_17          : 2;
        uint64_t rdimm_ena               : 1;       /**< Registered DIMM Enable - When set allows the use
                                                         of JEDEC Registered DIMMs which require Write
                                                         data to be registered in the controller. */
        uint64_t r2r_slot                : 1;       /**< R2R Slot Enable: When set, all read-to-read trans
                                                         will slot an additional 1 cycle data bus bubble to
                                                         avoid DQ/DQS bus contention. This is only a CYA bit,
                                                         in case the "built-in" DIMM and RANK crossing logic
                                                         which should auto-detect and perfectly slot
                                                         read-to-reads to the same DIMM/RANK. */
        uint64_t inorder_mwf             : 1;       /**< When set, forces LMC_MWF (writes) into strict, in-order
                                                         mode.  When clear, writes may be serviced out of order
                                                         (optimized to keep multiple banks active).
                                                         This bit is ONLY to be set at power-on and
                                                         should not be set for normal use.
                                                         NOTE: For PASS1, set as follows:
                                                             DDR-I -> 1
                                                             DDR-II -> 0
                                                         For Pass2, this bit is RA0, write ignore (this feature
                                                         is permanently disabled) */
        uint64_t inorder_mrf             : 1;       /**< When set, forces LMC_MRF (reads) into strict, in-order
                                                         mode.  When clear, reads may be serviced out of order
                                                         (optimized to keep multiple banks active).
                                                         This bit is ONLY to be set at power-on and
                                                         should not be set for normal use.
                                                         NOTE: For PASS1, set as follows:
                                                             DDR-I -> 1
                                                             DDR-II -> 0
                                                         For Pass2, this bit should be written ZERO for
                                                         DDR I & II */
        uint64_t dreset                  : 1;       /**< Dclk domain reset.  The reset signal that is used by the
                                                         Dclk domain is (DRESET || ECLK_RESET). */
        uint64_t mode128b                : 1;       /**< 128b data Path Mode
                                                         Set to 1 if we use all 128 DQ pins
                                                         0 for 64b DQ mode. */
        uint64_t fprch2                  : 1;       /**< Front Porch Enable: When set, the turn-off
                                                         time for the DDR_DQ/DQS drivers is 1 dclk earlier.
                                                         This bit should typically be set. */
        uint64_t bprch                   : 1;       /**< Back Porch Enable: When set, the turn-on time for
                                                         the DDR_DQ/DQS drivers is delayed an additional DCLK
                                                         cycle. This should be set to one whenever both SILO_HC
                                                         and SILO_QC are set. */
        uint64_t sil_lat                 : 2;       /**< SILO Latency: On reads, determines how many additional
                                                         dclks to wait (on top of TCL+1+TSKW) before pulling
                                                         data out of the pad silos.
                                                             - 00: illegal
                                                             - 01: 1 dclks
                                                             - 10: 2 dclks
                                                             - 11: illegal
                                                         This should always be set to 1. */
        uint64_t tskw                    : 2;       /**< This component is a representation of total BOARD
                                                         DELAY on DQ (used in the controller to determine the
                                                         R->W spacing to avoid DQS/DQ bus conflicts). Enter
                                                         the largest of the per byte Board delay
                                                             - 00: 0 dclk
                                                             - 01: 1 dclks
                                                             - 10: 2 dclks
                                                             - 11: 3 dclks */
        uint64_t qs_dic                  : 2;       /**< QS Drive Strength Control (DDR1):
                                                         & DDR2 Termination Resistor Setting
                                                         When in DDR2, a non Zero value in this register
                                                         enables the On Die Termination (ODT) in DDR parts.
                                                         These two bits are loaded into the RTT
                                                         portion of the EMRS register bits A6 & A2. If DDR2's
                                                         termination (for the memory's DQ/DQS/DM pads) is not
                                                         desired, set it to 00. If it is, chose between
                                                         01 for 75 ohm and 10 for 150 ohm termination.
                                                             00 = ODT Disabled
                                                             01 = 75 ohm Termination
                                                             10 = 150 ohm Termination
                                                             11 = 50 ohm Termination
                                                         Octeon, on writes, by default, drives the 4/8 ODT
                                                         pins (64/128b mode) based on what the masks
                                                         (LMC_WODT_CTL) are programmed to.
                                                         LMC_DDR2_CTL->ODT_ENA enables Octeon to drive ODT pins
                                                         for READS. LMC_RODT_CTL needs to be programmed based
                                                         on the system's needs for ODT. */
        uint64_t dic                     : 2;       /**< Drive Strength Control:
                                                         For DDR-I/II Mode, DIC[0] is
                                                         loaded into the Extended Mode Register (EMRS) A1 bit
                                                         during initialization. (see DDR-I data sheet EMRS
                                                         description)
                                                             0 = Normal
                                                             1 = Reduced
                                                         For DDR-II Mode, DIC[1] is used to load into EMRS
                                                         bit 10 - DQSN Enable/Disable field. By default, we
                                                         program the DDR's to drive the DQSN also. Set it to
                                                         1 if DQSN should be Hi-Z.
                                                             0 - DQSN Enable
                                                             1 - DQSN Disable */
#else
        uint64_t dic                     : 2;
        uint64_t qs_dic                  : 2;
        uint64_t tskw                    : 2;
        uint64_t sil_lat                 : 2;
        uint64_t bprch                   : 1;
        uint64_t fprch2                  : 1;
        uint64_t mode128b                : 1;
        uint64_t dreset                  : 1;
        uint64_t inorder_mrf             : 1;
        uint64_t inorder_mwf             : 1;
        uint64_t r2r_slot                : 1;
        uint64_t rdimm_ena               : 1;
        uint64_t reserved_16_17          : 2;
        uint64_t max_write_batch         : 4;
        uint64_t xor_bank                : 1;
        uint64_t slow_scf                : 1;
        uint64_t ddr__pctl               : 4;
        uint64_t ddr__nctl               : 4;
        uint64_t reserved_32_63          : 32;
#endif
    } cn58xx;
} cvmx_lmc_ctl_t;


/**
 * cvmx_lmc_ctl1
 *
 * LMC_CTL1 = LMC Control1
 * This register is an assortment of various control fields needed by the memory controller
 */
typedef union
{
    uint64_t u64;
    struct cvmx_lmc_ctl1_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN
        uint64_t reserved_10_63          : 54;
        uint64_t sil_mode                : 1;       /**< Read Silo mode.  0=envelope, 1=self-timed. */
        uint64_t dcc_enable              : 1;       /**< Duty Cycle Corrector Enable.
                                                         0=disable, 1=enable
                                                         If the memory part does not support DCC, then this bit
                                                         must be set to 0. */
        uint64_t reserved_2_7            : 6;
        uint64_t data_layout             : 2;       /**< Logical data layout per DQ byte lane:
                                                         In 32b mode, this setting has no effect and the data
                                                         layout DQ[35:0] is the following:
                                                             [E[3:0], D[31:24], D[23:16], D[15:8], D[7:0]]
                                                         In 16b mode, the DQ[35:0] layouts are the following:
                                                         0 - [0[3:0], 0[7:0], [0[7:2], E[1:0]], D[15:8], D[7:0]]
                                                         1 - [0[3:0], [0[7:2], E[1:0]], D[15:8], D[7:0], 0[7:0]]
                                                         2 - [[0[1:0], E[1:0]], D[15:8], D[7:0], 0[7:0], 0[7:0]]
                                                         where E means ecc, D means data, and 0 means logical 0 */
#else
        uint64_t data_layout             : 2;
        uint64_t reserved_2_7            : 6;
        uint64_t dcc_enable              : 1;
        uint64_t sil_mode                : 1;
        uint64_t reserved_10_63          : 54;
#endif
    } s;
    struct cvmx_lmc_ctl1_cn30xx
    {
#if __BYTE_ORDER == __BIG_ENDIAN
        uint64_t reserved_2_63           : 62;
        uint64_t data_layout             : 2;       /**< Logical data layout per DQ byte lane:
                                                         In 32b mode, this setting has no effect and the data
                                                         layout DQ[35:0] is the following:
                                                             [E[3:0], D[31:24], D[23:16], D[15:8], D[7:0]]
                                                         In 16b mode, the DQ[35:0] layouts are the following:
                                                         0 - [0[3:0], 0[7:0], [0[7:2], E[1:0]], D[15:8], D[7:0]]
                                                         1 - [0[3:0], [0[7:2], E[1:0]], D[15:8], D[7:0], 0[7:0]]
                                                         2 - [[0[1:0], E[1:0]], D[15:8], D[7:0], 0[7:0], 0[7:0]]
                                                         where E means ecc, D means data, and 0 means logical 0 */
#else
        uint64_t data_layout             : 2;
        uint64_t reserved_2_63           : 62;
#endif
    } cn30xx;
    struct cvmx_lmc_ctl1_s               cn50xx;
    struct cvmx_lmc_ctl1_cn58xx
    {
#if __BYTE_ORDER == __BIG_ENDIAN
        uint64_t reserved_10_63          : 54;
        uint64_t sil_mode                : 1;       /**< Read Silo mode.  0=envelope, 1=self-timed. */
        uint64_t dcc_enable              : 1;       /**< Duty Cycle Corrector Enable.
                                                         0=disable, 1=enable
                                                         If the memory part does not support DCC, then this bit
                                                         must be set to 0. */
        uint64_t reserved_0_7            : 8;
#else
        uint64_t reserved_0_7            : 8;
        uint64_t dcc_enable              : 1;
        uint64_t sil_mode                : 1;
        uint64_t reserved_10_63          : 54;
#endif
    } cn58xx;
} cvmx_lmc_ctl1_t;

/**
 * cvmx_lmc_dclk_cnt_hi
 *
 * LMC_DCLK_CNT_HI  = Performance Counters
 *
 */
typedef union
{
    uint64_t u64;
    struct cvmx_lmc_dclk_cnt_hi_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN
        uint64_t reserved_32_63          : 32;
        uint64_t dclkcnt_hi              : 32;      /**< Performance Counter that counts dclks
                                                         Upper 32-bits of a 64-bit counter. */
#else
        uint64_t dclkcnt_hi              : 32;
        uint64_t reserved_32_63          : 32;
#endif
    } s;
    struct cvmx_lmc_dclk_cnt_hi_s        cn3020;
    struct cvmx_lmc_dclk_cnt_hi_s        cn30xx;
    struct cvmx_lmc_dclk_cnt_hi_s        cn31xx;
    struct cvmx_lmc_dclk_cnt_hi_s        cn36xx;
    struct cvmx_lmc_dclk_cnt_hi_s        cn38xx;
    struct cvmx_lmc_dclk_cnt_hi_s        cn38xxp2;
} cvmx_lmc_dclk_cnt_hi_t;


/**
 * cvmx_lmc_dclk_cnt_lo
 *
 * LMC_DCLK_CNT_LO  = Performance Counters
 *
 */
typedef union
{
    uint64_t u64;
    struct cvmx_lmc_dclk_cnt_lo_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN
        uint64_t reserved_32_63          : 32;
        uint64_t dclkcnt_lo              : 32;      /**< Performance Counter that counts dclks
                                                         Lower 32-bits of a 64-bit counter. */
#else
        uint64_t dclkcnt_lo              : 32;
        uint64_t reserved_32_63          : 32;
#endif
    } s;
    struct cvmx_lmc_dclk_cnt_lo_s        cn3020;
    struct cvmx_lmc_dclk_cnt_lo_s        cn30xx;
    struct cvmx_lmc_dclk_cnt_lo_s        cn31xx;
    struct cvmx_lmc_dclk_cnt_lo_s        cn36xx;
    struct cvmx_lmc_dclk_cnt_lo_s        cn38xx;
    struct cvmx_lmc_dclk_cnt_lo_s        cn38xxp2;
} cvmx_lmc_dclk_cnt_lo_t;


/**
 * cvmx_lmc_ddr2_ctl
 *
 * LMC_DDR2_CTL = LMC DDR2 & DLL Control Register
 *
 */
typedef union
{
    uint64_t u64;
    struct cvmx_lmc_ddr2_ctl_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN
        uint64_t reserved_32_63          : 32;
        uint64_t bank8                   : 1;       /**< For 8 bank DDR2 parts
                                                         1 - DDR2 parts have 8 internal banks (BA is 3 bits
                                                         wide).
                                                         0 - DDR2 parts have 4 internal banks (BA is 2 bits
                                                         wide).
                                                         NOTE - PASS2 ONLY - no effect in PASS1 */
        uint64_t burst8                  : 1;       /**< 8-burst mode.
                                                         1 - DDR data transfer happens in burst of 8
                                                         0 - DDR data transfer happens in burst of 4
                                                         BURST8 should be set when DDR2T is set
                                                         to minimize the command bandwidth loss.
                                                         NOTE: PASS2 ONLY - no effect in PASS1 */
        uint64_t addlat                  : 3;       /**< Additional Latency for posted CAS
                                                         When Posted CAS is on, this configures the additional
                                                         latency. This should be set to
                                                                1 .. LMC_MEM_CFG1[TRCD]-2
                                                         (Note the implication that posted CAS should not
                                                         be used when tRCD is two.) */
        uint64_t pocas                   : 1;       /**< Enable the Posted CAS feature of DDR2. */
        uint64_t bwcnt                   : 1;       /**< Bus utilization counter Clear.
                                                         Clears the LMC_OPS_CNT_*, LMC_IFB_CNT_*, and
                                                         LMC_DCLK_CNT_* registers. SW should first write this
                                                         field to a one, then write this field to a zero to
                                                         clear the CSR's. */
        uint64_t twr                     : 3;       /**< DDR Write Recovery time (tWR). Last Wr Brst to Pre delay
                                                         This is not a direct encoding of the value. Its
                                                         programmed as below per DDR2 spec. The decimal number
                                                         on the right is RNDUP(tWR(ns) / clkFreq)
                                                          TYP=15ns
                                                             000: RESERVED
                                                             001: 2
                                                             010: 3
                                                             011: 4
                                                             100: 5
                                                             101: 6
                                                             110-111: RESERVED */
        uint64_t silo_hc                 : 1;       /**< Delays the read sample window by a Half Cycle. */
        uint64_t ddr_eof                 : 4;       /**< Early Fill Counter Init.
                                                         L2 needs to know a few cycle before a fill completes so
                                                         it can get its Control pipe started (for better overall
                                                         performance). This counter contains  an init value which
                                                         is a function of Eclk/Dclk ratio to account for the
                                                         asynchronous boundary between L2 cache and the DRAM
                                                         controller. This init value will
                                                         determine when to safely let the L2 know that a fill
                                                         termination is coming up.
                                                         DDR_EOF = RNDUP (DCLK period/Eclk Period). If the ratio
                                                         is above 3, set DDR_EOF to 3.
                                                             DCLK/ECLK period         DDR_EOF
                                                                Less than 1            1
                                                                Less than 2            2
                                                                More than 2            3 */
        uint64_t tfaw                    : 5;       /**< tFAW - Cycles = RNDUP[tFAW(ns)/tcyc(ns)] - 1
                                                         Four Access Window time. Relevant only in DDR2 AND in
                                                         8-bank parts.
                                                             tFAW = 5'b0 in DDR1 and DDR2-4bank
                                                             tFAW = RNDUP[tFAW(ns)/tcyc(ns)] - 1
                                                                      in DDR2-8bank
                                                         NOTE: Has NO effect in PASS1 & DDR1 parts. This is a
                                                         PASS2 only register. Keep it as 0 for DDR1 and 4 bank
                                                         DDR2 parts */
        uint64_t crip_mode               : 1;       /**< Cripple Mode - When set, the LMC allows only
                                                         1 inflight transaction (.vs. 8 in normal mode).
                                                         This bit is ONLY to be set at power-on and
                                                         should not be set for normal use. Moved from LMC_CTL
                                                         to LMC_DDR2_CTL for PASS2 */
        uint64_t ddr2t                   : 1;       /**< Turn on the DDR 2T mode. 2 cycle window for CMD and
                                                         address. This mode helps relieve setup time pressure
                                                         on the Address and command bus which nominally have
                                                         a very large fanout. Please refer to Micron's tech
                                                         note tn_47_01 titled "DDR2-533 Memory Design Guide
                                                         for Two Dimm Unbuffered Systems" for physical details.
                                                         There is a
                                                         PASS2 Feature. NO effect in PASS1
                                                         Program as zero for DDR1 and when not needed in DDR2
                                                         BURST8 should be set when DDR2T is set to minimize
                                                         add/cmd loss. */
        uint64_t odt_ena                 : 1;       /**< Enable ODT for DDR2 on Reads - PASS2 ONLY
                                                         When this is on, the following fields must also be
                                                         programmed:
                                                             LMC_CTL->QS_DIC - programs the termination value
                                                             LMC_RODT_CTL - programs the ODT I/O mask for Reads
                                                         Program as 0 for DDR1 mode and ODT needs to be off
                                                         on Octeon Reads */
        uint64_t qdll_ena                : 1;       /**< DDR Quad DLL Enable: A 0->1 transition on this bit after
                                                         erst deassertion will reset the DDR 90 DLL. Should
                                                         happen at startup before any activity in DDR. */
        uint64_t dll90_vlu               : 5;       /**< Contains the open loop setting value for the DDR90 delay
                                                         line. */
        uint64_t dll90_byp               : 1;       /**< DDR DLL90 Bypass: When set, the DDR90 DLL is to be
                                                         bypassed and the setting is defined by DLL90_VLU */
        uint64_t rdqs                    : 1;       /**< DDR2 RDQS mode. When set, configures memory subsystem to
                                                         use unidirectional DQS pins. RDQS/DM - Rcv & DQS - Xmit */
        uint64_t ddr2                    : 1;       /**< DDR2 Enable: When set, configures memory subsystem for
                                                         DDR-II SDRAMs. */
#else
        uint64_t ddr2                    : 1;
        uint64_t rdqs                    : 1;
        uint64_t dll90_byp               : 1;
        uint64_t dll90_vlu               : 5;
        uint64_t qdll_ena                : 1;
        uint64_t odt_ena                 : 1;
        uint64_t ddr2t                   : 1;
        uint64_t crip_mode               : 1;
        uint64_t tfaw                    : 5;
        uint64_t ddr_eof                 : 4;
        uint64_t silo_hc                 : 1;
        uint64_t twr                     : 3;
        uint64_t bwcnt                   : 1;
        uint64_t pocas                   : 1;
        uint64_t addlat                  : 3;
        uint64_t burst8                  : 1;
        uint64_t bank8                   : 1;
        uint64_t reserved_32_63          : 32;
#endif
    } s;
    struct cvmx_lmc_ddr2_ctl_cn3020
    {
#if __BYTE_ORDER == __BIG_ENDIAN
        uint64_t reserved_32_63          : 32;
        uint64_t bank8                   : 1;       /**< For 8 bank DDR2 parts
                                                         1 - DDR2 parts have 8 internal banks (BA is 3 bits
                                                         wide).
                                                         0 - DDR2 parts have 4 internal banks (BA is 2 bits
                                                         wide). */
        uint64_t burst8                  : 1;       /**< 8-burst mode.
                                                         1 - DDR data transfer happens in burst of 8
                                                         0 - DDR data transfer happens in burst of 4
                                                         BURST8 should be set when DDR2T is set to minimize
                                                         add/cmd bandwidth loss. */
        uint64_t addlat                  : 3;       /**< Additional Latency for posted CAS
                                                         When Posted CAS is on, this configures the additional
                                                         latency. This should be set to
                                                                1 .. LMC_MEM_CFG1[TRCD]-2
                                                         (Note the implication that posted CAS should not
                                                         be used when tRCD is two.) */
        uint64_t pocas                   : 1;       /**< Enable the Posted CAS feature of DDR2. */
        uint64_t bwcnt                   : 1;       /**< Bus utilization counter Clear.
                                                         Clears the LMC_OPS_CNT_*, LMC_IFB_CNT_*, and
                                                         LMC_DCLK_CNT_* registers. SW should first write this
                                                         field to a one, then write this field to a zero to
                                                         clear the CSR's. */
        uint64_t twr                     : 3;       /**< DDR Write Recovery time (tWR). Last Wr Brst to Pre delay
                                                         This is not a direct encoding of the value. Its
                                                         programmed as below per DDR2 spec. The decimal number
                                                         on the right is RNDUP(tWR(ns) / clkFreq)
                                                          TYP=15ns
                                                             000: RESERVED
                                                             001: 2
                                                             010: 3
                                                             011: 4
                                                             100: 5
                                                             101: 6
                                                             110-111: RESERVED */
        uint64_t silo_hc                 : 1;       /**< Delays the read sample window by a Half Cycle. */
        uint64_t ddr_eof                 : 4;       /**< Early Fill Counter Init.
                                                         L2 needs to know a few cycle before a fill completes so
                                                         it can get its Control pipe started (for better overall
                                                         performance). This counter contains  an init value which
                                                         is a function of Eclk/Dclk ratio to account for the
                                                         asynchronous boundary between L2 cache and the DRAM
                                                         controller. This init value will
                                                         determine when to safely let the L2 know that a fill
                                                         termination is coming up.
                                                         DDR_EOF = RNDUP (DCLK period/Eclk Period). If the ratio
                                                         is above 3, set DDR_EOF to 3.
                                                             DCLK/ECLK period         DDR_EOF
                                                                Less than 1            1
                                                                Less than 2            2
                                                                More than 2            3 */
        uint64_t tfaw                    : 5;       /**< tFAW - Cycles = RNDUP[tFAW(ns)/tcyc(ns)] - 1
                                                         Four Access Window time. Relevant only in
                                                         8-bank parts.
                                                             TFAW = 5'b0 for DDR2-4bank
                                                             TFAW = RNDUP[tFAW(ns)/tcyc(ns)] - 1 in DDR2-8bank */
        uint64_t crip_mode               : 1;       /**< Cripple Mode - When set, the LMC allows only
                                                         1 inflight transaction (.vs. 8 in normal mode).
                                                         This bit is ONLY to be set at power-on and
                                                         should not be set for normal use. */
        uint64_t ddr2t                   : 1;       /**< Turn on the DDR 2T mode. 2 cycle window for CMD and
                                                         address. This mode helps relieve setup time pressure
                                                         on the Address and command bus which nominally have
                                                         a very large fanout. Please refer to Micron's tech
                                                         note tn_47_01 titled "DDR2-533 Memory Design Guide
                                                         for Two Dimm Unbuffered Systems" for physical details.
                                                         BURST8 should be used when DDR2T is set to minimize
                                                         add/cmd bandwidth loss. */
        uint64_t odt_ena                 : 1;       /**< Enable ODT for DDR2 on Reads
                                                         When this is on, the following fields must also be
                                                         programmed:
                                                             LMC_CTL->QS_DIC - programs the termination value
                                                             LMC_RODT_CTL - programs the ODT I/O mask for writes
                                                         Program as 0 for DDR1 mode and ODT needs to be off
                                                         on Octeon Reads */
        uint64_t qdll_ena                : 1;       /**< DDR Quad DLL Enable: A 0->1 transition on this bit after
                                                         erst deassertion will reset the DDR 90 DLL. Should
                                                         happen at startup before any activity in DDR. */
        uint64_t dll90_vlu               : 5;       /**< Contains the open loop setting value for the DDR90 delay
                                                         line. */
        uint64_t dll90_byp               : 1;       /**< DDR DLL90 Bypass: When set, the DDR90 DLL is to be
                                                         bypassed and the setting is defined by DLL90_VLU */
        uint64_t reserved_1_1            : 1;
        uint64_t ddr2                    : 1;       /**< DDR2 Enable: When set, configures memory subsystem for
                                                         DDR-II SDRAMs. */
#else
        uint64_t ddr2                    : 1;
        uint64_t reserved_1_1            : 1;
        uint64_t dll90_byp               : 1;
        uint64_t dll90_vlu               : 5;
        uint64_t qdll_ena                : 1;
        uint64_t odt_ena                 : 1;
        uint64_t ddr2t                   : 1;
        uint64_t crip_mode               : 1;
        uint64_t tfaw                    : 5;
        uint64_t ddr_eof                 : 4;
        uint64_t silo_hc                 : 1;
        uint64_t twr                     : 3;
        uint64_t bwcnt                   : 1;
        uint64_t pocas                   : 1;
        uint64_t addlat                  : 3;
        uint64_t burst8                  : 1;
        uint64_t bank8                   : 1;
        uint64_t reserved_32_63          : 32;
#endif
    } cn3020;
    struct cvmx_lmc_ddr2_ctl_cn3020      cn30xx;
    struct cvmx_lmc_ddr2_ctl_cn3020      cn31xx;
    struct cvmx_lmc_ddr2_ctl_s           cn36xx;
    struct cvmx_lmc_ddr2_ctl_s           cn38xx;
    struct cvmx_lmc_ddr2_ctl_s           cn38xxp2;
    struct cvmx_lmc_ddr2_ctl_s           cn50xx;
    struct cvmx_lmc_ddr2_ctl_s           cn56xx;
    struct cvmx_lmc_ddr2_ctl_s           cn58xx;
} cvmx_lmc_ddr2_ctl_t;

/**
 * cvmx_lmc_delay_cfg
 *
 * LMC_DELAY_CFG = Open-loop delay line settings
 *
 */
typedef union
{
    uint64_t u64;
    struct cvmx_lmc_delay_cfg_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN
        uint64_t reserved_15_63          : 49;
        uint64_t dq                      : 5;       /**< Setting for DQ  delay line */
        uint64_t cmd                     : 5;       /**< Setting for CMD delay line */
        uint64_t clk                     : 5;       /**< Setting for CLK delay line */
#else
        uint64_t clk                     : 5;
        uint64_t cmd                     : 5;
        uint64_t dq                      : 5;
        uint64_t reserved_15_63          : 49;
#endif
    } s;
    struct cvmx_lmc_delay_cfg_s          cn30xx;
    struct cvmx_lmc_delay_cfg_cn36xx
    {
#if __BYTE_ORDER == __BIG_ENDIAN
        uint64_t reserved_14_63          : 50;
        uint64_t dq                      : 4;       /**< Setting for DQ  delay line */
        uint64_t reserved_9_9            : 1;
        uint64_t cmd                     : 4;       /**< Setting for CMD delay line */
        uint64_t reserved_4_4            : 1;
        uint64_t clk                     : 4;       /**< Setting for CLK delay line */
#else
        uint64_t clk                     : 4;
        uint64_t reserved_4_4            : 1;
        uint64_t cmd                     : 4;
        uint64_t reserved_9_9            : 1;
        uint64_t dq                      : 4;
        uint64_t reserved_14_63          : 50;
#endif
    } cn36xx;
    struct cvmx_lmc_delay_cfg_cn36xx     cn38xx;
} cvmx_lmc_delay_cfg_t;


/**
 * cvmx_lmc_ecc_synd
 *
 * LMC_ECC_SYND = MRD ECC Syndromes
 *
 */
typedef union
{
    uint64_t u64;
    struct cvmx_lmc_ecc_synd_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN
        uint64_t reserved_32_63          : 32;
        uint64_t mrdsyn3                 : 8;       /**< MRD ECC Syndrome Quad3
                                                         128b mode -  corresponds to DQ[127:64], Phase1
                                                         64b mode  -  corresponds to DQ[127:64], Phase1, cycle1 */
        uint64_t mrdsyn2                 : 8;       /**< MRD ECC Syndrome Quad2
                                                         128b mode -  corresponds to DQ[63:0], Phase1
                                                         64b mode  -  corresponds to DQ[63:0], Phase1, cycle0 */
        uint64_t mrdsyn1                 : 8;       /**< MRD ECC Syndrome Quad1
                                                         128b mode -  corresponds to DQ[127:64], Phase0
                                                         64b mode  -  corresponds to DQ[127:64], Phase0, cycle1 */
        uint64_t mrdsyn0                 : 8;       /**< MRD ECC Syndrome Quad0
                                                         In 128b mode, ecc is calulated on 1 cycle worth of data
                                                         SYND0 corresponds to DQ[63:0], Phase0
                                                         In 64b mode, ecc is calculated on 2 cycle worth of data
                                                         SYND0 corresponds to DQ[63:0], Phase0, cycle0 */
#else
        uint64_t mrdsyn0                 : 8;
        uint64_t mrdsyn1                 : 8;
        uint64_t mrdsyn2                 : 8;
        uint64_t mrdsyn3                 : 8;
        uint64_t reserved_32_63          : 32;
#endif
    } s;
    struct cvmx_lmc_ecc_synd_s           cn3020;
    struct cvmx_lmc_ecc_synd_s           cn30xx;
    struct cvmx_lmc_ecc_synd_s           cn31xx;
    struct cvmx_lmc_ecc_synd_s           cn36xx;
    struct cvmx_lmc_ecc_synd_s           cn38xx;
    struct cvmx_lmc_ecc_synd_s           cn38xxp2;
} cvmx_lmc_ecc_synd_t;


/**
 * cvmx_lmc_fadr
 *
 * LMC_FADR = LMC Failing Address Register (SEC/DED)
 * 
 * This register only captures the first transaction with ecc errors. A DBE error can
 * over-write this register with its failing addresses. If you write
 * LMC_MEM_CFG0->SEC_ERR/DED_ERR then it will clear the error bits and capture the
 * next failing address.
 * The phy mapping is a function of the num Col bits & # row bits
 * For eg., in wide mode (128b),
 * mem_address[3:0]=4'b0 (because we address 16B at a time)
 * mem_address[6:4]=col_addr[2:0]
 * mem_address[8:7]=bank[1:0]
 * mem_address[15:9]= col_addr[9:3] assuming #col bits=10 ie., row_lsb=2, if row_lsb is 3,
 * then #col_bits = 11 and you move it up one more bit and so on.
 * mem_address[29:16]=row_addr[13:0] assuming #row_bits=14 which means pbank_lsb=4, if pbank_lsb=3,
 * then #row bits =13 and so on
 * mem_address[30] = rank, assuming bunk_ena=1 ie., dual rank dimm
 * mem_address[31] - logical dimm
 * 
 * If failing dimm is 2 that means the error is in the higher bits dimm.
 */
typedef union
{
    uint64_t u64;
    struct cvmx_lmc_fadr_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN
        uint64_t reserved_32_63          : 32;
        uint64_t fdimm                   : 2;       /**< Failing DIMM# */
        uint64_t fbunk                   : 1;       /**< Failing Rank */
        uint64_t fbank                   : 3;       /**< Failing Bank[2:0] */
        uint64_t frow                    : 14;      /**< Failing Row Address[13:0] */
        uint64_t fcol                    : 12;      /**< Failing Column Start Address[11:0]
                                                         Represents the Failing read's starting column address
                                                         (and not the exact column address in which the SEC/DED
                                                         was detected) */
#else
        uint64_t fcol                    : 12;
        uint64_t frow                    : 14;
        uint64_t fbank                   : 3;
        uint64_t fbunk                   : 1;
        uint64_t fdimm                   : 2;
        uint64_t reserved_32_63          : 32;
#endif
    } s;
    struct cvmx_lmc_fadr_s               cn3020;
    struct cvmx_lmc_fadr_s               cn30xx;
    struct cvmx_lmc_fadr_s               cn31xx;
    struct cvmx_lmc_fadr_s               cn36xx;
    struct cvmx_lmc_fadr_s               cn38xx;
    struct cvmx_lmc_fadr_s               cn38xxp2;
} cvmx_lmc_fadr_t;


/**
 * cvmx_lmc_ifb_cnt_hi
 *
 * LMC_IFB_CNT_HI  = Performance Counters
 *
 */
typedef union
{
    uint64_t u64;
    struct cvmx_lmc_ifb_cnt_hi_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN
        uint64_t reserved_32_63          : 32;
        uint64_t ifbcnt_hi               : 32;      /**< Performance Counter to measure Bus Utilization
                                                         Upper 32-bits of 64-bit counter that increments every
                                                         cycle there is something in the in-flight buffer. */
#else
        uint64_t ifbcnt_hi               : 32;
        uint64_t reserved_32_63          : 32;
#endif
    } s;
    struct cvmx_lmc_ifb_cnt_hi_s         cn3020;
    struct cvmx_lmc_ifb_cnt_hi_s         cn30xx;
    struct cvmx_lmc_ifb_cnt_hi_s         cn31xx;
    struct cvmx_lmc_ifb_cnt_hi_s         cn36xx;
    struct cvmx_lmc_ifb_cnt_hi_s         cn38xx;
    struct cvmx_lmc_ifb_cnt_hi_s         cn38xxp2;
} cvmx_lmc_ifb_cnt_hi_t;


/**
 * cvmx_lmc_ifb_cnt_lo
 *
 * LMC_IFB_CNT_LO  = Performance Counters
 *
 */
typedef union
{
    uint64_t u64;
    struct cvmx_lmc_ifb_cnt_lo_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN
        uint64_t reserved_32_63          : 32;
        uint64_t ifbcnt_lo               : 32;      /**< Performance Counter
                                                         Low 32-bits of 64-bit counter that increments every
                                                         cycle there is something in the in-flight buffer. */
#else
        uint64_t ifbcnt_lo               : 32;
        uint64_t reserved_32_63          : 32;
#endif
    } s;
    struct cvmx_lmc_ifb_cnt_lo_s         cn3020;
    struct cvmx_lmc_ifb_cnt_lo_s         cn30xx;
    struct cvmx_lmc_ifb_cnt_lo_s         cn31xx;
    struct cvmx_lmc_ifb_cnt_lo_s         cn36xx;
    struct cvmx_lmc_ifb_cnt_lo_s         cn38xx;
    struct cvmx_lmc_ifb_cnt_lo_s         cn38xxp2;
} cvmx_lmc_ifb_cnt_lo_t;


/**
 * cvmx_lmc_mem_cfg0
 *
 * Specify the RSL base addresses for the block
 * 
 *                  LMC_MEM_CFG0 = LMC Memory Configuration Register0
 * 
 * This register controls certain parameters of  Memory Configuration
 */
typedef union
{
    uint64_t u64;
    struct cvmx_lmc_mem_cfg0_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN
        uint64_t reserved_32_63          : 32;
        uint64_t reset                   : 1;       /**< Reset oneshot pulse for refresh counter,
                                                         and LMC_OPS_CNT_*, LMC_IFB_CNT_*, and LMC_DCLK_CNT_*
                                                         CSR's. SW should write this to a one, then re-write
                                                         it to a zero to cause the reset. */
        uint64_t silo_qc                 : 1;       /**< Adds a Quarter Cycle granularity to generate
                                                         dqs pulse generation for silo.
                                                         Combination of Silo_HC and Silo_QC gives the
                                                         ability to position the read enable with quarter
                                                         cycle resolution. This is applied on all the bytes
                                                         uniformly.
                                                         NOTE - This bit has NO effect in PASS1 */
        uint64_t bunk_ena                : 1;       /**< Bunk Enable aka RANK ena (for use with dual-rank DIMMs)
                                                         For dual-rank DIMMs, the bunk_ena bit will enable
                                                         the drive of the CS_N[1:0] pins based on the
                                                         (pbank_lsb-1) address bit.
                                                         Write 0 for SINGLE ranked DIMM's. */
        uint64_t ded_err                 : 4;       /**< Double Error detected (DED) of Rd Data
                                                         In 128b mode, ecc is calulated on 1 cycle worth of data
                                                         [25] corresponds to DQ[63:0], Phase0
                                                         [26] corresponds to DQ[127:64], Phase0
                                                         [27] corresponds to DQ[63:0], Phase1
                                                         [28] corresponds to DQ[127:64], Phase1
                                                         In 64b mode, ecc is calculated on 2 cycle worth of data
                                                         [25] corresponds to DQ[63:0], Phase0, cycle0
                                                         [26] corresponds to DQ[63:0], Phase0, cycle1
                                                         [27] corresponds to DQ[63:0], Phase1, cycle0
                                                         [28] corresponds to DQ[63:0], Phase1, cycle1
                                                         Write of 1 will clear the corresponding error bit */
        uint64_t sec_err                 : 4;       /**< Single Error (corrected) of Rd Data
                                                         In 128b mode, ecc is calulated on 1 cycle worth of data
                                                         [21] corresponds to DQ[63:0], Phase0
                                                         [22] corresponds to DQ[127:64], Phase0
                                                         [23] corresponds to DQ[63:0], Phase1
                                                         [24] corresponds to DQ[127:64], Phase1
                                                         In 64b mode, ecc is calculated on 2 cycle worth of data
                                                         [21] corresponds to DQ[63:0], Phase0, cycle0
                                                         [22] corresponds to DQ[63:0], Phase0, cycle1
                                                         [23] corresponds to DQ[63:0], Phase1, cycle0
                                                         [24] corresponds to DQ[63:0], Phase1, cycle1
                                                         Write of 1 will clear the corresponding error bit */
        uint64_t intr_ded_ena            : 1;       /**< ECC Double Error Detect(DED) Interrupt Enable bit
                                                         When set, the memory controller raises a processor
                                                         interrupt on detecting an uncorrectable Dbl Bit ECC
                                                         error. */
        uint64_t intr_sec_ena            : 1;       /**< ECC Single Error Correct(SEC) Interrupt Enable bit
                                                         When set, the memory controller raises a processor
                                                         interrupt on detecting a correctable Single Bit ECC
                                                         error. */
        uint64_t tcl                     : 4;       /**< This register is not used in Pass2
                                                         and it has no effect on the controller's behavior */
        uint64_t ref_int                 : 6;       /**< Refresh interval represented in #of 512 dclk increments.
                                                         Program this to RND-DN(tREFI/clkPeriod/512)
                                                            000000: RESERVED
                                                            000001: 1 * 512  = 512 dclks
                                                             ...
                                                            111111: 63 * 512 = 32256 dclks */
        uint64_t pbank_lsb               : 4;       /**< Physical Bank address select
                                                                                 Reverting to the explanation for ROW_LSB,
                                                                                 PBank_LSB would be Row_LSB bit + #rowbits
                                                                                 + #rankbits
                                                                                 In the 512MB DIMM Example, assuming no rank bits:
                                                                                 pbank_lsb=mem_addr[15+13] for 64 b mode
                                                                                          =mem_addr[16+13] for 128b mode
                                                                                 Hence the parameter
                                                         0000:pbank[1:0] = mem_adr[28:27]    / rank = mem_adr[26] (if bunk_ena)
                                                         0001:pbank[1:0] = mem_adr[29:28]    / rank = mem_adr[27]      "
                                                         0010:pbank[1:0] = mem_adr[30:29]    / rank = mem_adr[28]      "
                                                         0011:pbank[1:0] = mem_adr[31:30]    / rank = mem_adr[29]      "
                                                         0100:pbank[1:0] = mem_adr[32:31]    / rank = mem_adr[30]      "
                                                         0101:pbank[1:0] = mem_adr[33:32]    / rank = mem_adr[31]      "
                                                         0110:pbank[1:0] =[1'b0,mem_adr[33]] / rank = mem_adr[32]      "
                                                         0111:pbank[1:0] =[2'b0]             / rank = mem_adr[33]      "
                                                         1000-1111: RESERVED */
        uint64_t row_lsb                 : 3;       /**< Encoding used to determine which memory address
                                                         bit position represents the low order DDR ROW address.
                                                         The processor's memory address[33:7] needs to be
                                                         translated to DRAM addresses (bnk,row,col,rank and dimm)
                                                         and that is a function of the following:
                                                         1. # Banks (4 or 8) - spec'd by BANK8
                                                         2. Datapath Width(64 or 128) - MODE128b
                                                         3. # Ranks in a DIMM - spec'd by BUNK_ENA
                                                         4. # DIMM's in the system
                                                         5. # Column Bits of the memory part - spec'd indirectly
                                                         by this register.
                                                         6. # Row Bits of the memory part - spec'd indirectly
                                                         by the register below (PBANK_LSB).
                                                         Illustration: For Micron's MT18HTF6472A,512MB DDR2
                                                         Unbuffered DIMM which uses 256Mb parts (8M x 8 x 4),
                                                         # Banks = 4 -> 2 bits of BA
                                                         # Columns = 1K -> 10 bits of Col
                                                         # Rows = 8K -> 13 bits of Row
                                                         Assuming that the total Data width is 128, this is how
                                                         we arrive at row_lsb:
                                                         Col Address starts from mem_addr[4] for 128b (16Bytes)
                                                         dq width or from mem_addr[3] for 64b (8Bytes) dq width
                                                         # col + # bank = 12. Hence row_lsb is mem_adr[15] for
                                                         64bmode or mem_adr[16] for 128b mode. Hence row_lsb
                                                         parameter should be set to 001 (64b) or 010 (128b).
                                                              000: row_lsb = mem_adr[14]
                                                              001: row_lsb = mem_adr[15]
                                                              010: row_lsb = mem_adr[16]
                                                              011: row_lsb = mem_adr[17]
                                                              100: row_lsb = mem_adr[18]
                                                              101-111:row_lsb = RESERVED */
        uint64_t ecc_ena                 : 1;       /**< ECC Enable: When set will enable the 8b ECC
                                                         check/correct logic. Should be 1 when used with DIMMs
                                                         with ECC. 0, otherwise.
                                                         When this mode is turned on, DQ[71:64] and DQ[143:137]
                                                         on writes, will contain the ECC code generated for
                                                         the lower 64 and upper 64 bits of data which will
                                                         written in the memory and then later on reads, used
                                                         to check for Single bit error (which will be auto-
                                                         corrected) and Double Bit error (which will be
                                                         reported). Please refer to SEC_ERR, DED_ERR,
                                                         LMC_FADR, and LMC_ECC_SYND registers
                                                         for diagnostics information when there is an error. */
        uint64_t init_start              : 1;       /**< A 0->1 transition starts the DDR memory initialization
                                                         sequence. */
#else
        uint64_t init_start              : 1;
        uint64_t ecc_ena                 : 1;
        uint64_t row_lsb                 : 3;
        uint64_t pbank_lsb               : 4;
        uint64_t ref_int                 : 6;
        uint64_t tcl                     : 4;
        uint64_t intr_sec_ena            : 1;
        uint64_t intr_ded_ena            : 1;
        uint64_t sec_err                 : 4;
        uint64_t ded_err                 : 4;
        uint64_t bunk_ena                : 1;
        uint64_t silo_qc                 : 1;
        uint64_t reset                   : 1;
        uint64_t reserved_32_63          : 32;
#endif
    } s;
    struct cvmx_lmc_mem_cfg0_s           cn3020;
    struct cvmx_lmc_mem_cfg0_s           cn30xx;
    struct cvmx_lmc_mem_cfg0_s           cn31xx;
    struct cvmx_lmc_mem_cfg0_s           cn36xx;
    struct cvmx_lmc_mem_cfg0_s           cn38xx;
    struct cvmx_lmc_mem_cfg0_s           cn38xxp2;
} cvmx_lmc_mem_cfg0_t;


/**
 * cvmx_lmc_mem_cfg1
 *
 * LMC_MEM_CFG1 = LMC Memory Configuration Register1
 * 
 * This register controls the External Memory Configuration Timing Parameters. Please refer to the
 * appropriate DDR1 or DDR2 part spec from your memory vendor for the various values in this CSR.
 * The details of each of these timing parameters can be found in the JEDEC spec or the vendor
 * spec of the memory parts.
 */
typedef union
{
    uint64_t u64;
    struct cvmx_lmc_mem_cfg1_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN
        uint64_t reserved_32_63          : 32;
        uint64_t comp_bypass             : 1;       /**< Compensation bypass. */
        uint64_t trrd                    : 3;       /**< tRRD cycles: ACT-ACT timing parameter for different
                                                         banks. (Represented in tCYC cycles == 1dclks)
                                                         TYP=15ns (66MHz=1,167MHz=3,200MHz=3)
                                                         For DDR2, TYP=7.5ns
                                                            000: RESERVED
                                                            001: 1 tCYC
                                                            010: 2 tCYC
                                                            011: 3 tCYC
                                                            100: 4 tCYC
                                                            101: 5 tCYC
                                                            110-111: RESERVED */
        uint64_t caslat                  : 3;       /**< CAS Latency Encoding which is loaded into each DDR
                                                         SDRAM device (MRS[6:4]) upon power-up (INIT_START=1).
                                                         (Represented in tCYC cycles == 1 dclks)
                                                            000 RESERVED
                                                            001 RESERVED
                                                            010 2.0 tCYC
                                                            011 3.0 tCYC
                                                            100 4.0 tCYC
                                                            101 5.0 tCYC
                                                            110 6.0 tCYC (DDR2)
                                                                2.5 tCYC (DDR1)
                                                            111 RESERVED
                                                         eg). The parameters TSKW, SILO_HC, and SILO_QC can
                                                         account for 1/4 cycle granularity in board/etch delays. */
        uint64_t tmrd                    : 3;       /**< tMRD Cycles
                                                         (Represented in dclk tCYC)
                                                         For DDR2, its TYP 2*tCYC)
                                                             000: RESERVED
                                                             001: 1
                                                             010: 2
                                                             011: 3
                                                             100: 4
                                                             101-111: RESERVED */
        uint64_t trfc                    : 5;       /**< 1/4 tRFC Cycles = RNDUP[tRFC(ns)/4*tcyc(ns)]
                                                         (Represented in tCYC cycles == 1dclks)
                                                         For DDR-I, the following encodings are used
                                                         TYP=70ns (133MHz - 3; 333MHz - 6)
                                                         For 2Gb, DDR2-667 parts, typ=195ns
                                                         (TRFC = 195/3/4 = 5'd17 = 0x11)
                                                             00000-00001: RESERVED
                                                             00010: 8
                                                             00011: 12
                                                             00100: 16
                                                             ...
                                                             11110: 120
                                                             11111: 124 */
        uint64_t trp                     : 4;       /**< tRP Cycles = RNDUP[tRP(ns)/tcyc(ns)]
                                                         (Represented in tCYC cycles == 1dclk)
                                                         TYP=15ns (66MHz=1,167MHz=3,400MHz=6 for TYP)
                                                             0000: RESERVED
                                                             0001: 1
                                                             ..
                                                             0111: 7
                                                             1000-1111: RESERVED
                                                         When using parts with 8 banks (LMC_DDR2_CTL->BANK8
                                                         is 1), load tRP cycles + 1 into this register. */
        uint64_t twtr                    : 4;       /**< tWTR Cycles = RNDUP[tWTR(ns)/tcyc(ns)]
                                                         Last Wr Data to Rd Command time.
                                                         (Represented in tCYC cycles == 1dclks)
                                                         TYP=15ns (66MHz=1,167MHz=3,400MHz=6, for TYP)
                                                             0000: RESERVED
                                                             0001: 1
                                                             ..
                                                             0111: 7
                                                             1000-1111: RESERVED */
        uint64_t trcd                    : 4;       /**< tRCD Cycles = RNDUP[tRCD(ns)/tcyc(ns)]
                                                         (Represented in tCYC cycles == 1dclk)
                                                         TYP=15ns (66MHz=1,167MHz=3,400MHz=6 for TYP)
                                                             0000: RESERVED
                                                             0001: 2 (2 is the smallest value allowed)
                                                             0002: 2
                                                             ..
                                                             0111: 7
                                                             1110-1111: RESERVED
                                                         In 2T mode, make this register TRCD-1, not going
                                                         below 2. */
        uint64_t tras                    : 5;       /**< tRAS Cycles = RNDUP[tRAS(ns)/tcyc(ns)]
                                                         (Represented in tCYC cycles == 1 dclk)
                                                         For DDR-I mode:
                                                         TYP=45ns (66MHz=3,167MHz=8,400MHz=18
                                                             00000-0001: RESERVED
                                                             00010: 2
                                                             ..
                                                             10100: 20
                                                             10101-11111: RESERVED */
#else
        uint64_t tras                    : 5;
        uint64_t trcd                    : 4;
        uint64_t twtr                    : 4;
        uint64_t trp                     : 4;
        uint64_t trfc                    : 5;
        uint64_t tmrd                    : 3;
        uint64_t caslat                  : 3;
        uint64_t trrd                    : 3;
        uint64_t comp_bypass             : 1;
        uint64_t reserved_32_63          : 32;
#endif
    } s;
    struct cvmx_lmc_mem_cfg1_s           cn3020;
    struct cvmx_lmc_mem_cfg1_s           cn30xx;
    struct cvmx_lmc_mem_cfg1_s           cn31xx;
    struct cvmx_lmc_mem_cfg1_cn36xx
    {
#if __BYTE_ORDER == __BIG_ENDIAN
        uint64_t reserved_31_63          : 33;
        uint64_t trrd                    : 3;       /**< tRRD cycles: ACT-ACT timing parameter for different
                                                         banks. (Represented in tCYC cycles == 1dclks)
                                                         TYP=15ns (66MHz=1,167MHz=3,200MHz=3)
                                                         For DDR2, TYP=7.5ns
                                                            000: RESERVED
                                                            001: 1 tCYC
                                                            010: 2 tCYC
                                                            011: 3 tCYC
                                                            100: 4 tCYC
                                                            101: 5 tCYC
                                                            110-111: RESERVED */
        uint64_t caslat                  : 3;       /**< CAS Latency Encoding which is loaded into each DDR
                                                         SDRAM device (MRS[6:4]) upon power-up (INIT_START=1).
                                                         (Represented in tCYC cycles == 1 dclks)
                                                            000 RESERVED
                                                            001 RESERVED
                                                            010 2.0 tCYC
                                                            011 3.0 tCYC
                                                            100 4.0 tCYC
                                                            101 5.0 tCYC
                                                            110 6.0 tCYC (DDR2)
                                                                2.5 tCYC (DDR1)
                                                            111 RESERVED
                                                         eg). The parameters TSKW, SILO_HC, and SILO_QC can
                                                         account for 1/4 cycle granularity in board/etch delays. */
        uint64_t tmrd                    : 3;       /**< tMRD Cycles
                                                         (Represented in dclk tCYC)
                                                         For DDR2, its TYP 2*tCYC)
                                                             000: RESERVED
                                                             001: 1
                                                             010: 2
                                                             011: 3
                                                             100: 4
                                                             101-111: RESERVED */
        uint64_t trfc                    : 5;       /**< 1/4 tRFC Cycles = RNDUP[tRFC(ns)/4*tcyc(ns)]
                                                         (Represented in tCYC cycles == 1dclks)
                                                         For DDR-I, the following encodings are used
                                                         TYP=70ns (133MHz - 3; 333MHz - 6)
                                                         For 2Gb, DDR2-667 parts, typ=195ns
                                                         (TRFC = 195/3/4 = 5'd17 = 0x11)
                                                             00000-00001: RESERVED
                                                             00010: 8
                                                             00011: 12
                                                             00100: 16
                                                             ...
                                                             11110: 120
                                                             11111: 124 */
        uint64_t trp                     : 4;       /**< tRP Cycles = RNDUP[tRP(ns)/tcyc(ns)]
                                                         (Represented in tCYC cycles == 1dclk)
                                                         TYP=15ns (66MHz=1,167MHz=3,400MHz=6 for TYP)
                                                             0000: RESERVED
                                                             0001: 1
                                                             ..
                                                             0111: 7
                                                             1000-1111: RESERVED
                                                         When using parts with 8 banks (LMC_DDR2_CTL->BANK8
                                                         is 1), load tRP cycles + 1 into this register. */
        uint64_t twtr                    : 4;       /**< tWTR Cycles = RNDUP[tWTR(ns)/tcyc(ns)]
                                                         Last Wr Data to Rd Command time.
                                                         (Represented in tCYC cycles == 1dclks)
                                                         TYP=15ns (66MHz=1,167MHz=3,400MHz=6, for TYP)
                                                             0000: RESERVED
                                                             0001: 1
                                                             ..
                                                             0111: 7
                                                             1000-1111: RESERVED */
        uint64_t trcd                    : 4;       /**< tRCD Cycles = RNDUP[tRCD(ns)/tcyc(ns)]
                                                         (Represented in tCYC cycles == 1dclk)
                                                         TYP=15ns (66MHz=1,167MHz=3,400MHz=6 for TYP)
                                                             0000: RESERVED
                                                             0001: 2 (2 is the smallest value allowed)
                                                             0002: 2
                                                             ..
                                                             0111: 7
                                                             1110-1111: RESERVED
                                                         In 2T mode, make this register TRCD-1, not going
                                                         below 2. */
        uint64_t tras                    : 5;       /**< tRAS Cycles = RNDUP[tRAS(ns)/tcyc(ns)]
                                                         (Represented in tCYC cycles == 1 dclk)
                                                         For DDR-I mode:
                                                         TYP=45ns (66MHz=3,167MHz=8,400MHz=18
                                                             00000-0001: RESERVED
                                                             00010: 2
                                                             ..
                                                             10100: 20
                                                             10101-11111: RESERVED */
#else
        uint64_t tras                    : 5;
        uint64_t trcd                    : 4;
        uint64_t twtr                    : 4;
        uint64_t trp                     : 4;
        uint64_t trfc                    : 5;
        uint64_t tmrd                    : 3;
        uint64_t caslat                  : 3;
        uint64_t trrd                    : 3;
        uint64_t reserved_31_63          : 33;
#endif
    } cn36xx;
    struct cvmx_lmc_mem_cfg1_cn36xx      cn38xx;
    struct cvmx_lmc_mem_cfg1_cn36xx      cn38xxp2;
} cvmx_lmc_mem_cfg1_t;


/**
 * cvmx_lmc_ops_cnt_hi
 *
 * LMC_OPS_CNT_HI  = Performance Counters
 *
 */
typedef union
{
    uint64_t u64;
    struct cvmx_lmc_ops_cnt_hi_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN
        uint64_t reserved_32_63          : 32;
        uint64_t opscnt_hi               : 32;      /**< Performance Counter to measure Bus Utilization
                                                         Upper 32-bits of 64-bit counter
                                                           DRAM bus utilization = LMC_OPS_CNT_* /LMC_DCLK_CNT_* */
#else
        uint64_t opscnt_hi               : 32;
        uint64_t reserved_32_63          : 32;
#endif
    } s;
    struct cvmx_lmc_ops_cnt_hi_s         cn3020;
    struct cvmx_lmc_ops_cnt_hi_s         cn30xx;
    struct cvmx_lmc_ops_cnt_hi_s         cn31xx;
    struct cvmx_lmc_ops_cnt_hi_s         cn36xx;
    struct cvmx_lmc_ops_cnt_hi_s         cn38xx;
    struct cvmx_lmc_ops_cnt_hi_s         cn38xxp2;
} cvmx_lmc_ops_cnt_hi_t;


/**
 * cvmx_lmc_ops_cnt_lo
 *
 * LMC_OPS_CNT_LO  = Performance Counters
 *
 */
typedef union
{
    uint64_t u64;
    struct cvmx_lmc_ops_cnt_lo_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN
        uint64_t reserved_32_63          : 32;
        uint64_t opscnt_lo               : 32;      /**< Performance Counter
                                                         Low 32-bits of 64-bit counter
                                                           DRAM bus utilization = LMC_OPS_CNT_* /LMC_DCLK_CNT_* */
#else
        uint64_t opscnt_lo               : 32;
        uint64_t reserved_32_63          : 32;
#endif
    } s;
    struct cvmx_lmc_ops_cnt_lo_s         cn3020;
    struct cvmx_lmc_ops_cnt_lo_s         cn30xx;
    struct cvmx_lmc_ops_cnt_lo_s         cn31xx;
    struct cvmx_lmc_ops_cnt_lo_s         cn36xx;
    struct cvmx_lmc_ops_cnt_lo_s         cn38xx;
    struct cvmx_lmc_ops_cnt_lo_s         cn38xxp2;
} cvmx_lmc_ops_cnt_lo_t;


/**
 * cvmx_lmc_pll_bwctl
 *
 * LMC_PLL_BWCTL  = DDR PLL Bandwidth Control Register
 *
 */
typedef union
{
    uint64_t u64;
    struct cvmx_lmc_pll_bwctl_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN
        uint64_t reserved_5_63           : 59;
        uint64_t bwupd                   : 1;       /**< Load this Bandwidth Register value into the PLL */
        uint64_t bwctl                   : 4;       /**< Bandwidth Control Register for DDR PLL */
#else
        uint64_t bwctl                   : 4;
        uint64_t bwupd                   : 1;
        uint64_t reserved_5_63           : 59;
#endif
    } s;
    struct cvmx_lmc_pll_bwctl_s          cn3020;
    struct cvmx_lmc_pll_bwctl_s          cn30xx;
    struct cvmx_lmc_pll_bwctl_s          cn31xx;
    struct cvmx_lmc_pll_bwctl_s          cn36xx;
    struct cvmx_lmc_pll_bwctl_s          cn38xx;
    struct cvmx_lmc_pll_bwctl_s          cn38xxp2;
} cvmx_lmc_pll_bwctl_t;


/**
 * cvmx_lmc_rodt_ctl
 *
 * LMC_RODT_CTL = LMC Read OnDieTermination control (PASS2 ONLY)
 * See Description on LMC_WODT_CTL - same applies for masks on READs. ODT_ENA also needs to be a 1
 * for Octeon to wiggle the ODT pins on reads.
 *
 * Notes:
 * When a given RANK in position N is selected, the RODT _HI and _LO masks for that position are used.
 * Mask[3:0] is used for RODT control of the RANKs in positions 3, 2, 1, and 0, respectively.
 * In  64b mode, DIMMs are assumed to be ordered in the following order:
 *  position 3: [unused        , DIMM1_RANK1_LO]
 *  position 2: [unused        , DIMM1_RANK0_LO]
 *  position 1: [unused        , DIMM0_RANK1_LO]
 *  position 0: [unused        , DIMM0_RANK0_LO]
 * In 128b mode, DIMMs are assumed to be ordered in the following order:
 *  position 3: [DIMM3_RANK1_HI, DIMM1_RANK1_LO]
 *  position 2: [DIMM3_RANK0_HI, DIMM1_RANK0_LO]
 *  position 1: [DIMM2_RANK1_HI, DIMM0_RANK1_LO]
 *  position 0: [DIMM2_RANK0_HI, DIMM0_RANK0_LO]
 */
typedef union
{
    uint64_t u64;
    struct cvmx_lmc_rodt_ctl_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN
        uint64_t reserved_32_63          : 32;
        uint64_t rodt_hi3                : 4;       /**< Read ODT mask for position 3, data[127:64] */
        uint64_t rodt_hi2                : 4;       /**< Read ODT mask for position 2, data[127:64] */
        uint64_t rodt_hi1                : 4;       /**< Read ODT mask for position 1, data[127:64] */
        uint64_t rodt_hi0                : 4;       /**< Read ODT mask for position 0, data[127:64] */
        uint64_t rodt_lo3                : 4;       /**< Read ODT mask for position 3, data[ 63: 0] */
        uint64_t rodt_lo2                : 4;       /**< Read ODT mask for position 2, data[ 63: 0] */
        uint64_t rodt_lo1                : 4;       /**< Read ODT mask for position 1, data[ 63: 0] */
        uint64_t rodt_lo0                : 4;       /**< Read ODT mask for position 0, data[ 63: 0] */
#else
        uint64_t rodt_lo0                : 4;
        uint64_t rodt_lo1                : 4;
        uint64_t rodt_lo2                : 4;
        uint64_t rodt_lo3                : 4;
        uint64_t rodt_hi0                : 4;
        uint64_t rodt_hi1                : 4;
        uint64_t rodt_hi2                : 4;
        uint64_t rodt_hi3                : 4;
        uint64_t reserved_32_63          : 32;
#endif
    } s;
    struct cvmx_lmc_rodt_ctl_s           cn3020;
    struct cvmx_lmc_rodt_ctl_s           cn30xx;
    struct cvmx_lmc_rodt_ctl_s           cn31xx;
    struct cvmx_lmc_rodt_ctl_s           cn36xx;
    struct cvmx_lmc_rodt_ctl_s           cn38xx;
    struct cvmx_lmc_rodt_ctl_s           cn38xxp2;
} cvmx_lmc_rodt_ctl_t;


/**
 * cvmx_lmc_wodt_ctl
 *
 * LMC_WODT_CTL = LMC Write OnDieTermination control
 * System designers may desire to terminate DQ/DQS/DM lines for higher frequency DDR operations
 * (667MHz and faster), especially on a multi-rank system. DDR2 DQ/DM/DQS I/O's have built in
 * Termination resistor that cab turned on or off by the controller, after meeting tAOND and tAOF
 * timing requirements. Each Rank has its own ODT pin that fans out to all the memory parts
 * in that DIMM. System designers may prefer different combinations of ODT ON's for read and write
 * into different ranks. Octeon supports full programmability by way of the mask register below.
 * Each Rank position has its own 4/8-bit programmable field (depending on 64/128b mode).
 * When the controller does a write to that rank, it sets the 4/8 ODT pins to the MASK pins below.
 * For eg., When doing a write into Rank0, a system designer may desire to terminate the lines
 * with the resistor on Rank1. The mask WODT_HI0 and WODT_LO0 would then be [0010 & 0010].
 * If ODT feature is not desired, the DDR parts can be programmed to not look at these pins by
 * writing 0 in QS_DIC. Octeon drives the appropriate mask values on the ODT pins by default.
 * If this feature is not required, write 0 in this register.
 *
 * Notes:
 * When a given RANK in position N is selected, the WODT _HI and _LO masks for that position are used.
 * Mask[3:0] is used for WODT control of the RANKs in positions 3, 2, 1, and 0, respectively.
 * In  64b mode, DIMMs are assumed to be ordered in the following order:
 *  position 3: [unused        , DIMM1_RANK1_LO]
 *  position 2: [unused        , DIMM1_RANK0_LO]
 *  position 1: [unused        , DIMM0_RANK1_LO]
 *  position 0: [unused        , DIMM0_RANK0_LO]
 * In 128b mode, DIMMs are assumed to be ordered in the following order:
 *  position 3: [DIMM3_RANK1_HI, DIMM1_RANK1_LO]
 *  position 2: [DIMM3_RANK0_HI, DIMM1_RANK0_LO]
 *  position 1: [DIMM2_RANK1_HI, DIMM0_RANK1_LO]
 *  position 0: [DIMM2_RANK0_HI, DIMM0_RANK0_LO]
 */
typedef union
{
    uint64_t u64;
    struct cvmx_lmc_wodt_ctl_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN
        uint64_t reserved_32_63          : 32;
        uint64_t wodt_hi3                : 4;       /**< Write ODT mask for position 3, data[127:64] */
        uint64_t wodt_hi2                : 4;       /**< Write ODT mask for position 2, data[127:64] */
        uint64_t wodt_hi1                : 4;       /**< Write ODT mask for position 1, data[127:64] */
        uint64_t wodt_hi0                : 4;       /**< Write ODT mask for position 0, data[127:64] */
        uint64_t wodt_lo3                : 4;       /**< Write ODT mask for position 3, data[ 63: 0] */
        uint64_t wodt_lo2                : 4;       /**< Write ODT mask for position 2, data[ 63: 0] */
        uint64_t wodt_lo1                : 4;       /**< Write ODT mask for position 1, data[ 63: 0] */
        uint64_t wodt_lo0                : 4;       /**< Write ODT mask for position 0, data[ 63: 0] */
#else
        uint64_t wodt_lo0                : 4;
        uint64_t wodt_lo1                : 4;
        uint64_t wodt_lo2                : 4;
        uint64_t wodt_lo3                : 4;
        uint64_t wodt_hi0                : 4;
        uint64_t wodt_hi1                : 4;
        uint64_t wodt_hi2                : 4;
        uint64_t wodt_hi3                : 4;
        uint64_t reserved_32_63          : 32;
#endif
    } s;
    struct cvmx_lmc_wodt_ctl_s           cn36xx;
    struct cvmx_lmc_wodt_ctl_s           cn38xx;
    struct cvmx_lmc_wodt_ctl_s           cn38xxp2;
} cvmx_lmc_wodt_ctl_t;


/**
 * cvmx_lmc_wodt_ctl0
 *
 * LMC_WODT_CTL0 = LMC Write OnDieTermination control
 * See the description in LMC_WODT_CTL1.
 *
 * Notes:
 * Together, the LMC_WODT_CTL1 and LMC_WODT_CTL0 CSRs control the write ODT mask.  See LMC_WODT_CTL1.
 *
 */
typedef union
{
    uint64_t u64;
    struct cvmx_lmc_wodt_ctl0_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN
        uint64_t reserved_32_63          : 32;
        uint64_t wodt_d1_r1              : 8;       /**< Write ODT mask DIMM1, RANK1/DIMM1 in SingleRanked */
        uint64_t wodt_d1_r0              : 8;       /**< Write ODT mask DIMM1, RANK0 */
        uint64_t wodt_d0_r1              : 8;       /**< Write ODT mask DIMM0, RANK1/DIMM0 in SingleRanked */
        uint64_t wodt_d0_r0              : 8;       /**< Write ODT mask DIMM0, RANK0 */
#else
        uint64_t wodt_d0_r0              : 8;
        uint64_t wodt_d0_r1              : 8;
        uint64_t wodt_d1_r0              : 8;
        uint64_t wodt_d1_r1              : 8;
        uint64_t reserved_32_63          : 32;
#endif
    } s;
    struct cvmx_lmc_wodt_ctl0_s          cn3020;
    struct cvmx_lmc_wodt_ctl0_s          cn30xx;
    struct cvmx_lmc_wodt_ctl0_s          cn31xx;
} cvmx_lmc_wodt_ctl0_t;


/**
 * cvmx_lmc_wodt_ctl1
 *
 * LMC_WODT_CTL1 = LMC Write OnDieTermination control
 * System designers may desire to terminate DQ/DQS/DM lines for higher frequency DDR operations
 * (667MHz and faster), especially on a multi-rank system. DDR2 DQ/DM/DQS I/O's have built in
 * Termination resistor that cab turned on or off by the controller, after meeting tAOND and tAOF
 * timing requirements. Each Rank has its own ODT pin that fans out to all the memory parts
 * in that DIMM. System designers may prefer different combinations of ODT ON's for read and write
 * into different ranks. Octeon supports full programmability by way of the mask register below.
 * Each Rank position has its own 8-bit programmable field.
 * When the controller does a write to that rank, it sets the 8 ODT pins to the MASK pins below.
 * For eg., When doing a write into Rank0, a system designer may desire to terminate the lines
 * with the resistor on Dimm0/Rank1. The mask WODT_D0_R0 would then be [00000010].
 * If ODT feature is not desired, the DDR parts can be programmed to not look at these pins by
 * writing 0 in QS_DIC. Octeon drives the appropriate mask values on the ODT pins by default.
 * If this feature is not required, write 0 in this register.
 *
 * Notes:
 * Together, the LMC_WODT_CTL1 and LMC_WODT_CTL0 CSRs control the write ODT mask.
 * When a given RANK is selected, the WODT mask for that RANK is used.  The resulting WODT mask is
 * driven to the DIMMs in the following manner:
 * Mask[7] -> DIMM3, RANK1
 * Mask[6] -> DIMM3, RANK0
 * Mask[5] -> DIMM2, RANK1
 * Mask[4] -> DIMM2, RANK0
 * Mask[3] -> DIMM1, RANK1
 * Mask[2] -> DIMM1, RANK0
 * Mask[1] -> DIMM0, RANK1
 * Mask[0] -> DIMM0, RANK0
 */
typedef union
{
    uint64_t u64;
    struct cvmx_lmc_wodt_ctl1_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN
        uint64_t reserved_32_63          : 32;
        uint64_t wodt_d3_r1              : 8;       /**< Write ODT mask DIMM3, RANK1/DIMM3 in SingleRanked */
        uint64_t wodt_d3_r0              : 8;       /**< Write ODT mask DIMM3, RANK0 */
        uint64_t wodt_d2_r1              : 8;       /**< Write ODT mask DIMM2, RANK1/DIMM2 in SingleRanked */
        uint64_t wodt_d2_r0              : 8;       /**< Write ODT mask DIMM2, RANK0 */
#else
        uint64_t wodt_d2_r0              : 8;
        uint64_t wodt_d2_r1              : 8;
        uint64_t wodt_d3_r0              : 8;
        uint64_t wodt_d3_r1              : 8;
        uint64_t reserved_32_63          : 32;
#endif
    } s;
    struct cvmx_lmc_wodt_ctl1_s          cn3020;
    struct cvmx_lmc_wodt_ctl1_s          cn30xx;
    struct cvmx_lmc_wodt_ctl1_s          cn31xx;
} cvmx_lmc_wodt_ctl1_t;






/**
 * cvmx_l2d_fus3
 *
 * L2D_FUS3 = L2C Data Store QUAD3 Fuse Register
 *
 */
typedef union
{
    uint64_t u64;
    struct cvmx_l2d_fus3_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN
        uint64_t reserved_36_63          : 28;
        uint64_t crip_256k               : 1;       /**< This is purely for debug and not needed in the general
                                                         manufacturing flow.
                                                         If the FUSE is not-blown, then this bit should read
                                                         as 0. If the FUSE is blown, then this bit should read
                                                         as 1.
                                                         *** NOTE: Pass2 Addition */
        uint64_t reserved_34_34          : 1;
        uint64_t q3fus                   : 34;      /**< Fuse Register for QUAD3
                                                         This is purely for debug and not needed in the general
                                                         manufacturing flow.
                                                         Note that the fuses are complementary (Assigning a
                                                         fuse to 1 will read as a zero). This means the case
                                                         where no fuses are blown result in these csr's showing
                                                         all ones.
                                                          Failure #1 Fuse Mapping
                                                             [16:14] bad bank
                                                             [13:7] bad high column
                                                             [6:0] bad low column
                                                           Failure #2 Fuse Mapping
                                                             [33:31] bad bank
                                                             [30:24] bad high column
                                                             [23:17] bad low column */
#else
        uint64_t q3fus                   : 34;
        uint64_t reserved_34_34          : 1;
        uint64_t crip_256k               : 1;
        uint64_t reserved_36_63          : 28;
#endif
    } s;
    struct cvmx_l2d_fus3_cn3020
    {
#if __BYTE_ORDER == __BIG_ENDIAN
        uint64_t reserved_35_63          : 29;
        uint64_t crip_128k               : 1;       /**< This is purely for debug and not needed in the general
                                                         manufacturing flow.
                                                         If the FUSE is not-blown, then this bit should read
                                                         as 0. If the FUSE is blown, then this bit should read
                                                         as 1. */
        uint64_t q3fus                   : 34;      /**< Fuse Register for QUAD3
                                                         This is purely for debug and not needed in the general
                                                         manufacturing flow.
                                                         Note that the fuses are complementary (Assigning a
                                                         fuse to 1 will read as a zero). This means the case
                                                         where no fuses are blown result in these csr's showing
                                                         all ones.
                                                          Failure #1 Fuse Mapping
                                                             [16:15] UNUSED
                                                             [14]    bad bank
                                                             [13:7] bad high column
                                                             [6:0] bad low column
                                                           Failure #2 Fuse Mapping
                                                             [33:32] UNUSED
                                                             [31]    bad bank
                                                             [30:24] bad high column
                                                             [23:17] bad low column */
#else
        uint64_t q3fus                   : 34;
        uint64_t crip_128k               : 1;
        uint64_t reserved_35_63          : 29;
#endif
    } cn3020;
    struct cvmx_l2d_fus3_cn3020          cn30xx;
    struct cvmx_l2d_fus3_cn3020          cn31xx;
    struct cvmx_l2d_fus3_cn36xx
    {
#if __BYTE_ORDER == __BIG_ENDIAN
        uint64_t reserved_36_63          : 28;
        uint64_t crip_256k               : 1;       /**< This is purely for debug and not needed in the general
                                                         manufacturing flow.
                                                         If the FUSE is not-blown, then this bit should read
                                                         as 0. If the FUSE is blown, then this bit should read
                                                         as 1.
                                                         *** NOTE: Pass2 Addition */
        uint64_t crip_512k               : 1;       /**< This is purely for debug and not needed in the general
                                                         manufacturing flow.
                                                         If the FUSE is not-blown, then this bit should read
                                                         as 0. If the FUSE is blown, then this bit should read
                                                         as 1.
                                                         *** NOTE: Pass2 Addition */
        uint64_t q3fus                   : 34;      /**< Fuse Register for QUAD3
                                                         This is purely for debug and not needed in the general
                                                         manufacturing flow.
                                                         Note that the fuses are complementary (Assigning a
                                                         fuse to 1 will read as a zero). This means the case
                                                         where no fuses are blown result in these csr's showing
                                                         all ones.
                                                          Failure #1 Fuse Mapping
                                                             [16:14] bad bank
                                                             [13:7] bad high column
                                                             [6:0] bad low column
                                                           Failure #2 Fuse Mapping
                                                             [33:31] bad bank
                                                             [30:24] bad high column
                                                             [23:17] bad low column */
#else
        uint64_t q3fus                   : 34;
        uint64_t crip_512k               : 1;
        uint64_t crip_256k               : 1;
        uint64_t reserved_36_63          : 28;
#endif
    } cn36xx;
    struct cvmx_l2d_fus3_cn36xx          cn38xx;
    struct cvmx_l2d_fus3_cn36xx          cn38xxp2;
} cvmx_l2d_fus3_t;


/**
 * cvmx_npi_mem_access_subid#
 *
 * NPI_MEM_ACCESS_SUBID3 = Memory Access SubId 3Register
 * 
 * Carries Read/Write parameters for PP access to PCI memory that use NPI SubId3.
 * Writes to this register are not ordered with writes/reads to the PCI Memory space.
 * To ensure that a write has completed the user must read the register before
 * making an access(i.e. PCI memory space) that requires the value of this register to be updated.
 */
typedef union
{
    uint64_t u64;
    struct cvmx_npi_mem_access_subidx_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN
        uint64_t reserved_38_63          : 26;
        uint64_t shortl                  : 1;       /**< Generate CMD-6 on PCI(x) when '1'.
                                                         Loads from the cores to the corresponding subid
                                                         that are 32-bits or smaller:
                                                         - Will generate the PCI-X "Memory Read DWORD"
                                                           command in PCI-X mode. (Note that "Memory
                                                           Read DWORD" appears much like an IO read on
                                                           the PCI-X bus.)
                                                         - Will generate the PCI "Memory Read" command
                                                           in PCI-X mode, irrespective of the
                                                           NPI_PCI_READ_CMD[CMD_SIZE] value.
                                                         NOT IN PASS 1 NOR PASS 2 */
        uint64_t nmerge                  : 1;       /**< No Merge. (NOT IN PASS 1 NOR PASS 2) */
        uint64_t esr                     : 2;       /**< Endian-Swap on read. */
        uint64_t esw                     : 2;       /**< Endian-Swap on write. */
        uint64_t nsr                     : 1;       /**< No-Snoop on read. */
        uint64_t nsw                     : 1;       /**< No-Snoop on write. */
        uint64_t ror                     : 1;       /**< Relax Read on read. */
        uint64_t row                     : 1;       /**< Relax Order on write. */
        uint64_t ba                      : 28;      /**< PCI Address bits [63:36]. */
#else
        uint64_t ba                      : 28;
        uint64_t row                     : 1;
        uint64_t ror                     : 1;
        uint64_t nsw                     : 1;
        uint64_t nsr                     : 1;
        uint64_t esw                     : 2;
        uint64_t esr                     : 2;
        uint64_t nmerge                  : 1;
        uint64_t shortl                  : 1;
        uint64_t reserved_38_63          : 26;
#endif
    } s;
    struct cvmx_npi_mem_access_subidx_cn3020
    {
#if __BYTE_ORDER == __BIG_ENDIAN
        uint64_t reserved_36_63          : 28;
        uint64_t esr                     : 2;       /**< Endian-Swap on read. */
        uint64_t esw                     : 2;       /**< Endian-Swap on write. */
        uint64_t nsr                     : 1;       /**< No-Snoop on read. */
        uint64_t nsw                     : 1;       /**< No-Snoop on write. */
        uint64_t ror                     : 1;       /**< Relax Read on read. */
        uint64_t row                     : 1;       /**< Relax Order on write. */
        uint64_t ba                      : 28;      /**< PCI Address bits [63:36]. */
#else
        uint64_t ba                      : 28;
        uint64_t row                     : 1;
        uint64_t ror                     : 1;
        uint64_t nsw                     : 1;
        uint64_t nsr                     : 1;
        uint64_t esw                     : 2;
        uint64_t esr                     : 2;
        uint64_t reserved_36_63          : 28;
#endif
    } cn3020;
    struct cvmx_npi_mem_access_subidx_s  cn30xx;
    struct cvmx_npi_mem_access_subidx_cn3020 cn31xx;
    struct cvmx_npi_mem_access_subidx_s  cn36xx;
    struct cvmx_npi_mem_access_subidx_s  cn38xx;
    struct cvmx_npi_mem_access_subidx_cn3020 cn38xxp2;
    struct cvmx_npi_mem_access_subidx_s  cn58xx;
} cvmx_npi_mem_access_subidx_t;


typedef union
{
    uint64_t u64;
    struct cvmx_pip_sft_rst_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN
        uint64_t reserved_1_63           : 63;
        uint64_t rst                     : 1;       /**< Soft Reset */
#else
        uint64_t rst                     : 1;
        uint64_t reserved_1_63           : 63;
#endif
    } s;
    struct cvmx_pip_sft_rst_s            cn3020;
    struct cvmx_pip_sft_rst_s            cn30xx;
    struct cvmx_pip_sft_rst_s            cn31xx;
    struct cvmx_pip_sft_rst_s            cn36xx;
    struct cvmx_pip_sft_rst_s            cn38xx;
    struct cvmx_pip_sft_rst_s            cn56xx;
    struct cvmx_pip_sft_rst_s            cn58xx;
} cvmx_pip_sft_rst_t;
#endif

/*
 * cvmx_lmc_rodt_comp_ctl
 *
 * LMC_RODT_COMP_CTL = LMC Compensation control
 *
 */
typedef union
{
    uint64_t u64;
    struct cvmx_lmc_rodt_comp_ctl_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN
        uint64_t reserved_17_63          : 47;
        uint64_t enable                  : 1;       /**< 0=not enabled, 1=enable */
        uint64_t reserved_12_15          : 4;
        uint64_t nctl                    : 4;       /**< Compensation control bits */
        uint64_t reserved_5_7            : 3;
        uint64_t pctl                    : 5;       /**< Compensation control bits */
#else
        uint64_t pctl                    : 5;
        uint64_t reserved_5_7            : 3;
        uint64_t nctl                    : 4;
        uint64_t reserved_12_15          : 4;
        uint64_t enable                  : 1;
        uint64_t reserved_17_63          : 47;
#endif
    } s;
    struct cvmx_lmc_rodt_comp_ctl_s      cn50xx;
    struct cvmx_lmc_rodt_comp_ctl_s      cn56xx;
    struct cvmx_lmc_rodt_comp_ctl_s      cn58xx;
} cvmx_lmc_rodt_comp_ctl_t;


/**
 * cvmx_lmc_pll_ctl
 *
 * LMC_PLL_CTL = LMC pll control
 *
 *
 * Notes:
 * Exactly one of EN2, EN4, EN6, EN8, EN12, EN16 must be set.
 * 
 * The resultant DDR_CK frequency is the DDR2_REF_CLK
 * frequency multiplied by:
 * 
 *     (CLKF + 1) / ((CLKR + 1) * EN(2,4,6,8,12,16))
 * 
 * The PLL frequency, which is:
 * 
 *     (DDR2_REF_CLK freq) * ((CLKF + 1) / (CLKR + 1))
 * 
 * must reside between 1.2 and 2.5 GHz. A faster PLL frequency is desirable if there is a choice.
 */
typedef union
{
    uint64_t u64;
    struct cvmx_lmc_pll_ctl_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN
        uint64_t reserved_29_63          : 35;
        uint64_t fasten_n                : 1;       /**< when 0, 0 < CLKF <= 128
                                                         when 1, 0 < CLKF <= 256 */
        uint64_t div_reset               : 1;       /**< Analog pll divider reset
                                                         De-assert at least 500*(CLKR+1) reference clock
                                                         cycles following RESET_N de-assertion. */
        uint64_t reset_n                 : 1;       /**< Analog pll reset
                                                         De-assert at least 5 usec after CLKF, CLKR,
                                                         and EN* are set up. */
        uint64_t clkf                    : 12;      /**< Multiply reference by CLKF + 1 */
        uint64_t clkr                    : 6;       /**< Divide reference by CLKR + 1 */
        uint64_t reserved_6_7            : 2;
        uint64_t en16                    : 1;       /**< Divide output by 16 */
        uint64_t en12                    : 1;       /**< Divide output by 12 */
        uint64_t en8                     : 1;       /**< Divide output by 8 */
        uint64_t en6                     : 1;       /**< Divide output by 6 */
        uint64_t en4                     : 1;       /**< Divide output by 4 */
        uint64_t en2                     : 1;       /**< Divide output by 2 */
#else
        uint64_t en2                     : 1;
        uint64_t en4                     : 1;
        uint64_t en6                     : 1;
        uint64_t en8                     : 1;
        uint64_t en12                    : 1;
        uint64_t en16                    : 1;
        uint64_t reserved_6_7            : 2;
        uint64_t clkr                    : 6;
        uint64_t clkf                    : 12;
        uint64_t reset_n                 : 1;
        uint64_t div_reset               : 1;
        uint64_t fasten_n                : 1;
        uint64_t reserved_29_63          : 35;
#endif
    } s;
    struct cvmx_lmc_pll_ctl_s            cn50xx;
    struct cvmx_lmc_pll_ctl_cn58xx
    {
#if __BYTE_ORDER == __BIG_ENDIAN
        uint64_t reserved_28_63          : 36;      /**< MBZ */
        uint64_t div_reset               : 1;       /**< Analog pll divider reset
                                                         De-assert at least 500*(CLKR+1) reference clock
                                                         cycles following RESET_N de-assertion. */
        uint64_t reset_n                 : 1;       /**< Analog pll reset
                                                         De-assert at least 5 usec after CLKF, CLKR,
                                                         and EN* are set up. */
        uint64_t clkf                    : 12;      /**< Multiply reference by CLKF + 1 */
        uint64_t clkr                    : 6;       /**< Divide reference by CLKR + 1 */
        uint64_t reserved_6_7            : 2;       /**< MBZ */
        uint64_t en16                    : 1;       /**< Divide output by 16 */
        uint64_t en12                    : 1;       /**< Divide output by 12 */
        uint64_t en8                     : 1;       /**< Divide output by 8 */
        uint64_t en6                     : 1;       /**< Divide output by 6 */
        uint64_t en4                     : 1;       /**< Divide output by 4 */
        uint64_t en2                     : 1;       /**< Divide output by 2 */
#else
        uint64_t en2                     : 1;
        uint64_t en4                     : 1;
        uint64_t en6                     : 1;
        uint64_t en8                     : 1;
        uint64_t en12                    : 1;
        uint64_t en16                    : 1;
        uint64_t reserved_6_7            : 2;
        uint64_t clkr                    : 6;
        uint64_t clkf                    : 12;
        uint64_t reset_n                 : 1;
        uint64_t div_reset               : 1;
        uint64_t reserved_28_63          : 36;
#endif
    } cn58xx;
} cvmx_lmc_pll_ctl_t;

/**
 * cvmx_pko_mem_debug8
 *
 * Notes:
 * Internal per-queue state intended for debug use only - pko_prt_qsb.state[91:64]
 * This CSR is a memory of 256 entries, and thus, the PKO_REG_READ_IDX CSR must be written before any
 * CSR read operations to this address can be performed.
 */
typedef union
{
    uint64_t u64;
    struct cvmx_pko_mem_debug8_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN
        uint64_t reserved_59_63          : 5;
        uint64_t tail                    : 1;       /**< This queue is the last (tail) in the queue array */
        uint64_t buf_siz                 : 13;      /**< Command buffer remaining size in words */
        uint64_t reserved_0_44           : 45;
#else
        uint64_t reserved_0_44           : 45;
        uint64_t buf_siz                 : 13;
        uint64_t tail                    : 1;
        uint64_t reserved_59_63          : 5;
#endif
    } s;
    struct cvmx_pko_mem_debug8_cn3020
    {
#if __BYTE_ORDER == __BIG_ENDIAN
        uint64_t qos                     : 5;       /**< QOS mask to enable the queue when set */
        uint64_t tail                    : 1;       /**< This queue is the last (tail) in the queue array */
        uint64_t buf_siz                 : 13;      /**< Command buffer remaining size in words */
        uint64_t buf_ptr                 : 33;      /**< Command word pointer */
        uint64_t qcb_widx                : 6;       /**< Buffer write index for QCB */
        uint64_t qcb_ridx                : 6;       /**< Buffer read  index for QCB */
#else
        uint64_t qcb_ridx                : 6;
        uint64_t qcb_widx                : 6;
        uint64_t buf_ptr                 : 33;
        uint64_t buf_siz                 : 13;
        uint64_t tail                    : 1;
        uint64_t qos                     : 5;
#endif
    } cn3020;
    struct cvmx_pko_mem_debug8_cn3020    cn30xx;
    struct cvmx_pko_mem_debug8_cn3020    cn31xx;
    struct cvmx_pko_mem_debug8_cn3020    cn36xx;
    struct cvmx_pko_mem_debug8_cn3020    cn38xx;
    struct cvmx_pko_mem_debug8_cn3020    cn38xxp2;
    struct cvmx_pko_mem_debug8_cn50xx
    {
#if __BYTE_ORDER == __BIG_ENDIAN
        uint64_t reserved_28_63          : 36;      /**< MBZ */
        uint64_t doorbell                : 20;      /**< Doorbell count */
        uint64_t reserved_6_7            : 2;
        uint64_t static_p                : 1;       /**< Static priority */
        uint64_t s_tail                  : 1;       /**< Static tail */
        uint64_t static_q                : 1;       /**< Static priority */
        uint64_t qos                     : 3;       /**< QOS mask to enable the queue when set */
#else
        uint64_t qos                     : 3;
        uint64_t static_q                : 1;
        uint64_t s_tail                  : 1;
        uint64_t static_p                : 1;
        uint64_t reserved_6_7            : 2;
        uint64_t doorbell                : 20;
        uint64_t reserved_28_63          : 36;
#endif
    } cn50xx;
    struct cvmx_pko_mem_debug8_cn50xx    cn56xx;
    struct cvmx_pko_mem_debug8_cn50xx    cn58xx;
} cvmx_pko_mem_debug8_t;




/**
 * cvmx_lmc#_pll_status
 *
 * LMC_PLL_STATUS = LMC pll status
 *
 */
typedef union
{
    uint64_t u64;
    struct cvmx_lmcx_pll_status_s
    {
#if __BYTE_ORDER == __BIG_ENDIAN
        uint64_t reserved_32_63          : 32;
        uint64_t ddr__nctl               : 5;       /**< DDR nctl from compensation circuit */
        uint64_t ddr__pctl               : 5;       /**< DDR pctl from compensation circuit */
        uint64_t reserved_2_21           : 20;
        uint64_t rfslip                  : 1;       /**< Reference clock slip */
        uint64_t fbslip                  : 1;       /**< Feedback clock slip */
#else
        uint64_t fbslip                  : 1;
        uint64_t rfslip                  : 1;
        uint64_t reserved_2_21           : 20;
        uint64_t ddr__pctl               : 5;
        uint64_t ddr__nctl               : 5;
        uint64_t reserved_32_63          : 32;
#endif
    } s;
    struct cvmx_lmcx_pll_status_s        cn50xx;
    struct cvmx_lmcx_pll_status_s        cn52xx;
    struct cvmx_lmcx_pll_status_s        cn56xx;
    struct cvmx_lmcx_pll_status_s        cn56xxp1;
    struct cvmx_lmcx_pll_status_cn58xx
    {
#if __BYTE_ORDER == __BIG_ENDIAN
        uint64_t reserved_2_63           : 62;
        uint64_t rfslip                  : 1;       /**< Reference clock slip */
        uint64_t fbslip                  : 1;       /**< Feedback clock slip */
#else
        uint64_t fbslip                  : 1;
        uint64_t rfslip                  : 1;
        uint64_t reserved_2_63           : 62;
#endif
    } cn58xx;
} cvmx_lmcx_pll_status_t;
