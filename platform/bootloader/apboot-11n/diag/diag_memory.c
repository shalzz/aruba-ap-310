
#include <common.h>

#include "diag.h"

/*******************************************************************************
*
* Name        : mem_data_bus_test
* Description : The function tests data bus of the DRAM. It writes walking one
*               pattern to a fixed memory location.
* Input arg   : None 
* Output arg  : None
* Return      : Returns 1 on failure, else 0
*******************************************************************************/

int mem_data_bus_test(void)
{
	unsigned int pattern  = 1;
	unsigned int *address = mem_test_start_offset;

	printf("\tDDR2 data bus test                               ");

	for(; pattern != 0; pattern <<= 1)
	{
		*address = pattern;
		
		if(pattern != *address)
		{
			printf("FAILED\n");
			printf("\t\tTest failed at offset = 0x%X\n"
			       "\t\tData written          = 0x%X\n"
			       "\t\tData read             = 0x%X\n", address, 
			       pattern, *address);
			return 1;
		}
	}
	printf("PASSED\n");
	return 0;
}

/*******************************************************************************
*
* Name        : mem_address_bus_test
* Description : The function tests address bus of the DRAM. It writes walking 
*               one pattern while changes the addresses in the same way. i.e. 
*		write 1 to offset 1, 8 to offset 8, etc. Verify the data after
*		writting to the buffer with length MEM_ADD_TEST_SIZE.
* Input arg   : None 
* Output arg  : None
* Return      : Returns 1 on failure, else 0
*******************************************************************************/

int mem_address_bus_test(void)
{
	unsigned int offset = 1, size;
	unsigned int *base_address = mem_test_start_offset;
       
	size = mem_test_end_offset - mem_test_start_offset;

	printf("\tDDR2 address bus test                            ");

	/* We should start with pattern = 1. As we want to use offset and 
	 * pattern the same variable, we will decrement base address by 1.
	 * So, even if we start with offset = pattern = 1, we have actually
	 * started from the given base address. */
	base_address--;

	for(; offset < size; offset <<= 1)
	{
		base_address[offset] = offset;
	}

	for(offset = 1; offset < size; offset <<= 1)
	{
		if(base_address[offset] != offset)
		{
			printf("FAILED\n");
			printf("\t\tTest failed at offset = 0x%X\n"
			       "\t\tData written          = 0x%X\n"
			       "\t\tData read             = 0x%X\n", base_address 
			       + offset, offset, base_address[offset]);
			return 1;
		}
	}
	printf("PASSED\n");
	return 0;
}

/*******************************************************************************
*
* Name        : mem_device_test
* Description : The function tests address bus of the DRAM. It writes walking 
*               one pattern while changes the addresses in the same way. i.e. 
*		write 1 to offset 1, 8 to offset 8, etc. Verify the data after
*		writting to the buffer with length MEM_ADD_TEST_SIZE.
* Input arg   : None
* Output arg  : None
* Return      : Returns 1 on failure, else 0
*******************************************************************************/

int mem_device_test(void)
{
	unsigned int offset = 1, size;
	unsigned int *base_address = mem_test_start_offset;
       
	size = mem_test_end_offset - mem_test_start_offset;

	printf("\tDDR2 device test                                 ");

	/* Debug print */
#ifdef DEBUG
	printf("Start address = %#X\n", base_address);
	printf("size = %#X\n", size);
#endif

	/* We should start with pattern = 1. As we want to use offset and 
	 * pattern the same variable, we will decrement base address by 1.
	 * So, even if we start with offset = pattern = 1, we have actually
	 * started from the given base address. */
	base_address--;

	for(; offset < size; offset++)
	{
		base_address[offset] = offset;
	}

	/* Check previously written pattern and write anti-pattern */
	for(offset = 1; offset < size; offset++)
	{
		if(base_address[offset] != offset)
		{
			printf("FAILED\n");
			printf("\t\tTest failed at offset = 0x%X\n"
			       "\t\tData written          = 0x%X\n"
			       "\t\tData read             = 0x%X\n", base_address 
			       + offset, offset, base_address[offset]);
			return 1;
		}

		/* Write anti-pattern */
		base_address[offset] = ~offset;
	}

	/* Debug print */
#ifdef DEBUG
	printf("Checking anti-pattern\n");
#endif

	/* Check anti-pattern */
	for(offset = 1; offset < size; offset++)
	{
		if(base_address[offset] != ~offset)
		{
			printf("FAILED\n");
			printf("\t\tTest failed at offset = 0x%X\n"
			       "\t\tData written          = 0x%X\n"
			       "\t\tData read             = 0x%X\n", base_address 
			       + offset, ~offset, base_address[offset]);
			return 1;
		}
	}

	printf("PASSED\n");
	return 0;
}

