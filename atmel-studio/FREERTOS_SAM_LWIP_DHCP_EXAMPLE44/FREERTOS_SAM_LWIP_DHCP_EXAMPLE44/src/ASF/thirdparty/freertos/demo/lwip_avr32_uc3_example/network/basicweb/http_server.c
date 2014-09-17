#include <errno.h>
#include <string.h>


#include "http_server.h"
#include "post_methods.h"
#include "low-level-api.h"
#include "http-utils.h"

/* Parse HEADer */

#define _GFV_INTEGER 1

int _get_filed_value(struct netbuf *b, char *fname, int fname_len, int type, void *res)
{
	int i, off = pbuf_strstr(b->p, fname);
	char buf[32];
	if( off == 0xFFFF )
		return -1;
		
	off += fname_len;

	for(i=0; ;off++, i++) {
		buf[i] = pbuf_get_at(b->p, off);
		if(buf[i] == '\r') {
			buf[i] = 0;
			break;
		}		
	}
	
	switch(type) {
		case _GFV_INTEGER: {
			int *ires = (int *) res;
			char *dummy;
			*ires = strtol(buf, &dummy, 0);
			break; 
		}
		default:
	
			return -1;			
	}
	return 0;
			

}


/*-----------------------------------------------------------*/

int _get_method_id(char *path)
{
	int method_id, saved_errno = errno;
	char *dummy;
	
	errno = 0;
	method_id = strtol(path + 4, &dummy, 0);
	if( errno != 0 ) {
		printf("errno=%d %.16s\n",errno,path);		
		goto bad_url;
	}
		
	errno = saved_errno;
	return method_id;
bad_url:

	
	error("bad_url");
	return -1;	
}

int _get_url_path(struct pbuf *buf, int skip, char *path)
{
	char c;
	int i = 0;

        do {    
	        c = pbuf_get_at(buf, skip ++ );
	        
	        if( i == 0 && c == '/' ) 
	        	continue;
	        	        
	        path[i++] = c;
        } while (c != ' ' && c != '\n' && c != '\r' && c != '\0' && i < MAX_URL_LEN);
        path[i-1]= '\0';
        
        if(c!=' ')  {

        	error("Strange path %s (len=%d)", path, i);
        	return -1;
        }
        
        return 0;		
}
void parse_request_head(struct netbuf *buf, struct http_request *req)
{
	req->req_type = HTTP_HEADER_ERR;
	req->body_size = 0;    
	
	if (pbuf_memfind(buf->p, (void*)"GET", 3, 0) != 0xFFFF) {	
	
		req->req_type = HTTP_HEADER_GET;
		
		char a = pbuf_get_at(buf->p, 4);
		char b = pbuf_get_at(buf->p, 5);
		
		if( a != '/' && b != ' ' ) {
			req_err(req, 404, "URL not found"); 
			return;
		}
        } else	if (pbuf_memfind(buf->p, (void*)"POST", 4, 0) != 0xFFFF) {
        
        	req->req_type = HTTP_HEADER_POST;
        	
        	char path[MAX_URL_LEN];
        	
            	if(_get_url_path(buf->p, 5, path) == -1) {
      	        	req_err(req, 400, "Broken URL");
            		error("parse_url(POST)");
            		return;
            	}
            	
        	if(strncmp(path, "api/", 4) != 0) {
        		req_err(req, 501, "Method not implemented");
        		return;
        	}
        	
        	
        	
        	int method_id = _get_method_id(path);
        	if(method_id == -1) {
        		req_err(req, 400, "Bad URL, bad method-id");
        		return;
        	}
        	
        	if(method_id > MAX_POST_METHOD || method_id < 0) {
			req_err(req, 400, "Bad URL, method-id out of range");
			return;
		}
        	req->method = &post_method_lookup[method_id];
        	
        	
        	
            	if(_get_filed_value(buf, "Content-Length:", 16, _GFV_INTEGER, &(req->body_size)) &&
	           _get_filed_value(buf, "content-length:", 16, _GFV_INTEGER, &(req->body_size)) ) {
            		req_err(req, 400, "<Content-Length> filed not found");
            		return;            	
            	}

        } else {       
        	req_err(req, 501, "Method not implemented");        	
        }	
}
/*-----------------------------------------------------------*/
ssize_t read_request_head(struct netconn *netconn, struct netbuf **res)
{		
	struct netbuf *nb, *head = NULL;
	int end;
	for(;;) {
		if(netconn_recv(netconn, &nb)) {
			error("netconn_recv()");
			return -1;
		}
		
		if(head) netbuf_chain( head, nb);
		else   	 head = nb;
		end = pbuf_strstr(head->p, "\r\n\r\n");
		if( end != 0xFFFF) 
			break;	
	}		
	*res = head;
	return end;

}



int read_request_body(struct netconn *netconn, struct netbuf **res, ssize_t bodylen )
{	
	struct netbuf *nb, *body = NULL;
	for(;;) {
#ifdef STRICT

		if(netconn_recv(netconn, &nb)) {
			error("netconn_recv()");
			return -1;
		}
#else
		if(__netconn_recv(netconn, &nb, bodylen)) {
			error("netconn_recv()");
			return -1;
		}
#endif
		if(body) netbuf_chain( body, nb);
		else   	 body = nb;
		
		if(body->p->tot_len >= bodylen)
			break;
	}		
	*res = body;
	return 0;

}

#include "wp.h"
int write_webpage(struct http_request *req)
{
	if(_reply_head(req) == -1) {
		error("_reply_head()");		
		return -1;
	}
	if(netconn_write(req->netconn, webpage, webpage_len, 0) == -1) {
		req_internal(req,"netconn_write()");
		return  -1;
	}
	return 0;	
}
int server_start(void)
{
	return open_api();	
}
void server_stop(void)
{
	close_api();	
}
void handle_request(struct netconn *netconn)
{
	struct http_request req;	
	struct netbuf *head, *body = NULL;
	int body_start;

	dprintf("================[HEAD]================\n");	
	
	req.netconn = netconn;
	req.http_code = 200;
	req.header_msg = NULL;
	req.body_size = 0;
	req.head_end = read_request_head(req.netconn, &head);

	if( req.head_end == -1) {	
		req_internal(&req,"read_request_head()");
		goto req_proc;
	}

	print_xjson(" \nHEAD ->", "<-\n", head, 0, req.head_end + 4);
	parse_request_head(head, &req);
	
req_proc:

	switch(req.req_type) {
		case HTTP_HEADER_POST:

			dprintf("------------HTTP_HEADER_POST------------\n");
			req.gzipped = 0;
			if(req.body_size) {		
				body_start = req.head_end + 4;
				dprintf("================[BODY]================\n");
				print_xjson(" \nALL  ->", "<-\n", head, 0, head->p->tot_len);
				if( body_start + req.body_size > head->p->tot_len) {
					
					if(read_request_body(req.netconn, &body, 
							     body_start + req.body_size - head->p->tot_len ) == -1) {
						req_internal(&req,"read_request_body()");
						goto req_proc;
					}
					
					
					netbuf_chain(head, body);
				}
				
				print_xjson(" \nBODY ->", "<-\n", head, req.head_end + 4, req.body_size);

				if(execute_post_method(&req, head, body_start) == -1) {
					req_err(&req, 401, "execute_post_method()");
					goto req_proc;
				}
				
				
			}
			break;
		case HTTP_HEADER_GET:
			dprintf("------------HTTP_HEADER_GET------------\n");  
			req.gzipped = 1;
			if(write_webpage(&req)) {
				error("write_webpage()");
				goto req_proc;
			}	
						
			break;
		case HTTP_HEADER_ERR:  
			dprintf("------------HTTP_HEADER_ERR------------\n");
			req.gzipped = 0;
			if(_reply_head(&req) == -1) 
				error("_reply_head()");							
				
			break;
	}
	
cleanup:
	netbuf_delete(head);
}
