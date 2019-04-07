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
#include <config.h>
#include <common.h>
#include <malloc.h>
#include <net.h>
#include <command.h>

#include "mvSysHwConfig.h"
#if defined(MV_INCLUDE_MONT_EXT) && defined (MV_INCLUDE_MONT_FFS)

#include "mvTypes.h"
#include "norflash/mvFlash.h"
#include "mvCpuIf.h"
/*#include "mv_mon.h"*/
#include "mv_fs.h"

#if defined(CONFIG_CMD_BSP)

/* #define DEBUG */
#ifdef DEBUG
#define DB(x)	x
#else
#define DB(x)
#endif

static MV_FLASH_INFO *mvFlashInfo;

/* File system data base variables */
/* FAT signature */
char                FATsignature[] = "FAT024136764000";
unsigned int        initializationFlag;
unsigned int        mvFSTimeStamp;
unsigned int        operationMode;
unsigned int        mvFSOffset;
unsigned int        mvFSTopOffset;
unsigned int        nextFreeBlock;
FS_FILE_TABLE_ENTRY    mvFSEntries[FS_MAX_NUM_OF_ENTRIES];
FS_FILE_CACHE_BLOCK    mvFSCacheBlocks[FS_NUMBER_OF_BLOCKS_FOR_CACHE];

/* Functions */
extern void d_i_cache_clean(unsigned int Address, unsigned int byteCount);
extern int load_net(MV_U32 *loadnet_addr);
extern unsigned int VIRTUAL_TO_PHY(unsigned int address); 
extern void mv_go(unsigned long addr,int argc, char *argv[]);
extern MV_FLASH_INFO* getMvFlashInfo(int index);

/*******************************************************************************
* mvFSFormat - Formats the mvFlash mvMemory for file system usage.
*
* DESCRIPTION:
*       This function formats the flash memory for file system usage. It 
*       allocates 10 blocks for FAT in a form of a linked list and write the 
*       signature (signature defind in mvFS.h) in each FAT header block 
*       within the flash. The format starts from the ‘offset’ parameter value 
*       within the flash if the offset is aligned with one of the flash's 
*       sectors start address, otherwise it will be incremented to the nearest
*       sector start address and then perform the format.The format can be 
*       accomplished in 2 modes as described below.
*
* INPUT:
*       offset  - Starting  point of the format within the flash.
*
*       mode - Defines whether an erasing counter will be enabled or not . 
*              When erasing counter is enabled , the function will allocate one
*              block at the end of  each sector to act as a counter for the 
*              number of erasures of  that sector . The possible values : 
*              - ENABLE_ERASING_COUNTER
*              - NO_ERASING_COUNTER
* OUTPUT:
*       Initialized flash for file system usage.
*
* RETURN:
*       The actual offset (after alignment to the nearest sector start address) 
*       or: 
*       FLASH_MEMORY_NOT_PRESENT - if no flash memory present . 
*       VALID_FILE_SYS_IN_LOWER_OFFSET - if a valid file system residing in a 
*                                        lower offset.
*
*******************************************************************************/
unsigned int mvFSFormat(unsigned int offset,unsigned int mode)
{
    unsigned int i,sectorSize,tempOffset,limit,previousCount;
    unsigned int erasingCounterWasEnabled = 0,status;

    DB(printf("start mvFSFormat\n"));
    DB(printf("Flash base addr 0x%x\n",mvFlashInfo->baseAddr));

    if(offset != mvFlashSecOffsGet(mvFlashInfo, mvFlashInWhichSec(mvFlashInfo, offset)))
    {
        offset = mvFlashSecOffsGet(mvFlashInfo, mvFlashInWhichSec(mvFlashInfo, offset) + 1);
    }
    if(offset != 0xffffffff)
    {
        if(mode == FS_ENABLE_ERASING_COUNTER)
        {
            tempOffset = mvFSSearchForSignature();
            if(tempOffset != FS_NO_VALID_FAT_STRING)
            {
                if(tempOffset < offset) 
                    return FS_VALID_FILE_SYS_IN_LOWER_OFFSET;
                if(mvFlash32Rd(mvFlashInfo, tempOffset + FS_ERSAING_COUNTER_FLAG_OFFSET) 
                   == FS_ENABLE_ERASING_COUNTER)
                {
                    erasingCounterWasEnabled = 1;
                }
            }
        }
        tempOffset = offset;
        /* Eraseing the sectors dedicated for the file system */
        while(mvFlashInWhichSec(mvFlashInfo, offset) != 0xffffffff)
        {
            if(mode == FS_NO_ERASING_COUNTER)
            {
		DB(printf("mvFSFormat - mode = FS_NO_ERASING_COUNTER\n"));
                i = mvFlashInWhichSec(mvFlashInfo, offset);
                sectorSize = mvFlashSecSizeGet(mvFlashInfo, i);
                limit = offset + sectorSize;
                for(; offset < limit ; offset+=4)
                {
                    if(mvFlash32Rd(mvFlashInfo, offset) != 0xffffffff)
                    {
			DB(printf("mvFSFormat - Erase flash sector 0x%x\n", mvFlashInWhichSec(mvFlashInfo, offset)));
                        status = mvFlashSecErase(mvFlashInfo, mvFlashInWhichSec(mvFlashInfo, offset));
			if (status != MV_OK)
			{
				printf("mvFlashSecErase - FAIL!\n");
				return status;
			}

                        break;
                    }
                }
                offset += sectorSize;
            }
            else /* mode == FS_ENABLE_ERASING_COUNTER */
            {
		DB(printf("mvFSFormat - mode = FS_ENABLE_ERASING_COUNTER\n"));
                i = mvFlashInWhichSec(mvFlashInfo, offset);
                sectorSize = mvFlashSecSizeGet(mvFlashInfo, i);
                limit = offset + sectorSize - FS_BLOCK_SIZE;
                previousCount = mvFlash32Rd(mvFlashInfo, limit);
                for(; offset <= (limit + 4) ; offset+=4)
                {
                    if(mvFlash32Rd(mvFlashInfo, offset) != 0xffffffff)
                    {
                        mvFlashSecErase(mvFlashInfo, mvFlashInWhichSec(mvFlashInfo, offset));
                        if(erasingCounterWasEnabled)
                        {
                            /* Increment the counter */
                            mvFlash32Wr(mvFlashInfo, limit, previousCount + 1);
                            mvFlash8Wr(mvFlashInfo, limit+FS_BLOCK_STATUS_FLAG_OFFSET,
                                           FS_BLOCK_USED_FOR_ERASING_COUNTER);
                        }
                        break;
                    }
                }
                if(!erasingCounterWasEnabled)
                {
                    /* Initialize the counter */
                    mvFlash32Wr(mvFlashInfo, limit, 0);
                    mvFlash8Wr(mvFlashInfo, limit + FS_BLOCK_STATUS_FLAG_OFFSET,
                                   FS_BLOCK_USED_FOR_ERASING_COUNTER);
                }
                offset = limit + FS_BLOCK_SIZE;
            }
        }
        /* Fill the FAT's header */
        offset = tempOffset;
        /* Update the status flag */
        mvFlash8Wr(mvFlashInfo, offset + FS_BLOCK_STATUS_FLAG_OFFSET,FS_FAT_BLOCK);
        /* Write the FAT's signature */
        mvFlashBlockWr(mvFlashInfo, offset + FS_SIGNATURE_OFFSET, FS_FAT_SIGNATURE_SIZE,
                        (MV_U8 *)FATsignature);
        /* Check if we were able to burn the mvFlash */
        for(i = 0 ; i < FS_FAT_SIGNATURE_SIZE ; i++)
        {
            if(mvFlash8Rd(mvFlashInfo, offset + FS_SIGNATURE_OFFSET + i) != FATsignature[i]) 
                return FS_FLASH_MEMORY_NOT_PRESENT;
        }
        if(mode == FS_ENABLE_ERASING_COUNTER)
        {
            mvFlash32Wr(mvFlashInfo, offset + FS_ERSAING_COUNTER_FLAG_OFFSET,
                           FS_ENABLE_ERASING_COUNTER);
        }
        mvFlash32Wr(mvFlashInfo, offset, offset + FS_BLOCK_SIZE);
        /* Alocate 9 more blocks for the FAT */
        for(i = 1 ; i < (FS_MAX_NUM_OF_FAT_BLOCKS - 1) ; i++)
        {
            /* Next block pointer */
            mvFlash32Wr(mvFlashInfo, offset+(FS_BLOCK_SIZE*(i)),offset+
                             (FS_BLOCK_SIZE*(i+1)));
            /* Flag */
            mvFlash8Wr(mvFlashInfo, offset+(FS_BLOCK_SIZE*(i)) + 
                             FS_BLOCK_STATUS_FLAG_OFFSET, FS_FAT_BLOCK);
        }
        /* The last block's next pointer is 0xffffffff since it is the last */
        mvFlash8Wr(mvFlashInfo, offset + (FS_BLOCK_SIZE * i) + 
                         FS_BLOCK_STATUS_FLAG_OFFSET, FS_FAT_BLOCK);
    }
    return offset;
}

/*******************************************************************************
* mvFSInit - Initializes the flash system's data base variables.
*
* DESCRIPTION:
*       This function initializes the flash system's database variables residing
*       in the systems dynamic memory (SDRAM) . It searches within the flash 
*       memory from the begining, for the first occurrence of a valid signature.
*       If found, a valid file system is assumed to be exist starting from the
*       sector the signature was found and all the database variables will than 
*       be initialized ,otherwise it returns NO_VALID_FAT_STRING or 
*       OFFSET_OUT_OF_RANGE to indicate that no valid file system was found 
*       within the flash .If a valid file system was found ,the function returns
*       the number of  files within the flash .The ‘mode’ parameter delivered to
*       the function is for future needs to support two cache modes(cache mode,
*       non - cache mode) and currently this feature is not implemented in this 
*       current version of this function. 
*      NOTE!!!!:
*       It is a MUST to call this function before any attempt to use file-system
*       driver.   
* INPUT:
*       mode - For future needs to support cache or non-cache modes (currently 
*              not implemented).
*       
* OUTPUT:
*       Initialized data base for the file system.
*
* RETURN:
*       Number of files currently on the system or:
*       NO_VALID_FAT_STRING - if no file system was found (Try to use 
*                             ‘fileSysFormat’ function in this case).
*       OFFSET_OUT_OF_RANGE - if there was an error finding a signature found is
*                             SYSTEM_ALREADY_INITIALIZED.
*
*******************************************************************************/
unsigned int mvFSInit(unsigned int mode)
{
    /* Initially we are not going to support the FS_ENABLE_CACHE mode */
    unsigned int offset = 0,i = 0,numOfFiles,nextBlockOffset;
    unsigned int fileEntryOffset,tempOffset;

    /* set the global flash info structure */
	mvFlashInfo = getMvFlashInfo(MAIN_FLASH_INDEX);

    /* Seek for the FAT signature */    
    offset = mvFSSearchForSignature();
    if(offset == FS_NO_VALID_FAT_STRING)   
        return FS_NO_VALID_FAT_STRING;
    if(offset >= mvFlashSizeGet(mvFlashInfo))        
        return FS_OFFSET_OUT_OF_RANGE;
    mvFSTopOffset = mvFlashSizeGet(mvFlashInfo);

    /* Check if the system is already initialized , if so we will not
       re-initialize */
    if(initializationFlag == FS_FILE_SYSTEM_INITIALIZED)
        return FS_SYSTEM_ALREADY_INITIALIZED;
    
    /* We found the FAT's signature, now we need to initialize the file system
       data base and count the number of files */
    mvFSTimeStamp    = 0;                              
    operationMode       = mode;                                 
    mvFSOffset       = offset;
    nextFreeBlock       = 0xffffffff - FS_BLOCK_SIZE;
    /* Initialize the files entries */
    for(i = 0 ; i < FS_MAX_NUM_OF_ENTRIES ; i++)
    {
        mvFSEntries[i].filePointer       = 0;
        mvFSEntries[i].statusFlag        = FS_FREE_ENTRY;
        mvFSEntries[i].mode[0]           = '\0';
        mvFSEntries[i].pointerToFAT      = 0xffffffff;
        mvFSEntries[i].EOFpointer        = 0;
    }
    /* Initialize the cache blocks */
    for(i = 0 ; i < FS_NUMBER_OF_BLOCKS_FOR_CACHE ; i++)
    {
        mvFSCacheBlocks[i].currentBlockAddress   = 0xffffffff;
        mvFSCacheBlocks[i].fileHandler           = 0;
        mvFSCacheBlocks[i].usageCounter          = 0;
        mvFSCacheBlocks[i].timeStamp             = 0;
    }
    /* Count the number of files currently on the system */
    numOfFiles      = 0;
    nextBlockOffset = mvFSOffset;
    while(nextBlockOffset != 0xffffffff)
    {
        /* Skip the block's header */
        fileEntryOffset = nextBlockOffset + FS_FAT_HEADER_SIZE;
        for(i = 0 ; i < 15 ; i++)
        {
            if(mvFlash8Rd(mvFlashInfo, fileEntryOffset + FS_ENTRY_STATUS_FLAG_OFFSET)
               == FS_ENTRY_IN_USE) 
                numOfFiles++;
            /* If we have files which were not properly closed we need to
               to remove them from the system */
            if(mvFlash8Rd(mvFlashInfo, fileEntryOffset + FS_ENTRY_STATUS_FLAG_OFFSET)
               == FS_ENTRY_IN_UPDATE)
            {
                mvFlash8Wr(mvFlashInfo, fileEntryOffset + FS_ENTRY_STATUS_FLAG_OFFSET,
                               FS_FREE_DIRTY_ENTRY);
                /* Free the file's data blocks */
                tempOffset = mvFlash32Rd(mvFlashInfo, fileEntryOffset);
                while(tempOffset != 0xffffffff)
                {
                    tempOffset = tempOffset & FS_BLOCK_OFFSET_MASK;
                    mvFlash8Wr(mvFlashInfo, tempOffset + FS_BLOCK_STATUS_FLAG_OFFSET,
                                   FS_FREE_DIRTY_BLOCK);
                    tempOffset = mvFlash32Rd(mvFlashInfo, tempOffset);
                }
            }
            /* point to the next entry */
            fileEntryOffset += FS_FAT_FILE_ENTRY_SIZE; 
        }
        nextBlockOffset = mvFlash32Rd(mvFlashInfo, nextBlockOffset);
    }
    initializationFlag = FS_FILE_SYSTEM_INITIALIZED;
    return numOfFiles;
}

/*******************************************************************************
* mvFSDelete - Deletes a file for the file system.
*
* DESCRIPTION:
*       This function deletes a file in the file-system. The function actually 
*       marks the file's entry status flag as FREE_DIRTY_ENTRY and the file's 
*       status flag of each data block with the value FREE_DIRTY_BLOCKs to 
*       indicate that this entery and all the file’s data blocks are no longer 
*       associated by this file. If the file has valid handles associated with
*       it the file will not be deleted .  
*      NOTE:
*       The file system's data base must be initialized by ‘fileSysInit’ 
*       function before using this function .         
*
* INPUT:
*       fileName - A string containing the file name.
*
* OUTPUT:
*       On success the file will be deleted..
*
* RETURN:
*       On success - The return value will be FS_FILE_DELETED.
*       On failure one of the following parameters:
*       FS_FILE_NOT_FOUND - if the data base was initialized or file was not
*                              found.
*       FS_FILE_IS_NOT_CLOSED - if the file has valid handles associated with
*                                  it.
*******************************************************************************/
unsigned int mvFSDelete(char * fileName)
{
    unsigned int fileOffset,numOfHandles,tempOffset;

    /* Check if the file system's data base was initialized */
    if(initializationFlag != FS_FILE_SYSTEM_INITIALIZED)
        return FS_FILE_NOT_FOUND;
    
    if((fileOffset = mvFSFileFind(fileName)) == FS_FILE_NOT_FOUND) 
        return FS_FILE_NOT_FOUND;
    if((numOfHandles = mvFSNumOfHandles(fileName)) == FS_FILE_NOT_FOUND)
        return FS_FILE_NOT_FOUND;
    if(numOfHandles > 0) 
        return FS_FILE_IS_NOT_CLOSED;
    
    mvFlash8Wr(mvFlashInfo, fileOffset + FS_ENTRY_STATUS_FLAG_OFFSET,
                     FS_FREE_DIRTY_ENTRY);
    /* Free the file's data blocks */
    tempOffset = mvFlash32Rd(mvFlashInfo, fileOffset);
    while(tempOffset != 0xffffffff)
    {
        tempOffset = tempOffset & FS_BLOCK_OFFSET_MASK;
        mvFlash8Wr(mvFlashInfo, tempOffset + FS_BLOCK_STATUS_FLAG_OFFSET,
                         FS_FREE_DIRTY_BLOCK);
        tempOffset = mvFlash32Rd(mvFlashInfo, tempOffset);
    }
    return FS_FILE_DELETED;
}

/*******************************************************************************
* mvFSFileFind - Check if a file exists in the file system.
*
* DESCRIPTION:
*       This function checks if the file ‘fileName’ exists in the file system.
*       The function will search for the file with the matching name and 
*       extension and if found , the file's entry offset in the FAT is returned.
*      NOTE:
*       The file system's data base must be initialized by ‘fileSysInit’ 
*       function before using this function .  
*       
* INPUT:
*       fileName - A string containing the file name.
*
* OUTPUT:
*       None.
*
* RETURN:
*       FILE_NOT_FOUND or the file's entery offset in the FAT.
*
*******************************************************************************/
unsigned int mvFSFileFind(char * fileName)
{
    unsigned int    nextBlockOffset,fileEntryOffset,index;
    char   tempString[20];
    
    /* Check if the file system's data base was initialized */
    if(initializationFlag != FS_FILE_SYSTEM_INITIALIZED)
        return FS_FILE_NOT_FOUND;
    nextBlockOffset = mvFSOffset;
    while(nextBlockOffset != 0xffffffff)
    {
        /* Skip the block's header */
        for(fileEntryOffset = nextBlockOffset + FS_FAT_HEADER_SIZE;
            fileEntryOffset < (nextBlockOffset + FS_BLOCK_SIZE);
            fileEntryOffset += FS_FAT_FILE_ENTRY_SIZE)
        {
            index = 0;
            if(mvFlash8Rd(mvFlashInfo, fileEntryOffset + FS_ENTRY_STATUS_FLAG_OFFSET)
               == FS_ENTRY_IN_USE) 
            {
                /* Read the file's name */
                mvFSReadFileNameFromFATentry(fileEntryOffset,tempString);
                if(strcmp(tempString,fileName) == 0) return fileEntryOffset;
                /* else , try the next entry */
            }
        }
        nextBlockOffset = mvFlash32Rd(mvFlashInfo, nextBlockOffset);
    }
    return FS_FILE_NOT_FOUND;
}

/*******************************************************************************
* mvFSNumOfHandles - Counts the number of handles currently pointing to a file.
*
* DESCRIPTION:
*       This function counts the number of handles currently associated to the 
*       file ‘fileName’.The entry table in the dynamic database is formed from 
*       an array of  FILE_TABLE_ENTRY (structure defined in mvFS.h) and the 
*       file handler acts as the index of that array . The function counts each 
*       entry within the array that points to the file ‘fileName’ and return 
*       that value.
*      NOTE:
*       The file system's data base must be initialized by ‘fileSysInit’ 
*       function before using this function .  
*       
* INPUT:
*       fileName - A string containing the file name.
*
* OUTPUT:
*       None.
*
* RETURN:
*       on success , the Number of handlers currently associated to the file 
*       ‘fileName’.
*       on failure , FS_FILE_NOT_FOUND.
*
*******************************************************************************/
unsigned int mvFSNumOfHandles(char * fileName)
{
    unsigned int    fileEntryOffset,i,numOfHandles;
    if((fileEntryOffset = mvFSFileFind(fileName)) == 0xffffffff)
        return FS_FILE_NOT_FOUND;

    numOfHandles = 0;
    for(i = 0 ; i < FS_MAX_NUM_OF_ENTRIES ; i++)
    {
        if(fileEntryOffset == mvFSEntries[i].filePointer) 
            numOfHandles++;
    }
    return numOfHandles;
}

/*******************************************************************************
* mvFSSearchForSignature - Searches for the FAT's signature string, if the 
*                          string is found we are assuming a valid file system.
*
* DESCRIPTION:
*       This function searches within the flash memory from the begining for the
*       first occurrence of a valid signature .If found , it returns the sector 
*       offset on which the signature first occurred , otherwise it returns 
*       NO_VALID_FAT_STRING to indicate the flash is not formated , in that case
*       you might want to use fileSysformat function. 
*
* INPUT:
*       None.
*
* OUTPUT:
*       None.
*
* RETURN:
*       The offset of the sector in which the signature was found,
*       otherwise FS_NO_VALID_FAT_STRING .
*
*******************************************************************************/
unsigned int mvFSSearchForSignature(void)
{
    unsigned int offset = 0,i,sectorNumber;
    
    sectorNumber = mvFlashInWhichSec(mvFlashInfo, offset);
    while((offset = mvFlashSecOffsGet(mvFlashInfo, sectorNumber)) != 0xffffffff)
    {
        for(i = 0 ; i < FS_FAT_SIGNATURE_SIZE ; i++)
        {
            if(mvFlash8Rd(mvFlashInfo, offset + FS_SIGNATURE_OFFSET + 
                               i) != FATsignature[i])
                break;
        }
        if(i == FS_FAT_SIGNATURE_SIZE) break;
        offset += mvFlashSecSizeGet(mvFlashInfo, sectorNumber);
        sectorNumber = mvFlashInWhichSec(mvFlashInfo, offset);
        if(sectorNumber == 0xffffffff) return FS_NO_VALID_FAT_STRING;
    }
    if(offset == 0xffffffff) return FS_NO_VALID_FAT_STRING;
    return offset;
}

/*******************************************************************************
* mvFSOpen - Opens a file for read/write/append.
*
* DESCRIPTION:
*       This function opens a file for reading , writing or appending data .The
*       function will act differantly upon each mode delivered by the parameter
*       ‘mode’.
*      NOTE:
*       The file system's data base must be initialized by ‘fileSysInit’ 
*       function before using this function .   
*
* INPUT:
*       mode - defines on which mode the file will be open .All possible values
*              for ‘mode’ parameter are described below:
*
“              "r"  - Opens the file for reading only . If the file does not 
*                     exist or cannot be found , the function fails . After the 
*                     file has been  found , the function searches for an empty 
*                     entry within the dynamic data base entry table and if it 
*                     found one  , it associates the file ‘fileName’ to that 
*                     entry and returns the handler of the file.
*              "r+" - Opens the file for both reading and writing. If the file
*                     does not exist or cannot be found  , the  function fails.
*                     After the file has been  found , the function searches 
*                     for an empty entry  within the dynamic data base entry 
*                     table and if  found  , it associates the file ‘fileName’ 
*                     to that entry and returns the handler of the file . When 
*                     a file is opened with the  “r+” all write operations 
*                     occur at the end of the file. The file pointer can be 
*                     repositioned using fileSysSeek or fileSysRewind , but is 
*                     always moved back to the end of the file before any write
*                     operation is carried out. Thus ,existing data cannot be 
*                     overwritten.
*              "w" -  Opens an empty  file for writing only. If the file does 
*                     not exist the function will create it, otherwise if the 
*                     given file exists the function will open it but its 
*                     content  will be  destroyed . On both cases the function
*                     searches for an empty entry within the dynamic data base
*                     entry table and if  found  , it associates  the file
*                     ‘fileName’ to that entry and returns the handler of the
*                     file .
*              "w+" - Opens an empty  file for both reading and writing data. If
*                     the given file exists, its contents is destroyed . When a
*                     file is opened with the "w+" access type , all write 
*                     operations occur at the end of the file. The file pointer
*                     can be repositioned using fileSysSeek or fileSysRewind , 
*                     but is always moved back to the end of the file before any
*                     write operation is carried out. Thus ,existing data cannot
*                     be overwritten.
*              "a+" - Opens a file  for reading and appending data. If the given
*                     file exists ,its contents is destroyed .The appending 
*                     operation includes the removal of the EOF marker before 
*                     new data is written to the file and the EOF marker is 
*                     restored after the writing has completed  . When a file is
*                     opened with the "a+" access type ,all write operations 
*                     occur at the end of the file. The file pointer can be 
*                     repositioned using fileSysSeek or fileSysRewind , but is 
*                     always moved back to the end of the file before any write 
*                     operation is carried out .Thus , existing data cannot be 
*                     overwritten.
*       fileName - A string containing the file name. 
*
* OUTPUT:
*       None.
*
* RETURN:
*       File handler on success or FILE_OPEN_ERROR in one of the following 
*       reasons:
*       - File system not initialized.
*       - In cases of  the modes  "r" or "r+" and the file is not found.
*       - No free entries in the file system's data base.
*       - In modes "w" and w+" if we try to open a file which already exists it 
*         must be deleted first , an error can occur at the delete function.
*       - No more free entries on the flash memory.
*       - Not supported mode.
*
*******************************************************************************/
FS_FILE_HANDLER mvFSOpen(char * fileName,char *mode)
{
    unsigned int fileFAToffset,fh;

    /* Check if the file system's data base was initialized */
    if(initializationFlag != FS_FILE_SYSTEM_INITIALIZED)
    {
        return FS_FILE_OPEN_ERROR;
    }
    /* Mode == "r" or "r+" - the file must exist */
    if((mode[0] == 'r') && 
       (mode[1] == '\0'|| (mode[1] == '+' && mode[2] == '\0')))
    {
        fileFAToffset = mvFSFileFind(fileName);
        if(fileFAToffset == FS_FILE_NOT_FOUND)
            return FS_FILE_OPEN_ERROR;
        for(fh = 0 ; fh < FS_MAX_NUM_OF_ENTRIES ; fh++)
        {
            if(mvFSEntries[fh].statusFlag == FS_FREE_ENTRY)
            {
                mvFSEntries[fh].statusFlag    = FS_ENTRY_IN_USE;
                mvFSEntries[fh].filePointer   = 0;
                mvFSEntries[fh].mode[0]       = mode[0];
                mvFSEntries[fh].mode[1]       = mode[1];
                mvFSEntries[fh].mode[2]       = mode[2];
                mvFSEntries[fh].pointerToFAT  = fileFAToffset;
                mvFSEntries[fh].EOFpointer    = mvFSGetEOFoffset(fileName);
                return fh;
            }
        }
        return FS_FILE_OPEN_ERROR;
    }
    /* Mode == "w" ,"w+" or "a+" */
    if((mode[0] == 'w' || mode[0] == 'a') &&
       (mode[1] == '\0'||(mode[1] == '+' && mode[2] == '\0')))
    {
        /* Check if the file exists on the system */
        fileFAToffset = mvFSFileFind(fileName);
        if(fileFAToffset != FS_FILE_NOT_FOUND)
        {
            if( mode[0] != 'a')
            {
                if(mvFSDelete(fileName) != FS_FILE_DELETED)
                    return FS_FILE_OPEN_ERROR;
                /* Get a free entry on the FAT */
                fileFAToffset = mvFSGetFreeEntry();
                if(fileFAToffset == FS_NO_FREE_ENTRIES)
                    return FS_FILE_OPEN_ERROR;
            }
        }
        else
        {
            /* Get a free entry on the FAT */
            fileFAToffset = mvFSGetFreeEntry();
            if(fileFAToffset == FS_NO_FREE_ENTRIES)
                return FS_FILE_OPEN_ERROR;
        }
        /* Search for a free entry on the file system's data base */
        for(fh = 0 ; fh < FS_MAX_NUM_OF_ENTRIES ; fh++)
        {
            if(mvFSEntries[fh].statusFlag == FS_FREE_ENTRY)
            {
                mvFSEntries[fh].statusFlag  = FS_ENTRY_IN_USE;
                mvFSEntries[fh].filePointer = 0;
                mvFSEntries[fh].mode[0]     = mode[0];
                mvFSEntries[fh].mode[1]     = mode[1];
                mvFSEntries[fh].mode[2]     = mode[2];
                mvFSEntries[fh].pointerToFAT= fileFAToffset;
                mvFSEntries[fh].EOFpointer  = mvFSGetEOFoffset(fileName);
                /* Update the entry on the FAT */
                /* Write the file's name */
                mvFSWriteFileNameToFATentry(fileFAToffset,fileName);
                /* Update the status flag */
                mvFlash8Wr(mvFlashInfo, fileFAToffset + FS_ENTRY_STATUS_FLAG_OFFSET,
                               FS_ENTRY_IN_UPDATE);
                return fh;
            }
        }
        return FS_FILE_OPEN_ERROR;
    }
    return FS_FILE_OPEN_ERROR; /* Has no meaning, just for the compiler */
}

/*******************************************************************************
* mvFSClose - Closes a file.
*
* DESCRIPTION:
*       This function closes a file by removing the file's entry from the file 
*       system's data base ,  updating  the file's FAT entry and data block's 
*       status flags and offset . After calling this function , the virtual EOF
*       character is created by updating the last block’s  9 bits of the next 
*       block pointer if data was not appended to that block . If data was 
*       appended , the offset field of the last block of data will be updated 
*       (as described at the Introduction ).
*
* INPUT:
*       fileName - A string containing the file name.
*
* OUTPUT:
*       None.
*
* RETURN:
*       The return value can be either FS_FILE_CLOSED on seccess or 
*       FILE_CLOSE_ERROR in one of the following reasons:
*       - If the handler is out of range (bigger or equal to MAX_NUM_OF_ENTRIES
*         defind in mvFS.h).
*       - If the handler points to an entry with a FREE_ENTRY status.
*
*******************************************************************************/
unsigned int mvFSClose(FS_FILE_HANDLER fileHandler)
{
    unsigned int FATentryOffset,nextBlockPointer,prevBlockPointer;
    unsigned int offset;

    if(fileHandler >= FS_MAX_NUM_OF_ENTRIES) 
        return FS_FILE_CLOSE_ERROR;
    if(mvFSEntries[fileHandler].statusFlag == FS_FREE_ENTRY)
        return FS_FILE_CLOSE_ERROR;
    /* mode = "r" */
    if(mvFSEntries[fileHandler].mode[0] == 'r' &&
       mvFSEntries[fileHandler].mode[1] == '\0')
    {
        mvFSEntries[fileHandler].statusFlag = FS_FREE_ENTRY;
        return FS_FILE_CLOSED;
    }
    /* mode = "r+","w","w+" or "a+" */
    FATentryOffset = mvFSEntries[fileHandler].pointerToFAT;
    /* Empty file => next block pointer == 0xFFFFFFFF */
    nextBlockPointer = mvFlash32Rd(mvFlashInfo, FATentryOffset);
    if(nextBlockPointer == 0xffffffff)
    {
        if(mvFlash8Rd(mvFlashInfo, FATentryOffset + FS_ENTRY_STATUS_FLAG_OFFSET) == 
           FS_ENTRY_IN_UPDATE)
        {
            /* Update the file's entry status flag */
            mvFlash8Wr(mvFlashInfo, FATentryOffset + FS_ENTRY_STATUS_FLAG_OFFSET,
                             FS_ENTRY_IN_USE);
        }
        mvFSEntries[fileHandler].statusFlag = FS_FREE_ENTRY;
        return FS_FILE_CLOSED;
    }
    /* search for the last block */
    prevBlockPointer = FATentryOffset;
    while(mvFlash32Rd(mvFlashInfo, nextBlockPointer & 
                          FS_BLOCK_OFFSET_MASK) != 0xffffffff)
    {
        prevBlockPointer = nextBlockPointer;
        nextBlockPointer = mvFlash32Rd(mvFlashInfo, nextBlockPointer & 
                                           FS_BLOCK_OFFSET_MASK);
    }
    
    if(mvFlash8Rd(mvFlashInfo, (nextBlockPointer & FS_BLOCK_OFFSET_MASK) 
                     + FS_BLOCK_STATUS_FLAG_OFFSET) == FS_BLOCK_IN_USE)
    {
        /* Old block, need to use the offset field */
        if(!((mvFlash8Rd(mvFlashInfo, (nextBlockPointer & FS_BLOCK_OFFSET_MASK) 
                            + FS_BLOCK_OFFSET_FLAG_OFFSET) == FS_FULL_BLOCK) ||
           (mvFlash16Rd(mvFlashInfo, (nextBlockPointer & FS_BLOCK_OFFSET_MASK) 
                           + FS_BLOCK_OFFSET_OFFSET) != 0xffff)))
        {
            /* File was modified */
            /* We need to update the offset field */
            if(prevBlockPointer == FATentryOffset)
            {
                offset = mvFlash32Rd(mvFlashInfo, prevBlockPointer) & FS_BLOCK_OFFSET;
            }
            else
            {
                offset = mvFlash32Rd(mvFlashInfo, prevBlockPointer & 
                                        FS_BLOCK_OFFSET_MASK) & FS_BLOCK_OFFSET;
            }
            offset = (mvFSEntries[fileHandler].EOFpointer 
                      & FS_BLOCK_OFFSET) - offset;
            mvFlash16Wr(mvFlashInfo, (nextBlockPointer & FS_BLOCK_OFFSET_MASK) 
                                + FS_BLOCK_OFFSET_OFFSET,offset);
        }
        /* else - File not modified */
        mvFSEntries[fileHandler].statusFlag = FS_FREE_ENTRY;
        return FS_FILE_CLOSED;
    }
    else
    {
        /* Update the block's status flag */
        mvFlash8Wr(mvFlashInfo, (nextBlockPointer & FS_BLOCK_OFFSET_MASK) + 
                       FS_BLOCK_STATUS_FLAG_OFFSET,FS_BLOCK_IN_USE);
        /* Update the offset at the previous block's next block pointer */
        offset = mvFSEntries[fileHandler].EOFpointer & FS_BLOCK_OFFSET;
        if(prevBlockPointer != FATentryOffset)
        {
            offset = (mvFlash32Rd(mvFlashInfo, prevBlockPointer & FS_BLOCK_OFFSET_MASK) 
                      & FS_BLOCK_OFFSET_MASK) | offset;
            mvFlash32Wr(mvFlashInfo, prevBlockPointer & FS_BLOCK_OFFSET_MASK,offset);
        } 
        else /* prevBlockPointer == FATentryOffset */
        {
            offset = (mvFlash32Rd(mvFlashInfo, prevBlockPointer) & FS_BLOCK_OFFSET_MASK) 
                      | offset;
            mvFlash32Wr(mvFlashInfo, prevBlockPointer, offset);
        }
        mvFSEntries[fileHandler].statusFlag = FS_FREE_ENTRY;
    }
    return FS_FILE_CLOSED;
}

/*******************************************************************************
* mvFSFileSize - Counts the number of bytes the file consist of.
*
* DESCRIPTION:
*       This function counts and return the number of bytes the file consist of.
*       If the file handler delivered to the function is erroneous the function
*       will return the value 0xFFFFFFFF. 
*
* INPUT:
*       fileHandler - The file handler to count its bytes.
*
* OUTPUT:
*       None.
*
* RETURN:
*       The file's number of bytes of the on success or 0xFFFFFFFF on failier.
*
*******************************************************************************/
unsigned int mvFSFileSize(FS_FILE_HANDLER fileHandler)
{
    unsigned int numOfBytes = 0,nextBlockPointer;
    
    if((fileHandler >= FS_MAX_NUM_OF_ENTRIES) ||
       (mvFSEntries[fileHandler].statusFlag == FS_FREE_ENTRY))
        return 0xffffffff;
    nextBlockPointer = mvFlash32Rd(mvFlashInfo, mvFSEntries[fileHandler].pointerToFAT);
    if(nextBlockPointer == 0xffffffff)
        return 0;
    while(nextBlockPointer != 0xffffffff)
    {
        if(((nextBlockPointer & FS_BLOCK_OFFSET) + 1) != FS_BLOCK_SIZE)
        {
            if(mvFlash8Rd(mvFlashInfo, (nextBlockPointer & FS_BLOCK_OFFSET_MASK) + 
                             FS_BLOCK_OFFSET_FLAG_OFFSET) == FS_FULL_BLOCK)
            {
                numOfBytes += (FS_BLOCK_SIZE - FS_BLOCK_HEADER_SIZE);
            }
            else
            {
                if(mvFlash16Rd(mvFlashInfo, (nextBlockPointer & FS_BLOCK_OFFSET_MASK) +
                                  FS_BLOCK_OFFSET_OFFSET) != 0xffff)
                {
                    numOfBytes += ((nextBlockPointer & FS_BLOCK_OFFSET) +
                        mvFlash16Rd(mvFlashInfo, (nextBlockPointer & 
                                       FS_BLOCK_OFFSET_MASK) + 
                                       FS_BLOCK_OFFSET_OFFSET) - 
                        FS_BLOCK_HEADER_SIZE + 1);
                }
                else
                {
                    numOfBytes += ((nextBlockPointer & FS_BLOCK_OFFSET) - 
                                   FS_BLOCK_HEADER_SIZE + 1);
                }
            }
        }
        else 
        {
            if((mvFSEntries[fileHandler].EOFpointer & FS_BLOCK_OFFSET_MASK) ==
               (nextBlockPointer & FS_BLOCK_OFFSET_MASK)) /* Last block */
            {
                numOfBytes += ((mvFSEntries[fileHandler].EOFpointer 
                               & FS_BLOCK_OFFSET)- FS_BLOCK_HEADER_SIZE + 1);
            }
            else
            {
                numOfBytes += (FS_BLOCK_SIZE - FS_BLOCK_HEADER_SIZE); 
            }
        }
        nextBlockPointer = mvFlash32Rd(mvFlashInfo, nextBlockPointer & 
                                           FS_BLOCK_OFFSET_MASK);
    }
    if(numOfBytes == 0)
        return 0;
    return numOfBytes;
}

/*******************************************************************************
* mvFSGetEOFoffset - Gets the file's EOF offset in the flash memory.
*
* DESCRIPTION:
*      This function searches for the file’s EOF offset relatively to the 
*      begining of the last block offset . The EOF offset could be either the 9 
*      least significant bits of the next block pointer or , if data was 
*      appended to last block , the offset field within that block .  
*
* INPUT:
*      fileName - The file name to get its EOF offset .
*
* OUTPUT:
*      None.
*
* RETURN:
*      The file's EOF offset or FILE_NOT_FOUND.
*
*******************************************************************************/
unsigned int mvFSGetEOFoffset(char * fileName)
{
    unsigned int EOFoffset = 0,fileFAToffset,nextBlockOffset;
    
    fileFAToffset = mvFSFileFind(fileName);
    if(fileFAToffset == FS_FILE_NOT_FOUND)
        return FS_FILE_NOT_FOUND;
    nextBlockOffset = mvFlash32Rd(mvFlashInfo, fileFAToffset);
    if(nextBlockOffset == 0xffffffff)
        return 0;
    while(nextBlockOffset != 0xffffffff)
    {
        EOFoffset = nextBlockOffset;
        nextBlockOffset = mvFlash32Rd(mvFlashInfo, nextBlockOffset & 
                                          FS_BLOCK_OFFSET_MASK);
    }
    nextBlockOffset = EOFoffset & FS_BLOCK_OFFSET_MASK;
    if(mvFlash8Rd(mvFlashInfo, nextBlockOffset + FS_BLOCK_OFFSET_FLAG_OFFSET) ==
       FS_FULL_BLOCK)
    {
        EOFoffset = nextBlockOffset + (FS_BLOCK_SIZE - 
                                       FS_BLOCK_HEADER_SIZE - 1);
    }
    else
    {
        if(mvFlash16Rd(mvFlashInfo, nextBlockOffset + FS_BLOCK_OFFSET_OFFSET) 
           != 0xffff)
        {
            EOFoffset = EOFoffset + mvFlash16Rd(mvFlashInfo, nextBlockOffset + 
                                                   FS_BLOCK_OFFSET_OFFSET);
        }
        else
        {
            return EOFoffset; /* Not necessary - just for readability */  
        }
    }
    return EOFoffset;
}

/*******************************************************************************
* mvFSGetFreeBlockForWrite - Returns the offset of a free block (if found).
*
* DESCRIPTION:
*       This function searches for a free block  in more efficient way than  
*       ‘fileSysGetFreeBlock’  function by  saving the last free block offset 
*       that was found in the previous search and searches from that point. In 
*       case it couldnt find a free block , it uses the function 
*       ‘fileSysGetFreeBlock’.  
*      NOTE:
*       The file system's data base must be initialized by ‘fileSysInit’ 
*       function before using  this function .
*       If during the copying of the sector containing the most free dirty block
*       to the dynamic memory the system will loose power the file system will 
*       crash.  
*  
* INPUT:
*       None.
*
* OUTPUT:
*       None.
*
* RETURN:
*       Offset to a free block , FS_NO_FREE_BLOCKS if there are no more free
*       blocks or the file system was not initilized (use ‘fileSysInit’ before 
*       using this function in that case).
*
*******************************************************************************/
unsigned int mvFSGetFreeBlockForWrite()
{
    unsigned char       status;

    nextFreeBlock += FS_BLOCK_SIZE;
    if((nextFreeBlock == 0xffffffff) || 
       ((nextFreeBlock != 0xffffffff) && (nextFreeBlock >= mvFSTopOffset)))
    {
        nextFreeBlock = mvFSGetFreeBlock();
        return nextFreeBlock;
    }
    else
    {
        while(1)
        {
            status = mvFlash8Rd(mvFlashInfo, nextFreeBlock + 
                                     FS_BLOCK_STATUS_FLAG_OFFSET);
            if(status == FS_FREE_BLOCK) 
            {
                return nextFreeBlock;
            }
            nextFreeBlock += FS_BLOCK_SIZE;
            if(nextFreeBlock >= mvFSTopOffset || status == FS_FREE_DIRTY_BLOCK)
            {
                nextFreeBlock = mvFSGetFreeBlock();
                return nextFreeBlock;
            }
        }
    }
}

/*******************************************************************************
* mvFSGetFreeBlock - Returns the offset of a free block (if found).
*
* DESCRIPTION:
*       This function searches for a free block from the begining of the file 
*       system ( from ‘fileSysOffset’ parameter initialized by ‘fileSysInit’ 
*       function ) to the last sector and return its offset in case it found 
*       one .While the function perform the search for a free block within each
*       sector it also searches for the sector containing the most free dirty 
*       block in case a free block will not be found .If indeed a free block was
*       not found ,the function erases that sector ( the one with the most free 
*       dirty blocks ) after copying it to a temporary buffer in the dynamic 
*       memory (SDRAM) and  write back only the valid blocks to it .In that way
*       the function free blocks from the sector containing the most free dirty 
*       blocks and return the next free block within that sector.   
*      NOTE:
*       The file system's data base must be initialized by ‘fileSysInit’ 
*       function before using  this function .
*       If during the copying of the sector containing the most free dirty block
*       to the dynamic memory the system will loose power the file system will 
*       crash.
*   
* INPUT:
*       None.
*
* OUTPUT:
*       None.
*
* RETURN:
*       Offset to a free block , NO_FREE_BLOCKS if there are no more free blocks
*       or the file system was not initilized (use ‘fileSysInit’ before using 
*       this function in that case).
*
*******************************************************************************/
unsigned int mvFSGetFreeBlock()
{
    unsigned int freeBlockOffset,sectorTopOffset,i;
    unsigned int maxFreeDirtyBlocks = 0,sectorNumber,lastSector;
    unsigned int dirtyBlocksCounter = 0,dirtyBlocksSector,blockSize;
    unsigned int sectorBaseOffset,previousCounter,counterOffset;
    char * rmwBlockPointer,* tempBlockPointer;

    /* Check if the file system's data base was initialized */
    if(initializationFlag != FS_FILE_SYSTEM_INITIALIZED)
        return FS_NO_FREE_BLOCKS;
    freeBlockOffset     = mvFSOffset + FS_BLOCK_SIZE;
    sectorNumber        = mvFlashInWhichSec(mvFlashInfo, freeBlockOffset);
    lastSector          = mvFlashNumOfSecsGet(mvFlashInfo);
    dirtyBlocksSector   = sectorNumber;
    /* Search for a free block, while doing that I am searching for the sector
       with the most dirty free blocks in case I will not find a free block */
    for(i = sectorNumber ; i < lastSector ; i++)
    {
        sectorTopOffset = mvFlashSecOffsGet(mvFlashInfo, sectorNumber) +
                          mvFlashSecSizeGet(mvFlashInfo, sectorNumber) - 1;
        dirtyBlocksCounter = 0;
        while(freeBlockOffset < sectorTopOffset)
        {
            if(mvFlash8Rd(mvFlashInfo, freeBlockOffset + FS_BLOCK_STATUS_FLAG_OFFSET)
               == FS_FREE_BLOCK) 
            {
                return freeBlockOffset;
            }
            if(mvFlash8Rd(mvFlashInfo, freeBlockOffset + FS_BLOCK_STATUS_FLAG_OFFSET)
               == FS_FREE_DIRTY_BLOCK) dirtyBlocksCounter++;
            freeBlockOffset += FS_BLOCK_SIZE;  
        }
        if(dirtyBlocksCounter > maxFreeDirtyBlocks)
        {
            maxFreeDirtyBlocks = dirtyBlocksCounter;
            dirtyBlocksSector  = sectorNumber;
        }
        sectorNumber = mvFlashInWhichSec(mvFlashInfo, freeBlockOffset);
    }
    /* We are here since we have'nt found a free block */
    /* Lets check if we have a free dirty block */
    if(maxFreeDirtyBlocks > 0)
    {
        /* Now we need to perform the read modify write dispensing the free
           dirty blocks */
        blockSize           = mvFlashSecSizeGet(mvFlashInfo, dirtyBlocksSector);
        sectorBaseOffset    = mvFlashSecOffsGet(mvFlashInfo, dirtyBlocksSector); 
        rmwBlockPointer     = (char *)malloc(blockSize);
        if(rmwBlockPointer == NULL) return FS_NO_FREE_BLOCKS;
        /* Read the block */
        for(i = 0 ; i < blockSize ; i++)
        {
            rmwBlockPointer[i] = mvFlash8Rd(mvFlashInfo, sectorBaseOffset + i);
        }
        /* Erase the sector */
        if(mvFlash32Rd(mvFlashInfo, mvFSOffset + FS_ERSAING_COUNTER_FLAG_OFFSET) 
           == FS_ENABLE_ERASING_COUNTER)
        {
            counterOffset   = sectorBaseOffset + blockSize - FS_BLOCK_SIZE;
            previousCounter = mvFlash32Rd(mvFlashInfo, counterOffset);
            mvFlashSecErase(mvFlashInfo, dirtyBlocksSector);
            mvFlash32Wr(mvFlashInfo, counterOffset, previousCounter + 1);
            mvFlash8Wr(mvFlashInfo, counterOffset + FS_BLOCK_STATUS_FLAG_OFFSET,
                           FS_BLOCK_USED_FOR_ERASING_COUNTER);
            blockSize -= FS_BLOCK_SIZE;
        }
        else
        {
            mvFlashSecErase(mvFlashInfo, dirtyBlocksSector);
        }
        /* Write back only the valid blocks */
        for(i = 0 ; i < blockSize ; i += FS_BLOCK_SIZE)
        {
            if(rmwBlockPointer[i + FS_BLOCK_STATUS_FLAG_OFFSET] == 
               FS_BLOCK_IN_USE || rmwBlockPointer[i + 
               FS_BLOCK_STATUS_FLAG_OFFSET] == FS_BLOCK_IN_UPDATE || 
               rmwBlockPointer[i + FS_BLOCK_STATUS_FLAG_OFFSET] == FS_FAT_BLOCK)
            {
                tempBlockPointer = rmwBlockPointer + i;
                mvFlashBlockWr(mvFlashInfo, sectorBaseOffset + i, FS_BLOCK_SIZE,
                                  (MV_U8 *)tempBlockPointer);
            }
        }
        free(rmwBlockPointer);
        for(i = 0 ; i < blockSize ; i += FS_BLOCK_SIZE)
        {
            if(mvFlash8Rd(mvFlashInfo, sectorBaseOffset + i + 
                               FS_BLOCK_STATUS_FLAG_OFFSET)
               == FS_FREE_BLOCK)
                return (sectorBaseOffset + i);
        }
    }
    return FS_NO_FREE_BLOCKS;
}

/*******************************************************************************
* mvFSGetFreeEntry - Finds a free entry in the FAT and returns its offset.
*
* DESCRIPTION:
*       This function finds a free entry in the FAT and returns its offset. 
*       During the search for a free entry  the function also finds the sector 
*       containing the most free dirty entries . If the fuction cant find any 
*       free entry within the FAT it copies the sector containing the most free
*       dirty entries to the dynamic memory (SDRAM), erase that sector and write
*       back only the valid entries. In that way the function  free space for 
*       new entries and return the next free entry offset.     
*      NOTE:
*       The file system's data base must be initialized by ‘fileSysInit’ 
*       function before using  this function . 
*       If during the copying of the sector containing the most free dirty 
*       enrties to the dynamic memory the system will loose power the file
*       system will crash.   
* 
* INPUT:
*       None.
*
* OUTPUT:
*       None.
*
* RETURN:
*       Offset to a free entry , NO_FREE_ENTRIES if there are no more free 
*       entries in  the file system`s FAT or the file system was not initialized
*       (use ‘fileSysInit’ before using this function in that case). 
*
*******************************************************************************/
unsigned int mvFSGetFreeEntry()
{
    unsigned int nextBlockOffset,sectorNumber,i,previousSector;
    unsigned int dirtyEntriesCounter = 0,dirtyEntriesSector,blockSize,j;
    unsigned int maxFreeDirtyBlocks = 0;
    unsigned int sectorBaseOffset,previousCounter,counterOffset;
    char * rmwBlockPointer,* tempBlockPointer;

    /* Check if the file system's data base was initialized */
    if(initializationFlag != FS_FILE_SYSTEM_INITIALIZED)
        return FS_NO_FREE_ENTRIES;
    /* First FAT block */
    nextBlockOffset     = mvFSOffset;
    sectorNumber        = mvFlashInWhichSec(mvFlashInfo, nextBlockOffset);
    dirtyEntriesSector  = sectorNumber;
    previousSector      = sectorNumber;
    while(nextBlockOffset != 0xffffffff)
    {
        if(previousSector != sectorNumber)
        {
            if(dirtyEntriesSector > maxFreeDirtyBlocks)
            {
                maxFreeDirtyBlocks = dirtyEntriesSector;
                dirtyEntriesSector = sectorNumber;
            }
            previousSector      = sectorNumber;
            dirtyEntriesSector  = 0;
        }
        sectorNumber = mvFlashInWhichSec(mvFlashInfo, nextBlockOffset);
        for(i = 0 ; i < FS_BLOCK_SIZE ; i += FS_FAT_FILE_ENTRY_SIZE)
        {
            if(mvFlash8Rd(mvFlashInfo, nextBlockOffset +i+ FS_ENTRY_STATUS_FLAG_OFFSET) 
               == FS_FREE_ENTRY) 
                return (nextBlockOffset + i);
            if(mvFlash8Rd(mvFlashInfo, nextBlockOffset +i+ FS_ENTRY_STATUS_FLAG_OFFSET) 
               == FS_FREE_DIRTY_ENTRY)
                dirtyEntriesCounter++;
        }
        nextBlockOffset = mvFlash32Rd(mvFlashInfo, nextBlockOffset);
    }
    /* Now we need to perform the read modify write dispensing the free
       dirty entries and blocks */
    blockSize           = mvFlashSecSizeGet(mvFlashInfo, dirtyEntriesSector);
    sectorBaseOffset    = mvFlashSecOffsGet(mvFlashInfo, dirtyEntriesSector); 
    rmwBlockPointer     = (char *)malloc(blockSize);
    if(rmwBlockPointer == NULL) return FS_NO_FREE_ENTRIES;
    /* Read the block */
    for(i = 0 ; i < blockSize ; i++)
    {
        rmwBlockPointer[i] = mvFlash8Rd(mvFlashInfo, sectorBaseOffset + i);
    }
    /* Erase the sector */
    if(mvFlash32Rd(mvFlashInfo, mvFSOffset + 20) == FS_ENABLE_ERASING_COUNTER)
    {
        counterOffset   = sectorBaseOffset + blockSize - FS_BLOCK_SIZE;
        previousCounter = mvFlash32Rd(mvFlashInfo, counterOffset);
        mvFlashSecErase(mvFlashInfo, dirtyEntriesSector);
        mvFlash32Wr(mvFlashInfo, counterOffset,previousCounter + 1);
        mvFlash8Wr(mvFlashInfo, counterOffset + FS_BLOCK_STATUS_FLAG_OFFSET,
                       FS_BLOCK_USED_FOR_ERASING_COUNTER);
        blockSize -= FS_BLOCK_SIZE;
    }
    else
    {
        mvFlashSecErase(mvFlashInfo, dirtyEntriesSector);
    }
    /* Write back only the valid blocks */
    for(i = 0 ; i < blockSize ; i += FS_BLOCK_SIZE)
    {
        if(rmwBlockPointer[i + FS_BLOCK_STATUS_FLAG_OFFSET] == 
           FS_BLOCK_IN_USE || rmwBlockPointer[i + FS_BLOCK_STATUS_FLAG_OFFSET] 
           == FS_BLOCK_IN_UPDATE)
        {
            tempBlockPointer = rmwBlockPointer + i;
            mvFlashBlockWr(mvFlashInfo, sectorBaseOffset + i, FS_BLOCK_SIZE,
                              (MV_U8 *)tempBlockPointer);
        }
        if(rmwBlockPointer[i + FS_BLOCK_STATUS_FLAG_OFFSET] == FS_FAT_BLOCK)
        {
            for(j = FS_FAT_FILE_ENTRY_SIZE ; j < FS_BLOCK_SIZE ;
                j+= FS_FAT_FILE_ENTRY_SIZE)
            {
                if(rmwBlockPointer[j + FS_ENTRY_STATUS_FLAG_OFFSET] 
                   == FS_ENTRY_IN_USE ||
                   rmwBlockPointer[j + FS_ENTRY_STATUS_FLAG_OFFSET] 
                   == FS_ENTRY_IN_UPDATE)
                {
                    tempBlockPointer = rmwBlockPointer + j;
                    mvFlashBlockWr(mvFlashInfo, sectorBaseOffset + j,
                                      FS_FAT_FILE_ENTRY_SIZE, (MV_U8 *)tempBlockPointer);
                }
            }
        }
    }
    free(rmwBlockPointer);
    for(i = 0 ; i < blockSize ; i += FS_BLOCK_SIZE)
    {
        if(mvFlash32Rd(mvFlashInfo, sectorBaseOffset + i + FS_BLOCK_STATUS_FLAG_OFFSET)
           == FS_FAT_BLOCK)
        {
            for(j = i + FS_FAT_FILE_ENTRY_SIZE ; j < (i + FS_BLOCK_SIZE) ;
                j+= FS_FAT_FILE_ENTRY_SIZE)
            {
                if(mvFlash32Rd(mvFlashInfo, sectorBaseOffset +j+
                                  FS_ENTRY_STATUS_FLAG_OFFSET) == FS_FREE_ENTRY)
                return (sectorBaseOffset + j);
            }
        }
    }
    return FS_NO_FREE_ENTRIES;
}

/*******************************************************************************
* mvFSWriteFileNameToFATentry - Write the file name to an entry in the FAT .
*
* DESCRIPTION:
*      This function writes the file name to an entry in the FAT.The function 
*      extarcts the file name and extention and write it to the entry at 
*      ‘entryOffset’ within the FAT.
*
* INPUT:
*      entryOffset - Pointer to the desired entry to write the file name at.
*      fileName - The file name and extention.
*
* OUTPUT:
*      None.
*
* RETURN:
*      0 for invalid file name, 1 for success.
*
*******************************************************************************/
unsigned int mvFSWriteFileNameToFATentry(unsigned int entryOffset,
                                            char * fileName)
{
    char   name[FS_FILE_NAME_LENGTH];
    char   ext[FS_FILE_EXT_LENGTH];
    unsigned int    index,extIndex;

    /* Extract the name */
    for(index = 0 ; index < FS_FILE_NAME_LENGTH ; index++)
    {
        if(fileName[index] == '.' || fileName[index] == '\0')
        {
            break;
        }
        else
        {
            name[index] = fileName[index];
        }
    }
    /* Write the name */
    if(index != 0)
        mvFlashBlockWr(mvFlashInfo, entryOffset + FS_FILE_NAME_OFFSET, index, (MV_U8 *)name);
    if(index == 0 && fileName[index] == '\0') /* Not a valid name */
        return 0;
    if(fileName[index] == '\0') /* No extension */
        return 1;
    /* Extract the extension */
    index++; /* Skip the dot */
    for(extIndex = 0 ; extIndex < FS_FILE_EXT_LENGTH ; extIndex++)
    {
        if(fileName[index + extIndex] == '\0')
        {
		// ronen fix a bug ---> extIndex++;
            break;
        }
        else
        {
            ext[extIndex] = fileName[index + extIndex];
        }
    }
    /* Write the extension */
    if(extIndex != 0)
        mvFlashBlockWr(mvFlashInfo, entryOffset + FS_FILE_EXT_OFFSET, extIndex, (MV_U8 *)ext);
    return 1;
}

/*******************************************************************************
* mvFSReadFileNameFromFATentry - Read the file name from an entry in the FAT.
*
* DESCRIPTION:
*       This function reads the file name from an entry in the FAT. If the given
*       entry is not used by any file , the function will return 0 to remark a 
*       failier.
*
* INPUT:
*       entryOffset - Pointer to the desired entry to read the file name from.
*       fileName - pointer to an empty array to be filled by this functoin with
*                  the file name and extention. 
* OUTPUT:
*       None.
*
* RETURN:
*       0 for failure, 1 for success.
*
*
*******************************************************************************/
unsigned int mvFSReadFileNameFromFATentry(unsigned int entryOffset,
                                             char * fileName)
{
    unsigned int    index = 0,offset;
    unsigned char   c;

    if( (mvFlash8Rd(mvFlashInfo, entryOffset + FS_ENTRY_STATUS_FLAG_OFFSET) 
         == FS_ENTRY_IN_USE) || 
        (mvFlash8Rd(mvFlashInfo, entryOffset + FS_ENTRY_STATUS_FLAG_OFFSET) 
        == FS_ENTRY_IN_UPDATE) )
    {
        for(offset = 0 ; offset < FS_FILE_NAME_LENGTH ; offset++)
        {
            c = mvFlash8Rd(mvFlashInfo, entryOffset + FS_FILE_NAME_OFFSET + offset);
            if(c == 0xff) 
                break;
            fileName[index++] = c;
        }
        if(mvFlash8Rd(mvFlashInfo, entryOffset + FS_FILE_EXT_OFFSET) != 0xff)
        {
            fileName[index++] = '.';
            for(offset = 0 ; offset < FS_FILE_EXT_LENGTH ; offset++)
            {
                c = mvFlash8Rd(mvFlashInfo, entryOffset + FS_FILE_EXT_OFFSET + offset);
                if(c == 0xff) break;
                fileName[index++] = c;
            }
        }
        fileName[index++] = '\0';
        return 1;
    }
    return 0;
}

/*******************************************************************************
* mvFSWrite - Writes a block of data to an opened file.
*
* DESCRIPTION:
*       This function writes a block of data to a file opened by all modes 
*       except mode “r” indicating the file is a read only file. The function
*       will append the data to the last block ( if the offset flag is not set 
*       yet ) or to a new block if this is a new file , the function also takes
*       care of updating the EOF pointer , updating the file's entry and data 
*       blocks status flags. During the write attempt  , the function will
*       return 0 value  in the following cases:
*       - if the given file handler is invalid  that is , grater or equal to 
*         MAX_NUM_OF_ENTRIES (defined in mvFS.h).
*       - if the entry of the file handler holds the status FREE_ENTRY.
*       - if the ‘numberOfBytes’ parameter is 0.
*       - if  this is the first write attempt to the file ( new file ) , the 
*         function will allocate a new block   for the data , in this case a 
*         failier can occur resulting from lack of free blocks. In all other 
*         cases the function will return the number of bytes actually written.
*         Note that the number of bytes actually written returned by this 
*         function may not  be equal to the ‘numberOfBytes’ parameter , in that
*         case consider the write action as a fatal error and do not try to 
*         re-write the data.    
*
* INPUT:
*       fileHandler - The file handler of the file to be written.
*       numOfBytes - The number of bytes to be written from the data array.
*       block - The data array to write to the file.
*
* OUTPUT:
*       None.
*
* RETURN:
*       Returns the number of bytes actually written, if the number of
*       bytes actually written is not equal to the numberOfBytes consider it as 
*       a fatal error, do not try to re-write the data.
*
*******************************************************************************/
unsigned int mvFSWrite(FS_FILE_HANDLER fileHandler,
                          unsigned int numberOfBytes,char * block)
{
    unsigned int remainingBytes,nextBlockPointer,newBlockPointer,blockSize;
    unsigned int freeBytesInBlock,writtenBytes,blockIndex = 0;
    unsigned int totalWrittenBytes = 0,numberOfBlocks,i;

    blockSize = FS_BLOCK_SIZE - FS_BLOCK_HEADER_SIZE;
    if((fileHandler >= FS_MAX_NUM_OF_ENTRIES) ||
       (mvFSEntries[fileHandler].statusFlag == FS_FREE_ENTRY) ||
       numberOfBytes == 0)
        return 0;
    if(mvFSEntries[fileHandler].mode[0] == 'r' &&
       mvFSEntries[fileHandler].mode[1] == '\0')
        return 0; /* File was opened as read only */
    nextBlockPointer = mvFlash32Rd(mvFlashInfo, mvFSEntries[fileHandler].pointerToFAT);
    remainingBytes = numberOfBytes;
    if(nextBlockPointer == 0xffffffff)
    {
        /* New file, no blocks allocated yet */
        newBlockPointer = mvFSGetFreeBlock();
        mvFSEntries[fileHandler].blockPointer = newBlockPointer;
        if(newBlockPointer == FS_NO_FREE_BLOCKS)
            return 0;
        /* Update the entry's status flag and next block pointer */
        mvFlash32Wr(mvFlashInfo, mvFSEntries[fileHandler].pointerToFAT,
                       newBlockPointer | FS_BLOCK_OFFSET);
        mvFlash8Wr(mvFlashInfo, mvFSEntries[fileHandler].pointerToFAT + 
                       FS_ENTRY_STATUS_FLAG_OFFSET, FS_ENTRY_IN_USE);
        /* Start writing the data */
        if(numberOfBytes <= blockSize)
        {
            /* Only one block needed */
            /* Update the blocks status flag */
            mvFlash8Wr(mvFlashInfo, newBlockPointer + FS_BLOCK_STATUS_FLAG_OFFSET,
                           FS_BLOCK_IN_UPDATE);
            /* Update the EOFpointer */
            mvFSEntries[fileHandler].EOFpointer = newBlockPointer
                + numberOfBytes + FS_BLOCK_HEADER_SIZE - 1;
            /* Update the File pointer */
            mvFSEntries[fileHandler].filePointer = 
                mvFSEntries[fileHandler].EOFpointer;
            /* Write the data */
            return 
                mvFlashBlockWr(mvFlashInfo, newBlockPointer + FS_BLOCK_HEADER_SIZE,
                                  numberOfBytes, (MV_U8 *)block);
        }
        else /* More than one block is needed */
        {
            /* I will take care of the first block in here, the rest 
               will be handled later in the fuction */ 
            /* Update the block's status flag */
            mvFlash8Wr(mvFlashInfo, newBlockPointer + FS_BLOCK_STATUS_FLAG_OFFSET,
                           FS_BLOCK_IN_USE);/* Fully exploited block */
            /* Write the data */
            writtenBytes = mvFlashBlockWr(mvFlashInfo, newBlockPointer + 
                                          FS_BLOCK_HEADER_SIZE, blockSize, (MV_U8 *)block);
            /* Update the EOFpointer */
            mvFSEntries[fileHandler].EOFpointer = newBlockPointer
                + writtenBytes + FS_BLOCK_HEADER_SIZE - 1;
            /* Update the File pointer */
            mvFSEntries[fileHandler].filePointer = 
                mvFSEntries[fileHandler].EOFpointer;
            if(writtenBytes != blockSize)
                return writtenBytes; /* Write operation failed */
            totalWrittenBytes += writtenBytes;
            remainingBytes = numberOfBytes - blockSize;
            nextBlockPointer = newBlockPointer;
            blockIndex += blockSize;
        }
    }
    /* Search for the last block */
    while(mvFlash32Rd(mvFlashInfo, nextBlockPointer & FS_BLOCK_OFFSET_MASK) != 
                                                                     0xffffffff)
    {
        nextBlockPointer = mvFlash32Rd(mvFlashInfo, nextBlockPointer & 
                                           FS_BLOCK_OFFSET_MASK);
        mvFSEntries[fileHandler].blockPointer = nextBlockPointer;
    }
    freeBytesInBlock = FS_BLOCK_SIZE - 1 -  
                       (mvFSEntries[fileHandler].EOFpointer & FS_BLOCK_OFFSET);
    if(mvFlash8Rd(mvFlashInfo, (nextBlockPointer & FS_BLOCK_OFFSET_MASK)
                     + FS_BLOCK_STATUS_FLAG_OFFSET) == FS_BLOCK_IN_USE)
    {
        /* We can use the offset flag to append data, or the FS_FULL_BLOCK
           flag in case the block will be fully exploited */
        if(freeBytesInBlock != 0) 
        {
            if((remainingBytes < freeBytesInBlock) &&
                mvFlash16Rd(mvFlashInfo, (nextBlockPointer & FS_BLOCK_OFFSET_MASK)
                                + FS_BLOCK_OFFSET_OFFSET) == 0xffff)
            {
                writtenBytes = mvFlashBlockWr(mvFlashInfo, 
                                              mvFSEntries[fileHandler].EOFpointer + 1,
                                              remainingBytes, (MV_U8 *)&block[blockIndex]);
                mvFSEntries[fileHandler].EOFpointer += writtenBytes;
                /* Update the File pointer */
                mvFSEntries[fileHandler].filePointer = 
                    mvFSEntries[fileHandler].EOFpointer;
                if(remainingBytes == freeBytesInBlock)
                {
                    mvFlash8Wr(mvFlashInfo, (nextBlockPointer & FS_BLOCK_OFFSET_MASK)
                                   + FS_BLOCK_OFFSET_FLAG_OFFSET, FS_FULL_BLOCK);
                }
                /* else the OFFSET will be updated by mvFSClose */
                return writtenBytes;
            }
            else
            {
                if(remainingBytes >= freeBytesInBlock)
                {
                    writtenBytes = mvFlashBlockWr(mvFlashInfo,
                                                  mvFSEntries[fileHandler].EOFpointer + 1,
                                                  freeBytesInBlock, (MV_U8 *)&block[blockIndex]);
                    mvFSEntries[fileHandler].EOFpointer += freeBytesInBlock;
                    /* Update the File pointer */
                    mvFSEntries[fileHandler].filePointer = 
                        mvFSEntries[fileHandler].EOFpointer;
                    totalWrittenBytes += writtenBytes;
                    if(writtenBytes != freeBytesInBlock) /* error */
                        return totalWrittenBytes;
                    mvFlash8Wr(mvFlashInfo, (nextBlockPointer & FS_BLOCK_OFFSET_MASK)
                                   + FS_BLOCK_OFFSET_FLAG_OFFSET, FS_FULL_BLOCK);
                    remainingBytes -= freeBytesInBlock;
                    blockIndex += freeBytesInBlock;
                }
            }
        }
    }
    else /* The block is not sealed yet, we can simply append the data */
    {
        if(remainingBytes <= freeBytesInBlock)
        {
            writtenBytes = mvFlashBlockWr(mvFlashInfo, 
                                          mvFSEntries[fileHandler].EOFpointer + 1,
                                          remainingBytes, (MV_U8 *)&block[blockIndex]);
            mvFSEntries[fileHandler].EOFpointer += writtenBytes;
            /* Update the File pointer */
            mvFSEntries[fileHandler].filePointer = 
                mvFSEntries[fileHandler].EOFpointer;
            if(remainingBytes == freeBytesInBlock)
            {
                mvFlash8Wr(mvFlashInfo, (nextBlockPointer & FS_BLOCK_OFFSET_MASK)
                               + FS_BLOCK_STATUS_FLAG_OFFSET, FS_BLOCK_IN_USE);
            }
            return writtenBytes; 
        }
        else /* More blocks are needed */
        {
            /* Fill the current block */
            writtenBytes = mvFlashBlockWr(mvFlashInfo,
                                          mvFSEntries[fileHandler].EOFpointer + 1,
                                          freeBytesInBlock, (MV_U8 *)&block[blockIndex]);
            mvFSEntries[fileHandler].EOFpointer += writtenBytes;
            /* Update the File pointer */
            mvFSEntries[fileHandler].filePointer = 
                mvFSEntries[fileHandler].EOFpointer;
            totalWrittenBytes += writtenBytes;
            if(writtenBytes != freeBytesInBlock) /* error */
                return totalWrittenBytes;
            mvFlash8Wr(mvFlashInfo, (nextBlockPointer & FS_BLOCK_OFFSET_MASK)
                           + FS_BLOCK_STATUS_FLAG_OFFSET, FS_BLOCK_IN_USE);
            remainingBytes -= freeBytesInBlock;
            blockIndex += freeBytesInBlock;
        }
    }
    numberOfBlocks = remainingBytes / blockSize;
    for(i = 0 ; i < numberOfBlocks ; i++)
    {
        /* All the blocks in this loop will be fully exploited */
        newBlockPointer = mvFSGetFreeBlockForWrite();
        if(newBlockPointer == FS_NO_FREE_BLOCKS)
            return totalWrittenBytes;
        /* Update the nextBlockPointer */
        mvFlash32Wr(mvFlashInfo, nextBlockPointer & FS_BLOCK_OFFSET_MASK,
                       newBlockPointer | FS_BLOCK_OFFSET);
        /* Update the status flag */
        mvFlash8Wr(mvFlashInfo, (newBlockPointer & FS_BLOCK_OFFSET_MASK) + 
                       FS_BLOCK_STATUS_FLAG_OFFSET, FS_BLOCK_IN_USE);
        /* Write The Data */
        writtenBytes = mvFlashBlockWr(mvFlashInfo, (newBlockPointer & 
                                      FS_BLOCK_OFFSET_MASK)
                                      + FS_BLOCK_HEADER_SIZE, blockSize,
                                      (MV_U8 *)&block[blockIndex]);
        blockIndex += blockSize;
        totalWrittenBytes += writtenBytes;
        /* Make sure the EOFpointer is set for error handling */
        mvFSEntries[fileHandler].EOFpointer = (newBlockPointer 
            & FS_BLOCK_OFFSET_MASK) + FS_BLOCK_HEADER_SIZE + writtenBytes - 1;
        /* Update the File pointer */
        mvFSEntries[fileHandler].filePointer = 
            mvFSEntries[fileHandler].EOFpointer;
        if(writtenBytes != blockSize)/* Error */
            return totalWrittenBytes;
        remainingBytes -= blockSize;
        nextBlockPointer = newBlockPointer;
    }
    if(remainingBytes == 0)
        return totalWrittenBytes;
    /* Allocate a new block for the remaining bytes */
    newBlockPointer = mvFSGetFreeBlockForWrite();
    if(newBlockPointer == FS_NO_FREE_BLOCKS)
        return totalWrittenBytes;
    /* Update the nextBlockPointer */
    mvFlash32Wr(mvFlashInfo, nextBlockPointer & FS_BLOCK_OFFSET_MASK,
                   newBlockPointer | FS_BLOCK_OFFSET);
    /* Update the status flag */
    mvFlash8Wr(mvFlashInfo, (newBlockPointer & FS_BLOCK_OFFSET_MASK) + 
                   FS_BLOCK_STATUS_FLAG_OFFSET, FS_BLOCK_IN_UPDATE);
    /* Write The Data */
    writtenBytes = mvFlashBlockWr(mvFlashInfo, (newBlockPointer & FS_BLOCK_OFFSET_MASK)
                                   + FS_BLOCK_HEADER_SIZE, remainingBytes,
                                   (MV_U8 *)&block[blockIndex]);
    mvFSEntries[fileHandler].EOFpointer = (newBlockPointer 
        & FS_BLOCK_OFFSET_MASK) + FS_BLOCK_HEADER_SIZE + writtenBytes - 1;
    /* Update the File pointer */
    mvFSEntries[fileHandler].filePointer = 
        mvFSEntries[fileHandler].EOFpointer;
    totalWrittenBytes += writtenBytes;
    return totalWrittenBytes;
}

/*******************************************************************************
* mvFSGetNumOfValidBytes - Return the number of valid bytes from 
*                          blockOffsetPointer to the end of the block.
*
* DESCRIPTION:
*       None.
*
* INPUT:
*       blockPointer - The offset of the block to read its valid bytes.
*       blockOffsetPointer - The offset within the block to read the valid 
*                            bytes from.
*       EOFpointer - The EOF offset.
* OUTPUT:
*       None.
*
* RETURN:
*       Number of valid bytes.
*
*******************************************************************************/
unsigned int mvFSGetNumOfValidBytes(unsigned int blockPointer,
                                       unsigned int blockOffsetPointer,
                                       unsigned int EOFpointer)
{
    if((blockOffsetPointer & FS_BLOCK_OFFSET_MASK) ==
       (EOFpointer & FS_BLOCK_OFFSET_MASK))
    {
        /* This is the last block */
        if(EOFpointer > blockOffsetPointer)
            return (EOFpointer - blockOffsetPointer);
        else
            return 0;
    }
    else
    {
        if((mvFlash8Rd(mvFlashInfo, (blockOffsetPointer & FS_BLOCK_OFFSET_MASK) + 
                                 FS_BLOCK_OFFSET_FLAG_OFFSET) == FS_FULL_BLOCK) 
           || ((blockPointer & FS_BLOCK_OFFSET) == FS_BLOCK_OFFSET))
        {
            return (FS_BLOCK_SIZE - 1 - (blockOffsetPointer & FS_BLOCK_OFFSET));
        }
        else /* offset field is used */
        {
            return ((blockPointer & FS_BLOCK_OFFSET) + mvFlash16Rd(mvFlashInfo,
                (blockPointer & FS_BLOCK_OFFSET_MASK) + FS_BLOCK_OFFSET_OFFSET) 
                - (blockOffsetPointer & FS_BLOCK_OFFSET));
        }
    }
}

/*******************************************************************************
* mvFSRead - Read a block of data into block from a file.
*
* DESCRIPTION:
*       This function reads a block of data from a file opened by all modes 
*       except “ w ” mode ,  into a given block  (‘block’ parameter ) .While the
*       file is opened , the function will start reading the file from the 
*       begining if its the first read attempt after opening the file or from 
*       the last point within the block  of data were the previous read action 
*       ended .The read action will not exceed the EOF pointer if the 
*       ‘numberOfBytes’ parameter is larger than the whole file or the number of
*       bytes left from the previous read action . During the read attempt , the
*       function will return an error code or 0 in the following cases:
*
*       - if the given file handler is invalid  that is , grater or equal to 
*         MAX_NUM_OF_ENTRIES   (defined in fileSys.h).
*       - if the entry of the file handler within the dynamic database holds the
*         status FREE_ENTRY .
*       - if the mode of the opened file is write only - “ w” .
*       - if the EOF pointer is equal to the file pointer that indicates the 
*         curruent position within the file.
*       - if the ‘nextBlockPointer’ field within the entry in the FAT is equal 
*         to 0xFFFFFFFF indicating the file is empty.  
*         In a successfull read operation the function will return the number of
*         bytes read , if this value is not equal to the ‘numberOfBytes’ 
*         parameter delivered  to the function , the read action reached EOF.   
*
* INPUT:
*       fileHandler - The file handler of the file to be read.
*       numOfBytes - The number of bytes to be read from the file to the data 
*                    array.
*       block - The data array to hold the read data from the file.
*
* OUTPUT:
*       block filled with data from the file.
*
* RETURN:
*       The number of bytes actually read . If this value is not equal to the 
*       ‘numberOfBytes’ parameter delivered  to the function , the read action
*       reached EOF.The function can also return the following values in case of
*       an error :
*       - FILE_READ_ERROR if the fileHandler is larger or equal to 
*         MAX_NUM_OF_ENTRIES defined in fileSys.h or if the status flag of the 
*         file entry in the dynamic database holds the value FREE_ENTRY.
*       - 0 if the file was opened as write only , the EOF pointer is equal to 
*         the file pointer indicating the current loaction within the file or 
*         the ‘nextBlockPointer’ field within the file entry in the flash is 
*         equal to 0xFFFFFFFF (empty file).  
*
*******************************************************************************/
unsigned int mvFSRead(FS_FILE_HANDLER fileHandler,
                         unsigned int numberOfBytes,char * block)
{
    unsigned int blockIndex = 0,nextBlockPointer,EOFpointer;
    unsigned int remainingBytes,freeBytesInBlock,bytesRead;
    unsigned int totalBytesRead = 0,blockSize;

    if((fileHandler >= FS_MAX_NUM_OF_ENTRIES) ||
       (mvFSEntries[fileHandler].statusFlag == FS_FREE_ENTRY) ||
       numberOfBytes == 0)
        return FS_FILE_READ_ERROR;
    if(mvFSEntries[fileHandler].mode[0] == 'w' &&
       mvFSEntries[fileHandler].mode[1] == '\0')
        return 0; /* File was opened as write only */
    if(mvFSEntries[fileHandler].EOFpointer ==
       mvFSEntries[fileHandler].filePointer)
        return 0;
    blockSize   = FS_BLOCK_SIZE - FS_BLOCK_HEADER_SIZE;
    EOFpointer  = mvFSEntries[fileHandler].EOFpointer;
    remainingBytes =  numberOfBytes;
    nextBlockPointer = mvFlash32Rd(mvFlashInfo, mvFSEntries[fileHandler].pointerToFAT);
    if(nextBlockPointer == 0xffffffff)
        return 0; /* An empty file */
    if(mvFSEntries[fileHandler].filePointer == 0)
    {
        /* Read one byte and set the filePointer to the first block */
        block[blockIndex] = mvFlash8Rd(mvFlashInfo, (nextBlockPointer 
                                & FS_BLOCK_OFFSET_MASK) + FS_BLOCK_HEADER_SIZE);
        remainingBytes--;
        totalBytesRead++;
        mvFSEntries[fileHandler].filePointer = (nextBlockPointer 
                                                   & FS_BLOCK_OFFSET_MASK) +
                                                    FS_BLOCK_HEADER_SIZE;
        blockIndex++;
        mvFSEntries[fileHandler].blockPointer= nextBlockPointer;
    }
    else
    {
        /* update the block's file pointer */
        nextBlockPointer = mvFSEntries[fileHandler].blockPointer;
    }
    while(1)
    {
        if((mvFSEntries[fileHandler].filePointer & FS_BLOCK_OFFSET_MASK) !=
           (nextBlockPointer & FS_BLOCK_OFFSET_MASK))
        {   /* Forward the file pointer to the current block */
            block[blockIndex] = mvFlash8Rd(mvFlashInfo, (nextBlockPointer 
                                    & FS_BLOCK_OFFSET_MASK) + 
                                                FS_BLOCK_HEADER_SIZE);
            remainingBytes--;
            totalBytesRead++;
            mvFSEntries[fileHandler].filePointer = (nextBlockPointer 
                                                       & FS_BLOCK_OFFSET_MASK) +
                                                        FS_BLOCK_HEADER_SIZE;
            blockIndex++;
        }
        freeBytesInBlock = mvFSGetNumOfValidBytes(nextBlockPointer,
                                mvFSEntries[fileHandler].filePointer,
                                                     EOFpointer);
        if(freeBytesInBlock <= remainingBytes)
        {
            bytesRead = mvFlashBlockRd(mvFlashInfo, mvFSEntries[fileHandler].filePointer
                                       +1,freeBytesInBlock, (MV_U8 *)&block[blockIndex]);
            remainingBytes -= bytesRead;
            totalBytesRead += bytesRead;                      
            mvFSEntries[fileHandler].filePointer += bytesRead;
            blockIndex += bytesRead;
            if(bytesRead != freeBytesInBlock) /* Fatal error */
            {
                return totalBytesRead;
            }
        }
        else
        {
            bytesRead = mvFlashBlockRd(mvFlashInfo, mvFSEntries[fileHandler].filePointer
                                       +1,remainingBytes,(MV_U8 *)&block[blockIndex]);
            totalBytesRead += bytesRead;                      
            mvFSEntries[fileHandler].filePointer += bytesRead;
            blockIndex += bytesRead;
            if(bytesRead != remainingBytes) /* Fatal error */
            {
                return totalBytesRead;
            }
            remainingBytes -= bytesRead;
        }
        if(((nextBlockPointer & FS_BLOCK_OFFSET_MASK) == 
           (EOFpointer & FS_BLOCK_OFFSET_MASK)) || remainingBytes == 0)
        {   /* Last valid block */
            return totalBytesRead;
        }
        nextBlockPointer = mvFlash32Rd(mvFlashInfo, nextBlockPointer & 
                                           FS_BLOCK_OFFSET_MASK);
        mvFSEntries[fileHandler].blockPointer = nextBlockPointer;
    }
    return totalBytesRead;
}

/*******************************************************************************
* mvFSRewind - Set the file pointer the beginning of the file.
*
* DESCRIPTION:
*       This function set the file pointer the beginning of the given file.
*
* INPUT:
*       fileHandler - The file handler of the file to rewind its pointer.
*
* OUTPUT:
*       None.
*
* RETURN:
*       0 on succes otherwise 0xffffffff.
*
*******************************************************************************/
unsigned int mvFSRewind(FS_FILE_HANDLER fileHandler)
{
    if((fileHandler >= FS_MAX_NUM_OF_ENTRIES) ||
       (mvFSEntries[fileHandler].statusFlag == FS_FREE_ENTRY))
        return 0xffffffff;
    mvFSEntries[fileHandler].filePointer = 0;
    return 0;
}

/*******************************************************************************
* mvFSSeek - Move the file pointer from the origin specified.
*
* DESCRIPTION:
*       This function move’s the file-pointer associated with ‘filehandler’ to a
*       new location that is ‘offset’ bytes from ‘origin’.The argument ‘origin’ 
*       must be one of the following constants , defined in mvFS.h :
*       
*        SEEK_CUR - Current position of file pointer.
*        SEEK_END - End of file.
*        SEEK_SET - Beginning of file. 
*
*       The ‘offset’ argument indicates the number of bytes to move relatively 
*       to ‘origin’.
*
* INPUT:
*       fileHandler - The file-handler of the file to reposition its 
*                     file-pointer.
*       Offset - The number of bytes to move the file pointer from ‘origin’.
*       Origin - The relative point within the file to move the file-pointer 
*                ‘offset’ bytes  from it.Must be one of the three values 
*                 described above.      
*
* OUTPUT:
*       File-pointer located on its new place ( on success ).
*
* RETURN:
*       true on succes, 0xffffffff otherwise.
*
*******************************************************************************/
unsigned int mvFSSeek(FS_FILE_HANDLER fileHandler,int offset,int origin)
{
    unsigned int    nextBlockPointer,fileSize;
    unsigned int    validBytes,temp;
    int             tempSize = 0,tempOffset = 0;

    if((fileHandler >= FS_MAX_NUM_OF_ENTRIES) ||
       (mvFSEntries[fileHandler].statusFlag == FS_FREE_ENTRY))
        return 0xffffffff;
    fileSize = mvFSFileSize(fileHandler);
    if(fileSize == 0xffffffff || fileSize == 0)
        return 0xffffffff;
    nextBlockPointer = mvFlash32Rd(mvFlashInfo, mvFSEntries[fileHandler].pointerToFAT);
    switch(origin)
    {
    case FS_SEEK_END:
        if(offset <= 0)
            return 1/*true*/;
        if((unsigned int)offset >= fileSize)
        {
            mvFSEntries[fileHandler].filePointer = 0;
            return 1/*true*/;
        }
        offset = fileSize - offset;
        break;
    case FS_SEEK_SET:
        if(offset <= 0)
        {
            mvFSEntries[fileHandler].filePointer = 0;
            return 1/*true*/;
        }
        if((unsigned int)offset >= fileSize)
        {
            mvFSEntries[fileHandler].filePointer =
                mvFSEntries[fileHandler].EOFpointer;
            return 1/*true*/;
        }
        while(tempOffset < offset)
        {
            validBytes = mvFSGetNumOfValidBytes(nextBlockPointer,
                            (nextBlockPointer & FS_BLOCK_OFFSET_MASK) 
                             + FS_BLOCK_HEADER_SIZE,
                             mvFSEntries[fileHandler].EOFpointer);
            if((tempOffset + (int)validBytes) >= offset)
                break;
            tempOffset += validBytes;
            nextBlockPointer = mvFlash32Rd(mvFlashInfo, nextBlockPointer 
                                             & FS_BLOCK_OFFSET_MASK);
            mvFSEntries[fileHandler].blockPointer = nextBlockPointer;
        }
        mvFSEntries[fileHandler].filePointer = 
            (nextBlockPointer & FS_BLOCK_OFFSET_MASK) +
            (offset - tempOffset) + FS_BLOCK_HEADER_SIZE - 1;
        return 1/*true*/;
    case FS_SEEK_CUR: /* In this case offset can be negative */
        if(offset == 0)
            return 1/*true*/;
        /* tempSize = Size in bytes up-to filePointer */
        temp = nextBlockPointer;
        nextBlockPointer = 
            mvFlash32Rd(mvFlashInfo, mvFSEntries[fileHandler].pointerToFAT);
        mvFSEntries[fileHandler].blockPointer = nextBlockPointer;
        while(nextBlockPointer != 0xffffffff)
        {
            if(mvFSEntries[fileHandler].filePointer == 0)
                break;
            if((mvFSEntries[fileHandler].filePointer & FS_BLOCK_OFFSET_MASK) ==
               (nextBlockPointer & FS_BLOCK_OFFSET_MASK)) /* Last block for us*/
            {
                tempSize += (mvFSEntries[fileHandler].filePointer 
                               & FS_BLOCK_OFFSET) - FS_BLOCK_HEADER_SIZE + 1;
                break;
            }
            if(((nextBlockPointer & FS_BLOCK_OFFSET) + 1) != FS_BLOCK_SIZE)
            {
                if(mvFlash8Rd(mvFlashInfo, (nextBlockPointer & FS_BLOCK_OFFSET_MASK) + 
                                 FS_BLOCK_OFFSET_FLAG_OFFSET) == FS_FULL_BLOCK)
                {
                    tempSize += (FS_BLOCK_SIZE - FS_BLOCK_HEADER_SIZE);
                }
                else
                {
                    if(mvFlash16Rd(mvFlashInfo, (nextBlockPointer & 
                                         FS_BLOCK_OFFSET_MASK) +
                                      FS_BLOCK_OFFSET_OFFSET) != 0xffff)
                    {
                        tempSize += ((nextBlockPointer & FS_BLOCK_OFFSET) +
                            mvFlash16Rd(mvFlashInfo, (nextBlockPointer & 
                                           FS_BLOCK_OFFSET_MASK) + 
                                           FS_BLOCK_OFFSET_OFFSET) - 
                            FS_BLOCK_HEADER_SIZE + 1);
                    }
                    else
                    {
                        tempSize += ((nextBlockPointer & FS_BLOCK_OFFSET) - 
                                       FS_BLOCK_HEADER_SIZE + 1);
                    }
                }
            }
            else 
            {
                tempSize += (FS_BLOCK_SIZE - FS_BLOCK_HEADER_SIZE); 
            }
            nextBlockPointer = mvFlash32Rd(mvFlashInfo, nextBlockPointer & 
                                               FS_BLOCK_OFFSET_MASK);
            mvFSEntries[fileHandler].blockPointer = nextBlockPointer;
        }
        nextBlockPointer = temp;
        offset = tempSize + offset;
        if(offset >= (int)fileSize)
        {
            mvFSEntries[fileHandler].filePointer =
                mvFSEntries[fileHandler].EOFpointer;
            return 1/*true*/;
        }
        if(offset <= (int)0)
        {
            mvFSEntries[fileHandler].filePointer = 0;
            return 1/*true*/;
        }
        while(tempOffset < offset)
        {
            validBytes = mvFSGetNumOfValidBytes(nextBlockPointer,
                            (nextBlockPointer & FS_BLOCK_OFFSET_MASK) 
                             + FS_BLOCK_HEADER_SIZE,
                             mvFSEntries[fileHandler].EOFpointer);
            if((tempOffset + (int)validBytes) >= offset)
                break;
            tempOffset += validBytes;
            nextBlockPointer = mvFlash32Rd(mvFlashInfo, nextBlockPointer 
                                             & FS_BLOCK_OFFSET_MASK);
            mvFSEntries[fileHandler].blockPointer = nextBlockPointer;
        }
        mvFSEntries[fileHandler].filePointer = 
            (nextBlockPointer & FS_BLOCK_OFFSET_MASK) +
            (offset - tempOffset) + FS_BLOCK_HEADER_SIZE - 1;
        return 1/*true*/;
    default:
        return 0xffffffff;
    }
    return 0xffffffff;
}

/*******************************************************************************
* mvFSReadErasingCounter - Reads the value of the erasing counter (if 
*                          implemented).
*
* DESCRIPTION:
*       This function reads the value of the erasing counter ( if implemented )
*       of the given sector .The erasing counter is enabled by the function 
*       ‘fileSysFormat’  (by the ‘mode’ parameter) . It is  implimented by 
*       allocating one block at the end of each sector and it helps monitoring 
*       the number of erasures of the sector the counter is at .
*      NOTE:
*       he file system's data base must be initialized by ‘fileSysInit’ function
*       before using  this function .           
*
* INPUT:
*       sectorNumber - The sector number to read its erasing counter 
*                      (if implemented). 
*
* OUTPUT:
*       None.
*
* RETURN:
*       The vlaue of the erasing counter, 0xffffffff in case the erasing
*       counter was not implemented or the sector number is not valid. 
*
*******************************************************************************/
unsigned int mvFSReadErasingCounter(unsigned int sectorNumber)
{
    unsigned int counterValue,sectorBaseAddr,sectorSize;
    
    if(initializationFlag != FS_FILE_SYSTEM_INITIALIZED)
        return 0xffffffff;
    if(mvFlash32Rd(mvFlashInfo, mvFSOffset + FS_ERSAING_COUNTER_FLAG_OFFSET) !=
       FS_ENABLE_ERASING_COUNTER)
        return 0xffffffff;
    if((sectorBaseAddr = mvFlashSecOffsGet(mvFlashInfo, sectorNumber)) == 0xffffffff)
        return 0xffffffff;
    if(sectorBaseAddr < mvFSOffset)
        return 0xffffffff;
    sectorSize = mvFlashSecSizeGet(mvFlashInfo, sectorNumber);
    counterValue = mvFlash32Rd(mvFlashInfo, sectorBaseAddr + sectorSize - FS_BLOCK_SIZE);
    return counterValue;
}

/*******************************************************************************
* mvFSGetFreeSize - Returns the number of free bytes in the file system.
*
* DESCRIPTION:
*       This function returns the number of free bytes in the entire file 
*       system. It counts the number of free blocks and dirty blocks ( blocks 
*       that are no longer in use by any file ) from the begining of the 
*       file-system to the end , and return that value multiply by block size 
*       (in bytes).         
*      NOTE:
*       The file system's data base must be initialized by ‘fileSysInit’ 
*       function before using  this function .  
* INPUT:
*       None.
*
* OUTPUT:
*       None.
*
* RETURN:
*       Number of free bytes in the entire file-system on success, 0xFFFFFFFF 
*       otherwise.
*
*******************************************************************************/
unsigned int mvFSGetFreeSize()
{
    unsigned int freeBlockOffset,sectorTopOffset,i;
    unsigned int sectorNumber,lastSector;
    unsigned int dirtyBlocksCounter = 0,dirtyBlocksSector;
    unsigned int freeBlocksCounter = 0;

    /* Check if the file system's data base was initialized */
    if(initializationFlag != FS_FILE_SYSTEM_INITIALIZED)
        return 0xffffffff;
    freeBlockOffset     = mvFSOffset + FS_BLOCK_SIZE;
    sectorNumber        = mvFlashInWhichSec(mvFlashInfo, freeBlockOffset);
    lastSector          = mvFlashNumOfSecsGet(mvFlashInfo);
    dirtyBlocksSector   = sectorNumber;
    for(i = sectorNumber ; i < lastSector ; i++)
    {
        sectorTopOffset = mvFlashSecOffsGet(mvFlashInfo, sectorNumber) +
                          mvFlashSecSizeGet(mvFlashInfo, sectorNumber) - 1;
        while(freeBlockOffset < sectorTopOffset)
        {
            if(mvFlash8Rd(mvFlashInfo, freeBlockOffset + FS_BLOCK_STATUS_FLAG_OFFSET)
               == FS_FREE_BLOCK) freeBlocksCounter++;
            if(mvFlash8Rd(mvFlashInfo, freeBlockOffset + FS_BLOCK_STATUS_FLAG_OFFSET)
               == FS_FREE_DIRTY_BLOCK) dirtyBlocksCounter++;
            freeBlockOffset += FS_BLOCK_SIZE;  
        }
        sectorNumber = mvFlashInWhichSec(mvFlashInfo, freeBlockOffset);
    }
    return (freeBlocksCounter + dirtyBlocksCounter) * 
           (FS_BLOCK_SIZE - FS_BLOCK_HEADER_SIZE);
}

/********************************************************************************/
/********************************************************************************/
/********************************************************************************/

/***********************************************************************/
/* ask the user to give a file name and chck if the file already exist.*/
/***********************************************************************/
MV_VOID get_filename(MV_8 *fileName, MV_BOOL exeType)
{
    int i;
    extern MV_8 console_buffer[];

    while(1)
    {
        i = 0;
        printf("Enter name & extension for the file (maximum %d characters for the name)\n",
               FS_FILE_NAME_LENGTH);
	readline(" # ");

	strcpy(fileName, console_buffer);
        while( i < FS_FILE_NAME_LENGTH &&
              fileName[i] != '\0' && fileName[i] != ' ') i++;
	fileName[i] = '\0';
        if(i == 0)
        {
            strcpy(fileName,"NoName");
            i = 6;
            fileName[i] = '\0';
        }
	/* if exe file type */
	if(exeType == MV_TRUE){
		i = 0;
		while(fileName[i] != '.' && i < FS_FILE_NAME_LENGTH &&
              		fileName[i] != '\0') i++;
		fileName[i] = '.';
	        fileName[i+1] = 'e';
	        fileName[i+2] = 'x';
	        fileName[i+3] = 'e';
	        fileName[i+4] = '\0';
	}

        if (mvFSFileFind(fileName) != FS_FILE_NOT_FOUND)
        {
            printf("File already exists, overwrite? (Y/N)\n");
	    readline(" ? ");
            if(strcmp(console_buffer,"N") == 0 || strcmp(console_buffer,"no") == 0 ||
               strcmp(console_buffer,"n") == 0)
            {
                continue;
            }
            else
            {
                if(mvFSDelete(fileName) != FS_FILE_DELETED)
                {
                    printf("File could not be deleted!\n");
                    continue;
                }
                else
                {
                    break;
                }
            }
        }
        else
        {
            break;
        }
    }
    return;
}

/***********************************************/
/* load a file to the flash FS using the tftp. */
/***********************************************/
int
tftpfs_cmd(cmd_tbl_t *cmdtp, int flag, int argc, MV_8 *argv[])
{
        int   	filesize;
	MV_8	fileName[CFG_CBSIZE];
	MV_U32    status;	
	FS_FILE_HANDLER    fileHandler;

    	if(!enaMonExt()){
		printf("This command can be used only if enaMonExt is set!\n");
		return 0;}

        load_addr = 0x400000;                                                         
        if(argc == 2) {
                copy_filename (BootFile, argv[1], sizeof(BootFile));
 
        }else{ printf ("Usage:\n%s\n", cmdtp->usage);
                return 0;
        }
 
    	status = mvFSInit(FS_NO_CACHE);
    	if(status == FS_NO_VALID_FAT_STRING)
    	{
        	printf("ERROR: FS_NO_VALID_FAT_STRING\n");
        	return 0;
    	}
    	if(status == FS_OFFSET_OUT_OF_RANGE)
    	{
        	printf("ERROR: FS_OFFSET_OUT_OF_RANGE\n");
        	return 0;
    	}                                  

        if ((filesize = NetLoop(TFTP)) < 0)
                return 0;
 
        /* done if no file was loaded (no errors though) */
        if (filesize <= 0)
                return 0;

	/* get a file name from the user */
	get_filename(fileName,MV_FALSE);

     	fileHandler = mvFSOpen(fileName,"w");
    	if(fileHandler == FS_FILE_OPEN_ERROR)
    	{
        	printf("ERROR: Can't open file!\n");
        	return 0;
    	}
    	if(mvFSWrite(fileHandler,filesize,(MV_8 *)load_addr) != filesize)
    	{
        	printf("ERROR: Writing data to file.\n");
        	return 0;
    	}
    	printf("File loaded to file system successfully...\n");
    	mvFSClose(fileHandler);

	return 1;
}

U_BOOT_CMD(
	FStftp,      2,     1,      tftpfs_cmd,
	"FStftp	- tftp a file to the Flash MV FS\n",
	" filename.\n"
	"\tDownload a file through the network interface to the DRAM(0x400000) using\n"
	"\ttftp, and copy the file to the MV Flash FS.\n"
	"\t(This command can be used only if enaMonExt is set!)\n"
);

/***********************************************/
/* load a file to the flash FS using the tftp. */
/***********************************************/
int
tftpefs_cmd(cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
        int     filesize;
        char    fileName[CFG_CBSIZE],c;
        unsigned int    status, entryAddress;
        FS_FILE_HANDLER    fileHandler;
        unsigned char   addressBuffer[8];
                                                                                                                             
        if(!enaMonExt()){
                printf("This command can be used only if enaMonExt is set!\n");
                return 0;}
                                                                                                                             
        if(argc == 3) {
                entryAddress = simple_strtoul(argv[1], NULL, 16);
                load_addr = entryAddress;
                copy_filename (BootFile, argv[2], sizeof(BootFile));
                                                                                                                             
        }else{
                printf ("Usage:\n%s %d\n", cmdtp->usage,argc);
                return 0;
        }
                                                                                                                             
        status = mvFSInit(FS_NO_CACHE);
        if(status == FS_NO_VALID_FAT_STRING)
        {
                printf("ERROR: FS_NO_VALID_FAT_STRING\n");
                return 0;
        }
        if(status == FS_OFFSET_OUT_OF_RANGE)
        {
                printf("ERROR: FS_OFFSET_OUT_OF_RANGE\n");
                return 0;
        }
 
        if ((filesize = NetLoop(TFTP)) < 0)
                return 0;
  
        /* done if no file was loaded (no errors though) */
        if (filesize <= 0)
                return 0;
 
        /* get a file name from the user */
        get_filename(fileName,MV_TRUE);
 
        fileHandler = mvFSOpen(fileName,"w");
        if(fileHandler == FS_FILE_OPEN_ERROR)
        {
                printf("ERROR: Can't open file!\n");
                return 0;
        }
 
        /* Set the entry address for the file */
        c = (entryAddress >> 28) & 0x0f;
        if(c < 10)
                addressBuffer[0] = c + '0';
        else
                addressBuffer[0] = c + 'A' - 10;
        c = (entryAddress >> 24) & 0x0f;
        if(c < 10)
                addressBuffer[1] = c + '0';
        else
                addressBuffer[1] = c + 'A' - 10;
        c = (entryAddress >> 20) & 0x0f;
        if(c < 10)
                addressBuffer[2] = c + '0';
        else
                addressBuffer[2] = c + 'A' - 10;
        c = (entryAddress >> 16) & 0x0f;
        if(c < 10)
                addressBuffer[3] = c + '0';
        else
                addressBuffer[3] = c + 'A' - 10;
        c = (entryAddress >> 12) & 0x0f;
        if(c < 10)
                addressBuffer[4] = c + '0';
        else
                addressBuffer[4] = c + 'A' - 10;
        c = (entryAddress >> 8) & 0x0f;
        if(c < 10)
                addressBuffer[5] = c + '0';
        else
                addressBuffer[5] = c + 'A' - 10;
        c = (entryAddress >> 4) & 0x0f;
        if(c < 10)
                addressBuffer[6] = c + '0';
        else
                addressBuffer[6] = c + 'A' - 10;
        c = (entryAddress) & 0x0f;
        if(c < 10)
                addressBuffer[7] = c + '0';
        else
                addressBuffer[7] = c + 'A' - 10;
  
        if(mvFSWrite(fileHandler,8,(char *)addressBuffer)!= 8)
        {
                printf("ERROR: Writing data to file.\n");
                return 0;
        }
 
        if(mvFSWrite(fileHandler,filesize,(char *)load_addr) != filesize)
        {
                printf("ERROR: Writing data to file.\n");
                return 0;
        }
        printf("File loaded to file system successfully...\n");
        mvFSClose(fileHandler);
 
        return 1;
}
 
U_BOOT_CMD(
        FStftpe,      3,     1,      tftpefs_cmd,
        "FStftpe - tftp an exe file to the Flash MV FS\n",
        " entry_address filename.\n"
        "\tDownload a file through the network interface to entry_address using\n"
        "\ttftp, and copy the file to the MV Flash FS.\n"
        "\t(This command can be used only if enaMonExt is set!)\n"
);

/**************************************************/
/* load a file to the flash FS using the loadnet. */
/**************************************************/
#if 0
int
loadfs_cmd(cmd_tbl_t *cmdtp, int flag, int argc, MV_8 *argv[])
{
        int   	filesize;
	MV_8	fileName[CFG_CBSIZE];
	MV_U32    status,addr = 0x20000;	
	FS_FILE_HANDLER    fileHandler;

    	if(!enaMonExt()){
		printf("This command can be used only if enaMonExt is set!\n");
		return 0;}

    	status = mvFSInit(FS_NO_CACHE);
    	if(status == FS_NO_VALID_FAT_STRING)
    	{
        	printf("ERROR: FS_NO_VALID_FAT_STRING\n");
        	return 0;
    	}
    	if(status == FS_OFFSET_OUT_OF_RANGE)
    	{
        	printf("ERROR: FS_OFFSET_OUT_OF_RANGE\n");
        	return 0;
    	}                                  

	filesize = load_net(&addr);
    	printf("\nFile loaded successfully...\n");
    	printf("File size: %d bytes.\n",filesize);

 
        /* done if no file was loaded (no errors though) */
        if (filesize <= 0)
                return 0;
 
	/* get a file name from the user */
	get_filename(fileName, MV_FALSE);

	fileHandler = mvFSOpen(fileName,"w");
    	if(fileHandler == FS_FILE_OPEN_ERROR)
    	{
        	printf("ERROR: Can't open file!\n");
        	return 0;
    	}
    	if(mvFSWrite(fileHandler,filesize,(MV_8 *)addr) != filesize)
    	{
        	printf("ERROR: Writing data to file.\n");
        	return 0;
    	}
    	printf("File loaded to file system successfully...\n");
    	mvFSClose(fileHandler);

	return 1;
}

U_BOOT_CMD(
	FSlf,      1,     1,      loadfs_cmd,
	"FSlf	- Load a file to the Flash MV FS\n",
	" \n"
	"\tLoad S-Record file via the first available ethernet port, to the DRAM\n"
	"\t(0x200000) and copy it to the MV Flash FS.\n"
	"\t(This command can be used only if enaMonExt is set!)\n"
);
#endif

/*******************************************************/
/* load an exe file to the flash FS using the loadnet. */
/*******************************************************/
#if 0
int
loadexefs_cmd(cmd_tbl_t *cmdtp, int flag, int argc, MV_8 *argv[])
{
        int   	filesize;
	MV_8	fileName[CFG_CBSIZE],c;
	MV_U32    status,entryAddress = 0x0;	
	FS_FILE_HANDLER    fileHandler;
    	MV_U8   addressBuffer[8];

    	if(!enaMonExt()){
		printf("This command can be used only if enaMonExt is set!\n");
		return 0;}

    	status = mvFSInit(FS_NO_CACHE);
    	if(status == FS_NO_VALID_FAT_STRING)
    	{
        	printf("ERROR: FS_NO_VALID_FAT_STRING\n");
        	return 0;
    	}
    	if(status == FS_OFFSET_OUT_OF_RANGE)
    	{
        	printf("ERROR: FS_OFFSET_OUT_OF_RANGE\n");
        	return 0;
    	}                                  

	filesize = load_net(&entryAddress);
    	printf("\nFile loaded successfully...\n");
    	printf("File size: %d bytes.\n",filesize);

 
        /* done if no file was loaded (no errors though) */
        if (filesize <= 0)
                return 0;
 
	/* get a file name from the user */
	get_filename(fileName, MV_TRUE);

    	fileHandler = mvFSOpen(fileName,"w");
    	if(fileHandler == FS_FILE_OPEN_ERROR)
    	{
        	printf("ERROR: Can't open file!\n");
        	return 0;
    	}
    	/* Set the entry address for the file */
    	c = (entryAddress >> 28) & 0x0f;
    	if(c < 10)
        	addressBuffer[0] = c + '0';
    	else
        	addressBuffer[0] = c + 'A' - 10;
    	c = (entryAddress >> 24) & 0x0f;
    	if(c < 10)
        	addressBuffer[1] = c + '0';
   	else
        	addressBuffer[1] = c + 'A' - 10;
    	c = (entryAddress >> 20) & 0x0f;
    	if(c < 10)
        	addressBuffer[2] = c + '0';
    	else
        	addressBuffer[2] = c + 'A' - 10;
    	c = (entryAddress >> 16) & 0x0f;
    	if(c < 10)
        	addressBuffer[3] = c + '0';
    	else
        	addressBuffer[3] = c + 'A' - 10;
    	c = (entryAddress >> 12) & 0x0f;
    	if(c < 10)
        	addressBuffer[4] = c + '0';
    	else
        	addressBuffer[4] = c + 'A' - 10;
    	c = (entryAddress >> 8) & 0x0f;
    	if(c < 10)
        	addressBuffer[5] = c + '0';
    	else
        	addressBuffer[5] = c + 'A' - 10;
    	c = (entryAddress >> 4) & 0x0f;
    	if(c < 10)
        	addressBuffer[6] = c + '0';
    	else
        	addressBuffer[6] = c + 'A' - 10;
    	c = (entryAddress) & 0x0f;
    	if(c < 10)
        	addressBuffer[7] = c + '0';
    	else
        	addressBuffer[7] = c + 'A' - 10;
 
    	if(mvFSWrite(fileHandler,8,(MV_8 *)addressBuffer)!= 8)
    	{
       		printf("ERROR: Writing data to file.\n");
        	return 0;
    	}
     
    	if(mvFSWrite(fileHandler,filesize,(MV_8 *)entryAddress)!= filesize)
    	{
        	printf("ERROR: Writing data to file.\n");
        	return 0;
    	}
    	printf("File loaded to file system successfully...\n");
    	mvFSClose(fileHandler);
    	return 1;
}

U_BOOT_CMD(
	FSlef,      1,     1,      loadexefs_cmd,
	"FSlef	- Load an exe file to the Flash MV FS\n",
	" \n"
	"\tLoad S-Record  EXE file via the first available ethernet port to the DRAM\n"
	"\t(0x200000) and copy it to the MV Flash FS.\n"
	"\tAfter load is complete the EXE file can be run by using: FSrun filename.\n"
	"\t(This command can be used only if enaMonExt is set!)\n"
);
#endif

/*********************************************************************************
 * Load Exe file from the Flash FS.
 *********************************************************************************/
MV_U32 loadExe(MV_8 * fname)
{
    register MV_U32 byteCount;
    register MV_U32 entryAddress = 0xffffffff;
    MV_8            addressBuffer[9];
    FS_FILE_HANDLER fileHandler;
    MV_U32          nextBlockPointer, sourceAddress, destAddress, status;
    MV_U32          mvFlashBaseAddress;
                                                                                                           
    mvFlashBaseAddress = MV_BOARD_FLASH_BASE_ADRS;

    status = mvFSInit(FS_NO_CACHE);
    if(status == FS_NO_VALID_FAT_STRING)
    {
        printf("ERROR: FS_NO_VALID_FAT_STRING\n");
        return entryAddress;
    }
    if(status == FS_OFFSET_OUT_OF_RANGE)
    {
        printf("ERROR: FS_OFFSET_OUT_OF_RANGE\n");
        return entryAddress;
    }
    if((fileHandler = mvFSOpen(fname,"r")) == FS_FILE_OPEN_ERROR)
    {
	printf("ERROR in file open\n");
        return entryAddress;
    }

    /* Extract the file's entry address */
    mvFSRead(fileHandler,8,addressBuffer);
	addressBuffer[8]='\0';
    entryAddress = simple_strtoul(addressBuffer, NULL, 16);
    debug("entryAddress = %x \n",entryAddress);

    nextBlockPointer = mvFlash32Rd(mvFlashInfo, mvFSEntries[fileHandler].pointerToFAT);
    /* The First block contain the address, thus treated differently */
    byteCount = FS_BLOCK_SIZE - FS_BLOCK_HEADER_SIZE - 8 /* Address size */ ;
    destAddress = entryAddress;
    sourceAddress = (nextBlockPointer & FS_BLOCK_OFFSET_MASK) + mvFlashBaseAddress
                    + FS_BLOCK_HEADER_SIZE + 8/* Address size */;

    /* Flush the block from the cache before executing it */
    debug("memcpy transfer src %x dst %x bytecnt %x\n",\
				sourceAddress,destAddress,byteCount);
    memcpy((void *)destAddress, (void *)sourceAddress, byteCount);
    destAddress += byteCount;
     
    nextBlockPointer = mvFlash32Rd(mvFlashInfo, nextBlockPointer & FS_BLOCK_OFFSET_MASK);
    byteCount = FS_BLOCK_SIZE - FS_BLOCK_HEADER_SIZE;
     
    while(nextBlockPointer != 0xffffffff)
    {
        sourceAddress = (nextBlockPointer & FS_BLOCK_OFFSET_MASK) + mvFlashBaseAddress
                        + FS_BLOCK_HEADER_SIZE;

	memcpy((MV_U32*)destAddress,(MV_U32*)sourceAddress,byteCount);

        destAddress += byteCount;
        nextBlockPointer = mvFlash32Rd(mvFlashInfo, nextBlockPointer & FS_BLOCK_OFFSET_MASK);
    }
    mvFSClose(fileHandler);
    return entryAddress;
}

int runexeFS_cmd(cmd_tbl_t *cmdtp, int flag, int argc, MV_8 *argv[])
{
	MV_8 fileName[CFG_CBSIZE];
	MV_U32 i,entryAddress;	

    	if(!enaMonExt()){
		printf("This command can be used only if enaMonExt is set!\n");
		return 0;}

        if(argc >= 2) {
                copy_filename(fileName, argv[1], sizeof(fileName));
 
        }else{ printf ("Usage:\n%s\n", cmdtp->usage);
                return 0;
        }
	
	for(i = 0 ; i < FS_FILE_NAME_LENGTH; i++)
            {
                if(fileName[i] == '.')
                    break;
                if(fileName[i] == '\0')
                {
                    fileName[i]    = '.';
                    fileName[i+1]  = 'e';
                    fileName[i+2]  = 'x';
                    fileName[i+3]  = 'e';
                    fileName[i+4]  = 0;
                    break;
                }
            }
	if((entryAddress = loadExe(fileName)) == 0xffffffff)
        {
        	printf ("%s: Command not found.\n", fileName);
                return 0;
        }
        mv_go(entryAddress, --argc, &argv[1]);
        return 1;
}

U_BOOT_CMD(
	FSrun,      CFG_MAXARGS,     1,      runexeFS_cmd,
	"FSrun	- Load an exe file from the Flash MV FS and run it\n",
	" filename\n"
	"\tLoad an exe file 'filename' from the Flash MV FS and run it.\n"
	"\t(This command can be used only if enaMonExt is set!)\n"
);


/******************************************************************************
* Functionality- The commands allows the user to view the contents of a file
*                located in the file system.
*****************************************************************************/
int type_cmd(cmd_tbl_t *cmdtp, int flag, int argc, MV_8 *argv[])
{
    MV_8 fileName[128]={},readBuffer[101],key;
    FS_FILE_HANDLER    fileHandler;
    MV_U32 readCount=0, newLineCount=0, flagm,readCounttmp, lineCount=0,i;

    if(!enaMonExt()){
	printf("This command can be used only if enaMonExt is set!\n");
	return 0;}
   
    if(argc == 2) {
         copy_filename(fileName, argv[1], sizeof(fileName));
    }else{ printf ("Usage:\n%s\n", cmdtp->usage);
         return 0;
    }                                                                                                            
    fileHandler = mvFSOpen(fileName,"r");
    if(fileHandler != FS_FILE_OPEN_ERROR)
    {
        while(readCount < mvFSFileSize(fileHandler))
        {
            readCounttmp = mvFSRead(fileHandler,100,readBuffer);
	    readBuffer[100] = '\0';
            if(readCounttmp < 100)
                readBuffer[readCounttmp] = '\0';
            readCount += readCounttmp;
            lineCount += readCounttmp;
            for(i = 0 ; i <= readCounttmp ; i++)
                if(readBuffer[i] == '\n')
                    newLineCount++;
            printf("%s",readBuffer);
            if(newLineCount >= 20 || lineCount > 1600)
            {
                flagm = 0;
                while(1)
                {
                    key = getc();
                    switch(key)
                    {
                    case ' ':
                        flagm = 1;
                        newLineCount = 0;
                        lineCount = 0;
                        break;
                    case 'x':
                        mvFSClose(fileHandler);
                        printf("\n");
                        return 1;
                    }
                    if(flagm == 1)
                        break;
                }
            }
        }
    }
    mvFSClose(fileHandler);
    printf("\n");
    return 1;
}
 
U_BOOT_CMD(
	FStype,      CFG_MAXARGS,     1,      type_cmd,
	"FStype	- cat file from the Flash MV FS\n",
	" filename \n"
	"\tDisplay an ascii file 'filename' found on the MV Flash FS.\n"
	"\t(This command can be used only if enaMonExt is set!)\n"
);



/******************************************************************************
* Functionality- Formats the mvFlash mvMemory at a given offset
*****************************************************************************/       
int format_cmd(cmd_tbl_t *cmdtp, int flag, int argc, MV_8 *argv[])
{
    MV_U32    offset =0,result;
    extern MV_8 console_buffer[];

    if(!enaMonExt()){
	printf("This command can be used only if enaMonExt is set!\n");
	return 0;}

    if(argc == 2) {
	offset = simple_strtoul(argv[1], NULL, 16); 
    }else{ printf ("Usage:\n%s\n", cmdtp->usage);
        return 0;
    }

    printf("\a\a\aThis action will erase the FLASH Memory!!!\n");
    printf("Are you sure (y/n) ");
    readline(" ? ");

    if(strcmp(console_buffer,"y") == 0 || strcmp(console_buffer,"yes") == 0 || strcmp(console_buffer,"Y") == 0)
    {
        if(offset == 0xffffffff)
        {
            printf("Bad offset!\n");
        }
        result = mvFSFormat(offset,FS_NO_ERASING_COUNTER);
        if(result == FS_VALID_FILE_SYS_IN_LOWER_OFFSET)
        {
            printf("ERROR: FS_VALID_FILE_SYS_IN_LOWER_OFFSET\n");
            return 0;
        }
        if(result == FS_FLASH_MEMORY_NOT_PRESENT)
        {
            printf("ERROR: FS_FLASH_MEMORY_NOT_PRESENT\n");
            return 1;
        }
        printf("Flash Memory formated successfully, actual offset is: %x\n"
               ,result);
    }
    return 1;
}

U_BOOT_CMD(
	FSformat,      2,     1,      format_cmd,
	"FSformat- format the Flash MV FS\n",
	" address \n"
	"\tInitialize the MV Flash FS starting at offset 'address' in the Flash.\n"
	"\t(This command can be used only if enaMonExt is set!)\n"
);


/******************************************************************************
* Functionality- Display the list of files currently on the system (file name + size)
*****************************************************************************/
int dir_cmd(cmd_tbl_t *cmdtp, int flag, int argc, MV_8 *argv[])
{
    MV_U32    fileEntryOffset,nextBlockOffset,mvFSOffset;
    MV_U32    status,i,j,spaces = 20;
    FS_FILE_HANDLER    fileHandler;
    MV_8            fileName[20];

    if(!enaMonExt()){
	printf("This command can be used only if enaMonExt is set!\n");
	return 0;}

    status = mvFSInit(FS_NO_CACHE);
    if(status == FS_NO_VALID_FAT_STRING)
    {
        printf("ERROR: FS_NO_VALID_FAT_STRING\n");
        return 0;
    }
    if(status == FS_OFFSET_OUT_OF_RANGE)
    {
        printf("ERROR: FS_OFFSET_OUT_OF_RANGE\n");
        return 0;
    }
    if(status != FS_SYSTEM_ALREADY_INITIALIZED)
    {
        printf("Number of files in system: %d\n",status);
    }
                                                                                                               
    mvFSOffset = mvFSSearchForSignature();
    nextBlockOffset = mvFSOffset;
    while(nextBlockOffset != 0xffffffff)
    {
        /* Skip the block's header */
        fileEntryOffset = nextBlockOffset + FS_FAT_HEADER_SIZE;
        for(i = 0 ; i < 15 ; i++)
        {
            if(mvFlash8Rd(mvFlashInfo, fileEntryOffset + FS_ENTRY_STATUS_FLAG_OFFSET)
               == FS_ENTRY_IN_USE)
            {
                mvFSReadFileNameFromFATentry(fileEntryOffset,fileName);
                fileHandler = mvFSOpen(fileName,"r");
                printf("%s",fileName);
                spaces = 22 - strlen(fileName);
                for(j = 0 ; j < spaces ; j++)
                    printf(" ");
                printf("size: %d bytes\n",mvFSFileSize(fileHandler));
                mvFSClose(fileHandler);
            }
            /* point to the next entry */
            fileEntryOffset += FS_FAT_FILE_ENTRY_SIZE;
        }
        nextBlockOffset = mvFlash32Rd(mvFlashInfo, nextBlockOffset);
    }
    printf("\n                            %d free bytes\n",
           mvFSGetFreeSize());
    return 1;
}


U_BOOT_CMD(
	FSdir,      1,     1,      dir_cmd,
	"FSdir	- ls the Flash MV FS\n",
	" \n"
	"\tDisplay the MV Flash FS contents.\n"
	"\t(This command can be used only if enaMonExt is set!)\n"
);

/******************************************************************************
* Functionality- Deletes a file from the system
*****************************************************************************/
int del_cmd(cmd_tbl_t *cmdtp, int flag, int argc, MV_8 *argv[])
{
    MV_U32 status;
    MV_8            cmd[30];

    if(!enaMonExt()){
	printf("This command can be used only if enaMonExt is set!\n");
	return 0;}

    if(argc == 2) {
    	copy_filename (cmd, argv[1], sizeof(cmd));
    }else{ printf ("Usage:\n%s\n", cmdtp->usage);
    	return 0;
    }                                                                                                           
    status = mvFSInit(FS_NO_CACHE);                                                                                                     
    if(status == FS_NO_VALID_FAT_STRING)
    {
        printf("ERROR: FS_NO_VALID_FAT_STRING\n");
        return 0;
    }
    if(status == FS_OFFSET_OUT_OF_RANGE)
    {
        printf("ERROR: FS_OFFSET_OUT_OF_RANGE\n");
        return 0;
    }
    if(mvFSFileFind(cmd) != FS_FILE_NOT_FOUND)
    {
        if(mvFSDelete(cmd) == FS_FILE_DELETED)
        {
            printf("File deleted\n");
        }
    }
    else
    {
        printf("%s:No such file\n",cmd);
    }
    return 1;
}

U_BOOT_CMD(
	FSdel,      2,     1,      del_cmd,
	"FSdel	- del a file from the Flash MV FS\n",
	" filename \n"
	"\tDelete a file 'filename' from the MV Flash FS.\n"
	"\t(This command can be used only if enaMonExt is set!)\n"
);


#endif /* CONFIG_CMD_BSP */

#endif /* MV_INC_BOARD_NOR_FLASH */
