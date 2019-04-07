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

#include <common.h>
#include <command.h>
#include <net.h>
#include <malloc.h>
#include <miiphy.h>

#if defined (MV_INCLUDE_GIG_ETH)
//#include "sys/mvSysGbe.h"
#include "mvOs.h"
#include "mvSysHwConfig.h"
#include "eth/mvEth.h"
#include "gpp/mvGppRegs.h"
#include "eth/gbe/mvEthGbe.h"
#include "eth-phy/mvEthPhy.h"
#include "eth-phy/mvEthPhyRegs.h"
#include "ethSwitch/mvSwitch.h"
#include "mvBoardEnvLib.h"
#include "mvSysEthApi.h"
#if defined(DUAL_OS_78200) || defined(DUAL_OS_SHARED_MEM_78200)
#include "mv78200/mvSocUnitMap.h"
#include "mv78200/mvSemaphore.h"
#endif
//#define MV_DEBUG
#ifdef MV_DEBUG
#define DB(x) x
#else
#define DB(x)
#endif

extern MV_VOID mvSysEthInit(void);

/****************************************************** 
 * driver internal definitions --                     *
 ******************************************************/ 
/* use only tx-queue0 and rx-queue0 */
#define EGIGA_DEF_TXQ 0
#define EGIGA_DEF_RXQ 0

/* rx buffer size */ 
#define ETH_HLEN       14
#define WRAP           (2 + ETH_HLEN + 4 + 32)  /* 2(HW hdr) 14(MAC hdr) 4(CRC) 32(extra for cache prefetch)*/
#define MTU            1500
#define RX_BUFFER_SIZE (MTU + WRAP)

/* rings length */
#define EGIGA_TXQ_LEN   20
#define EGIGA_RXQ_LEN   20


typedef struct _egigaPriv
{
	int port;
	MV_VOID *halPriv;
	MV_U32 rxqCount;
	MV_U32 txqCount;
	MV_BOOL devInit;
} egigaPriv; 

#ifdef MV_DEBUG
extern void    ethPortQueues(int port, int rxQueue, int txQueue, int mode);
extern void    ethRegs(int port);
extern void    ethPortRegs(int port);
extern void    ethPortStatus(int port);
#endif

/****************************************************** 
 * functions prototype --                             *
 ******************************************************/
static int mvEgigaLoad( int port, char *name, char *enet_addr );

static int mvEgigaInit( struct eth_device *dev, bd_t *p );
static int mvEgigaHalt( struct eth_device *dev );
int mvEgigaTx( struct eth_device *dev, volatile MV_VOID *packet, int len );
static int mvEgigaRx( struct eth_device *dev );

static MV_PKT_INFO* mvEgigaRxFill(MV_VOID);

/*********************************************************** 
 * mv_eth_initialize --                                    *
 *   main driver initialization. loading the interfaces.   *
 ***********************************************************/

int mv_eth_initialize( bd_t *bis ) 
{

	int port;
#ifdef CONFIG_MV_PORT_SWAP
	int nport;
#else
#define nport port
#endif
	MV_8 *enet_addr;
	MV_8 name[NAMESIZE+1];
	MV_8 enetvar[9];

	mvSysEthInit();

	for(port=0; port<mvCtrlEthMaxPortGet(); port++)
	{
#ifdef MV78200
#if defined(DUAL_OS_78200) || defined(DUAL_OS_SHARED_MEM_78200)
		/* Skip ports mapped to another CPU*/
		if (enaMP())
		{		
		    if (MV_FALSE == mvSocUnitIsMappedToThisCpu(GIGA0+port))
				continue;
		}
#endif
#endif

#ifdef CONFIG_MV_PORT_SWAP
	nport = !port;	/* XXX; only works for two ports */
#endif
        if (MV_FALSE == mvCtrlPwrClckGet(ETH_GIG_UNIT_ID, nport)) continue;
                if (port == 2)
		    continue;
		/* interface name */
#ifdef CONFIG_APBOOT
		sprintf( name, "eth%d", port );
		/* interface MAC addr extract */

		/*
		 * We only want one environment variable for APBoot
		 * In Palomino APBoot, we use the same MAC address for both ports,
		 * but I don't think that was a good idea; we'll adjust it in
		 * mvEgigaLoad()
		 */
		sprintf( enetvar, "%s", "ethaddr");
#else
		sprintf( name, "egiga%d", port );
		/* interface MAC addr extract */
		sprintf( enetvar, nport ? "eth%daddr" : "ethaddr", port );
#endif
		enet_addr = getenv( enetvar );
	
		mvEthPortPowerUp(nport);

        	MV_REG_WRITE(ETH_TX_QUEUE_COMMAND1_REG(nport), 0x8);
		mvEgigaLoad( nport, name, enet_addr );
	}

#ifdef RD_MV78XX0_AMC
	/* light ready led */
	MV_REG_BIT_SET(GPP_DATA_OUT_REG(0), BIT8);
	/* Change SATA led polarity */
	MV_REG_BIT_SET(SATA_REG_BASE + 0x2C, BIT3);
#endif
	return 0;
}

#if defined(CONFIG_MII) || (CONFIG_COMMANDS & CFG_CMD_MII)
int
mv_miiphy_read(char *p, unsigned char addr, unsigned char reg, unsigned short *value)
{
	if (mvEthPhyRegRead(addr, reg, value) != MV_OK) {
		return -1;
	}
	return 0;
}

int 
mv_miiphy_write(char *p, unsigned char addr, unsigned char reg, unsigned short value)
{
	extern MV_STATUS mvEthPhyRegWrite(MV_U32 phyAddr, MV_U32 regOffs, MV_U16 data);
	if (mvEthPhyRegWrite(addr, reg, value) != MV_OK) {
		return -1;
	}
	return 0;
}
#endif

/*********************************************************** 
 * mvEgigaLoad --                                          *
 *   load a network interface into uboot network core.     *
 *   initialize sw structures e.g. private, rings, etc.    *
 ***********************************************************/
static int mvEgigaLoad( int port, char *name, char *enet_addr ) 
{
	struct eth_device *dev = NULL;
	egigaPriv *priv = NULL;
	ETH_PORT_CTRL dummy_port_handle;

	DB( printf( "%s: %s load - ", __FUNCTION__, name ) );

    dev = malloc( sizeof(struct eth_device) );
	priv = malloc( sizeof(egigaPriv) );

	if( !dev ) {
		DB( printf( "%s: %s falied to alloc eth_device (error)\n", __FUNCTION__, name ) );
		goto error;
	}

	if( !priv ) {
		DB( printf( "%s: %s falied to alloc egiga_priv (error)\n", __FUNCTION__, name ) );
		goto error;
	}

	memset( priv, 0, sizeof(egigaPriv) );

	/* init device methods */
	memcpy( dev->name, name, NAMESIZE );
	mvMacStrToHex(enet_addr, (MV_U8 *)(dev->enetaddr));

	/* set MAC addres even if port was not used yet. */
	dummy_port_handle.portNo = port;

#if defined(CONFIG_APBOOT) && !defined(CONFIG_MV_PORT_SWAP)
	if (port & 1) {
		//
		// this assumes that manufacturing always assigns an even
		// base MAC, which currently has to be true or the radio
		// driver has issues
		//
		// if we have more than two ports, we'll just ping-pong to
		// avoid incrementing the rest of the address; I don't think
		// this will happen anytime soon
		//
		dev->enetaddr[5] += 1;
	}
#endif
	mvEthMacAddrSet( &dummy_port_handle, dev->enetaddr, EGIGA_DEF_RXQ);

	dev->init = (void *)mvEgigaInit;
	dev->halt = (void *)mvEgigaHalt;
	dev->send = (void *)mvEgigaTx;
	dev->recv = (void *)mvEgigaRx;
	dev->priv = priv;
	dev->iobase = 0;
	priv->port = port;

#if defined(CONFIG_APBOOT) && defined(CONFIG_MV_PORT_SWAP)
	if (port == 1) {
#endif

#if defined(CONFIG_MII) || (CONFIG_COMMANDS & CFG_CMD_MII)
	miiphy_register(dev->name, mv_miiphy_read, mv_miiphy_write);
#endif
	/* register the interface */
	eth_register(dev);

#if defined(CONFIG_APBOOT) && defined(CONFIG_MV_PORT_SWAP)
	}
#endif

	DB( printf( "%s: %s load ok\n", __FUNCTION__, name ) );
	return 0;

	error:
	printf( "%s: %s load failed\n", __FUNCTION__, name );
	if( priv ) free( dev->priv );
	if( dev ) free( dev );
	return -1;
}


static MV_PKT_INFO* mvEgigaRxFill(MV_VOID)
{
	MV_BUF_INFO *pBufInfo;
	MV_PKT_INFO *pPktInfo;
	MV_U8 *buf = (MV_U8 *)memalign( 32, RX_BUFFER_SIZE ); /* align on 32B */
	if( !buf ) {
		DB(printf("failed to alloc buffer\n"));
		return NULL;
	}

	if( ((MV_U32)buf) & 0xf ) 
        	printf( "un-align rx buffer %x\n", (MV_U32)buf );

	pPktInfo = malloc(sizeof(MV_PKT_INFO));
	if (pPktInfo == NULL) {
		printf("Error: cannot allocate memory for pktInfo\n");
		free(buf);
		return NULL;
	}

	pBufInfo = malloc(sizeof(MV_BUF_INFO));	
	if (pBufInfo == NULL) {
		printf("Error: cannot allocate memory for bufInfo\n");
		free(buf);
		free(pPktInfo);
		return NULL;
	}
        pBufInfo->bufPhysAddr = mvOsIoVirtToPhy(NULL, buf);
        pBufInfo->bufVirtPtr = buf;
        pBufInfo->bufSize = RX_BUFFER_SIZE;
        pBufInfo->dataSize = 0;
        pPktInfo->osInfo = (MV_ULONG)buf;
        pPktInfo->pFrags = pBufInfo;
        pPktInfo->pktSize = RX_BUFFER_SIZE; /* how much to invalidate */
        pPktInfo->numFrags = 1;
        pPktInfo->status = 0;
        pPktInfo->ownerId = -1;
	return pPktInfo;
}

#if 0
static void
port_status(int port)
{
    unsigned port_status = MV_REG_READ( ETH_PORT_STATUS_REG( port ) );

    if (port_status & ETH_LINK_UP_MASK) {
	    printf("up");

        /* check port status register */
        printf(", %s",(port_status & ETH_FULL_DUPLEX_MASK) ? "full duplex" : "half duplex" );

        if (port_status & ETH_GMII_SPEED_1000_MASK)  
	        printf(", speed 1 Gbps");
        else 
            printf(", %s", (port_status & ETH_MII_SPEED_100_MASK) ? "speed 100 Mbps" : "speed 10 Mbps");
        printf("\n");
    } else {
	    printf("down\n");
    }
}
#endif

#if 0
MV_STATUS mvNetaSpeedDuplexSet(int portNo, MV_ETH_PORT_SPEED speed, MV_ETH_PORT_DUPLEX duplex)
{
	MV_U32 regVal;

#if 0
	if ((portNo < 0) || (portNo >= mvNetaHalData.maxPort))
		return MV_BAD_PARAM;
#endif

	/* Check validity */
	if ((speed == MV_ETH_SPEED_1000) && (duplex == MV_ETH_DUPLEX_HALF))
		return MV_BAD_PARAM;

	regVal = MV_REG_READ(NETA_GMAC_AN_CTRL_REG(portNo));

	switch (speed) {
	case MV_ETH_SPEED_AN:
		regVal |= NETA_ENABLE_SPEED_AUTO_NEG_MASK;
		/* the other bits don't matter in this case */
		break;
	case MV_ETH_SPEED_1000:
		regVal &= ~NETA_ENABLE_SPEED_AUTO_NEG_MASK;
		regVal |= NETA_SET_GMII_SPEED_1000_MASK;
		regVal &= ~NETA_SET_MII_SPEED_100_MASK;
		/* the 100/10 bit doesn't matter in this case */
		break;
	case MV_ETH_SPEED_100:
		regVal &= ~NETA_ENABLE_SPEED_AUTO_NEG_MASK;
		regVal &= ~NETA_SET_GMII_SPEED_1000_MASK;
		regVal |= NETA_SET_MII_SPEED_100_MASK;
		break;
	case MV_ETH_SPEED_10:
		regVal &= ~NETA_ENABLE_SPEED_AUTO_NEG_MASK;
		regVal &= ~NETA_SET_GMII_SPEED_1000_MASK;
		regVal &= ~NETA_SET_MII_SPEED_100_MASK;
		break;
	default:
		mvOsPrintf("Unexpected Speed value %d\n", speed);
		return MV_BAD_PARAM;
	}

	switch (duplex) {
	case MV_ETH_DUPLEX_AN:
		regVal  |= NETA_ENABLE_DUPLEX_AUTO_NEG_MASK;
		/* the other bits don't matter in this case */
		break;
	case MV_ETH_DUPLEX_HALF:
		regVal &= ~NETA_ENABLE_DUPLEX_AUTO_NEG_MASK;
		regVal &= ~NETA_SET_FULL_DUPLEX_MASK;
		break;
	case MV_ETH_DUPLEX_FULL:
		regVal &= ~NETA_ENABLE_DUPLEX_AUTO_NEG_MASK;
		regVal |= NETA_SET_FULL_DUPLEX_MASK;
		break;
	default:
		mvOsPrintf("Unexpected Duplex value %d\n", duplex);
		return MV_BAD_PARAM;
	}

//printf("%x %x\n", NETA_GMAC_AN_CTRL_REG(portNo), regVal);
	MV_REG_WRITE(NETA_GMAC_AN_CTRL_REG(portNo), regVal);

	mvOsDelay(200);

	return MV_OK;
}
#endif

static void
eth_set_port_to_phy(egigaPriv *priv)
{
#if 0
	unsigned phyAddr = mvBoardPhyAddrGet(priv->port);
	MV_ETH_PORT_SPEED speed;
	MV_ETH_PORT_DUPLEX duplex;
	MV_U16 val;
	extern MV_STATUS mvNetaSpeedDuplexSet(int portNo, MV_ETH_PORT_SPEED speed, MV_ETH_PORT_DUPLEX duplex);

	if (mvEthPhyRegRead(phyAddr, ETH_PHY_SPEC_STATUS_REG, &val) != MV_OK) {
		return;
	}

	switch (val & ETH_PHY_SPEC_STATUS_SPEED_MASK) {
	default:
	case ETH_PHY_SPEC_STATUS_SPEED_1000MBPS:
		speed = MV_ETH_SPEED_1000;
		break;
	case ETH_PHY_SPEC_STATUS_SPEED_100MBPS:
		speed = MV_ETH_SPEED_100;
		break;
	case ETH_PHY_SPEC_STATUS_SPEED_10MBPS:
		speed = MV_ETH_SPEED_10;
		break;
	}
	if (val & ETH_PHY_SPEC_STATUS_DUPLEX_MASK) {
	    duplex = MV_ETH_DUPLEX_FULL;
	} else {
	    duplex = MV_ETH_DUPLEX_HALF;
	}

//printf("%x %x\n", speed, duplex);
	mvNetaSpeedDuplexSet(priv->port, speed, duplex);
#endif
}

unsigned int egiga_init=0;
MV_STATUS      mvEthPhyStatus( MV_U32 phyAddr, char *buf, unsigned len );

static int mvEgigaInit( struct eth_device *dev, bd_t *p )
{
	egigaPriv *priv = dev->priv;
	MV_ETH_PORT_INIT halInitStruct;
	MV_PKT_INFO *pktInfo;
	MV_STATUS status;
	int i;
	MV_BOARD_MAC_SPEED speed;

	DB( printf( "%s: %s init - ", __FUNCTION__, dev->name ) );

	/* egiga not ready */
	DB( printf ("mvBoardPhyAddrGet()=0x%x , priv->port =0x%x\n",mvBoardPhyAddrGet(priv->port),priv->port));

	/* If speed is not auto then link is force */
	if (BOARD_MAC_SPEED_AUTO == mvBoardMacSpeedGet(priv->port)
#if defined(CONFIG_APBOOT) && defined(ARUBA_MARVELL_SWITCH)
		|| BOARD_MAC_SPEED_1000M == mvBoardMacSpeedGet(priv->port)
#endif
	)
	{
		/* Check Link status on phy */
		if( mvEthPhyCheckLink( mvBoardPhyAddrGet(priv->port) ) == MV_FALSE ) {
			printf( "%s: down\n", dev->name );
			return 0;
		}
		else {
			char buf[64];
			DB( printf( "link up\n" ) );
			mvEthPhyStatus(mvBoardPhyAddrGet(priv->port), buf, sizeof(buf));
			printf( "%s: up, %s\n", dev->name, buf);
		}
	}

	egiga_init = 1;

	/* init the hal -- create internal port control structure and descriptor rings, */
	/* open address decode windows, disable rx and tx operations. mask interrupts.  */
//	halInitStruct.maxRxPktSize = RX_BUFFER_SIZE;
	halInitStruct.rxDefQ = EGIGA_DEF_RXQ;

	halInitStruct.txDescrNum[0] = EGIGA_TXQ_LEN;
	halInitStruct.rxDescrNum[0] = EGIGA_RXQ_LEN;
	halInitStruct.osHandle = NULL;

	priv->halPriv = mvEthPortInit( priv->port, &halInitStruct );

	if( !priv->halPriv ) {
		DB( printf( "falied to init eth port (error)\n" ) );
		goto error;
	}

	mvEthPhyAddrSet(priv->halPriv, mvBoardPhyAddrGet(priv->port));
	
    /* set new addr in hw */
    if( mvEthMacAddrSet( priv->halPriv, dev->enetaddr, EGIGA_DEF_RXQ) != MV_OK ) {
        printf("%s: ethSetMacAddr failed\n", dev->name );
	    goto error;
    }

	if (BOARD_MAC_SPEED_AUTO == mvBoardMacSpeedGet(priv->port)) {
		/* force port MAC settings to what the PHY reported */
		eth_set_port_to_phy(priv);
	}
	priv->devInit = MV_TRUE;

	/* fill rx ring with buffers */
	for( i=0; i<EGIGA_RXQ_LEN; i++ ) {

		pktInfo = mvEgigaRxFill();
		if (pktInfo == NULL)
			goto error;

		/* give the buffer to hal */
		status = mvEthPortRxDone( priv->halPriv, EGIGA_DEF_RXQ, pktInfo );
		if( status == MV_OK ) {
			priv->rxqCount++;
		}
		else if( status == MV_FULL ) {
			/* the ring is full */
			priv->rxqCount++;
			DB( printf( "ring full\n" ) );
			break;
		}
		else {
			printf( "error\n" );
			goto error;
		}
	}

#ifdef MV_DEBUG
	ethPortQueues(priv->port, EGIGA_DEF_RXQ, EGIGA_DEF_TXQ, 1);

	printf("%s : after calling ethPortQueues\n",__FUNCTION__);

#endif

	speed = mvBoardMacSpeedGet(priv->port);
//printf("got speed %u\n", speed);
#ifdef ARUBA_MARVELL_SWITCH
	if(speed != BOARD_MAC_SPEED_AUTO) {
		uint32_t reg;
		reg = MV_REG_READ(NETA_GMAC_AN_CTRL_REG(priv->port));
		reg |= (NETA_FORCE_LINK_PASS_MASK | NETA_SET_FULL_DUPLEX_MASK);
		if(speed == BOARD_MAC_SPEED_1000M) {
			reg |= NETA_SET_GMII_SPEED_1000_MASK;
		} else {
			reg &= ~(NETA_SET_GMII_SPEED_1000_MASK) ;
			reg |= NETA_SET_MII_SPEED_100_MASK;
		}
		reg &= ~(NETA_ENABLE_SPEED_AUTO_NEG_MASK | NETA_ENABLE_DUPLEX_AUTO_NEG_MASK);
		MV_REG_WRITE(NETA_GMAC_AN_CTRL_REG(priv->port), reg);
	} else {
		uint32_t reg;
		reg = MV_REG_READ(NETA_GMAC_AN_CTRL_REG(priv->port));
		reg &= ~(NETA_FORCE_LINK_PASS_MASK | NETA_SET_FULL_DUPLEX_MASK);
		reg |= (NETA_ENABLE_SPEED_AUTO_NEG_MASK | NETA_ENABLE_DUPLEX_AUTO_NEG_MASK);
		MV_REG_WRITE(NETA_GMAC_AN_CTRL_REG(priv->port), reg);
	}
#endif

	/* start the hal - rx/tx activity */
	/* Check if link is up for 2 Sec */
	for (i = 1; i < 100 ; i ++)
	{
		status = mvEthPortEnable( priv->halPriv );
		if (status == MV_OK)
			break;
		mvOsDelay(20);
	}
	
	if( status != MV_OK ) {
		printf( "%s: %s mvEthPortEnable failed (error)\n", __FUNCTION__, dev->name );
		goto error;
	}

#ifdef MV_DEBUG
	ethRegs(priv->port);
	ethPortRegs(priv->port);
	ethPortStatus(priv->port);

	ethPortQueues(priv->port, EGIGA_DEF_RXQ, -1/*EGIGA_DEF_TXQ*/, 0);
#endif

//port_status(priv->port);
//printf("ST 0 %x\n", MV_REG_READ(ETH_PORT_STATUS_REG(priv->port)));
//printf("SC 0 %x\n", MV_REG_READ(ETH_PORT_SERIAL_CTRL_REG(priv->port)));
//printf("SC 1 %x\n", MV_REG_READ(ETH_PORT_SERIAL_CTRL_1_REG(priv->port)));

	DB( printf( "%s: %s complete ok\n", __FUNCTION__, dev->name ) );
	return 1;

	error:
	if( priv->devInit )
		mvEgigaHalt( dev );
	printf( "%s: %s failed\n", __FUNCTION__, dev->name );
	return 0;
}

static int mvEgigaHalt( struct eth_device *dev )
{

	egigaPriv *priv = dev->priv;
	MV_PKT_INFO *pktInfo;

	DB( printf( "%s: %s halt - ", __FUNCTION__, dev->name ) );
	if( priv->devInit == MV_TRUE ) {

		/* stop the port activity, mask all interrupts */
		if( mvEthPortDisable( priv->halPriv ) != MV_OK ) 
			printf( "mvEthPortDisable failed (error)\n" );
    
		/* free the buffs in the rx ring */
		while( (pktInfo = mvEthPortForceRx( priv->halPriv, EGIGA_DEF_RXQ )) != NULL ) {
			priv->rxqCount--;
			if( pktInfo->osInfo )
				free( (void *)pktInfo->osInfo );
			else 
				printf( "mvEthPortForceRx failed (error)\n" );
			if( pktInfo->pFrags )
				free( (void *)pktInfo->pFrags );
			else
 				printf( "mvEthPortForceRx failed (error)\n" );
            free( (void *)pktInfo );
		}

   		/* Clear Cause registers (must come before mvEthPortFinish) */
    	MV_REG_WRITE(ETH_INTR_CAUSE_REG(((ETH_PORT_CTRL*)(priv->halPriv))->portNo),0);
    	MV_REG_WRITE(ETH_INTR_CAUSE_EXT_REG(((ETH_PORT_CTRL*)(priv->halPriv))->portNo),0);

   		/* Clear Cause registers */
    		MV_REG_WRITE(ETH_INTR_CAUSE_REG(((ETH_PORT_CTRL*)(priv->halPriv))->portNo),0);
    		MV_REG_WRITE(ETH_INTR_CAUSE_EXT_REG(((ETH_PORT_CTRL*)(priv->halPriv))->portNo),0);

		mvEthPortFinish( priv->halPriv );
		priv->devInit = MV_FALSE;

	}
	egiga_init = 0;

	DB( printf( "%s: %s complete\n", __FUNCTION__, dev->name ) );
	return 0;
}

int mvEgigaTx( struct eth_device *dev, volatile void *buf, int len )
{
	egigaPriv *priv = dev->priv;
	MV_BUF_INFO bufInfo;
    	MV_PKT_INFO pktInfo;
    	MV_PKT_INFO *pPktInfo;
	MV_STATUS status;
	MV_U32 timeout = 0;

	DB( printf( "mvEgigaTx start\n" ) );
	/* if no link exist */
	if(!egiga_init)
		return 0;

    pktInfo.osInfo = (MV_ULONG)0x44CAFE44;
    pktInfo.pktSize = len;
    pktInfo.pFrags = &bufInfo;
    pktInfo.status = 0;
    pktInfo.numFrags = 1;
    bufInfo.bufVirtPtr = (MV_U8*)buf;
    bufInfo.bufPhysAddr = mvOsIoVirtToPhy(NULL, (void *)buf);
    bufInfo.dataSize = len;

	/* send the packet */
	status = mvEthPortTx( priv->halPriv, EGIGA_DEF_TXQ, &pktInfo );

	if( status != MV_OK ) {
		if( status == MV_NO_RESOURCE )
			DB( printf( "ring is full (error)\n" ) );
		else if( status == MV_ERROR )
			printf( "error\n" );
		else
			printf( "unrecognize status (error) ethPortSend\n" );
		goto error;
	} 
	else DB( printf( "ok\n" ) );

	priv->txqCount++;

	/* release the transmitted packet(s) */
	while( 1 ) {

		if(timeout++ > 100)
			break;
		DB( printf( "%s: %s tx-done - ", __FUNCTION__, dev->name ) );

		/* get a packet */
		pPktInfo = mvEthPortTxDone( priv->halPriv, EGIGA_DEF_TXQ);

		if( pPktInfo != NULL ) {

			priv->txqCount--;

			/* validate skb */
			if( (pPktInfo != &pktInfo) || (pPktInfo->osInfo != 0x44CAFE44 ) ) {
				printf( "error\n" );
				goto error;
			}

			/* handle tx error */
			if( pPktInfo->status & (ETH_ERROR_SUMMARY_BIT) ) {
				printf( "bad status (error)\n" );
				goto error;
			}
			DB( printf( "ok\n" ) );
			break;
		}
		else 
			DB(printf( "NULL pPktInfo\n" ));
	}

	DB( printf( "%s: %s complete ok\n", __FUNCTION__, dev->name ) );
	return 0;

	error:
	printf( "%s: %s failed\n", __FUNCTION__, dev->name );
	return 1;
}


static int mvEgigaRx( struct eth_device *dev )
{
	egigaPriv*  priv = dev->priv;
    	MV_PKT_INFO *pktInfo;
	MV_STATUS   status;

	/* if no link exist */
	if(!egiga_init) return 0;

	while( 1 ) {

		/* get rx packet from hal */
		pktInfo = mvEthPortRx( priv->halPriv, EGIGA_DEF_RXQ);

		if( pktInfo != NULL ) {
			/*DB( printf( "good rx\n" ) );*/
			priv->rxqCount--;

			/* check rx error status */
			if( pktInfo->status & (ETH_ERROR_SUMMARY_MASK) ) {
				MV_U32 err = pktInfo->status & ETH_RX_ERROR_CODE_MASK;
				/*DB( printf( "bad rx status %08x, ", (MV_U32)pktInfo->status ) );*/
				if( err == ETH_RX_RESOURCE_ERROR )
					DB( printf( "(resource error)" ) );
				else if( err == ETH_RX_MAX_FRAME_LEN_ERROR )
					DB( printf( "(max frame length error)" ) );
				else if( err == ETH_RX_OVERRUN_ERROR )
					DB( printf( "(overrun error)" ) );
				else if( err == ETH_RX_CRC_ERROR )
					DB( printf( "(crc error)" ) );
				else {
					DB( printf( "(unknown error)" ) );
					goto error;
				}
				DB( printf( "\n" ) );
			}
			else {

				DB( printf( "%s: %s calling NetRecieve ", __FUNCTION__, dev->name) );
				DB( printf( "%s: calling NetRecieve pkInfo = 0x%x\n", __FUNCTION__, pktInfo) );
				DB( printf( "%s: calling NetRecieve osInfo = 0x%x\n", __FUNCTION__, pktInfo->osInfo) );
				DB( printf( "%s: calling NetRecieve pktSize = 0x%x\n", __FUNCTION__, pktInfo->pFrags->dataSize) );
				/* good rx - push the packet up (skip on two first empty bytes) */
				NetReceive( ((MV_U8 *)pktInfo->osInfo) + 2, (int)pktInfo->pFrags->dataSize);
			}


			DB( printf( "%s: %s refill rx buffer - ", __FUNCTION__, dev->name) );

			/* give the buffer back to hal (re-init the buffer address) */
	        	pktInfo->pktSize = RX_BUFFER_SIZE; /* how much to invalidate */
			status = mvEthPortRxDone( priv->halPriv, EGIGA_DEF_RXQ, pktInfo );
	
			if( status == MV_OK ) {
				priv->rxqCount++;
			}
			else if( status == MV_FULL ) {
				/* this buffer made the ring full */
				priv->rxqCount++;
				DB( printf( "ring full\n" ) );
				break;
			} 
			else {
				printf( "error\n" );
				goto error;
			}

		} else {
			/* no more rx packets ready */
			/*DB( printf( "no more work\n" ) );*/
			break;
		}
	}

	/*DB( printf( "%s: %s complete ok\n", __FUNCTION__, dev->name ) );*/
	return 0;

	error:
	DB( printf( "%s: %s failed\n", __FUNCTION__, dev->name ) );
	return 1;
}

int mvEgigaRx_test( struct eth_device *dev, uchar *pkt )
{
	egigaPriv*  priv = dev->priv;
    	MV_PKT_INFO *pktInfo;
	MV_STATUS   status;
	int len = 0;

	/* if no link exist */
	if(!egiga_init) return 0;

	while( 1 ) {

		/* get rx packet from hal */
		pktInfo = mvEthPortRx( priv->halPriv, EGIGA_DEF_RXQ);

		if( pktInfo != NULL ) {
			/*DB( printf( "good rx\n" ) );*/
			priv->rxqCount--;

			/* check rx error status */
			if( pktInfo->status & (ETH_ERROR_SUMMARY_MASK) ) {
				MV_U32 err = pktInfo->status & ETH_RX_ERROR_CODE_MASK;
				/*DB( printf( "bad rx status %08x, ", (MV_U32)pktInfo->cmdSts ) );*/
				if( err == ETH_RX_RESOURCE_ERROR )
					DB( printf( "(resource error)" ) );
				else if( err == ETH_RX_MAX_FRAME_LEN_ERROR )
					DB( printf( "(max frame length error)" ) );
				else if( err == ETH_RX_OVERRUN_ERROR )
					DB( printf( "(overrun error)" ) );
				else if( err == ETH_RX_CRC_ERROR )
					DB( printf( "(crc error)" ) );
				else {
					DB( printf( "(unknown error)" ) );
					goto error;
				}
				DB( printf( "\n" ) );
			}
			else {

				DB( printf( "%s: %s calling NetRecieve ", __FUNCTION__, dev->name) );
				DB( printf( "%s: calling NetRecieve pkInfo = 0x%x\n", __FUNCTION__, pktInfo) );
				DB( printf( "%s: calling NetRecieve osInfo = 0x%x\n", __FUNCTION__, pktInfo->osInfo) );
				DB( printf( "%s: calling NetRecieve pktSize = 0x%x\n", __FUNCTION__, pktInfo->pFrags->dataSize) );
				/* good rx - push the packet up (skip on two first empty bytes) */
				memcpy( pkt, ((MV_U8 *)pktInfo->osInfo) + 2, (int)pktInfo->pFrags->dataSize);
				len = (int)pktInfo->pFrags->dataSize;
//				NetReceive( ((MV_U8 *)pktInfo->osInfo) + 2, (int)pktInfo->pFrags->dataSize);
			}


			DB( printf( "%s: %s refill rx buffer - ", __FUNCTION__, dev->name) );

			/* give the buffer back to hal (re-init the buffer address) */
	        	pktInfo->pktSize = RX_BUFFER_SIZE; /* how much to invalidate */
			status = mvEthPortRxDone( priv->halPriv, EGIGA_DEF_RXQ, pktInfo );
	
			if( status == MV_OK ) {
				priv->rxqCount++;
			}
			else if( status == MV_FULL ) {
				/* this buffer made the ring full */
				priv->rxqCount++;
				DB( printf( "ring full\n" ) );
				break;
			} 
			else {
				printf( "error\n" );
				goto error;
			}

		} else {
			/* no more rx packets ready */
			/* DB( printf( "no more work\n" ) ); */
			return 0;
		}
	}

	/*DB( printf( "%s: %s complete ok\n", __FUNCTION__, dev->name ) );*/
	return len;

	error:
	DB( printf( "%s: %s failed\n", __FUNCTION__, dev->name ) );
	return 0;
}
#endif /* #if defined (MV_INCLUDE_GIG_ETH) */
