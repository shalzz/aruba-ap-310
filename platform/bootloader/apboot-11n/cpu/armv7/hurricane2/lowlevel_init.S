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
 
  #include "asm/arch/socregs.h"
 #include "config.h"
#define JEDEC_8G16_800	(1)
//#define JEDEC_8G16_1333		(1)
 
 
 .globl DDR_PHY_STRP_STAT
DDR_PHY_STRP_STAT:
	.word	0x180108b8
 
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
	.word	0x01004c0a
	.word	0x01104c0a
	.word	0x01204c0a
	.word	0x01304c0a
	.word	0x01404c0a
	.word	0x01504c0a
	.word	0x01604c0a
	.word	0x01704c0a
	.word	0x01804c0a
	.word	0x01904c0a
	.word	0x01a04c0a
	.word	0x01b04c0a
	.word	0x01c04c0a
	.word	0x01d04c0a
	.word	0x01e04c0a
	.word	0x01f04c0a
	.word	0x02004c0a
	.word	0x02104c0a
	.word	0x02204c0a
	.word	0x02304c0a
	.word	0x02404c0a
	.word	0x02504c0a
	.word	0x02604c0a
	.word	0x02704c0a
	.word	0x02804c0a
	.word	0x02904c0a
	.word	0x02a04c0a
	.word	0x02b04c0a
	.word	0x02c04c0a
	.word	0x02d04c0a
	.word	0x02e04c0a
	.word	0x02f04c0a
	.word	0x03004c0a
	.word	0x03104c0a
	.word	0x03204c0a
	.word	0x03304c0a
	.word	0x03404c0a
	.word	0x03504c0a
	.word	0x03604c0a
	.word	0x03704c0a
	.word	0x03804c0a
	.word	0x03904c0a
	.word	0x03a04c0a
	.word	0x03b04c0a
	.word	0x03c04c0a
	.word	0x03d04c0a
	.word	0x03e04c0a
	.word	0x03f04c0a
	.word	0x04004c0a
	.word	0x04104c0a
	.word	0x04204c0a
	.word	0x04304c0a
	.word	0x04404c0a
	.word	0x04504c0a
	.word	0x04604c0a
	.word	0x04704c0a
	.word	0x04804c0a
	.word	0x04904c0a
	.word	0x04a04c0a
	.word	0x04b04c0a
	.word	0x04c04c0a
	.word	0x04d04c0a
	.word	0x04e04c0a
	.word	0x04f04c0a
	.word	0x05004c0a
	.word	0x05104c0a
	.word	0x05204c0a
	.word	0x05304c0a
	.word	0x05404c0a
	.word	0x05504c0a
	.word	0x05604c0a
	.word	0x05704c0a
	.word	0x05804c0a
	.word	0x05904c0a
	.word	0x05a04c0a
	.word	0x05b04c0a
	.word	0x05c04c0a
	.word	0x05d04c0a
	.word	0x05e04c0a
	.word	0x05f04c0a
	.word	0x06004c0a
	.word	0x06104c0a
	.word	0x06204c0a
	.word	0x06304c0a
	.word	0x06404c0a
	.word	0x06504c0a
	.word	0x06604c0a
	.word	0x06704c0a
	.word	0x06804c0a
	.word	0x06904c0a
	.word	0x06a04c0a
	.word	0x06b04c0a
	.word	0x06c04c0a
	.word	0x06d04c0a
	.word	0x06e04c0a
	.word	0x06f04c0a
	.word	0x07004c0a
	.word	0x07104c0a
	.word	0x07204c0a
	.word	0x07304c0a
	.word	0x07404c0a
	.word	0x07504c0a
	.word	0x07604c0a
	.word	0x07704c0a
	.word	0x07804c0a
	.word	0x07904c0a
	.word	0x07a04c0a
	.word	0x07b04c0a
	.word	0x07c04c0a
	.word	0x07d04c0a
	.word	0x07e04c0a
	.word	0x07f04c0a
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
	.word	0x1b002c02 /* SRAM: Set to 0x1b014c02 to make it Shareable, full access, Non-cacheable */
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
	.word	0x1c016caa  @ make flash code L1&L2 cachable
	.word	0x1c104c0a
	.word	0x1c204c0a
	.word	0x1c304c0a
	.word	0x1c404c0a
	.word	0x1c504c0a
	.word	0x1c604c0a
	.word	0x1c704c0a
	.word	0x1c804c0a
	.word	0x1c904c0a
	.word	0x1ca04c0a
	.word	0x1cb04c0a
	.word	0x1cc04c0a
	.word	0x1cd04c0a
	.word	0x1ce04c0a
	.word	0x1cf04c0a
	.word	0x1d004c0a
	.word	0x1d104c0a
	.word	0x1d204c0a
	.word	0x1d304c0a
	.word	0x1d404c0a
	.word	0x1d504c0a
	.word	0x1d604c0a
	.word	0x1d704c0a
	.word	0x1d804c0a
	.word	0x1d904c0a
	.word	0x1da04c0a
	.word	0x1db04c0a
	.word	0x1dc04c0a
	.word	0x1dd04c0a
	.word	0x1de04c0a
	.word	0x1df04c0a
	.word	0x1e014caa
	.word	0x1e104c0a
	.word	0x1e204c0a
	.word	0x1e304c0a
	.word	0x1e404c0a
	.word	0x1e504c0a
	.word	0x1e604c0a
	.word	0x1e704c0a
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
	.word	0x50015ca2 /* L2CRAM locked region, make it L2 cacheable */
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
	.word	0x89002c02
	.word	0x89102c02
	.word	0x89202c02
	.word	0x89302c02
	.word	0x89402c02
	.word	0x89502c02
	.word	0x89602c02
	.word	0x89702c02
	.word	0x89802c02
	.word	0x89902c02
	.word	0x89a02c02
	.word	0x89b02c02
	.word	0x89c02c02
	.word	0x89d02c02
	.word	0x89e02c02
	.word	0x89f02c02
	.word	0x8a002c02
	.word	0x8a102c02
	.word	0x8a202c02
	.word	0x8a302c02
	.word	0x8a402c02
	.word	0x8a502c02
	.word	0x8a602c02
	.word	0x8a702c02
	.word	0x8a802c02
	.word	0x8a902c02
	.word	0x8aa02c02
	.word	0x8ab02c02
	.word	0x8ac02c02
	.word	0x8ad02c02
	.word	0x8ae02c02
	.word	0x8af02c02
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

.globl lowlevel_init
lowlevel_init:
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
	ldr r0, =IHOST_L2C_CACHE_ID /* L2CC reg base */
	ldr r1, [r0, #0x104]
	bic r1, r1, #0x000F0000 /* Clear the Way-size and associativity (8 way */
	ldr r2, =0x0A120000  /*8way set associative, 16KB way-size */

	orr r1, r1, r2 /* Non-secure interrupt access, Way-size 16KB, 16 way
						    and event monitoring */
    str r1, [r0, #0x104]
    /* Invalidate the entire L2 cache */
    ldr r1, =0x00FF
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
	MCR p15, 0, r0, c7, c5, 4 /* ISB */

    LDR r1, =0x50000000
    LDR r2, =0x50020000
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

	/* Disable lockdown by line */
    mov r3, #0
    ldr r0, =IHOST_L2C_CACHE_ID
    str r3, [r0, #0x950]		
l2cc_init_done:
#endif

#if 0 /* Bump up the CPU clock */
set_cru_policy7:
	ldr r0,=CMIC_DEV_REV_ID /* get the device id */
	ldr	r6,[r0]
	lsl r6,r6,#16
	lsr r6,r6, #16 

	ldr	r0,=IHOST_PROC_CLK_WR_ACCESS
	ldr	r1,=0xa5a501
	str	r1,[r0]

	ldr	r0,=IHOST_PROC_CLK_PLLARMA
	ldr	r1,=0x1004001 /* ndiv = 0x40, vco = 1600mhz */
	ldr r2,=0xb150
	cmp r2,r6
	bne setdiv
	/* default 1GHz for freq ID = 7, or 500 MHz for freq ID = 6 */
	orr r1,r1,#0x1000 /* r1 = 0x1005001, ndiv = 0x50, vco = 2GHz */
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
	ldr	r1,=0x86060606
	ldr r4,=0x01010101
	ldr r2,=0xb150
	cmp r2,r6
	bne setpolicy
	orr r1,r1,r4
setpolicy:	
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

#ifndef CONFIG_HURRICANE2_EMULATION
	b ddr_init_done
#endif	

#ifdef JEDEC_8G16_800
/* DDR initialization */
ddr_init:

        LDR     r1, =DDR_S1_IDM_RESET_CONTROL
        LDR     r2, =0x00000000
        STR     r2, [r1]
        LDR     r1, =DDR_S2_IDM_RESET_CONTROL
        STR     r2, [r1]
        
        LDR     r1, =DDR_S1_IDM_IO_CONTROL_DIRECT
        LDR     r2, [r1]
        LDR     r3, =0xf000ffff
        AND     r2, r2, r3
        ORR     r2, r2, #0x01900000
        STR     r2, [r1]

        LDR     r9, =DDR_PHY_CONTROL_REGS_REVISION
        LDR     r1, =0x0
        LDR     r3, =0x00019000

wait_for_ddr_phy_up:
#if 0
        CMP     r3, r1
        BEQ     ddr_phy_rdy
        SUB     r3, r3, #0x1
#endif     
        LDR     r2, =0x0
        LDR     r2, [r9]
        CMP     r2, r1
        BNE     ddr_phy_rdy
        B       wait_for_ddr_phy_up
ddr_phy_rdy:

        LDR     r1, =DDR_PHY_CONTROL_REGS_STRAP_CONTROL
        LDR     r2, =0x0190d4d7
        STR     r2, [r1]
        LDR     r3, [r9]

        LDR     r1, =DDR_PHY_CONTROL_REGS_PLL_CONFIG
        LDR     r2, =0x00000010
        STR     r2, [r1]
        LDR     r3, [r9]

        LDR     r4, =DDR_PHY_CONTROL_REGS_PLL_STATUS
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

        LDR     r1, =DDR_PHY_WORD_LANE_0_READ_DATA_DLY
        LDR     r2, =0x00000002
        STR     r2, [r1]
        LDR     r3, [r9]


/* Write 2 if ddr2, 3 if ddr3 */
        LDR     r1, =DDR_S1_IDM_IO_STATUS
        LDR     r3, [r1]
        LDR     r2, =0x00000001
        AND     r3, r2, r3
        ORR     r2, r3, #0x2
        LDR     r1, =DDR_PHY_WORD_LANE_0_WR_PREAMBLE_MODE
        STR     r2, [r1]
        LDR     r3, [r9]

        LDR     r1, =DDR_PHY_CONTROL_REGS_ZQ_PVT_COMP_CTL
        LDR     r2, =0x00100000
        STR     r2, [r1]
        LDR     r3, [r9]

        LDR     r1, =DDR_PHY_CONTROL_REGS_VDL_CALIBRATE
        LDR     r2, =0x00000101
        STR     r2, [r1]
        LDR     r3, [r9]
        
        
        LDR     r4, =DDR_PHY_CONTROL_REGS_VDL_CALIB_STATUS
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

        
        LDR     r4, =DDR_PHY_CONTROL_REGS_VDL_CALIB_STATUS
        LDR     r1, =0x2
        LDR     r2, [r4]
        AND     r2, r2, #0x2
        CMP     r2, r1
        BEQ     ddr_cntrl_prog


calib_override:
        LDR     r1, =DDR_PHY_CONTROL_REGS_VDL_OVRIDE_BIT_CTL
        LDR     r2, =0x0001003f
        STR     r2, [r1]
        LDR     r3, [r9]

        LDR     r1, =DDR_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE0_W
        LDR     r2, =0x0003003f
        STR     r2, [r1]
        LDR     r3, [r9]

        LDR     r1, =DDR_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE0_BIT0_W
        LDR     r2, =0x0003003f
        STR     r2, [r1]
        LDR     r3, [r9]

        LDR     r1, =DDR_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE0_BIT1_W
        LDR     r2, =0x0003003f
        STR     r2, [r1]
        LDR     r3, [r9]

        LDR     r1, =DDR_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE0_BIT2_W
        LDR     r2, =0x0003003f
        STR     r2, [r1]
        LDR     r3, [r9]

        LDR     r1, =DDR_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE0_BIT3_W
        LDR     r2, =0x0003003f
        STR     r2, [r1]
        LDR     r3, [r9]

        LDR     r1, =DDR_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE0_BIT4_W
        LDR     r2, =0x0003003f
        STR     r2, [r1]
        LDR     r3, [r9]

        LDR     r1, =DDR_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE0_BIT5_W
        LDR     r2, =0x0003003f
        STR     r2, [r1]
        LDR     r3, [r9]

        LDR     r1, =DDR_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE0_BIT6_W
        LDR     r2, =0x0003003f
        STR     r2, [r1]
        LDR     r3, [r9]

        LDR     r1, =DDR_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE0_BIT7_W
        LDR     r2, =0x0003003f
        STR     r2, [r1]
        LDR     r3, [r9]

        LDR     r1, =DDR_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE0_DM_W
        LDR     r2, =0x0003003f
        STR     r2, [r1]
        LDR     r3, [r9]

        LDR     r1, =DDR_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE1_W
        LDR     r2, =0x0003003f
        STR     r2, [r1]
        LDR     r3, [r9]

        LDR     r1, =DDR_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE1_BIT0_W
        LDR     r2, =0x0003003f
        STR     r2, [r1]
        LDR     r3, [r9]

        LDR     r1, =DDR_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE1_BIT1_W
        LDR     r2, =0x0003003f
        STR     r2, [r1]
        LDR     r3, [r9]

        LDR     r1, =DDR_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE1_BIT2_W
        LDR     r2, =0x0003003f
        STR     r2, [r1]
        LDR     r3, [r9]

        LDR     r1, =DDR_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE1_BIT3_W
        LDR     r2, =0x0003003f
        STR     r2, [r1]
        LDR     r3, [r9]

        LDR     r1, =DDR_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE1_BIT4_W
        LDR     r2, =0x0003003f
        STR     r2, [r1]
        LDR     r3, [r9]

        LDR     r1, =DDR_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE1_BIT5_W
        LDR     r2, =0x0003003f
        STR     r2, [r1]
        LDR     r3, [r9]

        LDR     r1, =DDR_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE1_BIT6_W
        LDR     r2, =0x0003003f
        STR     r2, [r1]
        LDR     r3, [r9]

        LDR     r1, =DDR_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE1_BIT7_W
        LDR     r2, =0x0003003f
        STR     r2, [r1]
        LDR     r3, [r9]

        LDR     r1, =DDR_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE1_DM_W
        LDR     r2, =0x0003003f
        STR     r2, [r1]
        LDR     r3, [r9]

        LDR     r1, =DDR_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE0_R_P
        LDR     r2, =0x0003003f
        STR     r2, [r1]
        LDR     r3, [r9]

        LDR     r1, =DDR_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE0_R_N
        LDR     r2, =0x0003003f
        STR     r2, [r1]
        LDR     r3, [r9]

        LDR     r1, =DDR_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE1_R_P
        LDR     r2, =0x0003003f
        STR     r2, [r1]
        LDR     r3, [r9]

        LDR     r1, =DDR_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE1_R_N
        LDR     r2, =0x0003003f
        STR     r2, [r1]
        LDR     r3, [r9]


ddr_cntrl_prog:
        LDR     r1, =DDR_DENALI_CTL_01
        LDR     r2, =0x00000000
        STR     r2, [r1]

        LDR     r1, =DDR_DENALI_CTL_03
        LDR     r2, =0x00000050
        STR     r2, [r1]

        LDR     r1, =DDR_DENALI_CTL_04
        LDR     r2, =0x000000c8
        STR     r2, [r1]

        LDR     r1, =DDR_DENALI_CTL_05
        LDR     r2, =0x0c050c00
        STR     r2, [r1]

        LDR     r1, =DDR_DENALI_CTL_06
        LDR     r2, =0x04040405
        STR     r2, [r1]

        LDR     r1, =DDR_DENALI_CTL_07
        LDR     r2, =0x06040f15
        STR     r2, [r1]

        LDR     r1, =DDR_DENALI_CTL_08
        LDR     r2, =0x040f1504
        STR     r2, [r1]

        LDR     r1, =DDR_DENALI_CTL_09
        LDR     r2, =0x0c040406
        STR     r2, [r1]

        LDR     r1, =DDR_DENALI_CTL_10
        LDR     r2, =0x03006db0
        STR     r2, [r1]

        LDR     r1, =DDR_DENALI_CTL_11
        LDR     r2, =0x0c040404
        STR     r2, [r1]

        LDR     r1, =DDR_DENALI_CTL_12
        LDR     r2, =0x03006db0
        STR     r2, [r1]

        LDR     r1, =DDR_DENALI_CTL_13
        LDR     r2, =0x01010004
        STR     r2, [r1]

        LDR     r1, =DDR_DENALI_CTL_14
        LDR     r2, =0x00000001
        STR     r2, [r1]

        LDR     r1, =DDR_DENALI_CTL_15
        LDR     r2, =0x000c0c00
        STR     r2, [r1]

        LDR     r1, =DDR_DENALI_CTL_16
        LDR     r2, =0x03000200
        STR     r2, [r1]

        LDR     r1, =DDR_DENALI_CTL_17
        LDR     r2, =0x00001616
        STR     r2, [r1]

        LDR     r1, =DDR_DENALI_CTL_18
        LDR     r2, =0x06060000
        STR     r2, [r1]

        LDR     r1, =DDR_DENALI_CTL_19
        LDR     r2, =0x00000000
        STR     r2, [r1]

        LDR     r1, =DDR_DENALI_CTL_20
        LDR     r2, =0x00008c01
        STR     r2, [r1]

        LDR     r1, =DDR_DENALI_CTL_21
        LDR     r2, =0x008c0c28
        STR     r2, [r1]

        LDR     r1, =DDR_DENALI_CTL_22
        LDR     r2, =0x00050c28
        STR     r2, [r1]

        LDR     r1, =DDR_DENALI_CTL_23
        LDR     r2, =0x00000300
        STR     r2, [r1]

        LDR     r1, =DDR_DENALI_CTL_24
        LDR     r2, =0x000a0003
        STR     r2, [r1]

        LDR     r1, =DDR_DENALI_CTL_25
        LDR     r2, =0x0000000a
        STR     r2, [r1]

        LDR     r1, =DDR_DENALI_CTL_26
        LDR     r2, =0x00000000
        STR     r2, [r1]

        LDR     r1, =DDR_DENALI_CTL_27
        LDR     r2, =0x02000000
        STR     r2, [r1]

        LDR     r1, =DDR_DENALI_CTL_28
        LDR     r2, =0x02000090
        STR     r2, [r1]

        LDR     r1, =DDR_DENALI_CTL_29
        LDR     r2, =0x00000090
        STR     r2, [r1]

        LDR     r1, =DDR_DENALI_CTL_30
        LDR     r2, =0x05000001
        STR     r2, [r1]

        LDR     r1, =DDR_DENALI_CTL_31
        LDR     r2, =0x00050505
        STR     r2, [r1]

        LDR     r1, =DDR_DENALI_CTL_32
        LDR     r2, =0x00000000
        STR     r2, [r1]

        LDR     r1, =DDR_DENALI_CTL_35
        LDR     r2, =0x00000000
        STR     r2, [r1]

        LDR     r1, =DDR_DENALI_CTL_36
        LDR     r2, =0x01000000
        STR     r2, [r1]

        LDR     r1, =DDR_DENALI_CTL_37
        LDR     r2, =0x10000000
        STR     r2, [r1]

        LDR     r1, =DDR_DENALI_CTL_38
        LDR     r2, =0x00100400
        STR     r2, [r1]

        LDR     r1, =DDR_DENALI_CTL_39
        LDR     r2, =0x00000400
        STR     r2, [r1]

        LDR     r1, =DDR_DENALI_CTL_40
        LDR     r2, =0x00000100
        STR     r2, [r1]

        LDR     r1, =DDR_DENALI_CTL_41
        LDR     r2, =0x00000000
        STR     r2, [r1]

        LDR     r1, =DDR_DENALI_CTL_42
        LDR     r2, =0x00000001
        STR     r2, [r1]

        LDR     r1, =DDR_DENALI_CTL_43
        LDR     r2, =0x00000000
        STR     r2, [r1]

        LDR     r1, =DDR_DENALI_CTL_44
        LDR     r2, =0x00042000
        STR     r2, [r1]

        LDR     r1, =DDR_DENALI_CTL_45
        LDR     r2, =0x00000046
        STR     r2, [r1]

        LDR     r1, =DDR_DENALI_CTL_46
        LDR     r2, =0x00460420
        STR     r2, [r1]

        LDR     r1, =DDR_DENALI_CTL_47
        LDR     r2, =0x00000000
        STR     r2, [r1]

        LDR     r1, =DDR_DENALI_CTL_48
        LDR     r2, =0x04200000
        STR     r2, [r1]

        LDR     r1, =DDR_DENALI_CTL_49
        LDR     r2, =0x00000046
        STR     r2, [r1]

        LDR     r1, =DDR_DENALI_CTL_50
        LDR     r2, =0x00460420
        STR     r2, [r1]

        LDR     r1, =DDR_DENALI_CTL_51
        LDR     r2, =0x00000000
        STR     r2, [r1]

        LDR     r1, =DDR_DENALI_CTL_52
        LDR     r2, =0x04200000
        STR     r2, [r1]

        LDR     r1, =DDR_DENALI_CTL_53
        LDR     r2, =0x00000046
        STR     r2, [r1]

        LDR     r1, =DDR_DENALI_CTL_54
        LDR     r2, =0x00460420
        STR     r2, [r1]

        LDR     r1, =DDR_DENALI_CTL_55
        LDR     r2, =0x00000000
        STR     r2, [r1]

        LDR     r1, =DDR_DENALI_CTL_56
        LDR     r2, =0x04200000
        STR     r2, [r1]

        LDR     r1, =DDR_DENALI_CTL_57
        LDR     r2, =0x00000046
        STR     r2, [r1]

        LDR     r1, =DDR_DENALI_CTL_58
        LDR     r2, =0x00460420
        STR     r2, [r1]

        LDR     r1, =DDR_DENALI_CTL_59
        LDR     r2, =0x00000000
        STR     r2, [r1]

        LDR     r1, =DDR_DENALI_CTL_60
        LDR     r2, =0x00000000
        STR     r2, [r1]

        LDR     r1, =DDR_DENALI_CTL_61
        LDR     r2, =0x00010100
        STR     r2, [r1]

        LDR     r1, =DDR_DENALI_CTL_62
        LDR     r2, =0x00000000
        STR     r2, [r1]

        LDR     r1, =DDR_DENALI_CTL_63
        LDR     r2, =0x00000000
        STR     r2, [r1]

        LDR     r1, =DDR_DENALI_CTL_64
        LDR     r2, =0x00000000
        STR     r2, [r1]

        LDR     r1, =DDR_DENALI_CTL_65
        LDR     r2, =0x00000000
        STR     r2, [r1]

        LDR     r1, =DDR_DENALI_CTL_66
        LDR     r2, =0x00000000
        STR     r2, [r1]

        LDR     r1, =DDR_DENALI_CTL_67
        LDR     r2, =0x00000000
        STR     r2, [r1]

        LDR     r1, =DDR_DENALI_CTL_68
        LDR     r2, =0x00000000
        STR     r2, [r1]

        LDR     r1, =DDR_DENALI_CTL_69
        LDR     r2, =0x00000000
        STR     r2, [r1]

        LDR     r1, =DDR_DENALI_CTL_70
        LDR     r2, =0x00000000
        STR     r2, [r1]

        LDR     r1, =DDR_DENALI_CTL_71
        LDR     r2, =0x00000000
        STR     r2, [r1]

        LDR     r1, =DDR_DENALI_CTL_72
        LDR     r2, =0x00000000
        STR     r2, [r1]

        LDR     r1, =DDR_DENALI_CTL_73
        LDR     r2, =0x00000000
        STR     r2, [r1]

        LDR     r1, =DDR_DENALI_CTL_74
        LDR     r2, =0x00000000
        STR     r2, [r1]

        LDR     r1, =DDR_DENALI_CTL_75
        LDR     r2, =0x00000000
        STR     r2, [r1]

        LDR     r1, =DDR_DENALI_CTL_76
        LDR     r2, =0x00000000
        STR     r2, [r1]

        LDR     r1, =DDR_DENALI_CTL_77
        LDR     r2, =0x00000000
        STR     r2, [r1]

        LDR     r1, =DDR_DENALI_CTL_78
        LDR     r2, =0x01000200
        STR     r2, [r1]

        LDR     r1, =DDR_DENALI_CTL_79
        LDR     r2, =0x02000040
        STR     r2, [r1]

        LDR     r1, =DDR_DENALI_CTL_80
        LDR     r2, =0x00400100
        STR     r2, [r1]

        LDR     r1, =DDR_DENALI_CTL_81
        LDR     r2, =0x00000200
        STR     r2, [r1]

        LDR     r1, =DDR_DENALI_CTL_82
        LDR     r2, =0x01000001
        STR     r2, [r1]

        LDR     r1, =DDR_DENALI_CTL_83
        LDR     r2, =0x01ffff0a
        STR     r2, [r1]

        LDR     r1, =DDR_DENALI_CTL_84
        LDR     r2, =0x01010101
        STR     r2, [r1]

        LDR     r1, =DDR_DENALI_CTL_85
        LDR     r2, =0x01010101
        STR     r2, [r1]

        LDR     r1, =DDR_DENALI_CTL_86
        LDR     r2, =0x03000003
        STR     r2, [r1]

        LDR     r1, =DDR_DENALI_CTL_87
        LDR     r2, =0x0000010c
        STR     r2, [r1]

        LDR     r1, =DDR_DENALI_CTL_88
        LDR     r2, =0x00010000
        STR     r2, [r1]

        LDR     r1, =DDR_DENALI_CTL_89
        LDR     r2, =0x00000000
        STR     r2, [r1]

        LDR     r1, =DDR_DENALI_CTL_91
        LDR     r2, =0x00000000
        STR     r2, [r1]

        LDR     r1, =DDR_DENALI_CTL_92
        LDR     r2, =0x00000000
        STR     r2, [r1]

        LDR     r1, =DDR_DENALI_CTL_93
        LDR     r2, =0x00000000
        STR     r2, [r1]

        LDR     r1, =DDR_DENALI_CTL_94
        LDR     r2, =0x00000000
        STR     r2, [r1]

        LDR     r1, =DDR_DENALI_CTL_95
        LDR     r2, =0x00000000
        STR     r2, [r1]

        LDR     r1, =DDR_DENALI_CTL_96
        LDR     r2, =0x00000000
        STR     r2, [r1]

        LDR     r1, =DDR_DENALI_CTL_97
        LDR     r2, =0x00000000
        STR     r2, [r1]

        LDR     r1, =DDR_DENALI_CTL_98
        LDR     r2, =0x00000000
        STR     r2, [r1]

        LDR     r1, =DDR_DENALI_CTL_99
        LDR     r2, =0x00000000
        STR     r2, [r1]

        LDR     r1, =DDR_DENALI_CTL_100
        LDR     r2, =0x00000000
        STR     r2, [r1]

        LDR     r1, =DDR_DENALI_CTL_101
        LDR     r2, =0x00000000
        STR     r2, [r1]

        LDR     r1, =DDR_DENALI_CTL_102
        LDR     r2, =0x00000000
        STR     r2, [r1]

        LDR     r1, =DDR_DENALI_CTL_103
        LDR     r2, =0x00000000
        STR     r2, [r1]

        LDR     r1, =DDR_DENALI_CTL_104
        LDR     r2, =0x00000000
        STR     r2, [r1]

        LDR     r1, =DDR_DENALI_CTL_105
        LDR     r2, =0x00000000
        STR     r2, [r1]

        LDR     r1, =DDR_DENALI_CTL_106
        LDR     r2, =0x00000000
        STR     r2, [r1]

        LDR     r1, =DDR_DENALI_CTL_107
        LDR     r2, =0x00000000
        STR     r2, [r1]

        LDR     r1, =DDR_DENALI_CTL_108
        LDR     r2, =0x02040108
        STR     r2, [r1]

        LDR     r1, =DDR_DENALI_CTL_109
        LDR     r2, =0x08010402
        STR     r2, [r1]

        LDR     r1, =DDR_DENALI_CTL_110
        LDR     r2, =0x02020202
        STR     r2, [r1]

        LDR     r1, =DDR_DENALI_CTL_111
        LDR     r2, =0x01000200
        STR     r2, [r1]

        LDR     r1, =DDR_DENALI_CTL_112
        LDR     r2, =0x00000200
        STR     r2, [r1]

        LDR     r1, =DDR_DENALI_CTL_113
        LDR     r2, =0x00000000
        STR     r2, [r1]

        LDR     r1, =DDR_DENALI_CTL_114
        LDR     r2, =0x00000000
        STR     r2, [r1]

        LDR     r1, =DDR_DENALI_CTL_115
        LDR     r2, =0x00000000
        STR     r2, [r1]

        LDR     r1, =DDR_DENALI_CTL_116
        LDR     r2, =0x0d000000
        STR     r2, [r1]

        LDR     r1, =DDR_DENALI_CTL_117
        LDR     r2, =0x00000028
        STR     r2, [r1]

        LDR     r1, =DDR_DENALI_CTL_118
        LDR     r2, =0x00000000
        STR     r2, [r1]

        LDR     r1, =DDR_DENALI_CTL_119
        LDR     r2, =0x00010001
        STR     r2, [r1]

        LDR     r1, =DDR_DENALI_CTL_120
        LDR     r2, =0x00010001
        STR     r2, [r1]

        LDR     r1, =DDR_DENALI_CTL_121
        LDR     r2, =0x00010001
        STR     r2, [r1]

        LDR     r1, =DDR_DENALI_CTL_122
        LDR     r2, =0x00010001
        STR     r2, [r1]

        LDR     r1, =DDR_DENALI_CTL_123
        LDR     r2, =0x00010001
        STR     r2, [r1]

        LDR     r1, =DDR_DENALI_CTL_124
        LDR     r2, =0x00000000
        STR     r2, [r1]

        LDR     r1, =DDR_DENALI_CTL_125
        LDR     r2, =0x00000000
        STR     r2, [r1]

        LDR     r1, =DDR_DENALI_CTL_126
        LDR     r2, =0x00000000
        STR     r2, [r1]

        LDR     r1, =DDR_DENALI_CTL_127
        LDR     r2, =0x00000000
        STR     r2, [r1]

        LDR     r1, =DDR_DENALI_CTL_128
        LDR     r2, =0x00232300
        STR     r2, [r1]

        LDR     r1, =DDR_DENALI_CTL_129
        LDR     r2, =0x23230001
        STR     r2, [r1]

        LDR     r1, =DDR_DENALI_CTL_130
        LDR     r2, =0x00000001
        STR     r2, [r1]

        LDR     r1, =DDR_DENALI_CTL_131
        LDR     r2, =0x00000000
        STR     r2, [r1]

        LDR     r1, =DDR_DENALI_CTL_132
        LDR     r2, =0x00000000
        STR     r2, [r1]

        LDR     r1, =DDR_DENALI_CTL_133
        LDR     r2, =0x00012323
        STR     r2, [r1]

        LDR     r1, =DDR_DENALI_CTL_134
        LDR     r2, =0x00012323
        STR     r2, [r1]

        LDR     r1, =DDR_DENALI_CTL_135
        LDR     r2, =0x00000000
        STR     r2, [r1]

        LDR     r1, =DDR_DENALI_CTL_136
        LDR     r2, =0x00000000
        STR     r2, [r1]

        LDR     r1, =DDR_DENALI_CTL_137
        LDR     r2, =0x00232300
        STR     r2, [r1]

        LDR     r1, =DDR_DENALI_CTL_138
        LDR     r2, =0x23230001
        STR     r2, [r1]

        LDR     r1, =DDR_DENALI_CTL_139
        LDR     r2, =0x00000001
        STR     r2, [r1]

        LDR     r1, =DDR_DENALI_CTL_140
        LDR     r2, =0x00000000
        STR     r2, [r1]

        LDR     r1, =DDR_DENALI_CTL_141
        LDR     r2, =0x00000000
        STR     r2, [r1]

        LDR     r1, =DDR_DENALI_CTL_142
        LDR     r2, =0x00012323
        STR     r2, [r1]

        LDR     r1, =DDR_DENALI_CTL_143
        LDR     r2, =0x00012323
        STR     r2, [r1]

        LDR     r1, =DDR_DENALI_CTL_144
        LDR     r2, =0x00000000
        STR     r2, [r1]

        LDR     r1, =DDR_DENALI_CTL_145
        LDR     r2, =0x00000000
        STR     r2, [r1]

        LDR     r1, =DDR_DENALI_CTL_146
        LDR     r2, =0x00232300
        STR     r2, [r1]

        LDR     r1, =DDR_DENALI_CTL_147
        LDR     r2, =0x23230001
        STR     r2, [r1]

        LDR     r1, =DDR_DENALI_CTL_148
        LDR     r2, =0x00000001
        STR     r2, [r1]

        LDR     r1, =DDR_DENALI_CTL_149
        LDR     r2, =0x00000000
        STR     r2, [r1]

        LDR     r1, =DDR_DENALI_CTL_150
        LDR     r2, =0x00000000
        STR     r2, [r1]

        LDR     r1, =DDR_DENALI_CTL_151
        LDR     r2, =0x00000000
        STR     r2, [r1]

        LDR     r1, =DDR_DENALI_CTL_152
        LDR     r2, =0x03030303
        STR     r2, [r1]

        LDR     r1, =DDR_DENALI_CTL_153
        LDR     r2, =0x03030303
        STR     r2, [r1]

        LDR     r1, =DDR_DENALI_CTL_156
        LDR     r2, =0x02006400
        STR     r2, [r1]

        LDR     r1, =DDR_DENALI_CTL_157
        LDR     r2, =0x02020202
        STR     r2, [r1]

        LDR     r1, =DDR_DENALI_CTL_158
        LDR     r2, =0x00020202
        STR     r2, [r1]

        LDR     r1, =DDR_DENALI_CTL_160
        LDR     r2, =0x01000000
        STR     r2, [r1]

        LDR     r1, =DDR_DENALI_CTL_161
        LDR     r2, =0x01010064
        STR     r2, [r1]

        LDR     r1, =DDR_DENALI_CTL_162
        LDR     r2, =0x01010101
        STR     r2, [r1]

        LDR     r1, =DDR_DENALI_CTL_163
        LDR     r2, =0x00000101
        STR     r2, [r1]

        LDR     r1, =DDR_DENALI_CTL_165
        LDR     r2, =0x00020000
        STR     r2, [r1]

        LDR     r1, =DDR_DENALI_CTL_166
        LDR     r2, =0x00000064
        STR     r2, [r1]

        LDR     r1, =DDR_DENALI_CTL_167
        LDR     r2, =0x00000000
        STR     r2, [r1]

        LDR     r1, =DDR_DENALI_CTL_168
        LDR     r2, =0x000b0b00
        STR     r2, [r1]

        LDR     r1, =DDR_DENALI_CTL_169
        LDR     r2, =0x0c280000
        STR     r2, [r1]

        LDR     r1, =DDR_DENALI_CTL_170
        LDR     r2, =0x02000200
        STR     r2, [r1]

        LDR     r1, =DDR_DENALI_CTL_171
        LDR     r2, =0x02000200
        STR     r2, [r1]

        LDR     r1, =DDR_DENALI_CTL_172
        LDR     r2, =0x00000c28
        STR     r2, [r1]

        LDR     r1, =DDR_DENALI_CTL_173
        LDR     r2, =0x00007990
        STR     r2, [r1]

        LDR     r1, =DDR_DENALI_CTL_174
        LDR     r2, =0x0c280505
        STR     r2, [r1]

        LDR     r1, =DDR_DENALI_CTL_175
        LDR     r2, =0x02000200
        STR     r2, [r1]

        LDR     r1, =DDR_DENALI_CTL_176
        LDR     r2, =0x02000200
        STR     r2, [r1]

        LDR     r1, =DDR_DENALI_CTL_177
        LDR     r2, =0x00000c28
        STR     r2, [r1]

        LDR     r1, =DDR_DENALI_CTL_178
        LDR     r2, =0x00007990
        STR     r2, [r1]

        LDR     r1, =DDR_DENALI_CTL_179
        LDR     r2, =0x02020506
        STR     r2, [r1]

        LDR     r1, =DDR_DENALI_CTL_180
        LDR     r2, =0x80000100
        STR     r2, [r1]

        LDR     r1, =DDR_DENALI_CTL_181
        LDR     r2, =0x04070303
        STR     r2, [r1]

        LDR     r1, =DDR_DENALI_CTL_182
        LDR     r2, =0x0000000a
        STR     r2, [r1]

        LDR     r1, =DDR_DENALI_CTL_183
        LDR     r2, =0x00000000
        STR     r2, [r1]

        LDR     r1, =DDR_DENALI_CTL_184
        LDR     r2, =0x00000000
        STR     r2, [r1]

        LDR     r1, =DDR_DENALI_CTL_185
        LDR     r2, =0x0010ffff
        STR     r2, [r1]

        LDR     r1, =DDR_DENALI_CTL_186
        LDR     r2, =0x00070303
        STR     r2, [r1]

        LDR     r1, =DDR_DENALI_CTL_187
        LDR     r2, =0x0000000f
        STR     r2, [r1]

        LDR     r1, =DDR_DENALI_CTL_188
        LDR     r2, =0x00000000
        STR     r2, [r1]

        LDR     r1, =DDR_DENALI_CTL_189
        LDR     r2, =0x00000000
        STR     r2, [r1]

        LDR     r1, =DDR_DENALI_CTL_190
        LDR     r2, =0x00000000
        STR     r2, [r1]

        LDR     r1, =DDR_DENALI_CTL_191
        LDR     r2, =0x00000000
        STR     r2, [r1]

        LDR     r1, =DDR_DENALI_CTL_192
        LDR     r2, =0x00000000
        STR     r2, [r1]

        LDR     r1, =DDR_DENALI_CTL_193
        LDR     r2, =0x00000000
        STR     r2, [r1]

        LDR     r1, =DDR_DENALI_CTL_194
        LDR     r2, =0x00000204
        STR     r2, [r1]

        LDR     r1, =DDR_DENALI_CTL_195
        LDR     r2, =0x00000000
        STR     r2, [r1]

        LDR     r1, =DDR_DENALI_CTL_197
        LDR     r2, =0x00000000
        STR     r2, [r1]

        LDR     r1, =DDR_DENALI_CTL_198
        LDR     r2, =0x00000000
        STR     r2, [r1]

        LDR     r1, =DDR_DENALI_CTL_199
        LDR     r2, =0x00000000
        STR     r2, [r1]

        LDR     r1, =DDR_DENALI_CTL_200
        LDR     r2, =0x00000000
        STR     r2, [r1]

        LDR     r1, =DDR_DENALI_CTL_201
        LDR     r2, =0x00000000
        STR     r2, [r1]

        LDR     r1, =DDR_DENALI_CTL_202
        LDR     r2, =0x00000004
        STR     r2, [r1]

        LDR     r1, =DDR_DENALI_CTL_203
        LDR     r2, =0x00000004
        STR     r2, [r1]

        LDR     r1, =DDR_DENALI_CTL_204
        LDR     r2, =0x00000000
        STR     r2, [r1]

        LDR     r1, =DDR_DENALI_CTL_206
        LDR     r2, =0x02040401
        STR     r2, [r1]

        LDR     r1, =DDR_DENALI_CTL_207
        LDR     r2, =0x00000002
        STR     r2, [r1]

        LDR     r1, =DDR_DENALI_CTL_210
        LDR     r2, =0x06060000
        STR     r2, [r1]

        LDR     r1, =DDR_DENALI_CTL_211
        LDR     r2, =0x00000606
        STR     r2, [r1]

        LDR     r1, =DDR_DENALI_CTL_212
        LDR     r2, =0x00000040
        STR     r2, [r1]

        LDR     r1, =DDR_DENALI_CTL_213
        LDR     r2, =0x00000000
        STR     r2, [r1]

        LDR     r1, =DDR_DENALI_CTL_214
        LDR     r2, =0x01010606
        STR     r2, [r1]

        LDR     r1, =DDR_DENALI_CTL_215
        LDR     r2, =0x00000101
        STR     r2, [r1]

        LDR     r1, =DDR_DENALI_CTL_216
        LDR     r2, =0x00001c1c
        STR     r2, [r1]

        LDR     r1, =DDR_DENALI_CTL_217
        LDR     r2, =0x00000000
        STR     r2, [r1]

        LDR     r1, =DDR_DENALI_CTL_00
        LDR     r2, =0x00000601
        STR     r2, [r1]

poll_ddr_ctrl_begin:
        LDR     r1, =DDR_DENALI_CTL_89
poll_ddr_ctrl:
        LDR     r0, [r1]
        LDR     r2, =0x100
        AND     r0, r2, r0
        CMP     r0, r2
        BNE     poll_ddr_ctrl

ddr_init_done:
#if 0
	b ddr_init_done
#endif	
#endif

#ifdef JEDEC_8G16_1333

ddr_init:

        LDR     r1, =DDR_S1_IDM_RESET_CONTROL
        LDR     r2, =0x00000000
        STR     r2, [r1]
        LDR     r1, =DDR_S2_IDM_RESET_CONTROL
        STR     r2, [r1]
        
        LDR     r1, =DDR_S1_IDM_IO_CONTROL_DIRECT
        LDR     r2, [r1]
        LDR     r3, =0xf000ffff
        AND     r2, r2, r3
        LDR     r3, =0x01900000
        ORR     r2, r2, r3
        STR     r2, [r1]

        LDR     r9, =DDR_PHY_CONTROL_REGS_REVISION
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

        LDR     r1, =DDR_PHY_CONTROL_REGS_STRAP_CONTROL
        LDR     r2, =0x0190d4d7
        STR     r2, [r1]
        LDR     r3, [r9]

        LDR     r1, =DDR_PHY_CONTROL_REGS_STRAP_CONTROL2
        LDR     r2, =0x00160286
        STR     r2, [r1]
        LDR     r3, [r9]

        ;LDR     r1, =DDR_PHY_CONTROL_REGS_PLL_DIVIDERS
        ;LDR     r2, =0x0c10
        ;STR     r2, [r1]
        ;LDR     r3, [r9]

        LDR     r1, =DDR_PHY_CONTROL_REGS_PLL_CONFIG
        LDR     r2, =0x00000010
        STR     r2, [r1]
        LDR     r3, [r9]

        LDR     r4, =DDR_PHY_CONTROL_REGS_PLL_STATUS
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

        LDR     r1, =DDR_PHY_WORD_LANE_0_READ_DATA_DLY
        LDR     r2, =0x00000002
        STR     r2, [r1]
        LDR     r3, [r9]


/* ;;Write 2 if ddr2, 3 if ddr3 */
        LDR     r1, =DDR_S1_IDM_IO_STATUS
        LDR     r3, [r1]
        LDR     r2, =0x00000001
        AND     r3, r2, r3
        ORR     r2, r3, #0x2
        LDR     r1, =DDR_PHY_WORD_LANE_0_WR_PREAMBLE_MODE
        STR     r2, [r1]
        LDR     r3, [r9]

        LDR     r1, =DDR_PHY_CONTROL_REGS_ZQ_PVT_COMP_CTL
        LDR     r2, =0x00100000
        STR     r2, [r1]
        LDR     r3, [r9]

        LDR     r1, =DDR_PHY_CONTROL_REGS_VDL_CALIBRATE
        LDR     r2, =0x00000101
        STR     r2, [r1]
        LDR     r3, [r9]
        
        
        LDR     r4, =DDR_PHY_CONTROL_REGS_VDL_CALIB_STATUS
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

        
        LDR     r4, =DDR_PHY_CONTROL_REGS_VDL_CALIB_STATUS
        LDR     r1, =0x2
        LDR     r2, [r4]
        AND     r2, r2, #0x2
        CMP     r2, r1
        BEQ     ddr_cntrl_prog


calib_override:
        LDR     r1, =DDR_PHY_CONTROL_REGS_VDL_OVRIDE_BIT_CTL
        LDR     r2, =0x0001003f
        STR     r2, [r1]
        LDR     r3, [r9]

        LDR     r1, =DDR_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE0_W
        LDR     r2, =0x0003003f
        STR     r2, [r1]
        LDR     r3, [r9]

        LDR     r1, =DDR_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE0_BIT0_W
        LDR     r2, =0x0003003f
        STR     r2, [r1]
        LDR     r3, [r9]

        LDR     r1, =DDR_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE0_BIT1_W
        LDR     r2, =0x0003003f
        STR     r2, [r1]
        LDR     r3, [r9]

        LDR     r1, =DDR_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE0_BIT2_W
        LDR     r2, =0x0003003f
        STR     r2, [r1]
        LDR     r3, [r9]

        LDR     r1, =DDR_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE0_BIT3_W
        LDR     r2, =0x0003003f
        STR     r2, [r1]
        LDR     r3, [r9]

        LDR     r1, =DDR_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE0_BIT4_W
        LDR     r2, =0x0003003f
        STR     r2, [r1]
        LDR     r3, [r9]

        LDR     r1, =DDR_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE0_BIT5_W
        LDR     r2, =0x0003003f
        STR     r2, [r1]
        LDR     r3, [r9]

        LDR     r1, =DDR_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE0_BIT6_W
        LDR     r2, =0x0003003f
        STR     r2, [r1]
        LDR     r3, [r9]

        LDR     r1, =DDR_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE0_BIT7_W
        LDR     r2, =0x0003003f
        STR     r2, [r1]
        LDR     r3, [r9]

        LDR     r1, =DDR_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE0_DM_W
        LDR     r2, =0x0003003f
        STR     r2, [r1]
        LDR     r3, [r9]

        LDR     r1, =DDR_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE1_W
        LDR     r2, =0x0003003f
        STR     r2, [r1]
        LDR     r3, [r9]

        LDR     r1, =DDR_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE1_BIT0_W
        LDR     r2, =0x0003003f
        STR     r2, [r1]
        LDR     r3, [r9]

        LDR     r1, =DDR_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE1_BIT1_W
        LDR     r2, =0x0003003f
        STR     r2, [r1]
        LDR     r3, [r9]

        LDR     r1, =DDR_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE1_BIT2_W
        LDR     r2, =0x0003003f
        STR     r2, [r1]
        LDR     r3, [r9]

        LDR     r1, =DDR_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE1_BIT3_W
        LDR     r2, =0x0003003f
        STR     r2, [r1]
        LDR     r3, [r9]

        LDR     r1, =DDR_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE1_BIT4_W
        LDR     r2, =0x0003003f
        STR     r2, [r1]
        LDR     r3, [r9]

        LDR     r1, =DDR_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE1_BIT5_W
        LDR     r2, =0x0003003f
        STR     r2, [r1]
        LDR     r3, [r9]

        LDR     r1, =DDR_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE1_BIT6_W
        LDR     r2, =0x0003003f
        STR     r2, [r1]
        LDR     r3, [r9]

        LDR     r1, =DDR_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE1_BIT7_W
        LDR     r2, =0x0003003f
        STR     r2, [r1]
        LDR     r3, [r9]

        LDR     r1, =DDR_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE1_DM_W
        LDR     r2, =0x0003003f
        STR     r2, [r1]
        LDR     r3, [r9]

        LDR     r1, =DDR_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE0_R_P
        LDR     r2, =0x0003003f
        STR     r2, [r1]
        LDR     r3, [r9]

        LDR     r1, =DDR_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE0_R_N
        LDR     r2, =0x0003003f
        STR     r2, [r1]
        LDR     r3, [r9]

        LDR     r1, =DDR_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE1_R_P
        LDR     r2, =0x0003003f
        STR     r2, [r1]
        LDR     r3, [r9]

        LDR     r1, =DDR_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE1_R_N
        LDR     r2, =0x0003003f
        STR     r2, [r1]
        LDR     r3, [r9]


ddr_cntrl_prog:
        LDR     r1, =0x18010004
        LDR     r2, =0x00000000
        STR     r2, [r1]

        LDR     r1, =0x1801000c
        LDR     r2, =0x00000086
        STR     r2, [r1]

        LDR     r1, =0x18010010
        LDR     r2, =0x0000014e
        STR     r2, [r1]

        LDR     r1, =0x18010014
        LDR     r2, =0x12071200
        STR     r2, [r1]

        LDR     r1, =0x18010018
        LDR     r2, =0x05040407
        STR     r2, [r1]

        LDR     r1, =0x1801001c
        LDR     r2, =0x09051821
        STR     r2, [r1]

        LDR     r1, =0x18010020
        LDR     r2, =0x05182105
        STR     r2, [r1]

        LDR     r1, =0x18010024
        LDR     r2, =0x0c040509
        STR     r2, [r1]

        LDR     r1, =0x18010028
        LDR     r2, =0x0400b6d0
        STR     r2, [r1]

        LDR     r1, =0x1801002c
        LDR     r2, =0x0c040504
        STR     r2, [r1]

        LDR     r1, =0x18010030
        LDR     r2, =0x0400b6d0
        STR     r2, [r1]

        LDR     r1, =0x18010034
        LDR     r2, =0x01010004
        STR     r2, [r1]

        LDR     r1, =0x18010038
        LDR     r2, =0x00000001
        STR     r2, [r1]

        LDR     r1, =0x1801003c
        LDR     r2, =0x00131300
        STR     r2, [r1]

        LDR     r1, =0x18010040
        LDR     r2, =0x03000200
        STR     r2, [r1]

        LDR     r1, =0x18010044
        LDR     r2, =0x00001e1e
        STR     r2, [r1]

        LDR     r1, =0x18010048
        LDR     r2, =0x09090000
        STR     r2, [r1]

        LDR     r1, =0x1801004c
        LDR     r2, =0x00000000
        STR     r2, [r1]

        LDR     r1, =0x18010050
        LDR     r2, =0x0000ea01
        STR     r2, [r1]

        LDR     r1, =0x18010054
        LDR     r2, =0x00ea1448
        STR     r2, [r1]

        LDR     r1, =0x18010058
        LDR     r2, =0x00051448
        STR     r2, [r1]

        LDR     r1, =0x1801005c
        LDR     r2, =0x00000400
        STR     r2, [r1]

        LDR     r1, =0x18010060
        LDR     r2, =0x00100004
        STR     r2, [r1]

        LDR     r1, =0x18010064
        LDR     r2, =0x00000010
        STR     r2, [r1]

        LDR     r1, =0x18010068
        LDR     r2, =0x00000000
        STR     r2, [r1]

        LDR     r1, =0x1801006c
        LDR     r2, =0x02000000
        STR     r2, [r1]

        LDR     r1, =0x18010070
        LDR     r2, =0x020000f0
        STR     r2, [r1]

        LDR     r1, =0x18010074
        LDR     r2, =0x000000f0
        STR     r2, [r1]

        LDR     r1, =0x18010078
        LDR     r2, =0x07000001
        STR     r2, [r1]

        LDR     r1, =0x1801007c
        LDR     r2, =0x00070707
        STR     r2, [r1]

        LDR     r1, =0x18010080
        LDR     r2, =0x00000000
        STR     r2, [r1]

        LDR     r1, =0x1801008c
        LDR     r2, =0x00000000
        STR     r2, [r1]

        LDR     r1, =0x18010090
        LDR     r2, =0x01000000
        STR     r2, [r1]

        LDR     r1, =0x18010094
        LDR     r2, =0x10000000
        STR     r2, [r1]

        LDR     r1, =0x18010098
        LDR     r2, =0x00100400
        STR     r2, [r1]

        LDR     r1, =0x1801009c
        LDR     r2, =0x00000400
        STR     r2, [r1]

        LDR     r1, =0x180100a0
        LDR     r2, =0x00000100
        STR     r2, [r1]

        LDR     r1, =0x180100a4
        LDR     r2, =0x00000000
        STR     r2, [r1]

        LDR     r1, =0x180100a8
        LDR     r2, =0x00000001
        STR     r2, [r1]

        LDR     r1, =0x180100ac
        LDR     r2, =0x00000000
        STR     r2, [r1]

        LDR     r1, =0x180100b0
        LDR     r2, =0x000a5000
        STR     r2, [r1]

        LDR     r1, =0x180100b4
        LDR     r2, =0x00100046
        STR     r2, [r1]

        LDR     r1, =0x180100b8
        LDR     r2, =0x00460a50
        STR     r2, [r1]

        LDR     r1, =0x180100bc
        LDR     r2, =0x00000010
        STR     r2, [r1]

        LDR     r1, =0x180100c0
        LDR     r2, =0x0a500000
        STR     r2, [r1]

        LDR     r1, =0x180100c4
        LDR     r2, =0x00100046
        STR     r2, [r1]

        LDR     r1, =0x180100c8
        LDR     r2, =0x00460a50
        STR     r2, [r1]

        LDR     r1, =0x180100cc
        LDR     r2, =0x00000010
        STR     r2, [r1]

        LDR     r1, =0x180100d0
        LDR     r2, =0x0a500000
        STR     r2, [r1]

        LDR     r1, =0x180100d4
        LDR     r2, =0x00100046
        STR     r2, [r1]

        LDR     r1, =0x180100d8
        LDR     r2, =0x00460a50
        STR     r2, [r1]

        LDR     r1, =0x180100dc
        LDR     r2, =0x00000010
        STR     r2, [r1]

        LDR     r1, =0x180100e0
        LDR     r2, =0x0a500000
        STR     r2, [r1]

        LDR     r1, =0x180100e4
        LDR     r2, =0x00100046
        STR     r2, [r1]

        LDR     r1, =0x180100e8
        LDR     r2, =0x00460a50
        STR     r2, [r1]

        LDR     r1, =0x180100ec
        LDR     r2, =0x00000010
        STR     r2, [r1]

        LDR     r1, =0x180100f0
        LDR     r2, =0x00000000
        STR     r2, [r1]

        LDR     r1, =0x180100f4
        LDR     r2, =0x00010100
        STR     r2, [r1]

        LDR     r1, =0x180100f8
        LDR     r2, =0x00000000
        STR     r2, [r1]

        LDR     r1, =0x180100fc
        LDR     r2, =0x00000000
        STR     r2, [r1]

        LDR     r1, =0x18010100
        LDR     r2, =0x00000000
        STR     r2, [r1]

        LDR     r1, =0x18010104
        LDR     r2, =0x00000000
        STR     r2, [r1]

        LDR     r1, =0x18010108
        LDR     r2, =0x00000000
        STR     r2, [r1]

        LDR     r1, =0x1801010c
        LDR     r2, =0x00000000
        STR     r2, [r1]

        LDR     r1, =0x18010110
        LDR     r2, =0x00000000
        STR     r2, [r1]

        LDR     r1, =0x18010114
        LDR     r2, =0x00000000
        STR     r2, [r1]

        LDR     r1, =0x18010118
        LDR     r2, =0x00000000
        STR     r2, [r1]

        LDR     r1, =0x1801011c
        LDR     r2, =0x00000000
        STR     r2, [r1]

        LDR     r1, =0x18010120
        LDR     r2, =0x00000000
        STR     r2, [r1]

        LDR     r1, =0x18010124
        LDR     r2, =0x00000000
        STR     r2, [r1]

        LDR     r1, =0x18010128
        LDR     r2, =0x00000000
        STR     r2, [r1]

        LDR     r1, =0x1801012c
        LDR     r2, =0x00000000
        STR     r2, [r1]

        LDR     r1, =0x18010130
        LDR     r2, =0x00000000
        STR     r2, [r1]

        LDR     r1, =0x18010134
        LDR     r2, =0x00000000
        STR     r2, [r1]

        LDR     r1, =0x18010138
        LDR     r2, =0x01000200
        STR     r2, [r1]

        LDR     r1, =0x1801013c
        LDR     r2, =0x02000040
        STR     r2, [r1]

        LDR     r1, =0x18010140
        LDR     r2, =0x00400100
        STR     r2, [r1]

        LDR     r1, =0x18010144
        LDR     r2, =0x00000200
        STR     r2, [r1]

        LDR     r1, =0x18010148
        LDR     r2, =0x01000001
        STR     r2, [r1]

        LDR     r1, =0x1801014c
        LDR     r2, =0x01ffff0a
        STR     r2, [r1]

        LDR     r1, =0x18010150
        LDR     r2, =0x01010101
        STR     r2, [r1]

        LDR     r1, =0x18010154
        LDR     r2, =0x01010101
        STR     r2, [r1]

        LDR     r1, =0x18010158
        LDR     r2, =0x03000003
        STR     r2, [r1]

        LDR     r1, =0x1801015c
        LDR     r2, =0x0000010c
        STR     r2, [r1]

        LDR     r1, =0x18010160
        LDR     r2, =0x00010000
        STR     r2, [r1]

        LDR     r1, =0x18010164
        LDR     r2, =0x00000000
        STR     r2, [r1]

        LDR     r1, =0x1801016c
        LDR     r2, =0x00000000
        STR     r2, [r1]

        LDR     r1, =0x18010170
        LDR     r2, =0x00000000
        STR     r2, [r1]

        LDR     r1, =0x18010174
        LDR     r2, =0x00000000
        STR     r2, [r1]

        LDR     r1, =0x18010178
        LDR     r2, =0x00000000
        STR     r2, [r1]

        LDR     r1, =0x1801017c
        LDR     r2, =0x00000000
        STR     r2, [r1]

        LDR     r1, =0x18010180
        LDR     r2, =0x00000000
        STR     r2, [r1]

        LDR     r1, =0x18010184
        LDR     r2, =0x00000000
        STR     r2, [r1]

        LDR     r1, =0x18010188
        LDR     r2, =0x00000000
        STR     r2, [r1]

        LDR     r1, =0x1801018c
        LDR     r2, =0x00000000
        STR     r2, [r1]

        LDR     r1, =0x18010190
        LDR     r2, =0x00000000
        STR     r2, [r1]

        LDR     r1, =0x18010194
        LDR     r2, =0x00000000
        STR     r2, [r1]

        LDR     r1, =0x18010198
        LDR     r2, =0x00000000
        STR     r2, [r1]

        LDR     r1, =0x1801019c
        LDR     r2, =0x00000000
        STR     r2, [r1]

        LDR     r1, =0x180101a0
        LDR     r2, =0x00000000
        STR     r2, [r1]

        LDR     r1, =0x180101a4
        LDR     r2, =0x00000000
        STR     r2, [r1]

        LDR     r1, =0x180101a8
        LDR     r2, =0x00000000
        STR     r2, [r1]

        LDR     r1, =0x180101ac
        LDR     r2, =0x00000000
        STR     r2, [r1]

        LDR     r1, =0x180101b0
        LDR     r2, =0x02040108
        STR     r2, [r1]

        LDR     r1, =0x180101b4
        LDR     r2, =0x08010402
        STR     r2, [r1]

        LDR     r1, =0x180101b8
        LDR     r2, =0x02020202
        STR     r2, [r1]

        LDR     r1, =0x180101bc
        LDR     r2, =0x01000200
        STR     r2, [r1]

        LDR     r1, =0x180101c0
        LDR     r2, =0x00000200
        STR     r2, [r1]

        LDR     r1, =0x180101c4
        LDR     r2, =0x00000000
        STR     r2, [r1]

        LDR     r1, =0x180101c8
        LDR     r2, =0x00000000
        STR     r2, [r1]

        LDR     r1, =0x180101cc
        LDR     r2, =0x00000000
        STR     r2, [r1]

        LDR     r1, =0x180101d0
        LDR     r2, =0x0d000000
        STR     r2, [r1]

        LDR     r1, =0x180101d4
        LDR     r2, =0x00000028
        STR     r2, [r1]

        LDR     r1, =0x180101d8
        LDR     r2, =0x00000000
        STR     r2, [r1]

        LDR     r1, =0x180101dc
        LDR     r2, =0x00010001
        STR     r2, [r1]

        LDR     r1, =0x180101e0
        LDR     r2, =0x00010001
        STR     r2, [r1]

        LDR     r1, =0x180101e4
        LDR     r2, =0x00010001
        STR     r2, [r1]

        LDR     r1, =0x180101e8
        LDR     r2, =0x00010001
        STR     r2, [r1]

        LDR     r1, =0x180101ec
        LDR     r2, =0x00010001
        STR     r2, [r1]

        LDR     r1, =0x180101f0
        LDR     r2, =0x00000000
        STR     r2, [r1]

        LDR     r1, =0x180101f4
        LDR     r2, =0x00000000
        STR     r2, [r1]

        LDR     r1, =0x180101f8
        LDR     r2, =0x00000000
        STR     r2, [r1]

        LDR     r1, =0x180101fc
        LDR     r2, =0x00000000
        STR     r2, [r1]

        LDR     r1, =0x18010200
        LDR     r2, =0x00232300
        STR     r2, [r1]

        LDR     r1, =0x18010204
        LDR     r2, =0x23230001
        STR     r2, [r1]

        LDR     r1, =0x18010208
        LDR     r2, =0x00000001
        STR     r2, [r1]

        LDR     r1, =0x1801020c
        LDR     r2, =0x00000000
        STR     r2, [r1]

        LDR     r1, =0x18010210
        LDR     r2, =0x00000000
        STR     r2, [r1]

        LDR     r1, =0x18010214
        LDR     r2, =0x00012323
        STR     r2, [r1]

        LDR     r1, =0x18010218
        LDR     r2, =0x00012323
        STR     r2, [r1]

        LDR     r1, =0x1801021c
        LDR     r2, =0x00000000
        STR     r2, [r1]

        LDR     r1, =0x18010220
        LDR     r2, =0x00000000
        STR     r2, [r1]

        LDR     r1, =0x18010224
        LDR     r2, =0x00232300
        STR     r2, [r1]

        LDR     r1, =0x18010228
        LDR     r2, =0x23230001
        STR     r2, [r1]

        LDR     r1, =0x1801022c
        LDR     r2, =0x00000001
        STR     r2, [r1]

        LDR     r1, =0x18010230
        LDR     r2, =0x00000000
        STR     r2, [r1]

        LDR     r1, =0x18010234
        LDR     r2, =0x00000000
        STR     r2, [r1]

        LDR     r1, =0x18010238
        LDR     r2, =0x00012323
        STR     r2, [r1]

        LDR     r1, =0x1801023c
        LDR     r2, =0x00012323
        STR     r2, [r1]

        LDR     r1, =0x18010240
        LDR     r2, =0x00000000
        STR     r2, [r1]

        LDR     r1, =0x18010244
        LDR     r2, =0x00000000
        STR     r2, [r1]

        LDR     r1, =0x18010248
        LDR     r2, =0x00232300
        STR     r2, [r1]

        LDR     r1, =0x1801024c
        LDR     r2, =0x23230001
        STR     r2, [r1]

        LDR     r1, =0x18010250
        LDR     r2, =0x00000001
        STR     r2, [r1]

        LDR     r1, =0x18010254
        LDR     r2, =0x00000000
        STR     r2, [r1]

        LDR     r1, =0x18010258
        LDR     r2, =0x00000000
        STR     r2, [r1]

        LDR     r1, =0x1801025c
        LDR     r2, =0x00000000
        STR     r2, [r1]

        LDR     r1, =0x18010260
        LDR     r2, =0x03030303
        STR     r2, [r1]

        LDR     r1, =0x18010264
        LDR     r2, =0x03030303
        STR     r2, [r1]

        LDR     r1, =0x18010270
        LDR     r2, =0x02006400
        STR     r2, [r1]

        LDR     r1, =0x18010274
        LDR     r2, =0x02020202
        STR     r2, [r1]

        LDR     r1, =0x18010278
        LDR     r2, =0x00020202
        STR     r2, [r1]

        LDR     r1, =0x18010280
        LDR     r2, =0x01000000
        STR     r2, [r1]

        LDR     r1, =0x18010284
        LDR     r2, =0x01010064
        STR     r2, [r1]

        LDR     r1, =0x18010288
        LDR     r2, =0x01010101
        STR     r2, [r1]

        LDR     r1, =0x1801028c
        LDR     r2, =0x00000101
        STR     r2, [r1]

        LDR     r1, =0x18010294
        LDR     r2, =0x00020000
        STR     r2, [r1]

        LDR     r1, =0x18010298
        LDR     r2, =0x00000064
        STR     r2, [r1]

        LDR     r1, =0x1801029c
        LDR     r2, =0x00000000
        STR     r2, [r1]

        LDR     r1, =0x180102a0
        LDR     r2, =0x000c0c00
        STR     r2, [r1]

        LDR     r1, =0x180102a4
        LDR     r2, =0x14480000
        STR     r2, [r1]

        LDR     r1, =0x180102a8
        LDR     r2, =0x02000200
        STR     r2, [r1]

        LDR     r1, =0x180102ac
        LDR     r2, =0x02000200
        STR     r2, [r1]

        LDR     r1, =0x180102b0
        LDR     r2, =0x00001448
        STR     r2, [r1]

        LDR     r1, =0x180102b4
        LDR     r2, =0x0000cad0
        STR     r2, [r1]

        LDR     r1, =0x180102b8
        LDR     r2, =0x14480708
        STR     r2, [r1]

        LDR     r1, =0x180102bc
        LDR     r2, =0x02000200
        STR     r2, [r1]

        LDR     r1, =0x180102c0
        LDR     r2, =0x02000200
        STR     r2, [r1]

        LDR     r1, =0x180102c4
        LDR     r2, =0x00001448
        STR     r2, [r1]

        LDR     r1, =0x180102c8
        LDR     r2, =0x0000cad0
        STR     r2, [r1]

        LDR     r1, =0x180102cc
        LDR     r2, =0x02020709
        STR     r2, [r1]

        LDR     r1, =0x180102d0
        LDR     r2, =0x80000100
        STR     r2, [r1]

        LDR     r1, =0x180102d4
        LDR     r2, =0x04070303
        STR     r2, [r1]

        LDR     r1, =0x180102d8
        LDR     r2, =0x0000000a
        STR     r2, [r1]

        LDR     r1, =0x180102dc
        LDR     r2, =0x00000000
        STR     r2, [r1]

        LDR     r1, =0x180102e0
        LDR     r2, =0x00000000
        STR     r2, [r1]

        LDR     r1, =0x180102e4
        LDR     r2, =0x0010ffff
        STR     r2, [r1]

        LDR     r1, =0x180102e8
        LDR     r2, =0x00070303
        STR     r2, [r1]

        LDR     r1, =0x180102ec
        LDR     r2, =0x0000000f
        STR     r2, [r1]

        LDR     r1, =0x180102f0
        LDR     r2, =0x00000000
        STR     r2, [r1]

        LDR     r1, =0x180102f4
        LDR     r2, =0x00000000
        STR     r2, [r1]

        LDR     r1, =0x180102f8
        LDR     r2, =0x00000000
        STR     r2, [r1]

        LDR     r1, =0x180102fc
        LDR     r2, =0x00000000
        STR     r2, [r1]

        LDR     r1, =0x18010300
        LDR     r2, =0x00000000
        STR     r2, [r1]

        LDR     r1, =0x18010304
        LDR     r2, =0x00000000
        STR     r2, [r1]

        LDR     r1, =0x18010308
        LDR     r2, =0x00000204
        STR     r2, [r1]

        LDR     r1, =0x1801030c
        LDR     r2, =0x00000000
        STR     r2, [r1]

        LDR     r1, =0x18010314
        LDR     r2, =0x00000000
        STR     r2, [r1]

        LDR     r1, =0x18010318
        LDR     r2, =0x00000000
        STR     r2, [r1]

        LDR     r1, =0x1801031c
        LDR     r2, =0x00000000
        STR     r2, [r1]

        LDR     r1, =0x18010320
        LDR     r2, =0x00000000
        STR     r2, [r1]

        LDR     r1, =0x18010324
        LDR     r2, =0x00000000
        STR     r2, [r1]

        LDR     r1, =0x18010328
        LDR     r2, =0x00000007
        STR     r2, [r1]

        LDR     r1, =0x1801032c
        LDR     r2, =0x00000007
        STR     r2, [r1]

        LDR     r1, =0x18010330
        LDR     r2, =0x00000000
        STR     r2, [r1]

        LDR     r1, =0x18010338
        LDR     r2, =0x02060601
        STR     r2, [r1]

        LDR     r1, =0x1801033c
        LDR     r2, =0x00000002
        STR     r2, [r1]

        LDR     r1, =0x18010348
        LDR     r2, =0x0a090000
        STR     r2, [r1]

        LDR     r1, =0x1801034c
        LDR     r2, =0x00000a09
        STR     r2, [r1]

        LDR     r1, =0x18010350
        LDR     r2, =0x00000040
        STR     r2, [r1]

        LDR     r1, =0x18010354
        LDR     r2, =0x00000000
        STR     r2, [r1]

        LDR     r1, =0x18010358
        LDR     r2, =0x01010606
        STR     r2, [r1]

        LDR     r1, =0x1801035c
        LDR     r2, =0x00000101
        STR     r2, [r1]

        LDR     r1, =0x18010360
        LDR     r2, =0x00002323
        STR     r2, [r1]

        LDR     r1, =0x18010364
        LDR     r2, =0x00000000
        STR     r2, [r1]

        LDR     r1, =0x18010000
        LDR     r2, =0x00000601
        STR     r2, [r1]

poll_ddr_ctrl_begin:
        LDR     r1, =DDR_DENALI_CTL_89
poll_ddr_ctrl:
        LDR     r0, [r1]
        LDR     r2, =0x100
        AND     r0, r2, r0
        CMP     r0, r2
        BNE     poll_ddr_ctrl

ddr_init_done:
#if 0
	b ddr_init_done 
#endif	
#endif

coreo_init_done:		
		mov	pc, lr