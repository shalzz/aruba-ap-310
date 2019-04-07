/*
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */
 
 #include "ddr_core.h"
 #include "asm/arch/socregs.h"
 #include "config.h"
 #define PRIMARY_CPU  0

#ifdef CONFIG_L2C_AS_RAM
	.section section_mmu_table
mmu_table1:
	.word	0x00004c0a
	.word	0x00104c0a
	.word	0x00204c0a
	.word	0x00304c0a
	.word	0x00404c0a
	.word	0x00504c0a
	.word	0x00604c0a
	.word	0x00704c0a
	.word	0x00804c0a
	.word	0x00904c0a
	.word	0x00a04c0a
	.word	0x00b04c0a
	.word	0x00c04c0a
	.word	0x00d04c0a
	.word	0x00e04c0a
	.word	0x00f04c0a
	.word	0x01004c06 @DDR_SHMOO_RELATED_CHANGE: make 16MB-32MB area as L1 cacheable write-back mode requested by shmoo memory test
	.word	0x01104c06 
	.word	0x01204c06 
	.word	0x01304c06
	.word	0x01404c06
	.word	0x01504c06
	.word	0x01604c06
	.word	0x01704c06
	.word	0x01804c06
	.word	0x01904c06
	.word	0x01a04c06
	.word	0x01b04c06
	.word	0x01c04c06
	.word	0x01d04c06
	.word	0x01e04c06
	.word	0x01f04c06 
	.word	0x02005c06 @DDR_SHMOO_RELATED_CHANGE: make 32MB-128MB area as L1&L2 cacheable write-back mode requested by stress memory test
	.word	0x02105c06 
	.word	0x02205c06 
	.word	0x02305c06 
	.word	0x02405c06
	.word	0x02505c06
	.word	0x02605c06
	.word	0x02705c06
	.word	0x02805c06
	.word	0x02905c06
	.word	0x02a05c06
	.word	0x02b05c06
	.word	0x02c05c06
	.word	0x02d05c06
	.word	0x02e05c06
	.word	0x02f05c06
	.word	0x03005c06
	.word	0x03105c06
	.word	0x03205c06
	.word	0x03305c06
	.word	0x03405c06
	.word	0x03505c06
	.word	0x03605c06
	.word	0x03705c06
	.word	0x03805c06
	.word	0x03905c06
	.word	0x03a05c06
	.word	0x03b05c06
	.word	0x03c05c06
	.word	0x03d05c06
	.word	0x03e05c06
	.word	0x03f05c06
	.word	0x04005c06
	.word	0x04105c06
	.word	0x04205c06
	.word	0x04305c06
	.word	0x04405c06
	.word	0x04505c06
	.word	0x04605c06
	.word	0x04705c06
	.word	0x04805c06
	.word	0x04905c06
	.word	0x04a05c06
	.word	0x04b05c06
	.word	0x04c05c06
	.word	0x04d05c06
	.word	0x04e05c06
	.word	0x04f05c06
	.word	0x05005c06
	.word	0x05105c06
	.word	0x05205c06
	.word	0x05305c06
	.word	0x05405c06
	.word	0x05505c06
	.word	0x05605c06
	.word	0x05705c06
	.word	0x05805c06
	.word	0x05905c06
	.word	0x05a05c06
	.word	0x05b05c06
	.word	0x05c05c06
	.word	0x05d05c06
	.word	0x05e05c06
	.word	0x05f05c06
	.word	0x06005c06
	.word	0x06105c06
	.word	0x06205c06
	.word	0x06305c06
	.word	0x06405c06
	.word	0x06505c06
	.word	0x06605c06
	.word	0x06705c06
	.word	0x06805c06
	.word	0x06905c06
	.word	0x06a05c06
	.word	0x06b05c06
	.word	0x06c05c06
	.word	0x06d05c06
	.word	0x06e05c06
	.word	0x06f05c06
	.word	0x07005c06
	.word	0x07105c06
	.word	0x07205c06
	.word	0x07305c06
	.word	0x07405c06
	.word	0x07505c06
	.word	0x07605c06
	.word	0x07705c06
	.word	0x07805c06
	.word	0x07905c06
	.word	0x07a04c0a @ Aruba APBoot: leave 6 MB with standard cache
	.word	0x07b04c0a @ policy so that we can relocate APBoot to just
	.word	0x07c04c0a @ below 128MB
	.word	0x07d04c0a @
	.word	0x07e04c0a @
	.word	0x07f04c0a @ DDR_SHMOO_RELATED_CHANGE: end of L1&L2 cacheable write-back mode
	.word	0x08002c02
	.word	0x08102c02
	.word	0x08202c02
	.word	0x08302c02
	.word	0x08402c02
	.word	0x08502c02
	.word	0x08602c02
	.word	0x08702c02
	.word	0x08802c02
	.word	0x08902c02
	.word	0x08a02c02
	.word	0x08b02c02
	.word	0x08c02c02
	.word	0x08d02c02
	.word	0x08e02c02
	.word	0x08f02c02
	.word	0x09002c02
	.word	0x09102c02
	.word	0x09202c02
	.word	0x09302c02
	.word	0x09402c02
	.word	0x09502c02
	.word	0x09602c02
	.word	0x09702c02
	.word	0x09802c02
	.word	0x09902c02
	.word	0x09a02c02
	.word	0x09b02c02
	.word	0x09c02c02
	.word	0x09d02c02
	.word	0x09e02c02
	.word	0x09f02c02
	.word	0x0a002c02
	.word	0x0a102c02
	.word	0x0a202c02
	.word	0x0a302c02
	.word	0x0a402c02
	.word	0x0a502c02
	.word	0x0a602c02
	.word	0x0a702c02
	.word	0x0a802c02
	.word	0x0a902c02
	.word	0x0aa02c02
	.word	0x0ab02c02
	.word	0x0ac02c02
	.word	0x0ad02c02
	.word	0x0ae02c02
	.word	0x0af02c02
	.word	0x0b002c02
	.word	0x0b102c02
	.word	0x0b202c02
	.word	0x0b302c02
	.word	0x0b402c02
	.word	0x0b502c02
	.word	0x0b602c02
	.word	0x0b702c02
	.word	0x0b802c02
	.word	0x0b902c02
	.word	0x0ba02c02
	.word	0x0bb02c02
	.word	0x0bc02c02
	.word	0x0bd02c02
	.word	0x0be02c02
	.word	0x0bf02c02
	.word	0x0c002c02
	.word	0x0c102c02
	.word	0x0c202c02
	.word	0x0c302c02
	.word	0x0c402c02
	.word	0x0c502c02
	.word	0x0c602c02
	.word	0x0c702c02
	.word	0x0c802c02
	.word	0x0c902c02
	.word	0x0ca02c02
	.word	0x0cb02c02
	.word	0x0cc02c02
	.word	0x0cd02c02
	.word	0x0ce02c02
	.word	0x0cf02c02
	.word	0x0d002c02
	.word	0x0d102c02
	.word	0x0d202c02
	.word	0x0d302c02
	.word	0x0d402c02
	.word	0x0d502c02
	.word	0x0d602c02
	.word	0x0d702c02
	.word	0x0d802c02
	.word	0x0d902c02
	.word	0x0da02c02
	.word	0x0db02c02
	.word	0x0dc02c02
	.word	0x0dd02c02
	.word	0x0de02c02
	.word	0x0df02c02
	.word	0x0e002c02
	.word	0x0e102c02
	.word	0x0e202c02
	.word	0x0e302c02
	.word	0x0e402c02
	.word	0x0e502c02
	.word	0x0e602c02
	.word	0x0e702c02
	.word	0x0e802c02
	.word	0x0e902c02
	.word	0x0ea02c02
	.word	0x0eb02c02
	.word	0x0ec02c02
	.word	0x0ed02c02
	.word	0x0ee02c02
	.word	0x0ef02c02
	.word	0x0f002c02
	.word	0x0f102c02
	.word	0x0f202c02
	.word	0x0f302c02
	.word	0x0f402c02
	.word	0x0f502c02
	.word	0x0f602c02
	.word	0x0f702c02
	.word	0x0f802c02
	.word	0x0f902c02
	.word	0x0fa02c02
	.word	0x0fb02c02
	.word	0x0fc02c02
	.word	0x0fd02c02
	.word	0x0fe02c02
	.word	0x0ff02c02
	.word	0x10002c02
	.word	0x10102c02
	.word	0x10202c02
	.word	0x10302c02
	.word	0x10402c02
	.word	0x10502c02
	.word	0x10602c02
	.word	0x10702c02
	.word	0x10802c02
	.word	0x10902c02
	.word	0x10a02c02
	.word	0x10b02c02
	.word	0x10c02c02
	.word	0x10d02c02
	.word	0x10e02c02
	.word	0x10f02c02
	.word	0x11002c02
	.word	0x11102c02
	.word	0x11202c02
	.word	0x11302c02
	.word	0x11402c02
	.word	0x11502c02
	.word	0x11602c02
	.word	0x11702c02
	.word	0x11802c02
	.word	0x11902c02
	.word	0x11a02c02
	.word	0x11b02c02
	.word	0x11c02c02
	.word	0x11d02c02
	.word	0x11e02c02
	.word	0x11f02c02
	.word	0x12002c02
	.word	0x12102c02
	.word	0x12202c02
	.word	0x12302c02
	.word	0x12402c02
	.word	0x12502c02
	.word	0x12602c02
	.word	0x12702c02
	.word	0x12802c02
	.word	0x12902c02
	.word	0x12a02c02
	.word	0x12b02c02
	.word	0x12c02c02
	.word	0x12d02c02
	.word	0x12e02c02
	.word	0x12f02c02
	.word	0x13002c02
	.word	0x13102c02
	.word	0x13202c02
	.word	0x13302c02
	.word	0x13402c02
	.word	0x13502c02
	.word	0x13602c02
	.word	0x13702c02
	.word	0x13802c02
	.word	0x13902c02
	.word	0x13a02c02
	.word	0x13b02c02
	.word	0x13c02c02
	.word	0x13d02c02
	.word	0x13e02c02
	.word	0x13f02c02
	.word	0x14002c02
	.word	0x14102c02
	.word	0x14202c02
	.word	0x14302c02
	.word	0x14402c02
	.word	0x14502c02
	.word	0x14602c02
	.word	0x14702c02
	.word	0x14802c02
	.word	0x14902c02
	.word	0x14a02c02
	.word	0x14b02c02
	.word	0x14c02c02
	.word	0x14d02c02
	.word	0x14e02c02
	.word	0x14f02c02
	.word	0x15002c02
	.word	0x15102c02
	.word	0x15202c02
	.word	0x15302c02
	.word	0x15402c02
	.word	0x15502c02
	.word	0x15602c02
	.word	0x15702c02
	.word	0x15802c02
	.word	0x15902c02
	.word	0x15a02c02
	.word	0x15b02c02
	.word	0x15c02c02
	.word	0x15d02c02
	.word	0x15e02c02
	.word	0x15f02c02
	.word	0x16002c02
	.word	0x16102c02
	.word	0x16202c02
	.word	0x16302c02
	.word	0x16402c02
	.word	0x16502c02
	.word	0x16602c02
	.word	0x16702c02
	.word	0x16802c02
	.word	0x16902c02
	.word	0x16a02c02
	.word	0x16b02c02
	.word	0x16c02c02
	.word	0x16d02c02
	.word	0x16e02c02
	.word	0x16f02c02
	.word	0x17002c02
	.word	0x17102c02
	.word	0x17202c02
	.word	0x17302c02
	.word	0x17402c02
	.word	0x17502c02
	.word	0x17602c02
	.word	0x17702c02
	.word	0x17802c02
	.word	0x17902c02
	.word	0x17a02c02
	.word	0x17b02c02
	.word	0x17c02c02
	.word	0x17d02c02
	.word	0x17e02c02
	.word	0x17f02c02
	.word	0x18002c02
	.word	0x18102c02
	.word	0x18202c02
	.word	0x18302c02
	.word	0x18402c02
	.word	0x18502c02
	.word	0x18602c02
	.word	0x18702c02
	.word	0x18802c02
	.word	0x18902c02
	.word	0x18a02c02
	.word	0x18b02c02
	.word	0x18c02c02
	.word	0x18d02c02
	.word	0x18e02c02
	.word	0x18f02c02
	.word	0x19002c02
	.word	0x19102c02
	.word	0x19202c02
	.word	0x19302c02
	.word	0x19402c02
	.word	0x19502c02
	.word	0x19602c02
	.word	0x19702c02
	.word	0x19802c02
	.word	0x19902c02
	.word	0x19a02c02
	.word	0x19b02c02
	.word	0x19c02c02
	.word	0x19d02c02
	.word	0x19e02c02
	.word	0x19f02c02
	.word	0x1a002c02
	.word	0x1a102c02
	.word	0x1a202c02
	.word	0x1a302c02
	.word	0x1a402c02
	.word	0x1a502c02
	.word	0x1a602c02
	.word	0x1a702c02
	.word	0x1a802c02
	.word	0x1a902c02
	.word	0x1aa02c02
	.word	0x1ab02c02
	.word	0x1ac02c02
	.word	0x1ad02c02
	.word	0x1ae02c02
	.word	0x1af02c02
	.word	0x1b002c02
	.word	0x1b102c02
	.word	0x1b202c02
	.word	0x1b302c02
	.word	0x1b402c02
	.word	0x1b502c02
	.word	0x1b602c02
	.word	0x1b702c02
	.word	0x1b802c02
	.word	0x1b902c02
	.word	0x1ba02c02
	.word	0x1bb02c02
	.word	0x1bc02c02
	.word	0x1bd02c02
	.word	0x1be02c02
	.word	0x1bf02c02
	.word	0x1c016caa   @ bcm95301x_svk_iproc_nand_config: make NAND flash code L1&L2 cachable
	.word	0x1c102c02
	.word	0x1c202c02
	.word	0x1c302c02
	.word	0x1c402c02
	.word	0x1c502c02
	.word	0x1c602c02
	.word	0x1c702c02
	.word	0x1c802c02
	.word	0x1c902c02
	.word	0x1ca02c02
	.word	0x1cb02c02
	.word	0x1cc02c02
	.word	0x1cd02c02
	.word	0x1ce02c02
	.word	0x1cf02c02
	.word	0x1d002c02
	.word	0x1d102c02
	.word	0x1d202c02
	.word	0x1d302c02
	.word	0x1d402c02
	.word	0x1d502c02
	.word	0x1d602c02
	.word	0x1d702c02
	.word	0x1d802c02
	.word	0x1d902c02
	.word	0x1da02c02
	.word	0x1db02c02
	.word	0x1dc02c02
	.word	0x1dd02c02
	.word	0x1de02c02
	.word	0x1df02c02
	.word	0x1e016caa  @DDR_SHMOO_RELATED_CHANGE: change 0x1e014ca2 to 0x1e016caa to make flash code L1&L2 cachable   
	.word	0x1e104c02
	.word	0x1e204c02
	.word	0x1e304c02
	.word	0x1e404c02
	.word	0x1e504c02
	.word	0x1e604c02
	.word	0x1e704c02
	.word	0x1e804c0a
	.word	0x1e904c0a
	.word	0x1ea04c0a
	.word	0x1eb04c0a
	.word	0x1ec04c0a
	.word	0x1ed04c0a
	.word	0x1ee04c0a
	.word	0x1ef04c0a
	.word	0x1f004c0a
	.word	0x1f104c0a
	.word	0x1f204c0a
	.word	0x1f304c0a
	.word	0x1f404c0a
	.word	0x1f504c0a
	.word	0x1f604c0a
	.word	0x1f704c0a
	.word	0x1f804c0a
	.word	0x1f904c0a
	.word	0x1fa04c0a
	.word	0x1fb04c0a
	.word	0x1fc04c0a
	.word	0x1fd04c0a
	.word	0x1fe04c0a
	.word	0x1ff04c0a
	.word	0x20002c02
	.word	0x20102c02
	.word	0x20202c02
	.word	0x20302c02
	.word	0x20402c02
	.word	0x20502c02
	.word	0x20602c02
	.word	0x20702c02
	.word	0x20802c02
	.word	0x20902c02
	.word	0x20a02c02
	.word	0x20b02c02
	.word	0x20c02c02
	.word	0x20d02c02
	.word	0x20e02c02
	.word	0x20f02c02
	.word	0x21002c02
	.word	0x21102c02
	.word	0x21202c02
	.word	0x21302c02
	.word	0x21402c02
	.word	0x21502c02
	.word	0x21602c02
	.word	0x21702c02
	.word	0x21802c02
	.word	0x21902c02
	.word	0x21a02c02
	.word	0x21b02c02
	.word	0x21c02c02
	.word	0x21d02c02
	.word	0x21e02c02
	.word	0x21f02c02
	.word	0x22002c02
	.word	0x22102c02
	.word	0x22202c02
	.word	0x22302c02
	.word	0x22402c02
	.word	0x22502c02
	.word	0x22602c02
	.word	0x22702c02
	.word	0x22802c02
	.word	0x22902c02
	.word	0x22a02c02
	.word	0x22b02c02
	.word	0x22c02c02
	.word	0x22d02c02
	.word	0x22e02c02
	.word	0x22f02c02
	.word	0x23002c02
	.word	0x23102c02
	.word	0x23202c02
	.word	0x23302c02
	.word	0x23402c02
	.word	0x23502c02
	.word	0x23602c02
	.word	0x23702c02
	.word	0x23802c02
	.word	0x23902c02
	.word	0x23a02c02
	.word	0x23b02c02
	.word	0x23c02c02
	.word	0x23d02c02
	.word	0x23e02c02
	.word	0x23f02c02
	.word	0x24002c02
	.word	0x24102c02
	.word	0x24202c02
	.word	0x24302c02
	.word	0x24402c02
	.word	0x24502c02
	.word	0x24602c02
	.word	0x24702c02
	.word	0x24802c02
	.word	0x24902c02
	.word	0x24a02c02
	.word	0x24b02c02
	.word	0x24c02c02
	.word	0x24d02c02
	.word	0x24e02c02
	.word	0x24f02c02
	.word	0x25002c02
	.word	0x25102c02
	.word	0x25202c02
	.word	0x25302c02
	.word	0x25402c02
	.word	0x25502c02
	.word	0x25602c02
	.word	0x25702c02
	.word	0x25802c02
	.word	0x25902c02
	.word	0x25a02c02
	.word	0x25b02c02
	.word	0x25c02c02
	.word	0x25d02c02
	.word	0x25e02c02
	.word	0x25f02c02
	.word	0x26002c02
	.word	0x26102c02
	.word	0x26202c02
	.word	0x26302c02
	.word	0x26402c02
	.word	0x26502c02
	.word	0x26602c02
	.word	0x26702c02
	.word	0x26802c02
	.word	0x26902c02
	.word	0x26a02c02
	.word	0x26b02c02
	.word	0x26c02c02
	.word	0x26d02c02
	.word	0x26e02c02
	.word	0x26f02c02
	.word	0x27002c02
	.word	0x27102c02
	.word	0x27202c02
	.word	0x27302c02
	.word	0x27402c02
	.word	0x27502c02
	.word	0x27602c02
	.word	0x27702c02
	.word	0x27802c02
	.word	0x27902c02
	.word	0x27a02c02
	.word	0x27b02c02
	.word	0x27c02c02
	.word	0x27d02c02
	.word	0x27e02c02
	.word	0x27f02c02
	.word	0x28002c02
	.word	0x28102c02
	.word	0x28202c02
	.word	0x28302c02
	.word	0x28402c02
	.word	0x28502c02
	.word	0x28602c02
	.word	0x28702c02
	.word	0x28802c02
	.word	0x28902c02
	.word	0x28a02c02
	.word	0x28b02c02
	.word	0x28c02c02
	.word	0x28d02c02
	.word	0x28e02c02
	.word	0x28f02c02
	.word	0x29002c02
	.word	0x29102c02
	.word	0x29202c02
	.word	0x29302c02
	.word	0x29402c02
	.word	0x29502c02
	.word	0x29602c02
	.word	0x29702c02
	.word	0x29802c02
	.word	0x29902c02
	.word	0x29a02c02
	.word	0x29b02c02
	.word	0x29c02c02
	.word	0x29d02c02
	.word	0x29e02c02
	.word	0x29f02c02
	.word	0x2a002c02
	.word	0x2a102c02
	.word	0x2a202c02
	.word	0x2a302c02
	.word	0x2a402c02
	.word	0x2a502c02
	.word	0x2a602c02
	.word	0x2a702c02
	.word	0x2a802c02
	.word	0x2a902c02
	.word	0x2aa02c02
	.word	0x2ab02c02
	.word	0x2ac02c02
	.word	0x2ad02c02
	.word	0x2ae02c02
	.word	0x2af02c02
	.word	0x2b002c02
	.word	0x2b102c02
	.word	0x2b202c02
	.word	0x2b302c02
	.word	0x2b402c02
	.word	0x2b502c02
	.word	0x2b602c02
	.word	0x2b702c02
	.word	0x2b802c02
	.word	0x2b902c02
	.word	0x2ba02c02
	.word	0x2bb02c02
	.word	0x2bc02c02
	.word	0x2bd02c02
	.word	0x2be02c02
	.word	0x2bf02c02
	.word	0x2c002c02
	.word	0x2c102c02
	.word	0x2c202c02
	.word	0x2c302c02
	.word	0x2c402c02
	.word	0x2c502c02
	.word	0x2c602c02
	.word	0x2c702c02
	.word	0x2c802c02
	.word	0x2c902c02
	.word	0x2ca02c02
	.word	0x2cb02c02
	.word	0x2cc02c02
	.word	0x2cd02c02
	.word	0x2ce02c02
	.word	0x2cf02c02
	.word	0x2d002c02
	.word	0x2d102c02
	.word	0x2d202c02
	.word	0x2d302c02
	.word	0x2d402c02
	.word	0x2d502c02
	.word	0x2d602c02
	.word	0x2d702c02
	.word	0x2d802c02
	.word	0x2d902c02
	.word	0x2da02c02
	.word	0x2db02c02
	.word	0x2dc02c02
	.word	0x2dd02c02
	.word	0x2de02c02
	.word	0x2df02c02
	.word	0x2e002c02
	.word	0x2e102c02
	.word	0x2e202c02
	.word	0x2e302c02
	.word	0x2e402c02
	.word	0x2e502c02
	.word	0x2e602c02
	.word	0x2e702c02
	.word	0x2e802c02
	.word	0x2e902c02
	.word	0x2ea02c02
	.word	0x2eb02c02
	.word	0x2ec02c02
	.word	0x2ed02c02
	.word	0x2ee02c02
	.word	0x2ef02c02
	.word	0x2f002c02
	.word	0x2f102c02
	.word	0x2f202c02
	.word	0x2f302c02
	.word	0x2f402c02
	.word	0x2f502c02
	.word	0x2f602c02
	.word	0x2f702c02
	.word	0x2f802c02
	.word	0x2f902c02
	.word	0x2fa02c02
	.word	0x2fb02c02
	.word	0x2fc02c02
	.word	0x2fd02c02
	.word	0x2fe02c02
	.word	0x2ff02c02
	.word	0x30002c02
	.word	0x30102c02
	.word	0x30202c02
	.word	0x30302c02
	.word	0x30402c02
	.word	0x30502c02
	.word	0x30602c02
	.word	0x30702c02
	.word	0x30802c02
	.word	0x30902c02
	.word	0x30a02c02
	.word	0x30b02c02
	.word	0x30c02c02
	.word	0x30d02c02
	.word	0x30e02c02
	.word	0x30f02c02
	.word	0x31002c02
	.word	0x31102c02
	.word	0x31202c02
	.word	0x31302c02
	.word	0x31402c02
	.word	0x31502c02
	.word	0x31602c02
	.word	0x31702c02
	.word	0x31802c02
	.word	0x31902c02
	.word	0x31a02c02
	.word	0x31b02c02
	.word	0x31c02c02
	.word	0x31d02c02
	.word	0x31e02c02
	.word	0x31f02c02
	.word	0x32002c02
	.word	0x32102c02
	.word	0x32202c02
	.word	0x32302c02
	.word	0x32402c02
	.word	0x32502c02
	.word	0x32602c02
	.word	0x32702c02
	.word	0x32802c02
	.word	0x32902c02
	.word	0x32a02c02
	.word	0x32b02c02
	.word	0x32c02c02
	.word	0x32d02c02
	.word	0x32e02c02
	.word	0x32f02c02
	.word	0x33002c02
	.word	0x33102c02
	.word	0x33202c02
	.word	0x33302c02
	.word	0x33402c02
	.word	0x33502c02
	.word	0x33602c02
	.word	0x33702c02
	.word	0x33802c02
	.word	0x33902c02
	.word	0x33a02c02
	.word	0x33b02c02
	.word	0x33c02c02
	.word	0x33d02c02
	.word	0x33e02c02
	.word	0x33f02c02
	.word	0x34002c02
	.word	0x34102c02
	.word	0x34202c02
	.word	0x34302c02
	.word	0x34402c02
	.word	0x34502c02
	.word	0x34602c02
	.word	0x34702c02
	.word	0x34802c02
	.word	0x34902c02
	.word	0x34a02c02
	.word	0x34b02c02
	.word	0x34c02c02
	.word	0x34d02c02
	.word	0x34e02c02
	.word	0x34f02c02
	.word	0x35002c02
	.word	0x35102c02
	.word	0x35202c02
	.word	0x35302c02
	.word	0x35402c02
	.word	0x35502c02
	.word	0x35602c02
	.word	0x35702c02
	.word	0x35802c02
	.word	0x35902c02
	.word	0x35a02c02
	.word	0x35b02c02
	.word	0x35c02c02
	.word	0x35d02c02
	.word	0x35e02c02
	.word	0x35f02c02
	.word	0x36002c02
	.word	0x36102c02
	.word	0x36202c02
	.word	0x36302c02
	.word	0x36402c02
	.word	0x36502c02
	.word	0x36602c02
	.word	0x36702c02
	.word	0x36802c02
	.word	0x36902c02
	.word	0x36a02c02
	.word	0x36b02c02
	.word	0x36c02c02
	.word	0x36d02c02
	.word	0x36e02c02
	.word	0x36f02c02
	.word	0x37002c02
	.word	0x37102c02
	.word	0x37202c02
	.word	0x37302c02
	.word	0x37402c02
	.word	0x37502c02
	.word	0x37602c02
	.word	0x37702c02
	.word	0x37802c02
	.word	0x37902c02
	.word	0x37a02c02
	.word	0x37b02c02
	.word	0x37c02c02
	.word	0x37d02c02
	.word	0x37e02c02
	.word	0x37f02c02
	.word	0x38002c02
	.word	0x38102c02
	.word	0x38202c02
	.word	0x38302c02
	.word	0x38402c02
	.word	0x38502c02
	.word	0x38602c02
	.word	0x38702c02
	.word	0x38802c02
	.word	0x38902c02
	.word	0x38a02c02
	.word	0x38b02c02
	.word	0x38c02c02
	.word	0x38d02c02
	.word	0x38e02c02
	.word	0x38f02c02
	.word	0x39002c02
	.word	0x39102c02
	.word	0x39202c02
	.word	0x39302c02
	.word	0x39402c02
	.word	0x39502c02
	.word	0x39602c02
	.word	0x39702c02
	.word	0x39802c02
	.word	0x39902c02
	.word	0x39a02c02
	.word	0x39b02c02
	.word	0x39c02c02
	.word	0x39d02c02
	.word	0x39e02c02
	.word	0x39f02c02
	.word	0x3a002c02
	.word	0x3a102c02
	.word	0x3a202c02
	.word	0x3a302c02
	.word	0x3a402c02
	.word	0x3a502c02
	.word	0x3a602c02
	.word	0x3a702c02
	.word	0x3a802c02
	.word	0x3a902c02
	.word	0x3aa02c02
	.word	0x3ab02c02
	.word	0x3ac02c02
	.word	0x3ad02c02
	.word	0x3ae02c02
	.word	0x3af02c02
	.word	0x3b002c02
	.word	0x3b102c02
	.word	0x3b202c02
	.word	0x3b302c02
	.word	0x3b402c02
	.word	0x3b502c02
	.word	0x3b602c02
	.word	0x3b702c02
	.word	0x3b802c02
	.word	0x3b902c02
	.word	0x3ba02c02
	.word	0x3bb02c02
	.word	0x3bc02c02
	.word	0x3bd02c02
	.word	0x3be02c02
	.word	0x3bf02c02
	.word	0x3c002c02
	.word	0x3c102c02
	.word	0x3c202c02
	.word	0x3c302c02
	.word	0x3c402c02
	.word	0x3c502c02
	.word	0x3c602c02
	.word	0x3c702c02
	.word	0x3c802c02
	.word	0x3c902c02
	.word	0x3ca02c02
	.word	0x3cb02c02
	.word	0x3cc02c02
	.word	0x3cd02c02
	.word	0x3ce02c02
	.word	0x3cf02c02
	.word	0x3d002c02
	.word	0x3d102c02
	.word	0x3d202c02
	.word	0x3d302c02
	.word	0x3d402c02
	.word	0x3d502c02
	.word	0x3d602c02
	.word	0x3d702c02
	.word	0x3d802c02
	.word	0x3d902c02
	.word	0x3da02c02
	.word	0x3db02c02
	.word	0x3dc02c02
	.word	0x3dd02c02
	.word	0x3de02c02
	.word	0x3df02c02
	.word	0x3e002c02
	.word	0x3e102c02
	.word	0x3e202c02
	.word	0x3e302c02
	.word	0x3e402c02
	.word	0x3e502c02
	.word	0x3e602c02
	.word	0x3e702c02
	.word	0x3e802c02
	.word	0x3e902c02
	.word	0x3ea02c02
	.word	0x3eb02c02
	.word	0x3ec02c02
	.word	0x3ed02c02
	.word	0x3ee02c02
	.word	0x3ef02c02
	.word	0x3f002c02
	.word	0x3f102c02
	.word	0x3f202c02
	.word	0x3f302c02
	.word	0x3f402c02
	.word	0x3f502c02
	.word	0x3f602c02
	.word	0x3f702c02
	.word	0x3f802c02
	.word	0x3f902c02
	.word	0x3fa02c02
	.word	0x3fb02c02
	.word	0x3fc02c02
	.word	0x3fd02c02
	.word	0x3fe02c02
	.word	0x3ff02c02
	.word	0x40002c02
	.word	0x40102c02
	.word	0x40202c02
	.word	0x40302c02
	.word	0x40402c02
	.word	0x40502c02
	.word	0x40602c02
	.word	0x40702c02
	.word	0x40802c02
	.word	0x40902c02
	.word	0x40a02c02
	.word	0x40b02c02
	.word	0x40c02c02
	.word	0x40d02c02
	.word	0x40e02c02
	.word	0x40f02c02
	.word	0x41002c02
	.word	0x41102c02
	.word	0x41202c02
	.word	0x41302c02
	.word	0x41402c02
	.word	0x41502c02
	.word	0x41602c02
	.word	0x41702c02
	.word	0x41802c02
	.word	0x41902c02
	.word	0x41a02c02
	.word	0x41b02c02
	.word	0x41c02c02
	.word	0x41d02c02
	.word	0x41e02c02
	.word	0x41f02c02
	.word	0x42002c02
	.word	0x42102c02
	.word	0x42202c02
	.word	0x42302c02
	.word	0x42402c02
	.word	0x42502c02
	.word	0x42602c02
	.word	0x42702c02
	.word	0x42802c02
	.word	0x42902c02
	.word	0x42a02c02
	.word	0x42b02c02
	.word	0x42c02c02
	.word	0x42d02c02
	.word	0x42e02c02
	.word	0x42f02c02
	.word	0x43002c02
	.word	0x43102c02
	.word	0x43202c02
	.word	0x43302c02
	.word	0x43402c02
	.word	0x43502c02
	.word	0x43602c02
	.word	0x43702c02
	.word	0x43802c02
	.word	0x43902c02
	.word	0x43a02c02
	.word	0x43b02c02
	.word	0x43c02c02
	.word	0x43d02c02
	.word	0x43e02c02
	.word	0x43f02c02
	.word	0x44002c02
	.word	0x44102c02
	.word	0x44202c02
	.word	0x44302c02
	.word	0x44402c02
	.word	0x44502c02
	.word	0x44602c02
	.word	0x44702c02
	.word	0x44802c02
	.word	0x44902c02
	.word	0x44a02c02
	.word	0x44b02c02
	.word	0x44c02c02
	.word	0x44d02c02
	.word	0x44e02c02
	.word	0x44f02c02
	.word	0x45002c02
	.word	0x45102c02
	.word	0x45202c02
	.word	0x45302c02
	.word	0x45402c02
	.word	0x45502c02
	.word	0x45602c02
	.word	0x45702c02
	.word	0x45802c02
	.word	0x45902c02
	.word	0x45a02c02
	.word	0x45b02c02
	.word	0x45c02c02
	.word	0x45d02c02
	.word	0x45e02c02
	.word	0x45f02c02
	.word	0x46002c02
	.word	0x46102c02
	.word	0x46202c02
	.word	0x46302c02
	.word	0x46402c02
	.word	0x46502c02
	.word	0x46602c02
	.word	0x46702c02
	.word	0x46802c02
	.word	0x46902c02
	.word	0x46a02c02
	.word	0x46b02c02
	.word	0x46c02c02
	.word	0x46d02c02
	.word	0x46e02c02
	.word	0x46f02c02
	.word	0x47002c02
	.word	0x47102c02
	.word	0x47202c02
	.word	0x47302c02
	.word	0x47402c02
	.word	0x47502c02
	.word	0x47602c02
	.word	0x47702c02
	.word	0x47802c02
	.word	0x47902c02
	.word	0x47a02c02
	.word	0x47b02c02
	.word	0x47c02c02
	.word	0x47d02c02
	.word	0x47e02c02
	.word	0x47f02c02
	.word	0x48002c02
	.word	0x48102c02
	.word	0x48202c02
	.word	0x48302c02
	.word	0x48402c02
	.word	0x48502c02
	.word	0x48602c02
	.word	0x48702c02
	.word	0x48802c02
	.word	0x48902c02
	.word	0x48a02c02
	.word	0x48b02c02
	.word	0x48c02c02
	.word	0x48d02c02
	.word	0x48e02c02
	.word	0x48f02c02
	.word	0x49002c02
	.word	0x49102c02
	.word	0x49202c02
	.word	0x49302c02
	.word	0x49402c02
	.word	0x49502c02
	.word	0x49602c02
	.word	0x49702c02
	.word	0x49802c02
	.word	0x49902c02
	.word	0x49a02c02
	.word	0x49b02c02
	.word	0x49c02c02
	.word	0x49d02c02
	.word	0x49e02c02
	.word	0x49f02c02
	.word	0x4a002c02
	.word	0x4a102c02
	.word	0x4a202c02
	.word	0x4a302c02
	.word	0x4a402c02
	.word	0x4a502c02
	.word	0x4a602c02
	.word	0x4a702c02
	.word	0x4a802c02
	.word	0x4a902c02
	.word	0x4aa02c02
	.word	0x4ab02c02
	.word	0x4ac02c02
	.word	0x4ad02c02
	.word	0x4ae02c02
	.word	0x4af02c02
	.word	0x4b002c02
	.word	0x4b102c02
	.word	0x4b202c02
	.word	0x4b302c02
	.word	0x4b402c02
	.word	0x4b502c02
	.word	0x4b602c02
	.word	0x4b702c02
	.word	0x4b802c02
	.word	0x4b902c02
	.word	0x4ba02c02
	.word	0x4bb02c02
	.word	0x4bc02c02
	.word	0x4bd02c02
	.word	0x4be02c02
	.word	0x4bf02c02
	.word	0x4c002c02
	.word	0x4c102c02
	.word	0x4c202c02
	.word	0x4c302c02
	.word	0x4c402c02
	.word	0x4c502c02
	.word	0x4c602c02
	.word	0x4c702c02
	.word	0x4c802c02
	.word	0x4c902c02
	.word	0x4ca02c02
	.word	0x4cb02c02
	.word	0x4cc02c02
	.word	0x4cd02c02
	.word	0x4ce02c02
	.word	0x4cf02c02
	.word	0x4d002c02
	.word	0x4d102c02
	.word	0x4d202c02
	.word	0x4d302c02
	.word	0x4d402c02
	.word	0x4d502c02
	.word	0x4d602c02
	.word	0x4d702c02
	.word	0x4d802c02
	.word	0x4d902c02
	.word	0x4da02c02
	.word	0x4db02c02
	.word	0x4dc02c02
	.word	0x4dd02c02
	.word	0x4de02c02
	.word	0x4df02c02
	.word	0x4e002c02
	.word	0x4e102c02
	.word	0x4e202c02
	.word	0x4e302c02
	.word	0x4e402c02
	.word	0x4e502c02
	.word	0x4e602c02
	.word	0x4e702c02
	.word	0x4e802c02
	.word	0x4e902c02
	.word	0x4ea02c02
	.word	0x4eb02c02
	.word	0x4ec02c02
	.word	0x4ed02c02
	.word	0x4ee02c02
	.word	0x4ef02c02
	.word	0x4f002c02
	.word	0x4f102c02
	.word	0x4f202c02
	.word	0x4f302c02
	.word	0x4f402c02
	.word	0x4f502c02
	.word	0x4f602c02
	.word	0x4f702c02
	.word	0x4f802c02
	.word	0x4f902c02
	.word	0x4fa02c02
	.word	0x4fb02c02
	.word	0x4fc02c02
	.word	0x4fd02c02
	.word	0x4fe02c02
	.word	0x4ff02c02
	.word	0x50002c02
	.word	0x50102c02
	.word	0x50202c02
	.word	0x50302c02
	.word	0x50402c02
	.word	0x50502c02
	.word	0x50602c02
	.word	0x50702c02
	.word	0x50802c02
	.word	0x50902c02
	.word	0x50a02c02
	.word	0x50b02c02
	.word	0x50c02c02
	.word	0x50d02c02
	.word	0x50e02c02
	.word	0x50f02c02
	.word	0x51002c02
	.word	0x51102c02
	.word	0x51202c02
	.word	0x51302c02
	.word	0x51402c02
	.word	0x51502c02
	.word	0x51602c02
	.word	0x51702c02
	.word	0x51802c02
	.word	0x51902c02
	.word	0x51a02c02
	.word	0x51b02c02
	.word	0x51c02c02
	.word	0x51d02c02
	.word	0x51e02c02
	.word	0x51f02c02
	.word	0x52002c02
	.word	0x52102c02
	.word	0x52202c02
	.word	0x52302c02
	.word	0x52402c02
	.word	0x52502c02
	.word	0x52602c02
	.word	0x52702c02
	.word	0x52802c02
	.word	0x52902c02
	.word	0x52a02c02
	.word	0x52b02c02
	.word	0x52c02c02
	.word	0x52d02c02
	.word	0x52e02c02
	.word	0x52f02c02
	.word	0x53002c02
	.word	0x53102c02
	.word	0x53202c02
	.word	0x53302c02
	.word	0x53402c02
	.word	0x53502c02
	.word	0x53602c02
	.word	0x53702c02
	.word	0x53802c02
	.word	0x53902c02
	.word	0x53a02c02
	.word	0x53b02c02
	.word	0x53c02c02
	.word	0x53d02c02
	.word	0x53e02c02
	.word	0x53f02c02
	.word	0x54002c02
	.word	0x54102c02
	.word	0x54202c02
	.word	0x54302c02
	.word	0x54402c02
	.word	0x54502c02
	.word	0x54602c02
	.word	0x54702c02
	.word	0x54802c02
	.word	0x54902c02
	.word	0x54a02c02
	.word	0x54b02c02
	.word	0x54c02c02
	.word	0x54d02c02
	.word	0x54e02c02
	.word	0x54f02c02
	.word	0x55002c02
	.word	0x55102c02
	.word	0x55202c02
	.word	0x55302c02
	.word	0x55402c02
	.word	0x55502c02
	.word	0x55602c02
	.word	0x55702c02
	.word	0x55802c02
	.word	0x55902c02
	.word	0x55a02c02
	.word	0x55b02c02
	.word	0x55c02c02
	.word	0x55d02c02
	.word	0x55e02c02
	.word	0x55f02c02
	.word	0x56002c02
	.word	0x56102c02
	.word	0x56202c02
	.word	0x56302c02
	.word	0x56402c02
	.word	0x56502c02
	.word	0x56602c02
	.word	0x56702c02
	.word	0x56802c02
	.word	0x56902c02
	.word	0x56a02c02
	.word	0x56b02c02
	.word	0x56c02c02
	.word	0x56d02c02
	.word	0x56e02c02
	.word	0x56f02c02
	.word	0x57002c02
	.word	0x57102c02
	.word	0x57202c02
	.word	0x57302c02
	.word	0x57402c02
	.word	0x57502c02
	.word	0x57602c02
	.word	0x57702c02
	.word	0x57802c02
	.word	0x57902c02
	.word	0x57a02c02
	.word	0x57b02c02
	.word	0x57c02c02
	.word	0x57d02c02
	.word	0x57e02c02
	.word	0x57f02c02
	.word	0x58002c02
	.word	0x58102c02
	.word	0x58202c02
	.word	0x58302c02
	.word	0x58402c02
	.word	0x58502c02
	.word	0x58602c02
	.word	0x58702c02
	.word	0x58802c02
	.word	0x58902c02
	.word	0x58a02c02
	.word	0x58b02c02
	.word	0x58c02c02
	.word	0x58d02c02
	.word	0x58e02c02
	.word	0x58f02c02
	.word	0x59002c02
	.word	0x59102c02
	.word	0x59202c02
	.word	0x59302c02
	.word	0x59402c02
	.word	0x59502c02
	.word	0x59602c02
	.word	0x59702c02
	.word	0x59802c02
	.word	0x59902c02
	.word	0x59a02c02
	.word	0x59b02c02
	.word	0x59c02c02
	.word	0x59d02c02
	.word	0x59e02c02
	.word	0x59f02c02
	.word	0x5a002c02
	.word	0x5a102c02
	.word	0x5a202c02
	.word	0x5a302c02
	.word	0x5a402c02
	.word	0x5a502c02
	.word	0x5a602c02
	.word	0x5a702c02
	.word	0x5a802c02
	.word	0x5a902c02
	.word	0x5aa02c02
	.word	0x5ab02c02
	.word	0x5ac02c02
	.word	0x5ad02c02
	.word	0x5ae02c02
	.word	0x5af02c02
	.word	0x5b002c02
	.word	0x5b102c02
	.word	0x5b202c02
	.word	0x5b302c02
	.word	0x5b402c02
	.word	0x5b502c02
	.word	0x5b602c02
	.word	0x5b702c02
	.word	0x5b802c02
	.word	0x5b902c02
	.word	0x5ba02c02
	.word	0x5bb02c02
	.word	0x5bc02c02
	.word	0x5bd02c02
	.word	0x5be02c02
	.word	0x5bf02c02
	.word	0x5c002c02
	.word	0x5c102c02
	.word	0x5c202c02
	.word	0x5c302c02
	.word	0x5c402c02
	.word	0x5c502c02
	.word	0x5c602c02
	.word	0x5c702c02
	.word	0x5c802c02
	.word	0x5c902c02
	.word	0x5ca02c02
	.word	0x5cb02c02
	.word	0x5cc02c02
	.word	0x5cd02c02
	.word	0x5ce02c02
	.word	0x5cf02c02
	.word	0x5d002c02
	.word	0x5d102c02
	.word	0x5d202c02
	.word	0x5d302c02
	.word	0x5d402c02
	.word	0x5d502c02
	.word	0x5d602c02
	.word	0x5d702c02
	.word	0x5d802c02
	.word	0x5d902c02
	.word	0x5da02c02
	.word	0x5db02c02
	.word	0x5dc02c02
	.word	0x5dd02c02
	.word	0x5de02c02
	.word	0x5df02c02
	.word	0x5e002c02
	.word	0x5e102c02
	.word	0x5e202c02
	.word	0x5e302c02
	.word	0x5e402c02
	.word	0x5e502c02
	.word	0x5e602c02
	.word	0x5e702c02
	.word	0x5e802c02
	.word	0x5e902c02
	.word	0x5ea02c02
	.word	0x5eb02c02
	.word	0x5ec02c02
	.word	0x5ed02c02
	.word	0x5ee02c02
	.word	0x5ef02c02
	.word	0x5f002c02
	.word	0x5f102c02
	.word	0x5f202c02
	.word	0x5f302c02
	.word	0x5f402c02
	.word	0x5f502c02
	.word	0x5f602c02
	.word	0x5f702c02
	.word	0x5f802c02
	.word	0x5f902c02
	.word	0x5fa02c02
	.word	0x5fb02c02
	.word	0x5fc02c02
	.word	0x5fd02c02
	.word	0x5fe02c02
	.word	0x5ff02c02
	.word	0x60002c02
	.word	0x60102c02
	.word	0x60202c02
	.word	0x60302c02
	.word	0x60402c02
	.word	0x60502c02
	.word	0x60602c02
	.word	0x60702c02
	.word	0x60802c02
	.word	0x60902c02
	.word	0x60a02c02
	.word	0x60b02c02
	.word	0x60c02c02
	.word	0x60d02c02
	.word	0x60e02c02
	.word	0x60f02c02
	.word	0x61002c02
	.word	0x61102c02
	.word	0x61202c02
	.word	0x61302c02
	.word	0x61402c02
	.word	0x61502c02
	.word	0x61602c02
	.word	0x61702c02
	.word	0x61802c02
	.word	0x61902c02
	.word	0x61a02c02
	.word	0x61b02c02
	.word	0x61c02c02
	.word	0x61d02c02
	.word	0x61e02c02
	.word	0x61f02c02
	.word	0x62002c02
	.word	0x62102c02
	.word	0x62202c02
	.word	0x62302c02
	.word	0x62402c02
	.word	0x62502c02
	.word	0x62602c02
	.word	0x62702c02
	.word	0x62802c02
	.word	0x62902c02
	.word	0x62a02c02
	.word	0x62b02c02
	.word	0x62c02c02
	.word	0x62d02c02
	.word	0x62e02c02
	.word	0x62f02c02
	.word	0x63002c02
	.word	0x63102c02
	.word	0x63202c02
	.word	0x63302c02
	.word	0x63402c02
	.word	0x63502c02
	.word	0x63602c02
	.word	0x63702c02
	.word	0x63802c02
	.word	0x63902c02
	.word	0x63a02c02
	.word	0x63b02c02
	.word	0x63c02c02
	.word	0x63d02c02
	.word	0x63e02c02
	.word	0x63f02c02
	.word	0x64002c02
	.word	0x64102c02
	.word	0x64202c02
	.word	0x64302c02
	.word	0x64402c02
	.word	0x64502c02
	.word	0x64602c02
	.word	0x64702c02
	.word	0x64802c02
	.word	0x64902c02
	.word	0x64a02c02
	.word	0x64b02c02
	.word	0x64c02c02
	.word	0x64d02c02
	.word	0x64e02c02
	.word	0x64f02c02
	.word	0x65002c02
	.word	0x65102c02
	.word	0x65202c02
	.word	0x65302c02
	.word	0x65402c02
	.word	0x65502c02
	.word	0x65602c02
	.word	0x65702c02
	.word	0x65802c02
	.word	0x65902c02
	.word	0x65a02c02
	.word	0x65b02c02
	.word	0x65c02c02
	.word	0x65d02c02
	.word	0x65e02c02
	.word	0x65f02c02
	.word	0x66002c02
	.word	0x66102c02
	.word	0x66202c02
	.word	0x66302c02
	.word	0x66402c02
	.word	0x66502c02
	.word	0x66602c02
	.word	0x66702c02
	.word	0x66802c02
	.word	0x66902c02
	.word	0x66a02c02
	.word	0x66b02c02
	.word	0x66c02c02
	.word	0x66d02c02
	.word	0x66e02c02
	.word	0x66f02c02
	.word	0x67002c02
	.word	0x67102c02
	.word	0x67202c02
	.word	0x67302c02
	.word	0x67402c02
	.word	0x67502c02
	.word	0x67602c02
	.word	0x67702c02
	.word	0x67802c02
	.word	0x67902c02
	.word	0x67a02c02
	.word	0x67b02c02
	.word	0x67c02c02
	.word	0x67d02c02
	.word	0x67e02c02
	.word	0x67f02c02
	.word	0x68002c02
	.word	0x68102c02
	.word	0x68202c02
	.word	0x68302c02
	.word	0x68402c02
	.word	0x68502c02
	.word	0x68602c02
	.word	0x68702c02
	.word	0x68802c02
	.word	0x68902c02
	.word	0x68a02c02
	.word	0x68b02c02
	.word	0x68c02c02
	.word	0x68d02c02
	.word	0x68e02c02
	.word	0x68f02c02
	.word	0x69002c02
	.word	0x69102c02
	.word	0x69202c02
	.word	0x69302c02
	.word	0x69402c02
	.word	0x69502c02
	.word	0x69602c02
	.word	0x69702c02
	.word	0x69802c02
	.word	0x69902c02
	.word	0x69a02c02
	.word	0x69b02c02
	.word	0x69c02c02
	.word	0x69d02c02
	.word	0x69e02c02
	.word	0x69f02c02
	.word	0x6a002c02
	.word	0x6a102c02
	.word	0x6a202c02
	.word	0x6a302c02
	.word	0x6a402c02
	.word	0x6a502c02
	.word	0x6a602c02
	.word	0x6a702c02
	.word	0x6a802c02
	.word	0x6a902c02
	.word	0x6aa02c02
	.word	0x6ab02c02
	.word	0x6ac02c02
	.word	0x6ad02c02
	.word	0x6ae02c02
	.word	0x6af02c02
	.word	0x6b002c02
	.word	0x6b102c02
	.word	0x6b202c02
	.word	0x6b302c02
	.word	0x6b402c02
	.word	0x6b502c02
	.word	0x6b602c02
	.word	0x6b702c02
	.word	0x6b802c02
	.word	0x6b902c02
	.word	0x6ba02c02
	.word	0x6bb02c02
	.word	0x6bc02c02
	.word	0x6bd02c02
	.word	0x6be02c02
	.word	0x6bf02c02
	.word	0x6c002c02
	.word	0x6c102c02
	.word	0x6c202c02
	.word	0x6c302c02
	.word	0x6c402c02
	.word	0x6c502c02
	.word	0x6c602c02
	.word	0x6c702c02
	.word	0x6c802c02
	.word	0x6c902c02
	.word	0x6ca02c02
	.word	0x6cb02c02
	.word	0x6cc02c02
	.word	0x6cd02c02
	.word	0x6ce02c02
	.word	0x6cf02c02
	.word	0x6d002c02
	.word	0x6d102c02
	.word	0x6d202c02
	.word	0x6d302c02
	.word	0x6d402c02
	.word	0x6d502c02
	.word	0x6d602c02
	.word	0x6d702c02
	.word	0x6d802c02
	.word	0x6d902c02
	.word	0x6da02c02
	.word	0x6db02c02
	.word	0x6dc02c02
	.word	0x6dd02c02
	.word	0x6de02c02
	.word	0x6df02c02
	.word	0x6e002c02
	.word	0x6e102c02
	.word	0x6e202c02
	.word	0x6e302c02
	.word	0x6e402c02
	.word	0x6e502c02
	.word	0x6e602c02
	.word	0x6e702c02
	.word	0x6e802c02
	.word	0x6e902c02
	.word	0x6ea02c02
	.word	0x6eb02c02
	.word	0x6ec02c02
	.word	0x6ed02c02
	.word	0x6ee02c02
	.word	0x6ef02c02
	.word	0x6f002c02
	.word	0x6f102c02
	.word	0x6f202c02
	.word	0x6f302c02
	.word	0x6f402c02
	.word	0x6f502c02
	.word	0x6f602c02
	.word	0x6f702c02
	.word	0x6f802c02
	.word	0x6f902c02
	.word	0x6fa02c02
	.word	0x6fb02c02
	.word	0x6fc02c02
	.word	0x6fd02c02
	.word	0x6fe02c02
	.word	0x6ff02c02
	.word	0x70002c02
	.word	0x70102c02
	.word	0x70202c02
	.word	0x70302c02
	.word	0x70402c02
	.word	0x70502c02
	.word	0x70602c02
	.word	0x70702c02
	.word	0x70802c02
	.word	0x70902c02
	.word	0x70a02c02
	.word	0x70b02c02
	.word	0x70c02c02
	.word	0x70d02c02
	.word	0x70e02c02
	.word	0x70f02c02
	.word	0x71002c02
	.word	0x71102c02
	.word	0x71202c02
	.word	0x71302c02
	.word	0x71402c02
	.word	0x71502c02
	.word	0x71602c02
	.word	0x71702c02
	.word	0x71802c02
	.word	0x71902c02
	.word	0x71a02c02
	.word	0x71b02c02
	.word	0x71c02c02
	.word	0x71d02c02
	.word	0x71e02c02
	.word	0x71f02c02
	.word	0x72002c02
	.word	0x72102c02
	.word	0x72202c02
	.word	0x72302c02
	.word	0x72402c02
	.word	0x72502c02
	.word	0x72602c02
	.word	0x72702c02
	.word	0x72802c02
	.word	0x72902c02
	.word	0x72a02c02
	.word	0x72b02c02
	.word	0x72c02c02
	.word	0x72d02c02
	.word	0x72e02c02
	.word	0x72f02c02
	.word	0x73002c02
	.word	0x73102c02
	.word	0x73202c02
	.word	0x73302c02
	.word	0x73402c02
	.word	0x73502c02
	.word	0x73602c02
	.word	0x73702c02
	.word	0x73802c02
	.word	0x73902c02
	.word	0x73a02c02
	.word	0x73b02c02
	.word	0x73c02c02
	.word	0x73d02c02
	.word	0x73e02c02
	.word	0x73f02c02
	.word	0x74002c02
	.word	0x74102c02
	.word	0x74202c02
	.word	0x74302c02
	.word	0x74402c02
	.word	0x74502c02
	.word	0x74602c02
	.word	0x74702c02
	.word	0x74802c02
	.word	0x74902c02
	.word	0x74a02c02
	.word	0x74b02c02
	.word	0x74c02c02
	.word	0x74d02c02
	.word	0x74e02c02
	.word	0x74f02c02
	.word	0x75002c02
	.word	0x75102c02
	.word	0x75202c02
	.word	0x75302c02
	.word	0x75402c02
	.word	0x75502c02
	.word	0x75602c02
	.word	0x75702c02
	.word	0x75802c02
	.word	0x75902c02
	.word	0x75a02c02
	.word	0x75b02c02
	.word	0x75c02c02
	.word	0x75d02c02
	.word	0x75e02c02
	.word	0x75f02c02
	.word	0x76002c02
	.word	0x76102c02
	.word	0x76202c02
	.word	0x76302c02
	.word	0x76402c02
	.word	0x76502c02
	.word	0x76602c02
	.word	0x76702c02
	.word	0x76802c02
	.word	0x76902c02
	.word	0x76a02c02
	.word	0x76b02c02
	.word	0x76c02c02
	.word	0x76d02c02
	.word	0x76e02c02
	.word	0x76f02c02
	.word	0x77002c02
	.word	0x77102c02
	.word	0x77202c02
	.word	0x77302c02
	.word	0x77402c02
	.word	0x77502c02
	.word	0x77602c02
	.word	0x77702c02
	.word	0x77802c02
	.word	0x77902c02
	.word	0x77a02c02
	.word	0x77b02c02
	.word	0x77c02c02
	.word	0x77d02c02
	.word	0x77e02c02
	.word	0x77f02c02
	.word	0x78002c02
	.word	0x78102c02
	.word	0x78202c02
	.word	0x78302c02
	.word	0x78402c02
	.word	0x78502c02
	.word	0x78602c02
	.word	0x78702c02
	.word	0x78802c02
	.word	0x78902c02
	.word	0x78a02c02
	.word	0x78b02c02
	.word	0x78c02c02
	.word	0x78d02c02
	.word	0x78e02c02
	.word	0x78f02c02
	.word	0x79002c02
	.word	0x79102c02
	.word	0x79202c02
	.word	0x79302c02
	.word	0x79402c02
	.word	0x79502c02
	.word	0x79602c02
	.word	0x79702c02
	.word	0x79802c02
	.word	0x79902c02
	.word	0x79a02c02
	.word	0x79b02c02
	.word	0x79c02c02
	.word	0x79d02c02
	.word	0x79e02c02
	.word	0x79f02c02
	.word	0x7a002c02
	.word	0x7a102c02
	.word	0x7a202c02
	.word	0x7a302c02
	.word	0x7a402c02
	.word	0x7a502c02
	.word	0x7a602c02
	.word	0x7a702c02
	.word	0x7a802c02
	.word	0x7a902c02
	.word	0x7aa02c02
	.word	0x7ab02c02
	.word	0x7ac02c02
	.word	0x7ad02c02
	.word	0x7ae02c02
	.word	0x7af02c02
	.word	0x7b002c02
	.word	0x7b102c02
	.word	0x7b202c02
	.word	0x7b302c02
	.word	0x7b402c02
	.word	0x7b502c02
	.word	0x7b602c02
	.word	0x7b702c02
	.word	0x7b802c02
	.word	0x7b902c02
	.word	0x7ba02c02
	.word	0x7bb02c02
	.word	0x7bc02c02
	.word	0x7bd02c02
	.word	0x7be02c02
	.word	0x7bf02c02
	.word	0x7c002c02
	.word	0x7c102c02
	.word	0x7c202c02
	.word	0x7c302c02
	.word	0x7c402c02
	.word	0x7c502c02
	.word	0x7c602c02
	.word	0x7c702c02
	.word	0x7c802c02
	.word	0x7c902c02
	.word	0x7ca02c02
	.word	0x7cb02c02
	.word	0x7cc02c02
	.word	0x7cd02c02
	.word	0x7ce02c02
	.word	0x7cf02c02
	.word	0x7d002c02
	.word	0x7d102c02
	.word	0x7d202c02
	.word	0x7d302c02
	.word	0x7d402c02
	.word	0x7d502c02
	.word	0x7d602c02
	.word	0x7d702c02
	.word	0x7d802c02
	.word	0x7d902c02
	.word	0x7da02c02
	.word	0x7db02c02
	.word	0x7dc02c02
	.word	0x7dd02c02
	.word	0x7de02c02
	.word	0x7df02c02
	.word	0x7e002c02
	.word	0x7e102c02
	.word	0x7e202c02
	.word	0x7e302c02
	.word	0x7e402c02
	.word	0x7e502c02
	.word	0x7e602c02
	.word	0x7e702c02
	.word	0x7e802c02
	.word	0x7e902c02
	.word	0x7ea02c02
	.word	0x7eb02c02
	.word	0x7ec02c02
	.word	0x7ed02c02
	.word	0x7ee02c02
	.word	0x7ef02c02
	.word	0x7f002c02
	.word	0x7f102c02
	.word	0x7f202c02
	.word	0x7f302c02
	.word	0x7f402c02
	.word	0x7f502c02
	.word	0x7f602c02
	.word	0x7f702c02
	.word	0x7f802c02
	.word	0x7f902c02
	.word	0x7fa02c02
	.word	0x7fb02c02
	.word	0x7fc02c02
	.word	0x7fd02c02
	.word	0x7fe02c02
	.word	0x7ff02c02
	.word	0x80002c02
	.word	0x80102c02
	.word	0x80202c02
	.word	0x80302c02
	.word	0x80402c02
	.word	0x80502c02
	.word	0x80602c02
	.word	0x80702c02
	.word	0x80802c02
	.word	0x80902c02
	.word	0x80a02c02
	.word	0x80b02c02
	.word	0x80c02c02
	.word	0x80d02c02
	.word	0x80e02c02
	.word	0x80f02c02
	.word	0x81002c02
	.word	0x81102c02
	.word	0x81202c02
	.word	0x81302c02
	.word	0x81402c02
	.word	0x81502c02
	.word	0x81602c02
	.word	0x81702c02
	.word	0x81802c02
	.word	0x81902c02
	.word	0x81a02c02
	.word	0x81b02c02
	.word	0x81c02c02
	.word	0x81d02c02
	.word	0x81e02c02
	.word	0x81f02c02
	.word	0x82002c02
	.word	0x82102c02
	.word	0x82202c02
	.word	0x82302c02
	.word	0x82402c02
	.word	0x82502c02
	.word	0x82602c02
	.word	0x82702c02
	.word	0x82802c02
	.word	0x82902c02
	.word	0x82a02c02
	.word	0x82b02c02
	.word	0x82c02c02
	.word	0x82d02c02
	.word	0x82e02c02
	.word	0x82f02c02
	.word	0x83002c02
	.word	0x83102c02
	.word	0x83202c02
	.word	0x83302c02
	.word	0x83402c02
	.word	0x83502c02
	.word	0x83602c02
	.word	0x83702c02
	.word	0x83802c02
	.word	0x83902c02
	.word	0x83a02c02
	.word	0x83b02c02
	.word	0x83c02c02
	.word	0x83d02c02
	.word	0x83e02c02
	.word	0x83f02c02
	.word	0x84002c02
	.word	0x84102c02
	.word	0x84202c02
	.word	0x84302c02
	.word	0x84402c02
	.word	0x84502c02
	.word	0x84602c02
	.word	0x84702c02
	.word	0x84802c02
	.word	0x84902c02
	.word	0x84a02c02
	.word	0x84b02c02
	.word	0x84c02c02
	.word	0x84d02c02
	.word	0x84e02c02
	.word	0x84f02c02
	.word	0x85002c02
	.word	0x85102c02
	.word	0x85202c02
	.word	0x85302c02
	.word	0x85402c02
	.word	0x85502c02
	.word	0x85602c02
	.word	0x85702c02
	.word	0x85802c02
	.word	0x85902c02
	.word	0x85a02c02
	.word	0x85b02c02
	.word	0x85c02c02
	.word	0x85d02c02
	.word	0x85e02c02
	.word	0x85f02c02
	.word	0x86002c02
	.word	0x86102c02
	.word	0x86202c02
	.word	0x86302c02
	.word	0x86402c02
	.word	0x86502c02
	.word	0x86602c02
	.word	0x86702c02
	.word	0x86802c02
	.word	0x86902c02
	.word	0x86a02c02
	.word	0x86b02c02
	.word	0x86c02c02
	.word	0x86d02c02
	.word	0x86e02c02
	.word	0x86f02c02
	.word	0x87002c02
	.word	0x87102c02
	.word	0x87202c02
	.word	0x87302c02
	.word	0x87402c02
	.word	0x87502c02
	.word	0x87602c02
	.word	0x87702c02
	.word	0x87802c02
	.word	0x87902c02
	.word	0x87a02c02
	.word	0x87b02c02
	.word	0x87c02c02
	.word	0x87d02c02
	.word	0x87e02c02
	.word	0x87f02c02
	.word	0x88002c02
	.word	0x88102c02
	.word	0x88202c02
	.word	0x88302c02
	.word	0x88402c02
	.word	0x88502c02
	.word	0x88602c02
	.word	0x88702c02
	.word	0x88802c02
	.word	0x88902c02
	.word	0x88a02c02
	.word	0x88b02c02
	.word	0x88c02c02
	.word	0x88d02c02
	.word	0x88e02c02
	.word	0x88f02c02
	.word	0x89000c02			@POST_TEST_RELATED_CHANGE: make 0x8900_0000 - 0x8aff_ffff area as non-cachable, non-bufferable, for data DMA.
	.word	0x89100c02
	.word	0x89200c02
	.word	0x89300c02
	.word	0x89400c02
	.word	0x89500c02
	.word	0x89600c02
	.word	0x89700c02
	.word	0x89800c02
	.word	0x89900c02
	.word	0x89a00c02
	.word	0x89b00c02
	.word	0x89c00c02
	.word	0x89d00c02
	.word	0x89e00c02
	.word	0x89f00c02
	.word	0x8a000c02
	.word	0x8a100c02
	.word	0x8a200c02
	.word	0x8a300c02
	.word	0x8a400c02
	.word	0x8a500c02
	.word	0x8a600c02
	.word	0x8a700c02
	.word	0x8a800c02
	.word	0x8a900c02
	.word	0x8aa00c02
	.word	0x8ab00c02
	.word	0x8ac00c02
	.word	0x8ad00c02
	.word	0x8ae00c02
	.word	0x8af00c02			@POST_TEST_RELATED_CHANGE: end of make 0x8900_0000 - 0x8aff_ffff area as non-cachable, non-bufferable, for data DMA.
	.word	0x8b002c02
	.word	0x8b102c02
	.word	0x8b202c02
	.word	0x8b302c02
	.word	0x8b402c02
	.word	0x8b502c02
	.word	0x8b602c02
	.word	0x8b702c02
	.word	0x8b802c02
	.word	0x8b902c02
	.word	0x8ba02c02
	.word	0x8bb02c02
	.word	0x8bc02c02
	.word	0x8bd02c02
	.word	0x8be02c02
	.word	0x8bf02c02
	.word	0x8c002c02
	.word	0x8c102c02
	.word	0x8c202c02
	.word	0x8c302c02
	.word	0x8c402c02
	.word	0x8c502c02
	.word	0x8c602c02
	.word	0x8c702c02
	.word	0x8c802c02
	.word	0x8c902c02
	.word	0x8ca02c02
	.word	0x8cb02c02
	.word	0x8cc02c02
	.word	0x8cd02c02
	.word	0x8ce02c02
	.word	0x8cf02c02
	.word	0x8d002c02
	.word	0x8d102c02
	.word	0x8d202c02
	.word	0x8d302c02
	.word	0x8d402c02
	.word	0x8d502c02
	.word	0x8d602c02
	.word	0x8d702c02
	.word	0x8d802c02
	.word	0x8d902c02
	.word	0x8da02c02
	.word	0x8db02c02
	.word	0x8dc02c02
	.word	0x8dd02c02
	.word	0x8de02c02
	.word	0x8df02c02
	.word	0x8e002c02
	.word	0x8e102c02
	.word	0x8e202c02
	.word	0x8e302c02
	.word	0x8e402c02
	.word	0x8e502c02
	.word	0x8e602c02
	.word	0x8e702c02
	.word	0x8e802c02
	.word	0x8e902c02
	.word	0x8ea02c02
	.word	0x8eb02c02
	.word	0x8ec02c02
	.word	0x8ed02c02
	.word	0x8ee02c02
	.word	0x8ef02c02
	.word	0x8f002c02
	.word	0x8f102c02
	.word	0x8f202c02
	.word	0x8f302c02
	.word	0x8f402c02
	.word	0x8f502c02
	.word	0x8f602c02
	.word	0x8f702c02
	.word	0x8f802c02
	.word	0x8f902c02
	.word	0x8fa02c02
	.word	0x8fb02c02
	.word	0x8fc02c02
	.word	0x8fd02c02
	.word	0x8fe02c02
	.word	0x8ff02c02
	.word	0x90002c02
	.word	0x90102c02
	.word	0x90202c02
	.word	0x90302c02
	.word	0x90402c02
	.word	0x90502c02
	.word	0x90602c02
	.word	0x90702c02
	.word	0x90802c02
	.word	0x90902c02
	.word	0x90a02c02
	.word	0x90b02c02
	.word	0x90c02c02
	.word	0x90d02c02
	.word	0x90e02c02
	.word	0x90f02c02
	.word	0x91002c02
	.word	0x91102c02
	.word	0x91202c02
	.word	0x91302c02
	.word	0x91402c02
	.word	0x91502c02
	.word	0x91602c02
	.word	0x91702c02
	.word	0x91802c02
	.word	0x91902c02
	.word	0x91a02c02
	.word	0x91b02c02
	.word	0x91c02c02
	.word	0x91d02c02
	.word	0x91e02c02
	.word	0x91f02c02
	.word	0x92002c02
	.word	0x92102c02
	.word	0x92202c02
	.word	0x92302c02
	.word	0x92402c02
	.word	0x92502c02
	.word	0x92602c02
	.word	0x92702c02
	.word	0x92802c02
	.word	0x92902c02
	.word	0x92a02c02
	.word	0x92b02c02
	.word	0x92c02c02
	.word	0x92d02c02
	.word	0x92e02c02
	.word	0x92f02c02
	.word	0x93002c02
	.word	0x93102c02
	.word	0x93202c02
	.word	0x93302c02
	.word	0x93402c02
	.word	0x93502c02
	.word	0x93602c02
	.word	0x93702c02
	.word	0x93802c02
	.word	0x93902c02
	.word	0x93a02c02
	.word	0x93b02c02
	.word	0x93c02c02
	.word	0x93d02c02
	.word	0x93e02c02
	.word	0x93f02c02
	.word	0x94002c02
	.word	0x94102c02
	.word	0x94202c02
	.word	0x94302c02
	.word	0x94402c02
	.word	0x94502c02
	.word	0x94602c02
	.word	0x94702c02
	.word	0x94802c02
	.word	0x94902c02
	.word	0x94a02c02
	.word	0x94b02c02
	.word	0x94c02c02
	.word	0x94d02c02
	.word	0x94e02c02
	.word	0x94f02c02
	.word	0x95002c02
	.word	0x95102c02
	.word	0x95202c02
	.word	0x95302c02
	.word	0x95402c02
	.word	0x95502c02
	.word	0x95602c02
	.word	0x95702c02
	.word	0x95802c02
	.word	0x95902c02
	.word	0x95a02c02
	.word	0x95b02c02
	.word	0x95c02c02
	.word	0x95d02c02
	.word	0x95e02c02
	.word	0x95f02c02
	.word	0x96002c02
	.word	0x96102c02
	.word	0x96202c02
	.word	0x96302c02
	.word	0x96402c02
	.word	0x96502c02
	.word	0x96602c02
	.word	0x96702c02
	.word	0x96802c02
	.word	0x96902c02
	.word	0x96a02c02
	.word	0x96b02c02
	.word	0x96c02c02
	.word	0x96d02c02
	.word	0x96e02c02
	.word	0x96f02c02
	.word	0x97002c02
	.word	0x97102c02
	.word	0x97202c02
	.word	0x97302c02
	.word	0x97402c02
	.word	0x97502c02
	.word	0x97602c02
	.word	0x97702c02
	.word	0x97802c02
	.word	0x97902c02
	.word	0x97a02c02
	.word	0x97b02c02
	.word	0x97c02c02
	.word	0x97d02c02
	.word	0x97e02c02
	.word	0x97f02c02
	.word	0x98002c02
	.word	0x98102c02
	.word	0x98202c02
	.word	0x98302c02
	.word	0x98402c02
	.word	0x98502c02
	.word	0x98602c02
	.word	0x98702c02
	.word	0x98802c02
	.word	0x98902c02
	.word	0x98a02c02
	.word	0x98b02c02
	.word	0x98c02c02
	.word	0x98d02c02
	.word	0x98e02c02
	.word	0x98f02c02
	.word	0x99002c02
	.word	0x99102c02
	.word	0x99202c02
	.word	0x99302c02
	.word	0x99402c02
	.word	0x99502c02
	.word	0x99602c02
	.word	0x99702c02
	.word	0x99802c02
	.word	0x99902c02
	.word	0x99a02c02
	.word	0x99b02c02
	.word	0x99c02c02
	.word	0x99d02c02
	.word	0x99e02c02
	.word	0x99f02c02
	.word	0x9a002c02
	.word	0x9a102c02
	.word	0x9a202c02
	.word	0x9a302c02
	.word	0x9a402c02
	.word	0x9a502c02
	.word	0x9a602c02
	.word	0x9a702c02
	.word	0x9a802c02
	.word	0x9a902c02
	.word	0x9aa02c02
	.word	0x9ab02c02
	.word	0x9ac02c02
	.word	0x9ad02c02
	.word	0x9ae02c02
	.word	0x9af02c02
	.word	0x9b002c02
	.word	0x9b102c02
	.word	0x9b202c02
	.word	0x9b302c02
	.word	0x9b402c02
	.word	0x9b502c02
	.word	0x9b602c02
	.word	0x9b702c02
	.word	0x9b802c02
	.word	0x9b902c02
	.word	0x9ba02c02
	.word	0x9bb02c02
	.word	0x9bc02c02
	.word	0x9bd02c02
	.word	0x9be02c02
	.word	0x9bf02c02
	.word	0x9c002c02
	.word	0x9c102c02
	.word	0x9c202c02
	.word	0x9c302c02
	.word	0x9c402c02
	.word	0x9c502c02
	.word	0x9c602c02
	.word	0x9c702c02
	.word	0x9c802c02
	.word	0x9c902c02
	.word	0x9ca02c02
	.word	0x9cb02c02
	.word	0x9cc02c02
	.word	0x9cd02c02
	.word	0x9ce02c02
	.word	0x9cf02c02
	.word	0x9d002c02
	.word	0x9d102c02
	.word	0x9d202c02
	.word	0x9d302c02
	.word	0x9d402c02
	.word	0x9d502c02
	.word	0x9d602c02
	.word	0x9d702c02
	.word	0x9d802c02
	.word	0x9d902c02
	.word	0x9da02c02
	.word	0x9db02c02
	.word	0x9dc02c02
	.word	0x9dd02c02
	.word	0x9de02c02
	.word	0x9df02c02
	.word	0x9e002c02
	.word	0x9e102c02
	.word	0x9e202c02
	.word	0x9e302c02
	.word	0x9e402c02
	.word	0x9e502c02
	.word	0x9e602c02
	.word	0x9e702c02
	.word	0x9e802c02
	.word	0x9e902c02
	.word	0x9ea02c02
	.word	0x9eb02c02
	.word	0x9ec02c02
	.word	0x9ed02c02
	.word	0x9ee02c02
	.word	0x9ef02c02
	.word	0x9f002c02
	.word	0x9f102c02
	.word	0x9f202c02
	.word	0x9f302c02
	.word	0x9f402c02
	.word	0x9f502c02
	.word	0x9f602c02
	.word	0x9f702c02
	.word	0x9f802c02
	.word	0x9f902c02
	.word	0x9fa02c02
	.word	0x9fb02c02
	.word	0x9fc02c02
	.word	0x9fd02c02
	.word	0x9fe02c02
	.word	0x9ff02c02
	.word	0xa0002c02
	.word	0xa0102c02
	.word	0xa0202c02
	.word	0xa0302c02
	.word	0xa0402c02
	.word	0xa0502c02
	.word	0xa0602c02
	.word	0xa0702c02
	.word	0xa0802c02
	.word	0xa0902c02
	.word	0xa0a02c02
	.word	0xa0b02c02
	.word	0xa0c02c02
	.word	0xa0d02c02
	.word	0xa0e02c02
	.word	0xa0f02c02
	.word	0xa1002c02
	.word	0xa1102c02
	.word	0xa1202c02
	.word	0xa1302c02
	.word	0xa1402c02
	.word	0xa1502c02
	.word	0xa1602c02
	.word	0xa1702c02
	.word	0xa1802c02
	.word	0xa1902c02
	.word	0xa1a02c02
	.word	0xa1b02c02
	.word	0xa1c02c02
	.word	0xa1d02c02
	.word	0xa1e02c02
	.word	0xa1f02c02
	.word	0xa2002c02
	.word	0xa2102c02
	.word	0xa2202c02
	.word	0xa2302c02
	.word	0xa2402c02
	.word	0xa2502c02
	.word	0xa2602c02
	.word	0xa2702c02
	.word	0xa2802c02
	.word	0xa2902c02
	.word	0xa2a02c02
	.word	0xa2b02c02
	.word	0xa2c02c02
	.word	0xa2d02c02
	.word	0xa2e02c02
	.word	0xa2f02c02
	.word	0xa3002c02
	.word	0xa3102c02
	.word	0xa3202c02
	.word	0xa3302c02
	.word	0xa3402c02
	.word	0xa3502c02
	.word	0xa3602c02
	.word	0xa3702c02
	.word	0xa3802c02
	.word	0xa3902c02
	.word	0xa3a02c02
	.word	0xa3b02c02
	.word	0xa3c02c02
	.word	0xa3d02c02
	.word	0xa3e02c02
	.word	0xa3f02c02
	.word	0xa4002c02
	.word	0xa4102c02
	.word	0xa4202c02
	.word	0xa4302c02
	.word	0xa4402c02
	.word	0xa4502c02
	.word	0xa4602c02
	.word	0xa4702c02
	.word	0xa4802c02
	.word	0xa4902c02
	.word	0xa4a02c02
	.word	0xa4b02c02
	.word	0xa4c02c02
	.word	0xa4d02c02
	.word	0xa4e02c02
	.word	0xa4f02c02
	.word	0xa5002c02
	.word	0xa5102c02
	.word	0xa5202c02
	.word	0xa5302c02
	.word	0xa5402c02
	.word	0xa5502c02
	.word	0xa5602c02
	.word	0xa5702c02
	.word	0xa5802c02
	.word	0xa5902c02
	.word	0xa5a02c02
	.word	0xa5b02c02
	.word	0xa5c02c02
	.word	0xa5d02c02
	.word	0xa5e02c02
	.word	0xa5f02c02
	.word	0xa6002c02
	.word	0xa6102c02
	.word	0xa6202c02
	.word	0xa6302c02
	.word	0xa6402c02
	.word	0xa6502c02
	.word	0xa6602c02
	.word	0xa6702c02
	.word	0xa6802c02
	.word	0xa6902c02
	.word	0xa6a02c02
	.word	0xa6b02c02
	.word	0xa6c02c02
	.word	0xa6d02c02
	.word	0xa6e02c02
	.word	0xa6f02c02
	.word	0xa7002c02
	.word	0xa7102c02
	.word	0xa7202c02
	.word	0xa7302c02
	.word	0xa7402c02
	.word	0xa7502c02
	.word	0xa7602c02
	.word	0xa7702c02
	.word	0xa7802c02
	.word	0xa7902c02
	.word	0xa7a02c02
	.word	0xa7b02c02
	.word	0xa7c02c02
	.word	0xa7d02c02
	.word	0xa7e02c02
	.word	0xa7f02c02
	.word	0xa8002c02
	.word	0xa8102c02
	.word	0xa8202c02
	.word	0xa8302c02
	.word	0xa8402c02
	.word	0xa8502c02
	.word	0xa8602c02
	.word	0xa8702c02
	.word	0xa8802c02
	.word	0xa8902c02
	.word	0xa8a02c02
	.word	0xa8b02c02
	.word	0xa8c02c02
	.word	0xa8d02c02
	.word	0xa8e02c02
	.word	0xa8f02c02
	.word	0xa9002c02
	.word	0xa9102c02
	.word	0xa9202c02
	.word	0xa9302c02
	.word	0xa9402c02
	.word	0xa9502c02
	.word	0xa9602c02
	.word	0xa9702c02
	.word	0xa9802c02
	.word	0xa9902c02
	.word	0xa9a02c02
	.word	0xa9b02c02
	.word	0xa9c02c02
	.word	0xa9d02c02
	.word	0xa9e02c02
	.word	0xa9f02c02
	.word	0xaa002c02
	.word	0xaa102c02
	.word	0xaa202c02
	.word	0xaa302c02
	.word	0xaa402c02
	.word	0xaa502c02
	.word	0xaa602c02
	.word	0xaa702c02
	.word	0xaa802c02
	.word	0xaa902c02
	.word	0xaaa02c02
	.word	0xaab02c02
	.word	0xaac02c02
	.word	0xaad02c02
	.word	0xaae02c02
	.word	0xaaf02c02
	.word	0xab002c02
	.word	0xab102c02
	.word	0xab202c02
	.word	0xab302c02
	.word	0xab402c02
	.word	0xab502c02
	.word	0xab602c02
	.word	0xab702c02
	.word	0xab802c02
	.word	0xab902c02
	.word	0xaba02c02
	.word	0xabb02c02
	.word	0xabc02c02
	.word	0xabd02c02
	.word	0xabe02c02
	.word	0xabf02c02
	.word	0xac002c02
	.word	0xac102c02
	.word	0xac202c02
	.word	0xac302c02
	.word	0xac402c02
	.word	0xac502c02
	.word	0xac602c02
	.word	0xac702c02
	.word	0xac802c02
	.word	0xac902c02
	.word	0xaca02c02
	.word	0xacb02c02
	.word	0xacc02c02
	.word	0xacd02c02
	.word	0xace02c02
	.word	0xacf02c02
	.word	0xad002c02
	.word	0xad102c02
	.word	0xad202c02
	.word	0xad302c02
	.word	0xad402c02
	.word	0xad502c02
	.word	0xad602c02
	.word	0xad702c02
	.word	0xad802c02
	.word	0xad902c02
	.word	0xada02c02
	.word	0xadb02c02
	.word	0xadc02c02
	.word	0xadd02c02
	.word	0xade02c02
	.word	0xadf02c02
	.word	0xae002c02
	.word	0xae102c02
	.word	0xae202c02
	.word	0xae302c02
	.word	0xae402c02
	.word	0xae502c02
	.word	0xae602c02
	.word	0xae702c02
	.word	0xae802c02
	.word	0xae902c02
	.word	0xaea02c02
	.word	0xaeb02c02
	.word	0xaec02c02
	.word	0xaed02c02
	.word	0xaee02c02
	.word	0xaef02c02
	.word	0xaf002c02
	.word	0xaf102c02
	.word	0xaf202c02
	.word	0xaf302c02
	.word	0xaf402c02
	.word	0xaf502c02
	.word	0xaf602c02
	.word	0xaf702c02
	.word	0xaf802c02
	.word	0xaf902c02
	.word	0xafa02c02
	.word	0xafb02c02
	.word	0xafc02c02
	.word	0xafd02c02
	.word	0xafe02c02
	.word	0xaff02c02
	.word	0xb0002c02
	.word	0xb0102c02
	.word	0xb0202c02
	.word	0xb0302c02
	.word	0xb0402c02
	.word	0xb0502c02
	.word	0xb0602c02
	.word	0xb0702c02
	.word	0xb0802c02
	.word	0xb0902c02
	.word	0xb0a02c02
	.word	0xb0b02c02
	.word	0xb0c02c02
	.word	0xb0d02c02
	.word	0xb0e02c02
	.word	0xb0f02c02
	.word	0xb1002c02
	.word	0xb1102c02
	.word	0xb1202c02
	.word	0xb1302c02
	.word	0xb1402c02
	.word	0xb1502c02
	.word	0xb1602c02
	.word	0xb1702c02
	.word	0xb1802c02
	.word	0xb1902c02
	.word	0xb1a02c02
	.word	0xb1b02c02
	.word	0xb1c02c02
	.word	0xb1d02c02
	.word	0xb1e02c02
	.word	0xb1f02c02
	.word	0xb2002c02
	.word	0xb2102c02
	.word	0xb2202c02
	.word	0xb2302c02
	.word	0xb2402c02
	.word	0xb2502c02
	.word	0xb2602c02
	.word	0xb2702c02
	.word	0xb2802c02
	.word	0xb2902c02
	.word	0xb2a02c02
	.word	0xb2b02c02
	.word	0xb2c02c02
	.word	0xb2d02c02
	.word	0xb2e02c02
	.word	0xb2f02c02
	.word	0xb3002c02
	.word	0xb3102c02
	.word	0xb3202c02
	.word	0xb3302c02
	.word	0xb3402c02
	.word	0xb3502c02
	.word	0xb3602c02
	.word	0xb3702c02
	.word	0xb3802c02
	.word	0xb3902c02
	.word	0xb3a02c02
	.word	0xb3b02c02
	.word	0xb3c02c02
	.word	0xb3d02c02
	.word	0xb3e02c02
	.word	0xb3f02c02
	.word	0xb4002c02
	.word	0xb4102c02
	.word	0xb4202c02
	.word	0xb4302c02
	.word	0xb4402c02
	.word	0xb4502c02
	.word	0xb4602c02
	.word	0xb4702c02
	.word	0xb4802c02
	.word	0xb4902c02
	.word	0xb4a02c02
	.word	0xb4b02c02
	.word	0xb4c02c02
	.word	0xb4d02c02
	.word	0xb4e02c02
	.word	0xb4f02c02
	.word	0xb5002c02
	.word	0xb5102c02
	.word	0xb5202c02
	.word	0xb5302c02
	.word	0xb5402c02
	.word	0xb5502c02
	.word	0xb5602c02
	.word	0xb5702c02
	.word	0xb5802c02
	.word	0xb5902c02
	.word	0xb5a02c02
	.word	0xb5b02c02
	.word	0xb5c02c02
	.word	0xb5d02c02
	.word	0xb5e02c02
	.word	0xb5f02c02
	.word	0xb6002c02
	.word	0xb6102c02
	.word	0xb6202c02
	.word	0xb6302c02
	.word	0xb6402c02
	.word	0xb6502c02
	.word	0xb6602c02
	.word	0xb6702c02
	.word	0xb6802c02
	.word	0xb6902c02
	.word	0xb6a02c02
	.word	0xb6b02c02
	.word	0xb6c02c02
	.word	0xb6d02c02
	.word	0xb6e02c02
	.word	0xb6f02c02
	.word	0xb7002c02
	.word	0xb7102c02
	.word	0xb7202c02
	.word	0xb7302c02
	.word	0xb7402c02
	.word	0xb7502c02
	.word	0xb7602c02
	.word	0xb7702c02
	.word	0xb7802c02
	.word	0xb7902c02
	.word	0xb7a02c02
	.word	0xb7b02c02
	.word	0xb7c02c02
	.word	0xb7d02c02
	.word	0xb7e02c02
	.word	0xb7f02c02
	.word	0xb8002c02
	.word	0xb8102c02
	.word	0xb8202c02
	.word	0xb8302c02
	.word	0xb8402c02
	.word	0xb8502c02
	.word	0xb8602c02
	.word	0xb8702c02
	.word	0xb8802c02
	.word	0xb8902c02
	.word	0xb8a02c02
	.word	0xb8b02c02
	.word	0xb8c02c02
	.word	0xb8d02c02
	.word	0xb8e02c02
	.word	0xb8f02c02
	.word	0xb9002c02
	.word	0xb9102c02
	.word	0xb9202c02
	.word	0xb9302c02
	.word	0xb9402c02
	.word	0xb9502c02
	.word	0xb9602c02
	.word	0xb9702c02
	.word	0xb9802c02
	.word	0xb9902c02
	.word	0xb9a02c02
	.word	0xb9b02c02
	.word	0xb9c02c02
	.word	0xb9d02c02
	.word	0xb9e02c02
	.word	0xb9f02c02
	.word	0xba002c02
	.word	0xba102c02
	.word	0xba202c02
	.word	0xba302c02
	.word	0xba402c02
	.word	0xba502c02
	.word	0xba602c02
	.word	0xba702c02
	.word	0xba802c02
	.word	0xba902c02
	.word	0xbaa02c02
	.word	0xbab02c02
	.word	0xbac02c02
	.word	0xbad02c02
	.word	0xbae02c02
	.word	0xbaf02c02
	.word	0xbb002c02
	.word	0xbb102c02
	.word	0xbb202c02
	.word	0xbb302c02
	.word	0xbb402c02
	.word	0xbb502c02
	.word	0xbb602c02
	.word	0xbb702c02
	.word	0xbb802c02
	.word	0xbb902c02
	.word	0xbba02c02
	.word	0xbbb02c02
	.word	0xbbc02c02
	.word	0xbbd02c02
	.word	0xbbe02c02
	.word	0xbbf02c02
	.word	0xbc002c02
	.word	0xbc102c02
	.word	0xbc202c02
	.word	0xbc302c02
	.word	0xbc402c02
	.word	0xbc502c02
	.word	0xbc602c02
	.word	0xbc702c02
	.word	0xbc802c02
	.word	0xbc902c02
	.word	0xbca02c02
	.word	0xbcb02c02
	.word	0xbcc02c02
	.word	0xbcd02c02
	.word	0xbce02c02
	.word	0xbcf02c02
	.word	0xbd002c02
	.word	0xbd102c02
	.word	0xbd202c02
	.word	0xbd302c02
	.word	0xbd402c02
	.word	0xbd502c02
	.word	0xbd602c02
	.word	0xbd702c02
	.word	0xbd802c02
	.word	0xbd902c02
	.word	0xbda02c02
	.word	0xbdb02c02
	.word	0xbdc02c02
	.word	0xbdd02c02
	.word	0xbde02c02
	.word	0xbdf02c02
	.word	0xbe002c02
	.word	0xbe102c02
	.word	0xbe202c02
	.word	0xbe302c02
	.word	0xbe402c02
	.word	0xbe502c02
	.word	0xbe602c02
	.word	0xbe702c02
	.word	0xbe802c02
	.word	0xbe902c02
	.word	0xbea02c02
	.word	0xbeb02c02
	.word	0xbec02c02
	.word	0xbed02c02
	.word	0xbee02c02
	.word	0xbef02c02
	.word	0xbf002c02
	.word	0xbf102c02
	.word	0xbf202c02
	.word	0xbf302c02
	.word	0xbf402c02
	.word	0xbf502c02
	.word	0xbf602c02
	.word	0xbf702c02
	.word	0xbf802c02
	.word	0xbf902c02
	.word	0xbfa02c02
	.word	0xbfb02c02
	.word	0xbfc02c02
	.word	0xbfd02c02
	.word	0xbfe02c02
	.word	0xbff02c02
	.word	0xc0015ca2
	.word	0xc0102c02
	.word	0xc0202c02
	.word	0xc0302c02
	.word	0xc0402c02
	.word	0xc0502c02
	.word	0xc0602c02
	.word	0xc0702c02
	.word	0xc0802c02
	.word	0xc0902c02
	.word	0xc0a02c02
	.word	0xc0b02c02
	.word	0xc0c02c02
	.word	0xc0d02c02
	.word	0xc0e02c02
	.word	0xc0f02c02
	.word	0xc1002c02
	.word	0xc1102c02
	.word	0xc1202c02
	.word	0xc1302c02
	.word	0xc1402c02
	.word	0xc1502c02
	.word	0xc1602c02
	.word	0xc1702c02
	.word	0xc1802c02
	.word	0xc1902c02
	.word	0xc1a02c02
	.word	0xc1b02c02
	.word	0xc1c02c02
	.word	0xc1d02c02
	.word	0xc1e02c02
	.word	0xc1f02c02
	.word	0xc2002c02
	.word	0xc2102c02
	.word	0xc2202c02
	.word	0xc2302c02
	.word	0xc2402c02
	.word	0xc2502c02
	.word	0xc2602c02
	.word	0xc2702c02
	.word	0xc2802c02
	.word	0xc2902c02
	.word	0xc2a02c02
	.word	0xc2b02c02
	.word	0xc2c02c02
	.word	0xc2d02c02
	.word	0xc2e02c02
	.word	0xc2f02c02
	.word	0xc3002c02
	.word	0xc3102c02
	.word	0xc3202c02
	.word	0xc3302c02
	.word	0xc3402c02
	.word	0xc3502c02
	.word	0xc3602c02
	.word	0xc3702c02
	.word	0xc3802c02
	.word	0xc3902c02
	.word	0xc3a02c02
	.word	0xc3b02c02
	.word	0xc3c02c02
	.word	0xc3d02c02
	.word	0xc3e02c02
	.word	0xc3f02c02
	.word	0xc4002c02
	.word	0xc4102c02
	.word	0xc4202c02
	.word	0xc4302c02
	.word	0xc4402c02
	.word	0xc4502c02
	.word	0xc4602c02
	.word	0xc4702c02
	.word	0xc4802c02
	.word	0xc4902c02
	.word	0xc4a02c02
	.word	0xc4b02c02
	.word	0xc4c02c02
	.word	0xc4d02c02
	.word	0xc4e02c02
	.word	0xc4f02c02
	.word	0xc5002c02
	.word	0xc5102c02
	.word	0xc5202c02
	.word	0xc5302c02
	.word	0xc5402c02
	.word	0xc5502c02
	.word	0xc5602c02
	.word	0xc5702c02
	.word	0xc5802c02
	.word	0xc5902c02
	.word	0xc5a02c02
	.word	0xc5b02c02
	.word	0xc5c02c02
	.word	0xc5d02c02
	.word	0xc5e02c02
	.word	0xc5f02c02
	.word	0xc6002c02
	.word	0xc6102c02
	.word	0xc6202c02
	.word	0xc6302c02
	.word	0xc6402c02
	.word	0xc6502c02
	.word	0xc6602c02
	.word	0xc6702c02
	.word	0xc6802c02
	.word	0xc6902c02
	.word	0xc6a02c02
	.word	0xc6b02c02
	.word	0xc6c02c02
	.word	0xc6d02c02
	.word	0xc6e02c02
	.word	0xc6f02c02
	.word	0xc7002c02
	.word	0xc7102c02
	.word	0xc7202c02
	.word	0xc7302c02
	.word	0xc7402c02
	.word	0xc7502c02
	.word	0xc7602c02
	.word	0xc7702c02
	.word	0xc7802c02
	.word	0xc7902c02
	.word	0xc7a02c02
	.word	0xc7b02c02
	.word	0xc7c02c02
	.word	0xc7d02c02
	.word	0xc7e02c02
	.word	0xc7f02c02
	.word	0xc8002c02
	.word	0xc8102c02
	.word	0xc8202c02
	.word	0xc8302c02
	.word	0xc8402c02
	.word	0xc8502c02
	.word	0xc8602c02
	.word	0xc8702c02
	.word	0xc8802c02
	.word	0xc8902c02
	.word	0xc8a02c02
	.word	0xc8b02c02
	.word	0xc8c02c02
	.word	0xc8d02c02
	.word	0xc8e02c02
	.word	0xc8f02c02
	.word	0xc9002c02
	.word	0xc9102c02
	.word	0xc9202c02
	.word	0xc9302c02
	.word	0xc9402c02
	.word	0xc9502c02
	.word	0xc9602c02
	.word	0xc9702c02
	.word	0xc9802c02
	.word	0xc9902c02
	.word	0xc9a02c02
	.word	0xc9b02c02
	.word	0xc9c02c02
	.word	0xc9d02c02
	.word	0xc9e02c02
	.word	0xc9f02c02
	.word	0xca002c02
	.word	0xca102c02
	.word	0xca202c02
	.word	0xca302c02
	.word	0xca402c02
	.word	0xca502c02
	.word	0xca602c02
	.word	0xca702c02
	.word	0xca802c02
	.word	0xca902c02
	.word	0xcaa02c02
	.word	0xcab02c02
	.word	0xcac02c02
	.word	0xcad02c02
	.word	0xcae02c02
	.word	0xcaf02c02
	.word	0xcb002c02
	.word	0xcb102c02
	.word	0xcb202c02
	.word	0xcb302c02
	.word	0xcb402c02
	.word	0xcb502c02
	.word	0xcb602c02
	.word	0xcb702c02
	.word	0xcb802c02
	.word	0xcb902c02
	.word	0xcba02c02
	.word	0xcbb02c02
	.word	0xcbc02c02
	.word	0xcbd02c02
	.word	0xcbe02c02
	.word	0xcbf02c02
	.word	0xcc002c02
	.word	0xcc102c02
	.word	0xcc202c02
	.word	0xcc302c02
	.word	0xcc402c02
	.word	0xcc502c02
	.word	0xcc602c02
	.word	0xcc702c02
	.word	0xcc802c02
	.word	0xcc902c02
	.word	0xcca02c02
	.word	0xccb02c02
	.word	0xccc02c02
	.word	0xccd02c02
	.word	0xcce02c02
	.word	0xccf02c02
	.word	0xcd002c02
	.word	0xcd102c02
	.word	0xcd202c02
	.word	0xcd302c02
	.word	0xcd402c02
	.word	0xcd502c02
	.word	0xcd602c02
	.word	0xcd702c02
	.word	0xcd802c02
	.word	0xcd902c02
	.word	0xcda02c02
	.word	0xcdb02c02
	.word	0xcdc02c02
	.word	0xcdd02c02
	.word	0xcde02c02
	.word	0xcdf02c02
	.word	0xce002c02
	.word	0xce102c02
	.word	0xce202c02
	.word	0xce302c02
	.word	0xce402c02
	.word	0xce502c02
	.word	0xce602c02
	.word	0xce702c02
	.word	0xce802c02
	.word	0xce902c02
	.word	0xcea02c02
	.word	0xceb02c02
	.word	0xcec02c02
	.word	0xced02c02
	.word	0xcee02c02
	.word	0xcef02c02
	.word	0xcf002c02
	.word	0xcf102c02
	.word	0xcf202c02
	.word	0xcf302c02
	.word	0xcf402c02
	.word	0xcf502c02
	.word	0xcf602c02
	.word	0xcf702c02
	.word	0xcf802c02
	.word	0xcf902c02
	.word	0xcfa02c02
	.word	0xcfb02c02
	.word	0xcfc02c02
	.word	0xcfd02c02
	.word	0xcfe02c02
	.word	0xcff02c02
	.word	0xd0002c02
	.word	0xd0102c02
	.word	0xd0202c02
	.word	0xd0302c02
	.word	0xd0402c02
	.word	0xd0502c02
	.word	0xd0602c02
	.word	0xd0702c02
	.word	0xd0802c02
	.word	0xd0902c02
	.word	0xd0a02c02
	.word	0xd0b02c02
	.word	0xd0c02c02
	.word	0xd0d02c02
	.word	0xd0e02c02
	.word	0xd0f02c02
	.word	0xd1002c02
	.word	0xd1102c02
	.word	0xd1202c02
	.word	0xd1302c02
	.word	0xd1402c02
	.word	0xd1502c02
	.word	0xd1602c02
	.word	0xd1702c02
	.word	0xd1802c02
	.word	0xd1902c02
	.word	0xd1a02c02
	.word	0xd1b02c02
	.word	0xd1c02c02
	.word	0xd1d02c02
	.word	0xd1e02c02
	.word	0xd1f02c02
	.word	0xd2002c02
	.word	0xd2102c02
	.word	0xd2202c02
	.word	0xd2302c02
	.word	0xd2402c02
	.word	0xd2502c02
	.word	0xd2602c02
	.word	0xd2702c02
	.word	0xd2802c02
	.word	0xd2902c02
	.word	0xd2a02c02
	.word	0xd2b02c02
	.word	0xd2c02c02
	.word	0xd2d02c02
	.word	0xd2e02c02
	.word	0xd2f02c02
	.word	0xd3002c02
	.word	0xd3102c02
	.word	0xd3202c02
	.word	0xd3302c02
	.word	0xd3402c02
	.word	0xd3502c02
	.word	0xd3602c02
	.word	0xd3702c02
	.word	0xd3802c02
	.word	0xd3902c02
	.word	0xd3a02c02
	.word	0xd3b02c02
	.word	0xd3c02c02
	.word	0xd3d02c02
	.word	0xd3e02c02
	.word	0xd3f02c02
	.word	0xd4002c02
	.word	0xd4102c02
	.word	0xd4202c02
	.word	0xd4302c02
	.word	0xd4402c02
	.word	0xd4502c02
	.word	0xd4602c02
	.word	0xd4702c02
	.word	0xd4802c02
	.word	0xd4902c02
	.word	0xd4a02c02
	.word	0xd4b02c02
	.word	0xd4c02c02
	.word	0xd4d02c02
	.word	0xd4e02c02
	.word	0xd4f02c02
	.word	0xd5002c02
	.word	0xd5102c02
	.word	0xd5202c02
	.word	0xd5302c02
	.word	0xd5402c02
	.word	0xd5502c02
	.word	0xd5602c02
	.word	0xd5702c02
	.word	0xd5802c02
	.word	0xd5902c02
	.word	0xd5a02c02
	.word	0xd5b02c02
	.word	0xd5c02c02
	.word	0xd5d02c02
	.word	0xd5e02c02
	.word	0xd5f02c02
	.word	0xd6002c02
	.word	0xd6102c02
	.word	0xd6202c02
	.word	0xd6302c02
	.word	0xd6402c02
	.word	0xd6502c02
	.word	0xd6602c02
	.word	0xd6702c02
	.word	0xd6802c02
	.word	0xd6902c02
	.word	0xd6a02c02
	.word	0xd6b02c02
	.word	0xd6c02c02
	.word	0xd6d02c02
	.word	0xd6e02c02
	.word	0xd6f02c02
	.word	0xd7002c02
	.word	0xd7102c02
	.word	0xd7202c02
	.word	0xd7302c02
	.word	0xd7402c02
	.word	0xd7502c02
	.word	0xd7602c02
	.word	0xd7702c02
	.word	0xd7802c02
	.word	0xd7902c02
	.word	0xd7a02c02
	.word	0xd7b02c02
	.word	0xd7c02c02
	.word	0xd7d02c02
	.word	0xd7e02c02
	.word	0xd7f02c02
	.word	0xd8002c02
	.word	0xd8102c02
	.word	0xd8202c02
	.word	0xd8302c02
	.word	0xd8402c02
	.word	0xd8502c02
	.word	0xd8602c02
	.word	0xd8702c02
	.word	0xd8802c02
	.word	0xd8902c02
	.word	0xd8a02c02
	.word	0xd8b02c02
	.word	0xd8c02c02
	.word	0xd8d02c02
	.word	0xd8e02c02
	.word	0xd8f02c02
	.word	0xd9002c02
	.word	0xd9102c02
	.word	0xd9202c02
	.word	0xd9302c02
	.word	0xd9402c02
	.word	0xd9502c02
	.word	0xd9602c02
	.word	0xd9702c02
	.word	0xd9802c02
	.word	0xd9902c02
	.word	0xd9a02c02
	.word	0xd9b02c02
	.word	0xd9c02c02
	.word	0xd9d02c02
	.word	0xd9e02c02
	.word	0xd9f02c02
	.word	0xda002c02
	.word	0xda102c02
	.word	0xda202c02
	.word	0xda302c02
	.word	0xda402c02
	.word	0xda502c02
	.word	0xda602c02
	.word	0xda702c02
	.word	0xda802c02
	.word	0xda902c02
	.word	0xdaa02c02
	.word	0xdab02c02
	.word	0xdac02c02
	.word	0xdad02c02
	.word	0xdae02c02
	.word	0xdaf02c02
	.word	0xdb002c02
	.word	0xdb102c02
	.word	0xdb202c02
	.word	0xdb302c02
	.word	0xdb402c02
	.word	0xdb502c02
	.word	0xdb602c02
	.word	0xdb702c02
	.word	0xdb802c02
	.word	0xdb902c02
	.word	0xdba02c02
	.word	0xdbb02c02
	.word	0xdbc02c02
	.word	0xdbd02c02
	.word	0xdbe02c02
	.word	0xdbf02c02
	.word	0xdc002c02
	.word	0xdc102c02
	.word	0xdc202c02
	.word	0xdc302c02
	.word	0xdc402c02
	.word	0xdc502c02
	.word	0xdc602c02
	.word	0xdc702c02
	.word	0xdc802c02
	.word	0xdc902c02
	.word	0xdca02c02
	.word	0xdcb02c02
	.word	0xdcc02c02
	.word	0xdcd02c02
	.word	0xdce02c02
	.word	0xdcf02c02
	.word	0xdd002c02
	.word	0xdd102c02
	.word	0xdd202c02
	.word	0xdd302c02
	.word	0xdd402c02
	.word	0xdd502c02
	.word	0xdd602c02
	.word	0xdd702c02
	.word	0xdd802c02
	.word	0xdd902c02
	.word	0xdda02c02
	.word	0xddb02c02
	.word	0xddc02c02
	.word	0xddd02c02
	.word	0xdde02c02
	.word	0xddf02c02
	.word	0xde002c02
	.word	0xde102c02
	.word	0xde202c02
	.word	0xde302c02
	.word	0xde402c02
	.word	0xde502c02
	.word	0xde602c02
	.word	0xde702c02
	.word	0xde802c02
	.word	0xde902c02
	.word	0xdea02c02
	.word	0xdeb02c02
	.word	0xdec02c02
	.word	0xded02c02
	.word	0xdee02c02
	.word	0xdef02c02
	.word	0xdf002c02
	.word	0xdf102c02
	.word	0xdf202c02
	.word	0xdf302c02
	.word	0xdf402c02
	.word	0xdf502c02
	.word	0xdf602c02
	.word	0xdf702c02
	.word	0xdf802c02
	.word	0xdf902c02
	.word	0xdfa02c02
	.word	0xdfb02c02
	.word	0xdfc02c02
	.word	0xdfd02c02
	.word	0xdfe02c02
	.word	0xdff02c02
	.word	0xe0002c02
	.word	0xe0102c02
	.word	0xe0202c02
	.word	0xe0302c02
	.word	0xe0402c02
	.word	0xe0502c02
	.word	0xe0602c02
	.word	0xe0702c02
	.word	0xe0802c02
	.word	0xe0902c02
	.word	0xe0a02c02
	.word	0xe0b02c02
	.word	0xe0c02c02
	.word	0xe0d02c02
	.word	0xe0e02c02
	.word	0xe0f02c02
	.word	0xe1002c02
	.word	0xe1102c02
	.word	0xe1202c02
	.word	0xe1302c02
	.word	0xe1402c02
	.word	0xe1502c02
	.word	0xe1602c02
	.word	0xe1702c02
	.word	0xe1802c02
	.word	0xe1902c02
	.word	0xe1a02c02
	.word	0xe1b02c02
	.word	0xe1c02c02
	.word	0xe1d02c02
	.word	0xe1e02c02
	.word	0xe1f02c02
	.word	0xe2002c02
	.word	0xe2102c02
	.word	0xe2202c02
	.word	0xe2302c02
	.word	0xe2402c02
	.word	0xe2502c02
	.word	0xe2602c02
	.word	0xe2702c02
	.word	0xe2802c02
	.word	0xe2902c02
	.word	0xe2a02c02
	.word	0xe2b02c02
	.word	0xe2c02c02
	.word	0xe2d02c02
	.word	0xe2e02c02
	.word	0xe2f02c02
	.word	0xe3002c02
	.word	0xe3102c02
	.word	0xe3202c02
	.word	0xe3302c02
	.word	0xe3402c02
	.word	0xe3502c02
	.word	0xe3602c02
	.word	0xe3702c02
	.word	0xe3802c02
	.word	0xe3902c02
	.word	0xe3a02c02
	.word	0xe3b02c02
	.word	0xe3c02c02
	.word	0xe3d02c02
	.word	0xe3e02c02
	.word	0xe3f02c02
	.word	0xe4002c02
	.word	0xe4102c02
	.word	0xe4202c02
	.word	0xe4302c02
	.word	0xe4402c02
	.word	0xe4502c02
	.word	0xe4602c02
	.word	0xe4702c02
	.word	0xe4802c02
	.word	0xe4902c02
	.word	0xe4a02c02
	.word	0xe4b02c02
	.word	0xe4c02c02
	.word	0xe4d02c02
	.word	0xe4e02c02
	.word	0xe4f02c02
	.word	0xe5002c02
	.word	0xe5102c02
	.word	0xe5202c02
	.word	0xe5302c02
	.word	0xe5402c02
	.word	0xe5502c02
	.word	0xe5602c02
	.word	0xe5702c02
	.word	0xe5802c02
	.word	0xe5902c02
	.word	0xe5a02c02
	.word	0xe5b02c02
	.word	0xe5c02c02
	.word	0xe5d02c02
	.word	0xe5e02c02
	.word	0xe5f02c02
	.word	0xe6002c02
	.word	0xe6102c02
	.word	0xe6202c02
	.word	0xe6302c02
	.word	0xe6402c02
	.word	0xe6502c02
	.word	0xe6602c02
	.word	0xe6702c02
	.word	0xe6802c02
	.word	0xe6902c02
	.word	0xe6a02c02
	.word	0xe6b02c02
	.word	0xe6c02c02
	.word	0xe6d02c02
	.word	0xe6e02c02
	.word	0xe6f02c02
	.word	0xe7002c02
	.word	0xe7102c02
	.word	0xe7202c02
	.word	0xe7302c02
	.word	0xe7402c02
	.word	0xe7502c02
	.word	0xe7602c02
	.word	0xe7702c02
	.word	0xe7802c02
	.word	0xe7902c02
	.word	0xe7a02c02
	.word	0xe7b02c02
	.word	0xe7c02c02
	.word	0xe7d02c02
	.word	0xe7e02c02
	.word	0xe7f02c02
	.word	0xe8002c02
	.word	0xe8102c02
	.word	0xe8202c02
	.word	0xe8302c02
	.word	0xe8402c02
	.word	0xe8502c02
	.word	0xe8602c02
	.word	0xe8702c02
	.word	0xe8802c02
	.word	0xe8902c02
	.word	0xe8a02c02
	.word	0xe8b02c02
	.word	0xe8c02c02
	.word	0xe8d02c02
	.word	0xe8e02c02
	.word	0xe8f02c02
	.word	0xe9002c02
	.word	0xe9102c02
	.word	0xe9202c02
	.word	0xe9302c02
	.word	0xe9402c02
	.word	0xe9502c02
	.word	0xe9602c02
	.word	0xe9702c02
	.word	0xe9802c02
	.word	0xe9902c02
	.word	0xe9a02c02
	.word	0xe9b02c02
	.word	0xe9c02c02
	.word	0xe9d02c02
	.word	0xe9e02c02
	.word	0xe9f02c02
	.word	0xea002c02
	.word	0xea102c02
	.word	0xea202c02
	.word	0xea302c02
	.word	0xea402c02
	.word	0xea502c02
	.word	0xea602c02
	.word	0xea702c02
	.word	0xea802c02
	.word	0xea902c02
	.word	0xeaa02c02
	.word	0xeab02c02
	.word	0xeac02c02
	.word	0xead02c02
	.word	0xeae02c02
	.word	0xeaf02c02
	.word	0xeb002c02
	.word	0xeb102c02
	.word	0xeb202c02
	.word	0xeb302c02
	.word	0xeb402c02
	.word	0xeb502c02
	.word	0xeb602c02
	.word	0xeb702c02
	.word	0xeb802c02
	.word	0xeb902c02
	.word	0xeba02c02
	.word	0xebb02c02
	.word	0xebc02c02
	.word	0xebd02c02
	.word	0xebe02c02
	.word	0xebf02c02
	.word	0xec002c02
	.word	0xec102c02
	.word	0xec202c02
	.word	0xec302c02
	.word	0xec402c02
	.word	0xec502c02
	.word	0xec602c02
	.word	0xec702c02
	.word	0xec802c02
	.word	0xec902c02
	.word	0xeca02c02
	.word	0xecb02c02
	.word	0xecc02c02
	.word	0xecd02c02
	.word	0xece02c02
	.word	0xecf02c02
	.word	0xed002c02
	.word	0xed102c02
	.word	0xed202c02
	.word	0xed302c02
	.word	0xed402c02
	.word	0xed502c02
	.word	0xed602c02
	.word	0xed702c02
	.word	0xed802c02
	.word	0xed902c02
	.word	0xeda02c02
	.word	0xedb02c02
	.word	0xedc02c02
	.word	0xedd02c02
	.word	0xede02c02
	.word	0xedf02c02
	.word	0xee002c02
	.word	0xee102c02
	.word	0xee202c02
	.word	0xee302c02
	.word	0xee402c02
	.word	0xee502c02
	.word	0xee602c02
	.word	0xee702c02
	.word	0xee802c02
	.word	0xee902c02
	.word	0xeea02c02
	.word	0xeeb02c02
	.word	0xeec02c02
	.word	0xeed02c02
	.word	0xeee02c02
	.word	0xeef02c02
	.word	0xef002c02
	.word	0xef102c02
	.word	0xef202c02
	.word	0xef302c02
	.word	0xef402c02
	.word	0xef502c02
	.word	0xef602c02
	.word	0xef702c02
	.word	0xef802c02
	.word	0xef902c02
	.word	0xefa02c02
	.word	0xefb02c02
	.word	0xefc02c02
	.word	0xefd02c02
	.word	0xefe02c02
	.word	0xeff02c02
	.word	0xf0002c02
	.word	0xf0102c02
	.word	0xf0202c02
	.word	0xf0302c02
	.word	0xf0402c02
	.word	0xf0502c02
	.word	0xf0602c02
	.word	0xf0702c02
	.word	0xf0802c02
	.word	0xf0902c02
	.word	0xf0a02c02
	.word	0xf0b02c02
	.word	0xf0c02c02
	.word	0xf0d02c02
	.word	0xf0e02c02
	.word	0xf0f02c02
	.word	0xf1002c02
	.word	0xf1102c02
	.word	0xf1202c02
	.word	0xf1302c02
	.word	0xf1402c02
	.word	0xf1502c02
	.word	0xf1602c02
	.word	0xf1702c02
	.word	0xf1802c02
	.word	0xf1902c02
	.word	0xf1a02c02
	.word	0xf1b02c02
	.word	0xf1c02c02
	.word	0xf1d02c02
	.word	0xf1e02c02
	.word	0xf1f02c02
	.word	0xf2002c02
	.word	0xf2102c02
	.word	0xf2202c02
	.word	0xf2302c02
	.word	0xf2402c02
	.word	0xf2502c02
	.word	0xf2602c02
	.word	0xf2702c02
	.word	0xf2802c02
	.word	0xf2902c02
	.word	0xf2a02c02
	.word	0xf2b02c02
	.word	0xf2c02c02
	.word	0xf2d02c02
	.word	0xf2e02c02
	.word	0xf2f02c02
	.word	0xf3002c02
	.word	0xf3102c02
	.word	0xf3202c02
	.word	0xf3302c02
	.word	0xf3402c02
	.word	0xf3502c02
	.word	0xf3602c02
	.word	0xf3702c02
	.word	0xf3802c02
	.word	0xf3902c02
	.word	0xf3a02c02
	.word	0xf3b02c02
	.word	0xf3c02c02
	.word	0xf3d02c02
	.word	0xf3e02c02
	.word	0xf3f02c02
	.word	0xf4002c02
	.word	0xf4102c02
	.word	0xf4202c02
	.word	0xf4302c02
	.word	0xf4402c02
	.word	0xf4502c02
	.word	0xf4602c02
	.word	0xf4702c02
	.word	0xf4802c02
	.word	0xf4902c02
	.word	0xf4a02c02
	.word	0xf4b02c02
	.word	0xf4c02c02
	.word	0xf4d02c02
	.word	0xf4e02c02
	.word	0xf4f02c02
	.word	0xf5002c02
	.word	0xf5102c02
	.word	0xf5202c02
	.word	0xf5302c02
	.word	0xf5402c02
	.word	0xf5502c02
	.word	0xf5602c02
	.word	0xf5702c02
	.word	0xf5802c02
	.word	0xf5902c02
	.word	0xf5a02c02
	.word	0xf5b02c02
	.word	0xf5c02c02
	.word	0xf5d02c02
	.word	0xf5e02c02
	.word	0xf5f02c02
	.word	0xf6002c02
	.word	0xf6102c02
	.word	0xf6202c02
	.word	0xf6302c02
	.word	0xf6402c02
	.word	0xf6502c02
	.word	0xf6602c02
	.word	0xf6702c02
	.word	0xf6802c02
	.word	0xf6902c02
	.word	0xf6a02c02
	.word	0xf6b02c02
	.word	0xf6c02c02
	.word	0xf6d02c02
	.word	0xf6e02c02
	.word	0xf6f02c02
	.word	0xf7002c02
	.word	0xf7102c02
	.word	0xf7202c02
	.word	0xf7302c02
	.word	0xf7402c02
	.word	0xf7502c02
	.word	0xf7602c02
	.word	0xf7702c02
	.word	0xf7802c02
	.word	0xf7902c02
	.word	0xf7a02c02
	.word	0xf7b02c02
	.word	0xf7c02c02
	.word	0xf7d02c02
	.word	0xf7e02c02
	.word	0xf7f02c02
	.word	0xf8002c02
	.word	0xf8102c02
	.word	0xf8202c02
	.word	0xf8302c02
	.word	0xf8402c02
	.word	0xf8502c02
	.word	0xf8602c02
	.word	0xf8702c02
	.word	0xf8802c02
	.word	0xf8902c02
	.word	0xf8a02c02
	.word	0xf8b02c02
	.word	0xf8c02c02
	.word	0xf8d02c02
	.word	0xf8e02c02
	.word	0xf8f02c02
	.word	0xf9002c02
	.word	0xf9102c02
	.word	0xf9202c02
	.word	0xf9302c02
	.word	0xf9402c02
	.word	0xf9502c02
	.word	0xf9602c02
	.word	0xf9702c02
	.word	0xf9802c02
	.word	0xf9902c02
	.word	0xf9a02c02
	.word	0xf9b02c02
	.word	0xf9c02c02
	.word	0xf9d02c02
	.word	0xf9e02c02
	.word	0xf9f02c02
	.word	0xfa002c02
	.word	0xfa102c02
	.word	0xfa202c02
	.word	0xfa302c02
	.word	0xfa402c02
	.word	0xfa502c02
	.word	0xfa602c02
	.word	0xfa702c02
	.word	0xfa802c02
	.word	0xfa902c02
	.word	0xfaa02c02
	.word	0xfab02c02
	.word	0xfac02c02
	.word	0xfad02c02
	.word	0xfae02c02
	.word	0xfaf02c02
	.word	0xfb002c02
	.word	0xfb102c02
	.word	0xfb202c02
	.word	0xfb302c02
	.word	0xfb402c02
	.word	0xfb502c02
	.word	0xfb602c02
	.word	0xfb702c02
	.word	0xfb802c02
	.word	0xfb902c02
	.word	0xfba02c02
	.word	0xfbb02c02
	.word	0xfbc02c02
	.word	0xfbd02c02
	.word	0xfbe02c02
	.word	0xfbf02c02
	.word	0xfc002c02
	.word	0xfc102c02
	.word	0xfc202c02
	.word	0xfc302c02
	.word	0xfc402c02
	.word	0xfc502c02
	.word	0xfc602c02
	.word	0xfc702c02
	.word	0xfc802c02
	.word	0xfc902c02
	.word	0xfca02c02
	.word	0xfcb02c02
	.word	0xfcc02c02
	.word	0xfcd02c02
	.word	0xfce02c02
	.word	0xfcf02c02
	.word	0xfd002c02
	.word	0xfd102c02
	.word	0xfd202c02
	.word	0xfd302c02
	.word	0xfd402c02
	.word	0xfd502c02
	.word	0xfd602c02
	.word	0xfd702c02
	.word	0xfd802c02
	.word	0xfd902c02
	.word	0xfda02c02
	.word	0xfdb02c02
	.word	0xfdc02c02
	.word	0xfdd02c02
	.word	0xfde02c02
	.word	0xfdf02c02
	.word	0xfe002c02
	.word	0xfe102c02
	.word	0xfe202c02
	.word	0xfe302c02
	.word	0xfe402c02
	.word	0xfe502c02
	.word	0xfe602c02
	.word	0xfe702c02
	.word	0xfe802c02
	.word	0xfe902c02
	.word	0xfea02c02
	.word	0xfeb02c02
	.word	0xfec02c02
	.word	0xfed02c02
	.word	0xfee02c02
	.word	0xfef02c02
	.word	0xff002c02
	.word	0xff102c02
	.word	0xff202c02
	.word	0xff302c02
	.word	0xff402c02
	.word	0xff502c02
	.word	0xff602c02
	.word	0xff702c02
	.word	0xff802c02
	.word	0xff902c02
	.word	0xffa02c02
	.word	0xffb02c02
	.word	0xffc02c02
	.word	0xffd02c02
	.word	0xffe02c02
	.word	0xfff02c02
	
	.text
.globl mmu_table_addr
mmu_table_addr:
	.word mmu_table1
#endif

.globl lowlevel_init
lowlevel_init:
	MRC     p15, 0, r0, c0, c0, 5      /* Identify current CPU */
    AND     r0, r0, #0xf
    CMP     r0, #PRIMARY_CPU
    BEQ     core0_init       
	WFE
    B       coreo_init_done

core0_init:
#ifdef CONFIG_L2C_AS_RAM
	/* Disable interrupts */
	mrs	r0, cpsr
	orr	r0, r0, #0xc0
	msr	cpsr,r0
	mov r1, #0
	/* Initialize cp15 registers */
    MCR	p15,0,r1,c5,c0,0   /* DFSR */
    MCR	p15,0,r1,c5,c0,1   /* IFSR */
    MCR	p15,0,r1,c6,c0,0   /* DFAR */
    MCR	p15,0,r1,c6,c0,1   /* WFAR */
	
	@ Disable MMU
	MRC p15, 0, r1, c1, c0, 0 @ Read Control Register configuration data
	BIC r1, r1, #0x1
	MCR p15, 0, r1, c1, c0, 0 @ Write Control Register configuration data
	@ Disable L1 Caches
	MRC p15, 0, r1, c1, c0, 0 @ Read Control Register configuration data
	BIC r1, r1, #(0x1 << 12) @ Disable I Cache
	BIC r1, r1, #(0x1 << 2) @ Disable D Cache
	MCR p15, 0, r1, c1, c0, 0 @ Write Control Register configuration data
	@ Invalidate L1 Caches
	@ Invalidate Instruction cache
	MOV r1, #0
	MCR p15, 0, r1, c7, c5, 0

    @ Invalidate BTAC
	MCR p15, 0, r0, c7, c5, 6 	
    mcr p15, 0, r1, c7, c10, 4 /* DSB */
    mcr p15, 0, r0, c7, c5, 4 /* ISB */ 

    @ Enable I cache
    MRC     p15, 0, r0, c1, c0, 0   @ Read Control Register configuration data
    ORR     r0, r0, #(0x3 << 11)    @ Enable I Cache and Branch prediction
    MCR     p15, 0, r0, c1, c0, 0   @ Write Control Register configuration data

    @  Invalidate Data cache
    @ To make the code general purpose, we calculate the cache size first and loop through each set + way
    MOV  r0, #0                  
    MCR  p15 ,2, r0, c0, c0, 0
    mcr p15, 0, r0, c7, c5, 4 /* ISB */ 
    MRC  p15, 1, r0, c0, c0, 0   @  Read Cache Size ID
    LDR  r4, =0x1ff
    AND  r4, r4, r0, LSR #3      @
    ADD  r4, r4, #1              @  r4 = no: of ways
    LDR  r3, =0x7fff
    AND  r0, r3, r0, LSR #13     @  r0 = no. of sets/lines - 1
    ADD  r0, r0, #1              @  r0 = no: of ways
    MOV  r1, #0                  @  r1 = way counter way_loop
way_loop:
    MOV  r3, #0                  @  r3 = set counter set_loop
set_loop:
    MOV  r2, r1, LSL #30 
    ORR  r2, r3, LSL #5          @  r2 = set/way cache operation format
    MCR  p15, 0, r2, c7, c6, 2   @  Invalidate line described by r2
    ADD  r3, r3, #1              @  Increment set counter
    CMP  r0, r3                  @  Last set reached yet?
    BNE  set_loop                @  if not, iterate set_loop
    ADD  r1, r1, #1              @  else, next
    CMP  r1, r4                  @  Last way reached yet?
    BNE  way_loop                @  if not, iterate way_loop

	@ Enable D Cache
    MCR  p15, 0, r1, c7, c10, 4 /* DSB */
    MRC  p15, 0, r0, c1, c0, 0  @ read CP15 register 1 into r0
    ORR  r0, r0, #0x4           @ enable D Cache
    MCR  p15, 0, r0, c1, c0, 0  @ write CP15 register 1
    MCR  p15, 0, r0, c7, c5, 4 /* ISB */ 


	@ Invalidate TLB
	MCR p15, 0, r1, c8, c7, 0	
#if 0	
	@ Branch Prediction Enable
	MOV r1, #0
	MRC p15, 0, r1, c1, c0, 0 @ Read Control Register configuration data
	ORR r1, r1, #(0x1 << 11) @ Global BP Enable bit
	MCR p15, 0, r1, c1, c0, 0 @ Write Control Register configuration data
	@ Enable D-side Prefetch
	MRC p15, 0, r1, c1, c0, 1 @ Read Auxiliary Control Register
	ORR r1, r0, #(0x1 <<2) @Enable D-side prefetch
	MCR p15, 0, r1, c1, c0, 1 @ Write Auxiliary Control Register
	mcr p15, 0, r1, c7, c10, 4 /* DSB */
	mcr p15, 0, r0, c7, c5, 4 /* ISB */
#endif
	@ DSB causes completion of all cache maintenance operations appearing in program
	@ order before the DSB instruction
	@ An ISB instruction causes the effect of all branch predictor maintenance
	@ operations before the ISB instruction to be visible to all instructions
	@ after the ISB instruction.
	
scu_enable:
	/* Invalidate and enable SCU */
    LDR     r0, =IHOST_SCU_INVALIDATE_ALL
    MOV     r1, #0xFF
    STR     r1, [r0]
    LDR     r0, =IHOST_SCU_CONTROL
    LDR     r1, [r0]
    ORR     r1, r1, #0x1
    STR     r1, [r0]
    
	/* MMU initialization */
    /* Configure translation table base (TTB) control register cp15,c2
     to a value of all zeros, indicates we are using TTB register 0 */
    mov     r0,#0x0
    mcr     p15, 0, r0, c2, c0, 2
	/* Load page table address to CP15 */    
	ldr r0, mmu_table_addr
	mcr p15, 0, r0, c2, c0, 0

	/* Set the access control to Client */
	ldr r0, =0x55555555
	mcr p15, 0, r0, c3, c0, 0

	/* Enable MMU */
    MRC p15, 0, r1, c1, c0, 0
    ORR r1, r1, #0x01
    MCR p15, 0, r1, c1, c0, 0
	MCR p15, 0, r0, c7, c5, 4 /* ISB */


	/* L2CC Initialization */
l2cc_init:	
	/* read chipID */
	ldr     r0,=0x18000000	/* CHIPID */
	ldr     r1,[r0, #0x0]
	ldr 	r3,=0x0000ffff	/* chipid is bit 0-15 */
	and	r3,r3,r1	/* r3 contains  chipid */

	ldr r0, =IHOST_L2C_CACHE_ID /* L2CC reg base */
	ldr r1, [r0, #0x104]
	bic r1, r1, #0x000F0000 /* Clear the Way-size and associativity (8 way */

	/* check if costar*/
	ldr 	r4, =0xcf19	/* 0xcf19/cf1a/cf1b  - costar; 0xcf12/0xcf13/0xcf14 - ns */
	cmp 	r3,r4
	bge	l2_512_l2cache		
	ldr r2, =0x0A130000  /*0x08150000 ? 0x0A130000*/
	/* northstar */
	b	l2_selection_done		
l2_512_l2cache:
	ldr r2, =0x0A150000  /*0x08150000 ? 0x0A130000*/
	/* costar */
l2_selection_done:
	/* orr r2, r2, #1   *//* Enable full line of write zero */
	orr r1, r1, r2 /* Non-secure interrupt access, Way-size 16KB, 16 way
						    and event monitoring */


    str r1, [r0, #0x104]
#if CONFIG_DEFAULT_L2_CACHE_LATENCY

#else
	/* Set the TAG RAM and DATA RAM latencies for Northstar, skip for co-star */
	cmp 	r3,r4
	bge	skip_l2_latency	 
    ldr r1, =0x0
    str r1, [r0, #0x108]
    ldr r1, =0x0777    
    str r1, [r0, #0x10C]
skip_l2_latency:
#endif
    /* Invalidate the entire L2 cache */
    ldr r1, =0xFFFF
    str r1, [r0, #0x77C] /* inv_way */
    
    mov r2, #0
test_inv_way:    
    ldr r1, [r0, #0x77C]
    cmp r1,r2
    BNE test_inv_way
    
    ldr r1, =0x1FF
    str r1, [r0, #0x220] /* int_clear */
    mov r1, #0x01
    str r1, [r0, #0x100] /* control */
	/* Memory barrier */
	mov r1, #0
	mcr p15, 0, r1, c7, c10, 4 /* DSB */

	/*
	MRC p15, 0, r1, c1, c0, 1 @ Read Auxiliary Control Register
	ORR r1, r1, #(0x1 <<3) @Enable write full line of zero mode
	MCR p15, 0, r1, c1, c0, 1 @ Write Auxiliary Control Register
	*/
	MCR p15, 0, r1, c7, c10, 4 /* DSB */
	MCR p15, 0, r0, c7, c5, 4 /* ISB */

    LDR r1, =0xc0000000
    LDR r2, =0xc0020000
    LDR r4, =0xa5a5a5a5
    LDR r5, =0xa5a5a5a5
    LDR r6, =0xa5a5a5a5
    LDR r7, =0xa5a5a5a5
    LDR r8, =0xa5a5a5a5
    LDR r9, =0xa5a5a5a5
    LDR r10, =0xa5a5a5a5
    LDR r11, =0xa5a5a5a5
    /* Enable lockdown by line */
    mov r3, #0x01
    ldr r0, =IHOST_L2C_CACHE_ID
    str r3, [r0, #0x950]
loop1:
    STMIA r1!, {r4-r11}
    ldr r3, =0x10
cdelay1:
    sub r3, r3, #1
    cmp r3, #0
    bne cdelay1
    cmp r1, r2
    bne loop1
#if 0
    /* Locking of cache, all the 16 ways are locked */
    LDR r0, =IHOST_L2C_DATA_LOCKDOWN       
    LDR r1, =0xFFFF
    STR r1, [r0,#0x10]
    LDR r0, =IHOST_L2C_INSTR_LOCKDOWN       
    STR r1, [r0,#0x10]
#endif        
	/* Disable lockdown by line */
    mov r3, #0
    ldr r0, =IHOST_L2C_CACHE_ID
    str r3, [r0, #0x950]		
l2cc_init_done:
#endif
	ldr r0,=ROM_S0_IDM_IO_STATUS  /* get the SKU id */
	ldr	r11,[r0]
	lsr r11,r11, #2 /* Bits [3:2] - strap_sku_vect, 0/3 - HIGH SKU, 1 - Low SKU, 2 - Medium SKU */
	and r11,r11, #3

/* PCIE workaround for MID SKU */
/* Issue: PCIE slave access is blocked by NIC in Medium SKU */
/* Workaround is to change the address mapping of interconnect to HIGH SKU */
	cmp r11, #2 /* Check for SKU */
	bne pcie_wa_done 
	/* Medium SKU */
	eor r1,r1,r1 /* r1=0 */
	ldr r0, =AXIIC_REMAP
	str r1, [r0]
	ldr r0, =SDROM_REMAP_SELECT
	str r1, [r0]
	orr r1, r1, #1
	ldr r0, =SDROM_REMAP_SELECT_ENABLE
	str r1, [r0]
pcie_wa_done:

/* Initialize LCPLL to required clocks for DDR */        
lcpll_init:
#if 0
	ldr	r0,=CRU_CLKSET_KEY_OFFSET
	ldr	r1,=0xea68
	str	r1,[r0]

	ldr	r3,=ChipcommonA_ChipID
	ldr	r4,[r3]
	lsr r4,r4,#ChipcommonA_ChipID__PackageOption_R
	and	r4,r4,#0xf

	cmp	r4,#1
	beq	1f
	cmp	r4,#2
	beq	2f
	/*Default : High SKU, DDR-1600 */
	ldr	r0,=CRU_LCPLL_CONTROL1_OFFSET
	ldr	r1,[r0]
	ldr	r2,=0xf00fffff
	and	r1,r1,r2
	ldr	r2,=0x08000000
	orr	r1,r1,r2
	str	r1,[r0]
	ldr	r0,=CRU_LCPLL_CONTROL2_OFFSET
	ldr	r1,=0x2010081a /* MDIV 0/1/2/3 */
	str	r1,[r0]
	b	en_change
1:  /* Low SKU, DDR-800 */
	ldr	r0,=CRU_LCPLL_CONTROL1_OFFSET
	ldr	r1,[r0]
	ldr	r2,=0xf00fffff
	and	r1,r1,r2
	ldr	r2,=0x07800000 /* NDIV 120 */
	orr	r1,r1,r2
	str	r1,[r0]
	ldr	r0,=CRU_LCPLL_CONTROL2_OFFSET
	ldr	r1,=0x1e0f0f19 /* MDIV 0/1/2/3 */
	str	r1,[r0]
	b	en_change
2: /* Mid SKU, DDR-1066*/
	ldr	r0,=CRU_LCPLL_CONTROL1_OFFSET
	ldr	r1,[r0]
	ldr	r2,=0xf00fffff
	and	r1,r1,r2
	ldr	r2,=0x08000000 /* NDIV 128 */
	orr	r1,r1,r2
	str	r1,[r0]
	ldr	r0,=CRU_LCPLL_CONTROL2_OFFSET
	ldr	r1,=0x20100c1a /* MDIV 0/1/2/3 */
	str	r1,[r0]
	b	en_change
en_change:	
	ldr	r0,=CRU_LCPLL_CONTROL0_OFFSET
	ldr	r1,[r0]
	orr	r1,r1,#0x7
	str	r1,[r0]
	and	r1,r1,#0xfffffff8
	str	r1,[r0]
	ldr	r0,=CRU_CLKSET_KEY_OFFSET
#ifdef CONFIG_DDR3_400MHZ
	ldr	r1,=0xea68
	str	r1,[r0]
	ldr	r0,=0x1800c104 /* LCPLL_NDIV_INT */
	ldr	r1,[r0]
	ldr	r2,=0xf00fffff
	and	r1,r1,r2
	ldr	r2,=0x08000000
	orr	r1,r1,r2
	str	r1,[r0]
	ldr	r0,=0x1800c108 /* LCPLL_CHX_MDIV */
	ldr	r1,[r0]
	ldr	r2,=0x000000ff
	and	r1,r1,r2
	ldr	r2,=0x20101000
	orr	r1,r1,r2
	str	r1,[r0]
	ldr	r0,=0x1800c100 
	ldr	r1,[r0]
	ldr	r2,=0xfffffff8
	and	r1,r1,r2
	ldr	r2,=0x00000007
	orr	r1,r1,r2
	str	r1,[r0]
	ldr	r0,=0x1800c100 
	ldr	r1,[r0]
	ldr	r2,=0xfffffff8
	and	r1,r1,r2
	str	r1,[r0]
	ldr	r0,=0x1800c180 
#endif
	ldr	r1,=0x0
	str	r1,[r0]

#endif

#ifndef CONFIG_L2C_AS_RAM /* Do the DDR initialization */
/* DDR initialization */
ddr_init:
        /* Workaround to configure DDR3 voltage to 1.5v */
        LDR r0, =DDR_S1_IDM_IO_STATUS
        LDR r1, [r0]
	    ANDS r1,r1, #1
	    BEQ ddr3_wa_done 
	    /* DDR3 */
        LDR     r0, =PCU_1V8_1V5_VREGCNTL_OFFSET
        LDR     r1, =0x00000200
        STR     r1, [r0]
        NOP
ddr3_wa_done:	    
        LDR r9, =0x18010000
    	MOV	r8,r9		/* save r9 */

        LDR     r1, =0x18108800
        LDR     r2, =0x00000000
        STR     r2, [r1]
        LDR     r1, =0x18109800
        STR     r2, [r1]
        
        LDR     r1, =0x18108408
        LDR     r2, [r1]
        LDR     r3, =0xf000ffff
        AND     r2, r2, r3
        ORR     r2, r2, #0x01900000
        STR     r2, [r1]

        LDR     r9, =0x18010800
        LDR     r1, =0x0
        LDR     r3, =0x00019000

wait_for_ddr_phy_up:
        CMP     r3, r1
        BEQ     ddr_phy_rdy
        SUB     r3, r3, #0x1
        LDR     r2, =0x0
        LDR     r2, [r9]
        CMP     r2, r1
        BNE     ddr_phy_rdy
        B       wait_for_ddr_phy_up
ddr_phy_rdy:


        LDR     r1, =0x18010814
        LDR     r2, =0x00000010
        STR     r2, [r1]
        LDR     r3, [r9]

        LDR     r4, =0x18010810
        LDR     r1, =0x1
        LDR     r3, =0x00001400

wait_for_ddr_phy_pll_lock:
        CMP     r3, r1
        BEQ     ddr_phy_pll_lock_done 
        SUB     r3, r3, #0x1
        LDR     r2, =0x0
        LDR     r2, [r4]
        AND     r2, r2, #0x1
        CMP     r2, r1
        BEQ     ddr_phy_pll_lock_done
        B       wait_for_ddr_phy_pll_lock
ddr_phy_pll_lock_done:

        LDR     r1, =0x18010b60
        LDR     r2, =0x00000002
        STR     r2, [r1]
        LDR     r3, [r9]


	/* Write 2 if ddr2, 3 if ddr3 */
        LDR     r1, =0x18108500
        LDR     r3, [r1]
        LDR     r2, =0x00000001
        AND     r3, r2, r3
        ORR     r2, r3, #0x2
        LDR     r1, =0x18010bac
        STR     r2, [r1]
        LDR     r3, [r9]

        LDR     r1, =0x1801083c
        LDR     r2, =0x00100000
        STR     r2, [r1]
        LDR     r3, [r9]

        LDR     r1, =0x18010848
        LDR     r2, =0x00000101
        STR     r2, [r1]
        LDR     r3, [r9]
        
        
        LDR     r4, =0x1801084C
        LDR     r1, =0x1
        LDR     r3, =0x00001400


wait_for_ddr_phy_calib_lock:
        CMP     r3, r1
        BEQ     ddr_phy_calib_lock_done 
        SUB     r3, r3, #0x1
        LDR     r2, =0x0
        LDR     r2, [r4]
        AND     r2, r2, #0x1
        CMP     r2, r1
        BEQ     ddr_phy_calib_lock_done
        B       wait_for_ddr_phy_calib_lock
ddr_phy_calib_lock_done:

        
        LDR     r4, =0x1801084C
        LDR     r1, =0x2
        LDR     r2, [r4]
        AND     r2, r2, #0x2
        CMP     r2, r1
        BEQ     ddr_cntrl_prog


calib_override:
        LDR     r1, =0x18010834
        LDR     r2, =0x0001003f
        STR     r2, [r1]
        LDR     r3, [r9]

        LDR     r1, =0x18010a04
        LDR     r2, =0x0003003f
        STR     r2, [r1]
        LDR     r3, [r9]

        LDR     r1, =0x18010a10
        LDR     r2, =0x0003003f
        STR     r2, [r1]
        LDR     r3, [r9]

        LDR     r1, =0x18010a14
        LDR     r2, =0x0003003f
        STR     r2, [r1]
        LDR     r3, [r9]

        LDR     r1, =0x18010a18
        LDR     r2, =0x0003003f
        STR     r2, [r1]
        LDR     r3, [r9]

        LDR     r1, =0x18010a1c
        LDR     r2, =0x0003003f
        STR     r2, [r1]
        LDR     r3, [r9]

        LDR     r1, =0x18010a20
        LDR     r2, =0x0003003f
        STR     r2, [r1]
        LDR     r3, [r9]

        LDR     r1, =0x18010a24
        LDR     r2, =0x0003003f
        STR     r2, [r1]
        LDR     r3, [r9]

        LDR     r1, =0x18010a28
        LDR     r2, =0x0003003f
        STR     r2, [r1]
        LDR     r3, [r9]

        LDR     r1, =0x18010a2c
        LDR     r2, =0x0003003f
        STR     r2, [r1]
        LDR     r3, [r9]

        LDR     r1, =0x18010a30
        LDR     r2, =0x0003003f
        STR     r2, [r1]
        LDR     r3, [r9]

        LDR     r1, =0x18010aa4
        LDR     r2, =0x0003003f
        STR     r2, [r1]
        LDR     r3, [r9]

        LDR     r1, =0x18010ab0
        LDR     r2, =0x0003003f
        STR     r2, [r1]
        LDR     r3, [r9]

        LDR     r1, =0x18010ab4
        LDR     r2, =0x0003003f
        STR     r2, [r1]
        LDR     r3, [r9]

        LDR     r1, =0x18010ab8
        LDR     r2, =0x0003003f
        STR     r2, [r1]
        LDR     r3, [r9]

        LDR     r1, =0x18010abc
        LDR     r2, =0x0003003f
        STR     r2, [r1]
        LDR     r3, [r9]

        LDR     r1, =0x18010ac0
        LDR     r2, =0x0003003f
        STR     r2, [r1]
        LDR     r3, [r9]

        LDR     r1, =0x18010ac4
        LDR     r2, =0x0003003f
        STR     r2, [r1]
        LDR     r3, [r9]

        LDR     r1, =0x18010ac8
        LDR     r2, =0x0003003f
        STR     r2, [r1]
        LDR     r3, [r9]

        LDR     r1, =0x18010acc
        LDR     r2, =0x0003003f
        STR     r2, [r1]
        LDR     r3, [r9]

        LDR     r1, =0x18010ad0
        LDR     r2, =0x0003003f
        STR     r2, [r1]
        LDR     r3, [r9]

        LDR     r1, =0x18010a08
        LDR     r2, =0x0003003f
        STR     r2, [r1]
        LDR     r3, [r9]

        LDR     r1, =0x18010a0c
        LDR     r2, =0x0003003f
        STR     r2, [r1]
        LDR     r3, [r9]

        LDR     r1, =0x18010aa8
        LDR     r2, =0x0003003f
        STR     r2, [r1]
        LDR     r3, [r9]

        LDR     r1, =0x18010aac
        LDR     r2, =0x0003003f
        STR     r2, [r1]
        LDR     r3, [r9]

ddr_cntrl_prog:
        mov	r9,r8		/* restore r9 */

	ldr	r6,=0x0 /* No sdram params */
	ldr	r5,=0x0
init_regs:
	mov	fp,lr			/* save lr across calls */
	adrl	r0,ddr_init_tab
	bl	ddr_init_regs
	mov	lr,fp			/* restore lr */
	
	/* ddr2 or ddr3 */
#if 0
	ldr	r0,[r8,#AI_IOSTATUS]
#else
	ldr	r8,=0x18108500
	ldr	r0,[r8]
#endif
	ldr	r2,=DDR_STAT_DDR3
	and	r0,r0,r2
	cmp	r0,r2
	beq	ddr3_init

ddr2_init:
	mov	fp,lr			/* save lr across calls */
	adrl	r0,ddr2_init_tab
	bl	ddr_init_regs
	mov	lr,fp			/* restore lr */
	b	chparams

ddr3_init:
	mov	fp,lr			/* save lr across calls */
	adrl	r0,ddr3_init_tab
	bl	ddr_init_regs
	mov	lr,fp			/* restore lr */

chparams:
	ldr	r0,=0x0
	cmp	r6,r0
	beq	turnon			/* No sdram params, use default values */

chhalf:
	ldr	r1,=0x80
	and	r1,r1,r6
	cmp	r1,r0
	beq	ch8banks
setreduc:
    ldr r9, =0x18010000
    ldr r1, [r9, #DDRC_CONTROL87]
    ldr	r2,=0x00000100
    orr	r1,r1,r2
    str	r1,[r9,#DDRC_CONTROL87]

ch8banks:
	ldr	r1,=0x40
	and	r1,r1,r6
	cmp	r1,r0
	beq	do4banks
do8banks:
        ldr     r1,[r9,#DDRC_CONTROL82]
        ldr	r2,=~0x00000300
        and	r1,r1,r2
        str	r1,[r9,#DDRC_CONTROL82]
	b	docols
do4banks:
        ldr     r1,[r9,#DDRC_CONTROL82]
        ldr	r2,=~0x00000300
        and	r1,r1,r2
        ldr	r2,=0x00000100
        orr	r1,r1,r2
        str	r1,[r9,#DDRC_CONTROL82]

docols:	
        ldr     r1,[r9,#DDRC_CONTROL82]
        ldr	r2,=~0x07070000
        and	r1,r1,r2
        ldr	r2,=0x700
        and	r2,r2,r6
        lsl	r2,r2,#16
        orr	r1,r1,r2
        str	r1,[r9,#DDRC_CONTROL82]

docaslat:
	ldr	r0,=0x1f
	and	r0,r0,r6		/* cas latency */
	sub	r1,r0,#1		/* wrlat */
	ldr	r2,[r9,#DDRC_CONTROL05]
	ldr	r3,=~0x3f1f3f00
	and	r2,r2,r3
	lsl	r3,r0,#9		/* cas << 9 */
	orr	r2,r2,r3	
	lsl	r3,r0,#25		/* cas << 25 */
	orr	r2,r2,r3
	lsl	r3,r1,#16		/* wrlat << 16 */
	orr	r2,r2,r3
	str	r2,[r9,#DDRC_CONTROL05]
	ldr	r2,[r9,#DDRC_CONTROL06]
	ldr	r3,=~0x0000001f
	and	r2,r2,r3
	orr	r2,r2,r1
	str	r2,[r9,#DDRC_CONTROL06]
	
	ldr	r2,[r9,#DDRC_CONTROL174]
	ldr	r3,=~0x00001f3f
	and	r2,r2,r3
	lsl	r3,r1,#8
	orr	r2,r2,r3
	orr	r2,r2,r1
	str	r2,[r9,#DDRC_CONTROL174]

	ldr	r2,[r9,#DDRC_CONTROL44]
	ldr	r3,=~0x00007000
	and	r2,r2,r3
	mov	r3,r0
	lsl	r3,r3,#12
	orr	r2,r2,r3
	str	r2,[r9,#DDRC_CONTROL44]

	ldr	r2,[r9,#DDRC_CONTROL186]
	ldr	r3,=~0xff000000
	and	r2,r2,r3
	add	r3,r0,#16
	lsl	r3,r3,#24
	orr	r2,r2,r3
	str	r2,[r9,#DDRC_CONTROL186]

	ldr	r8,=0x18108500
	ldr	r3,[r8]
	ldr	r2,=DDR_STAT_DDR3
	and	r3,r3,r2
	cmp	r3,r2
	bne	turnon

	ldr	r2,[r9,#DDRC_CONTROL44]
	ldr	r3,=~0x00007000
	and	r2,r2,r3
	sub	r3,r0,#4
	lsl	r3,r3,#12
	orr	r2,r2,r3
	str	r2,[r9,#DDRC_CONTROL44]
	ldr	r2,[r9,#DDRC_CONTROL45]
	ldr	r3,=~0x00380000
	and	r2,r2,r3
	sub	r3,r0,#6
	lsl	r3,r3,#19
	orr	r2,r2,r3
	str	r2,[r9,#DDRC_CONTROL45]

	ldr	r2,[r9,#DDRC_CONTROL206]
	ldr	r3,=~0x00001f00
	and	r2,r2,r3
	sub	r3,r0,#2
	lsl	r3,r3,#8
	orr	r2,r2,r3
	str	r2,[r9,#DDRC_CONTROL206]

turnon:
	ldr	r0,=DDRC00_START
	ldr	r1,[r9,#DDRC_CONTROL00]
	orr	r1,r1,r0
        str     r1,[r9,#DDRC_CONTROL00]

poll_ddr_ctrl:
        ldr     r0,[r9,#DDRC_CONTROL89]
        ldr     r2,=DDR_INT_INIT_DONE
        and     r0,r2,r0
        cmp     r0,r2
        bne     poll_ddr_ctrl
		
ddr_init_done:
#endif  /* CONFIG_L2C_AS_RAM */     		

#if 1 /* Bump up the CPU clock */
set_cru_policy7:
	ldr r0,=0x1810D500 /* get the SKU id */
	ldr	r3,[r0]
	lsr r3,r3, #2
	and r3,r3, #3
	lsr r2,r3, #1
	and r3,r3, #1
	eor r3,r2,r3
	ldr	r0,=IHOST_PROC_CLK_WR_ACCESS
	ldr	r1,=0xa5a501
	str	r1,[r0]
	ldr	r0,=IHOST_PROC_CLK_PLLARMA
	ldr	r1,=0x1004001
	eor r2,r2,r2
	cmp r3,r2 
	bne setdiv
	orr r1,r1,#0x1000 /* r1 = 0x1006001 */
setdiv:
	str	r1,[r0]
	ldr	r2,=0x0
	
	ldr	r4,=0x100000
poll_plllock:
	cmp	r4,r2
	beq	poll_plllock_done
	sub	r4,r4,#1
	ldr	r1,[r0]
	and	r3,r1,#(1 << IHOST_PROC_CLK_PLLARMA__pllarm_lock)
	cmp	r3,r2
	beq	poll_plllock
poll_plllock_done:
	orr	r1,r1,#(1 << IHOST_PROC_CLK_PLLARMA__pllarm_soft_post_resetb)
	str	r1,[r0]
	ldr	r0,=IHOST_PROC_CLK_POLICY_FREQ
#ifdef CONFIG_HOST_500MHZ
	ldr	r1,=0x86060606
#else
	ldr	r1,=0x87070707
#endif
	str	r1,[r0]
	ldr	r0,=IHOST_PROC_CLK_CORE0_CLKGATE
	ldr	r1,=0x00000303
	str	r1,[r0]
	ldr	r0,=IHOST_PROC_CLK_CORE1_CLKGATE
	str	r1,[r0]
	ldr	r0,=IHOST_PROC_CLK_ARM_SWITCH_CLKGATE
	str	r1,[r0]
	ldr	r0,=IHOST_PROC_CLK_ARM_PERIPH_CLKGATE
	str	r1,[r0]
	ldr	r0,=IHOST_PROC_CLK_APB0_CLKGATE
	str	r1,[r0]

	ldr	r0,=IHOST_PROC_CLK_POLICY_CTL
	ldr	r2,=(1 << IHOST_PROC_CLK_POLICY_CTL__GO)
	ldr	r3,=(1 << IHOST_PROC_CLK_POLICY_CTL__GO_AC)
	orr	r3,r3,r2
	str	r3,[r0]
	ldr	r3,=0x0
	ldr	r4,=0x100000
poll_ccu:
	cmp	r4,r3
	beq	poll_ccu_done
	sub	r4,r4,#1
	ldr	r1,[r0]
	and	r1,r1,r2
	cmp	r1,r3
	bne	poll_ccu
poll_ccu_done:
#endif /* bump up the CPU clock */

/* Setup CCA UART clock divider to 2*/
        LDR     r1, =ChipcommonA_ClkDiv
        LDR     r2, [r1]
        AND     r2, r2, #0xFFFFFF00
        ORR     r2, r2, #0x2 
        STR     r2, [r1]
coreo_init_done:		
		mov	pc, lr

#ifndef CONFIG_L2C_AS_RAM /* DDR init value tables */
ddr_init_regs:
	mov	r2,r0
	ldr	r3,=DDR_TABLE_END
1:	ldr	r4,[r2]
	cmp	r4,r3
	beq	2f
	ldr	r5,[r2,#4]
	lsl	r4,r4,#2
	add	r1,r9,r4
	str	r5,[r1]
	add	r2,r2,#8
	b	1b
2:
	mov	pc,lr

	.align 4
ddr_init_tab:
	.word	14,	0x05051001
	.word	15,	0x000a0a05
	.word	36,	0x01000000
	.word	37,	0x10000000
	.word	38,	0x00100400
	.word	39,	0x00000400
	.word	40,	0x00000100
	.word	42,	0x00000001
	.word	61,	0x00010100
	.word	78,	0x01000200
	.word	79,	0x02000040
	.word	80,	0x00400100
	.word	81,	0x00000200
	.word	83,	0x01ffff0a
	.word	84,	0x01010101
	.word	85,	0x03010101
	.word	86,	0x01000003
	.word	87,	0x0000010c
	.word	88,	0x00010000
	.word	112,	0x00000200
	.word	116,	0x19000000
	.word	117,	0x00000028
	.word	119,	0x00010001
	.word	120,	0x00010001
	.word	121,	0x00010001
	.word	122,	0x00010001
	.word	123,	0x00010001
	.word	130,	0x00000001
	.word	139,	0x00000001
	.word	148,	0xffff0001
	.word	149,	0x00ffff00
	.word	150,	0x0000ffff
	.word	152,	0x03030303
	.word	153,	0x03030303
	.word	156,	0x02006400
	.word	157,	0x02020202
	.word	158,	0x02020202
	.word	160,	0x01020202
	.word	161,	0x01010064
	.word	162,	0x01010101
	.word	163,	0x01010101
	.word	165,	0x00020101
	.word	166,	0x00000064
	.word	168,	0x000a0a00
	.word	170,	0x02000200
	.word	171,	0x02000200
	.word	175,	0x02000200
	.word	176,	0x02000200
	.word	180,	0x80000100
	.word	181,	0x04070303
	.word	182,	0x0000000a
	.word	185,	0x0010ffff
	.word	187,	0x0000000f
	.word	194,	0x00000204
	.word	205,	0x00000040
	.word	DDR_TABLE_END

	.align 4
ddr2_init_tab:
	.word	0,	0x00000400
	.word	3,	0x00000043
	.word	4,	0x000000a7
	.word	5,	0x0a040a02
	.word	6,	0x04020404
	.word	7,	0x05030e14
	.word	8,	0x030e1404
	.word	9,	0x04020305
	.word	10,	0x03005b26
	.word	11,	0x04020303
	.word	12,	0x03005b26
	.word	13,	0x01000003
	.word	16,	0x030000c8
	.word	17,	0x00860000
	.word	18,	0x05050086
	.word	20,	0x00002301
	.word	21,	0x00230a24
	.word	22,	0x00050a24
	.word	23,	0x00000200
	.word	24,	0x000a0002
	.word	25,	0x0002000a
	.word	26,	0x00020008
	.word	27,	0x00c80008
	.word	28,	0x00c80027
	.word	29,	0x00000027
	.word	30,	0x03000001
	.word	31,	0x00030303
	.word	44,	0x00085300
	.word	45,	0x00000004
	.word	46,	0x00040853
	.word	48,	0x08530000
	.word	49,	0x00000004
	.word	50,	0x00040853
	.word	52,	0x08530000
	.word	53,	0x00000004
	.word	54,	0x00040853
	.word	56,	0x08530000
	.word	57,	0x00000004
	.word	58,	0x00040853
	.word	82,	0x01030101
	.word	108,	0x02020101
	.word	109,	0x08080404
	.word	110,	0x03020200
	.word	111,	0x01000202
	.word	128,	0x001d1d00
	.word	129,	0x1d1d0001
	.word	133,	0x00011d1d
	.word	134,	0x00011d1d
	.word	137,	0x001d1d00
	.word	138,	0x1d1d0001
	.word	142,	0x00011d1d
	.word	143,	0x00011d1d
	.word	146,	0x001d1d00
	.word	147,	0x1d1d0001
	.word	169,	0x0a240000
	.word	172,	0x00000a24
	.word	173,	0x000032b4
	.word	174,	0x0a240404
	.word	177,	0x00000a24
	.word	178,	0x000032b4
	.word	179,	0x02020404
	.word	186,	0x15070303
	.word	202,	0x0001046b
	.word	203,	0x0001046b
	.word	206,	0x01030301
	.word	207,	0x00000001
	.word	DDR_TABLE_END

	.align 4
ddr3_init_tab:
	.word	0,	0x00000600
	.word	3,	0x0001046b
	.word	4,	0x00028b0b
	.word	5,	0x0c050c00
	.word	6,	0x04040405
	.word	7,	0x05040e14
	.word	8,	0x040e1404
	.word	9,	0x0c040405
	.word	10,	0x03005b68
	.word	11,	0x0c040404
	.word	12,	0x03005b68
	.word	13,	0x01000004
	.word	16,	0x03000200
	.word	17,	0x00000f0f
	.word	18,	0x05050000
	.word	20,	0x00007801
	.word	21,	0x00780a20
	.word	22,	0x00050a20
	.word	23,	0x00000300
	.word	24,	0x000a0003
	.word	25,	0x0000000a
	.word	27,	0x02000000
	.word	28,	0x0200005a
	.word	29,	0x0000005a
	.word	30,	0x05000001
	.word	31,	0x00050505
	.word	44,	0x00022000
	.word	45,	0x00000046
	.word	46,	0x00460210
	.word	48,	0x02100000
	.word	49,	0x00000046
	.word	50,	0x00460210
	.word	52,	0x02100000
	.word	53,	0x00000046
	.word	54,	0x00460210
	.word	56,	0x02100000
	.word	57,	0x00000046
	.word	58,	0x00460210
	.word	82,	0x01010001
	.word	108,	0x02040108
	.word	109,	0x08010402
	.word	110,	0x02020202
	.word	111,	0x01000201
	.word	128,	0x00212100
	.word	129,	0x21210001
	.word	133,	0x00012121
	.word	134,	0x00012121
	.word	137,	0x00212100
	.word	138,	0x21210001
	.word	142,	0x00012121
	.word	143,	0x00012121
	.word	146,	0x00212100
	.word	147,	0x21210001
	.word	169,	0x0a200000
	.word	172,	0x00000a20
	.word	173,	0x000032a0
	.word	174,	0x0a200505
	.word	177,	0x00000a20
	.word	178,	0x000032a0
	.word	179,	0x02020505
	.word	186,	0x16070303
	.word	202,	0x00000004
	.word	203,	0x00000004
	.word	206,	0x01040401
	.word	207,	0x00000001
	.word	DDR_TABLE_END
#endif /* CONFIG_L2C_AS_RAM */
