/*****************************************************************************
 *
 * \file
 *
 * \brief Basic WEB Server for AVR32 UC3.
 *
 * Copyright (c) 2009-2012 Atmel Corporation. All rights reserved.
 *
 * \asf_license_start
 *
 * \page License
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. The name of Atmel may not be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * 4. This software may only be redistributed and used in connection with an
 *    Atmel microcontroller product.
 *
 * THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
 * EXPRESSLY AND SPECIFICALLY DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 * \asf_license_stop
 *
 *****************************************************************************/


/*
  Implements a simplistic WEB server.  Every time a connection is made and
  data is received a dynamic page that shows the current FreeRTOS.org kernel
  statistics is generated and returned.  The connection is then closed.

  This file was adapted from a FreeRTOS lwIP slip demo supplied by a third
  party.
*/

#if (HTTP_USED == 1)


/* Standard includes. */
#include <stdio.h>
#include <string.h>

#include "conf_eth.h"

/* Scheduler includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "partest.h"

/* Demo includes. */
/* Demo app includes. */
#include "portmacro.h"

/* lwIP includes. */
#include "lwip/api.h"
#include "lwip/tcpip.h"
#include "lwip/memp.h"
#include "lwip/stats.h"
#include "lwip/init.h"
#if ( (LWIP_VERSION) == ((1U << 24) | (3U << 16) | (2U << 8) | (LWIP_VERSION_RC)) )
#include "netif/loopif.h"
#endif

/* ethernet includes */
#include "ethernet.h"


/*! \brief WEB server main task
 *         check for incoming connection and process it
 *
 *  \param pvParameters   Input. Not Used.
 *
 */
#include "http_server.h"	

portTASK_FUNCTION( vBasicWEBServer, pvParameters );
portTASK_FUNCTION( vBasicWEBServer, pvParameters )
{
struct netconn *pxHTTPListener, *pxNewConnection;

	/* Create a new tcp connection handle */
	pxHTTPListener = netconn_new( NETCONN_TCP );
	volatile err_t err;
	netconn_bind(pxHTTPListener, NULL, 80);
	netconn_listen( pxHTTPListener );
	if(server_start()) {
		while(1);
	}
	
	/* Loop forever */
	for( ;; )
	{
#if ( (LWIP_VERSION) == ((1U << 24) | (3U << 16) | (2U << 8) | (LWIP_VERSION_RC)) )
		/* Wait for a first connection. */
		pxNewConnection = netconn_accept(pxHTTPListener);
#else
        while(netconn_accept(pxHTTPListener, &pxNewConnection) != ERR_OK)
        {
            vTaskDelay( 500 );
		}
#endif
		vParTestSetLED(webCONN_LED, pdTRUE);

		if(pxNewConnection != NULL)
		{
		
			//prvweb_ParseHTMLRequest(pxNewConnection);
			handle_request(pxNewConnection);
			
			err = netconn_close( pxNewConnection );
			if(err) {
					
			}
			err = netconn_delete( pxNewConnection );
			if(err) {
				
			}
			pxNewConnection = NULL;
		}/* end if new connection */

		vParTestSetLED(webCONN_LED, pdFALSE);

	} /* end infinite loop */
	server_stop();
}

#endif

