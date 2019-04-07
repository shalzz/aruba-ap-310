
/*******************************************************************************
Copyright (C) Marvell International Ltd. and its affiliates

********************************************************************************
Marvell GPL License Option

If you received this File from Marvell, you may opt to use, redistribute and/or 
modify this File in accordance with the terms and conditions of the General 
Public License Version 2, June 1991 (the "GPL License"), a copy of which is 
available along with the File in the license.txt file or by writing to the Free 
Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 or 
on the worldwide web at http://www.gnu.org/licenses/gpl.txt. 

THE FILE IS DISTRIBUTED AS-IS, WITHOUT WARRANTY OF ANY KIND, AND THE IMPLIED 
WARRANTIES OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE ARE EXPRESSLY 
DISCLAIMED.  The GPL License provides additional details about this warranty 
disclaimer.

*******************************************************************************/

#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/mman.h>

#define _HOST_COMPILER
#include "bootstrap_def.h"

#ifndef O_BINARY                /* should be define'd on __WIN32__ */
#define O_BINARY        0
#endif

#define DOIMAGE_VERSION		"1.00"

void print_usage(void);

int f_in = -1;
int f_out = -1;
int f_header = -1;


typedef enum 
{
	IMG_SATA,
	IMG_UART,
	IMG_FLASH,
	IMG_BOOTROM,
	IMG_NAND,
	IMG_HEX,
	IMG_BIN,
	IMG_PEX,
	IMG_I2C
}IMG_TYPE;

typedef enum 
{
	HDR_IMG_ONE_FILE = 1,	/* Create one file with header and image */
	HDR_IMG_TWO_FILES = 2,	/* Create seperate header and image files */
	HDR_ONLY = 3,		/* Create only header */
	IMG_ONLY = 4,		/* Create only image */

}HEADER_MODE;


#define T_OPTION_MASK		0x1	/* image type */
#define D_OPTION_MASK		0x2	/* image destination */
#define E_OPTION_MASK		0x4	/* image execution address */
#define S_OPTION_MASK		0x8	/* image source */
#define R_OPTION_MASK		0x10    /* DRAM file */
#define C_OPTION_MASK		0x20	/* NAND ECC mode */
#define P_OPTION_MASK		0x40	/* NAND Page size */
#define M_OPTION_MASK		0x80	/* TWSI serial init file */
#define W_OPTION_MASK		0x100	/* HEX file width */
#define H_OPTION_MASK		0x200	/* Header mode*/
#define X_OPTION_MASK		0x400	/* Pre padding */
#define Y_OPTION_MASK		0x800	/* Post padding*/
#define I_OPTION_MASK		0x1000	/* SATA PIO mode*/
#define L_OPTION_MASK		0x2000	/* delay time in mseconds*/



#define SATA_MUST_OPT		(D_OPTION_MASK|T_OPTION_MASK|E_OPTION_MASK|S_OPTION_MASK)
#define UART_MUST_OPT		(D_OPTION_MASK|T_OPTION_MASK|E_OPTION_MASK)
#define FLASH_MUST_OPT		(D_OPTION_MASK|T_OPTION_MASK|E_OPTION_MASK)
#define PEX_MUST_OPT		(D_OPTION_MASK|T_OPTION_MASK|E_OPTION_MASK)
#define I2C_MUST_OPT		(D_OPTION_MASK|T_OPTION_MASK|E_OPTION_MASK|M_OPTION_MASK)
#define MAX_TWSI_HDR_SIZE	(60*1024) /* MAX eeprom is 64K & leave 4K for image and header */
#define BOOTROM_MUST_OPT 	(T_OPTION_MASK)

#define NAND_MUST_OPT		(D_OPTION_MASK|T_OPTION_MASK|E_OPTION_MASK|P_OPTION_MASK)
#define HEX_MUST_OPT 		(T_OPTION_MASK|W_OPTION_MASK)
#define BIN_MUST_OPT 		(T_OPTION_MASK|W_OPTION_MASK)

/* 32 bit checksum */
u32 checksum32(u32 start, u32 len, u32 csum);
u8 checksum8(u32 start, u32 len,u8 csum);
u32 crc32(u32 crc, volatile u32 *buf, u32 len);

int main(int argc,char** argv)
{
	char *image_type = NULL;
	IMG_TYPE img;
	unsigned int image_dest=0,image_exec=0,header_size=0, image_source = 0, twsi_size=0;
	char *fname_in,*fname_out=NULL,*fname_hdr_out=NULL,*fname_romc=NULL,
		  *fname_dram=NULL,*fname_twsi=NULL;
    	char *inst_set;
        unsigned char *ptr;
	BHR_t hdr;
	ExtBHR_t extHdr;
    	struct stat fs_stat,fs_stat_code;
	char *buf_in = NULL ,*buf_code = NULL;
	int override[2];
	int err,size_written=0; 
	char *tmpHeader = NULL;
	char *tmpTwsi = NULL;
	unsigned int opts=0,required_opts = 0;
	unsigned int chsum32 = 0;
	unsigned int nandPageSize=0, nandEccMode=0, hex_width=0,header_mode = HDR_IMG_ONE_FILE,ms_delay=0;
	FILE*   f_dram, *f_hex,*f_hex2, *f_hex3, *f_twsi;
	int   f_code;
	unsigned char  *hex8=NULL, tmp8;
	unsigned short  *hex16=NULL, tmp16;
	unsigned long   *hex32=NULL, tmp32;
	unsigned long lastDword = 0;
	int i=0;
	unsigned int bytesToAlign=0;
	char** f_out_names[2];
	int pre_padding=0, post_padding=0, padding_size=0;
	#define IMG_FILE_INDX	0
	#define HDR_FILE_INDX	1

	f_out_names[IMG_FILE_INDX] = &fname_out;
	f_out_names[HDR_FILE_INDX] = &fname_hdr_out;

	memset((void*)&hdr,0,sizeof(BHR_t));
	memset((void*)&extHdr,0,sizeof(ExtBHR_t));

	if (argc ==1)
	{
		print_usage();
		exit(1);
	}

	while (--argc > 0 && **++argv == '-') 
	{
		while (*++*argv) 
		{
			switch (**argv) 
			{
			case 'T': /* image type */
				if (--argc <= 0) {print_usage();exit(1);}
                                image_type = *++argv;
				if (opts & T_OPTION_MASK) {print_usage();exit(1);}
				opts |= T_OPTION_MASK;
                                break;
			case 'D': /* image destination  */
				if (--argc <= 0) {print_usage();exit(1);}
                                image_dest = strtoul (*++argv, (char **)&ptr, 16);
				if (*ptr) {print_usage();exit(1);}
				if (opts & D_OPTION_MASK) {print_usage();exit(1);}
				opts |= D_OPTION_MASK;
				break;
			case 'E': /* image execution  */
				if (--argc <= 0) {print_usage();exit(1);}
                                image_exec = strtoul (*++argv, (char **)&ptr, 16);
				if (*ptr) {print_usage();exit(1);}
				if (opts & E_OPTION_MASK) {print_usage();exit(1);}
				opts |= E_OPTION_MASK;
				break;
            		case 'X': /* Pre - Padding */
				if (--argc <= 0) {print_usage();exit(1);}
				padding_size = strtoul (*++argv, (char **)&ptr, 16);
				pre_padding=1;
				if (*ptr) {print_usage();exit(1);}
				if (opts & X_OPTION_MASK) {print_usage();exit(1);}
				opts |= X_OPTION_MASK;
				break;
            		case 'Y': /* Post - Padding */
				if (--argc <= 0) {print_usage();exit(1);}
				padding_size = strtoul (*++argv, (char **)&ptr, 16);
				post_padding=1;
				if (*ptr) {print_usage();exit(1);}
				if (opts & Y_OPTION_MASK) {print_usage();exit(1);}
				opts |= Y_OPTION_MASK;
				break;
			case 'I': /* PIO */
				if (argc <= 0) {print_usage();exit(1);}
				if (opts & I_OPTION_MASK) {print_usage();exit(1);}
				opts |= I_OPTION_MASK;
				break;
            		case 'S': /* starting sector */
				if (--argc <= 0) {print_usage();exit(1);}
				image_source = strtoul (*++argv, (char **)&ptr, 16);
				if (*ptr) {print_usage();exit(1);}
				if (opts & S_OPTION_MASK) {print_usage();exit(1);}
				opts |= S_OPTION_MASK;
				break;
            		case 'P': /* NAND Page Size */
				if (--argc <= 0) {print_usage();exit(1);}
				nandPageSize = strtoul (*++argv, (char **)&ptr, 10);
				if (*ptr) {print_usage();exit(1);}
				if (opts & P_OPTION_MASK) {print_usage();exit(1);}
				opts |= P_OPTION_MASK;
				break;
			case 'C': /* NAND ECC mode */
				if (--argc <= 0) {print_usage();exit(1);}
				nandEccMode = strtoul (*++argv, (char **)&ptr, 10);
				if (*ptr) {print_usage();exit(1);}
				if (opts & C_OPTION_MASK) {print_usage();exit(1);}
				opts |= C_OPTION_MASK;
				break;
			case 'L': /* Delay time */
				if (--argc <= 0) {print_usage();exit(1);}
				ms_delay = strtoul (*++argv, (char **)&ptr, 10);
				if (*ptr) {print_usage();exit(1);}
				if (opts & L_OPTION_MASK) {print_usage();exit(1);}
				opts |= L_OPTION_MASK;
				break;
            		case 'W': /* HEX file width */
				if (--argc <= 0) {print_usage();exit(1);}
				hex_width = strtoul (*++argv, (char **)&ptr, 10);
				if (*ptr) {print_usage();exit(1);}
				if (opts & W_OPTION_MASK) {print_usage();exit(1);}
				opts |= W_OPTION_MASK;
				break;
            		case 'H': /* Header file mode */
				if (--argc <= 0) {print_usage();exit(1);}
				header_mode = strtoul (*++argv, (char **)&ptr, 10);
				if (*ptr) {print_usage();exit(1);}
				if (opts & H_OPTION_MASK) {print_usage();exit(1);}
				opts |= H_OPTION_MASK;
				break;
            		case 'R': /* dram file*/
				if (--argc <= 0) {print_usage();exit(1);}
				fname_dram = *++argv;
				if (opts & R_OPTION_MASK) {print_usage();exit(1);}
				opts |= R_OPTION_MASK;
				break;
			case 'M': /* TWSI serial init file*/
				if (--argc <= 0) {print_usage();exit(1);}
				fname_twsi = *++argv;
				if (opts & M_OPTION_MASK) {print_usage();exit(1);}
				opts |= M_OPTION_MASK;
				break;
			}
		}
	}

	/* 2 sperate images is used with SATA only */
	if (header_mode == HDR_IMG_TWO_FILES)
	{
        	if (!(opts & S_OPTION_MASK)) 
		{
			fprintf(stderr,"Error: -S option is missing\n\n\n\n\n");
			print_usage();exit(1);
		} 		
	}

	/* verify HEX file width selection to be valid */
	if (opts & W_OPTION_MASK)
	{
		if ((hex_width != 8)&&(hex_width != 16)&&(hex_width != 32)&&(hex_width != 64))
		{print_usage();exit(1);}
	}

	/* get the minimum option set based on boot mode */
	if (opts & T_OPTION_MASK)
	{
		if (strcmp(image_type, "sata") == 0) 
		{
			img=IMG_SATA;
			required_opts = SATA_MUST_OPT;	
		}
		else if (strcmp(image_type, "nand") == 0) 
		{
			img=IMG_NAND;
			required_opts = NAND_MUST_OPT;
		}
		else if (strcmp(image_type, "hex") == 0) 
		{
			img=IMG_HEX;
			required_opts = HEX_MUST_OPT;
		}
		else if (strcmp(image_type, "bin") == 0) 
		{
			img=IMG_BIN;
			required_opts = BIN_MUST_OPT;
		}
		else if (strcmp(image_type, "uart") == 0)
		{
			img=IMG_UART;
			required_opts = UART_MUST_OPT;
		}
		else if (strcmp(image_type, "flash") == 0)
		{
			img=IMG_FLASH;
			required_opts = FLASH_MUST_OPT;
		}
		else if (strcmp(image_type, "bootrom") == 0)
		{
			img=IMG_BOOTROM;
			required_opts = BOOTROM_MUST_OPT;
		}
		else if (strcmp(image_type, "pex") == 0)
		{
			img=IMG_PEX;
			required_opts = PEX_MUST_OPT;
		}
		else if (strcmp(image_type, "i2c") == 0)
		{
			img=IMG_I2C;
			required_opts = I2C_MUST_OPT;

		}
		else {print_usage();exit(1);}

	}
	else  
	{
		print_usage();
		exit(1);
	}

	if (header_mode == IMG_ONLY)
	{
		required_opts &= ~(D_OPTION_MASK|E_OPTION_MASK|S_OPTION_MASK|R_OPTION_MASK|P_OPTION_MASK);
	}

	if (required_opts != (opts & required_opts)) {print_usage();exit(1);}

	hdr.ddrInitDelay = ms_delay;
	
	hdr.destinationAddr = image_dest;
	hdr.executionAddr = image_exec;

	switch (img) 
	{
	case IMG_BOOTROM:
		header_mode = IMG_ONLY;
		break;
	case IMG_HEX:
		header_mode = IMG_ONLY;
		break;
	case IMG_BIN:
		header_mode = IMG_ONLY;
		break;
	case IMG_SATA:
		hdr.blockID = IBR_HDR_SATA_ID;
		header_size = 512;
		if (image_source) hdr.sourceAddr = image_source;
			else hdr.sourceAddr = 2; /* default */

		if (!(opts & H_OPTION_MASK))
		{
			header_mode = HDR_IMG_TWO_FILES/*HDR_ONLY*/;
		}
		if (opts & I_OPTION_MASK)
		{
			hdr.sataPioMode = 1;
		}	    
		break;
	case IMG_UART:
		hdr.blockID = IBR_HDR_UART_ID;
		if (opts & R_OPTION_MASK)
		{
			header_size = 512;
		} else header_size = 128;
		hdr.sourceAddr = header_size;
		break;
	case IMG_FLASH:
		hdr.blockID = IBR_HDR_SPI_ID;
		if (opts & R_OPTION_MASK)
		{
			header_size = 512;
		} else header_size = sizeof(BHR_t);
		if ((image_source)&&(image_source >= header_size))
		{
			 hdr.sourceAddr = image_source;
		}else
		{
			hdr.sourceAddr = header_size; /* default */
		}

		break;
	case IMG_NAND:
		hdr.blockID = IBR_HDR_NAND_ID;
		if (opts & R_OPTION_MASK)
		{
			header_size = 512;
		} else header_size = sizeof(BHR_t);
		if ((image_source)&&(image_source >= header_size))
		{
			 hdr.sourceAddr = image_source;
		}else
		{
			hdr.sourceAddr = header_size; /* default */
		}

		hdr.nandPageSize = (MV_U16)nandPageSize;		
		hdr.nandEccMode = (MV_U8)nandEccMode;
		break;
	case IMG_PEX:
		hdr.blockID = IBR_HDR_PEX_ID;
		if (opts & R_OPTION_MASK)
		{
			header_size = 512;
		} else header_size = sizeof(BHR_t);
		if ((image_source)&&(image_source >= header_size))
		{
			 hdr.sourceAddr = image_source;
		}else
		{
			hdr.sourceAddr = header_size; /* default */
		}
	case IMG_I2C:
		hdr.blockID = IBR_HDR_I2C_ID;
		if (opts & R_OPTION_MASK)
		{
			header_size = 512;
		} else header_size = sizeof(BHR_t);
		if ((image_source)&&(image_source >= header_size))
		{
			 hdr.sourceAddr = image_source;
		}else
		{
			hdr.sourceAddr = header_size; /* default */
		}
	}

	if ((header_mode == HDR_IMG_TWO_FILES) || ((header_mode == IMG_ONLY) && (img == IMG_BIN)))
	{
		if (argc == 4) /* In case ROMC is needed */
		{
			fname_in = *argv++;
			fname_out = *argv++;
			fname_hdr_out = *argv++;
			fname_romc = *argv++;

			if ((0 == strcmp(fname_in, fname_out)) ||
			   (0 == strcmp(fname_in, fname_hdr_out)) ||
			   (0 == strcmp(fname_in, fname_romc)) ||
			   (0 == strcmp(fname_out, fname_hdr_out)) ||
			   (0 == strcmp(fname_out, fname_romc)) ||
			   (0 == strcmp(fname_hdr_out, fname_romc)))
			{
				fprintf(stderr,"Error: Input and output images can't be the same\n");
				exit(1);
			}
		}
		else if (argc == 3)
		{
			fname_in = *argv++;
			fname_out = *argv++;
			fname_hdr_out = *argv++;

			if ((0 == strcmp(fname_in, fname_out))||
			   (0 == strcmp(fname_in, fname_hdr_out))||
			   (0 == strcmp(fname_out, fname_hdr_out)))
			{
				fprintf(stderr,"Error: Input and output images can't be the same\n");
				exit(1);
			}
		}
		else
		{			
			print_usage();
			exit(1);
		}
	}
	else
	{
		if (argc == 2)
		{
			fname_in = *argv++;
			fname_out = *argv++;

			if (0 == strcmp(fname_in, fname_out))
			{
				fprintf(stderr,"Error: Input and output images can't be the same\n");
				exit(1);
			}
		}
		else
		{
			print_usage();
			exit(1);
		}
	}

	/* check if the output image exist */ 
	i = 0;
	do
	{
		if (*f_out_names[i])
		{
			f_out = open(*f_out_names[i],O_RDONLY|O_BINARY);			
			if (f_out != -1)
			{
				char c;

				close(f_out);
				f_out = -1;
				fprintf(stderr,"File '%s' already exist! override (y/n)?",*f_out_names[i]);
				c = getc(stdin);
				if ((c == 'N')||(c == 'n'))
				{
					printf("exit.. nothing done. \n");
					exit(0);
				}
				/* for the Enter */
				c = getc(stdin);
				override[i] = 1;				
			}
			else
			{
				override[i] = 0;
            		}
		}

		i++;
		if (i == 2) break;
	}while(1);
	
	if (header_mode != HDR_ONLY)
	{
		/* open input image */
		f_in = open(fname_in,O_RDONLY|O_BINARY);
		if (f_in == -1)
		{
			fprintf(stderr,"File '%s' not found \n",fname_in);
			exit(0);
		}

		/* get the size of the input image */
		err = fstat(f_in, &fs_stat);

		if (0 != err)
		{
			close(f_in);
			fprintf(stderr,"fstat failed for file: '%s' err=%d\n",fname_in,err);
			exit(1);
		}

		if ((fs_stat.st_size > BOOTROM_SIZE)&&(img == IMG_BOOTROM))
		{
			printf("ERROR : bootstrap.bin size is bigger than %d bytes \n",BOOTROM_SIZE);
			close(f_in);
			exit(1);
		}
		/* map the input image */
		buf_in = mmap(0, fs_stat.st_size, PROT_READ, MAP_SHARED, f_in, 0);
		if (!buf_in)
		{
			fprintf(stderr,"Error mapping %s file \n",fname_in);
			goto end;
		}
	}
	
	/* open the output image */
	if (override[IMG_FILE_INDX] == 0)
	{
        	f_out = open(fname_out,O_RDWR|O_TRUNC|O_CREAT|O_BINARY,0666);
	}
	else f_out = open(fname_out,O_RDWR|O_BINARY);

	if (f_out == -1)
	{
		fprintf(stderr,"Error openning %s file \n",fname_out);
    	}

	if (header_mode == HDR_IMG_TWO_FILES)
	{
		/* open the output header file */
		if (override[HDR_FILE_INDX] == 0)
		{
			f_header = open(fname_hdr_out,O_RDWR|O_TRUNC|O_CREAT|O_BINARY,0666);
		}
		else f_header = open(fname_hdr_out,O_RDWR|O_BINARY);

		if (f_header == -1)
		{
			fprintf(stderr,"Error openning %s file \n",fname_hdr_out);
		}
	}

	/* Image Header  */
	if (header_mode != IMG_ONLY)
	{
		hdr.blockSize = fs_stat.st_size;

		if (opts & R_OPTION_MASK)
		{
			hdr.ext = 1;
		}

		/* for FLASH\NAND , we have extra word for checksum */
		if ((img == IMG_FLASH)||(img == IMG_NAND)||(img == IMG_SATA)||(img == IMG_PEX)||(img == IMG_I2C))
		{
			/*hdr.blockSize++;*/
			hdr.blockSize +=4;
		}

		/* in sata headers, blocksize is in sectors (512 byte)*/
		if (img == IMG_SATA)
		{
			/*hdr.blockSize = (hdr.blockSize + 511) >> 9;*/
		}

		/* Update Block size address */
		if (padding_size)
		{
			/* Align padding to 32 bit */
			if (padding_size & 0x3)
			{
				padding_size += (4 - (padding_size & 0x3));
			}
			hdr.blockSize += padding_size;
		}

		/* Align size to 4 byte*/
		if (hdr.blockSize & 0x3)
		{
			printf("hdr.blockSize = 0x%x fs_stat.st_size = 0x%x\n", hdr.blockSize, fs_stat.st_size);
			bytesToAlign = (4 - (hdr.blockSize & 0x3));
			hdr.blockSize += bytesToAlign;
		}

		tmpTwsi = malloc(MAX_TWSI_HDR_SIZE);
		memset(tmpTwsi, 0xFF, MAX_TWSI_HDR_SIZE);

		if (opts & M_OPTION_MASK)
		{
			if (fname_twsi) 
			{
				int i;
				unsigned int * twsi_reg = (unsigned int *)tmpTwsi;;

                		f_twsi = fopen(fname_twsi, "r");
				if (f_twsi == NULL)
				{
					fprintf(stderr,"File '%s' not found \n",fname_twsi);
					exit(1);
				}

				for (i=0; i<(MAX_TWSI_HDR_SIZE/4); i++)
				{
					if (EOF == fscanf(f_twsi,"%x\n",twsi_reg))
						break;

					/* Swap Enianess */
					*twsi_reg = ( ((*twsi_reg >> 24) & 0xFF) |
						      ((*twsi_reg >> 8)  & 0xFF00) |
						      ((*twsi_reg << 8)  & 0xFF0000) |
						      ((*twsi_reg << 24) & 0xFF000000) );
					twsi_reg++;
				}

				fclose(f_twsi);

				twsi_size = ((((i+2)*4) & ~0x1FF) + 0x200); /*size=512,1024,.. with at least 8 0xFF bytes */
				
				if ((write(f_out, tmpTwsi, twsi_size)) != twsi_size)
				{
					fprintf(stderr,"Error writing %s file \n",fname_out);
					goto end;
				}
			}
		}

		tmpHeader = malloc(header_size);
		memset(tmpHeader, 0 ,header_size);

		hdr.checkSum = checksum8((u32)&hdr, sizeof(BHR_t) ,0);
		memcpy(tmpHeader, &hdr, sizeof(BHR_t));

        	/* Header extension */
		if (opts & R_OPTION_MASK)
		{
			int dram_buf_size=0,code_buf_size=0;
			
			/* First we will take of DRAM */
			if (fname_dram) 
			{
				int i;
				/*unsigned int dram_reg[DRAM_REGS_NUM];*/
				unsigned int dram_reg[(512>>2)];

                		f_dram = fopen(fname_dram, "r");
				
				if (f_dram == NULL)
				{
					fprintf(stderr,"File '%s' not found \n",fname_dram);
					exit(1);
				}

				/*for (i=0; i< DRAM_REGS_NUM ; i++)*/
				i=0;
                		while (EOF != fscanf(f_dram,"%x\n",&dram_reg[i++]));

				fclose(f_dram);

				/*dram_buf_size = DRAM_REGS_NUM * 4;*/
				dram_buf_size = (i-1)*4;
				memcpy(tmpHeader + sizeof(BHR_t) + sizeof(ExtBHR_t),
					   dram_reg, dram_buf_size);

				extHdr.dramRegsOffs = sizeof(BHR_t) + sizeof(ExtBHR_t);
			}			

			memcpy(tmpHeader + sizeof(BHR_t), &extHdr, sizeof(ExtBHR_t));
			*(MV_U8*)(tmpHeader + header_size  - 1) = checksum8((u32)(tmpHeader + sizeof(BHR_t)), header_size - sizeof(BHR_t),0);
		}

		if (header_mode == HDR_IMG_TWO_FILES)
		{
			/* copy header to output image */
			size_written = write(f_header, tmpHeader, header_size);
			if (size_written != header_size)
			{
				fprintf(stderr,"Error writing %s file \n",fname_hdr_out);
				goto end;
			}

			fprintf(stdout, "%s was created \n", *f_out_names[HDR_FILE_INDX]);
		}
		else
		{
			/* copy header to output image */
			size_written = write(f_out, tmpHeader, header_size);
			if (size_written != header_size)
			{
				fprintf(stderr,"Error writing %s file \n",fname_out);
				goto end;
			}
		}
	}


	if (header_mode != HDR_ONLY)
	{
		char *padding = NULL;
		int new_file_size = 0;

		if (img == IMG_BOOTROM)
		{
			char *tmp1;
			int tmpSize = BOOTROM_SIZE - sizeof(chsum32);

			/* PAD image with Zeros until BOOTROM_SIZE*/
			tmp1 = malloc(tmpSize);
			
			if (tmp1 == NULL) 
				goto end;

            		memcpy(tmp1, buf_in, fs_stat.st_size);
            		memset(tmp1 + fs_stat.st_size, 0, tmpSize - fs_stat.st_size);
            		fs_stat.st_size = tmpSize;

			/* copy input image to output image */
			size_written = write(f_out, tmp1, fs_stat.st_size);
			
			/* calculate checsum */
			chsum32 = crc32(0, (u32*)tmp1, (fs_stat.st_size/4));
			printf("Image Chacksum (size = %d) = 0x%08x\n", fs_stat.st_size, chsum32);
			fs_stat.st_size += sizeof(chsum32) ;
			
			size_written += write(f_out, &chsum32, sizeof(chsum32));

			if (tmp1) 
				free(tmp1);

			new_file_size = fs_stat.st_size;
		}
		else if (img == IMG_HEX)
		{
			char *tmp1 = NULL;
            
			f_hex = fopen(fname_out, "w");
			if (f_hex == NULL) goto end;

			int hex_len = fs_stat.st_size;
			int hex_unaligned_len = 0;

			switch (hex_width)
			{
			case 8:
                		hex8 = (unsigned char*)buf_in;
				do
				{
					fprintf(f_hex,"%02X\n",*hex8);
					hex8++;
					size_written += 1;
					hex_len--;

				}while(hex_len);
				break;
			case 16:
                		hex16 = (unsigned short*)buf_in;
				hex_unaligned_len = (fs_stat.st_size & 0x1);

				if (hex_unaligned_len) 
				{
					hex_len -= hex_unaligned_len;
					hex_len += 2;
					tmp1 = malloc(hex_len);
					hex16 = (unsigned short*)tmp1;
					memset(tmp1, 0, (hex_len));
					memcpy(tmp1, buf_in, fs_stat.st_size);
				}
				do
				{
					fprintf(f_hex,"%04X\n",*hex16++);
					size_written += 2;
					hex_len -= 2;

				}while(hex_len);
				break;
			case 32:
				hex32 = (long*)buf_in;
				hex_unaligned_len = (fs_stat.st_size & 0x3);
				if (hex_unaligned_len) 
				{
					hex_len -= hex_unaligned_len;
					hex_len += 4;
					tmp1 = malloc(hex_len);
					hex16 = (unsigned short*)tmp1;
					memset(tmp1, 0, (hex_len));
					memcpy(tmp1, buf_in, fs_stat.st_size);
				}
				do
				{
					fprintf(f_hex,"%08X\n",*hex32++);
					size_written += 4;
					hex_len -= 4;

				}while(hex_len);
				break;

			case 64:
				hex32 = (long*)buf_in;
				hex_unaligned_len = (fs_stat.st_size & 0x7);
				if (hex_unaligned_len) 
				{
					hex_len -= hex_unaligned_len;
					hex_len += 8;
					tmp1 = malloc(hex_len);
					hex16 = (unsigned short*)tmp1;
					memset(tmp1, 0, (hex_len));
					memcpy(tmp1, buf_in, fs_stat.st_size);
				}
				do
				{
					fprintf(f_hex,"%08X%08X\n",*hex32++, *hex32++);
					size_written += 8;
					hex_len -= 8;

				}while(hex_len);
				break;
			}
			size_written = fs_stat.st_size;
			if (tmp1) free(tmp1);
			fclose(f_hex);

			new_file_size = fs_stat.st_size;
		}
		else if (img == IMG_BIN)
		{
			char *tmp1 = NULL;
			int one_file_len;
			int hex_len = fs_stat.st_size;
            
			f_hex = fopen(fname_out, "w");
			if (f_hex == NULL) goto end;
			f_hex2 = fopen(fname_hdr_out, "w");
			if (f_hex2 == NULL) goto end;
			if (fname_romc)
			{
				f_hex3 = fopen(fname_romc, "w");
				if (f_hex3 == NULL) goto end;
				one_file_len = (hex_len / 3);
			}
			else
			{
				one_file_len = hex_len * 0.5;
			}
			int hex_unaligned_len = 0;

			switch (hex_width)
			{
			case 8:
                		hex8 = (unsigned char*)buf_in;
				do
				{
					tmp8 = *hex8;
					if (hex_len > one_file_len)
					{
						for (i=0; i<hex_width ; i++)
						{
							fprintf(f_hex,"%d",((tmp8 & 0x80) >> 7));
							tmp8 <<= 1; 
						}
						fprintf(f_hex,"\n");
					}
					else
					{
						for (i=0; i<hex_width ; i++)
						{
							fprintf(f_hex2,"%d",((tmp8 & 0x80) >> 7));
							tmp8 <<= 1; 
						}
						fprintf(f_hex2,"\n");
					}
					hex8++;
					size_written += 1;
					hex_len--;
				}while(hex_len);
				break;
			case 16:
                		hex16 = (unsigned short*)buf_in;
				hex_unaligned_len = (fs_stat.st_size & 0x1);

				if (hex_unaligned_len) 
				{
					hex_len -= hex_unaligned_len;
					hex_len += 2;
					tmp1 = malloc(hex_len);
					hex16 = (unsigned short*)tmp1;
					memset(tmp1, 0, (hex_len));
					memcpy(tmp1, buf_in, fs_stat.st_size);
				}
				do
				{
					tmp16 = *hex16;
					for (i=0; i<hex_width ; i++)
					{
                        			fprintf(f_hex,"%d",((tmp16 & 0x8000) >> 15));
						tmp16 <<= 1; 
					}
					fprintf(f_hex,"\n");

					hex16++;

					size_written += 2;
					hex_len -= 2;

				}while(hex_len);
				break;
			case 32:
				hex32 = (long*)buf_in;
				hex_unaligned_len = (fs_stat.st_size & 0x3);
				if (hex_unaligned_len) 
				{
					hex_len -= hex_unaligned_len;
					hex_len += 4;
					tmp1 = malloc(hex_len);
					hex16 = (unsigned short*)tmp1;
					memset(tmp1, 0, (hex_len));
					memcpy(tmp1, buf_in, fs_stat.st_size);
				}
				do
				{
					tmp32 = *hex32;

					if (fname_romc)
					{
						if (hex_len > (2 * one_file_len))
						{
							for (i=0; i<hex_width ; i++)
							{
								fprintf(f_hex,"%d",((tmp32 & 0x80000000) >> 31));
								tmp32 <<= 1; 
							}
							fprintf(f_hex,"\n");

						}
						else if (hex_len > one_file_len)
						{
							for (i=0; i<hex_width ; i++)
							{
								fprintf(f_hex2,"%d",((tmp32 & 0x80000000) >> 31));
								tmp32 <<= 1; 
							}
							fprintf(f_hex2,"\n");
						}
						else
						{
							for (i=0; i<hex_width ; i++)
							{
								fprintf(f_hex3,"%d",((tmp32 & 0x80000000) >> 31));
								tmp32 <<= 1; 
							}
							fprintf(f_hex3,"\n");
						}
					}
					else
					{
						if (hex_len > one_file_len)
						{
							for (i=0; i<hex_width ; i++)
							{
								fprintf(f_hex,"%d",((tmp32 & 0x80000000) >> 31));
								tmp32 <<= 1; 
							}
							fprintf(f_hex,"\n");
						}
						else
						{
							for (i=0; i<hex_width ; i++)
							{
								fprintf(f_hex2,"%d",((tmp32 & 0x80000000) >> 31));
								tmp32 <<= 1; 
							}
							fprintf(f_hex2,"\n");
						}
					}
					hex32++;
					size_written += 4;
					hex_len -= 4;

				}while(hex_len);
				break;
			case 64:
				fprintf(stderr,"Error: 64 Bit is not supported for binary files\n\n\n\n\n");	
				break;
			}
			size_written = fs_stat.st_size;
			if (tmp1) free(tmp1);
			fclose(f_hex);
			fclose(f_hex2);

			new_file_size = fs_stat.st_size;
		}
		else
		{

			size_written = 0;

			if ((pre_padding)&&(padding_size))
			{                
                		padding = malloc(padding_size);
				if (padding)
				{
					new_file_size += padding_size;
					memset((void*)padding, 0x5, padding_size);
					size_written += write(f_out, padding, padding_size);

					chsum32 = checksum32((u32)padding, padding_size, chsum32);
				}
				
			}
			
			new_file_size += fs_stat.st_size;

			/* Calculate checksum */
			chsum32 = checksum32((u32)buf_in, (u32)((u32)fs_stat.st_size - bytesToAlign), chsum32);
			if (bytesToAlign)
			{
				memcpy(&lastDword, (buf_in + (fs_stat.st_size - bytesToAlign)) , bytesToAlign);
			}
			chsum32 = checksum32((u32)&lastDword, 4,chsum32);


			/* copy input image to output image */
			size_written += write(f_out, buf_in, fs_stat.st_size);
			if (bytesToAlign)
			{
				size_written += write(f_out, &lastDword, bytesToAlign);
			}


			if ((post_padding)&&(padding_size))
			{
                		padding = malloc(padding_size);
				if (padding)
				{
					new_file_size += padding_size;
					memset((void*)padding, 0xa, padding_size);
					size_written += write(f_out, padding, padding_size);
					chsum32 = checksum32((u32)padding, padding_size, chsum32);
				}				
			}

			/* write checksum */
			size_written += write(f_out, &chsum32, sizeof(chsum32));
			new_file_size +=4 ;
		}

	
		if (size_written != new_file_size)
		{
			fprintf(stderr,"Error writing %s file \n",fname_out);
			goto end;
		}

		fprintf(stdout, "%s was created \n", *f_out_names[IMG_FILE_INDX]);
	}

	
end:
	if (tmpHeader) 
		free(tmpHeader);

	/* close handles */
	if (f_out != -1) 
		close(f_out);
	if (f_header != -1)
		close(f_header);
    	if (buf_in) 
		munmap((void*)buf_in, fs_stat.st_size);
	if (f_in != -1) 
		close(f_in);
	return 0;
}


void print_usage(void)
{
	printf("\n");
	printf("Marvell doimage Tool version %s\n", DOIMAGE_VERSION);
	printf("Supported SoC devices: \n");
	printf("    Marvell 88F6082 - A1\n");
	printf("    Marvell 88F6180 - A0\n");
	printf("    Marvell 88F6192 - A0\n");
	printf("    Marvell 88F6190 - A0\n");
	printf("    Marvell 88F6281 - A0\n");
	printf("\n");
	printf("usage: \n");
	printf("doimage <must_options> [other_options] image_in image_out [header_out]\n");
	printf("\n<must_options> - can be one or more of the following:\n\n");
	printf("-T image_type:   sata\\uart\\flash\\bootrom\\nand\\hex\\pex\n");
	printf("                 if image_type is sata, the image_out will\n");
	printf("                 include header only.\n");
	printf("-D image_dest:   image destination in dram (in hex)\n");
	printf("-E image_exec:   execution address in dram (in hex)\n");
	printf("                 if image_type is 'flash' and image_dest is 0xffffffff\n");
	printf("                 then execution address on the flash\n");
	printf("-S image_source: if image_type is sata then the starting sector of\n");
	printf("                 the source image on the disk - mandatory for sata\n");
	printf("                 if image_type is flash\\nand then the starting offset of\n");
	printf("                 the source image at the flash - optional for flash\\nand\n");
	printf("-W hex_width :   HEX file width, can be 8,16,32,64 \n");
	printf("-M twsi_file:    ascii file name that contains the I2C init regs set by h/w.\n");
	printf("                 this is used in i2c boot only\n");
	printf("\n<other_options> - optional and can be one or more of the following:\n\n");
	printf("-R dram_file: ascii file name that contains the list of dram regs\n");
	printf("-P nand_page_size (decimal 512, 2048, ..): NAND Page size\n");
	printf("-C nand_ecc_mode (1=Hamming, 2=RS, 3=None)\n");
	printf("-L delay in mili seconds before DRAM init\n");
	printf("-I copy image in PIO mode (valid for SATA only)\n");
	printf("-X pre_padding_size (hex)\n");
	printf("-Y post_padding_size (hex)\n");
	printf("-H header_mode: Header mode, can be:\n");
	printf("   -H 1 :will create one file (image_out) for header and image\n");
	printf("   -H 2 :will create two files, (image_out) for image , (header_out) for header\n");
	printf("   -H 3 :will create one file (image_out) for header only \n");
	printf("   -H 4 :will create one file (image_out) for image only \n");
	printf("\ncommand possibilities: \n\n");
	printf("doimage -T hex -W width image_in image_out\n");
	printf("doimage -T bootrom image_in image_out\n");
	printf("doimage -T sata -S sector -D image_dest -E image_exec\n");
	printf("         [other_options] image_in image_out header_out\n\n");
	printf("doimage -T flash -D image_dest -E image_exec [-S address]\n");
	printf("         [other_options] image_in image_out\n\n");
	printf("doimage -T pex -D image_dest -E image_exec \n");
	printf("         [other_options] image_in image_out\n\n");
	printf("doimage -T nand -D image_dest -E image_exec [-S address] -P page_size\n");
	printf("         [other_options] image_in image_out\n\n");
	printf("doimage -T uart -D image_dest -E image_exec\n");
	printf("         [other_options] image_in image_out\n\n");
	printf("doimage -T pex -D image_dest -E image_exec \n");
	printf("         [other_options] image_in image_out\n\n");
	printf("doimage -T i2c -D image_dest -E image_exec -M twsi_init_file\n");
	printf("         [other_options] image_in image_out\n\n");
	printf("\n\n\n");

}


/* 8 bit checksum */
u8 checksum8(u32 start, u32 len,u8 csum)
{
    register u8 sum = csum;
	volatile u8* startp = (volatile u8*)start;

    do{
	  	sum += *startp;
		startp++;
    }while(--len);

    return (sum);
}

/* 32 bit checksum */
u32 checksum32(u32 start, u32 len, u32 csum)
{
    register u32 sum = csum;
	volatile u32* startp = (volatile u32*)start;


    do{
	  	sum += *(u32*)startp;
		startp++;
		len -= 4;
    }while(len);

    return (sum);
}

void make_crc_table(MV_U32 *crc_table)
{
  	MV_U32 	c;
	MV_32 	n, k;
	MV_U32 	poly;

	/* terms of polynomial defining this crc (except x^32): */
	static const MV_U8 p[] = {0,1,2,4,5,7,8,10,11,12,16,22,23,26};

	/* make exclusive-or pattern from polynomial (0xedb88320L) */
	poly = 0L;
	for (n = 0; n < sizeof(p)/sizeof(MV_U8); n++)
    		poly |= 1L << (31 - p[n]);

	for (n = 0; n < 256; n++)
	{
		c = (MV_U32)n;
		for (k = 0; k < 8; k++)
			c = c & 1 ? poly ^ (c >> 1) : c >> 1;
 		crc_table[n] = c;
  	}
}

#define DO1(buf) crc = crc_table[((int)crc ^ (*buf++)) & 0xff] ^ (crc >> 8);
#define DO2(buf)  DO1(buf); DO1(buf);
#define DO4(buf)  DO2(buf); DO2(buf);
#define DO8(buf)  DO4(buf); DO4(buf);

MV_U32 crc32(MV_U32 crc, volatile MV_U32 *buf, MV_U32 len)
{		
	MV_U32 crc_table[256];

	/* Create the CRC table */
	make_crc_table(crc_table);

	crc = crc ^ 0xffffffffL;
    	while (len >= 8)
    	{
      		DO8(buf);
      		len -= 8;
    	}

    	if (len) do 
	{
      		DO1(buf);
    	} while (--len);
    
	return crc ^ 0xffffffffL;
}
