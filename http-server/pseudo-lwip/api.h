
#ifndef __API_H__
#define __API_H__

#define BSIZE 1024

#include "../macros.h"
#include "pbuf.h"
#include "netbuf.h"
#include <sys/types.h>


struct netconn {
	int sk;
};


err_t __netconn_recv(struct netconn *netconn, struct netbuf **nb, int bsize);
err_t netconn_recv(struct netconn *netconn, struct netbuf **nb);
err_t netconn_write(struct netconn *netconn, const void *data, size_t size, u8_t api);


int listen_sk(int backlog, int port);
ssize_t sk_writeline(int sockd, const void *vptr, size_t n);
ssize_t sk_readline(int sk, void *vp, size_t maxlen);
ssize_t _sk_readline(int sockd, void *vptr, size_t maxlen);
int write_file(int sk,int  fd);



#endif
