/**
 * <b>File:</b> net_launc_srv.h
 *
 * <b>Project:</b> Remote Calculator Demo
 *
 * <b>Description:</b> This module declares the public vStartEthernetTasks
 * function that starts all taks needed by the demo.<br/>
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

#ifndef ETHERNET_H
#define ETHERNET_H

// The IP address being used.
#define emacIPADDR0		192
#define emacIPADDR1 	168
#define emacIPADDR2   	0
#define emacIPADDR3   	88

// The gateway address being used.
#define emacGATEWAY_ADDR0   192
#define emacGATEWAY_ADDR1   168
#define emacGATEWAY_ADDR2   0
#define emacGATEWAY_ADDR3   253

// The network mask being used.
#define emacNET_MASK0 		255
#define emacNET_MASK1 		255
#define emacNET_MASK2 		255
#define emacNET_MASK3 		0

void vStartEthernetTasks( unsigned portBASE_TYPE uxPriority );

#endif
