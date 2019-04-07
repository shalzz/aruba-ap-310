Doimage tool
============

Doimage tool is used for preparation of binary image of boot loader for 
a various types of supported boot media.
The tool is supplied in source code adopted for Linux compilation.
Supported boot media types:
- SPI flash
- NAND flash
- SATA
- PEX
- TWSI
- UART

Doimage supports a special secure boot mode. In this mode image and its headers
are signed using private RSA key (up to 2048 bit).
An appropriate signatures and public RSA key needed for verification are 
embedded in the image header preventing unauthorized boot image replacement. 
The validity of supplied RSA public key is then verified against eFuse values 
stored in SOC. Additionally the binary image can be encrypted using AES-128-CBC 
algorithm and then decrypted by the HW using key stored in SOC eFuse.

Usage:
======
Doimage uses list of parameters supplied in command line. 
Some parameters are mandatory and some are optional.
Below is the command line format used by doimage utility:

doimage <mandatory parameters> [optional parameters] image_in image_out [header_out]

Mandatory parameters:
=====================

-T image_type   One of the following types: 
      sata\uart\flash\bootrom\nand\hex\pex
      The "flash" type should be used for SPI flash images. 
      If "sata" type is selected the command output will produce two 
      files - header and image.

-D image_dest   Hexadecimal RAM destination address used for copying the binary
      image during the boot stage.

-E image_exec   Hexadecimal address in system RAM used for image execution 
      (entry point). If selected image is "flash" and the image 
      destination address is 0xFFFFFFFF then the image will be 
      executed directly from flash media bypassing copy to RAM stage.
      In this case the execution address should be locates in SPI 
      flash memory space.

-S image_source   Hexadecimal byte offset of image from the boot media device 
      start point. For sata devices the meaning is LBA offset in 
      sectors from the beginning of disk.
      The parameter is mandatory only for sata and ignored for pex 
      and twsi boot devices. If omitted, it assumed equal to 0 and 
      the binary image is located next to the image headers. 
      The offset must be aligned to 512 bytes boundary for nand boot
      devices. Typical offset value for sata devices is 0x3F

-W hex_width   Mandatory only for "hex" image type. 
      Possible values are 8,16,32,64 bytes.

Mandatory parameters for NAND boot source:
=========================================

-L nand_blk_size 
      NAND block size in KBytes (decimal int in range 64-16320).
      This parameter is ignored for flashes with  512B pages. 
      Such small page flashes always use 16K block sizes

-N nand_cell_typ 
      NAND cell technology type (char: M for MLC, S for SLC)

-P nand_pg_size
      NAND page size: (decimal 512, 2048, 4096 or 8192)\n");

-C nand_ecc
      Option is IGNORED in this release (1=Hamming, 2=RS, 3=None)

Optional parameters:
===================

-M twsi_file   ASCII file name that contains the list of I2C init registers 
      to be set by the HW. 
      See the example file supplied with this utility

-G exec_file   ascii file name that contains binary routine (ARM 5TE THUMB)
      to run before the bootloader image execution. The routine must contain
      an appropriate code for saving all registers at the routine start and
      restore them before return from the routine.

-R regs_file   ASCII file name that contains the list of address-value 
      pairs used for configuring the device registers before 
      the binary image execution. Usually used for DRAM configuration.

-X pre_pad   Hexadecimal number of bytes to be placed before the binary image.

-X post_pad   Hexadecimal number of bytes to be placed after the binary image.

-H header_mode   The header mode. Supported modes:
      -H 1 : creates one file (image_out) for header and image
      -H 2 : creates two files - (image_out) for image and 
         (header_out) for header
      -H 3 : creates one file (image_out) for header only
      -H 4 : creates one file (image_out) for image only

Secure mode parameters:
======================

-Z prv_key   ASCII file name containing RSA private key up to 2048 bit length.
      The RSA key is used for creating signatures for the image header
      and binary image. Additionally it allows generation of public RSA
      key, which is embedded in the image header.
      The key file must be in TXT format - see the example key supplied 
      with this utility. 
      This parameter activates secure boot mode and enables all other 
      secure mode parameters usage.

-A aes_key   ASCII file name containing AES-128 symmetric key. 
      The boot image (execpting the header) will be encrypted using 
      AES-128-CBC algorithm. The key file must be in binary format 
      (array of 16 bytes).
      See the example key file supplied with this utility.

-J jtag_delay   Enable JTAG and delay the boot sequence execution 
      by <jtag_delay> * 10 seconds.
      Used for overriding the eFuse value for devices with 
      disabled JTAG functionality.

-B hex_box_id   Hexadecimal Box ID in range of 0 to 0xffffffff

F hex_flash_id   Hexadecimal Flash ID in range of 0 to 0xffff


Usage examples:
==============

1. Image for SPI flash. Secure mode is enabled. JTAG support is enabled. 
Delay boot execution by 10 sec for JTAG probe connection.
Run custom executable with 2 parameters and DRAM registers configuration
before running U-boot. Load U-boot into DRAM address 0x600000,  
entry point is 0x670000.

doimage -T flash -D 0x600000 -E 0x670000 -Z rsa_priv.txt -A aes128.bin \
   -J 1 -B 0xDEADBEEF -F 0xBABE -R dramregs_400db_A.txt -G binhdr.bin -K 2 \
   u-boot-db88f6281abp.bin u-boot-db88f6281abp_400db_flash_secure.bin

2. Image for NAND flash. Flash page size is 2K. Secure mode is enabled. 
JTAG support is enabled. Delay boot execution by 10 sec for JTAG probe connection.
Run custom executable with 2 parameters and DRAM registers configuration
before running U-boot. Load U-boot into DRAM address 0x600000,  
entry point is 0x670000.

doimage -T nand -D 0x600000 -E 0x670000 -P 2048 -Z rsa_priv.txt -A aes128.bin \
   -J 1 -B 0xDEADBEEF -F 0xBABE -R dramregs_400db_A.txt -G binhdr.bin -K 2 \
   u-boot-db88f6281abp.bin u-boot-db88f6281abp_400db_nand_secure.bin

3. Image for UART. Secure mode is disabled.

doimage -T uart -D 0x600000 -E 0x670000 -R dramregs_400db_A.txt \
   u-boot-db88f6281abp.bin u-boot-db88f6281abp_400db_uart.bin

4. Image for SATA disk. Secure mode is enabled. JTAG support is enabled. 
Delay boot execution by 10 sec for JTAG probe connection.
Run custom executable with 2 parameters and DRAM registers configuration
before running U-boot. Load U-boot into DRAM address 0x600000,  
entry point is 0x670000. U-boot image is located at LBA sector 0x3F.
Create two images - one for header and one for application.

doimage -T sata -D 0x600000 -E 0x670000 -S 0x3F -Z rsa_priv.txt -A aes128.bin \
   -J 1 -B 0xDEADBEEF -F 0xBABE -R dramregs_400db_A.txt -G binhdr.bin -K 2 \
   u-boot-db88f6281abp.bin u-boot-db88f6281abp_400db_sata_img_secure.bin \
   u-boot-db88f6281abp_400db_sata_hdr_secure.bin

In order to copy SATA boot image and header to disk the following technique is used:
1. Copy the header (first SATA device /dev/sda is uses as example)
   dd if=<header file name> of=/dev/sda seek=1
2. Copy the boot image (u-boot) to the first partition (starting by default at LBAsector 63)   
   dd if=<uboot with checksum> of=/dev/sda1
