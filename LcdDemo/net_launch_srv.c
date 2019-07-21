/**
 * <b>File:</b> net_launch_srv.c
 *
 * <b>Project:</b> Remote Calculator Demo
 *
 * <b>Description:</b> This module implements the public vStartEthernetTasks
 * function that starts all taks needed by the demo.<br/>
 * The TCP/IP stack task is started first, than the web server and the remote
 * calculator task are started.
 *
 * <b>Cereated:</b> 27/11/2007
 *
 * <dl>
 * <dt><b>Autor</b>:</dt>
 * <dd>Stefano Oliveri</dd>
 * <dt><b>E-mail:</b></dt>
 * <dd>stefano.oliveri@st.com</dd>
 * </dl>
 */

// Standard include files
#include <string.h>

// Scheduler include files.
#include "FreeRTOS.h"
#include "task.h"

// Demo program include files.
#include "net_launch_srv.h"
#include "lwipWebServer/httpd.h"
#include "lwipNetCalc/NetCalcApp.h"

// lwIP includes
#include "lwip/sys.h"
#include "lwip/api.h"
#include "lwip/tcpip.h"
#include "lwip/memp.h"
#include "lwip/stats.h"
#include "netif/loopif.h"
#include "lwip/dhcp.h"


#define HTTPD_STACK_SIZE			400
#define RC_STACK_SIZE					250


/**
 * Local variable containing MAC configuration for lwIP (hw addr, IP, GW, ...).
 */
static struct netif s_EMAC_if;


static void prvlwIPInit( void );

static void prvEthernetConfigureInterface(void * param);

/**
 * Create all Ethernet tasks.
 *
 * @param uxPriority Input. Specifies the priority for the task suggested by
 * the application.
 */
void vStartEthernetTasks( unsigned portBASE_TYPE uxPriority )
{
  	// Setup lwIP.
  	prvlwIPInit();

  	// Create the WEB server task.  This uses the lwIP RTOS abstraction layer.
  	sys_thread_new( "httpd", httpd, ( void * ) NULL, HTTPD_STACK_SIZE, uxPriority );

	// Create the Remote Calculator demo task.
	sys_thread_new( "n_calc", NetworkCalculatorLoop, ( void * ) NULL, RC_STACK_SIZE, uxPriority );
}


/**
 * Initialization required by lwIP.
 */
static void prvlwIPInit( void )
{
	// once TCP stack has been initalized, set hw and IP parameters, initialize MAC too.
	tcpip_init( prvEthernetConfigureInterface, NULL );
}

/**
 * Initialize the Ethernet interface and notifies it to lwIP.
 *
 * @param param not used.
 */
static void prvEthernetConfigureInterface(void * param)
{
struct ip_addr xIpAddr, xNetMast, xGateway;
extern err_t ethernetif_init( struct netif *netif );

	/* Parameters are not used - suppress compiler error. */
	( void ) param;

	/* Create and configure the EMAC interface. */
	IP4_ADDR( &xIpAddr, emacIPADDR0, emacIPADDR1, emacIPADDR2, emacIPADDR3 );
	IP4_ADDR( &xNetMast, emacNET_MASK0, emacNET_MASK1, emacNET_MASK2, emacNET_MASK3 );
	IP4_ADDR( &xGateway, emacGATEWAY_ADDR0, emacGATEWAY_ADDR1, emacGATEWAY_ADDR2, emacGATEWAY_ADDR3 );
	netif_add( &s_EMAC_if, &xIpAddr, &xNetMast, &xGateway, NULL, ethernetif_init, tcpip_input );


	/* bring it up */

	// comment this line to use DHCP











	netif_set_up(&s_EMAC_if);

	// un-comment this line to use DHCP. TODO: the support to DHCP has to be
	// completed. See the file lwip/src/core/dhcp.c
	//err_t res = dhcp_start(&s_EMAC_if);

	/* make it the default interface */
	netif_set_default(&s_EMAC_if);
}



