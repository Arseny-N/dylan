

#define JSONX_LWIP
#include "generic-json-zero-copy.h"
#include "http_server.h"

struct post_method {
	int (*execute)  (struct post_method *m, struct http_request *req);
	
	JSON_array_cb  parse_array;
	JSON_object_cb parse_object;

	void *data;	
	char *name;
};

struct parse_cb_arg {
	struct post_method *m;
	struct http_request *req;
};

extern struct post_method post_method_lookup[];
#define MAX_POST_METHOD 4
int execute_post_method(struct http_request *req, struct netbuf *body, int skip);	
