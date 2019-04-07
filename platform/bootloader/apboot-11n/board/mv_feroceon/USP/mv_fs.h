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
/********************************************************************************
* FileSys.h - Flash file system header
*
* DESCRIPTION:                                                               
* File system API for the flash unit integrated within the devices.
* DEPENDENCIES:
*       None.
*
*******************************************************************************/

#ifndef __INCmvFSh 
#define __INCmvFSh

#ifdef __cplusplus
extern "C" { 
#endif

/* defines */
/* 10 Blocks are dedicated for the FAT, the first one includes the FAT
   signature which is 32 Bytes, each file entry is 32 Bytes long , each block
   has a header 8 bytes long (in the FAT block the other 24 are not used) =>
   FS_MAX_NUM_OF_ENTRIES = 150 */
#define FS_FILE_HANDLER                    unsigned int
#define FS_MAX_NUM_OF_FAT_BLOCKS           10
#define FS_MAX_NUM_OF_ENTRIES              150 
#define FS_FAT_SIGNATURE_SIZE              15
#define FS_SIGNATURE_OFFSET                5
#define FS_FAT_HEADER_SIZE                 32
#define FS_FAT_FILE_ENTRY_SIZE             32
#define FS_FILE_SYSTEM_INITIALIZED         0x24136764
#define FS_FILE_NAME_OFFSET                5
#define FS_FILE_NAME_LENGTH                16
#define FS_FILE_EXT_OFFSET                 (FS_FILE_NAME_OFFSET+  \
                                                           FS_FILE_NAME_LENGTH)
#define FS_FILE_EXT_LENGTH                 3

#define FS_BLOCK_SIZE                      512
#define FS_BLOCK_HEADER_SIZE               8
#define FS_BLOCK_OFFSET_MASK               0xfffffe00
#define FS_BLOCK_OFFSET                    (0xffffffff - FS_BLOCK_OFFSET_MASK)
#define FS_ERSAING_COUNTER_FLAG_OFFSET     (FS_SIGNATURE_OFFSET+    \
                                                          FS_FAT_SIGNATURE_SIZE) 
#define FS_BLOCK_STATUS_FLAG_OFFSET        4
#define FS_BLOCK_OFFSET_FLAG_OFFSET        5
#define FS_BLOCK_OFFSET_OFFSET             6
#define FS_NO_FREE_BLOCKS                  0xffffffff
#define FS_FREE_BLOCK                      0xff
#define FS_BLOCK_IN_UPDATE                 0x7f
#define FS_FAT_BLOCK                       0x3f
#define FS_BLOCK_IN_USE                    0x1f
#define FS_BLOCK_USED_FOR_ERASING_COUNTER  0x01
#define FS_FREE_DIRTY_BLOCK                0x00
#define FS_FULL_BLOCK                      0x0F

#define FS_ENTRY_STATUS_FLAG_OFFSET        4
#define FS_NO_FREE_ENTRIES                 0xffffffff
#define FS_FREE_ENTRY                      0xff
#define FS_ENTRY_IN_UPDATE                 0x7f
#define FS_ENTRY_IN_USE                    0x1f
#define FS_FREE_DIRTY_ENTRY                0x00

#define FS_FILE_NOT_FOUND                  0xffffffff
#define FS_FILE_IS_NOT_CLOSED              0x1
#define FS_FILE_DELETED                    0x2
#define FS_FILE_EXIST                      0x3
#define FS_ENABLE_ERASING_COUNTER          0X0000ffff
#define FS_NO_ERASING_COUNTER              0Xffffffff
#define FS_NO_VALID_FAT_STRING             0xffffffff
#define FS_OFFSET_OUT_OF_RANGE             0xfffffff0
#define FS_FLASH_MEMORY_NOT_PRESENT        0xffffffff
#define FS_SYSTEM_ALREADY_INITIALIZED      0xfffffffc
#define FS_VALID_FILE_SYS_IN_LOWER_OFFSET  0xfffffff0
#define FS_FILE_OPEN_ERROR                 0xffffffff
#define FS_FILE_CLOSED                     0x1
#define FS_FILE_CLOSE_ERROR                0xffffffff
#define FS_FILE_READ_ERROR                 0xffffffff
/* Cache size = 8 Kb */
#define FS_NUMBER_OF_BLOCKS_FOR_CACHE      16
/* Operation mode defined in the call to mvFSInit */
#define FS_ENABLE_CACHE                    1
#define FS_NO_CACHE                        0
/* Defines for the seek operation */
/* Beginning of file */
#define FS_SEEK_SET                        0
/* End of file */
#define FS_SEEK_END                        -1
/* Current position of file pointer */
#define FS_SEEK_CUR                        1

/* File entry on the system's dynamic mvMemory */
typedef struct _fileTableEntry
{
    char   mode[4];
    unsigned int    pointerToFAT;
    unsigned int    filePointer;
    unsigned int    blockPointer; /* For read optimization */
    unsigned int    EOFpointer;
    unsigned int    statusFlag;
} FS_FILE_TABLE_ENTRY;
/* File entry on the FLASH mvMemory (FAT) - 32 Bytes long */
typedef struct _fileEntry
{
    unsigned int   nextBlockAddr;
    unsigned int   reserved1;
    unsigned int   reserved2;
    char           statusFlag;
    char           fileName[16];
    char           fileExt[3];
} FS_FILE_ENTRY;
/* 8 Bytes long */
typedef struct _blockHeader
{
    unsigned int    nextBlockAddr;
    char   statusFlag;
    char   offsetFlag;
    unsigned short  offset;
} FS_BLOCK_HEADER;
/* Cache properties defines */
typedef struct _fileCacheBlock
{
    unsigned int    fileHandler;
    unsigned int    currentBlockAddress;
    unsigned int    usageCounter;
    unsigned int    timeStamp;
    char   data[512];
} FS_FILE_CACHE_BLOCK;
 
/* Functions */

unsigned int    mvFSFormat(unsigned int offset,unsigned int mode);
unsigned int    mvFSInit(unsigned int mode);

FS_FILE_HANDLER    mvFSOpen(char * fileName,char *mode);
unsigned int    mvFSClose(FS_FILE_HANDLER fileHandler);
unsigned int    mvFSDelete(char * fileName);
unsigned int    mvFSFileFind(char * fileName);
unsigned int    mvFSFileSize(FS_FILE_HANDLER fileHandler);
unsigned int    mvFSWrite(FS_FILE_HANDLER fileHandler,
                             unsigned int numberOfBytes,char * block);
unsigned int    mvFSRead(FS_FILE_HANDLER fileHandler,
                            unsigned int numberOfBytes,char * block);
unsigned int    mvFSSeek(FS_FILE_HANDLER fileHandler,int offset,int origin);
unsigned int    mvFSRewind(FS_FILE_HANDLER fileHandler);
unsigned int    mvFSReadErasingCounter(unsigned int sectorNumber);

unsigned int    mvFSNumOfHandles(char * fileName);
unsigned int    mvFSSearchForSignature(void);
unsigned int    mvFSGetEOFoffset(char * fileName);
unsigned int    mvFSGetFreeEntry(void);
unsigned int    mvFSGetFreeBlock(void);
unsigned int    mvFSGetFreeBlockForWrite(void);
unsigned int    mvFSWriteFileNameToFATentry(unsigned int entryOffset,
                                               char * fileName);
unsigned int    mvFSReadFileNameFromFATentry(unsigned int entryOffset,
                                                char * fileName);
unsigned int    mvFSGetFreeSize(void);
unsigned int    mvFSFlushCache(void);


#ifdef __cplusplus
}
#endif

#endif /* __INCmvFSh */

