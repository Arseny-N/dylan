#include "network-stack.h"
#include "http-utils.h"

#include <stdarg.h>

int netconn_printf(struct netconn *nc, char *format, ...) 
{
	char buf[NETCONN_PRINTF_BUF];
	ssize_t len;
	va_list ap;
	
        va_start(ap, format);
                
	len = vsnprintf(buf, NETCONN_PRINTF_BUF-1, format, ap);
	
	va_end(ap);
	
	if(len == -1) {
		error("vsprintf()");
		return -1;
	}
	
	if( netconn_write(nc, (void*)buf, len, NETCONN_COPY) != ERR_OK )
		return -1;
	return 0;	
}
int _reply_head(struct http_request *req)
{
	char *str = req->header_msg, buf[64];
	if(str == NULL ) 
		str = "OK";
		
	int len = snprintf(buf, 64, "HTTP/1.0 %.3d %s\r\n", req->http_code, str);
	if( netconn_write(req->netconn, buf, len, NETCONN_COPY) != ERR_OK )
		return -1;	
		
	if(req->gzipped) {
		if( netconn_write_static(req->netconn, "Content-Encoding: gzip\r\n", 0) != ERR_OK )
			return -1;			
	}
	
	if( netconn_write_static(req->netconn, "\r\n", 0) != ERR_OK )
			return -1;	
	
	return 0;
}
int _reply_body(struct http_request *req, char *data, size_t len)
{	
	return (netconn_printf(req->netconn, "{\"result\" : %.*s }", len, data) < 0) ? -1 : 0;
}
int reply_request(struct http_request *req, char *data, size_t len)
{

	if(_reply_head(req)) {
		error("_reply_head()");
		return -1;
	}

	if(_reply_body(req, data, len)) {
		error("_reply_body()");
		return -1;
	}

	return 0;
}
