#include "mpi.h"
#include "vcs7385.h"

void mpi_init(void)
{
    cvmx_mpi_cfg_t      mpiCfg;
    cvmx_mpi_sts_t      mpiStatus;


    cn3005_evb_hs5_vcs_chip_select(1); /* Deassert VCS chip select */
    cn3005_evb_hs5_vcs_reset(1); /* Assert Reset */
    udelay(100000);
    cn3005_evb_hs5_vcs_reset(0); /* Deassert Reset */
    udelay(100000);	/* Give the switch/phy complex a full .1s to come back after the reset */

      /* ---- SPI Configure ---- */

      /* Turn on the SPI/MPI interface. It operates as an 1Mhz SPI interface.
       */
    mpiCfg.u64 = 0;
    mpiCfg.s.clkdiv   = 200;  // MPI clock rate divisor (0..8192)
                              // 400Mhz core to 1Mhz SPI rate
    mpiCfg.s.tritx    = 1;    // MPI Tx is tri-stated when not sending
    mpiCfg.s.idleclks = 0;    // Idle SClk cycles between commands (0..3).
    mpiCfg.s.cshi     = 0;    // CS is low asserted
    mpiCfg.s.csena    = 1;    // MPI CS driven by MPI Tx instruction
    mpiCfg.s.int_ena  = 0;    // No interrupt, use polling
    mpiCfg.s.lsbfirst = 0;    // MSB appears first
    mpiCfg.s.wireor   = 0;    // SPI (MPI Tx and Rx are separate wires).
    mpiCfg.s.clk_cont = 0;    // SPI clocks only during commands
    mpiCfg.s.idlelo   = 0;    // SPI clock idles in asserted state
    mpiCfg.s.enable   = 1;
    cvmx_write_csr (CVMX_MPI_CFG, mpiCfg.u64);

    do { /* Wait for any preceeding SPI/MPI operation to complete. */
        mpiStatus.u64 = cvmx_read_csr (CVMX_MPI_STS);
    } while (mpiStatus.s.busy);
}
