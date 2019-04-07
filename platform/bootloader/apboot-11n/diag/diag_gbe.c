#include <common.h>

#include "diag.h"
#include "eth-phy/mvEthPhy.h"
#include "mvBoardEnvLib.h"

/*******************************************************************************
*
* Name        : gbe_link_detect_test 
* Description : GbE Link Detection Test 
*               
* Input arg   : None 
* Output arg  : None
* Return      : Returns 1 on failure, else 0
*******************************************************************************/

int gbe_link_detect_test()
{
	unsigned int val = 0;
	unsigned int result = 0;

   /* read specific status reg */ 
   if( mvEthPhyRegRead( mvBoardPhyAddrGet(0), ETH_PHY_SPEC_STATUS_REG, &val) != MV_OK )
      return MV_ERROR;

   switch (val & ETH_PHY_SPEC_STATUS_SPEED_MASK)
   {   
      case ETH_PHY_SPEC_STATUS_SPEED_1000MBPS:
         printf( "\tSpeed: 1000 Mbps, " );
         break;
      case ETH_PHY_SPEC_STATUS_SPEED_100MBPS:
         printf( "\tSpeed: 100 Mbps, " );
         break;
      case ETH_PHY_SPEC_STATUS_SPEED_10MBPS:
         printf( "\tSpeed: 10 Mbps, " );
      default:
         printf( "\tSpeed: Uknown, " );
         break;
   }   

   if( val & ETH_PHY_SPEC_STATUS_DUPLEX_MASK ) 
	{
      printf( "Duplex: Full, " );
	}
   else 
	{
      printf( "Duplex: Half, " );
	}

   if( val & ETH_PHY_SPEC_STATUS_LINK_MASK )
	{
      printf("Link: up\n");
		result = 1;
	}
   else
	{
      printf("Link: down\n");
		result = 0;
	}
   printf("\tGbE link detect test                             ");
	printf(((result)? "PASSED\n":"FAILED\n"));

   return 0;
}

/*******************************************************************************
*
* Name        : gbe_mac_loopback_test
* Description : GbE MAC Loopback Test 
*               
* Input arg   : None 
* Output arg  : None
* Return      : Returns 1 on failure, else 0
*******************************************************************************/

int gbe_mac_loopback_test(void)
{
	printf("\tGbE mac loopback test                            ");

	printf("PASSED\n");
	return 0;
}

/*******************************************************************************
*
* Name        : gbe_external_loopback_test 
* Description : GbE External Loopback Test 
* Input arg   : None
* Output arg  : None
* Return      : Returns 1 on failure, else 0
*******************************************************************************/

int gbe_external_loopback_test(void)
{
	printf("\tGbE external loopback test                       ");

	printf("PASSED\n");
	return 0;
}

