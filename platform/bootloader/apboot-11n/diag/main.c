
#include <common.h>
#include <diag.h>

diag_func_t *diag_sequence[] = 
{
    mem_data_bus_test,      /* DRAM Data bus test */ 
    mem_address_bus_test,   /* DRAM Address bus test */
    mem_device_test,        /* DRAM device test */
    mvNandDetectTest,
    mvNandBadBlockTest,
    gbe_link_detect_test,
    //gbe_mac_loopback_test,
    //gbe_external_loopback_test,
    mvNandReadWriteTest,
    rtc_test,               /* RTC test */
    NULL,
};

unsigned int *mem_test_start_offset;
unsigned int *mem_test_end_offset;

void run_diag(void)
{
	char board_name[30];
	diag_func_t **diag_func_ptr;

	printf("\n\nRunning diagnostics ...\n");

	/* Get the start and the end memory address offset for memory test */
	diag_get_mem_detail(&mem_test_start_offset, &mem_test_end_offset);

	for (diag_func_ptr = diag_sequence; *diag_func_ptr; ++diag_func_ptr)
	{
		printf("\n");
		if((*diag_func_ptr)())
			break;
	}

	if(*diag_func_ptr == NULL)
		printf("\nDiag completed\n");
	else
		printf("\nDiag FAILED\n");
}

void diag_init(char *board_name)
{
	/* Init interrupts - we would need this as we use timer interrupt in udelay */
	interrupt_init();

	/* Initialize board so that all the GPIO are set & we are good to init serial */
	diag_board_init(board_name);

	/* Get the start and the end memory address offset for memory test */
	diag_get_mem_detail(&mem_test_start_offset, &mem_test_end_offset);

	/* Serial init */
	diag_serial_init(DIAG_SERIAL_CONSOLE_PORT, DIAG_BAUD_RATE);
}
