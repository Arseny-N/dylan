#ifndef __HTTP_SERV_H__
#define __HTTP_SERV_H__


#define DEBUG
//#define STRICT
#define MAX_URL_LEN 16

#include <stdio.h>
#include <stdlib.h>

#include "network-stack.h"



int server_start(void);
void server_stop(void);
void handle_request(struct netconn *netconn);

struct http_request {	
	unsigned short req_type;	
		
	struct netconn *netconn;	
	struct post_method *method;

	unsigned int http_code;
	int gzipped;	
	ssize_t head_end;	
	size_t body_size;	
	
#ifdef DEBUG	
	struct debug_context dbg_ctx;
#endif /* DEBUG */	

	char *header_msg;
};



#endif
