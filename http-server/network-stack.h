#ifndef __NETWORK_STACK_H__
#define __NETWORK_STACK_H__

#include "macros.h"

#ifndef STRICT

 #include "pseudo-lwip/pbuf.h"
 #include "pseudo-lwip/netbuf.h"
 #include "pseudo-lwip/api.h"
 #define NETCONN_COPY 0
 
#else

 #include "lwip/pbuf.h"
 #include "lwip/netbuf.h"
 #include "lwip/api.h"

#endif

#define netconn_write_static(netconn, str, flg) ( netconn_write((netconn), (str), sizeof(str) -1, (flg)) )

#endif /* __NETWORK_STACK_H__ */
