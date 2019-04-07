
#include "uart/mvUart.h"
#include "rtc/integ_rtc/mvRtc.h"

/* UART timeout = UART_TIMEOUT * 100 usec */
#define UART_TIMEOUT 500

/* Function prototypes */
MV_VOID mvBoardEnvInit(MV_VOID);
MV_STATUS mvCtrlEnvInit(MV_VOID);
MV_VOID mvCpuIfInit(MV_VOID);

MV_U32 mvGppValueGet(MV_U32 group, MV_U32 mask);
MV_U32 mvDramIfBankBaseGet(MV_U32 bankNum);
MV_U32 mvDramIfSizeGet(MV_VOID);

/* Global variables */
extern unsigned int _TEXT_BASE;

/* Source code */

void diag_board_init(char *board_name)
{
	mvBoardEnvInit();
	mvCtrlEnvInit();
	mvCpuIfInit();

	mvBoardNameGet(board_name);
}

void diag_get_mem_detail(unsigned int **mem_test_start_offset, 
                         unsigned int **mem_test_end_offset)
{
	unsigned int start_offset, end_offset;

	/* Get the base address of memory bank 0 */
	end_offset = start_offset = mvDramIfBankBaseGet(0);

	/* Start from the location where u-boot ends
         * FIXME: Additional _1M is added for heap size
	 */
	start_offset += _TEXT_BASE + _1M + _1M;

	/* TODO: If we are using same DRAM register init file for base and 
	 * client, we need to hard-code end-offset for base to be 256M */
	end_offset += mvDramIfSizeGet();

	*mem_test_start_offset = (unsigned int *)start_offset;
	*mem_test_end_offset = (unsigned int *)end_offset;
}

void diag_serial_init(int port, int baud_rate)
{
	mvUartInit(port, CFG_TCLK / (16 * baud_rate), mvUartBase(port));

#if defined(LCP_88F6281)
	unsigned int reg_val = 0;
	if(port)
	{
		/* Change MPP settings for UART1 */
		reg_val = MV_REG_READ(mvCtrlMppRegGet(1));

		reg_val &= ~(0xFF << 20);
		reg_val |= (0x33 << 20);

		MV_REG_WRITE(mvCtrlMppRegGet(1), reg_val);

		/* Select UART from SD/UART selection switch - MPP34 */
		MV_REG_WRITE(0x10140, MV_REG_READ(0x10140) | BIT2);

		/* Output enable */
		MV_REG_WRITE(0x10144, MV_REG_READ(0x10144) & ~(BIT2));
	}
#endif

}

void diag_int_lpbk(int port, int set)
{
	volatile MV_UART_PORT *pUartPort = (volatile MV_UART_PORT *)mvUartBase(port);

	if(set)
		pUartPort->mcr |= 0x10;
	else
		pUartPort->mcr &= ~(0x10);
}

void diag_uart_putc(int port, unsigned char c)
{
	mvUartPutc(port, c);
}

int diag_uart_getc(int port, unsigned char *c)
{
	volatile MV_UART_PORT *pUartPort = (volatile MV_UART_PORT *)mvUartBase(port);
	int timeout = 0;

	while ( ((pUartPort->lsr & LSR_DR) == 0) && (++timeout < UART_TIMEOUT) )
	{
		udelay(100);
	}

	if (timeout == UART_TIMEOUT)
		return 1;

	*c = pUartPort->rbr;
	return 0;
}

void diag_get_rtc_time(int *hour, int *minute, int *second)
{
	MV_RTC_TIME mvRtcTime;

	mvRtcTimeGet(&mvRtcTime);

	*hour   = mvRtcTime.hours;
	*minute = mvRtcTime.minutes;
	*second = mvRtcTime.seconds;
}

void diag_set_rtc_time(int hour, int minute, int second)
{
	MV_RTC_TIME mvRtcTime;

	mvRtcTime.hours   = hour;
	mvRtcTime.minutes = minute;
	mvRtcTime.seconds = second;

	mvRtcTimeSet(&mvRtcTime);
}
