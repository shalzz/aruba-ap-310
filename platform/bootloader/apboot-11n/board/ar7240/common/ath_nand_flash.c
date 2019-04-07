#include <common.h>
#include <command.h>
#include <asm/addrspace.h>
#include <asm/io.h>
#include <asm/types.h>
#include "ath_nand_flash.h"
#include "ar7240_soc.h"

#if !defined(ATH_DUAL_FLASH)
#	define	ath_nand_flash_erase		flash_erase
#	define	ath_nand_write_buff		write_buff
#	define	ath_nand_read_buff		read_buff
#	define	ath_nand_flash_info		flash_info
#	define	ath_nand_flash_print_info	flash_print_info
#	define	ath_nand_flash_init		flash_init
#endif

flash_info_t		ath_nand_flash_info[CFG_MAX_FLASH_BANKS];
ath_nand_flinfo_t	nf_info;
int			ath_nand_init_done = 0;

#define ATH_NF_HW_ECC		1
#define ATH_NAND_INIT		0x61746e66	/* echo atnf | hexdump -C */
#define ath_nand_inited()	(ath_nand_init_done == ATH_NAND_INIT)

unsigned ath_nand_status(void)
{
	unsigned	i, rddata;
#define ATH_MAX_STATUS_TRY		1000

	rddata = ar7240_reg_rd(ATH_NF_STATUS);
	for (i = 0; i < ATH_MAX_STATUS_TRY && rddata != 0xff; i++) {
		udelay(25);
		rddata = ar7240_reg_rd(ATH_NF_STATUS);
	}

	if (i == ATH_MAX_STATUS_TRY) {
		ath_nand_flash_init();
		return ATH_NF_STATUS_FAIL;
	}

	ar7240_reg_wr(ATH_NF_COMMAND, 0x07024);  // READ STATUS
	rddata = ar7240_reg_rd(ATH_NF_RD_STATUS);

	return rddata;
}

/*
 * globals
 */
void ath_nand_read_id(ath_nand_id_t *id)
{
	u_int8_t	*u = (u_int8_t *)id;

	flush_cache(id, 8);
	ar7240_reg_wr(ATH_NF_DMA_ADDR, (unsigned)virt_to_phys(id));
	ar7240_reg_wr(ATH_NF_ADDR0_0, 0x0);
	ar7240_reg_wr(ATH_NF_ADDR0_1, 0x0);
	ar7240_reg_wr(ATH_NF_DMA_COUNT, 0x8);
	ar7240_reg_wr(ATH_NF_PG_SIZE, 0x8);
	ar7240_reg_wr(ATH_NF_DMA_CTRL, 0xcc);
	ar7240_reg_wr(ATH_NF_COMMAND, 0x9061); 	// READ ID

	ath_nand_status();	// Don't check for status

	flush_cache(id, 8);

	printf(	"____ %s _____\n"
		"  vid did wc  ilp nsp ct  dp  sa1 org bs  sa0 ss  ps  res1 pls pn  res2\n"
		"0x%3x %3x %3x %3x %3x %3x %3x %3x %3x %3x %3x %3x %3x %3x  %3x %3x %3x\n"
		"-------------\n", __func__,
			id->vid, id->did, id->wc, id->ilp,
			id->nsp, id->ct, id->dp, id->sa1,
			id->org, id->bs, id->sa0, id->ss,
			id->ps, id->res1, id->pls, id->pn,
			id->res2);
	printf("0x%0x 0x%0x 0x%0x 0x%0x 0x%0x 0x%0x 0x%0x 0x%0x\n",
		u[0], u[1], u[2], u[3], u[4], u[5], u[6], u[7]);
}

void ath_nand_flash_print_info (flash_info_t *info)
{
	ath_nand_id_t	id;
	ath_nand_read_id(&id);
}


void nand_block_erase(unsigned addr0, unsigned addr1)
{
	unsigned	rddata;

	//printf("NAND FLASH BLOCK ERASE - 0x%x%x\n", addr1, addr0);

	ar7240_reg_wr(ATH_NF_ADDR0_0, addr0);
	ar7240_reg_wr(ATH_NF_ADDR0_1, addr1);
	ar7240_reg_wr(ATH_NF_COMMAND, 0xd0600e);	// BLOCK ERASE

	rddata = (ath_nand_status() & ATH_NF_RD_STATUS_MASK);
	if (rddata != ATH_NF_STATUS_OK) {
		printf("Erase Failed. status = 0x%x", rddata);
	}
}


int
ath_nand_flash_erase(flash_info_t *info, int s_first, int s_last)
{
	int i, j;

	printf("\nFirst %#x last %#x sector size %#x\n",
		s_first, s_last, nf_info.erasesize);

	for (j = 0, i = s_first; i <= s_last; j++, i += nf_info.erasesize) {
		ulong ba0, ba1, b, p;

		b = i >> nf_info.erasesize_shift;
		p = (i & nf_info.erasesize_mask) >> nf_info.writesize_shift;

		ba0 = (b << 22) | (p << 16);
		ba1 = (b >>  9) & 0xf;
#if 1
		printf("\b\b\b\b%4d", j);
#else
		printf("flash_erase: 0x%x 0x%x%08x\n", i, ba1, ba0);
#endif
		nand_block_erase(ba0, ba1);

	}

	printf("\n");

	return 0;
}

unsigned
rw_page(int rd, unsigned addr0, unsigned addr1, unsigned count, unsigned *buf)
{
	unsigned	i = 0, rddata;
#define ATH_MAX_RETRY	3
retry:
	ar7240_reg_wr(ATH_NF_ADDR0_0, addr0);
	ar7240_reg_wr(ATH_NF_ADDR0_1, addr1);
	ar7240_reg_wr(ATH_NF_DMA_ADDR, (unsigned)buf);
	ar7240_reg_wr(ATH_NF_DMA_COUNT, count);

#if ATH_NF_HW_ECC
	if (nf_info.ecc_offset) {
		ar7240_reg_wr(ATH_NF_ECC_OFFSET, nf_info.ecc_offset);
		ar7240_reg_wr(ATH_NF_ECC_CTRL, 0x20e0);
		ar7240_reg_wr(ATH_NF_CTRL,	ATH_NF_CTRL_ADDR_CYCLE0_5 |
					ATH_NF_CTRL_BLOCK_SIZE_64 |
					ATH_NF_CTRL_PAGE_SIZE_2048 |
					ATH_NF_CTRL_ECC_EN);
	} else
#endif
	{
		ar7240_reg_wr(ATH_NF_CTRL,	ATH_NF_CTRL_ADDR_CYCLE0_5 |
					ATH_NF_CTRL_BLOCK_SIZE_64 |
					ATH_NF_CTRL_PAGE_SIZE_2048 |
					ATH_NF_CTRL_CUSTOM_SIZE_EN);
		ar7240_reg_wr(ATH_NF_PG_SIZE, count);
	}

	if (rd) {	// Read Page
		ar7240_reg_wr(ATH_NF_DMA_CTRL, 0xcc);
		ar7240_reg_wr(ATH_NF_COMMAND, 0x30006a);
	} else {	// Write Page
		ar7240_reg_wr(ATH_NF_DMA_CTRL, 0x8c);
		ar7240_reg_wr(ATH_NF_COMMAND, 0x10804c);
	}

	printf("\r");

	rddata = (ath_nand_status() & ATH_NF_RD_STATUS_MASK);

	if ((rddata != ATH_NF_STATUS_OK) && (i < ATH_MAX_RETRY)) {
		i++;
		goto retry;
	}

	if (rddata != ATH_NF_STATUS_OK) {
		(rd == 1) ? printf("Read") : printf("Write");
		printf(" Failed. status = 0x%x 0x%x\n", rddata, addr0);
	}

	return rddata;
}

/* max page and spare size */
uchar partial_read_buffer[4096 + 256];

int
rw_buff(int rd, uchar *buf, ulong addr, ulong len, unsigned pgsz)
{
	ulong		total = 0;
	unsigned	ret = ATH_NF_STATUS_OK,
			dbs = nf_info.writesize;
	uchar		*ptr;

	if (!ath_nand_inited()) {
		/*
		 * env_init() tries to read the flash. However, it is
		 * called before flash_init. Hence read with defaults
		 */
		nf_info.erasesize_shift = ATH_NF_BLK_SIZE_S;
		nf_info.erasesize_mask = ATH_NF_BLK_SIZE_M;
		dbs = pgsz = nf_info.writesize = 2048;
	}

	while (total < len && ret == ATH_NF_STATUS_OK) {
		ulong ba0, ba1, b, p;

		b = addr >> nf_info.erasesize_shift;
		p = (addr & nf_info.erasesize_mask) >> nf_info.writesize_shift;

		ba0 = (b << 22) | (p << 16);
		ba1 = (b >>  9) & 0xf;

		ptr = buf;

		if ((len - total) < pgsz) {
			ptr = partial_read_buffer;
			if (!rd) {
				memcpy(ptr, buf, len - total);
				printf("memcpy(%p, %p, %d)\n", ptr, buf, len-total);
			}
		}

		//if (!rd) {
		//	printf("rw_buff(%d): 0x%x 0x%x%08x buf = %p pgsz %u\n",
		//			rd, addr, ba1, ba0, ptr, pgsz);
		//}

		if (!rd) flush_cache(ptr, pgsz);	// for writes...
		ret = rw_page(rd, ba0, ba1, pgsz, virt_to_phys(ptr));
		/*
		 * u-boot doesn't seem to have a way to invalidate cache.
		 * Hence, load those entries with correct values from the
		 * uncached address...
		 */
		if (rd) memcpy(ptr, virt_to_phys(ptr), pgsz);	// for reads...

		total += pgsz;
		buf += pgsz;
		addr += dbs;
	}

	if (rd && ptr == partial_read_buffer) {
		buf -= dbs;
		total -= dbs;
		memcpy(buf, ptr, len - total);
	}

	return (ret != ATH_NF_STATUS_OK);
}

int
ath_nand_write_buff(flash_info_t *info, uchar *buf, ulong addr, ulong len)
{
	if (info) {
		return rw_buff(0 /* write */, buf, addr, len, nf_info.writesize + nf_info.oobsize);
	} else {
		return rw_buff(0 /* write */, buf, addr, len, nf_info.writesize);
	}
}

int
ath_nand_read_buff(flash_info_t *info, uchar *buf, ulong addr, ulong len)
{
	if (info) {
		return rw_buff(1 /* read */, buf, addr, len, nf_info.writesize + nf_info.oobsize);
	} else {
		return rw_buff(1 /* read */, buf, addr, len, nf_info.writesize);
	}
}


unsigned long
ath_nand_flash_init(void)
{
	uint64_t ath_plane_size[] =
		{ 64 << 20, 1 << 30, 2 << 30, 4 << 30, 8 << 30 };
	ath_nand_id_t		*id;

	ar7240_reg_rmw_set(AR7240_RESET,
			RST_RESET_ETH_SWITCH_ARESET_MASK |
			RST_RESET_NANDF_RESET_MASK);
	/*
	 * For Boot-ROM mode, boot-rom will pull nand out of reset
	 * For dual-flash mode, these might seem in-effective. But
	 * we need this for re-init due to read-status failure.
	 */
	ar7240_reg_rmw_clear(AR7240_RESET, RST_RESET_ETH_SWITCH_ARESET_MASK);
	udelay(250);

	ar7240_reg_rmw_clear(AR7240_RESET, RST_RESET_NANDF_RESET_MASK);
	udelay(100);

	// TIMINGS_ASYN Reg Settings
	ar7240_reg_wr(ATH_NF_TIMINGS_ASYN, ATH_NF_TIMING_ASYN);

	// NAND Mem Control Reg
	ar7240_reg_wr(ATH_NF_MEM_CTRL, 0xff00);

	// Reset Command
	ar7240_reg_wr(ATH_NF_COMMAND, 0xff00);

	if (ath_nand_inited()) {
		return 0;
	}

	id = &nf_info.id;
	ath_nand_read_id(id);

	nf_info.writesize_shift	= 10 + id->ps;
	nf_info.writesize	= (1024 << id->ps);
	nf_info.writesize_mask	= (nf_info.writesize - 1);

	nf_info.erasesize_shift	= (16 + id->bs);
	nf_info.erasesize	= (1 << nf_info.erasesize_shift);
	nf_info.erasesize_mask	= (nf_info.erasesize - 1);

	nf_info.oobsize		= (nf_info.writesize / 512) * (8 << id->ss);
	if (nf_info.oobsize == 128) {
		nf_info.ecc_offset = nf_info.writesize + 80;
	} else if (nf_info.oobsize == 64) {
		nf_info.ecc_offset = nf_info.writesize + 40;
	} else {
		nf_info.ecc_offset = 0;
	}

	printf("Erase = 0x%x Write = 0x%x oob = 0x%x\n",
		nf_info.erasesize, nf_info.writesize, nf_info.oobsize);

#define NAND_MFR_HYNIX		0xad	// From linux sources

	// Conservatively set to 1Gbit
	nf_info.size = (1 << 30) / 8;

	if (nf_info.id.vid == NAND_MFR_HYNIX) {
		switch (nf_info.id.did) {
		case 0xd3:
			/* 8 Gb NAND Flash H27U8G8T2B */
			nf_info.size = ath_plane_size[id->pls] << id->pn;
			break;
		case 0xf1:
			/* 1 Gb NAND Flash H27U1G8F2B */
			nf_info.size = nf_info.erasesize * 1024;
			break;
		default:
			printf("Unknown Hynix Nand device (0x%x)\n", nf_info.id.did);
		}
	} else {
		printf("Unknown Nand device (0x%x:0x%x)\n",
			nf_info.id.vid, nf_info.id.did);
	}

	ath_nand_init_done = ATH_NAND_INIT;

	/*
	 * hook into board specific code to fill flash_info
	 */
	printf("NAND Flash Init\n");

	return (nf_info.size);
}
