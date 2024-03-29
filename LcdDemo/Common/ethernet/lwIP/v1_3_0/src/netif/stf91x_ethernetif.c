/**
 * <b>File:</b> str91x_ethernetif.c
 *
 * <b>Project:</b> STR91x Eclipse demo
 *
 * <b>Description:</b> This module is used to implement a network interface bridge between
 *                     the STR91x MAC peripheral and lwIP stack.
 *
 *
 * <b>Created:</b> 27/08/2008
 *
 * <dl>
 * <dt><b>Author</b>:</dt>
 * <dd>Stefano Oliveri</dd>
 * <dt><b>E-mail:</b></dt>
 * <dd>software@stf12.net</dd>
 * </dl>
 */

/*
 * This file is a skeleton for developing Ethernet network interface
 * drivers for lwIP. Add code to the low_level functions and do a
 * search-and-replace for the word "ethernetif" to replace it with
 * something that better describes your network interface.
 */

#include "lwip/def.h"
#include "lwip/mem.h"
#include "lwip/pbuf.h"
#include "lwip/sys.h"
#include <lwip/stats.h>
#include <lwip/snmp.h>
#include "netif/etharp.h"

// ST Library include
#include "91x_enet.h"

// FreeRTOS include
#include "task.h"

// Standard library include
#include <stdio.h>
#include <string.h>

#define netifMTU													( 1500 )
#define netifINTERFACE_TASK_STACK_SIZE		( 350 )
#define netifINTERFACE_TASK_PRIORITY			( configMAX_PRIORITIES - 1 )
#define netifBUFFER_WAIT_ATTEMPTS					10
#define netifBUFFER_WAIT_DELAY						(10 / portTICK_RATE_MS)
#define IFNAME0 'e'
#define IFNAME1 'n'

/* The time to block waiting for input. */
#define emacBLOCK_TIME_WAITING_FOR_INPUT	( ( portTickType ) 100 )

/* Interrupt status bit definition. */
#define DMI_RX_CURRENT_DONE 0x8000

static u8_t s_rxBuff[1520];

/* The semaphore used by the ISR to wake the lwIP task. */
static xSemaphoreHandle s_xSemaphore = NULL;

struct ethernetif {
  struct eth_addr *ethaddr;
  /* Add whatever per-interface state that is needed here. */
};

/* Forward declarations. */
static void ethernetif_input( void *pParams );
u8 *pcGetNextBuffer( void );

/**
 * In this function, the hardware should be initialized.
 * Called from ethernetif_init().
 *
 * @param netif the already initialized lwip network interface structure
 *        for this ethernetif
 */
static void low_level_init(struct netif *netif)
{
  /* set MAC hardware address length */
  netif->hwaddr_len = ETHARP_HWADDR_LEN;

  /* set MAC hardware address */
  netif->hwaddr[0] = MAC_ADDR0;
  netif->hwaddr[1] = MAC_ADDR1;
  netif->hwaddr[2] = MAC_ADDR2;
  netif->hwaddr[3] = MAC_ADDR3;
  netif->hwaddr[4] = MAC_ADDR4;
  netif->hwaddr[5] = MAC_ADDR5;

  /* maximum transfer unit */
  netif->mtu = netifMTU;

  // device capabilities.
	// don't set NETIF_FLAG_ETHARP if this device is not an ethernet one
  netif->flags |= NETIF_FLAG_BROADCAST | NETIF_FLAG_ETHARP;

  /* Do whatever else is needed to initialize interface. */

  if( s_xSemaphore == NULL )
  {
      vSemaphoreCreateBinary( s_xSemaphore );
      xSemaphoreTake( s_xSemaphore,  0);
  }

  /* Initialise the MAC. */
  ENET_InitClocksGPIO();
  ENET_Init(PHY_AUTO_NEGOTIATION);
  ENET_Start();

  portENTER_CRITICAL();
  {
      /*set MAC physical*/
      ENET_MAC->MAH = (MAC_ADDR5<<8) + MAC_ADDR4;
      ENET_MAC->MAL = (MAC_ADDR3<<24) + (MAC_ADDR2<<16) + (MAC_ADDR1<<8) + MAC_ADDR0;

      VIC_Config( ENET_ITLine, VIC_IRQ, 1 );
      VIC_ITCmd( ENET_ITLine, ENABLE );
      ENET_DMA->ISR = DMI_RX_CURRENT_DONE;
      ENET_DMA->IER = DMI_RX_CURRENT_DONE;
  }
  portEXIT_CRITICAL();

  netif->flags |= NETIF_FLAG_LINK_UP;

  /* Create the task that handles the EMAC. */
  xTaskCreate( ethernetif_input, ( signed portCHAR * ) "ETH_INT", netifINTERFACE_TASK_STACK_SIZE, (void *)netif, netifINTERFACE_TASK_PRIORITY, NULL );
}


/**
 * This function should do the actual transmission of the packet. The packet is
 * contained in the pbuf that is passed to the function. This pbuf
 * might be chained.
 *
 * @param netif the lwip network interface structure for this ethernetif
 * @param p the MAC packet to send (e.g. IP packet including MAC addresses and type)
 * @return ERR_OK if the packet could be sent
 *         an err_t value if the packet couldn't be sent
 *
 * @note Returning ERR_MEM here if a DMA queue of your MAC is full can lead to
 *       strange results. You might consider waiting for space in the DMA queue
 *       to become availale since the stack doesn't retry to send a packet
 *       dropped because of memory failure (except for the TCP timers).
 */
static err_t low_level_output(struct netif *netif, struct pbuf *p)
{
  struct pbuf *q;
  int i;
  u32_t l = 0;
  unsigned portCHAR *pcTxData;

#if ETH_PAD_SIZE
  pbuf_header(p, -ETH_PAD_SIZE);			/* drop the padding word */
#endif

	/* Get a DMA buffer into which we can write the data to send. */
	for( i=0; i < netifBUFFER_WAIT_ATTEMPTS; i++ )
	{
		pcTxData = pcGetNextBuffer();

		if( pcTxData ){
			break;
		} else {
			vTaskDelay( netifBUFFER_WAIT_DELAY );
		}
	}

	if (pcTxData == NULL) {
		portNOP();

		return ERR_BUF;
	}
	else {

		for(q = p; q != NULL; q = q->next) {
			/* Send the data from the pbuf to the interface, one pbuf at a
				 time. The size of the data in each pbuf is kept in the ->len
				 variable. */

			vTaskSuspendAll();
			memcpy(&pcTxData[l], (u8_t*)q->payload, q->len);
			xTaskResumeAll();
			l += q->len;
		}
	}

	ENET_TxPkt( &pcTxData, l );

	#if ETH_PAD_SIZE
		pbuf_header(p, ETH_PAD_SIZE);			/* reclaim the padding word */
	#endif

		LINK_STATS_INC(link.xmit);

  return ERR_OK;
}

/**
 * Should allocate a pbuf and transfer the bytes of the incoming
 * packet from the interface into the pbuf.
 *
 * @param netif the lwip network interface structure for this ethernetif
 * @return a pbuf filled with the received packet (including MAC header)
 *         NULL on memory error
 */
static struct pbuf *low_level_input(struct netif *netif)
{
  struct pbuf *p, *q;
  u16_t len, l;

  l = 0;
  p = NULL;


	/* Obtain the size of the packet and put it into the "len"
		 variable. */
	len = ENET_HandleRxPkt(s_rxBuff);

	if(len)
	{
	#if ETH_PAD_SIZE
		len += ETH_PAD_SIZE;						/* allow room for Ethernet padding */
	#endif

		/* We allocate a pbuf chain of pbufs from the pool. */
		p = pbuf_alloc(PBUF_RAW, len, PBUF_POOL);

		if (p != NULL) {

	#if ETH_PAD_SIZE
			pbuf_header(p, -ETH_PAD_SIZE);			/* drop the padding word */
	#endif

			/* We iterate over the pbuf chain until we have read the entire
			 * packet into the pbuf. */
			for(q = p; q != NULL; q = q->next) {
				/* Read enough bytes to fill this pbuf in the chain. The
				 * available data in the pbuf is given by the q->len
				 * variable. */
				vTaskSuspendAll();
				memcpy((u8_t*)q->payload, &s_rxBuff[l], q->len);
				xTaskResumeAll();
				l = l + q->len;
			}


	#if ETH_PAD_SIZE
			pbuf_header(p, ETH_PAD_SIZE);			/* reclaim the padding word */
	#endif

			LINK_STATS_INC(link.recv);

		} else {

			LINK_STATS_INC(link.memerr);
			LINK_STATS_INC(link.drop);

		} /* End else */
	} /* End if */

  return p;
}

/**
 * This function should be called when a packet is ready to be read
 * from the interface. It uses the function low_level_input() that
 * should handle the actual reception of bytes from the network
 * interface.Then the type of the received packet is determined and
 * the appropriate input function is called.
 *
 * @param netif the lwip network interface structure for this ethernetif
 */

static void ethernetif_input( void *pParams )
{
	struct netif *netif;
  struct ethernetif *ethernetif;
  struct eth_hdr *ethhdr;
  struct pbuf *p;

	netif = (struct netif*) pParams;
	ethernetif = netif->state;

	for( ;; )
	{
		do
		{

			/* move received packet into a new pbuf */
			p = low_level_input( netif );

			if( p == NULL )
			{
				/* No packet could be read.  Wait a for an interrupt to tell us
				there is more data available. */
				xSemaphoreTake( s_xSemaphore, emacBLOCK_TIME_WAITING_FOR_INPUT );
			}

		} while( p == NULL );

		/* points to packet payload, which starts with an Ethernet header */
		ethhdr = p->payload;

		switch (htons(ethhdr->type)) {
			/* IP or ARP packet? */

		case ETHTYPE_IP:

			/* full packet send to tcpip_thread to process */
			if (netif->input(p, netif) != ERR_OK)
			{
				LWIP_DEBUGF(NETIF_DEBUG, ("ethernetif_input: IP input error\n"));
				pbuf_free(p);
				p = NULL;
			}
			break;

		case ETHTYPE_ARP:

#if ETHARP_TRUST_IP_MAC
			etharp_ip_input(netif, p);
#endif

			etharp_arp_input(netif, ethernetif->ethaddr, p);
			break;

		default:
			pbuf_free(p);
			p = NULL;
			break;
		}
	}
}

/**
 * Should be called at the beginning of the program to set up the
 * network interface. It calls the function low_level_init() to do the
 * actual setup of the hardware.
 *
 * This function should be passed as a parameter to netif_add().
 *
 * @param netif the lwip network interface structure for this ethernetif
 * @return ERR_OK if the loopif is initialized
 *         ERR_MEM if private data couldn't be allocated
 *         any other err_t on error
 */
err_t
ethernetif_init(struct netif *netif)
{
  struct ethernetif *ethernetif;

	LWIP_ASSERT("netif != NULL", (netif != NULL));

  ethernetif = mem_malloc(sizeof(struct ethernetif));

  if (ethernetif == NULL)
  {
  	LWIP_DEBUGF(NETIF_DEBUG, ("ethernetif_init: out of memory\n"));
  	return ERR_MEM;
  }

#if LWIP_NETIF_HOSTNAME
  /* Initialize interface hostname */
  netif->hostname = "lwip";
#endif /* LWIP_NETIF_HOSTNAME */

  /*
	 * Initialize the snmp variables and counters inside the struct netif.
	 * The last argument should be replaced with your link speed, in units
	 * of bits per second.
	 */
	NETIF_INIT_SNMP(netif, snmp_ifType_ethernet_csmacd, 100);

  netif->state = ethernetif;
  netif->name[0] = IFNAME0;
  netif->name[1] = IFNAME1;
  /* We directly use etharp_output() here to save a function call.
	 * You can instead declare your own function an call etharp_output()
	 * from it if you have to do some checks before sending (e.g. if link
	 * is available...)
	 */
  netif->output = etharp_output;
  netif->linkoutput = low_level_output;

  ethernetif->ethaddr = (struct eth_addr *)&(netif->hwaddr[0]);

  low_level_init(netif);

  return ERR_OK;
}
/*-----------------------------------------------------------*/

void ENET_IRQHandler(void)
{
portBASE_TYPE xSwitchRequired;

	/* Give the semaphore in case the lwIP task needs waking. */
	xSwitchRequired = xSemaphoreGiveFromISR( s_xSemaphore, &xSwitchRequired );

	/* Clear the interrupt. */
	ENET_DMA->ISR = DMI_RX_CURRENT_DONE;

	/* Switch tasks if necessary. */
	portEND_SWITCHING_ISR( xSwitchRequired );
}
/*-----------------------------------------------------------*/
