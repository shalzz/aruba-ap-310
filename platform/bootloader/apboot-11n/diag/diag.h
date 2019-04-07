#ifndef _DIAG_H
#define _DIAG_H

#define DIAG_BAUD_RATE            115200
#define DIAG_SERIAL_CONSOLE_PORT  0

#define DIAG_PASS      (0x00)
#define DIAG_FAIL      (0x01)
 
#define PAGES_PER_BLOCK 64

typedef int (diag_func_t) (void);
extern unsigned int *mem_test_start_offset;
extern unsigned int *mem_test_end_offset;

/* Function prototypes */
int mem_data_bus_test(void);
int mem_address_bus_test(void);
int mem_device_test(void);
int gbe_link_detect_test(void);
int gbe_mac_loopback_test(void);
int gbe_external_loopback_test(void);
int rtc_test(void);
int mvNandDetectTest(void);
int mvNandBadBlockTest(void);
int mvNandReadWriteTest(void);

void diag_init(char *);
void diag_board_init(char *);
void diag_serial_init(int, int);
void diag_get_mem_detail(unsigned int **, unsigned int **);
void diag_int_lpbk(int, int);
void diag_uart_putc(int, unsigned char);
int diag_uart_getc(int, unsigned char *);

#endif /* _DIAG_H */
