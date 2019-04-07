
#include <common.h>

#include "diag.h"

int rtc_test(void)
{
	int hour1, minute1, second1;
	int hour2, minute2, second2;
	int ret_val = 0;

	printf("\tRTC test ");

	diag_get_rtc_time(&hour1, &minute1, &second1);
	
	/* FIXME: Observation says RTC requires additional udelay(1) to 
	 * read proper value
	 */
	udelay(1000000); 

	/* Set a time that changes all the digits after a second */
	diag_set_rtc_time(9, 59, 59);

	/* Wait for 1 second */
	/* Observation says RTC takes additional one second when new time is set */
	udelay(2000000); 

	diag_get_rtc_time(&hour2, &minute2, &second2);

	if(hour2   != 10 ||
	   minute2 != 0  ||
	   second2 >  1) 
	{
		/* Second should be 0 or max 1 (if error in udealy), not > 1 */

		printf("FAILED\n");
		printf("\t\tGot:      hour %02d minute %02d second %02d\n", hour2, minute2, second2);
		printf("\t\tExpected: hour 10 minute 00 second 00\n");
		ret_val = 1;
	}
	else
	{
		printf("\t\t\t\t\t PASSED\n");
	}

	/* As incrementing a second here will require complex calculation,
	 * we will ignore that passed second */
	diag_set_rtc_time(hour1, minute1, second1);

	return ret_val;
}
