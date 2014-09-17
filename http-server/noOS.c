#include <sys/types.h>

#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/stat.h>
#include <mqueue.h>
#include <unistd.h>
#include <errno.h>

#include "network-stack.h"
#include "http_server.h"


struct params {
	int port;
};
void params_defaults(struct params *p)
{
	p->port = 5000;
}
void http_server_task(void *params) 
{
	struct params *args = (struct params *) params;
	int lfd, cfd;
	printf("listening on port %d...\n", args->port);	
	lfd = listen_sk(0, args->port);
	if(lfd == -1) {
		error("listen_sk()");
		exit(EXIT_FAILURE);
	}

	for(;;) {
		cfd = accept(lfd, NULL, NULL);
		if(cfd == -1 && errno != EINTR) {
			error("accept()");
			exit(EXIT_FAILURE);
		}

		if(cfd != -1) {
			struct netconn netconn;	
			netconn.sk = cfd;			
			usleep((useconds_t) 10);
			handle_request(&netconn);
						
			if(close(cfd) == -1) {
				error("close()");
			}
		}
	}
}


int main ( int argc, char * argv [] )
{
	struct params params;
	char *dummy;
	
	setbuf(stdout, NULL);
	setbuf(stderr, NULL);
	params_defaults(&params);

	if(argv[1])
		params.port = strtol(argv[1], &dummy, 0);
	if(server_start()) {
		fprintf(stderr, "Failed to start server, exitting.");
		exit(EXIT_FAILURE);
	}
		
	http_server_task(&params);
	server_stop();
	exit(EXIT_SUCCESS);
}


