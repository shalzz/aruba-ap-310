
#include <common.h>

#include <nand.h>
#include "diag.h"

extern nand_info_t nand_info[];

/*******************************************************************************
 * mvNandDetectTest - Detect NAND flash with it’s geometry and prints details.
 *
 * DESCRIPTION:
 *		This routine prints NAND detection status. Also displays NAND flash size,
 *		page size, number of blocks if detected.
 *
 * INPUT:
 *		None.
 *
 * OUTPUT:
 *		None.
 *
 * RETURN:
 *		Returns 1 on failure, else 0
 *******************************************************************************/
int mvNandDetectTest(void)
{
	int testFlag;
	testFlag = DIAG_PASS;

	if(NULL != nand_info[0].name)
	{
		printf("\tDevice: 0, Size: %lu MB, Page Size: %d KB, Block Size: %d KB\n", \
	        (nand_info[0].size/1024)/1024, nand_info[0].oobblock/1024, \
	        nand_info[0].erasesize/1024);
	}
	else
	{
		printf("\tError: NAND not detected!");
		testFlag = DIAG_FAIL;
	}

	printf("\tNAND detection test                              ");
	printf(((testFlag==DIAG_PASS)?"PASSED\n":"FAIL\n"));

	return testFlag;
}

/*******************************************************************************
 * mvNandBadBlockTest - Prints all bad blocks on NAND memory
 *
 * DESCRIPTION:
 *		This routine checks all blocks against bad block table and prints the
 *		block if it is marked as bad.
 *
 * INPUT:
 *		None.
 *
 * OUTPUT:
 *		None.
 *
 * RETURN:
 *		Returns 1 on failure, else 0
 *******************************************************************************/
int mvNandBadBlockTest(void)
{
	int testFlag;
	unsigned int off;
	testFlag = DIAG_PASS;

	for (off=0; off < nand_info[0].size; off += nand_info[0].erasesize)
	{
		if (nand_block_isbad(&nand_info[0], off))
		{
			printf("\tBad Block: %08x\n", off);
		}
	}
	printf("\tNAND bad-block detection test                    ");
	printf((testFlag==DIAG_PASS)?"PASSED\n":"FAIL\n");

	return testFlag;
}

/*******************************************************************************
 * mvNandReadWriteTest - Performs read-write test on NAND chip.
 *
 * DESCRIPTION:
 *		This routine writes pattern 0x55,0xaa in 16 blocks at offset immediately
 *		after ENV section and reads them back and verifies.
 *
 * INPUT:
 *		None.
 *
 * OUTPUT:
 *		None.
 *
 * RETURN:
 *		Returns 1 on failure, else 0
 *******************************************************************************/
int mvNandReadWriteTest(void)
{
	int i,j,ret;
	int testFlag;
	unsigned int pageSize;
	unsigned char rbuf[2048];
	nand_write_options_t wr_opts;
	nand_erase_options_t er_opts;
	nand_read_options_t rd_opts;
	testFlag = DIAG_PASS;

	printf("\tNAND read/write test                             ");

	do
	{
		pageSize = nand_info[0].oobblock;

		/***************** ERASE ***********************/
		memset(&er_opts, 0, sizeof(er_opts));
		er_opts.offset = CFG_ENV_OFFSET + CFG_ENV_SIZE;
		er_opts.length = pageSize * PAGES_PER_BLOCK * 16;
		er_opts.quiet = 1;
		ret = nand_erase_opts(&nand_info[0], &er_opts);
		if (ret)
		{
			printf("\tError: NAND Erase faild!\n");
			testFlag = DIAG_FAIL;
			break;
		}
		if(DIAG_PASS != testFlag)
		{
			break;
		}

		/***************** WRITE ***********************/
		memset(&wr_opts, 0, sizeof(wr_opts));
		for(i=0; i < pageSize; i=i+2)
		{
			wr_opts.buffer[i] = 0x55;
			wr_opts.buffer[i+1] = 0xaa;
		}
		wr_opts.length = pageSize;
		wr_opts.offset = CFG_ENV_OFFSET + CFG_ENV_SIZE;
		/* opts.forcejffs2 = 1; */
		wr_opts.pad = 1;
		wr_opts.blockalign = 1;
		wr_opts.quiet = 1;

		for(i=0; i < PAGES_PER_BLOCK * 16;i++)
		{
			ret = nand_write_opts(&nand_info[0], &wr_opts);
			if (ret)
			{
				printf("\tError: NAND write faild!\n");
				testFlag = DIAG_FAIL;
				break;
			}
			wr_opts.offset += pageSize;
		}
		if(DIAG_PASS != testFlag)
		{
			break;
		}

		/**************** READ *************************/
		memset(&rd_opts, 0, sizeof(rd_opts));
		rd_opts.buffer = rbuf;
		rd_opts.length = (ulong)pageSize;
		rd_opts.offset = CFG_ENV_OFFSET + CFG_ENV_SIZE;
		rd_opts.quiet = 1;

		for(i=0; i < PAGES_PER_BLOCK * 16;i++)
		{
			ret = nand_read_opts(&nand_info[0], &rd_opts);
			if (ret)
			{
				printf("\tError: NAND read faild!\n");
				testFlag = DIAG_FAIL;
				break;
			}

			for(j=0; j < pageSize; j=j+4)
			{
				if (rbuf[j] != 0x55 && rbuf[j+1] != 0xaa && rbuf[j+2] != 0x55 && rbuf[j+3] != 0xaa)
				{
					printf("\tError: Data verify failed\n");
					testFlag = DIAG_FAIL;
					break;
				}
			}
			rd_opts.offset += pageSize;
		}
		if(DIAG_PASS != testFlag)
		{
			 break;
		}

	}while(0);

	printf((testFlag==DIAG_PASS)?"PASSED\n":"FAIL\n");

	return testFlag;
}


