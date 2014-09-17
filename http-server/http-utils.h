#ifndef  _HTTP_UTILS_H_
#define  _HTTP_UTILS_H_

#include "http_server.h"
#include "generic-json-zero-copy.h"

#define NETCONN_PRINTF_BUF 128

int _reply_head(struct http_request *req);
int _reply_body(struct http_request *req, char *data, size_t len);
int reply_request(struct http_request *req, char *data, size_t len);

int netconn_printf(struct netconn *nc, char *format, ...);

#endif
