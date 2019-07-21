/**
 * <b>File:</b> httpd.c
 *
 * <b>Project:</b> Web Server Demo
 *
 * <b>Description:</b>   Implements a simplistic WEB server.
 * Every time a connection is made and data is received, the server look for the
 * request file in a ROM file system implemented in th file fsdata.c. If the page
 * is not found a standard 404.html page is send back. The connection is closed after
 * the page is sent to the client.<br/>
 * Note that this very simple web server run as task and communicate with the
 * lwIP stack using the serial API.
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

// Standard includes.
#include <stdio.h>
#include <string.h>

// Scheduler includes.
#include "FreeRTOS.h"
#include "task.h"

// Demo app includes.
#include "lwipWebServer/fs.h"

// lwIP includes.
#include "lwip/api.h"
#include "lwip/tcpip.h"
#include "lwip/memp.h"
#include "lwip/stats.h"
#include "netif/loopif.h"

// The size of the buffer in which the dynamic Debug WEB page is created.
#define webMAX_PAGE_SIZE	2048

// Standard GET response.
#define webHTTP_OK	"HTTP/1.0 200 OK\r\nContent-type: text/html\r\n\r\n"

// The port on which we listen.
#define webHTTP_PORT		( 80 )

#define webSHORT_DELAY	( 10 )

// Format of the dynamic page.
#define webHTML_START \
"<html>\
<head>\
</head>\
<BODY onLoad=\"window.setTimeout(&quot;location.href='debug.htm'&quot;,2000)\" bgcolor=\"#FFFFFF\" text=\"#2477E6\">\
\r\nPage Hits = "

#define webHTML_END \
"\r\n</pre>\
\r\n</font></BODY>\
</html>"

/**
 * Module variable used as buffer to build the dynamic debug.htm page.
 */
static portCHAR s_cDynamicPage[ webMAX_PAGE_SIZE ];

/**
 * Module variable that store the hits count of the debug.htm page.
 */
static unsigned short s_nPageHits = 0;

static void ParseHTMLRequest( struct netconn *pxNetCon );

/**
 * Web server task control function.
 *
 * @param  pvParameters   Input. Not Used.
 */
portTASK_FUNCTION( httpd, pvParameters )
{
struct netconn *pxHTTPListener, *pxNewConnection;

	// Create a new tcp connection handle.
	pxHTTPListener = netconn_new( NETCONN_TCP );
	netconn_bind(pxHTTPListener, NULL, webHTTP_PORT );
	netconn_listen( pxHTTPListener );

	// Loop forever
	for( ;; )
	{
		// Wait for a first connection.
		pxNewConnection = netconn_accept(pxHTTPListener);

		if(pxNewConnection != NULL)
		{
			ParseHTMLRequest(pxNewConnection);

			netconn_close( pxNewConnection );
			while ( netconn_delete( pxNewConnection ) != ERR_OK ) {
				vTaskDelay( webSHORT_DELAY );
			}

		}
	}
}

/**
 * Parse the incoming HTML request and send file.
 *
 * @param pxNetCon   Input. The netconn to use to send and receive data.
 */
static void ParseHTMLRequest( struct netconn *pxNetCon )
{
	struct fs_file file;
	struct netbuf *pxRxBuffer;
	portCHAR *pcRxString;
	unsigned portSHORT usLength;
	int i;
	//static unsigned portLONG ulPageHits = 0;

	// We expect to immediately get data.
	pxRxBuffer = netconn_recv( pxNetCon );

	if( pxRxBuffer != NULL )
	{
		// Where is the data?
		netbuf_data( pxRxBuffer, ( void * ) &pcRxString, &usLength );

		// Is this a GET?  We don't handle anything else.
		if( !strncmp( pcRxString, "GET ", 4 ) )
		{
        for(i = 4; i < usLength; i++) {
          if (pcRxString[i] == ' ' ||
              pcRxString[i] == '\r' ||
              pcRxString[i] == '\n') {
            pcRxString[i] = 0;
          }
        }

				// Is the page requested debug.html ?
				if ( !strncmp((char *)pcRxString + 4, "/debug.htm", 9) ) {
					// Build the dynamic page
					file.data = s_cDynamicPage;

					// Update the hit count.
					++s_nPageHits;
					char strPageHits[10];
					sprintf( strPageHits, "%d", (int)s_nPageHits );

					// Write out the HTTP OK header.
					netconn_write( pxNetCon, webHTTP_OK, (u16_t) strlen( webHTTP_OK ), NETCONN_COPY );

					// Generate the dynamic page... First the page header.
					strcpy( s_cDynamicPage, webHTML_START );

					// ... Then the hit count...
					strcat( s_cDynamicPage, strPageHits );
					strcat( s_cDynamicPage, "<p><pre>Task          State  Priority  Stack	#<br>************************************************<br>" );

					// ... Then the list of tasks and their status...
					vTaskList( ( signed portCHAR * ) s_cDynamicPage + strlen( s_cDynamicPage ) );

					// ... Finally the page footer.
					strcat( s_cDynamicPage, webHTML_END );

					file.len = strlen(s_cDynamicPage);
				}
				else {
					int nFileFound = 0;
					if (*(char *)(pcRxString + 4) == '/' &&
						 *(char *)(pcRxString + 5) == 0) {
						nFileFound = fs_open("/index.html", &file);
					}
					else {
						nFileFound =  fs_open((char *)pcRxString + 4, &file);
					}
					if (!nFileFound) {
						fs_open("/404.html", &file);
					}
				}

				// Write out the dynamically generated page.
				u32_t nSize = file.len;
				char *pData = file.data;
				err_t  err;
				while (nSize > 0) {
					if (nSize > 0xffff) {
						err = netconn_write( pxNetCon, pData, (u16_t) 0xffff, NETCONN_NOCOPY );
						nSize -= 0xffff;
						pData += 0xffff;
					}
					else {
						err = netconn_write( pxNetCon, pData, (u16_t) nSize, NETCONN_NOCOPY );
						nSize = 0;
					}


#ifdef DEBUG
					if (err != ERR_OK)
						printf("lwip:httpd - error\n");
#endif

				}
		}
		netbuf_delete( pxRxBuffer );
	}
}
