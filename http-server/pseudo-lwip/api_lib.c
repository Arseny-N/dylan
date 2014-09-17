#include <stdio.h>
#include <stdlib.h>


#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/stat.h>
#include <mqueue.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <error.h>
#include <string.h>

#include <sys/sendfile.h>

#include "api.h"
#include "pbuf.h"
#include "netbuf.h"



/*-----------------------------------------------------------*/
/* Post methods */


int write_file(sk, fd) 
{
	off_t size = lseek(fd, 0, SEEK_END);
	off_t zero = 0;
	if(size == (off_t) -1) {
		error("lseek()");
		return -1;
	}
	
	if(sendfile(sk, fd, &zero, size ) == -1) {
		error("sendfile()");
		return -1;
	}
	return 0;
}
/*-----------------------------------------------------------*/
ssize_t sk_readline(int sockd, void *vptr, size_t maxlen) {
	ssize_t n, numRead;
	char c, *buffer;

	buffer = vptr;

	for ( n = 1; n <= maxlen; n++ ) {		
		numRead = read(sockd, &c, 1);
		switch(numRead) {
			case 1:

				*buffer = c;				
			    	if ( c == '\n' )			    		
					goto out;				
				buffer ++;
				break;
			case 0:
		    		if ( n == 1 )
					return 0;
			    	else
					goto out;
			default:
		    		if ( errno == EINTR ) {
		    			n--;
					continue;
				}
		    		error("read()");
				return -1;	
		}
	}
out:
    return n; /* We read exactly n of then, n-1 is the offset of the last element !*/
}

ssize_t sk_writeline(int sockd, const void *vptr, size_t n) 
{
	const char *buffer;
	ssize_t numWritten;    
	size_t numLeft;

	buffer = vptr;
	numLeft  = n;
	
	
	// dbg("buffer=\"%.*s\",len=%d, sockd=%d, errno=%d", n, buffer, n, sockd, errno);
	errno = 0;
	while ( numLeft > 0 ) {
		if ( (numWritten = write(sockd, buffer, numLeft)) <= 0 ) {
			if ( errno == 0 ) {
				continue;
			} else 	if ( errno == EINTR  ) {
				errno = 0;
				numWritten = 0;
	    		} else {
	    			error("write()");
				return -1;
			}
		}
		numLeft  -= numWritten;
		buffer += numWritten;
    	}
    	return n;
}

/*-----------------------------------------------------------*/

int listen_sk(int backlog, int port)
{	
    	struct sockaddr_in addr;  

    	int sk = socket(AF_INET, SOCK_STREAM, 0);
	if(sk == -1) {
		error("socket()");
		return -1;
	}
	memset(&addr, 0, sizeof(addr));
    	addr.sin_family      = AF_INET;
	addr.sin_addr.s_addr = htonl(INADDR_ANY);
    	addr.sin_port        = htons(port);
	if ( bind(sk, (struct sockaddr *) &addr, sizeof(addr)) == -1 ) {
		error("bind()");
		return -1;
	}
	if(listen(sk, backlog)) {
		error("listen()");
		return -1;
	}
	return sk;
}
/*-----------------------------------------------------------*/
void mem_printf(char *string, size_t len) 
{
	size_t i;
	for(i=0; i<len; ++i) {
		char c[2] = { string[i], '\0' };
		if( c[0] < ' ') {
			c[1] =  (c[0] == '\0' ? '0' : 
				 c[0] == '\n' ? 'n' :
				 c[0] == '\r' ? 'r' : '?');
			c[0] = '\\';
		}
		printf("%.*s%s",2, c, c[1] == 'n' ? "\n" : "" );							
	}
}
err_t netconn_write(struct netconn *netconn, const void *data, size_t size, u8_t api)
{
	return sk_writeline(netconn->sk, data, size) == -1 ? ERR_BUF : ERR_OK;
}
err_t __netconn_recv(struct netconn *netconn, struct netbuf **nb, int bsize)
{
	size_t numRead = 0;
	char *buf = malloc(bsize + 1);
	if(!buf) {
		error("malloc(NULL)");
		return -1;
	}
	struct netbuf *b = netbuf_new();
	if(!b) {
		error("netbuf_new()");
		return -1;
	}
	
	numRead = sk_readline(netconn->sk, buf, bsize);	
	if(numRead == -1) {
		error("read(%d)", netconn->sk);
		return -1;
	}
	dprintf("line [%d]:", numRead);
	mem_printf(buf, MIN(numRead, bsize-1));
	
	numRead -= (buf[MIN(numRead, bsize) - 1] == '\0');		
		
	if(netbuf_ref(b, buf, numRead) != ERR_OK) {
		error("netbuf_ref()");
		return -1;	
	}
	*nb = b;
	return 0;
}
err_t netconn_recv(struct netconn *netconn, struct netbuf **nb)
{
	return __netconn_recv(netconn, nb, BSIZE);
}
/*-----------------------------------------------------------*/

